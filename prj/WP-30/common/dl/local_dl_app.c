/***************** (C) COPYRIGHT 2015 START Computer equipment *****************
 * File Name          : local_dl_app.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/13/2015 9:54:42 AM
 * Description        : 
 *******************************************************************************/
#include "app_common.h"
#include "local_dl_app.h"

static uint8_t gLocalDlCom=WORK_COMPORT;   //下载口

static void dlcom_isr(void)
{
    char ch;

    ch = hw_uart_getchar((UARTn)gLocalDlCom);
    QueueWrite(&sgSerialOpt[gLocalDlCom].queue,
               (unsigned char *)&ch,
               1);
}

int dlcom_open(uint8_t commport,int bps, uint8_t *buf_dl, uint32_t buf_size)
{
    //todo: add other download port:e.g USB CDC
    if (commport == USBD_CDC)
    {
#if defined(CFG_USBD_CDC)
        return drv_usbd_cdc_open(buf_dl,buf_size);
#else
        return -1;
#endif
    } else
    {
        gLocalDlCom = commport;
        return drv_uart_open((UARTn)commport, bps,
                             buf_dl,buf_size,
                             dlcom_isr);
    }
}

int dlcom_close(uint8_t commport)
{
    //todo: add other download port:e.g USB CDC
    if (commport == USBD_CDC)
    {
#if defined(CFG_USBD_CDC)
        return drv_usbd_cdc_close();
#else
        return 0;
#endif
    } else
    {
        return drv_uart_close((UARTn)commport);
    }
}

int dlcom_check_readbuf(uint8_t commport)
{
    //todo: add other download port:e.g USB CDC
    if (commport == USBD_CDC)
    {
#if defined(CFG_USBD_CDC)
        return drv_usbd_cdc_check_readbuf();
#else
        return -1;
#endif
    } else
    {
        return drv_uart_check_readbuf((UARTn)commport);   
    }
}

int dlcom_read(uint8_t commport, uint8_t *output, uint32_t length, int t_out_ms)
{
    //todo: add other download port:e.g USB CDC
    if (commport == USBD_CDC)
    {
#if defined(CFG_USBD_CDC)
        return  drv_usbd_cdc_read(output,length,t_out_ms);
#else
        return -1;
#endif
    } else
    {
        return drv_uart_read((UARTn)commport, output, length, t_out_ms);
    }
}

int dlcom_write(uint8_t commport, uint8_t const *input, uint32_t length)
{
    //todo: add other download port:e.g USB CDC
    if (commport == USBD_CDC)
    {
#if defined(CFG_USBD_CDC)
        return drv_usbd_cdc_write(input,length);
#else
        return -1;
#endif
    } else
    {
        return drv_uart_write((UARTn)commport, input, length);
    }
}

int dlcom_clear(uint8_t commport)
{
    //todo: add other download port:e.g USB CDC
    if (commport == USBD_CDC)
    {
#if defined(CFG_USBD_CDC)
        return drv_usbd_cdc_clear();
#else
        return -1;
#endif
    } else
    {
        return  drv_uart_clear((UARTn)commport);
	}
}

//return: commport number
uint8_t localdl_com_open(struct LocalDlOpt *localdl)
{
    uint8_t i = 0;

    for (i=0; i<lOCALDL_COMPORT_NUM; i++)
    {
        if (SERIL_NOTVALID != localdl->commbuf[i])
        {
            dlcom_close(localdl->commbuf[i]);
            dlcom_open(localdl->commbuf[i],9600,localdl->serialbuf[i],localdl->seriallen[i]);
        }
    }
    return i;
}

void localdl_com_close(struct LocalDlOpt *localdl)
{
    uint8_t i = 0;

    for (i=0; i<sizeof(localdl->commbuf); i++)
    {
        if (SERIL_NOTVALID != localdl->commbuf[i])
        {
            dlcom_close(localdl->commbuf[i]);
        }
    }
}

int s_localdl_save_data(void *data)
{
    uint32_t addr,space_size;
    uint32_t offset;
    uint16_t data_len;
    struct LocalDlOpt *localdl = (struct LocalDlOpt *)data;

//    TRACE("\n-|savedata");
    switch (localdl->request)
    {
    case FILE_TYPE_BOOT:
        addr = SA_BOOT;
        localdl->contrl.bit.boot_update = 1;
        space_size = addr+LEN_BOOT;
        if (localdl->filelen > LEN_BOOT) {
            return EAPP_NOSPACE;
        }
        break;
    case FILE_TYPE_CTRL:
        addr = SA_CTRL;
        localdl->contrl.bit.ctrl_update = 1;
        space_size = addr+LEN_CTRL;
        if (localdl->filelen > LEN_CTRL) {
            return EAPP_NOSPACE;
        }
        break;
    case FILE_TYPE_FONT:
//        addr = SA_KEYZONE;
//        space_size = addr+LEN_ZK;
//        if (localdl->filelen > LEN_ZK) {
//            return EAPP_NOSPACE;
//        }
        return EAPP_NOSPACE;
        break;
    default:
        return EAPP_ERROR;
    }
    offset = msb_byte4_to_uint32(localdl->frame->bin+1);
    addr += offset;
    data_len = msb_byte4_to_uint32(localdl->frame->bin+5);
//    TRACE("\n-|--Addr:%08X len:%d toal:%d",addr,data_len,localdl->filelen);
//    TRACE_BUF("data",localdl->frame->bin+9,data_len);
    if (addr+data_len >= space_size) {
        return EAPP_NOSPACE;
    }
    if (flash_write(addr,data_len,localdl->frame->bin+9)) {
        return EAPP_WRITE;
    }
    localdl->percent = ((data_len+offset)*100)/localdl->filelen;
    if ((localdl->contrl.bit.ctrl_update || localdl->contrl.bit.boot_update) && localdl->percent == 100) {
        if (localdl->callback->after_done != NULL) {
            return localdl->callback->after_done(localdl);
        }
    }
//    TRACE_BUF("flash",(uint8_t *)addr,data_len);
    return OK;
}

int localdl_get_boot_version(const VERSION_INFO_NEW ** const ver)
{

    VERSION_INFO_NEW *pVer= (VERSION_INFO_NEW *)(*((uint32_t *)(SA_BOOT+0x410)));
//    TRACE("\n\t-|SA:%04X-%04X-VERADDR:%04X-%4X",pVer,&pVer,(uint32_t)(pVer),(uint32_t)&(*pVer));
//    TRACE("\n-|product:%s",pVer->product);
//    TRACE("\n-|version:%s",pVer->version);
//    TRACE("\n-|complietime:%s",pVer->time);
    if ((ver == NULL) || ((uint32_t)pVer + sizeof(VERSION_INFO_NEW) >= SA_CTRL))
    {
        return -ERROR;
    } else {
        *ver = pVer;
        return OK;
    }
}

int localdl_get_ctrl_version(const VERSION_INFO_NEW ** const ver)
{

    VERSION_INFO_NEW *pVer= (VERSION_INFO_NEW *)(*((uint32_t *)(SA_CTRL+0x410)));
//    TRACE("\n\t-|SA:%04X-%04X-VERADDR:%04X-%4X",pVer,&pVer,(uint32_t)(pVer),(uint32_t)&(*pVer));
//    TRACE("\n-|product:%s",pVer->product);
//    TRACE("\n-|version:%s",pVer->version);
//    TRACE("\n-|complietime:%s",pVer->time);
    if ((ver == NULL) || ((uint32_t)pVer + sizeof(VERSION_INFO_NEW) >= SA_CTRL+LEN_CTRL))
    {
        return -ERROR;
    } else {
        *ver = pVer;
        return OK;
    }
}

int localdl_get_font_version(char *ver)
{
    char *p;
    uint8_t i;

    p= (char *)SA_KEYZONE;
    for (i=0; i<18; i++)
    {
        if (p[i] == 0 || p[i] == '\0')
        {
            strcpy(ver, (char*)SA_KEYZONE);
            return 0;
        }
    }
    strcpy(ver,"NULL");
    return 0;
}

int s_localdl_get_terminal_info(struct LocalDlFrame *frame)
{
	char  temp[64];
    char *pTinfo = (char *)frame->bin;

	memset(pTinfo, 0x00, DL_LOCAL_FRAME_SIZE);
	//  设置配置信息
	sprintf(pTinfo+strlen(pTinfo), "DL_VER=%s\r\n", DL_CFG_VERSION);
	sprintf(pTinfo+strlen(pTinfo), "DL_COMPRESS=%s\r\n", DL_CFG_COMPRESS);
	sprintf(pTinfo+strlen(pTinfo), "DL_ENCRYPT=%s\r\n", DL_CFG_ENCRYPT);
	sprintf(pTinfo+strlen(pTinfo), "DL_CHECKSUM=%s\r\n", DL_CFG_CHECKSUM);
	sprintf(pTinfo+strlen(pTinfo), "DL_AUTH=%s\r\n", DL_CFG_AUTH);
	sprintf(pTinfo+strlen(pTinfo), "DL_PACKLEN=%d\r\n", DL_CFG_PACKLEN);
	sprintf(pTinfo+strlen(pTinfo), "DL_MAXBAUD=%d\r\n", DL_CFG_MAX_BAUD);
	//  设置终端信息
	sprintf(pTinfo+strlen(pTinfo), "TERM_VENDOR=%s\r\n", DL_CFG_VENDOR);
	sprintf(pTinfo+strlen(pTinfo), "TERM_MODULE=%s\r\n", DL_CFG_MODULE);
	memset(temp, 0x00, sizeof(temp));
	s_localdl_get_sn(temp);
	sprintf(pTinfo+strlen(pTinfo), "TERM_SN=%s\r\n", temp);
	//  设置硬件版本，需要修改接口函数
	memset(temp, 0x00, sizeof(temp));
	sys_read_ver(READ_MAINB_VER, temp);
	sprintf(pTinfo+strlen(pTinfo), "HW_MAINB=%s\r\n", temp);
//	memset(verinfo, 0x00, sizeof(verinfo));
////	sys_read_ver(READ_KEYB_VER, verinfo);
////	sprintf(pTinfo->bin+strlen(pTinfo->bin), "HW_KEYB=%s\r\n", verinfo);
//	memset(verinfo, 0x00, sizeof(verinfo));
//	sys_read_ver(READ_PORTB_VER, verinfo);
//	sprintf(pTinfo->bin+strlen(pTinfo->bin), "HW_PORTB=%s\r\n", verinfo);
	//  设置软件信息
	sprintf(pTinfo+strlen(pTinfo), "SW_BOOT=%s\r\n",S1000_BOOT_VER);
	sprintf(pTinfo+strlen(pTinfo), "SW_CTRL=%s\r\n", S1000_CTRL_VER);
    //MK210提供应用版本V3.0.0.X  监控版本与wp30同步(pc端查看)
//	sprintf(pTinfo+strlen(pTinfo), "SW_CTRL=%s\r\n", MK210_CTRL_VER);
    localdl_get_font_version(temp);
	sprintf(pTinfo+strlen(pTinfo), "SW_FONT=%s\r\n",(char*)temp);
	sprintf(pTinfo+strlen(pTinfo), "SW_CUR=%s\r\n","1.0.0");
//	freespace = fs_freespace();
//	sprintf(pTinfo->bin+strlen(pTinfo->bin), "FS_FREESPACE=%d\r\n", freespace);

//	s_dl_show_ret(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE, OK);
//	s_dl_send_pack(CommPort, recv_cmd, OK, strlen(frame->bin), frame->bin);
    msb_uint16_to_byte2(0x00,frame->recode);
    frame->bin_len = strlen(pTinfo);
//    TRACE("\nbinlen:%d",frame->bin_len);

	return(OK);
}

int read_sm_fsk(uint type, uchar *key)
{
    if ( type == FSK_SMAPP) {
        // 先明文获取后面修改为密文
        memcpy(key,(uint8_t *)DEFAULT_FSK_SM,16);
    } else {
    }
    return 0;
}
// 计算BOOT和CTRL层的签名
int GetCodeSignature(uint8_t *sha, uint8_t *mac, uint32_t ctrl_len)
{
	uint8_t fsk[FSK_LEN];
//    lcd_Display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE,"ctrl_len:%x",ctrl_len);
	if (!sha256((uint8_t *)SA_BOOT,(LEN_BOOT+ctrl_len),sha))
	{		
		if(!read_sm_fsk(FSK_SMAPP,fsk))
		{
			cacul_mac_ecb(FSK_LEN,fsk,LEN_BSV_SHA,sha,LEN_BSV_MAC,mac);
			CLRBUF(fsk);
			return 0;
		}				
	}
    return -1;
}
// BOOT层校验CTRL
int localdl_boot_make_check(void *data)
{
    struct LocalDlOpt *localdl = (struct LocalDlOpt *)data;
    uint8_t flash[FLASH_SECTOR_SIZE];
    uint8_t buffer[OFFSET_BPS];
    SYSZONE_DEF *pSyszone = (SYSZONE_DEF *)buffer;

    //make BSV
    s_read_syszone(OFFSET_APP_CHECK,sizeof(buffer),buffer);
    strcpy((char *)pSyszone->appcheck,"STAR");
    lsb_uint32_to_byte4(localdl->filelen,pSyszone->appcheck+4); 
    strcpy((char *)pSyszone->boot_info, LABLE_BOOTINFO);
    if (GetCodeSignature(pSyszone->boot_sha2,pSyszone->boot_mac,localdl->filelen)) {
        return EAPP_WRITE;
    }
    //update app frist-run flag
    pSyszone->app_update = SYSZONE_APPUPDATE_FLAG;
    if (s_write_syszone(flash,OFFSET_APP_CHECK,OFFSET_BPS,buffer)) {
        return EAPP_WRITE;
    }
    return 0;
}
// CTRL层校验BOOT
int localdl_ctrl_make_check(void *data)
{
    uint8_t *flash=NULL;
    uint8_t buffer[32+4];
    uint8_t sha[32];
    uint8_t mac[4];
    uint32_t ctrl_len = sys_get_ctrllen(0);
    //make BSV
    if (GetCodeSignature(sha,mac,ctrl_len)) {
        return EAPP_WRITE;
    }
    memcpy(buffer,sha,sizeof(sha));
    memcpy(buffer+32,mac,sizeof(mac));
    if ((flash = malloc(FLASH_SECTOR_SIZE)) == NULL)
    {
        return EAPP_WRITE;
    }
    if (s_write_syszone(flash,(FPOS(SYSZONE_DEF,boot_sha2)),sizeof(buffer),buffer)) {
        free(flash);
        return EAPP_WRITE;
    }
    free(flash);
    return 0;
}



int s_localdl_cancle(void)
{
    return drv_kb_ifkey(KEY_CANCEL);
}

int s_localdl_save_sn(uint8_t *sn,uint8_t length)
{
    uint8_t *backbuf = NULL;
    uint8_t snbuf[SYS_SN_DEV_SIZE];
    uint8_t i;
    int iRet=OK;

    if (length > SYS_SN_DEV_SIZE)
    {
        return -1;
    }
    for (i = 0; i < length; i++)
    {
        if (!((sn[i] >= '0' && sn[i] <= '9') || (sn[i] >= 'a' && sn[i] <= 'z') || (sn[i] >= 'A' && sn[i] <= 'Z'))) {
            return ERROR;
        }
    }
    CLRBUF(snbuf);
    memcpy(snbuf,sn,length);
    backbuf = malloc(FLASH_SECTOR_SIZE);
    if (NULL != backbuf) 
    {
        iRet = s_write_syszone(backbuf, SYS_SN_DEV_ADDR, SYS_SN_DEV_SIZE,snbuf);
        free(backbuf);
    }
    return (iRet?ERROR:OK);
}

int s_localdl_get_sn(void *sn)
{
    int i,len;

    uchar   *psn = (uchar *)sn;
    if(sn == NULL)
    {
        return -1;
    }
    s_read_syszone(SYS_SN_DEV_ADDR, SYS_SN_DEV_SIZE, psn);
    len = strlen((char const *)psn);
    if ( len > SYS_SN_DEV_SIZE ) {
        memset(psn,0x30, SYS_SN_DEV_SIZE);
        psn[SYS_SN_DEV_SIZE] = 0;
        return 0;
    }
    for (i = 0; i < len; i++)
    {
        if (!((psn[i] >= '0' && psn[i] <= '9') 
              || (psn[i] >= 'a' && psn[i] <= 'z')
              || (psn[i] >= 'A' && psn[i] <= 'Z'))) {
            memset(psn,0x30, SYS_SN_DEV_SIZE);
            break;
        }
    }
    psn[SYS_SN_DEV_SIZE] = 0;
    return 0;
}


const char *  const gcpLocaldlDisTab[] = {
    "失败",         //0
    "命令不支持",   //1
    "包检验错误",   //2
    "文件检验错误", //3
    "写入失败",     //4
    "空间不足",     //5
    "应用个数达最大值", //6
    "文件个数达最大值", //7
    "无应用",           //8
    "应用格式不支持",   //9
    "BPS不支持",        //10
    "时钟错误",         //11
    "超时错误",         //12
    "结束错误",         //13
    "下载错误",         //14
    "内存不足",         //15
    "RSA不支持",        //16
    "RSA不支持",        //17
};

int s_localdl_display(int mode,void *data)
{
#if defined (CFG_LCD)
    struct LocalDlOpt *localdl = (struct LocalDlOpt *)data;

    if (localdl->dllevel == DL_HANDSHAKE_BOOT) 
    {
        lcd_display(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY, "BOOT本地下载");
    } else
    {
        lcd_display(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY, "CTRL本地下载");
    }
    if (mode == 1)
    {
        lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "%s",localdl->title);
        return 0;
    }
    switch (msb_byte2_to_uint16(localdl->frame->cmd))
    {
        case DLCMD_TRANSDATA:
            switch (localdl->request)
            {
            case FILE_TYPE_BOOT:
                lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "BOOT %d%%", localdl->percent);
                break;
            case FILE_TYPE_CTRL:
                lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "CTRL %d%%", localdl->percent);
                break;
            case FILE_TYPE_FONT:
                lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "FONT %d%%", localdl->percent);
                break;
            default:
                lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "下载类型错误:%02X",localdl->request);
                break;
            }
            break;
        case DLCMD_SAVEFILE:
        case  DLCMD_FINISH:
            lcd_display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "%s", localdl->title);
            break;
        case DLCMD_FAC_CTRL:
            break;
        default:
            lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "%s", localdl->title);
            if (localdl->dlrecode>=EAPP_ERROR && localdl->dlrecode <=EAPP_RSAKEYCHECK) {
                lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "%s",gcpLocaldlDisTab[localdl->dlrecode-EAPP_ERROR]);
            } else if (localdl->dlrecode) {
                lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "cmd:%02X失败%02X",msb_byte2_to_uint16(localdl->frame->cmd),localdl->dlrecode);
            } else {
                lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "成功!", localdl->dlrecode);
            }
            break;
    }
    
#endif
    return 0;
}






