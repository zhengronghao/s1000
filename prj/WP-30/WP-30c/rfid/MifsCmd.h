/*********************************************************************
* 版权所有 (C)2003, 福建实达电脑设备有限公司。
* 文件名称： 
*   MifsCmd.h
* 当前版本： 
*    01-01-01
* 内容摘要： 
*   本文件实现MifsCmd.c的头文件,向上提供接口
* 历史纪录：
*   修改人      日期			    	版本号
*             06-27-2003 	            01-01-01  
******************************************************************/
 // 只包含一次
#ifndef _MIFS_CMD_H_
#define _MIFS_CMD_H_


/**********************************************************************

                         宏定义
                          
***********************************************************************/
#define  RFID_DEFAULT_WATER_LEVEL  4  //CLRC663 default is 5

/**********************************************************************

                         全局函数原型
                          
***********************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

	void EI_mifs_vInit(void);			//初始化非接触卡模块
	void EI_mifs_vEnd(void);
	uchar EI_mifs_ucHandleCmd(uchar cmd);	//完成对非接触卡模块的命令请求和应答处理
	void EI_mifs_vSetTimer(ulong time);	//设置非接触卡模块的定时器
	void EI_vMifsHisr (void);
	uint if_timerout(uint timer0,uint timerout);

#ifdef __cplusplus
}
#endif

#endif									//_MIFS_CMD_H_
