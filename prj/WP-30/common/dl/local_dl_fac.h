/*
 * =====================================================================================
 *
 *       Filename:  local_dl_fac.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  1/27/2015 3:54:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhangjp (), zhangjp@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#ifndef __LOCAL_DL_FAC_H__
#define __LOCAL_DL_FAC_H__ 

//协议相关(子命令)
#define SUB_CMD_DEL_FAC			  	0x02		//删除生产测试
#define SUB_CMD_GET_TESTRES			0x03		//获取生产测试结果
#define SUB_CMD_GET_ATTACK			0x04		//获取防拆情况
#define SUB_CMD_ALARM			    0x05		//报警
#define SUB_CMD_WRITE_WOINFO		0x06		//写工单信息到文件系统
#define	SUB_CMD_SET_ATTACK			0x07		//设置防拆情况
#define SUB_CMD_MODULE_EXIST		0x08		//查询相关模块是否存在
#define SUB_CMD_DEL_ALLSAPP			0x09		//删除所有子应用(除了生产测试)
#define SUB_CMD_GET_TIME			0x0a		//获取POS时间
#define SUB_CMD_GET_APP_DESCRPT		0x0b		//获取所有应用的DESCRIPTION
#define SUB_CMD_SET_MAINBOARDID     0x0C        //设置主板ID
#define SUB_CMD_SENDINFOR           0x0D        //发送关机、重启、管控检成功信息、失败信息，批量下载成功、失败信息

extern const struct LocalDlCallBack gcLocaldlCallBack;

#endif
