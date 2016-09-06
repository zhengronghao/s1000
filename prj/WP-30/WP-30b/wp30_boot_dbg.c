/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : wp30_boot_dbg.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/22/2014 11:16:54 AM
 * Description        : 
 *******************************************************************************/
#include "wp30_boot.h"
#include "wp30_boot_dbg.h"
#include "../download/download.h"

#ifdef DEBUG_BOOT 
#define DL_FRAME_SIZE       (2*1024)
void dbg_ctrl_download(int com)
{
    uint8_t download[DL_BUFSIZE_B];
    uint32_t length;
//    int key;
//    int i;

    TRACE("\nctrl dl main");
    s_DelayMs(500);
    console_close();
    dl_open(com,download,DL_BUFSIZE_B);
    dl_ctrl(&length,5000);
    dl_close();
//    s_DelayMs(2000);
    console_init(CNL_COMPORT,gSysBuf_b.console,CNL_BUFSIZE_B);
//    TRACE("\npress any key to continue");
//    InkeyCount(0);
//    TRACE("\ncode length:%dKB %d 0x%02X",length/1024,length,length);
//
//    TRACE("\r\nDisplay ctrl bin:1-yes other-no |");
//    key = InkeyCount(0);
//    if (key == 1)
//    {
//        TRACE("\ndl bin: \n");
//        for (i=0; i<length ; i += 2)
//        {
//            if (i%16 == 0) {
//                TRACE("\n%07X:",i);
//            }
//            TRACE(" %02X%02X",((uint8_t *)CTRL_SA)[i],((uint8_t *)CTRL_SA)[i+1]);
//        }
//    }
}


void hw_power_keep(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = POWER_KEEP_GPIO;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
    gpio_init.PORT_Pin = POWER_KEEP_PINx;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(POWER_KEEP_PINx);
    hw_gpio_init(&gpio_init);
    hw_gpio_set_bits(GPIOD,1<<POWER_KEEP_PINx);
}


extern void dbg_flash(void);
extern int dlboot_process(int t_out_ms);
void dbg_boot(void)
{
#ifdef DEBUG_Dx 
    int key;

//    hw_power_keep();

#if  defined (CFG_LED)
 //   hw_led_init();
  //  hw_led_on(LED_ALL);
#endif
    TRACE("\r\n\r\nEnter Boot:%s %s rom:%08X",__DATE__, __TIME__,sizeof(FLASH_ROM));
    dbg_out_srs();
    cpu_identify();
    TRACE("\n-|Boot:%05X %dKB-%d",SA_BOOT,LEN_BOOT/1024,LEN_BOOT);
    TRACE("\n-|CTRL:%05X %dKB-%d",SA_CTRL,LEN_CTRL/1024,LEN_CTRL);
//    TRACE("\nBoot irq0-31:%0X, irq32-63:%0X, irq64-93:%0X ;\n",NVICISER0,NVICISER1,NVICISER2);
    while (1)
    {
        TRACE("\r\n-|*******************BOOT debug*************************|-");
        TRACE("\r\n-|-------core %02dMHz periph %05dKHz mcg %05dKHz--------|-",core_clk_mhz,periph_clk_khz,mcg_clk_khz);
        TRACE("\r\n-|1-uart 2-mcg 3-flash 4-ctrl download 5-boot_main      |-");
        TRACE("\r\n-|6-go ctrl 7-led 8-localdl 10-update_test 88-reset     |-");
        TRACE("\r\n-|******************************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
#ifdef CFG_DBG_UART
        case 1:
            dbg_uart();
            break;
#endif
#ifdef CFG_DBG_MCG
        case 2:
            dbg_mcg(gSysBuf_b.console,CNL_BUFSIZE_B);
            break;
#endif
#ifdef CFG_DBG_FLASH
        case 3:
            dbg_flash();
            break;
#endif
        case 4:
            dbg_ctrl_download(DL_COMPORT);
            break;
        case 5:
//            boot_main();
            break;
        case 6:
            enter_ctrl();
            break;
#ifdef CFG_DBG_LED
        case 7:
            dbg_led();
            break;
#endif
        case 8:
            dlboot_process(-1);
            break;
#ifdef CFG_DBG_USB
        case 9:
            dbg_usb();
            break;
#endif
        case 10:
            update_ctrl();
            break;

        case 88:
            NVIC_SystemReset();
            break;
        case 99:
            TRACE("\r\n");
            return;
        default:
            break;
        }
    }
#endif
}
#endif

