/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : pinpad_sys.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 3/30/2014 2:51:22 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"

#ifdef CFG_TAMPER
//typedef struct ST_MMAP_KEY_
//{
//	uchar aucMMK[MK_UNIT_LEN];   //16
//	uchar aucELRCK[ELRCK_LEN];   //8  ELRCK is MagKey
//	uchar check[CHECK_LEN];      //4 0-aucMMK+aucELRCK check 1-fsk check 2-AppFsk 3-CtrlFSK
//    uchar facchk[FUN_LEN];       //4
//} ST_MMAP_KEY;
//#define MMAP_KEY_LEN  (MK_UNIT_LEN+ELRCK_LEN)
//
//ST_MMAP_KEY gMKey;
//
//#define TAMPER_MASK             (0x3F0003U)
//#define TAMPER_PIN_MASK         (0x07U)
//#define TAMPER_PIN_SHIFT        (1U)
//
//#define SECURITY_LEVEL_NOTENABLE    (~((uint32_t)0))
//#define SECURITY_LEVEL_FIXKEY       ((uint32_t)0)
//#define SECURITY_LEVEL_HIGHST       TAMPER_MASK 
//

//void PCI_MakeKeyCheck(uchar *in,uint len,uchar *check)   
//{
//    cacul_mac_ecb(ELRCK_LEN,gMKey.aucELRCK,len,in,4,check);
//}

ST_MMAP_KEY gMKey;

static void _tamper_event_save(struct TamperEvent *event)
{
    uint8_t backbuf[SYSZONE_LEN];
    s_write_syszone(backbuf,SYS_TAMPER_EVENT_ADDR,SYS_TAMPER_EVENT_SIZE,(uint8_t *)event);
}

void sys_tamper_event_reset(void)
{
    struct TamperEvent event;
    memset(&event,0xFF,sizeof(struct TamperEvent));
    TRACE("\r\n-|Save tamper event:Reset");
    _tamper_event_save(&event);//恢复
}

void sys_tamper_event_record(void)
{
    struct TamperEvent event;
    uint8_t i;

    for (i=0; i<SYS_TAMPER_EVENT_SIZE/4; i++)
    {
        if (gpSys->tamper[i] != ~0U) {
            return;
        }
    }
    drv_dryice_get_reg(&event);
    TRACE("\r\n-|Save tamper event:record");
    _tamper_event_save(&event);
}

void sys_tamper_event_get(struct TamperEvent *event)
{
    memcpy(event,gpSys->tamper,SYS_TAMPER_EVENT_SIZE);
    if (event->time_s == (~(uint32_t)(0U)) 
        && event->tsr == (~(uint32_t)(0U))
        && event->sources == (~(uint32_t)(0U))
        && event->level == (~(uint32_t)(0U))) {
        TRACE("\n-|get reg");
        drv_dryice_get_reg(event);
    }
    TRACE("\n-|second:%02X sr:%02X src:%02X ppr:%02X",
          event->time_s,event->tsr,
          event->sources,event->level);
}

void MakeKeyCheck(uchar *in,uint len,uchar *check)   
{
    cacul_mac_ecb(ELRCK_LEN,gMKey.aucELRCK,len,in,4,check);
}

//int sys_tamper_init(uint32_t mode)
//{
//    ST_MMAP_KEY ramkey; 
//
//    TRACE("\n-|pinpad init:%02X",mode);
//    drv_dryice_init((hal_dry_tamper_t)DRY_TER_TPE(mode),
//                    (hal_dry_interrupt_t)0,
//                    0,ENABLE);
//    RNG_FillRandom((uint8_t *)&ramkey, MMAP_KEY_LEN);
//    MakeKeyCheck((uchar *)&ramkey,MMAP_KEY_LEN,ramkey.check);
//    memcpy(ramkey.facchk,STR_SRAM,4);
//    TRACE_BUF("dryice mmk",ramkey.aucMMK,32);
//    hw_dryice_write_key((FPOS(ST_MMAP_KEY,aucMMK)>>2),
//                        ((MK_UNIT_LEN+ELRCK_LEN+CHECK_LEN+FUN_LEN)>>2),
//                        (uint32_t *)&ramkey);
//    return 0;
//}

int sys_write_security_level(uint32_t levels)
{
    uint8_t backbuf[SYSZONE_LEN];
    if (gpSys->secu_leve !=  levels)
    {
        TRACE("\n-|Modify security level");
        return s_write_syszone(backbuf,OFFSET_SECURITY_LEVELS,LEN_SECURITY_LEVELS,(uint8_t *)&levels);
    } 
    return 0;
}

//0xFFFF FFFF:未设置
//0x0000 0000:固定密钥
//0x0000 000L:secruriy level
uint32_t security_level_get(void)
{
//    TRACE("\nSecuritylevel:0x%02X",gpSys->secu_leve);
    return gpSys->secu_leve;
}
/****************************************************************************** 
BIT31	BIT30	BIT29	BIT28	BIT27	BIT26	BIT25	BIT24
预留	预留	预留	预留	预留	预留	预留	预留
BIT23	BIT22	BIT21	BIT20	BIT19	BIT18	BIT17	BIT16
引脚7   引脚6	引脚5	引脚4	引脚3	引脚2	引脚1	引脚0
BIT15	BIT14	BIT13	BIT12	BIT11	BIT10	BIT9	BIT8
预留	预留	预留	预留	预留	预留	测试模式 Flash安全标志位
BIT7	BIT6	BIT5	BIT4	BIT3	        BIT2	        BIT1	BIT0
安全	温度	时钟	VBAT	单调计数器溢出	时间溢出标志位	预留	预留
 ******************************************************************************/
//void Tamper_Init(void);
int security_level_set(uint32_t levels)
{
    levels &= TAMPER_MASK;
    if ((levels) != 0)
    {
//        Tamper_Init();
        pinpad_open(levels);
    } else
    {
        levels = ~0;
    }
    if (0 == sys_write_security_level(levels))
    {
//        pinpad_init_kek();
//#ifdef CFG_CMD_V1	
//        return create_default_V1APP();
//#endif
    }
    return 0;
}

uint32_t security_level_if_safe(void)
{
    uint32_t levels;

    levels = security_level_get();
    if (levels == SECURITY_LEVEL_NOTENABLE)
    {
        levels = NO;
    } else if (levels == SECURITY_LEVEL_FIXKEY) 
    {
        levels = NO;
    } else if (levels <= SECURITY_LEVEL_HIGHST) 
    {
        levels = YES;
    } else {
        //todo:安全主允许状态
        levels = NO;
    }
    return levels;
}

//uint Fac_SRAM(uint mode)
//{
//#ifdef CFG_TAMPER 
////    if (security_level_if_safe() == YES) 
//    {
//        CLRBUF(gMKey.facchk);
//        hw_dryice_read_key((FPOS(ST_MMAP_KEY,facchk)>>2),1,(uint32_t *)&gMKey.facchk);
////        TRACE_BUF("fack",gMKey.facchk,4);
//        if(mode == 0)
//        {
//            if(memcmp(gMKey.facchk,STR_SRAM,FUN_LEN) == 0)
//            {
//                return 0;
//            }
//            else
//            {
//                return 1;
//            }
//        }
//        else
//        {
//            if(memcmp(gMKey.facchk,STR_SRAM,FUN_LEN) != 0)
//            {
//                mode >>= 1;
//                sys_tamper_init(mode);
//                hw_dryice_read_key((FPOS(ST_MMAP_KEY,facchk)>>2),1,(uint32_t *)&gMKey.facchk);
//                return (memcmp(gMKey.facchk,STR_SRAM,FUN_LEN) != 0);
//            }
//            return 0;
//        }
//    }
//#endif
//    return 0;
//}

void sys_TamperPinStatus(uchar *pucData,ushort *pusLen)
{
#if (defined CFG_TAMPER)
    if (pucData[0] == 0)
    {
        if (security_level_if_safe() == YES) {
            pucData[0] = hw_dryice_read_trigger();//tamper pin trigger
            pucData[1] = hw_dryice_read_pin();//tamper pin level
        } else {
            pucData[0] = 0xFF;//tamper pin normal status mask:0000 0111
            pucData[1] = 0x0F;
        }
        *pusLen = 2;
        TRACE("\n-|Tamper PIN:%02X %02X",pucData[0],pucData[1]);
    } else if (pucData[0] == 1)
    {
        if (security_level_if_safe() == YES) {
            sys_tamper_event_get((struct TamperEvent *)pucData);
            *pusLen = sizeof(struct TamperEvent);
        } else {
            pucData[0] = 0xEE;//tamper pin normal status mask:0000 0111
            pucData[1] = 0x0F;
            *pusLen = 2;
        }
    }
#else
        pucData[0] = 0xEE;//tamper pin normal status mask:0000 0111
        pucData[1] = 0x00;
        *pusLen = 2;
#endif
}

int ksr_read_elk(uint8_t *length,uint8_t *elk)
{
#ifdef CFG_TAMPER
    uint8_t buffer[ELRCK_LEN];
    if (hw_dryice_read_key((FPOS(ST_MMAP_KEY,aucELRCK)>>2),ELRCK_LEN/4,(uint32_t*)buffer)) {
        TRACE("\nRead elk error");
        return -1;
    }
    memcpy(elk,buffer,ELRCK_LEN);
    *length = ELRCK_LEN;
#else
    uint8_t elkey[ELRCK_LEN] = "elkelk8";

    memcpy(elk,elkey,ELRCK_LEN);
    *length = ELRCK_LEN;
#endif
    return 0;
}

int ksr_read_mmap_key(void)
{
#ifdef CFG_TAMPER
    if (security_level_if_safe() == NO) {
        return 0;
    }
    if (hw_dryice_read_key((FPOS(ST_MMAP_KEY,aucMMK)>>2),(MMAP_KEY_LEN+CHECK_LEN)/4,(uint32_t*)gMKey.aucMMK)) {
        TRACE("\nRead elk error");
        return -1;
    }
#endif
    return 0;
}

//void sys_TamperInit(void)
//{
////    uint8_t length;
//    ST_MMAP_KEY ramkey;
//
//    drv_dryice_init((hal_dry_tamper_t)(DRY_TER_TPE(0x3F)|kDryTamper_Voltage|kDryTamper_Clock),
//                    (hal_dry_interrupt_t)0,
//                    0x2C,ENABLE);
//////    sys_tamper_open(mode);
////    ksr_read_mmk(&length,gMKey.aucMMK);
////    DISPBUF("MMK",32,0,gMKey.aucMMK);
////    ksr_read_elk(&length,gMKey.aucELRCK);
////    RNG_FillRandom(tmp,MK_UNIT_LEN);
////    PCI_WriteKeyToFlash(0,KEY_TYPE_MAGK,0,MK_UNIT_LEN,tmp);
////    RNG_FillRandom(tmp,MK_UNIT_LEN);
////    PCI_WriteKeyToFlash(0,KEY_TYPE_MCK,0,MK_UNIT_LEN,tmp);
//////    DISPBUF("MMK",32,0,gMKey.aucMMK);
//
//    RNG_FillRandom((uint8_t *)&ramkey, MMAP_KEY_LEN);
//    MakeKeyCheck((uchar *)&ramkey,MMAP_KEY_LEN,ramkey.check);
//    memcpy(ramkey.facchk,STR_SRAM,4);
//    TRACE_BUF("dryice mmk",ramkey.aucMMK,32);
//    hw_dryice_write_key((FPOS(ST_MMAP_KEY,aucMMK)>>2),
//                        ((MK_UNIT_LEN+ELRCK_LEN+CHECK_LEN+FUN_LEN)>>2),
//                        (uint32_t *)&ramkey);
//    return ;
//}

uint CheckPbulicKey(void)
{
#ifdef CFG_TAMPER
    uchar check[CHECK_LEN];

    if (security_level_if_safe() == NO) {
        return 0;
    }
    if (ksr_read_mmap_key())
    {
        sys_tamper_event_record();
        TRACE("\n-|MMK ERR");
        return 1;
    }
    MakeKeyCheck(gMKey.aucMMK,MMAP_KEY_LEN,check);
    if(memcmp(gMKey.check,check,CHECK_LEN))
    {
        TRACE("\n-|MMK CHK ERR");
        sys_tamper_event_record();
        return 1;
    }
    else
    {
        return 0;
    }
#else
    return 0;
#endif
}

uint Fac_SRAM(uint mode)
{
#if  defined(CFG_TAMPER)
    ST_MMAP_KEY mmk;
    CLRBUF(mmk.facchk);
    hw_dryice_read_key((FPOS(ST_MMAP_KEY,facchk)>>2),1,(uint32_t *)&mmk.facchk);
    TRACE_BUF("fack",mmk.facchk,4);
    if(mode == 0)
    {
        if(memcmp(mmk.facchk,STR_SRAM,FUN_LEN) == 0)
        {
            //正常
            return 0;
        }
        else
        {
            //防拆异常
            return 1;
        }
    }
    else
    {
        if(memcmp(mmk.facchk,STR_SRAM,FUN_LEN) != 0)
        {
            sys_tamper_open(mode);
            RNG_FillRandom((uint8_t *)&mmk, MMAP_KEY_LEN);
            memcpy(mmk.facchk,STR_SRAM,FUN_LEN);
            hw_dryice_write_key((FPOS(ST_MMAP_KEY,aucMMK)>>2),
                                ((MK_UNIT_LEN+ELRCK_LEN+CHECK_LEN+FUN_LEN)>>2),
                                (uint32_t *)&mmk);
            hw_dryice_read_key((FPOS(ST_MMAP_KEY,facchk)>>2),1,(uint32_t *)&mmk.facchk);
            return (memcmp(mmk.facchk,STR_SRAM,FUN_LEN) != 0);
        }
        return 0;
    }
#else
    return 0;
#endif
}



#endif

//#if defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
#if 0 


void keyextend(uint keylen, uchar *inbuf, uchar *outbuf)
{
    memcpy(outbuf,inbuf,keylen);
    if(keylen == 8)
    {
        memcpy(outbuf+8,inbuf,8);
        memcpy(outbuf+8+8,inbuf,8);
    }
    else if(keylen == 16)
    {
        memcpy(outbuf+16,inbuf,8);
    }
}
int app_get_current_key(uint8_t appno)
{
    uint8_t length;
    uint8_t i;
    uchar key[KEY_UNIT_LEN];
    if (PCI_ReadKeyFromFlash(appno,KEY_TYPE_APPEK,0,&length,key) < 0)
    {
        TRACE("appek err");
        CLRBUF(key);
        return -1;
    }
    for (i=0; i<NUM_PINK; i++)
    {
        if (PCI_ReadKeyFromFlash(appno,KEY_TYPE_PINK,i,&length,key) < 0) {
            TRACE(" pink err");
            CLRBUF(key);
            return -1;
        }
    }
    for (i=0; i<NUM_MACK; i++)
    {
        if (PCI_ReadKeyFromFlash(appno,KEY_TYPE_MACK,i,&length,key) < 0) {
            TRACE("mack err");
            CLRBUF(key);
            return -1;
        }
    }
    for (i=0; i<NUM_DESK; i++)
    {
        if (PCI_ReadKeyFromFlash(appno,KEY_TYPE_DESK,i,&length,key) < 0) {
            TRACE("des err");
            CLRBUF(key);
            return -1;
        }
    }
    CLRBUF(key);
    return 0;
}




#ifdef CFG_KEYCHECK_MAC
void PCI_MakeKeyCheck(uchar *in,uint len,uchar *check)   
{
    cacul_mac_ecb(ELRCK_LEN,gMKey.aucELRCK,len,in,4,check);
}	
#endif
#ifdef CFG_KEYCHECK_LRC
void PCI_MakeKeyCheck(uchar *in,uint len,uchar *check)   
{
#if 1
    uint *p,i,start;
    p = (uint *)in;
    start = 0xaa55aa55;
    for(i=0;i<len/sizeof(uint);i++)
    {
        start ^= p[i];
    }
    memcpy(check,(uchar *)&start,sizeof(uint));
#else	
    check[0] = MakeLrc(in,(ushort)len);
#endif	
}	
#endif


// mode=0  初始化指针
//     =1  清空单个应用密钥区
//     =2  清空公共密钥区+全部应用密钥区
//     =3  初始公共密钥区和应用密钥区
void PCI_InitPublicKey(uint mode)
{	
    uint i;
#ifndef CFG_TAMPER
    uint8_t tmp[MK_UNIT_LEN];
#endif
    uint8_t length;

    if(mode == 1)
    {		
        PCI_ClearAppKey(gtCurKey.appid);
        PCI_ClearPubKey(gtCurKey.appid);
    }
    else if(mode == 2)
    {
        for (i=1; i<= NUM_APP; i++) {
            PCI_ClearAppKey(i);
            PCI_ClearPubKey(i);
        }
        //add clear MMK
    }
    else if(mode == 3)
    {
#ifdef CFG_TAMPER
        pinpad_open(mode);
#else
        memset(gMKey.aucMMK,0xAA,sizeof(gMKey.aucMMK));
        memset(gMKey.aucELRCK,0xBB,sizeof(gMKey.aucELRCK));
        PCI_MakeKeyCheck((uchar *)&gMKey,MMAP_KEY_LEN,gMKey.check);
        memset(tmp,0xCC,MK_UNIT_LEN);
        PCI_WriteKeyToFlash(0,KEY_TYPE_MAGK,0,MK_UNIT_LEN,tmp);
        memset(tmp,0xDD,MK_UNIT_LEN);
        PCI_WriteKeyToFlash(0,KEY_TYPE_MCK,0,MK_UNIT_LEN,tmp);
        memcpy(gMKey.facchk,STR_SRAM,4);
#endif
    }
    ksr_read_mmk(&length,gMKey.aucMMK);
    ksr_read_elk(&length,gMKey.aucELRCK);
    DISPBUF("PUBK",sizeof(gMKey),0,&gMKey);
}


void PCI_DisKey(uint8_t mode,uint8_t appid,uint8_t type,uint8_t index,uint8_t len,uint8_t *data)
{
#ifdef DEBUG_Dx 
    char title[64];
    CLRBUF(title);
    switch (type)
    {
    case KEY_TYPE_AUTHMK:	  
        sprintf(title,"AUMK:%d",index);
        break;
    case KEY_TYPE_AUTHPINK: 
        sprintf(title,"AUPINK:%d",index);
        break;
    case KEY_TYPE_AUTHMACK: 
        sprintf(title,"AUMACK:%d",index);
        break;
    case KEY_TYPE_AUTHDESK:
        sprintf(title,"AUDESK:%d",index);
        break;
    case KEY_TYPE_MK:		  
        sprintf(title,"MK:%d",index);
        break;
    case KEY_TYPE_PINK:   
        sprintf(title,"PINK:%d",index);
        break;
    case KEY_TYPE_MACK:   
        sprintf(title,"MACK:%d",index);
        break;
    case KEY_TYPE_DESK:   
        sprintf(title,"DESK:%d",index);
        break;
    case KEY_TYPE_APPAK:    
        strcat(title,"APPAK");
        break;
    case KEY_TYPE_APPEK:   
        strcat(title,"APPEK");
        break;
    case KEY_TYPE_TRSK:     
        strcat(title,"TRSK");
        break;
    case KEY_TYPE_APPFLG  :
        strcat(title,"APPFLAG");
        break;
    case KEY_TYPE_APPNAME :
        strcat(title,"APPNAME");
        break;
    case KEY_TYPE_APPPSWA :
        strcat(title,"APPPSWA");
        break;
    case KEY_TYPE_APPPSWB :
        strcat(title,"APPPSWB");
        break;
    case KEY_TYPE_APPSN   :
        sprintf(title,"APPSN:%d",index);
        break;
    case KEY_TYPE_FSKDEV  :
        sprintf(title,"FSKDEV:%d",index);
        break;
    case KEY_TYPE_FSKHST_BOOT  :
        sprintf(title,"FSKHST_BOOT:%d",index);
        break;
    case KEY_TYPE_FSKHST_CTRL  :
        sprintf(title,"FSKHST_CTRL:%d",index);
        break;
    case KEY_TYPE_FSKHST_APP  :
        sprintf(title,"FSKHST_APP:%d",index);
        break;
    case KEY_TYPE_FSKAPPAK:
        sprintf(title,"FSKAPP:%d",index);
        break;
    default:
        TRACE("\n keytype not define");
        break;
    }
    if (mode == 0) //read
//        TRACE("\n-|READ| appid:%d",appid);
        sprintf(&title[strlen(title)]," read app:%d",appid);
    else
        sprintf(&title[strlen(title)]," write app:%d",appid);
//        TRACE("\n-|WRITE| appid:%d",appid);
    TRACE_BUF(title,data,len);
#endif
}



uint PCI_WriteKeyToFlash(uchar appid,uchar type,uchar index, uchar len, uchar *inkey)
{
    struct KEY_DEF ksr;
    int iRet;
    uchar key[24];
    if (appid == 0 || appid > NUM_APP) {
        TRACE("\n-|appid Error:%d",appid);
        return -1;
    }
    PCI_DisKey(1,appid,type,index,len,key);
    /*-----------------------------------------------------------------------------}
     *  MK,WK,认证密钥,传输密钥统一扩展成24B
     *-----------------------------------------------------------------------------{*/
    if ( type >= KEY_TYPE_AUTHMK && type <= KEY_TYPE_TRSK) {
        keyextend(len,inkey,key);
        len = 24;
    } else {
        memcpy(key,inkey,len);
    }
    //key expand
    switch (type)
    {
    case KEY_TYPE_AUTHMK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_AUTHMK_NUM;
        iRet = ksr_write_msk(&ksr,len,key);
        break;
    case KEY_TYPE_AUTHPINK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_AUTHPINK_NUM;
        iRet = ksr_write_msk(&ksr,len,key);
        break;
    case KEY_TYPE_AUTHMACK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_AUTHMACK_NUM;
        iRet = ksr_write_msk(&ksr,len,key);
        break;
    case KEY_TYPE_AUTHDESK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_AUTHDESK_NUM;
        iRet = ksr_write_msk(&ksr,len,key);
        break;
    case KEY_TYPE_MK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_MSK_NUM;
        iRet = ksr_write_msk(&ksr,len,key);
        break;
    case KEY_TYPE_PINK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_PINK_NUM;
        iRet = ksr_write_msk(&ksr,len,key);
        break;
    case KEY_TYPE_MACK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_MACK_NUM;
        iRet = ksr_write_msk(&ksr,len,key);
        break;
    case KEY_TYPE_DESK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_DESK_NUM;
        iRet = ksr_write_msk(&ksr,len,key);
        break;
    case KEY_TYPE_APPFLG:
        iRet = ksr_write_kek(CURRENT_APPFLAG,0,len,key);
        break;
    case KEY_TYPE_APPNAME:
        iRet = ksr_write_kek(CURRENT_APPNAME,0,len,key);
        break;
    case KEY_TYPE_APPPSWA:
        iRet = ksr_write_kek(CURRENT_APPPSWA,0,len,key);
        break;
    case KEY_TYPE_APPPSWB:
        iRet = ksr_write_kek(CURRENT_APPPSWB,0,len,key);
        break;
    case KEY_TYPE_APPSN:
        iRet = ksr_write_kek(CURRENT_APPSN,0,len,key);
        break;
    case KEY_TYPE_APPAK: //encrypt key is mmk
        iRet = ksr_write_kek(CURRENT_APPAK,0,len,key);
        break;
    case KEY_TYPE_APPEK: //encrypt key is mmk
        iRet = ksr_write_kek(CURRENT_APPEK,0,len,key);
        break;
    case KEY_TYPE_TRSK:
        iRet = ksr_write_kek(CURRENT_TRSK,0,len,key);
        break;
    case KEY_TYPE_FSKHST_APP:
        iRet = ksr_write_kek(CURRENT_APPFSK,0,len,key);
        break;
    case KEY_TYPE_MAGK:
        iRet = ksr_write_kek(CURRENT_MAGK,0,len,key);
        break;
    case KEY_TYPE_MCK:
        iRet = ksr_write_kek(CURRENT_MCK,0,len,key);
        break;
    default:
        TRACE("\n-|key type error");
        iRet = -1;
        break;
    }
    return iRet;
}


// index:from 0
int PCI_ReadKeyFromFlash(uchar appid,uchar type,uchar index, uchar *len, uchar *outkey)
{
    struct KEY_DEF ksr;
    int iRet;
    if (appid == 0 || appid > NUM_APP) {
        TRACE("\n-|appid Error:%d",appid);
        return -1;
    }
    //key expand
    switch (type)
    {
    case KEY_TYPE_AUTHMK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_AUTHMK_NUM;
        iRet = ksr_read_msk(&ksr,len,outkey);
        break;
    case KEY_TYPE_AUTHPINK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_AUTHPINK_NUM;
        iRet = ksr_read_msk(&ksr,len,outkey);
        break;
    case KEY_TYPE_AUTHMACK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_AUTHMACK_NUM;
        iRet = ksr_read_msk(&ksr,len,outkey);
        break;
    case KEY_TYPE_AUTHDESK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_AUTHDESK_NUM;
        iRet = ksr_read_msk(&ksr,len,outkey);
        break;
    case KEY_TYPE_MK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_MSK_NUM;
        iRet = ksr_read_msk(&ksr,len,outkey);
        break;
    case KEY_TYPE_PINK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_PINK_NUM;
        iRet = ksr_read_msk(&ksr,len,outkey);
        break;
    case KEY_TYPE_MACK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_MACK_NUM;
        iRet = ksr_read_msk(&ksr,len,outkey);
        break;
    case KEY_TYPE_DESK:
        ksr.kekno = CURRENT_APPEK;
        ksr.mno = CURRENT_DESK_NUM;
        iRet = ksr_read_msk(&ksr,len,outkey);
        break;
    case KEY_TYPE_APPFLG:
        iRet = ksr_read_kek(CURRENT_APPFLAG,0,len,outkey);
        break;
    case KEY_TYPE_APPNAME:
        iRet = ksr_read_kek(CURRENT_APPNAME,0,len,outkey);
        break;
    case KEY_TYPE_APPPSWA:
        iRet = ksr_read_kek(CURRENT_APPPSWA,0,len,outkey);
        break;
    case KEY_TYPE_APPPSWB:
        iRet = ksr_read_kek(CURRENT_APPPSWB,0,len,outkey);
        break;
    case KEY_TYPE_APPSN:
        iRet = ksr_read_kek(CURRENT_APPSN,0,len,outkey);
        break;
    case KEY_TYPE_APPAK: //encrypt key is mmk
        iRet = ksr_read_kek(CURRENT_APPAK,0,len,outkey);
        break;
    case KEY_TYPE_APPEK: //encrypt key is mmk
        iRet = ksr_read_kek(CURRENT_APPEK,0,len,outkey);
        break;
    case KEY_TYPE_TRSK:
        iRet = ksr_read_kek(CURRENT_TRSK,0,len,outkey);
        break;
    case KEY_TYPE_FSKHST_APP:
        iRet = ksr_read_kek(CURRENT_APPFSK,0,len,outkey);
        break;
    case KEY_TYPE_MAGK:
        iRet = ksr_read_kek(CURRENT_MAGK,0,len,outkey);
        break;
    case KEY_TYPE_MCK:
        iRet = ksr_read_kek(CURRENT_MCK,0,len,outkey);
        break;
    default:
        TRACE("\n-|key type error");
        iRet = -1;
        break;
    }
    if (iRet == 0)
        PCI_DisKey(0,appid,type,index,*len,outkey);
    return iRet;
}

uint PCI_ReadCurrentKey(uchar type,uchar index, uchar *len, uchar *outkey)
{
    return PCI_ReadKeyFromFlash(gtCurKey.appid,type,index,len,outkey);
}

uint PCI_WriteCurrentKey(uchar type,uchar index, uchar len, uchar *outkey)
{
    return PCI_WriteKeyToFlash(gtCurKey.appid,type,index,len,outkey);
}


int PCI_ClearAppKey(uint8_t appno)
{
    return ksr_clear_sector(FPOS(struct APP_ZONE,app[appno-1].auth_mk[0]),
                            FPOS(struct APP_ZONE,app[appno-1].wsk_back));
}

int PCI_ClearPubKey(uint8_t appno)
{
    return ksr_clear_sector(FPOS(struct APP_ZONE,app[appno-1].pub),
                            FPOS(struct APP_ZONE,app[appno-1].pub));
}


uint PCI_ReadFSK(uchar type,uchar *fsk)
{
    uint ret=0;
    switch(type)
    {
    case FSK_SMAPP:
        //			return sys_MMKDecryptRead(OFFSET_FSK_SM,FSK_LEN,fsk);
    case FSK_CMCTRL:
        //			return sys_MMKDecryptRead(OFFSET_FSK_CTRL,FSK_LEN,fsk);
    default:
        app_set_APPEK(type,1);
        //			ret = sys_APPEKDecryptRead(OFFSET_FSK_APP_ZONE+((uint)type-FSK_CMAPP)*LEN_FSK_APP_ZONE,FSK_LEN,fsk);
        app_set_APPEK(type,0);
        return ret;
    }
}

uint PCI_SaveFSK(uchar type,uchar *fsk)
{
    if(type == FSK_CMCTRL)
    {
        //		return sys_MMKEncryptWrite(OFFSET_FSK_CTRL,FSK_LEN,fsk);
    }
    else if(type == FSK_CMAPP)
    {
        return PCI_WriteCurrentKey(KEY_TYPE_FSKHST_APP,0,16,fsk);
    }
    else if(type == FSK_APPAK)
    {
        return PCI_WriteCurrentKey(KEY_TYPE_APPAK,0,16,fsk);
    }
    return 0;
}


uint PKS_CheckPbulicKey(void)
{
#ifdef CFG_TAMPER
    uchar check[CHECK_LEN];

    if (ksr_read_mmap_key())
    {
        TRACE("\n-|MMK ERR");
        return 1;
    }
    PCI_MakeKeyCheck(gMKey.aucMMK,MMAP_KEY_LEN,check);
    if(memcmp(gMKey.check,check,CHECK_LEN))
    {
        TRACE("\n-|MMK CHK ERR");
        return 1;
    }
    else
    {
        return 0;
    }
#else
    return 0;
#endif
}
uint PKS_CheckWorkKey(void)
{
    return app_get_current_key(gtCurKey.appid);
}

uint PCI_ReadAuthErrTimes(uint appno)
{
    return 0;
}
// times=0 清0
// times=1 自加1
uint PCI_WriteAuthErrTimes(uint appno,uint times)
{
    return 0;	
}


uint PCI_ReadMCKey(uchar type,uchar index, uchar *len, uchar *outkey)
{
#if PRODUCT_TYPE == PRODUCT_TYPE_F12	
    return PCI_ReadKeyFromFlash(0,KEY_TYPE_MCK,0,len,outkey);
#else
    return 0;
#endif	
}
#endif







