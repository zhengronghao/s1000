/*
* =====================================================================================
*
*       Filename:  hcm4003.c
*
*    Description:  软件解码兴平
*
*        Version:  1.0
*        Created:  5/9/2015 
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  ty
*        Company:  START
*
* =====================================================================================
*/
#include "wp30_ctrl.h"
#if (defined CFG_MAGCARD)

#if 11//MAG_PRODUCT == EM_HCM4003 
#include "bitbuf.h"
/*-----------------------------------------------------------------------------}
*  全局变量
*-----------------------------------------------------------------------------{*/


extern uint gMagTeckTimeout;

Mag_TimeInfo MagtimeInfo;  

volatile int          MAG_Swipe_Flag;  //idtech刷卡数据获取成功

Mag_DecodeInfo        *Decode_buf;


/*****************************软件程序****************************************/

/**********************************************************************
* 函数名称： 
*     
* 功能描述： 脉宽数据数组操作函数
*     
* 输入参数： 数据 
*     
* 输出参数：
*     
* 返回值： 如果数据长度不足返回 0
*     
* 注:
*
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
*    
***********************************************************************/
//获取脉宽
ushort get_width(Time_width *width,uchar loca)
{
  if(loca <= width->len)
    return width->data[(width->present + loca)%TIME_DATA_MAX];
  else
    return 0;
}
//数据取出后 指针移动
void present_move(Time_width *width,uchar count)
{
  if(count <= width->len)
  {
    width->present = (width->present + count)%TIME_DATA_MAX;
    width->len -= count;
  }
  else
  {
    //width->present = (width->present + width->data_len)%TIME_DATA_MAX;
    width->len =0;		
  }
}
//插入脉宽
uchar insert_width(Time_width *width,uint value)
{
  if(width->len < TIME_DATA_MAX)
  {
    width->data[(width->present + width->len)%TIME_DATA_MAX] = value;
    width->len++;
    return OK;
  }
  else
  {
    return ERROR;
  }
}



/**********************************************************************
* 函数名称： find_precede
*     
* 功能描述： 寻找前导 0
*     
* 输入参数： 数据 
*     
* 输出参数：
*     
* 返回值： 1-成功  0-失败
*     
* 注:数组至少要有4个数据
*
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
*    
***********************************************************************/
uchar find_precede(Time_width* data)
{
  uchar i, found=0;
  
  for(i=0; i<3; i++) 
  {
    if( (get_width(data,i+1) > ((get_width(data,i)*3)/4)) && (get_width(data,i+1) < ((get_width(data,i)*5)/4)) ) 
    {
      found++;
      if(found >= 3)
        return 1;
    } 
  }
  return 0;
}
/**********************************************************************
* 函数名称： decode_getwidth
*     
* 功能描述：脉宽时间解析为01,并以byte存储
*     
* 输入参数： 脉宽数据 长度
*     
* 输出参数：
*     
* 注: 脉宽数据至少5个才能解析
*
* 返回值： 解后长度
*     
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
*    
***********************************************************************/
ushort decode_getwidth(Mag_DecodeInfo *mag_data)
{
  uint i,  k;
  uint cur_width, cur_width2;
  
  i = 0;
  //滤掉毛刺
  
  if(get_width(&(mag_data->timer_data),i) <(mag_data->sample_width/5)) 
  { 
    cur_width = get_width(&(mag_data->timer_data),i)+ get_width(&(mag_data->timer_data),i+1) + get_width(&(mag_data->timer_data),i+2);
    i+=2;
  } 
  else 
  {
    cur_width = get_width(&mag_data->timer_data,i);
  }
  //滤掉毛刺
  if(get_width(&mag_data->timer_data,i+2) <(mag_data->sample_width/5)) 
  { 
    cur_width2 = get_width(&(mag_data->timer_data),i+1) + get_width(&(mag_data->timer_data),i+2) + get_width(&(mag_data->timer_data),i+3);
    k = 3;
  } 
  else 
  {
    cur_width2 = get_width(&(mag_data->timer_data),i+1);
    k = 1;
  }
  
  if(abs(cur_width + cur_width2 - (mag_data->sample_width)) < abs(cur_width - (mag_data->sample_width)))
  {
     if(mag_data->BitOneNum >= MAG_UINT_LEN)
     {
    	mag_data->BitOneNum = 0;
     }
    (mag_data->buf[(mag_data->BitOneNum)]) |= 1<<(mag_data->byte_bit);
    i+=k;
    (mag_data->sample_width) = ((mag_data->sample_width) + cur_width + cur_width2)/2;
    
  } 
  else 
  {
    if(mag_data->BitOneNum >= MAG_UINT_LEN)
    {
        mag_data->BitOneNum = 0;
    }
    (mag_data->buf[(mag_data->BitOneNum)]) |= 0<<(mag_data->byte_bit);
    (mag_data->sample_width) = ((mag_data->sample_width) + cur_width)/2;
  }
  
  (mag_data->byte_bit)++;
  if((mag_data->byte_bit) == 8)
  {
    (mag_data->byte_bit) =0;
    (mag_data->BitOneNum) ++;
  }
  i++;
  present_move(&(mag_data->timer_data), i);		
  return i;
}

/**********************************************************************
* 函数名称： s_mag_analog2digital
*     
* 功能描述： 模拟信号解为数字信号
*     
* 输入参数： 
*     无
* 输出参数：
*     无
* 返回值： 
*   
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
*   
***********************************************************************/
void  decode_getwidth_end(void)
{
  
  /*   while(MagicInfo.timer_data1.data_len)
  {
  decode_getwidth(&(MagicInfo.timer_data1),&(MagicInfo.byte_bit1),&MagicInfo.sample_width1,&EG_MagOper.k_MSR_buf[0]);
}
  */
  while( Decode_buf[1].timer_data.len)
  {
    decode_getwidth(&Decode_buf[1]);
  }
  while( Decode_buf[2].timer_data.len)
  {
    decode_getwidth(&Decode_buf[2]);
  }

  if(Decode_buf[1].BitOneNum > 2 || Decode_buf[2].BitOneNum > 2)
	{
		MAG_Swipe_Flag = TRUE;
	}
  
  MagtimeInfo.swipe_flag = FALSE;
  
}

//磁道解码
static void mag_decode(Mag_DecodeInfo *mag_data,int now)
{
 if(mag_data->timer_data.len < TIME_DATA_MAX)
    {
      insert_width(&(mag_data->timer_data),now - mag_data->count_last);
	  
      if(mag_data->timer_data.len >= 7)
      {
        if(mag_data->sample_width != 0)
        {
          decode_getwidth(mag_data);
        }
        else
        {
          if(find_precede(&(mag_data->timer_data)))//寻找前导0 初始参考脉宽赋值
          {
            mag_data->sample_width = get_width(&(mag_data->timer_data),2);
			present_move(&(mag_data->timer_data), 1);
          }
          else
          {
            present_move(&(mag_data->timer_data), 1);	
          }
        }
      }			 
      mag_data->count_last = now;
      MagtimeInfo.irq_last = MagtimeInfo.t_irq;
    }
    else
    {
      MagtimeInfo.err |= 0x02; 
    }
}

/******************************软解结束**********************************************************/

/*-----------------------------------------------------------------------------}
*  函数定义
*-----------------------------------------------------------------------------{*/

//二次解码后调用
void hcm4003_reset(void)
{
  int i;
  
  MagtimeInfo.err = 0x00;
  MagtimeInfo.fisrtrun = TRUE;
  MAG_Swipe_Flag = FALSE;
  
  for(i=0;i<3;i++)
  {
  		  Decode_buf[i].BitOneNum = 0;
		  Decode_buf[i].timer_data.len = 0;
		  Decode_buf[i].byte_bit = 0;
		  Decode_buf[i].count_last = 0;
		  Decode_buf[i].sample_width = 0;
    Decode_buf[i].BitOneNum = 0;
    memset(Decode_buf[i].buf, 0, sizeof(Decode_buf[i].buf));
  }
  
}

//IO 控制中调用 
int hcm4003_check(void)
{
  if(MAG_Swipe_Flag == TRUE)
  {
    return 0;  //0 为OK 
  }
  else
  {
    return 1;
  }	
}



//定时器中断处理
void drv_mag_timer_irq(void)
{ 
  
  uint now;    //当前时间
  //char i;
  uchar iolever;
  
  //第一次中断 开启定时器 
  if(MagtimeInfo.fisrtrun == TRUE)
  {   
    MagtimeInfo.fisrtrun = FALSE;
/*
	for(i = 0; i< 3; i++)
		{
		  Decode_buf[i].BitOneNum = 0;
		  Decode_buf[i].timer_data.len = 0;
		  Decode_buf[i].byte_bit = 0;
		  Decode_buf[i].count_last = 0;
		  Decode_buf[i].sample_width = 0;
		}
	MagtimeInfo.irq_last = 0;
	MagtimeInfo.t_irq = 0;
    MagtimeInfo.swipe_flag = FALSE;
    MagtimeInfo.err = 0x00;
   */ 
    LPLD_FTM_Enable_FTM(HCM_TIM); //开启溢出中断    
  }
  //定时器溢出中断
  if(LPLD_FTM_IsTOF(HCM_TIM))
  {
    LPLD_FTM_ClearTOF(HCM_TIM);   //清除中断
    if(MagtimeInfo.t_irq < (MagtimeInfo.irq_last + 20))//一次刷卡最长延时
    {	
      MagtimeInfo.t_irq++;
    }
    else
    { 
      //刷卡结束
      LPLD_FTM_Disable_FTM(HCM_TIM);       //关闭溢出中断
      MagtimeInfo.t_irq = 0;
 
      decode_getwidth_end();  //处理最后几位数据
  
      MagtimeInfo.fisrtrun = TRUE;
    }
  }
  
  now = MagtimeInfo.t_irq * MAGMOD + LPLD_FTM_GetCounter(HCM_TIM);//当前定时器经过的时钟数
  
  //Ch1 捕获中断 磁道2
  if(LPLD_FTM_IsCHnF(HCM_TIM,FTM_Ch1))
  {
    LPLD_FTM_ClearCHnF(HCM_TIM,FTM_Ch1);


	iolever = hw_mag_iolevel_ch2read();
    if ( iolever != Decode_buf[1].io_lever ) 
		{
              mag_decode(&(Decode_buf[1]),now);        //磁道解码		
        }
    	Decode_buf[1].io_lever = iolever;	   
  }
  //Ch2 捕获中断 磁道3
  if(LPLD_FTM_IsCHnF(HCM_TIM,FTM_Ch2))
  {
    LPLD_FTM_ClearCHnF(HCM_TIM,FTM_Ch2);
	iolever = hw_mag_iolevel_ch3read();
    if ( iolever != Decode_buf[2].io_lever ) 
		{
            mag_decode(&(Decode_buf[2]),now);
        }
		Decode_buf[2].io_lever = iolever;
  }
}

void hcm4003_Init(void)
{
    hw_mag_power_init(HCM_CS_GPIO, HCM_POWER_PINx);
}

void hcm4003_enable_VCC(void)
{
    hw_mag_power_open();
}

void hcm4003_disable_VCC(void)
{	
    hw_mag_power_close();
}

int mag_hcm4003_init(int mode)
{
  //IO 定时器 初始化
  FTM_InitTypeDef ftm_init_struct;

//  hw_set_gpio_output(HCM_CS_GPIO,HCM_CS_PIN);
//  hw_gpio_set_bits(HCM_CS_GPIO,HCM_CS_PIN);
  hcm4003_Init();
  hcm4003_disable_VCC();

  memset((char *)&ftm_init_struct,0,sizeof(ftm_init_struct));
  ftm_init_struct.FTM_Ftmx = HCM_TIM;             //定时器0
  ftm_init_struct.FTM_Mode = FTM_MODE_IC;     //输入捕获模式
  ftm_init_struct.FTM_ClkDiv = FTM_CLK_DIV2;   //分频
  ftm_init_struct.FTM_ToiEnable = TRUE;        //使能计数溢出中断
  ftm_init_struct.FTM_Isr = drv_mag_timer_irq; //中断处理函数
  
  LPLD_FTM_Init(ftm_init_struct);  //初始化
  
  //LPLD_FTM_IC_Enable(HCM_TIM,FTM_Ch1,PTC2,CAPTURE_RIFA); //设置PTC2、3上升下降沿捕获
  //LPLD_FTM_IC_Enable(HCM_TIM,FTM_Ch2,PTC3,CAPTURE_RIFA);
  
  LPLD_FTM_Disable_FTM(HCM_TIM);
  
  set_irq_priority (FTM0_IRQn, INT_PRT_FTM_0);  //设置中断优先级
  
  enable_irq(FTM0_IRQn);


  Decode_buf = (Mag_DecodeInfo *)pMagBufTmp0; //分配磁卡使用的空间

  return 0;
}


//关闭模块 拉高 PD6引脚
int mag_hcm4003_close(int mode)
{
  int i;
  
//  hw_gpio_set_bits(HCM_CS_GPIO,HCM_CS_PIN);
  hcm4003_disable_VCC();
  LPLD_FTM_DisableChn(HCM_TIM,FTM_Ch1);
  LPLD_FTM_DisableChn(HCM_TIM,FTM_Ch2);    

  MagtimeInfo.err = 0x00;
  MagtimeInfo.fisrtrun = TRUE;
  MAG_Swipe_Flag = FALSE;
  
  for(i=0;i<3;i++)
  {
  		  Decode_buf[i].BitOneNum = 0;
		  Decode_buf[i].timer_data.len = 0;
		  Decode_buf[i].byte_bit = 0;
		  Decode_buf[i].count_last = 0;
		  Decode_buf[i].sample_width = 0;
    Decode_buf[i].BitOneNum = 0;
    memset(Decode_buf[i].buf, 0, sizeof(Decode_buf[i].buf));
  }
  
  return 0;
}
//开启模块时 拉低 PD6引脚
int mag_hcm4003_open(int mode)
{ 
//  hw_gpio_reset_bits(HCM_CS_GPIO,HCM_CS_PIN);
    hcm4003_enable_VCC();
 TRACE("\r\nHCM4003 Open");
  MagtimeInfo.fisrtrun = TRUE;
  
  LPLD_FTM_IC_Enable(HCM_TIM,FTM_Ch1,PTC2,CAPTURE_RIFA); //设置PTC2、3上升下降沿捕获
  LPLD_FTM_IC_Enable(HCM_TIM,FTM_Ch2,PTC3,CAPTURE_RIFA);     

  hcm4003_reset();

  
  return 0;
}
int mag_hcm4003_ioctl(int mode,int value)
{
  if ( mode == 0 ) {
    //check
    return (int)hcm4003_check();
  }else if ( mode == 1 ) {
    //clear
    hcm4003_reset();
  }
  return 0;
}

//返回值  magret[32:0]        [6:4]-3个磁道失败标志 [2:0]-3个磁道成功标志
int mag_hcm4003_read(void *Track1, void *Track2, void *Track3)
{
  BitBuf SrcBitBuf;
  uchar ret;
  uchar Track1Len ,Track2Len ,Track3Len ;
  int magret = 0;
  uchar  *ptrk;
  
  Track1Len = Track2Len = Track3Len =0;
  Track1Len = Track1Len;
  
  if(MagtimeInfo.err)
  {
    hcm4003_reset();
    return magret;
  }
  
  if (MAG_Swipe_Flag == FALSE)
  {
    // 未刷卡
    magret  = ((1<<6) | (1<<5) ) | (1<<4);
    // DISPPOS(magret);
    return magret;
  }
/*
   int i;
TRACE("\r\nTrk2:%d\r\n",Decode_buf[1].BitOneNum);
for(i = 0;i <Decode_buf[1].BitOneNum; i++)
{
  TRACE("%02X,",Decode_buf[1].buf[i]);
}
TRACE("\r\nTrk3:%d\r\n",Decode_buf[2].BitOneNum);
for(i = 0;i <Decode_buf[2].BitOneNum; i++)
{
  TRACE("%02X,",Decode_buf[2].buf[i]);
}
*/
  // Trk2 decode
  ret = EM_mag_NULL;
  ptrk = Track2;
  if(NULL != ptrk) {
    if(Decode_buf[1].BitOneNum)
    {
      bitbufInit(&SrcBitBuf, Decode_buf[1].buf, MAG_UINT_LEN * 8);
      ret = EI_mag_vProcTrk23(&SrcBitBuf, (Decode_buf[1].BitOneNum+1)*8, &Track2Len, ptrk, 1);	// forward
      TRACE("\r\n Trk2F:%X", ret);
      if (ret != EM_mag_SUCCESS)
      {
        ret = EI_mag_vProcTrk23(&SrcBitBuf, (Decode_buf[1].BitOneNum+1)*8, &Track2Len, ptrk, 0);	// backward
        TRACE("\r\n Trk2B:%X", ret);
      }
      if (ret == EM_mag_SUCCESS)
      {
        // DISPBUFTITLE("trk2",Track2Len,0,ptrk);TRACE("\r\n trk2:%s",ptrk);
        magret  |= (1<<1);
      }
    }
    if(ret)
    {
      magret  |= (1<<5);  //磁道2错误
      Track2Len = 0;
    }
  }else{
      magret  |= (1<<5);  //磁道2错误
      Track2Len = 0;
  }
  //Trk3 decode
  ret = EM_mag_NULL;
  ptrk = Track3;
  if(NULL != ptrk) {
    if(Decode_buf[2].BitOneNum)
    {
      bitbufInit(&SrcBitBuf, Decode_buf[2].buf, MAG_UINT_LEN * 8);
      ret = EI_mag_vProcTrk23(&SrcBitBuf, (Decode_buf[2].BitOneNum+1)*8, &Track3Len, ptrk, 1);	// forward
      TRACE("\r\n Trk3F:%X", ret);
      if (ret != EM_mag_SUCCESS)
      {
        ret = EI_mag_vProcTrk23(&SrcBitBuf, (Decode_buf[2].BitOneNum+1)*8, &Track3Len, ptrk, 0);	// backward
         TRACE("\r\n Trk3B:%X", ret);
      }
      if (ret == EM_mag_SUCCESS)
      {
        //            DISPBUFTITLE("trk3",Track3Len,0,ptrk);TRACE("\r\n trk3:%s",ptrk);
        magret  |= (1<<2);
      }
    }
    if(ret)
    {
      magret  |= (1<<6);
      Track3Len = 0;
    }
  }else{
      magret  |= (1<<6);
      Track3Len = 0;
  }
  
  hcm4003_reset();
  magret |= ((Track3Len<<24) | (Track2Len<<16) ); //(Track1Len<<8)
  return magret;
}
/* 
* magteck_main - [GENERIC] 
* @ 
*/
int mag_hcm4003_main (int mode)
{

  return 0;
}
#endif
#endif
