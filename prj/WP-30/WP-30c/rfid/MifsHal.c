/*********************************************************************
* 版权所有 (C)2003, 福建实达电脑设备有限公司。
* 文件名称： 
*   MifsHAL.c
* 当前版本： 
*   01-01-01
* 内容摘要： 
*   本文件实现非接触卡模块硬件相关层操作。
* 历史纪录：
*   修改人          日期                版本号      修改记录
******************************************************************/
#include "wp30_ctrl.h"

#ifdef CFG_RFID
/**********************************************************************

                          全局函数定义
                          
***********************************************************************/
/**********************************************************************

                          本地全局变量定义
                          
***********************************************************************/

/**********************************************************************

                          局部函数定义
                          
***********************************************************************/
void EI_mifs_vOpenCS(void);
void EI_mifs_vCloseCS(void);

/**********************************************************************
* 函数名称： 
*   s_rfid_ReadReg
* 功能描述： 
*   从多个寄存器读数据（SPI模式）
* 输入参数： 
*   ucRegAddr：寄存器地址列表
*   pucData：存放读出的数据
*   ulLen:   数据/地址长度
* 输出参数： 
*   无
* 返回值： 
*   无
* 其它说明： 
*   无
* 历史纪录：
*   修改人      日期                版本号
*             06-26-2003          01-01-01
***********************************************************************/
uchar spi_exchange_block(uchar data, uchar cs)
{
//	uchar tmp;
//	SPI_SendCommand(RFID_SPIn,SPIBUS0_RFID,&data,1,&tmp);
//    hw_spi_master_write(LCD_SPIn,LCD_PCS_PIN,data,length); 
//    hw_spi_master_WriteRead(ExFLASH_SPIn,byte,ExFLASH_CSPIN,SPI_PCS_ASSERTED);
    if ( cs == 0 )
        return hw_spi_master_WriteRead(RFID_SPIn,data,RFID_PCS_PIN,SPI_PCS_ASSERTED);
    else
        return hw_spi_master_WriteRead(RFID_SPIn,data,RFID_PCS_PIN,SPI_PCS_INACTIVE);
//	return hw_spi_master_WriteRead(RFID_SPIn,data,RFID_PCS_PIN,SPI_PCS_INACTIVE);
}

void s_rfid_ReadReg(uint len, uchar *ucRegAddr, uchar *pucData)
{
#if defined(EM_RC531_Module) || defined(EM_PN512_Module)  //RC531
    uint i;
    uchar data;
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531 ||
         EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512) {
        EI_mifs_vOpenCS();
        data = (((*ucRegAddr++) << 1) & (uchar) 0x7E);
        pucData[0] = spi_exchange_block(data|0x80, 0);
        for(i=0;i<(len-1);i++)
        {
            pucData[i] =  spi_exchange_block((*ucRegAddr++)<<1, 0);
        }
        pucData[i] = spi_exchange_block(0, 1);
        EI_mifs_vCloseCS();
    }
#endif
}
/**********************************************************************
* 函数名称： 
*   void EI_mifs_vReadRegSPI (ulong ulLen, uchar ucRegAddr, uchar* pucData);
* 功能描述： 
*   从寄存器读数据（SPI模式）
* 输入参数： 
*   ucRegAddr：寄存器地址
*   pucData：存放读出的数据
*   ulLen:   数据/地址长度
* 输出参数： 
*   无
* 返回值： 
*   无
* 其它说明： 
*   无
* 历史纪录：
*   修改人      日期                版本号
*             06-26-2003          01-01-01
***********************************************************************/
void EI_mifs_vReadRegSPI(ulong ulLen, uchar ucRegAddr, uchar *pucData)
{
	uint i;
	vuchar tmp;
	if (ulLen == 0)
		return;
	s_CloseIsr();
	EI_mifs_vOpenCS();
#if defined(EM_AS3911_Module) //as3911
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
        i = 0;
        tmp = 0;
        i = i;
        tmp = tmp;
    }
#endif
#if defined(EM_RC531_Module) || defined(EM_PN512_Module)  //RC531
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531 ||
         EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512) {
        ucRegAddr = ((ucRegAddr << 1) & (uchar) 0x7E);	
        pucData[0] = spi_exchange_block(ucRegAddr|0x80, 0);
        for(i=0;i<(ulLen-1);i++)
        {
            //发送和FIFO同样的地址或0xFF 12022015 chenf
            tmp = spi_exchange_block(ucRegAddr|0x80, 0);
            pucData[i] = tmp;
        }
        tmp = spi_exchange_block(0, 1);
        pucData[i] = tmp;
    }
#endif
#if defined(EM_RC663_Module)
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {
        ucRegAddr = ((ucRegAddr << 1) | (uchar) 0x01);
        pucData[0] = spi_exchange_block(ucRegAddr, 0);	
        for(i=0;i<(ulLen-1);i++)
        {
            tmp = spi_exchange_block(ucRegAddr, 0);
            pucData[i] = tmp; 	
        }
        tmp = spi_exchange_block(0, 1);	
        pucData[i] = tmp;
    }
#endif
	EI_mifs_vCloseCS();
	s_OpenIsr();
}

/**********************************************************************
* 函数名称： 
*   void EI_mifs_vWriteRegSPI(ulong ulLen, uchar ucRegAddr, uchar* pucData);
* 功能描述： 
*   从寄存器读数据（SPI模式）
* 输入参数： 
*   ucRegAddr：寄存器地址
*   pucData：要写入数据
*   ulLen:   数据长度
* 输出参数： 
*   无
* 返回值： 
*    无
* 其它说明： 
*   无
* 历史纪录：
*   修改人      日期                版本号
*             06-26-2003          01-01-01
***********************************************************************/
uchar EI_mifs_vWriteRegSPI(ulong ulLen, uchar ucRegAddr, uchar * pucData)
{
	uint i;
	vuchar data;
	if (ulLen == 0)
		return 0;
	s_CloseIsr();
	EI_mifs_vOpenCS();
#if defined(EM_AS3911_Module) //as3911
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
        i = 0;
        data = 0;
        i = i;
        data = data;
    }
#endif
#if defined(EM_RC531_Module) || defined(EM_PN512_Module)  //RC531
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531 ||
         EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512) {
        ucRegAddr = ((ucRegAddr<<1) & 0x7f);
        data = spi_exchange_block(ucRegAddr, 0);
        for(i=0;i<(ulLen-1);i++)
        {
            data = spi_exchange_block(pucData[i], 0);
        }
        data = spi_exchange_block(pucData[i], 1);
    }
#endif
#if defined(EM_RC663_Module)
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {
        ucRegAddr = ucRegAddr<<1;
        data = spi_exchange_block(ucRegAddr, 0);
        for(i=0;i<(ulLen-1);i++)
        {
            data = spi_exchange_block(pucData[i], 0);
        }
        data = spi_exchange_block(pucData[i], 1);
    }
#endif
	EI_mifs_vCloseCS();
	s_OpenIsr(); 
    return data;
}

/**********************************************************************
* 函数名称： 
*   void EI_mifs_vSetBitMaskSPI(uchar ucRegAddr, uchar ucBitMask)
* 功能描述： 
*   将某个寄存器特定位置1（SPI模式）
* 输入参数： 
*   ucRegAddr：寄存器地址
*   ucBitMask: 要置1位的掩码
* 输出参数： 
*   无
* 返回值： 
*   无
* 其它说明： 
*   无
* 历史纪录：
*   修改人      日期                版本号
*             06-26-2003          01-01-01
***********************************************************************/
void EI_mifs_vSetBitMaskSPI(uchar ucRegAddr, uchar ucBitMask)
{
	uchar ucTemp;
	EI_mifs_vReadRegSPI(1, ucRegAddr, &ucTemp);
	ucTemp |= ucBitMask;
	EI_mifs_vWriteRegSPI(1, ucRegAddr, &ucTemp);
}

/**********************************************************************
* 函数名称： 
*   void EI_mifs_vClrBitMaskSPI(uchar ucRegAddr, uchar ucBitMask)
* 功能描述： 
*   将某个寄存器特定位清0（SPI模式）
* 输入参数： 
*   ucRegAddr：寄存器地址
*   ucBitMask: 要清0位的掩码
* 输出参数： 
*   无
* 返回值： 
*   无
* 其它说明： 
*   无
* 历史纪录：
*   修改人      日期                版本号
*             06-26-2003          01-01-01
***********************************************************************/
void EI_mifs_vClrBitMaskSPI(uchar ucRegAddr, uchar ucBitMask)
{
	uchar ucTemp;
	EI_mifs_vReadRegSPI(1, ucRegAddr, &ucTemp);
	ucTemp &= ~ucBitMask;
	EI_mifs_vWriteRegSPI(1, ucRegAddr, &ucTemp);

}

/**********************************************************************
* 函数名称： 
*   void EI_mifs_vFlushFIFOSPI(void)
* 功能描述： 
*   清空FIFO缓冲区(并行模式)
* 输入参数： 
*   无
* 输出参数： 
*   无
* 返回值： 
*   无
* 其它说明： 
*   无
* 历史纪录：
*   修改人      日期                版本号
*             06-26-2003          01-01-01
***********************************************************************/
void EI_mifs_vFlushFIFOSPI(void)
{
	uchar ucTemp;
	uchar ucRegAddr;
		
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
#ifdef EM_AS3911_Module
        ucTemp = 0;
        ucRegAddr = 0;
        ucTemp = ucTemp;
        ucRegAddr = ucRegAddr;
#endif
    }
    else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
    {

#ifdef EM_RC663_Module
        EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FIFOCONTROL, 0x10);
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{
	
#ifdef EM_PN512_Module
		ucRegAddr = FIFOLevelReg;
		EI_mifs_vReadRegSPI(1, ucRegAddr, &ucTemp);
		ucTemp |= 0x80;
		EI_mifs_vWriteRegSPI(1, ucRegAddr, &ucTemp);
#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module
		ucRegAddr = EM_mifs_REG_CONTROL;
		EI_mifs_vReadRegSPI(1, ucRegAddr, &ucTemp);
		ucTemp |= 0x01;
		EI_mifs_vWriteRegSPI(1, ucRegAddr, &ucTemp);
#endif

	}

	
}

/**********************************************************************
* 函数名称： 
*   void EI_mifs_vOpenCS(void)
* 功能描述： 
*    开始使用SPI模式
* 输入参数： 
*    无
* 输出参数： 
*   无
* 返回值： 
*   无
* 其它说明： 
*   无
* 历史纪录：
*   修改人      日期                版本号          修改备注
*             06-26-2003          01-01-01        创建
***********************************************************************/
void EI_mifs_vOpenCS(void)
{
//	SPI_SetCS(RFID_SPIn,SPIBUS0_RFID,ON);
//    hw_gpio_reset_bits(RFID_RST_GPIO,1<<PORT_Pin_5);
}

/**********************************************************************
* 函数名称： 
*   void EI_mifs_vCloseCS(void)
* 功能描述： 
*   停止使用SPI模式
* 输入参数： 
*   无
* 输出参数： 
*   无
* 返回值： 
*   无
* 其它说明： 
*   无
* 历史纪录：
*   修改人      日期                版本号          修改备注
*             06-26-2003          01-01-01        创建
***********************************************************************/
void EI_mifs_vCloseCS(void)
{
//	SPI_SetCS(RFID_SPIn,SPIBUS0_RFID,OFF);
//    hw_gpio_set_bits(RFID_RST_GPIO,1<<PORT_Pin_5);
}

/**********************************************************************
* 函数名称： 
*     void s_Rfid_SetInt(uint mode)
* 功能描述： 
*     控制Rfid外部中断
* 输入参数： 
*     无
* 输出参数： 
*     无
* 返回值： 
*     无
* 其它说明： 
*     无
* 历史纪录：
*   修改人      日期                版本号
***********************************************************************/
void s_Rfid_SetInt(uint mode)
{
	if(mode == ON)
	{
        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
            //3911修改为高电平中断(上升沿中断可用有可能不可靠)
//            requst_pio_interrupt(RFID_INTERRUPT, INT_HIGH, 3, EI_vMifsHisr);
//            set_irq_priority (RFID_IO_IRQn, INT_PRI_MF);
            enable_irq(RFID_IO_IRQn);
        }else{
//            requst_pio_interrupt(RFID_INTERRUPT, INT_LOW, 3, EI_vMifsHisr);
            enable_irq(RFID_IO_IRQn);
        }
//		enable_pio_interrupt(RFID_INTERRUPT);
	}
	else
	{
//		disable_pio_interrupt(RFID_INTERRUPT);
        disable_irq(RFID_IO_IRQn);
	}
}
void RFID_IO_IRQHandler(void)
{
    if (PORTx_IRQPinx(RFID_IO_PORT,RFID_IO_PINx)) {
        PORTx_IRQPinx_Clear(RFID_IO_PORT,RFID_IO_PINx);
        EI_vMifsHisr();
    }
}
/**********************************************************************
* 函数名称： 
*     void  s_Rfid_vHalInit(void)
* 功能描述： 
*     初始化射频硬件部分
* 输入参数： 
*     无
* 输出参数： 
*     无
* 返回值： 
*     无
* 其它说明： 
*     无
* 历史纪录：
*   修改人      日期                版本号
***********************************************************************/
int hw_rfid_spi_init(void)
{
    SPI_InitDef spi_init;

    memset(&spi_init,0,sizeof(SPI_InitDef));
    spi_init.spix = RFID_SPIn;
    memset(spi_init.pcs,PTxInvid,SPI_PCS_MAX);
    spi_init.pcs[RFID_SPI_PCSn] = RFID_PTxy_PCSn;
    spi_init.sck  = RFID_PTxy_SCLK;
    spi_init.mosi = RFID_PTxy_MOSI;
    spi_init.miso = RFID_PTxy_MISO;
    spi_init.mode = SPI_MODE_MASTER;
    spi_init.TxFIFO_switch = FALSE;
    spi_init.RxFIFO_switch = FALSE;
    spi_init.TxCompleteIrq = FALSE;
    spi_init.TxQueueEndIrq = FALSE;
    spi_init.TxFIFO_UnderflowIrq = FALSE;
    spi_init.RxFIFO_OverflowIrq  = FALSE;
    spi_init.TxFIFO_FillIrq  = FALSE;
    spi_init.RxFIFO_DrainIrq = FALSE;
    spi_init.TxFIFO_IrqMode  = FALSE;
    spi_init.RxFIFO_IrqMode  = FALSE;
    spi_init.p_asc  = 1; //设置sck后cs的延迟时间 必须要否则3911spi工作异常
    spi_init.t_asc  = 1; //设置sck后cs的延迟时间
    spi_init.p_csc  = 1; //设置sck前cs提早拉低的时间
    spi_init.t_csc  = 0;
    spi_init.t_dt  = 0;
    spi_init.p_dt  = 0;
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
        //spi clk == 8M
        spi_init.attr = SCK_BR_DIV_2|SCK_PBR_DIV_3|MODE_MSBit
            |CPOL_CLK_LOW | CPHA_DATA_CAPTURED_FollowingEdge 
            |FRAME_SIZE_8;
    }else{
        //60M外设频率 4分频 2分频 共8分频7.5M
        spi_init.attr = SCK_BR_DIV_4|SCK_PBR_DIV_2|MODE_MSBit
            |CPOL_CLK_LOW | CPHA_DATA_CAPTURED_LeadingEdge 
            |FRAME_SIZE_8;
    }

    return hw_spi_init(&spi_init);
}

void s_Rfid_vHalInit(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = RFID_IO_GPIO;//gcKbOpt[index].gpiox;
//    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PU 
//        |PORT_Mode_IN_PFE|PORT_Mode_IRQ_EXTI_FEdge;
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
        gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PD
            |PORT_Mode_IN_PFE|PORT_Mode_IRQ_EXTI_REdge;
    }else{
        gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PD
            |PORT_Mode_IN_PFE|PORT_Mode_IRQ_EXTI_FEdge;
    }
    gpio_init.PORT_Pin = RFID_IO_PINx;
    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(gpio_init.PORT_Pin);
    hw_gpio_init(&gpio_init);

    //17550复位
    gpio_init.GPIOx = RFID_RST_GPIO;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_DSH;
    gpio_init.PORT_Pin = RFID_RST_PINx;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(RFID_RST_PINx);
    hw_gpio_init(&gpio_init);

//    gpio_init.GPIOx = GPIOE;
//    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_DSH;
//    gpio_init.PORT_Pin = PORT_Pin_6;
//    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(RFID_RST_PINx);
//    hw_gpio_init(&gpio_init);

    hw_rfid_spi_init();

    //设置spi的cs作为io使用 相应要修改spi_exchange_block_EX和AS3911_SEN_ON()
    /*
    gpio_init.GPIOx = GPIOE;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF|PORT_Mode_Out_DSL;
    gpio_init.PORT_Pin = PORT_Pin_4;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(gpio_init.PORT_Pin);
    hw_gpio_init(&gpio_init);
    hw_gpio_set_bits(GPIOE,GPIO_Pin_4);
    */
//	set_pio_output(RFID_Reset,1);
//	enable_pio(RFID_INTERRUPT);   // USC_INT
//	set_pio_input(RFID_INTERRUPT,1);
}
/**********************************************************************
* 函数名称： 
*    void s_Rfid_SetRST(uint mode)
* 功能描述： 
*     控制RC531复位引脚
* 输入参数： 
*     无
* 输出参数： 
*     无
* 返回值： 
*     无
* 其它说明： 
*     无
* 历史纪录：
*   修改人      日期                版本号
***********************************************************************/
void s_Rfid_SetRST(uint mode)
{
//	set_pio_output(RFID_Reset, (int)mode);
    if ( mode ) {
        hw_gpio_set_bits(RFID_RST_GPIO,1<<RFID_RST_PINx);
    }else
        hw_gpio_reset_bits(RFID_RST_GPIO,1<<RFID_RST_PINx);
}

/*****************************************************************
* 函数名称：
*        
* 功能描述：
*        设置中断
* 被以下函数调用：
*        mode=1 打开中断  =0关闭中断
*        irq:中断位
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
****************************************************************/
void s_rfid_setRC531Int(uint mode, uchar irq)
{
	uchar tmp;
	s_CloseIsr();

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
#ifdef EM_AS3911_Module
        tmp = 0;
        tmp = tmp;
#endif
    }
	if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{
	
#ifdef EM_PN512_Module

		tmp = 0;
		if(mode == 1)
		{
			gtRfidProInfo.IntEnable |= irq;
			tmp = irq & 0x7F;
			EI_mifs_vSetBitMask(CommIEnReg,tmp);
		}
		else
		{
			gtRfidProInfo.IntEnable &= (~irq);
			tmp = irq & 0x7F;
			EI_mifs_vClrBitMask(CommIEnReg,tmp);
		}	
		gtRfidProInfo.IntEnable &= 0x7F;
		
#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		if(mode == 1)
		{
			gtRfidProInfo.IntEnable |= irq;
			tmp = irq | 0x80;
		}
		else
		{
			gtRfidProInfo.IntEnable &= (~irq);
			tmp = irq & 0x3F;
		}	
		gtRfidProInfo.IntEnable &= 0x3F;	
		EI_mifs_vWriteReg(1, EM_mifs_REG_INTERRUPTEN, &tmp);

#endif

	}

	s_OpenIsr();
}

/*****************************************************************
* 函数名称：
*        
* 功能描述：
*        设置中断标志
* 被以下函数调用：
*        mode=1 清除中断标志  =0设置标志中断
*        irq:中断位
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
****************************************************************/
void s_rfid_setRC531IntFlg(uint mode, uchar irq)
{
	uchar tmp;

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
#ifdef EM_AS3911_Module
        tmp = 0;
        tmp = tmp;
#endif
    }
	if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{
	
#ifdef EM_PN512_Module
 
		tmp = irq&0x7F;
		if(mode == 1)
		{
			tmp = irq | 0x80;
		}
		EI_mifs_vWriteReg(1, ComIrqReg, &tmp);
	
#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		tmp = irq;
		if(mode == 1)
		{
			tmp = irq | 0x80;		
		}
		EI_mifs_vWriteReg(1, EM_mifs_REG_INTERRUPTRQ, &tmp);

#endif

	}
}

/*****************************************************************
* 函数名称：
*        
* 功能描述：
*        设置中断
* 被以下函数调用：
*        mode=1 打开中断  =0关闭中断
*        irq:中断位
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
****************************************************************/
void s_rfid_setRC663Int(uint mode, uint irq)
{
#ifdef EM_RC663_Module
	uchar tmp;
	s_CloseIsr();

	if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
	{
	
#ifdef EM_RC663_Module

		tmp = 0;
		if(mode == 1)
		{
            //先设置定时器 irq0设置就产生中断
			tmp = (uchar)((irq>>8) & 0x7F);
			gtRfidProInfo.TimerIntEnable |= tmp;
			EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_IRQ1EN,tmp);

			tmp = (uchar)(irq & 0x7F);
			gtRfidProInfo.IntEnable |= tmp;
			EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_IRQ0EN,tmp);
		}
		else
		{
			tmp = (uchar)(irq & 0x7F);
			gtRfidProInfo.IntEnable &= (~tmp);
			EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_IRQ0EN,tmp);

			tmp = (uchar)((irq>>8) & 0x7F);
			gtRfidProInfo.TimerIntEnable &= (~tmp);
			EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_IRQ1EN,tmp);
		}	
		gtRfidProInfo.IntEnable &= 0x7F;
		gtRfidProInfo.TimerIntEnable &= 0x7F;
		
#endif

	}
	
	s_OpenIsr();
#endif
}

/*****************************************************************
* 函数名称：
*        
* 功能描述：
*        设置中断标志
* 被以下函数调用：
*        mode=1 清除中断标志  =0设置标志中断
*        irq:中断位
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
****************************************************************/
void s_rfid_setRC663IntFlg(uint mode, uint irq)
{
#ifdef EM_RC663_Module
	uchar tmp;
    uchar time;

	if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
	{
	
#ifdef EM_RC663_Module
 
		tmp = (uchar)(irq&0x7F);
		time = (uchar)((irq>>8)&0x7F);
		if(mode == 1)
		{
			tmp |= 0x80;
			time |= 0x80;
		}
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_IRQ0, &tmp);
		EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_IRQ1, &time);
	
#endif

	}
#endif
}

/*****************************************************************
* 函数名称：
*        
* 功能描述：
*        设置定时器
* 被以下函数调用：
*        
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
****************************************************************/
void s_rfid_setRC531Timercontrol(uchar control)
{
#ifdef EM_RC531_Module

	EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCONTROL, &control);

#else

	D1(TRACE("\r\n RFID Moudle Wrong!!!"););
	
#endif
}

int s_rfid_mainVersion(int mode)
{
//#if  defined(PRODUCT_F16_2)
//    uchar buf[8];
//    memset(buf, 0, sizeof(buf));
//    sys_read_ver(READ_MAINB_VER,buf);
////    TRACE("\r\nrfid main verion:%x,%x,%x,%x\r\n",buf[0],buf[1],buf[2],buf[3]);
//    if ( buf[3] >= '1' ) {
//        return 1;
//    }else
//        return 0;
//#else
//    return 1;
//#endif
    return 1;
}
#endif
