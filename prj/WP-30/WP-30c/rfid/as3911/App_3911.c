/*********************************************************************
* 版权所有 (C)2009,
* 文件名称：
*     F05_2监控层应用程序
* 当前版本：
*     1.0
* 内容摘要：
*
* 历史纪录：
*     修改人		 日期		    	 版本号       修改记录
*******************************************************************************/
#include "wp30_ctrl.h"

#define __APP_3911_C__
#ifdef  __APP_3911_C__

#ifdef EM_AS3911_Module
#if defined(EMV_AS3911)
/**********************************************************************
*
*
*  外部函数和外部变量声明
*
*
***********************************************************************/
void test_as3911(void);
void  Read3911_new(uint len, uchar *ucBuf);
int32_t Write3911_new(uint uiLen, uchar *ucBuf);
void vDispBufKey(uint uiLen, uchar *ucBuf);
 
/**********************************************************************
*
*
*   调试宏定义
*
*
***********************************************************************/
#define USE_LOGGER
#ifdef USE_LOGGER
#define LOG(...)    Dprintk(__VA_ARGS__)
#else
#define LOG(...)    {}
#endif

/**********************************************************************
*
*
*   宏定义
*
*
***********************************************************************/
#define APPL_COM_EMV_TOGGLE_CARRIER 0xE0 /*!< EMV TTA L1 carrier button pressed. */
#define APPL_COM_EMV_POLL           0xE1 /*!< EMV TTA L1 poll button prossed. */
#define APPL_COM_EMV_RESET          0xE2 /*!< EMV TTA L1 reset button presed. */
#define APPL_COM_EMV_WUPA           0xE3 /*!< EMV TTA L1 WUPA button pressed. */
#define APPL_COM_EMV_WUPB           0xE4 /*!< EMV TTA L1 WUPB button pressed. */
#define APPL_COM_EMV_RATS           0xE5 /*!< EMV TTA L1 RATS button pressed. */
#define APPL_COM_EMV_ATTRIB         0xE6 /*!< EMV TTA L1 ATTRIB button pressed. */
#define APPL_COM_EMV_PREVALIDATION  0xE7 /*!< EMV TTA L1 prevalidation test start button pressed. */
#define APPL_COM_EMV_DIGITAL        0xE8 /*!< EMV TTA L1 digital test start button pressed. */
#define APPL_COM_EMV_STOP           0xEA /*!< EMV TTA L1 prevalidation or digital test application stop button pressed. */
#define APPL_COM_EMV_INIT           0xEF /*!< EMV mode initialization command. */
#define APPL_COM_REG_DUMP           0xF0 /*!< Dump register content. */
#define APPL_COM_REG_WRITE          0xF1 /*!< Write register. */
#define APPL_COM_REG_READ           0xF2 /*!< Read register. */
#define APPL_COM_DIRECT_COMMAND		0xF3 /*!< Execute an AS3911 direct command. */


/*! SPI device ID of the AS3911. */
#define SPI_DEVICE_ID_AS3911 (0)
/*! Maximum size of the user supplied modulation level table. */
#define MAX_MODULATION_TABLE_SIZE 10
/*! Maximum size of the user supplied gain table. */
#define MAX_GAIN_TABLE_SIZE 10
/**********************************************************************
*
*
*全局变量声明:
*
*
***********************************************************************/
static u8 mainModulationTableX[MAX_MODULATION_TABLE_SIZE];
static u8 mainModulationTableY[MAX_MODULATION_TABLE_SIZE];
//static AS3911ModulationLevelTable_t mainModulationTable = {
AS3911ModulationLevelTable_t mainModulationTable = {
	0,
	&mainModulationTableX[0],
	&mainModulationTableY[0]
	};

static AS3911ModulationLevelAutomaticAdjustmentData_t mainModulationAutomaticAdjustmentData;

static u8 mainGainTableX[MAX_GAIN_TABLE_SIZE];
static u8 mainGainTableY[MAX_GAIN_TABLE_SIZE];
static AS3911GainTable_t mainGainTable = {
	0,
	&mainGainTableX[0],
	&mainGainTableY[0]
	};

/*!
 *****************************************************************************
 * Indicates that a start of one of the EMV test applications has been
 * requested by the GUI. Possible Value:
 *
 * 0:                         No test application start requested.
 * AMS_COM_EMV_DIGITAL:       Digital test application start requested.
 * AMS_COM_EMV_PREVALIDATION: Prevalidation test application start requested.
 *****************************************************************************
 */
static u8 emvGuiTestApplicationRequested = 0;

/*
******************************************************************************
* GLOBAL VARIABLES
******************************************************************************
*/
int IRQ_COUNT;

typedef struct
{
    uchar loadflg;      //是否有效数据标志 0-无效 1-有效
    uchar reg2Ah;       //寄存器2ah
    uchar reg27h;       //寄存器27h
    uchar reg21h;       //寄存器21h
    uchar reg02h;       //寄存器02h
    uchar reg01h;       //寄存器01h
    uchar reg0Ah;       //寄存器0ah
    uchar reg0Bh;       //寄存器0bh
    uchar reg0Ch;       //寄存器0ch
    uchar reg0Dh;       //寄存器0dh
    uchar regtable[26];
} RFID_EMV_REGCONFIG;

RFID_EMV_REGCONFIG gas3911paramtable[4];

const uchar gas3911paramdemo[] = {
    APPL_COM_EMV_INIT,          /*初始化参数*/
    0x0f,                       /*bit4 设置01h bit7 sup3V*/
                                /*bit3-0 设置01h bit6 vspd_off    2Ah bit6-3 rege3-0*/
    0x02,                       /*bit3-0 设置21h bit6-3 tre3-0*/
    0x04,                       /*bit2-1 设置02h bit5-4 rx_chn rx_man  bit0 设置0Ah bit7 ch_sel*/
    0x07,                       /*bit2-0 设置0ch bit7-5 rg1_am2-0*/
    0x00,                       /*bit2-0 设置0ch bit4-2 rg1_pm2-0*/
    0x00,                       /*bit3-0 设置0dh bit7-4 rg2_am3-0*/
    0x00,                       /*bit3-0 设置0dh bit3-0 rg2_pm3-0*/
    0x00,                       /*bit3-0 WriteTestRegister*/
    0x12,                       /*bit7-0 设置0bh bit7-0*/
    0x00,                       /*gain mode 0-不调整 1-根据RF振幅(gain表) 2-根据RFI和RFO(无效)*/
    0x00,                       /*gain table  gain表数据组个数*/
    0x02,                       /*type b 调制模式 0-不调整 1-自动调整 2-根据RF振幅(lookup table) 3-根据RFI和RFO(无效)*/

    0x06,                       /*type b 模式表数据组个数(这里是根据RF振幅调制 6组12个)*/
    0x66,                       /*第1组 x值*/
    0x74,                       /*第1组 y值*/
    0x9a,                       /*第2组 x值*/
    0x80,                       /*第2组 y值*/
    0xbd,                       /*第3组 x值*/
    0x88,                       /*第3组 y值*/
    0xcc,                       /*第4组 x值*/
    0x8e,                       /*第4组 y值*/
    0xd3,                       /*第5组 x值*/
    0x8e,                       /*第5组 y值*/
    0x5c,                       /*第6组 x值*/
    0x7c                        /*第6组 y值*/
};
/**********************************************************************
*    功能描述:
*
*
***********************************************************************/




/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void displayRegisterValue(u8 address)
{
    u8 value = 0;
    SETSIGNAL_H();
    as3911ReadRegister(address, &value);
    SETSIGNAL_L();
    LOG("REG: 0x%02X: 0x%02X %03d\r\n", address, value, value);
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void displayTestRegisterValue(u8 address)
{
    u8 value = 0;
    as3911ReadTestRegister(address, &value);
    LOG("Test REG: 0x%02hhx: 0x%02hhx\r\n", address, value);
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
u8 applProcessCmd (u8 const * rxData, const u8 rxSize, u8 * txData, u8 * txSize)
{
    s8 retVal = ERR_NONE;
	const u8 *rxByte;
	u8 modulationDepthMode = 0;
	u8 gainMode = 0;
	int index = 0;

	*txSize = 0;
    switch (rxData[0])
    {
    case APPL_COM_EMV_TOGGLE_CARRIER:
        emvGuiToggleCarrier();
        break;
    case APPL_COM_EMV_POLL:
        emvGuiPoll();
        break;
    case APPL_COM_EMV_RESET:
        emvGuiReset();
        break;
    case APPL_COM_EMV_WUPA:
        emvGuiWupa();
        break;
    case APPL_COM_EMV_WUPB:
        emvGuiWupb();
        break;
    case APPL_COM_EMV_RATS:
        emvGuiRats();
        break;
    case APPL_COM_EMV_ATTRIB:
        emvGuiAttrib();
        break;
    case APPL_COM_EMV_PREVALIDATION:
        emvGuiTestApplicationRequested = APPL_COM_EMV_PREVALIDATION;
        break;
    case APPL_COM_EMV_DIGITAL:
        emvGuiTestApplicationRequested = APPL_COM_EMV_DIGITAL;
        break;
    case APPL_COM_EMV_STOP:
        emvGuiStop();
        break;
    case APPL_COM_EMV_INIT:
        /* EMV Mode initialization command. */
		LOG("\r\nEMV: analog settings: ");
		for (index = 0; index < rxSize; index++)
			LOG("%02hhx ", rxData[index]);
		LOG("\r\n");

		/* Voltage Regulator setup. */
        as3911ModifyRegister(AS3911_REG_IO_CONF2, 0x80, (rxData[1] & 0x10) << 3);
        if (0x00 == (rxData[1] & 0x0F))
        {
            /* Disable the voltage regulator. */
            as3911ModifyRegister(AS3911_REG_IO_CONF2, 0x40, 0x40);
        }
        else
        {
            /* Enable the voltage regulator. */
            as3911ModifyRegister(AS3911_REG_IO_CONF2, 0x40, 0x00);
            as3911ModifyRegister(AS3911_REG_REGULATOR_CONTROL, 0xF8, 0x80 | ((rxData[1] & 0x0F) << 3));
        }

        /* Antenna trim setup. */
        as3911ModifyRegister(AS3911_REG_ANT_CAL_CONTROL, 0xF8, 0x80 | ((rxData[2] & 0x0F) << 3));

        /* Receive channel setup. */
        as3911ModifyRegister(AS3911_REG_OP_CONTROL, 0x30, ((rxData[3] & 0x06) << 3));
        as3911ModifyRegister(AS3911_REG_RX_CONF1, 0x80, ((rxData[3] & 0x01) << 7));

        /* First stage gain reduction. */
        as3911ModifyRegister(AS3911_REG_RX_CONF3, 0xFC, ((rxData[4] & 0x07) << 5) | ((rxData[5] & 0x07) << 2));

        /* Second/Third stage gain reduction. */
        as3911WriteRegister(AS3911_REG_RX_CONF4, ((rxData[6] & 0x0F) << 4) | (rxData[7] & 0x0F));
        as3911ExecuteCommand(AS3911_CMD_CLEAR_SQUELCH);

        /* Test output. */
        as3911WriteTestRegister(AS3911_REG_ANALOG_TEST, rxData[8] & 0x0F);

		/* Automatic gain control and squelch. */
		as3911ModifyRegister(AS3911_REG_RX_CONF2, 0x1F, rxData[9]);

		/* Gain adjustment based on lookup table. */
		/* Readout gain lookup table. */
		rxByte = &rxData[10];
		gainMode = *rxByte++;
		mainGainTable.length = *rxByte++;
		for (index = 0; index < mainGainTable.length; index++)
		{
			mainGainTableX[index] = *rxByte++;
			mainGainTableY[index] = *rxByte++;
		}
		LOG("EMV: gain reduction table length %hhd\r\n", mainGainTable.length);
		for (index = 0; index < mainGainTable.length; index++)
			LOG("EMV: gainTable[%d] = 0x%hhx, 0x%hhx\r\n", index, mainGainTable.x[index], mainGainTable.y[index]);

		if (0x00 == gainMode)
		{
			as3911SetGainMode(AS3911_GAIN_FIXED, NULL);
			LOG("EMV: using fixed gain reduction\r\n");
		}
		else if (0x01 == gainMode)
		{
//			int index;

			as3911SetGainMode(AS3911_GAIN_FROM_AMPLITUDE, &mainGainTable);
			LOG("EMV: using table based gain reduction\r\n");
		}
		else
			LOG("EMV: Error: unkown adaptive gain mode byte: 0x%hhx\r\n", gainMode);

		/* Read ISO14443B modulation depth mode byte. */
		modulationDepthMode = *rxByte++;
		if (0x00 == modulationDepthMode)
		{
			u8 modulationDepth = *rxByte++;
			as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x80);
			as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, modulationDepth);
			LOG("EMV: using fixed am driver strength %hhx\r\n", modulationDepth);
			emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
		}
		else if (0x01 == modulationDepthMode)
		{
			u8 adjustmentTargetValue = *rxByte++;
			u8 adjustmentDelay = *rxByte++;

			as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x7F & adjustmentTargetValue);
			LOG("EMV: using automatic modulation depth adjustment\r\n");
			LOG("EMV: adjustment target value: %hhX\r\n", adjustmentTargetValue);
			LOG("EMV: post adjustment delay: %hhx\r\n", adjustmentDelay);
			mainModulationAutomaticAdjustmentData.targetValue = adjustmentTargetValue;
			mainModulationAutomaticAdjustmentData.hysteresis = adjustmentDelay;
			emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_AUTOMATIC, &mainModulationAutomaticAdjustmentData);
		}
		else if (0x02 == modulationDepthMode)
		{
			mainModulationTable.length = *rxByte++;
			for (index = 0; index < mainModulationTable.length; index++)
			{
				mainModulationTable.x[index] = *rxByte++;
				mainModulationTable.y[index] = *rxByte++;
			}

			LOG("EMV: using table based modulation depth adjustment\r\n");
			LOG("EMV: modulation depth adjustment table length %hhd\r\n", mainModulationTable.length);
			for (index = 0; index < mainModulationTable.length; index++)
				LOG("EMV: modulationTable[%d] = 0x%hhx, 0x%hhx\r\n", index, mainModulationTable.x[index], mainModulationTable.y[index]);

			/* FIXME: configuration of the mod depth conf register should be done inside the
			 * modulation level adjustment module.
			 */
			as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x80);
			emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_FROM_AMPLITUDE, &mainModulationTable);
		}
		else
		{
			LOG("Error: unkown ISO14443B modulation depth mode byte: 0x%hhx\r\n", modulationDepthMode);
		}

        LOG("EMV: settings applied\r\n");
        displayRegisterValue(AS3911_REG_IO_CONF2);
        displayRegisterValue(AS3911_REG_REGULATOR_CONTROL);
        displayRegisterValue(AS3911_REG_ANT_CAL_CONTROL);
        displayRegisterValue(AS3911_REG_OP_CONTROL);
        displayRegisterValue(AS3911_REG_RX_CONF1);
        displayRegisterValue(AS3911_REG_RX_CONF2);
        displayRegisterValue(AS3911_REG_RX_CONF3);
        displayRegisterValue(AS3911_REG_RX_CONF4);
		displayRegisterValue(AS3911_REG_AM_MOD_DEPTH_CONTROL);
		displayRegisterValue(AS3911_REG_RFO_AM_ON_LEVEL);
        displayTestRegisterValue(AS3911_REG_ANALOG_TEST);
        break;
    case APPL_COM_REG_DUMP:
    {
        u8 regAddress = 0;
        for (regAddress = rxData[1]; regAddress < rxData[1] + rxData[2]; regAddress++)
            displayRegisterValue(regAddress);
    }
        break;
    case APPL_COM_REG_WRITE:
		LOG("REG: 0x%02hhx: set to 0x%02hhx\r\n", rxData[1], rxData[2]);
        as3911WriteRegister(rxData[1], rxData[2]);
        break;
	case APPL_COM_DIRECT_COMMAND:
		LOG("Executing direct command 0x%02hhx\r\n", rxData[1]);
		as3911ExecuteCommand(rxData[1]);
		break;
    default:
        retVal = ERR_PARAMS;
        break;
    }

    return retVal;
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
uchar amlevel = 0x00;
void test_as3911(void)
{
    volatile int key = 0;  //由于IAR编译器优化 当key 不加 volatile修饰时 选择任何一项 总是硬件错误 原因不明
    int i,j,k,m,ret,flg = 1;
    uchar ucVal;
    uchar buf[64];
    char  mifret;
    u16 miflen;
    EmvPicc_t picc;

//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_1);

//    IO_interrupt();
//    s_as3911_init();
    INIT_DEBUGBUF();
    guiDebugFlg = 0;
    sys_beep();
//	rfid_open(0);
    while ( 1 ) {
        Dprintk("\r\n\r\n-----------------as3911 basic function test------------------");
        Dprintk("\r\n\r\n 1-wupa  2-wupb  3-wupa/b  4-active a  5-active b");
        Dprintk("\r\n\r\n 9-set RFO Normal Level");
        Dprintk("\r\n\r\n 13-display the register  14-set the register");

        if(flg == 0)
        {
            flg = 1;
            key = 3;
        }
        else
            key = InkeyCount(0); //由于IAR编译器优化 当key 不加 volatile修饰时 选择任何一项 总是硬件错误 原因不明
        switch(key)
        {
//        case 0:
//            lcd_bmp_disp(4, 0, startlog);
//            sys_logo_show();
//            memcpy(gucPubBuf,(uchar *)EMBSYSZONE_SA,2048);
//            //DISPBUF(gucPubBuf, 4096, 0);
//
//            memcpy(gucPubBuf+2048,(uchar *)EMBSYSZONEBACK_SA,2048);
//            //DISPBUF(gucPubBuf, 4096, 0);
//            for(i = 0; i < 2048; i++)
//            {
//                if(gucPubBuf[i] != gucPubBuf[2048+i])
//                    Dprintk("\r\n---%d---sys error",i);
//            }
//            InkeyCount(0);
//            break;
        case 1:
            j = 0;
            k = 0;
            guiDebugFlg = 1;
            while ( 1 ) {
                CLRBUF(buf);
                INIT_DEBUGBUF();
//                emvHalResetField();
                s_DelayMs(500);
                if ( IfInkey(0) ) {
                    if ( InkeyCount(0) == 1 ) {
                        break;
                    }
                }
//                    if ( InkeyCount(0) == 1 ) {
//                        break;
//                    }
                ret = s_emvGuiWupa(buf);
//                ucVal = 0;
//                as3911ReadRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, &ucVal);
//                Dprintk("\r\n reg 24h:%x \r\n",ucVal);
//                ucVal = 0;
//                as3911ReadRegister(AS3911_REG_RFO_AM_ON_LEVEL, &ucVal);
//                Dprintk("\r\n reg 26h:%x \r\n",ucVal);
                Dprintk("\r\n --------j=%d--k=%d----wupa:%x \r\n",j,k,ret);
                j++;
                if ( ret == 0 ) {
                    k++;
                    sys_beep();
                    s_emvGuiHaltA();
                    DISPBUF("atqa", 2, 0, buf);
                }else{
                    Dprintk("\r\n error-----------------wupa:%x",ret);
                    DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                    vDispBufKey(guiDebugi,gcDebugBuf);
                }
                guiDebugi = 0;
            }
            guiDebugFlg = 0;
            break;
        case 2:
//            spi_setbps(SPI1, SPI_BPS_2250K);
//            TI_PN512ModuleTest();
            j = 0;
            k = 0;
            guiDebugFlg = 1;
            while ( 1 ) {
                CLRBUF(buf);
                INIT_DEBUGBUF();
//                emvHalResetField();
                s_DelayMs(500);
                if ( IfInkey(0) ) {
                    if ( InkeyCount(0) == 1 ) {
                        break;
                    }
                }
                ret = s_emvGuiWupb(&ucVal, buf);
//                ucVal = 0;
//                as3911ReadRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, &ucVal);
//                Dprintk("\r\n reg 24h:%x \r\n",ucVal);
//                ucVal = 0;
//                as3911ReadRegister(AS3911_REG_RFO_AM_ON_LEVEL, &ucVal);
//                Dprintk("\r\n reg 26h:%x \r\n",ucVal);
                Dprintk("\r\n --------j=%d--k=%d----wupb:%x \r\n",j,k,ret);
                j++;
                if ( ret == 0 ) {
                    k++;
                    sys_beep();
                    DISPBUF("atqb", ucVal, 0, buf);
//                    DISPBUF(gucDebugBuf, guiDebugS3, 0);
                }else{
                    Dprintk("\r\n error-----------------wupb:%x",ret);
                    DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                    vDispBufKey(guiDebugi,gcDebugBuf);
                }
                guiDebugi = 0;
            }
            guiDebugFlg = 0;
            break;
        case 3:
            j = 0;
            k = 0;
            guiDebugFlg = 1;
            while ( 1 ) {
                CLRBUF(buf);
                INIT_DEBUGBUF();
//                emvHalResetField();
                s_DelayMs(500);
                if ( IfInkey(0) ) {
                    if ( InkeyCount(0) == 1 ) {
                        break;
                    }
                }
                ret = s_emvGuiWupa(buf);
                Dprintk("\r\n\r\n--------j=%d--------------k=%d---- \r\n\r\n",j,k);
                j++;
                if ( ret == 0 ) {
                    k++;
                    sys_beep();
                    s_emvGuiHaltA();
//                    DISPBUF(buf, 2, 0);
//                    DISPBUF(gucDebugBuf, guiDebugS3, 0);
                }else{
                    s_DelayMs(50);
                    ret = s_emvGuiWupb(&ucVal, buf);
                    if ( ret == 0 ) {
                        k++;
                        sys_beep();
                    }else{
                        Dprintk("\r\n error-----------------wupa:%x",ret);
                        Dprintk("\r\n error-----------------wupb:%x",ret);
                    }
                }
                DISPBUF("poll", ucVal, 0, buf);
//                DISPBUF(gcDebugBuf, guiDebugi, 0);
                vDispBufKey(guiDebugi,gcDebugBuf);
                guiDebugi = 0;
            }
            guiDebugFlg = 0;
            break;
        case 4:
            guiDebugFlg = 1;
            Dprintk("\r\n -----typea anticollision  active -----",ret);
            while ( 1 ) {
                memset((uchar *)&picc, 0, sizeof(EmvPicc_t));
                emvHalResetField();
                if ( IfInkey(0) ) {
                    if ( InkeyCount(0) == 1 ) {
                        break;
                    }
                }
                s_DelayMs(500);
                guiDebugi = 0;
                ret = emvTypeAAnticollision(&picc);
                Dprintk("\r\n\r\n anticollision :%d",ret);
                if ( !ret ) {
                    Dprintk("\r\n\r\n uid :%x  %x  %x  %x  %x  %x",picc.uid[0],picc.uid[1],picc.uid[2],picc.uid[3],picc.uid[4],picc.uid[5]);
                    ret = picc.activate(&picc);
                    Dprintk("\r\n\r\n activate :%x ",ret);
                    if ( !ret ) {
                        CLRBUF(buf);
                        buf[0] = 0x00;
                        buf[1] = 0x84;
                        buf[2] = 0x00;
                        buf[3] = 0x00;
                        buf[4] = 0x04;
                        i = 0;
                        //                    j = (U32_C(4096) + 384) << picc.fwi;
                        //                    Dprintk("\r\n\r\n --------time  :%d    %d",j,picc.fwi);
                        //                    ret = emvPrelayer4Transceive(buf, 5, &buf[5], 4, &i,
                        //                                                   j, EMV_HAL_TRANSCEIVE_WITH_CRC);
                        s_DelayMs(50);
//                        guiDebugFlg = 1;
                        emvInitLayer4(&picc);
                        ret = emvTransceiveApdu(buf, 5, &buf[5], 16, (uint *)&i);
//                        guiDebugFlg = 0;
                        Dprintk("\r\n\r\n --------ret  :%d   ",ret);
                        if ( !ret ) {
                            DISPBUF("apdu",i,0,&buf[5]);
                            sys_beep();
                        }
                        DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                		vDispBufKey(guiDebugi,gcDebugBuf);
                    }else{
                        Dprintk("\r\n\r\n active---ret  :%d   ",ret);
                        DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                        vDispBufKey(guiDebugi,gcDebugBuf);
                    }
                }else{
                    Dprintk("\r\n\r\n anticollision---ret  :%d   ",ret);
                    DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                    vDispBufKey(guiDebugi,gcDebugBuf);
                }
            }
            guiDebugi = 0;
            guiDebugFlg = 0;
            break;
        case 5:
            guiDebugFlg = 1;
            Dprintk("\r\n -----typeb anticollision  active -----",ret);
            while ( 1 ) {
                memset((uchar *)&picc, 0, sizeof(EmvPicc_t));
                emvHalResetField();
                if ( IfInkey(0) ) {
                    if ( InkeyCount(0) == 1 ) {
                        break;
                    }
                }
                s_DelayMs(500);
                guiDebugi = 0;
                ret = emvTypeBAnticollision(&picc);
                Dprintk("\r\n\r\n anticollision :%d",ret);
                if ( !ret ) {
                    Dprintk("\r\n\r\n uid :%x  %x  %x  %x  %x  %x",picc.uid[0],picc.uid[1],picc.uid[2],picc.uid[3],picc.uid[4],picc.uid[5]);
                    ret = picc.activate(&picc);
                    Dprintk("\r\n\r\n activate :%d ",ret);
                    if ( !ret ) {
                        CLRBUF(buf);
                        buf[0] = 0x00;
                        buf[1] = 0x84;
                        buf[2] = 0x00;
                        buf[3] = 0x00;
                        buf[4] = 0x04;
                        i = 0;
                        //                    j = (U32_C(4096) + 384) << picc.fwi;
                        //                    Dprintk("\r\n\r\n --------time  :%d    %d",j,picc.fwi);
                        //                    ret = emvPrelayer4Transceive(buf, 5, &buf[5], 4, &i,
                        //                                                   j, EMV_HAL_TRANSCEIVE_WITH_CRC);
                        s_DelayMs(50);
//                        guiDebugFlg = 1;
                        emvInitLayer4(&picc);
                        ret = emvTransceiveApdu(buf, 5, &buf[5], 16, (uint *)&i);
//                        guiDebugFlg = 0;
                        Dprintk("\r\n\r\n --------ret  :%d   ",ret);
                        if ( !ret ) {
                            DISPBUF("apdu",i,0,&buf[5]);
                            sys_beep();
                        }
                    }else{
                        Dprintk("\r\n\r\n active---ret  :%d   ",ret);
                        DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                        vDispBufKey(guiDebugi,gcDebugBuf);
                    }
                }else{
                    Dprintk("\r\n\r\n anticollision---ret  :%d   ",ret);
                    DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                    vDispBufKey(guiDebugi,gcDebugBuf);
                }
            }
            guiDebugFlg = 0;
            guiDebugi = 0;
            break;
        case 6:
//            CLRBUF(buf);
//            memset(buf,0xFF,sizeof(buf));
//            as3911ContinuousRead(AS3911_REG_IRQ_MASK_MAIN, (u8*) buf, 3);
//            DISPBUF(buf, 3, 0);
            Dprintk("\r\n ----- anticollision  active -----");
            m = j = k = 0;
            guiDebugi = 0;
            while ( 1 ) {
                memset((uchar *)&picc, 0, sizeof(EmvPicc_t));
                s_DelayMs(200);
                emvHalResetField();
                if ( IfInkey(0) ) {
                    Dprintk("\r\n\r\n -------poll----m:%d---type a:%d type b:%d",m,j,k);
                    if ( InkeyCount(0) == 1 ) {
                        break;
                    }
                }
                s_DelayMs(300);
                Dprintk("\r\n\r\n --------------type a----m:%d---succ:%d ",m,j);
                guiDebugFlg = 1;
                ret = emvTypeAAnticollision(&picc);
                guiDebugFlg = 0;
                Dprintk("\r\n\r\n anticollision :%d",ret);
                if ( !ret ) {
                    Dprintk("\r\n\r\n uid :%x  %x  %x  %x  %x  %x",picc.uid[0],picc.uid[1],picc.uid[2],picc.uid[3],picc.uid[4],picc.uid[5]);
                    ret = picc.activate(&picc);
                    Dprintk("\r\n\r\n activate :%d ",ret);
                    if ( !ret ) {
                        CLRBUF(buf);
                        buf[0] = 0x00;
                        buf[1] = 0x84;
                        buf[2] = 0x00;
                        buf[3] = 0x00;
                        buf[4] = 0x04;
                        i = 0;
                        //                    j = (U32_C(4096) + 384) << picc.fwi;
                        //                    Dprintk("\r\n\r\n --------time  :%d    %d",j,picc.fwi);
                        //                    ret = emvPrelayer4Transceive(buf, 5, &buf[5], 4, &i,
                        //                                                   j, EMV_HAL_TRANSCEIVE_WITH_CRC);
                        s_DelayMs(50);
                        guiDebugFlg = 1;
                        emvInitLayer4(&picc);
                        ret = emvTransceiveApdu(buf, 5, &buf[5], 16, (uint *)&i);
                        guiDebugFlg = 0;
                        if ( !ret ) {
                            DISPBUF("apdu",i,0,&buf[5]);
//                            sys_beep();
                            ++j;
                        }
//                        DISPBUF(gcDebugBuf, guiDebugi, 0);
//                        vDispBufKey(guiDebugi,gcDebugBuf);
                    }
                }else{
                    Dprintk("\r\n\r\n wrong wrong---------type a--------ret  :%d   ",ret);
                    DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                    vDispBufKey(guiDebugi,gcDebugBuf);
                    while ( 1 ) {
                        sys_beep();
                        if ( IfInkey(0) ) {
                            if ( InkeyCount(0) == 1 ) {
                                goto testcase6;
                            }else{
                                break;
                            }
                        }
                        s_DelayMs(500);
                    }
                }

                guiDebugi = 0;

                s_DelayMs(300);
                Dprintk("\r\n\r\n --------------type b----m:%d---succ:%d ",m,k);
                guiDebugFlg = 1;
                ret = emvTypeBAnticollision(&picc);
                guiDebugFlg = 0;
                Dprintk("\r\n\r\n anticollision :%d",ret);
                if ( !ret ) {
                    Dprintk("\r\n\r\n uid :%x  %x  %x  %x  %x  %x",picc.uid[0],picc.uid[1],picc.uid[2],picc.uid[3],picc.uid[4],picc.uid[5]);
                    ret = picc.activate(&picc);
                    Dprintk("\r\n\r\n activate :%d ",ret);
                    if ( !ret ) {
                        CLRBUF(buf);
                        buf[0] = 0x00;
                        buf[1] = 0x84;
                        buf[2] = 0x00;
                        buf[3] = 0x00;
                        buf[4] = 0x04;
                        i = 0;
                        //                    j = (U32_C(4096) + 384) << picc.fwi;
                        //                    Dprintk("\r\n\r\n --------time  :%d    %d",j,picc.fwi);
                        //                    ret = emvPrelayer4Transceive(buf, 5, &buf[5], 4, &i,
                        //                                                   j, EMV_HAL_TRANSCEIVE_WITH_CRC);
                        s_DelayMs(50);
                        //                        guiDebugFlg = 1;
                        emvInitLayer4(&picc);
                        ret = emvTransceiveApdu(buf, 5, &buf[5], 16, (uint *)&i);
                        //                        guiDebugFlg = 0;
                        if ( !ret ) {
                            DISPBUF("apdu",i,0,&buf[5]);
//                            sys_beep();
                            ++k;
                        }
                        //                        
//                    DISPBUF(gcDebugBuf, guiDebugi, 0);
;
//                        vDispBufKey(guiDebugi,gcDebugBuf);
                    }
                }else{
                    Dprintk("\r\n\r\n wrong wrong---------type b--------ret  :%d   ",ret);
                    DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                    vDispBufKey(guiDebugi,gcDebugBuf);
                    while ( 1 ) {
                        sys_beep();
                        if ( IfInkey(0) ) {
                            if ( InkeyCount(0) == 1 ) {
                                goto testcase6;
                            }else{
                                break;
                            }
                        }
                        s_DelayMs(500);
                    }
                }
                guiDebugi = 0;
                ++m;
            }
testcase6:
            break;
        case 7:
            emvHalActivateField(ON);
            break;
        case 8:
            emvHalActivateField(OFF);
            break;
        case 9:
            ucVal = 0;
            as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &ucVal);
            Dprintk("\r\n\r\n input the hex   bit0-bit7  27h:%x",ucVal);
            amlevel = InkeyHex(0);
            as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, amlevel);
            s_DelayMs(20);
            ucVal = 0;
            as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &ucVal);
            Dprintk("\r\n\r\n reg 0x27:%x",ucVal);
            break;
        case 10:
            as3911ExecuteCommand(AS3911_CMD_SET_DEFAULT);
            ucVal = 0;
            as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &ucVal);
            Dprintk("\r\n\r\n reg 27h  :%x",ucVal);
            break;
        case 11:
//            Dprintk("\r\n\r\n stop:%x  %d",emvStopRequestReceived(),TRUE);
            CLRBUF(buf);

            ucVal = 0;
            as3911ReadRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, &ucVal);
            Dprintk("\r\n\r\n reg 24h  :%x",ucVal);

            guiDebugFlg = 1;
            as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
            as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
            buf[0] = AS3911_CMD_MEASURE_AMPLITUDE;
            as3911ExecuteCommand(buf[0]);
            i = 0;
            as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 20, (u32 *)&i);
            Dprintk("\r\n\r\n interrupt flag  :%x",i);
            as3911ContinuousRead(AS3911_REG_IRQ_MAIN, (u8*) &buf[5], 3);
            Dprintk("\r\n\r\n after interrupt flag  :%x %x %x",buf[5],buf[6],buf[7]);
            guiDebugFlg = 0;
            as3911DisableInterrupts(AS3911_IRQ_MASK_DCT);
            as3911ReadRegister(AS3911_REG_AD_RESULT, &buf[2]);
            Dprintk("\r\n\r\n the ad value  :%x",buf[2]);
            DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
            vDispBufKey(guiDebugi,gcDebugBuf);
            guiDebugi = 0;

            break;
        case 12:
            SETSIGNAL_H();
            as3911ExecuteCommand(AS3911_CMD_CLEAR_FIFO);
            as3911ExecuteCommand(AS3911_CMD_UNMASK_RECEIVE_DATA);
            SETSIGNAL_L();
            break;
        case 13:
            for (i = 0; i < 0x3D; i++)
                displayRegisterValue(i);
            break;
        case 14:
            Dprintk("\r\n\r\n -----set the registers -----");
            Dprintk("\r\n\r\n -----input the count -----");
            j = InkeyCount(0);
            for ( i=0 ; i<j ; i++  ) {
                Dprintk("\r\n add:");
                buf[i] = InkeyHex(0);
                Dprintk("\r\n value:");
                ucVal = InkeyHex(0);
                as3911WriteRegister(buf[i], ucVal);
            }

            for ( i=0 ; i<j ; i++  ) {
                as3911ReadRegister(buf[i], &ucVal);
                Dprintk("\r\n add:%02Xh:%02X",buf[i],ucVal);
            }
//            do_program_download((DOWNLOAD_MODE_AUTO) | DOWNLOAD_MODE_ASYN);
//            do_program_download(DOWNLOAD_MODE_AUTO);
            /*
            //结论 直接计算4096和分别计算两次2048结果是一致的
            for(i = 0; i < 16; i++)
            {
            for(j = 0; j < 256; j++)
            gucPubBuf[i*256+j] = (uchar)(j+i);
            }
            DISPBUF(gucPubBuf, 4096, 0);
            i = STM32_SoftCRC32_Test((uint *)gucPubBuf,4096/4,0xFFFFFFFF);
            Dprintk("\r\n\r\n----crc:%08x",i);
            */
            break;
        case 20:
            EI_paypass_vResetPICC();
            EI_paypass_vDelay(2000);
            ret = emvTypeAAnticollision(&picc);
            Dprintk("\r\n---mifare active:%d",ret);
            if ( !ret ) {
                DISPBUF("uid", picc.uidLength, 0, picc.uid);
                //                INIT_DEBUGBUF();
                //                guiDebugFlg = 1;
                //                ret = picc.activate(&picc);
                //                guiDebugFlg = 0;
                //                DISPBUF(gucDebugBuf, guiDebugS3, 0);
                //                vDispBufKey(guiDebugS3,gucDebugBuf);
#if 11
                //mifare

                for ( i = 0 ; i < 6 ; i++ ) {
                    buf[i] = 0xFF;
                }

                Dprintk("\r\n---mifare step 1--- input the Sector\r\n");
                m = InkeyCount(0);

                as3911WriteTestRegister(0x1,0x01);
                Dprintk("\r\n---mifare init:%x\r\n",mifareInitialize());
                mifareResetCipher();
                s_DelayMs(50);

                INIT_DEBUGBUF();
                guiDebugFlg = 1;
                mifret = mifareAuthenticateStep1(MIFARE_AUTH_KEY_A,
                                                 ((m + 1) * EM_mifs_MIFARE_BLOCKNUMPERSECTOR - 1),
                                                 picc.uid,
                                                 (uchar)picc.uidLength,
                                                 buf);
                Dprintk("\r\n\r\n AuthenticateStep1:%d\r\n",mifret);
                DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                vDispBufKey(guiDebugi,gcDebugBuf);
                INIT_DEBUGBUF();


                Dprintk("\r\n\r\n---mifare step 2--- \r\n");
                //                InkeyCount(0);
                if ( !mifret ) {
                    mifret = mifareAuthenticateStep2(0x11223344);
                    Dprintk("\r\n\r\n AuthenticateStep2:%d\r\n",mifret);
                }
                DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                vDispBufKey(guiDebugi,gcDebugBuf);
                INIT_DEBUGBUF();

                Dprintk("\r\n\r\n---mifare read--- \r\n");
                //                InkeyCount(0);
                for ( i = 0 ; i < 4 ; i++ ) {
                    buf[0] = MIFARE_READ_BLOCK;
                    buf[1] = (uchar)(m*4 + i);
                    mifret = mifareSendRequest(buf,
                                               2,
                                               &buf[2],
                                               (16+2),
                                               &miflen,
                                               MIFARE_TRANSCEIVE_DEFTIME,
                                               FALSE);
                    if ( mifret == 0 ) {
                        Dprintk("\r\n\r\n---read block %d succeed",buf[1]);
                        DISPBUF("mifread", miflen, 0, &buf[2]);
                        sys_beep();
                    }else
                        Dprintk("\r\n\r\n---read block %d err",mifret);
                }
                guiDebugFlg = 0;
                //                DISPBUF(gucDebugBuf, guiDebugS3, 0);
                //                vDispBufKey(guiDebugS3,gucDebugBuf);
                INIT_DEBUGBUF();

#else
                if ( !ret ) {
                    CLRBUF(buf);
                    buf[0] = 0x00;
                    buf[1] = 0x84;
                    buf[2] = 0x00;
                    buf[3] = 0x00;
                    buf[4] = 0x04;
                    i = 0;
                    s_DelayMs(10);
                    emvInitLayer4(&picc);
                    ret = emvTransceiveApdu(buf, 5, &buf[5], 16, (uint *)&i);
                    if ( !ret ) {
                        DISPBUF("apdu",i,0,&buf[5]);
                        sys_beep();
                        m++;
                    }
                }
#endif
            }else{

            }
            //            InkeyCount(0);

            //操作完mifare卡后要恢复
            //            as3911ModifyRegister(AS3911_REG_NUM_TX_BYTES2, 0x07, 0);
            //            EI_paypass_vResetPICC();
            //            EI_paypass_vDelay(2000);

            //            for (ucVal = 0; ucVal < 0x3D; ucVal++)
            //            {
            //                as3911ReadRegister(ucVal, &buf[0]);
            //                Dprintk("\r\n %02x reg :%02x",ucVal,buf[0]);
            //            }
            break;
        case 21:
            INIT_DEBUGBUF();

            Dprintk("\r\n\r\n---mifare write block 1--- \r\n");
            k = InkeyCount(0);

            guiDebugFlg = 1;

            buf[0] = MIFARE_WRITE_BLOCK;
            buf[1] = 1;//(uchar)InkeyCount(0);
            mifret = mifareSendRequest(buf,
                                       2,
                                       &buf[2],
                                       1,
                                       &miflen,
                                       MIFARE_TRANSCEIVE_DEFTIME,
                                       TRUE);

            Dprintk("\r\n\r\n---mifare write step 2--- \r\n");
            DISPBUF("write", miflen,0,&buf[2]);
            /* Stop processing write request if an error occured. */
            if (ERR_NONE == mifret)
            {
                /* No error occured. Send the data. */
                for ( i = 0 ; i < 16 ; i++ ) {
                    buf[i] = (uchar)(k);
                }

                mifret = mifareSendRequest(&buf[0],
                                           16,
                                           buf,
                                           64,
                                           &miflen,
                                           MIFARE_TRANSCEIVE_DEFTIME,
                                           FALSE);
                sys_beep();
                DISPBUF("write", miflen,0,buf);
                //                    if ( !mifret ) {
                //                        sys_beep();
                //                        DISPBUF(buf, miflen,0);
                //                    }else{
                //                        Dprintk("\r\n\r\n step 2---write %d block %d err",j,mifret);
                //                    }
            }else
                Dprintk("\r\n\r\n step 1---write %d block %d err",j,mifret);


            //                Dprintk("\r\n\r\n---mifare write block 2--- \r\n");
            //                buf[0] = MIFARE_WRITE_BLOCK;
            //                buf[1] = 2;//(uchar)InkeyCount(0);
            //                mifret = mifareSendRequest(buf,
            //                                           2,
            //                                           &buf[2],
            //                                           1,
            //                                           &miflen,
            //                                           MIFARE_TRANSCEIVE_DEFTIME,
            //                                           TRUE);
            //
            //                /* Stop processing write request if an error occured. */
            //                if (ERR_NONE == mifret)
            //                {
            //                    /* No error occured. Send the data. */
            //                    for ( i = 0 ; i < 16 ; i++ ) {
            //                        buf[i] = (uchar)k;
            //                    }
            //
            //                    mifret = mifareSendRequest(&buf[0],
            //                                               16,
            //                                               buf,
            //                                               64,
            //                                               &miflen,
            //                                               MIFARE_TRANSCEIVE_DEFTIME,
            //                                               FALSE);
            //                    sys_beep();
            //                    DISPBUF(buf, miflen,0);
            ////                    if ( !mifret ) {
            ////                        sys_beep();
            ////                        DISPBUF(buf, miflen,0);
            ////                    }else{
            ////                        Dprintk("\r\n\r\n step 2---write %d block %d err",j,mifret);
            ////                    }
            //                }else
            //                    Dprintk("\r\n\r\n step 1---write %d block %d err",j,mifret);
            guiDebugFlg = 0;
            DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
            vDispBufKey(guiDebugi,gcDebugBuf);
            INIT_DEBUGBUF();

            m = 0;
            for ( i = 0 ; i < 4 ; i++ ) {
                buf[0] = MIFARE_READ_BLOCK;
                buf[1] = (uchar)(m*4 + i);
                mifret = mifareSendRequest(buf,
                                           2,
                                           &buf[2],
                                           (16+2),
                                           &miflen,
                                           MIFARE_TRANSCEIVE_DEFTIME,
                                           FALSE);
                Dprintk("\r\n\r\n---read block %d ====%d",buf[1],mifret);
                DISPBUF("mifread", miflen, 0, &buf[2]);
            }
            sys_beep();
            break;
        case 22:
            Dprintk("\r\n\r\n input the sector");
            m = InkeyCount(0);
            guiDebugFlg = 1;

            for ( i = 0 ; i < 4 ; i++ ) {
                buf[0] = MIFARE_READ_BLOCK;
                buf[1] = (uchar)(m*4 + i);
                mifret = mifareSendRequest(buf,
                                           2,
                                           &buf[2],
                                           (16+2),
                                           &miflen,
                                           MIFARE_TRANSCEIVE_DEFTIME,
                                           FALSE);
                Dprintk("\r\n\r\n---read block %d ====%d",buf[1],mifret);
                DISPBUF("mifread", miflen, 0, &buf[2]);
            }
            sys_beep();
            guiDebugFlg = 0;
            DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
            vDispBufKey(guiDebugi,gcDebugBuf);
            INIT_DEBUGBUF();
            break;
        case 23:
            INIT_DEBUGBUF();
            Dprintk("\r\n\r\n increase step 1");
            guiDebugFlg = 1;
            EG_mifs_tWorkInfo.aucCmdBuf[0] = EM_mifs_PICC_DECREMENT;
            EG_mifs_tWorkInfo.aucCmdBuf[1] = 1;
            i = 1;
            memcpy(EG_mifs_tWorkInfo.aucCmdBuf + 2, (uchar *)&i, 4);
            DISPBUF("key", 6, 0, EG_mifs_tWorkInfo.aucCmdBuf);
            EG_mifs_tWorkInfo.ulSendBytes = 2;
            mifret = mifareSendRequest(EG_mifs_tWorkInfo.aucCmdBuf,
                                       (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                       EG_mifs_tWorkInfo.aucBuffer,
                                       2,
                                       &miflen,
                                       MIFARE_TRANSCEIVE_DEFTIME,
                                       TRUE);
            Dprintk("\r\n\r\n increase %d",mifret);
            DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
            vDispBufKey(guiDebugi,gcDebugBuf);
            INIT_DEBUGBUF();

            Dprintk("\r\n\r\n increase step 2");
            InkeyCount(0);


            if ( mifret == EM_SUCCESS ) {
                EG_mifs_tWorkInfo.ulSendBytes = 4;
                //                mifareflg = 1;
                mifret = mifareSendRequest(&EG_mifs_tWorkInfo.aucCmdBuf[2],
                                           (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                           EG_mifs_tWorkInfo.aucBuffer,
                                           2,
                                           &miflen,
                                           MIFARE_TRANSCEIVE_DEFTIME,
                                           TRUE);
            }
            //            mifareflg = 0;
            DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
            vDispBufKey(guiDebugi,gcDebugBuf);
            INIT_DEBUGBUF();

            for ( i = 0 ; i < 4 ; i++ ) {
                buf[0] = MIFARE_READ_BLOCK;
                buf[1] = (uchar)(0*4 + i);
                mifret = mifareSendRequest(buf,
                                           2,
                                           &buf[2],
                                           (16+2),
                                           &miflen,
                                           MIFARE_TRANSCEIVE_DEFTIME,
                                           FALSE);
                Dprintk("\r\n\r\n---read block %d ====%d",buf[1],mifret);
                DISPBUF("mifread", miflen, 0, &buf[2]);
            }

            //            DISPBUF(gucDebugBuf, guiDebugS3, 0);
            //            vDispBufKey(guiDebugS3,gucDebugBuf);
            INIT_DEBUGBUF();
            Dprintk("\r\n\r\n transfer");
            InkeyCount(0);

            EG_mifs_tWorkInfo.ulSendBytes = 2;
            EG_mifs_tWorkInfo.ucDisableDF = 0;
            EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_TRANSFER;
            EG_mifs_tWorkInfo.aucBuffer[1] = 1;
            mifret = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                       (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                       EG_mifs_tWorkInfo.aucBuffer,
                                       2,
                                       &miflen,
                                       MIFARE_TRANSCEIVE_DEFTIME,
                                       TRUE);

            DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
            vDispBufKey(guiDebugi,gcDebugBuf);
            INIT_DEBUGBUF();
            guiDebugFlg = 0;

            for ( i = 0 ; i < 4 ; i++ ) {
                buf[0] = MIFARE_READ_BLOCK;
                buf[1] = (uchar)(0*4 + i);
                mifret = mifareSendRequest(buf,
                                           2,
                                           &buf[2],
                                           (16+2),
                                           &miflen,
                                           MIFARE_TRANSCEIVE_DEFTIME,
                                           FALSE);
                Dprintk("\r\n\r\n---read block %d ====%d",buf[1],mifret);
                DISPBUF("mifread", miflen, 0, &buf[2]);
            }
            sys_beep();
            break;
        case 24:
            EG_mifs_tWorkInfo.aucCmdBuf[0] = EM_mifs_PICC_RESTORE;
            EG_mifs_tWorkInfo.aucCmdBuf[1] = 0x01;
            memset(EG_mifs_tWorkInfo.aucCmdBuf + 2, 0, 4);


            INIT_DEBUGBUF();
            Dprintk("\r\n\r\n restore step 1");
            guiDebugFlg = 1;

            EG_mifs_tWorkInfo.ulSendBytes = 2;
            mifret = mifareSendRequest(EG_mifs_tWorkInfo.aucCmdBuf,
                                       (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                       EG_mifs_tWorkInfo.aucBuffer,
                                       2,
                                       &miflen,
                                       MIFARE_TRANSCEIVE_DEFTIME,
                                       TRUE);

            Dprintk("\r\n\r\n restore 1 %d",mifret);
            DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
            vDispBufKey(guiDebugi,gcDebugBuf);
            INIT_DEBUGBUF();

            Dprintk("\r\n\r\n restore step 2");
            InkeyCount(0);

            if ( mifret == EM_SUCCESS ) {
                EG_mifs_tWorkInfo.ulSendBytes = 4;
                //                mifareflg = 1;
                mifret = mifareSendRequest(&EG_mifs_tWorkInfo.aucCmdBuf[2],
                                           (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                           EG_mifs_tWorkInfo.aucBuffer,
                                           2,
                                           &miflen,
                                           MIFARE_TRANSCEIVE_DEFTIME,
                                           TRUE);
            }
            //            mifareflg = 0;
            DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
            vDispBufKey(guiDebugi,gcDebugBuf);
            INIT_DEBUGBUF();

            for ( i = 0 ; i < 4 ; i++ ) {
                buf[0] = MIFARE_READ_BLOCK;
                buf[1] = (uchar)(0*4 + i);
                mifret = mifareSendRequest(buf,
                                           2,
                                           &buf[2],
                                           (16+2),
                                           &miflen,
                                           MIFARE_TRANSCEIVE_DEFTIME,
                                           FALSE);
                Dprintk("\r\n\r\n---read block %d ====%d",buf[1],mifret);
                DISPBUF("mifread", miflen, 0, &buf[2]);
            }

            //            DISPBUF(gucDebugBuf, guiDebugS3, 0);
            //            vDispBufKey(guiDebugS3,gucDebugBuf);
            INIT_DEBUGBUF();

            Dprintk("\r\n\r\n transfer");
            InkeyCount(0);

            EG_mifs_tWorkInfo.ulSendBytes = 2;
            EG_mifs_tWorkInfo.ucDisableDF = 0;
            EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_TRANSFER;
            EG_mifs_tWorkInfo.aucBuffer[1] = 2;
            mifret = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                       (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                       EG_mifs_tWorkInfo.aucBuffer,
                                       2,
                                       &miflen,
                                       MIFARE_TRANSCEIVE_DEFTIME,
                                       TRUE);

            DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
            vDispBufKey(guiDebugi,gcDebugBuf);
            INIT_DEBUGBUF();
            guiDebugFlg = 0;

            for ( i = 0 ; i < 4 ; i++ ) {
                buf[0] = MIFARE_READ_BLOCK;
                buf[1] = (uchar)(0*4 + i);
                mifret = mifareSendRequest(buf,
                                           2,
                                           &buf[2],
                                           (16+2),
                                           &miflen,
                                           MIFARE_TRANSCEIVE_DEFTIME,
                                           FALSE);
                Dprintk("\r\n\r\n---read block %d ====%d",buf[1],mifret);
                DISPBUF("mifread", miflen, 0, &buf[2]);
            }
            sys_beep();
            break;
//        case 90:
//            break;
//        case 91:
//            break;
//        case 92:
//            break;
//        case 93:
//            break;
        case 94:
//            GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
//            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//            GPIO_Init(GPIOB, &GPIO_InitStructure);
//            Dprintk("\r\n1-请输入占空比百倍值0-100\r\n");
//            i = InkeyCount(0);
//            if(i >100)
//            {
//                i = 100;
//            }
//            Dprintk("\r\n请输入计数模式:0-向上计数16-向下计数\r\n");
//            j = InkeyCount(0);
////            j = 0;
//            Dprintk("\r\n请输入PWM模式:96-PWM模式一112-PWM模式二\r\n");
//            k = InkeyCount(0);
////            k = 96;
//            Dprintk("\r\n请输入输出比较极性:0-高电平2-低电平\r\n");
//            ret = InkeyCount(0);
////            ret = 0;
//            TIM_Cmd(TIM3,DISABLE); // 关闭时钟
//            Timer_Config(j);
//            PWM_Config(k, ret, i);
////            InkeyCount(0);
//            TIM_Cmd(TIM3,ENABLE); // 打开时钟
            break;
        case 95:
//            Dprintk("\r\n---download font\r\n");
//            DL_Main(DOWNLOAD_MODE_ZK,&info);


            //no-response timer test
//            s_as3911_SetInt(ON);
//            as3911ClearInterrupts(AS3911_IRQ_MASK_ALL);
//            as3911EnableInterrupts(AS3911_IRQ_MASK_ALL);
//            as3911SetReceiveTimeout(InkeyCount(0));
//            as3911ReadRegister(AS3911_REG_NO_RESPONSE_TIMER1, &buf[0]);
//            as3911ReadRegister(AS3911_REG_NO_RESPONSE_TIMER2, &buf[1]);
//            DISPBUF(buf, 2, 0);
//            as3911ExecuteCommand(AS3911_CMD_START_NO_RESPONSE_TIMER);
//            GPIO_SetBits(GPIOB,GPIO_Pin_1);
//            InkeyCount(0);
//            as3911ClearInterrupts(AS3911_IRQ_MASK_ALL);

            i = 0;
            while ( ++i < 10 ) {
                s_DelayUs(50);
                //GPIO_ResetBits(GPIOB,GPIO_Pin_0);
//                GPIO_ResetBits(GPIOB,GPIO_Pin_1);
                //s_DelayUs(50);
                //GPIO_SetBits(GPIOB,GPIO_Pin_0);
//                GPIO_SetBits(GPIOB,GPIO_Pin_1);
            }

            CLRBUF(buf);
            s_as3911ReadRegister(3, AS3911_REG_IRQ_MAIN, buf);
            DISPBUF("reg",3,0,buf);
//            CLRBUF(EG_Param);
//            //TYPE A
//            EG_Param[0] = 0x8E;
//            EG_Param[1] = 0x08;
//            EG_Param[2] = 0x21;
//            EG_Param[3] = 0x1A;
//            EG_Param[4] = 0x27;
//            EG_Param[5] = 0x32;
//            EG_Param[6] = 0x12;
//            EG_Param[7] = 0x0A;
//            //TYPE B
//            EG_Param[8] = 0x8F;
//            EG_Param[9] = 0x4F;
//            EG_Param[10] = 0x01;
//            EG_Param[11] = 0x06;
//            EG_Param[12] = 0x00;
//            EG_Param[13] = 0x3F;
//            EG_Param[14] = 0x12;
//            EG_Param[15] = 0x0A;
//            TI_PN512ModuleTest();
//            CLRBUF(buf);
//            i = 0;
//            s_as3911ReadRegister(3, AS3911_REG_IRQ_MAIN, (uchar*)&i);
//            Dprintk("\r\n----i:%x",i);
//            CLRBUF(buf);
//            s_as3911ReadRegister(10, AS3911_REG_IO_CONF1, buf);
//            DISPBUF(buf,10,0);
            break;
        case 96:
            Dprintk("\r\n---通讯方式设置---0-串口  1-usb\r\n");
            if(InkeyCount(0) == 0)
            {
                //gucComNo = UART_COM1;
                //gucDebugCom = UART_COM1;
            }
            else
            {
                //gucComNo = UART_USB;
                //gucDebugCom = UART_USB;
            }

            i = 0;
            while(1)
            {
                Dprintk("\r\n---usb test---%d\r\n",i++);
                if(InkeyCount(0) == 1)
                    break;
            }
            //gucComNo = UART_COM1;
            //gucDebugCom = UART_COM1;
            break;
        case 97:
            //vCtrlMainTest();
            //    vCtrlMainTest();
            //    local_test_menu_com(0);
            break;
        case 98:
//            Dprintk("\r\n0-串口更新Boot   1-USB更新Boot\r\n");
//            if(InkeyCount(0) == 1)
//            {
//                gucComNo = UART_USB;
//                gucDebugCom = UART_USB;
//            }
//            DL_Main(DOWNLOAD_MODE_AUTO,&info);
//            gucComNo = UART_COM1;
//            gucDebugCom = UART_COM1;
            break;
        case 99:
            //system_reset();
            return;
            //break;
        default:
            break;
        }
    }
}

void test_loadas3911regtable(int id, char *xytable)
{
    uchar ucVal;
    int i;

//    if ( id != 0 ) {
        as3911WriteRegister(AS3911_REG_REGULATOR_CONTROL, gas3911paramtable[id].reg2Ah);
        as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, gas3911paramtable[id].reg27h);
        as3911WriteRegister(AS3911_REG_ANT_CAL_CONTROL, gas3911paramtable[id].reg21h);
        as3911WriteRegister(AS3911_REG_OP_CONTROL, gas3911paramtable[id].reg02h);
        as3911WriteRegister(AS3911_REG_IO_CONF2, gas3911paramtable[id].reg01h);
        as3911WriteRegister(AS3911_REG_RX_CONF1, gas3911paramtable[id].reg0Ah);
        as3911WriteRegister(AS3911_REG_RX_CONF2, gas3911paramtable[id].reg0Bh);
        as3911WriteRegister(AS3911_REG_RX_CONF3, gas3911paramtable[id].reg0Ch);
        as3911WriteRegister(AS3911_REG_RX_CONF4, gas3911paramtable[id].reg0Dh);

        as3911ReadRegister(AS3911_REG_REGULATOR_CONTROL, &ucVal);
        Dprintk("\r\n --2Ah: %02x",ucVal);
        as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &ucVal);
        Dprintk("\r\n --27h: %02x",ucVal);
        as3911ReadRegister(AS3911_REG_ANT_CAL_CONTROL, &ucVal);
        Dprintk("\r\n --21h: %02x",ucVal);
        as3911ReadRegister(AS3911_REG_OP_CONTROL, &ucVal);
        Dprintk("\r\n --02h: %02x",ucVal);
        as3911ReadRegister(AS3911_REG_IO_CONF2, &ucVal);
        Dprintk("\r\n --01h: %02x",ucVal);
        as3911ReadRegister(AS3911_REG_RX_CONF1, &ucVal);
        Dprintk("\r\n --0Ah: %02x",ucVal);
        as3911ReadRegister(AS3911_REG_RX_CONF2, &ucVal);
        Dprintk("\r\n --0Bh: %02x",ucVal);
        as3911ReadRegister(AS3911_REG_RX_CONF3, &ucVal);
        Dprintk("\r\n --0Ch: %02x",ucVal);
        as3911ReadRegister(AS3911_REG_RX_CONF4, &ucVal);
        Dprintk("\r\n --0Dh: %02x",ucVal);
//    }

    if ( xytable ) {
        mainModulationTable.length = 6;
        for (i = 0; i < mainModulationTable.length; i++)
        {
            mainModulationTable.x[i] = xytable[i*2];
            mainModulationTable.y[i] = xytable[i*2+1];
        }
        DISPBUF("x table", 6, 0, mainModulationTable.x);
        DISPBUF("y table", 6, 0, mainModulationTable.y);
        as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x80);
        emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_FROM_AMPLITUDE, &mainModulationTable);
    }
    return;
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void displayCardinfo(void)
{
    int i;

    Dprintk("\r\n===========card information===========bit8-bit1\r\n");
    if ( gemvcardinfo.cardtype == 0 ) {
        Dprintk("\r\n\r\n------A/B TYPE POLL------");

    }else if ( gemvcardinfo.cardtype == 1 ) {
        Dprintk("\r\n\r\n-----A TYPE CARD------");
        Dprintk("\r\n\r\n-ATQA: %02X  %02X",gemvcardinfo.ATQA[0],gemvcardinfo.ATQA[1]);
        Dprintk("\r\n\r\n-UID:");
        for ( i = 0 ; i < sizeof(gemvcardinfo.uidLength) ; i++ ) {
            Dprintk(" %02X",gemvcardinfo.uid[i]);
        }
        Dprintk("\r\n\r\n-ATS:");
        for ( i = 0 ; i < sizeof(gemvcardinfo.ATS) ; i++ ) {
            Dprintk(" %02X",gemvcardinfo.ATS[i]);
        }
        Dprintk("\r\n TL- %02X", gemvcardinfo.ATS[0]);
        Dprintk("\r\n T0- %02X \r\n     b7-5: %02X \r\n     FSCI: %02X", gemvcardinfo.ATS[1], gemvcardinfo.ATS[1]>>4&0x07
               , gemvcardinfo.ATS[1]&0x0F );
        Dprintk("\r\n TA- %02X \r\n     bit8: %02X \r\n     b7-5: %02X \r\n     b3-1: %02X",
               gemvcardinfo.ATS[2],gemvcardinfo.ATS[2]>>7&0x01,gemvcardinfo.ATS[2]>>4&0x07,gemvcardinfo.ATS[2]&0x07);
        Dprintk("\r\n TB- %02X \r\n     FWI : %02X \r\n     SFGI: %02X",
               gemvcardinfo.ATS[3], gemvcardinfo.ATS[3]>>4&0x0F, gemvcardinfo.ATS[3]&0x0F);
        Dprintk("\r\n TC- %02X \r\n     CID : %02X \r\n     NAD : %02X",
               gemvcardinfo.ATS[4], gemvcardinfo.ATS[4]>>1&0x01, gemvcardinfo.ATS[4]&0x01);
    }else if ( gemvcardinfo.cardtype == 2 ) {
        Dprintk("\r\n\r\n-----B TYPE CARD------");
        Dprintk("\r\n\r\n-ATQB:");
        for ( i = 0 ; i < sizeof(gemvcardinfo.ATQB) ; i++ ) {
            Dprintk(" %02X",gemvcardinfo.ATQB[i]);
        }
        Dprintk("\r\n D1- %02X", gemvcardinfo.ATQB[0]);
        Dprintk("\r\n UI- %02X %02X %02X %02X",
               gemvcardinfo.ATQB[1], gemvcardinfo.ATQB[2], gemvcardinfo.ATQB[3], gemvcardinfo.ATQB[4]);
        Dprintk("\r\n AP- %02X %02X %02X %02X",
               gemvcardinfo.ATQB[5], gemvcardinfo.ATQB[6], gemvcardinfo.ATQB[7], gemvcardinfo.ATQB[8]);
        Dprintk("\r\n B1- %02X \r\n     bit8: %02X \r\n     b7-5: %02X \r\n     b3-1: %02X",
               gemvcardinfo.ATQB[9],gemvcardinfo.ATQB[9]>>7&0x01,gemvcardinfo.ATQB[9]>>4&0x07,gemvcardinfo.ATQB[9]&0x07);
        Dprintk("\r\n B2- %02X \r\n     FSCI: %02X \r\n     TYPE: %02X",
               gemvcardinfo.ATQB[10],gemvcardinfo.ATQB[10]>>4&0x0F, gemvcardinfo.ATQB[10]&0x0F);
        Dprintk("\r\n B3- %02X \r\n     FWI : %02X \r\n     ADC : %02X \r\n     FO  : %02X",
               gemvcardinfo.ATQB[11],gemvcardinfo.ATQB[11]>>4&0x0F,gemvcardinfo.ATQB[11]>>2&0x03,gemvcardinfo.ATQB[11]&0x03);
        Dprintk("\r\n B4(SFGI)- %02X", gemvcardinfo.ATQB[12]);

        Dprintk("\r\n\r\n-ATTRIB:");
        for ( i = 0 ; i < sizeof(gemvcardinfo.ATTRIB) ; i++ ) {
            Dprintk(" %02X",gemvcardinfo.ATTRIB[i]);
        }
        Dprintk("\r\n D1- %02X", gemvcardinfo.ATTRIB[0]);
        Dprintk("\r\n UI- %02X %02X %02X %02X",
               gemvcardinfo.ATTRIB[1], gemvcardinfo.ATTRIB[2], gemvcardinfo.ATTRIB[3], gemvcardinfo.ATTRIB[4]);
        Dprintk("\r\n P1- %02X", gemvcardinfo.ATTRIB[5]);
        Dprintk("\r\n P2- %02X \r\n     C2D:%02X \r\n     D2C:%02X \r\n     FSD:%02X",
               gemvcardinfo.ATTRIB[6],gemvcardinfo.ATTRIB[6]>>6&0x03,gemvcardinfo.ATTRIB[6]>>4&0x03,gemvcardinfo.ATTRIB[6]&0x0F);
        Dprintk("\r\n P3- %02X", gemvcardinfo.ATTRIB[7]);
        Dprintk("\r\n P4- %02X", gemvcardinfo.ATTRIB[8]);
    }else{
        Dprintk("\r\n------NO CARD TYPE------\r\n");
    }
}

int test_loadas3911xytable(int id)
{
//    int i;
//    uchar lookuptable[12];
//
//    s_read_syszone(SZ_3911EMV_TABLE0,sizeof(gas3911paramtable),gucDebugBuf);
//    for ( i = 1 ; i < 4 ; i++ ) {
//        memcpy(&gas3911paramtable[i], gucDebugBuf+i*sizeof(RFID_EMV_REGCONFIG),sizeof(RFID_EMV_REGCONFIG));
//    }
//
//    memcpy(lookuptable, &gas3911paramtable[id].regtable[14],sizeof(lookuptable));
//    mainModulationTable.length = 6;
//    for (i = 0; i < mainModulationTable.length; i++)
//    {
//        mainModulationTable.x[i] = lookuptable[i*2];
//        mainModulationTable.y[i] = lookuptable[i*2+1];
//    }
//    DISPBUF(mainModulationTable.x, 6, 0);
//    DISPBUF(mainModulationTable.y, 6, 0);
//    as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x80);
//    emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_FROM_AMPLITUDE, &mainModulationTable);
//    s_DelayMs(10);



    mainModulationTable.length = 6;
#if 0 
    mainModulationTable.x[0] = 0x33;
    mainModulationTable.y[0] = 0xB0;

    mainModulationTable.x[1] = 0x44;
    mainModulationTable.y[1] = 0xA0;

    mainModulationTable.x[2] = 0x58;
    mainModulationTable.y[2] = 0xA8;

    mainModulationTable.x[3] = 0x64;
    mainModulationTable.y[3] = 0xB0;

    mainModulationTable.x[4] = 0x6C;
    mainModulationTable.y[4] = 0xB4;

    mainModulationTable.x[5] = 0x70;
    mainModulationTable.y[5] = 0xB2;
#else
    //09252013 chenf
//    mainModulationTable.x[0] = 0x49;
//    mainModulationTable.y[0] = 0x5C;
//
//    mainModulationTable.x[1] = 0x72;
//    mainModulationTable.y[1] = 0x60;
//
//    mainModulationTable.x[2] = 0x90;
//    mainModulationTable.y[2] = 0x68;
//
//    mainModulationTable.x[3] = 0xA8;
//    mainModulationTable.y[3] = 0x6E;
//
//    mainModulationTable.x[4] = 0xBB;
//    mainModulationTable.y[4] = 0x70;
//
//    mainModulationTable.x[5] = 0xC1;
//    mainModulationTable.y[5] = 0x6E;

    int i;
    uchar lookuptable[12];

    Read3911_new(12,lookuptable);

    mainModulationTable.length = 6;
    for (i = 0; i < mainModulationTable.length; i++)
    {
        mainModulationTable.x[i] = lookuptable[i*2];
        mainModulationTable.y[i] = lookuptable[i*2+1];
    }
#endif

    DISPBUF("x table", 6, 0, mainModulationTable.x);
    DISPBUF("y table", 6, 0, mainModulationTable.y);
    Dprintk("\r\n");
    as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x80);
    emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_FROM_AMPLITUDE, &mainModulationTable);
    s_DelayMs(10);
    return 0;
}

int test_loadas3911_typeBAUTOMATIC(int id)
{
    int i;
    as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x24);
    as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
    as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
    as3911ExecuteCommand(AS3911_CMD_CALIBRATE_MODULATION);
    i = 0;
    as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 200, (ulong *)&i);
    mainModulationAutomaticAdjustmentData.targetValue = 0x20;
    mainModulationAutomaticAdjustmentData.hysteresis = 0x20;
    emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_AUTOMATIC, &mainModulationAutomaticAdjustmentData);
    Dprintk("\r\n--- type b  automatic");
    return 0;
}

void test_as3911_ioinit(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;	
//
//    GPIO_InitStructure.GPIO_Pin = Pin_SAM_IO;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	
//    GPIO_Init(Port_SAM, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin = Pin_SAM_ClkEN;
//    GPIO_Init(Port_SAM_ClkEN, &GPIO_InitStructure);
//
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_6
//                                 |GPIO_Pin_7|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
//
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8
//                                 |GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_9);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

char as3911Initialize()
{
    u8 reg;

    /* Reset the AS3911 */
    as3911ExecuteCommand(AS3911_CMD_SET_DEFAULT);

    /* Enable pull downs on miso/mosi */
    as3911ModifyRegister(AS3911_REG_IO_CONF2, 0, 0x18);

    as3911ReadRegister(0xb, &reg);
    if (0x1a != reg) return ERR_IO;
    as3911ReadRegister(0xc, &reg);
    if (0xd8 != reg) return ERR_IO;

    /* MCU_CLK and LF MCU_CLK off, 27MHz XTAL */
    as3911WriteRegister(AS3911_REG_IO_CONF1, 0x0F);

    /* Enable Oscillator and receiver. */
    as3911WriteRegister(AS3911_REG_OP_CONTROL, 0xC0);
#if 0
    /* FIXME */
    sleepMilliseconds(5);
#else
    {
    	int i;
    	for (i=0; i<500*1000; i++);
    }
#endif

    /* Enable AM/PM receiver signal output on CSI/CSO. */
    // as3911WriteTestRegister(0x01, 0x03);

    /* Enable AS3911 IRQ handling. */
	AS3911_IRQ_ON();

    return ERR_NONE;
}

char as3911Deinitialize()
{
    AS3911_IRQ_OFF();
    return as3911WriteRegister(AS3911_REG_OP_CONTROL, 0x0);
}

void test_as3911_menu(void)
{
    Dprintk("\r\n-----------------as3911 emv test---------------%d  %d  %d  \r\n",sizeof(RFID_EMV_DEBUGFLG),sizeof(RFID_EMV_REGCONFIG),sizeof(gas3911paramtable));
    Dprintk("\r\n\r\n 1-emv electric \t2-emv protocol \t3-config the emv parameter");
    Dprintk("\r\n\r\n 4-set electric register \t5-make look up table \t6-save configuration to flash");
    Dprintk("\r\n\r\n 7-load the config from flash \t8-set all register \t9-set the table and mode");
    Dprintk("\r\n\r\n 10-display all regs\t11-s950 type a \t12-s950 type b");
    Dprintk("\r\n\r\n 13-type a/b \t14-reg type a/b  \t15-set reg27h  \t16-set reg26  17-save reg26/27");
    Dprintk("\r\n");
    Dprintk("\r\n\r\n 91-as3911 hard function ");
    Dprintk("\r\n\r\n 92-start the timer      93-update the timer");
    Dprintk("\r\n\r\n 94-set the poll debug flg");
    Dprintk("\r\n\r\n 95-set the acollision debug flg");
    Dprintk("\r\n\r\n 96-set the active debug flg");
    Dprintk("\r\n\r\n 97-set the apdu debug flg");
    Dprintk("\r\n\r\n 98-set the default type b modulation depth");
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void  Read3911_new(uint len, uchar *ucBuf)
{
    MODULE_RFIDEMV_INFO rfidemvinfo;

    memset((uchar*)&rfidemvinfo, 0xFF, sizeof(MODULE_RFIDEMV_INFO));
    s_sysinfo_rfidemvinfo(0, &rfidemvinfo);
    memcpy(ucBuf, (uchar*)&rfidemvinfo, len);
    return;
}
int32_t Write3911_new(uint uiLen, uchar *ucBuf)
{
    MODULE_RFIDEMV_INFO rfidemvinfo;

    memset((uchar*)&rfidemvinfo, 0xFF, sizeof(MODULE_RFIDEMV_INFO));
    memcpy((uchar*)&rfidemvinfo, ucBuf, uiLen);
    s_sysinfo_rfidemvinfo(1, &rfidemvinfo);
    return 0;
}
#if 0
const uchar rfidlogo[] = {
    0x42,0x4d,0xae,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x28,0x00,
    0x00,0x00,0xa6,0x00,0x00,0x00,0x5a,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,
    0x00,0x00,0x70,0x08,0x00,0x00,0xc3,0x0e,0x00,0x00,0xc3,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x7f,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xff,0xff,0xff,0xff,0xf8,0x00,0x00,0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
    0x00,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xff,0xff,0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x0f,0xff,0xfc,
    0x00,0x00,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xff,0xff,0xf8,0x00,0x1f,0xff,0xff,0xff,0xfc,0x00,0x0f,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xc0,0x03,0xff,0xff,0xff,0xff,
    0xff,0xe0,0x01,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xff,0xfe,0x00,0x1f,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x3f,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0xf8,0x01,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xc0,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xff,0xe0,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf8,0x03,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0x80,0x3f,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfe,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xfe,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,0x3f,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xfc,0x07,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xf0,0x1f,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xf0,0x0f,0xff,0xff,0xff,0xff,0xff,0xf8,0x7f,0xff,0xff,0xfc,0x07,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xe0,0x3f,0xff,0xff,0xff,0xff,0xff,0xf0,
    0x3f,0xff,0xff,0xff,0x03,0xff,0xff,0xe0,0x3f,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0x80,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x3f,0xff,0xff,0xff,0x80,0xff,0xff,0x80,
    0x3f,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0x03,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,
    0x1f,0xff,0xff,0xff,0xe0,0x7f,0xff,0x00,0x7f,0xff,0xfc,0x00,0x00,0x00,0xff,0xfe,
    0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x0f,0xff,0xff,0xff,0xf8,0x3f,0xfe,0x03,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xfc,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,
    0x0f,0xff,0xff,0xff,0xfc,0x1f,0xf8,0x0f,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xf0,
    0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x07,0xff,0xff,0xff,0xfe,0x0f,0xf0,0x3f,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xe0,0x7f,0xff,0xff,0xff,0xff,0xff,0x8f,0xf0,
    0x07,0xff,0xff,0xff,0xff,0x07,0xf0,0x7f,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xc0,
    0xff,0xff,0xff,0xff,0xff,0xfe,0x07,0xf8,0x03,0xff,0xff,0xff,0xff,0x00,0x00,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xc1,0xff,0xff,0xff,0xff,0xff,0xfe,0x03,0xf8,
    0x03,0xff,0xff,0xff,0xfc,0x00,0x01,0xff,0xff,0xff,0x8c,0x00,0x00,0x00,0xff,0x83,
    0xff,0xff,0xff,0xff,0xff,0xfe,0x01,0xf8,0x03,0xff,0xff,0xff,0xe0,0x00,0x01,0xff,
    0xff,0xff,0x0c,0x00,0x00,0x00,0xff,0x07,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0xfc,
    0x01,0xff,0xff,0xff,0x80,0x1f,0xc3,0xff,0xff,0xfe,0x0c,0x00,0x00,0x00,0xfe,0x0f,
    0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0xfc,0x01,0xff,0xff,0xfe,0x00,0xe3,0xc3,0xff,
    0xff,0xfc,0x0c,0x00,0x00,0x00,0xfc,0x1f,0xff,0xff,0xff,0xff,0xf0,0x7e,0x00,0x7e,
    0x00,0xff,0xff,0xfe,0x07,0xc1,0x87,0xff,0xff,0xfc,0x1c,0x00,0x00,0x00,0xfc,0x1f,
    0xff,0xff,0xff,0xff,0xe0,0x3f,0x00,0x7e,0x00,0xff,0xff,0xfe,0x1f,0x00,0x87,0xff,
    0xff,0xf8,0x3c,0x00,0x00,0x00,0xf8,0x3f,0xff,0xff,0xff,0xff,0xe0,0x3f,0x80,0x3e,
    0x00,0xff,0xff,0xfe,0x1e,0x00,0x0f,0xff,0xff,0xf0,0x7c,0x00,0x00,0x00,0xf8,0x7f,
    0xff,0xff,0xff,0xff,0xe0,0x1f,0x80,0x3f,0x00,0xff,0xff,0xfe,0x08,0x08,0x0f,0xff,
    0xff,0xe0,0x7c,0x00,0x00,0x00,0xf0,0x7f,0xff,0xff,0xff,0xcf,0xe0,0x0f,0x80,0x1f,
    0x00,0x7f,0xff,0xff,0x00,0x3c,0x0f,0xff,0xff,0xc0,0xfc,0x00,0x00,0x00,0xf0,0xff,
    0xff,0xff,0xff,0x03,0xe0,0x07,0xc0,0x1f,0x00,0x7f,0xff,0xff,0x80,0x78,0x1f,0xff,
    0xff,0xc1,0xfc,0x00,0x00,0x00,0xe0,0xff,0xff,0xff,0xfe,0x03,0xf0,0x07,0xc0,0x1f,
    0x00,0x7f,0xff,0xff,0x01,0xe0,0x3f,0xff,0xff,0x83,0xfc,0x00,0x00,0x00,0xe1,0xff,
    0xff,0xff,0xfe,0x01,0xf8,0x07,0xe0,0x1f,0x00,0x7f,0xff,0xfe,0x03,0xc0,0x7f,0xff,
    0xff,0x03,0xfc,0x00,0x00,0x00,0xc1,0xff,0xff,0xff,0xfe,0x00,0xf8,0x03,0xe0,0x0f,
    0x80,0x7f,0xff,0xfc,0x07,0x01,0xff,0xff,0xff,0x07,0xfc,0x00,0x00,0x00,0xc3,0xff,
    0xff,0xff,0xfe,0x00,0xf8,0x03,0xe0,0x0f,0x80,0x7f,0xff,0xf8,0x1e,0x03,0xff,0xff,
    0xfe,0x0f,0xfc,0x00,0x00,0x00,0xc3,0xff,0xff,0xff,0xff,0x80,0x7c,0x01,0xe0,0x0f,
    0x80,0x3f,0xff,0xf8,0x3c,0x0f,0xff,0xff,0xfc,0x1f,0xfc,0x00,0x00,0x00,0x83,0xff,
    0xff,0xff,0xff,0x80,0x7c,0x01,0xf0,0x0f,0x80,0x3f,0xff,0xf0,0x70,0x1f,0xff,0xff,
    0xfc,0x3f,0xfc,0x00,0x00,0x00,0x83,0xff,0xff,0xff,0xff,0xc0,0x3c,0x01,0xf0,0x07,
    0x80,0x3f,0xff,0xf8,0x60,0x7f,0xff,0xff,0xf8,0x3f,0xfc,0x00,0x00,0x00,0x87,0xff,
    0xff,0xff,0xff,0xc0,0x3c,0x01,0xf0,0x07,0x80,0x3f,0xff,0xf8,0x00,0xff,0xff,0xff,
    0xf0,0x7f,0xfc,0x00,0x00,0x00,0x87,0xff,0xff,0xff,0xff,0xe0,0x3e,0x01,0xf0,0x07,
    0x80,0x3f,0xff,0xf0,0x01,0xff,0xf9,0xff,0xe0,0xff,0xfc,0x00,0x00,0x00,0x87,0xff,
    0xff,0xff,0xff,0xe0,0x3e,0x01,0xf0,0x07,0x80,0x3f,0xf0,0x00,0x07,0xff,0xf0,0xff,
    0xc1,0xff,0xfc,0x00,0x00,0x00,0x87,0xff,0xff,0xff,0xff,0xe0,0x3e,0x00,0xf0,0x07,
    0x80,0x3f,0x80,0x00,0x0f,0xff,0xc0,0xff,0xc3,0xff,0xfc,0x00,0x00,0x00,0x87,0xff,
    0xff,0xff,0xff,0xe0,0x3e,0x00,0xf0,0x07,0x80,0x3e,0x00,0x00,0x3f,0xff,0x80,0xff,
    0x83,0xff,0xfc,0x00,0x00,0x00,0x87,0xff,0xff,0xff,0xff,0xe0,0x3e,0x00,0xf0,0x07,
    0x80,0x3e,0x00,0x00,0x7f,0xfe,0x03,0xff,0x07,0xff,0xfc,0x00,0x00,0x00,0x87,0xff,
    0xff,0xff,0xff,0xe0,0x3e,0x00,0xf0,0x07,0x80,0x3e,0x0f,0x80,0xff,0xf8,0x07,0xff,
    0x0f,0xff,0xfc,0x00,0x00,0x00,0x87,0xff,0xff,0xff,0xff,0xc0,0x3e,0x01,0xf0,0x07,
    0x80,0x3e,0x1f,0x03,0xff,0xe0,0x1f,0xfe,0x1f,0xff,0xfc,0x00,0x00,0x00,0x87,0xff,
    0xff,0xff,0xff,0xc0,0x3e,0x01,0xf0,0x07,0x80,0x3e,0x1e,0x07,0xff,0x80,0x7f,0xfc,
    0x1f,0xff,0xfc,0x00,0x00,0x00,0x87,0xff,0xff,0xff,0xff,0xc0,0x3c,0x01,0xf0,0x07,
    0x80,0x3e,0x1e,0x0f,0xff,0x00,0xff,0xf8,0x3f,0xff,0xfc,0x00,0x00,0x00,0x83,0xff,
    0xff,0xff,0xff,0x80,0x7c,0x01,0xf0,0x07,0x80,0x3f,0x0e,0x1f,0xfc,0x01,0xff,0xf8,
    0x7f,0xff,0xfc,0x00,0x00,0x00,0x83,0xff,0xff,0xff,0xff,0x00,0x7c,0x01,0xf0,0x0f,
    0x80,0x3f,0x0e,0x0f,0xf0,0x07,0xff,0xf0,0xff,0xff,0xfc,0x00,0x00,0x00,0xc3,0xff,
    0xff,0xff,0xfe,0x00,0x7c,0x03,0xf0,0x0f,0x80,0x3f,0x0e,0x0f,0xc0,0x0f,0xff,0xe0,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xc3,0xff,0xff,0xff,0xfe,0x00,0xf8,0x03,0xe0,0x0f,
    0x80,0x7f,0x0f,0x03,0x00,0x03,0xff,0x01,0xff,0xff,0xfc,0x00,0x00,0x00,0xc1,0xff,
    0xff,0xff,0xfe,0x00,0xf8,0x03,0xe0,0x0f,0x80,0x7f,0x07,0x80,0x01,0x80,0x00,0x03,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xc1,0xff,0xff,0xff,0xfe,0x01,0xf0,0x07,0xe0,0x1f,
    0x00,0x7f,0x87,0xc0,0x07,0x80,0x00,0x07,0xff,0xff,0xfc,0x00,0x00,0x00,0xe0,0xff,
    0xff,0xff,0xff,0x03,0xf0,0x07,0xc0,0x1f,0x00,0x7f,0x87,0xc0,0x1f,0xc0,0x00,0x1f,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xe0,0xff,0xff,0xff,0xff,0xc7,0xe0,0x0f,0xc0,0x1f,
    0x00,0x7f,0x87,0xf0,0x7f,0xc0,0x00,0x7f,0xff,0xff,0xfc,0x00,0x00,0x00,0xf0,0x7f,
    0xff,0xff,0xff,0xff,0xe0,0x1f,0xc0,0x1f,0x00,0x7f,0x83,0xff,0xff,0xc7,0x87,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xf0,0x7f,0xff,0xff,0xff,0xff,0xe0,0x1f,0x80,0x3f,
    0x00,0xff,0xc3,0xff,0xfc,0x07,0x0f,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xf8,0x3f,
    0xff,0xff,0xff,0xff,0xe0,0x3f,0x80,0x3e,0x00,0xff,0xc3,0xff,0x00,0x07,0x0f,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xf8,0x3f,0xff,0xff,0xff,0xff,0xe0,0x7f,0x00,0x7e,
    0x00,0xff,0xc1,0x80,0x00,0x0e,0x1f,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xfc,0x1f,
    0xff,0xff,0xff,0xff,0xf8,0xff,0x00,0x7e,0x00,0xff,0xe0,0x00,0x00,0x1c,0x1f,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xfe,0x0f,0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0xfc,
    0x01,0xff,0xe0,0x00,0x03,0xf8,0x3f,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0x07,
    0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0xfc,0x01,0xff,0xf0,0x00,0xff,0xf0,0x7f,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0x03,0xff,0xff,0xff,0xff,0xff,0xfe,0x01,0xf8,
    0x03,0xff,0xfc,0x7f,0xff,0xf0,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0x81,
    0xff,0xff,0xff,0xff,0xff,0xfe,0x03,0xf8,0x03,0xff,0xff,0xff,0xff,0xe0,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xff,0x03,0xf8,
    0x03,0xff,0xff,0xff,0xff,0xc1,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xe0,
    0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x07,0xff,0xff,0xff,0xff,0x03,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xf0,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,
    0x07,0xff,0xff,0xff,0xfe,0x07,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xf8,
    0x1f,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x0f,0xff,0xff,0xff,0xfc,0x0f,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xfc,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,
    0x1f,0xff,0xff,0xff,0xf8,0x1f,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0x03,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x1f,0xff,0xff,0xff,0xe0,0x7f,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0x80,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,
    0x3f,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xc0,0x7f,0xff,0xff,0xff,0xff,0xff,0xfc,0xff,0xff,0xff,0xff,0x01,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xf0,0x1f,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xfc,0x07,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xf8,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x0f,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xfe,0x01,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xc0,0x3f,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xff,0x80,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0xe0,0x0f,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xf8,0x03,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xff,0xf8,0x01,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x07,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0xfe,0x00,0x3f,0xff,0xff,0xff,0xff,
    0xff,0xfe,0x00,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xff,0xff,0x80,0x03,0xff,0xff,0xff,0xff,0xff,0xf0,0x00,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xf0,0x00,0x3f,0xff,0xff,0xff,
    0xfe,0x00,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xff,0xff,0xfe,0x00,0x00,0x3f,0xff,0xff,0x00,0x00,0x3f,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xe0,0x00,0x00,0x00,0x00,
    0x00,0x01,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x1f,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x00,0x00,0x00,
    0x03,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xf0,0x00,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xfc,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00
};
#else
//wp70彩屏logo
//const uchar rfidlogo[] = {
//    0x42, 0x4d, 0xe6, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x28, 0x00,
//    0x00, 0x00, 0xaa, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0xa8, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00,
//    0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x03, 0xff, 0xff, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x07, 0xff, 0xff,
//    0x80, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xfe, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xf8, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0x80, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xf8, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xe0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0x80, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0x80, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xfc, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xfc, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xf8, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc,
//    0x0f, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xf8, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xe0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0xff, 0xff, 0xff, 0xe0, 0x3f, 0xff, 0xe0,
//    0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8,
//    0x0f, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xff, 0xe0, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xc0,
//    0x1f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8,
//    0x03, 0xff, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0x80, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff, 0xff, 0xff, 0x07, 0xfe, 0x03,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8,
//    0x01, 0xff, 0xff, 0xff, 0xff, 0x83, 0xfc, 0x0f, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xf8,
//    0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xfc, 0x01, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xfc, 0x1f,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xfe,
//    0x00, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xf0,
//    0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0xfe, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x7f,
//    0xff, 0xff, 0xe3, 0xc0, 0x00, 0x00, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x7e,
//    0x00, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xc3, 0xc0, 0x00, 0x00, 0xff, 0xc1,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x3f, 0x00, 0x7f, 0xff, 0xff, 0xe0, 0x07, 0xf0, 0xff,
//    0xff, 0xff, 0x83, 0xc0, 0x00, 0x00, 0xff, 0x83, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x3f,
//    0x00, 0x7f, 0xff, 0xff, 0x80, 0x38, 0xf0, 0xff, 0xff, 0xff, 0x03, 0xc0, 0x00, 0x00, 0xff, 0x07,
//    0xff, 0xff, 0xff, 0xff, 0xfc, 0x1f, 0x80, 0x1f, 0x80, 0x3f, 0xff, 0xff, 0x81, 0xf0, 0x61, 0xff,
//    0xff, 0xff, 0x07, 0xc0, 0x00, 0x00, 0xff, 0x07, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0xc0, 0x1f,
//    0x80, 0x3f, 0xff, 0xff, 0x83, 0xe0, 0x61, 0xff, 0xff, 0xfe, 0x0f, 0xc0, 0x00, 0x00, 0xff, 0x07,
//    0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0xc0, 0x1f, 0x80, 0x3f, 0xff, 0xff, 0x87, 0xc0, 0x01, 0xff,
//    0xff, 0xfe, 0x0f, 0xc0, 0x00, 0x00, 0xfe, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0xe0, 0x0f,
//    0x80, 0x3f, 0xff, 0xff, 0x87, 0x80, 0x03, 0xff, 0xff, 0xfc, 0x1f, 0xc0, 0x00, 0x00, 0xfe, 0x1f,
//    0xff, 0xff, 0xff, 0xff, 0xf8, 0x07, 0xe0, 0x0f, 0xc0, 0x3f, 0xff, 0xff, 0x82, 0x02, 0x03, 0xff,
//    0xff, 0xf8, 0x1f, 0xc0, 0x00, 0x00, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xf3, 0xf8, 0x03, 0xe0, 0x07,
//    0xc0, 0x1f, 0xff, 0xff, 0xc0, 0x0f, 0x03, 0xff, 0xff, 0xf0, 0x3f, 0xc0, 0x00, 0x00, 0xfc, 0x3f,
//    0xff, 0xff, 0xff, 0xc0, 0xf8, 0x01, 0xf0, 0x07, 0xc0, 0x1f, 0xff, 0xff, 0xe0, 0x1e, 0x07, 0xff,
//    0xff, 0xf0, 0x7f, 0xc0, 0x00, 0x00, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0x80, 0xfc, 0x01, 0xf0, 0x07,
//    0xc0, 0x1f, 0xff, 0xff, 0xc0, 0x78, 0x0f, 0xff, 0xff, 0xe0, 0xff, 0xc0, 0x00, 0x00, 0xf8, 0x7f,
//    0xff, 0xff, 0xff, 0x80, 0x7e, 0x01, 0xf8, 0x07, 0xc0, 0x1f, 0xff, 0xff, 0x80, 0xf0, 0x1f, 0xff,
//    0xff, 0xc0, 0xff, 0xc0, 0x00, 0x00, 0xf0, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x3e, 0x00, 0xf8, 0x03,
//    0xe0, 0x1f, 0xff, 0xff, 0x01, 0xc0, 0x7f, 0xff, 0xff, 0xc1, 0xff, 0xc0, 0x00, 0x00, 0xf0, 0xff,
//    0xff, 0xff, 0xff, 0x80, 0x3e, 0x00, 0xf8, 0x03, 0xe0, 0x1f, 0xff, 0xfe, 0x07, 0x80, 0xff, 0xff,
//    0xff, 0x83, 0xff, 0xc0, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x1f, 0x00, 0x78, 0x03,
//    0xe0, 0x0f, 0xff, 0xfe, 0x0f, 0x03, 0xff, 0xff, 0xff, 0x07, 0xff, 0xc0, 0x00, 0x00, 0xe0, 0xff,
//    0xff, 0xff, 0xff, 0xe0, 0x1f, 0x00, 0x7c, 0x03, 0xe0, 0x0f, 0xff, 0xfc, 0x1c, 0x07, 0xff, 0xff,
//    0xff, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0x00, 0x7c, 0x01,
//    0xe0, 0x0f, 0xff, 0xfc, 0x18, 0x0f, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0xe0, 0xff,
//    0xff, 0xff, 0xff, 0xf0, 0x0f, 0x00, 0x7c, 0x01, 0xe0, 0x0f, 0xff, 0xfe, 0x18, 0x1f, 0xff, 0xff,
//    0xfe, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0x00, 0x7c, 0x01,
//    0xe0, 0x0f, 0xff, 0xfe, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xff,
//    0xff, 0xff, 0xff, 0xf8, 0x0f, 0x80, 0x7c, 0x01, 0xe0, 0x0f, 0xff, 0xfc, 0x00, 0x7f, 0xfe, 0x7f,
//    0xf8, 0x3f, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0x80, 0x7c, 0x01,
//    0xe0, 0x0f, 0xfc, 0x00, 0x01, 0xff, 0xfc, 0x3f, 0xf0, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xff,
//    0xff, 0xff, 0xff, 0xf8, 0x0f, 0x80, 0x3c, 0x01, 0xe0, 0x0f, 0xe0, 0x00, 0x03, 0xff, 0xf0, 0x3f,
//    0xf0, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0x80, 0x3c, 0x01,
//    0xe0, 0x0f, 0x80, 0x00, 0x0f, 0xff, 0xe0, 0x3f, 0xe0, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xff,
//    0xff, 0xff, 0xff, 0xf8, 0x0f, 0x80, 0x3c, 0x01, 0xe0, 0x0f, 0x80, 0x00, 0x1f, 0xff, 0x80, 0xff,
//    0xc1, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0x80, 0x3c, 0x01,
//    0xe0, 0x0f, 0x83, 0xe0, 0x3f, 0xfe, 0x01, 0xff, 0xc3, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xff,
//    0xff, 0xff, 0xff, 0xf0, 0x0f, 0x80, 0x7c, 0x01, 0xe0, 0x0f, 0x87, 0xc0, 0xff, 0xf8, 0x07, 0xff,
//    0x87, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0x80, 0x7c, 0x01,
//    0xe0, 0x0f, 0x87, 0x81, 0xff, 0xe0, 0x1f, 0xff, 0x07, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xff,
//    0xff, 0xff, 0xff, 0xf0, 0x0f, 0x00, 0x7c, 0x01, 0xe0, 0x0f, 0x87, 0x83, 0xff, 0xc0, 0x3f, 0xfe,
//    0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x1f, 0x00, 0x7c, 0x01,
//    0xe0, 0x0f, 0xc3, 0x87, 0xff, 0x00, 0x7f, 0xfe, 0x1f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xe0, 0xff,
//    0xff, 0xff, 0xff, 0xe0, 0x1f, 0x00, 0x7c, 0x01, 0xe0, 0x0f, 0xc3, 0x87, 0xfe, 0x00, 0xff, 0xfe,
//    0x1f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x1f, 0x00, 0x7c, 0x03,
//    0xe0, 0x0f, 0xc3, 0x83, 0xfc, 0x01, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xf0, 0xff,
//    0xff, 0xff, 0xff, 0x80, 0x1f, 0x00, 0xfc, 0x03, 0xe0, 0x0f, 0xc3, 0x83, 0xf0, 0x03, 0xff, 0xf8,
//    0x3f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x80, 0x3e, 0x00, 0xf8, 0x03,
//    0xe0, 0x1f, 0xc3, 0xc0, 0xc0, 0x00, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xf0, 0x7f,
//    0xff, 0xff, 0xff, 0x80, 0x3e, 0x00, 0xf8, 0x03, 0xe0, 0x1f, 0xc1, 0xe0, 0x00, 0x60, 0x00, 0x00,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xf0, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x7c, 0x01, 0xf8, 0x07,
//    0xc0, 0x1f, 0xe1, 0xf0, 0x01, 0xe0, 0x00, 0x01, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xf8, 0x3f,
//    0xff, 0xff, 0xff, 0xc0, 0xfc, 0x01, 0xf0, 0x07, 0xc0, 0x1f, 0xe1, 0xf0, 0x07, 0xf0, 0x00, 0x07,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xf1, 0xf8, 0x03, 0xf0, 0x07,
//    0xc0, 0x1f, 0xe1, 0xfc, 0x1f, 0xf0, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xfc, 0x1f,
//    0xff, 0xff, 0xff, 0xff, 0xf8, 0x07, 0xf0, 0x07, 0xc0, 0x1f, 0xe0, 0xff, 0xff, 0xf1, 0xe1, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x07, 0xe0, 0x0f,
//    0xc0, 0x3f, 0xf0, 0xff, 0xff, 0x01, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xfe, 0x0f,
//    0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0xe0, 0x0f, 0x80, 0x3f, 0xf0, 0xff, 0xc0, 0x01, 0xc3, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xfe, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x1f, 0xc0, 0x1f,
//    0x80, 0x3f, 0xf0, 0x60, 0x00, 0x03, 0x87, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xfe, 0x0f,
//    0xff, 0xff, 0xff, 0xff, 0xfc, 0x1f, 0xc0, 0x1f, 0x80, 0x3f, 0xf0, 0x00, 0x00, 0x03, 0x87, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xc0, 0x1f,
//    0x80, 0x3f, 0xf8, 0x00, 0x00, 0x07, 0x07, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0x83,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x3f, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0xfe, 0x0f, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x3f,
//    0x00, 0x7f, 0xfc, 0x00, 0x3f, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xc0,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x7e, 0x00, 0xff, 0xff, 0x1f, 0xff, 0xfc, 0x3f, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0xfe,
//    0x00, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xf0,
//    0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xfe, 0x00, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x7f, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc,
//    0x01, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xfc,
//    0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x01, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8,
//    0x03, 0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc,
//    0x07, 0xff, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xe0, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc,
//    0x0f, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xf0, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xc0, 0x7f, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xfc, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xfe, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xe0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xfe, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xfe, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x01, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x80, 0x0f, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0x80, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xe0, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x0f, 0xff, 0xff, 0xff,
//    0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x0f, 0xff, 0xff,
//    0xc0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x01,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//    0xff, 0xff, 0xff, 0xc0, 0x00, 0x00
//};

//黑白屏s980logo
char s980rfidlogo[] = {
0x42, 0x4d, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x28, 0x00,
0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0xc8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff,
0x00, 0x00, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff,
0xf9, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xc7, 0xff, 0xff, 0xff,
0x8f, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff,
0x00, 0x00, 0xff, 0xfe, 0x7f, 0xff, 0xfc, 0xff, 0xfd, 0xfc, 0x7f, 0xff, 0x00, 0x00, 0xff, 0xfc,
0xff, 0xff, 0xfc, 0x7f, 0xfc, 0xf9, 0xff, 0xff, 0x00, 0x00, 0xff, 0xfb, 0xff, 0xff, 0xfc, 0x3f,
0xfe, 0x77, 0xff, 0xff, 0x00, 0x00, 0xff, 0xf7, 0xff, 0xff, 0x8e, 0x3f, 0xf8, 0xe7, 0xff, 0xff,
0x00, 0x00, 0xff, 0xef, 0xff, 0xff, 0xce, 0x3f, 0xf3, 0xaf, 0xfc, 0xff, 0x00, 0x00, 0xff, 0xcf,
0xff, 0xf1, 0xc6, 0x3f, 0xf6, 0x0f, 0xfd, 0xff, 0x00, 0x00, 0xff, 0xdf, 0xff, 0xf0, 0xe7, 0x1f,
0xf8, 0x9f, 0xfb, 0xff, 0x00, 0x00, 0xff, 0xdf, 0xff, 0x38, 0xc3, 0x1f, 0xf3, 0x3f, 0xf3, 0xff,
0x00, 0x00, 0xff, 0x9f, 0xff, 0x1c, 0xe3, 0x1f, 0xe6, 0x7f, 0xe7, 0xff, 0x00, 0x00, 0xff, 0xbf,
0xff, 0x9c, 0x63, 0x1f, 0xed, 0xff, 0xef, 0xff, 0x00, 0x00, 0xff, 0xbf, 0xff, 0x8c, 0x63, 0x1f,
0xa3, 0xff, 0xdf, 0xff, 0x00, 0x00, 0xff, 0xbf, 0xff, 0xcc, 0x61, 0x1c, 0x0f, 0xe7, 0x9f, 0xff,
0x00, 0x00, 0xff, 0xbf, 0xff, 0x9c, 0x63, 0x1d, 0xdf, 0x9f, 0xbf, 0xff, 0x00, 0x00, 0xff, 0xbf,
0xff, 0x9c, 0x63, 0x1d, 0xbe, 0x3f, 0x7f, 0xff, 0x00, 0x00, 0xff, 0x9f, 0xff, 0x1c, 0x63, 0x1d,
0x98, 0x7e, 0x7f, 0xff, 0x00, 0x00, 0xff, 0x9f, 0xff, 0x38, 0xc3, 0x1d, 0xc3, 0x01, 0xff, 0xff,
0x00, 0x00, 0xff, 0xdf, 0xff, 0xf1, 0xc3, 0x1c, 0xff, 0x27, 0xff, 0xff, 0x00, 0x00, 0xff, 0xcf,
0xff, 0xf1, 0xc6, 0x3e, 0xfe, 0x4f, 0xff, 0xff, 0x00, 0x00, 0xff, 0xe7, 0xff, 0xff, 0x8e, 0x1e,
0x01, 0xdf, 0xff, 0xff, 0x00, 0x00, 0xff, 0xf3, 0xff, 0xff, 0x8e, 0x3f, 0xff, 0x9f, 0xff, 0xff,
0x00, 0x00, 0xff, 0xf9, 0xff, 0xff, 0xdc, 0x3f, 0xff, 0x3f, 0xff, 0xff, 0x00, 0x00, 0xff, 0xfc,
0xff, 0xff, 0xfc, 0x7f, 0xfe, 0x7f, 0xff, 0xff, 0x00, 0x00, 0xff, 0xfe, 0x7f, 0xff, 0xfc, 0xff,
0xf9, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff,
0x00, 0x00, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff,
0xf1, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0xe1,
0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff,
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x00, 0x00
};

char rfidlogo[] = {
  0x42, 0x4d, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00,
  0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x4b, 0x00, 0x00, 0x00, 0x26, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x01,
  0x00, 0x00, 0xc4, 0x0e, 0x00, 0x00, 0xc4, 0x0e, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
  0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xff, 0x80, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0x87, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0x9f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xfe, 0x7f, 0xff, 0xf7, 0xff, 0xdf, 0x8f, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xfe, 0xff, 0xff, 0xf3, 0xff, 0xcf, 0x3f, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xfb, 0xff, 0xff, 0xf1, 0xff, 0xee, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xfb, 0xff, 0xfe, 0x39, 0xff, 0x8c, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xe7, 0xff, 0xff, 0x39, 0xff, 0x31, 0xff, 0x3f, 0xe0,
  0x00, 0x00, 0xff, 0xef, 0xff, 0xe3, 0x98, 0xff, 0xc1, 0xff, 0x7f, 0xe0,
  0x00, 0x00, 0xff, 0xef, 0xff, 0xe3, 0x9c, 0xff, 0x93, 0xfe, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xcf, 0xfe, 0x33, 0x8c, 0xff, 0x67, 0xfc, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xdf, 0xff, 0x39, 0x8c, 0xfe, 0xdf, 0xf9, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xdf, 0xff, 0x19, 0x8c, 0xfe, 0x3f, 0xfb, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xdf, 0xff, 0x99, 0x8c, 0xe0, 0xfc, 0xf7, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xdf, 0xff, 0x39, 0x8c, 0xed, 0xfb, 0xf7, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xdf, 0xff, 0x39, 0x8c, 0xe9, 0xe7, 0xcf, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xcf, 0xfe, 0x31, 0x8c, 0xec, 0x87, 0xdf, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xcf, 0xff, 0x63, 0x0c, 0xee, 0x30, 0x3f, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xef, 0xff, 0xe3, 0x18, 0xe7, 0xe4, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xe7, 0xff, 0xff, 0x38, 0xf0, 0x19, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xf3, 0xff, 0xfe, 0x38, 0xff, 0xfb, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xf9, 0xff, 0xff, 0x71, 0xff, 0xf3, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xfc, 0xff, 0xff, 0xf3, 0xff, 0xe7, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xfe, 0x7f, 0xff, 0xf7, 0xff, 0x9f, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0x83, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
  0x00, 0x00
};
#endif
int as3911_showlogo(int mode)
{
    //屏幕312x210
//    int i = 80,j = 50;
//    lcd_cls();
//    Dprintk("\r\n x 坐标");
//    i = InkeyCount(0);
//    Dprintk("\r\n y 坐标");
//    j = InkeyCount(0);

     //wp70彩屏logo
//    lcd_bmp_disp(i, j, rfidlogo);

    //黑白屏s980logo
//    lcd_bmp_disp(24, 12, rfidlogo);
//    lcd_cls();
//    kb_getkey(-1);
//    lcd_bmpmap_disp(16, 0, 100, 48, picrfidlogo);
//    kb_getkey(-1);
    return 0;
}
void rfid_dbggpio_output(GPIO_Type GPIOx,PORTPin_TypeDef pin)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = GPIOx;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_DSH;
    gpio_init.PORT_Pin = pin;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(pin);
    hw_gpio_init(&gpio_init);
}

void rfid_dbggpio_init(void)
{
    rfid_dbggpio_output(GPIOA,PORT_Pin_0);
    rfid_dbggpio_output(GPIOA,PORT_Pin_1);
    rfid_dbggpio_output(GPIOA,PORT_Pin_2 );
    rfid_dbggpio_output(GPIOA,PORT_Pin_3);
    rfid_dbggpio_output(GPIOA,PORT_Pin_4);
    SETSIGNAL_L();
    SETSIGNAL1_L();
    SETSIGNAL2_L();
    SETSIGNAL3_L();
    SETSIGNAL4_L();
}

extern uint gas3911emvtb145flg;
extern uchar gTypeBmodulation;
extern int test_err(int mode);
int test_as3911_emv(int mode)
{
    volatile int key = 0;  //由于IAR编译器优化 当key 不加 volatile修饰时 选择任何一项 总是硬件错误 原因不明
    volatile int parmflg = 0;  //设置后采用掉电保存的数据
    int i,j,k,m,ret,flg = 0;
    uchar buf[64];
    uchar lookuptable[12];
    uchar ucVal;
    uint count, succ,fail;
    EmvPicc_t picc;
	long long timer;

    rfid_dbggpio_init();
    test_as3911_ioinit();
    s_as3911_init();
//    as3911Initialize();
//    as3911_InitTimer(AS3911_TIMER);
    INIT_DEBUGBUF();
    guiDebugFlg = 0;
    CLRBUF(lookuptable);
    memset(&gemvdebugflg, 0, sizeof(RFID_EMV_DEBUGFLG));
    memset(&gemvcardinfo, 0, sizeof(RFID_EMV_CARDINFO));
    memset(gas3911paramtable, 0, sizeof(RFID_EMV_REGCONFIG)*4);

    gas3911paramtable[0].loadflg = 1;
    gas3911paramtable[0].reg2Ah = 0xF8;
    gas3911paramtable[0].reg27h = 0;  //rfo max
    gas3911paramtable[0].reg21h = 0x90;
    gas3911paramtable[0].reg02h = 0xE8;
    gas3911paramtable[0].reg01h = 0x00;
    gas3911paramtable[0].reg0Ah = 0x00;
    gas3911paramtable[0].reg0Bh = 0x12;
    gas3911paramtable[0].reg0Ch = 0xE0;
    gas3911paramtable[0].reg0Dh = 0x00;
    memcpy(gas3911paramtable[0].regtable, gas3911paramdemo, sizeof(gas3911paramdemo));

    sys_beep();
    test_as3911_menu();
//    lcd_bmp_disp(80, 50, rfidlogo);
    while ( 1 ) {
        kb_flush();
//        as3911_showlogo(0);
//        lcd_display(0,0*FONT_SIZE8,DISP_CLRLINE | FONT_SIZE8 | DISP_MEDIACY ,"qpboc test:%d ",EG_mifs_tWorkInfo.RFIDModule);
//        Dprintk("\r\n\r\n===============as3911 emv test===============\r\n");
//        lcd_cls();
//        lcd_display(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "==QPBOC TEST==");
//        lcd_display(0, DISP_FONT_LINE2,DISP_CLRLINE | DISP_FONT,"1-digital test");
//        lcd_display(0, DISP_FONT_LINE4,DISP_CLRLINE | DISP_FONT,"2-carrier on/off");
//        lcdDispMultiLang(0,3*FONT_SIZE12,DISP_CLRLINE | FONT_SIZE12,"3-field reset","2-Field reset");

#if 11
        if(flg == 0)
        {
            flg = 1;
            key = 1;
        }
        else
            key = InkeyCount(0);
#else
        if ( flg ) {
            key = kb_getkey(-1);
        }else
            key = InkeyCount(0); //由于IAR编译器优化 当key 不加 volatile修饰时 选择任何一项 总是硬件错误 原因不明
#endif
        switch(key)
        {
        case 0:
            test_as3911_menu();
            break;
        case 1:
            emvHalActivateField(ON);
            i = 0;
            j = 10;
            while ( j-- ) {
                hw_gpio_set_bits(GPIOA,1<<i);
                s_DelayMs(50);
                hw_gpio_reset_bits(GPIOA,1<<i); 
                s_DelayMs(50);
            }

            kb_flush();
            console_clear();
            INIT_DEBUGBUF();
            guiDebugFlg = 0;
//            dlcom_clear(gucDebugCom);

            //load the table
            test_loadas3911xytable(1);

//            test_loadas3911_typeBAUTOMATIC(0);
//            if ( parmflg == 0 ) {
//                memset(buf,0,sizeof(buf));
//                Read3911_new(sizeof(MODULE_RFIDEMV_INFO),buf);
//                as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, buf[12]);
//                as3911SetModulationLevelMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
//                as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONF, 0x80);//固定type b调制深度
//                as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, buf[13]);
//            }else{
////                as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, 0xBF);
////                as3911SetModulationLevelMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
////                as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONF, 0x80);//固定type b调制深度
////                as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, 0xD6);
//            }
            sys_beep();
            s_DelayMs(100);

            gas3911emvtb145flg = 1;
            emvGuiDigital();
            emvGuiTestApplicationRequested = 0;
            gas3911emvtb145flg = 0;

            vDispBufKey(guiDebugi,gcDebugBuf);
            DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);

            i = 0;
            j = 10;
            while ( j-- ) {
                hw_gpio_set_bits(GPIOA,1<<i);
                s_DelayMs(50);
                hw_gpio_reset_bits(GPIOA,1<<i); 
                s_DelayMs(50);
            }
            break;
        case 2:
            INIT_DEBUGBUF();
            guiDebugFlg = 0;
//            i = 1;
//            j = 10;
//            while ( j-- ) {
//                hw_gpio_set_bits(GPIOA,1<<i);
//                s_DelayMs(50);
//                hw_gpio_reset_bits(GPIOA,1<<i); 
//                s_DelayMs(50);
//            }
            emvHalActivateField(OFF);
            break;
        case 3:
            Dprintk("\r\n\r\n ------config the emv parameter-------");
            Dprintk("\r\n------chose the parmeter 0~3");
            i = InkeyCount(0);
            if ( i>3 ) {
                i = 3;
            }
            if ( gas3911paramtable[i].loadflg == 1 ) {
//                DISPBUF((uchar *)&gas3911paramtable[i], sizeof(RFID_EMV_REGCONFIG),0);
//                CLRBUF(buf);
//                //            buf[0] = APPL_COM_EMV_POLL;
//                memcpy(buf, gas3911paramdemo, sizeof(gas3911paramdemo));
//                memcpy(lookuptable, &gas3911paramtable[i].regtable[14], 12);
//                memcpy(&buf[14], lookuptable, sizeof(lookuptable));
//                DISPBUF(buf, sizeof(gas3911paramtable[i].regtable), 0);
//                ucVal = 0;
//                applProcessCmd(buf, sizeof(gas3911paramtable[i].regtable), buf, &ucVal);

                test_loadas3911regtable(i, (char *)&gas3911paramtable[i].regtable[14]);

                Dprintk("\r\n\r\n ------loading succeed!!!\r\n");
            }else{
                Dprintk("\r\n\r\n ------loading failure!!!\r\n");
            }
            break;
        case 4:
            Dprintk("\r\n\r\n -----set the electric register-----");
            as3911ReadRegister(AS3911_REG_REGULATOR_CONTROL, &ucVal);
            Dprintk("\r\n --2Ah: %02x",ucVal);
            as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &ucVal);
            Dprintk("\r\n --27h: %02x",ucVal);
            as3911ReadRegister(AS3911_REG_ANT_CAL_CONTROL, &ucVal);
            Dprintk("\r\n --21h: %02x",ucVal);
            as3911ReadRegister(AS3911_REG_OP_CONTROL, &ucVal);
            Dprintk("\r\n --02h: %02x",ucVal);
            as3911ReadRegister(AS3911_REG_IO_CONF2, &ucVal);
            Dprintk("\r\n --01h: %02x",ucVal);
            as3911ReadRegister(AS3911_REG_RX_CONF1, &ucVal);
            Dprintk("\r\n --0Ah: %02x",ucVal);
            as3911ReadRegister(AS3911_REG_RX_CONF2, &ucVal);
            Dprintk("\r\n --0Bh: %02x",ucVal);
            as3911ReadRegister(AS3911_REG_RX_CONF3, &ucVal);
            Dprintk("\r\n --0Ch: %02x",ucVal);
            as3911ReadRegister(AS3911_REG_RX_CONF4, &ucVal);
            Dprintk("\r\n --0Dh: %02x",ucVal);


            Dprintk("\r\n\r\n -----chose the table  0-no 1/2/3-yes");
            i = InkeyCount(0);
            if ( i == 0 ) {
                break;
            }
            gas3911paramtable[i].loadflg = 1;
            Dprintk("\r\n --2Ah");
            gas3911paramtable[i].reg2Ah = InkeyHex(0);
            as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &ucVal);
            gas3911paramtable[i].reg27h = ucVal;
            Dprintk("\r\n --21h");
            gas3911paramtable[i].reg21h = InkeyHex(0);
            Dprintk("\r\n --02h");
            gas3911paramtable[i].reg02h = InkeyHex(0);
            Dprintk("\r\n --01h");
            gas3911paramtable[i].reg01h = InkeyHex(0);
            Dprintk("\r\n --0Ah");
            gas3911paramtable[i].reg0Ah = InkeyHex(0);
            Dprintk("\r\n --0Bh");
            gas3911paramtable[i].reg0Bh = InkeyHex(0);
            Dprintk("\r\n --0Ch");
            gas3911paramtable[i].reg0Ch = InkeyHex(0);
            Dprintk("\r\n --0Dh");
            gas3911paramtable[i].reg0Dh = InkeyHex(0);

//            test_loadas3911regtable(i, NULL);
            break;
        case 5:
            Dprintk("\r\n\r\n ---------make the look up table-------");
            ucVal = 0;
            as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &ucVal);
            Dprintk("\r\n\r\n --------enter the RFO Normal Level (27h) :%x-------",ucVal);
            Dprintk("\r\n\r\n -------config the 27h 0-no 1-yes");
            if ( InkeyCount(0) != 0) {
                ucVal = InkeyHex(0);
                as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, ucVal);
                s_DelayMs(50);
            }

            Dprintk("\r\n\r\n -------config the lookup table? 0-no 1-yes");
            if ( InkeyCount(0) == 0) {
                break;
            }

            Dprintk("\r\n\r\n ----x table----");
            CLRBUF(lookuptable);
            for ( i=0 ; i<6 ; i++  ) {
                if ( InkeyCount(0) == 1 ) {
                    break;
                }
                as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
                as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
                as3911ExecuteCommand(AS3911_CMD_MEASURE_AMPLITUDE);
                as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 20, (ulong *)&j);
                Dprintk("\r\n\r\n interrupt flag  :%x",j);
                if (j != AS3911_IRQ_MASK_DCT)
                {
                    /* ToDo: enter debug code here. */
                    Dprintk("\r\n\r\n ---------get the interrupt status error-------  :%x",j);
                }
                as3911DisableInterrupts(AS3911_IRQ_MASK_DCT);
                as3911ReadRegister(AS3911_REG_AD_RESULT, &ucVal);
                Dprintk("\r\n\r\n %d the ad value  :%x", i, ucVal);
                lookuptable[i*2] = ucVal;
            }

            Dprintk("\r\n\r\n ----y table----");
            as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x1E);//type b调制深度11%
            for ( i=0 ; i<6 ; i++  ) {
                if ( InkeyCount(0) == 1 ) {
                    break;
                }
                as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
                as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
                as3911ExecuteCommand(AS3911_CMD_CALIBRATE_MODULATION);
                as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 20, (ulong *)&j);
                Dprintk("\r\n\r\n interrupt flag  :%x",j);
                if (j != AS3911_IRQ_MASK_DCT)
                {
                    /* ToDo: enter debug code here. */
                    Dprintk("\r\n\r\n ---------get the interrupt status error-------  :%x",j);
                }
                as3911DisableInterrupts(AS3911_IRQ_MASK_DCT);
                as3911ReadRegister(AS3911_REG_AM_MOD_DEPTH_RESULT, &ucVal);
                Dprintk("\r\n\r\n %d the 25h value  :%x", i, ucVal);
                lookuptable[i*2+1] = ucVal;
            }
            as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x80);
            DISPBUF("lookuptable", 12, 0, lookuptable);
            for ( i = 0 ; i < 6 ; i++ ) {
                Dprintk("\r\n x[%d]:%x    y[%d]:%x",i,lookuptable[i*2],i,lookuptable[i*2+1]);
            }

            Dprintk("\r\n\r\n load the parametertable ?  0-no  1-table1  2-table2  3-table3");
            i = InkeyCount(0);
            if ( i ) {
                gas3911paramtable[i].loadflg = 1;
                as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &ucVal);
                gas3911paramtable[i].reg27h = ucVal;
                memcpy(&gas3911paramtable[i].regtable[14], lookuptable, sizeof(lookuptable));
            }
            break;
        case 6:
            Dprintk("\r\n\r\n -----save the configuration  0-no 1/2/3-yes");
            i = InkeyCount(0);
            if ( i ) {
                if ( i > 3 ) {
                    i = 3;
                }
                CLRBUF(gcDebugBuf);
//                j = Write3911_new(28, (uchar *)&gas3911paramtable[i]);
                j = Write3911_new(12, (uchar *)&gas3911paramtable[i].regtable[14]);
                Dprintk("\r\n\r\n -----save result:%d",j);
            }
            CLRBUF(gcDebugBuf);
//            Read3911_new(sizeof(gas3911paramtable),gcDebugBuf);
//            for ( i = 0 ; i < 4 ; i++ ) {
//                DISPBUF(gcDebugBuf+i*sizeof(RFID_EMV_REGCONFIG), sizeof(RFID_EMV_REGCONFIG),0);
//            }
            Read3911_new(12,gcDebugBuf);
            DISPBUF("xytable", 12, 0, gcDebugBuf);
            break;
        case 7:
            CLRBUF(gcDebugBuf);
//            Read3911_new(sizeof(gas3911paramtable),gcDebugBuf);
////            for ( i = 0 ; i < 4 ; i++ ) {
////                DISPBUF(gucDebugBuf+i*sizeof(RFID_EMV_REGCONFIG), sizeof(RFID_EMV_REGCONFIG),0);
////            }
//            for ( i = 1 ; i < 4 ; i++ ) {
//                memcpy(&gas3911paramtable[i], gcDebugBuf+i*sizeof(RFID_EMV_REGCONFIG),sizeof(RFID_EMV_REGCONFIG));
//            }
//            for ( i=0 ; i<4 ; i++  ) {
//                DISPBUF((uchar *)&gas3911paramtable[i],sizeof(RFID_EMV_REGCONFIG),0);
//            }
            Read3911_new(12,gcDebugBuf);
            i = 1;
            memcpy(&gas3911paramtable[i].regtable[14], gcDebugBuf, 12);
            DISPBUF("xytable", 12, 0, gcDebugBuf);

            break;
        case 8:
//            default set
            Dprintk("\r\n\r\n -----set the registers -----");
            Dprintk("\r\n\r\n -----input the count -----");
            j = InkeyCount(0);
            for ( i=0 ; i<j ; i++  ) {
                Dprintk("\r\n add:");
                buf[i] = InkeyHex(0);
                Dprintk("\r\n value:");
                ucVal = InkeyHex(0);
                as3911WriteRegister(buf[i], ucVal);
            }

            for ( i=0 ; i<j ; i++  ) {
                as3911ReadRegister(buf[i], &ucVal);
                Dprintk("\r\n add:%02Xh:%02X",buf[i],ucVal);
            }

//            applProcessCmd(gas3911paramdemo, sizeof(gas3911paramdemo), buf, &ucVal);
//            as3911ExecuteCommand(AS3911_CMD_SET_DEFAULT);
//            s_DelayMs(100);
//			as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x1E);

//            as3911SetModulationLevelMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
//            emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
//            as3911SetGainMode(AS3911_GAIN_FIXED, NULL);

            break;
        case 9:
            Dprintk("\r\n\r\n -----set the table and default register-----");
            CLRBUF(gcDebugBuf);
            Dprintk("\r\n\r\n table loading the flash  0-no  1-yes");
            i = InkeyCount(0);
            if ( i != 0 ) {
            	Read3911_new(sizeof(gas3911paramtable),gcDebugBuf);
                for ( i = 1 ; i < 4 ; i++ ) {
                    memcpy(&gas3911paramtable[i], gcDebugBuf+i*sizeof(RFID_EMV_REGCONFIG),sizeof(RFID_EMV_REGCONFIG));
                }
                for ( i=0 ; i<4 ; i++  ) {
                    DISPBUF("data",sizeof(RFID_EMV_REGCONFIG),0,(uchar *)&gas3911paramtable[i]);
                }
            }

            Dprintk("\r\n\r\n -----chose the table 0~4");
            i = InkeyCount(0);
            if ( i>3 ) {
                i = 3;
            }
            memcpy(lookuptable, &gas3911paramtable[i].regtable[14],sizeof(lookuptable));
            mainModulationTable.length = 6;
			for (i = 0; i < mainModulationTable.length; i++)
			{
				mainModulationTable.x[i] = lookuptable[i*2];
				mainModulationTable.y[i] = lookuptable[i*2+1];
			}
            DISPBUF("x table", 6, 0, mainModulationTable.x);
            DISPBUF("y table", 6, 0, mainModulationTable.y);
			as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x80);
			emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_FROM_AMPLITUDE, &mainModulationTable);
            break;
        case 10:
            for (i = 0; i < 0x3D; i++)
                displayRegisterValue(i);
            break;
        case 11:
            i = 0;
//            lcd_cls();
//            as3911_showlogo(0);
            test_loadas3911xytable(1);
//            test_loadas3911_typeBAUTOMATIC(0);
//            as3911SetModulationLevelMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
//            as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x80);//固定type b调制深度
//            as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, 0x80);
            while ( 1 ) {
                CLRBUF(buf);
                INIT_DEBUGBUF();
//                emvHalResetField();
//                s_DelayMs(500);
                emvHalSleepMilliseconds (EMV_T_P);
                if ( IfInkey(0) ) {
                    //dlcom_clear(gucDebugCom);
                    break;
                }
                ret = s_emvGuiWupa(buf);
//                Dprintk("\r\n --type a:%d\r\n",++i);
                if ( ret == 0 ) {
                    DISPBUF("wupa", 2, 0, buf);
                    sys_beep();
                    s_emvGuiHaltA();
//                    DISPBUF(buf, 2, 0);
                }
//                DISPBUF(gucDebugBuf, guiDebugS3, 0);
//                vDispBufKey(guiDebugS3,gucDebugBuf);
//                guiDebugS3 = 0;
                s_DelayMs(100);
            }
            break;
        case 12:
            i = 0;
//            lcd_cls();
//            as3911_showlogo(0);
            test_loadas3911xytable(1);
//            test_loadas3911_typeBAUTOMATIC(0);
//            as3911SetModulationLevelMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
//            as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x80);//固定type b调制深度
//            as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, 0x80);
            while ( 1 ) {
                CLRBUF(buf);
                INIT_DEBUGBUF();
//                emvHalResetField();
//                s_DelayMs(500);
                emvHalSleepMilliseconds (EMV_T_P);
                if ( IfInkey(0) ) {
                    //dlcom_clear(gucDebugCom);
                    break;
                }
                ret = s_emvGuiWupb(&ucVal, buf);
//                Dprintk("\r\n --type b:%d\r\n",++i);
                if ( ret == 0 ) {
                    DISPBUF("wupb", 13, 0, buf);
                    sys_beep();
//                    DISPBUF(buf, ucVal, 0);
                }
//                DISPBUF(gucDebugBuf, guiDebugS3, 0);
//                vDispBufKey(guiDebugS3,gucDebugBuf);
//                guiDebugS3 = 0;
                s_DelayMs(100);
            }
            break;
        case 13:
            i = 0;
            while ( 1 ) {
                CLRBUF(buf);
                INIT_DEBUGBUF();
//                emvHalResetField();
                s_DelayMs(500);
//                Dprintk("\r\n ----type a/b:%d\r\n",++i);
                if ( IfInkey(0) ) {
                    Dprintk("\r\n ----1 to exit\r\n",++i);
                    if ( InkeyCount(0) == 1 ) {
                        break;
                    }
                }
                ret = s_emvGuiWupa(buf);
                if ( ret == 0 ) {
                    sys_beep();
                    s_emvGuiHaltA();
                    Dprintk("\r\n type a ok");
//                    DISPBUF(buf, 2, 0);
//                    DISPBUF(gucDebugBuf, guiDebugS3, 0);
                }else{
                    s_DelayMs(50);
                    ret = s_emvGuiWupb(&ucVal, buf);
                    if ( ret == 0 ) {
                        Dprintk("\r\n type b ok");
                        sys_beep();
                    }else{
//                        Dprintk("\r\n error---wupa:%x",ret);
//                        Dprintk("\r\n error---wupb:%x",ret);
                    }
                }
//                DISPBUF(buf, ucVal, 0);
//                DISPBUF(gucDebugBuf, guiDebugS3, 0);
//                vDispBufKey(guiDebugS3,gucDebugBuf);
                guiDebugi = 0;
            }
            break;
        case 14:
            Dprintk("\r\n\r\n -----set the polling type: 0-a/b  1-a  2-b");
            j = InkeyCount(0);
            Dprintk("\r\n\r\n -----set the registers -----");
            Dprintk("\r\n add:");
            lookuptable[0] = InkeyHex(0);
            Dprintk("\r\n value:");
            ucVal = InkeyHex(0);
            as3911WriteRegister(lookuptable[0], ucVal);
            i = 0;
            while ( 1 ) {
//                emvHalResetField();
                s_DelayMs(500);
                Dprintk("\r\n ----polling:%d\r\n",++i);
                if ( IfInkey(0) ) {
                    Dprintk("\r\n ----1 to exit\r\n",++i);
                    lookuptable[1] = (uchar)InkeyCount(0);
                    if (lookuptable[1]  == 1 ) {
                        break;
                    }else if ( lookuptable[1] == 0 ) {
                        as3911ReadRegister(lookuptable[0], &ucVal);
                        Dprintk("\r\n add:%x  %x",lookuptable[0],ucVal);
                        Dprintk("\r\n value:");
                        ucVal = InkeyHex(0);
                        as3911WriteRegister(lookuptable[0], ucVal);
                    }
                }
                if ( j < 2 ) {
                    ret = s_emvGuiWupa(buf);
                }else{
                    ret = 1;
                }
                if ( ret == 0 ) {
                    sys_beep();
                    s_emvGuiHaltA();
                    Dprintk("\r\n ----type a\r\n");
//                    DISPBUF(buf, 2, 0);
//                    DISPBUF(gucDebugBuf, guiDebugS3, 0);
                }else{
                    if ( j != 1 ) {
                        ret = s_emvGuiWupb(&ucVal, buf);
                    }
                    if ( ret == 0 ) {
                        sys_beep();
                        Dprintk("\r\n ----type b\r\n");
                    }else{
//                        Dprintk("\r\n error---wupa:%x",ret);
//                        Dprintk("\r\n error---wupb:%x",ret);
                    }
                }
//                DISPBUF(buf, ucVal, 0);
//                DISPBUF(gucDebugBuf, guiDebugS3, 0);
//                vDispBufKey(guiDebugS3,gucDebugBuf);
                guiDebugi = 0;
            }
            break;
        case 15:
            parmflg = 1;
            Dprintk("\r\n\r\n---set 27h");
            i = InkeyHex(0);
            as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, (uchar)i);
            break;
        case 16:
            parmflg = 1;
            as3911SetModulationLevelMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
            as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x80);//固定type b调制深度
            Dprintk("\r\n\r\n---set 26h");
            i = InkeyHex(0);
            as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, (uchar)i);
            break;
        case 17:
            parmflg = 0;
            memset(buf,0xFF,sizeof(buf));
            Dprintk("\r\n\r\n---set 27h");
            buf[12] = InkeyHex(0);
            Dprintk("\r\n\r\n---set 26h");
            buf[13] = InkeyHex(0);
            Write3911_new(sizeof(MODULE_RFIDEMV_INFO),buf);
            break;
        case 18:
//            lcd_cls();
//            lcdDispMultiLang(0,0*FONT_SIZE12,DISP_CLRLINE | FONT_SIZE12 | DISP_MEDIACY,"digital test","digital test");
//            lcdDispMultiLang(0,2*FONT_SIZE12,DISP_CLRLINE | FONT_SIZE12,"any key to exit","any key to exit");
//            lcdDispMultiLang(0,3*FONT_SIZE12,DISP_CLRLINE | FONT_SIZE12,"...","...");
//            lcdDispMultiLang(0,4*FONT_SIZE12,DISP_CLRLINE | FONT_SIZE12,"              ","              ");
            kb_flush();
            INIT_DEBUGBUF();
            guiDebugFlg = 0;
//            dlcom_clear(gucDebugCom);

            //load the table
            gas3911emvtb145flg = 0;
//            lcd_cls();
            as3911_showlogo(0);
//            test_loadas3911xytable(1);
            test_loadas3911_typeBAUTOMATIC(0);
            sys_beep();
            s_DelayMs(500);

//            for ( i = 0 ; i<31 ; i++ ) {
//                disable_dev_irq(i);
//            }
//            enable_dev_irq(TC5_ID);
//            enable_dev_irq(US3_ID);
//            enable_dev_irq(PIOC_ID);

//            if (APPL_COM_EMV_DIGITAL == emvGuiTestApplicationRequested)
            {
                emvGuiDigital();
                emvGuiTestApplicationRequested = 0;

                displayCardinfo();

                Dprintk("\r\n\r\n ------config the emv parameter-------");
                DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                vDispBufKey(guiDebugi,gcDebugBuf);
                guiDebugi = 0;
            }
            if (APPL_COM_EMV_PREVALIDATION == emvGuiTestApplicationRequested)
            {
                emvGuiPrevalidation();
                emvGuiTestApplicationRequested = 0;
            }
            break;
        case 19:
//            rfid_dbggpio_init();
//            i = 0;
//            while ( 1 ) {
//                Dprintk("\r\n\r\n----- io PA:%02d",i);
//                if ( InkeyCount(0) == 1 ) {
//                    break;
//                }
//                while ( 1 ) {
//                    if ( IfInkey(0) ) {
//                        break;
//                    }
//                    hw_gpio_set_bits(GPIOA,1<<i);
//                    s_DelayMs(50);
//                    hw_gpio_reset_bits(GPIOA,1<<i); 
//                    s_DelayMs(50);
//                }
//                i++;
//                if ( i>4 ) {
//                    i = 0;
//                }
//            }

            emvHalActivateField(ON);
            as3911_showlogo(0);
            kb_flush();
//            test_loadas3911xytable(1);
            sys_beep();
            i = 0;
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }

                emvHalSleepMilliseconds(EMV_T_P);

                if (emvTypeACardPresent())
                {
                    emvHalTransceive("\x50\x00", 2, buf, 0, (size_t*)&j, 1250, EMV_HAL_TRANSCEIVE_WITH_CRC);
                    Dprintk("\r\n----TYEP A");
                    sys_beep();
                }

                emvHalSleepMilliseconds (EMV_T_P);

                if (emvTypeBCardPresent())
                {
                    sys_beep();
                    Dprintk("\r\n----TYEP B");
                }
                s_DelayMs(500);
            }
            break;
        case 20:
            while ( 1 ) {
                Dprintk("\r\n----SFGT time test :%d",sizeof(timer));
                i = InkeyCount(0);//from 0 to 14
                if ( i >= 15 ) {
                    break;
                }
                timer = (4096 + 384)<<i;
                timer = timer*1000000;
                timer = timer/13560000;
                Dprintk("\r\n ---(4096 + 384)<<%02d: %d \ttimeMs: %d  \ttimeUs: %d",i,(4096 + 384)<<i,
                        ((4096 + 384)<<i)/13560+1, timer);

                hw_gpio_set_bits(GPIOA,1<<2);
                sleepMilliseconds(((4096 + 384)<<i)/13560+1);
                hw_gpio_reset_bits(GPIOA,1<<2);
            }
            break;
        case 21:

            as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
            as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
            as3911ExecuteCommand(AS3911_CMD_CALIBRATE_ANTENNA);
            as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 20, (ulong *)&j);
            Dprintk("\r\n\r\n interrupt flag  :%x",j);
            if (j != AS3911_IRQ_MASK_DCT)
            {
                /* ToDo: enter debug code here. */
                Dprintk("\r\n\r\n ---------get the interrupt status error-------  :%x",j);
            }
            as3911ReadRegister(AS3911_REG_ANT_CAL_RESULT, &ucVal);
            Dprintk("\r\n\r\n ----the reg23h:%x---- ",ucVal);

            break;
        case 22:
            as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
            as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
            as3911ExecuteCommand(AS3911_CMD_MEASURE_AMPLITUDE);
            as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 20, (ulong *)&j);
            Dprintk("\r\n\r\n interrupt flag  :%x",j);
            if (j != AS3911_IRQ_MASK_DCT)
            {
                /* ToDo: enter debug code here. */
                Dprintk("\r\n\r\n ---------get the interrupt status error-------  :%x",j);
            }
            as3911DisableInterrupts(AS3911_IRQ_MASK_DCT);
            as3911ReadRegister(AS3911_REG_AD_RESULT, &ucVal);
            Dprintk("\r\n\r\n reg 20h  :%x   %d",ucVal, ucVal);
            break;
        case 23:
            Dprintk("\r\n\r\n set typeA and typeB receive  :%x  %x",gtRfidProInfo.gTypeArec, gtRfidProInfo.gTypeBrec);
            Dprintk("\r\n\r\n set typeA receive :%x  %x",gtRfidProInfo.gTypeArec);
            gtRfidProInfo.gTypeArec = InkeyHex(0);
            gtRfidProInfo.gTypeArec &= 0xFC;
            Dprintk("\r\n\r\n set typeB receive :%x  %x",gtRfidProInfo.gTypeBrec);
            gtRfidProInfo.gTypeBrec = InkeyHex(0);
            gtRfidProInfo.gTypeBrec &= 0xFC;
            Dprintk("\r\n\r\n set typeA and typeB receive  :%x  %x",gtRfidProInfo.gTypeArec, gtRfidProInfo.gTypeBrec);
            break;
        case 24:
            Dprintk("\r\n\r\n set typeB modulation  :%d",gTypeBmodulation);
            gTypeBmodulation = InkeyCount(0);
            break;
        case 25:
            DISPBUF("x table", 6, 0, mainModulationTable.x);
            DISPBUF("y table", 6, 0, mainModulationTable.y);
            while ( 1 ) {
                if ( InkeyCount(0) == 1 ) {
                    break;
                }
                as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
                as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
                as3911ExecuteCommand(AS3911_CMD_MEASURE_AMPLITUDE);
                as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 20, (ulong *)&j);
                Dprintk("\r\n\r\n interrupt flag  :%x",j);
                if (j != AS3911_IRQ_MASK_DCT)
                {
                    /* ToDo: enter debug code here. */
                    Dprintk("\r\n\r\n ---------get the interrupt status error-------  :%x",j);
                }
                as3911DisableInterrupts(AS3911_IRQ_MASK_DCT);
                as3911ReadRegister(AS3911_REG_AD_RESULT, &ucVal);
                Dprintk("\r\n\r\n  the ad value  :%x",  ucVal);

                for (i = 0; i < mainModulationTable.length; i++)
                {
                    if(ucVal <= mainModulationTable.x[i])
                        break;
                }
                /* Use the last interpolation level dataset for any values outside the highest.
                 * x-value from the datasets.
                 */
                if (i == mainModulationTable.length)
                    i--;

                if (i == 0)
                    k = as3911GetInterpolatedValue(
                        mainModulationTable.x[i],
                        mainModulationTable.y[i],
                        mainModulationTable.x[i+1],
                        mainModulationTable.y[i+1],
                        ucVal);
                else
                    k = as3911GetInterpolatedValue(
                        mainModulationTable.x[i-1],
                        mainModulationTable.y[i-1],
                        mainModulationTable.x[i],
                        mainModulationTable.y[i],
                        ucVal);
                Dprintk("\r\n\r\n %d  26H :%x",i,k);
            }
            break;
        case 26:
            as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &ucVal);
            Dprintk("\r\n\r\n --------enter the RFO Normal Level (27h) :%x-------",ucVal);
            Dprintk("\r\n\r\n set 27H ");
            ucVal = InkeyHex(0);
            as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, ucVal); //根据硬件调节0x9F
            break;
        case 88:
            test_err(0);
            NVIC_SystemReset();
            break;
        case 89:
            NVIC_SystemReset();
            break;
        case 90:
            test_loadas3911xytable(1);
            ucVal = 0;
            if ( ucVal == 1 ) {
//            s_read_syszone(SZ_3911EMV_PRETESTADD,4,(uchar *)&count);
//            s_read_syszone(SZ_3911EMV_PRETESTADD+4,4,(uchar *)&succ);
//            s_read_syszone(SZ_3911EMV_PRETESTADD+8,4,(uchar *)&fail);
            }else{
                count = succ = fail = 0;
            }
            Dprintk("\r\n ----- pressure test count:%d --succ:%d --fail:%d -----",count,succ,fail);
            i = j = k = m = 0;
            guiDebugFlg = 0;
            guiDebugi = 0;
            ret = 0;
            gemvdebugflg.polldeflg = ret;
            gemvdebugflg.acolldeflg = ret;
            gemvdebugflg.activedeflg = ret;
            gemvdebugflg.apdudeflg = ret;
            ret = 0;
            while ( 1 ) {
#if 11 
                if ( IfInkey(0)) {
                    Dprintk("\r\n----press 99 to exit\r\n");
                    if ( InkeyCount(0) == 99 ) {
                        /* Received stop request, stop terminal main loop. */
                        Dprintk("\r\n ----- pressure test count:%d --succ:%d --fail:%d -----",count,succ,fail);
//                        return EMV_ERR_STOPPED;
                        if ( ucVal == 1 ) {
//                        s_write_syszone(gucDebugBuf, SZ_3911EMV_PRETESTADD, 4, (uchar *)&count);
//                        s_write_syszone(gucDebugBuf, SZ_3911EMV_PRETESTADD+4, 4, (uchar *)&succ);
//                        s_write_syszone(gucDebugBuf, SZ_3911EMV_PRETESTADD+8, 4, (uchar *)&fail);
                        }
                        break;
                    }
                }
                if (EMV_ERR_STOPPED == ret)
                {
                    /* Received stop request, stop terminal main loop. */
//                    return EMV_ERR_STOPPED;
                    if ( ucVal == 1 ) {
//                    s_write_syszone(gucDebugBuf, SZ_3911EMV_PRETESTADD, 4, (uchar *)&count);
//                    s_write_syszone(gucDebugBuf, SZ_3911EMV_PRETESTADD+4, 4, (uchar *)&succ);
//                    s_write_syszone(gucDebugBuf, SZ_3911EMV_PRETESTADD+8, 4, (uchar *)&fail);
                    }
                    break;
                }
                if (EMV_ERR_OK != ret)
                {
                    Dprintk("\r\n pollret:%d\t acollret:%d\t activeret:%d\t apduret:%d\t removeret:%d\r\n"
                           ,i, j, k, m, 0);
                    emvDisplayError(ret);

                    /* Reset field and continue with polling. */
                    emvHalResetField();

                    i = EMV_ERR_OK;
                    j = EMV_ERR_OK;
                    k = EMV_ERR_OK;
                    m = EMV_ERR_OK;
//                    removeret = EMV_ERR_OK;

                    fail++;
                    displayCardinfo();

                    Dprintk("\r\n\r\n ------config the emv parameter-------");
//                    DISPBUF(gcDebugBuf, guiDebugi, 0);
//                    vDispBufKey(guiDebugi,gcDebugBuf);
                    guiDebugi = 0;
                    sys_beep();
//                    InkeyCount(0);
                }

                Dprintk("\r\n ------------count:%d   fail:%d\r\n",count,fail);
                if ( (count%20000) == 0 ) {
                    if ( ucVal == 1 ) {
//                    s_write_syszone(gucDebugBuf, SZ_3911EMV_PRETESTADD, 4, (uchar *)&count);
//                    s_write_syszone(gucDebugBuf, SZ_3911EMV_PRETESTADD+4, 4, (uchar *)&succ);
//                    s_write_syszone(gucDebugBuf, SZ_3911EMV_PRETESTADD+8, 4, (uchar *)&fail);
                    }
                }
                count++;

                guiDebugi = 0;
                /* Polling. */
                emvDisplayMessage(EMV_M_POLLING);
                guiDebugFlg = gemvdebugflg.polldeflg;
                ret = emvPoll();
                i = ret;
                guiDebugFlg = 0;
                if (EMV_ERR_OK != ret)
                    continue;

                Dprintk("\r\n--acollision");
                /* Anticollision. */
                sleepMilliseconds(EMV_T_P);
                guiDebugFlg = gemvdebugflg.acolldeflg;
                ret = emvCollisionDetection(&picc);
                j = ret;
                guiDebugFlg = 0;
                if (EMV_ERR_OK != ret)
                    continue;

                /* Activation. */
                guiDebugFlg = gemvdebugflg.activedeflg;
                Dprintk("\r\n--active");
//                DISPBUF(picc.uid, picc.uidLength, 0);
                ret = emvActivate(&picc);
                k = ret;
                guiDebugFlg = 0;
                if (EMV_ERR_OK != ret)
                    continue;

                /* Wait for SFGT. */
                /* test sfgt real time
                 * sfgtMilliseconds         timer(ms)           standard(ms)    1/fc
                 *      0                   0.33(not use)       0.5             4480                    
                 *      1                   1.01                0.66            8960
                 *      2                   2.01                1.321           17920
                 *      3                   3.03                2.643           35840
                 *      4                   6.00                5.286           71680
                 *      5                   11.00               10.572          143360
                 *      6                   22.10               21.144          286720
                 *      7                   43.10               42.289          573440
                 *      8                   84.90               84.578          1146880
                 *      9                   170.00              169.156         2293760
                 *      10                  339.00              338.321         4587520
                 *      11                  677.00              676.625         9175040
                 *      12                  1350.00             1353.250        18350080
                 *      13                  2710.00             2706.501        36700160
                 *      14                  5411.00(warning)    5413.002        73400320
                 */
                if(picc.sfgi > 0)
                {
                    ret = (4096UL + 384) << picc.sfgi;
                    key = (ret / 13560) + 1;
                    sleepMilliseconds(key);
                }

                /* Initialize layer 4. */
                ret = emvInitLayer4(&picc);
                if (EMV_ERR_OK != ret)
                    continue;

                guiDebugFlg = gemvdebugflg.apdudeflg;
                /* Start terminal application. */
                Dprintk("\r\n--aptu");

                CLRBUF(buf);
                buf[0] = 0x00;
                buf[1] = 0x84;
                buf[2] = 0x00;
                buf[3] = 0x00;
                buf[4] = 0x04;

                ret = emvTransceiveApdu(buf, 5, &buf[5], 16, (uint *)&key);
                if ( !ret ) {
                    DISPBUF("apdu",(int)key,0,&buf[5]);
                    //            sys_beep();
                    succ++;
                }

                m = ret;
                guiDebugFlg = 0;

                if (EMV_ERR_OK != ret)
                    continue;

                emvHalResetField();
                emvHalSleepMilliseconds(EMV_T_P);
                /* Card removal. */
                //        emvDisplayMessage(EMV_M_REMOVE_CARD);
                //        ret = emvRemove(&picc);
                //        removeret = ret;
                //        if (EMV_ERR_OK != ret)
                //            continue;

#else
                guiDebugi = 0;
                memset((uchar *)&picc, 0, sizeof(EmvPicc_t));
                emvHalResetField();
                emvHalSleepMilliseconds(EMV_T_P);
                if ( IfInkey(0) ) {
                    Dprintk("\r\n\r\n test----:%d---type a:%d type b:%d",j,k,m);
                    if ( InkeyCount(0) == 1 ) {
                        break;
                    }
                }
                Dprintk("\r\n\r\n --------------type a----count:%d---succ:%d ",j,k);
                ret = emvTypeAAnticollision(&picc);
                Dprintk("\r\n\r\n type a anticollision :%d",ret);
                if ( !ret ) {
                    Dprintk("\r\n\r\n uid :%x  %x  %x  %x  %x  %x",picc.uid[0],picc.uid[1],picc.uid[2],picc.uid[3],picc.uid[4],picc.uid[5]);
                    ret = picc.activate(&picc);
                    Dprintk("\r\n\r\n activate :%d ",ret);
                    if ( !ret ) {
                        CLRBUF(buf);
                        buf[0] = 0x00;
                        buf[1] = 0x84;
                        buf[2] = 0x00;
                        buf[3] = 0x00;
                        buf[4] = 0x04;
                        i = 0;
                        //                    j = (U32_C(4096) + 384) << picc.fwi;
                        //                    Dprintk("\r\n\r\n --------time  :%d    %d",j,picc.fwi);
                        //                    ret = emvPrelayer4Transceive(buf, 5, &buf[5], 4, &i,
                        //                                                   j, EMV_HAL_TRANSCEIVE_WITH_CRC);
                        //                        s_DelayMs(50);

                        if(picc.sfgi > 0)
                        {
                            ret = (4096UL + 384) << picc.sfgi;
                            i = (ret / 13560) + 1;
                            sleepMilliseconds(i);
                        }
                        emvInitLayer4(&picc);
                        ret = emvTransceiveApdu(buf, 5, &buf[5], 16, (uint *)&i);
                        if ( !ret ) {
                            DISPBUF("apdu",i,0,&buf[5]);
                            //                            sys_beep();
                            ++k;
                        }
                    }
                }else{
                    Dprintk("\r\n\r\n --------------type b----count:%d---succ:%d ",j,m);
                    ret = emvTypeBAnticollision(&picc);
                    if ( !ret ) {
                        Dprintk("\r\n\r\n uid :%x  %x  %x  %x  %x  %x",picc.uid[0],picc.uid[1],picc.uid[2],picc.uid[3],picc.uid[4],picc.uid[5]);
                        ret = picc.activate(&picc);
                        Dprintk("\r\n\r\n activate :%d ",ret);
                        if ( !ret ) {
                            CLRBUF(buf);
                            buf[0] = 0x00;
                            buf[1] = 0x84;
                            buf[2] = 0x00;
                            buf[3] = 0x00;
                            buf[4] = 0x04;
                            i = 0;
                            //                    j = (U32_C(4096) + 384) << picc.fwi;
                            //                    Dprintk("\r\n\r\n --------time  :%d    %d",j,picc.fwi);
                            //                    ret = emvPrelayer4Transceive(buf, 5, &buf[5], 4, &i,
                            //                                                   j, EMV_HAL_TRANSCEIVE_WITH_CRC);
                            s_DelayMs(50);
                            emvInitLayer4(&picc);
                            ret = emvTransceiveApdu(buf, 5, &buf[5], 16, (uint *)&i);
                            if ( !ret ) {
                                DISPBUF("apdu",i,0,&buf[5]);
                                //                                sys_beep();
                                ++m;
                            }
                        }
                    }else{
                        Dprintk("\r\n\r\n wrong wrong-------ret  :%d   ",ret);
                        DISPBUF("dbg", guiDebugi, 0, gcDebugBuf);
                        vDispBufKey(guiDebugi,gcDebugBuf);
                        sys_beep();
                        //                        while ( 1 ) {
                        //                            sys_beep();
                        //                            if ( IfInkey(0) ) {
                        //                                if ( InkeyCount(0) == 1 ) {
                        //                                    goto testcase90;
                        //                                }else{
                        //                                    break;
                        //                                }
                        //                            }
                        //                            s_DelayMs(500);
                        //                        }
                    }
                }

                ++j;
#endif

            }
//testcase90:
            guiDebugi = 0;
            guiDebugFlg = 0;
        case 91:
            test_as3911();
            break;
        case 92:
            Dprintk("\r\n\r\n -----start the timer-----");
//            as3911_StartTimer(AS3911_TIMER);
//
//            i = InkeyCount(0);
//
//            SETSIGNAL1_H();
//            as3911_WaitForTimer(AS3911_TIMER,i);
//            SETSIGNAL1_L();
            break;
        case 93:
            Dprintk("\r\n\r\n -----update the timer-----");
//            as3911_StopTimer(AS3911_TIMER);
//            as3911_UpdateTimer(AS3911_TIMER, InkeyCount(0));
//            s_DelayMs(100);
//            as3911_StartTimer(AS3911_TIMER);
            break;
        case 94:
            Dprintk("\r\n\r\n -----set the poll flg-----%x",gemvdebugflg.polldeflg);
            gemvdebugflg.polldeflg = InkeyCount(0);
            break;
        case 95:
            Dprintk("\r\n\r\n -----set the acollision flg-----%x",gemvdebugflg.acolldeflg);
            gemvdebugflg.acolldeflg = InkeyCount(0);
            break;
        case 96:
            Dprintk("\r\n\r\n -----set the active flg-----%x",gemvdebugflg.activedeflg);
            gemvdebugflg.activedeflg = InkeyCount(0);
            break;
        case 97:
            Dprintk("\r\n\r\n -----set the apdu flg-----%x",gemvdebugflg.apdudeflg);
            gemvdebugflg.apdudeflg = InkeyCount(0);
            break;
        case 98:
            as3911ReadRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, &ucVal);
            Dprintk("\r\n\r\n -----set the 24h :%02x-----input",ucVal);
            ucVal = InkeyHex(0);
//            as3911ModifyRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x7E, 0x1E);
            as3911ModifyRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x7E, ucVal);
            as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
            as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
            as3911ExecuteCommand(AS3911_CMD_CALIBRATE_MODULATION);
            as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 20, (ulong *)&j);
            Dprintk("\r\n\r\n interrupt flag  :%x",j);
            if (j != AS3911_IRQ_MASK_DCT)
            {
                /* ToDo: enter debug code here. */
                Dprintk("\r\n\r\n ---------get the interrupt status error-------  :%x",j);
            }
            as3911ReadRegister(AS3911_REG_AM_MOD_DEPTH_RESULT, &ucVal);
            Dprintk("\r\n\r\n  the 25h value  :%x", ucVal);
            break;
        case 99:
            //system_reset();
            return 0;
            //break;
        default:
            break;
        }
    }
//    return 0;
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
#if 0
int Usb_EnumIfSucc(void)
{
	uint32_t timer,dif_val;
	int iRet=OK;

	timer = s_GetSysTime();
	while(1)
	{
		if(a_usb_ready() == USBSER_SUCCESS)
		{
			//gucComNo = UART_USB;
			//gucDebugCom = UART_USB;
			break;
		}
		dif_val = s_GetSysTime() - timer;
		if(dif_val > 5000)
		{
			//s_LedOff(LED_ALL);
			//s_LedOn(LED_RED);
			//s_DelayMs(400);
			//s_LedOff(LED_ALL);
			//s_LedOn(LED_GREEN);
			//s_DelayMs(400);
		} else if (dif_val > 6000)
		{
			iRet = -1;
			//s_LedOn(LED_ALL);
			break;
		}
	}
	TRACE("\r\n USB Enum Sus!");
	return iRet;
}
#endif
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void CtrlAppMain(void)
{
    while(1)
    {
        test_as3911_emv(0);
    }
}

#endif
#endif
#endif



