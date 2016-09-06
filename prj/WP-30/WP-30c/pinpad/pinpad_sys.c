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
//#if defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
#if 0

/*-----------------------------------------------------------------------------}
 * 密钥空间分配:1个密钥区占用12K,支持6个应用
 * 基地址:密钥区-12K*6
 *-----------------------------------------------------------------------------{*/
const struct APP_ZONE * const gpApp = (struct APP_ZONE *)(SA_FILESYS-sizeof(struct APP_ZONE));
void ksr_disp_key(const struct KEY_BLK *pkey)
{
//    TRACE_BUF(NULL,(uint8_t *)pkey,KEY_BLK_LEN);
//    DISPBUF(NULL,KEY_BLK_LEN,0,(uint8_t *)pkey);
}


int key_read_flash(struct KEY_BLK *key_blk,uint16_t keynum)
{
#ifdef DEBUG_Dx
    //char title[64];
#endif
    memcpy(key_blk,(uint32_t *)KEY_ADDR(keynum),KEY_BLK_LEN);
//    TRACE("\n-|flash read key%d ciphertext:",key_num);
//    ksr_disp_key(key_blk);
#ifdef DEBUG_Dx
//    CLRBUF(title);
//    sprintf(title,"BLK密文:0x%03X-%02d--",keynum,keynum%64);
//    if ( keynum < FPOS(_KEY_APP_,auth_mk) ) {
//        strcat(title,"PUBK");
//    } else if( keynum < FPOS(_KEY_APP_,msk)){
//        strcat(title,"AUTK_MK");
//    } else if( keynum < FPOS(_KEY_APP_,auth_wk)){
//        strcat(title,"MK");
//    } else if( keynum < FPOS(_KEY_APP_,wsk)){
//        strcat(title,"AUTH_WK");
//    } else if( keynum < FPOS(_KEY_APP_,wsk_back)){
//        strcat(title,"WK");
//    } else {
//        strcat(title,"WKBack");
//    }
//    DISPBUF(title,KEY_BLK_LEN,0,(uint8_t *)key_blk);
#endif
    return 0;
}




inline int key_wirte_flash(struct KEY_BLK *key_blk,uint16_t key_num)
{
//    struct KEY_BLK buffer[FLASH_SECTOR_SIZE/KEY_BLK_LEN];
//    uint32_t page_addr = KEY_ZONE_ADDR;
//    int iRet=0;
//
////    TRACE("\n-|flash write ciphertext:%d",key_num);
////    ksr_disp_key(key_blk);
//    page_addr = KEY_PAGE_ADDR(key_num);
//    TRACE("\n page addr:%08X",page_addr);
//    TRACE(" page index:%02d",KYE_PAGE_INDX(key_num));
//    memcpy(buffer,(uint8_t *)page_addr,sizeof(buffer));
////    TRACE_BUF("prev blk",(uint8_t *)buffer,FLASH_SECTOR_SIZE);
//    iRet = drv_flash_SectorErase(page_addr);
//    if (iRet) {
//        TRACE("\n-|key write: flash erase ERROR!");
//        return -1;
//    }
//    memcpy(buffer+KYE_PAGE_INDX(key_num),key_blk,KEY_BLK_LEN);
////    TRACE_BUF("now blk",(uint8_t *)buffer,FLASH_SECTOR_SIZE);
//    iRet = drv_flash_write(page_addr,(uint32_t *)buffer,FLASH_SECTOR_SIZE);
//    if (iRet) {
//        TRACE("\n-|key write: flash write ERROR!");
//        return -1;
//    }
//    return 0
//    TRACE("\r\n[****write flash:%d-%x****]",key_num,KEY_ADDR(key_num));
    exit_lowerpower_freq();
    return flash_write(KEY_ADDR(key_num),KEY_BLK_LEN,(uint8_t*)key_blk);
}

void key_lrc(uint32_t length,uint8_t *input,uint8_t *output)
{
	uint32_t *p,i,start;

	p = (uint32_t *)input;
	start = 0xaa55aa55;
    length /= sizeof(uint32_t);
	for(i=0;i<length;i++)
	{
		start ^= p[i];
	}
	memcpy(output,(uchar *)&start,sizeof(uint32_t));
}



//key stystem respository:rtk(root key)
int ksr_read_mmk(uint8_t *length,uint8_t *rtk)
{
#ifdef CFG_TAMPER
    uint8_t buffer[MK_UNIT_LEN];
    if (hw_dryice_read_key((FPOS(ST_MMAP_KEY,aucMMK)>>2),MK_UNIT_LEN/4,(uint32_t*)buffer)) {
        TRACE("\nRead mmk error");
        return -1;
    }
    memcpy(rtk,buffer,MK_UNIT_LEN);
    *length = MK_UNIT_LEN;
    TRACE("\n-|MMK:");
    vDispBuf(*length,0,rtk); 
#else
    uint8_t rootkey[] = "1234567890abcdeffedcba54";

    memcpy(rtk,rootkey,24);
    *length = 24;
#endif
//    TRACE("\n-|RTK plaintex:");
//    vDispBuf(*length,0,rtk);
    return 0;
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
    if (hw_dryice_read_key((FPOS(ST_MMAP_KEY,aucMMK)>>2),(MMAP_KEY_LEN+CHECK_LEN)/4,(uint32_t*)gMKey.aucMMK)) {
        TRACE("\nRead elk error");
        return -1;
    }
#endif
    return 0;
}


int ksr_write_rtk(uint8_t length,uint8_t *rtk)
{

    return 0;
}

void key_check(uint8_t mode,uint16_t chkno,uint32_t length,uint8_t *input,uint8_t *output)
{
    if (mode == KEY_CHK_LRC) {
        key_lrc(length,input,output);
    } else {
//        cacul_mac_ecb();
    }
}

void key_check_root(uint32_t length,uint8_t *input,uint8_t *output)
{
    uint8_t rootkey[24];
    uint8_t klength;

    ksr_read_elk(&klength,rootkey);
    cacul_mac_ecb(klength,rootkey,length,input,4,output);
}

/*-----------------------------------------------------------------------------}
 *  判断blk是否为空
 *  返回: 1-为空 0-不为空
 *-----------------------------------------------------------------------------{*/
int blk_if_blank(struct KEY_BLK *p)
{
    uint i;
    uint *q = (uint*)p;
    for ( i=0 ; i<sizeof(struct KEY_BLK)/sizeof(uint) ; i++  ) {
        if ( *q++ != 0xFFFFFFFF ) {
            return 0;
        }
    }
    return 1; 
}
//key encrypt key
// 应用信息,APPAK,PTK,MAGK,MCK用MMK加密
int ksr_read_kek(uint16_t kekno,uint32_t chkno,uint8_t *length, uint8_t *output)
{
    struct KEY_BLK key_blk;
    uint8_t check[KEY_CHK_LEN];
    uint8_t key_b[KEY_MAX_LEN];
    uint8_t key_len;
    int ret;
    //frist read RTK
    key_read_flash(&key_blk,kekno);
    if ( blk_if_blank(&key_blk) ) {
        TRACE("\nkek NULL");
        ret = KEY_STATUS_NULL;
        goto ksr_read_kek_end;
    }
    ksr_read_mmk(&key_len,key_b);
    if (key_len == 24)
        des3_decrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
    else
        des2_decrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
#ifdef DEBUG_Dx
//    TRACE("\r\n ----%s kekno:%x chkno:%x",__FUNCTION__,kekno,chkno);
//    DISPBUF("密文",KEY_BLK_LEN,0,&key_blk);
//    DISPBUF("mmk",key_len,0,&key_b);
//    DISPBUF("明文",KEY_BLK_LEN,0,&key_blk);
#endif
    //make check
//    if(key_blk.length % DES_BLOCK_LENGTH) {
    if (key_blk.valid == KEY_STATUS_NULL) {
        TRACE("\nkek NULL");
        ret = KEY_STATUS_NULL;
        goto ksr_read_kek_end;
    }
    if(key_blk.length > 24) {
        TRACE("\nkek key lengh :%d error\n",key_blk.length);
        return -1;
    }
    key_check_root(KEY_MAX_LEN,key_blk.key,check);
    if (memcmp(check,key_blk.chk,KEY_CHK_LEN)){
        TRACE("\ncheck err");
        ret = -1;
        goto ksr_read_kek_end;
    }
    *length = key_blk.length;
    memcpy(output,key_blk.key,key_blk.length);
    ret = 0;
ksr_read_kek_end:
    if ( ret ) {
        *length = 0;
    }
    CLRBUF(key_b);
    memset((uchar *)&key_blk,0,sizeof(struct KEY_BLK));
    return ret;
}
// 用MMK加密保存
int ksr_write_kek(uint16_t kekno,uint32_t chkno,uint8_t length, uint8_t *input)
{
    struct KEY_BLK key_blk;
    uint8_t key_b[KEY_MAX_LEN];
    uint8_t key_len;

    //frist read RTK and decrypt
//    if(length % DES_BLOCK_LENGTH) {
    if(length > 24) {
        TRACE("\nlength error");
        return -1;
    }
    //        4B Head                        + 24B Data + 4B Check
    // 写格式:AA+00(RFU)+len+00(LRC)/01(MAC) + 24B Data + 4B Check
    memset(&key_blk,0x00,KEY_BLK_LEN);
    key_blk.valid = KEY_STATUS_VALID;
    key_blk.chk_mod = KEY_CHK_MOD;
    key_blk.length = length;
    memcpy(key_blk.key,input,length);
    //make check
    key_check_root(KEY_MAX_LEN,key_blk.key,key_blk.chk);
//    TRACE("\n-|KEK %d plaintex",kekno);
//    ksr_disp_key(&key_blk);
    //then encrypt
    memset(key_b,0x00,sizeof(key_b));
    ksr_read_mmk(&key_len,key_b);
#ifdef DEBUG_Dx 
#endif
//    vDispBufTitle("writekek明文",KEY_BLK_LEN,0,&key_blk);
    if (key_len == 24)
        des3_encrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
    else
        des2_encrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
//    vDispBufTitle("writekek密文",KEY_BLK_LEN,0,&key_blk);
    key_wirte_flash(&key_blk,kekno);
    
    CLRBUF(key_b);
    memset((uchar *)&key_blk,0,sizeof(struct KEY_BLK));
    return 0;
}

//mastkey key
// MK,WK,AuthMK读取用APPEK加密
// 输入参数: msk.kekno=APPEK msk.mno=index
int ksr_read_msk(struct KEY_DEF *msk,uint8_t *length, uint8_t *output)
{
    struct KEY_BLK key_blk;
    uint8_t check[KEY_CHK_LEN];
    uint8_t key_b[KEY_MAX_LEN];
    uint8_t key_len;
    int ret;
    //frist read APPEK and decrypt
    key_read_flash(&key_blk,msk->mno);
    if ( blk_if_blank(&key_blk) ) {
        TRACE("\nmsk NULL");
        ret = KEY_STATUS_NULL;
        goto ksr_read_msk_end;
    }
    //then decrypt 
    if(ksr_read_kek(msk->kekno,msk->chkno,&key_len,key_b))
    {
        TRACE("\nread kek err");
        ret = -1;
        goto ksr_read_msk_end;
    }
    if (key_len == 24)
        des3_decrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
    else
        des2_decrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
#if 0//def DEBUG_Dx
    TRACE("\r\n ----%s kekno:%x mno:%x",__FUNCTION__,msk->kekno,msk->mno);
    DISPBUF("密文",KEY_BLK_LEN,0,&key_blk);
    DISPBUF("APPEK",key_len,0,&key_b);
    DISPBUF("明文",KEY_BLK_LEN,0,&key_blk);
#endif
    //make check
//    if(key_blk.length % DES_BLOCK_LENGTH) {
    if (key_blk.valid == KEY_STATUS_NULL) {
        TRACE("\nmsk NULL");
        ret = KEY_STATUS_NULL;
        goto ksr_read_msk_end;
    }
    if(key_blk.length > 24) {
        TRACE("\nkey lengh error");
        ret = -1;
        goto ksr_read_msk_end;
    }
    key_check(key_blk.chk_mod,msk->chkno,KEY_MAX_LEN,key_blk.key,check);
    if (memcmp(check,key_blk.chk,KEY_CHK_LEN)){
        TRACE("\ncheck err");
        ret = -1;
        goto ksr_read_msk_end;
    }
    *length = key_blk.length;
    memcpy(output,key_blk.key,key_blk.length);
    ret = 0;
ksr_read_msk_end:
    if ( ret ) {
        *length = 0;
    }
    CLRBUF(key_b);
    memset((uchar *)&key_blk,0,sizeof(struct KEY_BLK));
    return ret;
}

int ksr_write_msk(struct KEY_DEF *msk,uint8_t length, uint8_t *input)
{
    struct KEY_BLK key_blk;
    uint8_t key_b[KEY_MAX_LEN];
    uint8_t key_len;
    int ret;
    //frist read RTK and decrypt
//    if(length % DES_BLOCK_LENGTH) {
    if(length > 24) {
        TRACE("\nlength error");
        return -1;
    }
//    DISPBUF("明文",length,0,(uint8_t *)input);
    memset(&key_blk,0x00,KEY_BLK_LEN);
    key_blk.valid = KEY_STATUS_VALID;
    key_blk.chk_mod = KEY_CHK_MOD;
    key_blk.length = length;
    memcpy(key_blk.key,input,length);
    //make check
    key_check(key_blk.chk_mod,msk->chkno,KEY_MAX_LEN,key_blk.key,key_blk.chk);
//    TRACE("\n-|MSK:%d plaintex",msk->mno);
//    ksr_disp_key(&key_blk);

    //then encrypt
    memset(key_b,0x00,sizeof(key_b));
    if(ksr_read_kek(msk->kekno,msk->chkno,&key_len,key_b))
    {
        TRACE("\nread kek err");
        ret = -1;
        goto ksr_write_msk_end;
    }
//    vDispBufTitle("writemsk明文",KEY_BLK_LEN,0,&key_blk);
    if (key_len == 24)
        des3_encrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
    else
        des2_encrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
//    vDispBufTitle("writemsk密文",KEY_BLK_LEN,0,&key_blk);
    key_wirte_flash(&key_blk,msk->mno);
    ret = 0;
ksr_write_msk_end:   
    CLRBUF(key_b);
    memset((uchar *)&key_blk,0,sizeof(struct KEY_BLK));
    return ret;
}

//work key
int ksr_read_wsk(struct KEY_DEF *wsk,uint8_t *length, uint8_t *output)
{
    struct KEY_BLK key_blk;
    uint8_t check[KEY_CHK_LEN];
    uint8_t key_b[KEY_MAX_LEN];
    uint8_t key_len;
    int ret;
    //frist read WSK and decrypt
    key_read_flash(&key_blk,wsk->wno);
    if ( blk_if_blank(&key_blk) ) {
        TRACE("\nkek NULL");
        ret = KEY_STATUS_NULL;
        goto ksr_read_wsk_end;
    }
    ksr_read_msk(wsk,&key_len,key_b);
    if (key_len == 24)
        des3_decrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
    else
        des2_decrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
    TRACE("\n-|WSK:%d plaintex",wsk->wno);
    ksr_disp_key(&key_blk);
    //make check
    if (key_blk.valid == KEY_STATUS_NULL) {
        TRACE("\nwsk NULL");
        ret = KEY_STATUS_NULL;
        goto ksr_read_wsk_end;
    }
    if(key_blk.length % DES_BLOCK_LENGTH) {
        TRACE("\nkey lengh error");
        ret = -1;
        goto ksr_read_wsk_end;
    }
    key_check(key_blk.chk_mod,wsk->chkno,KEY_MAX_LEN,key_blk.key,check);
    if (memcmp(check,key_blk.chk,KEY_CHK_LEN)){
        TRACE("\ncheck err");
        ret = -1;
        goto ksr_read_wsk_end;
    }
    *length = key_blk.length;
    memcpy(output,key_blk.key,key_blk.length);
    ret = 0;
ksr_read_wsk_end:
    if ( ret ) {
        *length = 0;
    }
    CLRBUF(key_b);
    memset((uchar *)&key_blk,0,sizeof(struct KEY_BLK));
    return ret;
}

int ksr_write_wsk(struct KEY_DEF *wsk,uint8_t length, uint8_t *input)
{
    struct KEY_BLK key_blk;
    uint8_t key_b[KEY_MAX_LEN];
    uint8_t key_len;

    //frist make key block
    if(length % DES_BLOCK_LENGTH) {
        TRACE("\nlength error");
        return -1;
    }
    memset(&key_blk,0x00,KEY_BLK_LEN);
    key_blk.valid = KEY_STATUS_VALID;
    key_blk.chk_mod = KEY_CHK_MOD;
    key_blk.length = length;
    memcpy(key_blk.key,input,length);
    //make check
    key_check(key_blk.chk_mod,wsk->chkno,KEY_MAX_LEN,key_blk.key,key_blk.chk);
    TRACE("\n-|WSK:%d plaintex",wsk->wno);
    ksr_disp_key(&key_blk);

    //then encrypt
    memset(key_b,0x00,sizeof(key_b));
    ksr_read_msk(wsk,&key_len,key_b);
    if (key_len == 24)
        des3_encrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
    else
        des2_encrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
    key_wirte_flash(&key_blk,wsk->wno);
    
    CLRBUF(key_b);
    memset((uchar *)&key_blk,0,sizeof(struct KEY_BLK));
    return 0;
}




int ksr_clear_sector(uint32_t start_addr,uint32_t end_addr)
{
    int iRet; 

    TRACE("\n-|ksr erase sector");
    if (start_addr >KEYZONE_LEN
        || end_addr < start_addr)
    {
        return -1;
    }
    start_addr += KEY_ZONE_ADDR;
    end_addr += KEY_ZONE_ADDR;

    for ( ;start_addr <= end_addr; start_addr += FLASH_SECTOR_SIZE)
    {
        TRACE(" %08X",start_addr);
        iRet = drv_flash_SectorErase(start_addr);
        if (iRet) {
            TRACE("\nksr flash err:s-%08X e-%08X",start_addr,end_addr);
            break;
        }
    }

    return iRet;
}

/*-----------------------------------------------------------------------------}
 *  快速写密钥区
 *-----------------------------------------------------------------------------{*/
int fast_ksr_write_key(uint appid,uint type)
{
    struct KEY_BLK key_blk;
    int i,num,ret;
    uint addr;
    uint8_t key_b[KEY_MAX_LEN];
    uint8_t key_len,length;
    length = 24;
    memset(&key_blk,0x00,KEY_BLK_LEN);
    key_blk.valid = KEY_STATUS_VALID;
    key_blk.chk_mod = KEY_CHK_MOD;
    key_blk.length = length;
    addr = KEY_ZONE_APP_ADDR(appid-1);
#ifdef DEBUG_Dx
    TRACE("\r\n ====fast写密钥%d 应用:%d-%x ====",type,appid,addr);
//    for ( i=0 ; i<APP_TOTAL_NUM ; i++ ) {
//        TRACE("\r\n 应用%d起始地址:%x",i+1,KEY_ZONE_APP_ADDR(i));
//    }
#endif
    // APPEK加密
    PCI_ReadKeyFromFlash(appid,KEY_TYPE_APPEK,0,&key_len,key_b);
    if ( type == 0) {
        num = NUM_MK;
        addr += FPOS(_KEY_APP_,msk);
        memset(key_blk.key,'8',length);
    } else {
        num = NUM_WK;
        addr += FPOS(_KEY_APP_,wsk);
        memset(key_blk.key,0,length);
    }
    key_check(key_blk.chk_mod,0,KEY_MAX_LEN,key_blk.key,key_blk.chk);
#ifdef DEBUG_Dx
    TRACE("\r\n addr:%x num:%d",addr,num);
    DISPBUF("明文",sizeof(key_blk),0,&key_blk);
    DISPBUF("密钥",key_len,0,&key_b);
#endif
    if (key_len == 24) {
        des3_encrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
    } else {
        des2_encrypt_ecb((uint8_t *)&key_blk,KEY_BLK_LEN,key_b,(uint8_t *)&key_blk);
    }
#ifdef DEBUG_Dx
    DISPBUF("密文",sizeof(key_blk),0,&key_blk);
#endif
    for ( i=0 ; i<num ; i++  ) {
        memcpy(&gucBuff[i*KEY_BLK_LEN],(uchar *)&key_blk,KEY_BLK_LEN);
    }
    CLRBUF(key_b);
    memset((uchar *)&key_blk,0,sizeof(key_blk));
    // 1次写到flash
    ret = drv_flash_SectorErase(addr);
    if ( ret ) {
        return 1;
    }
    ret = drv_flash_write(addr,(uint *)gucBuff,num*KEY_BLK_LEN);
    if ( ret ) {
        return 2;
    }
    return ret;
}


#endif




