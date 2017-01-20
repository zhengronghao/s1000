/*********************************************************************
* 版权所有 (C)2003, 福建实达电脑设备有限公司。
* 文件名称： 
*   Mifs_HDEFS.h
* 当前版本： 
*   01-01-01
* 内容摘要： 
*   本文件实现非接触卡模块一些硬件相关的定义
* 历史纪录：
*   修改人          日期                    版本号      修改说明
******************************************************************/

#ifndef _MIFS_HDEFS_H_
#define _MIFS_HDEFS_H_

#include "wp30_ctrl.h"


/**********************************************************************

                         常量定义
                          
***********************************************************************/
/**********************************************************************

                         寄存器地址定义
                          
***********************************************************************/
#ifdef EM_AS3911_Module
#include "./rfid/as3911/as3911.h"
#endif

#define RFID_Module_RC531 	1
#define RFID_Module_PN512 	2
#define RFID_Module_RC663 	3
#define RFID_Module_AS3911 	4


#ifdef EM_RC531_Module

// 页0: 命令和状态
#define     EM_mifs_REG_PAGE                  0x00
#define     EM_mifs_REG_COMMAND               0x01
#define     EM_mifs_REG_FIFODATA              0x02
#define     EM_mifs_REG_PRIMARYSTATUS         0x03
#define     EM_mifs_REG_FIFOLENGTH            0x04
#define     EM_mifs_REG_SECONDARYSTATUS       0x05
#define     EM_mifs_REG_INTERRUPTEN           0x06
#define     EM_mifs_REG_INTERRUPTRQ           0x07

// 页1: 控制和状态
#define     EM_mifs_REG_CONTROL               0x09
#define     EM_mifs_REG_ERRORFLAG             0x0A
#define     EM_mifs_REG_COLLPOS               0x0B
#define     EM_mifs_REG_TIMERVALUE            0x0C
#define     EM_mifs_REG_CRCRESULTLSB          0x0D
#define     EM_mifs_REG_CRCRESULTMSB          0x0E
#define     EM_mifs_REG_BITFRAMING            0x0F

// 页2：传输和编码控制
#define     EM_mifs_REG_TXCONTROL             0x11
#define     EM_mifs_REG_CWCONDUCTANCE         0x12
#define     EM_mifs_REG_MODCONDUCTANCE        0x13
#define     EM_mifs_REG_CODERCONTROL          0x14
#define     EM_mifs_REG_MODWIDTH              0x15
#define     EM_mifs_REG_MODWIDTHSOF           0x16
#define     EM_mifs_REG_TYPEBFRAMING          0x17

// 页3：接收和解码控制
#define     EM_mifs_REG_RXCONTROL1            0x19
#define     EM_mifs_REG_DECODERCONTROL        0x1A
#define     EM_mifs_REG_BITPHASE              0x1B
#define     EM_mifs_REG_RXTHRESHOLD           0x1C
#define     EM_mifs_REG_BPSKDEMCONTROL        0x1D
#define     EM_mifs_REG_RXCONTROL2            0x1E
#define     EM_mifs_REG_CLOCKQCONTROL         0x1F

// 页4：RF时序和通道冗余
#define     EM_mifs_REG_RXWAIT                0x21
#define     EM_mifs_REG_CHANNELREDUNDANCY     0x22
#define     EM_mifs_REG_CRCPRESETLSB          0x23
#define     EM_mifs_REG_CRCPRESETMSB          0x24
#define     EM_mifs_REG_TIMESLOTPERIOD        0x25
#define     EM_mifs_REG_MFOUTSELECT           0x26
#define     EM_mifs_REG_RFU27                 0x27

// 页5：FIFO, 定时器和中断引脚配置
#define     EM_mifs_REG_FIFOLEVEL             0x29
#define     EM_mifs_REG_TIMERCLOCK            0x2A
#define     EM_mifs_REG_TIMERCONTROL          0x2B
#define     EM_mifs_REG_TIMERRELOAD           0x2C
#define     EM_mifs_REG_IRQPINCONFIG          0x2D
#define     EM_mifs_REG_RFU2E                 0x2E
#define     EM_mifs_REG_RFU2F                 0x2F

// 页6：RFU
#define     EM_mifs_REG_RFU31                 0x31
#define     EM_mifs_REG_RFU32                 0x32
#define     EM_mifs_REG_RFU33                 0x33
#define     EM_mifs_REG_RFU34                 0x34
#define     EM_mifs_REG_RFU35                 0x35
#define     EM_mifs_REG_RFU36                 0x36
#define     EM_mifs_REG_RFU37                 0x37

// 页7：测试控制
#define     EM_mifs_REG_RFU39                 0x39
#define     EM_mifs_REG_TESTANASELECT         0x3A
#define     EM_mifs_REG_RFU3B                 0x3B
#define     EM_mifs_REG_RFU3C                 0x3C
#define     EM_mifs_REG_TESTCONFIGURATION     0x3C
#define     EM_mifs_REG_TESTDIGISELECT        0x3D
#define     EM_mifs_REG_RFU3E                 0x3E
#define     EM_mifs_REG_TESTDIGIACCESS        0x3F



//中断寄存器各位配置
#define  RC531_IRQ_LO    (1<<0)  //0000 0001
#define  RC531_IRQ_HI    (1<<1)  //0000 0010
#define  RC531_IRQ_IDLE  (1<<2)  //0000 0100
#define  RC531_IRQ_RX    (1<<3)  //0000 1000
#define  RC531_IRQ_TX    (1<<4)  //0001 0000
#define  RC531_IRQ_TIMER (1<<5)  //0010 0000 
#define  RC531_IRQ_ALL   (RC531_IRQ_LO|RC531_IRQ_HI|RC531_IRQ_IDLE|RC531_IRQ_RX|RC531_IRQ_TX|RC531_IRQ_TIMER)
#define  RC531_IRQ_NONE  0

// 错误状态寄存器配置
#define  RC531_COLLERR     (1<<0)
#define  RC531_PARITYERR   (1<<1)
#define  RC531_FRAMINGERR  (1<<2)
#define  RC531_CRCERR      (1<<3)
#define  RC531_OVFLERR     (1<<4)
#define  RC531_ALLERR      (RC531_COLLERR | RC531_PARITYERR | RC531_FRAMINGERR | RC531_CRCERR | RC531_OVFLERR)

//定时器控制寄存器配置Timercontrol
#define  RC531_TCON_TxBegin  (1<<0)
#define  RC531_TCON_TxEnd    (1<<1)
#define  RC531_TCON_RxBegin  (1<<2)
#define  RC531_TCON_RxEnd    (1<<3)

//TimerControl 
#define  RC531_TIMECON_STOP_RXEnd    (1<<3)
#define  RC531_TIMECON_STOP_RXBegin  (1<<2)
#define  RC531_TIMECON_STOP_TXEnd    (1<<1)
#define  RC531_TIMECON_STOP_TXBegin  (1<<0)

#endif

#ifdef EM_PN512_Module

/**********************************************************************

			PN512命令

***********************************************************************/
    #define     PN512CMD_IDLE            	0x00	    // 空闲命令
    #define 	PN512CMD_CONFIG		    	0x01		// 配置命令（PN512）
    #define 	PN512CMD_MEM		    	0x01		// 存储25字节到内部缓冲区（RC52x）
    #define 	PN512CMD_GENRANID    		0x02		// 获取10字节的随机数
    #define     PN512CMD_CALCCRC         	0x03		// 计算CRC
    #define     PN512CMD_TRANSMIT        	0x04		// 发送数据 
    #define     PN512CMD_NOCMDCHANGE     	0x07		// 修改命令寄存器的其他位（不改变当前命令）
    #define     PN512CMD_RECEIVE         	0x08		// 激活接收器
    #define     PN512CMD_TRANSCEIVE			0x0C		// 发送FIFO中的数据到天线，传输后激活接收电路
    #define 	PN512CMD_AUTOCOLL	    	0x0D
    #define     PN512CMD_AUTHENT         	0x0E		// MIFARE认证
    #define     PN512CMD_SOFTRESET       	0x0F		// 软件复位

/**********************************************************************

PN512寄存器

***********************************************************************/
    
    //Page 0
    #define     PageReg           		     	  0x00
    #define     CommandReg        		          0x01
    #define     CommIEnReg		 	             0x02
    #define     DivIEnReg        				 0x03
    #define     ComIrqReg          				  0x04
    #define     DivIrqReg      					 0x05
    #define     ErrorReg        				   0x06
    #define     Status1Reg        				   0x07
    
    #define     Status2Reg         				  0x08
    #define     FIFODataReg        			       0x09
    #define     FIFOLevelReg      			       0x0A
    #define     WaterLevelReg     		         0x0B
    #define     ControlReg        			    0x0C
    #define     BitFramingReg   		       0x0D
    #define     CollReg        				  0x0E
    #define     RFUF           					 0x0F
    
    //Page 1
    //#define     PageReg             0x10
    #define     ModeReg          				   0x11
    #define     TxModeReg       				  0x12
    #define     RxModeReg       				 0x13
    #define     TxControlReg      				    0x14
    #define     TxAutoReg          				    0x15
    #define     TxSelReg        				   0x16
    #define     RxSelReg        				  0x17
    
    #define     RxThresholdReg   				 0x18
    #define     DemodReg        				    0x19
    #define     FelNFC1Reg      				  0x1A
    #define     FelNFC2Reg        			      0x1B
    #define     MifNFCReg         				  0x1C
    #define     ManualRCVReg       				 0x1D
    #define     TypeBReg         				   0x1E
    #define     SerialSpeedReg     			    0x1F
    
    //Page 2
    //#define     PageReg             0x20
    #define     CRCResultMSB       		         0x21
    #define     CRCResultLSB    				 0x22
    #define     GsNOffReg         				 0x23
    #define     ModWidthReg        			  0x24
    #define     TxBitPhaseReg      			  0x25
    #define     RFCfgReg         			  0x26
    #define     GsNOnReg         		        0x27
    #define     CWGsPReg						0x28
    
    #define     ModGsPReg          			   0x29
    #define     TModeReg           				 0x2A
    #define     TPrescalerReg         			 0x2B
    #define     TReloadVal_Hi          			 0x2C
    #define     TReloadVal_Lo         			 0x2D
    #define     TCounterVal_Hi                 0x2E
    #define     TCounterVal_Lo                 0x2F
    
    //Page 3
    //#define     PageReg             0x30
    #define     TestSel1Reg              	   0x31
    #define     TestSel2Reg             	    0x32
    #define     TestPinEnReg            	     0x33
    #define     TestPinValueReg                 0x34
    #define     TestBusReg              	   0x35
    #define     AutoTestReg              	   0x36
    #define     VersionReg               	  0x37
    
    #define     AnalogTestReg				0x38
    #define     TestDAC1Reg                 0x39
    #define     TestDAC2Reg      		   0x3A
    #define     TestADCReg                 0x3B
    #define     RFU3C             		    0x3C
    #define     RFU3D     					0x3D
    #define     RFU3E        				0x3E
    #define     RFU3F              	 	 	 0x3F
    
    
    //中断寄存器各位配置
    #define  PN512_IRQ_LO    (1<<2)  
    #define  PN512_IRQ_HI    (1<<3)  
    #define  PN512_IRQ_IDLE  (1<<4) 
    #define  PN512_IRQ_RX    (1<<5)  
    #define  PN512_IRQ_TX    (1<<6)  
    #define  PN512_IRQ_TIMER (1<<0)  
    #define  PN512_IRQ_ERR 	 (1<<1)  
    #define  PN512_IRQ_ALL   (PN512_IRQ_LO|PN512_IRQ_HI|PN512_IRQ_IDLE|PN512_IRQ_RX|PN512_IRQ_TX|PN512_IRQ_TIMER)
    #define  PN512_IRQ_NONE  0
    
    // 错误状态寄存器配置
    #define  PN512_COLLERR     (1<<3)
    #define  PN512_PARITYERR   (1<<1)
    #define  PN512_FRAMINGERR  (1<<0)
    #define  PN512_CRCERR      (1<<2)
    #define  PN512_OVFLERR     (1<<4)
    #define  PN512_ALLERR      (PN512_COLLERR | PN512_PARITYERR | PN512_FRAMINGERR | PN512_CRCERR | PN512_OVFLERR)

#endif

#endif									//_MIFS_HDEFS_H_
