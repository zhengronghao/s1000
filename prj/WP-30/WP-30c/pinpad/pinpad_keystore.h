/***************** (h) COPYRIGHT 2016 START Computer equipment *****************
 * File Name          : pinpad_keystore.h
 * bfief              : 
 * Author             : luocs 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 4/24/2016 1:57:12 PM
 * Description        : 
 *******************************************************************************/
#ifndef __PINPAD_KEYSTORE_H__
#define __PINPAD_KEYSTORE_H__ 

#define KEY_MAX_LEN    (24)
#define KEY_CHK_LEN    (4)
#define KEY_CHK_LRC    0
#define KEY_CHK_MAC    1
#define KEY_CHK_MOD    (KEY_CHK_LRC)  //0-lrc  1-mac

#define KEY_STATUS_VALID 0xAA
#define KEY_STATUS_NULL  0xFF
typedef struct KEY_BLK 
{
    uint8_t valid;   //加密 0xAA 
    uint8_t rfu;     //加密
    uint8_t length;  //加密
    uint8_t chk_mod; // 校验模式 0-LRC 1-MAC
    uint8_t val[KEY_MAX_LEN]; // 密文
    uint8_t chk[KEY_CHK_LEN]; // 校验值:对val校验
}_KEY_BLK_;
#define KEY_BLK_LEN                 sizeof(struct KEY_BLK)
#define OFFSET_BLK_LENGTH           FPOS(struct KEY_BLK,length)
#define KEY_BLK_ENCRYPT_LEN         (KEY_BLK_LEN-OFFSET_BLK_LENGTH)

/*-----------------------------------------------------------------------------}
 *  K21F密钥分配，1块=4096B，每块可以分配128个单元
 *  主密钥126组，每1组主密钥下包含42组PINK，42组DESK和42组MACK
 *  密钥空间大小: 2(主密钥和备份主密钥)+126(工作密钥)=128块(512K)
 *  K21D密钥分配, 1块=2048B，每块可以分配64个单元
 *  主密钥63组，每1组主密钥下包含21组PINK，21组DESK和21组MACK
 *  密钥空间大小: 2(主密钥和备份主密钥)+63(工作密钥)=65块(130K)
 *-----------------------------------------------------------------------------{*/
#define  KEY_SECTOR_SIZE        FLASH_SECTOR_SIZE 
#define  BLKPERBLOCK_MAX_NUM    (KEY_SECTOR_SIZE/KEY_BLK_LEN)  //每密钥扇区最多保存密钥单元个数
#define  BLKPERBLOCK_NUM    (63)  //每密钥扇区实际使用密钥单元个数
#define  WORKINGKEY_NUM     (BLKPERBLOCK_NUM/3) //工作密钥数:各21
#define  MASTERKEY_NUM      48 // (BLKPERBLOCK_NUM) // 主密钥数:48

typedef struct KEY_PUB 
{
    struct KEY_BLK TK[1];
    struct KEY_BLK rfu_w[BLKPERBLOCK_MAX_NUM-1];
}_KEY_PUB_;

typedef struct KEY_WORKING 
{//32*64 = 2048 or 32*128 = 4096
    struct KEY_BLK pink[WORKINGKEY_NUM];
    struct KEY_BLK mack[WORKINGKEY_NUM];
    struct KEY_BLK desk[WORKINGKEY_NUM];
    struct KEY_BLK rfu_w[BLKPERBLOCK_MAX_NUM-WORKINGKEY_NUM*3];
}_KEY_WORK_;

typedef struct KEY_MASTER
{//dual backup
    struct KEY_BLK orig[BLKPERBLOCK_MAX_NUM]; //original
    struct KEY_BLK copy[BLKPERBLOCK_MAX_NUM];
}_KEY_MK_;

typedef struct KEY_ZONE
{
    struct KEY_MASTER  mk;
    struct KEY_WORKING wk[MASTERKEY_NUM];   //encrypt key is appek 0x0C0~
    struct KEY_PUB  pubk;
}_KEY_ZONE_;

/*-----------------------------------------------------------------------------}
 *  密钥区起始地址:0x45000  单密钥区大小:0x3000(6*2K=12K) 
 *  应用数:6
 *  整个密钥区大小:0x12000(72K=12K*6)
 *-----------------------------------------------------------------------------{*/
//#define KEYZONE_LEN                  0
//#define KEY_ZONE_ADDR                (SA_KEYZONE-KEYZONE_LEN)
#define KEY_ZONE_ADDR                SA_KEYZONE
#define KEY_ADDR(blknum)             (KEY_ZONE_ADDR+((blknum)*KEY_BLK_LEN)) 
#define KYE_SECTOR_INDX(blknum)      ((blknum)%(FLASH_SECTOR_SIZE/KEY_BLK_LEN))
#define KEY_SECTOR_ADDR(blknum)      (KEY_ADDR(blknum)&(~(FLASH_SECTOR_SIZE-1))) 
/*-----------------------------------------------------------------------------}
 * 密钥空间分配:1个密钥区占用12K,支持6个应用
 *-----------------------------------------------------------------------------{*/
#define APP_TOTAL_NUM   1

#define SRK_BLK_NUM         ((int)-1) //Security Root Key
//Return Value:RTV
#define RTV_KEY_SUCCE       (int)0 
#define RTV_KEY_ERR         (int)(-1) 
#define RTV_KEY_ERR_PARA    (int)(-2) 
#define RTV_KEY_ERR_NULL    (int)(-3) 
#define RTV_KEY_ERR_CHECK   (int)(-4) 
#define RTV_KEY_ERR_FLASH   (int)(-5) 

int key_second_level_read(int kek_blk_num,int key_blk_num,
                          uint16_t *length, uint8_t *output);
int key_second_level_write(int kek_blk_num,int key_blk_num,
                           uint16_t length, const uint8_t *input);
int key_second_level_read_nobackup(int kek_blk_num,int key_blk_num,
                          uint16_t *length, uint8_t *output);
int key_second_level_write_nobackup(int kek_blk_num,int key_blk_num,
                           uint16_t length, const uint8_t *input);
int key_third_level_read(int kek_blk_num,int key_blk_num,
                         uint16_t *length, uint8_t *output);
int key_third_level_write(int kek_blk_num,int key_blk_num,
                          uint16_t length, const uint8_t *input);

int ksr_read_mmk(uint8_t *length,uint8_t *rtk);

#endif


