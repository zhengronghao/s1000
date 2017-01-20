/*********************************************************************
* 版权所有 (C)2003, 福建实达电脑设备有限公司。
* 文件名称：
*   Mifs.c
* 内容摘要：
*   本文件负责实现非接触卡接口层的程序
* 历史纪录：
*   修改人      日期                版本号      修改记录
*   叶华峰     2010-04-08                     模块启动时间从10s减少到3s
******************************************************************/
#include "wp30_ctrl.h"

#ifdef CFG_RFID
/**********************************************************************

                          全局变量定义

***********************************************************************/
const ulong EG_tulMaxLenList[16] =
	{ 16, 24, 32, 40, 48, 64, 96, 128, 256, 16, 16, 16, 16, 16, 16, 16 };

//const s_rfidpara_info rfidpara_rc531[4] =
//{{0,0x09}, {1,0x0B}, {2,0x34}, {3,0x3F}};
//
////寄存器27h
//const s_rfidpara_info rfidpara_FM17550_V1[4] =
//{{0,0x82}, {1,0x82}, {2,0x82}, {3,0x82}};
////寄存器28h
//const s_rfidpara_info rfidpara_FM17550_V1EX[4] =
//{{0,0x38}, {1,0x38}, {2,0x38}, {3,0x38}};
//
//#ifdef PRODUCT_F12
//const s_rfidpara_info rfidpara_as3911[4] =
//{{0,0xE2}, {1,0xCF}, {2,0xC0}, {3,0x00}};
//#else
//const s_rfidpara_info rfidpara_as3911[4] =
////{{0,0x8F}, {1,0x80}, {2,0x6F}, {3,0x00}};
////{{0,0xA0}, {1,0x90}, {2,0x80}, {3,0x70}};
//{{0,0xE0}, {1,0xDE}, {2,0xA0}, {3,0x90}};
//#endif

#define RFID_READY(x) {if(!x) return (-RFID_NODEVICE);}
/**********************************************************************

                          内部函数声明

***********************************************************************/
uchar EI_mifs_ucBeginProtect(DevHandle * phDevHandle, uchar ucMode);
void EI_mifs_vEndProtect(void);
uchar EI_mifs_ucCommand(DevHandle hDevHandle, uchar ucType, uchar ucCID, uint uiSendLen,
	void *pvSendData, uint * puiRecLen, void *pvRecData);
uchar EI_mifs_ucProcess(uchar ucType, uint uiSendLen, uint * puiRecLen);
uchar EI_mifs_ucExchange(DevHandle hDevHandle, uchar ucType, uchar ucCID, uint uiSendLen,
	void *pvSendData, uint * puiRecLen, void *pvRecData);
uchar EI_mifs_ucDeselect(uchar ucType, uchar ucCID);
uchar EI_mifs_ucSendSBlock(uchar ucType, uchar ucCID, uint * puiRecLen);
uchar EI_mifs_ucSendRBlock(uchar ucType, uchar ucCID, uint * puiRecLen);
uchar EI_mifs_ucHandleATQA(uint uiLength);
uchar EI_mifs_ucHandleATQB(uint uiLength);
uchar EI_mifs_ucAuthentication(uchar ucKeyType, uchar ucSecNr);
extern int s_rfid_getPara(int module, int para, int index, s_rfidpara_info *rfdata);
int if_rfid_module(void);
/**********************************************************************

                          API函数定义

***********************************************************************/


/**********************************************************************
* 函数名称：
*     EA_ucMifsOpen(uchar ucOpenMode)
* 功能描述：
*   打开非接触卡设备，并获得设备使用权
* 输入参数：
*   ucOpenMode          Mifs模块的打开方式
*           EM_mifs_TYPEA           以TYPEA方式打开
*           EM_mifs_TYPEB           以TYPEB方式打开
* 输出参数：
*   无
* 返回值：
*   EM_SUCCESS      成功
*   EM_ERRPARAM     参数错误
*   EM_DEVICE_USED  非接触卡模块正在使用中（已经调用函数EA_ucMifsOpen打开了设备）
*   EM_TIMEOUT      初始化时超时
*   EM_FAILED       其他错误（系统错误等）
* 历史纪录：
*   修改人          日期            版本号      修改记录
***********************************************************************/
#define  OpenRFID_TIMEOUT  2100
uchar EA_ucMifsOpen(uchar ucOpenMode)
{
	uint timer0 = 0;
	uchar data;
	uchar ucResult;
	DevHandle *phDevHandle = EM_NULL;
	ucResult = EI_mifs_ucBeginProtect(phDevHandle, 0);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

//    if ( EG_mifs_tWorkInfo.ucFirstOpenFlag == 1 ) {
//		EI_paypass_vSelectType(ucOpenMode);
//        return EM_SUCCESS;
//    }
    if ( if_rfid_module() ) {
        //没有射频模块
        return 1;
    }
    s_rfid_init();

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module  //AS3911
        data = 0;
        data = data;
//        memset(&gemvcardinfo, 0, sizeof(RFID_EMV_CARDINFO));

//        s_as3911_SetInt(OFF);
        /* Reset the AS3911 */
        as3911ExecuteCommand(AS3911_CMD_SET_DEFAULT);

        /* MCU_CLK and LF MCU_CLK off, 27MHz XTAL */
        as3911WriteRegister(AS3911_REG_IO_CONF1, 0x0F);
        as3911WriteRegister(AS3911_REG_IO_CONF2, 0x80);
        /* Enable Oscillator, Transmitter and receiver. */
        //先使能bit 7晶振和寄存器设置type b调制深度
        as3911WriteRegister(AS3911_REG_OP_CONTROL, 0xD0);
        as3911WriteRegister(AS3911_REG_MODE, 0x08);

        s_DelayMs(10);

        //clear interrupt
        s_as3911ReadRegister(3, AS3911_REG_IRQ_MAIN, (uchar*)&timer0);

        //type b 设置调制深度恢复默认 各个调试表恢复默认
        as3911SetModulationLevelMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
        emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
        as3911SetGainMode(AS3911_GAIN_FIXED, NULL);

//        as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, 0x00); //根据硬件调节0x9F
        as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, gtRfidDebugInfo.CW_A); //根据参数区获取

        as3911WriteRegister(AS3911_REG_ANT_CAL_CONTROL, 0x80); //0x21 设置最大
        as3911ModifyRegister(AS3911_REG_RX_CONF3, 0xE0, 0xC0);
        gas3911Reg.gTypeArec = 0xD8;
        gas3911Reg.gTypeBrec = 0xD8;

//        as3911ModifyRegister(AS3911_REG_RX_CONF3, 0xE0, 0xE0);//针对C测试机 将接收调节最大
//        as3911WriteRegister(AS3911_REG_RX_CONF1, 0x40); //修改0AH bit6 针对C测试机

//        s_as3911_SetInt(ON);

        //type b 调制深度自适应
        as3911ModifyRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x7E, 0x1E);
        as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
        as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
        as3911ExecuteCommand(AS3911_CMD_CALIBRATE_MODULATION);
        timer0 = 0;
        as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 200, (ulong *)&timer0);
//        Dprintk("\r\n\r\n interrupt flag  :%x",timer0);
        if ( timer0 == AS3911_IRQ_MASK_DCT) {
            EG_mifs_tWorkInfo.ucFirstOpenFlag = 1;
            if ( s_rfid_mainVersion(0) == 1 ) {
                as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x80);//固定type b调制深度
                as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, gas3911Reg.gTypeBmodule);
            }
            if ( ucOpenMode ==  EM_mifs_NULL ) {
                //关闭载波
                as3911WriteRegister(AS3911_REG_OP_CONTROL, 0xD0);
            }else{
                //打开载波和接收电路
                as3911WriteRegister(AS3911_REG_OP_CONTROL, 0xD8);
            }
//            s_Rfid_SetInt(OFF); //port B中断和其他模块复用 不能关闭
        }else{
            //初始化失败
            as3911WriteRegister(AS3911_REG_OP_CONTROL, 0x00);
            EG_mifs_tWorkInfo.ucFirstOpenFlag = 0;
//            s_Rfid_SetInt(OFF); //port B中断和其他模块复用 不能关闭
            return 1;
        }
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module  //RC663
		timer0 = timer0;
		s_Rfid_SetInt(OFF);
		rfid_delayms(10);
		s_Rfid_SetRST(0);
		rfid_delayms(10);

		EG_mifs_tWorkInfo.ucFirstOpenFlag = 1;

        EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_IRQ0EN,0x80);
        EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_IRQ1EN,0x80);
        data = 0;
        EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_COMMAND, &data);
        data = 0x80;
        EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_FIFOCONTROL, &data);
        EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_FIFOCONTROL, &data);
        if ( (data&0x80) == 0) {
            EG_mifs_tWorkInfo.FIFOSIZE = 512;
        }else{
            EG_mifs_tWorkInfo.FIFOSIZE = 255;
        }
		data = 0x86;
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_DRVMODE, &data);
//		data = 0x15;
//		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXAMP, &data);
//		data = 0x21;
//		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_DRVCON, &data);
//		data = 0x0A;
//		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXI, &data);
//		data = 0x18;
//		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXCRCCON, &data);
//		data = 0x18;
//		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXCRCCON, &data);
		data = 0x08;//0x0F
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXDATANUM, &data);
//		data = 0x27;
//		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXMODWIDTH, &data);
		data = 0x00;
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM10BURSTLEN, &data);
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXSOFD, &data);
//		data = 0xC0;
//		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXWAITCTRL, &data);
//		data = 0xCF;
//		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_FRAMECON, &data);
//		data = 0x04;
//		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXCTRL, &data);
        data = 0x90;
        EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXWAIT, &data);
//		data = 0x3F;//0x66;//0x32
//		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXTHRESHOLD, &data);
		data = 0x12;
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RCV, &data);
		data = 0x0A;
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXANA, &data);
		data = 0x04;
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXDATACON, &data);
		data = 0x00;
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM0H, &data);
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM, &data);
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM2, &data);
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM3, &data);
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM32LEN, &data);
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM10BURSTCTRL, &data);
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXEOFSYM, &data);
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXSYNCVALH, &data);
        data = 0x80;
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXCORR, &data);
        data = 0xF0;
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_FABCALI, &data);
		s_Rfid_SetInt(ON);
		EI_paypass_vSelectType(ucOpenMode);//2012-05-08:EM_mifs_TYPEA
        EI_mifs_vEndProtect();
#endif

    }
    else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		timer0 = timer0;
//		s_rfid_init();
		s_Rfid_SetInt(OFF);
		s_Rfid_SetRST(0);
		rfid_delayms(1);
		s_Rfid_SetRST(1);
		rfid_delayms(10);

		EG_mifs_tWorkInfo.ucFirstOpenFlag = 1;

		data = BIT4;
		EI_mifs_vSetBitMask(ControlReg, data);	//initiator 发送后接收

		data = gcMifReg.GsNOn;//0xF4;
		EI_mifs_vWriteReg(1, GsNOnReg, &data);
		rfid_delayms(1);
		ucResult = 0;
		EI_mifs_vReadReg(1, GsNOnReg, &ucResult);

		if(ucResult != data){
            s_Rfid_SetInt(ON);
			return ERROR;
        }

		data = 0x84;
		EI_mifs_vWriteReg(1, RxThresholdReg, &data);

		//data = 0x3B;
		//EI_mifs_vWriteReg(1, ModeReg, &data);

		data = 0x04;
		EI_mifs_vWriteReg(1, WaterLevelReg, &data);

		data = BIT7;
		EI_mifs_vSetBitMask(CommIEnReg, data);
		EI_mifs_vSetBitMask(DivIEnReg, data);

		data = 0x26;
		EI_mifs_vWriteReg(1, ModWidthReg, &data);

		// 禁止并清除所有中断源
		data = 0x7F;
		EI_mifs_vWriteReg(1, ComIrqReg, &data);
		EI_mifs_vWriteReg(1, DivIrqReg, &data);

		data = 0x00;
		EI_mifs_vWriteReg(1, CommandReg, &data);

		data = 0x15;
		EI_mifs_vWriteReg(1, TxSelReg, &data);

		data = 0x38;
		EI_mifs_vWriteReg(1, AnalogTestReg, &data);

		s_Rfid_SetInt(ON);
		EI_paypass_vSelectType(ucOpenMode);//2012-05-08:EM_mifs_TYPEA
		EI_mifs_vEndProtect();

#endif

	}
#ifdef EM_RC531_Module  //RC531
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{
        s_Rfid_SetRST(0);
        rfid_delayms(1);
        s_Rfid_SetRST(1);
        rfid_delayms(1);
        s_Rfid_SetRST(0);

		// 如果第一次调用该函数，需要初始化RC531芯片
		/*
		if (EG_mifs_tWorkInfo.ucFirstOpenFlag != 0)
		{
			goto mifsPowerOn;
		}
		*/
        // 等待初始化完成，超时2.1秒
        timer0 = sys_get_counter();
        rfid_delayms(2);
		// 等待开始阶段结束
		while (1)
		{
			EI_mifs_vReadReg(1, EM_mifs_REG_COMMAND, &data);
			if ((data & 0x3F) == 0x00)
			{
				break;
			}
			if(if_timerout(timer0,OpenRFID_TIMEOUT))
			{
				break;
			}
		}
        data = 0x80;
        EI_mifs_vWriteReg(1, EM_mifs_REG_PAGE, &data);	//init RC531 mcu
        while (1)
        {
            EI_mifs_vReadReg(1, EM_mifs_REG_COMMAND, &data);
            if (data == 0x00)
            {
                break;
            }
            if(if_timerout(timer0,OpenRFID_TIMEOUT))
            {
                return ERROR;
            }
        }
        EG_mifs_tWorkInfo.ucFirstOpenFlag = 1;
        //mifsPowerOn:

        data = 0x00;
        EI_mifs_vWriteReg(1, EM_mifs_REG_PAGE, &data);	//SPI模式

        // Q-Clock
        EI_mifs_vWriteReg(1, EM_mifs_REG_CLOCKQCONTROL, &data);
        data = 0x40;
        EI_mifs_vWriteReg(1, EM_mifs_REG_CLOCKQCONTROL, &data);
        rfid_delayms(1);  //100us
        EI_mifs_vClrBitMask(EM_mifs_REG_CLOCKQCONTROL, 0x40);

        data = 0xAD;
        EI_mifs_vWriteReg(1, EM_mifs_REG_BITPHASE, &data);  // 发送和接收时钟的相位
        data = 0xFF;
        EI_mifs_vWriteReg(1, EM_mifs_REG_RXTHRESHOLD, &data);
        data = 0x06;
        EI_mifs_vWriteReg(1, EM_mifs_REG_RXWAIT, &data);

        // Rx常开 Q-Clock
        data = 0x01;
        EI_mifs_vWriteReg(1, EM_mifs_REG_RXCONTROL2, &data);

        // FIFO警告等级: 4
        data = 0x04;
        EI_mifs_vWriteReg(1, EM_mifs_REG_FIFOLEVEL, &data);

        // 低电平中断, CMOS标准
        data = 0x3;
        EI_mifs_vWriteReg(1, EM_mifs_REG_IRQPINCONFIG, &data);

        // Miller编码脉冲宽度
        //data = 0x13;
        EI_mifs_vWriteReg(1, EM_mifs_REG_MODWIDTH, &gtRfidDebugInfo.ModWidth_A);

        // Tx2RF-En, Tx1RF-En固定输出电平
        data = 0x58;
        EI_mifs_vWriteReg(1,EM_mifs_REG_TXCONTROL, &data);

		// 禁止并清除所有中断源
		data = 0x7F;
		EI_mifs_vWriteReg(1, EM_mifs_REG_INTERRUPTEN, &data);
		EI_mifs_vWriteReg(1, EM_mifs_REG_INTERRUPTRQ, &data);
		// 使能中断
		s_Rfid_SetInt(ON);
		EI_paypass_vSelectType(ucOpenMode);//2012-05-08:EM_mifs_TYPEA
		EI_mifs_vEndProtect();
    }
#endif
    else{
        //没有射频模块
        return 1;
    }
	return 0;
}

/**********************************************************************
* 函数名称：
*    EA_ucMifsClose(void)
* 功能描述：
*   打开非接触卡设备，并获得设备使用权
* 输入参数：
*   无
* 输出参数：
*   无
* 返回值：
*   EM_SUCCESS      成功
*   EM_FAILED       其他错误（系统错误等）
* 历史纪录：
*   修改人          日期            版本号      修改记录
***********************************************************************/
uchar EA_ucMifsClose(void)
{
	EI_mifs_ucBeginProtect(0, 0);
	if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911)
	{

#ifdef EM_AS3911_Module

        //power down mode
//        emvHalActivateField(OFF);
        as3911WriteRegister(AS3911_REG_IO_CONF1, 0x0F);
        as3911WriteRegister(AS3911_REG_IO_CONF2, 0x80);
        /* Enable Oscillator, Transmitter and receiver. */
        as3911WriteRegister(AS3911_REG_OP_CONTROL, 0x00);
        as3911WriteRegister(AS3911_REG_MODE, 0x08);
//		s_Rfid_SetInt(OFF); //port B中断和其他模块复用 不能关闭

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
	{

#ifdef EM_RC663_Module

		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_DRVMODE, PHHAL_HW_RC663_BIT_TXEN);

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		EI_mifs_vClrBitMask(TxControlReg, 0x03);

#endif

	}
#ifdef EM_RC531_Module
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{
		EI_mifs_vClrBitMask(EM_mifs_REG_TXCONTROL, 0x03);
        //S980支持低功耗模式
#if defined(PRODUCT_F16_2)
        EI_mifs_vSetBitMask(EM_mifs_REG_CONTROL, 1<<4);  //进入powerdown模式
#endif
	}
#endif
	/*
	uchar ucResult;
	uchar ucTempData;
	DevHandle *phDevHandle = EM_NULL;

	ucResult = EI_mifs_ucBeginProtect(phDevHandle, 0);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

	// TX1和TX2固定输出
	EI_mifs_vClrBitMask(EM_mifs_REG_TXCONTROL, 0x03);

	// 禁止并清除所有中断源
	ucTempData = 0x7F;
	EI_mifs_vWriteReg(1, EM_mifs_REG_INTERRUPTEN, &ucTempData);
	EI_mifs_vWriteReg(1, EM_mifs_REG_INTERRUPTRQ, &ucTempData);
	s_Rfid_SetInt(OFF);
	//复位脚RST置高
	s_Rfid_SetRST(1);
	EI_mifs_vEndProtect();
	*/
	EG_mifs_tWorkInfo.ucFirstOpenFlag = 0;
	EI_mifs_vEndProtect();
	return EM_SUCCESS;
}

/**********************************************************************
* 函数名称：
*     EA_ucMIFSetTime(DevHandle hDevHandle, uchar ucTime)
* 功能描述：
*     改变卡的超时等待时间
* 输入参数：
*     ucTime: 超时时间，范围0~14
* 输出参数：
*     无
* 返回值：
*     EM_SUCCESS    成功
*     EM_ERRHANDLE  错误的句柄
*     EM_ERRPARAM   参数错误(卡类型ucType错误)
*     EM_FAILED 其他错误（系统错误等）
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFSetTime(DevHandle hDevHandle, uchar ucTime)
{
	if (ucTime > EM_mifs_FWI_MAXVALUE)
	{
		return EM_ERRPARAM;
	}
	EG_mifs_tWorkInfo.ucFWI = ucTime;
	return EM_SUCCESS;
}

/**********************************************************************
* 函数名称：
*     EA_ucMIFSetRegVal(DevHandle hDevHandle , uchar ucAddr , uchar ucVal)
* 功能描述：
*    设置指定寄存器的数值
* 输入参数：
*     hDevHandle     设备句柄
*     ucAddr		 寄存器地址
*     ucVal		 设置值
* 输出参数：
*     无
* 返回值：
*     EM_SUCCESS    成功
*     EM_ERRHANDLE  错误的句柄
*     EM_ERRPARAM   参数错误
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFSetRegVal(DevHandle hDevHandle, uchar ucAddr, uchar ucVal)
{
	uchar ucResult;

	// 判断寄存器地址是否有效
	if ((ucAddr >= 0x11 && ucAddr <= 0x26) || ucAddr == 0x3D)
	{
		if (ucAddr == 0x16 || ucAddr == 0x18 || ucAddr == 0x1D || ucAddr == 0x20 || ucAddr == 0x25)
		{
			return EM_ERRPARAM;
		}
	}
	else
	{
		return EM_ERRPARAM;
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 3);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}
	EI_mifs_vWriteReg(1, ucAddr, &ucVal);

	EI_mifs_vEndProtect();
	return EM_SUCCESS;
}

/**********************************************************************

                         Mifare卡函数定义

***********************************************************************/
/**********************************************************************
* 函数名称：
*     EA_ucMIFRequest(DevHandle hDevHandle, uchar ucMode, ushort *pusTagType)
* 功能描述：
*     提示天线感应范围内的卡返回一个卡片类型识别号
* 输入参数：
*    hDevHandle     设备句柄
*     ucMode
                   EM_mifs_IDLE -- REQUEST IDLE只给一张卡复位。且只有当该卡离开射频天线的有
                                   效范围后再进入时， 该函数才再次有效。(推荐使用该模式)
                   EM_mifs_HALT -- REQUEST ALL给射频天线的有效范围内的所有卡复位。
* 输出参数：
      pusTagType：卡复位时返回的卡片类型识别号
* 返回值：
*     处理结果
* 历史纪录：
*     修改人       日期              版本号
***********************************************************************/
uchar EA_ucMIFRequest(DevHandle hDevHandle, uchar ucMode, ushort * pusTagType)
{
	uchar ucTempData;
	uchar ucResult;
	uchar ucCMD = 0;

	if (pusTagType == EM_NULL)
	{
		return EM_ERRPARAM;
	}
	/*
	 * if ((ucMode != EM_mifs_HALT) && (ucMode != EM_mifs_IDLE))
	 * {
	 * return EM_ERRPARAM;
	 * }
	 */
	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}
	EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_S50;
	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = 0;
        ucTempData = ucTempData;
        ucCMD = EMV_HAL_TRANSCEIVE_WUPA;
        EG_mifs_tWorkInfo.expectMaxRec = 2;
        EI_paypass_vSelectType(EM_mifs_TYPEA);
		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
	{

#ifdef EM_RC663_Module

//        EI_paypass_vSelectType(EM_mifs_TYPEA); //此函数只有mifare卡使用 type a
		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
		// 禁止 RxCRC 和 TxCRC , 允许奇偶校验
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_STATUS, 0x20);
        //冲突需要设置?
//		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXBITCTRL, 0x80);
        ucTempData = 0x01;
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);

		// set TxLastBits to 7, 短帧格式
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXDATANUM, 0x07);

		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
		// 禁止 RxCRC 和 TxCRC , 允许奇偶校验
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(Status2Reg, 0x08);
		//cf_pn512 冲突位
		ucTempData = 0x80;
		EI_mifs_vSetBitMask(CollReg, ucTempData);
		EI_mifs_vClrBitMask(TxModeReg, 0xF0);
		EI_mifs_vClrBitMask(RxModeReg, 0xF0);

		// set TxLastBits to 7, 短帧格式
		ucTempData = 0x07;
		EI_mifs_vWriteReg(1, BitFramingReg, &ucTempData);
		ucCMD = PN512CMD_TRANSCEIVE;

#endif

	}
#ifdef EM_RC531_Module
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{
		// 设置定时器
		EI_mifs_vSetTimer(6);

		// 每个字节带奇校验
		ucTempData = 0x03;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

		// 禁止crypto1单元
		EI_mifs_vClrBitMask(EM_mifs_REG_CONTROL, 0x08);

		// set TxLastBits to 7
		ucTempData = 0x07;
		EI_mifs_vWriteReg(1, EM_mifs_REG_BITFRAMING, &ucTempData);

		ucCMD = EM_mifs_PCD_TRANSCEIVE;
	}
#endif

	// 准备发送数据: 命令码
	EG_mifs_tWorkInfo.ulSendBytes = 1;
	EG_mifs_tWorkInfo.ucDisableDF = 1;
	if (ucMode == EM_mifs_HALT)
	{
		EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_REQALL;
	}
	else
	{
		EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_REQSTD;
	}

	ucResult = EI_mifs_ucHandleCmd(ucCMD);

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
	EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXDATANUM, 0x07); //发送完毕后发送完整字节
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module
	EI_mifs_vClrBitMask(BitFramingReg, 0x07); //发送完毕后发送完整字节
#endif

	}

	// 处理接收数据
	// 期望接收到两个完整字节: 卡类型识别号(2字节)
	if ((ucResult == EM_SUCCESS) && (EG_mifs_tWorkInfo.lBitsReceived != 16))
	{
		// 卡送上来位错误
		ucResult = EM_mifs_BITCOUNTERR;
	}

	// 位冲突但是已经接收到16个字节( 所有接收到的数据联合了起来)
	if ((ucResult == EM_mifs_COLLERR) && (EG_mifs_tWorkInfo.lBitsReceived == 16))
	{
		ucResult = EM_SUCCESS;
	}

	// 如果发生错误，且没有接收到数据，则清空接收数据区
	if ((ucResult != EM_SUCCESS) || (EG_mifs_tWorkInfo.ulBytesReceived == 0))
	{
		EG_mifs_tWorkInfo.aucBuffer[0] = 0;
		EG_mifs_tWorkInfo.aucBuffer[1] = 0;
	}
	else if ((EG_mifs_tWorkInfo.ulBytesReceived == 1) && (ucResult == EM_SUCCESS))
	{
		// 只收到一个字节
		EG_mifs_tWorkInfo.aucBuffer[1] = 0;
	}

	// 保存接收到的类型号
	memcpy(pusTagType, EG_mifs_tWorkInfo.aucBuffer, 2);

	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************
* 函数名称：
*     EA_ucMIFAntiColl(DevHandle hDevHandle,ulong *pulSnr)
* 功能描述：
*     在所有处于天线感应范围内的卡中选择一张，并返回它的卡号。(防冲突处理)
* 输入参数：
*    hDevHandle     设备句柄
* 输出参数：
*     pulSnr -- 卡序列号
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFAntiColl(DevHandle hDevHandle, ulong * pulSnr)
{
	uchar ucTempData;
	ulong ulBCnt;						// 当前要发送数据的合法位数
	ulong ulBytes;						// 当前要发送数据的合法字节数
	uchar ucCompleted;					// 表明操作是否完成
	uchar ucTempVal;
	uint i;
	uchar ucResult;
    uchar select = 0,getcol = 0;
	uchar ucCMD = 0;

	if (pulSnr == EM_NULL)
	{
		return EM_ERRPARAM;
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        select = select;getcol = getcol;
        ucTempData = 0;
        ucTempData = ucTempData;
        ucCMD = EMV_HAL_TRANSCEIVE_WITHOUT_CRC;
        EG_mifs_tWorkInfo.expectMaxRec = 5;
//        EI_paypass_vSelectType(EM_mifs_TYPEA);
		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
	{

#ifdef EM_RC663_Module

		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
		ucTempData = 0x80;
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXBITCTRL, ucTempData);
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_STATUS, 0x20);

		// 禁止 RxCRC 和 TxCRC , 允许奇偶校验
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXCRCCON, 0x01);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCRCCON, 0x01);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
		EI_mifs_vClrBitMask(TxModeReg, 0x80);
		EI_mifs_vClrBitMask(RxModeReg, 0x80);
		// 禁止 RxCRC 和 TxCRC , 允许奇偶校验
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(CollReg, 0x80);   //cf_pn512 是否要清0 还是置1
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(Status2Reg, 0x08);
		ucCMD = PN512CMD_TRANSCEIVE;

#endif

	}
#ifdef EM_RC531_Module
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{
		select = select;
		getcol = getcol;
		// 设置定时器
		EI_mifs_vSetTimer(2);

		// 冲突后数据都置0, Rx帧格式为ISO14443A
		ucTempData = 0x28;
		EI_mifs_vWriteReg(1, EM_mifs_REG_DECODERCONTROL, &ucTempData);	// ZeroAfterColl aktivieren

		// 禁止crypto1单元
		EI_mifs_vClrBitMask(EM_mifs_REG_CONTROL, 0x08);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;
	}
#endif
	memset(EG_mifs_tWorkInfo.aucCmdBuf, 0, 5);

	ucCompleted = 0;
	ulBCnt = 0;

	// 持续发送AntiColl命令，直到防冲突成功
	while ((ucCompleted == 0) && (ucResult == EM_SUCCESS))
	{
		EG_mifs_tWorkInfo.ucDisableDF = 1;

        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
            ;
#endif

        }
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
		{

#ifdef EM_RC663_Module
			;
#endif

		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
		{

#ifdef EM_PN512_Module
			;
#endif

		}
#ifdef EM_RC531_Module
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
		{
			// 每个字节带奇校验
			ucTempData = 0x03;
			EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		}
#endif
		EG_mifs_tWorkInfo.ucnBits = (uchar) (ulBCnt % 8);
		if (EG_mifs_tWorkInfo.ucnBits != 0)
		{
			// 有不满一个字节的数据
			// 根据多出的不满一个字节的位数设置接收数据起始位（使得最后的数据正好收满一个字节）
			// 和最后一个数据位数, 获得收到数据位的起始位和发送结束位,如ucnBits=1,则ucTempData=0x11
			ucTempData = (EG_mifs_tWorkInfo.ucnBits % 0x10 * 0x10) | EG_mifs_tWorkInfo.ucnBits;

            if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
                ;
#endif

            }
			else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
			{

#ifdef EM_RC663_Module
                EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXBITCTRL, 0x70);
                EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXDATANUM, 0x07);
                EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXDATANUM, (ucTempData&0x07));
//                ucTempData &= 0x70;
                EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXBITCTRL, (ucTempData&0x70));
#endif

			}
			else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
			{

#ifdef EM_PN512_Module
				ucTempData &= 0x7F;
				EI_mifs_vWriteReg(1, BitFramingReg, &ucTempData);
#endif

			}
#ifdef EM_RC531_Module
			else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
			{
				// 设置要发送的停止位
				EI_mifs_vWriteReg(1, EM_mifs_REG_BITFRAMING, &ucTempData);
			}
#endif
			ulBytes = ulBCnt / 8 + 1;	//设置需要发送的字节数

#ifdef EM_RC531_Module
			if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
			{
				if (EG_mifs_tWorkInfo.ucnBits == 7)
				{
					// 在中断中进行处理
					EG_mifs_tWorkInfo.lBitsReceived = 7;
					// 第一个字节接收起始位置改为0
					EI_mifs_vWriteReg(1, EM_mifs_REG_BITFRAMING, (uchar *) & EG_mifs_tWorkInfo.ucnBits);
				}
			}
#endif
		}
		else
		{
			ulBytes = ulBCnt / 8;
		}

		// 准备发送数据: SEL+NVB+上次成功接收到的所有数据
		EG_mifs_tWorkInfo.ucAnticol = EM_mifs_PICC_ANTICOLL1;
		EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_ANTICOLL1;	// SEL
		// NVB: 高4位 = 传输的所有合法数据位数/8(包括SEL和NVB,不包括多余位数)
		// 低4位=传输的所有合法数据位数 mod 8
		// *0x10相当于左移4位表示字节数
		EG_mifs_tWorkInfo.aucBuffer[1] =
			(uchar) (0x20 + ((ulBCnt / 8) * 0x10) + EG_mifs_tWorkInfo.ucnBits);

		for (i = 0; i < ulBytes; i++)
		{
			EG_mifs_tWorkInfo.aucBuffer[i + 2] = EG_mifs_tWorkInfo.aucCmdBuf[i];
		}
		EG_mifs_tWorkInfo.ulSendBytes = 2 + ulBytes;	// how many bytes to send

		// 执行命令
		ucResult = EI_mifs_ucHandleCmd(ucCMD);
		//发送第7位
		if (EG_mifs_tWorkInfo.ucnBits == 7)
		{
			// 这种情况算作冲突错误, 但仍然存储
			EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.aucBuffer[0] << 7;
			EG_mifs_tWorkInfo.ucCollPos = 8;
			ucResult = EM_mifs_COLLERR;
			EG_mifs_tWorkInfo.lBitsReceived = 40 - ulBCnt;
		}

		if ((ucResult == EM_SUCCESS) || (ucResult == EM_mifs_COLLERR))
		{
			// 接收数据长度错误
			if (EG_mifs_tWorkInfo.lBitsReceived != (long)(40 - ulBCnt))	// not 5 bytes answered
			{
				ucResult = EM_mifs_BITCOUNTERR;
			}
			else
			{
                if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
#ifdef EM_AS3911_Module
                    if ( ucResult == EM_mifs_COLLERR ) {
                        //防冲撞暂不实现
                        ucCompleted = 1;
                    }

					// 新接收的数据从本次发送数据产生冲突的位开始存放
					ucTempVal = 0;
					if (EG_mifs_tWorkInfo.ucnBits != 0)	// 最后一个字节不完整
					{
						// 由于设置, 从FIFO中接收的只有第一个字节不完整
						// 所以加入上次接收的最后一个字节中
						EG_mifs_tWorkInfo.aucCmdBuf[ulBytes - 1] |= EG_mifs_tWorkInfo.aucBuffer[0];
						ucTempVal = 1;
					}
					for (i = 0; i < (4 - ulBytes); i++)
					{
						EG_mifs_tWorkInfo.aucCmdBuf[i + ulBytes] =
							EG_mifs_tWorkInfo.aucBuffer[ucTempVal + i];
					}

                    if (ucResult == EM_SUCCESS)
                    {
                        // 成功检查校验和
                        ucTempVal = 0;
                        for (i = 0; i < 4; i++)
                        {
                            ucTempVal ^= EG_mifs_tWorkInfo.aucCmdBuf[i];
                        }
//                        DISPBUF(EG_mifs_tWorkInfo.aucCmdBuf,5,0);
                        if (ucTempVal !=
                            EG_mifs_tWorkInfo.aucBuffer[EG_mifs_tWorkInfo.ulBytesReceived - 1])
                        {
                            // 校验错误
                            ucResult = EM_mifs_SERNRERR;
                        }
                        else
                        {
                            //防冲撞完成
                            ucCompleted = 1;
                        }
                    }
					if (ucResult == EM_mifs_COLLERR) //如果是冲突 置为成功
						ucResult = EM_mifs_SUCCESS;
#endif
                }
				else if((EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
                   || (EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663))
				{

//#ifdef EM_PN512_Module
#if (defined(EM_PN512_Module) || defined(EM_RC663_Module))

					if (ucResult == EM_mifs_COLLERR) // 冲突
					{
                        if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
                        {

#ifdef EM_RC663_Module
                            ulBCnt = ulBCnt + EG_mifs_tWorkInfo.ucCollPos+1 - EG_mifs_tWorkInfo.ucnBits;
                            select = (EG_mifs_tWorkInfo.ucCollPos)/8;  //collpos为0则保留bit0
                            getcol = (0x01<<((EG_mifs_tWorkInfo.ucCollPos%8)+1)) - 1;
                            EG_mifs_tWorkInfo.aucBuffer[select] = EG_mifs_tWorkInfo.aucBuffer[select]&getcol;

#endif

                        }
                        else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
                        {

#ifdef EM_PN512_Module
                            ulBCnt = ulBCnt + EG_mifs_tWorkInfo.ucCollPos - EG_mifs_tWorkInfo.ucnBits;
                            select = (EG_mifs_tWorkInfo.ucCollPos-1)/8;
                            if(!(EG_mifs_tWorkInfo.ucCollPos%8))   //collpos为1则保留bit0  为8保留bit0~bit7  为9保留bit0
                                getcol = (0x01<<8)-1;
                            else
                                getcol = (0x01<<(EG_mifs_tWorkInfo.ucCollPos%8)) - 1;

                            EG_mifs_tWorkInfo.aucBuffer[select] = EG_mifs_tWorkInfo.aucBuffer[select]&getcol;
#endif

                        }
//                        TRACE("\r\n--%x---%x----%x---%x",(0x01<<8)-1, select, getcol,EG_mifs_tWorkInfo.aucBuffer[select]);
					}

					// 新接收的数据从本次发送数据产生冲突的位开始存放
					ucTempVal = 0;
					if (EG_mifs_tWorkInfo.ucnBits != 0)	// 最后一个字节不完整
					{
						// 由于设置, 从FIFO中接收的只有第一个字节不完整
						// 所以加入上次接收的最后一个字节中
						EG_mifs_tWorkInfo.aucCmdBuf[ulBytes - 1] |= EG_mifs_tWorkInfo.aucBuffer[0];
						ucTempVal = 1;
					}
					for (i = 0; i < (4 - ulBytes); i++)
					{
						EG_mifs_tWorkInfo.aucCmdBuf[i + ulBytes] =
							EG_mifs_tWorkInfo.aucBuffer[ucTempVal + i];
					}

					if (ucResult == EM_SUCCESS)
					{
						// 成功检查校验和
						ucTempVal = 0;
						for (i = 0; i < 4; i++)
						{
							ucTempVal ^= EG_mifs_tWorkInfo.aucCmdBuf[i];
						}
						if (ucTempVal !=
							EG_mifs_tWorkInfo.aucBuffer[EG_mifs_tWorkInfo.ulBytesReceived - 1])
						{
							// 校验错误
							ucResult = EM_mifs_SERNRERR;
						}
						else
						{
							//防冲撞完成
							ucCompleted = 1;
						}
					}

					if (ucResult == EM_mifs_COLLERR) //如果是冲突 置为成功
						ucResult = EM_mifs_SUCCESS;

#endif

				}
#ifdef EM_RC531_Module
				else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
				{
                    // 新接收的数据从本次发送数据产生冲突的位开始存放
                    ucTempVal = 0;
                    if (EG_mifs_tWorkInfo.ucnBits != 0)	// 最后一个字节不完整
                    {
                        // 由于设置, 从FIFO中接收的只有第一个字节不完整
                        // 所以加入上次接收的最后一个字节中
                        EG_mifs_tWorkInfo.aucCmdBuf[ulBytes - 1] |= EG_mifs_tWorkInfo.aucBuffer[0];
                        ucTempVal = 1;
                    }
                    for (i = 0; i < (4 - ulBytes); i++)
                    {
                        EG_mifs_tWorkInfo.aucCmdBuf[i + ulBytes] =
                            EG_mifs_tWorkInfo.aucBuffer[ucTempVal + i];
                    }
                    if (ucResult != EM_mifs_COLLERR)	// 没有错误产生
                    {
                        // 检查校验和
                        ucTempVal = 0;
                        for (i = 0; i < 4; i++)
                        {
                            ucTempVal ^= EG_mifs_tWorkInfo.aucCmdBuf[i];
                        }
                        if (ucTempVal !=
                            EG_mifs_tWorkInfo.aucBuffer[EG_mifs_tWorkInfo.ulBytesReceived - 1])
                        {
                            // 校验错误
                            ucResult = EM_mifs_SERNRERR;
                        }
                        else
                        {
                            ucCompleted = 1;
                        }
                    }
                    else					// 冲突
                    {
                        // 重新设置发送数量: 即第一个冲突位的位置之前的数据发送给卡
                        // 卡会回送从第一个冲突位以后的数据
                        ulBCnt = ulBCnt + EG_mifs_tWorkInfo.ucCollPos - EG_mifs_tWorkInfo.ucnBits;
                        ucResult = EM_SUCCESS;
                    }
				}
#endif
			}							// if(EG_mifs_tWorkInfo.lBitsReceived != (40 - ulBCnt))
		}								// if(ucResult == EM_SUCCESS || ucResult == EM_mifs_COLLERR)
	}									// while(ucCompleted == 0 && ucResult == EM_SUCCESS)

	if (ucResult == EM_SUCCESS)
	{
		// 保存取到的卡号
		memcpy(pulSnr, EG_mifs_tWorkInfo.aucCmdBuf, 4);
	}

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
#ifdef EM_AS3911_Module
        ;
#endif
    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
	{

#ifdef EM_RC663_Module
        EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXBITCTRL, 0x70);
        EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXDATANUM, 0x07);
#endif

	}
#ifdef EM_PN512_Module
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{
		//EI_mifs_vSetBitMask(CollReg, 0x80);	//cf_pn512 是否要置1
        EI_mifs_vClrBitMask(BitFramingReg, 0x07); //发送完毕后发送完整字节 chenf 20121204
        EI_mifs_vClrBitMask(BitFramingReg, 0x70); //发送完毕后接收完整字节 chenf 20121204
	}
#endif
#ifdef EM_RC531_Module
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{
		// 结束处理
		EI_mifs_vClrBitMask(EM_mifs_REG_DECODERCONTROL, 0x20);	// ZeroAfterColl disable
	}
#endif
	EG_mifs_tWorkInfo.ucAnticol = 0;

	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************
* 函数名称：
*      EA_ucMIFSelect(DevHandle hDevHandle,ulong ulSnr, ushort *pusSize)
* 功能描述：
*     选择接受处理命令的卡
* 被以下函数调用
*     API
* 调用以下函数：
*     下层函数
* 输入参数：
*     hDevHandle        设备句柄
*     ulSnr   -- 卡号
* 输出参数：
*     puiSize -- 被选择的卡返回它的存储器的大小(kbit)
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
***********************************************************************/
uchar EA_ucMIFSelect(DevHandle hDevHandle, ulong ulSnr, uint * puiSize)
{
	uchar ucTempData = 0;
	uint i;
	uchar ucResult;
	uchar ucCMD = 0;

	if (puiSize == EM_NULL)
	{
		return EM_ERRPARAM;
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;
        ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
        EG_mifs_tWorkInfo.expectMaxRec = 1+2;
		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = ucTempData;
		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_STATUS, 0x20);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, 0x01);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, 0x01);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = ucTempData;
		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, 0x80);
		EI_mifs_vSetBitMask(RxModeReg, 0x80);

		// 禁止crypto1单元
		EI_mifs_vClrBitMask(Status2Reg, 0x08);
		ucCMD = PN512CMD_TRANSCEIVE;

#endif

	}
#ifdef EM_RC531_Module
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{
		EI_mifs_vSetTimer(106);
		// 发送和接收帧最后一个字节为CRC校验，每个字节带奇校验
		ucTempData = 0x0F;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

		// 禁止crypto1单元
		EI_mifs_vClrBitMask(EM_mifs_REG_CONTROL, 0x08);	// disable crypto 1 unit
		ucCMD = EM_mifs_PCD_TRANSCEIVE;
	}
#endif
	EG_mifs_tWorkInfo.ulSendBytes = 7;
	EG_mifs_tWorkInfo.ucDisableDF = 1;

	EG_mifs_tWorkInfo.ulCardID = ulSnr;	// 保存当前选中卡号

	// 准备发送数据: 0x93+0x70+4字节卡号+卡号校验
	memcpy(EG_mifs_tWorkInfo.aucBuffer + 2, (void *)&ulSnr, 4);
	EG_mifs_tWorkInfo.aucBuffer[6] = 0;
	for (i = 0; i < 4; i++)
	{
		EG_mifs_tWorkInfo.aucBuffer[6] ^= EG_mifs_tWorkInfo.aucBuffer[i + 2];
	}
	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_ANTICOLL1;
	EG_mifs_tWorkInfo.aucBuffer[1] = 0x70;

	ucResult = EI_mifs_ucHandleCmd(ucCMD);

	// 返回数据: 卡容量大小(1字节)
	if (ucResult == EM_SUCCESS)
	{
		// 一个字节没有收满
		if (EG_mifs_tWorkInfo.lBitsReceived != 8)
		{
			ucResult = EM_mifs_BITCOUNTERR;
		}
	}
	*puiSize = (uint) EG_mifs_tWorkInfo.aucBuffer[0];

	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************
* 函数名称：
*     EA_ucMIFAuthExtend(DevHandle hDevHandle,uchar ucMode, uchar ucAddr)
* 功能描述：
*     使用存放在非接触卡模块中的密钥对卡片指定扇区进行认证
* 输入参数：
*    hDevHandle     设备句柄
*    ucMode： 0--15     A套第1～16组密码
*             16～31    B套第1～16组密码
*    ucAddr：扇区密钥所在块号
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2005-05-18          01-01-01    创建
* 备    注：
***********************************************************************/
uchar EA_ucMIFAuthExtend(DevHandle hDevHandle, uchar ucMode, uchar ucAddr)
{
#ifdef EM_RC531_Module
	uchar ucTempData;
	ushort usAddr;
	uchar ucResult;

	usAddr = ucMode;
	if (usAddr < EM_mifs_KEYNUM)
	{
		usAddr = usAddr * 0x18 + 0x80;
	}
	else if (usAddr < EM_mifs_KEYNUM * 2)
	{
		usAddr = (usAddr - EM_mifs_KEYNUM) * 0x18 + 0x8C;
	}
	else
	{
		return EM_ERRPARAM;
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

/**********************************************************************
 载入密钥过程
***********************************************************************/
	// 发送帧最后一个字节为CRC校验，每个字节带奇校验
	ucTempData = 0x07;
	EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

	// 设置定时器
	EI_mifs_vSetTimer(2);

	// 准备发送数据: 密钥地址(2)
	EG_mifs_tWorkInfo.aucBuffer[0] = (uchar) usAddr;
	EG_mifs_tWorkInfo.aucBuffer[1] = (uchar) (usAddr >> 8);
	EG_mifs_tWorkInfo.ulSendBytes = 2;	// how many bytes to send
	EG_mifs_tWorkInfo.ucDisableDF = 0;

	EI_mifs_vFlushFIFO();				// empty FIFO

	// 执行命令
	ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_LOADKEYE2);
	if (ucResult != EM_SUCCESS)
	{
		goto mifsAuthorEnd;
	}

	if (ucMode < EM_mifs_KEYNUM)
	{
		ucResult = EI_mifs_ucAuthentication(EM_mifs_KEYA, ucAddr);
	}
	else
	{
		ucResult = EI_mifs_ucAuthentication(EM_mifs_KEYB, ucAddr);
	}

  mifsAuthorEnd:
	EI_mifs_vEndProtect();
	return ucResult;

#else

	//使用EEROM内密钥认证扇区 PN512不支持EEROM里存储密钥 此API不支持
	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif

}

/**********************************************************************
* 函数名称：
*      EA_ucMIFAuthentication(DevHandle hDevHandle,uchar ucMode, uchar ucSecNr)
* 功能描述：
*     使用存放在非接触卡模块中的密钥对卡片指定扇区进行认证
* 输入参数：
*    hDevHandle     设备句柄
*    ucMode： 0--15     A套第1～16组密码
*             16～31    B套第1～16组密码
*    ucSecnr：扇区号
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
* 备    注：
***********************************************************************/
uchar EA_ucMIFAuthentication(DevHandle hDevHandle, uchar ucMode, uchar ucSecNr)
{
#ifdef EM_RC531_Module
	return EA_ucMIFAuthExtend(hDevHandle, ucMode,
		(uchar) ((ucSecNr + 1) * EM_mifs_MIFARE_BLOCKNUMPERSECTOR - 1));
#else
	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;
#endif

}

/**********************************************************************
* 函数名称：
*      EA_ucMIFAuthToRam(DevHandle hDevHandle, uchar ucSecNr,uchar ucKeyType,
*                               uchar *pucKey);
* 功能描述：
*     根据提供的密钥对指定的扇区进行认证
* 输入参数：
*    hDevHandle     设备句柄
*    ucKeyType EM_mifs_KEYA A套密码
*              EM_mifs_KEYB B套密码
*    ucSecnr：对于S50结构的Mifare One卡，其扇区号（0--15）
*             对于S70结构的Mifare One卡，其扇区号（0--39）
*    pucKey: 存放需要进行认证的密钥
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFAuthToRam(DevHandle hDevHandle, uchar ucSecNr, uchar ucKeyType, uchar * pucKey)
{
	uchar i = 0, j = 0, k = 0;
	uchar ucResult;

	if (pucKey == EM_NULL)
	{
		return EM_ERRPARAM;
	}

	if ((ucKeyType != EM_mifs_KEYA) && (ucKeyType != EM_mifs_KEYB))
	{
		return EM_ERRPARAM;
	}

	if (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S50)
	{
		if (ucSecNr > 15)
		{
			return EM_ERRPARAM;
		}
	}
	if (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S70)
	{
		if (ucSecNr > 39)
		{
			return EM_ERRPARAM;
		}
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        i = i;
        j = j;
        k = k;
        memcpy(gtRfidProInfo.gMifareKey,pucKey,6);//暂存密钥
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
	{

#ifdef EM_RC663_Module

		i = i;
		j = j;
		k = k;
		memcpy(gtRfidProInfo.gMifareKey,pucKey,6);//暂存密钥
		//ucResult = EI_mifs_ucAuthentication(ucKeyType, (uchar) ((ucSecNr + 1) * EM_mifs_MIFARE_BLOCKNUMPERSECTOR - 1));

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		i = i;
		j = j;
		k = k;
		memcpy(gtRfidProInfo.gMifareKey,pucKey,6);//暂存密钥
		//ucResult = EI_mifs_ucAuthentication(ucKeyType, (uchar) ((ucSecNr + 1) * EM_mifs_MIFARE_BLOCKNUMPERSECTOR - 1));

#endif

	}
#ifdef EM_RC531_Module
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{


/**********************************************************************
 载入密钥
***********************************************************************/
        // 准备发送数据: 密钥(12字节),6字节密钥扩展为12字节
        // 每半字节进行取反扩展(高字节取反)
        for (i = 0; i < 6; i++)
        {
            k = (pucKey[i] & 0xF0) >> 4;
            j = pucKey[i] & 0x0F;
            EG_mifs_tWorkInfo.aucBuffer[i * 2] = ((k << 4) ^ 0xF0) + k;
            EG_mifs_tWorkInfo.aucBuffer[i * 2 + 1] = ((j << 4) ^ 0xF0) + j;
        }

        EI_mifs_vFlushFIFO();				// empty FIFO

        // 设置定时器
        EI_mifs_vSetTimer(4);
        EG_mifs_tWorkInfo.ulSendBytes = 12;	// how many bytes to send
        EG_mifs_tWorkInfo.ucDisableDF = 0;

        // 执行命令
        ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_LOADKEY);
        if (ucResult != EM_SUCCESS)
        {
            //goto mifsAuthorRamEnd;
            EI_mifs_vEndProtect();
            return ucResult;
        }
	}
#endif
	if (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S50)
	{
		ucResult =
			EI_mifs_ucAuthentication(ucKeyType,
			(uchar) ((ucSecNr + 1) * EM_mifs_MIFARE_BLOCKNUMPERSECTOR - 1));
	}
	else if (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S70)
	{
		if (ucSecNr < 32)
		{
			// 前面的32个扇区密钥存储在第3个Block
			ucResult =
				EI_mifs_ucAuthentication(ucKeyType,
				(uchar) ((ucSecNr + 1) * EM_mifs_MIFARE_BLOCKNUMPERSECTOR - 1));
		}
		else
		{
			// 后面8个扇区的密钥存储在第15个Block
			ucResult =
				EI_mifs_ucAuthentication(ucKeyType, (uchar) (32 * 4 + (ucSecNr - 32) * 16 + 15));
		}
	}
	else
	{
		//非逻辑加密卡
		ucResult = 0xFF;
	}

  //mifsAuthorRamEnd:
	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************
* 函数名称：
*     uchar EA_ucMIFAuthToRamExtend(DevHandle hDevHandle, uchar ucAddr,uchar ucKeyType,
*                               uchar *pucKey);
* 功能描述：
*     根据提供的密钥对指定的扇区进行认证
* 输入参数：
*    hDevHandle     设备句柄
*    ucKeyType EM_mifs_KEYA A套密码
*              EM_mifs_KEYB B套密码
*    ucAddr：扇区密钥所在块号
*    pucKey: 存放需要进行认证的密钥
* 输出参数：
*     无
* 返回值：
*     认证失败有两种值
*		EM_mifs_NOTAGERR----无卡
*		EM_mifs_AUTHERR ----不可能认证
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFAuthToRamExtend(DevHandle hDevHandle, uchar ucAddr, uchar ucKeyType, uchar * pucKey)
{
	uchar i = 0, j = 0, k = 0;
	uchar ucResult;

	if ((ucKeyType != EM_mifs_KEYA) && (ucKeyType != EM_mifs_KEYB))
	{
		return EM_ERRPARAM;
	}

	if (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S50)
	{
		if (ucAddr > 63)
		{
			return EM_ERRPARAM;
		}
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        i = i;
        j = j;
        k = k;
		memcpy(gtRfidProInfo.gMifareKey,pucKey,6);//暂存密钥
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
	{

#ifdef EM_RC663_Module

		i = i;
		j = j;
		k = k;
		memcpy(gtRfidProInfo.gMifareKey,pucKey,6);//暂存密钥

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		i = i;
		j = j;
		k = k;
		memcpy(gtRfidProInfo.gMifareKey,pucKey,6);//暂存密钥

#endif

	}
#ifdef EM_RC531_Module
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{
/**********************************************************************
  载入密钥
 ***********************************************************************/
        // 准备发送数据: 密钥(12字节)
        for (i = 0; i < 6; i++)
        {
            k = (pucKey[i] & 0xF0) >> 4;
            j = pucKey[i] & 0x0F;
            EG_mifs_tWorkInfo.aucBuffer[i * 2] = ((k << 4) ^ 0xF0) + k;
            EG_mifs_tWorkInfo.aucBuffer[i * 2 + 1] = ((j << 4) ^ 0xF0) + j;
        }

        EI_mifs_vFlushFIFO();				// empty FIFO

        // 设置定时器
        EI_mifs_vSetTimer(4);
        EG_mifs_tWorkInfo.ulSendBytes = 12;	// how many bytes to send
        EG_mifs_tWorkInfo.ucDisableDF = 0;

        // 执行命令
        ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_LOADKEY);
        if (ucResult != EM_SUCCESS)
        {
            //goto mifsAuthorRamEnd;
            EI_mifs_vEndProtect();
            return ucResult;
        }
	}
#endif
	ucResult = EI_mifs_ucAuthentication(ucKeyType, ucAddr);

  //mifsAuthorRamEnd:
	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************
* 函数名称：
*     uchar EA_ucMIFLoadKey(DevHandle hDevHandle,uchar ucMode, uchar *pucNKey);
* 功能描述：
*     把指定扇区的密码传给非接触卡模块
* 输入参数：
*    hDevHandle     设备句柄
*    ucMode： 0--15     A套第1～16组密码
*             16～31    B套第1～16组密码
*    pucNKey  要传送的密码即实际与卡进行质询的密码（6字节）
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
* 备    注：
***********************************************************************/
uchar EA_ucMIFLoadKey(DevHandle hDevHandle, uchar ucMode, uchar * pucNKey)
{
#ifdef EM_RC531_Module
	uchar ucTemp1, ucTemp2;
	uint i;
	uchar ucResult;
	ushort usAddr;

	if (pucNKey == EM_NULL)
	{
		return EM_ERRPARAM;
	}

	usAddr = ucMode;
	if (usAddr < EM_mifs_KEYNUM)
	{
		usAddr = usAddr * 0x18 + 0x80;
	}
	else if (usAddr < EM_mifs_KEYNUM * 2)
	{
		usAddr = (usAddr - EM_mifs_KEYNUM) * 0x18 + 0x8C;
	}
	else
	{
		return EM_ERRPARAM;
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

	// 准备发送数据: EEPROM地址(2)+密钥（12）
	EG_mifs_tWorkInfo.ulSendBytes = 14;

	// 存放EEPROM地址
	EG_mifs_tWorkInfo.aucBuffer[0] = (uchar) usAddr;
	EG_mifs_tWorkInfo.aucBuffer[1] = (uchar) (usAddr >> 8);

	// 将密钥换算成EEPROM规定的格式
	for (i = 0; i < 6; i++)
	{
		ucTemp1 = pucNKey[i] >> 4;
		ucTemp2 = pucNKey[i] & 0x0F;
		EG_mifs_tWorkInfo.aucBuffer[i * 2 + 2] = (~ucTemp1 << 4) | ucTemp1;
		EG_mifs_tWorkInfo.aucBuffer[i * 2 + 3] = (~ucTemp2 << 4) | ucTemp2;
	}
	EG_mifs_tWorkInfo.ucDisableDF = 0;

	// long timeout
	EI_mifs_vSetTimer(9);

	// 发送命令
	ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_WRITEE2);

	EI_mifs_vEndProtect();
	return ucResult;

#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif

}

/**********************************************************************
* 函数名称：
*     uchar EA_ucMIFHalt(DevHandle hDevHandle);
* 功能描述：
*     将卡置于睡眠模式，从而使卡退出通讯进程
* 输入参数：
*    hDevHandle     设备句柄
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFHalt(DevHandle hDevHandle)
{
	uchar ucResult;
	uchar ucTempData;
	uchar ucCMD = 0,ucRegAddr = 0;

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

	// 定时器时间设置
	EI_mifs_vSetTimer(106);

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucRegAddr = 0;
        ucTempData = 0;
        ucTempData = ucTempData;
        ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
        EG_mifs_tWorkInfo.expectMaxRec = 0;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, 0x01);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, 0x01);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
		ucRegAddr = PHHAL_HW_RC663_REG_COMMAND;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, 0x80);
		EI_mifs_vSetBitMask(RxModeReg, 0x80);
		ucCMD = PN512CMD_TRANSCEIVE;
		ucRegAddr = CommandReg;

#endif

	}
#ifdef EM_RC531_Module
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{
		// 发送帧最后一个字节为CRC校验，每个字节带奇校验
		ucTempData = 0x07;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;
		ucRegAddr = EM_mifs_REG_COMMAND;
	}
#endif
	// 准备发送数据: 停止命令码, 0
	EG_mifs_tWorkInfo.ulSendBytes = 2;
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_HALT;	// Halt command code
	EG_mifs_tWorkInfo.aucBuffer[1] = 0x00;	// dummy address

	// 执行命令
    gtRfidProInfo.Cmd = EM_mifs_PICC_HALT;
	ucResult = EI_mifs_ucHandleCmd(ucCMD);
    gtRfidProInfo.Cmd = 0;

	// 超时或者执行成功, 都算成功
	if ((ucResult == EM_mifs_NOTAGERR) || (ucResult == EM_TIMEOUT))
	{
		ucResult = EM_SUCCESS;
	}

	// 复位命令寄存器
	if(EG_mifs_tWorkInfo.RFIDModule != RFID_Module_AS3911)
    {
        ucTempData = EM_mifs_PCD_IDLE;
        EI_mifs_vWriteReg(1, ucRegAddr, &ucTempData);
    }

	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************
* 函数名称：
*      s_rfid_UpdataCrc  s_rfid_CrcAB
* 功能描述：
*       计算CRC_A和CRC_B校验码
* 输入参数：
*    hDevHandle     设备句柄
*    ucAddr:		对于S50结构Mifare One卡片，其块号（0-63）
*                   对于S70结构Mifare One卡片，其块号(0 - 255)
* 输出参数：
*     pucData: 数据缓冲区（16字节）
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
#define RFID_CRC_A   0
#define RFID_CRC_B   1
ushort s_rfid_UpdataCrc(uchar ch, ushort *lpwCrc)       //以字节为单位进行计算   
{
    ch = (ch ^ (uchar)((*lpwCrc) & 0x00ff));
    ch = (ch ^ (ch << 4));
    *lpwCrc = (*lpwCrc >> 8) ^ ((ushort)ch << 8)
        ^ ((ushort)ch << 3) ^ ((ushort)ch >> 4);
    return(*lpwCrc);
}

ushort s_rfid_CrcAB(uchar Crctype, uchar *Data, int Sum, ushort* wCrc)
{
    uchar chBlock;
    //ushort wCrc = 0X6B7D;//0x6363;         //寄存器初始化   

    if ( Crctype == RFID_CRC_A ) {
        *wCrc = 0x6363;
    }else{
        *wCrc = 0xFFFF;
    }
    do{
        chBlock = *Data++;
        s_rfid_UpdataCrc(chBlock, wCrc);
    } while (--Sum);

    if ( Crctype == RFID_CRC_B ) {
        *wCrc = ~(*wCrc);
    }
    return *wCrc;
}
/**********************************************************************
* 函数名称：
*      EA_ucMIFRead(DevHandle hDevHandle,uchar ucAddr, uchar *pucData)
* 功能描述：
*     读取一个16字节的数据块
* 输入参数：
*    hDevHandle     设备句柄
*    ucAddr:		对于S50结构Mifare One卡片，其块号（0-63）
*                   对于S70结构Mifare One卡片，其块号(0 - 255)
* 输出参数：
*     pucData: 数据缓冲区（16字节）
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFRead(DevHandle hDevHandle, uchar ucAddr, uchar * pucData)
{
    uchar ucTempData = 0;
	uchar ucResult;
	uchar ucCMD = 0;

	if (pucData == EM_NULL)
	{
		return EM_ERRPARAM;
	}

	if (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S50)
	{
		if (ucAddr > 63)
		{
			return EM_ERRPARAM;
		}
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

	// 清空缓冲区
	EI_mifs_vFlushFIFO();				// empty FIFO

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;

        u16 miflen = 0;
        u16 usCrc = 0;

        memset(EG_mifs_tWorkInfo.aucBuffer, 0, 18);
        EG_mifs_tWorkInfo.ulSendBytes = 2;
        EG_mifs_tWorkInfo.ucDisableDF = 0;
        EG_mifs_tWorkInfo.aucBuffer[1] = ucAddr;
        EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_READ;	// read command code

        ucResult = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                   (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                   EG_mifs_tWorkInfo.aucBuffer,
                                   (16+2),
                                   &miflen,
                                   MIFARE_TRANSCEIVE_DEFTIME,
                                   FALSE);
        if ( (ucResult == EM_SUCCESS) && (miflen >= 16) ) {
            memcpy(pucData, EG_mifs_tWorkInfo.aucBuffer, 16);
            s_rfid_CrcAB(0, EG_mifs_tWorkInfo.aucBuffer, 16, &usCrc);
//            TRACE("\r\n --------crc %x--:%x %x",usCrc,EG_mifs_tWorkInfo.aucBuffer[17],EG_mifs_tWorkInfo.aucBuffer[16]);
            if ( (((uchar)(usCrc&0xFF)) == EG_mifs_tWorkInfo.aucBuffer[16]) && 
                 (((uchar)((usCrc>>8)&0xFF)) == EG_mifs_tWorkInfo.aucBuffer[17]) )
            {
                //crc_a succeed
                ucResult = EM_SUCCESS;
            }else{
                ucResult = EM_mifs_CODEERR;
            }
        }else{
            ucResult = EM_mifs_CODEERR;
        }
        EI_mifs_vEndProtect();
        return ucResult;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = 0x01;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT);				// long timeout
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = 0x80;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT);				// long timeout
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, ucTempData);
		EI_mifs_vSetBitMask(RxModeReg, ucTempData);
		ucCMD = PN512CMD_TRANSCEIVE;

#endif

	}
#ifdef EM_RC531_Module
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

		EI_mifs_vSetTimer(4);				// long timeout
		// 发送和接收帧最后一个字节为CRC校验，每个字节带奇校验
		ucTempData = 0x0F;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;
	}
#endif
	// 准备发送数据: 命令码, 地址
	EG_mifs_tWorkInfo.ulSendBytes = 2;
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[1] = ucAddr;
	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_READ;	// read command code

	// 执行命令
	ucResult = EI_mifs_ucHandleCmd(ucCMD);

	// 判断结果
	if ((ucResult != EM_SUCCESS) || (EG_mifs_tWorkInfo.lBitsReceived != 0x80))
	{
		// 接收不成功，或者没有收满16个字节
		if (ucResult != EM_mifs_NOTAGERR)
		{

			if ((ucResult == EM_mifs_CRCERR)	// 认证成不功时，肯定会发生CRC错误。因为卡不回送CRC字节
				&& ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x0a) == 0)	// 接收到的未认证信息
				&& (EG_mifs_tWorkInfo.lBitsReceived == 0))	// 卡在等待超时中断时，会自动清除寄存器的RxLastBits，导致lBitsReceived = 0;
			{
				ucResult = EM_mifs_NOTAUTHERR;
			}
			// 不是无卡错误, 则要求收到4位或者8位，即处理结果
			else if ((EG_mifs_tWorkInfo.lBitsReceived != 4)
				&& (EG_mifs_tWorkInfo.lBitsReceived != 8))
			{
				ucResult = EM_mifs_BITCOUNTERR;
			}
			else
			{
				// 返回数据为处理结果
				EG_mifs_tWorkInfo.aucBuffer[0] &= 0x0f;	// mask out upper nibble
				if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x0a) == 0)
				{
					ucResult = EM_mifs_NOTAUTHERR;
				}
				else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x0a)
				{
					ucResult = EM_SUCCESS;
				}
				else
				{
					ucResult = EM_mifs_CODEERR;
				}
			}							// if(EG_mifs_tWorkInfo.lBitsReceived != 4 && EG_mifs_tWorkInfo.lBitsReceived != 8)
		}								// if(ucResult != EM_mifs_NOTAGERR)
	}									// if(reuslt != EM_SUCCESS || EG_mifs_tWorkInfo.lBitsReceived != 0x80)
	else
	{
		// 接收数据
		memcpy(pucData, EG_mifs_tWorkInfo.aucBuffer, 16);
	}

	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************
* 函数名称：
*     EA_ucMIFWrite(DevHandle hDevHandle,uchar ucAddr, uchar *pucData)
* 功能描述：
*     读取一个16字节的数据块
* 输入参数：
*    hDevHandle     设备句柄
*    ucAddr:		对于S50结构Mifare One卡片，其块号（0-63）
*                   对于S70结构Mifare One卡片，其块号(0 - 255)
*     pucData: 数据缓冲区（16字节）
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFWrite(DevHandle hDevHandle, uchar ucAddr, uchar * pucData)
{
    uchar ucTempData = 0;
	uchar ucResult;
	uchar ucCMD = 0;
	uint time = 0;

	if (pucData == EM_NULL)
	{
		return EM_ERRPARAM;
	}

	if (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S50)
	{
		if (ucAddr > 63)
		{
			return EM_ERRPARAM;
		}
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

	// 清空缓冲区
	EI_mifs_vFlushFIFO();				// empty FIFO

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;

        u16 miflen = 0;

        EG_mifs_tWorkInfo.ulSendBytes = 2;
        EG_mifs_tWorkInfo.ucDisableDF = 0;
        EG_mifs_tWorkInfo.aucBuffer[1] = ucAddr;
        EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_WRITE;	// read command code

        ucResult = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                   (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                   EG_mifs_tWorkInfo.aucBuffer,
                                   1,
                                   &miflen,
                                   MIFARE_TRANSCEIVE_DEFTIME,
                                   TRUE);
//        TRACE("\r\n11--ret:%d  len:%d",ucResult,miflen);
//        DISPBUF(EG_mifs_tWorkInfo.aucBuffer, miflen, 0);
        if ( (ucResult == EM_SUCCESS) && (miflen >= 1)) {
            EG_mifs_tWorkInfo.ulSendBytes = 16;
            memcpy(EG_mifs_tWorkInfo.aucBuffer, pucData, 16);
            EG_mifs_tWorkInfo.ucDisableDF = 0;
            ucResult = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                         (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                         EG_mifs_tWorkInfo.aucBuffer,
                                         1,
                                         &miflen,
                                         MIFARE_TRANSCEIVE_DEFTIME,
                                         TRUE);
            if ( (ucResult == EM_SUCCESS) && (miflen >= 1)) {
                ucResult = EM_SUCCESS;
            }else
                ucResult = EM_mifs_CODEERR;
        }else{
            ucResult = EM_mifs_CODEERR;
        }
        EI_mifs_vEndProtect();
        return ucResult;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = 0x01;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT);				// long timeout
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);    //接收不需要CRC校验
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
		time = EM_mifs_DEFAULT*5;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = 0x80;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT);
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, ucTempData);
		EI_mifs_vClrBitMask(RxModeReg, ucTempData);    //接收不需要CRC校验
		ucCMD = PN512CMD_TRANSCEIVE;
		time = EM_mifs_DEFAULT*5;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		// 设置定时器时间
		EI_mifs_vSetTimer(4);

		// 发送帧最后一个字节为CRC校验，每个字节带奇校验
		ucTempData = 0x07;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;
		time = 6;

#endif

	}

	// 准备发送数据: 命令码+地址
	EG_mifs_tWorkInfo.ulSendBytes = 2;
	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_WRITE;
	EG_mifs_tWorkInfo.aucBuffer[1] = ucAddr;
	EG_mifs_tWorkInfo.ucDisableDF = 0;

	// 执行命令
	ucResult = EI_mifs_ucHandleCmd(ucCMD);

	if (ucResult != EM_mifs_NOTAGERR)
	{
		// 不是无卡错误, 则要求收到4位或者8位，即处理结果
		if ((EG_mifs_tWorkInfo.lBitsReceived != 4) && (EG_mifs_tWorkInfo.lBitsReceived != 8))
		{
			ucResult = EM_mifs_BITCOUNTERR;
		}
		else
		{
			EG_mifs_tWorkInfo.aucBuffer[0] &= 0x0f;	// mask out upper nibble
			if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x0a) == 0)
			{
				ucResult = EM_mifs_NOTAUTHERR;
			}
			else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x0a)
			{
				ucResult = EM_SUCCESS;
			}
			else
			{
				ucResult = EM_mifs_CODEERR;
			}
		}								// if(EG_mifs_tWorkInfo.lBitsReceived != 4 && EG_mifs_tWorkInfo.lBitsReceived != 8)
	}

	// 成功，则发送要写入的数据
	if (ucResult == EM_SUCCESS)
	{
		// long timeout
		EI_mifs_vSetTimer(time);

		// 准备发送数据: 16个字节的数据
		EG_mifs_tWorkInfo.ulSendBytes = 16;
		memcpy(EG_mifs_tWorkInfo.aucBuffer, pucData, 16);
		EG_mifs_tWorkInfo.ucDisableDF = 0;

		// 执行命令
		ucResult = EI_mifs_ucHandleCmd(ucCMD);
		if (ucResult != EM_mifs_NOTAGERR)
		{
			// 不是无卡错误, 则要求收到4位或者8位，即处理结果
			if ((EG_mifs_tWorkInfo.lBitsReceived != 4) && (EG_mifs_tWorkInfo.lBitsReceived != 8))
			{
				ucResult = EM_mifs_BITCOUNTERR;
			}
			else
			{
				EG_mifs_tWorkInfo.aucBuffer[0] &= 0x0f;	// mask out upper nibble
				if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x0a) == 0)
				{
					ucResult = EM_mifs_NOTAUTHERR;
				}
				else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x0a)
				{
					ucResult = EM_SUCCESS;
				}
				else
				{
					ucResult = EM_mifs_CODEERR;
				}
			}							// if(EG_mifs_tWorkInfo.lBitsReceived != 4 && EG_mifs_tWorkInfo.lBitsReceived != 8)
		}								// if(ucResult != EM_mifs_NOTAGERR)
	}									// if(ucResult == EM_SUCCESS)

	EI_mifs_vEndProtect();
	return (ucResult);
}

/**********************************************************************
* 函数名称：
*      EA_ucMIFIncrement(DevHandle hDevHandle,uchar ucAddr, ulong ulValue)
* 功能描述：
*     把指定的值加至一数值块中
* 输入参数：
*    hDevHandle     设备句柄
*    ucAddr:		对于S50结构Mifare One卡片，其块号（0-63）
*                   对于S70结构Mifare One卡片，其块号(0 - 255)
*     ulValue: 指定的数值
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFIncrement(DevHandle hDevHandle, uchar ucAddr, ulong ulValue)
{
	uchar ucResult;
    uchar ucTempData = 0;
	uchar i;
	uchar ucCMD = 0;
	uint time = 0;

	if (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S50)
	{
		if (ucAddr > 63)
		{
			return EM_ERRPARAM;
		}
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 1;

	// 填写命令缓冲区
	EG_mifs_tWorkInfo.aucCmdBuf[0] = EM_mifs_PICC_INCREMENT;
	EG_mifs_tWorkInfo.aucCmdBuf[1] = ucAddr;
	memcpy(EG_mifs_tWorkInfo.aucCmdBuf + 2, (void *)&ulValue, 4);

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;

        u16 miflen = 0;

        EG_mifs_tWorkInfo.ulSendBytes = 2;
        EG_mifs_tWorkInfo.ucDisableDF = 0;
        EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.aucCmdBuf[0];
        EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.aucCmdBuf[1];
        ucResult = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                     (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                     EG_mifs_tWorkInfo.aucBuffer,
                                     2,
                                     &miflen,
                                     MIFARE_TRANSCEIVE_DEFTIME,
                                     TRUE);
        if ( ucResult == EM_SUCCESS ) {
            EG_mifs_tWorkInfo.ulSendBytes = 4;
            memcpy(EG_mifs_tWorkInfo.aucBuffer, &EG_mifs_tWorkInfo.aucCmdBuf[2], 4);
            EG_mifs_tWorkInfo.ucDisableDF = 0;
            ucResult = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                         (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                         EG_mifs_tWorkInfo.aucBuffer,
                                         2,
                                         &miflen,
                                         MIFARE_TRANSCEIVE_DEFTIME,
                                         TRUE);
            ucResult = EM_SUCCESS;
        }else{
            ucResult = EM_mifs_CODEERR;
        }
        EI_mifs_vEndProtect();
        return ucResult;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = 0x01;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT*4);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);    //接收不需要CRC校验
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
		time = EM_mifs_DEFAULT*4;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = 0x80;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT*4);
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, ucTempData);
		EI_mifs_vClrBitMask(RxModeReg, ucTempData);    //接收不需要CRC校验
		ucCMD = PN512CMD_TRANSCEIVE;
		time = EM_mifs_DEFAULT*4;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		// short timeout
		EI_mifs_vSetTimer(6);

		// 发送帧最后一个字节为CRC校验，每个字节带奇校验
		ucTempData = 0x07;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;
		time = 6;

#endif

	}

	// 第一次先发送: 命令码+源块号
	EG_mifs_tWorkInfo.ulSendBytes = 2;
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.aucCmdBuf[0];
	EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.aucCmdBuf[1];

	ucResult = EI_mifs_ucHandleCmd(ucCMD);

	if (ucResult != EM_mifs_NOTAGERR)
	{
		// 不是无卡错误, 必须接收4位或者8位
		if ((EG_mifs_tWorkInfo.lBitsReceived != 4) && (EG_mifs_tWorkInfo.lBitsReceived != 8))
		{
			ucResult = EM_mifs_BITCOUNTERR;
		}
		else
		{
			EG_mifs_tWorkInfo.aucBuffer[0] &= 0x0f;	// mask out upper nibble
			if (EG_mifs_tWorkInfo.aucBuffer[0] == 0)
			{
				ucResult = EM_mifs_NOTAUTHERR;
			}
			else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x0a)
			{
				ucResult = EM_SUCCESS;
			}
			else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x01)
			{
				ucResult = EM_mifs_VALERR;
			}
			else
			{
				ucResult = EM_mifs_CODEERR;
			}
		}								// if(EG_mifs_tWorkInfo.lBitsReceived != 4 && EG_mifs_tWorkInfo.lBitsReceived != 8)
	}									// if(ucResult != EM_mifs_NOTAGERR)

	if (ucResult == EM_SUCCESS)
	{
		// 第二次命令执行

		// short timeout
		EI_mifs_vSetTimer(time);

		// 准备发送数据: 修改值（4字节）
		EG_mifs_tWorkInfo.ulSendBytes = 4;
		for (i = 0; i < 4; i++)
		{
			EG_mifs_tWorkInfo.aucBuffer[i] = EG_mifs_tWorkInfo.aucCmdBuf[i + 2];
		}
		EG_mifs_tWorkInfo.ucDisableDF = 0;

		// 执行命令
		ucResult = EI_mifs_ucHandleCmd(ucCMD);

		if (ucResult == EM_SUCCESS)
		{
			// 无超时错误, 必须接收4位或者8位
			if ((EG_mifs_tWorkInfo.lBitsReceived != 4) && (EG_mifs_tWorkInfo.lBitsReceived != 8))
			{
				ucResult = EM_mifs_BITCOUNTERR;
			}
			else
			{
				EG_mifs_tWorkInfo.aucBuffer[0] &= 0x0f;	// mask out upper nibble
				if (EG_mifs_tWorkInfo.aucBuffer[0] == 0)
				{
					ucResult = EM_mifs_NOTAUTHERR;
				}
				else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x01)
				{
					ucResult = EM_mifs_VALERR;
				}
				else
				{
					ucResult = EM_mifs_CODEERR;
				}
			}							// if(EG_mifs_tWorkInfo.lBitsReceived != 4 && EG_mifs_tWorkInfo.lBitsReceived != 8)
		}								// if(ucResult == EM_SUCCESS)
		else if (ucResult == EM_mifs_NOTAGERR)
		{
			// 没响应算成功
			ucResult = EM_SUCCESS;
		}
	}									// if(ucResult == EM_SUCCESS)

	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************
* 函数名称：
*      EA_ucMIFDecrement(DevHandle hDevHandle,uchar ucAddr, ulong ulValue)
* 功能描述：
*     从一数值块中减去指定的值
* 输入参数：
*    hDevHandle     设备句柄
*    ucAddr:		对于S50结构Mifare One卡片，其块号（0-63）
*                   对于S70结构Mifare One卡片，其块号(0 - 255)
*     ulValue: 指定的数值
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFDecrement(DevHandle hDevHandle, uchar ucAddr, ulong ulValue)
{
	uchar ucResult;
    uchar ucTempData = 0;
	uchar i = 0;
	uchar ucCMD = 0;
	uint time = 0;

	if (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S50)
	{
		if (ucAddr > 63)
		{
			return EM_ERRPARAM;
		}
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 1;

	// 填写命令缓冲区
	EG_mifs_tWorkInfo.aucCmdBuf[0] = EM_mifs_PICC_DECREMENT;
	EG_mifs_tWorkInfo.aucCmdBuf[1] = ucAddr;
	memcpy(EG_mifs_tWorkInfo.aucCmdBuf + 2, (void *)&ulValue, 4);

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;

        u16 miflen = 0;

        EG_mifs_tWorkInfo.ulSendBytes = 2;
        EG_mifs_tWorkInfo.ucDisableDF = 0;
        EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.aucCmdBuf[0];
        EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.aucCmdBuf[1];
        ucResult = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                     (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                     EG_mifs_tWorkInfo.aucBuffer,
                                     2,
                                     &miflen,
                                     MIFARE_TRANSCEIVE_DEFTIME,
                                     TRUE);
        if ( ucResult == EM_SUCCESS ) {
            EG_mifs_tWorkInfo.ulSendBytes = 4;
            memcpy(EG_mifs_tWorkInfo.aucBuffer, &EG_mifs_tWorkInfo.aucCmdBuf[2], 4);
            EG_mifs_tWorkInfo.ucDisableDF = 0;
            ucResult = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                         (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                         EG_mifs_tWorkInfo.aucBuffer,
                                         2,
                                         &miflen,
                                         MIFARE_TRANSCEIVE_DEFTIME,
                                         TRUE);
            ucResult = EM_SUCCESS;
        }else{
            ucResult = EM_mifs_CODEERR;
        }
        EI_mifs_vEndProtect();
        return ucResult;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = 0x01;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT*4);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);    //接收不需要CRC校验
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
		time = EM_mifs_DEFAULT*4;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = 0x80;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT*4);
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, ucTempData);
		EI_mifs_vClrBitMask(RxModeReg, ucTempData);    //接收不需要CRC校验
		ucCMD = PN512CMD_TRANSCEIVE;
		time = EM_mifs_DEFAULT*4;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		// short timeout
		EI_mifs_vSetTimer(6);

		// 发送帧最后一个字节为CRC校验，每个字节带奇校验
		ucTempData = 0x07;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;
		time = 6;

#endif

	}

	// 第一次先发送: 命令码+源块号
	EG_mifs_tWorkInfo.ulSendBytes = 2;
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.aucCmdBuf[0];
	EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.aucCmdBuf[1];

	ucResult = EI_mifs_ucHandleCmd(ucCMD);

	if (ucResult != EM_mifs_NOTAGERR)
	{
		// 不是无卡错误, 必须接收4位或者8位
		if ((EG_mifs_tWorkInfo.lBitsReceived != 4) && (EG_mifs_tWorkInfo.lBitsReceived != 8))
		{
			ucResult = EM_mifs_BITCOUNTERR;
		}
		else
		{
			EG_mifs_tWorkInfo.aucBuffer[0] &= 0x0f;	// mask out upper nibble
			if (EG_mifs_tWorkInfo.aucBuffer[0] == 0)
			{
				ucResult = EM_mifs_NOTAUTHERR;
			}
			else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x0a)
			{
				ucResult = EM_SUCCESS;
			}
			else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x01)
			{
				ucResult = EM_mifs_VALERR;
			}
			else
			{
				ucResult = EM_mifs_CODEERR;
			}
		}								// if(EG_mifs_tWorkInfo.lBitsReceived != 4 && EG_mifs_tWorkInfo.lBitsReceived != 8)
	}									// if(ucResult != EM_mifs_NOTAGERR)

	if (ucResult == EM_SUCCESS)
	{
		// 第二次命令执行

		// short timeout
		EI_mifs_vSetTimer(time);

		// 准备发送数据: 修改值（4字节）
		EG_mifs_tWorkInfo.ulSendBytes = 4;
		for (i = 0; i < 4; i++)
		{
			EG_mifs_tWorkInfo.aucBuffer[i] = EG_mifs_tWorkInfo.aucCmdBuf[i + 2];
		}
		EG_mifs_tWorkInfo.ucDisableDF = 0;

		// 执行命令
		ucResult = EI_mifs_ucHandleCmd(ucCMD);

		if (ucResult == EM_SUCCESS)
		{
			// 无超时错误, 必须接收4位或者8位
			if ((EG_mifs_tWorkInfo.lBitsReceived != 4) && (EG_mifs_tWorkInfo.lBitsReceived != 8))
			{
				ucResult = EM_mifs_BITCOUNTERR;
			}
			else
			{
				EG_mifs_tWorkInfo.aucBuffer[0] &= 0x0f;	// mask out upper nibble
				if (EG_mifs_tWorkInfo.aucBuffer[0] == 0)
				{
					ucResult = EM_mifs_NOTAUTHERR;
				}
				else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x01)
				{
					ucResult = EM_mifs_VALERR;
				}
				else
				{
					ucResult = EM_mifs_CODEERR;
				}
			}							// if(EG_mifs_tWorkInfo.lBitsReceived != 4 && EG_mifs_tWorkInfo.lBitsReceived != 8)
		}								// if(ucResult == EM_SUCCESS)
		else if (ucResult == EM_mifs_NOTAGERR)
		{
			// 没响应算成功
			ucResult = EM_SUCCESS;
		}
	}									// if(ucResult == EM_SUCCESS)

	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************
* 函数名称：
*     EA_ucMIFRestore(DevHandle hDevHandle,uchar ucAddr)
* 功能描述：
*     拷贝指定数值块的内容到内存中
* 输入参数：
*    hDevHandle     设备句柄
*    ucAddr:		对于S50结构Mifare One卡片，其块号（0-63）
*                   对于S70结构Mifare One卡片，其块号(0 - 255)
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFRestore(DevHandle hDevHandle, uchar ucAddr)
{
	uchar ucResult;
    uchar ucTempData = 0;
	uchar i = 0;
	uchar ucCMD = 0;
	uint time = 0;

	if (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S50)
	{
		if (ucAddr > 63)
		{
			return EM_ERRPARAM;
		}
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EG_mifs_tWorkInfo.ucEnableTransfer = 1;

	// 填写命令缓冲区
	EG_mifs_tWorkInfo.aucCmdBuf[0] = EM_mifs_PICC_RESTORE;
	EG_mifs_tWorkInfo.aucCmdBuf[1] = ucAddr;
	memset(EG_mifs_tWorkInfo.aucCmdBuf + 2, 0, 4);

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;

        u16 miflen = 0;

        EG_mifs_tWorkInfo.ulSendBytes = 2;
        EG_mifs_tWorkInfo.ucDisableDF = 0;
        EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.aucCmdBuf[0];
        EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.aucCmdBuf[1];
        ucResult = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                     (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                     EG_mifs_tWorkInfo.aucBuffer,
                                     2,
                                     &miflen,
                                     MIFARE_TRANSCEIVE_DEFTIME,
                                     TRUE);
        if ( ucResult == EM_SUCCESS ) {
            EG_mifs_tWorkInfo.ulSendBytes = 4;
            memcpy(EG_mifs_tWorkInfo.aucBuffer, &EG_mifs_tWorkInfo.aucCmdBuf[2], 4);
            EG_mifs_tWorkInfo.ucDisableDF = 0;
            ucResult = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                         (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                         EG_mifs_tWorkInfo.aucBuffer,
                                         2,
                                         &miflen,
                                         MIFARE_TRANSCEIVE_DEFTIME,
                                         TRUE);
            ucResult = EM_SUCCESS;
        }else{
            ucResult = EM_mifs_CODEERR;
        }
        EI_mifs_vEndProtect();
        return ucResult;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = 0x01;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT*4);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);    //接收不需要CRC校验
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
		time = EM_mifs_DEFAULT*4;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = 0x80;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT*4);
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, ucTempData);
		EI_mifs_vClrBitMask(RxModeReg, ucTempData);    //接收不需要CRC校验
		ucCMD = PN512CMD_TRANSCEIVE;
		time = EM_mifs_DEFAULT*4;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		// short timeout
		EI_mifs_vSetTimer(6);

		// 发送帧最后一个字节为CRC校验，每个字节带奇校验
		ucTempData = 0x07;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;
		time = 6;

#endif

	}

	// 第一次先发送: 命令码+源块号
	EG_mifs_tWorkInfo.ulSendBytes = 2;
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.aucCmdBuf[0];
	EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.aucCmdBuf[1];

	ucResult = EI_mifs_ucHandleCmd(ucCMD);

	if (ucResult != EM_mifs_NOTAGERR)
	{
		// 不是无卡错误, 必须接收4位或者8位
		if ((EG_mifs_tWorkInfo.lBitsReceived != 4) && (EG_mifs_tWorkInfo.lBitsReceived != 8))
		{
			ucResult = EM_mifs_BITCOUNTERR;
		}
		else
		{
			EG_mifs_tWorkInfo.aucBuffer[0] &= 0x0f;	// mask out upper nibble
			if (EG_mifs_tWorkInfo.aucBuffer[0] == 0)
			{
				ucResult = EM_mifs_NOTAUTHERR;
			}
			else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x0a)
			{
				ucResult = EM_SUCCESS;
			}
			else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x01)
			{
				ucResult = EM_mifs_VALERR;
			}
			else
			{
				ucResult = EM_mifs_CODEERR;
			}
		}								// if(EG_mifs_tWorkInfo.lBitsReceived != 4 && EG_mifs_tWorkInfo.lBitsReceived != 8)
	}									// if(ucResult != EM_mifs_NOTAGERR)

	if (ucResult == EM_SUCCESS)
	{
		// 第二次命令执行

		// short timeout
		EI_mifs_vSetTimer(time);

		// 准备发送数据: 修改值（4字节）
		EG_mifs_tWorkInfo.ulSendBytes = 4;
		for (i = 0; i < 4; i++)
		{
			EG_mifs_tWorkInfo.aucBuffer[i] = EG_mifs_tWorkInfo.aucCmdBuf[i + 2];
		}
		EG_mifs_tWorkInfo.ucDisableDF = 0;

		// 执行命令
		ucResult = EI_mifs_ucHandleCmd(ucCMD);

		if (ucResult == EM_SUCCESS)
		{
			// 无超时错误, 必须接收4位或者8位
			if ((EG_mifs_tWorkInfo.lBitsReceived != 4) && (EG_mifs_tWorkInfo.lBitsReceived != 8))
			{
				ucResult = EM_mifs_BITCOUNTERR;
			}
			else
			{
				EG_mifs_tWorkInfo.aucBuffer[0] &= 0x0f;	// mask out upper nibble
				if (EG_mifs_tWorkInfo.aucBuffer[0] == 0)
				{
					ucResult = EM_mifs_NOTAUTHERR;
				}
				else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x01)
				{
					ucResult = EM_mifs_VALERR;
				}
				else
				{
					ucResult = EM_mifs_CODEERR;
				}
			}							// if(EG_mifs_tWorkInfo.lBitsReceived != 4 && EG_mifs_tWorkInfo.lBitsReceived != 8)
		}								// if(ucResult == EM_SUCCESS)
		else if (ucResult == EM_mifs_NOTAGERR)
		{
			// 没响应算成功
			ucResult = EM_SUCCESS;
		}
	}									// if(ucResult == EM_SUCCESS)

	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************
* 函数名称：
*     EA_ucMIFTransfer(DevHandle hDevHandle,uchar ucAddr)
* 功能描述：
*     将卡的RAM寄存器的内容拷贝到指定数值块
* 输入参数：
*    hDevHandle     设备句柄
*    ucAddr:		对于S50结构Mifare One卡片，其块号（0-63）
*                   对于S70结构Mifare One卡片，其块号(0 - 255)
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFTransfer(DevHandle hDevHandle, uchar ucAddr)
{
    uchar ucTempData = 0;
	uchar ucResult;
	uchar ucCMD = 0;

	if (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S50)
	{
		if (ucAddr > 63)
		{
			return EM_ERRPARAM;
		}
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	if (EG_mifs_tWorkInfo.ucEnableTransfer == 0)
	{
		// 不允许执行Transfer命令
		ucResult = EM_mifs_CODEERR;
		goto mifsTransferEnd;
	}

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;

        u16 miflen = 0;

        EG_mifs_tWorkInfo.ulSendBytes = 2;
        EG_mifs_tWorkInfo.ucDisableDF = 0;
        EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_TRANSFER;
        EG_mifs_tWorkInfo.aucBuffer[1] = ucAddr;
        ucResult = mifareSendRequest(EG_mifs_tWorkInfo.aucBuffer,
                                     (u16)EG_mifs_tWorkInfo.ulSendBytes,
                                     EG_mifs_tWorkInfo.aucBuffer,
                                     2,
                                     &miflen,
                                     MIFARE_TRANSCEIVE_DEFTIME,
                                     TRUE);
        EG_mifs_tWorkInfo.ucEnableTransfer = 0;
        if ( ucResult != EM_SUCCESS ) {
            ucResult = EM_mifs_CODEERR;
        }
        EI_mifs_vEndProtect();
        return ucResult;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = 0x01;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT*4);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);    //接收不需要CRC校验
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = 0x80;
		EI_mifs_vSetTimer(EM_mifs_DEFAULT*4);
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, ucTempData);
		EI_mifs_vClrBitMask(RxModeReg, ucTempData);    //接收不需要CRC校验
		ucCMD = PN512CMD_TRANSCEIVE;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		// short timeout
		EI_mifs_vSetTimer(6);

		// 发送帧最后一个字节为CRC校验，每个字节带奇校验
		ucTempData = 0x07;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif

	}

	// 准备发送数据: 命令码+源块号+4字节数据+目标块号
	EG_mifs_tWorkInfo.aucCmdBuf[6] = ucAddr;

	// 准备发送数据: 命令码+目标块号
	EG_mifs_tWorkInfo.ulSendBytes = 2;
	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_TRANSFER;
	EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.aucCmdBuf[6];
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	ucResult = EI_mifs_ucHandleCmd(ucCMD);

	if (ucResult != EM_mifs_NOTAGERR)
	{
		// 无超时错误, 必须接收4位或者8位
		if ((EG_mifs_tWorkInfo.lBitsReceived != 4) && (EG_mifs_tWorkInfo.lBitsReceived != 8))
		{
			ucResult = EM_mifs_BITCOUNTERR;
		}
		else
		{
			EG_mifs_tWorkInfo.aucBuffer[0] &= 0x0f;	// mask out upper nibble
			if (EG_mifs_tWorkInfo.aucBuffer[0] == 0)
			{
				ucResult = EM_mifs_NOTAUTHERR;
			}
			else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x0a)
			{
				ucResult = EM_SUCCESS;
			}
			else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x01)
			{
				ucResult = EM_mifs_VALERR;
			}
			else
			{
				ucResult = EM_mifs_CODEERR;
			}
		}								// if(EG_mifs_tWorkInfo.lBitsReceived != 4 && EG_mifs_tWorkInfo.lBitsReceived != 8)
	}									// if(ucResult != EM_mifs_NOTAGERR)

  mifsTransferEnd:
	EG_mifs_tWorkInfo.ucEnableTransfer = 0;
	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************

                          MifarePro卡(TYPE A)函数定义

***********************************************************************/
/**********************************************************************
* 函数名称：
*     EA_ucMIFResetPro(DevHandle hDevHandle, uchar ucMode, uint *puiRespLen, void *pvResp)
* 功能描述：
*     完成mifpro卡的激活操作
* 输入参数：
*    hDevHandle     设备句柄
*    ucMode: 激活模式
* 输出参数：
*     puiRespLen      :  实际返回数据长度
*     pvResp     : 返回的数据,不包括卡类型
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFResetPro(DevHandle hDevHandle, uchar ucMode, uint * puiRespLen, void *pvResp)
{

#ifdef EM_RC531_Module

	ushort tagType;
	uint cardSize;
	uchar ucResult;
	ulong cardSerial;
	uchar *pucResp = (uchar *) pvResp;

	if ((puiRespLen == EM_NULL) || (pvResp == EM_NULL))
	{
		return EM_ERRPARAM;
	}
	if ((ucMode != EM_mifs_HALT) && (ucMode != EM_mifs_IDLE))
	{
		return EM_ERRPARAM;
	}

	*puiRespLen = 0;

	EI_paypass_vDelay(500);
	// 请求卡片类型识别号
	ucResult = EA_ucMIFRequest(hDevHandle, ucMode, &tagType);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	pucResp[0] = (uchar) tagType;
	pucResp[1] = (uchar) (tagType >> 8);

	// 防冲突, 取得卡号
	EI_paypass_vDelay(60);
	ucResult = EA_ucMIFAntiColl(hDevHandle, &cardSerial);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}
	EI_paypass_vDelay(60);
	pucResp[2] = (uchar) cardSerial;
	pucResp[3] = (uchar) (cardSerial >> 8);
	pucResp[4] = (uchar) (cardSerial >> 16);
	pucResp[5] = (uchar) (cardSerial >> 24);
	// ?????
	//pucResp[6] = pucResp[4]^pucResp[5]^pucResp[6]^pucResp[7];
	pucResp[6] = pucResp[2] ^ pucResp[3] ^ pucResp[4] ^ pucResp[5];
	// 选卡, 返回MifarePro卡标识符或者卡大小
	ucResult = EA_ucMIFSelect(hDevHandle, cardSerial, &cardSize);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}
	pucResp[7] = (uchar) cardSize;

	// Added by Pan Pingbin 2005.09.27
	// 确保Pro卡有足够的时间响应RATS命令
	EI_paypass_vDelay(100);

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	if ((pucResp[7] & 0x20) != 0)
	{
		EG_mifs_tWorkInfo.aucBuffer[0] = 0xE0;
		//yehf 2010-09-03 Level-1要求修改
		EG_mifs_tWorkInfo.aucBuffer[1] = 0x80;	//  0x70;    // 可接收最大帧为128字节 CID=0
		// 给MifarePro卡上电，获取ATR值
		ucResult = EI_mifs_ucProcess(EM_mifs_TYPEA, 2, puiRespLen);
		if (ucResult != EM_SUCCESS)
		{
			goto resetProEnd;
		}
		// 增加长度越界判断，2006.10.09
		if (*puiRespLen > 255)
		{
			ucResult = EM_mifs_CODEERR;
			*puiRespLen = 8;
			goto resetProEnd;
		}

		// 分析上电复位数据
		ucResult = EI_mifs_ucHandleATQA(*puiRespLen);
		if (ucResult != EM_SUCCESS)
		{
			// 上电复位数据错误
			*puiRespLen = 8;
			goto resetProEnd;
		}
		// 保存读出数据
		memcpy(pucResp + 8, EG_mifs_tWorkInfo.aucBuffer, *puiRespLen);
		*puiRespLen += 8;
	}
	else
	{
		// 不是MifarePro卡
		ucResult = EM_mifs_CODEERR;
		*puiRespLen = 8;
	}

  resetProEnd:
	EI_mifs_vEndProtect();
	return ucResult;

#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif

}

/**********************************************************************
* 函数名称：
*     EA_ucMIFExchangePro(DevHandle hDevHandle, uint uiSendLen, void *pvSendData, uint*puiRecLen, void *pvRecData)
* 功能描述：
*     按ISO14443-4标准完成读写器与mifpro卡的数据交换操作。
* 输入参数：
*    hDevHandle     设备句柄
*    uiSendLen: 要发送数据长度
*    pvSendData:   要发送数据
* 输出参数：
*     puiRecLen      :  实际返回数据长度
*     pvRecData   : 返回的数据,不包括卡类型
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFExchangePro(DevHandle hDevHandle, uint uiSendLen, void *pvSendData, uint * puiRecLen,
	void *pvRecData)
{
#ifdef EM_RC531_Module

	return EI_mifs_ucExchange(hDevHandle, EM_mifs_TYPEA, 0, uiSendLen, pvSendData, puiRecLen,
		pvRecData);
#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif
}

/**********************************************************************
* 函数名称：
*    EA_ucMIFCommandPro(DevHandle hDevHandle, uint uiSendLen, void *pvSendData, uint*puiRecLen, void *pvRecData)
* 功能描述：
*     完成读写器与mifpro卡的数据交换操作。
* 输入参数：
*    hDevHandle     设备句柄
*    uiSendLen: 要发送数据长度
*    pvSendData:   要发送数据
* 输出参数：
*     puiRecLen      :  实际返回数据长度
*     pvRecData   : 返回的数据,不包括卡类型
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFCommandPro(DevHandle hDevHandle, uint uiSendLen, void *pvSendData, uint * puiRecLen,
	void *pvRecData)
{
#ifdef EM_RC531_Module

	return EI_mifs_ucCommand(hDevHandle, EM_mifs_TYPEA, 0, uiSendLen, pvSendData, puiRecLen,
		pvRecData);
#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif
}

/**********************************************************************
* 函数名称：
*     EA_ucMIFDeselectPro(DevHandle hDevHandle)
* 功能描述：
*     将mifpro卡设置为halt状态
* 输入参数：
*    hDevHandle     设备句柄
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFDeselectPro(DevHandle hDevHandle)
{

#ifdef EM_RC531_Module

	uchar ucResult;

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	// ISO14443-4规范要求: 如果第一次Deselect命令发生超时错误,则应重发一次Deselect

	ucResult = EI_mifs_ucDeselect(EM_mifs_TYPEA, 0);
	if (ucResult != EM_SUCCESS)
	{
		ucResult = EI_mifs_ucDeselect(EM_mifs_TYPEA, 0);;
	}

	EI_mifs_vEndProtect();
	return ucResult;

#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif

}

/**********************************************************************

                          TYPE-B卡函数定义

***********************************************************************/
/**********************************************************************
* 函数名称：
*     EA_ucMIFRequestB(DevHandle hDevHandle, uchar ucState, uchar ucType,
*                             uchar ucCount, uint *puiRespLen, void *pvResp)
* 功能描述：
*     激活在天线范围内的TYPE B卡
* 输入参数：
*    hDevHandle     设备句柄
*     ucState: 选择激活卡片时的卡片状态,EM_mifs_IDLE-IDLE状态,EM_mifs_HALT-HALT状态
*     ucType: 应用类型标识,如果为EM_mifs_TYPEB_REQALL则所有区域内的卡片都应响应,其它值的设置需根据规范
*     ucCount: 时隙总数参数,取值范围0到4,当有多张卡时可采用此方法防冲突,每张卡在第一个时隙响应的概率为1/2^n
*             0-1,1-2,2-4,3-8,4-16
* 输出参数：
*     puiRespLen: 接收到数据长度
*     pvResp: 接收到的数据, 格式如下:
*       字节1－0x50,该值固定
*       字节2～5－伪唯一的PICC标示符，可以是PICC序列号的缩短形式；芯片序列号；PICC产生的单次随机数
*                 该标示符在HLTB和ATRRIB命令中还要用到
*       字节6～9－应用控制字节，组成如下：
*                 字节6－ 当PICC为单应用时可参照相关应用表格确定卡片的应用范围，
*                         当为一卡多用时需要通过CRC_B获得支持的应用范围
*                 字节7~8－ 用来计算一卡多用时的PICC支持应用范围（7816-5）
*                 字节9－ 支持的应用数，0-15
*       字节10～12－协议控制字节，组成如下：
*                 字节10－ 定义PCD与PICC之间的ETU和数据传输速率，该值在应用中需要改变
*                 字节11－ 低半字节确定PICC是否支持ISO14443-4,1-支持,0-不支持；
*                          高半字节确定PICC可接收的最大帧长度
*                 字节12－ 高半字节确定接收帧时的最大等待时间（程序中已做处理）；
*                          低半字节的高两个字节确定应用协议的所有权
*                                  00-应用协议是私有的，01-应用协议按照应用控制字节定义；
*                          低半字节的低两个字节确定如下信息：1X-卡片支持NAD，X1-卡片支持CID
*     卡片的标示符需要应用程序保存,应用程序需要根据实际支持的卡片对卡片的返回进行判断以选择相应的卡片
*     另外卡片可接收的最大数据长度由用户保存，卡片是否支持ISP14443-4协议也要保存。因为实际应用中可能同时对多张卡进行操作
*     先暂时固定为不用NAD
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
***********************************************************************/
uchar EA_ucMIFRequestB(DevHandle hDevHandle, uchar ucState, uchar ucType,
	uchar ucCount, uint * puiRespLen, void *pvResp)
{

#ifdef EM_RC531_Module

	uchar ucResult = EM_SUCCESS;
	uchar ucTempData;

	if ((ucState != EM_mifs_IDLE) && (ucState != EM_mifs_HALT))
	{
		return EM_ERRPARAM;
	}

	if ((ucCount > EM_mifs_TIMESLOT_MAXINT) || (puiRespLen == EM_NULL) || (pvResp == EM_NULL))
	{
		return EM_ERRPARAM;
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 2);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EI_mifs_vFlushFIFO();				//empty FIFO

	// 发送和接收帧最后2个字节为CRC16校验，每个字节不带校验位
	ucTempData = 0x2C;
	EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

	EI_mifs_vClrBitMask(EM_mifs_REG_CONTROL, 0x08);	// 禁止crypto 1加密单元
	EI_mifs_vSetBitMask(EM_mifs_REG_TXCONTROL, 0x03);	// 打开TX1和TX2天线

	EI_mifs_vSetTimer(7);

	// 准备发送数据: 命令码
	EG_mifs_tWorkInfo.ulSendBytes = 3;	// how many bytes to send
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_REQB;	//激活命令
	EG_mifs_tWorkInfo.aucBuffer[1] = ucType;
	EG_mifs_tWorkInfo.aucBuffer[2] = ucCount;
	// 确定当前采用的激活方式设置位置在EG_mifs_tWorkInfo.aucBuffer[2]的第3位
	if (ucState == EM_mifs_HALT)
	{
		EG_mifs_tWorkInfo.aucBuffer[2] |= 0x08;
	}

	ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_TRANSCEIVE);
	// 如果第一次RequestB无响应,则在至少延迟5ms后,发起第二次RequestB命令
	// Added by panpingbin 2005.10.21
	if (ucResult == EM_mifs_NOTAGERR)
	{
		ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_TRANSCEIVE);
	}

	if ((ucResult != EM_SUCCESS) && (ucResult != EM_mifs_NOTAGERR) &&
		((EG_mifs_tWorkInfo.aucBuffer[0] == 0x50) || (EG_mifs_tWorkInfo.aucBuffer[0] == 0x05)))
	{
		// 卡片冲突
		ucResult = EM_mifs_COLLERR;
	}

	if (ucResult == EM_SUCCESS)
	{
		// 增加长度越界判断
		if (EG_mifs_tWorkInfo.ulBytesReceived != 12)
		{
			ucResult = EM_mifs_CODEERR;
			*puiRespLen = 0;
			goto ReqBEnd;
		}
		ucResult = EI_mifs_ucHandleATQB(EG_mifs_tWorkInfo.ulBytesReceived);
		if (ucResult == EM_SUCCESS)
		{
			*puiRespLen = EG_mifs_tWorkInfo.ulBytesReceived;
			memcpy(pvResp, EG_mifs_tWorkInfo.aucBuffer, *puiRespLen);
		}
		else
		{
			*puiRespLen = 0;
		}
	}
	else
	{
		*puiRespLen = 0;
	}

  ReqBEnd:
	// 延时500us
	EI_paypass_vDelay(60);
	EI_mifs_vEndProtect();
	return ucResult;

#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif

}

/**********************************************************************
* 函数名称：
*    uchar EA_ucMIFSlotMarkerB(DevHandle hDevHandle, uchar ucNumber, uint *puiRespLen,
*                                void *pvResp);
* 功能描述：
*     设置时隙标号,当卡片不在第一个时隙响应时,卡发送该命令使对应时隙的卡片应答
* 被以下函数调用
*     API
* 调用以下函数：
*     下层函数
* 输入参数：
*    hDevHandle     设备句柄
*     ucNumber: 时隙编号,1-15,对应时隙2～16
* 输出参数：
*     puiRespLen: 接收到数据长度
*     pvResp: 接收到的数据, 格式如下:
*       字节1－0x50,该值固定
*       字节2～5－伪唯一的PICC标示符，可以是PICC序列号的缩短形式；芯片序列号；PICC产生的单次随机数
*                 该标示符在HLTB和ATRRIB命令中还要用到
*       字节6～9－应用控制字节，组成如下：
*                 字节6－ 当PICC为单应用时可参照相关应用表格确定卡片的应用范围，
*                         当为一卡多用时需要通过CRC_B获得支持的应用范围
*                 字节7~8－ 用来计算一卡多用时的PICC支持应用范围（7816-5）
*                 字节9－ 支持的应用数，0-15
*       字节10～12－协议控制字节，组成如下：
*                 字节10－ 定义PCD与PICC之间的ETU和数据传输速率，该值在应用中需要改变
*                 字节11－ 低半字节确定PICC是否支持ISO14443-4,1-支持,0-不支持；
*                          高半字节确定PICC可接收的最大帧长度
*                 字节12－ 高半字节确定接收帧时的最大等待时间（程序中已做处理）；
*                          低半字节的高两个字节确定应用协议的所有权
*                                  00-应用协议是私有的，01-应用协议按照应用控制字节定义；
*                          低半字节的低两个字节确定如下信息：1X-卡片支持NAD，X1-卡片支持CID
*     卡片的标示符需要应用程序保存,应用程序需要根据实际支持的卡片对卡片的返回进行判断以选择相应的卡片
*     另外卡片可接收的最大数据长度由用户保存，卡片是否支持ISP14443-4协议也要保存。因为实际应用中可能同时对多张卡进行操作
*     先暂时固定为不用NAD
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
*                 2006-10-09          01-05-02    增加长度越界判断
***********************************************************************/
uchar EA_ucMIFSlotMarkerB(DevHandle hDevHandle, uchar ucNumber, uint * puiRespLen, void *pvResp)
{

#ifdef EM_RC531_Module

	uchar ucResult = EM_SUCCESS;
	uchar ucTempData;

	if ((puiRespLen == NULL) || (pvResp == NULL))
	{
		return EM_ERRPARAM;
	}

	if ((ucNumber < 1) || (ucNumber >= EM_mifs_TIMESLOT_MAXNUM))
	{
		return EM_ERRPARAM;
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 2);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EI_mifs_vFlushFIFO();				//empty FIFO

	// RxCRC and TxCRC enable, parity
	ucTempData = 0x2C;
	EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

	EI_mifs_vSetTimer(5);

	// 准备发送数据: 命令码
	EG_mifs_tWorkInfo.ulSendBytes = 1;	// how many bytes to send
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[0] = (ucNumber << 4) + EM_mifs_PICC_SLOTMARKER;	//激活命令

	ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_TRANSCEIVE);

	if ((ucResult != EM_SUCCESS) && (ucResult != EM_mifs_NOTAGERR) &&
		((EG_mifs_tWorkInfo.aucBuffer[0] == 0x50) || (EG_mifs_tWorkInfo.aucBuffer[0] == 0x05)))
	{
		// 卡片冲突
		ucResult = EM_mifs_COLLERR;
	}

	if (ucResult == EM_SUCCESS)
	{
		// 增加长度越界判断，
		if (EG_mifs_tWorkInfo.ulBytesReceived != 12)
		{
			ucResult = EM_mifs_CODEERR;
			*puiRespLen = 0;
			goto SlotBEnd;
		}
		ucResult = EI_mifs_ucHandleATQB(EG_mifs_tWorkInfo.ulBytesReceived);
		if (ucResult == EM_SUCCESS)
		{
			*puiRespLen = EG_mifs_tWorkInfo.ulBytesReceived;
			memcpy(pvResp, EG_mifs_tWorkInfo.aucBuffer, *puiRespLen);
		}
		else
		{
			*puiRespLen = 0;
		}

	}
	else
	{
		*puiRespLen = 0;
	}

  SlotBEnd:
	EI_mifs_vEndProtect();
	return ucResult;

#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif

}

/**********************************************************************
* 函数名称：
*     EA_ucMIFAttriB(DevHandle hDevHandle, uint uiSendLen, void* pvSendData, uint* puiRecLen,
*                       void *pvRecData)
* 功能描述：
*     设置通讯参数命令并建立与卡片的通道
* 输入参数：
*    hDevHandle     设备句柄
*     ulPUPI: 伪唯一的PICC标示符，可以是PICC序列号的缩短形式；芯片序列号；PICC产生的单次随机数
*     ucProtFlag: 确定卡片是否支持ISO14443-4协议,0-不支持,1-支持
*     ucCID: 指定卡号0-14
*     uiSendLen: 要发送数据长度
*     pvSendData: 存放要发送数据
* 输出参数：
*     puiRecLen: 接收到数据长度
*     pvRecData: 接收到的数据,其第一个字节定义如下
*          b8~b5: MBLI,最大缓冲区长度索引。PICC以此通知PCD其接收链接帧的内部缓冲区大小。
*          b4~b1: CID，必须与参数ucCID一致。
*
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                2004-08-14          01-01-01    创建
*                2005-10-21          01-02-01    增加一次重发机制
***********************************************************************/
uchar EA_ucMIFAttriB(DevHandle hDevHandle, uint uiSendLen, void *pvSendData, uint * puiRecLen,
	void *pvRecData)
{

#ifdef EM_RC531_Module

	uchar ucResult = EM_SUCCESS;
	uchar ucTempData;
	uchar *pucSendData = (uchar *) pvSendData;

	if ((pvSendData == EM_NULL) || (puiRecLen == EM_NULL) || (pvRecData == EM_NULL)
		|| (uiSendLen < 8) || (uiSendLen >= EM_mifs_BUFFER_LEN))
	{
		return EM_ERRPARAM;
	}

	if ((pucSendData[6] != EM_mifs_SUPPORTB) && (pucSendData[6] != EM_mifs_NOTSUPPORTB))
	{
		return EM_ERRPARAM;
	}

	if (pucSendData[7] > EM_mifs_MAXPID)
	{
		return EM_ERRPARAM;
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 2);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EI_mifs_vFlushFIFO();				//empty FIFO

	// RxCRC and TxCRC enable, parity
	ucTempData = 0x2C;
	EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

	// 准备发送数据
	EG_mifs_tWorkInfo.ulSendBytes = uiSendLen + 1;	// 要发送数据还得加上0x1D+PUPI+P1+P2+P3+P4
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_ATTRIB;
	memcpy(&EG_mifs_tWorkInfo.aucBuffer[1], pvSendData, uiSendLen);
	ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_TRANSCEIVE);
	// 如果第一次AttriB命令无响应,则发送第二次Attrib命令.
	// Added by panpingbin 2005.10.21
	if (ucResult == EM_mifs_NOTAGERR)
	{
		ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_TRANSCEIVE);
	}

	if (ucResult == EM_SUCCESS)
	{
		if (EG_mifs_tWorkInfo.ulBytesReceived == 0)
		{
			*puiRecLen = 0;
			ucResult = EM_mifs_NOTAGERR;
		}
		else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x0F) == pucSendData[7])
		{
			*puiRecLen = EG_mifs_tWorkInfo.ulBytesReceived;
			memcpy(pvRecData, EG_mifs_tWorkInfo.aucBuffer, EG_mifs_tWorkInfo.ulBytesReceived);
			EG_mifs_tWorkInfo.ucCurPCB = 1;
		}
		else
		{
			*puiRecLen = 0;
			ucResult = EM_mifs_CODEERR;
		}
	}
	else
	{
		*puiRecLen = 0;
	}

	// 延时500us
	EI_paypass_vDelay(60);
	EI_mifs_vEndProtect();

	return ucResult;

#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif

}

/**********************************************************************
* 函数名称：
*     EA_ucMIFExchangeB(DevHandle hDevHandle,uchar ucCID, uint uiSendLen, void *pvSendData, uint*puiRecLen, void *pvRecData)
* 功能描述：
*     按ISO14443-4要求与TYPEB卡交换数据
* 输入参数：
*    hDevHandle     设备句柄
*     ucCID: 分配给卡的临时卡号
*     uiSendLen: 要发送数据长度
*     pvSendData: 存放要发送数据
* 输出参数：
*     puiRecLen: 接收到数据长度
*     pvRecData: 接收到的数据
*
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
***********************************************************************/
uchar EA_ucMIFExchangeB(DevHandle hDevHandle, uchar ucCID, uint uiSendLen, void *pvSendData,
	uint * puiRecLen, void *pvRecData)
{
#ifdef EM_RC531_Module

	return EI_mifs_ucExchange(hDevHandle, EM_mifs_TYPEB, ucCID, uiSendLen, pvSendData, puiRecLen,
		pvRecData);
#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif

}

/**********************************************************************
* 函数名称：
*    EA_ucMIFCommandB(DevHandle hDevHandle, uint uiSendLen, void *pvSendData, uint*puiRecLen, void *pvRecData)
* 功能描述：
*     完成读写器与TYPEB卡的数据交换操作。
* 输入参数：
*    hDevHandle     设备句柄
*    ucCID: 分配给卡的临时卡号
*    uiSendLen: 要发送数据长度
*    pvSendData:
* 输出参数：要发送数据
*     puiRecLen      :  实际返回数据长度
*     pvRecData   : 返回的数据,不包括卡类型
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-12-29          01-01-01    创建
***********************************************************************/
uchar EA_ucMIFCommandB(DevHandle hDevHandle, uchar ucCID, uint uiSendLen, void *pvSendData,
	uint * puiRecLen, void *pvRecData)
{
#ifdef EM_RC531_Module

	return EI_mifs_ucCommand(hDevHandle, EM_mifs_TYPEB, ucCID, uiSendLen, pvSendData, puiRecLen,
		pvRecData);

#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif
}

/**********************************************************************
* 函数名称：
*    EA_ucMIFDeselectB(DevHandle hDevHandle, uchar ucCID)
* 功能描述：
*     使已建立通道的卡片处于HALT状态
* 输入参数：
*    hDevHandle     设备句柄
*     ucCID: 指定卡号0-14
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
*                 2005-10-21          01-02-01    增加一次重发机制
***********************************************************************/
uchar EA_ucMIFDeselectB(DevHandle hDevHandle, uchar ucCID)
{

#ifdef EM_RC531_Module

	uchar ucResult;

	if (ucCID > EM_mifs_MAXPID)
	{
		return EM_ERRPARAM;
	}

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 2);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	// ISO14443-4规范要求: 如果第一次Deselect命令发生超时错误,则应重发一次Deselect

	ucResult = EI_mifs_ucDeselect(EM_mifs_TYPEB, ucCID);
	if (ucResult != EM_SUCCESS)
	{
		ucResult = EI_mifs_ucDeselect(EM_mifs_TYPEB, ucCID);
	}

	EI_mifs_vEndProtect();
	return ucResult;

#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif

}

/**********************************************************************
* 函数名称：
*     EA_ucMIFHaltB(DevHandle hDevHandle, ulong ulPUPI)
* 功能描述：
*     使尚未建立通道的卡片进入停止状态
* 输入参数：
*    hDevHandle     设备句柄
*    ulPUPI: 伪唯一的PICC标示符，可以是PICC序列号的缩短形式；芯片序列号；PICC产生的单次随机数
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
***********************************************************************/
uchar EA_ucMIFHaltB(DevHandle hDevHandle, ulong ulPUPI)
{

#ifdef EM_RC531_Module

	uchar ucResult = EM_SUCCESS;
	uchar ucTempData;

	ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 2);
	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	EI_mifs_vFlushFIFO();				//empty FIFO

	// RxCRC and TxCRC enable, parity
	ucTempData = 0x2C;
	EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

	EI_mifs_vSetTimer(4);

	// 准备发送数据: 命令码＋PUPI
	EG_mifs_tWorkInfo.ulSendBytes = 5;	// how many bytes to send
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_HALT;
	EG_mifs_tWorkInfo.aucBuffer[1] = ((uchar *) & ulPUPI)[0];
	EG_mifs_tWorkInfo.aucBuffer[2] = ((uchar *) & ulPUPI)[1];
	EG_mifs_tWorkInfo.aucBuffer[3] = ((uchar *) & ulPUPI)[2];
	EG_mifs_tWorkInfo.aucBuffer[4] = ((uchar *) & ulPUPI)[3];

	ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_TRANSCEIVE);

	if (ucResult == EM_SUCCESS)
	{
		if (EG_mifs_tWorkInfo.ulBytesReceived == 0)
		{
			ucResult = EM_mifs_NOTAGERR;
		}
		else if ((EG_mifs_tWorkInfo.ulBytesReceived != 1)
			|| (EG_mifs_tWorkInfo.aucBuffer[0] != 0x0))
		{
			ucResult = EM_mifs_CODEERR;
		}
	}

	EI_mifs_vEndProtect();
	return ucResult;

#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif

}

/*************************************************************************

                  新增API函数定义

**************************************************************************/

/*******************************************************************************
* 函数名称:
*        EA_ucMIFPollMifCard(DevHandle hDevHandle, uchar ucAntiFlag, uchar* pucMifCardType)
* 功能描述:
*        轮询感应区，判断是否有MifCard进行感应区
* 被以下函数调用:
*        无
* 调用以下函数：
*        EI_paypass_ucPOLL
* 输入参数：
*        hDevHandle    设备句柄
*        ucAntiFlag    是否允许从多张TypeA卡中获得一张卡片的序列号，取值如下：
*                       0 - 不允许感应区有多张卡
*                       1 - 允许感应区有多张卡
* 输出参数:
*       pucMifCardType   进入感应区的卡片类型
*                        EM_mifs_S50          S50结构的小容量Mifare One卡
*                        EM_mifs_S70          S70结构的大容量Mifare One卡
*                        EM_mifs_PROCARD      Pro卡
*                        EM_mifs_PRO_S50      即可以做为S50使用，也可以做为Pro卡使用
*                        EM_mifs_PRO_S70      即可以做为S70使用，也可以做为Pro卡使用
*                        EM_mifs_TYPEBCARD    TypeB卡
* 返 回 值：
*       EM_ERRHANDLE     句柄错误
*       EM_ERRPARAM      参数错误
*       EM_mifs_SUCCESS  询卡成功
*       EM_mifs_NOCARD   在规定时间内没有判断到任何射频卡
*       EM_mifs_MULTIERR 有多张卡在感应区
*       EM_mifs_PROTERR  协议错误
*       EM_mifs_TRANSERR 通信错误
* 历史纪录：
*        修改人           修改日期             版本号       修改内容
*                        2006-10-09           01-05-02        创建
***************************************************************/
uchar EA_ucMIFPollMifCard(DevHandle hDevHandle, uchar ucAntiFlag, uchar * pucMifCardType)
{
	uchar result;

	if (pucMifCardType == EM_NULL)
	{
		return EM_ERRPARAM;
	}

	if ((ucAntiFlag != 0) && (ucAntiFlag != 1))
	{
		return EM_ERRPARAM;
	}

	result = EI_mifs_ucBeginProtect(&hDevHandle, 3);
	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}

	// 先初始化相关变量
	EI_paypass_vInit();
	EG_mifs_tWorkInfo.ucAnticollFlag = ucAntiFlag;
	EG_mifs_tWorkInfo.ucMifActivateFlag = RFID_ACTIVE_NONE;
//    if(EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV) //emv 模式不能关闭载波
        EI_paypass_vOptField(ON);
//    else
//        EI_paypass_vResetPICC();
	//s_DelayMs(10);
	// 先判断感应区内是否有卡
	result = EI_paypass_ucPOLL(pucMifCardType);
	if (result == EM_mifs_TIMEOUT)
	{
		result = EM_mifs_NOCARD;
	}
	if(!result)
	{
		if(EG_mifs_tWorkInfo.ucMifActivateFlag == RFID_ACTIVE_NONE)
		{
			//未寻卡置为寻卡成功，如果已经激活则不设置
			EG_mifs_tWorkInfo.ucMifActivateFlag = RFID_ACTIVE_POLLOK;
		}
	}
	EI_mifs_vEndProtect();
	return result;
}

/******************************************************************************
* 函数名称：
*       EA_ucMIFActivate(DevHandle hDevHandle, uchar ucMifType, uint* puiOutLen, void* pvOut)
* 功能描述：
*       激活感应区内的MifCard
* 被以下函数调用：
*       API
* 调用以下函数：
*       EI_paypass_ucActPro
*       EI_paypass_ucActTypeB
*       EI_paypass_vResetPICC
*       EI_paypass_vDelay
* 输入参数：
*       hDevHandle  射频卡设备句柄
*       ucMifType   感应区内的卡片类型,取值如下：
*                     EM_mifs_S50          S50结构的小容量Mifare One卡
*                     EM_mifs_S70          S70结构的大容量Mifare One卡
*                     EM_mifs_PROCARD      Pro卡
*                     EM_mifs_TYPEBCARD    TypeB卡
* 输出参数：
*       puiOutLen  输出的数据长度
*       pvOut      输出的数据地址，建议应用程序开辟大于256字节的空间，
*       ATQA(LByte+HByte)+ sn(卡号低字节在前4B)+sncheck(卡号校验1B) + SAK(1B) + ATR
* 返 回 值：
*       EM_ERRHANDLE     句柄错误
*       EM_ERRPARAM      参数错误
*       EM_mifs_SUCCESS  激活成功
*       EM_mifs_TIMEOUT  激活超时
*       EM_mifs_TRANSERR 激活过程中出现传输错误
*       EM_mifs_PROTERR  卡片返回数据不符合规范
* 历史纪录：
*        修改人           修改日期             版本号       修改内容
*                        2006-10-09           01-05-02        创建
***************************************************************/
uchar EA_ucMIFActivate(DevHandle hDevHandle, uchar ucMifType, uint * puiOutLen, void *pvOut)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucATSLen = 0x00;
	uchar ucATSData[255];
	uchar *pucOut = (uchar *) pvOut;
    uint i;
	memset(ucATSData, 0x00, sizeof(ucATSData));

	if ((puiOutLen == EM_NULL) || (pvOut == EM_NULL))
	{
		return EM_ERRPARAM;
	}

	*puiOutLen = 0;

	result = EI_mifs_ucBeginProtect(&hDevHandle, 3);
	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}

	EI_mifs_vFlushFIFO();				// empty FIFO
	if(EG_mifs_tWorkInfo.ucMifActivateFlag < RFID_ACTIVE_POLLOK)
	{
		//未poll直接返回超时
		return EM_mifs_TIMEOUT;
	}
	if(EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV)
	{
		//poll只执行WUPA和WUPB
	//ActivateAB:
		if(EG_mifs_tWorkInfo.ucCurType == EM_mifs_TYPEB)
		{
			// 激活该TypeB卡
	ActivateB:
			result = EI_paypass_ucActTypeB(&ucATSLen, ucATSData);
		}
		else
		{
			result = EI_paypass_ucActPro(&ucATSLen, ucATSData);
		}
	}
	else
	{
		//ISO通常模式
		if(EG_mifs_tWorkInfo.ucCurType == EM_mifs_TYPEB)
		{
			//寻到B卡，未激活
			goto ActivateB;
		}
		else
		{
			//寻到A卡已经激活
			//上海电信部分射频卡调试发现不能2次激活卡
			result = EM_mifs_SUCCESS;
		}
		/*
		if(EG_mifs_tWorkInfo.ucAnticollFlag == 0)
		{
			//可以寻1张卡,寻到A卡或B卡会切换载波，因此需要重新激活
			EI_paypass_vResetPICC();
			goto ActivateAB;
		}
		else
		{
			//寻多张卡
			if(EG_mifs_tWorkInfo.ucCurType == EM_mifs_TYPEB)
			{
				//寻到B卡，未激活
				goto ActivateB;
			}
			else
			{
				//寻到A卡已经激活
				//上海电信部分射频卡调试发现不能2次激活卡
				result = EM_mifs_SUCCESS;
			}
		}
		*/
	}
	if(result == EM_mifs_SUCCESS)
	{
		i = 0;
		if(EG_mifs_tWorkInfo.ucCurType == EM_mifs_TYPEA)
		{
			// 返回激活过程中获得的全部数据
			memcpy(&pucOut[i],EG_mifs_tWorkInfo.ucATQA,2);
			i += 2;
			if (EG_mifs_tWorkInfo.ucUIDLen >= 4)
			{
				memcpy(&pucOut[i], EG_mifs_tWorkInfo.ucUIDCL1, 5);
				i += 5;
				memcpy(&pucOut[i], &EG_mifs_tWorkInfo.ucSAK1, 1);
				i += 1;
			}
			else if (EG_mifs_tWorkInfo.ucUIDLen >= 7)
			{
				memcpy(&pucOut[i], EG_mifs_tWorkInfo.ucUIDCL2, 5);
				i += 5;
				memcpy(&pucOut[i], &EG_mifs_tWorkInfo.ucSAK2, 1);
				i += 1;
			}
			else
			{
				memcpy(&pucOut[i], EG_mifs_tWorkInfo.ucUIDCL3, 5);
				i += 5;
				memcpy(&pucOut[i], &EG_mifs_tWorkInfo.ucSAK3, 1);
				i += 1;
			}
            memcpy(EG_mifs_tWorkInfo.ATS, ucATSData, ucATSLen);
			memcpy(&pucOut[i], EG_mifs_tWorkInfo.ATS, EG_mifs_tWorkInfo.ATS[0]);
			i += EG_mifs_tWorkInfo.ATS[0];
		}
		else
		{
			// ATS数据
			memcpy(&pucOut[i], ucATSData, ucATSLen);
			i += ucATSLen;
		}
		//2012-01-04  返回参数增加1个字节表示卡类型
		memcpy(&pucOut[i], &EG_mifs_tWorkInfo.ucMifCardType, 1);
		i += 1;
		*puiOutLen = i;
	}
	EI_mifs_vEndProtect();
	return result;
}

/**************************************************************************
* 函数名称：
*        EA_ucMIFAPDU(DevHandle hDevHandle, uint uiSendLen, void* pvInData,
*                     uint* puiRecLen, void* pvOutData)
* 功能描述:
*        实现设备与Mif卡之间按照ISO14443-4协议进行通信
* 被以下函数调用：
*        API
* 调用以下函数：
*        无
* 输入参数：
*        hDevHandle   设备句柄
*        uiSendLen    终端发送的数据长度
*        pvInData     终端要发送的数据
* 输出参数：
*        puiRecLen    卡回送的数据长度
*        pvOutData    卡回送的数据
* 返 回 值：
*        EM_ERRHANDLE     句柄错误
*        EM_ERRPARAM      参数错误
*        EM_mifs_SUCCESS  数据交换成功（卡片返回状态字节为0x9000）
*        EM_mifs_NOACT    卡片未激活
*        EM_mifs_SWDIFF   卡片返回状态字节为（不等于 0x9000）
*        EM_mifs_TIMEOUT  卡片无返回
*        EM_mifs_PROTERR  卡片返回的数据违反协议
*        EM_mifs_TRANSERR 通信过程中出现错误
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
***************************************************************/
uchar EA_ucMIFAPDU(DevHandle hDevHandle, uint uiSendLen, void *pvInData,
	uint * puiRecLen, void *pvOutData)
{
	uchar ucRet = EM_mifs_SUCCESS;
	uchar pucSendData[300];
	uchar pucRecData[300];
	uint uiSendLenTemp = 0;
	uint uiRecLenTemp = 0;

	if ((pvInData == EM_NULL) || (puiRecLen == EM_NULL) || (pvOutData == EM_NULL))
	{
		return EM_ERRPARAM;
	}
	memset(pucSendData, 0x00, sizeof(pucSendData));
	memset(pucRecData, 0x00, sizeof(pucRecData));
	// 初始化变量
	*puiRecLen = 0;
	memcpy(pucSendData, (uchar *) pvInData, uiSendLen);
	uiSendLenTemp = uiSendLen;

	if (EG_mifs_tWorkInfo.ucMifActivateFlag < RFID_ACTIVE_OK)
	{
		// 还没有进行任何激活卡片的操作
		return EM_mifs_NOACT;
	}
	if(EG_mifs_tWorkInfo.ucMifCardType < RFID_CARD_PRO)
	{
		return EM_mifs_TIMEOUT;
	}
	ucRet = EI_mifs_ucBeginProtect(&hDevHandle, 3);
	if (ucRet != EM_mifs_SUCCESS)
	{
		return ucRet;
	}
	if ((uiSendLen != 4) && (uiSendLen != 5)	// 当数据长度为4时，表示既不发送也不接收数据
		&& (uiSendLen != (uint) (pucSendData[4] + 5)))
	{
		if (uiSendLen == (uint) (pucSendData[4] + 6))
		{
			if (pucSendData[4] == 0)	// Lc = 0
			{
				pucSendData[4] = pucSendData[5];
				uiSendLenTemp = 5;
			}
		}
		else
		{
			return EM_ERRPARAM;
		}
	}

	if (uiSendLen == 4)
	{
		// CASE 1 情况下，P3 = 0
		pucSendData[4] = 0x00;
		uiSendLenTemp = 5;
	}
	ucRet = EI_paypass_ucExchange(uiSendLenTemp, pucSendData, &uiRecLenTemp, pucRecData);
	//TRACE("\r\n ucRet:%x",ucRet);
	if ((ucRet != EM_mifs_SUCCESS) && (ucRet != EM_mifs_SWDIFF))
	{
		// 数据通信失败，则发起DEACTIVATE过程。
//        if(!EG_mifs_tWorkInfo.ucCIDPollFlg)
        {
            EI_paypass_ucDeSelect();
        }
		*puiRecLen = 0x00;
	}
	else
	{
		*puiRecLen = uiRecLenTemp;
		memcpy(pvOutData, (void *)pucRecData, uiRecLenTemp);
	}

	EI_mifs_vEndProtect();

	return ucRet;
}

/**********************************************************************
* 函数名称：
*       EA_ucMIFDeActivate(DevHandle hDevHandle)
* 功能描述：
*       将卡片置为HALT，并等待卡片离开感应区
* 被以下函数调用：
*       API
* 调用以下函数：
*       无
* 输入参数：
*       hDevHanle 设备句柄
* 输出参数：
*       无
* 返 回 值：
*        EM_ERRHANDLE      句柄错误
*        EM_ERRPARAM       参数错误
*        EM_mifs_SUCCESS   操作成功
*        EM_mifs_CARDEXIST 在规定时间内没有将卡拿开感应区
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
***************************************************************/
uchar EA_ucMIFDeactivate(DevHandle hDevHandle)
{
	uchar result = EM_mifs_SUCCESS;

	result = EI_mifs_ucBeginProtect(&hDevHandle, 3);
	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}

	EI_mifs_vFlushFIFO();				// empty FIFO

	if ((EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S50)
		|| (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S70))
	{
        if (EG_mifs_tWorkInfo.ucMifActivateFlag > RFID_ACTIVE_NONE)
            EI_paypass_vHALTA();
		result = EM_mifs_SUCCESS;
	}
	else
	{
		if (EG_mifs_tWorkInfo.ucMifActivateFlag == RFID_ACTIVE_OK)
		{
			// 发起DEACTIVATE过程
//			result = EI_paypass_ucDeSelect();
			result = EM_mifs_SUCCESS; //银联直联入网不需要发送deselect命令04122016
		}
		else
		{
			result = EM_mifs_SUCCESS;
		}

	}

	EG_mifs_tWorkInfo.ucMifActivateFlag = RFID_ACTIVE_NONE;

	EI_mifs_vEndProtect();
	return EM_mifs_SUCCESS;
}

/**********************************************************************

                         Mifare卡函数定义

***********************************************************************/
/**********************************************************************
* 函数名称：
*     EI_mifs_ucHandleATQB(uint uiLength)
* 功能描述：
*     处理RequestB或者SlotMarkerB的返回数据
* 输入参数：
*    uiLength   返回数据长度
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
***********************************************************************/
uchar EI_mifs_ucHandleATQB(uint uiLength)
{
	if (uiLength == 0)
	{
		return EM_mifs_NOTAGERR;
	}
	if ((uiLength < 3) || ((EG_mifs_tWorkInfo.aucBuffer[0] != 0x50)
			&& (EG_mifs_tWorkInfo.aucBuffer[0] != 0x05)))
	{
		return EM_mifs_CODEERR;
	}
	else if (uiLength > 12)
	{
		return EM_mifs_COLLERR;
	}

	if (uiLength == 12)
	{
		if ((EG_mifs_tWorkInfo.aucBuffer[11] & 0x1) != 0)
		{
			// 支持CID
			EG_mifs_tWorkInfo.ucCIDSupport = 1;
		}
		else
		{
			// 不支持CID
			EG_mifs_tWorkInfo.ucCIDSupport = 0;
		}

		EG_mifs_tWorkInfo.ulMaxLen = (EG_mifs_tWorkInfo.aucBuffer[10] >> 4) & 0x0F;
		EG_mifs_tWorkInfo.ucFWI = (EG_mifs_tWorkInfo.aucBuffer[11] >> 4) & 0x0F;
	}
	return EM_SUCCESS;
}

/**********************************************************************
* 函数名称：
*     EI_mifs_ucHandleATQA(uint uiLength)
* 功能描述：
*     处理MifarePro卡激活时的返回数据
* 输入参数：
*    uiLength   返回数据长度
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
***********************************************************************/
uchar EI_mifs_ucHandleATQA(uint uiLength)
{
	uint uiTemp;
	uchar ucFormat;
	uint uiCurPos = 1;

	// 接收长度字节
	uiTemp = EG_mifs_tWorkInfo.aucBuffer[0];

	if (uiTemp != uiLength)
	{
		// 接收数据长度错误
		return EM_mifs_CODEERR;
	}

	ucFormat = EG_mifs_tWorkInfo.aucBuffer[1];
	EG_mifs_tWorkInfo.ulMaxLen = 128;	//ucFormat & 0x0F; // FSC长度

	if ((ucFormat & 0x10) == 0x10)
	{
		uiCurPos++;
		if (uiCurPos >= uiTemp)
		{
			return EM_mifs_CODEERR;
		}
	}

	if ((ucFormat & 0x20) == 0x20)
	{
		// 存在TB1
		uiCurPos++;
		if (uiCurPos >= uiTemp)
		{
			return EM_mifs_CODEERR;
		}
		EG_mifs_tWorkInfo.ucFWI = (EG_mifs_tWorkInfo.aucBuffer[uiCurPos] >> 4) & 0x0F;
	}

	if ((ucFormat & 0x40) == 0x40)
	{
		// 存在TC1
		uiCurPos++;
		if (uiCurPos >= uiTemp)
		{
			return EM_mifs_CODEERR;
		}
		if ((EG_mifs_tWorkInfo.aucBuffer[uiCurPos] & 0x02) == 0x02)
		{
			EG_mifs_tWorkInfo.ucCIDSupport = 1;
		}
		else
		{
			EG_mifs_tWorkInfo.ucCIDSupport = 0;
		}
	}

	EG_mifs_tWorkInfo.ucCurPCB = 1;

	return EM_SUCCESS;

}

/**********************************************************************
* 函数名称：
*     EI_mifs_ucExchange(DevHandle hDevHandle, uchar ucType, uchar ucCID,uint uiSendLen, void *pvSendData, uint*puiRecLen, void *pvRecData)
* 功能描述：
*     完成读写器与卡的符合14443-4协议的数据交换操作。
* 输入参数：
*    hDevHandle     设备句柄
*    ucType: 卡类型
*    ucCID: 临时卡号
*    uiSendLen: 要发送数据长度
*    pvSendData:   要发送数据
* 输出参数：
*     puiRecLen      :  实际返回数据长度
*     pvRecData   : 返回的数据,不包括卡类型
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                   2004-08-14          01-01-01    创建
*                   2004-10-21          01-02-01    修改CASE4实现
*                   2006-10-09          01-05-02    增加了长度越界的判断
***********************************************************************/
uchar EI_mifs_ucExchange(DevHandle hDevHandle, uchar ucType, uchar ucCID, uint uiSendLen,
	void *pvSendData, uint * puiRecLen, void *pvRecData)
{
	uchar *pucSendTemp = (uchar *) pvSendData;
	uint uiSendLeftLen = 0, uiTempLen = 0, uiSendCurrPos = 0, uiCurrRecvLen = 0;
	uchar ucResult = EM_SUCCESS;
	uchar *pucRecData = (uchar *) pvRecData;
	uchar ucReSendNo = 0x00;

	if (ucCID > EM_mifs_MAXPID)
	{
		return EM_ERRPARAM;
	}

	if ((uiSendLen < 4) || (pvSendData == EM_NULL) || (puiRecLen == EM_NULL)
		|| (pvRecData == EM_NULL))
	{
		return EM_ERRPARAM;
	}

	if (uiSendLen > 5)
	{
		if ((uiSendLen != (uint) pucSendTemp[4] + 6) && (uiSendLen != (uint) pucSendTemp[4] + 5))
		{
			return EM_ERRPARAM;
		}
	}

	if (uiSendLen == 4)					// 根据ISO7816-4标准，对于CASE1情况，应设置P3 = 0x00，同时连同CLA、INS、P1、P2一同发过去
	{
		uiSendLen = 5;
		*(pucSendTemp + 4) = 0x00;
	}

	uiSendLeftLen = uiSendLen;
	*puiRecLen = 0;

	if (ucType == EM_mifs_TYPEA)
	{
		ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	}
	else
	{
		ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 2);
	}

	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}
	/*
	D1(
		TRACE("\r\n 发送数据:%d - %d", uiSendLeftLen, EG_mifs_tWorkInfo.ulMaxLen);
		DISPBUF(pvSendData, uiSendLeftLen, 0);
	);
	*/
	//后续有I_block
	//最大发送帧长度包括两个CRC校验和字节和PCB、CID,如果有NAD则应减5
	while (uiSendLeftLen > (EG_mifs_tWorkInfo.ulMaxLen - 4))
	{
		uiTempLen = EG_mifs_tWorkInfo.ulMaxLen - 4;	// 按最大帧长度发送
		EG_mifs_tWorkInfo.ucCurPCB = (EG_mifs_tWorkInfo.ucCurPCB + 1) & 0x01;
		if (EG_mifs_tWorkInfo.ucCIDSupport == 1)
		{
			// 支持CID
			memcpy(EG_mifs_tWorkInfo.aucBuffer + 2, pucSendTemp + uiSendCurrPos, uiTempLen);
			// 更新帧号
			EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.ucCurPCB | 0x1A;	// PCB附值,当前无NAD
			EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.ucPwrLevel | ucCID;	//CID附值,EG_Pli默认值为0,在卡片的S请求中会修改
			ucResult = EI_mifs_ucProcess(ucType, uiTempLen + 2, &uiCurrRecvLen);
		}
		else
		{
			// 不支持CID
			memcpy(EG_mifs_tWorkInfo.aucBuffer + 1, pucSendTemp + uiSendCurrPos, uiTempLen);
			// 更新帧号
			EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.ucCurPCB | 0x12;	// PCB附值,当前无NAD
			ucResult = EI_mifs_ucProcess(ucType, uiTempLen + 1, &uiCurrRecvLen);
		}
		// 如何确定是卡片传输数据时接收错误或其它原因,由于不好确定是否是有效的返回值,因此对于所有的错误采取RULE 4的规定
		if ((ucResult != EM_SUCCESS) || (uiCurrRecvLen == 0))	//与卡片传输数据失败,要求重发并发送R_block(NAK)连续3次
		{
			ucResult = EI_mifs_ucSendRBlock(ucType, ucCID, &uiCurrRecvLen);
			if (ucResult != EM_SUCCESS)
			{
				goto mifsExchangeEnd;
			}
		}

		if (uiCurrRecvLen == 0)
		{
			// 数据交换失败
			ucResult = EM_mifs_CODEERR;
			goto mifsExchangeErr;
		}

		if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0x02)
		{
			// 收到I-Block，错误
			ucResult = EM_mifs_CODEERR;
			goto mifsExchangeErr;
		}
		else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0xA2)
		{
			// R-Block
			if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x08) == 0x08)
			{
				// 存在CID
				if (EG_mifs_tWorkInfo.aucBuffer[1] != (EG_mifs_tWorkInfo.ucPwrLevel | ucCID))
				{
					// 收到R_block应答块错误,有CID字节但CID错误
					ucResult = EM_mifs_CODEERR;
					goto mifsExchangeErr;
				}
			}
			if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x10) == 0x10)
			{
				// 卡不应该发送NAK
				ucResult = EM_mifs_CODEERR;
				goto mifsExchangeErr;
			}
			if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x01) == EG_mifs_tWorkInfo.ucCurPCB)
			{
				// 收到R_block(ACK)应答块块号正确, 准备发送下一个信息块
				uiSendCurrPos += uiTempLen;
				uiSendLeftLen -= uiTempLen;
			}
			else
			{
				if (ucReSendNo++ > 3)
				{
					ucResult = EM_mifs_CODEERR;
					goto mifsExchangeErr;
				}
				// 如果块号不正确, 则重发
				EG_mifs_tWorkInfo.ucCurPCB = (EG_mifs_tWorkInfo.ucCurPCB + 1) & 0x01;
			}
		}
		else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xC7) == 0xC2)
		{
			// S-BLock
			while (1)
			{
				// 发送S-Block应答
				ucResult = EI_mifs_ucSendSBlock(ucType, ucCID, &uiCurrRecvLen);
				if (ucResult != EM_SUCCESS)
				{
					goto mifsExchangeEnd;
				}

				if (uiCurrRecvLen == 0)
				{
					// 数据交换失败
					ucResult = EM_mifs_CODEERR;
					goto mifsExchangeErr;
				}

				if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0x02)
				{
					// 收到I-Block, 错误
					ucResult = EM_mifs_CODEERR;
					goto mifsExchangeErr;
				}
				else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0xA2)
				{
					// R-Block
					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x08) == 0x08)
					{
						if (EG_mifs_tWorkInfo.aucBuffer[1] !=
							(EG_mifs_tWorkInfo.ucPwrLevel | ucCID))
						{
							// 收到R_block应答块错误,有CID字节但CID错误
							ucResult = EM_mifs_CODEERR;
							goto mifsExchangeErr;
						}
					}

					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x10) == 0x10)
					{
						// 收到NAK, 错误
						ucResult = EM_mifs_CODEERR;
						goto mifsExchangeErr;
					}
					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x01) == EG_mifs_tWorkInfo.ucCurPCB)
					{
						// 收到R_block(ACK)应答块块号正确, 准备发送下一个信息块
						uiSendCurrPos += uiTempLen;
						uiSendLeftLen -= uiTempLen;
					}
					else
					{
						if (ucReSendNo++ > 3)
						{
							ucResult = EM_mifs_CODEERR;
							goto mifsExchangeErr;
						}
						// 如果块号不正确, 则重发
						EG_mifs_tWorkInfo.ucCurPCB = (EG_mifs_tWorkInfo.ucCurPCB + 1) & 0x01;
					}
					break;
				}
				else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xC7) != 0xC2)
				{
					// 接收到其他不明Block
					return EM_mifs_CODEERR;
				}
			}
		}
		else
		{
			// 收到错误的块
			ucResult = EM_mifs_CODEERR;
			goto mifsExchangeErr;
		}
	}

	ucReSendNo = 0x00;
	// 最后一帧的数据处理
	while (1)
	{
		EG_mifs_tWorkInfo.ucCurPCB = (EG_mifs_tWorkInfo.ucCurPCB + 1) & 0x01;
		if (EG_mifs_tWorkInfo.ucCIDSupport == 1)
		{
			// 支持CID
			memcpy(EG_mifs_tWorkInfo.aucBuffer + 2, pucSendTemp + uiSendCurrPos, uiSendLeftLen);
			EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.ucCurPCB | 0x0A;	// PCB附值,当前无NAD
			EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.ucPwrLevel | ucCID;	//CID附值,EG_Pli默认值为0,在卡片的S请求中会修改
			ucResult = EI_mifs_ucProcess(ucType, uiSendLeftLen + 2, &uiCurrRecvLen);
		}
		else
		{
			// 不支持CID
			memcpy(EG_mifs_tWorkInfo.aucBuffer + 1, pucSendTemp + uiSendCurrPos, uiSendLeftLen);
			EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.ucCurPCB | 0x02;	// PCB附值,当前无NAD
			ucResult = EI_mifs_ucProcess(ucType, uiSendLeftLen + 1, &uiCurrRecvLen);
		}
		// 如何确定是卡片传输数据时接收错误或其它原因,由于不好确定是否是有效的返回值,因此对于所有的错误采取RULE 4的规定
		if ((ucResult != EM_SUCCESS) || (uiCurrRecvLen == 0))	// 与卡片传输数据失败,要求重发并发送R_block(NAK)连续3次
		{
			ucResult = EI_mifs_ucSendRBlock(ucType, ucCID, &uiCurrRecvLen);
			if (ucResult != EM_SUCCESS)
			{
				goto mifsExchangeEnd;
			}
		}

		if (uiCurrRecvLen == 0)
		{
			// 数据交换失败
			ucResult = EM_mifs_CODEERR;
			goto mifsExchangeErr;
		}

		if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0xA2)
		{
			// R-Block
			if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x08) == 0x08)
			{
				if (EG_mifs_tWorkInfo.aucBuffer[1] != (EG_mifs_tWorkInfo.ucPwrLevel | ucCID))
				{
					// 收到R_block应答块错误,有CID字节但CID错误
					ucResult = EM_mifs_CODEERR;
					goto mifsExchangeErr;
				}
			}
			if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x10) == 0x10)
			{
				// 卡不应该发送NAK
				ucResult = EM_mifs_CODEERR;
				goto mifsExchangeErr;
			}
			if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x01) == EG_mifs_tWorkInfo.ucCurPCB)
			{
				// 收到R_block(ACK)应答块错误
				ucResult = EM_mifs_CODEERR;
				goto mifsExchangeErr;
			}
			else
			{
				if (ucReSendNo++ > 3)
				{
					ucResult = EM_mifs_CODEERR;
					goto mifsExchangeErr;
				}
				// 请求重发
				EG_mifs_tWorkInfo.ucCurPCB = (EG_mifs_tWorkInfo.ucCurPCB + 1) & 0x01;
			}
		}
		else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xC7) == 0xC2)
		{
			// S-BLock
			while (1)
			{
				// 发送S-Block应答
				ucResult = EI_mifs_ucSendSBlock(ucType, ucCID, &uiCurrRecvLen);
				if (ucResult != EM_SUCCESS)
				{
					goto mifsExchangeEnd;
				}

				if (uiCurrRecvLen == 0)
				{
					// 数据交换失败
					ucResult = EM_mifs_CODEERR;
					goto mifsExchangeErr;
				}

				if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0x02)
				{
					// 收到I-Block
					break;
				}
				else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0xA2)
				{
					// R-Block
					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x08) == 0x08)
					{
						if (EG_mifs_tWorkInfo.aucBuffer[1] !=
							(EG_mifs_tWorkInfo.ucPwrLevel | ucCID))
						{
							// 收到R_block应答块错误,有CID字节但CID错误
							ucResult = EM_mifs_CODEERR;
							goto mifsExchangeErr;
						}
					}
					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x10) == 0x10)
					{
						// 卡不应该发送NAK
						ucResult = EM_mifs_CODEERR;
						goto mifsExchangeErr;
					}
					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x01) == EG_mifs_tWorkInfo.ucCurPCB)
					{
						// 收到R_block(ACK)应答块错误
						ucResult = EM_mifs_CODEERR;
						goto mifsExchangeErr;
					}
					else
					{
						if (ucReSendNo++ > 3)
						{
							ucResult = EM_mifs_CODEERR;
							goto mifsExchangeErr;
						}
						// 请求重发
						EG_mifs_tWorkInfo.ucCurPCB = (EG_mifs_tWorkInfo.ucCurPCB + 1) & 0x01;
					}
					break;
				}
				else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xC7) != 0xC2)
				{
					// 接收到其他不明Block
					return EM_mifs_CODEERR;
				}
			}
			if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0x02)
			{
				// 收到I-Block
				break;
			}
		}
		else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0x02)
		{
			// 准备接收卡回送的I-Block
			break;
		}
		else
		{
			// 收到错误的块
			ucResult = EM_mifs_CODEERR;
			goto mifsExchangeErr;
		}
	}

	// 接收卡发来I-block
	while (1)
	{
		if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x01) != EG_mifs_tWorkInfo.ucCurPCB)
		{
			// 收到R_block(ACK)应答块错误
			ucResult = EM_mifs_CODEERR;
			goto mifsExchangeErr;
		}
		if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x08) == 0x08)
		{
			// 增加长度越界判断，
			if (uiCurrRecvLen < 2)
			{
				// 收到I_block块错误,但是长度错误
				ucResult = EM_mifs_CODEERR;
				goto mifsExchangeErr;
			}
			if (EG_mifs_tWorkInfo.aucBuffer[1] != (EG_mifs_tWorkInfo.ucPwrLevel | ucCID))
			{
				// 收到I_block块错误,有CID字节但CID错误
				ucResult = EM_mifs_CODEERR;
				goto mifsExchangeErr;
			}
			// 存在CID
			memcpy(&pucRecData[*puiRecLen], &EG_mifs_tWorkInfo.aucBuffer[2], uiCurrRecvLen - 2);
			*puiRecLen += uiCurrRecvLen - 2;
		}
		else
		{
			// 增加长度越界判断，
			if (uiCurrRecvLen < 1)
			{
				// 收到I_block块错误,但是长度错误
				ucResult = EM_mifs_CODEERR;
				goto mifsExchangeErr;
			}
			// 不存在CID
			memcpy(&pucRecData[*puiRecLen], &EG_mifs_tWorkInfo.aucBuffer[1], uiCurrRecvLen - 1);
			*puiRecLen += uiCurrRecvLen - 1;
		}
		if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x10) == 0)
		{
			// 最后一帧
			break;
		}

		EG_mifs_tWorkInfo.ucCurPCB = (EG_mifs_tWorkInfo.ucCurPCB + 1) & 0x01;
		ucReSendNo = 0x00;
		// ISO14443-4规范规定，在PICC链接的情况下，PCD在任何情况下只能发送R(ACK)
		while (1)
		{
			// 发送R-Block
			if (EG_mifs_tWorkInfo.ucCIDSupport == 1)
			{
				EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.ucCurPCB | 0xAA;	// PCB附值,当前无NAD
				EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.ucPwrLevel | ucCID;	//CID附值,EG_Pli默认值为0,在卡片的S请求中会修改
				ucResult = EI_mifs_ucProcess(ucType, 2, &uiCurrRecvLen);
			}
			else
			{
				EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.ucCurPCB | 0xA2;	// PCB附值,当前无NAD
				ucResult = EI_mifs_ucProcess(ucType, 1, &uiCurrRecvLen);
			}

			if ((ucResult != EM_SUCCESS) || (uiCurrRecvLen == 0))
			{
				// 接收错误，则重发R_Block
				if (ucReSendNo++ > 3)
				{
					ucResult = EM_mifs_CODEERR;
					goto mifsExchangeErr;
				}
				continue;
			}

			if (uiCurrRecvLen == 0)
			{
				// 数据交换失败
				ucResult = EM_mifs_CODEERR;
				goto mifsExchangeErr;
			}
			if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0x02)
			{
				// I_Block
				break;
			}
			else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0xA2)
			{
				// R-Block
				if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x08) == 0x08)
				{
					if (EG_mifs_tWorkInfo.aucBuffer[1] != (EG_mifs_tWorkInfo.ucPwrLevel | ucCID))
					{
						// 收到R_block应答块错误,有CID字节但CID错误
						ucResult = EM_mifs_CODEERR;
						goto mifsExchangeErr;
					}
				}
				if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x10) == 0x10)
				{
					// 卡不应该发送NAK
					ucResult = EM_mifs_CODEERR;
					goto mifsExchangeErr;
				}
				if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x01) == EG_mifs_tWorkInfo.ucCurPCB)
				{
					// 收到R_block(ACK)应答块错误
					ucResult = EM_mifs_CODEERR;
					goto mifsExchangeErr;
				}

				if (ucReSendNo++ > 3)
				{
					ucResult = EM_mifs_CODEERR;
					goto mifsExchangeErr;
				}
				continue;
			}
			else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xC7) == 0xC2)
			{
				while (1)
				{
					// 发送S-Block应答
					ucResult = EI_mifs_ucSendSBlock(ucType, ucCID, &uiCurrRecvLen);
					if (ucResult != EM_SUCCESS)
					{
						goto mifsExchangeEnd;
					}

					if (uiCurrRecvLen == 0)
					{
						// 数据交换失败
						ucResult = EM_mifs_CODEERR;
						goto mifsExchangeErr;
					}

					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0x02)
					{
						// 收到I-Block
						break;
					}
					else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0xA2)
					{
						// R-Block
						if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x08) == 0x08)
						{
							if (EG_mifs_tWorkInfo.aucBuffer[1] !=
								(EG_mifs_tWorkInfo.ucPwrLevel | ucCID))
							{
								// 收到R_block应答块错误,有CID字节但CID错误
								ucResult = EM_mifs_CODEERR;
								goto mifsExchangeErr;
							}
						}
						if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x10) == 0x10)
						{
							// 卡不应该发送NAK
							ucResult = EM_mifs_CODEERR;
							goto mifsExchangeErr;
						}
						if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x01) == EG_mifs_tWorkInfo.ucCurPCB)
						{
							// 收到R_block(ACK)应答块错误
							ucResult = EM_mifs_CODEERR;
							goto mifsExchangeErr;
						}
						else
						{
							if (ucReSendNo++ > 3)
							{
								ucResult = EM_mifs_CODEERR;
								goto mifsExchangeErr;
							}
						}
						break;
					}
					else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xC7) != 0xC2)
					{
						// 接收到其他不明Block
						return EM_mifs_CODEERR;
					}
				}
				if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xE2) == 0x02)
				{
					// 收到I-Block
					break;
				}
			}
			else
			{
				return EM_mifs_CODEERR;
			}
		}
		ucReSendNo = 0;					// 接收到I_Block, 则继续进行接收
	}

	if (*puiRecLen == 0)
	{
		ucResult = EM_mifs_NOTAGERR;
	}
	else if (*puiRecLen > 1)
	{
		if ((pucRecData[*puiRecLen - 2] != 0x90) || (pucRecData[*puiRecLen - 1] != 0x00))
		{
			ucResult = EM_mifs_SWDIFF;
		}
		goto mifsExchangeEnd;
	}
	else
	{
		ucResult = EM_mifs_CODEERR;
	}
  mifsExchangeErr:
	if ((ucResult != EM_TIMEOUT) && (ucResult != EM_mifs_NOTAGERR))
	{
		// 如果已经超时，或者卡没有响应，没必要再调用deselect。
		EI_mifs_ucDeselect(ucType, ucCID);
	}

  mifsExchangeEnd:

	EI_mifs_vEndProtect();
	return ucResult;
}

/**********************************************************************
* 函数名称：
*     EI_mifs_ucCommand(DevHandle hDevHandle, uchar ucType, uchar ucCID,uint uiSendLen, void *pvSendData, uint*puiRecLen, void *pvRecData)
* 功能描述：
*     完成读写器与卡的数据交换操作。
* 输入参数：
*    hDevHandle     设备句柄
*    ucType: 卡类型
*    ucCID: 临时卡号
*    uiSendLen: 要发送数据长度
*    pvSendData:   要发送数据
* 输出参数：
*     puiRecLen   :  实际返回数据长度
*     pvRecData   : 返回的数据,不包括卡类型
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                  2004-08-14          01-01-01    创建
*                  2004-10-21          01-02-01    修改CASE4实现
***********************************************************************/
uchar EI_mifs_ucCommand(DevHandle hDevHandle, uchar ucType, uchar ucCID, uint uiSendLen,
	void *pvSendData, uint * puiRecLen, void *pvRecData)
{
	uchar ucResult = EM_SUCCESS;

	if ((ucCID > EM_mifs_MAXPID) || (uiSendLen > EM_mifs_BUFFER_LEN) || (pvSendData == EM_NULL)
		|| (puiRecLen == EM_NULL) || (pvRecData == EM_NULL))
	{
		return EM_ERRPARAM;
	}

	*puiRecLen = 0;

	if (ucType == EM_mifs_TYPEA)
	{
		ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 1);
	}
	else
	{
		ucResult = EI_mifs_ucBeginProtect(&hDevHandle, 2);
	}

	if (ucResult != EM_SUCCESS)
	{
		return ucResult;
	}

	memcpy(EG_mifs_tWorkInfo.aucBuffer, pvSendData, uiSendLen);

	// 按照ISO7816-4标准，CASE1情况下的命令。必须置P3为0x00
	if (uiSendLen == 5)
	{
		EG_mifs_tWorkInfo.aucBuffer[uiSendLen] = 0x00;
		uiSendLen = 6;
	}

	ucResult = EI_mifs_ucProcess(ucType, uiSendLen, puiRecLen);
	if (ucResult == EM_SUCCESS)
	{
		memcpy(pvRecData, EG_mifs_tWorkInfo.aucBuffer, *puiRecLen);
	}

	EI_mifs_vEndProtect();
	return ucResult;

}

/**********************************************************************
* 函数名称：
*    EI_mifs_ucSendRBlock(uchar ucType, uchar ucCID, uint* puiRecLen)
* 功能描述：
*    发送R-BLOCK
* 输入参数：
*    ucType: 卡类型
*    ucCID: 临时卡号
* 输出参数：
*     puiRecLen      :  实际返回数据长度
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
***********************************************************************/
uchar EI_mifs_ucSendRBlock(uchar ucType, uchar ucCID, uint * puiRecLen)
{
	uint i;
	uchar ucResult;
	for (i = 0; i < 3; i++)
	{
		if (EG_mifs_tWorkInfo.ucCIDSupport == 1)
		{
			EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.ucCurPCB | 0xBA;	// NAK
			// CID附值,PLI默认值为0,在卡片的S请求中会修改
			EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.ucPwrLevel | ucCID;
			ucResult = EI_mifs_ucProcess(ucType, 2, puiRecLen);
		}
		else
		{
			EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.ucCurPCB | 0xB2;	// NAK
			ucResult = EI_mifs_ucProcess(ucType, 1, puiRecLen);
		}

		if ((ucResult == EM_SUCCESS) && (*puiRecLen != 0))
		{
			break;
		}
	}
	return ucResult;
}

/**********************************************************************
* 函数名称：
*    EI_mifs_ucSendSBlock(uchar ucType, uchar ucCID, uint* puiRecLen)
* 功能描述：
*    发送S-BLOCK
* 输入参数：
*    ucType: 卡类型
*    ucCID: 临时卡号
* 输出参数：
*     puiRecLen      :  实际返回数据长度
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
***********************************************************************/
uchar EI_mifs_ucSendSBlock(uchar ucType, uchar ucCID, uint * puiRecLen)
{
	uchar ucTemp, ucResult;

	if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x30) != 0x30)
	{
		// 不是WTX，错误
		return EM_mifs_CODEERR;
	}
	if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x08) == 0x08)
	{
		if (EG_mifs_tWorkInfo.aucBuffer[1] != (EG_mifs_tWorkInfo.ucPwrLevel | ucCID))
		{
			// 收到R_block应答块错误,有CID字节但CID错误
			return EM_mifs_CODEERR;
		}
		// 存在CID
		ucTemp = EG_mifs_tWorkInfo.aucBuffer[2];
	}
	else
	{
		ucTemp = EG_mifs_tWorkInfo.aucBuffer[1];
	}

	// 回送S_block应答块
	if (EG_mifs_tWorkInfo.ucCIDSupport == 1)
	{
		EG_mifs_tWorkInfo.aucBuffer[0] = 0xfa;
		EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.ucPwrLevel | ucCID;
		EG_mifs_tWorkInfo.aucBuffer[2] = ucTemp & 0x3F;
		ucResult = EI_mifs_ucProcess(ucType, 3, puiRecLen);
	}
	else
	{
		EG_mifs_tWorkInfo.aucBuffer[0] = 0xf2;
		EG_mifs_tWorkInfo.aucBuffer[1] = ucTemp & 0x3F;
		ucResult = EI_mifs_ucProcess(ucType, 2, puiRecLen);
	}
	if ((ucResult != EM_SUCCESS) || (*puiRecLen == 0))
	{
		return (EI_mifs_ucSendRBlock(ucType, ucCID, puiRecLen));
	}

	return ucResult;
}

/**********************************************************************
* 函数名称：
*     EI_mifs_ucAuthentication(uchar ucKeyType,uchar ucAddr)
* 功能描述：
*     对Type A卡进行认证
* 输入参数：
*    ucKeyType EM_mifs_KEYA A套密码
*              EM_mifs_KEYB B套密码
*    ucAddr：扇区密钥所在块号
* 输出参数：
*     无
* 返回值：
*     EM_mifs_NOTAGERR----无卡
*	  EM_mifs_AUTHERR ----不可能认证
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
*                 2005-05-18          01-01-01    修改
***********************************************************************/
uchar EI_mifs_ucAuthentication(uchar ucKeyType, uchar ucAddr)
{
	uchar ucTempData;
	uchar ucResult = EM_SUCCESS;
	uint time = 0;

	time = time;
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        mifareInitialize();
        mifareResetCipher();
		if (ucKeyType == EM_mifs_KEYA)
			ucTempData = EM_mifs_PICC_AUTHENT1A;
		else
			ucTempData = EM_mifs_PICC_AUTHENT1B;
		memcpy(EG_mifs_tWorkInfo.aucBuffer, &EG_mifs_tWorkInfo.ulCardID, 4);
        ucResult = mifareAuthenticate(ucTempData, ucAddr, EG_mifs_tWorkInfo.aucBuffer, 4, gtRfidProInfo.gMifareKey);
        if ( ucResult ) {
            return EM_mifs_AUTHERR;
        }
        return ucResult;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
	{

#ifdef EM_RC663_Module

		s_rfid_setRC663Int(0,RC663_IRQ_ALL);
		s_rfid_setRC663IntFlg(0,RC663_IRQ_ALL);

		// 禁止crypto1单元
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_STATUS, 0x20);
		// 清空FIFO缓冲区
		EI_mifs_vFlushFIFO();
		// 结束当前正在运行的命令
        EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_COMMAND, 0x1F);

        //load key
        EI_mifs_vWriteReg(6, PHHAL_HW_RC663_REG_FIFODATA, gtRfidProInfo.gMifareKey);
		ucTempData = PHHAL_HW_RC663_CMD_LOADKEY;
        EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_COMMAND, ucTempData);
        s_DelayUs(1000);//未知时间多少
		// 结束当前正在运行的命令
        EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_COMMAND, 0x1F);
		// 清空FIFO缓冲区
		EI_mifs_vFlushFIFO();

		if (ucKeyType == EM_mifs_KEYA)
			EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_AUTHENT1A;
		else
			EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_AUTHENT1B;

		EG_mifs_tWorkInfo.aucBuffer[1] = ucAddr;
//		memcpy(EG_mifs_tWorkInfo.aucBuffer+2,gtPN512Info.gPN512Buf,6); //key
//		memcpy(EG_mifs_tWorkInfo.aucBuffer + 2 + 6, &EG_mifs_tWorkInfo.ulCardID, 4);
		memcpy(EG_mifs_tWorkInfo.aucBuffer + 2, &EG_mifs_tWorkInfo.ulCardID, 4);

//        TRACE("\r\nRC663 API logic card\r\n");
//        DISPBUF(EG_mifs_tWorkInfo.aucBuffer, 6, 0);

        EI_mifs_vWriteReg(6, PHHAL_HW_RC663_REG_FIFODATA, EG_mifs_tWorkInfo.aucBuffer);

		ucTempData = PHHAL_HW_RC663_CMD_MFAUTHENT;
        EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_COMMAND, ucTempData);

		//EI_mifs_vReadReg(1,PHHAL_HW_RC663_REG_ERROR,&ucTempData);
		//TRACE("\r\n 错误寄存器:%x",ucTempData);

		time = sys_get_counter();
		while(1)
		{
			// 看Crypto1 加密是否已经启动，如果没有启动，说明认证失败
			EI_mifs_vReadReg(1,PHHAL_HW_RC663_REG_STATUS,&ucTempData);
			if(ucTempData&0x20)
				break;
			if(if_timerout(time,100))
			{
				ucResult = EM_mifs_AUTHERR;
				break;
			}
		}

//        EI_mifs_vReadReg(1,PHHAL_HW_RC663_REG_STATUS,&ucTempData);
//        TRACE("\r\n---Crypto1:%x",ucTempData);

		// 结束当前正在运行的命令
        s_DelayUs(5000); //一定要加 不然清空COMMAND寄存器会影响读mifare卡
        EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_COMMAND, 0x1F);

		return ucResult;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		s_rfid_setRC531Int(0,PN512_IRQ_ALL);
		s_rfid_setRC531IntFlg(0,PN512_IRQ_ALL);

		// 禁止crypto1单元
		EI_mifs_vClrBitMask(Status2Reg, 0x08);

		//EI_mifs_vReadReg(1,Status2Reg,&ucTempData);
		//TRACE("\r\nCrypto1:%x",ucTempData);

		// 清空FIFO缓冲区
		EI_mifs_vFlushFIFO();

		// 结束当前正在运行的命令
		ucTempData = PN512CMD_IDLE;
		ucTempData &= 0x0F;
		EI_mifs_vClrBitMask(CommandReg, 0x0F);
		EI_mifs_vSetBitMask(CommandReg, ucTempData);

		if (ucKeyType == EM_mifs_KEYA)
		{
			EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_AUTHENT1A;
		}
		else
		{
			EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_AUTHENT1B;
		}

		EG_mifs_tWorkInfo.aucBuffer[1] = ucAddr;
		memcpy(EG_mifs_tWorkInfo.aucBuffer+2,gtRfidProInfo.gMifareKey,6); //key
		memcpy(EG_mifs_tWorkInfo.aucBuffer + 2 + 6, &EG_mifs_tWorkInfo.ulCardID, 4);

		//TRACE("\r\nAPI logic card\r\n");
		//DISPBUF(EG_mifs_tWorkInfo.aucBuffer, 12, 0);

		EI_mifs_vWriteReg(12, FIFODataReg, EG_mifs_tWorkInfo.aucBuffer);

		ucTempData = PN512CMD_AUTHENT;
		ucTempData &= 0x0F;
		EI_mifs_vClrBitMask(CommandReg, 0x0F);
		EI_mifs_vSetBitMask(CommandReg, ucTempData);

		//EI_mifs_vReadReg(1,ErrorReg,&ucTempData);
		//TRACE("\r\n 错误寄存器:%x",ucTempData);

		time = sys_get_counter();
		while(1)
		{
			// 看Crypto1 加密是否已经启动，如果没有启动，说明认证失败
			EI_mifs_vReadReg(1,Status2Reg,&ucTempData);
			if(ucTempData&0x08)
				break;
			if(if_timerout(time,100))
			{
				ucResult = EM_mifs_AUTHERR;
				break;
			}
		}

		s_rfid_setRC531Int(0,PN512_IRQ_ALL);
		s_rfid_setRC531IntFlg(0,PN512_IRQ_ALL);
		/*
		ucTempData = PN512CMD_IDLE;
		ucTempData &= 0x0F;
		EI_mifs_vClrBitMask(CommandReg, 0x0F);
		EI_mifs_vSetBitMask(CommandReg, ucTempData);
		*/

		return ucResult;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module
/**********************************************************************
 执行认证过程1
***********************************************************************/
        // RxCRC disable,TxCRC, Parity enable
        ucTempData = 0x07;
        EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

        // 设置定时器时间
        EI_mifs_vSetTimer(4);

        // 准备发送数据: 认证命令, 扇区所在首块号, 卡号（4字节）
        if (ucKeyType == EM_mifs_KEYA)
        {
            EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_AUTHENT1A;
        }
        else
        {
            EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_AUTHENT1B;
        }

        EG_mifs_tWorkInfo.aucBuffer[1] = ucAddr;
        memcpy(EG_mifs_tWorkInfo.aucBuffer + 2, &EG_mifs_tWorkInfo.ulCardID, 4);
        EG_mifs_tWorkInfo.ulSendBytes = 6;	// how many bytes to send
        EG_mifs_tWorkInfo.ucDisableDF = 0;

        // 执行命令
        ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_AUTHENT1);
        if (ucResult != EM_SUCCESS)
        {
            return ucResult;
        }

        EI_mifs_vReadReg(1, EM_mifs_REG_SECONDARYSTATUS, &ucTempData);
        if ((ucTempData & 0x07) != 0)
        {
            return EM_mifs_BITCOUNTERR;
        }

/**********************************************************************
 执行认证过程2
***********************************************************************/
        // RxCRC,TxCRC disable, Parity enable
        ucTempData = 0x03;
        EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

        // 不需要准备发送数据
        EG_mifs_tWorkInfo.ulSendBytes = 0;	// how many bytes to send
        EG_mifs_tWorkInfo.ucDisableDF = 0;

        // 发送命令
        ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_AUTHENT2);
        //逻辑卡认证出错有两种情况
        //出错情况1:(如密钥错) 则ucResult == EM_SUCCESS 但Crypto1 加密没有启动返回4
        //出错情况2:(如密钥错) 则ucResult == EM_mifs_NOTAGERR 直接返回2
        if (ucResult != EM_SUCCESS)
        {
            return ucResult;
        }

        // 看Crypto1 加密是否已经启动，如果没有启动，说明认证失败
        EI_mifs_vReadReg(1, EM_mifs_REG_CONTROL, &ucTempData);
        if ((ucTempData & 0x08) != 0)
        {
            return EM_SUCCESS;
        }
        else
        {
            return EM_mifs_AUTHERR;
        }
#endif

	}

	return 0xFF;
}

/**********************************************************************
* 函数名称：
*     EI_mifs_ucDeselect(uchar ucType, uchar ucCID)
* 功能描述：
*     使已建立通道的卡片处于HALT状态
* 输入参数：
*    ucType     卡片类型
*     ucCID: 指定卡号
* 输出参数：
*     无
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
***********************************************************************/
uchar EI_mifs_ucDeselect(uchar ucType, uchar ucCID)
{
	uint uiLen;
	uchar ucResult;

	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_DESELECT;
	if (EG_mifs_tWorkInfo.ucCIDSupport == 1)
	{
		EG_mifs_tWorkInfo.aucBuffer[1] = ucCID;
		ucResult = EI_mifs_ucProcess(ucType, 2, &uiLen);
	}
	else
	{
		EG_mifs_tWorkInfo.aucBuffer[0] &= 0xF7;
		ucResult = EI_mifs_ucProcess(ucType, 1, &uiLen);
	}
	if (ucResult == EM_SUCCESS)
	{
		if (uiLen == 0)
		{
			return EM_mifs_NOTAGERR;
		}
		if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xF6) != (EM_mifs_PICC_DESELECT & 0xF6))
		{
			return EM_mifs_CODEERR;
		}
		if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x08) == 0x08)
		{
			// 存在CID
			if (uiLen != 2)
			{
				return EM_mifs_CODEERR;
			}
		}
		else
		{
			// 不存在CID
			if (uiLen != 1)
			{
				return EM_mifs_CODEERR;
			}
		}
	}
	return ucResult;
}

/**********************************************************************
* 函数名称：
*     EI_mifs_ucProcess(uchar ucType,uint uiSendLen, uint* puiRecLen)
* 功能描述：
*     完成读写器与卡的数据交换操作。
* 输入参数：
*    ucType: 卡类型
*    uiSendLen: 要发送数据长度
* 输出参数：
*    puiRecLen: 接收到数据长度
* 返回值：
*     处理结果
* 历史纪录：
*     修改人        日期                版本号      修改记录
*                 2004-08-14          01-01-01    创建
***********************************************************************/
uchar EI_mifs_ucProcess(uchar ucType, uint uiSendLen, uint * puiRecLen)
{

#ifdef EM_RC531_Module

	uchar ucTempData;
	uchar ucResult;

	EI_mifs_vFlushFIFO();
	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

	EI_mifs_vSetTimer(EG_mifs_tWorkInfo.ucFWI);

	switch (ucType)
	{
	case EM_mifs_TYPEB:
		{
			ucTempData = 0x2C;
			EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
			break;
		}
	case EM_mifs_TYPEA:
	default:
		{
			// RxCRC,TxCRC, Parity enable
			ucTempData = 0x0F;
			EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
			break;
		}
	}

	D2(
		DATAIN(0x55);
		DATAIN(LBYTE(uiSendLen));
		BUFFIN(uiSendLen,EG_mifs_tWorkInfo.aucBuffer);
	);
	// 准备发送数据: 命令码
	EG_mifs_tWorkInfo.ulSendBytes = uiSendLen;	// how many bytes to send
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_TRANSCEIVE);
	if (ucResult == EM_SUCCESS)
	{
		// 保存接收到的数据
		*puiRecLen = EG_mifs_tWorkInfo.ulBytesReceived;
	}
	else
	{
		*puiRecLen = 0;
	}

	D2(
		DATAIN(0x66);
		DATAIN(LBYTE(*puiRecLen));
		BUFFIN(*puiRecLen,EG_mifs_tWorkInfo.aucBuffer);
	);
	// 延时500us
	EI_paypass_vDelay(70);
	return ucResult;

#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	return 0xFF;

#endif

}

/**********************************************************************
* 函数名称：
*    EI_mifs_ucBeginProtect(DevHandle* phDevHandle,uchar ucMode)
* 功能描述：
*   获取射频卡模块公共资源，spi总线
* 输入参数：
* 输出参数：
*   无
* 返回值：
*   EM_SUCCESS: 成功
*   EM_mifs_ERRTYPE: 卡类型错误
*   EM_ERRHANDLE: 非法句柄
*   EM_ERRPARAM: 参数错误
*   EM_FAILED: 其他错误（系统错误等）
* 历史纪录：
*   修改人      日期            版本号      修改内容
*             2004-08-14      01-01-01    创建
**********************************************************************/
uchar EI_mifs_ucBeginProtect(DevHandle * phDevHandle, uchar ucMode)
{
//#ifdef EM_AS3911_Module
    hw_rfid_spi_init();
//#endif
	return 0;
}

/**********************************************************************
* 函数名称：
*      EI_mifs_vEndProtect(void)
* 功能描述：
*     释放射频卡模块公共资源，spi总线
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人        日期            版本号      修改内容
*                 2004-08-14      01-01-01    创建
**********************************************************************/
void EI_mifs_vEndProtect(void)
{
#ifdef CFG_W25XFLASH
    hw_w25x_spi_init();
#endif
}

//************************************************************************************
//
//
//         采用新射频API
//
//
//************************************************************************************

/**********************************************************************
* 函数名称：
*
* 功能描述：
*     //寻TypeB卡 0-成功有卡 1-有多张B卡  2-无卡
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
int rfid_change_ret(uint ret)
{
	switch(ret)
	{
		case 0:
		case EM_mifs_SWDIFF:
			return RFID_SUCCESS;
		case EM_ERRPARAM:
			return -RFID_ERRPARAM;
		case EM_mifs_TIMEOUT:
			return -RFID_TIMEOUT;
		case EM_mifs_NOCARD:
		case EM_mifs_NOTAGERR:
			return -RFID_NOCARD;
		case EM_mifs_TRANSERR:
		case EM_mifs_CODEERR:
			return -RFID_TRANSERR;
		case EM_mifs_PROTERR:
			return -RFID_PROTERR;
		case EM_mifs_MULTIERR:
			return -RFID_MULTIERR;
		case EM_mifs_NOACT:
			return -RFID_NOACT;
		case EM_mifs_CARDEXIST:
			return -RFID_CARDEXIST;
		case EM_mifs_AUTHERR:
		case EM_mifs_NOTAUTHERR:
			return -RFID_NOAUTH;
		default:
			return -RFID_ERROR;
	}
}

/**********************************************************************
* 函数名称：
*      if_rfid_module
* 功能描述：
*     激判断是否有射频模块
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人        日期            版本号      修改内容
*                 2004-08-14      01-01-01    创建
**********************************************************************/
int if_rfid_module(void)
{
	if(sys_get_module_type(MODULE_RF_TYPE) == MODULE_NOTEXIST)
	{
		return -RFID_ERROR;
	}
	else
    {
		return RFID_SUCCESS;
    }
}
/**********************************************************************
* 函数名称：
*      rfid_is_open
* 功能描述：
*     激判断是否有射频模块
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人        日期            版本号      修改内容
*                 2004-08-14      01-01-01    创建
**********************************************************************/
int rfid_is_open(void)
{
	if(EG_mifs_tWorkInfo.ucFirstOpenFlag)
	{
		return RFID_SUCCESS;
	}
	else
	{
		return -RFID_ERROR;
	}
}
/**********************************************************************
* 函数名称：
*      rfid_close
* 功能描述：
*     激活卡
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人        日期            版本号      修改内容
*                 2004-08-14      01-01-01    创建
**********************************************************************/
int rfid_open(uint mode)
{
	uchar openmode;
    int iRet;
	Assert_OK(if_rfid_module());
	if(mode == 0)
	{
		openmode = EM_mifs_TYPEA;
	}
	else if(mode == 1)
	{
		openmode = EM_mifs_TYPEB;
	}
	else
	{
		openmode = EM_mifs_NULL;
	}
    iRet = rfid_change_ret((uint)EA_ucMifsOpen(openmode));
    if (iRet == RFID_SUCCESS) {
    }
	return iRet;
}
/**********************************************************************
* 函数名称：
*      rfid_close
* 功能描述：
*     激活卡
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人        日期            版本号      修改内容
*                 2004-08-14      01-01-01    创建
**********************************************************************/
int rfid_close(void)
{
    int iRet;
	Assert_OK(if_rfid_module());
    iRet = rfid_change_ret((uint)EA_ucMifsClose());
    if (iRet == RFID_SUCCESS) {
    }
	return iRet;
}
/**********************************************************************
* 函数名称：
*      rfid_activate
* 功能描述：
*     激活卡
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人        日期            版本号      修改内容
*                 2004-08-14      01-01-01    创建
**********************************************************************/
int rfid_poll(uint mode, uint *cardtype)
{
	uchar ret;
	uchar type=0;
	Assert_OK(if_rfid_module());
	Assert_OK(rfid_is_open());
	if((cardtype == EM_NULL)
       || (mode & (~0x0F)))
	{
	    return -RFID_ERRPARAM;
	}
//    if ( mode&RFID_MODE_EMV ) {
//        //EMV模式 转为ISO模式
//        mode &= (~RFID_MODE_EMV);
//    }
    EG_mifs_tWorkInfo.OptMode = mode;
//    D1(
//        TRACE("\r\n poll:");
//        if ( mode&RFID_MODE_EMV ) {
//        	TRACE("EMV");
//        } else {
//        	TRACE("ISO");
//        }
//        TRACE(" FRAME:%d CID:%d NAD:%d",GETBIT(mode,1)
//              ,GETBIT(mode,2),GETBIT(mode,3));
//    );
    if ( EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV) {
		EG_mifs_tWorkInfo.ucAnticollFlag = 0;
    } else {
		EG_mifs_tWorkInfo.ucAnticollFlag = 1;
    }
	CLRBUF(&gMifareData);
	ret = EA_ucMIFPollMifCard(0,EG_mifs_tWorkInfo.ucAnticollFlag,&type);
	if(!ret)
	{
		if(EG_mifs_tWorkInfo.ucCurType == EM_mifs_TYPEA)
		{
			type = RFID_TYPEA;
		}
		else
		{
			type = RFID_TYPEB;
		}
		*cardtype = type;
	}

	return rfid_change_ret((uint)ret);
}

/**********************************************************************
* 函数名称：
*      rfid_activate
* 功能描述：
*     激活卡
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人        日期            版本号      修改内容
*                 2004-08-14      01-01-01    创建
**********************************************************************/
int rfid_powerup(uint cardtype, uint *puiOutLen, uchar *pvOut)
{
	uint ret;
//    uint recvlen;
//    uchar recv[8];
	Assert_OK(if_rfid_module());
	Assert_OK(rfid_is_open());
	ret = EA_ucMIFActivate(0,cardtype,puiOutLen,pvOut);
	if(!ret)
	{
		if(EG_mifs_tWorkInfo.ucMifCardType <= RFID_CARD_S70 ||
			(EG_mifs_tWorkInfo.ucMifCardType >= RFID_CARD_PROS50
				&& EG_mifs_tWorkInfo.ucMifCardType <= RFID_CARD_PROS70))
		{
			gMifareData.pwred = 1;
			if(EG_mifs_tWorkInfo.ucMifCardType <= RFID_CARD_S70)
				gMifareData.type = EG_mifs_tWorkInfo.ucMifCardType;
			else
				gMifareData.type = EG_mifs_tWorkInfo.ucMifCardType-3;
			//TRACE("\r\n MIFARE:%d-%d",gMifareData.pwred,gMifareData.type);
		}
#if 0
        // 根据部分卡返回CID但是却不支持CID的特性，再试验上电成功后获取随机数命令
        DISPPOS(EG_mifs_tWorkInfo.ucCIDFlag);
        if(EG_mifs_tWorkInfo.ucCIDFlag == 1)
        {
            if(EG_mifs_tWorkInfo.ucCIDPollFlg == 0){
                // 卡返回支持CID,需要探测
                EG_mifs_tWorkInfo.ucCIDPollFlg = 1;
                //修改时间
                EG_mifs_tWorkInfo.ulFWT = s_rfid_FWI2FWT(4);
                ret = EA_ucMIFAPDU(0,5,"\x00\x84\x00\x00\x02",&recvlen,recv);
                DISPPOS(ret);
                EG_mifs_tWorkInfo.ucCIDPollFlg = 0;
                EG_mifs_tWorkInfo.ulFWT = s_rfid_FWI2FWT(EG_mifs_tWorkInfo.ucFWI);
                if ( ret != EM_mifs_SWDIFF && ret != 0) {
                    //探测失败,重新上电
                    ret = EA_ucMIFPollMifCard(0,EG_mifs_tWorkInfo.ucAnticollFlag,&recv);
                    if ( ret==0 ) {
                        ret = EA_ucMIFActivate(0,recv[0],puiOutLen,pvOut);
                        if ( ret==0 ) {
                            //卡不支持CID
                            EG_mifs_tWorkInfo.ucCIDFlag = 0;
                            return rfid_change_ret(ret);
                        } else {
                            return rfid_change_ret(ret);
                        }
                    } else {
                        return rfid_change_ret(ret);
                    }
//                    EG_mifs_tWorkInfo.ucCIDFlag = 0;
                    ret = 0;
                }else{
                    //卡确实支持CID

                }
            }
        }
#endif
	}
	return rfid_change_ret(ret);
}

/**********************************************************************
* 函数名称：
*      rfid_exchangedata
* 功能描述：
*      卡数据交换
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人        日期            版本号      修改内容
*                 2004-08-14      01-01-01    创建
**********************************************************************/
int rfid_exchangedata(uint sendlen, uchar *psenddata, uint *precvlen, uchar *precvdata)
{
	Assert_OK(if_rfid_module());
	Assert_OK(rfid_is_open());
	return rfid_change_ret((uint)EA_ucMIFAPDU(0,sendlen,psenddata,precvlen,precvdata));
}

/**********************************************************************
* 函数名称：
*      rfid_deactivate
* 功能描述：
*     将卡片置为HALT，并等待卡片离开感应区
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人        日期            版本号      修改内容
*                 2004-08-14      01-01-01    创建
**********************************************************************/
int rfid_powerdown(void)
{
    int ret;
	Assert_OK(if_rfid_module());
	Assert_OK(rfid_is_open());
    ret = rfid_change_ret((uint)EA_ucMIFDeactivate(0));
//    if(EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV) //emv 模式
//        EI_paypass_vResetPICC();  //交互完毕 或失败 复位射频场(根据qpobc level1)
    rfid_ResetField(0);
	return ret;
}


//*************************************************************************
//
//         Mifare卡API
//
//
//*************************************************************************
/**********************************************************************
* 函数名称：
*    mifs_if_block
* 功能描述：
*    判断输入参数是否有错，并转换成绝对块号
* 输入参数：
*     mode:  0-必须为最后1块 1-不能为最后1块
*     sector:扇区号 (S50:1个区4个块 S70:1个区4个或16个块)
*     block: 扇区内块号
* 输出参数：
*      pBlk:块
* 返回值：
*     -1-失败  >0-该sector区的块数
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
int mifs_if_sector(uint sector)
{
	D1(vDispBuf(sizeof(gMifareData), 0, (uchar *)&gMifareData););
	if(EG_mifs_tWorkInfo.ucMifCardType == RFID_CARD_PRO
		|| EG_mifs_tWorkInfo.ucMifCardType == RFID_CARD_B)
	{
		//卡类型不是M1卡直接返回超时
		return -RFID_TIMEOUT;
	}
	if(!gMifareData.pwred)
	{
		return -RFID_NOACT;
	}
	if(gMifareData.type == EM_mifs_S50)
	{
		if (sector >= S50SECTOR_SIZE)
		{
			return -RFID_ERRPARAM;
		}
	}
	else
	{
		if (sector >= S70SECTOR_SIZE)
		{
			return -RFID_ERRPARAM;
		}
	}
	return 0;
}

int mifs_TrailerNumInSector(uchar sector,uchar *pBlkNum)
{
	int iRet;

	iRet = 0x00;
	if (sector < S70SECTOR_SIZE_PRE32)
	{
		*pBlkNum = S70BLK_SIZE_PRE32-1;
	} else if (sector < S70SECTOR_SIZE)
	{
		*pBlkNum = S70BLK_SIZE_LAST8-1;
	} else
	{
		*pBlkNum = 0x00;
		iRet = -1;
	}
	return iRet;
}
int mifs_if_block(uint mode, uchar sector, uint block, uint *blk)
{
	uint i;
	uint blkpersec;
	uchar blkNo;
	if(!mode)
	{
		if(gMifareData.pwred == 0)
		{
			return -RFID_NOACT;
		}
		if(gMifareData.pwred == 1)
		{
			return -RFID_NOAUTH;
		}
	}
	mifs_TrailerNumInSector(sector,&blkNo);
	if (block > blkNo)
	{
		return -RFID_ERRPARAM;
	}
	if(gMifareData.type == EM_mifs_S50)
	{
		blkpersec = S50BLK_SIZE;
		i = sector*blkpersec;
	}
	else
	{
		if (sector < S70SECTOR_SIZE_PRE32)
		{
			blkpersec = S70BLK_SIZE_PRE32;
			i = sector*blkpersec;
		}
		else
		{
			blkpersec = S70BLK_SIZE_LAST8;
			i = S70SECTOR_SIZE_PRE32*S70BLK_SIZE_PRE32
				+ (sector - S70SECTOR_SIZE_PRE32)*blkpersec;
		}
	}
	*blk = i+block;
	TRACE("\r\n [Sec:%d block:%d]",sector,*blk);
	return 0;
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*      Sec和block:Mifare卡区和块 (S50:1个区4个块 S70:1个区4个或16个块)
*      keytype:0-A 1-B
* 输出参数：
*     pData:密钥
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
int rfid_MIFAuth(uint Sec, uint keytype, uchar *pData)
{
	uchar ucRet,ucBlkNumOfSec;
	uchar ucReSendNo,result;
	uchar ucResp[2];
	uint blk;
	uint len;
	uchar buf[64];
	DevHandle hDevHandle;
    if ( pData == NULL) {
        return -RFID_ERRPARAM;
    }
	Assert_OK(rfid_is_open());
	Assert_OK(mifs_if_sector(Sec));
	mifs_TrailerNumInSector(Sec,&ucBlkNumOfSec);
	Assert_OK(mifs_if_block(1,Sec,ucBlkNumOfSec,&blk));

	result = EI_mifs_ucBeginProtect(&hDevHandle, 3);
	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}
	EI_paypass_vResetPICC();
	EI_paypass_vSelectType(EM_mifs_TYPEA);
	s_DelayMs(6);
	for (ucReSendNo = 0; ucReSendNo < 3; ucReSendNo++)
	{
		// 先进行WUPA操作
		result = EI_paypass_ucWUPA(ucResp); //逻辑卡再次寻卡
		if (result != EM_mifs_TIMEOUT)
		{
			break;
		}
	}

	// 其他错误情况则不重发，直接返回给终端
	if (result != EM_mifs_SUCCESS)
	{
		return rfid_change_ret((uint)result);
	}

	result = EI_paypass_ucGetUID();

	if(result != EM_mifs_SUCCESS)
	{
		return rfid_change_ret((uint)result);  //逻辑卡再次上电
	}

	ucRet = EA_ucMIFAuthToRamExtend(0,(uchar)blk,(uchar)keytype,pData);
	if (ucRet == EM_SUCCESS)
	{
		gMifareData.pwred = 2;
		gMifareData.sector = Sec;
		gMifareData.key = keytype+1;
		memcpy(gMifareData.keybuf,pData,6);
	}
	else
	{
		if(EM_mifs_NOTAGERR == ucRet)
		{
			ucRet = EA_ucMIFActivate(0,gMifareData.type,&len,buf);
			if(ucRet == EM_mifs_TIMEOUT)
			{
				//真的无卡
				ucRet = EM_mifs_NOTAGERR;
			}
			else
			{
				//认证失败
				return -RFID_AUTHERR;
			}
		}
	}
	return rfid_change_ret((uint)ucRet);
}

/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*      pMem:Mifare卡区和块 (S50:1个区4个块 S70:1个区4个或16个块)
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
int rfid_MIFRead(uint block, uchar *out)
{
	uint blk;
    if ( out == NULL) {
        return -RFID_ERRPARAM;
    }
	Assert_OK(rfid_is_open());
	Assert_OK(mifs_if_block(0,gMifareData.sector,block,&blk));
	return rfid_change_ret((uint)EA_ucMIFRead(0,(uint)blk,out));
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*      pMem:Mifare卡区和块 (S50:1个区4个块 S70:1个区4个或16个块)
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
int rfid_MIFWrite(uint block, uchar *in)
{
	uint blk;
    if ( in == NULL) {
        return -RFID_ERRPARAM;
    }
	Assert_OK(rfid_is_open());
	Assert_OK(mifs_if_block(0,gMifareData.sector,block,&blk));
	return rfid_change_ret((uint)EA_ucMIFWrite(0,(uint)blk,in));
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*      pMem:Mifare卡区和块 (S50:1个区4个块 S70:1个区4个或16个块)
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void rfid_MIFMakevalue(long value,uchar addr, uchar *out)
{
	long data1;
	uchar pucBuf[16];
    if ( out == NULL) {
        return;
    }
	data1 = ~value; // 准备存入数值为125
	pucBuf [12] = pucBuf [14] = addr;
	pucBuf [13] = pucBuf [15] = ~addr;
	memcpy((char *)&pucBuf[0],(char *)&value,4);
	memcpy((char *)&pucBuf[4],(char *)&data1,4);
	memcpy((char *)&pucBuf[8],(char *)&value,4);
	memcpy(out,(char *)pucBuf,sizeof(pucBuf));
}


int rfid_MIFIncrement(uint block, ulong value)
{
	uint blk;
	Assert_OK(rfid_is_open());
	Assert_OK(mifs_if_block(0,gMifareData.sector,block,&blk));
	return rfid_change_ret((uint)EA_ucMIFIncrement(0, (uint)blk, value));
}

int rfid_MIFDecrement(uint block, ulong value)
{
	uint blk;
	Assert_OK(rfid_is_open());
	Assert_OK(mifs_if_block(0,gMifareData.sector,block,&blk));
	return rfid_change_ret((uint)EA_ucMIFDecrement(0, (uint)blk, value));
}

/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*      pMem:Mifare卡区和块 (S50:1个区4个块 S70:1个区4个或16个块)
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
int rfid_MIFRestore(uint block)
{
	uint blk;
	Assert_OK(rfid_is_open());
	Assert_OK(mifs_if_block(0,gMifareData.sector,block,&blk));
	return rfid_change_ret((uint)EA_ucMIFRestore(0, (uint)blk));
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*      pMem:Mifare卡区和块 (S50:1个区4个块 S70:1个区4个或16个块)
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
int rfid_MIFTransfer(uint block)
{
	uint blk;
	Assert_OK(rfid_is_open());
	Assert_OK(mifs_if_block(0,gMifareData.sector,block,&blk));
	return rfid_change_ret((uint)EA_ucMIFTransfer(0, (uint)blk));
}

/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
uchar rfid_MIFParseStatus(const ACCESSBIT *pAccess, uchar ucBlkNum)
{
	uchar ucRet;

	if (ucBlkNum > 0x3)
	{
		return 0xFF;
	}
	ucRet = 0;
	ucBlkNum = (0x01<<ucBlkNum);
	if (pAccess->c1&ucBlkNum)
	{
		ucRet |= 0x04;
	}
	if (pAccess->c2&ucBlkNum)
	{
		ucRet |= 0x02;
	}
	if (pAccess->c3&ucBlkNum)
	{
		ucRet |= 0x01;
	}

	return ucRet;
}

/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*      access:从尾块读出来的接入条件
*	  authKey:认证过的密钥类型  1-KeyA  2-KeyB
* 输出参数：
*
* 返回值：
*     0x00-成功  0x10-不可修改  0x20-认证密钥无权限
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
uchar rfid_MIFAccessIfAlter(uchar access,uchar authKey)
{
	uchar ucRet;

	ucRet = 0x00;
	switch (access)
	{
		case 0x01:
			if (authKey != 1)//Not KeyA
			{
				ucRet = 0x20;
			}
			break;
		case 0x03:
		case 0x05:
			if (authKey != 2)//Not KeyB
			{
				ucRet = 0x20;
			}
			break;
		default:
			ucRet = 0x10;
			break;
	}
	return ucRet;
}

/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*      access:从尾块读出来的接入条件
*	  authKey:认证过的密钥类型  1-KeyA  2-KeyB
* 输出参数：
*
* 返回值：
*     0x00-成功  0x10-不可修改  0x20-认证密钥无权限
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
uchar rfid_MIFKeyIfAlter(uchar access,uchar authKey)
{
	uchar ucRet;

	ucRet = 0x00;
	switch (access)
	{
		case 0x00:
		case 0x01:
			if (authKey != 1)//Not KeyA
			{
				ucRet = 0x20;
			}
			break;
		case 0x03:
		case 0x04:
			if (authKey != 2)//Not KeyB
			{
				ucRet = 0x20;
			}
			break;
		default:
			ucRet = 0x10;
			break;
	}
	return ucRet;
}

/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*      ucIndata:(Fig 9. access condition)( tab3 and tab4)
*	   ucBlkNum:块号  一个区里的块号
*	   pAccess:指向卡最后一区数据
* 输出参数：
*     pAccess:修改后的数据
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void rfid_MIFMakeStatus(uchar ucIndata,uchar ucBlkNum, ACCESSBIT *pAccess)
{
	uchar bit;

	ucBlkNum = 0x01<<ucBlkNum;
	for (bit=0x01; bit<=0x40; bit<<=1)
	{
		switch (bit)
		{
			case 1:
				if (ucIndata&bit)
				{
					pAccess->c3 |= ucBlkNum;
				} else
				{
					pAccess->c3 &= ~ucBlkNum;
				}
				break;
			case 0x02:
				if (ucIndata&bit)
				{
					pAccess->c2 |= ucBlkNum;
				} else
				{
					pAccess->c2 &= ~ucBlkNum;
				}
				break;
			case 0x04:
				if (ucIndata&bit)
				{
					pAccess->c1 |= ucBlkNum;
				} else
				{
					pAccess->c1 &= ~ucBlkNum;
				}
				break;

			default:
				break;
		}
	}
	pAccess->versac1 = ~pAccess->c1;
	pAccess->versac2 = ~pAccess->c2;
	pAccess->versac3 = ~pAccess->c3;
}

/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
int rfid_MIFModify(uint block, const BLKALTER *alterVal)
{
	uint blk;
	TRAILER trailer;	//用于保存读取的尾块数据
	uchar i,ucTmp;
	uchar ucTrailNo,ucTrailBlk;
	uchar ucRet;

	TRACE("\r\nModify-Sect:%d, Blk:%d",gMifareData.sector,block);
	TRACE(" mode:%02x access:%02x",alterVal->mode,alterVal->access);
	Assert_OK(rfid_is_open());
	Assert_OK(mifs_if_block(0,gMifareData.sector,block,&blk));
	if ((alterVal->mode&0xF8) != 0x00
		|| alterVal->access > 0x07)
	{
		return -RFID_ERRPARAM;
	}
	memset(&trailer,0x00,sizeof(trailer));
	mifs_TrailerNumInSector(gMifareData.sector,&ucTrailNo);
	//分析卡内尾块数据
	ucTrailBlk = (uchar)(blk - block);
	ucTrailBlk += ucTrailNo;
	TRACE("\r\nTrailNO:%d",ucTrailBlk);
	ucRet = EA_ucMIFRead(0,ucTrailBlk,(uchar*)&trailer);
	D1(vDispBuf(16,0,(uchar *)&trailer););
	if (ucRet != EM_SUCCESS)
	{
		return rfid_change_ret((uint)ucRet);
	}
	ucTmp = ucTrailNo;
	if (gMifareData.type == EM_mifs_S70
		&& ucTrailNo >= S70BLK_SIZE_PRE32)
	{
			ucTmp = ucTrailNo/5;
	}
	//查看是否有修改权限blk=3
//		TRACE("\r\n acces:");
	D1(vDispBuf(sizeof(trailer.access),0,(uchar *)&trailer.access););
	ucRet = rfid_MIFParseStatus(&trailer.access,ucTmp);
	TRACE("\r\nPase:%02x,ucTmp:%02x",ucRet,ucTmp);
	if (ucRet > 0x02)	//在B密钥不可读时，若修改密钥必须同时修改A/B
	{
		if (ucRet > 0x07)
		{
			return -RFID_PROTERR;
		}
		if ((alterVal->mode&0x06) == 0x02
			|| (alterVal->mode&0x06) == 0x04)
		{
			return -RFID_ERRPARAM;
		}
	}
	if (gMifareData.key == 1)	//KeyA
	{
		memcpy(&trailer.keyA,gMifareData.keybuf,6);
	} else if (gMifareData.key == 2)	//KeyB
	{
		memcpy(&trailer.keyB,gMifareData.keybuf,6);
	} else
	{
		return -RFID_NOAUTH;
	}
	ucTmp = 0x00;
	for (i=1; i<=0x04; i<<=1)
	{
		switch (alterVal->mode & i)
		{
			case 0x01:	//access
				TRACE(" Access%02x",i);
				ucTmp = rfid_MIFAccessIfAlter(ucRet,gMifareData.key);
				rfid_MIFMakeStatus(alterVal->access,(uchar)block,&trailer.access);
				break;
			case 0x02:	//keyA
				TRACE(" keyA%02x",i);
				if (block != ucTrailNo)
				{
					TRACE("\r\nERR-A");
					return -RFID_ERRPARAM;
				}
				ucTmp = rfid_MIFKeyIfAlter(ucRet,gMifareData.key);
				memcpy(&trailer.keyA,alterVal->keyA,6);
				break;
			case 0x04:	//keyB
				TRACE(" keyB%02x",i);
				if (block != ucTrailNo)
				{
					TRACE("\r\nERR-B");
					return -RFID_ERRPARAM;
				}
				ucTmp = rfid_MIFKeyIfAlter(ucRet,gMifareData.key);
				memcpy(&trailer.keyB,alterVal->keyB,6);
				break;
			default:
				TRACE(" None%02x",i);
				break;
		}
		if (ucTmp == 0x10)
		{
			return -RFID_UNCHANGE;
		} else if (ucTmp == 0x20)
		{
			return -RFID_KEYNOTPOWER;
		}
	}
	D1(vDispBuf(16,0,(uchar *)&trailer););
	if (alterVal->mode == 0)
	{
		ucRet = EM_SUCCESS;
	} else
	{
		ucRet = EA_ucMIFWrite(0,ucTrailBlk,(uchar*)&trailer);
	}
	if (ucRet == EM_SUCCESS)
	{
		if (alterVal->mode == 0x02)
		{
			memcpy(gMifareData.keybuf,&trailer.keyA,6);
		} else if (alterVal->mode == 0x04)
		{
			memcpy(gMifareData.keybuf,&trailer.keyB,6);
		}
	}
	return rfid_change_ret((uint)ucRet);
}

//调节射频参数 para-调整参数  step-调节步骤 1-poll 2-active 3-random/MIFAuth
int rfid_autoParaAdjust(int para, int step)
{
	int ret = OK;
	int i,j,paraback,type;
	uchar buf[64];

	ret = if_rfid_module();
	if(ret)
	{
		return ret;
	}
	ret = rfid_open(0);
	if(ret)
	{
		return ret;
	}

	if(para == RFID_PARA_PWRSETTIME)
	{
		lcd_cls();
		lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "当前场强时间: %d", "Cur Pwr Time: %d",EG_mifs_tWorkInfo.PwrAdTime);
		lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "自动调节中...", "Auto Adjust...");
		paraback = EG_mifs_tWorkInfo.PwrAdTime;
		for(i = 0; i < (RFID_DEFAULT_PWRSETTIME+1); i++)
		{
			EG_mifs_tWorkInfo.PwrAdTime = RFID_DEFAULT_PWRSETTIME+i;
			ret = rfid_poll(RFID_MODE_ISO,(uint *)&type);
			if(!ret)
			{
				ret = rfid_powerup((uint)type,(uint *)&j,buf);
				if(!ret)
				{
					type = buf[j-1];
					if(buf[j-1] >= EM_mifs_PROCARD)
                    {
                    	ret = rfid_exchangedata(sizeof(gApduGet4rand), (uchar *)gApduGet4rand,(uint *)&j,buf);
                		//ret = rfid_exchangedata(sizeof(gApduSelMaindir), gApduSelMaindir,&j,buf);
	                    if(ret == EM_mifs_SUCCESS || ret == EM_mifs_SWDIFF)
	                    {
	                        lcdDispMultiLang(0,  DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE,"寻卡:%d 读卡[%02X-%02X]","Type:%d Rd:[%02X-%02X]",type,buf[j-2],buf[j-1]);
							lcdDispMultiLang(0,  DISP_FONT_LINE6, DISP_FONT|DISP_CLRLINE,"调整成功 :%dms","Adjust Succeed :%dms",EG_mifs_tWorkInfo.PwrAdTime);
	                        sys_beep_pro(DEFAULT_SOUNDFREQUENCY,30,YES);
							ret = WriteRfid_Para_Set(RFID_PARA_PWRSETTIME, EG_mifs_tWorkInfo.PwrAdTime);
							kb_getkey(3000);
							return ret;
	                    }
                    }
                    else
                    {
                    	//fac_MIFtest();
                    	lcdDispMultiLang(0,  DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE,"寻卡:%d Mifare卡","Type:%d Mifare card",type);
						lcdDispMultiLang(0,  DISP_FONT_LINE6, DISP_FONT|DISP_CLRLINE,"调整成功 :%dms","Adjust Succeed :%dms",EG_mifs_tWorkInfo.PwrAdTime);
                        sys_beep_pro(DEFAULT_SOUNDFREQUENCY,30,YES);
						ret = WriteRfid_Para_Set(RFID_PARA_PWRSETTIME, EG_mifs_tWorkInfo.PwrAdTime);
						kb_getkey(3000);
						return ret;
                    }
				}
			}
		}
		EG_mifs_tWorkInfo.PwrAdTime = paraback;
		lcdDispMultiLang(0,  DISP_FONT_LINE6, DISP_FONT|DISP_CLRLINE,"调整失败: %dms","Adjust Failure: %dms",EG_mifs_tWorkInfo.PwrAdTime);
		ret = ERROR;
		kb_getkey(3000);
	}
	else
	{
		ret = ERROR;
	}

	return ret;
}

//复位射频场
int rfid_ResetField(int para)
{
	EI_mifs_ucBeginProtect(0, 3);
    EI_paypass_vResetPICC();
    EI_mifs_vEndProtect();
    return 0;
}

//调节射频参数 para-调整参数  value -调整参数
int rfid_ParaAdjust(int para, int value)
{
	int ret = OK;
//	int i,j,paraback,type;
    s_rfidpara_info rfdata;
//	uchar buf[64];

	ret = if_rfid_module();
	if(ret)
	{
		return ret;
	}

	if(para == RFID_PARA_PWRSETTIME){
        if ( (value >= RFID_DEFAULT_PWRSETTIME) && (value <= RFID_MAX_PWRSETTIME)) {
            EG_mifs_tWorkInfo.PwrAdTime = value;
            s_DelayMs(20);
            ret = RFID_SUCCESS;
        }else
            ret = -RFID_ERRPARAM;
	}
	else if ( para == RFID_PARA_PWR) {
        if ( (value >= RFID_PWR_LEVEL0) && (value <= RFID_MAX_PWR)) {
            s_rfid_getPara(EG_mifs_tWorkInfo.RFIDModule, RFID_PARA_PWR, value, &rfdata);
            gtRfidDebugInfo.CW_A = (uchar)rfdata.pwrfield;
            gtRfidDebugInfo.CW_B = gtRfidDebugInfo.CW_A;
            if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
#ifdef EM_AS3911_Module  //AS3911
                gas3911Reg.gTypeBmodule = (uchar)rfdata.regpara;
                as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, gtRfidDebugInfo.CW_A); //根据硬件调节0x9F
                as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, gas3911Reg.gTypeBmodule);
                if ( s_rfid_mainVersion(0) == 0 ) {
                    //wp70v1.00是自动调整type b深度
                    as3911ModifyRegister(AS3911_REG_AM_MOD_DEPTH_CONTROL, 0x7E, 0x1E);
                    as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
                    as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
                    as3911ExecuteCommand(AS3911_CMD_CALIBRATE_MODULATION);
                    as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 200, (ulong *)&ret);
                    if ( ret != AS3911_IRQ_MASK_DCT) {
                        return -RFID_ERROR;
                    }
                }
#endif
            }
            if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512 ) {
#ifdef EM_PN512_Module  //PN512
//                gcMifReg.GsNOn = (uchar)rfdata.pwrfield;
//                gcMifReg.CWGsP = (uchar)rfidpara_FM17550_V1EX[value].pwrfield;
                gcMifReg.CWGsP = (uchar)rfdata.pwrfield;
//                EI_mifs_vWriteReg(1, RFCfgReg, &gcMifReg.RFCfg);
#endif
            }
            ret = RFID_SUCCESS;
        }else
            ret = -RFID_ERRPARAM;
    }else if ( para == RFID_PARA_TYPEBMODULE) {
#ifdef EM_AS3911_Module  //AS3911
        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
            as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, value);
            gas3911Reg.gTypeBmodule = value;
        }
#endif
    }else{
		ret = -RFID_ERRPARAM;
	}

	return ret;
}

int adjust_rfid_PwrFieldSetTime(int mode)
{
	int key,iRet = 0,param,tmp = 0;
	int PwrAdTimeback = 0;

    iRet = iRet;PwrAdTimeback = PwrAdTimeback;
	PwrAdTimeback = EG_mifs_tWorkInfo.PwrAdTime;
	while(1)
	{
		lcd_cls();
		lcdDispMultiLang(0,  DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY, "场强建立时间", "PwrField Time");
		lcdDispMultiLang(0,  DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "当前时间:%d", "Cur Time:%d",EG_mifs_tWorkInfo.PwrAdTime);
		lcdDispMultiLang(0,  DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "1-自动调节",  "1-Auto Adjust");
		lcdDispMultiLang(0,  DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "2-手动调节",  "2-Manual Adjust");
		lcdDispMultiLang(0,  DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE, "3-恢复默认",  "3-Default");

		key = kb_getkey(-1);
		switch(key)
		{
    		case KEY1:
//    			iRet = rfid_ParaAdjust(RFID_PARA_PWRSETTIME, 4);
    			break;
    		case KEY2:
				lcd_cls();
				param = EG_mifs_tWorkInfo.PwrAdTime;
				while(1)
				{
                    lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY,
                                     "场强时间设置", "PwrField Set up");
                    lcdDispMultiLang(0,  DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE,
                                     "当前值: %d", "Cur Level: %d", param);
                    lcdDispMultiLang(0,  DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,
                                     "[\x1E/\x1F]: 调节", "[\x1E/\x1F]: Adjust");
                    lcdDispMultiLang(0,  DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,
                                     "[确认]: 保存", "[Enter]: Save");
					key = kb_getkey(-1);
			        switch(key)
			        {
			        case KEY_ENTER:
						iRet = WriteRfid_Para_Set(RFID_PARA_PWRSETTIME, param);
                        iRet = rfid_ParaAdjust(RFID_PARA_PWRSETTIME, param);
						tmp = 1;
						break;

			        case KEY_SYMBOL:
			        case KEY_UP:
						if(param<RFID_MAX_PWRSETTIME)
							param++;
			            break;

			        case KEY_ALPHA:
			        case KEY_DOWN:
						if(param>RFID_DEFAULT_PWRSETTIME)
							param--;
			            break;

					case KEY_CANCEL:
						tmp = 1;
						iRet = 1;
						break;
			        default:
			            break;
			        }

					if(tmp)
					{
						tmp = 0;
						break;
					}
				}
        		break;
            case  KEY3:
                iRet = rfid_ParaAdjust(RFID_PARA_PWRSETTIME, RFID_DEFAULT_PWRSETTIME);
    			iRet = WriteRfid_Para_Set(RFID_PARA_PWRSETTIME, EG_mifs_tWorkInfo.PwrAdTime);
    			break;
			case KEY_CANCEL:
				return 0;
    		default:
    			break;
		}
    }
//    return 0;
}
//调整射频场场强
int adjust_rfid_PwrField(int mode)
{
	int key,iRet = 0,param,tmp = 0;
	int PwrAdback = 0;
    s_rfidpara_info rfdata;

    iRet = iRet;
	iRet = ReadRfid_Para_Get(RFID_PARA_PWR, &param);
	PwrAdback = param;
	while(1)
	{
		lcd_cls();
		lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY, "场强设置", "Field Level Set");
		lcdDispMultiLang(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "当前场强等级:%d(0x%02X)", "Field Level:%d(0x%02X)",PwrAdback,gtRfidDebugInfo.CW_A);
		lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "1-手动设置",  "1-Adjust");
		lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "2-恢复默认",  "2-Default");
		key = kb_getkey(-1);
		switch(key)
		{
    		case KEY1:
				lcd_cls();
				while(1)
				{
                    s_rfid_getPara(EG_mifs_tWorkInfo.RFIDModule, RFID_PARA_PWR, param, &rfdata);
                    lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY,
                                     "场强等级设置", "Field Level Set");
                    lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE,
                                     "当前场强等级:%d(0x%02X)", "Field Level:%d(0x%02X)", param,rfdata.pwrfield);
                    lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,
                                     "[\x1E/\x1F]: 调节", "[\x1E/\x1F]: Adjust");
                    lcdDispMultiLang(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,
                                     "[确认]: 保存", "[Enter]: Save");
					key = kb_getkey(-1);
			        switch(key)
			        {
			        case KEY_ENTER:
                        rfid_ParaAdjust(RFID_PARA_PWR, param);
						iRet = WriteRfid_Para_Set(RFID_PARA_PWR, param);
						tmp = 1;
                        PwrAdback = param;
						break;

			        case KEY_SYMBOL:
			        case KEY_UP:
						if(param<RFID_MAX_PWR)
							param++;
			            break;

			        case KEY_ALPHA:
			        case KEY_DOWN:
						if(param>RFID_PWR_LEVEL0)
							param--;
			            break;

					case KEY_CANCEL:
						tmp = 1;
						iRet = 1;
						break;
			        default:
			            break;
			        }
					if(tmp)
					{
						tmp = 0;
						break;
					}
				}
        		break;
            case  KEY2:
                param = RFID_DEFAULT_PWR;
                PwrAdback = param;
                iRet = rfid_ParaAdjust(RFID_PARA_PWR, param);
    			iRet = WriteRfid_Para_Set(RFID_PARA_PWR, param);
    			break;
			case KEY_CANCEL:
				return 0;
    		default:
    			break;
		}
    }
//    return 0;
}

//调整调制深度
int adjust_rfid_TypeBModule(int mode)
{
#ifdef EM_AS3911_Module  //AS3911
	int key,param;
    char databuf[16];

    if ( EG_mifs_tWorkInfo.RFIDModule != RFID_Module_AS3911 ) {
        //其他芯片暂时不支持设置
        return 0;
    }
	ReadRfid_Para_Get(RFID_PARA_TYPEBMODULE, &param);
	while(1)
	{
		lcd_cls();
		lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY, "调制设置", "Module Level Set");
		lcdDispMultiLang(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "当前调制值:0x%02X %02X", "Module Level:0x%02X %02X",gas3911Reg.gTypeBmodule,param);
		lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "1-手动设置",  "1-Adjust");
		lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "2-恢复默认",  "2-Default");
		key = kb_getkey(-1);
		switch(key)
		{
    		case KEY1:
				lcd_cls();
                lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY,
                                 "调制设置", "Module Level Set");
                lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE,
                                 "当前调制值:0x%02X", "Module Level:0x%02X", param);
                //                    lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,
                //                                     "[\x1E/\x1F]: 调节", "[\x1E/\x1F]: Adjust");
                lcdDispMultiLang(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,
                                 "设置值确认保存", "Set and Save");
                //					key = kb_getkey(-1);
                memset(databuf, 0, sizeof(databuf));
                lcd_goto(0,DISP_FONT_LINE5);
                key = kb_getstr(KB_SMALL_NUM, 1, 3, -1, (char *)databuf);
                TRACE("\r\n---key:%x %d",key,key);
                if ( key == -KB_CANCEL ) {
                    break;
                }
                param = (int)atoi((char const *)databuf);
                rfid_ParaAdjust(RFID_PARA_TYPEBMODULE, param);
                WriteRfid_Para_Set(RFID_PARA_TYPEBMODULE, param);
        		break;
            case  KEY2:
                param = AS3911_TYPEB_MODULATION;
                rfid_ParaAdjust(RFID_PARA_TYPEBMODULE, param);
    			WriteRfid_Para_Set(RFID_PARA_TYPEBMODULE, param);
    			break;
			case KEY_CANCEL:
				return 0;
    		default:
    			break;
		}
    }
#else
    return 0;
#endif
}
//int rfid_type_menu(int mode)
//{
//    const char *name[] = {"RC531","FM17550","RC663","AS3911","未知"};
//    uint i;
//    i = EG_mifs_tWorkInfo.RFIDModule;
//    if ( i == 0 || i >= DIM(name) ) {
//        i = DIM(name)-1;
//    } else {
//        --i;
//    }
//    lcd_cls();
//    lcdDispMultiLang(0, DISP_FONT_LINE0,DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY, "芯片类型", "CHIP",name[i]);
//    lcd_display(0, DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE, "%s", name[i]);
//    kb_getkey(-1);
//    return 0;
//}
int rfid_para2offset(int type, int *offset, int *len)
{
    int i;
    switch ( type )
    {
    case RFID_PARA_PWRSETTIME :
        i = FPOS(MODULE_RFID_INFO,rfid_powerfield_time);
        *len = FSIZE(MODULE_RFID_INFO,rfid_powerfield_time);
        break;
    case RFID_PARA_PWR :
        i = FPOS(MODULE_RFID_INFO,field_strength);
        *len = FSIZE(MODULE_RFID_INFO,field_strength);
        break;
    case RFID_PARA_CHIP :
        i = FPOS(MODULE_RFID_INFO,type);
        *len = FSIZE(MODULE_RFID_INFO,type);
        break;
    case RFID_PARA_TYPEBMODULE :
        i = FPOS(MODULE_RFID_INFO,gTypeBmodule);
        *len = FSIZE(MODULE_RFID_INFO,gTypeBmodule);
        break;
    default :
        return -1;
        break;
    }
    *offset = i+FPOS(SYSZONE_DEF,trfidinfo);
    return 0;
}
int WriteRfid_Para_Set(int paramIndex, int param)
{
    int ret;
    int offset,len;
    ret = rfid_para2offset(paramIndex,&offset,&len);
    if ( ret ) {
        return ret;
    }
    return s_sysconfig_write(offset,len,&param);
}

int ReadRfid_Para_Get(int paramIndex, int *param)
{
    int ret;
    int offset,len;
    ret = rfid_para2offset(paramIndex,&offset,&len);
    if ( ret ) {
        return ret;
    }
    ret = s_sysconfig_read(offset,len,param);
//    if ( !ret ) {
//        if ( *param > RFID_PWR_LEVEL3) {
//            *param = RFID_DEFAULT_PWR;
//        }
//    }
    return ret;
}

#endif


