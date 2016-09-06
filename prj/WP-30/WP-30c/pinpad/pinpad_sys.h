/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : pinpad_sys.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 3/30/2014 1:37:10 PM
 * Description        : 
 *******************************************************************************/

#ifndef __PINPAD_SYS_H__
#define __PINPAD_SYS_H__
#if 0
#define KEY_MAX_LEN    (24)
#define KEY_CHK_LEN    (4)
#define KEY_CHK_LRC    0
#define KEY_CHK_MAC    1
#define KEY_CHK_MOD    (KEY_CHK_LRC)  //0-lrc  1-mac

#define KEY_STATUS_VALID 0xAA
#define KEY_STATUS_NULL  0xFF
typedef struct KEY_BLK 
{
    uint8_t valid;   // 不加密 0xAA 
    uint8_t rfu;     // 不加密
    uint8_t length;
    uint8_t chk_mod; // 校验模式 0-LRC 1-MAC
    uint8_t key[KEY_MAX_LEN]; // 密文
    uint8_t chk[KEY_CHK_LEN]; // 校验值
}_KEY_BLK_;
#define KEY_BLK_LEN    sizeof(struct KEY_BLK)
#define OFFSET_BLK_LENGTH           FPOS(struct KEY_BLK,length)
#define KEY_BLK_ENCRYPT_LEN         (KEY_BLK_LEN-OFFSET_BLK_LENGTH)

#define APP_TOTAL_NUM     6
#define MSK_DECRYPT_NUM   0
#define WORKKEY_NUM       10  // 工作密钥数
#define MASTERKEY_NUM     64  // 主密钥数
typedef struct KEY_WORK
{//32*64 = 2048
    struct KEY_BLK pink[WORKKEY_NUM];
    struct KEY_BLK mack[WORKKEY_NUM];
    struct KEY_BLK desk[WORKKEY_NUM];
    struct KEY_BLK rfu_w[64-WORKKEY_NUM*3];
}_KEY_WORK_;

#define APP_INF_LEN sizeof(struct APP_INF)
typedef struct APP_PUB
{//2KB every union is 32B
    struct KEY_BLK appflag; //1encrypt key is mmk 
    struct KEY_BLK appname; //2encrypt key is mmk 
    struct KEY_BLK apppswa; //3encrypt key is mmk 
    struct KEY_BLK apppswb; //4encrypt key is mmk 
    struct KEY_BLK appsn;   //5encrypt key is mmk 
    struct KEY_BLK appak;   //6encrypt key is mmk 
    struct KEY_BLK appek;   //7encrypt key is mmk 
    struct KEY_BLK trsk;    //9encrypt key is mmk:transferkey 
    struct KEY_BLK appfsk;  //9encrypt key is mmk 
    struct KEY_BLK magkey;  //10encrypt key is mmk 
    struct KEY_BLK mckey;   //11encrypt key is mmk 
    struct KEY_BLK app_key_rfu[64-11]; //encrypt key is mmk 
}_APP_PUB_;
typedef struct KEY_APP
{
    struct APP_PUB pub;      // 0 
    struct KEY_BLK auth_mk[MASTERKEY_NUM];//encrypt key is appek 0x040~
    struct KEY_BLK msk[MASTERKEY_NUM];    //encrypt key is appek 0x080~ 
    struct KEY_WORK auth_wk;   //encrypt key is appek 0x0C0~
    struct KEY_WORK wsk;       //encrypt key is appek 0x100~
    struct KEY_WORK wsk_back;  //the back up of wsk   0x140~
}_KEY_APP_;
typedef struct APP_ZONE 
{
    struct KEY_APP app[APP_TOTAL_NUM];  //12K*6 = 72K
}_APP_ZONE_;

// ram 各应用密钥区 8+60+472*4=1956B 剩余92B
typedef struct _RAM_KEY
{
    uint8_t bck[8];     //0
    uint8_t mapp[60];   //8
    uint8_t app1_key[472]; //68
    uint8_t app2_key[472]; //540
    uint8_t app3_key[472]; //1012
    uint8_t dl_flag[8]; //1484
                        //1492

}RAM_KEY_DEF;

struct KEY_DEF
{
    uint16_t wno;    //工作密钥索引 
    uint16_t mno;    //索引
    uint16_t kekno;  // 指定加密密钥
    uint16_t chkno;  // 校验类型
};

/*-----------------------------------------------------------------------------}
 *  密钥区起始地址:0x45000  单密钥区大小:0x3000(6*2K=12K) 
 *  应用数:6
 *  整个密钥区大小:0x12000(72K=12K*6)
 *-----------------------------------------------------------------------------{*/
#define KEYZONE_LEN                 sizeof(struct APP_ZONE)
#define KEY_ZONE_ADDR               (SA_ZK-KEYZONE_LEN)
#define KEY_ZONE_APP_ADDR(id)       (KEY_ZONE_ADDR+(id)*sizeof(struct KEY_APP))
#define KEY_ADDR(key_num)           (KEY_ZONE_ADDR+((key_num)*KEY_BLK_LEN)) 
#define KYE_PAGE_INDX(key_num)      ((key_num)%(FLASH_SECTOR_SIZE/KEY_BLK_LEN))
#define KEY_PAGE_ADDR(key_num)      (KEY_ADDR(key_num)&(~(FLASH_SECTOR_SIZE-1))) 
/*-----------------------------------------------------------------------------}
 * 密钥空间分配:1个密钥区占用12K,支持6个应用
 *-----------------------------------------------------------------------------{*/
extern const struct APP_ZONE * const gpApp;

int ksr_read_mmk(uint8_t *length,uint8_t *rtk);
int ksr_read_elk(uint8_t *length,uint8_t *elk);
int ksr_read_kek(uint16_t kekno,uint32_t chkno,uint8_t *length, uint8_t *output);
int ksr_read_mmap_key(void);
int ksr_write_kek(uint16_t kekno,uint32_t chkno,uint8_t length, uint8_t *input);
int ksr_read_msk(struct KEY_DEF *msk,uint8_t *length, uint8_t *output);
int ksr_write_msk(struct KEY_DEF *msk,uint8_t length, uint8_t *input);
int ksr_write_wsk(struct KEY_DEF *wsk,uint8_t length, uint8_t *input);
int ksr_read_wsk(struct KEY_DEF *wsk,uint8_t *length, uint8_t *output);
int ksr_clear_sector(uint32_t start_addr,uint32_t end_addr);
int fast_ksr_write_key(uint appid,uint type);
#endif
#endif


