/***************** (h) COPYRIGHT 2016 START Computer equipment *****************
 * File Name          : pinpad_parser.h
 * bfief              : 
 * Author             : luocs 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 4/24/2016 1:53:38 PM
 * Description        : 
 *******************************************************************************/
#ifndef __PINPAD_PARSER_H__
#define __PINPAD_PARSER_H__ 


struct PED_SYS {
    uint8_t status;  //是否开启 0-空闲 1-开启,表示不同安全等级  FF-防拆触发不能使用密码键盘
    uint8_t curr_index_mk;//current MK index:0-无效
    uint8_t curr_index_wk;//current WK index:0-无效
    uint8_t curr_type_wk; //current WK type :0-无效
    uint8_t rand[24];     //过程密钥或随机数
};

typedef struct __OFFLINE_KEYINFO
{
	uint8_t keylen; //秘钥长度
    uint8_t indatalen;  //加密数据长度
    uint8_t keybuf[24]; //秘钥最大24
}_OFFLINE_KEYINFO;


/* 
 *    mode:0-解析工作密钥  1-解析主密钥
 *    00(2B)+Type(密钥类型,1B) + Index(密钥索引,1B) + KEKType(加密密钥类型,1B)+KEKIndex (加密密钥索引,1B)
 *    + EncMode(加密模式,1B) + CheckMode(校验模式,1B) +checklen(校验码长度,1B)+ KeyLen(密钥长度,1B) 
 *    + Key_E (密钥,变长) + Check(校验码，变长) +IV(初始变量，变长) 
 *    Encode加密模式:  WK 0-不允许 1-加密
 *                   非WK 0-明文   1-加密 
 *    CheckMode校验模式: 0-不校验  1-明文密钥对8字节0x00作3DES加密得出结果
 *    checklen校验码长度:  4-4字节 8-8字节
 * @ 
 */
struct PEDLoadKeyHead {
    uint16_t ver;
    uint8_t type;
    uint8_t index;
    uint8_t kektype;
    uint8_t kekindex;
    uint8_t algr;
    uint8_t encmode;
    uint8_t checkmode;
    uint8_t checklen;
    uint8_t ivlen;
    uint8_t keylen;
    uint8_t keyval[8];//minlen
};

struct PEDOnlinePinHead {
    uint8_t type;
    uint8_t index;
    uint8_t mode;
    uint8_t minlen;
    uint8_t maxlen;
    uint8_t timeout;
    uint8_t showlenflag;
    uint8_t beepflag;
    char    cardno[12];
    char* string;//minlen:字符格式
};

struct PEDOfflinePinHead {
    uint8_t mode;
    uint8_t minlen;
    uint8_t maxlen;
    uint8_t timeout;
    uint8_t showlenflag;
    uint8_t beepflag;
    uint16_t rfu;
    uint8_t encmode;
    char    cardno[12];
    char* string;//minlen:字符格式
};


#define PED_PIN_LEN_MIN    (uint8_t)0
#define PED_PIN_LEN_MAX    (uint8_t)14
#define PED_PIN_TOUT_MIN   (uint8_t)30
#define PED_PIN_TOUT_MAX   (uint8_t)120

struct PEDEncrDataHead {
    uint8_t type;
    uint8_t index;
    uint8_t algr;
    uint8_t scatter;
    uint8_t rfu[2];
    uint8_t len_l;
    uint8_t len_h;
    uint8_t val[8];//minlen
};

struct PEDCalcuMACHead {
    uint8_t type;
    uint8_t index;
    uint8_t algr;
    uint8_t scatter;
    uint8_t mode; //0-X99 1-ECB 2-X919 
    uint8_t flag; //1-还有后续块 0-最后1块，用于计算MAC的报文超过1024B时使用
    uint8_t rfu[3];
    uint8_t ivlen;//初始向量长度
    uint8_t len_l;
    uint8_t len_h;
    uint8_t val[8];//minlen
};


struct PEDSelectKeyCHead {
    uint8_t mode_l;
    uint8_t mode_h;
    uint8_t type;
    uint8_t index;
};


// key type
#define KEY_TYPE_SRK            0x00  ////Security Root Key(MMK)
#define KEY_TYPE_AUTHMK		    0x01
#define KEY_TYPE_AUTHPINK		0x02
#define KEY_TYPE_AUTHMACK		0x03
#define KEY_TYPE_AUTHDESK		0x04
#define KEY_TYPE_MK				0x05
#define KEY_TYPE_PINK			0x06
#define KEY_TYPE_MACK			0x07
#define KEY_TYPE_DESK			0x08
#define KEY_TYPE_TK             0x09   //传输密钥
#define KEY_TYPE_PROCESSKEY     0x0C   //过程密钥
#define KEY_TYPE_CURKEY         0x0D   //当前密钥
#define KEY_TYPE_RSAKEY         0x0E

enum PED_ALGR {
    PED_ALGR_DES_ENC = 1,
    PED_ALGR_DES_DEC,
    PED_ALGR_AES_ENC, 
    PED_ALGR_AES_DEC, 
    PED_ALGR_SM1_ENC, 
    PED_ALGR_SM1_DEC, 
    PED_ALGR_SM4_ENC, 
    PED_ALGR_SM4_DEC, 
    PED_ALGR_RSA = 0x20, 
    PED_ALGR_SM2 = 0x21, 
};



//Return Value:RTV
typedef enum
{
    RTV_PED_SUCCE = 0,
    RTV_PED_ERR,
    RTV_PED_ERR_CMD,
    RTV_PED_ERR_PARA,
    RTV_PED_ERR_MACK,
    RTV_PED_ERR_ALGR,
    RTV_PED_ERR_USAGE_FAULT,
    RTV_PED_ERR_KEYLEN,
    RTV_PED_ERR_KEYTYPE,
    RTV_PED_ERR_KEYINDEX,
    RTV_PED_ERR_KEYNULL,
    RTV_PED_ERR_KEYFLASH,
    RTV_PED_ERR_KEYCHECK,
    RTV_PED_ERR_INPUT_NONE,
    RTV_PED_ERR_INPUT_CANCEL,
    RTV_PED_ERR_INPUT_TIMEOUT,
}PED_RTV;

enum PED_CMD {
    PED_CMD_OPEN      = 1,
    PED_CMD_CLOSE     = 2, 
    PED_CMD_LOADMK    = 3, 
    PED_CMD_LOADWK    = 4, 
    PED_CMD_RANDOM    = 5, 
    PED_CMD_ONLINE    = 6, 
    PED_CMD_OFFLINE   = 7, 
    PED_CMD_ENCRDATA  = 8, 
    PED_CMD_MAC       = 9, 
    PED_CMD_SELECTKEY = 0x0A, 
    PED_CMD_AUTHKEY   = 0x0B, 
    PED_CMD_SHAKEHADN = 0x0D, 
    PED_CMD_EXTEND    = 0x12, 
};

typedef struct
{
	uchar frametype;
	uchar button;
    uchar titlealign;
    uchar contextalign;
    ushort rfu;
    ushort titlelen;
	uchar* title;
    ushort contextlen;
    uchar* context;
}s_lcd_emv;

int ped_init(uint32_t mode);
int ped_open(uint32_t mode);
int ped_close(uint32_t mode);
int ped_load_mk(uint32_t length,uint8_t *protocol);
int ped_load_wk(uint32_t length,uint8_t *protocol);
int ped_get_rand(uint32_t inlen,uint8_t *input,uint16_t *outlen,uint8_t *output);
int ped_get_pin_online(uint32_t inlen,uint8_t *packet,uint16_t *outlen,uint8_t *output);
int ped_get_pin_offline(uint32_t inlen,uint8_t *input,uint16_t *outlen,uint8_t *output);
int ped_get_encrpt_data(uint32_t inlen,uint8_t *packet,uint16_t *outlen,uint8_t *output);
int ped_get_mac(uint32_t inlen,uint8_t *packet,uint16_t *outlen,uint8_t *output);
int ped_select_key(uint32_t inlen, uint8_t *input);
int ped_extern_main(uint32_t inlen,uint8_t *input,uint16_t *outlen,uint8_t *output);

int pubk_get_blk_num(uint8_t index);
int ped_parse_cmd(uint32_t inlen,uint8_t *input,uint32_t *outlen,uint8_t *output);

#endif


