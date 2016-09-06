#include "wp30_ctrl.h"
//#define CFG_MB_TESTER
#ifdef CFG_MB_TESTER
#include "boardtest.h"
#include "S1000_boardtest.h"


extern STRUCT_PIN_LIST gMagPinlist[1];
extern STRUCT_PIN_LIST gKeyPinlist[8];
extern STRUCT_PIN_LIST gSAMPinlist[3];
extern STRUCT_PIN_LIST gICPinlist[5];
extern STRUCT_FAC_MODULE gDeviceModuleList[MODULE_AMOUNT];

/*-----------------------------------------------------------------------------}
 *  宏定义
 *-----------------------------------------------------------------------------{*/
#define FAC_COM_USBD		0xfe
#define FAC_COM_USBH		0xff
#define FAC_DEV_WORK_COM   UART_COM1        //测试主机工作端口
//#define FAC_DEV_WORK_COM  FAC_COM_USBD   //测试主机工作端口
#define FAC_DEV_WORK_BPS  "115200,8,n,1"   //测试主机波特率
#define FAC_DEV_DBG_COM   FAC_COM_USBD    //测试主机debug端口
#define FAC_DEV_DBG_BPS   "115200,8,n,1"   //测试主机工作端口
/*-----------------------------------------------------------------------------}
 *  测试协议:
 *                        HOST            DEVICE
 *  1(主机与主板握手)      P     -->       p
 *  2(主板下载BOOT)
 *  3(主板下载CTRL)
 *  4(主机与测试流程握手) 
 *  5(主机开始测试)       采用PCD协议: 02+len(2B,MSB+LSB)+CID(DF+00/01)+Data+ Lrc(CID+Data区) + 03
 *-----------------------------------------------------------------------------{*/
#define  FAC_SHAKETEST_CHAR       'P'    //上电握手 大写P
#define  FAC_SHAKETEST_CHAR_ACK   'p'    //小写p

#define MASK_TESTPIN		 (1 <<0)
#define MASK_TESTFUNC		 (1 << 1)

#define  SET_TESTER_STAT(status)  do{gFacTesterInfo.stat = status;}while(0)
#define  GET_TESTER_STAT()  gFacTesterInfo.stat

/*-----------------------------------------------------------------------------}
 *  函数声明
 *-----------------------------------------------------------------------------{*/
int fac_Devices_shake_test(int mode);
int fac_Devices_maintest(void *param);
static int fac_function_test_demo(void *module);
int fac_send(uchar *data, uint len);
STRUCT_FAC_MODULE *getModuleById(uchar moduleId);

/*-----------------------------------------------------------------------------}
 *  全局变量
 *-----------------------------------------------------------------------------{*/
STRUCT_WORKINFO gFacTesterInfo;
// 主机端操作方法
const STRUCT_FAC_OPT gTesterHostOptList[] = 
{
    {
	    0,0,
	    "S1000 Tester device",
	    fac_init,
	    NULL,
            fac_Devices_shake_test,
	    NULL,
	    fac_dl_boot,
	    NULL,
	    fac_dl_ctrl,//DownLoadCode
	    NULL,
	    fac_dl_font,//DownLoadCode
	    NULL,
	    fac_Devices_maintest,
	},

};

/*-----------------------------------------------------------------------------}
 *  函数定义
 *-----------------------------------------------------------------------------{*/
void boardtest_set_pio_output(int pin, uchar val)
{
	gpio_set_output((GPIOPin_Def)pin, GPIO_OUTPUT_OpenDrainDisable, val);
}

void boardtest_set_pio_input(int pin)
{
	gpio_set_input((GPIOPin_Def)pin,GPIO_INPUT_PULLDISABLE);
}

static void boardtest_module_input(uchar moduleId)
{
	uchar i;
	STRUCT_FAC_MODULE *pt_module = NULL;
	
	pt_module = getModuleById(moduleId);
	if (pt_module!= NULL)
	{
		for(i = 0; i < pt_module->listnum; i++)
		{
            if ( pt_module->pinList != NULL ) {
                boardtest_set_pio_input(pt_module->pinList[i].pin);
            }
		}
	}
}

int testcom_printf(int uart_id, const char *format, ...)
{
	int       count=0;
	va_list     marker;
	char        buff[256 + 4];	/* 1K 就够了，8K太浪费 */

	if (format == NULL)
    {
        return -EUART_VAL;
    }

	memset(buff, 0, sizeof(buff));
	va_start( marker, format);
	count = vsnprintf(buff, sizeof(buff)-4, format, marker);
	va_end( marker );
	if(count == -1)
	{
		count = sizeof(buff)-4;
	}

	if (uart_id == FAC_COM_USBH)
    {
//	    return usbhserial_write(UART_USB, (uchar*)buff, count);
    }
	else if (uart_id == FAC_COM_USBD)
	{
//		return usbdserial_write((uchar*)buff, count);
	}
	else
    {
	    return uart_Write(uart_id, buff, count);
    }
    return 0;
}

int testcom_clear(int uart_id)
{
    if (uart_id == FAC_COM_USBH)
    {
//        return usbhserial_flush(UART_USB);

    }
	else if (uart_id == FAC_COM_USBD)
	{
//		return usbdserial_clear();
	}
	else
    {
        return uart_Flush(uart_id);
    }
    return 0;
}

int testcom_close(int uart_id)
{
    if (uart_id == FAC_COM_USBH)
    {
//        return usbhserial_close(UART_USB);
    }
	else if (uart_id == FAC_COM_USBD)
	{
//		   return usbdserial_close();
	}
	else
    {
        return uart_Close(uart_id);
    }
    return 0;
}

int testcom_open(int uart_id, const char *uart_para)
{
    if (uart_id == FAC_COM_USBH)
    {
//        return usbhserial_open();
    }
	else if (uart_id == FAC_COM_USBD)
	{
//		   return usbdserial_open();
	}
	else
    {
        return uart_Open(uart_id, uart_para);
    }
  return 0;
}

int testcom_ready(int uart_id)
{
	if (uart_id == FAC_COM_USBD)
	{
//		return usbdserial_ready();
	}
	else
	{
		return 0;
	}
	
	return 0;
}

int testcom_write(int uart_id, const void *inbuf, int bytelen)
{
    if (uart_id == FAC_COM_USBH)
    {
//        return usbhserial_write(UART_USB, (void *)inbuf, bytelen);
    }
	else if (uart_id == FAC_COM_USBD)
	{
//		return usbdserial_write((void *)inbuf, bytelen);
	}
	else
    {
        return uart_Write(uart_id, inbuf, bytelen);
    }
    return 0;
}

int testcom_read(int uart_id, void *outbuf, int bytelen, int timeout_ms)
{
    if (uart_id == FAC_COM_USBH)
    {
//        return usbhserial_read(UART_USB, outbuf, bytelen, timeout_ms);
    }
	else if (uart_id == FAC_COM_USBD)
	{
//		return usbdserial_read(outbuf, bytelen, timeout_ms);
	}
	else
    {
        return uart_Read(uart_id, outbuf, bytelen, timeout_ms);
    }
    return 0;
}


/*
static
int testcom_check_writebuf(int uart_id)
{
    if (uart_id == UART_USB)
    {
        return 0;
    } else
    {
        return uart_check_writebuf(uart_id);
    }
}
*/

int testcom_check_readbuf(int uart_id)
{
	if (uart_id == FAC_COM_USBH)
    {
//        return usbhserial_check_read_buffer(UART_USB);
    }
	else if (uart_id == FAC_COM_USBD)
	{
//    		return usbdserial_check_readbuf();
	}
	else
    {
 //    return uart_check_readbuf(uart_id);
       return uart_CheckReadBuf(uart_id);
    }
    return 0;
}

static uchar boardtest_makeLrc(uchar check, uchar *buf, uint len)
{
	//采用LRC
	uchar *p, i;
	p = (uchar *) buf;
	for (i = 0; i < len ; i++)
	{
		check ^= *p++;
	}
	return check;
}

#ifdef FAC_TESTER_DBG
#define TRACE_BTEST(...)		testcom_printf(gFacTesterInfo.dbgcom, __VA_ARGS__)
#else
#define TRACE_BTEST(...)		do{}while(0)
#endif

/*******************************************************************
功能:发送数据
********************************************************************/
int fac_send(uchar *data, uint len)
{
	int i;
    i = 0;
    uchar sendBuf[64];

    sendBuf[i++]=FAC_STX;
	//长度, 包含了lrc校验码和结尾标识
    sendBuf[i++]=HBYTE(len+2);
    sendBuf[i++]=LBYTE(len+2);

    memcpy(&sendBuf[i],data,len);
    i += len;
    sendBuf[i]=boardtest_makeLrc(0,sendBuf, i);
	i++;
    sendBuf[i++]=FAC_ETX;
	testcom_write(gFacTesterInfo.workcom, sendBuf, i);
    return 0;
}

/*
 * fac_receive - [GENERIC] 信息交互
 * 握手协议:            HOST(先)         DEVICE
 *   (1)主板握手      'T'       <->     't'
 *   (2)测试握手      'C'       <->     'c'
 *   (3)测试过程      采用PCD协议: 02 + len(2B) + Data + Lrc(Data区) + 03
 * 返回值: 0-成功 1-超时 2-收到错误数据
 * outbuf - 输出的纯净数据data, outlen - data的长度
 * @
 */
int fac_receive (uint *outlen, uchar *outbuf, uint timeout)
{
 //   int ret;
    uchar buf[64];
	int recvLen = 0;
	uchar lrc;
    uint len;

	if(!testcom_read(gFacTesterInfo.workcom, buf, 1, timeout))
    {
        return 1;
    }
    if (buf[0] == FAC_SHAKETEST_CHAR)
    {
		//收到握手信号
		outbuf[0] = buf[0];
		*outlen = 1;
        return 0;
    }
	else if(buf[0] == FAC_STX)
	{
		recvLen++;
		len = testcom_read(gFacTesterInfo.workcom, &buf[recvLen],2,500);
        if(len < 2)
        {
            return 3;
        }
        len = COMBINE16(buf[recvLen],buf[recvLen+1]);
		recvLen += 2;
        if (testcom_read(gFacTesterInfo.workcom, &buf[recvLen],len,500) < (len))
		{
            return 4;
        }
		//去除掉头尾
        memcpy(outbuf,&buf[recvLen],len - 2);
		recvLen += (len);
		//做校验,
		lrc = boardtest_makeLrc(0,buf,recvLen - 2);

        if (lrc != buf[recvLen - 2]) {
            return 5;
        }
        /**/

        *outlen = len - 2;
        return 0;
    } else {
        return 2;
    }
}		/* -----  end of function fac_exchage  ----- */


/**************************************************************************************
*   功能:接收上电时握手信号，接收到，发响应信号
*
*	返回值:
			异常时小于0;
			0 - 成功
			1 - 失败
***************************************************************************************/
int fac_Devices_shake_test(int mode)
{
    int ret = 1;
    uint len;
    uchar buf[4];

	while (1)
	{
		//接收握手信号
	    ret = fac_receive(&len,&buf[0], 500);
	    if (!ret)
		{
	        if ( buf[0] == FAC_SHAKETEST_CHAR)
			{
				//回应ack
				buf[0] = FAC_SHAKETEST_CHAR_ACK;
				testcom_write(gFacTesterInfo.workcom, buf, 1);
				sys_delay_ms(1500);
				
	            return 0;
	        }
	    }
		else
		{
			return 1;
		}
	 }
}

/*******************************************************
功能测试的模块，它们的口线初始化，都设为输入态。
*******************************************************/
int ModulePinInit()
{
	int i = 0;
	int inum = 0;

	inum = gFacTesterInfo.moduleNum;
	for(i=0; i<inum; i++)
	{
		if(gFacTesterInfo.pModulelist[i].function_test_func == NULL)//功能模块的引脚都设为输入
		{
			boardtest_module_input(gFacTesterInfo.pModulelist[i].Moduleid);
		}
	}
        
    return 0;
}


/*******************************************************
功能测试的模块，它们的口线初始化，都设为输出0或输出1。
*******************************************************/
void tester_module_output(STRUCT_PIN_LIST *pin, int len, uchar value)
{
    int i;
	STRUCT_PIN_LIST * p = pin;
//    ver = s_getProduct();
	
    for(i=0;i<len;i++)
    {
    	if(p[i].mode & 0x80)
    	{
            boardtest_set_pio_output(p[i].pin, !value);
    	}
		else
		{
            boardtest_set_pio_output(p[i].pin, value);
		}
    }
}

/*******************************************************
非功能测试的模块，它们的口线初始化.三种模式设置

Mode: 0x01 -- 输入
      0x02 -- 输出低电平
      0x03 -- 输出高电平电平
*******************************************************/
int UnModulePinInit(uchar Mode)
{
	int i = 0;
	int inum = 0;

	inum = gFacTesterInfo.moduleNum;
	for(i=0; i<inum; i++)
	{
		//如果主机端与设备端没有通过命令进行测试的模块，这些模块的引脚根据发送过来的命令进行操作。
		if(gFacTesterInfo.pModulelist[i].function_test_func == NULL)
		{

			switch(Mode)
			{
				case 0x01:  //设置为输入
							boardtest_module_input(gFacTesterInfo.pModulelist[i].Moduleid);
						    break;
				case 0x02:
							//设置为输出0
							tester_module_output(gFacTesterInfo.pModulelist[i].pinList, gFacTesterInfo.pModulelist[i].listnum, 0);
						    break;
				case 0x03:
							//设置为输出1
							tester_module_output(gFacTesterInfo.pModulelist[i].pinList, gFacTesterInfo.pModulelist[i].listnum, 1);
						    break;
				default:break;
			}
		}
	}	

	return 0;
}

STRUCT_FAC_MODULE *getModuleById(uchar moduleId)
{
	int i;
	STRUCT_FAC_MODULE *pt_module = NULL;
	for (i = 0; i < gFacTesterInfo.moduleNum; i++)
	{
		if (gFacTesterInfo.pModulelist[i].Moduleid == moduleId)
		{
			pt_module = (STRUCT_FAC_MODULE *)&(gFacTesterInfo.pModulelist[i]);
			break;
		}
	}
	return pt_module;
}

STRUCT_PIN_LIST *getPinById( ushort pinId)
{
	int i,j;
	STRUCT_FAC_MODULE *pt_module = NULL;
	STRUCT_PIN_LIST   *pt_pin = NULL;
	for (j = 0; j < gFacTesterInfo.moduleNum; j++)
	{
		pt_module = (STRUCT_FAC_MODULE *)&(gFacTesterInfo.pModulelist[j]);
		for (i = 0; i < pt_module->listnum; i++)
		{
	//		lcd_display(0,48,FONT_SIZE12 | DISP_CLRLINE,"i%d, pinId:%x, %x",i, pinId, pt_module->pinList[i].pinid);
	//		kb_getkey(-1);
			if (pinId == pt_module->pinList[i].pinid)
			{
				pt_pin = (STRUCT_PIN_LIST   *)&(pt_module->pinList[i]);
				break;
			}
		}
	}
	return pt_pin;
}

int fac_functionTest(uchar moduleId)
{
    int ret = 1;
    STRUCT_FAC_MODULE *pt_module = NULL;
	
	pt_module =getModuleById(moduleId);
	if (pt_module == NULL)
	{
		return -1;
	}
	if (pt_module->function_test_func != NULL)
	{
		 ret = pt_module->function_test_func(pt_module);
	}
	return ret;
}

int fac_pinHandleTest(uchar moduleId, ushort pinId, int pinMode)
{
	STRUCT_FAC_MODULE *pt_module = NULL;
	STRUCT_PIN_LIST   *pt_pin = NULL;
	int i,mode;
	uchar buf[16];

	pt_module =getModuleById(moduleId);
	if (pt_module == NULL)
	{
		i = 0;
		buf[i++] = 0x00;
		buf[i++] = 0x01;
		fac_send(buf, i);
		
		return -1;
	}

	pt_pin = getPinById(pinId);
	if (pt_pin == NULL)
	{
		i = 0;
		buf[i++] = 0x00;
		buf[i++] = 0x01;
		fac_send(buf, i);
		
		return -1;
	}

    if(pinMode & (1<< 4))
    {
	     mode = 1; //口线结束测试
		 pt_module->pin_test_func(pt_pin,&mode);
	}
	else if(pinMode &(1 << 5))
	{
		if (pinMode &(1 << 6))
		{
                boardtest_set_pio_output(pt_pin->pin, 1);
                s_DelayMs(2);	
        }
		else
		{
                boardtest_set_pio_output(pt_pin->pin, 0);
                s_DelayMs(2);	
        }
	}
	else
	{
		boardtest_set_pio_input(pt_pin->pin);
	}

	i = 0;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	
	fac_send(buf, i);

	return 0;
}

int fac_pinTest(uchar moduleId, ushort pinId, int pinMode)
{
	STRUCT_FAC_MODULE *pt_module = NULL;
	STRUCT_PIN_LIST   *pt_pin = NULL;
//	STRUCT_PIN_LIST   *pt_prePin = NULL;
	int i,mode;
	uchar buf[16];
	
	pt_module =getModuleById(moduleId);
	if (pt_module == NULL)
	{
		i = 0;
		buf[i++] = 0x00;
		buf[i++] = 0x01;
		fac_send(buf, i);
		
		return -1;
	}
	pt_pin = getPinById(pinId);
	if (pt_pin == NULL)
	{
		i = 0;
		buf[i++] = 0x00;
		buf[i++] = 0x02;
		fac_send(buf, i);
		
		return -1;
	}

	if (pinMode & (1 << 4))
	{
		//特殊流程
		if (pt_module->pin_test_func != NULL)
		{
            mode = 0;
			pt_module->pin_test_func(pt_pin,&mode);
		}
	}
	else
	{
		//通用流程
		if (pinMode & (1 << 0))
		{
                boardtest_set_pio_output(pt_pin->pin, 1);
                s_DelayMs(1);
        }
		else
		{
               boardtest_set_pio_output(pt_pin->pin, 0);
                s_DelayMs(1);
		}

    }
    
    i = 0;
    buf[i++] = 0x00;
    buf[i++] = 0x00;

    fac_send(buf, i);

	return 0;
}


/***************************************************************************
设备端的针床测试主入口:接收各个命令进行处理

****************************************************************************/
int fac_Devices_maintest(void *param)
{
	int ret;
	uint len;
	uchar buf[100];
	uchar sendbuf[100];
	uchar cidl;
	uchar moduleId = -1;
	uchar InitMode = -1;
	uchar uResult = -1;
	ushort pinId = -1;
//	ushort pre_pinId = 0;
	int   moduleMode = 0;
	ushort  pinMode = 0;
	int   count = 0;
	uchar uvalue = 0;
	int i = 0;
   
    uchar backbuf[SYSZONE_LEN];
	uchar des_key[8];
    char HostMainId[20] = {0};
	char DeviceMainId[20] = {0};
	int iflg = 0;
	uchar iReturn = 0;
	
	while (1)
	{
		//接收测试信号
        ret = fac_receive(&len,&buf[0], -1);
		count++;
		if ( !ret )
		{
			if (buf[0] == FAC_CIDH)
			{
				cidl = buf[1];
				switch (cidl)
				{
					case FAC_CIDL_UNMODLEPININIT:
					{
						InitMode = buf[2];
						UnModulePinInit(InitMode);
						i = 0;
						sendbuf[i++] = 0x00;
						sendbuf[i++] = 0x00;
						fac_send(sendbuf, i);
						break;
					}
					case FAC_CIDL_TESTMODULE:
					{
						moduleId = buf[2];
						moduleMode = buf[3];
						i = 0;
						sendbuf[i++] = 0x00;
						sendbuf[i++] = 0x00;
						fac_send(sendbuf, i);
						break;
					}
					case FAC_CIDL_TESTPIN:
					{
						if (moduleMode & MASK_TESTPIN)
						{
							//口线测试流程
							pinId = COMBINE16(buf[2] ,buf[3]);
							pinMode = COMBINE16(buf[5] ,buf[6]);
//							pre_pinId = COMBINE16(buf[7] ,buf[8]);
//							fac_pinTest(moduleId, pinId, pinMode, pre_pinId);
							fac_pinTest(moduleId, pinId, pinMode);
						}
						break;
					}
					case FAC_CIDL_HANDLE_PIN:
					{
						if (moduleMode & MASK_TESTPIN)
						{
							pinId = COMBINE16(buf[2] ,buf[3]);
							pinMode = COMBINE16(buf[5] ,buf[6]);
							fac_pinHandleTest(moduleId, pinId, pinMode);
						}
						break;
					}
					case FAC_CIDL_TESTFUNCTION:
					{
						if ((moduleMode & MASK_TESTFUNC) != 0)
						{
							//特殊测试流程
                            i = 0;
    						memset(buf, 0, sizeof(buf));
    						buf[i++] = 0x00;
    						buf[i++] = fac_functionTest(moduleId);
    						fac_send(buf, i);
						}
						break;
					}
					case FAC_CIDL_ENDTEST://结束测试
					{
//						uchar uflag = 0;
						uResult = buf[2];
						if(uResult == 0x00) //板卡检写成功的标识
						{
						  uvalue = fac_ctrl_set_autotest_res(FAC_PHASE_DEBUG, FAC_CTRL_SUCCESS);
//						  uflag = 0x02;
						}
						else//板卡检写失败的标识
						{
						  uvalue = fac_ctrl_set_autotest_res(FAC_PHASE_DEBUG, FAC_CTRL_FAIL);
//						  uflag = 0x03;
						}
						i = 0;
						sendbuf[i++] = 0x00;
						if(uvalue == 0)
						{
						  sendbuf[i++] = 0x00;
						}
						else
						{
						  sendbuf[i++] = 0x01;
						}
//						sendbuf[i++] = uflag;
//						sendbuf[i++] = uvalue;
						fac_send(sendbuf, i);

						return 0;
					}
					case FAC_CIDL_ENDMODULETEST:
					{
						i = 0;
						sendbuf[i++] = 0x00;
						sendbuf[i++] = 0x00;
						fac_send(sendbuf, i);
						break;
					}
					case FAC_CIDL_WRITE_ID:
						 
						 memcpy(HostMainId, &buf[2], len-2);
						 iflg = fac_ctrl_set_MBID(1, (uchar *)HostMainId);
						 if(iflg == 0)//主板ID不存在
						 {
						 	fac_ctrl_get_MBID((uchar *)DeviceMainId);
							if(memcmp(HostMainId, DeviceMainId, sizeof(DeviceMainId)) == 0 )
							{
								iReturn = 0x00;//成功
							}
							else
							{
								iReturn = 0x01;//失败
							}
						 }
						 else
						 {
						 	fac_ctrl_get_MBID((uchar *)DeviceMainId);
						 }
						 //生成exflash 密码 写入系统信息区
                         RNG_FillRandom(des_key,sizeof(des_key));
                         s_write_syszone(backbuf,SYS_EXFLASH_KEY_ADDR,SYS_EXFLASH_KEY_SIZE,des_key);

						 i = 0;
						 sendbuf[i++] = 0x00;
						 sendbuf[i++] = iReturn;
						 memcpy(&sendbuf[i], DeviceMainId, sizeof(DeviceMainId));
						 i = i + sizeof(DeviceMainId);
						 fac_send(sendbuf, i);	
						 
						 break;
					default:
					{
						 break;
					}
				}
		    }
		}
	}
}

 /*----------------------------------------------------------------------------
 * 设备端测试初始化
 *----------------------------------------------------------------------------*/
int tester_Devices_init(void)
{
    memset((char *)&gFacTesterInfo,0,sizeof(gFacTesterInfo));
    gFacTesterInfo.workcom = FAC_DEV_WORK_COM;
    gFacTesterInfo.workbps = FAC_DEV_WORK_BPS;
    gFacTesterInfo.dbgcom = FAC_DEV_DBG_COM;
    gFacTesterInfo.dbgbps = FAC_DEV_DBG_BPS;
	gFacTesterInfo.pOpt   =  (STRUCT_FAC_OPT *)&(gTesterHostOptList[0]);

	gFacTesterInfo.pModulelist = gDeviceModuleList;
	gFacTesterInfo.moduleNum = DIM(gDeviceModuleList);

    return 0;
}/* -----  end of function tester_host_init  ----- */

/****************************************************************************
设备端测试主函数:
*****************************************************************************/
int fac_mb_main (int mode)
{
    int ret = -1;
    uchar buf[128];
	uchar sendbuf[128+32];
	int len;
	uchar cidL;
	uchar shakeMode;
	int i = 0;
    const STRUCT_FAC_OPT *ptFacOpt;

	tester_Devices_init();
	ptFacOpt = gFacTesterInfo.pOpt;

#ifdef FAC_TESTER_DBG
    testcom_open((uint *)gFacTesterInfo.dbgcom,gFacTesterInfo.dbgbps);
#endif

	//打开通讯端口
	testcom_close(gFacTesterInfo.workcom);
    testcom_open(gFacTesterInfo.workcom,gFacTesterInfo.workbps);
    testcom_clear(gFacTesterInfo.workcom);

	while (testcom_ready(gFacTesterInfo.workcom) != 0);
	
	//上电时握手
	SET_TESTER_STAT(FAC_STAT_INIT);
	if (ptFacOpt->shake_func != NULL)
	{	
		lcd_display(0,0,FONT_SIZE12 | DISP_CLRLINE,"握手");
    	ret = ptFacOpt->shake_func(0);
	}

	if (ret != 0)
	{
		testcom_clear(gFacTesterInfo.workcom);
		testcom_close(gFacTesterInfo.workcom);
#ifdef FAC_TESTER_DBG
    	testcom_close(gFacTesterInfo.dbgcom);
#endif
		return 0;

	}

	ModulePinInit();//初始化口线应放到握手成功后
	SET_TESTER_STAT(FAC_STAT_SHAKE_HOST);
	
	while (1)
	{
		ret = fac_receive((uint *)&len,&buf[0], -1);
		if (!ret)
		{
			if (buf[0] == FAC_CIDH)
			{
				cidL = buf[1];
				if(cidL == FAC_CIDL_SHAKE)
				{
					shakeMode = buf[2];
					switch(shakeMode)
                    {
                    case FAC_SHAKE_TEST:
                        ret = ptFacOpt->init_func(0); //调用测试初始化函数
                        i = 0;
                        sendbuf[i++] = 0x00;
                        sendbuf[i++] = 0x00;
                        fac_send(sendbuf, i);				

                        SET_TESTER_STAT(FAC_STAT_TESTING);
                        if (ptFacOpt->maintest_func != NULL)
                        {
                            ret = ptFacOpt->maintest_func(NULL);
                        }

                        break;
                    case FAC_SHAKE_BOOT:
                        i = 0;
                        sendbuf[i++] = 0x00;
                        sendbuf[i++] = 0x00;
                        fac_send(sendbuf, i);	

                        SET_TESTER_STAT(FAC_STAT_DL_BOOT);
                        if (ptFacOpt->dlboot_func!= NULL)
                        {
                            ret = ptFacOpt->dlboot_func(NULL);
                        }

                        break;
                    case FAC_SHAKE_CTRL:
                        i = 0;
                        sendbuf[i++] = 0x00;
                        sendbuf[i++] = 0x00;
                        fac_send(sendbuf, i);	

                        SET_TESTER_STAT(FAC_STAT_SHAKE_CTRL);
                        if (ptFacOpt->dlctrl_func!= NULL)
                        {
                            ret = ptFacOpt->dlctrl_func(NULL);
                        }
                        break;
                    case FAC_SHAKE_FONT:
                        i = 0;
                        sendbuf[i++] = 0x00;
                        sendbuf[i++] = 0x00;
                        fac_send(sendbuf, i);

                        SET_TESTER_STAT(FAC_STAT_SHAKE_CTRL);
                        if (ptFacOpt->dlfontlib_func!= NULL)
                        {
                            ret = ptFacOpt->dlfontlib_func(NULL);
                        }
                        break;
                    case FAC_SHAKE_INFO:
                        i = 0;
                        sendbuf[i++] = 0x00;
                        sendbuf[i++] = 0x00;
                        sendbuf[i++] = k_MainVer.main;
                        sendbuf[i++] = k_MainVer.secondary; 
                        sendbuf[i++] = k_MainVer.revise;

                        sendbuf[i++] = 0x00; //pb版本
                        sendbuf[i++] = 0x00;
                        sendbuf[i++] = 0x00;

                        fac_send(sendbuf, i);	
                        SET_TESTER_STAT(FAC_STAT_SHAKE_INFO);

                        break;
                    default:break;
                    }
				}
			}	
		 }
		 memset(buf, 0x00, sizeof(buf));
	  }
			
}		/* -----  end of function fac_mb_main  ----- */


#endif

