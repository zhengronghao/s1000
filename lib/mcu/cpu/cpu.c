/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : cpu.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/22/2014 3:28:15 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"


/********************************************************************/
/* Actual system clock frequency */
int mcg_clk_hz;
int mcg_clk_khz;
// 内核时钟50MHz
int core_clk_mhz=MCK_CLK;
int core_clk_khz=MCK_CLK/1000;
// 总线时钟50MHz
int periph_clk_khz=MCK_CLK/1000;

void cpu_clk_refresh(void)
{
    mcg_clk_khz = mcg_clk_hz / 1000;
    core_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
    core_clk_mhz = mcg_clk_khz/1000; 
    periph_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);
}

/********************************************************************/
void cpu_clk_init(void)
{
    /* Enable all of the port clocks. These have to be enabled to configure
     * pin muxing options, so most code will need all of these on anyway.
     */
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
                  | SIM_SCGC5_PORTB_MASK
                  | SIM_SCGC5_PORTC_MASK
                  | SIM_SCGC5_PORTD_MASK
                  | SIM_SCGC5_PORTE_MASK );

    /* Ramp up the system clock */
    /* Set the system dividers */
    /* NOTE: The PLL init will not configure the system clock dividers,
     * so they must be configured appropriately before calling the PLL
     * init function to ensure that clocks remain in valid ranges.
     */  
    if (PMC_REGSC &  PMC_REGSC_ACKISO_MASK)
        PMC_REGSC |= PMC_REGSC_ACKISO_MASK;	

    SIM_CLKDIV1 = ( 0
                    | SIM_CLKDIV1_OUTDIV1(0)    //Core/system       120 Mhz
                    | SIM_CLKDIV1_OUTDIV2(1)    //Busclk            60 Mhz         
                    | SIM_CLKDIV1_OUTDIV3(2)    //FlexBus           40 Mhz
                    | SIM_CLKDIV1_OUTDIV4(4) ); //Flash             24 Mhz  

//    //write to release hold on I/O         
//    PMC_REGSC |= PMC_REGSC_ACKISO_MASK;

    /* Initialize PLL */ 
    /* PLL will be the source for MCG CLKOUT so the core, system, and flash clocks are derived from it */ 
    mcg_clk_hz = pll_init(CLK0_FREQ_HZ,  /* CLKIN0 frequency */
                          LOW_POWER,     /* Set the oscillator for low power mode */
                          CLK0_TYPE,     /* Crystal or canned oscillator clock input */
                          PLL0_PRDIV,    /* PLL predivider value */
                          PLL0_VDIV,     /* PLL multiplier */
                          MCGOUT);       /* Use the output from this PLL as the MCGOUT */

    /* Check the value returned from pll_init() to make sure there wasn't an error */
    if (mcg_clk_hz < 0x100)
        while(1);

    /*
     * Use the value obtained from the pll_init function to define variables
     * for the core clock in kHz and also the peripheral clock. These
     * variables can be used by other functions that need awareness of the
     * system frequency.
     */
//    mcg_clk_khz = mcg_clk_hz / 1000;
//    core_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
//    core_clk_mhz = mcg_clk_khz/1000; 
//    periph_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);
    cpu_clk_refresh();

    /* For debugging purposes, enable the trace clock and/or FB_CLK so that
     * we'll be able to monitor clocks and know the PLL is at the frequency
     * that we expect.
     */
    trace_clk_init();
//    clkout_init();
    /* Enable pin interrupt for the abort button - PTA4 */
    /* This pin could also be used as the NMI interrupt, but since the NMI
     * is level sensitive each button press will cause multiple interrupts.
     * Using the GPIO interrupt instead means we can configure for an edge
     * sensitive interrupt instead = one interrupt per button press.
     */
    //enable_abort_button();
}

/********************************************************************/
/*!
 * \brief   flash Identify
 * \return  None
 *
 * This is primarly a reporting function that displays information
 * about the specific flash parameters and flash version ID for 
 * the current device. These parameters are obtained using a special
 * flash command call "read resource." The first four bytes returned
 * are the flash parameter revision, and the second four bytes are
 * the flash version ID.
 */
void flash_identify (void)
{
  
#if 0
    /* Get the flash parameter version */

    /* Write the flash FCCOB registers with the values for a read resource command */
    FTFE_FCCOB0 = 0x03;
    FTFE_FCCOB1 = 0x00;
    FTFE_FCCOB2 = 0x00;
    FTFE_FCCOB3 = 0x00;
    FTFE_FCCOB8 = 0x01;

    /* All required FCCOBx registers are written, so launch the command */
    FTFE_FSTAT = FTFE_FSTAT_CCIF_MASK;

    /* Wait for the command to complete */
    while(!(FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK));
    
    TRACE("Flash parameter version %d.%d.%d.%d\n",FTFE_FCCOB4,FTFE_FCCOB5,FTFE_FCCOB6,FTFE_FCCOB7);

    /* Get the flash version ID */   

    /* Write the flash FCCOB registers with the values for a read resource command */
    FTFE_FCCOB0 = 0x03;
    FTFE_FCCOB1 = 0x00;
    FTFE_FCCOB2 = 0x00;
    FTFE_FCCOB3 = 0x04;
    FTFE_FCCOB8 = 0x01;

    /* All required FCCOBx registers are written, so launch the command */
    FTFE_FSTAT = FTFE_FSTAT_CCIF_MASK;

    /* Wait for the command to complete */
    while(!(FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK));

    TRACE("Flash version ID %d.%d.%d.%d\n",FTFE_FCCOB4,FTFE_FCCOB5,FTFE_FCCOB6,FTFE_FCCOB7);  
    
#endif
}

/********************************************************************/
/*!
 * \brief   Kinetis Identify
 * \return  None
 *
 * This is primarly a reporting function that displays information
 * about the specific CPU to the default terminal including:
 * - Kinetis K12/K22/K11/K21 family
 * - package
 * - die revision
 * - P-flash size
 * - Ram size
 */
void cpu_identify (void)
{
    /* Determine the Kinetis K12/K22/K11/K21 family */
    switch((SIM_SDID & SIM_SDID_FAMID(0x7))>>SIM_SDID_FAMID_SHIFT) 
    {  
    case 0x0:
      TRACE("\nK12D-");
      break;
    case 0x1:
      TRACE("\nK22D-");
      break;
    case 0x2:
      TRACE("\nK11D-");
      break;
    case 0x3:
      TRACE("\nK21F-");
      break;
    default:
      TRACE("\nUnrecognized Kinetis family device.\n");  
      break;  	
    }

    /* Determine the package size */
    switch((SIM_SDID & SIM_SDID_PINID(0xF))>>SIM_SDID_PINID_SHIFT) 
    {  
    case 0x2:
      TRACE("32 pin       ");
      break;
    case 0x4:      
      TRACE("48 pin       ");
      break;
    case 0x5:
      TRACE("64 pin       ");
      break;
    case 0x6:
      TRACE("80 pin       ");
      break;
    case 0x7:
      TRACE("81 pin       ");
      break;
    case 0x8:
      TRACE("100 pin      ");
      break;
    case 0x9:
      TRACE("121 pin      ");
      break;
    case 0xA:
      TRACE("144 pin      ");
      break;
    case 0xC:
      TRACE("196 pin      ");
      break;
    case 0xE:
      TRACE("256 pin      ");
      break;
    default:
      TRACE("\nUnrecognized Kinetis package code.      ");  
      break;  	
    }                

    /* Determine the revision ID */

 switch((SIM_SDID & SIM_SDID_REVID(0xF))>>SIM_SDID_REVID_SHIFT) 
    { 
   
    case 0x0:
      TRACE("Silicon rev 1.0   \n");
      break;
    case 0x1:
      TRACE("Silicon rev 1.1   \n");
      break;
    case 0x2:
      TRACE("Silicon rev 1.2   \n");
      break;
    default:
      TRACE("\nThis version of software doesn't recognize the revision code.");  
      break;  
    }
    
    /* Determine the flash revision */
    flash_identify();    
    
    /* Determine the P-flash size */
  switch((SIM_FCFG1 & SIM_FCFG1_PFSIZE(0xF))>>SIM_FCFG1_PFSIZE_SHIFT)
    {
    case 0x7:
      TRACE("128 kBytes of P-flash	");
      break;
    case 0x9:
      TRACE("256 kBytes of P-flash	");
      break;
    case 0xB:
      TRACE("512 kBytes of P-flash     ");
      break;
    case 0xD:
      TRACE("1024 kBytes of P-flash    ");
      break;      
    case 0xF:
      TRACE("512 kBytes of P-flash	");
      break;
    default:
      TRACE("ERR!! Undefined P-flash size\n");  
      break;  	  		
    }

    /* Determine the RAM size */
    switch((SIM_SOPT1 & SIM_SOPT1_RAMSIZE(0xF))>>SIM_SOPT1_RAMSIZE_SHIFT)
    {
    case 0x5:
      TRACE(" 32 kBytes of RAM\n\n");
      break;
    case 0x7:
      TRACE(" 64 kBytes of RAM\n\n");
      break;
    case 0x9:
      TRACE(" 128 kBytes of RAM\n\n");
      break;
    default:
      TRACE(" ERR!! Undefined RAM size\n\n");  
      break;  		
    }
}



/********************************************************************/
void trace_clk_init(void)
{
	/* Set the trace clock to the core clock frequency */
	SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;
        	
	/* Enable the TRACE_CLKOUT pin function on PTA6 (alt7 function) */
	PORTA_PCR6 = ( PORT_PCR_MUX(0x7)
					| PORT_PCR_DSE_MASK);	// enable high drive strength to support high toggle rate
}
/********************************************************************/
void clkout_init(void)
{
#if (defined(MCU_MK21FZ50)) // MK21F has FlexBus, MK21D does not
	/* Enable the clock to the FlexBus module */
        SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;

 	/* Enable the FB_CLKOUT function on PTC3 (alt5 function) */
	PORTC_PCR3 = ( PORT_PCR_MUX(0x5)
					| PORT_PCR_DSE_MASK);	// enable high drive strength to support high toggle rate
        
#elif (defined(MCU_MK21DZ50)) // MK21D does not have FlexBus,  so output another clock instead
 	/* Enable the FB_CLKOUT function on PTC3 (alt5 function) */
        SIM_SOPT2 &= ~SIM_SOPT2_CLKOUTSEL_MASK; // clear clkoout field
        SIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL(2);    // select flash clock
	PORTC_PCR3 = ( PORT_PCR_MUX(0x5) | PORT_PCR_DSE_MASK );        
#endif
}
/********************************************************************/
void enable_abort_button(void)
{
    /* Configure the PTA4 pin for its GPIO function */
    PORTA_PCR4 = PORT_PCR_MUX(0x1); // GPIO is alt1 function for this pin
    
    /* Configure the PTA4 pin for rising edge interrupts */
    PORTA_PCR4 |= PORT_PCR_IRQC(0x9); 
    
    /* Enable the associated IRQ in the NVIC */
    enable_irq(87);      
}
/********************************************************************/
void mcg_pee_fei(void)
{
    int clk_hz=0;

    clk_hz = pee_pbe(8*1000*1000);
    if (clk_hz < 0x100)
        while(1);
    clk_hz = pbe_fbe(8*1000*1000);
    if (clk_hz < 0x100)
        while(1);
    clk_hz = fbe_fei(32*1000);
    if (clk_hz < 0x100)
        while(1);
}

