/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : sys_inf_zone.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 4/3/2014 7:01:05 PM
 * Description        : 
 *******************************************************************************/
#include "app_common.h"
#include "sys_inf_zone.h"
//#include "wp30_ctrl_cfg.h"
//#include "wp30_ctrl.h"

const SYSZONE_DEF *const gpSys = (SYSZONE_DEF *)SA_SYSZONE; 
version_t k_MainVer;
const char *const k_TermName = TERMNAME;
/**********************************************************************
  from buf save syszone_back
***********************************************************************/
uint save_syszone_back(uchar *buf)
{
	if(eraseFlashPage(SA_SYSZONE_BACK/2))
	{
		return ERROR;
	}
	if(writeFlashPage(SA_SYSZONE_BACK/2,buf,SYSZONE_LEN))
	{
		return ERROR;
	}
	return OK;
}
/**********************************************************************
*    read data from syszone
* in： 
*     uiOffset:offset
*     len:len
* out：
***********************************************************************/
uint s_read_syszone(uint offset, uint len, uchar *ucBuf)
{
	readFlashPage((SA_SYSZONE+offset)/2, ucBuf, (ushort)len);
	return 0;
}
/**********************************************************************
*    write data from syszone
* in： 
*     uiOffset:offset
*     len:len
* out：
***********************************************************************/
uint s_write_syszone(uchar *backbuf, uint uiOffset, uint uiLen, uchar *ucBuf)
{
	uint Crc;
	readFlashPage(SA_SYSZONE/2, backbuf, OFFSET_SYSZONE_CHECK);
	memcpy(&backbuf[uiOffset],ucBuf,uiLen);
    Crc = calcCrc32((uint *)backbuf,OFFSET_SYSZONE_CHECK/sizeof(uint),0);
	memcpy(&backbuf[OFFSET_SYSZONE_CHECK],(uchar *)&Crc,LEN_SYSZONE_CHECK);
	if(eraseFlashPage(SA_SYSZONE/2))
	{
		return ERROR;
	}
	if(writeFlashPage(SA_SYSZONE/2,backbuf,SYSZONE_LEN))
	{
		return ERROR;
	}
//vDispBufTitle("SYS0", SYSZONE_LEN, 0, backbuf);	
	return save_syszone_back(backbuf);
}

int32_t  s_sysconfig_read(int offset,int readlen,void *vbuf)
{
    s_read_syszone(offset, readlen, (uchar *)vbuf);
    return 0;
}

int32_t  s_sysconfig_write(int offset,int writelen,void *vbuf)
{
    uchar *buf = (uchar *)vbuf;
    int ret = -1;
    uchar *backup;

    backup = malloc(SYSZONE_LEN);
    if ( backup == NULL ) {
        return ret;
    }
    TRACE("(%d)offset:%d writelen:%d\r\n",__LINE__,offset,writelen);
    s_read_syszone(0, SYSZONE_LEN, (uchar *)backup);
	if(memcmp(&backup[offset], buf, writelen)) {
        // 不一样需要修改
        ret = s_write_syszone(backup,offset, writelen, buf);
    }
    free(backup);
    return 0;
}


/**********************************************************************
*   from backup syszone recover syszone
***********************************************************************/
uint resume_syszone(uchar *buf)
{
	if(eraseFlashPage(SA_SYSZONE/2))
	{
		return ERROR;
	}
	if(writeFlashPage(SA_SYSZONE/2,buf,SYSZONE_LEN))
	{
		return ERROR;
	}
	return OK;
}
/**********************************************************************
*   check syszone
*   type=0  syszone
*       =1  backup syszone 
***********************************************************************/
uint check_syszone(uint type, uchar *buf)
{
	uint check;
	if(type == 0)
	{
 		readFlashPage(SA_SYSZONE/2, buf, SYSZONE_LEN);
	}
	else
	{
		readFlashPage(SA_SYSZONE_BACK/2, buf, SYSZONE_LEN);
	}	
    check = calcCrc32((uint *)buf,OFFSET_SYSZONE_CHECK/sizeof(uint),0);
//    vDispBufTitle(NULL, SYSZONE_LEN, 0,buf);
//    TRACE("\r\ncheck=<%x>",check);
	if(memcmp((uchar *)&check,&buf[OFFSET_SYSZONE_CHECK],LEN_SYSZONE_CHECK))
	{
		check = 1;
	}
	else
	{
		check = 0;
	}
	return check;
}

//***************************************
//  sys psw
// mode: 0-super admin psw
//       1-adminA psw  2-adminB psw
//***************************************
uint read_syspsw(uint mode, uint *len, uchar *psw)
{
    uchar key[8];
    uchar psw_buf[LEN_PSW];
    uint i;
    if ( mode == 0 ) {
        s_read_syszone(SYS_SUPER_PSW_ADDR,SYS_SUPER_PSW_SIZE,psw_buf);
    } else if (mode == 1) {
        s_read_syszone(SYS_ADMINA_PSW_ADDR,SYS_ADMINA_PSW_SIZE,psw_buf);
    } else {
        s_read_syszone(SYS_ADMINB_PSW_ADDR,SYS_ADMINB_PSW_SIZE,psw_buf);
    }
    PCI_GetHSK(1,key);
    des_decrypt_ecb(psw_buf, psw, LEN_PSW, key, sizeof(key)/8);
//    DISPBUF("read psw",LEN_PSW,0,psw);
    CLRBUF(key);
    for ( i=0 ; i<LEN_PSW ; i++ ) {
        if ( psw[i] < '0' || psw[i] > '9' ) {
            if ( mode == 0 ) {
                memcpy(psw,DEFAULT_SUPERPSW_SYS,strlen(DEFAULT_SUPERPSW_SYS));
            } else if (mode == 1) {
                memcpy(psw,DEFAULT_ADMINAPSW_SYS,strlen(DEFAULT_ADMINAPSW_SYS));
            } else {
                memcpy(psw,DEFAULT_ADMINBPSW_SYS,strlen(DEFAULT_ADMINBPSW_SYS));
            }
            break;
        }
    }
    *len = LEN_PSW;
    return 0;
}

uint write_syspsw(uint mode, uint len, uchar *psw)
{		
    uchar key[8];
    uchar psw_buf[LEN_PSW];
    uchar backbuf[SYSZONE_LEN];
    PCI_GetHSK(1,key);
    des_encrypt_ecb(psw,psw_buf,LEN_PSW, key, sizeof(key)/8);	
//    DISPBUF("write psw",LEN_PSW,0,psw_buf);
    CLRBUF(key);
    if ( mode == 0 ) {
        return s_write_syszone(backbuf,SYS_SUPER_PSW_ADDR,SYS_SUPER_PSW_SIZE,psw_buf);
    } else if (mode == 1) {
        return s_write_syszone(backbuf,SYS_ADMINA_PSW_ADDR,SYS_ADMINA_PSW_SIZE,psw_buf);
    } else {
        return s_write_syszone(backbuf,SYS_ADMINB_PSW_ADDR,SYS_ADMINB_PSW_SIZE,psw_buf);
    }
}
//sysinfo参数
int s_sysinfo_sysinfo (uint mode, void *p)
{
    int ret;
    uint offset,len;
    offset = FPOS(SYSZONE_DEF,tsysinfo);
    len = FSIZE(SYSZONE_DEF,tsysinfo);
    if ( mode ) {
        ret = s_sysconfig_write(offset,len,p);
    } else {
        ret = s_sysconfig_read(offset,len,p);
    }
    return ret;
}		/* -----  end of function s_sysinfo_sysinfo  ----- */

//rfid参数
int s_sysinfo_rfidinfo (uint mode, void *p)
{
    int ret;
    uint offset,len;
    offset = FPOS(SYSZONE_DEF,trfidinfo);
    len = FSIZE(SYSZONE_DEF,trfidinfo);
    if ( mode ) {
        ret = s_sysconfig_write(offset,len,p);
    } else {
        ret = s_sysconfig_read(offset,len,p);
    }
    return ret;
}		/* -----  end of function s_sysinfo_rfidinfo  ----- */

//bluetooth参数
int s_sysinfo_bluetoothinfo (uint mode, void *p)
{
    int ret;
    uint offset,len;
    offset = FPOS(SYSZONE_DEF,bluetoothinfo);
    len = FSIZE(SYSZONE_DEF,bluetoothinfo);
    if ( mode ) {
        ret = s_sysconfig_write(offset,len,p);
    } else {
        ret = s_sysconfig_read(offset,len,p);
    }
    return ret;
}		/* -----  end of function s_sysinfo_bluetoothinfo  ----- */

int s_sysinfo_magcardinfo (uint mode, void *p)
{
    int ret;
    uint offset,len;
    offset = FPOS(SYSZONE_DEF,tmagcardinfo);
    len = FSIZE(SYSZONE_DEF,tmagcardinfo);
    if ( mode ) {
        ret = s_sysconfig_write(offset,len,p);
    } else {
        ret = s_sysconfig_read(offset,len,p);
    }
    return ret;
}		/* -----  end of function s_sysinfo_magcardinfo  ----- */

int s_sysinfo_iccardinfo (uint mode, void *p)
{
    int ret;
    uint offset,len;
    offset = FPOS(SYSZONE_DEF,ticcardinfo);
    len = sizeof(MODULE_ICCARD_INFO);
    if ( mode ) {
        ret = s_sysconfig_write(offset,len,p);
    } else {
        ret = s_sysconfig_read(offset,len,p);
    }
    return ret;
}		/* -----  end of function s_sysinfo_iccardinfo  ----- */

//main version  pcb参数
int s_sysinfo_mbVersioninfo(uint mode, void *p)
{
    int ret;
    uint offset,len;
    offset = FPOS(SYSZONE_DEF,main_version);
    len = FSIZE(SYSZONE_DEF,main_version);
    if ( mode ) {
        ret = s_sysconfig_write(offset,len,p);
    } else {
        ret = s_sysconfig_read(offset,len,p);
    }
    return ret;
}		/* -----  end of function s_sysinfo_rfidinfo  ----- */

//gapk sha num  pcb参数
int s_sysinfo_gpakshainfo(uint mode, uint32_t *p)
{
    int ret;
    uint offset,len;
    offset = FPOS(SYSZONE_DEF,gapksha_num);
    len = FSIZE(SYSZONE_DEF,gapksha_num);
    if ( mode ) {
        ret = s_sysconfig_write(offset,len,p);
    } else {
        ret = s_sysconfig_read(offset,len,p);
    }
    return ret;
}

//security random 获取random随机数,三组随机数，每组8字节，用于系统降级等操作 
//mode  :  0-read       1- write
//index :  0-2 valid    index>=3 invalid return err
//p     :  mode=0 get result mode=1-null 
//return:  0-suc  其他-fail
int s_sysinfo_security_random(uint mode, uint index, char *p)
{
    int ret;
    uint offset,len;
    //    uchar random[8];
    if ( index >= 3) {
       return  -1; 
    }
    offset = FPOS(SYSZONE_DEF,secure_random);
    offset += index*8;
//    len = FSIZE(SYSZONE_DEF,secure_random);
    len = 8;
    if ( mode ) {
//        RNG_FillRandom((uchar *)&random, 8);
//        TRACE_BUF("write random", random, 8);
//        ret = s_sysconfig_write(offset, 8, random);
        ret = s_sysconfig_write(offset, 8, p);
    } else {
        ret = s_sysconfig_read(offset,len,p);
//        TRACE_BUF("read random", p, 8);
    }
    return ret;
}		/* -----  end of function s_sysinfo_rfidinfo  ----- */


//0-hsk  1234567890abcdef
//1-系统密码加密密钥
//2-系统恢复密钥
//3-download app requst shakehand key
uint PCI_GetHSK(uint mode, uchar *hsk)
{
	uint i;
	const uchar buf[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef };
	if(mode == 0)
	{
		memcpy(hsk,buf,8);
	}
	else if(mode == 1)
	{
		// 16byte
		for(i=0;i<sizeof(buf);i++)
		{
			hsk[i] = buf[i] ^ 0x01;
		}
	}
	else if(mode == 2)
	{
		for(i=0;i<sizeof(buf);i++)
		{
			hsk[i] = buf[i] ^ LBYTE(i);
		}
	}
    else if(mode == 3)
    {
		for(i=0;i<sizeof(buf);i++)
		{
			hsk[i] = buf[i] ^ 0x45;
		}
    }
	return OK;
}

int s_getinfo_from_syszone (uint module, uint *value)
{
    uint offset,len;
    uchar data[128];
    int ret = 0;
    switch(module)
    {
//    case MODULE_PRINT_TYPE:
//        offset = FPOS(module_type_t,printer);
//        break;
//    case MODULE_LCD_TYPE:
//        offset = FPOS(module_type_t,lcd);
//        break;
    case MODULE_MAGNETIC_TYPE:
        offset = FPOS(MODULE_MAGCARD_INFO,type);
        offset += FPOS(SYSZONE_DEF,tmagcardinfo);
        len = 1;
        s_sysconfig_read(offset,len,&data);
        TRACE("*****mag:%d\r\n",data[0]);
        if ( data[0] == 1) {
            *value = MODULE_MAGNETIC_IDTECH;
        }else if ( data[0] == 2 ) {
            *value = MODULE_MAGNETIC_MTK211;
        }else if ( data[0] == 3 ) {
            *value = MODULE_MAGNETIC_MAGTEK;
        }else if ( data[0] == 4 ) {
            *value = MODULE_MAGNETIC_HCM4003;
        }else{
            *value = MODULE_NOTEXIST;
            ret = -1;
        }
        return ret;
        break;
//    case MODULE_MODEM_TYPE:
//        offset = FPOS(module_type_t,modem);
//        break;
//    case MODULE_GPRS_TYPE:
//        offset = FPOS(module_type_t,gprs);
//        break;
//    case MODULE_CDMA_TYPE:
//        offset = FPOS(module_type_t,cdma);
//        break;
    case MODULE_BLUETOOTH_TYPE:
        offset = FPOS(MODULE_BLUET_INFO,bluetooth);
        offset += FPOS(SYSZONE_DEF,bluetoothinfo);
        len = 1;
        s_sysconfig_read(offset,len,&data);
        TRACE("**************bule:%d\r\n",data[0]);
        if ( data[0] == 1) {
            //BM77
            *value = MODULE_BLUETOOTH_BM77SPP;
        }else{
            *value = MODULE_NOTEXIST;
            ret = -1;
        }
        return ret;
//    case MODULE_WIFI_TYPE:
//        offset = FPOS(module_type_t,wifi);
//        break;
//    case MODULE_LAN_TYPE:
//        offset = FPOS(module_type_t,ethernet);
//        break;
    case MODULE_RF_TYPE:
    case MODULE_PN512_TYPE:
        offset = FPOS(MODULE_RFID_INFO,type);
        offset += FPOS(SYSZONE_DEF,trfidinfo);
        len = 1;
        s_sysconfig_read(offset,len,&data);
//        TRACE("**************rfid:%d\r\n",data[0]);
        if ( data[0] == 1) {
            //as3911
            *value = MODULE_RF_AS3911;
        }else if ( data[0] == 2) {
            *value = MODULE_RF_FM17550;
        }else{
            *value = MODULE_NOTEXIST;
            ret = -1;
        }
        return ret;
    case MODULE_COM1_TYPE:
        *value = MODULE_COM1;
        return 0;
    case MODULE_COM2_TYPE:
        *value  = MODULE_NOTEXIST;
        return 0;
//    case MODULE_INFR_TYPE:
//        offset = FPOS(module_type_t,infr);
//        break;
//    case MODULE_ZIGBEE_TYPE:
//        offset = FPOS(module_type_t,zigbee);
//        break;
//    case MODULE_BATTERY_TYPE:
//        offset = FPOS(module_type_t,battery);
//        break;
//    case MODULE_USBHOST_TYPE:
//        offset = FPOS(module_type_t,usbhost);
//        break;
//    case MODULE_NAND_TYPE:
//        offset = FPOS(module_type_t,nandflash);
//        break;
//    case MODULE_SCANNER_TYPE:
//        offset = FPOS(module_type_t,scanner);
//        break;
//    case MODULE_COM3_TYPE:
//        offset = FPOS(module_type_t,com3);
//        break;
//    case MODULE_COM4_TYPE:
//        offset = FPOS(module_type_t,com4);
//        break;
//    case MODULE_STANDBY_BAT_TYPE:
//        offset = FPOS(module_type_t,standbybat);
//        break;
    default:
        return -1;
    }
//    offset += FPOS(SYSZONE_DEF,trfidinfo);
//    s_sysconfig_read(offset,sizeof(uint),&data);
//    if ( H3BYTE(data) == 0xFF) {
//        return -1;
//    } else {
//        *value = data & 0x00FFFFFF;
//        return 0;
//    }
}		/* -----  end of function s_getinfo_from_syszone  ----- */

//int s_fac_readinfo (uint offset, uint len, void *outbuf)
//{
//    int ret;
//    if ( offset > sizeof(FAC_TESTINFO) ) {
//        return -1;
//    }
//    ret = s_read_syszone(offset+FPOS(SYSZONE_DEF,tTestInfo),len,(uchar *)outbuf);
//    return ret;
//}		/* -----  end of function fac_readinfo  ----- */

//主板ID,有效字节18:产品号(产品平台+产品号,2B)+00+工单号(6B)+时间(6B)+预留(3B)+CRC16(2B)
int s_fac_ctrl_get_MBID(uchar *id)
{
    uint offset,size;
    ushort crc16;
    offset = FPOS(FAC_TESTINFO,mbno);
    size = FSIZE(FAC_TESTINFO,mbno);
//    s_fac_readinfo(offset,size,id);
    s_read_syszone(offset+FPOS(SYSZONE_DEF,tTestInfo),size,(uchar *)id);
    crc16 = calc_crc16(id,(size-2),0);
    if ( !memcmp((uchar *)&crc16,&id[(size-2)],2)) {
        return 0;
    } else {
        return 1;
    }
}

int sys_get_sysinfo(uint type,uint *outlen,void *outbuf)
{
    int ret;
    uint offset=0;
    uchar buf[256];
    uint *p=(uint *)outbuf;
    CLRBUF(buf);
    if ( outlen == NULL || outbuf == NULL ) {
        return -2;
    }
    if ( (type >= MODULE_PRINT_TYPE && type <= MODULE_STANDBY_BAT_TYPE) ) {
        if ( !(type==MODULE_S980_TYPE
               ||type==MODULE_CARDREADER_TYPE
               ||type==MODULE_IOM_TYPE)) {
            // 硬件配置区
            ret = s_getinfo_from_syszone(type,(uint *)outbuf);
            *outlen = 4;
            return ret;
        }
    }
    switch ( type )
    {
    case MODULE_MODE_FONT:
        s_sysinfo_font_info(outlen,buf);
        break;
    case MODULE_MODE_RFID:
        *outlen = sizeof(MODULE_RFID_INFO);
        s_sysinfo_rfidinfo(0,buf);
        break;
    case MODULE_MODE_ICCARD:
        *outlen = sizeof(MODULE_ICCARD_INFO);
        s_sysinfo_iccardinfo(0,buf);
        ret = 0;
        if ( buf[0] == 1) {
            *p = MODULE_CPUCARD_IOREV;
        }else if ( buf[0] == 2 ) {
            *p = MODULE_CPUCARD_IO;
        }else if ( buf[0] == 3 ) {
            *p = MODULE_CPUCARD_8035;
        }else if ( buf[0] == 4 ) {
            *p = MODULE_CPUCARD_8113;
        }else if ( buf[0] == 5 ) {
            *p = MODULE_CPUCARD_6001;
        }else{
            *p = MODULE_NOTEXIST;
            ret = -1;
        }
        return ret;
        break;
    default :
        return -1;
    }
    memcpy(outbuf,&buf[offset],*outlen);
    return 0;
}

int sys_get_module_type(int module)
{
    uint len;
    uint data;
    uint ret;
    ret = sys_get_sysinfo((uint)module,&len,&data);
    if ( ret == 0 ) {
        return (int)data;
    } else {
        return MODULE_NOTEXIST;
    }
}
int sys_GetSysinfo(uint type, uint *outlen, void *outbuf)
{
    return sys_get_sysinfo(type,outlen,outbuf);
}
int sys_GetModuleType(int module)
{
    return sys_get_module_type(module);
}


int sys_read_ver(int module, char *ver)
{
    const VERSION_INFO_NEW *pVer;
    char tmp[32];
    // 0          1    2   3      4      5      6      7
    //{"PRODUCT","MB","ID","BOOT","CTRL","TIME","FONT","SN"};
    int i,ret=0;
    if(ver == NULL)
    {
        return ERROR;
    }
    switch(module)
    {
    case READ_PRODUCT_VER:
        sprintf(ver, "%s", k_TermName);
        break;
    case READ_MAINB_VER:
//        strcpy(ver,"1.0.0");
        sprintf(ver, "%d.%d.%d", k_MainVer.main, k_MainVer.secondary, k_MainVer.revise);
        break;
    case READ_MAINBID_VER:
        CLRBUF(tmp);
        ret = s_fac_ctrl_get_MBID((uchar *)tmp);
        //主板id只要传票号+时间12bytes
        memcpy(ver,tmp+3,12);
        for ( i = 0 ; i < 6 ; i++ ) {
            tmp[2*i] = ((ver[6+i]>>4)&0x0F)+0x30;
            tmp[2*i+1] = (ver[6+i]&0x0F)+0x30;
        }
        //copy时间
        memcpy(ver+6,tmp,12);
        break;
    case READ_BOOT_VER:
        if (localdl_get_boot_version(&pVer)) {
            return ERROR;
        }
        strcpy(ver,pVer->version);
//        TRACE("boot:%s\n",pVer->version);
        break;
    case READ_CTRL_VER:
        localdl_get_ctrl_version(&pVer);
        strcpy(ver,pVer->version);
        break;
    case READ_CTRLTIME_VER:
        localdl_get_ctrl_version(&pVer);
        strcpy(ver,pVer->time);
        break;
    case READ_FONT_VER:
        localdl_get_font_version(tmp);
        //cpoy点阵信息
        memcpy(ver,tmp,6);
        memcpy(ver+6,tmp+7,5);
        memcpy(ver+11,tmp+13,4);
        break;
    case READ_SN_VER:
//        ret = s_sysconfig_read(FPOS(SYSZONE_DEF,sn_dev),FSIZE(SYSZONE_DEF,sn_dev),ver);
        ret = s_localdl_get_sn(ver);
        break;
    default:
        return ERROR;
    }
    if ( ret != 0 || ver[0] == NULL) {
        memcpy(ver,STR_NULL,4);
        ver[4] = 0;
    }
	return OK;
}


/* 
 * sys_get_ctrllen - [GENERIC] 获取CTRL层代码区大小
 * @ 
 */
int sys_get_ctrllen (int mode)
{
    uint codelen;
    s_read_syszone(OFFSET_APP_CHECK+4,sizeof(codelen),(uchar *)&codelen);
    return codelen;
}		/* -----  end of function sys_get_applen  ----- */

