/*
 * =====================================================================================
 *
 *       Filename:  ctc_sys.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/12/2016 3:38:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#include "ctc_sys.h"
#include "wp30_ctrl.h"

MK210_sys_memory gled_sys_memory;

dl_info sys_dl_info;

#ifdef CFG_LOWPWR
uint32 Lowpower_Timer;
#endif

/*
 *功能：LED灯控制
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
extern void hw_led_on(uint32_t ledbit);
extern void hw_led_off(uint32_t ledbit);
uint16_t sys_manage_led(uint8_t* data)
{
    uint8_t type,ctr,flash,i;
    uint16_t f_num,f_cyc,f_on_time;

    type = data[0];
    ctr = data[1];
    flash = data[2];
    f_num = data[3]|data[4]<<8; 
    f_cyc = data[5]|data[6]<<8;
    f_on_time = data[7]|data[8]<<8;
   
    gled_sys_memory.led.status.type = type;
    gled_sys_memory.led.status.ctr = ctr;
    
   // gled_sys_memory.led.status.flash = flash;

    gled_sys_memory.led.flashing_times = f_num; 
    gled_sys_memory.led.flashing_cyc = f_cyc; 
    gled_sys_memory.led.Ontime = f_on_time; 

//    gled_sys_memory.led.status.type = 0xf;
//    gled_sys_memory.led.status.ctr = 0xf;
//    
//    gled_sys_memory.led.status.flash = 1;
//
//    gled_sys_memory.led.flashing_times = 0xffff; 
//    gled_sys_memory.led.flashing_cyc = 200; 
//    gled_sys_memory.led.Ontime = 100; 
    if(flash & 0x0f)
    {
        gled_sys_memory.sys_tick.flash_flag = 1;
        memcpy((char *)&gled_sys_memory.sys_tick.flash_led,(char *)&flash,1); 
        gled_sys_memory.led.time_count = gled_sys_memory.led.flashing_cyc; //周期时间
        hw_led_on(gled_sys_memory.led.status.type & flash); //受控灯全部亮
    }
    else
    {
        gled_sys_memory.sys_tick.flash_flag = 0;
    }

    for(i = 0; i < 4; i++)
    {
        if((flash & (1 << i)) == 0)//灯不闪烁
        {
            if((type & (1 << i)))
            {
                if(ctr & (1 << i))
                {
                    hw_led_on(1 << i);  
                }
                else
                {
                    hw_led_off(1 << i);
                }
            }
        }
    }

    return CMDST_OK;  
}
/*
 *功能：蜂鸣器控制
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
extern void sys_beep_pro(uint freq, uint duration_ms, int choke);
uint16_t sys_manage_beep(uint16_t time,uint16_t frequency)
{
    uint freq;
    freq = frequency;
    if(freq == 0){
        freq = BEEP_PWM_HZ; 
    }
    sys_beep_pro(freq,time*10,YES);

    return CMDST_OK;
}

/*
 *功能：查询终端软件版本
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
//#define VERSION_SOFT "V1.0.0.1"
#define VERSION_HARD "V1.0.0.0"
//uint16_t terminal_info_soft(uint32_t *len,uint8_t* buffer)
//{
//    char timebuf[30];
//    memset(timebuf, 0, sizeof(timebuf));
//    memcpy(buffer, VERSION_SOFT, strlen(VERSION_SOFT));
//    sprintf(timebuf, " %s %s", __DATE__, __TIME__);
//    memcpy((char *)(buffer+strlen(VERSION_SOFT)), timebuf, strlen(timebuf));
//    *len = strlen(VERSION_SOFT) + strlen(timebuf);
//    return CMDST_OK;
//}

uint16_t terminal_info_soft(uint32_t *len,uint8_t* buffer)
{
    char timebuf[30];
    memset(timebuf, 0, sizeof(timebuf));

    sys_read_ver(READ_CTRL_VER,&timebuf[0]);
//    *len = strlen(timebuf); 
//    memcpy(buffer, VERSION_SOFT, strlen(VERSION_SOFT));
    sprintf((char *)(&timebuf[strlen(timebuf)]), " %s %s", __DATE__, __TIME__);
    memcpy(buffer, timebuf, strlen(timebuf));
    TRACE("get soft version:%s\r\n", timebuf);
    *len = strlen(timebuf);
    return CMDST_OK;
}

uint16_t terminal_info_hard(uint32_t *len,uint8_t* buffer)
{
    memcpy(buffer, VERSION_HARD, strlen(VERSION_HARD));
    *len = strlen(VERSION_HARD);
    return CMDST_OK;
}
/*
 *功能：设置rtc时钟
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */

uint16_t sys_manage_get_rtc(struct rtc_time* tm)
{
    unsigned long second0=0;
    int ret;

    ret = hw_rtc_read_second((uint32_t *)&second0);
    if ( ret != 0 ) {
       return CMDST_OTHER_ER;  
    }

    rtc_time_to_tm(second0,tm);
    return CMDST_OK;
}

/*
 *功能：设置RTC时间
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t sys_manage_set_rtc(struct rtc_time* tm)
{
    unsigned long second=0;
    int ret;

    ret = rtc_valid_tm(tm);
    if ( 0 != ret ) {
       return CMDST_OTHER_ER;  
    }

    rtc_tm_to_time(tm,&second);
    hw_rtc_init(second,0);

    return CMDST_OK;
}

/*
 *功能：写外部FLASH
 *
 *参数：mode 0-读写限制（不允许读写8-31）  1-不限制
 *
 *返回值：
 *
 *注：扇区大小 4K  number:0-15(每个大小512字节) 共8K, 0-7为一个扇区 8-15为一个扇区。
 *
 */
uint16_t sys_manage_exflash_write(uint8_t mode,uint16_t number,uint16_t data_len,uint8_t* data)
{
    uint8_t data_buf[EXFLASH_DATA_LEN_MAX];
    uint8_t des_buf[EXFLASH_DATA_LEN_MAX];
    uint8_t des_key[SYS_EXFLASH_KEY_SIZE];
    if(mode == 0)
    {
        if(number >= 8 && number <= 31)
        {
            return CMDST_PARAM_ER;
        }
    }
    if(number > EXFLASH_NUMBER_MAX)
    {
        return CMDST_PARAM_ER;
    }
    if(data_len > EXFLASH_DATA_LEN_MAX || data_len < 1)
    {
        return CMDST_PARAM_ER;
    }

    memset((char *)data_buf,0,EXFLASH_DATA_LEN_MAX);
    memcpy(data_buf,data,data_len);

    s_read_syszone(SYS_EXFLASH_KEY_ADDR,SYS_EXFLASH_KEY_SIZE,des_key);
   
    des1_encrypt_ecb(data_buf,EXFLASH_DATA_LEN_MAX,des_key,des_buf);
    
    exflash_write(number*EXFLASH_DATA_LEN_MAX,EXFLASH_DATA_LEN_MAX,des_buf); 
    
    return CMDST_OK;
}
/*
 *功能：读外部FLASH
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t sys_manage_exflash_read(uint8_t mode,uint16_t number,uint16_t data_len,uint8_t* data)
{
    uint8_t data_buf[EXFLASH_DATA_LEN_MAX];
    uint8_t des_buf[EXFLASH_DATA_LEN_MAX];
    uint8_t des_key[SYS_EXFLASH_KEY_SIZE];
    if(mode == 0)
    {
        if(number >= 8 && number <= 31)
        {
            return CMDST_PARAM_ER;
        }
    }

    if(number > EXFLASH_NUMBER_MAX )
    {
        return CMDST_PARAM_ER;
    }
    if(data_len > EXFLASH_DATA_LEN_MAX || data_len < 1)
    {
        return CMDST_PARAM_ER;
    }
    memset((char *)data_buf,0,EXFLASH_DATA_LEN_MAX);

    exflash_read(number*EXFLASH_DATA_LEN_MAX,EXFLASH_DATA_LEN_MAX,des_buf);
    
    s_read_syszone(SYS_EXFLASH_KEY_ADDR,SYS_EXFLASH_KEY_SIZE,des_key);
   
    des1_decrypt_ecb(des_buf,EXFLASH_DATA_LEN_MAX,des_key,data_buf);
    
    memcpy(data,data_buf,data_len);
   
    return CMDST_OK;
}
/*
 *功能：
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t sys_mdf_status_get(int *status, uchar *time)
{
    struct  TamperEvent even;
    struct rtc_time tm;
    drv_dryice_get_reg(&even);
    *status = even.tsr & MDF_MASK;
    if ( even.time_s ) {
        rtc_time_to_tm(even.time_s, &tm); 
        TRACE("year:%d,month:%d,day:%d,hour:%d,min:%d,sec:%d\r\n", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour,tm.tm_min,tm.tm_sec);
        time[0] = (tm.tm_year+1900)&0xff;
        time[1] = ((tm.tm_year+1900)>>8)&0xff;
        time[2] = (tm.tm_mon+1) & 0xff + 1;
        time[3] = (tm.tm_mday & 0xff); // month + 1
        time[4] = tm.tm_hour & 0xff;
        time[5] = tm.tm_min & 0xff;
        time[6] = tm.tm_sec & 0xff;
    }else{
        memset(time, 0, 7);
    }
    return CMDST_OK;
}
/*
 *功能：
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
extern void MakeKeyCheck(uchar *in,uint len,uchar *check);   
uint16_t sys_mdf_unlock(void)
{
    
    ST_MMAP_KEY ramkey; 

    drv_dryice_init((hal_dry_tamper_t)(DRY_TER_TPE(0x3F)|kDryTamper_Voltage),
                    (hal_dry_interrupt_t)0,
                    0x2C,ENABLE);
    RNG_FillRandom((uint8_t *)&ramkey, MMAP_KEY_LEN);
    MakeKeyCheck((uchar *)&ramkey,MMAP_KEY_LEN,ramkey.check);
    memcpy(ramkey.facchk,STR_SRAM,FUN_LEN);
    TRACE_BUF("dryice mmk",ramkey.aucMMK,32);
    hw_dryice_write_key((FPOS(ST_MMAP_KEY,aucMMK)>>2),
                        ((MK_UNIT_LEN+ELRCK_LEN+CHECK_LEN+FUN_LEN)>>2),
                        (uint32_t *)&ramkey);

    return CMDST_OK;
}

/*
 *功能：开始下载文件
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t sys_manage_download_inform(uint8_t *data,uint8_t *buffer,uint32_t *data_len)
{

    update_info sys_update;
    memcpy((char *)&sys_dl_info,(char *)data,sizeof(dl_info));
   
   TRACE("\r\n file type:%d",sys_dl_info.file_info.type);
   TRACE_BUF("version",sys_dl_info.file_info.version,32);
   
   sys_dl_info.current_offset = 0;
   sys_dl_info.current_len = 0; 
   if( ((sys_dl_info.file_info.type == DOWNLOAD_TYPE_CTRL) && (sys_dl_info.file_info.len > LEN_CTRL_BACK)) 
      || ((sys_dl_info.file_info.type == DOWNLOAD_TYPE_GAPK) && (sys_dl_info.file_info.len > EXFLASH_GSHA_SIZE)) 
      || ((sys_dl_info.file_info.type == DOWNLOAD_TYPE_SAPK) && (sys_dl_info.file_info.len > 4096)) 
       )
   {
       TRACE("\r\n文件过大..");
       return CMDST_CANCEL_ER;
   }
   *data_len = sizeof(int);
   memcpy((char *)buffer,(char *)&(sys_dl_info.current_offset),*data_len);
   
   sha256_init(&sys_dl_info.sha);
	
   //版本判断
   if ( sys_dl_info.file_info.type == DOWNLOAD_TYPE_CTRL) {
      
        sys_update.data_len = 0;
        sys_update.flag = 0x55;
        //写入升级标志
       flash_write(SA_CTRL_BACK-sizeof(update_info),sizeof(update_info),(uchar *)&sys_update);
 
       
       hw_flash_init();
   }

   return CMDST_OK;
}


uint16_t sys_manage_download_data(uint8_t *data,uint8_t *buffer,uint32_t *buffer_len)
{
   int file_type = 0;  //文件类型
   int offset = 0;     //偏移
   int data_len = 0;   //数据长度
   int address;
   int ret; 
   uchar *pointer;
   
   TRACE("\r\ndl data offset:%d",sys_dl_info.current_offset);
   pointer = data;
   
   memcpy((char *)(&file_type),pointer,sizeof(int));
   if(file_type != sys_dl_info.file_info.type)
   {

       TRACE("\r\ndl type:%d",file_type);
       TRACE("\r\ndl type:%d",sys_dl_info.file_info.type);
       return CMDST_PARAM_ER;
   }
   else
   {
      switch ( file_type )
      {
      case DOWNLOAD_TYPE_CTRL :
          address = SA_CTRL_BACK;
          break;
      case DOWNLOAD_TYPE_GAPK :
      case DOWNLOAD_TYPE_SAPK :
          address = EXFLASH_TMP_BUF_ADDR;
          break;
      default :
          return CMDST_PARAM_ER;
      }
   }
   pointer += 4;
   memcpy((char *)(&offset),(char *)pointer,sizeof(int));
   if(offset != sys_dl_info.current_offset)
   {
       TRACE("\r\ndl offset:%d",offset);
       return CMDST_PARAM_ER;
   }
   if(offset > LEN_CTRL_BACK)
   {
       TRACE("\r\ndl offset:%d",offset);
       return CMDST_PARAM_ER;
   }
   pointer += 4;
   memcpy((char *)(&data_len),(char *)pointer,sizeof(int));
   
   pointer += 4;
   
   switch ( file_type )
   {
   case DOWNLOAD_TYPE_CTRL :
       ret = flash_write(address+sys_dl_info.current_offset,data_len,pointer);
       break;
   case DOWNLOAD_TYPE_GAPK :  // save gapk in the tempbuf in the extern spiflash
   case DOWNLOAD_TYPE_SAPK :   // save sapk in the 
       TRACE("data_len:%d\r\n", data_len);
       ret = exflash_write(address+sys_dl_info.current_offset, data_len, pointer);
       break;
   default :
       return CMDST_PARAM_ER;
   }

   if(ret != 0)
   {
       TRACE("\r\nwrite error:%d",ret);
   }
   else
   {
       //数据写入成功
       sys_dl_info.current_offset += data_len; 
       sha256_update(&(sys_dl_info.sha),pointer,data_len);
   }   
   *buffer_len = sizeof(int);
   memcpy((char *)buffer,(char *)&(sys_dl_info.current_offset),*buffer_len);
   return CMDST_OK; 
}


extern const VERSION_INFO_NEW gCtrlVerInfo;
uint16_t sys_manage_download_finish(uint8_t *data)
{
    uint8_t hash[SHA256_HASH_SIZE];
    update_info sys_update;
    int ret; 
    ST_taginfo ctrl_tag;
    
    TRACE("\r\ndl finish");

    sha256_final(&sys_dl_info.sha, hash);
    
    if(strncmp((char *)sys_dl_info.file_info.sha,(char *)hash,SHA256_HASH_SIZE) != 0)
    {
        return CMDST_HASE_ERR;
    }
    if(sys_dl_info.current_offset != sys_dl_info.file_info.len)
    {
        return CMDST_OTHER_ER;
    }
    switch (  sys_dl_info.file_info.type)
    {
    case DOWNLOAD_TYPE_CTRL :

        memcpy((char*)&ctrl_tag,(char *)SA_CTRL_BACK+sys_dl_info.file_info.len -sizeof(ST_taginfo),sizeof(ST_taginfo));
        ret = memcmp(ctrl_tag.version,gCtrlVerInfo.product,10); 
        if(ret != 0)
        {
            return CMDST_OTHER_ER;
        }

        sys_update.data_len = sys_dl_info.file_info.len;
        sys_update.flag = 0xaa;
        //写入升级标志
        ret = flash_write(SA_CTRL_BACK-sizeof(update_info),sizeof(update_info),(uchar *)&sys_update);
        break;
    case DOWNLOAD_TYPE_GAPK :
        // 处理GAPK
        if ( sys_dl_info.file_info.len > (512*16+512+16) ) {
            return CMDST_STORE_FULL;
        }
        ret = app_gapk_authenticate(sys_dl_info.file_info.len);
        // 读取最后512字节进行验签
        break;
    case DOWNLOAD_TYPE_SAPK :
        ret = app_sapk_authenticate(sys_dl_info.file_info.len);
        break;
    default :
        break;
    }
    
    if(ret != 0)
    {
        TRACE("\r\n写入失败：%d",ret);
        return CMDST_OTHER_ER;
        //写入失败
    }
    return CMDST_OK;
}

/*
0-	工作状态 
1-	防拆检测等级(等级查看参见3.1.5）
2-	安全认证等级
3-	防拆触发事件
4-	版本信息
5-	主板ID
6-	硬件配置信息
7-	整机SN
8-	CPU SN（预留）
 */
#define CPU_WORKSTAUS           0
#define CPU_ATTACKCHECK_CLASS   1
#define CPU_AUTH_CLASS          2
#define CPU_ATTACK_STATUS       3
#define CPU_VERSION             4
#define CPU_MAINBOAR_ID         5
#define CPU_HARAWARE_INFO       6
#define CPU_SN                  7
#define CPU_CPUSN               8

#define CPU_WORKSTATUS_NORMAL           0

#define CPU_VERSION_BOOT                0
#define CPU_VERSION_CTRL                1
#define CPU_VERSION_HARD                2

#define CPU_HARAWARE_INFO_MAG           0   // 磁卡
#define CPU_HARAWARE_INFO_RFID          1   // 非接
#define CPU_HARAWARE_INFO_IC            2   // IC卡
#define CPU_HARAWARE_INFO_SCANER        3   // 扫描头
#define CPU_HARAWARE_PRINTER            4   // 打印 
#define CPU_HARAWARE_SPIFLASH           5   // 外部spiflash
#define CPU_HARAWARE_SECUCHIOP          6   // 国密安全CPU

extern int s_fac_ctrl_get_MBID(uchar *id);
extern uint32_t security_level_get(void);
uint16_t sys_cpu_get_inform(uint8_t *data,uint8_t *output,uint32_t *data_len)
{
    char type; 
    uint16_t ret = 0;
    char tmpbuffer[32];
//    int securlevel;
    int temp;
    memset(tmpbuffer, 0, sizeof(tmpbuffer));
    type = data[0];
    switch ( type )
    {
    case  CPU_WORKSTAUS:
        output[0] = CPU_WORKSTATUS_NORMAL; 
        *data_len = 1;
        break;
    case  CPU_ATTACKCHECK_CLASS:
        temp = security_level_get();
        TRACE("securlevel :%x\r\n", temp);
        if ( temp == 0xffffffff ) {
            // 没有设置过安全等级 返回0
            memset(output, 0, 4);            
        }else{
            output[0] = temp & 0xff;            
            output[1] = (temp>>8) & 0xff;            
            output[2] = (temp>>16) & 0xff;            
            output[3] = (temp>>24) & 0xff;            
        }
        *data_len = 4;
        break;
    case  CPU_AUTH_CLASS:
        output[0] = 0;
        *data_len = 1;
        break;
    case  CPU_ATTACK_STATUS:
        // 只读取状态
        sys_mdf_status_get(&temp, (uchar *)(&tmpbuffer[0]));
        TRACE("get the attack status:%x\r\n", temp);
        output[0] = temp & 0xff;            
        output[1] = (temp>>8) & 0xff;            
        output[2] = (temp>>16) & 0xff;            
        output[3] = (temp>>24) & 0xff;
        *data_len = 4;
        break;
    case  CPU_VERSION:
        // data[1]  version info
        switch ( data[1] )
        {
        case CPU_VERSION_BOOT :
            sys_read_ver(READ_BOOT_VER,tmpbuffer);
            TRACE("boot:%s\r\n", tmpbuffer);
            *data_len = strlen(tmpbuffer);
            memcpy(output, tmpbuffer, *data_len);
            break;
        case CPU_VERSION_CTRL :
            sys_read_ver(READ_CTRL_VER,tmpbuffer);
            TRACE("ctrl:%s\r\n", tmpbuffer);
            *data_len = strlen(tmpbuffer);
            memcpy(output, tmpbuffer, *data_len);
            break;
        case CPU_VERSION_HARD :
            memcpy(output, VERSION_HARD, strlen(VERSION_HARD));
            TRACE("hardware:%s\r\n", VERSION_HARD);
            *data_len = strlen(VERSION_HARD);
            break;
        default :
            *data_len = 0;
            ret = CMDST_PARAM_ER;
            break;
        }
        break;
    case  CPU_MAINBOAR_ID:
        ret = fac_ctrl_get_MBID((uchar *)tmpbuffer);
        TRACE_BUF("MBID", (uchar *)tmpbuffer, 18);
        if ( ret  ) {
            ret = CMDST_FRAME_FORMAT_ER;
        }else{
            memcpy(output, tmpbuffer, FSIZE(FAC_TESTINFO,mbno));
            *data_len = FSIZE(FAC_TESTINFO,mbno);
        }
        break;
    case  CPU_HARAWARE_INFO:
        output[0] = 0x00;
        *data_len = 0;
//#ifdef CFG_MAGCARD 
//        output[0] |= 1<<CPU_HARAWARE_INFO_MAG;
//#endif 
//#ifdef CFG_RFID 
//        output[0] |= 1<<CPU_HARAWARE_INFO_RFID;
//#endif 
//#ifdef CFG_ICCARD 
//        output[0] |= 1<<CPU_HARAWARE_INFO_IC;
//#endif 
//#ifdef CFG_SCANER 
//        output[0] |= 1<<CPU_HARAWARE_INFO_SCANER;
//#endif 
//#ifdef CFG_TPRINTER 
//        output[0] |= 1<<CPU_HARAWARE_PRINTER;
//#endif 
//#ifdef CFG_W25XFLASH 
//        output[0] |= 1<<CPU_HARAWARE_SPIFLASH;
//#endif 
//#ifdef CFG_CFG_SECURITY_CHIP 
//        output[0] |= 1<<CPU_HARAWARE_SECUCHIOP;
//#endif 
        break;
    case  CPU_SN:
        sys_ReadSn_new((char *)(tmpbuffer));
        *data_len = strlen(tmpbuffer);
        memcpy(output, tmpbuffer, *data_len);
        break;
    case  CPU_CPUSN:
        *data_len = 0;
        break;
    default :
        ret = CMDST_PARAM_ER;
        break;
    }
    return ret;
}

extern int fac_ctrl_set_MBID(uint8_t mode ,uchar *id);
extern int sys_WriteSn_new(void *sn);
extern int security_level_set(uint32_t levels);
uint16_t sys_cpu_set_inform(uint8_t *input, int input_len)
{
    char type; 
    uint16_t ret = 0;
    char tmpbuffer[32];
    int securlevel = 0;
    uint16 crc16;
    memset(tmpbuffer, 0, sizeof(tmpbuffer));
    type = input[0];
    switch ( type )
    {
    case  CPU_ATTACKCHECK_CLASS:
        if ( input_len == 6 ) {
            // 设置安全等级，满足设置4字节
            securlevel = input[2]|(input[3]<<8)|(input[4]<<16)|(input[5]<<24);
//            securlevel |= input[3] & 0xff;
//            securlevel |= (input[4]<<8) & 0x000ff00;
//            securlevel |= (input[5]<<16) & 0x00ff0000;
//            securlevel |= (input[6]<<24) & 0xff000000;
            TRACE("set securlevel:%x\r\n", securlevel);
            security_level_set(securlevel);
        }else{
//            TRACE("\r\n&&&&&&&&&&&&&&&&&&&&&attacklen err:%d\r\n", input_len);
            TRACE_BUF("set attack status", input, input_len);
            ret = CMDST_DATA_LEN_ER; 
        }
        break;
    case  CPU_AUTH_CLASS:
//        output[0] = 0;
//        *output_len = 1;
        break;
    case CPU_HARAWARE_INFO:
//        output[0] = 0;
//        *output_len = 1;
        break;
    case  CPU_MAINBOAR_ID:
        if ( input_len >= FSIZE(FAC_TESTINFO,mbno)) {
            // 校验重新计算 
            memcpy(tmpbuffer, (char *)(&input[2]), FSIZE(FAC_TESTINFO,mbno)-2);   
            TRACE_BUF("MBID", (uchar *)tmpbuffer, FSIZE(FAC_TESTINFO,mbno)-2);
            crc16 = calc_crc16(tmpbuffer,(FSIZE(FAC_TESTINFO,mbno)-2),0);
            tmpbuffer[(FSIZE(FAC_TESTINFO,mbno)-2)] = crc16 & 0xff;
            tmpbuffer[(FSIZE(FAC_TESTINFO,mbno)-1)] = (crc16>>8) & 0xff;
            ret = fac_ctrl_set_MBID(1, (uchar *)(&tmpbuffer));
            if ( ret ) {
                TRACE("set MBID ERR:%d\r\n", ret);
                ret = CMDST_OTHER_ER;
            }
        }else{
            TRACE("len err:%d\r\n", input_len);
            ret = CMDST_FRAME_FORMAT_ER;
        }
        break;
    case  CPU_SN:
        if ( input_len > (SYS_SN_DEV_SIZE + 2) || (input_len < 2)) {
            TRACE("len err :%d\r\n", input_len);
            ret = CMDST_PARAM_ER;
        }else{
            memcpy(tmpbuffer, (char *)(&input[2]), input_len-2);
            ret = sys_WriteSn_new((char *)(tmpbuffer));
            if ( ret  ) {
                ret = CMDST_OTHER_ER;
            }
        }
        break;
        //    case  CPU_ATTACK_STATUS:
//    case  CPU_VERSION:
//    case  CPU_HARAWARE_INFO:
//    case  CPU_WORKSTAUS:
//    case  CPU_CPUSN:
    default :
//        *data_len = 0;
        ret = CMDST_CMD_ER;
        break;
    }
    return ret;
}

/*
 *  描述    ：生产测试装配检测时需要向k21请求写入防拆标志，总检检测防拆标志是否
 *  入口参数：0-写入生产防拆标志 1-读取生产防拆状态
 *  返回值  ：写入生产防拆标志：0-写入成功 1-写入失败
 *            读取生产防拆状态：0-未开启   1-已开启
 *
 * */
extern uint Fac_SRAM(uint mode);
uint16_t fac_attack(uint8_t mode, uint8_t *output)
{
    uint status;    
    status = Fac_SRAM(mode);
    output[0] = status&0xff; 
    return 0;
}

uint sys_write_project_id(uchar *id, int len)
{		
    uchar backbuf[SYSZONE_LEN];
    uchar idbuf[32];
    if ( len > 32 ) {
        return ERROR;
    }
    memset(idbuf, 0, sizeof(idbuf));
    memcpy(idbuf, id, len);
    return s_write_syszone(backbuf, SYS_PROJIECT_ID_ADDR, SYS_PROJIECT_ID_SIZE, (uchar *)(&idbuf[0]));
}

/*
 *功能：系统进入低功耗
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t sys_entry_lowpower(uint8_t* data)
{
    //判断当前是否可以进入休眠
    gSystem.lpwr.bm.low_en = 1; //收到低功耗指令 时延后可以进入低功耗
    
    return CMDST_OK; 
}
/*
 *功能：
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
void sys_power_state_inform(void)
{
    MCUPCK res;
    uint status;
    uchar buffer[24];
    if(gSystem.lpwr.bm.low_to_normal == 1)
    {
        gSystem.lpwr.bm.low_to_normal = 0;
        TRACE("\r\nsys normal");       
        status = SYS_STATE_NORMAL; 
        buffer[0] = (uint8_t)status & 0xFF;
        buffer[1] = (uint8_t)(status >> 8) & 0xFF;
        buffer[2] = (uint8_t)(status >> 16) & 0xFF;
        buffer[3] = (uint8_t)(status >> 24) & 0xFF;
        res.cmdcls = CMD_SYS_MANAGE;
        res.cmdop = CMD_SYS_MANAGE_STATE_CHANGE;
        res.len = MIN_CMD_RESPONSE + 2;
        res.sno1 = 0; 
        res.sno2 = 1; 
        ctc_send_frame(res, buffer);
        sys_DelayMs(200);
        ctc_send_frame(res, buffer);
    }
    if(gSystem.lpwr.bm.low_en == 1)
    {
        if(sys_get_counter() > Lowpower_Timer + LOWPOWER_DELAYED)
        {
            gSystem.lpwr.bm.low_en = 0; //进入休眠后，失能休眠
            TRACE("\r\nsys enter_lowerpower_freq");       
            status = SYS_STATE_LOWPOWER; 
            buffer[0] = (uint8_t)status & 0xFF;
            buffer[1] = (uint8_t)(status >> 8) & 0xFF;
            buffer[2] = (uint8_t)(status >> 16) & 0xFF;
            buffer[3] = (uint8_t)(status >> 24) & 0xFF;
            res.cmdcls = CMD_SYS_MANAGE;
            res.cmdop = CMD_SYS_MANAGE_STATE_CHANGE;
            res.sno1 = 0; 
            res.sno2 = 1; 
            res.len = MIN_CMD_RESPONSE + 2;
            ctc_send_frame(res, buffer);

            s_DelayMs(20);

            enter_lowerpower_freq();
        }
    }

}

/**
    1-"卡不在位",
    2-"",
 */
uint16_t s_authentinfo_retvalue(int retvalue)
{
    uint16_t ret;
    switch ( retvalue )
    {
    // 0-"操作成功",
    case  0:
        ret = CMDST_OK;
        break;
        // 1-"卡不在位",
    case  1:
        ret = CMDST_IC_EXIST_ER;
        break;
        // 2-"上电失败",
    case  2:
        ret = CMDST_IC_RESET_ER;
        break;
        // 3-"无效卡",
    case  3:
        ret = CMDST_IC_INVALID_ER;
        break;
        // 4-"卡片过期无效",
    case  4:
        ret = CMDST_IC_INVALID_ER;
        break;
        // 5-"卡片认证失败",
    case  5:
        ret = CMDST_IC_CODE_ER;
        break;
        // 6-"密码错误",
    case  6:
        ret = CMDST_IC_INVALID_ER;
        break;
        // 7-"非法卡",
    case  7:
        ret = CMDST_IC_INVALID_ER;
        break;
        // 8-"卡有效次数用尽",
    case  8:
        ret = CMDST_IC_CODECOUNT_ER;
        break;
        // 9-"卡有效次数不足",
    case  9:
        ret = CMDST_IC_CODECOUNT_ER;
        break;
        // 10-"卡片异常",
    case  10:
        ret = CMDST_IC_RESET_ER;
        break;
        // 11-"密码长度错误",
    case  11:
        ret = CMDST_IC_CODE_ER;
        break;
        // 12-"卡片已锁定",
    case  12:
        ret = CMDST_IC_LOCK_ER;
        break;
    default :
        ret = CMDST_OTHER_ER;
        break;
    }
    return ret;
}

/*
 *功能：系统进入低功耗
 *参数：data字节流包括：
        type 管理类型：     0-切机管理 1-防拆解除管理 2-特权模式
        mode 工作模式：     0-采用IC卡模式 1-权限包模式
 *      authdata 权限包:    (切机包或防拆解除包）:
 *返回值：
 *注：
 */
uint16_t sys_safe_manage(uint8_t* data, int datalen, uint8_t* output, uint32_t *outlen)
{
    char type;
    char mode;
    char *authdata;
    int result;
    char outputbuf[50];
//    int outlen;
    uint16_t ret;

    type = data[0];
    mode = data[1];

    *outlen = 0;
    ret = CMDST_OTHER_ER;

    if ( (type > SAFEMANAGE_TYPE_SUPERAUTH)||(mode > SAFEMANAGE_MODE_AUTHEPAG) ) {
        ret = CMDST_PARAM_ER;
    }
    if ( datalen > 2 ) {
        authdata = (char *)&data[2];
    }
    
    switch ( type )
    {
    case  SAFEMANAGE_TYPE_ORIGIN:
        // 切机回到正常模式 
        if (  mode == SAFEMANAGE_MODE_IC ) {
            // ic卡
            result = s_OperAuthICCard(SAFEMANAGE_TYPE_ORIGIN, datalen-2, authdata, (int *)outlen,(char *)&outputbuf[0]);
            ret = s_authentinfo_retvalue(result);
            memcpy(output, outputbuf, *outlen);
            if ( ret == CMDST_OK ) {

            }
        }else{
            // 权限包
        }
        // 允许切机操作
        if ( ret == CMDST_OK ) {
            // 读取原保存在系统中设置的检测防拆点
            result = security_level_get();
            if ( 0 == result ) {
                result = TAMPER_MASK; // 如果没有设置过反拆开启默认 
            }
            security_level_set(result);
            TRACE("get the attack level:%x\r\n", result);
        }

        break;
    case  SAFEMANAGE_TYPE_TAMPERDIS:
        // 防拆解除模式
        if (  mode == SAFEMANAGE_MODE_IC ) {
            result = s_OperAuthICCard(SAFEMANAGE_TYPE_ORIGIN, datalen-2, authdata, (int *)outlen,(char *)&outputbuf[0]);
            ret = s_authentinfo_retvalue(result);
            memcpy(output, outputbuf, *outlen);
            // 解除防拆
        }else{
       
        }
        if ( ret == CMDST_OK ) {
            // 读取原保存在系统中设置的检测防拆点
            result = security_level_get();
            if ( 0 == result ) {
                result = TAMPER_MASK; // 如果没有设置过反拆开启默认 
            }
            security_level_set(result);
            TRACE("get the attack level:%x\r\n", result);
        }
        break;
    case  SAFEMANAGE_TYPE_SUPERAUTH:
        ret = CMDST_OK;
        // 超级管理模式 
        break;
    default :
        break;
    }
    
    return ret; 
}


