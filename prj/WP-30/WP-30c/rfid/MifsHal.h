/*********************************************************************
* 版权所有 (C)2003, 福建实达电脑设备有限公司。
* 文件名称： 
*   MifsHal.h
* 当前版本： 
*   01-01-01
* 内容摘要： 
*   本文件实现MifsHal.c的头文件,向上提供接口。
* 历史纪录：
*   修改人      日期            版本号          修改记录
******************************************************************/
 // 只包含一次
#ifndef _MIFS_HAL_H_
#define _MIFS_HAL_H_

#include "wp30_ctrl.h"

/**********************************************************************

                         全局函数原型
                          
***********************************************************************/
#define AS3911_IRQMODE  1
//#define SPI_RFID        SPI_ID_1            //spi0总线
#define SPIBUS0_RFID    0


#if defined(PRODUCT_F16_2)
#define BOARD_PIN_RF_INT   PIN_INT_RFID
#define BOARD_PIN_RF_RST   PIN_PWREN_RFID
#define RFID_INTERRUPT 	   BOARD_PIN_RF_INT    //PC5
#define RFID_Reset         BOARD_PIN_RF_RST    //PC4
#define RFID_IRQ_DEV       PIOC_ID
#endif

//#define s_CloseIsr()   disable_dev_irq(RFID_IRQ_DEV) //disable_Interrupts(IRQ_MASK)
//#define s_OpenIsr()    enable_dev_irq(RFID_IRQ_DEV)  //enable_Interrupts(IRQ_MASK)  
/**********************************************************************

                         全局函数原型
                          
***********************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

	#define  EI_mifs_vReadReg       EI_mifs_vReadRegSPI
	#define  EI_mifs_vWriteReg      EI_mifs_vWriteRegSPI
	#define  EI_mifs_vSetBitMask    EI_mifs_vSetBitMaskSPI
	#define  EI_mifs_vClrBitMask    EI_mifs_vClrBitMaskSPI
	#define  EI_mifs_vFlushFIFO     EI_mifs_vFlushFIFOSPI

	void EI_mifs_vReadRegSPI(ulong ulLen, uchar ucRegAddr, uchar * pucData);	// 读寄存器
	uchar EI_mifs_vWriteRegSPI(ulong ulLen, uchar ucRegAddr, uchar * pucData);	// 写寄存器
	void EI_mifs_vSetBitMaskSPI(uchar ucRegAddr, uchar ucBitMask);	// 将寄存器相应位置1
	void EI_mifs_vClrBitMaskSPI(uchar ucRegAddr, uchar ucBitMask);	// 将寄存器相应位置0
	void EI_mifs_vFlushFIFOSPI(void);	//清空RC531的FIFO缓冲区

	void EI_mifs_vReadReg(ulong ulLen, uchar ucRegAddr, uchar * pucData);	// 读寄存器
	uchar EI_mifs_vWriteReg(ulong ulLen, uchar ucRegAddr, uchar * pucData);	// 写寄存器
	void EI_mifs_vSetBitMask(uchar ucRegAddr, uchar ucBitMask);	// 将寄存器相应位置1
	void EI_mifs_vClrBitMask(uchar ucRegAddr, uchar ucBitMask);	// 将寄存器相应位置0
	void EI_mifs_vFlushFIFO(void);		//清空RC531的FIFO缓冲区
	void s_Rfid_vHalInit(void);			//初始化硬件部分
	void s_Rfid_SetInt(uint mode);
	void s_Rfid_SetRST(uint mode);
	void s_rfid_setRC531Int(uint mode, uchar irq);
	void s_rfid_setRC531IntFlg(uint mode, uchar irq);
    void s_rfid_setRC663Int(uint mode, uint irq);
    void s_rfid_setRC663IntFlg(uint mode, uint irq);
	void s_rfid_setRC531Timercontrol(uchar control);
	void s_rfid_ReadReg(uint len, uchar *ucRegAddr, uchar *pucData);
    uchar spi_exchange_block(uchar data, uchar cs);
    void RFID_IO_IRQHandler(void);
    int s_rfid_mainVersion(int mode);
    int hw_rfid_spi_init(void);
#ifdef __cplusplus
}
#endif

#endif									//_MIFS_HAL_H_
