#ifndef __FAC_CTRL_H__
#define __FAC_CTRL_H__


#define FAC_CTRL_NOTTEST			0   // 未测试
#define FAC_CTRL_SUCCESS			1   // 测试成功 
#define FAC_CTRL_FAIL			  	2   // 测试失败
#define FAC_CTRL_FORCE_SUC      	3   // 强制成功


#define FAC_CTRL_NOINFO			  	-1000
#define FAC_CTRL_ERRORSIZE			-1001
#define FAC_CTRL_INVALPARA			-1002
#define FAC_CTRL_ERRORDATA			-1003
#define FAC_CTRL_CANCEL			  	-1004
// 测试阶段
#define FAC_PHASE_DEBUG				0	//板卡调试阶段
#define FAC_PHASE_ASSEMBLE			1	//装配检
#define FAC_PHASE_AGING				2	//老化
#define FAC_PHASE_PRODUCT			3	//成品检
#define FAC_PHASE_NUM			  	4	//生产测试总阶段数

#define AUTO_TEST_SHIFT_SRAM        0
#define AUTO_TEST_SHIFT_KEY         1
#define AUTO_TEST_SHIFT_LCD         2
#define AUTO_TEST_SHIFT_PRINT       3
#define AUTO_TEST_SHIFT_MAG         4
#define AUTO_TEST_SHIFT_IC          5
#define AUTO_TEST_SHIFT_TF          6
#define AUTO_TEST_SHIFT_SERIAL      7
#define AUTO_TEST_SHIFT_RTC         8
#define AUTO_TEST_SHIFT_TEMP        9
#define AUTO_TEST_SHIFT_NAND        10
#define AUTO_TEST_SHIFT_RFID        11
#define AUTO_TEST_SHIFT_MODEM       12
#define AUTO_TEST_SHIFT_LAN         13
#define AUTO_TEST_SHIFT_WIRELESS    14
#define AUTO_TEST_SHIFT_SIM         15
#define AUTO_TEST_SHIFT_INFR        16
#define AUTO_TEST_SHIFT_USBH        17
#define AUTO_TEST_SHIFT_USB         18
#define AUTO_TEST_SHIFT_BATTERY     19
#define AUTO_TEST_SHIFT_SCANNER     20
#define AUTO_TEST_SHIFT_LED			21
#define AUTO_TEST_SHIFT_BT          22
#define AUTO_TEST_SHIFT_MAX         AUTO_TEST_SHIFT_BT

// 需要单独测试的模块 最多8个
#define	FAC_CTRL_MODEM			0
#define	FAC_CTRL_WIRELESS		1
#define	FAC_CTRL_BLUETOOTH		2
#define FAC_CTRL_ETHERNET		3
#define FAC_CTRL_SCANNER		4	//扫描头
#define	FAC_CTRL_INFR			5

#define DEFAULT_ASSEMBLE_CNF   ((1<<AUTO_TEST_SHIFT_KEY|(1<<AUTO_TEST_SHIFT_LCD) \
                               |(1<<AUTO_TEST_SHIFT_MAG)|(1<<AUTO_TEST_SHIFT_IC) \
                               )
#define DEFAULT_PRODUCT_CNF   ((1<<AUTO_TEST_SHIFT_KEY|(1<<AUTO_TEST_SHIFT_LCD) \
                               |(1<<AUTO_TEST_SHIFT_MAG)|(1<<AUTO_TEST_SHIFT_IC) \
                               )

//版本对应的宏信息
#define   MANA_VER_MAC          0    //机器名
#define   MANA_VER_MAIN         1    //主板版本
#define   MANA_VER_FLOOR        2	 //底板版本
#define   MANA_VER_BOOT         3	 //BOOT版本
#define   MANA_VER_CTRL         4    //CTRL版本
#define   MANA_VER_FONT         5    //FONT版本
#define   MANA_VER_MAPP         6    //MAOO版本
#define   MANA_VER_FAC          7    //生产测试的版本
#define   MANA_VER_SAPP1        8    //子应用1的版本
#define   MANA_VER_SAPP2        9    //子应用2的版本
#define   MANA_VER_SAPP3        10   //子应用3的版本
#define   MANA_VER_SAPP4        11   //子应用4的版本


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


//自动测试结果的宏
#define AUTO_DEBUG_RES      		0   //板卡自动测试结果
#define AUTO_ASSEMBLE_RES   		1   //装配检自动测试结果
#define AUTO_AGING_RES      		2   //老化测试结果
#define AUTO_PRODUCT_RES    		3   //总检自动测试结果
#define ALL_CHECK_OK    			4   //总结果
#define MANA_CHECK_DATE     		5   //核对日期
#define MANA_CHECK_TAMPER   		6   //核对防拆
#define MANA_CHECK_DELFAC   		7   //核对删除生产测试

/*-----------------------------------------------------------------------------}
 *  new_phase:FAC_PHASE_DEBUG    主板测试结果 D0:1-成功 0-失败 
 *                               背板测试结果 D1:1-成功 0-失败
 *                               D2:1-主板强制  D3:1-背板强制
 *                               D8:主板调试    D9:背板调试
 *
 *            FAC_PHASE_ASSEMBLE 0-未测试 1-成功  2-失败 3-强制成功
 *            FAC_PHASE_AGING    0-未测试 1-成功  2-失败 3-强制成功
 *            FAC_PHASE_PRODUCT  0-未测试 1-成功  2-失败
 *-----------------------------------------------------------------------------{*/
int fac_ctrl_get_test_res(int phase, int id,int *result);
int fac_ctrl_set_test_res(int phase, int id,int result);
uchar fac_ctrl_get_cur_phase(void);
int fac_ctrl_set_cur_phase(int phase);
int fac_ctrl_get_autotest_res(int phase);
int fac_ctrl_set_autotest_res(int phase, int res);
int fac_ctrl_get_MBID(uchar *id);
int fac_ctrl_set_MBID(uint8_t mode,uchar *id);
int fac_ctrl_get_test_detail(int phase, int *value);
int fac_ctrl_set_test_detail(int phase, int value);
int fac_write_aginglog (s_aging_log *log);
int fac_read_aginglog (int *id,s_aging_log *log);
int fac_ctrl_get_fac_testinfo(FAC_TESTINFO *fac_tinfo);
int fac_ctrl_set_fac_testinfo(FAC_TESTINFO *fac_tinfo);
int fac_ctrl_set_fac_ctrl_flag(uchar fac_ctrl_flag);
uchar fac_ctrl_get_fac_ctrl_flag(void);
int  fac_ctrl_set_Voucherno(uchar *id);
int fac_ctrl_get_Voucherno(uchar *id);
int fac_save_log(int mode);
#endif


