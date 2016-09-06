/***************** (C) COPYRIGHT 2016 START Computer equipment *****************
 * File Name          : pinpad_keystore.c
 * bfief              : 
 * Author             : luocs 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 4/24/2016 1:57:04 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"
#include "pinpad_keystore.h"


/*-----------------------------------------------------------------------------}
 *  判断blk是否为空
 *  返回: 1-为空 0-不为空
 *-----------------------------------------------------------------------------{*/
static int blk_if_blank(struct KEY_BLK *p)
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

static void key_lrc(uint32_t length,uint8_t *input,uint8_t *output)
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



int key_frist_level_read(struct KEY_BLK *blk)
{
    uint8_t srk[24];
#ifdef CFG_TAMPER
    if (hw_dryice_read_key((FPOS(ST_MMAP_KEY,aucMMK)>>2),MK_UNIT_LEN/4,(uint32_t*)srk)) {
        TRACE("\nRead mmk error");
        return -1;
    }
    blk->length = MK_UNIT_LEN;
#else
    uint8_t rootkey[] = "1234567890abcdeffedcba54";
    memcpy(srk,rootkey,24);
    blk->length = 24;
#endif
    blk->valid = KEY_STATUS_VALID;
    memcpy(blk->val,srk,blk->length);
//    TRACE("\n-|SRK:");
//    vDispBuf(blk->length,0,blk->val); 
    return 0;
}



static int key_blk_normal_read(int32_t blknum,struct KEY_BLK *blk)
{
    memcpy((uint8_t *)blk,(uint8_t *)KEY_ADDR(blknum),KEY_BLK_LEN);
    if (blk_if_blank(blk) == 1)
    {
        return RTV_KEY_ERR_NULL;
    }
    return 0;
}

static int key_blk_normal_write(int32_t blknum,const struct KEY_BLK *blk)
{
#ifdef CFG_LOWPWR
    exit_lowerpower_freq();
#endif
    if (flash_write(KEY_ADDR(blknum),KEY_BLK_LEN,(uint8_t*)blk) != 0) {
        return RTV_KEY_ERR_FLASH;
    } else {
        return RTV_KEY_SUCCE;
    }
}

struct DualBack {
    uint32_t magic;//magic number:0XAA55AA55
    uint32_t counter;
    uint32_t crc32;
    uint32_t rfu[8-3];
};
#define DualBackMagic     ((uint32_t)0XAA55AA55)

uint32_t dual_back_addr_read(uint32_t orig_sector_addr,uint32_t copy_sector_addr)
{
    uint32_t opt_addr = 0;
    struct DualBack *orig_ctrl = (struct DualBack *)(orig_sector_addr+ FLASH_SECTOR_SIZE-KEY_BLK_LEN);
    struct DualBack *copy_ctrl = (struct DualBack *)(copy_sector_addr+ FLASH_SECTOR_SIZE-KEY_BLK_LEN);

    if ((orig_ctrl->magic ==  DualBackMagic) && (copy_ctrl->magic ==  DualBackMagic))
    {
        if (orig_ctrl->counter > copy_ctrl->counter) {
            opt_addr = orig_sector_addr;
        } else {
            opt_addr = copy_sector_addr;
        }
    } else 
    {
        if (orig_ctrl->magic == DualBackMagic) {
            opt_addr = orig_sector_addr;
        } else {
            opt_addr = copy_sector_addr;
        }
    }
//    TRACE("\r\n==|opt_add r:%08X",opt_addr);
    return opt_addr;
}

uint32_t dual_back_addr_write(uint32_t orig_sector_addr,uint32_t copy_sector_addr)
{
    return ((dual_back_addr_read(orig_sector_addr, copy_sector_addr) == orig_sector_addr) ? copy_sector_addr:orig_sector_addr);
}

int key_blk_dual_back_write(int32_t blknum,const struct KEY_BLK *blk)
{
    uint8_t buffer[FLASH_SECTOR_SIZE];
    uint32_t tmp_addr = 0;
    uint32_t opt_addr = 0;
    struct DualBack *dual = NULL;
#ifdef CFG_LOWPWR
    exit_lowerpower_freq();
#endif
    //get original sector start addr
    tmp_addr = KEY_SECTOR_ADDR(blknum);
    tmp_addr = dual_back_addr_read(tmp_addr, tmp_addr+FLASH_SECTOR_SIZE);
    //copy from origianl sector
    memcpy(buffer,(uint8_t *)tmp_addr,FLASH_SECTOR_SIZE);
    dual = (struct DualBack *)(buffer+ FLASH_SECTOR_SIZE-KEY_BLK_LEN);
    if (dual->magic == DualBackMagic) {
        dual->counter++;
    } else {
        dual->magic = DualBackMagic;
        dual->counter = 0;
    }
    //save KEY values to the offset addr
    tmp_addr  = KEY_ADDR(blknum)%FLASH_SECTOR_SIZE;
    memcpy(buffer+tmp_addr ,blk,KEY_BLK_LEN);
    //get copy sector start addr
    tmp_addr = KEY_SECTOR_ADDR(blknum);
    opt_addr = dual_back_addr_write(tmp_addr, tmp_addr+FLASH_SECTOR_SIZE);
//    TRACE("\r\n==|copy_add w:%08X",opt_addr);
    if (drv_flash_SectorErase(opt_addr) == Flash_OK ) 
    {
        if (drv_flash_write(opt_addr,(uint32_t *)buffer,
                            FLASH_SECTOR_SIZE) == Flash_OK )
        {
            CLRBUF(buffer);
            return RTV_KEY_SUCCE;
        }
    }
    CLRBUF(buffer);
    return RTV_KEY_ERR_FLASH;
}


int key_blk_dual_back_read(int32_t blknum,struct KEY_BLK *blk)
{
    uint32_t tmp_addr = 0;
    uint32_t opt_addr = 0;

    tmp_addr = KEY_SECTOR_ADDR(blknum);
    opt_addr = dual_back_addr_read(tmp_addr, tmp_addr+FLASH_SECTOR_SIZE);
    //offset addr
    tmp_addr  = KEY_ADDR(blknum)%FLASH_SECTOR_SIZE;
    opt_addr += tmp_addr;
    memcpy((uint8_t *)blk,(uint8_t *)opt_addr,KEY_BLK_LEN);
    if (blk_if_blank(blk) == 1)
    {
        return RTV_KEY_ERR_NULL;
    }
    return 0;
}

int key_blk_check(struct KEY_BLK *blk)
{
    uint8_t check[KEY_CHK_LEN];

    if (blk->valid == KEY_STATUS_NULL) {
        return  RTV_KEY_ERR_NULL;
    }
    if(blk->length > 24) {
        TRACE("\nkek key lengh :%d error\n",blk->length);
        return RTV_KEY_ERR;
    }
    if (blk->chk_mod == KEY_CHK_LRC) 
    {
        key_lrc(KEY_MAX_LEN,blk->val,check);
    } else {
        //cacul_mac_ecb();
        //@todo:
        return RTV_KEY_SUCCE;
    }
    if (memcmp(check,blk->chk,KEY_CHK_LEN))
    {
        return RTV_KEY_ERR_CHECK;
    }

    return RTV_KEY_SUCCE;
}

int key_blk_decrypt(uint8_t kek_len,const uint8_t *kek,struct KEY_BLK *key)
{
    if (kek_len == 24)
    {
        des3_decrypt_ecb((uint8_t *)key,KEY_BLK_LEN,kek,(uint8_t *)key);
    } else
    {
        des2_decrypt_ecb((uint8_t *)key,KEY_BLK_LEN,kek,(uint8_t *)key);
    }
    return key_blk_check(key);
}

int key_blk_encrypt(uint8_t kek_len,const uint8_t *kek,struct KEY_BLK *key)
{
    key->rfu = 0;
    key->valid = KEY_STATUS_VALID;
    if (key->chk_mod == KEY_CHK_LRC) 
    {
        key_lrc(KEY_MAX_LEN,key->val,key->chk);
    } else {
        //@todo:
        //cacul_mac_ecb();
    }
    if (kek_len == 24)
    {
        des3_encrypt_ecb((uint8_t *)key,KEY_BLK_LEN,kek,(uint8_t *)key);
    } else
    {
        des2_encrypt_ecb((uint8_t *)key,KEY_BLK_LEN,kek,(uint8_t *)key);
    }
    return 0;
}


//key system respository
int key_second_level_read(int kek_blk_num,int key_blk_num,
                          uint16_t *length, uint8_t *output)
{
    struct KEY_BLK key;
    struct KEY_BLK kek;
    int iRet = 0;

    if (kek_blk_num != SRK_BLK_NUM) {
        return -1;
    }
    if ((iRet = key_frist_level_read(&kek)) != 0)
    {
        return iRet;
    }
    if ((iRet = key_blk_dual_back_read(key_blk_num,&key)) != 0)
    {
        memset(&kek,0,KEY_BLK_LEN);
        return iRet;
    }
    if ((iRet = key_blk_decrypt(kek.length,kek.val,&key)) != 0) 
    {
        memset(&kek,0,KEY_BLK_LEN);
        memset(&key,0,KEY_BLK_LEN);
        return iRet;
    }
    *length = key.length;
    memcpy(output,key.val,key.length);
    memset(&kek,0,KEY_BLK_LEN);
    memset(&key,0,KEY_BLK_LEN);
    return 0;
}


int key_second_level_write(int kek_blk_num,int key_blk_num,
                           uint16_t length, const uint8_t *input)
{
    struct KEY_BLK key;
    struct KEY_BLK kek;
    int iRet = 0;

    if (kek_blk_num != SRK_BLK_NUM) {
        return -1;
    }
    memset(&kek,0,KEY_BLK_LEN);
    memset(&key,0,KEY_BLK_LEN);
    if ((iRet = key_frist_level_read(&kek)) != 0)
    {
        DISPERR(iRet);
        return iRet;
    }
    key.chk_mod =  KEY_CHK_MOD;
    key.length = length;
    memcpy(key.val,input,length);
    TRACE("\r\n kekindex:%d %d",kek_blk_num,key_blk_num);
//    DISPBUF("2级写密钥",sizeof(kek),0,&kek);
//    DISPBUF("明文",sizeof(key),0,&key);
    if ((iRet = key_blk_encrypt(kek.length,kek.val,&key)) != 0)
    {
        memset(&kek,0,KEY_BLK_LEN);
        memset(&key,0,KEY_BLK_LEN);
        DISPPOS(iRet);
        return iRet;
    }
//    DISPBUF("密文",sizeof(key),0,&key);
    iRet = key_blk_dual_back_write(key_blk_num,&key);
    memset(&kek,0,KEY_BLK_LEN);
    memset(&key,0,KEY_BLK_LEN);
    return iRet;
}


int key_third_level_read(int kek_blk_num,int key_blk_num,
                         uint16_t *length, uint8_t *output)
{
    struct KEY_BLK key;
    uint16_t kek_len;
    uint8_t  kek_val[24];
    int iRet = 0;

    if ((iRet = key_second_level_read(SRK_BLK_NUM,kek_blk_num,&kek_len,kek_val)) != 0)
    {
        return iRet;
    }
    if ((iRet = key_blk_normal_read(key_blk_num,&key)) != 0)
    {
        memset(kek_val,0,KEY_BLK_LEN);
        return iRet;
    }
    if ((iRet = key_blk_decrypt(kek_len,kek_val,&key)) != 0) 
    {
        memset(kek_val,0,KEY_BLK_LEN);
        memset(&key,0,KEY_BLK_LEN);
        return iRet;
    }
    *length = key.length;
    memcpy(output,key.val,key.length);
    memset(kek_val,0,KEY_BLK_LEN);
    memset(&key,0,KEY_BLK_LEN);
    return 0;
}

//key system respository
int key_second_level_read_nobackup(int kek_blk_num,int key_blk_num,
                          uint16_t *length, uint8_t *output)
{
    struct KEY_BLK key;
    struct KEY_BLK kek;
    int iRet = 0;

    if (kek_blk_num != SRK_BLK_NUM) {
        return -1;
    }
    if ((iRet = key_frist_level_read(&kek)) != 0)
    {
        DISPERR(iRet);
        return iRet;
    }
    TRACE("\r\n kek_blk_num:%d-%d",kek_blk_num,key_blk_num);
    if ((iRet = key_blk_normal_read(key_blk_num,&key)) != 0)
    {
        DISPERR(iRet);
        return iRet;
    }
//    DISPBUF("mmk",sizeof(kek),0,&kek);
//    DISPBUF("TK密文",sizeof(key),0,&key);
    if ((iRet = key_blk_decrypt(kek.length,kek.val,&key)) != 0) 
    {
        memset(&kek,0,KEY_BLK_LEN);
        memset(&key,0,KEY_BLK_LEN);
        return iRet;
    }
//    DISPBUF("TK明文",sizeof(key),0,&key);
    *length = key.length;
    memcpy(output,key.val,key.length);
    memset(&kek,0,KEY_BLK_LEN);
    memset(&key,0,KEY_BLK_LEN);
    return 0;
}


int key_second_level_write_nobackup(int kek_blk_num,int key_blk_num,
                           uint16_t length, const uint8_t *input)
{
    struct KEY_BLK key;
    struct KEY_BLK kek;
    int iRet = 0;
    if (kek_blk_num != SRK_BLK_NUM) {
        return -1;
    }
    DISPPOS(key_blk_num);
    memset(&kek,0,KEY_BLK_LEN);
    memset(&key,0,KEY_BLK_LEN);
    if ((iRet = key_frist_level_read(&kek)) != 0)
    {
        DISPERR(iRet);
        return iRet;
    }
    key.chk_mod =  KEY_CHK_MOD;
    key.length = length;
    memcpy(key.val,input,length);
//    DISPBUF("明文",length,0,(uchar *)input);
//    DISPBUF("MMK",sizeof(_KEY_BLK_),0,&kek);
    if ((iRet = key_blk_encrypt(kek.length,kek.val,&key)) != 0)
    {
        memset(&kek,0,KEY_BLK_LEN);
        memset(&key,0,KEY_BLK_LEN);
        return iRet;
    }
//    DISPBUF("密文",sizeof(key),0,&key);
    iRet = key_blk_normal_write(key_blk_num,&key);
    memset(&kek,0,KEY_BLK_LEN);
    memset(&key,0,KEY_BLK_LEN);
    return iRet;
}

int key_third_level_write(int kek_blk_num,int key_blk_num,
                          uint16_t length, const uint8_t *input)
{
    struct KEY_BLK key;
    uint16_t kek_len;
    uint8_t kek_val[24];
    int iRet = 0;

    memset(kek_val,0,KEY_BLK_LEN);
    memset(&key,0,KEY_BLK_LEN);
    if ((iRet = key_second_level_read(SRK_BLK_NUM,kek_blk_num,&kek_len,kek_val)) != 0)
    {
        return iRet;
    }
    key.chk_mod =  KEY_CHK_MOD;
    key.length = length;
    memcpy(key.val,input,length);
    if ((iRet = key_blk_encrypt(kek_len,kek_val,&key)) != 0)
    {
        memset(kek_val,0,KEY_BLK_LEN);
        memset(&key,0,KEY_BLK_LEN);
        return iRet;
    }
    iRet = key_blk_normal_write(key_blk_num,&key);
    memset(kek_val,0,KEY_BLK_LEN);
    memset(&key,0,KEY_BLK_LEN);
    return iRet;
}




//void ksr_disp_key(const struct KEY_BLK *pkey)
//{
//    TRACE_BUF(NULL,(uint8_t *)pkey,KEY_BLK_LEN);
//    DISPBUF(NULL,KEY_BLK_LEN,0,(uint8_t *)pkey);
//}


//key stystem respository:rtk(root key)
int ksr_read_mmk(uint8_t *length,uint8_t *rtk)
{
    struct KEY_BLK blk;
    key_frist_level_read(&blk);
    *length = blk.length;
    memcpy(rtk,blk.val,*length);
    return 0;
}

//int ksr_read_mmap_key(void)
//{
//#ifdef CFG_TAMPER1
//    if (hw_dryice_read_key((FPOS(ST_MMAP_KEY,aucMMK)>>2),(MMAP_KEY_LEN+CHECK_LEN)/4,(uint32_t*)gMKey.aucMMK)) {
//        TRACE("\nRead elk error");
//        return -1;
//    }
//#endif
//    return 0;
//}

#ifdef DEBUG_Dx 
extern int mk_get_blk_num(uint8_t index);
extern int wk_get_blk_num(uint8_t mk_index,uint8_t type,uint8_t index);
extern int pubk_get_blk_num(uint8_t index);
int dbg_keyzone(void)
{
    _KEY_ZONE_ *p;
    int i;
    p = (_KEY_ZONE_ *)KEY_ZONE_ADDR;
    uchar mmk[24];
    while ( 1 ) {
        TRACE("\r\n KEY_ZONE_ADDR:%x MK:%x wk:%x pubk:%x",KEY_ZONE_ADDR,&p->mk,&p->wk,&p->pubk);
        TRACE("\r\n LEN:%x MK:%x wk:%x pubk:%x",sizeof(_KEY_ZONE_),sizeof(p->mk),sizeof(p->wk),sizeof(p->pubk));
        TRACE("\r\n Index: MK:%d-%d TK:%d",mk_get_blk_num(0),mk_get_blk_num(BLKPERBLOCK_NUM-1),pubk_get_blk_num(0));
        TRACE("\r\n WK0-0 :%d-%d-%d",wk_get_blk_num(0,KEY_TYPE_PINK,0),wk_get_blk_num(0,KEY_TYPE_MACK,0),wk_get_blk_num(0,KEY_TYPE_DESK,0));
        TRACE("\r\n WK63-0:%d-%d-%d",wk_get_blk_num(62,KEY_TYPE_PINK,0),wk_get_blk_num(62,KEY_TYPE_MACK,0),wk_get_blk_num(62,KEY_TYPE_DESK,0));
        TRACE("\r\n 1-MK 2-WK 3-pubk 4-mmk");
        switch ( InkeyCount(0) )
        {
        case 1 :
            DISPBUF("MK0",sizeof(p->mk.orig),0,p->mk.orig);
            DISPBUF("MK1",sizeof(p->mk.orig),0,p->mk.orig);
            break;
        case 2 :
            TRACE("\r\n 主密钥索引:");
            i = InkeyCount(0);
            DISPBUF("PINK",sizeof(p->wk[i].pink),0,p->wk[i].pink);
            DISPBUF("MACK",sizeof(p->wk[i].mack),0,p->wk[i].mack);
            DISPBUF("DESK",sizeof(p->wk[i].desk),0,p->wk[i].desk);
            break;
        case 3 :
            DISPBUF("TK",sizeof(p->pubk.TK),0,p->pubk.TK);
            break;
        case 4 :
            i = 0;
            ksr_read_mmk((uchar *)&i,mmk);
            DISPBUF("mmk",i,0,mmk);
            break;
        case 99 :
            return 0;
            break;
        default :
            break;
        }
    }
}
#endif

