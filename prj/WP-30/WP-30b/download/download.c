/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : download.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/14/2014 10:28:05 AM
 * Description        : 
 *******************************************************************************/
#include "../wp30_boot.h"
#include "download.h"

//-------------------------------------------------------------
uint gDLCom=WORK_COMPORT;   //下载口

static void dl_isr(void)
{
    char ch;

    ch = hw_uart_getchar((UARTn)gDLCom);
    QueueWrite(&sgSerialOpt[gDLCom].queue,
               (unsigned char *)&ch,
               1);
//    TRACE("\nw:t:%dh:%d",UART_QUEUE_PTR(gDLCom)->tail,UART_QUEUE_PTR(gDLCom)->head);
}

int dl_open(uint32_t com, uint8_t *buf_dl, uint32_t buf_size)
{
    gDLCom = com;
    return drv_uart_open((UARTn)gDLCom,DL_BAUD,
                         buf_dl,buf_size,
                         dl_isr);
}

int dl_check_readbuf(void)
{
    return drv_uart_check_readbuf((UARTn)gDLCom);   
}

int dl_clear_buffer(void)
{
    return drv_uart_clear((UARTn)gDLCom);
}
int dl_read(uint8_t *output, uint32_t length, int t_out_ms)
{
    return drv_uart_read((UARTn)gDLCom, output, length, t_out_ms);
}

int dl_write(uint8_t const *input, uint32_t length)
{
    return drv_uart_write((UARTn)gDLCom, input, length);
}


int dl_close(void)
{
    return drv_uart_close((UARTn)gDLCom);
}

static int dl_recv_frame(uint8_t *data,uint16_t *flength, int t_out_ms)
{
    uint32_t rvlength = 0;
    uint32_t time_s = 0;
    uint16_t size,index;

    //DL_CHAR_START
    //    TRACE("\nRecv frame. ");
    index = 0;
    time_s = sys_get_counter();
    rvlength = 1; 
    while (1)
    {
        if(rvlength == dl_read(data,rvlength,100)) {
            if (data[index] == DL_CHAR_START) {
                index++;
                break;
            }
        }
        if (t_out_ms>0 && sys_get_counter() - time_s > 1000) {
            //			TRACE("DL_CHAR_START timeout\n");
            return -1;
        }
    }
    *flength = rvlength;
    time_s = sys_get_counter();  	
    //frame length:the bin packet length
    rvlength = 2; 
    while (1)
    {
        if(rvlength == dl_read(data+index,rvlength,100)) {
            size = (data[index] << 8) | data[index+1];
            index += rvlength ;
            break;
        }
        if (t_out_ms>0 && sys_get_counter() - time_s > 1000) {
            //			TRACE("\nlength timeout"); 
            return -1;
        }
    }
    *flength += rvlength;
    time_s = sys_get_counter();
    //bin+lrc+etx
    rvlength = size+1+1; 	
    while (1)
    {
        if (rvlength == dl_check_readbuf()) {
            if(rvlength == dl_read(data+index,rvlength,t_out_ms)) {
                break;
            }
        }
        if (t_out_ms>0 && sys_get_counter() - time_s > (rvlength<<1)) {
            //			TRACE("\ndata timeout:%d",rvlength);
            return -1;
        }
        s_DelayUs(rvlength);
    }
    *flength += rvlength;

    return 0;
}

static int dl_check_frame(DLTYPE_DEF *frame, uint16_t flength)
{
    uint16_t i;
    uint8_t lrc=0;

    if (frame->stx != DL_CHAR_START) {
        //        TRACE("\nSTX err");
        return -1;
    }
    frame->bin_length = ((frame->length_h<<8)|frame->length_l);
    if (flength - 5 != frame->bin_length) {
        //        TRACE("\nlength err");
        return -1;
    }
    for (i=0; i<frame->bin_length; i++)
    {
        lrc ^= frame->bin[i];
    }
    if (frame->bin[frame->bin_length] != lrc) {
        //        TRACE("\nlrc err");
        return -1;
    }
    if (frame->bin[frame->bin_length+1] == DL_CHAR_END_ALL) {
        frame->etx = DL_CHAR_END_ALL;
    } else if (frame->bin[frame->bin_length+1] != DL_CHAR_END_ONE) {
        //        TRACE("\nETX err");
        return -1;
    }
    return 0;
}

static int dl_hand_shake(int32_t t_out_ms)
{
    uint8_t ch=0;
    uint32_t time_s;
    uint8_t version[48];

    //    TRACE("\nhand shake");
    time_s = sys_get_counter();
    while (1)
    {
        if (t_out_ms > 0 && (sys_get_counter() - time_s > t_out_ms) ) {
            //            TRACE(" req timeout");
            return -1;
        }
        if (dl_check_readbuf() >= 1) {
            dl_read(&ch,1,100);
            if (ch == DL_CHAR_HAND_REQ) {
                dl_write(&ch,1);
                break;
            }
        }
    }
    dl_clear_buffer();
    memset(version,0x00,sizeof(version));
    memcpy(version,(uint8_t const *)&gBootVerInfo,sizeof(gBootVerInfo));
    dl_write(version,sizeof(version));
    while (1)
    {
        if (t_out_ms > 0 && (sys_get_counter() - time_s > t_out_ms) ) {
            //            TRACE(" ack timeout");
            return -1;
        }
        if (dl_check_readbuf() >= 1) {
            dl_read(&ch,1,100);
            if (ch == DL_CHAR_HAND_ACK) {
                break;
            }
        }
    }

    return 0;
}

inline static void dl_send_ack(uint8_t ch)
{
    dl_write(&ch,1);
}

static int dl_rev_bin(uint32_t addr_start,uint32_t *code_length,int t_out_ms)
{
    DLTYPE_DEF frame;
    uint32_t time_s;
    uint16_t length=0;

    hw_flash_init();
    *code_length = length;
    time_s = sys_get_counter();
    while (1) 
    {
        if (t_out_ms > 0 && sys_get_counter() - time_s > t_out_ms) {
            //            TRACE("\nbin time out");
            return -1;
        }
        if (dl_recv_frame((uint8_t *)&frame,&length,t_out_ms) != 0) {
            //            TRACE("\nrecv frame error");
            return -1;
        }
        if (dl_check_frame(&frame,length) != 0) {
            //            TRACE("\nframe check error");
            dl_send_ack(DL_FRAME_ERR);
            return -1;
        }
        length = frame.bin_length;
        //todo:save flash
        if (flash_write(addr_start+*code_length,
                        length,frame.bin) != Flash_OK) {
            //            TRACE("\nflash Write error");
            dl_send_ack(DL_PROGRAM_ERR);
            return -1;
        }
        dl_send_ack(DL_FRAME_SUCCESS);
        *code_length += length; 
        time_s = sys_get_counter();
        if (frame.etx == DL_CHAR_END_ALL) {
            //            TRACE("\ndown load over");
            return 0;
        }
    }
}

int dl_ctrl(uint32_t *code_length,int t_out_ms)
{
    //    dl_close();
    if (dl_hand_shake(t_out_ms)) {
        return -1;
    }
    if (dl_rev_bin(SA_CTRL,code_length,t_out_ms)) {
        return -1;
    }
    return 0;
}

int dl_write_flash(uint32_t addr,uint32_t *code_length,int t_out_ms)
{
    //    dl_close();
    if (dl_hand_shake(t_out_ms)) {
        return -1;
    }
    if (dl_rev_bin(addr,code_length,t_out_ms)) {
        return -1;
    }
    return 0;
}



//不能在BOOT层调用，只能在监控层调用
int dl_boot(uint32_t *code_length,int t_out_ms)
{
    //    dl_close();
    if (dl_hand_shake(t_out_ms)) {
        return -1;
    }
    if (dl_rev_bin(SA_BOOT,code_length,t_out_ms)) {
        return -1;
    }
    return 0;
}





