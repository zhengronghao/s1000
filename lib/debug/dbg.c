/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : dbg.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/22/2014 11:16:54 AM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "drv_inc.h"

const char *gmcg_mode[] = {"error","BLPI","FBI","FEI","FEE","FBE","BLPE","PBE","PEE"};

#ifdef CFG_DBG_MCG
void dbg_mcg(uint8_t *buf_cnl,uint32_t buf_size)
{
    uint32_t key;
    uint32_t i;
    uint8_t mode;
    uint32_t mcg_out;

    while (1) 
    {
        mode = what_mcg_mode();
        TRACE("\nMCG current mode:%s",gmcg_mode[mode]);
        TRACE("\nMCG mode switch:");
        for (i=1; i<=8; i++)
        TRACE(" %d-%s ",i,gmcg_mode[i]);
        key = InkeyCount(0);
        mcg_out = 0;
        switch (key)
        {
        case BLPI:
            break;
        case FBI:
            break;
        case FEI:
            TRACE("\nFBE->FEI"); 
            s_DelayMs(100);
            mcg_out = fbe_fei(32*1000);
            break;
        case FEE:
            break;
        case FBE:
            TRACE("\nPBE->FBE"); 
            s_DelayMs(100);
            mcg_out = pbe_fbe(8*1000*1000);
            break;
        case BLPE:
            break;
        case PBE:
            TRACE("\nPEE->PBE"); 
            s_DelayMs(500);
            mcg_out = pee_pbe(8*1000*1000);
            break;
        case PEE:
            /* Perform processor initialization */
            cpu_clk_init();
            console_close();
            console_init(CNL_COMPORT,buf_cnl,buf_size);
            break;
        case 99:
            return;
        default:
            break;
        }
        if (mcg_out > 0) {
            console_close();
            core_clk_khz=mcg_out/1000;     //core clock
            periph_clk_khz=core_clk_khz;  //peripheral clock
            console_init(CNL_COMPORT,buf_cnl,buf_size);
        }
    }
}
#endif

/* OutSRS routine - checks the value in the SRS registers and sends
 * messages to the terminal announcing the status at the start of the 
 * code.
 */
void dbg_out_srs(void)
{   
    if (RCM_SRS1 & RCM_SRS1_TAMPER_MASK)
        TRACE("Tamper Detect Reset\n");
    if (RCM_SRS1 & RCM_SRS1_SACKERR_MASK)
        TRACE("Stop Mode Acknowledge Error Reset\n");
    if (RCM_SRS1 & RCM_SRS1_EZPT_MASK)
        TRACE("EzPort Reset\n");
    if (RCM_SRS1 & RCM_SRS1_MDM_AP_MASK)
        TRACE("MDM-AP Reset\n");
    if (RCM_SRS1 & RCM_SRS1_SW_MASK)
        TRACE("Software Reset\n");
    if (RCM_SRS1 & RCM_SRS1_LOCKUP_MASK)
        TRACE("Core Lockup Event Reset\n");
    if (RCM_SRS1 & RCM_SRS1_JTAG_MASK)
        TRACE("JTAG Reset\n");

    if (RCM_SRS0 & RCM_SRS0_POR_MASK)
        TRACE("Power-on Reset\n");
    if (RCM_SRS0 & RCM_SRS0_PIN_MASK)
        TRACE("External Pin Reset\n");
    if (RCM_SRS0 & RCM_SRS0_WDOG_MASK)
        TRACE("Watchdog(COP) Reset\n");
    if (RCM_SRS0 & RCM_SRS0_LOC_MASK)
        TRACE("Loss of Clock Reset\n");
    if (RCM_SRS0 & RCM_SRS0_LVD_MASK)
        TRACE("Low-voltage Detect Reset\n");
    if (RCM_SRS0 & RCM_SRS0_WAKEUP_MASK)
    {
        TRACE("[outSRS]Wakeup bit set from low power mode exit\n");
        TRACE("[outSRS]SMC_PMPROT   = %#02X \r\n", (SMC_PMPROT))  ;
        TRACE("[outSRS]SMC_PMCTRL   = %#02X \r\n", (SMC_PMCTRL))  ;
        TRACE("[outSRS]SMC_VLLSCTRL   = %#02X \r\n", (SMC_VLLSCTRL))  ;
        TRACE("[outSRS]SMC_PMSTAT   = %#02X \r\n", (SMC_PMSTAT))  ;

        if ((SMC_PMCTRL & SMC_PMCTRL_STOPM_MASK)== 3)
            TRACE("[outSRS] LLS exit \n") ;
        if (((SMC_PMCTRL & SMC_PMCTRL_STOPM_MASK)== 4) && ((SMC_VLLSCTRL & SMC_VLLSCTRL_VLLSM_MASK)== 0))
            TRACE("[outSRS] VLLS0 exit \n") ;
        if (((SMC_PMCTRL & SMC_PMCTRL_STOPM_MASK)== 4) && ((SMC_VLLSCTRL & SMC_VLLSCTRL_VLLSM_MASK)== 1))
            TRACE("[outSRS] VLLS1 exit \n") ;
        if (((SMC_PMCTRL & SMC_PMCTRL_STOPM_MASK)== 4) && ((SMC_VLLSCTRL & SMC_VLLSCTRL_VLLSM_MASK)== 2))
            TRACE("[outSRS] VLLS2 exit \n") ;
        if (((SMC_PMCTRL & SMC_PMCTRL_STOPM_MASK)== 4) && ((SMC_VLLSCTRL & SMC_VLLSCTRL_VLLSM_MASK)== 3))
            TRACE("[outSRS] VLLS3 exit \n") ; 
    }

    if ((RCM_SRS0 == 0) && (RCM_SRS1 == 0)) 
    {
        TRACE("[outSRS]RCM_SRS0 is ZERO   = %#02X \r\n", (RCM_SRS0))  ;
        TRACE("[outSRS]RCM_SRS1 is ZERO   = %#02X \r\n", (RCM_SRS1))  ;	 
    }
}

#ifdef CFG_DBG_GPIO 
const PORT_MemMapPtr PORT_BASE_TAB[5] = 
{ 
    PORTA_BASE_PTR, PORTB_BASE_PTR, PORTC_BASE_PTR, PORTD_BASE_PTR, PORTE_BASE_PTR 
};

const GPIO_MemMapPtr GPIO_BASE_TAB[5] = 
{
    PTA_BASE_PTR, PTB_BASE_PTR, PTC_BASE_PTR, PTD_BASE_PTR, PTE_BASE_PTR 
};

//ptx: 0-PTA 1-PTB 2-PTC 3-PTD 4-PTE
void dbg_port_reg(uint32_t ptx,uint32_t pinx)
{
    const char *ptx_tab[] = {"PTA","PTB","PTC","PTD","PTE"}; 
    uint32_t reg;
    ptx %= 5;
    pinx %= 32;
    TRACE("\n-|-----------------------%s Pin%d-----------------------|-",ptx_tab[ptx],pinx);
    TRACE("\n-|-------------------fedcba9876543210fedcba9876543210---|-");
    reg = PORT_PCR_REG(PORT_BASE_TAB[ptx],pinx);
    TRACE("\n-|PCR :0x%08X  0b%032b---|-",reg,reg);
    reg = PORT_GPCLR_REG(PORT_BASE_TAB[ptx]);
    TRACE("\n-|GCLR:0x%08X  0b%032b---|-",reg,reg);
    reg = PORT_GPCHR_REG(PORT_BASE_TAB[ptx]);
    TRACE("\n-|GCHR:0x%08X  0b%032b---|-",reg,reg);
    reg = PORT_ISFR_REG(PORT_BASE_TAB[ptx]);
    TRACE("\n-|ISFR:0x%08X  0b%032b---|-",reg,reg);
    reg = PORT_DFER_REG(PORT_BASE_TAB[ptx]);
    TRACE("\n-|DFER:0x%08X  0b%032b---|-",reg,reg);
    reg = PORT_DFCR_REG(PORT_BASE_TAB[ptx]);
    TRACE("\n-|DFCR:0x%08X  0b%032b---|-",reg,reg);
    reg = PORT_DFWR_REG(PORT_BASE_TAB[ptx]);
    TRACE("\n-|DFWR:0x%08X  0b%032b---|-",reg,reg);
    reg = GPIO_PDOR_REG(GPIO_BASE_TAB[ptx]);
    TRACE("\n-|PDOR:0x%08X  0b%032b---|-",reg,reg);
    reg = GPIO_PSOR_REG(GPIO_BASE_TAB[ptx]);
    TRACE("\n-|PSOR:0x%08X  0b%032b---|-",reg,reg);
    reg = GPIO_PCOR_REG(GPIO_BASE_TAB[ptx]);
    TRACE("\n-|PCOR:0x%08X  0b%032b---|-",reg,reg);
    reg = GPIO_PTOR_REG(GPIO_BASE_TAB[ptx]);
    TRACE("\n-|PTOR:0x%08X  0b%032b---|-",reg,reg);
    reg = GPIO_PDIR_REG(GPIO_BASE_TAB[ptx]);
    TRACE("\n-|PDIR:0x%08X  0b%032b---|-",reg,reg);
    reg = GPIO_PDDR_REG(GPIO_BASE_TAB[ptx]);
    TRACE("\n-|PDDR:0x%08X  0b%032b---|-",reg,reg);
    TRACE("\n-|-------------------fedcba9876543210fedcba9876543210---|-");

}


void dbg_gpio_reg(void)
{
    int key;
    int pinx;

    while (1)
    {
        TRACE("\n-| 0-PTA 1-PTB 2-PTC 3-PTD 4-PTE ");
        key = InkeyCount(0);
        if (key == 99)
            return;
        TRACE("\n-| pinx:0~31 ");
        pinx = InkeyCount(0);
        if (key == 99)
            return;
        dbg_port_reg(key,pinx);
    }
}
#endif




#ifdef CFG_DBG_TIME
static void dbg_time_dispay(struct rtc_time *tm)
{
    const uint8_t *week[] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"}; 
    TRACE("\n-|Now is %04d-%02d-%02d %02d:%02d:%02d",tm->tm_year+1900,
          tm->tm_mon+1, tm->tm_mday,
          tm->tm_hour,tm->tm_min, tm->tm_sec);
    if (tm->tm_wday < 6)
    TRACE("\n-|week:%s.This year has been %03d days!",week[tm->tm_wday],tm->tm_yday);
}

void dbg_time_read(void)
{
    unsigned long second=0;
    struct rtc_time tm;

    if (hw_rtc_read_second((uint32_t *)&second)) {
        TRACE("\nRead second reg error!");
        return;
    }
//    TRACE("\nsecond:%d-02%08X",second);
    rtc_time_to_tm(second,&tm);
    dbg_time_dispay(&tm);
}

void dbg_time_autoread(void)
{
    unsigned long second0=0;
    unsigned long second1=0;
    struct rtc_time tm;

    if (hw_rtc_read_second((uint32_t *)&second0)) {
        TRACE("\nRead second reg error!");
        return;
    }
    second1 = second0;
    while (1)
    {
        if (IfInkey(0)) {
            rtc_time_to_tm(second0,&tm);
            TRACE("\n-|Now is %04d-%02d-%02d %02d:%02d:%02d",tm.tm_year+1900,
                  tm.tm_mon+1, tm.tm_mday,tm.tm_hour,tm.tm_min, tm.tm_sec);
            break;
        }
        if (hw_rtc_read_second((uint32_t *)&second0)) {
            TRACE("\nRead second reg error!");
            return;
        }
        if (second0 > second1) {
            second1 = second0;
            rtc_time_to_tm(second0,&tm);
            TRACE("\n-|Now is %04d-%02d-%02d %02d:%02d:%02d",tm.tm_year+1900,
                  tm.tm_mon+1, tm.tm_mday,tm.tm_hour,tm.tm_min, tm.tm_sec);
        }
        s_DelayMs(30);
    }
}
void dbg_time_set(void)
{
    unsigned long second=0;
    struct rtc_time tm;

    memset(&tm,0x00,sizeof(tm));
    TRACE("\n-|year: ");
    tm.tm_year = InkeyCount(0)-1900;
//    tm.tm_year = 2015-1900;
    TRACE("\n-|month: ");
    tm.tm_mon = InkeyCount(0)-1;
    TRACE("\n-|day: ");
    tm.tm_mday = InkeyCount(0);
    TRACE("\n-|hour: ");
    tm.tm_hour= InkeyCount(0);
    TRACE("\n-|minute: ");
    tm.tm_min= InkeyCount(0);
    TRACE("\n-|second: ");
    tm.tm_sec= InkeyCount(0);
    if (rtc_valid_tm(&tm)) {
        TRACE("\nerror");
    } else {
        rtc_tm_to_time(&tm,&second);
    }
    hw_rtc_init(second,0);
    dbg_time_read();
}

void dbg_time(void)
{
    int key;

    while (1)
    {
        TRACE("\n-|time: 1-read 2-set 3-reg ");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            dbg_time_autoread();
            break;
        case 2:
            dbg_time_set();
            TRACE("\nSet ok");
            break;
        case 3:
            hw_rtc_reg_report();
            break;
        case 99:
            return;
        default:
            break;
        }
    }
}
#endif

#ifdef CFG_DBG_RTC
void dbg_rtc(void)
{
    int key;
    uint32_t second=0;

    if (hw_rtc_IfValid() == 1)
        TRACE("\nrtc counter is valid");
    else
        TRACE("\nrtc counter is NOT valid");

    while (1)
    {
        TRACE("\n-|rtc: 1-init 2-close 3-reg report 4-second irq 5-rtc irq ");
        TRACE("\n-|     6-if runing 7-read second req\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            hw_rtc_init(0,10);
            break;
        case 2:
            hw_rtc_colse();
            break;
        case 3:
            hw_rtc_reg_report();
            break;
        case 4:
            hw_rtc_irq_second_enable();
            break;
        case 5:
            hw_rtc_irq_enable();
            break;
        case 6:
            if (hw_rtc_IfValid() == 1)
                TRACE("\nrtc counter is valid");
            else
                TRACE("\nrtc counter is NOT valid");
            break;
        case 7:
            if (hw_rtc_read_second(&second)) {
                TRACE("\nRead second reg error!");
                break;
            }
            TRACE("\nsecond:%d-02%08X",second);
            break;
        case 8:
            break;
        case 99:
            return;
        default:
            break;
        }
    
    }
}
#endif

//NVICISER0  = 0xFFFFFF;
//NVICISER1  = 0xFFFFFF;
//NVICISER2  = 0xFFFFFF;
//    TRACE("\nSystemZone Sruct size:%d",sizeof(SYSZONE_DEF));
#ifdef CFG_DBG_UART
static void dbg_dl_isr(void)
{
    char ch;

    ch = hw_uart_getchar(DL_COMPORT);
    QueueWrite(UART_QUEUE_PTR(DL_COMPORT),
               (unsigned char *)&ch,
               1);
}

void dbg_dl_com_attr(void)
{
    int i;

    TRACE("\nbuffsize:%d head:%d tail:%d data size:%d",
           UART_QUEUE_PTR(DL_COMPORT)->bufSize,
           UART_QUEUE_PTR(DL_COMPORT)->head,
           UART_QUEUE_PTR(DL_COMPORT)->tail,
           QueueDataSize(UART_QUEUE_PTR(DL_COMPORT)));
    TRACE("\nbufer data:");
    for (i=0; i< UART_QUEUE_PTR(DL_COMPORT)->bufSize; i++)
        TRACE( " %02X", UART_QUEUE_PTR(DL_COMPORT)->pBuffer[i]);
    TRACE("\nread data:");
    for (i=0; i< drv_uart_check_readbuf(DL_COMPORT); i++)
        TRACE(" %02X", UART_QUEUE_PTR(DL_COMPORT)->pBuffer[(UART_QUEUE_PTR(DL_COMPORT)->tail+i)%UART_QUEUE_PTR(DL_COMPORT)->bufSize]);
}

void dbg_uart(void)
{
    int key;
    uint8_t ch;
    uint8_t buffer[5];
    uint32_t i;

    while (1)
    {
        TRACE("\r\nDL_COMPORT: 1-init 2-wite 3-close 4-buffer attr 5-read");

        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            drv_uart_open(DL_COMPORT,DL_BAUD, buffer,sizeof(buffer),dbg_dl_isr);
            break;
        case 2:
            TRACE("\ninput: ");
//            ch = InkeyCount(0);
            ch = 0xAA;
            for (i=0; i<500; i++)
            hw_uart_putchar(DL_COMPORT,ch);
            break;
        case 3:
            drv_uart_close(DL_COMPORT);
            break;
        case 4:
            dbg_dl_com_attr();
            break;
        case 5:
            if (drv_uart_read(DL_COMPORT,&ch,1,100) > 0)
                TRACE("\nread:%d",ch);
            else 
                TRACE("\nread time out");
            break;
        case 99:
            return;
        default:
            break;
        }
        TRACE("\n");
    }
}
#endif

#ifdef CFG_DBG_FLASH
void dbg_flash(void)
{
    uint8_t ch;
    uint8_t ret;
    uint8_t buffer[FLASH_SECTOR_SIZE];
    uint32_t flash_addr;
    uint32_t flash_offset=0;
    uint32_t i;
    uint32_t number=0;

    TRACE("\n-|Flash Addr input:1024*?");
    flash_addr = InkeyCount(0);
    flash_addr *= 1024;
    TRACE("\nFlash addr:%d %02X",flash_addr,flash_addr);

    hw_flash_init();
    while (1) {
        TRACE("\n1-Erase sector");
        TRACE("\n2-Write sector");
        TRACE("\n3-Read sector");
        TRACE("\n4-set offset addr");
        TRACE("\n-|Flash SA:%05X offset:%d",flash_addr,flash_offset);
        ch = InkeyCount(0);
        switch (ch)
        {
        case 1:
            ret = drv_flash_SectorErase(flash_addr+flash_offset);
            s_DelayMs(5000);
            TRACE("\nret:%02X",ret);
            break;
        case 2:
            for (i=0; i<FLASH_SECTOR_SIZE; i++) {
                buffer[i] = (uint8_t)i;
            }
            number = FLASH_SECTOR_SIZE;
            TRACE("\n-|write len:");
            number = InkeyCount(0);
            if (number > FLASH_SECTOR_SIZE)
                number = FLASH_SECTOR_SIZE;
            ret = flash_write(flash_addr+flash_offset,number,buffer);
            TRACE("\nret:%02X",ret);
            break;
        case 3:
            memset(buffer,0,sizeof(buffer));
            for (i=0; i<FLASH_SECTOR_SIZE; i++) {
                buffer[i] = *(((uint8_t *)flash_addr+flash_offset)+i);
            }
            TRACE("\n-|display len:");
            number = InkeyCount(0);
            if (number > FLASH_SECTOR_SIZE)
                number = FLASH_SECTOR_SIZE;
            vDispBuf(number,3,buffer);
            break;
        case 4:
            TRACE("\n-|flash offset:");
            flash_offset = InkeyCount(0);
            TRACE("\n-|Flash SA:%05X offset:%d",flash_addr,flash_offset);
            break;
        case 99:
            return;
        default :
            break;
        }
    }
}
#endif

#ifdef CFG_DBG_LED
/*
 * Initialize GPIO for Tower switches and LED's
 *
 *   PTC5 - SW6
 *   PTC13 - SW7
 *
 *   PTC10 - Blue LED (D17)
 *   PTC9 - Green LED (D14)
 *   PTC8 - Yellow LED (D15)
 *   PTC7 - Orange LED (D16)
 */
void init_gpio(void)
{
    /* Turn on all port clocks */
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
    
    //Set PTC10, PTC7, PTC8, and PTC9 to GPIO
////    PORTC_PCR10=(0|PORT_PCR_MUX(1));
////    PORTC_PCR7=(0|PORT_PCR_MUX(1));
////    PORTC_PCR8=(0|PORT_PCR_MUX(1));
////    PORTC_PCR9=(0|PORT_PCR_MUX(1));
    
    // Initialize GPIO connected to LEDs
    PORTD_PCR7=(0|PORT_PCR_MUX(1));
    PORTD_PCR6=(0|PORT_PCR_MUX(1));
    PORTD_PCR5=(0|PORT_PCR_MUX(1));
    PORTD_PCR4=(0|PORT_PCR_MUX(1));
    
    // Initialize GPIO connected to pushbutton switches SW2 and SW3 and enable internal pull-up
    PORTC_PCR6=(0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK);
    PORTC_PCR7=(0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK);    
    
    //Set PTC10, PTC7, PTC8, PTC9 as an output
//    GPIOC_PDDR=GPIO_PDDR_PDD(GPIO_PIN(7) | GPIO_PIN(8) | GPIO_PIN(9) | GPIO_PIN(10));	
    GPIOD_PDDR = ((1<<4)|(1<<5)|(1<<6)|(1<<7));    
}

//Defines for GPIO
#define GPIO_PIN_MASK            0x1Fu
#define GPIO_PIN(x)              (((1)<<(x & GPIO_PIN_MASK)))
//void _dbg_led(void)
//{
//    PORT_InitTypeDef port_init;
//
//    port_init.PORTx = PORTC_BASE_PTR;
//    port_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PU;
//    port_init.PORT_Pin = PORT_Pin_6;
//    hw_port_init(&port_init);
//    port_init.PORT_Pin = PORT_Pin_7;
//    hw_port_init(&port_init);
//
//    init_gpio();
//    GPIOD_PDOR&=~GPIO_PDOR_PDO(GPIO_PIN(4)); //Set PTD4 to 0 (turns on green LED)
//    GPIOD_PDOR&=~GPIO_PDOR_PDO(GPIO_PIN(5)); //Set PTD5 to 0 (turns on yellow LED)          
//    GPIOD_PDOR&=~GPIO_PDOR_PDO(GPIO_PIN(6)); //Set PTD6 to 0 (turns on red LED)
//    GPIOD_PDOR&=~GPIO_PDOR_PDO(GPIO_PIN(7)); //Set PTD7 to 0 (turns on blue LED) 
//}
#endif

#ifdef CFG_DBG_RNG
void dbg_rnga(void)
{
    int key;
    int iRet;
    uint32_t random_number,i,j;
    uint32_t buffer[16];
        
    /*FSL: enable RNGB clk*/
    SIM_SCGC6 |= SIM_SCGC6_RNGA_MASK;
    while (1)
    {
        TRACE("\n-|rnga: 1-init 2-get  3-stop 4-reg report\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            hw_rnga_init(0x41234567);
            break;
        case 2:
            iRet = hw_rnga_getnumber(&random_number);
            TRACE("\niRet:%d random data:%X",iRet,random_number);
            break;
        case 3:
            hw_rnga_stop();
            break;
        case 4:
        default:
            hw_rnga_reg_report();
            break;
        case 5:
            TRACE("\n-|rand test times:");
            key = InkeyCount(0);
            i =0;
            while (key--)
            {
                TRACE("\n|rand:%04d |",++i);
                if (drv_rand(0x78375992,16,buffer) == 0) {
                    for (j=0; j<16; j++)
                        TRACE(" %08X",buffer[j]);
                } else {
                    TRACE("\nError");
                    hw_rnga_reg_report();
                }
            }
            break;
        case 99:
            return;
        }
    }
}
#endif

#ifdef CFG_DBG_SECURYTY
#define SHA256_RESULT_LENGTH                     32
void dbg_security(void)
{
    int key;
    unsigned int dummy_var[SHA256_RESULT_LENGTH/4];

#if FREESCALE_MMCAU  
    TRACE("\nTesting mmCAU\n");
#elif FREESCALE_CAU
    TRACE("\nTesting CAU\n");  
#else
#error "Declare CAU or mmCAU coprocessor"
#endif
  TRACE("\nIf no error message is shown, then everything's OK\n");
  TRACE("\nRunning crypto in HW\n");

    while (1)
    {
        TRACE("\n-|Security: 1-des 2-aes 3-sha1 4-sha256 5-md5\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
#ifdef  DBG_DES
            des_self_test( 1);
#endif
            break;
//        case 2:
//            aes_main();
//            break;
        case 3:
#ifdef  DBG_SHA1
            sha1_self_test();
#endif
            break;
        case 4:
#ifdef  DBG_SHA1
            if( !cau_sha256_initialize_output(dummy_var) )
                sha256_self_test();
            else
                TRACE("No support for SHA256\n");
#endif
            break;
//        case 5:
//            md5_main();
//            break;
        case 99:
            return;
        default:
            break;
        }
    }
}
#endif

#ifdef CFG_DBG_DELAY
#define DELAY_GPIO  GPIOA
#define DELAY_PINx   PORT_Pin_12 
void dbg_delay(void)
{
    int key;
    GPIO_InitTypeDef gpio_init;

    while (1)
    {
        TRACE("\r\n-|******************delay debug**********************|-");
        TRACE("\r\n-|1-init 2-ms 3-us 4-hight 5-low                     |-");
        TRACE("\r\n-|***************************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            gpio_init.GPIOx = DELAY_GPIO;
            gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
            gpio_init.PORT_Pin = DELAY_PINx;
            gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(DELAY_PINx);
            hw_gpio_init(&gpio_init);
            hw_gpio_reset_bits(DELAY_GPIO,1<<DELAY_PINx);
            break;
        case 2:
            TRACE("\nInput cn:");
            key = InkeyCount(0);
            hw_gpio_set_bits(DELAY_GPIO,1<<DELAY_PINx);
            s_DelayMs(key);
            hw_gpio_reset_bits(DELAY_GPIO,1<<DELAY_PINx);
            break;
        case 3:
            TRACE("\nInput cn:");
            key = InkeyCount(0);
            hw_gpio_set_bits(DELAY_GPIO,1<<DELAY_PINx);
            s_DelayUs(key);
            hw_gpio_reset_bits(DELAY_GPIO,1<<DELAY_PINx);
            break;
        case 4:
            hw_gpio_set_bits(DELAY_GPIO,1<<DELAY_PINx);
            break;
        case 5:
            hw_gpio_reset_bits(DELAY_GPIO,1<<DELAY_PINx);
            break;
        case 99:
            return;
        default:
            break;
        }
    }
}
#endif

#ifdef CFG_DBG_RSA
/*
 * Example RSA-1024 keypair, for test purposes
 */
#define KEY_LEN 128

#define RSA_N   "9292758453063D803DD603D5E777D788" \
                "8ED1D5BF35786190FA2F23EBC0848AEA" \
                "DDA92CA6C3D80B32C4D109BE0F36D6AE" \
                "7130B9CED7ACDF54CFC7555AC14EEBAB" \
                "93A89813FBF3C4F8066D2D800F7C38A8" \
                "1AE31942917403FF4946B0A83D3D3E05" \
                "EE57C6F5F5606FB5D4BC6CD34EE0801A" \
                "5E94BB77B07507233A0BC7BAC8F90F79"

#define RSA_E   "10001"

#define RSA_D   "24BF6185468786FDD303083D25E64EFC" \
                "66CA472BC44D253102F8B4A9D3BFA750" \
                "91386C0077937FE33FA3252D28855837" \
                "AE1B484A8A9A45F7EE8C0C634F99E8CD" \
                "DF79C5CE07EE72C7F123142198164234" \
                "CABB724CF78B8173B9F880FC86322407" \
                "AF1FEDFDDE2BEB674CA15F3E81A1521E" \
                "071513A1E85B5DFA031F21ECAE91A34D"

#define RSA_P   "C36D0EB7FCD285223CFB5AABA5BDA3D8" \
                "2C01CAD19EA484A87EA4377637E75500" \
                "FCB2005C5C7DD6EC4AC023CDA285D796" \
                "C3D9E75E1EFC42488BB4F1D13AC30A57"

#define RSA_Q   "C000DF51A7C77AE8D7C7370C1FF55B69" \
                "E211C2B9E5DB1ED0BF61D0D9899620F4" \
                "910E4168387E3C30AA1E00C339A79508" \
                "8452DD96A9A5EA5D9DCA68DA636032AF"

#define RSA_DP  "C1ACF567564274FB07A0BBAD5D26E298" \
                "3C94D22288ACD763FD8E5600ED4A702D" \
                "F84198A5F06C2E72236AE490C93F07F8" \
                "3CC559CD27BC2D1CA488811730BB5725"

#define RSA_DQ  "4959CBF6F8FEF750AEE6977C155579C7" \
                "D8AAEA56749EA28623272E4F7D0592AF" \
                "7C1F1313CAC9471B5C523BFE592F517B" \
                "407A1BD76C164B93DA2D32A383E58357"

#define RSA_QP  "9AE7FBC99546432DF71896FC239EADAE" \
                "F38D18D2B2F0E2DD275AA977E2BF4411" \
                "F5A3B2A5D33605AEBBCCBA7FEB9F2D2F" \
                "A74206CEC169D74BF5A8C50D6F48EA08"

#define PT_LEN  24
#define RSA_PT  "\xAA\xBB\xCC\x03\x02\x01\x00\xFF\xFF\xFF\xFF\xFF" \
                "\x11\x22\x33\x0A\x0B\x0C\xCC\xDD\xDD\xDD\xDD\xDD"

/*
 * Example RSA-2048 keypair, for test purposes
 */
#define KEY_LEN_2048     (2048/8) 

#define RSA_N_2048  "8E70DB3A90EB6036AECCBB7BA2AABA4C9ACEBD45C98F3B39836CB60625AC621EA8C355567F5AED4FF6DC1470D6407CEF38A9C68190B4BB280B50B305E6352BA934B20B2E3FEBFF6F5642609134EF23CFA8F1F327FC343000C60BAAB867B4286B91C6D97D38455C29A0D029AB26E455A226B41D492D7FB7DE171B9A193308BBAB9DB4FB9BB5A4EA0D900E2B9114B015953DB6E538593D1050BE56552379272BAA298B58FF243FAB63ACC42C01F0242D58B522D72AC1EE9A2D2F297E49F9977F9BE8D0B2D5584D25D6B9321E60441DD16E4EFF1D6A4A05A19985D81BF266DA0704BF3151F857F96075BE62D552B253861ED23C9A2401BE289DEB0842D40F5F54D3"
#define RSA_E_2048  "010001"

#define RSA_D_2048  "7D804438AADD282DA5FFEF2474E37637EEA23F33D0EEA920F881FD331758762F4B44A7D9096704F6D2AB803B171B44B8C5B51898A9F6037CB9D38598AA07A90B71C9797E6B85B4CED532B0C0DFE1E02CA44F618D967228E555998BF6E43060F8BE3D74FD166A4E54C34305A3AE25BAA99EEE4D6B5F724B7F9BDE8EFCAAF183DFD53C979FBA727392853D87DD3851D6AC4660BDFC048E077965A38D0F36D322EAE7978D3956C7DD33B9526B8B4A43C2FB112F634D89E3C6F17D4FF826877BE889366E405DCB66537CE99A98BC42F12CC823579A82AAC19C7A7A5C4E48BE894E8575174176C63A4ABC32A0017DBE38C5EBC4C4CC5717306D326931D699EA1D33D1"
#define RSA_P_2048	"CD1D918D0C0AE3788AF6D2F6C37ECE5DC5CDBECC51DD0B110DF45EBE5A394993ECBDDDC7A5AF4D9E3E5867C3DC46890687CDFCDA404E1DC0480947AB4F2378C99619E435CF4960EC07C2614038D9FD40E0F143FA260C62EF78F778C2FB2B92681D36470AE607B43C9F6682BA03CBCA44D5EEBF8D1E577AC6B320943E0F21616B"
#define RSA_Q_2048  "B1C6F5870203321547B2EDA36B0E919F5FE99781BA64853349C55ECFF79941205487451014C372CD03BE319285AADF9D53DA0126D70DAC69687CB15C9AD3EE415F3B685FE6102CA99F85EC149138270188AC7165B9A650F833006793D3537E9205538DC69F72D3EC21FEBABF72F26B5E70B94324C7329C6E6A23A1BF6FE6EC39"
#define RSA_DP_2048 "432C96D961834361DBE8DFC84ED7FD8FBA531A5461CF096BC96D29861F98395F879DDABF22A4D9BFA45D618644629947993C3DBDE06774D8DEFEA309187EDE74EE828639952C4BE0A0DDE8E99674D5BAE4B03EF9F2720608B54E1EAF84808D8F2959F8443A77757091278A528F21B3997D92960B0A4FCDF6BAFD456F59972E07"
#define RSA_DQ_2048 "3E329EDA6250DDE7E612EAE37EE434FCAEA0DA5A0FED7550D161955F6B8019F206C0E0DFD473E1296B42D2B331C2EBC36E7FDA614D62424D0ABA7D8F29B8362676FC20F354599A56AA323FBB9E6727F6786BE4ACB37C45DDE07226CAE867D0E3A3AB6CF1EDFD4DB9D2D6FD5D58090A999B14A6322AE34129BC46C523663243B9"
#define RSA_QP_2048 "7BE26A09339247E99817042C88A59B15B3A25211D69DC837BEA952914E37966CB98AA10BE7F702FE3CFD8CBBBC4F7B9046C271F7DD4C78BB69C648218E8CA0486C461AA9C0739AD53A970742D1B488701BB5A2908529FC13B860BD7112E993B8589C1ACF7649D915D04F1741ADE5ECD12C22E4FA05CCF90D94990161B1331D12"

//#define PT_LEN  24
//#define RSA_PT  "\xAA\xBB\xCC\x03\x02\x01\x00\xFF\xFF\xFF\xFF\xFF" \
//                "\x11\x22\x33\x0A\x0B\x0C\xCC\xDD\xDD\xDD\xDD\xDD"

static int myrand( void *rng_state, unsigned char *output, size_t len )
{
 //   size_t i;

    if( rng_state != NULL )
        rng_state  = NULL;

//    for( i = 0; i < len; ++i )
//        output[i] = rand();
    RNG_FillRandom(output,len);
    
    return( 0 );
}

int dbg_rsa_gen(void)
{
    rsa_context rsa;
    uint32_t tmp=0;
    uint32_t cycle_start,cycle_end;

    TRACE("\n-|RSA-1024 generate");
//    DisableInterrupts;
//    hw_systick_close();
    cycle_start = sys_get_counter();
    rsa_init( &rsa, RSA_PKCS_V15, 0 );
    if (rsa_gen_key(&rsa,myrand,&tmp,1024,65537)){
        TRACE("\nError");
        goto RSA_CLEAN;
    }
    cycle_end  = sys_get_counter();
    TRACE(" cost times:%dms",(cycle_end-cycle_start));
    TRACE("\n-|RSA-2048 generate");
    cycle_start = sys_get_counter();
    rsa_init( &rsa, RSA_PKCS_V15, 0 );
    if (rsa_gen_key(&rsa,myrand,&tmp,2048,65537)){
        TRACE("\nError");
        goto RSA_CLEAN;
    }
    cycle_end  = sys_get_counter();
//    hw_systick_open();
//    EnableInterrupts;
    TRACE(" cost times:%dms",(cycle_end-cycle_start));
RSA_CLEAN:
    rsa_free( &rsa );
    return 0;
}

/* Bob's 1024-bit RSA encryption key */
const uint8_t rsa1024_n[] = {
0xA9, 0xE1, 0x67, 0x98, 0x3F, 0x39, 0xD5, 0x5F, 
0xF2, 0xA0, 0x93, 0x41, 0x5E, 0xA6, 0x79, 0x89, 
0x85, 0xC8, 0x35, 0x5D, 0x9A, 0x91, 0x5B, 0xFB, 
0x1D, 0x01, 0xDA, 0x19, 0x70, 0x26, 0x17, 0x0F, 
0xBD, 0xA5, 0x22, 0xD0, 0x35, 0x85, 0x6D, 0x7A, 
0x98, 0x66, 0x14, 0x41, 0x5C, 0xCF, 0xB7, 0xB7, 
0x08, 0x3B, 0x09, 0xC9, 0x91, 0xB8, 0x19, 0x69, 
0x37, 0x6D, 0xF9, 0x65, 0x1E, 0x7B, 0xD9, 0xA9, 
0x33, 0x24, 0xA3, 0x7F, 0x3B, 0xBB, 0xAF, 0x46, 
0x01, 0x86, 0x36, 0x34, 0x32, 0xCB, 0x07, 0x03, 
0x59, 0x52, 0xFC, 0x85, 0x8B, 0x31, 0x04, 0xB8, 
0xCC, 0x18, 0x08, 0x14, 0x48, 0xE6, 0x4F, 0x1C, 
0xFB, 0x5D, 0x60, 0xC4, 0xE0, 0x5C, 0x1F, 0x53, 
0xD3, 0x7F, 0x53, 0xD8, 0x69, 0x01, 0xF1, 0x05, 
0xF8, 0x7A, 0x70, 0xD1, 0xBE, 0x83, 0xC6, 0x5F, 
0x38, 0xCF, 0x1C, 0x2C, 0xAA, 0x6A, 0xA7, 0xEB,
};
const uint8_t rsa1024_e[] = { 0x01, 0x00, 0x01 };
#if 0 
const uint8_t rsa1024_d[] = {
0x67, 0xCD, 0x48, 0x4C, 0x9A, 0x0D, 0x8F, 0x98, 
0xC2, 0x1B, 0x65, 0xFF, 0x22, 0x83, 0x9C, 0x6D, 
0xF0, 0xA6, 0x06, 0x1D, 0xBC, 0xED, 0xA7, 0x03, 
0x88, 0x94, 0xF2, 0x1C, 0x6B, 0x0F, 0x8B, 0x35, 
0xDE, 0x0E, 0x82, 0x78, 0x30, 0xCB, 0xE7, 0xBA, 
0x6A, 0x56, 0xAD, 0x77, 0xC6, 0xEB, 0x51, 0x79, 
0x70, 0x79, 0x0A, 0xA0, 0xF4, 0xFE, 0x45, 0xE0, 
0xA9, 0xB2, 0xF4, 0x19, 0xDA, 0x87, 0x98, 0xD6, 
0x30, 0x84, 0x74, 0xE4, 0xFC, 0x59, 0x6C, 0xC1, 
0xC6, 0x77, 0xDC, 0xA9, 0x91, 0xD0, 0x7C, 0x30, 
0xA0, 0xA2, 0xC5, 0x08, 0x5E, 0x21, 0x71, 0x43, 
0xFC, 0x0D, 0x07, 0x3D, 0xF0, 0xFA, 0x6D, 0x14, 
0x9E, 0x4E, 0x63, 0xF0, 0x17, 0x58, 0x79, 0x1C, 
0x4B, 0x98, 0x1C, 0x3D, 0x3D, 0xB0, 0x1B, 0xDF, 
0xFA, 0x25, 0x3B, 0xA3, 0xC0, 0x2C, 0x98, 0x05, 
0xF6, 0x10, 0x09, 0xD8, 0x87, 0xDB, 0x03, 0x19,
};

const uint8_t rsa1024_data[] = { 
0x4E, 0x63, 0x6A, 0xF9, 0x8E, 0x40, 0xF3, 0xAD, 
0xCF, 0xCC, 0xB6, 0x98, 0xF4, 0xE8, 0x0B, 0x9F,
};
#endif
const uint8_t rsa1024_eb[] = {
0x00, 0x02, 0x25, 0x7F, 0x48, 0xFD, 0x1F, 0x17, 
0x93, 0xB7, 0xE5, 0xE0, 0x23, 0x06, 0xF2, 0xD3, 
0x22, 0x8F, 0x5C, 0x95, 0xAD, 0xF5, 0xF3, 0x15, 
0x66, 0x72, 0x9F, 0x13, 0x2A, 0xA1, 0x20, 0x09, 
0xE3, 0xFC, 0x9B, 0x2B, 0x47, 0x5C, 0xD6, 0x94, 
0x4E, 0xF1, 0x91, 0xE3, 0xF5, 0x95, 0x45, 0xE6, 
0x71, 0xE4, 0x74, 0xB5, 0x55, 0x79, 0x9F, 0xE3, 
0x75, 0x60, 0x99, 0xF0, 0x44, 0x96, 0x40, 0x38, 
0xB1, 0x6B, 0x21, 0x48, 0xE9, 0xA2, 0xF9, 0xC6, 
0xF4, 0x4B, 0xB5, 0xC5, 0x2E, 0x3C, 0x6C, 0x80, 
0x61, 0xCF, 0x69, 0x41, 0x45, 0xFA, 0xFD, 0xB2, 
0x44, 0x02, 0xAD, 0x18, 0x19, 0xEA, 0xCE, 0xDF, 
0x4A, 0x36, 0xC6, 0xE4, 0xD2, 0xCD, 0x8F, 0xC1, 
0xD6, 0x2E, 0x5A, 0x12, 0x68, 0xF4, 0x96, 0x00, 
0x4E, 0x63, 0x6A, 0xF9, 0x8E, 0x40, 0xF3, 0xAD, 
0xCF, 0xCC, 0xB6, 0x98, 0xF4, 0xE8, 0x0B, 0x9F,
};
const uint8_t rsa1024_cipher[] = {
0x3D, 0x2A, 0xB2, 0x5B, 0x1E, 0xB6, 0x67, 0xA4, 
0x0F, 0x50, 0x4C, 0xC4, 0xD7, 0x78, 0xEC, 0x39, 
0x9A, 0x89, 0x9C, 0x87, 0x90, 0xED, 0xEC, 0xEF, 
0x06, 0x2C, 0xD7, 0x39, 0x49, 0x2C, 0x9C, 0xE5, 
0x8B, 0x92, 0xB9, 0xEC, 0xF3, 0x2A, 0xF4, 0xAA, 
0xC7, 0xA6, 0x1E, 0xAE, 0xC3, 0x46, 0x44, 0x98, 
0x91, 0xF4, 0x9A, 0x72, 0x23, 0x78, 0xE0, 0x08, 
0xEF, 0xF0, 0xB0, 0xA8, 0xDB, 0xC6, 0xE6, 0x21, 
0xED, 0xC9, 0x0C, 0xEC, 0x64, 0xCF, 0x34, 0xC6, 
0x40, 0xF5, 0xB3, 0x6C, 0x48, 0xEE, 0x93, 0x22, 
0x80, 0x8A, 0xF8, 0xF4, 0xA0, 0x21, 0x2B, 0x28, 
0x71, 0x5C, 0x76, 0xF3, 0xCB, 0x99, 0xAC, 0x7E, 
0x60, 0x97, 0x87, 0xAD, 0xCE, 0x05, 0x58, 0x39, 
0x82, 0x9E, 0x01, 0x42, 0xC4, 0x4B, 0x67, 0x6D, 
0x21, 0x81, 0x11, 0xFF, 0xE6, 0x9F, 0x9D, 0x41, 
0x42, 0x4E, 0x17, 0x7C, 0xBA, 0x3A, 0x43, 0x5B,
};

//#define  RSA_UNIT_LEN  256
//typedef struct     // 公钥结构
//{
//	uint bits;                   /* length in bits of modulus */
//	uchar modulus[RSA_UNIT_LEN];           /* modulus */
//	uchar exponent[4];             /* public exponent LSB...MSB */	
//} R_RSA_PUBLIC_KEY;

typedef struct     // 私钥结构
{
	R_RSA_PUBLIC_KEY tPubinfo;
	uchar prikey[RSA_UNIT_LEN];  /* private key */
} R_RSA_PRIVATE_KEY;

//uint Rsa_calt_pub(int inlen, const uchar *in, R_RSA_PUBLIC_KEY *keyinfo,int *outlen, uchar *out)
//{
//    rsa_context rsa;
//    uint32_t ret=0;
//    uint32_t i;
//    uint8_t *p=NULL;
//
//    rsa_init( &rsa, RSA_PKCS_V15, 0 );
//    ret = mpi_read_little_endian(&rsa.N,keyinfo->modulus,keyinfo->bits>>3);
//    ret += mpi_read_little_endian(&rsa.E,keyinfo->exponent,sizeof(keyinfo->exponent));
//    if (ret) {
//        TRACE("\nRead rsa error");
//        goto PUB_CLEAN;
//    }
//    rsa.len = ( mpi_msb( &rsa.N ) + 7 ) >> 3;
////    TRACE_BUF("N",(uint8_t *)rsa.N.p,rsa.N.n<<2);
////    TRACE_BUF("E",(uint8_t *)rsa.E.p,rsa.E.n<<2);
////    TRACE("\n-|rsa len:%d",rsa.len);
//    if( rsa_check_pubkey(&rsa) != 0)
//    {
//        TRACE("\nrsa key error");
//        ret = 1;
//        goto PUB_CLEAN;
//    }
//    *outlen = 0;
//    if (inlen >= rsa.len)
//    {
//        for (i=0; i<inlen; i += rsa.len)
//        {
//            if (rsa_public(&rsa,in+i,out+i)) {
//                TRACE("\n-|Erro:rsa pub error");
//                ret = -1;
//                goto PUB_CLEAN;
//            }
//            *outlen += rsa.len;
//        }
//    }
//    if (*outlen < inlen) 
//    {
//        if( ( p = (uint8_t *) malloc(rsa.len+4) ) == NULL ) {
//            ret = -1;
//            goto PUB_CLEAN;
//        }
//        memset(p,0,rsa.len);
//        i = inlen-*outlen;
//        memcpy(p+rsa.len-i,in+*outlen,i);
//        if (rsa_public(&rsa,p,out+*outlen)) {
//            TRACE("\n-|Erro:rsa pub error");
//            free(p);
//            p = NULL;
//            ret = -1;
//            goto PUB_CLEAN;
//        }
//        *outlen += rsa.len;
//        free(p);
//        p = NULL;
//    }
////    TRACE_BUF("Chiper cau",out,rsa.len);
//PUB_CLEAN:
//    rsa_free( &rsa );
//    return ret;
//}
////in: cipher_text  len = keyinfo->bits/8
////keyinfo->modulus:  pubkey+00
////out:cipher_text  len = keyinfo->bits/8
//uint Rsa_calt_pri(int inlen, uchar *in, R_RSA_PRIVATE_KEY *keyinfo,int *outlen, uchar *out)
//{
//    rsa_context rsa;
//    uint32_t ret=0;
//    uint32_t i;
//    uint8_t *p=NULL;
//
//    rsa_init( &rsa, RSA_PKCS_V15, 0 );
//    ret = mpi_read_little_endian(&rsa.N,keyinfo->tPubinfo.modulus,keyinfo->tPubinfo.bits>>3);
//    ret = mpi_read_little_endian(&rsa.D,keyinfo->prikey,keyinfo->tPubinfo.bits>>3);
//    ret += mpi_read_little_endian(&rsa.E,keyinfo->tPubinfo.exponent,sizeof(keyinfo->tPubinfo.exponent));
//    if (ret) {
//        TRACE("\nRead rsa error");
//        goto PUB_CLEAN;
//    }
//    rsa.len = ( mpi_msb( &rsa.N ) + 7 ) >> 3;
////    TRACE_BUF("N",(uint8_t *)rsa.N.p,rsa.N.n<<2);
////    TRACE_BUF("E",(uint8_t *)rsa.E.p,rsa.E.n<<2);
////    TRACE("\n-|rsa len:%d",rsa.len);
//    if( rsa_check_pubkey(&rsa) != 0)
//    {
//        TRACE("\nrsa key error");
//        ret = 1;
//        goto PUB_CLEAN;
//    }
//    *outlen = 0;
//    if (inlen >= rsa.len)
//    {
//        for (i=0; i<inlen; i += rsa.len)
//        {
//            if (rsa_private(&rsa,in+i,out+i)) {
//                TRACE("\n-|Erro:rsa pub error");
//                ret = -1;
//                goto PUB_CLEAN;
//            }
//            *outlen += rsa.len;
//        }
//    }
//    if (*outlen < inlen) 
//    {
//        if( ( p = (uint8_t *) malloc(rsa.len+4) ) == NULL ) {
//            ret = -1;
//            goto PUB_CLEAN;
//        }
//        memset(p,0,rsa.len);
//        i = inlen-*outlen;
//        memcpy(p+rsa.len-i,in+*outlen,i);
//        if (rsa_private(&rsa,p,out+*outlen)) {
//            TRACE("\n-|Erro:rsa pub error");
//            free(p);
//            p = NULL;
//            ret = -1;
//            goto PUB_CLEAN;
//        }
//        *outlen += rsa.len;
//        free(p);
//        p = NULL;
//    }
////    TRACE_BUF("Chiper cau",out,rsa.len);
//PUB_CLEAN:
//    rsa_free( &rsa );
//    return ret;
//}

void dbg_rsa_pub_encrypt(void)
{
    R_RSA_PUBLIC_KEY rsa_pub;
    R_RSA_PRIVATE_KEY rsa_prv;
    int outlen=0;
    uint8_t out[256+16];
    uint8_t out1[256+16];
    mpi n,d;
    mpi e;
    uint32_t cycle_start,cycle_end;
    TRACE("\n\n-|RSA-1024 public encrypt:");
    memset(&rsa_pub,0,sizeof(rsa_pub));
    memset(&n,0,sizeof(mpi));
    memset(&e,0,sizeof(mpi));
    mpi_read_string(&n, 16,
                    "AB9790BDF51BCB87AD7C5689EA1B86BF2F5A913F1850C69885B3414C4B02EE80B3B15935FF8FDB1BF587F3341F6721F9612D9AB1D06465FC85C80BDBBB930931246F15E3F830232BB5250A504A3A1E32D54D381A06B6EEF4101BD54C4F97DA9AA085781C31E981398E89C4068BB29FD2A197D881F57DDDEE17A66A55B76F6EA3"
                    );
    mpi_read_string(&e, 16,"010001");
    memcpy(rsa_pub.modulus,n.p,sizeof(rsa1024_n));
    memset(rsa_pub.exponent,0,4);
    memcpy(rsa_pub.exponent,e.p,sizeof(rsa1024_e));
    rsa_pub.bits = n.n<<5 ; 
    mpi_free(&n);
    mpi_free(&e);
    cycle_start = hw_get_cycle();
//    Rsa_calt_pub(sizeof(rsa1024_eb), rsa1024_eb, &rsa_pub,&outlen,out);
    Rsa_calt_pub(4,"\x30\x31\x32\x33", &rsa_pub,&outlen,out);
    cycle_end = hw_get_cycle();
    if (memcmp(rsa1024_cipher,out,outlen) == 0)
    {
        TRACE_BUF("Succe. cipher",out,outlen);
        TRACE("\nRSA-%d encryption times:%dms",rsa_pub.bits,(cycle_end-cycle_start)/core_clk_khz);
    } else 
    {
        TRACE_BUF("cipher",out,outlen);
        Rsa_calt_pub(2,"\x11\x23", &rsa_pub,&outlen,out);
        TRACE_BUF("cipher1",out,outlen);
        TRACE("\nError:olen%d",outlen);
    }
    //--------2048-----------
    TRACE("\nRSA-2048 public");
    memset(&rsa_pub,0,sizeof(rsa_pub));
    memset(&n,0,sizeof(mpi));
    memset(&e,0,sizeof(mpi));
    //E:"010001"
    //N:CB2093024AE8893B6BE5BA44635DF66EB6FDF441E03211099D2E16CD9C395B32EDF2D0F05819D753D16812AA6B0248B88F803F6BB7B1DFDEEF54568F6B2C5DB232E9B6F4B618445D4CE0ED443A64F048BEEA2A8D812346634560C8CC3641B67F02D135DC2D56D8F69EFB364DB8CA26716455690CD2399FDF00FC6E54274785646A93718A0D510969AF60B7DD5854103D3A4217259680A764CBC77FCC2E135736A8F4EB7350188BEA5A4F0B787B2E097C2E8F71B2A8D80D513D721588CE24263EA3AF18136609E552B62203BF831781039A24511570153D5A39E1B314B0181F5E1471CBCB557C66913AE5596C9A4E0FC17B0E5A12E9442B101E7D8500A6A436EB
    //D:2589848144F489FFB825C0F14867A1D5BDED5A491E34425FC37333F5952C28455621AC28EE2F0052D5FCE48E27AE496009ECCA4B8EABC9523A6A1C80EE6E497657E046D61C1C89EDAFFA54FE69C8237B5E048A0E9251C17D30E1FCD9782156A74C0EFB640FE0DFC15B89D2F433F4EF7BBEEDE5B6DFD6944BB2B3F3738BBAB4D49010FEF15A3EB751A1DE4C5C93E8AED97C0E9ABC304E9E14EA8045786A600555469E51D75976B1908E3D1C7C834DFE1157FDC3A3CF38DE22E09DD822E671DD5A4177EEA9D1040861E28862F31A85A99D5EEAEA644AD2313E2BEA93BE89F0FDF6D89CDFAD6604D0BBDE2974D072893CEF01276546FCC0DE11C9C87C875BD42301
    //P:9F21980204CB800CF8ACBE178778C435D9A294FFAFC8FEA034591CCBFAC81
    //Q:F36DD803C61CC04956FF2420DC336583FD4D3B568925FD52A2A81C57BC9D6B
    //DP:528AB7AB41693245CF44B318B689E555395C65C4EA1A311A67318A05573C16C29A8D47487C0BF7BE4F0C7E61FAD9C4244213212F68DEDEF6F351893FF3737F193B265327D7A56657761EB48597420396F9D89933A698CA0998C6A3FA875734C720AA567BA1A081C424B8367D9A0044579524720EE341C6075C4AC19218BE9481
    //DQ:F54A1BAB398DAEDDB6E69A2A7EE9BEE01B581E8F80476903288D90A63AAC9B
    //Qinv:895EE03B436CDABC215EE11730EC605C4047B1B7A7F2DA3DB806A1278AD3CC2
    mpi_read_string(&n, 16,
"CB2093024AE8893B6BE5BA44635DF66EB6FDF441E03211099D2E16CD9C395B32EDF2D0F05819D753D16812AA6B0248B88F803F6BB7B1DFDEEF54568F6B2C5DB232E9B6F4B618445D4CE0ED443A64F048BEEA2A8D812346634560C8CC3641B67F02D135DC2D56D8F69EFB364DB8CA26716455690CD2399FDF00FC6E54274785646A93718A0D510969AF60B7DD5854103D3A4217259680A764CBC77FCC2E135736A8F4EB7350188BEA5A4F0B787B2E097C2E8F71B2A8D80D513D721588CE24263EA3AF18136609E552B62203BF831781039A24511570153D5A39E1B314B0181F5E1471CBCB557C66913AE5596C9A4E0FC17B0E5A12E9442B101E7D8500A6A436EB"
                    );
    mpi_read_string(&e, 16,"010001");
    memcpy(rsa_pub.modulus,n.p,n.n<<2);
    memset(rsa_pub.exponent,0,4);
    memcpy(rsa_pub.exponent,e.p,e.n<<2);
    rsa_pub.bits = n.n<<5 ; 
    mpi_free(&n);
    mpi_free(&e);
    cycle_start = hw_get_cycle();
    Rsa_calt_pub(4, "\x01\x05\x99\x88", &rsa_pub,&outlen,out);
    cycle_end = hw_get_cycle();
    TRACE_BUF("Succe. cipher",out,outlen);
    //cipher:42C84DC012E8D58662D1AB363B63C197C5224522B96837DD41BF95797E3FAC49EAF59FF8752301DF20D363DB15624E60505F42C95EDD27A8212C9B37E164100249953AD0306FF7111590164B5D5296453B7DB391102C182D62F6E702EC0D4CCC66B79FA02DA9664490AC6976CDF7D72A92B4200F8D62C59BAC52475F1DA71B06B85AC60CE69280E01BED49817BFFF62B2DCE3E0A79AFA20D965A75152A4B0322F4BCD5A9ABF85F73CC197975EC99F8D6ABB9229E8BFA487FB56FD2A737192A95D7143F42A1CE5F8D6818397ABDA5E4A6664EF6056ACD24266B3F8D492C66B8373ABB38632E8E7BDADD2A1D7AB24916B95B75959585800EA6E802EF3B77566B01
    TRACE("\nRSA-%d encryption times:%dms",rsa_pub.bits,(cycle_end-cycle_start)/core_clk_khz);

    TRACE("\nRSA-2048 private");
    memset(&rsa_prv,0,sizeof(rsa_prv));
    memset(&n,0,sizeof(mpi));
    memset(&e,0,sizeof(mpi));
    memset(&d,0,sizeof(mpi));
    //E:"010001"
    //N:CB2093024AE8893B6BE5BA44635DF66EB6FDF441E03211099D2E16CD9C395B32EDF2D0F05819D753D16812AA6B0248B88F803F6BB7B1DFDEEF54568F6B2C5DB232E9B6F4B618445D4CE0ED443A64F048BEEA2A8D812346634560C8CC3641B67F02D135DC2D56D8F69EFB364DB8CA26716455690CD2399FDF00FC6E54274785646A93718A0D510969AF60B7DD5854103D3A4217259680A764CBC77FCC2E135736A8F4EB7350188BEA5A4F0B787B2E097C2E8F71B2A8D80D513D721588CE24263EA3AF18136609E552B62203BF831781039A24511570153D5A39E1B314B0181F5E1471CBCB557C66913AE5596C9A4E0FC17B0E5A12E9442B101E7D8500A6A436EB
    //D:2589848144F489FFB825C0F14867A1D5BDED5A491E34425FC37333F5952C28455621AC28EE2F0052D5FCE48E27AE496009ECCA4B8EABC9523A6A1C80EE6E497657E046D61C1C89EDAFFA54FE69C8237B5E048A0E9251C17D30E1FCD9782156A74C0EFB640FE0DFC15B89D2F433F4EF7BBEEDE5B6DFD6944BB2B3F3738BBAB4D49010FEF15A3EB751A1DE4C5C93E8AED97C0E9ABC304E9E14EA8045786A600555469E51D75976B1908E3D1C7C834DFE1157FDC3A3CF38DE22E09DD822E671DD5A4177EEA9D1040861E28862F31A85A99D5EEAEA644AD2313E2BEA93BE89F0FDF6D89CDFAD6604D0BBDE2974D072893CEF01276546FCC0DE11C9C87C875BD42301
    //P:9F21980204CB800CF8ACBE178778C435D9A294FFAFC8FEA034591CCBFAC81
    //Q:F36DD803C61CC04956FF2420DC336583FD4D3B568925FD52A2A81C57BC9D6B
    //DP:528AB7AB41693245CF44B318B689E555395C65C4EA1A311A67318A05573C16C29A8D47487C0BF7BE4F0C7E61FAD9C4244213212F68DEDEF6F351893FF3737F193B265327D7A56657761EB48597420396F9D89933A698CA0998C6A3FA875734C720AA567BA1A081C424B8367D9A0044579524720EE341C6075C4AC19218BE9481
    //DQ:F54A1BAB398DAEDDB6E69A2A7EE9BEE01B581E8F80476903288D90A63AAC9B
    //Qinv:895EE03B436CDABC215EE11730EC605C4047B1B7A7F2DA3DB806A1278AD3CC2
    mpi_read_string(&n, 16,
"CB2093024AE8893B6BE5BA44635DF66EB6FDF441E03211099D2E16CD9C395B32EDF2D0F05819D753D16812AA6B0248B88F803F6BB7B1DFDEEF54568F6B2C5DB232E9B6F4B618445D4CE0ED443A64F048BEEA2A8D812346634560C8CC3641B67F02D135DC2D56D8F69EFB364DB8CA26716455690CD2399FDF00FC6E54274785646A93718A0D510969AF60B7DD5854103D3A4217259680A764CBC77FCC2E135736A8F4EB7350188BEA5A4F0B787B2E097C2E8F71B2A8D80D513D721588CE24263EA3AF18136609E552B62203BF831781039A24511570153D5A39E1B314B0181F5E1471CBCB557C66913AE5596C9A4E0FC17B0E5A12E9442B101E7D8500A6A436EB"
                    );
    mpi_read_string(&d, 16,
"2589848144F489FFB825C0F14867A1D5BDED5A491E34425FC37333F5952C28455621AC28EE2F0052D5FCE48E27AE496009ECCA4B8EABC9523A6A1C80EE6E497657E046D61C1C89EDAFFA54FE69C8237B5E048A0E9251C17D30E1FCD9782156A74C0EFB640FE0DFC15B89D2F433F4EF7BBEEDE5B6DFD6944BB2B3F3738BBAB4D49010FEF15A3EB751A1DE4C5C93E8AED97C0E9ABC304E9E14EA8045786A600555469E51D75976B1908E3D1C7C834DFE1157FDC3A3CF38DE22E09DD822E671DD5A4177EEA9D1040861E28862F31A85A99D5EEAEA644AD2313E2BEA93BE89F0FDF6D89CDFAD6604D0BBDE2974D072893CEF01276546FCC0DE11C9C87C875BD42301"
                    );
    mpi_read_string(&e, 16,"010001");
    memcpy(rsa_prv.tPubinfo.modulus,n.p,n.n<<2);
    memcpy(rsa_prv.tPubinfo.exponent,e.p,e.n<<2);
    memcpy(rsa_prv.prikey,d.p,d.n<<2);
    rsa_prv.tPubinfo.bits = n.n<<5 ; 
    mpi_free(&n);
    mpi_free(&e);
    mpi_free(&d);
    cycle_start = hw_get_cycle();
    Rsa_calt_pri(outlen, out, &rsa_prv,&outlen,out1);
    cycle_end = hw_get_cycle();
    TRACE_BUF("Succe. cipher",out1,outlen);
    //cipher:42C84DC012E8D58662D1AB363B63C197C5224522B96837DD41BF95797E3FAC49EAF59FF8752301DF20D363DB15624E60505F42C95EDD27A8212C9B37E164100249953AD0306FF7111590164B5D5296453B7DB391102C182D62F6E702EC0D4CCC66B79FA02DA9664490AC6976CDF7D72A92B4200F8D62C59BAC52475F1DA71B06B85AC60CE69280E01BED49817BFFF62B2DCE3E0A79AFA20D965A75152A4B0322F4BCD5A9ABF85F73CC197975EC99F8D6ABB9229E8BFA487FB56FD2A737192A95D7143F42A1CE5F8D6818397ABDA5E4A6664EF6056ACD24266B3F8D492C66B8373ABB38632E8E7BDADD2A1D7AB24916B95B75959585800EA6E802EF3B77566B01
    TRACE("\nRSA-%d decryption times:%dms",rsa_pub.bits,(cycle_end-cycle_start)/core_clk_khz);
}



/*
 * Checkup routine
 */
int dbg_rsa_1024( int verbose )
{
    size_t len;
    int iRet;
    rsa_context rsa;
    unsigned char rsa_plaintext[PT_LEN];
    unsigned char rsa_decrypted[PT_LEN];
    unsigned char rsa_ciphertext[KEY_LEN];
    unsigned char sha1sum[20];
    uint32_t cycle_start=0;
    uint32_t cycle_end=0;

    cycle_start = hw_get_cycle();
    rsa_init( &rsa, RSA_PKCS_V15, 0 );

#if 2
    TRACE_BUF("n_src",rsa1024_n,sizeof(rsa1024_n));
    mpi_read_big_endian(&rsa.N,rsa1024_n,sizeof(rsa1024_n));
    TRACE_BUF("n_dest",(uint8_t *)rsa.N.p,rsa.N.n<<2);
    TRACE_BUF("e_src",rsa1024_e,sizeof(rsa1024_e));
    mpi_read_big_endian(&rsa.E,rsa1024_e,sizeof(rsa1024_e));
    TRACE_BUF("e_dest",(uint8_t *)rsa.E.p,rsa.E.n<<2);
    rsa_free( &rsa );
    return 0;
//    mpi_read_big_endian( &rsa.D , 16, RSA_D  );
#endif
//    rsa.len = KEY_LEN;
    mpi_read_string( &rsa.N , 16, RSA_N  );
    mpi_read_string( &rsa.E , 16, RSA_E  );
    mpi_read_string( &rsa.D , 16, RSA_D  );
#if defined(POLARSSL_RSA_NO_CRT)
    mpi_read_string( &rsa.P , 16, RSA_P  );
    mpi_read_string( &rsa.Q , 16, RSA_Q  );
    mpi_read_string( &rsa.DP, 16, RSA_DP );
    mpi_read_string( &rsa.DQ, 16, RSA_DQ );
    mpi_read_string( &rsa.QP, 16, RSA_QP );
#endif

    rsa.len = ( mpi_msb( &rsa.N ) + 7 ) >> 3;

    if( verbose == 2 )
        TRACE( "\n RSA-%d key validation: ",rsa.len*8 );

//    if( rsa_check_pubkey(  &rsa ) != 0 ||
//        rsa_check_privkey( &rsa ) != 0 )
//    {
//        if( verbose != 0 )
//            TRACE( "failed\n" );
//
//        goto RSA_CLEAN;
//    }

    if( verbose == 2 )
        TRACE( "\npassed\n  PKCS#1 encryption : " );

    memcpy( rsa_plaintext, RSA_PT, PT_LEN );

    if( (iRet = rsa_pkcs1_encrypt( &rsa, &myrand, NULL, RSA_PUBLIC, PT_LEN,
                           rsa_plaintext, rsa_ciphertext )) != 0 )
    {
        if( verbose != 0 )
            TRACE( "failed:%04X\n",-iRet );

        goto RSA_CLEAN;
    }
    cycle_end = hw_get_cycle();
    TRACE("\nRSA-%d encryption times:%dms",rsa.len*8,(cycle_end-cycle_start)/core_clk_khz);

    if( verbose == 2 )
        TRACE( "\npassed\n  PKCS#1 decryption : " );

    cycle_start = hw_get_cycle();
    if( (iRet = rsa_pkcs1_decrypt( &rsa, RSA_PRIVATE, &len,
                           rsa_ciphertext, rsa_decrypted,
                           sizeof(rsa_decrypted) )) != 0 )
    {
        if( verbose != 0 )
            TRACE( "failed:%04X\n",-iRet );

        goto RSA_CLEAN;
    }
    cycle_end = hw_get_cycle();
    TRACE("\nRSA-%d decryption times:%dms",rsa.len*8,(cycle_end-cycle_start)/core_clk_khz);

    if( memcmp( rsa_decrypted, rsa_plaintext, len ) != 0 )
    {
        if( verbose != 0 )
            TRACE( "failed\n" );

        goto RSA_CLEAN;
    }

    if( verbose == 2 )
        TRACE( "\npassed\n  PKCS#1 data sign  : " );

    cycle_start = hw_get_cycle();
    sha1( rsa_plaintext, PT_LEN, sha1sum );

    if( (iRet = rsa_pkcs1_sign( &rsa, NULL, NULL, RSA_PRIVATE, SIG_RSA_SHA1, 20,
                        sha1sum, rsa_ciphertext )) != 0 )
    {
        if( verbose != 0 )
            TRACE( "failed:%04X\n",-iRet );

        goto RSA_CLEAN;
    }
    cycle_end = hw_get_cycle();
    TRACE("\nRSA-%d sign times:%dms",rsa.len*8,(cycle_end-cycle_start)/core_clk_khz);

    if( verbose == 2 )
        TRACE( "\npassed\n  PKCS#1 sig. verify: " );

    cycle_start = hw_get_cycle();
    if( (iRet = rsa_pkcs1_verify( &rsa, RSA_PUBLIC, SIG_RSA_SHA1, 20,
                          sha1sum, rsa_ciphertext )) != 0 )
    {
        if( verbose != 0 )
            TRACE( "failed:%04X\n",-iRet );
        goto RSA_CLEAN;

    }
    cycle_end = hw_get_cycle();
    TRACE("\nRSA-%d verify times:%dms",rsa.len*8,(cycle_end-cycle_start)/core_clk_khz);

    if( verbose != 0 )
        TRACE( "\npassed\n\n" );

RSA_CLEAN:
    rsa_free( &rsa );

    return( 0 );
}

int dbg_rsa_2048( int verbose )
{
    size_t len;
    int iRet;
    rsa_context rsa;
    unsigned char rsa_plaintext[PT_LEN];
    unsigned char rsa_decrypted[PT_LEN];
    unsigned char rsa_ciphertext[KEY_LEN_2048];
    unsigned char sha1sum[20];
    uint32_t cycle_start=0;
    uint32_t cycle_end=0;

    cycle_start = hw_get_cycle();
    rsa_init( &rsa, RSA_PKCS_V15, 0 );

//    rsa.len = KEY_LEN_2048;
    mpi_read_string( &rsa.N , 16, RSA_N_2048  );
    mpi_read_string( &rsa.E , 16, RSA_E_2048  );
    mpi_read_string( &rsa.D , 16, RSA_D_2048  );
#if defined(POLARSSL_RSA_NO_CRT)
    mpi_read_string( &rsa.P , 16, RSA_P_2048  );
    mpi_read_string( &rsa.Q , 16, RSA_Q_2048  );
    mpi_read_string( &rsa.DP, 16, RSA_DP_2048 );
    mpi_read_string( &rsa.DQ, 16, RSA_DQ_2048 );
    mpi_read_string( &rsa.QP, 16, RSA_QP_2048 );
#endif

    rsa.len = ( mpi_msb( &rsa.N ) + 7 ) >> 3;
    if( verbose == 2 )
        TRACE( "\nRSA-%d key validation: ",rsa.len*8 );

//    if( rsa_check_pubkey(  &rsa ) != 0 ||
//        rsa_check_privkey( &rsa ) != 0 )
//    {
//        if( verbose != 0 )
//            TRACE( "failed\n" );
//
//        goto RSA_CLEAN;
//    }

    if( verbose == 2 )
        TRACE( "\npassed\n  PKCS#1 encryption : " );

    memcpy( rsa_plaintext, RSA_PT, PT_LEN );

    if( (iRet = rsa_pkcs1_encrypt( &rsa, &myrand, NULL, RSA_PUBLIC, PT_LEN,
                           rsa_plaintext, rsa_ciphertext )) != 0 )
    {
        if( verbose != 0 )
            TRACE( "failed:%04X\n",-iRet );

        goto RSA_CLEAN;
    }
    cycle_end = hw_get_cycle();
    TRACE("\nRSA-%d encryption times:%dms",rsa.len*8,(cycle_end-cycle_start)/core_clk_khz);

    if( verbose == 2 )
        TRACE( "\npassed\n  PKCS#1 decryption : " );

    cycle_start = hw_get_cycle();
    if( (iRet = rsa_pkcs1_decrypt( &rsa, RSA_PRIVATE, &len,
                           rsa_ciphertext, rsa_decrypted,
                           sizeof(rsa_decrypted) )) != 0 )
    {
        if( verbose != 0 )
            TRACE( "\nfailed:%04X\n",-iRet );

        goto RSA_CLEAN;
    }
    cycle_end = hw_get_cycle();
    TRACE("\nRSA-%d decryption times:%dms",rsa.len*8,(cycle_end-cycle_start)/core_clk_khz);

    if( memcmp( rsa_decrypted, rsa_plaintext, len ) != 0 )
    {
        if( verbose != 0 )
            TRACE( "failed\n" );

        goto RSA_CLEAN;
    }

    if( verbose == 2 )
        TRACE( "\npassed\n  PKCS#1 data sign  : " );

    cycle_start = hw_get_cycle();
    sha1( rsa_plaintext, PT_LEN, sha1sum );

    if( (iRet = rsa_pkcs1_sign( &rsa, NULL, NULL, RSA_PRIVATE, SIG_RSA_SHA1, 20,
                        sha1sum, rsa_ciphertext )) != 0 )
    {
        if( verbose != 0 )
            TRACE( "failed:%04X\n",-iRet );

        goto RSA_CLEAN;
    }
    cycle_end = hw_get_cycle();
    TRACE("\nRSA-%d sign times:%dms",rsa.len*8,(cycle_end-cycle_start)/core_clk_khz);

    if( verbose == 2 )
        TRACE( "\npassed\n  PKCS#1 sig. verify: " );

    cycle_start = hw_get_cycle();
    if( (iRet = rsa_pkcs1_verify( &rsa, RSA_PUBLIC, SIG_RSA_SHA1, 20,
                          sha1sum, rsa_ciphertext )) != 0 )
    {
        if( verbose != 0 )
            TRACE( "failed:%04X\n",-iRet );

        goto RSA_CLEAN;
    }
    cycle_end = hw_get_cycle();
    TRACE("\nRSA-%d verify times:%dms",rsa.len*8,(cycle_end-cycle_start)/core_clk_khz);

    if( verbose != 0 )
        TRACE( "\npassed\n\n" );

RSA_CLEAN:
    rsa_free( &rsa );

    return( 0 );
}

/*  
RSA-1024 encryption times:102ms  | 18ms
RSA-1024 decryption times:231ms 
RSA-1024 sign   times:226ms
RSA-1024 verify times:12ms

RSA-2048 encryption times:309ms  | 60ms
RSA-2048 decryption times:1446ms | 4952ms
RSA-2048 sign   times:1435ms
RSA-2048 verify times:45ms

RSA-1024 generate cost times:6520ms   8420ms  23970ms  11710ms  11970ms   11130ms
RSA-2048 generate cost times:139500ms 90560ms 139900ms 249330ms 395470ms  198940ms
 *  */
void dbg_rsa(void)
{
    hw_cycle_open();
    dbg_rsa_1024(1);
//    dbg_rsa_2048(1);
    dbg_rsa_pub_encrypt();
//    dbg_rsa_gen();
    hw_cycle_close();
}


#endif

//#define CFG_DBG_USB
#ifdef CFG_DBG_USB
//uint8_t gucBuff[5*1024];
void dbg_usb(void)
{
//    extern uint8_t gucBuff[5*1024];
    uint8_t rcvbuffer[2048+64];
    int iRet,len=64;
    while (1)
    {
        TRACE("\n1-open 2-send 3-recv 4-recv_send 5-close");
        switch (InkeyCount(0))
        {
        case 1:
            drv_usbd_cdc_open(gucBuff,2048+64);
            break;
        case 2:
            memcpy(rcvbuffer,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ3456789012345",64);
            iRet = drv_usbd_cdc_write(rcvbuffer,len);
            TRACE("\nsend:%d",iRet);
            break;
        case 3:
            iRet = drv_usbd_cdc_check_readbuf();
            if (iRet > 0)
            {
                len = drv_usbd_cdc_read(rcvbuffer,iRet,1000);
                TRACE("\niRet:%d len:%d",iRet,len);
                TRACE_BUF("Read",rcvbuffer,len);
            }
            else
                TRACE("\ncheck rebuff:%d",iRet);
            break;
        case 4:
            while (1)
            {
                iRet = drv_usbd_cdc_check_readbuf();
                if (iRet > 0)
                {
                    len = drv_usbd_cdc_read(rcvbuffer,iRet,1000);
//                    TRACE("\niRet:%d len:%d",iRet,len);
//                    TRACE_BUF("Read",rcvbuffer,len);
                    iRet = drv_usbd_cdc_write(rcvbuffer,len);
                    s_DelayMs(200);
                }
            }
            break;
        case 5:
            drv_usbd_cdc_close();
            break;
        case 99:
            drv_usbd_cdc_close();
            return;
        default:
            break;
        }
    }
}
#endif

#ifdef CFG_DBG_LCD
//#define SPI_SOFT

void dbg_spi(void)
{
    int key;
    
    while (1)
    {
        TRACE("\r\n-|***************spi debug***************|-");
        TRACE("\r\n-|1-init 2-w_55 3-w_AA 4-reg             |-");
        TRACE("\r\n-|***************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            hw_lcd_spi_init();
//            spi_init(LCD_SPIn,MASTER);
            break;
        case 2:
            hw_spi_master_write(LCD_SPIn,SPI_PCS0,"\x55\xAA",2); //Software reset
            break;
        case 3:
            hw_spi_master_write(LCD_SPIn,SPI_PCS0,"\xAA\xAA",2); //Software reset
            break;
        case 4:
            dbg_spi_reg(LCD_SPIn);
            break;
        case 99:
            return;
        }
    }
}

void test(uint8_t ph, uint8_t pl)
{
    int page,i;

    for(page=0;page<8;page++)
    {
        hw_lcd_set_column(0);
        hw_lcd_set_page(page);
        for(i=0;i<64;i++) {
            hw_lcd_write_bytedata(ph);
            hw_lcd_write_bytedata(pl);
        }
    }
}

void graphic(const uint8_t *pic)
{
    int page;

    for(page=0;page<8;page++)
    {
        hw_lcd_set_column(0);
        hw_lcd_set_page(page);
        hw_lcd_write_data(pic,128);
        pic += 128;
    }
}


uint8_t const BMP[]={/*--  调入了一幅图像：D:\My Documents\My Pictures\128 X64 (1).bmp  --*/
    /*--  宽度x高度=128x64  --*/
    0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,
    0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x07,0x07,0xFF,0xFF,0xFF,0xFF,0x87,0x87,0x87,0x87,0x87,0x87,0x87,0xCF,
    0xFF,0xFE,0xFC,0x78,0x00,0x00,0x00,0x07,0x07,0xFF,0xFF,0xFF,0xFF,0x07,0x07,0x00,
    0x00,0x00,0x07,0x07,0xFF,0xFF,0xFF,0xFF,0x07,0x07,0x07,0xE7,0xE7,0x07,0x0F,0x1F,
    0x7F,0x3C,0x30,0x00,0x07,0x07,0xFF,0xFF,0xFF,0xFF,0x07,0x07,0x07,0x07,0x07,0x0F,
    0x0F,0x3E,0xFE,0xFC,0xF8,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xFE,0xFF,
    0x1F,0x3F,0xFF,0xFF,0xF8,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
    0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x80,0x80,0xFF,0xFF,0xFF,0xFF,0x83,0x83,0x07,0x0F,0x1F,0x3F,0x7F,0xFF,
    0xF9,0xF1,0xE0,0xC0,0x80,0x80,0x80,0x80,0x80,0xFF,0xFF,0xFF,0xFF,0x80,0x80,0x00,
    0x00,0x00,0x80,0x80,0xFF,0xFF,0xFF,0xFF,0x87,0x87,0x07,0x3F,0x3F,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x80,0x80,0xFF,0xFF,0xFF,0xFF,0x80,0x80,0x80,0x80,0x80,0xC0,
    0xE0,0xF0,0xFF,0x7F,0x3F,0x0F,0x00,0x00,0x80,0x80,0xC0,0xF8,0xFF,0xBF,0x9F,0x1C,
    0x1C,0x1C,0x9C,0x9F,0x9F,0xFF,0xFF,0xFC,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
    0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
    0x01,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,
    0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
    0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,
    0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
    0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x90,0x90,
    0x90,0x20,0x70,0x00,0x10,0xF0,0x90,0x80,0x80,0x90,0xF0,0x10,0x00,0x10,0xF0,0x90,
    0x90,0xD0,0x10,0x20,0x00,0x10,0xF0,0x30,0xC0,0x00,0x10,0xF0,0x10,0x00,0x20,0x10,
    0x10,0x90,0x50,0x30,0x10,0x00,0x10,0xF0,0x90,0x80,0x80,0x90,0xF0,0x10,0x00,0x10,
    0xF0,0x90,0x90,0xD0,0x10,0x20,0x00,0x10,0xF0,0x30,0xC0,0x00,0x10,0xF0,0x10,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
    0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x04,0x08,
    0x08,0x09,0x07,0x00,0x08,0x0F,0x08,0x00,0x00,0x08,0x0F,0x08,0x00,0x08,0x0F,0x08,
    0x08,0x09,0x08,0x06,0x00,0x08,0x0F,0x08,0x00,0x01,0x06,0x0F,0x00,0x00,0x08,0x0C,
    0x0A,0x09,0x08,0x08,0x06,0x00,0x08,0x0F,0x08,0x00,0x00,0x08,0x0F,0x08,0x00,0x08,
    0x0F,0x08,0x08,0x09,0x08,0x06,0x00,0x08,0x0F,0x08,0x00,0x01,0x06,0x0F,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
    0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x10,0xF0,0x90,0x90,0x90,0x90,0x60,0x00,0x10,0xF0,0x10,0x00,0x10,0xF0,0x90,
    0x90,0xD0,0x10,0x20,0x10,0xF0,0x10,0x10,0x10,0x10,0x20,0xC0,0x00,0x00,0x00,0xE0,
    0x10,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0xF0,0x10,0x00,0x00,0x00,0x00,
    0xC0,0x20,0x10,0x10,0x10,0x20,0x70,0x00,0x10,0xF0,0x10,0x10,0x10,0x10,0x20,0xC0,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0xF0,0x10,0x00,0x00,0x00,0x00,
    0x20,0x10,0x10,0xF0,0x10,0x10,0x20,0x00,0x10,0xF0,0x10,0x10,0x10,0x10,0x20,0xC0,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
    0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x88,0x8F,0x88,0x81,0x82,0x84,0x88,0x80,0x88,0x8F,0x88,0x80,0x88,0x8F,0x88,
    0x80,0x81,0x80,0x80,0x88,0x8F,0x88,0x88,0x88,0x88,0x84,0x83,0x80,0x88,0x8F,0x81,
    0x81,0x81,0x8F,0x88,0x80,0x80,0x80,0x80,0x80,0x88,0x8F,0x88,0x88,0x88,0x88,0x84,
    0x83,0x84,0x88,0x88,0x88,0x88,0x84,0x80,0x88,0x8F,0x88,0x88,0x88,0x88,0x84,0x83,
    0x80,0x80,0x88,0x80,0x80,0x80,0xA8,0x98,0x80,0x88,0x8F,0x88,0x88,0x88,0x88,0x84,
    0x80,0x80,0x88,0x8F,0x88,0x80,0x80,0x80,0x88,0x8F,0x88,0x88,0x88,0x88,0x84,0x83,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,
};
/****************************************************/
//图片存放
const uint8_t BMP1[]={
    //----- d:\My Documents\桌面\106391ceshi\12864.bmp
    //----- L - H, x direct, 8 pixel mode, Mono color.
    //----- width x height = 128 x 64, Total bytes = 1024
    0xff,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xff,
    0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
    0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,
    0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0xc2,0xf2,0xfe,0x7e,0x1e,0x06,0x02,
    0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x02,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0xf8,0xf0,0x10,0x10,0x10,0x10,0x11,0x17,
    0x1f,0x1f,0x10,0x10,0x10,0x10,0x10,0xf8,0xf8,0xf8,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
    0xff,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0xff,0xff,0xff,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
    0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x80,0xc0,0xe0,0x78,0x3c,0x1f,0x0f,0xff,0xff,0xff,0x03,0x00,0x00,
    0x04,0x0c,0x18,0x30,0x70,0xe0,0xc0,0xc0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x82,0x82,0x82,0x82,0x82,0x82,
    0x82,0x82,0x82,0x82,0x82,0x82,0x82,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x1f,0x1f,
    0x1f,0x84,0x84,0x84,0x04,0x04,0x04,0x84,0x84,0x84,0x04,0x1f,0x1f,0x0f,0x00,0x00,
    0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
    0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x30,0x18,
    0x0c,0x0e,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x1f,0x1f,0x1e,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00,0x03,0x0f,0x3c,
    0x78,0xe0,0xe0,0xa0,0x30,0x10,0x18,0x19,0x0d,0x0f,0x0e,0x06,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x01,0x01,0x01,0x01,0x01,
    0x01,0xff,0xff,0xff,0x01,0x00,0x00,0xff,0xff,0xff,0x01,0x01,0x01,0x01,0x01,0x01,
    0xff,0xff,0xff,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
    0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x3f,0x18,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x3f,0x3f,0x1f,0x0c,0x0c,0x06,0x02,0x03,0x01,
    0x01,0x00,0x01,0x03,0x03,0x07,0x06,0x0e,0x0c,0x1c,0x1c,0x1c,0x18,0x18,0x08,0x08,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x1f,0x02,0x02,0x02,0x02,0x02,
    0x02,0x1f,0x1f,0x0f,0x00,0x00,0x00,0x1f,0x1f,0x1f,0x02,0x02,0x02,0x02,0x02,0x02,
    0x1f,0x1f,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
    0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
    0xff,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xff,
};

void test2(void)
{
//    hw_lcd_write_cmd(0x24);       //3.5 //6.5   // 4.5 Regulation Ratio = 5.0
//    hw_lcd_write_cmd(0x81);        // EV Setting
//    hw_lcd_write_cmd(0x2b);        // 00 EV = 20H //fine adjust  28
    while (1)
    {
        TRACE("\n-|BMP");
        graphic(BMP);
        s_DelayMs(500);
        TRACE("\n-|BMP1");
        graphic(BMP1);
        s_DelayMs(500);
        TRACE("\n-|test 55-55");
        test(0x55,0x55);
        s_DelayMs(500);
        if (drv_kb_hit())
        {
            drv_kb_clear();
            if (IfInkey(0))
            {
                break;
            }
        }
    }
}

void dbg_lcd_line(void)
{
	uint8_t sx, sy;
	uint8_t ex, ey;
	uint8_t val;
	
	TRACE("\r\n**Draw straight line..");
	TRACE("\r\n |0-Auto 1-manual");
	val = InkeyCount(0);
	if (val == 0)
	{
		TRACE("\r\n\t L1:(0,0),(63,63) L2:(0,0),(127,31) L3:(32,32),(127,31)");
		drv_lcd_drawline(0,0,63,63,1);
		drv_lcd_drawline(0,0,127,31,1);
		drv_lcd_drawline(63,63,127,31,1);
		return;
	}
	while (1)
	{
		TRACE("\r\n**Draw straight line..");
		TRACE("\r\n\t-Start_X:[0,127]");
		sx = InkeyCount(0);
		TRACE("\r\n\t-Start_Y:[0,63]");
		sy = InkeyCount(0);
		TRACE("\r\n\t-End_X:[0,127]");
		ex = InkeyCount(0);
		TRACE("\r\n\t-End_Y:[0,63]");
		ey = InkeyCount(0);
		TRACE("\r\n\t-Val:0-white 1-black");
		val = InkeyCount(0);	
		drv_lcd_drawline(sx,sy,ex,ey,val);

		TRACE("\r\n\t**1-exit!");
		if (InkeyCount(0)==1)
			break;
	}	
}

void dbg_lcd_cricle(void)
{
	uint8_t xc, yc;
	uint8_t r;
	uint8_t val;

	TRACE("\r\n**Draw Circle..");
	TRACE("\r\n |0-Auto 1-manual");
	val = InkeyCount(0);
	if (val == 0)
	{
		TRACE("\r\n\tO1:(63,31)8  O2:(63,31)16 O3:(63,31)32 ");
		drv_lcd_circle(63,31,8,1);
		drv_lcd_circle(63,31,16,1);
		drv_lcd_circle(63,31,32,1);
		return;
	}
	while (1)
	{
		TRACE("\r\n**Draw cricle..");
		TRACE("\r\n\t-Cricle dot X:[0,127]");
		xc = InkeyCount(0);
		TRACE("\r\n\t-Cricle dot Y:[0,63]");
		yc = InkeyCount(0);
		TRACE("\r\n\t-Cricle radius:[1,127]");
		r = InkeyCount(0);
		val = 1;	
		drv_lcd_circle(xc,yc,r,val);

		TRACE("\r\n\t**1-exit!");
		if (InkeyCount(0)==1)
			break;
	}	
}

void dbg_lcd_rectangle(void)
{
	uint8_t x,y;
	uint8_t length,width;
	uint8_t input[1024];

	TRACE("\r\n\trectangle start coordinate(x,y)");
	TRACE("\r\n\t-x:[0-131]");
	x = InkeyCount(0);
	TRACE("\r\n\t-y:[0- 63]");
	y = InkeyCount(0);
	TRACE("\r\n\tlength:");
	length = InkeyCount(0);
	TRACE("\r\n\twidth :");
	width = InkeyCount(0);
	memset(input,0xFF,sizeof(input));
    drv_lcd_FillVertMatrix(x,y,length,width,input);
}

void dbg_lcd(void)
{
    int key;
    int i =0;
//    char ch,assii[255];
    
    while (1)
    {
        TRACE("\r\n-|**********************lcd debug************************|-");
        TRACE("\r\n-|1-spi_dbg 2-lcd init 3-w_data 4-w_cmd 5-reg 6-tmp 7-bmp|-");
        TRACE("\r\n-|8-clr 9-line 10-cricle 11-rectangle                    |-");
        TRACE("\r\n-|*******************************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            dbg_spi();
            break;
        case 2:
            drv_lcd_init(LCD_MODE_TIMEROUT);
            break;
        case 3:
            hw_lcd_write_bytedata(0x55); 
            break;
        case 4:
            hw_lcd_write_cmd(0x55); 
            break;
        case 5:
            dbg_spi_reg(LCD_SPIn);
            break;
        case 6:
            hw_lcd_write_cmd(0x10);
            hw_lcd_write_cmd(0x00);
            hw_lcd_write_cmd(0+0xb0);
            for(i=0;i<64;i++) {
                hw_lcd_write_bytedata(0xD5);
                hw_lcd_write_bytedata(0xFF);
            }
            hw_lcd_write_cmd(0x10);
            hw_lcd_write_cmd(0x00);
            hw_lcd_write_cmd(1+0xb0);
            for(i=0;i<64;i++) {
                hw_lcd_write_bytedata(0x00);
                hw_lcd_write_bytedata(0x00);
            }
            break;
        case 7:
            test2();
            break;
        case 8:
            drv_lcd_cls();
            break;
        case 9:
            dbg_lcd_line();
            break;
        case 10:
            dbg_lcd_cricle();
            break;
        case 11:
            dbg_lcd_rectangle();
            break;
        case 12:
            gui_drawbox(2,2,100,60,BoxModel_NoBox,FillType_Clear); //清扫出一片空地
            InkeyCount(0);
            gui_drawbox(2,2,100,60,BoxModel_Draw,FillType_NoFill); //窗体轮廓
            InkeyCount(0);
            gui_drawbox(3,3,99,22,BoxModel_NoBox,FillType_Clear); //反显效果，注意坐标的细节
            break;
        case 99:
            return;
        }
    }
}
#endif


#ifdef CFG_DBG_LPWR
void dbg_llwu(void)
{
    PORT_InitTypeDef port_init;
    PORTC_PCR11 =(PORT_PCR_ISF_MASK | //clear flag if there
                  PORT_PCR_MUX(1) | //set pin functionality -GPIO
                  PORT_PCR_IRQC(0x0A)| //falling edge interrupt enable
                  PORT_PCR_PE_MASK | // pull enable
                  PORT_PCR_PS_MASK); // pullup enable
    SMC_PMPROT =  SMC_PMPROT_AVLLS_MASK |
                  SMC_PMPROT_ALLS_MASK|
                  SMC_PMPROT_AVLP_MASK;
    TRACE("\n");
    llwu_configure(1<<11,0x02,0); //SIM_SCGC4_LLWU_MASK
    {
        //keyboard row init
        port_init.PORTx = KB_PORT_ROW;
        port_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
        port_init.PORT_Pin = KB_PINx_ROW1;
        hw_port_init(&port_init);
        port_init.PORT_Pin = KB_PINx_ROW1;
        hw_port_init(&port_init);
        port_init.PORT_Pin = KB_PINx_ROW1;
        hw_port_init(&port_init);
        port_init.PORT_Pin = KB_PINx_ROW1;
        hw_port_init(&port_init);
        hw_set_gpio_output(KB_GPIO_ROW,(1<<KB_PINx_ROW1)|(1<<KB_PINx_ROW1)|(1<<KB_PINx_ROW1)|(1<<KB_PINx_ROW1));
        hw_kb_reset_row(KB_COL_MAXTRIX_MASK);
    }
}


void dbg_lowerpower(void)
{
    int key;

    
    /* Need to make sure the clock monitor(s) is disabled if using
     * an external clock source. This assumes OSC0 is used as the 
     * external clock source and that the clock gate for MCG has 
     * already been enabled.
     */
    MCG_C6 &= ~MCG_C6_CME0_MASK; //CME=0 clock monitor disable

    while (1)
    {
        TRACE("\r\n-|*****************lowerpower debug*********************|-");
        TRACE("\r\n-|-1-VLLS1 2-VLLS2 3-VLLS3 4-LLS 5-stop                 |-");
        TRACE("\r\n-|******************************************************|-\t");
        key = InkeyCount(0);
        hw_led_off(LED_ALL);
        switch (key)
        {
        case 1:
            dbg_llwu();
            enable_irq(LLW_IRQn);
            enable_lpwui();
            enter_vlls1(); 
            break;
        case 2:
            dbg_llwu();
            enable_irq(LLW_IRQn);
            enable_lpwui();
            enter_vlls2(); 
            break;
        case 3:
            dbg_llwu();
            enable_irq(LLW_IRQn);
            enable_lpwui();
            enter_vlls3(); 
            break;
        case 4:
            dbg_llwu();
            enable_irq(LLW_IRQn);
            enter_lls(); 
            hw_led_on(LED_ALL);
            if(what_mcg_mode()==PBE)
            {
                mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
            }
            break;
        case 5:
            enable_irq(LLW_IRQn);
            enter_stop();
            hw_led_on(LED_ALL);
            if(what_mcg_mode()==PBE)
            {
                mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
            }
            break;
        case 99:
            return;
        default:
            break;
        }
//        enable_irq(LLW_IRQn);
        TRACE("\nwait mode");
        hw_systick_close();
        enter_wait();
        hw_systick_open();
        if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
            TRACE("\nin VLPR Mode!");
        } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
            TRACE("\nin Run Mode! ");
        }
    }
}
#endif

#ifdef CFG_DBG_CTRL_WORK_COMPORT 
static void dbg_wk_isr(void)
{
    char ch;

    ch = hw_uart_getchar(WORK_COMPORT);
    QueueWrite(UART_QUEUE_PTR(WORK_COMPORT),
               (unsigned char *)&ch,
               1);
}
void dbg_work_uart(void)
{
    int key;
    uint8_t ch;
    uint8_t buffer[5];
    uint32_t i;

    drv_kb_open();
    while (1)
    {
        TRACE("\r\nWork COMPORT: 1-init 2-wite 3-close 4--read");

        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            drv_uart_open(WORK_COMPORT,WORK_BAUD, buffer,sizeof(buffer),dbg_wk_isr);
            break;
        case 2:
            while (1)
            {
                if (drv_kb_hit())
                {
                   ch = (uint8_t)drv_kb_getkey(100);
                   TRACE(" |%02X",ch);
                   hw_uart_putchar(WORK_COMPORT,ch);
                }
                if (IfInkey(0))
                    break;
            }
            break;
        case 3:
            drv_uart_close(WORK_COMPORT);
            break;
        case 4:
            if (drv_uart_read(WORK_COMPORT,&ch,1,100) > 0)
                TRACE("\nread:%d",ch);
            else 
                TRACE("\nread time out");
            break;
        case 99:
            return;
        default:
            break;
        }
        TRACE("\n");
    }
}
#endif

#ifdef CFG_DBG_KEYBOARD
void hw_kb_notgpio_open(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = KB_GPIO_INDEPENDENT0;
//    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PU 
//        |PORT_Mode_IN_PFE|PORT_Mode_IRQ_EXTI_FEdge;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PU
        |PORT_Mode_IN_PFE;
    gpio_init.PORT_Pin = KB_PINx_INDEPENDENT0;
    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(KB_PINx_INDEPENDENT0);
    hw_gpio_init(&gpio_init);
}



void dbg_key(void)
{
    uint8_t buffer[16];
    int key;
    uint32_t i;
    while (1)
    {
        TRACE("\r\n-|******************key debug************************|-");
        TRACE("\r\n-|1-open 2-read 3-flush 4-hit 5-close  6-notmaxIO    |-");
        TRACE("\r\n-|***************************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            drv_kb_open();
            break;
        case 2:
            i = 0;
            TRACE("\n-|0x07-ALPHA 0x08-BackSpace 0x0D-Enter 0x1B-Cancel 0x13-SYMBOL 0x15-F1 0x2E-Clear");
            while (1)
            {
                if (drv_kb_hit()) {
                    if (i %16 == 0)
                        TRACE("\n-|");
                    key = drv_kb_getkey(1000);
                    TRACE(" %02X-%c",key,key);
                    i++;
                }
                if (IfInkey(0)) {
                    break;
                }
            }
            break;
        case 3:
            drv_kb_clear();
            break;
        case 4:
            if (drv_kb_hit())
                TRACE("\nhit");
            else
                TRACE("\nNULL");
            break;
        case 5:
            drv_kb_close();
            break;
        case 6:
            drv_kb_close();
            hw_kb_notgpio_open();
            while (1)
            {
                TRACE("\n-|%d",hw_kb_get_independent0());
                if (IfInkey(0)) {
                    break;
                }
            }
            break;
        case 7:
            key = drv_kb_read_buffer(buffer);
            if(key > 0)
            TRACE_BUF("",buffer,key);
            else
                TRACE("\nret:%d",key);
            break;
        case 99:
            return;
        default:
            break;
        }
    }
}
#endif



#ifdef CFG_DBG_ADC
//#define ADC_CHANNEL ADC_SC1n_ChnSelt_SE6a_b //980
//#define ADC_CHANNEL ADC_SC1n_ChnSelt_SE12 //towerboard
#define ADC_DIF 
void dbg_adc(void)
{
    int ad_val;
    uint8_t mux_ab;
    uint32_t times =0;

    TRACE("\n-|ADC Test");
#if (ADC_CHANNEL == ADC_SC1n_ChnSelt_SE12 )
    mux_ab = ADC_CFG2_Mux_ADxxa;
#endif
#if (ADC_CHANNEL == ADC_SC1n_ChnSelt_SE6a_b)
    mux_ab = ADC_CFG2_Mux_ADxxb;
#endif
#ifdef ADC_DIF
    mux_ab = ADC_CFG2_Mux_ADxxa;
#endif
    if (drv_adc_open(mux_ab,ADC_SC2_TRIGER_SOFTWARE,ADC_SC1n_SingleEnded,
                     ADC_SC3_HARDWARE_AVERAGE_ENABLE|ADC_SC3_HARDWARE_AVERAGE_32))
    {
        TRACE("\nError");
        return;
    }
    while (1) 
    {
        times++;
        TRACE("\n\n-|times:%03d",times);
#ifdef ADC_DIF 
        ad_val = drv_adc_caculate(ADC_SC1n_ChnSelt_SEDP0_DiffDAD0);
        TRACE("\n-|DP0[DAD0]AD Val:%04d ",ad_val);
#endif
        s_DelayMs(500);
#ifndef ADC_DIF 
        ad_val = drv_adc_caculate(ADC_CHANNEL);
        TRACE("\n AD Val:%04d ",ad_val);
        s_DelayMs(500);
#endif
  
        if (IfInkey(0)) {
            drv_adc_close(mux_ab);
            break;
        }
    }
}
#endif

#ifdef CFG_DBG_DRYICE
//uint Fac_SRAM(uint mode);
//#warning "yes"
extern void dryice_display_active_tamper_regs(void);
void dbg_dryice(void)
{
    int key;
    int ret;
    uint32_t skey[9];
//    uint8_t pin0,pin1,pin2;
//    uint8_t trig_level;
    uint32_t atr=0;
    struct  TamperEvent even;
    struct rtc_time tm;
    memset(skey,0x00,sizeof(skey));
    while (1)
    {
        TRACE("\r\n-|***************dryice********************|-");
        TRACE("\r\n-|1-DisReg 2-Open 3-write key 4-read key   |-");
        TRACE("\r\n-|5-reset  6-Irq  7-facsram 8-pin 9-active |-");
        TRACE("\r\n-|10-active_xor 11-disATR 12-get           |-");
        TRACE("\r\n-|*****************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            dbg_dryice_reg();
            break;
        case 2:
            drv_dryice_init((hal_dry_tamper_t)(DRY_TER_TPE(0x3F)|kDryTamper_Voltage|kDryTamper_Clock),
                            (hal_dry_interrupt_t)0,
                            0x2C,ENABLE);
//            pin0 = pin1 = pin2 = 0;
//            TRACE("\r\n-|pin0:1-enable 2-disable");
//            if (InkeyCount(0) == 1) {
//                pin0 = 1;
//            }
//            TRACE("\r\n-|pin1:1-enable 2-disable");
//            if (InkeyCount(0) == 1) {
//                pin1 = 1;
//            }
//            TRACE("\r\n-|pin2:1-enable 2-disable");
//            if (InkeyCount(0) == 1) {
//                pin2 = 1;
//            }
//            TRACE("\r\n-|trigger level:1-lower 2-high");
//            if (InkeyCount(0) == 1) {
//                trig_level = 0;
//            } else {
//                trig_level = 1;
//            }
//            drv_dryice_open(pin0,pin1,pin2,trig_level,ENABLE);
            dbg_dryice_reg();
//            break;
        case 3:
            SIM_SCGC5|=SIM_SCGC5_SECREG_MASK | SIM_SCGC5_DRYICE_MASK;
            memcpy(skey,"SEY1SEY2SEY3SEY4SEY5SEY6SEY7SEY8",32);
            ret = hw_dryice_write_key(0,8,skey);
            TRACE("\r\n ret:%x",ret);
            break;
        case 4:
            SIM_SCGC5|=SIM_SCGC5_SECREG_MASK | SIM_SCGC5_DRYICE_MASK;
            memset(skey,0x00,sizeof(skey));
            ret = hw_dryice_read_key(0,8,skey);
            TRACE("\r\n ret:%x",ret);
            if ( !ret ) {
                TRACE("\nkey:%s",skey);
                vDispBufTitle("key",sizeof(skey),0,skey);
            }
            break;
        case 5:
            TRACE("\nBefore Reset");
            dbg_dryice_reg();
            DRY_CR &= DRY_CR_SWR_MASK;
            TRACE("\nAfter Reset");
            dbg_dryice_reg();
            break;
        case 6:
            SIM_SCGC5|=SIM_SCGC5_SECREG_MASK | SIM_SCGC5_DRYICE_MASK;
            enable_irq(Tamper_IRQn); 
            break;
        case 7:
            TRACE("\n-|0-check 1-write");
            key = InkeyCount(0);
//            ret = Fac_SRAM(key);
            TRACE("\n-|ret:%02d",ret);
            break;
        case 8:
            TRACE("\n-|PIN:%02X",hw_dryice_read_pin());
            break;
        case 9:
            TRACE("\r\n-|active tamper mode: 1-pin0_out pin1_in  pin2_in ");
            TRACE("\r\n                      2-pin0_in  pin1_out pin2_in ");
            TRACE("\r\n                      3-pin0_in  pin1_in  pin2_out");
            TRACE("\r\n                      4-pin1_out pin2_in  active0  pin3_out pin4_in active1");
            TRACE("\n_%s",__TIME__);
            key = InkeyCount(0);
            DisableInterrupts;
            hw_dryice_init();
            switch (key)
            {
            case 1:
                drv_dryice_active_tamper_config(ACTIVE_TAMPER_0,0xAADD,0x5522);
                drv_dryice_active_output(TAMPER_PIN0,FILTER_TAMPER_PIN_ACTIVE_0);
                drv_dryice_active_input(TAMPER_PIN1,ACTIVE_TAMPER_0,10,FILTER_CLK_512_HZ);
                drv_dryice_active_input(TAMPER_PIN2,ACTIVE_TAMPER_0,10,FILTER_CLK_32_KHZ);
                break;
            case 2:
                drv_dryice_active_tamper_config(ACTIVE_TAMPER_0,0xAADD,0x5522);
                drv_dryice_active_output(TAMPER_PIN1,FILTER_TAMPER_PIN_ACTIVE_0);
                drv_dryice_active_input(TAMPER_PIN0,ACTIVE_TAMPER_0,10,FILTER_CLK_32_KHZ);
                drv_dryice_active_input(TAMPER_PIN2,ACTIVE_TAMPER_0,10,FILTER_CLK_512_HZ);
                break;
            case 3:
                drv_dryice_active_tamper_config(ACTIVE_TAMPER_0,0xAADD,0x5522);
                drv_dryice_active_output(TAMPER_PIN2,FILTER_TAMPER_PIN_ACTIVE_0);
                drv_dryice_active_input(TAMPER_PIN0,ACTIVE_TAMPER_0,10,FILTER_CLK_512_HZ);
                drv_dryice_active_input(TAMPER_PIN1,ACTIVE_TAMPER_0,10,FILTER_CLK_32_KHZ);
                break;
            case 4:
                drv_dryice_active_tamper_config(ACTIVE_TAMPER_0,0xADAD,0x525D);
                drv_dryice_active_output(TAMPER_PIN1,FILTER_TAMPER_PIN_ACTIVE_0);
                drv_dryice_active_input(TAMPER_PIN4,ACTIVE_TAMPER_0,10,FILTER_CLK_512_HZ);
                drv_dryice_active_tamper_config(ACTIVE_TAMPER_1,0xAADD,0x5522);
                drv_dryice_active_output(TAMPER_PIN3,FILTER_TAMPER_PIN_ACTIVE_1);
                drv_dryice_active_input(TAMPER_PIN2,ACTIVE_TAMPER_1,10,FILTER_CLK_512_HZ);
                break;
            default:
                break;
            }
//            dryice_int_tamper_interrupt_config(DRYICE_TAMPER,ENABLE);
//            enable_irq(Tamper_IRQn); 
            dryice_enable_TamperForceSystem(ENABLE);
            EnableInterrupts;
            while (1)
            {
                if (IfInkey(0))
                    break;
                if (atr != DRY_ATR0)
                {
                    atr = DRY_ATR0;
                    DRY_TRACE("\nDRY_ATR0 : %08X  ",atr);
                }
                s_DelayUs(10);
            }
            break;
        case 10:
            DisableInterrupts;
            hw_dryice_init();
            drv_dryice_active_tamper_config(ACTIVE_TAMPER_0,0xAADD,0x5522);
            drv_dryice_active_tamper_config(ACTIVE_TAMPER_1,0xAADD,0xA55A);
            drv_dryice_active_output(TAMPER_PIN1,FILTER_TAMPER_PIN_ACTIVE_0_XOR_1);
            drv_dryice_active_input(TAMPER_PIN2,ACTIVE_TAMPER_0_1,10,FILTER_CLK_32_KHZ);
            drv_dryice_active_input(TAMPER_PIN0,ACTIVE_TAMPER_0_1,10,FILTER_CLK_512_HZ);
//            dryice_int_tamper_interrupt_config(DRYICE_TAMPER,ENABLE);
//            enable_irq(Tamper_IRQn); 
            dryice_enable_TamperForceSystem(ENABLE);
            EnableInterrupts;
            break;
        case 11:
            SIM_SCGC5|=SIM_SCGC5_SECREG_MASK | SIM_SCGC5_DRYICE_MASK;
            dryice_display_active_tamper_regs();
            break;
        case 12:
            drv_dryice_get_reg(&even);
            if ( even.time_s ) {
               rtc_time_to_tm(even.time_s, &tm); 
               TRACE("year:%d,month:%d,day:%d,hour:%d,min:%d,sec:%d\r\n", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour,tm.tm_min,tm.tm_sec);
            }
            break;
        case 99:
            return;
        default:
            break;
        }
    }
}
#endif





