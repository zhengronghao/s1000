/*********************************************************************
* 版权所有 (C)2003, 福建实达电脑设备有限公司。
* 文件名称： 
*   MifsCmd.c
* 当前版本： 
*   01-01-01
* 内容摘要： 
*   本文件负责实现非接触卡命令层的程序
* 历史纪录：
*   修改人      日期                版本号      修改记录
******************************************************************/
#include "wp30_ctrl.h"

#ifdef CFG_RFID
/**********************************************************************

                          本地常量定义
                          
***********************************************************************/
#define EM_mifs_DEFAULT_WATER_LEVEL     (uchar)0x20	// 缺省的通知阀值

/**********************************************************************

                          全局函数定义
                          
***********************************************************************/
ET_mifs_WORKSTRUCT EG_mifs_tWorkInfo UPAREA;	// 工作变量结构

//_t_RfidPn512_Info_ gtPN512Info INIRAM;

_t_RfidPro_Info_ gtRfidProInfo UPAREA;

MIFARECARD gMifareData UPAREA;
/**********************************************************************
* 函数名称： 
*   uchar EI_mifs_ucHandleCmd(uchar cmd)
* 功能描述： 
*   完成命令请求和应答处理
* 输入参数： 
*   cmd: 命令代码  
* 输出参数：
*   无
* 返回值： 
*   命令执行的结果
*   EM_SUCCESS                  成功
*   EM_mifs_UNKNOWN_COMMAND     未知命令
*   EM_mifs_COLLERR             冲突
*   EM_mifs_PARITYERR           奇偶校验错
*   EM_mifs_FRAMINGERR          帧错误
*   EM_mifs_OVFLERR             FIFO溢出
*   EM_mifs_CRCERR              CRC校验错
*   EM_mifs_NOTAGERR            操作范围内无卡
*   EM_ERROR                    错误的执行结果
*   EM_TIMEOUT                  处理超时
*   EM_mifs_RECBUF_OVERFLOW     接收缓冲区溢出
* 历史纪录：
*   修改人      日期                版本号      修改记录
***********************************************************************/
uint if_timerout(uint timer0,uint timerout)
{
	uint timer1=sys_get_counter();
	if(timer1 >= timer0)
	{
		if(timer1 - timer0 >= timerout)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		timer0 = UINT_MAX - timer0;
		if(timer0+timer1 >= timerout)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}
#define  OptRFID_TIMEOUT  6000
uint get_timeout(ulong FWT)
{
	// FWT*128/13.56 us = FWT*128/(13.56*10^3) ms = FWT*128/13560 = FWT*16/1695 ms
	uint timeout = (uint)FWT*16/1695;
	D2(TRACE("\r\n FWT:%d-%d timeout:%d",EG_mifs_tWorkInfo.ucFWI,EG_mifs_tWorkInfo.ulFWT,timeout););
	if(timeout < OptRFID_TIMEOUT)
	{
		timeout = OptRFID_TIMEOUT;
	}	
	return (timeout+1);
}
/**********************************************************************
* 函数名称：
*   uchar EI_mifs_ucHandleCmd(uchar cmd)
* 功能描述：
*   完成命令请求和应答处理
* 输入参数：
*   cmd: 命令代码
* 输出参数：
*   无
* 返回值：
*   命令执行的结果
*   EM_SUCCESS                  成功
*   EM_mifs_UNKNOWN_COMMAND     未知命令
*   EM_mifs_COLLERR             冲突
*   EM_mifs_PARITYERR           奇偶校验错
*   EM_mifs_FRAMINGERR          帧错误
*   EM_mifs_OVFLERR             FIFO溢出
*   EM_mifs_CRCERR              CRC校验错
*   EM_mifs_NOTAGERR            操作范围内无卡
*   EM_ERROR                    错误的执行结果
*   EM_TIMEOUT                  处理超时
*   EM_mifs_RECBUF_OVERFLOW     接收缓冲区溢出
* 历史纪录：
*   修改人      日期                版本号      修改记录
***********************************************************************/

uchar EI_mifs_ucHandleCmd(uchar cmd)
{
	uchar ucTempData;					// 暂存要操作的寄存器数据
	uchar ucWaterLevelBak;				// 备份FIFO的WaterLevel值
	uchar ucTimerCtl;					// 命令中用到的定时器控制方式
	uchar ucIntWait;					// 命令结束时期待发生的中断
    uchar ucIntWait1 = 0;               // CLRC663定时器相关操作
	uchar ucIntEnable;					// 该命令允许发生的中断
	uchar ucrxMultiple;					// 是否要接收多个帧
	uint timer0,timeout;
	D1( LABLE(0xCC);
		DATAIN(cmd);
		DATAIN(LBYTE(EG_mifs_tWorkInfo.ulSendBytes));
		for (guiDebugj = 0; guiDebugj < EG_mifs_tWorkInfo.ulSendBytes; guiDebugj++)
		{
			DATAIN(EG_mifs_tWorkInfo.aucBuffer[guiDebugj]);
		}
		//DISPBUF(EG_mifs_tWorkInfo.aucBuffer,EG_mifs_tWorkInfo.ulSendBytes,0);
	);
		
	//timeout=get_timeout(EG_mifs_tWorkInfo.ulFWT);	

	timeout=OptRFID_TIMEOUT;
	
	// 给工作变量赋初始值
	EG_mifs_tWorkInfo.ucCurResult = EM_SUCCESS;
	EG_mifs_tWorkInfo.ucINTSource = 0;
	EG_mifs_tWorkInfo.TimeSource = 0;  //CLRC663 timer flag
	EG_mifs_tWorkInfo.ulBytesSent = 0;	// how many bytes already sent
	EG_mifs_tWorkInfo.ulBytesReceived = 0;	// how many bytes received
	EG_mifs_tWorkInfo.lBitsReceived = 0;	// how many bits received
	EG_mifs_tWorkInfo.ucErrFlags = 0;
	EG_mifs_tWorkInfo.ucSaveErrState = 0x00;
	EG_mifs_tWorkInfo.ucCollPos = 0x00;
	ucrxMultiple = 0x00;
	ucIntEnable = 0x00;
	ucIntWait = 0x00;
	ucTimerCtl = 0x02;
    ucTempData = 0;
    ucWaterLevelBak = 0;

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;
        ucWaterLevelBak = ucWaterLevelBak;
        ucTimerCtl = ucTimerCtl;
        ucIntWait = ucIntWait;
        ucIntEnable = ucIntEnable;
        ucrxMultiple = ucrxMultiple;
        timeout = timeout;

        int error;
        timer0 = 0;
        error = emvHalTransceive((u8 *)EG_mifs_tWorkInfo.aucBuffer, EG_mifs_tWorkInfo.ulSendBytes, 
                                        EG_mifs_tWorkInfo.aucBuffer, EG_mifs_tWorkInfo.expectMaxRec, (size_t *)&timer0, 
                                        EG_mifs_tWorkInfo.ulTimeout, (EmvHalTransceiveMode_t)cmd);
        EG_mifs_tWorkInfo.ucCurResult = s_as3911errorCode(error);

#endif

    }
    else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
    {
        ucIntWait1 = ucIntWait1;
#ifdef EM_RC663_Module
        // 禁止并清除所有中断源
        s_rfid_setRC663Int(0,RC663_IRQ_ALL);
        s_rfid_setRC663IntFlg(0,RC663_IRQ_ALL);
//        s_rfid_setRC663Int(1,RC663_IRQ_GLOBAL); //全局中断使能

        // 清空FIFO缓冲区
        EI_mifs_vFlushFIFO();

        // 结束当前正在运行的命令
        ucTempData = PHHAL_HW_RC663_CMD_IDLE;
        EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_COMMAND, 0x1F);
//        EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_COMMAND, ucTempData);

        // 设置FIFO警告值
        EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_WATERLEVEL, &ucWaterLevelBak);
        ucTempData = RFID_DEFAULT_WATER_LEVEL;
        EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_WATERLEVEL, &ucTempData);
        // 根据不同命令进行处理
        switch (cmd)
        {
        case PHHAL_HW_RC663_CMD_IDLE:
            {
                // 空闲
                ucTimerCtl = 0x00;			//不使用定时器
                ucIntEnable = RC663_IRQ_NONE;			//不允许中断
                ucIntWait = RC663_IRQ_NONE;			//不等待中断
                break;

            }
        case PHHAL_HW_RC663_CMD_TRANSCEIVE:
            {
                // 接收数据
                // 根据接收起始位的位置来计算实际收到位数
                EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_RXBITCTRL, &ucTempData);
                ucTempData &= 0x7F;
                EG_mifs_tWorkInfo.lBitsReceived = -(long)(ucTempData >> 4);
                //TRACE("\r\nEG_mifs_tWorkInfo.lBitsReceived:%x       ucTempData:%x",EG_mifs_tWorkInfo.lBitsReceived,ucTempData);
                ucTimerCtl = PHHAL_HW_RC663_BIT_TSTOP_RX | PHHAL_HW_RC663_BIT_TSTART_TX | PHHAL_HW_RC663_VALUE_TCLK_T0;	// 数据发送完毕时定时器自动开始
                ucIntEnable = (uchar)RC663_IRQ_ALL;			//attention CLRC663 timer
                ucIntWait1 = (uchar)(RC663_IRQ_TIMER1>>8);
                ucIntWait = RC663_IRQ_IDLE | RC663_IRQ_RX;			// IdleIRq
                break;
            }

        default:
            {
                //非法命令
                EG_mifs_tWorkInfo.ucCurResult = EM_mifs_UNKNOWN_COMMAND;
                break;
            }
        }									// switch(cmd)


        if (EG_mifs_tWorkInfo.ucCurResult == EM_SUCCESS)
        {
            // 设置为多帧接收

            EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_RXCTRL, &ucTempData);  //
            ucrxMultiple = ucTempData & 0x40;
            if (ucrxMultiple == 0)
            {
                //EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCTRL, 0x40);  //开启多帧接收会 多收一个ErrorReg寄存器的值
            }

            //有使用定时器
            if ( ucTimerCtl ) {
                // 设置定时器控制方式
                ucTempData = PHHAL_HW_RC663_BIT_TAUTORESTARTED | PHHAL_HW_RC663_BIT_TSTART_TX | PHHAL_HW_RC663_VALUE_TCLK_1356_MHZ;
                EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_T0CONTROL, &ucTempData);//发送启动 自动重载
                EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_T1CONTROL, &ucTimerCtl);
                // 如果定时器要人工启动，就启动它
                if ((ucTimerCtl & 0x30) == 0)
                {
                    //POINT(ucTimerCtl,99);
                    ucTempData = PHHAL_HW_RC663_BIT_T0STARTSTOPNOW | PHHAL_HW_RC663_BIT_T0RUNNING | PHHAL_HW_RC663_BIT_T1STARTSTOPNOW | PHHAL_HW_RC663_BIT_T1RUNNING;
                    EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TCONTROL, &ucTempData);
                }
            }
            //D2(TRACE("\r\n IntWait:%2X IntEnable:%2X",ucIntWait,ucIntEnable););
            // 启动该命令
//            TRACE("\r\n%x--data:%x",gtRfidProInfo.Cmd,EG_mifs_tWorkInfo.aucBuffer[0]);
            if ( EG_mifs_tWorkInfo.FIFOSIZE == 512 ) {
                EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_FIFOCONTROL, &ucTimerCtl);
                ucTimerCtl &= 0x03;
                EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_FIFOLENGTH, &ucTempData);
                timer0 = EG_mifs_tWorkInfo.FIFOSIZE - RFID_DEFAULT_WATER_LEVEL - (ushort)((ucTimerCtl<<8)|ucTempData) - 1;
            }else{
                //255
                EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_FIFOLENGTH, &ucTempData);
                timer0 = EG_mifs_tWorkInfo.FIFOSIZE - RFID_DEFAULT_WATER_LEVEL - ucTempData - 1;
            }
//            D2(TRACE("\r\n buf len:%d",timer0););
            if ((EG_mifs_tWorkInfo.ulSendBytes - EG_mifs_tWorkInfo.ulBytesSent) <= timer0)
            {
                timer0 = EG_mifs_tWorkInfo.ulSendBytes - EG_mifs_tWorkInfo.ulBytesSent;
            }
//            D2(TRACE("\r\n fifo size:%d--data len:%d",EG_mifs_tWorkInfo.FIFOSIZE,timer0););
//            D2(LABLE(0xCC);DATAIN(LBYTE(timer0)););	
            if(timer0)
            {
                //第一帧数据在前台发送 
                //由于第一帧若数据量很少会马上发送完毕lo标志会再次产生!
                //数据量多时未做实验 注意此时后台还是否会再发送！！
                EI_mifs_vWriteReg(timer0, PHHAL_HW_RC663_REG_FIFODATA,
                                  EG_mifs_tWorkInfo.aucBuffer + EG_mifs_tWorkInfo.ulBytesSent);
                EG_mifs_tWorkInfo.ulBytesSent += timer0;
//                EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_COMMAND, 0x1F); //方法1:在lo中断后fifo填充数据后再送cmd也可以
//                EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_COMMAND, cmd);
            }			
//            EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_FIFOLENGTH, &ucTempData);
//            D2(TRACE("\r\n len:%d",ucTempData););
            cmd &= 0x1F;
            EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_COMMAND, 0x1F);
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_COMMAND, cmd);
            // 将上面屏蔽的代码转移到命令发送完成后，避免因为其它中断或者优先级高的任务导致命令发送操作没有执行而使操作失败
            // 总是允许定时器超时中断,并且等待这种中断

//            s_rfid_setRC663IntFlg(0,RC663_IRQ_ALL);
//			EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_IRQ0, &ucTempData);		
//            D2(TRACE("\r\n irq flag:%x",ucTempData););
            s_rfid_setRC663Int(1, (uint)(ucIntEnable|RC663_IRQ_TIMER1|RC663_IRQ_GLOBAL));
            // 等待期望的中断请求发生，这样就可以结束命令
            // 如果超过6秒没有处理，则返回错误
            timer0 = sys_get_counter();
            while (1)
            {
                if(((EG_mifs_tWorkInfo.ucINTSource & ucIntWait) != 0)
                   ||(EG_mifs_tWorkInfo.TimeSource & ucIntWait1) != 0) 
                {
                    break;
                }
                if(if_timerout(timer0,timeout))
                {
                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_NOTAGERR;
                    D2(LABLE(0x22););
                    break;
                }

            }

            // 如果是写EEPROM操作, 必须等待结束
            /*
               if (cmd == EM_mifs_PCD_WRITEE2)
               {
               while (1)
               {
               EI_mifs_vReadReg(1, EM_mifs_REG_SECONDARYSTATUS, &ucTempData);
               if ((ucTempData & 0x40) == 0x40)
               {
               break;
               }
               if(if_timerout(timer0,OptRFID_TIMEOUT))
               {
            // 超时
            EG_mifs_tWorkInfo.ucCurResult = EM_TIMEOUT;
            EG_mifs_tWorkInfo.ucCollPos = 0;
            break;
            }
            }
            }
            */
            //命令结束处理
            s_rfid_setRC663Int(0, RC663_IRQ_ALL); // 禁止所有中断
            s_rfid_setRC663IntFlg(0, RC663_IRQ_ALL); // 清除所有中断请求位

            ucTempData = PHHAL_HW_RC663_BIT_T0RUNNING | PHHAL_HW_RC663_BIT_T1RUNNING;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TCONTROL, &ucTempData);  //stop T0/T1

            // 返回到IDLE模式
            ucTempData = PHHAL_HW_RC663_CMD_IDLE;
            EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_COMMAND, 0x1F);
//            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_COMMAND, ucTempData);

            // 根据命令去掉一些不用处理的错误
//            if ((cmd == EM_mifs_PCD_WRITEE2) || (cmd == EM_mifs_PCD_READE2)
//                || (cmd == EM_mifs_PCD_LOADKEYE2) || (cmd == EM_mifs_PCD_LOADKEY))
//            {
//                // 只处理缓冲区溢出
//                // 主要是因为这4个命令只涉及到CPU与RC531之间的通信
//                EG_mifs_tWorkInfo.ucSaveErrState &= PN512_OVFLERR;
//            }
//            else
//            {
//                // 只处理缓冲区溢出、冲突错误、奇偶错误、帧错误、CRC错误
//                EG_mifs_tWorkInfo.ucSaveErrState &= PN512_ALLERR;
//            }
            // 返回错误
            if (EG_mifs_tWorkInfo.ucSaveErrState != 0)
            {
                if ((EG_mifs_tWorkInfo.ucSaveErrState & RC663_COLLERR) != 0)
                {
                    // 冲突
                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_COLLERR;
                }
//                else if ((EG_mifs_tWorkInfo.ucSaveErrState & PN512_PARITYERR) != 0)
//                {
//                    // 奇偶校验错
//                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_PARITYERR;
//                }
                else if ((EG_mifs_tWorkInfo.ucSaveErrState & RC663_FRAMINGERR) != 0)
                {
                    // 帧错误
                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_FRAMINGERR;
                }
                else if ((EG_mifs_tWorkInfo.ucSaveErrState & RC663_CRCERR) != 0)
                {
                    // CRC错误
                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_CRCERR;
                }
                else if ((EG_mifs_tWorkInfo.ucSaveErrState & RC663_OVFLERR) != 0)
                {
                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_OVFLERR;
                }
//                else if (EG_mifs_tWorkInfo.ucCurResult == EM_SUCCESS)
                else
                {
                    EG_mifs_tWorkInfo.ucCurResult = EM_ERROR;
                }
            }
            // 恢复单帧接收设置

            if (ucrxMultiple == 0)
            {
                //EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCTRL, 0x40);
            }

        }									// if (EG_mifs_tWorkInfo.ucCurResult == EM_SUCCESS)

        // 恢复先前WaterLevel设置
        EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_WATERLEVEL, &ucWaterLevelBak);

#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{
	
#ifdef EM_PN512_Module

		// 禁止并清除所有中断源
		s_rfid_setRC531Int(0,PN512_IRQ_ALL);
		s_rfid_setRC531IntFlg(0,PN512_IRQ_ALL);

		// 清空FIFO缓冲区
		EI_mifs_vFlushFIFO();
			
		// 结束当前正在运行的命令
		ucTempData = PN512CMD_IDLE;
		ucTempData &= 0x0F;
		EI_mifs_vClrBitMask(CommandReg, 0x0F);
		EI_mifs_vSetBitMask(CommandReg, ucTempData);

		// 设置FIFO警告值
		EI_mifs_vReadReg(1, WaterLevelReg, &ucWaterLevelBak);
		ucTempData = RFID_DEFAULT_WATER_LEVEL;
		EI_mifs_vWriteReg(1, WaterLevelReg, &ucTempData);
		// 根据不同命令进行处理
		switch (cmd)
		{
		case PN512CMD_IDLE:
			{
				// 空闲
				ucTimerCtl = 0x00;			//不使用定时器
				ucIntEnable = PN512_IRQ_NONE;			//不允许中断
				ucIntWait = PN512_IRQ_NONE;			//不等待中断
				break;
			
			}
		case PN512CMD_TRANSCEIVE:
			{
				// 接收数据
				// 根据接收起始位的位置来计算实际收到位数
				EI_mifs_vReadReg(1, BitFramingReg, &ucTempData);
				ucTempData &= 0x7F;
				EG_mifs_tWorkInfo.lBitsReceived = -(long)(ucTempData >> 4);
				//TRACE("\r\nEG_mifs_tWorkInfo.lBitsReceived:%x       ucTempData:%x",EG_mifs_tWorkInfo.lBitsReceived,ucTempData);
				ucTimerCtl = 0x80;			// 数据发送完毕时定时器自动开始
				ucIntEnable = PN512_IRQ_ALL;			// TxIrq, RxIrq, IdleIRq and LoAlert
				ucIntWait = PN512_IRQ_IDLE | PN512_IRQ_RX;			// IdleIRq
				break;
			}
			
		default:
			{
				//非法命令
				EG_mifs_tWorkInfo.ucCurResult = EM_mifs_UNKNOWN_COMMAND;
				break;
			}
		}									// switch(cmd)


		if (EG_mifs_tWorkInfo.ucCurResult == EM_SUCCESS)
		{
			// 设置为多帧接收
			
			EI_mifs_vReadReg(1, RxModeReg, &ucTempData);  //多帧接收在106k/s 时无效
			ucrxMultiple = ucTempData & 0x04;
			if (ucrxMultiple == 0)
			{
				//EI_mifs_vSetBitMask(RxModeReg, 0x04);  //开启多帧接收会 多收一个ErrorReg寄存器的值
			}
			
			// 设置定时器控制方式
			EI_mifs_vSetBitMask(TModeReg, ucTimerCtl);
			// 如果定时器要人工启动，就启动它
			if ((ucTimerCtl & 0x80) == 0)
			{
				//POINT(ucTimerCtl,99);
				EI_mifs_vSetBitMask(ControlReg, 0x40); 
			}
			//D2(TRACE("\r\n IntWait:%2X IntEnable:%2X",ucIntWait,ucIntEnable););
			// 启动该命令
			cmd &= 0x0F;
			EI_mifs_vReadReg(1, CommandReg, &ucTempData);
			ucTempData &= 0xF0;
			ucTempData |= cmd;
			EI_mifs_vWriteReg(1, CommandReg, &ucTempData);
			
			timer0 = sys_get_counter();
			// 将上面屏蔽的代码转移到命令发送完成后，避免因为其它中断或者优先级高的任务导致命令发送操作没有执行而使操作失败
			// 总是允许定时器超时中断,并且等待这种中断
			ucIntEnable |= PN512_IRQ_TIMER;
			ucIntWait |= PN512_IRQ_TIMER;

	//TI_ShowPN512Reg();
		
			s_rfid_setRC531Int(1, ucIntEnable);
			// 等待期望的中断请求发生，这样就可以结束命令
			// 如果超过10秒没有处理，则返回错误
			while (1)
			{
				if ((EG_mifs_tWorkInfo.ucINTSource & ucIntWait) != 0)
				{
					break;
				}
				if(if_timerout(timer0,timeout))
				{
					EG_mifs_tWorkInfo.ucCurResult = EM_mifs_NOTAGERR;
					D2(LABLE(0x22););
					break;
				}
			
			}
			
			// 如果是写EEPROM操作, 必须等待结束
			/*
			if (cmd == EM_mifs_PCD_WRITEE2)
			{
				while (1)
				{
					EI_mifs_vReadReg(1, EM_mifs_REG_SECONDARYSTATUS, &ucTempData);
					if ((ucTempData & 0x40) == 0x40)
					{
						break;
					}
					if(if_timerout(timer0,OptRFID_TIMEOUT))
					{
						// 超时
						EG_mifs_tWorkInfo.ucCurResult = EM_TIMEOUT;
						EG_mifs_tWorkInfo.ucCollPos = 0;
						break;
					}
				}
			}
			*/
			//命令结束处理
			s_rfid_setRC531Int(0, PN512_IRQ_ALL); // 禁止所有中断
			s_rfid_setRC531IntFlg(0, PN512_IRQ_ALL); // 清除所有中断请求位
			
			EI_mifs_vSetBitMask(ControlReg, 0x80);	// 停止定时器
			EI_mifs_vClrBitMask(BitFramingReg, 0x80);  //停止发送
		
			// 返回到IDLE模式
			ucTempData = PN512CMD_IDLE;
			ucTempData &= 0x0F;
			EI_mifs_vClrBitMask(CommandReg, 0x0F);
			EI_mifs_vSetBitMask(CommandReg, ucTempData);

			// 根据命令去掉一些不用处理的错误
			if ((cmd == EM_mifs_PCD_WRITEE2) || (cmd == EM_mifs_PCD_READE2)
				|| (cmd == EM_mifs_PCD_LOADKEYE2) || (cmd == EM_mifs_PCD_LOADKEY))
			{
				// 只处理缓冲区溢出
				// 主要是因为这4个命令只涉及到CPU与RC531之间的通信
				EG_mifs_tWorkInfo.ucSaveErrState &= PN512_OVFLERR;
			}
			else
			{
				// 只处理缓冲区溢出、冲突错误、奇偶错误、帧错误、CRC错误
				EG_mifs_tWorkInfo.ucSaveErrState &= PN512_ALLERR;
			}
			// 返回错误
			if (EG_mifs_tWorkInfo.ucSaveErrState != 0)
			{
				if ((EG_mifs_tWorkInfo.ucSaveErrState & PN512_COLLERR) != 0)
				{
					// 冲突
					EG_mifs_tWorkInfo.ucCurResult = EM_mifs_COLLERR;
				}
				else if ((EG_mifs_tWorkInfo.ucSaveErrState & PN512_PARITYERR) != 0)
				{
					// 奇偶校验错
					EG_mifs_tWorkInfo.ucCurResult = EM_mifs_PARITYERR;
				}
				else if ((EG_mifs_tWorkInfo.ucSaveErrState & PN512_FRAMINGERR) != 0)
				{
					// 帧错误
					EG_mifs_tWorkInfo.ucCurResult = EM_mifs_FRAMINGERR;
				}
				else if ((EG_mifs_tWorkInfo.ucSaveErrState & PN512_CRCERR) != 0)
				{
					// CRC错误
					EG_mifs_tWorkInfo.ucCurResult = EM_mifs_CRCERR;
				}
				else if ((EG_mifs_tWorkInfo.ucSaveErrState & PN512_OVFLERR) != 0)
				{
					EG_mifs_tWorkInfo.ucCurResult = EM_mifs_OVFLERR;
				}
				else if (EG_mifs_tWorkInfo.ucCurResult == EM_SUCCESS)
				{
					EG_mifs_tWorkInfo.ucCurResult = EM_ERROR;
				}
			}
			// 恢复单帧接收设置
			
			if (ucrxMultiple == 0)
			{
				//EI_mifs_vClrBitMask(RxModeReg, 0x04);
			}
			
		}									// if (EG_mifs_tWorkInfo.ucCurResult == EM_SUCCESS)

		// 恢复先前WaterLevel设置
		EI_mifs_vWriteReg(1, WaterLevelReg, &ucWaterLevelBak);
	
#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module  //RC531

        // 禁止并清除所有中断源 
        s_rfid_setRC531Int(0,RC531_IRQ_ALL);
        s_rfid_setRC531IntFlg(0,RC531_IRQ_ALL);

        // 清空FIFO缓冲区
        EI_mifs_vFlushFIFO();

        // 结束当前正在运行的命令
        ucTempData = EM_mifs_PCD_IDLE;
        EI_mifs_vWriteReg(1, EM_mifs_REG_COMMAND, &ucTempData);

        // 设置FIFO警告值
        EI_mifs_vReadReg(1, EM_mifs_REG_FIFOLEVEL, &ucWaterLevelBak);
        ucTempData = RFID_DEFAULT_WATER_LEVEL;
        EI_mifs_vWriteReg(1, EM_mifs_REG_FIFOLEVEL, &ucTempData);
        // 根据不同命令进行处理
        switch (cmd)
        {
        case EM_mifs_PCD_IDLE:
            {
                // 空闲
                ucTimerCtl = 0x02;			//不使用定时器
                ucIntEnable = RC531_IRQ_NONE;			//不允许中断
                ucIntWait = RC531_IRQ_NONE;			//不等待中断
                break;
            }
        case EM_mifs_PCD_WRITEE2:
            {
                // 写EEPROM
                ucTimerCtl = 0x00;			// 手动启动和停止定时器
                ucIntEnable = RC531_IRQ_LO|RC531_IRQ_TX;			// LoAlert and TxIRq
                ucIntWait = RC531_IRQ_TX;			// TxIRq
                break;
            }
        case EM_mifs_PCD_READE2:
            {
                // 读EEPROM
                ucTimerCtl = 0x00;			// 手动启动和停止定时器
                ucIntEnable = RC531_IRQ_LO|RC531_IRQ_HI|RC531_IRQ_IDLE; // HiAlert, LoAlert and IdleIRq
                ucIntWait = RC531_IRQ_IDLE;			// IdleIRq
                break;
            }
        case EM_mifs_PCD_LOADKEYE2:
            {
                //从EEPROM载入密钥
                ucTimerCtl = 0x00;			// 手动启动和停止定时器
                ucIntEnable = RC531_IRQ_LO|RC531_IRQ_IDLE;			// IdleIRq and LoAlert
                ucIntWait = RC531_IRQ_IDLE;			// IdleIRq
                break;
            }
        case EM_mifs_PCD_LOADCONFIG:
            {
                // 载入配置
                ucTimerCtl = 0x00;			// 手动启动和停止定时器 
                ucIntEnable = RC531_IRQ_LO|RC531_IRQ_IDLE;			// IdleIRq and LoAlert
                ucIntWait = RC531_IRQ_IDLE;			// IdleIRq
                break;
            }
        case EM_mifs_PCD_AUTHENT1:
            {
                //认证步骤1
                ucTimerCtl = 0x02;			// 数据发送完毕时定时器自动开始
                ucIntEnable = RC531_IRQ_LO|RC531_IRQ_IDLE;			// IdleIRq and LoAlert
                ucIntWait = RC531_IRQ_IDLE;			// IdleIRq
                break;
            }
        case EM_mifs_PCD_CALCCRC:
            {
                //CRC校验
                ucTimerCtl = 0x00;			// 手动启动和停止定时器             
                ucIntEnable = RC531_IRQ_LO|RC531_IRQ_TX;			// LoAlert and TxIRq
                ucIntWait = RC531_IRQ_TX;			// TxIRq
                break;
            }
        case EM_mifs_PCD_AUTHENT2:
            {
                //认证步骤2
                ucTimerCtl = 0x02;			// 数据发送完毕时定时器自动开始
                ucIntEnable = RC531_IRQ_IDLE;			// IdleIRq
                ucIntWait = RC531_IRQ_IDLE;			// IdleIRq
                break;
            }
        case EM_mifs_PCD_RECEIVE:
            {
                //接收数据
                //根据接收起始位的位置来计算实际收到位数
                EI_mifs_vReadReg(1, EM_mifs_REG_BITFRAMING, &ucTempData);
                EG_mifs_tWorkInfo.lBitsReceived = -(long)(ucTempData >> 4);

                ucTimerCtl = 0x04;			//手动开始, 数据接收完毕时定时器自动停止
                ucIntEnable = RC531_IRQ_HI|RC531_IRQ_IDLE|RC531_IRQ_RX;			// HiAlert and IdleIRq
                ucIntWait = RC531_IRQ_IDLE|RC531_IRQ_RX;			// IdleIRq
                break;
            }
        case EM_mifs_PCD_LOADKEY:
            {
                //从FIFO缓冲区载入密钥
                ucTimerCtl = 0x00;			// 手动启动和停止定时器             
                ucIntEnable = RC531_IRQ_LO|RC531_IRQ_IDLE;			// LoAlert and IdleIRq
                ucIntWait = RC531_IRQ_IDLE;			// IdleIRq
                break;
            }
        case EM_mifs_PCD_TRANSMIT:
            {
                //发送数据
                ucTimerCtl = 0x00;			// 手动启动和停止定时器          
                ucIntEnable = RC531_IRQ_LO|RC531_IRQ_IDLE;			// LoAlert and IdleIRq
                ucIntWait = RC531_IRQ_IDLE;			// IdleIRq
                break;
            }
        case EM_mifs_PCD_TRANSCEIVE:
            {
                // 接收数据
                // 根据接收起始位的位置来计算实际收到位数
                EI_mifs_vReadReg(1, EM_mifs_REG_BITFRAMING, &ucTempData);
                EG_mifs_tWorkInfo.lBitsReceived = -(long)(ucTempData >> 4);
                ucTimerCtl = 0x06;			// 数据发送完毕时定时器自动开始
                ucIntEnable = RC531_IRQ_ALL;			// TxIrq, RxIrq, IdleIRq and LoAlert
                ucIntWait = RC531_IRQ_IDLE | RC531_IRQ_RX;			// IdleIRq
                break;
            }
        default:
            {
                //非法命令
                EG_mifs_tWorkInfo.ucCurResult = EM_mifs_UNKNOWN_COMMAND;
                break;
            }
        }									// switch(cmd)

        // 刚才没有错误
        if (EG_mifs_tWorkInfo.ucCurResult == EM_SUCCESS)
        {
            // 设置为多帧接收
            EI_mifs_vReadReg(1, EM_mifs_REG_DECODERCONTROL, &ucTempData);
            ucrxMultiple = ucTempData & 0x40;
            if (ucrxMultiple == 0)
            {
                EI_mifs_vSetBitMask(EM_mifs_REG_DECODERCONTROL, 0x40);
            }
            // 设置定时器控制方式
            EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCONTROL, &ucTimerCtl);
            // 如果定时器要人工启动，就启动它
            if ((~ucTimerCtl & 0x02) != 0)
            {
                EI_mifs_vSetBitMask(EM_mifs_REG_CONTROL, 0x02);
            }
            //D2(TRACE("\r\n IntWait:%2X IntEnable:%2X",ucIntWait,ucIntEnable););
            // 启动该命令
            EI_mifs_vWriteReg(1, EM_mifs_REG_COMMAND, &cmd);

            timer0 = sys_get_counter();
            // 将上面屏蔽的代码转移到命令发送完成后，避免因为其它中断或者优先级高的任务导致命令发送操作没有执行而使操作失败
            // 总是允许定时器超时中断,并且等待这种中断
            ucIntEnable |= RC531_IRQ_TIMER;
            ucIntWait |= RC531_IRQ_TIMER;
            s_rfid_setRC531Int(1, ucIntEnable);
            // 等待期望的中断请求发生，这样就可以结束命令
            // 如果超过10秒没有处理，则返回错误
            while (1)
            {
                if ((EG_mifs_tWorkInfo.ucINTSource & ucIntWait) != 0)
                {
                    break;
                }
                if(if_timerout(timer0,timeout))
                {
                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_NOTAGERR;
                    D2(LABLE(0x22););			
                    break;
                }
            }
            // 如果是写EEPROM操作, 必须等待结束
            if (cmd == EM_mifs_PCD_WRITEE2)
            {
                while (1)
                {
                    EI_mifs_vReadReg(1, EM_mifs_REG_SECONDARYSTATUS, &ucTempData);
                    if ((ucTempData & 0x40) == 0x40)
                    {
                        break;
                    }
                    if(if_timerout(timer0,timeout))
                    {
                        // 超时
                        EG_mifs_tWorkInfo.ucCurResult = EM_TIMEOUT;
                        EG_mifs_tWorkInfo.ucCollPos = 0;
                        break;
                    }
                }
            }
            //命令结束处理      
            EI_mifs_vSetBitMask(EM_mifs_REG_CONTROL, 0x04);	// 停止定时器        
            s_rfid_setRC531Int(0, RC531_IRQ_ALL); // 禁止所有中断
            s_rfid_setRC531IntFlg(0, RC531_IRQ_ALL); // 清除所有中断请求位
            // 返回到IDLE模式
            ucTempData = EM_mifs_PCD_IDLE;
            EI_mifs_vWriteReg(1, EM_mifs_REG_COMMAND, &ucTempData);

            // 根据命令去掉一些不用处理的错误        
            if ((cmd == EM_mifs_PCD_WRITEE2) || (cmd == EM_mifs_PCD_READE2)
                || (cmd == EM_mifs_PCD_LOADKEYE2) || (cmd == EM_mifs_PCD_LOADKEY))
            {
                // 只处理缓冲区溢出
                // 主要是因为这4个命令只涉及到CPU与RC531之间的通信
                EG_mifs_tWorkInfo.ucSaveErrState &= RC531_OVFLERR;
            }
            else
            {
                // 只处理缓冲区溢出、冲突错误、奇偶错误、帧错误、CRC错误
                EG_mifs_tWorkInfo.ucSaveErrState &= RC531_ALLERR;
            }
            // 返回错误
            if (EG_mifs_tWorkInfo.ucSaveErrState != 0)
            {
                if ((EG_mifs_tWorkInfo.ucSaveErrState & RC531_COLLERR) != 0)
                {
                    // 冲突
                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_COLLERR;
                }
                else if ((EG_mifs_tWorkInfo.ucSaveErrState & RC531_PARITYERR) != 0)
                {
                    // 奇偶校验错
                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_PARITYERR;
                }
                else if ((EG_mifs_tWorkInfo.ucSaveErrState & RC531_FRAMINGERR) != 0)
                {
                    // 帧错误
                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_FRAMINGERR;
                }		
                else if ((EG_mifs_tWorkInfo.ucSaveErrState & RC531_CRCERR) != 0)
                {
                    // CRC错误
                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_CRCERR;
                }
                else if ((EG_mifs_tWorkInfo.ucSaveErrState & RC531_OVFLERR) != 0)
                {
                    EG_mifs_tWorkInfo.ucCurResult = EM_mifs_OVFLERR;
                }
                else if (EG_mifs_tWorkInfo.ucCurResult == EM_SUCCESS)	
                {
                    EG_mifs_tWorkInfo.ucCurResult = EM_ERROR;
                }
            }								
            // 恢复单帧接收设置
            if (ucrxMultiple == 0)
            {
                EI_mifs_vClrBitMask(EM_mifs_REG_DECODERCONTROL, 0x40);
            }
        }									// if (EG_mifs_tWorkInfo.ucCurResult == EM_SUCCESS)

        // 恢复先前WaterLevel设置
        EI_mifs_vWriteReg(1, EM_mifs_REG_FIFOLEVEL, &ucWaterLevelBak);
		
#endif

	}
	

	
    D1( LABLE(0xDD);
        DATAIN(EG_mifs_tWorkInfo.ucCurResult);
        //DATAIN(EG_mifs_tWorkInfo.ucINTSource);
        DATAIN(EG_mifs_tWorkInfo.ucSaveErrState);
        DATAIN(LBYTE(EG_mifs_tWorkInfo.ulBytesReceived));
        DATAIN(LBYTE(EG_mifs_tWorkInfo.lBitsReceived));
        //DATAIN(EG_mifs_tWorkInfo.ucnBits);
        //DATAIN(EG_mifs_tWorkInfo.ucCollPos);		
        for (guiDebugj = 0; guiDebugj < EG_mifs_tWorkInfo.ulBytesReceived; guiDebugj++)
        {
        DATAIN(EG_mifs_tWorkInfo.aucBuffer[guiDebugj]);
        }
      );
    return EG_mifs_tWorkInfo.ucCurResult;
}

/**********************************************************************
* 函数名称： 
*   void EI_mifs_vSetTimer(uchar time)
* 功能描述： 
*   定时器设置
* 输入参数： 
*   time: 定时时间
*   0~14: 0.000302*2^time     
*   其他: 128*(TEMP)/ 13560000秒     
* 输出参数：
*   无
* 返回值： 
*   无
* 历史纪录：
*   修改人      日期                版本号
***********************************************************************/
void EI_mifs_vSetTimer(ulong time)
{
	uchar ucTempData = 0;

    if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911)
    {
        ucTempData = ucTempData;
#ifdef EM_AS3911_Module
        EI_paypass_vSetTimer(time);
#endif

    }
    else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
    {
        ucTempData = ucTempData;
#ifdef EM_RC663_Module
        EI_paypass_vSetTimer(time);
#endif

    }
    else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{
	
#ifdef EM_PN512_Module

		ucTempData = 0x40;      //64/6.78 = 9.43us
		EI_mifs_vWriteReg(1, TPrescalerReg, &ucTempData);
		ucTempData = 0x0F;
		EI_mifs_vClrBitMask(TModeReg, ucTempData);

		ucTempData = (uchar)(time&0xFF); 
		EI_mifs_vWriteReg(1, TReloadVal_Lo, &ucTempData);
		ucTempData = (uchar)((time>>8)&0xFF); ; 
		EI_mifs_vWriteReg(1, TReloadVal_Hi, &ucTempData);

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module  //RC531

        switch (time)
        {
        case 0:							// (0.302 ms) FWI = 0
            {
                // TAutoRestart = 0, TPrescale = 128
                ucTempData = 0x07;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 33
                ucTempData = 0x21;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 1:							// (0.604 ms) FWI=1
            {
                // TAutoRestart=0,TPrescale=128
                ucTempData = 0x07;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 65
                ucTempData = 0x41;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 2:							// (1.208 ms) FWI=2
            {
                // TAutoRestart=0,TPrescale=128
                ucTempData = 0x07;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 129
                ucTempData = 0x81;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 3:							// (2.416 ms) FWI=3
            {
                // TAutoRestart=0,TPrescale=512
                ucTempData = 0x09;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 65
                ucTempData = 0x41;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 4:							// (4.833 ms) FWI=4
            {
                // TAutoRestart=0,TPrescale=512
                ucTempData = 0x09;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 129
                ucTempData = 0x81;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 5:							// (9.666 ms) FWI=5
            {
                // TAutoRestart=0,TPrescale=2048
                ucTempData = 0x0B;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 65
                ucTempData = 0x41;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 6:							// (19.33 ms) FWI=6  13560
            {
                // TAutoRestart=0,TPrescale=2048
                ucTempData = 0x0B;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 129
                ucTempData = 0x81;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 7:							// (38.66 ms) FWI=7
            {
                // TAutoRestart=0,TPrescale=8192
                ucTempData = 0x0D;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 65
                ucTempData = 0x41;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 8:							// (77.32 ms) FWI=8
            {
                // TAutoRestart=0,TPrescale=8192
                ucTempData = 0x0D;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 129
                ucTempData = 0x81;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 9:							// (154.6 ms) FWI=9
            {
                // TAutoRestart=0,TPrescale=32768
                ucTempData = 0x0F;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 65
                ucTempData = 0x41;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 10:							// (309.3 ms) FWI=10
            {
                // TAutoRestart=0,TPrescale=32768
                ucTempData = 0x0F;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 129
                ucTempData = 0x81;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 11:							// (618.6 ms) FWI=11
            {
                // TAutoRestart=0,TPrescale=524288
                ucTempData = 0x13;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 17
                ucTempData = 0x11;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 12:							// (1.2371 s) FWI=12
            {
                // TAutoRestart=0,TPrescale=524288
                ucTempData = 0x13;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 33
                ucTempData = 0x21;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 13:							// (2.4742 s) FWI=13
            {
                // TAutoRestart=0,TPrescale=524288
                ucTempData = 0x13;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 65
                ucTempData = 0x41;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        case 14:							// (4.9485 s) FWI=14
            {
                // TAutoRestart=0,TPrescale=524288
                ucTempData = 0x13;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

                // TReloadVal = 129
                ucTempData = 0x81;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        default:
            {
                //缺省情况下直接设置
                // TAutoRestart=0,TPrescale=128
                ucTempData = 0x07;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);
                ucTempData = (uchar)time;
                EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
                break;
            }
        }
		
#endif

	}
	
}

/**********************************************************************
* 函数名称： 
*   void EI_vMifsHisr (void) 
* 功能描述： 
*   RC531芯片的中断处理程序
* 输入参数： 
*   无
* 输出参数：
*   无
* 返回值： 
*   无
* 历史纪录：
*   修改人      日期                版本号
***********************************************************************/
void EI_vMifsHisr(void)
{
	uchar ucTempData = 0,ucTempData1 = 0;
	uchar ucIrqPending = 0;
	ulong ulTemp = 0;
	vuchar Data = 0;

    if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911){

        ucTempData = ucTempData;
        ucTempData1 = ucTempData1;
        ucIrqPending = ucIrqPending;
        ulTemp = ulTemp;
        Data = Data;
#ifdef EM_AS3911_Module
        #if AS3911_IRQMODE
        as3911Isr();
        #endif
#endif

    }
    else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
    {
        ucTempData1 = ucTempData1;
#ifdef EM_RC663_Module

#if 0
        GPIO_SetBits(Port_LedGreen,Pin_LedGreen);	
        EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_IRQ0, &ucTempData);
        EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_IRQ1, &ucIrqPending);
        s_rfid_setRC663IntFlg(0,0x1F7F);
        GPIO_ResetBits(Port_LedGreen,Pin_LedGreen);	
#endif
		while(1)
		{	
			if((gtRfidProInfo.IntEnable == 0)||((gtRfidProInfo.TimerIntEnable&(RC663_IRQ_TIMER1>>8)) == 0))
			{
				break;
			}
			EI_mifs_vReadReg(1,PHHAL_HW_RC663_REG_IRQ1,(uchar *)&Data);	
//rfid_isr_start:	
			EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_IRQ0, &ucIrqPending);		
            EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_ERROR, (uchar *) &EG_mifs_tWorkInfo.ucErrFlags);
//			EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_IRQ0EN, &ucTempData1);		
//            D2(LABLE(0xBB);DATAIN(ucIrqPending);DATAIN(Data);DATAIN(ucTempData1);DATAIN(gtRfidProInfo.IntEnable););	
//            if ( ++i > 100000 ) {
//                TRACE("\r\n%x-%x-%x-%x", ucIrqPending, Data,gtRfidProInfo.IntEnable,gtRfidProInfo.TimerIntEnable);
//                break;
//            }
			ucIrqPending &= gtRfidProInfo.IntEnable;
//            Data &= gtRfidProInfo.TimerIntEnable;
			EG_mifs_tWorkInfo.ucINTSource = EG_mifs_tWorkInfo.ucINTSource | ucIrqPending;
			EG_mifs_tWorkInfo.TimeSource = EG_mifs_tWorkInfo.TimeSource | Data;
			D2(LABLE(0xAA);DATAIN(ucIrqPending);DATAIN(Data);DATAIN(EG_mifs_tWorkInfo.ucErrFlags););	
            if ((Data&(RC663_IRQ_TIMER1>>8)) == 0) {
                //time1 中断未产生时
                if ( ucIrqPending == 0 ) {
                    D2(LABLE(0xBC);DATAIN(ucIrqPending);DATAIN(Data););	
                    break;//没有需要的中断 退出
                }
            }
			// 发送LoAlert  第一帧数据在前台发送
			if (ucIrqPending & RC663_IRQ_LO)
			{		
				s_rfid_setRC663IntFlg(0, RC663_IRQ_LO);
                if ( EG_mifs_tWorkInfo.FIFOSIZE == 512 ) {
                    EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_FIFOCONTROL, &ucTempData1);
                    ucTempData1 &= 0x03;
                    EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_FIFOLENGTH, &ucTempData);
                    ulTemp = EG_mifs_tWorkInfo.FIFOSIZE - RFID_DEFAULT_WATER_LEVEL - (ushort)((ucTempData1<<8)|ucTempData) - 1;
                }else{
                    //255
                    EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_FIFOLENGTH, &ucTempData);
                    ulTemp = EG_mifs_tWorkInfo.FIFOSIZE - RFID_DEFAULT_WATER_LEVEL - ucTempData - 1;
                }

				if ((EG_mifs_tWorkInfo.ulSendBytes - EG_mifs_tWorkInfo.ulBytesSent) <= ulTemp)
				{
					ulTemp = EG_mifs_tWorkInfo.ulSendBytes - EG_mifs_tWorkInfo.ulBytesSent;
					s_rfid_setRC663Int(0, RC663_IRQ_LO);
				}
				D2(LABLE(0xF2);DATAIN(LBYTE(ulTemp)););	
				if(ulTemp)
				{
					EI_mifs_vWriteReg(ulTemp, PHHAL_HW_RC663_REG_FIFODATA,
						EG_mifs_tWorkInfo.aucBuffer + EG_mifs_tWorkInfo.ulBytesSent);
					EG_mifs_tWorkInfo.ulBytesSent += ulTemp;

//                    EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_COMMAND, 0x1F); //方法1:在lo中断后fifo填充数据后再送cmd也可以
//                    EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_COMMAND, cmd);
				}			
			}
			// TxIRQ 发送完成准备接受 
			if (ucIrqPending & RC663_IRQ_TX)
			{			
				D2(LABLE(0xF3););
				s_rfid_setRC663IntFlg(0, RC663_IRQ_TX);
				if(gtRfidProInfo.Cmd != EM_mifs_PICC_HALT)
				{		
					// 下面这种情况，忽略校验
					if (((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL1)
							&& (EG_mifs_tWorkInfo.lBitsReceived == 7))
						|| ((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL2)
							&& (EG_mifs_tWorkInfo.lBitsReceived == 7))
						|| ((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL3)
							&& (EG_mifs_tWorkInfo.lBitsReceived == 7)))
					{
						// RxCRC and TxCRC disable, parity disable               
						ucTempData = 0xC0;
						EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_FRAMECON, ucTempData); // Parity disable
						ucTempData = 0x01;
						EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData); // CRC disable
						EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData); // CRC disable
					}
				}
				else
				{
					s_rfid_setRC663Int(0, RC663_IRQ_ALL);
					s_rfid_setRC663IntFlg(0, RC663_IRQ_ALL);
					EG_mifs_tWorkInfo.ucINTSource |= RC663_IRQ_RX;
					return;
				}
			}
			
			if (ucIrqPending & (RC663_IRQ_RX | RC663_IRQ_HI))
			{	
				
				if(ucIrqPending & RC663_IRQ_RX)
				{
					//RX
					s_rfid_setRC663IntFlg(0, RC663_IRQ_RX);
					D2(LABLE(0xF4););				
				}
				if(ucIrqPending & RC663_IRQ_HI)
				{
					s_rfid_setRC663IntFlg(0, RC663_IRQ_HI);
					D2(LABLE(0xF5););				
				}
				// 读数据时才会读取错误寄存器
				EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_ERROR, (uchar *) &EG_mifs_tWorkInfo.ucErrFlags);
				EG_mifs_tWorkInfo.ucErrFlags &= RC663_ALLERR;			
				D2(DATAIN(EG_mifs_tWorkInfo.ucErrFlags););				
				if(EG_mifs_tWorkInfo.ucErrFlags & RC663_COLLERR)
				{
					//col错误 只有防冲撞命令,才允许冲突 读取冲突位置
					if ((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL1)			
						|| (EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL2)			
						|| (EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL3))
					{
						EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_RXCOLL, (uchar *) & EG_mifs_tWorkInfo.ucCollPos);
						EG_mifs_tWorkInfo.ucCollPos &= 0x7F; //bit0 - bit6 为冲突位置
                        EG_mifs_tWorkInfo.ucErrFlags &= ~RC663_CRCERR;   //冲突检测忽略奇偶和CRC校验
					}
					if(EG_mifs_tWorkInfo.ucnBits == 7)
					{
						//冲突检测，且位数为7, 则忽略奇偶校验错
//						EG_mifs_tWorkInfo.ucErrFlags &= (~RC531_PARITYERR);
					}					
				}
				EG_mifs_tWorkInfo.ucSaveErrState |= EG_mifs_tWorkInfo.ucErrFlags;
				EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_FIFOLENGTH, &ucTempData);
				if(ucTempData != 0)
				{
					D2(DATAIN(ucTempData););
					ucTempData = (uchar)MIN((ulong)ucTempData,(EM_mifs_BUFFER_LEN - EG_mifs_tWorkInfo.ulBytesReceived));
					EI_mifs_vReadReg(ucTempData,PHHAL_HW_RC663_REG_FIFODATA,EG_mifs_tWorkInfo.aucBuffer + EG_mifs_tWorkInfo.ulBytesReceived);
					EG_mifs_tWorkInfo.ulBytesReceived += ucTempData;
//					D2(DATAIN(EG_mifs_tWorkInfo.aucBuffer[0]);
//                       DATAIN(EG_mifs_tWorkInfo.aucBuffer[1]);
//                       DATAIN(EG_mifs_tWorkInfo.aucBuffer[2]);
//                       );
				}
				if(ucIrqPending & RC663_IRQ_RX)
				{
					// 接收的最后一个字节位数
					EI_mifs_vReadReg(1, PHHAL_HW_RC663_REG_RXBITCTRL, &ucTempData);
					ucTempData &= 0x07;
					if (ucTempData != 0)
					{
						// 最后一个字节没有接收完
						EG_mifs_tWorkInfo.lBitsReceived +=
							(EG_mifs_tWorkInfo.ulBytesReceived - 1) * 8 + ucTempData;
						D2(LABLE(0xF7);DATAIN(ucTempData););
					}
					else
					{
						// 最后一个字节接收完
						EG_mifs_tWorkInfo.lBitsReceived += EG_mifs_tWorkInfo.ulBytesReceived * 8;
					}
				}		
			}
			if (ucIrqPending & RC663_IRQ_IDLE)
			{
				D2(LABLE(0xF8););
				s_rfid_setRC663IntFlg(0, RC663_IRQ_TIMER1 | RC663_IRQ_IDLE);
				s_rfid_setRC663Int(0, RC663_IRQ_TIMER1);
				EG_mifs_tWorkInfo.TimeSource &= ~(RC663_IRQ_TIMER1>>8);
//				Data &= ~(RC663_IRQ_TIMER1>>8);
			}
			if (Data & (RC663_IRQ_TIMER1>>8))
			{
				//定时器中断
				D1(LABLE(0xF0););
				EG_mifs_tWorkInfo.ucCurResult = EM_mifs_NOTAGERR;
				s_rfid_setRC663IntFlg(0, RC663_IRQ_TIMER1);
				s_rfid_setRC663Int(0, RC663_IRQ_TIMER1);
			}		
		}
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{
	
#ifdef EM_PN512_Module
		while(1)
		{
			//SETSIGNAL1_H();
			EI_mifs_vReadReg(1, ComIrqReg, &ucIrqPending);
			EI_mifs_vReadReg(1, CommIEnReg, &ucTempData);
	
			D2(LABLE(0xAA);DATAIN(ucIrqPending);DATAIN(ucTempData);DATAIN(gtRfidProInfo.IntEnable););
			if (gtRfidProInfo.IntEnable == 0)
			{
				break;
			}
			EI_mifs_vReadReg(1,Status1Reg,(uchar *)&Data);
			//SETSIGNAL1_L();
			D2(LABLE(0xBB);DATAIN(Data););
			if ((Data & 0x10) == 0)
			{
				//D2(LABLE(0xFF););
				break;
			}

		//rfid_isr_start:
			//EI_mifs_vReadReg(1, ComIrqReg, &ucIrqPending);
			//EI_mifs_vReadReg(1, CommIEnReg, &ucTempData);
		      //ucIrqPending &= 0x7F;
			//D2(LABLE(0xAA);DATAIN(ucIrqPending);DATAIN(ucTempData);DATAIN(gtRfidProInfo.IntEnable););
			ucIrqPending &= gtRfidProInfo.IntEnable;
			EG_mifs_tWorkInfo.ucINTSource = EG_mifs_tWorkInfo.ucINTSource | ucIrqPending;
			//D2(LABLE(0xAC);DATAIN(EG_mifs_tWorkInfo.ucINTSource););
			//D2(LABLE(0xAD);DATAIN(ucIrqPending););
			// 发送LoAlert
			if (ucIrqPending & PN512_IRQ_LO)
			{
				s_rfid_setRC531IntFlg(0, PN512_IRQ_LO);
				EI_mifs_vReadReg(1, FIFOLevelReg, &ucTempData);
				ucTempData &= 0x7F; //PN512 bit7 是清空
				ulTemp = 64 - RFID_DEFAULT_WATER_LEVEL - ucTempData - 1;
				if ((EG_mifs_tWorkInfo.ulSendBytes - EG_mifs_tWorkInfo.ulBytesSent) <= ulTemp)
				{
					ulTemp = EG_mifs_tWorkInfo.ulSendBytes - EG_mifs_tWorkInfo.ulBytesSent;
					s_rfid_setRC531Int(0, PN512_IRQ_LO);
				}
				D2(LABLE(0xF2);DATAIN(LBYTE(ulTemp)););
				if(ulTemp)
				{
					EI_mifs_vWriteReg(ulTemp, FIFODataReg,
						EG_mifs_tWorkInfo.aucBuffer + EG_mifs_tWorkInfo.ulBytesSent);
					EG_mifs_tWorkInfo.ulBytesSent += ulTemp;
					
					EI_mifs_vSetBitMask(BitFramingReg, 0x80);  //启动发送
				}
			}
			// TxIRQ 发送完成准备接受
			if (ucIrqPending & PN512_IRQ_TX)
			{
				D2(LABLE(0xF3););
				s_rfid_setRC531IntFlg(0, PN512_IRQ_TX);
				if(gtRfidProInfo.Cmd != EM_mifs_PICC_HALT)
				{
					// 下面这种情况，忽略校验
					if (((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL1)
							&& (EG_mifs_tWorkInfo.lBitsReceived == 7))
						|| ((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL2)
							&& (EG_mifs_tWorkInfo.lBitsReceived == 7))
						|| ((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL3)
							&& (EG_mifs_tWorkInfo.lBitsReceived == 7)))
					{
						// RxCRC and TxCRC disable, parity disable
						ucTempData = 0x10;
						EI_mifs_vSetBitMask(ManualRCVReg, ucTempData);  // disable 奇偶校验
					}
				}
				else
				{
					s_rfid_setRC531Int(0, PN512_IRQ_ALL);
					s_rfid_setRC531IntFlg(0, PN512_IRQ_ALL);
					EG_mifs_tWorkInfo.ucINTSource |= PN512_IRQ_RX;
					return;
				}
			}

			if (ucIrqPending & (PN512_IRQ_RX | PN512_IRQ_HI))
			{
				if(ucIrqPending & PN512_IRQ_RX)
				{
					//RX
					s_rfid_setRC531IntFlg(0, PN512_IRQ_RX);
					D2(LABLE(0xF4););
				}
				if(ucIrqPending & PN512_IRQ_HI)
				{
					s_rfid_setRC531IntFlg(0, PN512_IRQ_HI);
					D2(LABLE(0xF5););
				}
				// 读数据时才会读取错误寄存器
				EI_mifs_vReadReg(1, ErrorReg, (uchar *) &EG_mifs_tWorkInfo.ucErrFlags);
				EG_mifs_tWorkInfo.ucErrFlags &= PN512_ALLERR;
				D2(DATAIN(EG_mifs_tWorkInfo.ucErrFlags););
				if(EG_mifs_tWorkInfo.ucErrFlags & PN512_COLLERR)
				{
					//col错误 只有防冲撞命令,才允许冲突 读取冲突位置
					if ((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL1)
						|| (EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL2)
						|| (EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL3))
					{
						EI_mifs_vReadReg(1, CollReg, (uchar *) & EG_mifs_tWorkInfo.ucCollPos);
						EG_mifs_tWorkInfo.ucCollPos &= 0x1F; //bit0 - bit4 为冲突位置
					}
					if(EG_mifs_tWorkInfo.ucnBits == 7)
					{
						//冲突检测，且位数为7, 则忽略奇偶校验错
						EG_mifs_tWorkInfo.ucErrFlags &= (~PN512_PARITYERR);
					}
				}
				EG_mifs_tWorkInfo.ucSaveErrState |= EG_mifs_tWorkInfo.ucErrFlags;
				EI_mifs_vReadReg(1, FIFOLevelReg, &ucTempData);
				ucTempData &= 0x7F;
				D2(DATAIN(ucTempData););
				if(ucTempData != 0)
				{
					//D2(DATAIN(ucTempData););
					ucTempData = (uchar)MIN((ulong)ucTempData,(EM_mifs_BUFFER_LEN - EG_mifs_tWorkInfo.ulBytesReceived));
					
					EI_mifs_vReadReg(ucTempData,FIFODataReg,EG_mifs_tWorkInfo.aucBuffer + EG_mifs_tWorkInfo.ulBytesReceived);
					EG_mifs_tWorkInfo.ulBytesReceived += ucTempData;
				}
				if(ucIrqPending & PN512_IRQ_RX)
				{
					// 接收的最后一个字节位数
					EI_mifs_vReadReg(1, ControlReg, &ucTempData);
					ucTempData &= 0x07;
					if (ucTempData != 0)
					{
						// 最后一个字节没有接收完
						EG_mifs_tWorkInfo.lBitsReceived +=
							(EG_mifs_tWorkInfo.ulBytesReceived - 1) * 8 + ucTempData;
						D2(LABLE(0xF7);DATAIN(ucTempData););
					}
					else
					{
						// 最后一个字节接收完
						EG_mifs_tWorkInfo.lBitsReceived += EG_mifs_tWorkInfo.ulBytesReceived * 8;
					}
				}
			}
			if (ucIrqPending & PN512_IRQ_IDLE)
			{
				D2(LABLE(0xF8););
				s_rfid_setRC531IntFlg(0, PN512_IRQ_TIMER | PN512_IRQ_IDLE);
				s_rfid_setRC531Int(0, PN512_IRQ_TIMER);
				EG_mifs_tWorkInfo.ucINTSource &= ~(PN512_IRQ_TIMER);
				ucIrqPending &= ~(PN512_IRQ_TIMER);
			}
			if (ucIrqPending & PN512_IRQ_TIMER)
			{
				//定时器中断
				D1(LABLE(0xF0););
				EG_mifs_tWorkInfo.ucCurResult = EM_mifs_NOTAGERR;
				s_rfid_setRC531IntFlg(0, PN512_IRQ_TIMER);
			}

		}
#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

        while(1)
        {	
            //2011-02-25 yehf增加
            if(gtRfidProInfo.IntEnable == 0)
            {
                break;
            }
            EI_mifs_vReadReg(1,EM_mifs_REG_PRIMARYSTATUS,(uchar *)&Data);	
            //D2(LABLE(0xAA);DATAIN(Data););	
            if ((Data & 0x08) == 0)
            {
                break;
            }

            //rfid_isr_start:
            EI_mifs_vReadReg(1, EM_mifs_REG_INTERRUPTRQ, &ucIrqPending);
            ucIrqPending &= gtRfidProInfo.IntEnable;
            EG_mifs_tWorkInfo.ucINTSource = EG_mifs_tWorkInfo.ucINTSource | ucIrqPending;
            D2(LABLE(0xAA);DATAIN(ucIrqPending););
            // 发送LoAlert
            if (ucIrqPending & RC531_IRQ_LO)
            {
                s_rfid_setRC531IntFlg(0, RC531_IRQ_LO);
                EI_mifs_vReadReg(1, EM_mifs_REG_FIFOLENGTH, &ucTempData);
                ulTemp = 64 - RFID_DEFAULT_WATER_LEVEL - ucTempData - 1;
                if ((EG_mifs_tWorkInfo.ulSendBytes - EG_mifs_tWorkInfo.ulBytesSent) <= ulTemp)
                {
                    ulTemp = EG_mifs_tWorkInfo.ulSendBytes - EG_mifs_tWorkInfo.ulBytesSent;
                    s_rfid_setRC531Int(0, RC531_IRQ_LO);
                }
                D2(LABLE(0xF2);DATAIN(LBYTE(ulTemp)););
                if(ulTemp)
                {
                    EI_mifs_vWriteReg(ulTemp, EM_mifs_REG_FIFODATA,
                                      EG_mifs_tWorkInfo.aucBuffer + EG_mifs_tWorkInfo.ulBytesSent);
                    EG_mifs_tWorkInfo.ulBytesSent += ulTemp;
                }
            }
            // TxIRQ 发送完成准备接受
            if (ucIrqPending & RC531_IRQ_TX)
            {
                D2(LABLE(0xF3););
                s_rfid_setRC531IntFlg(0, RC531_IRQ_TX);
                if(gtRfidProInfo.Cmd != EM_mifs_PICC_HALT)
                {
                    // 下面这种情况，忽略校验
                    if (((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL1)
                         && (EG_mifs_tWorkInfo.lBitsReceived == 7))
                        || ((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL2)
                            && (EG_mifs_tWorkInfo.lBitsReceived == 7))
                        || ((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL3)
                            && (EG_mifs_tWorkInfo.lBitsReceived == 7)))
                    {
                        // RxCRC and TxCRC disable, parity disable
                        ucTempData = 0x02;
                        EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
                    }
                }
                else
                {
                    s_rfid_setRC531Int(0, RC531_IRQ_ALL);
                    s_rfid_setRC531IntFlg(0, RC531_IRQ_ALL);
                    EG_mifs_tWorkInfo.ucINTSource |= RC531_IRQ_RX;
                    return;
                }
            }

            if (ucIrqPending & (RC531_IRQ_RX | RC531_IRQ_HI))
            {
                if(ucIrqPending & RC531_IRQ_RX)
                {
                    //RX
                    s_rfid_setRC531IntFlg(0, RC531_IRQ_RX);
                    D2(LABLE(0xF4););
                }
                if(ucIrqPending & RC531_IRQ_HI)
                {
                    s_rfid_setRC531IntFlg(0, RC531_IRQ_HI);
                    D2(LABLE(0xF5););
                }
                // 读数据时才会读取错误寄存器
                EI_mifs_vReadReg(1, EM_mifs_REG_ERRORFLAG, (uchar *) &EG_mifs_tWorkInfo.ucErrFlags);
                EG_mifs_tWorkInfo.ucErrFlags &= RC531_ALLERR;
                D2(DATAIN(EG_mifs_tWorkInfo.ucErrFlags););
                if(EG_mifs_tWorkInfo.ucErrFlags & RC531_COLLERR)
                {
                    //col错误 只有防冲撞命令,才允许冲突 读取冲突位置
                    if ((EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL1)
                        || (EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL2)
                        || (EG_mifs_tWorkInfo.ucAnticol == EM_mifs_PICC_ANTICOLL3))
                    {
                        EI_mifs_vReadReg(1, EM_mifs_REG_COLLPOS, (uchar *) & EG_mifs_tWorkInfo.ucCollPos);
                    }
                    if(EG_mifs_tWorkInfo.ucnBits == 7)
                    {
                        //冲突检测，且位数为7, 则忽略奇偶校验错
                        EG_mifs_tWorkInfo.ucErrFlags &= (~RC531_PARITYERR);
                    }
                }
                EG_mifs_tWorkInfo.ucSaveErrState |= EG_mifs_tWorkInfo.ucErrFlags;
                EI_mifs_vReadReg(1, EM_mifs_REG_FIFOLENGTH, &ucTempData);
                if(ucTempData != 0)
                {
                    D2(DATAIN(ucTempData););
                    ucTempData = (uchar)MIN((ulong)ucTempData,(EM_mifs_BUFFER_LEN - EG_mifs_tWorkInfo.ulBytesReceived));
                    EI_mifs_vReadReg(ucTempData,EM_mifs_REG_FIFODATA,EG_mifs_tWorkInfo.aucBuffer + EG_mifs_tWorkInfo.ulBytesReceived);
                    EG_mifs_tWorkInfo.ulBytesReceived += ucTempData;
                }
                if(ucIrqPending & RC531_IRQ_RX)
                {
                    // 接收的最后一个字节位数
                    EI_mifs_vReadReg(1, EM_mifs_REG_SECONDARYSTATUS, &ucTempData);
                    ucTempData &= 0x07;
                    if (ucTempData != 0)
                    {
                        // 最后一个字节没有接收完
                        EG_mifs_tWorkInfo.lBitsReceived +=
                            (EG_mifs_tWorkInfo.ulBytesReceived - 1) * 8 + ucTempData;
                        D2(LABLE(0xF7);DATAIN(ucTempData););
                    }
                    else
                    {
                        // 最后一个字节接收完
                        EG_mifs_tWorkInfo.lBitsReceived += EG_mifs_tWorkInfo.ulBytesReceived * 8;
                    }
                }
            }
            if (ucIrqPending & RC531_IRQ_IDLE)
            {
                D2(LABLE(0xF8););
                s_rfid_setRC531IntFlg(0, RC531_IRQ_TIMER | RC531_IRQ_IDLE);
                s_rfid_setRC531Int(0, RC531_IRQ_TIMER);
                EG_mifs_tWorkInfo.ucINTSource &= ~(RC531_IRQ_TIMER);
                ucIrqPending &= ~(RC531_IRQ_TIMER);
            }
            if (ucIrqPending & RC531_IRQ_TIMER)
            {
                //定时器中断
                D1(LABLE(0xF0););
                EG_mifs_tWorkInfo.ucCurResult = EM_mifs_NOTAGERR;
                s_rfid_setRC531IntFlg(0, RC531_IRQ_TIMER);
            }
        }

#endif

    }
}
#endif
