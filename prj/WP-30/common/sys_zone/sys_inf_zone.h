/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : sys_inf_zone.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 4/3/2014 7:01:13 PM
 * Description        : 
 *******************************************************************************/

#ifndef __SYS_INF_ZONE_H__
#define __SYS_INF_ZONE_H__

#include "map.h"

//安全认证
// FSK ID
enum {
    SAFE_CTRLFSK_ID  = 0,
    SAFE_MAPPFSK_ID,
    SAFE_RFU_ID
};
// AUTHKEY ID
enum {
    SAFE_FAC_AUTHKEY_ID  = 0,
    SAFE_RFU0_AUTHKEY_ID,
    SAFE_RFU1_AUTHKEY_ID
};

//sys_set_sysinfo sys_get_sysinfo sys_get_module_type 获取版本信息输入参数
#define     MODULE_PRINT_TYPE         0x01
#define     MODULE_LCD_TYPE           0x02
#define     MODULE_MAGNETIC_TYPE      0x03
#define     MODULE_MODEM_TYPE         0x04
#define     MODULE_GPRS_TYPE          0x05
#define     MODULE_CDMA_TYPE          0x06
#define     MODULE_BLUETOOTH_TYPE     0x07
#define     MODULE_WIFI_TYPE          0x08
#define     MODULE_LAN_TYPE           0x09
#define     MODULE_RF_TYPE            0x0A
#define     MODULE_COM1_TYPE          0x0B
#define     MODULE_COM2_TYPE          0x0C
#define     MODULE_INFR_TYPE          0x0D
#define     MODULE_ZIGBEE_TYPE        0x0E
#define     MODULE_PN512_TYPE         0x0F
#define     MODULE_BATTERY_TYPE       0x10
#define     MODULE_USBHOST_TYPE       0x11
#define     MODULE_NAND_TYPE          0x12
#define     MODULE_SCANNER_TYPE       0x13
#define     MODULE_S980_TYPE          0x14  //k_S980_type
#define     MODULE_CARDREADER_TYPE    0x15
#define     MODULE_COM3_TYPE          0x16
#define     MODULE_COM4_TYPE          0x17
#define     MODULE_IOM_TYPE           0x18
#define     MODULE_STANDBY_BAT_TYPE   0x19  //备用电源


#define     MODULE_MODE_PRODUCT       0x1000  //产品相关 4字节:产品主类型+子类型+版本+预留
#define     MODULE_MODE_TESTMODE      0x1001  //工作模式 0-正常模式 1-EMC测试 2-认证测试
#define     MODULE_MODE_LCD           0x1002  //液晶类型
#define     MODULE_MODE_SDCARD        0x1003  // SD卡
#define     MODULE_MODE_COMM          0x1004  //通信
#define     MODULE_MODE_SCANNER       0x1005  //扫描头
#define     MODULE_MODE_ICCARD        0x1006  //IC卡
#define     MODULE_MODE_MAGCARD       0x1007  //磁卡
#define     MODULE_MODE_RFID          0x1008  //射频
#define     MODULE_MODE_PRINTER       0x1009  //打印
#define     MODULE_MODE_FONT          0x100A  //字库类型
#define     MODULE_MODE_FACWORK       0x1010  //生产测试工单信息

// 以下只供监控调用
#define     SU_MODULE_MODE_PINPAD        0x2000
#define     SU_MODULE_MODE_SAFT          0x2001  //防拆状态 4B: 防拆设置(0-未开启 1-开启)+防拆状态(0-未触发 1-触发) + 2B(预留)
#define     SU_MODULE_MODE_APPAUTH       0x2002  //应用认证 4B: 状态(0-未开启 !0-开启) + 3B(预留)

// sys_get_module_type返回值

#define     MODULE_NOTEXIST             0x000
#define     MODULE_PRINT_FUJITSU        0x010
#define     MODULE_PRINT_SEIKO          0x011
#define     MODULE_PRINT_OTHER          0x01F
#define     MODULE_LCD_TRULYSEMI        0x020
#define     MODULE_LCD_GOWORLD          0x021
#define     MODULE_LCD_ICON             0x022
#define     MODULE_LCD_NOICON           0x023
#define     MODULE_LCD_LIERDA_TFT       0x024
#define     MODULE_LCD_TOUCH_TFT        0x025
#define     MODULE_LCD_OTHER            0x02F
#define     MODULE_MAGNETIC_MAGTEK      0x030
#define     MODULE_MAGNETIC_IDTECH      0x031
#define     MODULE_MAGNETIC_MT318       0x032
#define     MODULE_MAGNETIC_HCM4003     0x033
#define     MODULE_MAGNETIC_MTK211      0x034
#define     MODULE_MAGNETIC_OHTER       0x03F
#define     MODULE_MODEM_CX_MODEM       0x040
#define     MODULE_MODEM_SI_MODEM       0x041
#define     MODULE_MODEM_OTHER          0x04F
#define     MODULE_GPRS_EM310           0x050
#define     MODULE_GPRS_GC864           0x051
#define     MODULE_GPRS_GSM0306         0x052
#define     MODULE_GPRS_MG323           0x053
#define     MODULE_CDMA_EM200           0x060
#define     MODULE_CDMA_MC323           0x061
#define     MODULE_CDMA_CC864           0x062
#define     MODULE_BLUETOOTH_AUBTM      0x070
#define     MODULE_BLUETOOTH_BLK        0x071
#define     MODULE_BLUETOOTH_BM77SPP    0x072
#define     MODULE_WIFI_OTHER           0x08F
#define     MODULE_LAN_OTHER            0x09F
#define     MODULE_RF_RC531             0x0A0
#define     MODULE_RF_RC663             0x0A1
#define     MODULE_RF_PN512_BASE        0x0A2
#define     MODULE_RF_PN512_ENLARGE     0x0A3
#define     MODULE_RF_AS3911            0x0A4
#define     MODULE_RF_AS3910            0x0A5
#define     MODULE_RF_FM17550           0x0A6
#define     MODULE_COM1                 0x0B0
#define     MODULE_COM2                 0x0C0
#define     MODULE_INFR                 0x0D0
#define     MODULE_ZIGBEE               0x0E0
#define     MODULE_PN512                0x0F0
#define     MODULE_CPUCARD_IOREV        0x0F1 //io反向
#define     MODULE_CPUCARD_IO           0x0F2 //io直连
#define     MODULE_CPUCARD_8035         0x0F3
#define     MODULE_CPUCARD_8113         0x0F4
#define     MODULE_CPUCARD_6001         0x0F5
#define     MODULE_BATTERY_1LI          0x100
#define     MODULE_BATTERY_2LI          0x101
#define     MODULE_USBHOST              0x110
#define     MODULE_NAND                 0x120
#define     MODULE_SCANNER              0x130
#define     MODULE_SCANNER_EM1300       MODULE_SCANNER  //一维码 EM_1300
#define     MODULE_SCANNER_5X1080       0x131  //二维码 5X10-80
#define     MODULE_SCANNER_EM3000	    0x132  //二维码 EM_3000
#define     MODULE_COM3                 0x140
#define     MODULE_COM4                 0x150
#define     MODULE_IOM                  0x160
#define     MODULE_STANDBY_BATTERY      0x170  //备用电源


#define     MODULE_GPRS_MASK       (1 << 9)
#define     MODULE_CDMA_MASK       (1 << 8)
#define     MODULE_BLUETOOTH_AUBTM_MASK  (1 << 7)
#define     MODULE_WIFI_OTHER_MASK       (1 << 6)

// 射频相关  4*5=20B
typedef struct __MODULE_RFID_INFO {
    uchar type;                  // FF-不支持 0- 1-3911 2-FM17550 3-PN512
//    uchar rfu[3];
    uchar gTypeBmodule;  //type b调制深度
    uchar gTypeArec;     //type a接收灵敏度
    uchar gTypeBrec;     //type b接收灵敏度
    uint rfid_powerfield_time;   // 场强建立延时时间 (4B)
    uint field_strength;         // 场强大小
    uint  irfu[2];
}MODULE_RFID_INFO;				/* ----------  end of struct MODULE_RFID_INFO  ---------- */

//蓝牙相关
typedef struct __MODULE_BLUET_INFO {
    uchar bluetooth;             // FF-不支持 0-不支持 1-BM77
    uchar rfu[3];
}MODULE_BLUET_INFO;				/* ----------  end of struct MODULE_BLUET_INFO  ---------- */

// 磁卡相关
typedef struct __MODULE_MAGCARD_INFO {
    uchar type;             // 磁头类型:FF-默认3 0-不支持 1-IDTECH 2-MTK211 3-MAGTEK 4-HCM4003
    uchar trkflag;          // 支持磁道数  0-3个磁道 1-2、3磁道
    uchar rfu[2];
}MODULE_MAGCARD_INFO;				/* ----------  end of struct MODULE_MAGCARD_INFO  ---------- */

//cpu卡相关
typedef struct __MODULE_ICCARD_INFO {
    uchar type;            // IC卡控制芯片类型 0-不支持 1-反向 2-cpu口线 3-8035 4-8113 5-6001
    uchar excardreader;    // 是否支持外置读卡器
    uchar slots;            // 总卡座数
    uchar insamslots;       // 内置SAM卡座
    uchar exmaxslots;       // 外置大卡座数
    uchar exsamslots;       // 外置SAM卡座数
    uchar rfu[2];
}MODULE_ICCARD_INFO;				/* ----------  end of struct MODULE_ICCARD_INFO  ---------- */

// 系统参数相关  32B
typedef struct __MODULE_SYS_INFO {
    uchar menupassword[16];  // 菜单密码
    uchar lcdcontrast;       //液晶对比读
    uchar lcdlighttime;      //背光时间
    uchar keytone;           //按键伴音
    uchar language;          //系统语言  0-英文 1-中文
    uint  irfu[3];
}MODULE_SYS_INFO;				/* ----------  end of struct MODULE_SYS_INFO  ---------- */

// 生产相关信息
// 20+12+8+4+16=60B
typedef struct _FAC_TESTINFO_{
    uchar mbno[20];   //主板ID 
    uchar voucherno[12];// 传票号
    uchar phase;      //当前测试阶段
    uchar fac_ctrl_flag; //管控是否成功标识
    uchar rfu[6];
    // D0~D7  D0:modem D1:WIRELESS D2:BT D3:ETH D4:SCANNER D5:INFR
    uchar module_res[4]; //模块测试结果 1-成功 0-失败
    // D0~D29:各个单项测试内容  1-成功 0-失败
    // D30~D31:03-强制成功  02-失败 01-正常成功 00-未测试
    uint auto_res[4];    //自动测试结果
}FAC_TESTINFO;

// 6+11*2=
typedef struct
{
	uchar  aging_allok;	//老化测试结束,(如果是WP50或S980还有最后的充电流程未进行)
	uchar  fail_step;//测试失败的环节
	uchar  rfu[4];
	//battery_error_info电池老化错误信息:各bit为0时表示正常
	//D0 - 1:拔适配器时电压未达到规定值     charge_error  
	//D1 - 1:电池老化结束时电压比预设值低   discharge_error
	//D2 - 1:充电时长不够,适配器却被拔除,记录适配器掉电 need_charge
	//D3 - 1:最后充电阶段如果充满电后（适配器未拔，电池未拔出），10分钟内又自动启动充电，判电池不良 recharge_error
	//D4 - 1:最后充电未完成(充电时间不够或者没充满) recharge_incomplete
	//D5 ~ D15:预留
	ushort battery_error_info;	
	ushort totaltimes;//一共测试了x轮
	ushort error_timer;//在第x轮测试失败
	ushort prn_count;//打印测试次数
	ushort baterry_mv1;//拔适配器时电池电压(mv)
	ushort baterry_mv2;//放电结束时电池电压(mv)	
	ushort total_time;//老化测试总时间(分钟)
	ushort charge_time;//总充电时间
	ushort discharge_time;//放电时间
	ushort baterry_mv_actual;//实时检测电压
	ushort recharge_time;//第二次充电时长
}s_aging_log;

// 硬件版本信息
typedef struct
{
    unsigned char flag; // FF-无效  00-AD采样实际值 0x01：手动配置值
    unsigned char main;
    unsigned char secondary;
    unsigned char revise;
} hardware_verion_t;

// 软件版本信息
typedef struct
{
    unsigned char main;
    unsigned char secondary;
    unsigned char revise;
    unsigned char reserve;
} version_t;

typedef struct
{
    uchar lcd;
    uchar rfid;
    uchar printer;
    uchar iccard;
    uchar gprs;
    uchar cdma;
    uchar bluetooth;
    uchar modem;
    uchar wifi;
    uchar ethernet;
    uchar scanner;
    uchar magcard;
    uchar sd;
    uchar pinpad;
    uchar battery;
    uchar com1;
    uchar com2;
    uchar com3;
    uchar com4;
    uchar infr;
    uchar usbhost;
    uchar nandflash;
    uchar zigbee;
    uchar standbybat;//备用电池
    uchar wlm_flag;//初次采样只进行无线有无判断，不进行具体模块识别 1-有 0xFF-无
    uchar reserve[7];
} module_type_t;

typedef struct _SYSZONE_DEF
{
    uint8_t appcheck[8];    //0  "STAR"+4 app_length(4B)
    uint8_t boot_info[16];  //8  "WP-30BOOT"
    uint8_t boot_sha2[32];  //24
    uint8_t boot_mac[4];    //56
    uint32_t first_run;     //60 /*CTRL第一次运行，用于初始化系统信息区*/
    uint32_t app_enable;    //64 /*CTRL使能标志，用在生产测试完成后*/
    uint32_t app_update;    //68 /*应用每次更新标志:用作应用第一次运行*/
    uint32_t bps;           //72
    uint32_t beep;          //76
    uint32_t lcd_contract;  //80
    uint32_t attack_flag;   //84
    uint8_t superpsw[8];    //88
    uint8_t adminApsw[8];   //96
    uint8_t adminBpsw[8];   //104 
    uint8_t sn_dev[32];     //112 
    uint8_t fsk_dev[24];    //144 
    uint8_t fsk_host[24];   //168
    MODULE_RFID_INFO trfidinfo; //192  sizeof(MODULE_RFID_INFO)= 20 
	MODULE_SYS_INFO tsysinfo;   //212  sizeof(MODULE_SYS_INFO) = 32
    FAC_TESTINFO tTestInfo;     //244  sizeof(FAC_TESTINFO)    = 60
    hardware_verion_t main_version; //304  sizeof(hardware_verion_t) = 4
    MODULE_BLUET_INFO bluetoothinfo;//308 sizeof(MODULE_BLUET_INFO) = 4
    MODULE_MAGCARD_INFO tmagcardinfo;//312 sizeof(MODULE_MAGCARD_INFO) = 4
    MODULE_ICCARD_INFO ticcardinfo; //316 sizeof(MODULE_ICCARD_INFO) = 8
    uint32_t secu_leve;         //324 
    uint32_t tamper[4];         //328
    uint8_t exflash_key[8];     //344 
    uint8_t project_id[32];     //352
    uint32_t gapksha_num;       //384 保存在外置flash的gapk sha的组数 
    uint8_t secure_random[24];  //388  生产3组8字节随机数据 
    uint8_t rfu1[2048+1632];    //412  | 2044-412 = 1632
    uint32_t syszone_check;     //2048+2044
}SYSZONE_DEF; /*修改SYSZONE_DEF结构务必确保整个结构大小为2048字节,注意字节对齐*/

//*******************************************************
//  syszone 分配  1024Byte
//*******************************************************
#define OFFSET_APP_CHECK         (FPOS(SYSZONE_DEF,appcheck))   //应用程序标志  8B
	#define LEN_APP_CHECK           (FSIZE(SYSZONE_DEF,appcheck))  //“STAR”+ 4字节应用程序代码长度
#define OFFSET_BSV              (OFFSET_APP_CHECK+LEN_APP_CHECK)  //52B
	#define OFFSET_BSV_BOOTINFO  0   //boot版本信息"WP-30BOOT"
	#define LEN_BSV_BOOTINFO        (FSIZE(SYSZONE_DEF,boot_info))
	#define OFFSET_BSV_SHA      (OFFSET_BSV_BOOTINFO+LEN_BSV_BOOTINFO) //SHA2校验码
	#define LEN_BSV_SHA             (FSIZE(SYSZONE_DEF,boot_sha2))
	#define OFFSET_BSV_MAC      (OFFSET_BSV_SHA+LEN_BSV_SHA)
	#define LEN_BSV_MAC             (FSIZE(SYSZONE_DEF,boot_mac))
	#define LEN_BSV                 (LEN_BSV_BOOTINFO+LEN_BSV_SHA+LEN_BSV_MAC) //
	
#define OFFSET_FIRST_FLAG       (FPOS(SYSZONE_DEF,first_run)) //第一次运行标志 
	#define LEN_FIRST_FLAG           (FSIZE(SYSZONE_DEF,first_run))  //为0xFF为第一次运行，设置WP-30”后为非第一次运行
#define OFFSET_APPENABLE_FLAG   (FPOS(SYSZONE_DEF,app_enable))  //启用应用标志  0xFFFFFFFF  0x55AA-写入掉电标志  0-启用
	#define LEN_APPENABLE_FLAG       (FSIZE(SYSZONE_DEF,app_enable))
#define OFFSET_APPUPDATE_FLAG   (FPOS(SYSZONE_DEF,app_update))  //应用每次更新标志:用作应用第一次运行
	#define LEN_APPUPDATE_FLAG       (FSIZE(SYSZONE_DEF,app_update))
#define OFFSET_BPS              (FPOS(SYSZONE_DEF,bps))           //波特率 预留“115200”
	#define LEN_BPS                  (FSIZE(SYSZONE_DEF,bps))
#define OFFSET_BEEP             (FPOS(SYSZONE_DEF,beep)) //蜂鸣器开关（预留）
	#define LEN_BEEP                 (FSIZE(SYSZONE_DEF,beep))
#define OFFSET_LCD              (FPOS(SYSZONE_DEF,lcd_contract))//对比度（预留）
	#define LEN_LCD                  (FSIZE(SYSZONE_DEF,lcd_contract))
#define OFFSET_ATTACKFLG        (FPOS(SYSZONE_DEF,attack_flag))  //
	#define LEN_ATTACKFLG            (FSIZE(SYSZONE_DEF,attack_flag))
#define OFFSET_SECURITY_LEVELS  (FPOS(SYSZONE_DEF,secu_leve))  //安全等级
	#define LEN_SECURITY_LEVELS       (FSIZE(SYSZONE_DEF,secu_leve))
#define SYS_TAMPER_EVENT_ADDR    (FPOS(SYSZONE_DEF,tamper))     // tamper reg
    #define SYS_TAMPER_EVENT_SIZE    (FSIZE(SYSZONE_DEF,tamper))

#define SYS_SUPER_PSW_ADDR        (FPOS(SYSZONE_DEF,superpsw))
#define SYS_SUPER_PSW_SIZE        (FSIZE(SYSZONE_DEF,superpsw))
#define OFFSET_PSW       SYS_SUPER_PSW_ADDR
#define LEN_PSW          SYS_SUPER_PSW_SIZE

#define SYS_ADMINA_PSW_ADDR       (FPOS(SYSZONE_DEF,adminApsw))
#define SYS_ADMINA_PSW_SIZE       (FSIZE(SYSZONE_DEF,adminApsw))

#define SYS_ADMINB_PSW_ADDR       (FPOS(SYSZONE_DEF,adminBpsw))
#define SYS_ADMINB_PSW_SIZE       (FSIZE(SYSZONE_DEF,adminBpsw))

#define SYS_SN_DEV_ADDR         (FPOS(SYSZONE_DEF,sn_dev))
#define SYS_SN_DEV_SIZE         (FSIZE(SYSZONE_DEF,sn_dev))
#define OFFSET_SN               SYS_SN_DEV_ADDR 
	#define LEN_SN                   SYS_SN_DEV_SIZE

#define OFFSET_FSK_SM    SYS_SN_DEV_ADDR  

#define SYS_FSK_host_ADDR         (FPOS(SYSZONE_DEF,fsk_host))
#define SYS_FSK_host_SIZE         (FSIZE(SYSZONE_DEF,fsk_host))
#define OFFSET_FSK_CTRL  SYS_FSK_host_ADDR  
#if 0
#define OFFSET_FSK_SM           (OFFSET_PSW+SYSPSWINFO_LEN*3)   //20B      //fixpsw encrypt 
#define OFFSET_FSK_CTRL         (OFFSET_FSK_SM+FSKINFO_LEN)     //20B
#define OFFSET_APP_ZONE         (OFFSET_FSK_CTRL+FSKINFO_LEN)   //
	#define LEN_APP_ZONE            (APPINFO_LEN*(NUM_APP+1))   //48*5=240
#define OFFSET_FSK_APP_ZONE     (OFFSET_APP_ZONE+LEN_APP_ZONE)
    #define LEN_FSK_APP_ZONE        (FSKINFO_LEN*(NUM_APP+1))   //20*5=100
#define OFFSET_TRANSFERKEY_ZONE (OFFSET_FSK_APP_ZONE+LEN_FSK_APP_ZONE)	
    #define LEN_TRANSFERKEY_UNIT    (24)
	#define LEN_TRANSFERKEY         (LEN_TRANSFERKEY_UNIT*(NUM_APP+1)) //24*5=120
#endif
#define SYS_RFIDINFO_ADDR         (FPOS(SYSZONE_DEF,trfidinfo))
#define SYS_RFIDINFO_SIZE         (FSIZE(SYSZONE_DEF,trfidinfo))

#define SYS_MAINVERSION_ADDR         (FPOS(SYSZONE_DEF,main_version))
#define SYS_MAINVERSION_SIZE         (FSIZE(SYSZONE_DEF,main_version))

#define OFFSET_Reserved1          (FPOS(SYSZONE_DEF,rfu1))                              //剩下 228B

#define SYS_EXFLASH_KEY_ADDR      (FPOS(SYSZONE_DEF,exflash_key))
#define SYS_EXFLASH_KEY_SIZE        (FSIZE(SYSZONE_DEF,exflash_key))

#define SYS_PROJIECT_ID_ADDR      (FPOS(SYSZONE_DEF,project_id))
#define SYS_PROJIECT_ID_SIZE        (FSIZE(SYSZONE_DEF,project_id))

#define SYS_GAPKSHA_NUM_ADDR      (FPOS(SYSZONE_DEF,gapksha_num))
#define SYS_GPAKSHA_NUM_SIZE        (FSIZE(SYSZONE_DEF,gapksha_num))

#define SYS_SECURITY_RANDOM_ADDR  (FPOS(SYSZONE_DEF,secure_random))
#define SYS_SECURITY_RANDOM_SIZE    (FSIZE(SYSZONE_DEF,secure_random))

#define OFFSET_SYSZONE_CHECK      (FPOS(SYSZONE_DEF,syszone_check))
	#define LEN_SYSZONE_CHECK        (FSIZE(SYSZONE_DEF,syszone_check))
#define OFFSET_SYSZONE_END        (OFFSET_SYSZONE_CHECK + LEN_SYSZONE_CHECK) 	//1024B

//*******************************************************
// 
//  Appsyszone 分配在App代码最后288B SHA校验范围:源代码+版本信息(16B) 偏移为负
// APP_INFO(16B) + APP_CHECK(256) + CHECK_FLAG(16B)
// APP_CHECK:APP_SHA(32B) + APP_MAC(4B) + 预留(220)
// 
//*******************************************************
#if 0
#define OFFSET_APPSYS_APPINFO    288
	#define LEN_APPSYS_APPINFO     16
#define OFFSET_APPSYS_SHA        (OFFSET_APPSYS_APPINFO-LEN_APPSYS_APPINFO)
	#define LEN_APPSYS_SHA         32
#define OFFSET_APPSYS_MAC        (OFFSET_APPSYS_SHA-LEN_APPSYS_SHA)
	#define LEN_APPSYS_MAC         4
#define OFFSET_APPSYS_RESERVED   (OFFSET_APPSYS_MAC-LEN_APPSYS_MAC)
	#define LEN_APPSYS_RESERVED    220
#define OFFSET_APPSYS_SHAFLG     (OFFSET_APPSYS_SHA-LEN_APPSYS_SHA)
	#define LEN_APPSYS_SHAFLG      16
#endif

//*******************************************************
// 
//   APPINFO offset
// 
//*******************************************************
//#define SA_APPINFO(appno)      (OFFSET_APP_ZONE+(appno-1)*APPINFO_LEN)

/*-----------------------------------------------------------------------------}
 * 系统信息区常用标志宏
 *-----------------------------------------------------------------------------{*/
#define SYSZONE_APPUPDATE_FLAG     (uint32_t)0xAA55A5A5


/*-----------------------------------------------------------------------------}
 *  默认配置
 *-----------------------------------------------------------------------------{*/
#define DEFAULT_MENU_PASSWORD  "000000" //菜单密码 
//默认系统密码
#define DEFAULT_SUPERPSW_SYS    "20112010"  
#define DEFAULT_ADMINAPSW_SYS   "00000000"  
#define DEFAULT_ADMINBPSW_SYS   "00000000"  
//默认安全模块FSK
#define DEFAULT_FSK_SM     "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"


/*-----------------------------------------------------------------------------}
 * 产品版本 
 *-----------------------------------------------------------------------------{*/
#define     READ_PRODUCT_VER  0x00   //产品
#define     READ_MAINB_VER    0x01
#define     READ_MAINBID_VER  0x02   //主板ID
#define     READ_BOOT_VER     0x03   //BOOT
#define     READ_CTRL_VER     0x04   //CTRL
#define     READ_CTRLTIME_VER 0x05   //字库
#define     READ_FONT_VER     0x06   //字库
#define     READ_SN_VER       0x07
/*-----------------------------------------------------------------------------}
 * 全局变量 
 *-----------------------------------------------------------------------------{*/
extern const SYSZONE_DEF * const gpSys; 
extern version_t k_MainVer;

//int sys_zone_read(uint32_t addr,uint32_t length, uint8_t *output);
//int sys_zone_write(uint32_t addr,uint32_t length,const uint8_t *input);
//int sys_zone_recover(void);
//int sys_zoneback_recover(void);
//int sys_first_run(void);

inline void readFlashPage(uint32_t pageAddr, uint8_t *data, uint16_t len)
{
    memcpy(data,(char *)(pageAddr*2),len);
}
inline void readFlash(uint32_t pageAddr, uint32_t offset, uint8_t * data, uint16_t len)
{
    memcpy(data,(uint8_t *)(pageAddr+offset),len);
}
inline int eraseFlashPage(int pageAddress)
{
    return (int)drv_flash_SectorErase(pageAddress*2);
}
inline int writeFlashPage(uint32_t pageAddr, uint8_t *data, uint16_t len)
{
    return (int)drv_flash_write(pageAddr*2,(uint32_t *)data,(uint)len);
}

extern const char * const k_TermName;

uint s_read_syszone(uint offset, uint len, uchar *ucBuf);
uint s_write_syszone(uchar *backbuf, uint uiOffset, uint uiLen, uchar *ucBuf);
uint save_syszone_back(uchar *buf);
uint sys_start(void);
uint read_syspsw(uint mode, uint *len, uchar *psw);
uint write_syspsw(uint mode, uint len, uchar *psw);
uint PCI_GetHSK(uint mode, uchar *hsk);
int s_sysinfo_rfidinfo (uint mode, void *p);
int s_sysinfo_bluetoothinfo (uint mode, void *p);
int s_sysinfo_magcardinfo (uint mode, void *p);
int s_sysinfo_iccardinfo(uint mode, void *p);
int s_sysinfo_sysinfo (uint mode, void *p);
int s_sysinfo_mbVersioninfo(uint mode, void *p);
int s_sysinfo_gpakshainfo(uint mode, uint32_t *p);
int sys_get_module_type(int module);
int s_sysinfo_security_random(uint mode, uint index, char *p);
int32_t  s_sysconfig_read(int offset,int readlen,void *vbuf);
int32_t  s_sysconfig_write(int offset,int writelen,void *vbuf);
uint check_syszone(uint type, uchar *buf);
uint resume_syszone(uchar *buf);
int sys_read_ver(int module, char *ver);
int sys_get_ctrllen (int mode);


#endif

