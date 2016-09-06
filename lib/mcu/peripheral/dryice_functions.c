/*
 * File:		dryice_functinos.c
 * Purpose:		Functions used for DryIce module.
 *
 */

#include "common.h"
#include "drv_inc.h"
#include "dryice_functions.h"


//#define TAMPER_PIN_MASK 0x070000u


/********************************************************************/
////////////////////////////////////////////////////////////////////////////////
// Coverts pin number to corresponding register bit location
int power(int n)
{
    int i, p;
    p = 1;
    for (i= 1; i <= n; ++i)
        p = p * 2;
    return p;
}

////////////////////////////////////////////////////////////////////////////////
void dryice_prescaler_config(int desired_prescaler_value)
{
#ifdef CFG_DBG_HW_DRYICE 
    int current_prescaler_value;
#endif


    DRY_TRACE("\n-------------------------------------------\n");
    DRY_TRACE("  DryIce Prescaler Config\n");
    DRY_TRACE("-------------------------------------------\n");
    DRY_TRACE("\n");

    if ((DRY_RAC & DRY_RAC_CRR_MASK)>>DRY_RAC_CRR_SHIFT == 0)
    {
        DRY_TRACE("Cannot read the DryIce prescaler value. The DRY_CR register's read access is blocked. Remove the read access block and try again.\n");
        return;
    } 

    if (((DRY_LR & DRY_LR_CRL_MASK)>>DRY_LR_CRL_SHIFT == 0) || ((DRY_WAC & DRY_WAC_CRW_MASK)>>DRY_WAC_CRW_SHIFT == 0))
    {
        DRY_TRACE("Cannot write the DryIce prescaler value. The DRY_CR register is locked or its write access is blocked. Remove the lock or its write access block and try again.\n");
        DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);
        DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);
        DRY_TRACE("\n");
        return;    
    }    

    //Cannot write prescaler if DryIce clocks are enabled
    if ((DRY_CR & DRY_CR_DEN_MASK) != 0x0)
    {
        DRY_TRACE("DryIce clock and prescaler are enabled. Please disable them first before specifying a new prescaler value.\n\n");
        return;
    }

    if ((desired_prescaler_value < 0x0) || (desired_prescaler_value > 0x7FFF))
    {
        DRY_TRACE("Invalid prescaler value:\n");
        DRY_TRACE("Valid 'ps' syntax:\n");
        DRY_TRACE("DRYICE_CMD> ps  <0x0..0x7FFF = prescaler value>\n\n");
        DRY_TRACE("\n");    
        return;    
    }

    DRY_CR &= ~DRY_CR_DPR(0x7FFF);
    DRY_CR |= DRY_CR_DPR(desired_prescaler_value);

#ifdef CFG_DBG_HW_DRYICE 
    current_prescaler_value = (DRY_CR & DRY_CR_DPR_MASK);
    DRY_TRACE("\nNew prescaler value is \t%i\n",current_prescaler_value>>DRY_CR_DPR_SHIFT);
    DRY_TRACE("\n");  
#endif
    return;
}

////////////////////////////////////////////////////////////////////////////////
//void dryice_tamper_pin_polarity_config(int pin_number, int desired_pin_polarity)
//{
//    int32 corresponding_tamper_pin;  
//
//    DRY_TRACE("\n-------------------------------------------\n");
//    DRY_TRACE("  DryIce Pin Polarity Config Utility\n");
//    DRY_TRACE("-------------------------------------------\n");
//    DRY_TRACE("\n");
//    if ((DRY_RAC & DRY_RAC_PPR_MASK)>>DRY_RAC_PPR_SHIFT == 0)
//    {
//        DRY_TRACE("Cannot read the pin polarity setting. The DRY_PPR register's read access is blocked. Remove the read access block and try again.\n");
//        return;
//    }         
//
//    if (((DRY_LR & DRY_LR_PPL_MASK)>>DRY_LR_PPL_SHIFT == 0) || ((DRY_WAC & DRY_WAC_PPW_MASK)>>DRY_WAC_PPW_SHIFT == 0))
//    {
//        DRY_TRACE("Cannot write the pin polarity setting. The DRY_PPR register is locked or its write access is blocked. Remove the lock or its write access block and try again.\n");
//        return;    
//    }   
//
//    if (((DRY_LR & DRY_LR_PPL_MASK) == 0x0) || ((DRY_WAC & DRY_WAC_PPW_MASK) == 0x0) || ((DRY_RAC & DRY_RAC_PPR_MASK) == 0x0))
//    {
//        DRY_TRACE("Cannot write DRY_PPR because it is either locked or its read or write access is blocked. Remove the lock or the read/write access block and try again.\n");
//        return;
//    }
//
//
//    //Determine value to write into PPR[TPP]
//    corresponding_tamper_pin = power(pin_number);
//
//    if ((pin_number < 0) || (pin_number > 2))
//    {
//        DRY_TRACE("Invalid tamper pin source:\n");    
//        return;
//    }
//
//    if ((desired_pin_polarity < 0) || (desired_pin_polarity > 1))
//    {
//        DRY_TRACE("Invalid pin polarity:\n"); 
//        return;
//    }
//
//
//    if (desired_pin_polarity == 0) {
//        DRY_PPR &= ~DRY_PPR_TPP(corresponding_tamper_pin);
//        DRY_TRACE("Tamper Pin %i expected value is not inverted.\n",pin_number);
//    }
//    else
//    {
//        DRY_PPR |= DRY_PPR_TPP(corresponding_tamper_pin);
//        DRY_TRACE("Tamper Pin %i expected value is inverted.\n",pin_number);            
//    }
//
//    return;
//}// end dryice_tamper_pin_polarity_config
//////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////
//void dryice_tamper_pin_direction_config(int pin_number, int desired_pin_direction)
//{
//    int32 corresponding_tamper_pin;
//
//
//    DRY_TRACE("\n-------------------------------------------\n");
//    DRY_TRACE("  DryIce Pin Direction Config Utility\n");
//    DRY_TRACE("-------------------------------------------\n");
//    DRY_TRACE("\n");
//    if ((DRY_RAC & DRY_RAC_PDR_MASK)>>DRY_RAC_PDR_SHIFT == 0)
//    {
//        DRY_TRACE("Cannot read the pin direction. The DRY_PDR register's read access is blocked. Remove the read access block and try again.\n");
//        return;
//    }          
//
//    if (((DRY_LR & DRY_LR_PDL_MASK)>>DRY_LR_PDL_SHIFT == 0) || ((DRY_WAC & DRY_WAC_PDW_MASK)>>DRY_WAC_PDW_SHIFT == 0))
//    {
//        DRY_TRACE("Cannot write the pin direction. The DRY_PDR register is locked or its write access is blocked. Remove the lock or its write access block and try again.\n");
//        return;    
//    }  
//
//    //Determine value to write into register based on pin_number
//    corresponding_tamper_pin = power(pin_number);  
//
//    if ((pin_number < 0) || (pin_number > 2))
//    {
//        DRY_TRACE("Invalid tamper pin source:\n");
//        return;
//    }
//
//    if ((desired_pin_direction < 0) || (desired_pin_direction > 1))
//    {
//        DRY_TRACE("Invalid pin direction:\n");
//        return;
//    }
//
//    if (desired_pin_direction == 0) {
//        DRY_PDR &= ~DRY_PDR_TPD(corresponding_tamper_pin);
//        DRY_TRACE("Tamper Pin %i pin direction set to input.\n",pin_number);
//    }
//    else
//    {
//        DRY_PDR |= DRY_PDR_TPD(corresponding_tamper_pin);
//        DRY_TRACE("Tamper Pin %i pin direction set to output and drives inverse of expected value.\n",pin_number);
//    }
//    return;
//}// end dryice_tamper_pin_direction_config
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//void dryice_tamper_pin_glitch_filter_width_config(int tamper_pin, int desired_glitch_filter_width)
//{
//#ifdef CFG_DBG_HW_DRYICE 
//    unsigned char current_glitch_filter_width;
//#endif
//
//    DRY_TRACE("\n-------------------------------------------\n");
//    DRY_TRACE("  DryIce Pin Glitch Width Config Utility\n");
//    DRY_TRACE("-------------------------------------------\n");
//    DRY_TRACE("\n");
//    if ((DRY_RAC & DRY_RAC_GFR_MASK) != (DRY_RAC_GFR_MASK & TAMPER_PIN_MASK))
//    {
//        DRY_TRACE("WARNING: Some PGFRx registers are blocked from reading.\n");
//        DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);
//        DRY_TRACE("\n");
//    }    
//
//
//    if ((tamper_pin < 0) || (tamper_pin > 2))
//    {
//        DRY_TRACE("Invalid tamper pin source:\n");
//        return;
//    }
//
//    if ((desired_glitch_filter_width < 0) || (desired_glitch_filter_width > 63))
//    {
//        DRY_TRACE("Invalid glitch filter width:\n");
//        return;
//    }
//
//    switch (tamper_pin)
//    {
//    case 0:
//        DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0); 
//        if ((DRY_PGFR0 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter width when glitch filter is enabled. Please disable it first.\n");
//            DRY_TRACE("\n");
//            return;
//        }
//#ifdef CFG_DBG_HW_DRYICE 
//        current_glitch_filter_width = (DRY_PGFR0 & DRY_PGFR_GFW_MASK);
//        DRY_TRACE("Previous Tamper Pin 0 glitch filter width set to (%i + 1) * 2.\n",current_glitch_filter_width);    
//#endif
//        DRY_PGFR0 &= ~DRY_PGFR_GFW(0x3F);
//        DRY_PGFR0 |= DRY_PGFR_GFW(desired_glitch_filter_width);
//        DRY_TRACE("Tamper Pin 0 glitch filter width set to (%i + 1) * 2.\n",desired_glitch_filter_width);
//        DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        break;
//    case 1:
//        DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);        
//        if ((DRY_PGFR1 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter width when glitch filter is enabled. Please disable it first.\n");
//            DRY_TRACE("\n");
//            return;
//        }
//#ifdef CFG_DBG_HW_DRYICE 
//        current_glitch_filter_width = (DRY_PGFR1 & DRY_PGFR_GFW_MASK);
//        DRY_TRACE("Previous Tamper Pin 1 glitch filter width set to (%i + 1) * 2.\n",current_glitch_filter_width);    
//#endif
//        DRY_PGFR1 &= ~DRY_PGFR_GFW(0x3F);
//        DRY_PGFR1 |= DRY_PGFR_GFW(desired_glitch_filter_width);
//        DRY_TRACE("Tamper Pin 1 glitch filter width set to (%i + 1) * 2.\n",desired_glitch_filter_width);
//        DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);    
//        break;
//    case 2:
//        DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);        
//        if ((DRY_PGFR2 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter width when glitch filter is enabled. Please disable it first.\n");
//            DRY_TRACE("\n");
//            return;
//        }
//#ifdef CFG_DBG_HW_DRYICE 
//        current_glitch_filter_width = (DRY_PGFR2 & DRY_PGFR_GFW_MASK);
//        DRY_TRACE("Previous Tamper Pin 2 glitch filter width set to (%i + 1) * 2.\n",current_glitch_filter_width);    
//#endif
//        DRY_PGFR2 &= ~DRY_PGFR_GFW(0x3F);
//        DRY_PGFR2 |= DRY_PGFR_GFW(desired_glitch_filter_width);
//        DRY_TRACE("Tamper Pin 2 glitch filter width set to (%i + 1) * 2.\n",desired_glitch_filter_width);
//        DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);    
//        break;
//    }
//    return;
//}// end dryice_tamper_pin_glitch_filter_width_config
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//void dryice_tamper_pin_glitch_filter_prescaler_config(int tamper_pin, int desired_glitch_filter_prescaler)
//{
//    int32 corresponding_tamper_pin;
//
//    DRY_TRACE("\n-------------------------------------------\n");
//    DRY_TRACE("  DryIce Pin Glitch Filter Prescaler Utility\n");
//    DRY_TRACE("-------------------------------------------\n");
//    DRY_TRACE("\n");
//    if ((DRY_RAC & DRY_RAC_GFR_MASK) != (DRY_RAC_GFR_MASK & TAMPER_PIN_MASK))
//    {
//        DRY_TRACE("WARNING: Some PGFRx registers are blocked from reading.\n");
//        DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);
//        DRY_TRACE("\n");
//    }  
//
//    corresponding_tamper_pin = power(tamper_pin);
//
//    if ((tamper_pin < 0) || (tamper_pin > 2))
//    {
//        DRY_TRACE("Invalid tamper pin source:\n");
//        return;
//    }
//
//    if (((DRY_LR & DRY_LR_GFL(corresponding_tamper_pin))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0) || ((DRY_WAC & DRY_WAC_GFW(corresponding_tamper_pin))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0) || ((DRY_RAC & DRY_RAC_GFR(corresponding_tamper_pin))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//    {
//        DRY_TRACE("Cannot write DRY_PGFR%i because it is either locked or its read or write access is blocked. Remove the lock or the read/write access block and try again.\n",tamper_pin);
//        return;
//    }      
//
//    if ((desired_glitch_filter_prescaler != 0) && (desired_glitch_filter_prescaler != 1))
//    {
//        DRY_TRACE("Invalid glitch filter source value:\n");
//        return;
//    }
//
//    switch (tamper_pin)
//    {
//    case 0:
//        if ((DRY_PGFR0 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter prescaler setting when glitch filter is enabled. Please disable it first.\n");
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (desired_glitch_filter_prescaler == 0) {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 &= ~DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 0 glitch filter sourced by 512 Hz clock.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 |= DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 0 glitch filter sourced by 32.768 kHz clock.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        break;
//    case 1:
//        if ((DRY_PGFR1 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter prescaler setting when glitch filter is enabled. Please disable it first.\n");
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (desired_glitch_filter_prescaler == 0) {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 &= ~DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 1 glitch filter sourced by 512 Hz clock.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 |= DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 1 glitch filter sourced by 32.768 kHz clock.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        break;
//    case 2:
//        if ((DRY_PGFR2 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter prescaler setting when glitch filter is enabled. Please disable it first.\n");
//            DRY_TRACE("\n");
//            return;
//        }
//        if (desired_glitch_filter_prescaler == 0) {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 &= ~DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 2 glitch filter sourced by 512 Hz clock.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 |= DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 2 glitch filter sourced by 32.768 kHz clock.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        break;
//    }   
//    return;
//}// end dryice_tamper_pin_glitch_filter_prescaler_config
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//void dryice_tamper_pin_glitch_filter_config(int tamper_pin, int desired_glitch_filter_setting)
//{
//    int32 corresponding_tamper_pin;
//
//    DRY_TRACE("\n-------------------------------------------\n");
//    DRY_TRACE("  DryIce Pin Glitch Filter Enable/Disable Utility\n");
//    DRY_TRACE("-------------------------------------------\n");
//    DRY_TRACE("\n");
//    if ((DRY_RAC & DRY_RAC_GFR_MASK) != (DRY_RAC_GFR_MASK & TAMPER_PIN_MASK))
//    {
//        DRY_TRACE("WARNING: Some PGFRx registers are blocked from reading.\n");
//        DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);
//        DRY_TRACE("\n");
//    }  
//
//    corresponding_tamper_pin = power(tamper_pin);
//
//    if ((tamper_pin < 0) || (tamper_pin > 2))
//    {
//        DRY_TRACE("Invalid tamper pin source:\n");
//        return;
//    }
//
//    if (((DRY_LR & DRY_LR_GFL(corresponding_tamper_pin))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0) || ((DRY_WAC & DRY_WAC_GFW(corresponding_tamper_pin))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0) || ((DRY_RAC & DRY_RAC_GFR(corresponding_tamper_pin))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//    {
//        DRY_TRACE("Cannot write DRY_PGFR%i because it is either locked or its read or write access is blocked. Remove the lock or the read/write access block and try again.\n",tamper_pin);
//        DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);
//        DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);
//        DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
//        DRY_TRACE("\n");
//        return;
//    }  
//
//
//    if ((desired_glitch_filter_setting != 0) && (desired_glitch_filter_setting != 1))
//    {
//        DRY_TRACE("Invalid glitch filter enable/disable switch:\n");
//        return;
//    }
//
//    //DryIce prescaler and clock must be enabled first before glitch filter can be enabled  
//    if (((DRY_CR & DRY_CR_DEN_MASK) != DRY_CR_DEN_MASK) && (desired_glitch_filter_setting == 1))
//    {
//        DRY_TRACE("DryIce prescaler and clock is disabled. Please enable it first before enabling the glitch filter.\n");
//        DRY_TRACE("\n");
//        return;
//    }
//
//    switch (tamper_pin)
//    {
//    case 0:
//        if (desired_glitch_filter_setting == 0) {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 &= ~DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 0 glitch filter disabled.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 |= DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 0 glitch filter enabled.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        break;
//    case 1:
//        if (desired_glitch_filter_setting == 0) {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 &= ~DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 1 glitch filter disabled.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 |= DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 1 glitch filter enabled.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        break;
//    case 2:
//        if (desired_glitch_filter_setting == 0) {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 &= ~DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 2 glitch filter disabled.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 |= DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 2 glitch filter enabled.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        break;
//    }
//    return;
//}// end dryice_tamper_pin_glitch_filter_config
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//void dryice_tamper_pin_expect_config(int tamper_pin, int expect_value)
//{
//    int32 corresponding_tamper_pin;
//
//    DRY_TRACE("\n-------------------------------------------\n");
//    DRY_TRACE("  DryIce Pin Expect Value Config Utility\n");
//    DRY_TRACE("-------------------------------------------\n");
//    DRY_TRACE("\n");
//    if ((DRY_RAC & DRY_RAC_GFR_MASK) != (DRY_RAC_GFR_MASK & TAMPER_PIN_MASK))
//    {
//        DRY_TRACE("WARNING: Some PGFRx registers are blocked from reading.\n");
//        DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);
//        DRY_TRACE("\n");
//    }  
//
//    corresponding_tamper_pin = power(tamper_pin);
//
//    if ((tamper_pin < 0) || (tamper_pin > 2))
//    {
//        DRY_TRACE("Invalid tamper pin source:\n");
//        return;
//    }
//
//    if (((DRY_LR & DRY_LR_GFL(corresponding_tamper_pin))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0) || ((DRY_WAC & DRY_WAC_GFW(corresponding_tamper_pin))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0) || ((DRY_RAC & DRY_RAC_GFR(corresponding_tamper_pin))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//    {
//        DRY_TRACE("Cannot write DRY_PGFRx because it is either locked or its read or write access is blocked. Remove the lock or the read/write access block and try again.\n");
//        DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);
//        DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);
//        DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
//        DRY_TRACE("\n");
//        return;
//    }  
//
//    if ((expect_value < 0) || (expect_value > 3))
//    {
//        DRY_TRACE("Invalid expect value:\n");
//        return;
//    }
//
//    switch (tamper_pin)
//    {
//    case 0:
//        if (expect_value == 0) {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 &= ~DRY_PGFR_TPEX(3);
//            DRY_TRACE("Tamper Pin 0 expected value is logic 0.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        else if (expect_value == 1)
//        {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 &= 0xFFFCFFFF;
//            DRY_PGFR0 |= DRY_PGFR_TPEX(1);
//            DRY_TRACE("Tamper Pin 0 expected value is active tamper 0 output.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        else if (expect_value == 2)
//        {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 &= 0xFFFCFFFF;
//            DRY_PGFR0 |= DRY_PGFR_TPEX(2);      
//            DRY_TRACE("Tamper Pin 0 expected value is active tamper 1 output.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        else if (expect_value == 3)
//        {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 &= 0xFFFCFFFF;
//            DRY_PGFR0 |= DRY_PGFR_TPEX(3);      
//            DRY_TRACE("Tamper Pin 0 expected value is active tamper 0 output XOR'ed with active tamper 1 output.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }    
//        break;
//    case 1:
//        if (expect_value == 0) {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 &= ~DRY_PGFR_TPEX(3);
//            DRY_TRACE("Tamper Pin 1 expected value is logic 0.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        else if (expect_value == 1)
//        {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 &= 0xFFFCFFFF;
//            DRY_PGFR1 |= DRY_PGFR_TPEX(1);
//            DRY_TRACE("Tamper Pin 1 expected value is active tamper 0 output.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        else if (expect_value == 2)
//        {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 &= 0xFFFCFFFF;
//            DRY_PGFR1 |= DRY_PGFR_TPEX(2);      
//            DRY_TRACE("Tamper Pin 1 expected value is active tamper 1 output.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        else if (expect_value == 3)
//        {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 &= 0xFFFCFFFF;
//            DRY_PGFR1 |= DRY_PGFR_TPEX(3);      
//            DRY_TRACE("Tamper Pin 1 expected value is active tamper 0 output XOR'ed with active tamper 1 output.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }    
//        break;
//    case 2:
//        if (expect_value == 0) {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 &= ~DRY_PGFR_TPEX(3);
//            DRY_TRACE("Tamper Pin 2 expected value is logic 0.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        else if (expect_value == 1)
//        {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 &= 0xFFFCFFFF;
//            DRY_PGFR2 |= DRY_PGFR_TPEX(1);
//            DRY_TRACE("Tamper Pin 2 expected value is active tamper 0 output.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        else if (expect_value == 2)
//        {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 &= 0xFFFCFFFF;
//            DRY_PGFR2 |= DRY_PGFR_TPEX(2);      
//            DRY_TRACE("Tamper Pin 2 expected value is active tamper 1 output.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        else if (expect_value == 3)
//        {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 &= 0xFFFCFFFF;
//            DRY_PGFR2 |= DRY_PGFR_TPEX(3);      
//            DRY_TRACE("Tamper Pin 2 expected value is active tamper 0 output XOR'ed with active tamper 1 output.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }    
//        break;
//    }
//    return;
//}// end dryice_tamper_pin_expect_config
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//void dryice_tamper_pin_pull_config(int tamper_pin, int desired_pin_pull_setting)
//{
//
//    int32 corresponding_tamper_pin;
//
//    DRY_TRACE("\n-------------------------------------------\n");
//    DRY_TRACE("  DryIce Pin Pull Resistor Enable/Disable Utility\n");
//    DRY_TRACE("-------------------------------------------\n");
//    DRY_TRACE("\n");
//
//    corresponding_tamper_pin = power(tamper_pin);
//
//
//    if ((tamper_pin < 0) || (tamper_pin > 2))
//    {
//        DRY_TRACE("Invalid tamper pin source:\n");
//        return;
//    }
//
//    if (((DRY_LR & DRY_LR_GFL(corresponding_tamper_pin))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0) || ((DRY_WAC & DRY_WAC_GFW(corresponding_tamper_pin))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0) || ((DRY_RAC & DRY_RAC_GFR(corresponding_tamper_pin))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//    {
//        DRY_TRACE("Cannot write DRY_PGFR%i because it is either locked or its read or write access is blocked. Remove the lock or the read/write access block and try again.\n",tamper_pin);
//        DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);
//        DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);
//        DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
//        DRY_TRACE("\n");
//        return;
//    }   
//
//    if ((desired_pin_pull_setting != 0) && (desired_pin_pull_setting != 1))
//    {
//        DRY_TRACE("Invalid pull enable/disable switch:\n");
//        return;
//    }
//
//    switch (tamper_pin)
//    {
//    case 0:
//        if (desired_pin_pull_setting == 0) {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 &= ~DRY_PGFR_TPE_MASK;
//            DRY_TRACE("Tamper Pin 0 pull resistor disabled.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 |= DRY_PGFR_TPE_MASK;
//            DRY_TRACE("Tamper Pin 0 pull resistor enabled.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        break;
//    case 1:
//        if (desired_pin_pull_setting == 0) {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 &= ~DRY_PGFR_TPE_MASK;
//            DRY_TRACE("Tamper Pin 1 pull resistor disabled.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 |= DRY_PGFR_TPE_MASK;
//            DRY_TRACE("Tamper Pin 1 pull resistor enabled.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        break;
//    case 2:
//        if (desired_pin_pull_setting == 0) {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 &= ~DRY_PGFR_TPE_MASK;
//            DRY_TRACE("Tamper Pin 2 pull resistor disabled.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 |= DRY_PGFR_TPE_MASK;
//            DRY_TRACE("Tamper Pin 2 pull resistor enabled.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);                        
//        }
//        break;
//    }
//    return;
//}// end dryice_tamper_pin_pull_config
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void dryice_int_tamper_interrupt_config(int internal_tamper_source, int enable_disable)
{

    DRY_TRACE("\n-------------------------------------------\n");
    DRY_TRACE("  DryIce Internal Tamper Flag Interrupt Enable/Disable Utility\n");
    DRY_TRACE("-------------------------------------------\n");
    DRY_TRACE("\n");

    if (((DRY_LR & DRY_LR_IEL_MASK) == 0x0) || ((DRY_WAC & DRY_WAC_IEW_MASK) == 0x0) || ((DRY_RAC & DRY_RAC_IER_MASK) == 0x0))
    {
        DRY_TRACE("Cannot read or write the interrupt enable settings. DRY_IER is either locked or its read or write access is blocked. Remove the lock or the read/write access block and try again.\n");
        return;
    }


    if ((internal_tamper_source < 0) || (internal_tamper_source == 1) || (internal_tamper_source > 9))
    {
        DRY_TRACE("Invalid internal tamper interrupt source:\n");
        return;
    }

    if ((enable_disable != 0) && (enable_disable != 1))
    {
        DRY_TRACE("Invalid tamper interrupt enable/disable switch:\n");
        return;
    }

    switch (internal_tamper_source)
    {
    case 0:
        if (enable_disable == 0) {
            DRY_IER &= ~DRY_IER_DTIE_MASK;
            DRY_TRACE("DryIce Interrupt will not be generated if DRY_SR[DTF] is set.\n");
        }
        else
        {
            DRY_IER |= DRY_IER_DTIE_MASK;
            DRY_TRACE("DryIce Interrupt will be generated if DRY_SR[DTF] is set.\n");
        }
        break;
    case 2:
        if (enable_disable == 0) {
            DRY_IER &= ~DRY_IER_TOIE_MASK;
            DRY_TRACE("DryIce Interrupt will not be generated if DRY_SR[TOF] is set.\n");
        }
        else
        {
            DRY_IER |= DRY_IER_TOIE_MASK;
            DRY_TRACE("DryIce Interrupt will be generated if DRY_SR[TOF] is set.\n");
        }
        break;
    case 3:
        if (enable_disable == 0) {
            DRY_IER &= ~DRY_IER_MOIE_MASK;
            DRY_TRACE("DryIce Interrupt will not be generated if DRY_SR[MOF] is set.\n");
        }
        else
        {
            DRY_IER |= DRY_IER_MOIE_MASK;
            DRY_TRACE("DryIce Interrupt will be generated if DRY_SR[MOF] is set.\n");
        }
        break;
    case 4:
        if (enable_disable == 0) {
            DRY_IER &= ~DRY_IER_VTIE_MASK;
            DRY_TRACE("DryIce Interrupt will not be generated if DRY_SR[VTF] is set.\n");
        }
        else
        {
            DRY_IER |= DRY_IER_VTIE_MASK;
            DRY_TRACE("DryIce Interrupt will be generated if DRY_SR[VTF] is set.\n");
        }
        break;
    case 5:
        if (enable_disable == 0) {
            DRY_IER &= ~DRY_IER_CTIE_MASK;
            DRY_TRACE("DryIce Interrupt will not be generated if DRY_SR[CTF] is set.\n");
        }
        else
        {
            DRY_IER |= DRY_IER_CTIE_MASK;
            DRY_TRACE("DryIce Interrupt will be generated if DRY_SR[CTF] is set.\n");
        }
        break;
    case 6:
        if (enable_disable == 0) {
            DRY_IER &= ~DRY_IER_TTIE_MASK;
            DRY_TRACE("DryIce Interrupt will not be generated if DRY_SR[TTF] is set.\n");
        }
        else
        {
            DRY_IER |= DRY_IER_TTIE_MASK;
            DRY_TRACE("DryIce Interrupt will be generated if DRY_SR[TTF] is set.\n");
        }
        break;
    case 7:
        if (enable_disable == 0) {
            DRY_IER &= ~DRY_IER_STIE_MASK;
            DRY_TRACE("DryIce Interrupt will not be generated if DRY_SR[STF] is set.\n");
        }
        else
        {
            DRY_IER |= DRY_IER_STIE_MASK;
            DRY_TRACE("DryIce Interrupt will be generated if DRY_SR[STF] is set.\n");
        }
        break;
    case 8:
        if (enable_disable == 0) {
            DRY_IER &= ~DRY_IER_FSIE_MASK;
            DRY_TRACE("DryIce Interrupt will not be generated if DRY_SR[FSF] is set.\n");
        }
        else
        {
            DRY_IER |= DRY_IER_FSIE_MASK;
            DRY_TRACE("DryIce Interrupt will be generated if DRY_SR[FSF] is set.\n");
        }
        break;
    case 9:
        if (enable_disable == 0) {
            DRY_IER &= ~DRY_IER_TMIE_MASK;
            DRY_TRACE("DryIce Interrupt will not be generated if DRY_SR[TMF] is set.\n");
        }
        else
        {
            DRY_IER |= DRY_IER_TMIE_MASK;
            DRY_TRACE("DryIce Interrupt will be generated if DRY_SR[TMF] is set.\n");
        }
    }
    return;    
}//end dryice_int_tamper_interrupt_config
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void dryice_pin_tamper_interrupt_config(int pin_number, int enable_disable)
{
    int32 corresponding_tamper_pin;

    DRY_TRACE("\n-------------------------------------------\n");
    DRY_TRACE("  DryIce Pin Tamper Flag Interrupt Enable/Disable Utility\n");
    DRY_TRACE("-------------------------------------------\n");
    DRY_TRACE("\n");
    if (((DRY_LR & DRY_LR_IEL_MASK) == 0x0) || ((DRY_WAC & DRY_WAC_IEW_MASK) == 0x0) || ((DRY_RAC & DRY_RAC_IER_MASK) == 0x0))
    {
        DRY_TRACE("Cannot read or write interrupt enable settings. DRY_IER is either locked or its read or write access is blocked. Remove the lock or the read/write access block and try again.\n");
        return;
    }    


    corresponding_tamper_pin = power(pin_number);    

    if ((pin_number < 0) || (pin_number > 2))
    {
        DRY_TRACE("Invalid pin tamper source:\n");
        return;
    }

    if ((enable_disable != 0) && (enable_disable != 1))
    {
        DRY_TRACE("Invalid pin tamper interrupt enable/disable switch:\n");
        return;
    }


    if (enable_disable == 0) {
        DRY_IER &= ~DRY_IER_TPIE(corresponding_tamper_pin);
        DRY_TRACE("If Tamper Pin %i Flag is set, an interrupt will not be generated.\n",pin_number);
    }
    else
    {
        DRY_IER |= DRY_IER_TPIE(corresponding_tamper_pin);
        DRY_TRACE("If Tamper Pin %i Flag is set, an interrupt will be generated.\n",pin_number);
    }
    return;
}// end dryice_pin_tamper_interrupt_config
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void dryice_int_tamper_config(int internal_tamper_source, int enable_disable)
{

    DRY_TRACE("\n-------------------------------------------\n");
    DRY_TRACE("  DryIce Internal Tamper Flag Setting DTF Enable/Disable Utility\n");
    DRY_TRACE("-------------------------------------------\n");
    DRY_TRACE("\n");
    if (((DRY_LR & DRY_LR_TEL_MASK) == 0x0) || ((DRY_WAC & DRY_WAC_TEW_MASK) == 0x0) || ((DRY_RAC & DRY_RAC_TER_MASK) == 0x0))
    {
        DRY_TRACE("Cannot read or write tamper enable settings. DRY_TER either locked or its read or write access is blocked. Remove the lock or the read/write access block and try again.\n");
        return;
    }    



    if ((internal_tamper_source < 2) || (internal_tamper_source > 9))
    {
        DRY_TRACE("Invalid internal tamper source:\n");
        return;
    }

    if ((enable_disable != 0) && (enable_disable != 1))
    {
        DRY_TRACE("Invalid tamper enable/disable switch:\n");
        return;
    }

    switch (internal_tamper_source)
    {
    case 2:
        if (enable_disable == 0) {
            DRY_TER &= ~DRY_TER_TOE_MASK;
            DRY_TRACE("Time Overflow Flag (DRY_SR[TOF]) assertion will not set DRY_SR[DTF].\n");
        }
        else
        {
            DRY_TER |= DRY_TER_TOE_MASK;
            DRY_TRACE("Time Overflow Flag (DRY_SR[TOF]) assertion will set DRY_SR[DTF].\n");
        }
        break;
    case 3:
        if (enable_disable == 0) {
            DRY_TER &= ~DRY_TER_MOE_MASK;
            DRY_TRACE("Monotonic Overflow Flag (DRY_SR[MOF]) assertion will not set DRY_SR[DTF].\n");
        }
        else
        {
            DRY_TER |= DRY_TER_MOE_MASK;
            DRY_TRACE("Monotonic Overflow Flag (DRY_SR[MOF]) assertion will set DRY_SR[DTF].\n");
        }
        break;
    case 4:
        if (enable_disable == 0) {
            DRY_TER &= ~DRY_TER_VTE_MASK;
            DRY_TRACE("Disabling clock tamper circuitry.\n");            
            DRY_TRACE("Voltage Tamper Flag (DRY_SR[VTF]) assertion will not set DRY_SR[DTF].\n");
        }
        else
        {
            DRY_TER |= DRY_TER_VTE_MASK;
            DRY_TRACE("Enabling voltage tamper circuitry.\n");            
            DRY_TRACE("Voltage Tamper Flag (DRY_SR[VTF]) assertion will set DRY_SR[DTF].\n");
            DRY_TRACE("Please also enable clock tamper detect.\n");      
        }
        break;
    case 5:
        if (enable_disable == 0) {
            DRY_TER &= ~DRY_TER_CTE_MASK;      
            DRY_TRACE("Disabling clock tamper circuitry.\n");            
            DRY_TRACE("Clock Tamper Flag (DRY_SR[CTF]) assertion will not set DRY_SR[DTF].\n");
            DRY_TRACE("Please also disable voltage and temperature tamper detect.\n");
        }
        else
        {
            DRY_TER |= DRY_TER_CTE_MASK; 
            DRY_TRACE("Enabling clock tamper circuitry.\n");            
            DRY_TRACE("Clock Tamper Flag (DRY_SR[CTF]) assertion will set DRY_SR[DTF].\n");
        }
        break;
    case 6:
        if (enable_disable == 0) {
            DRY_TER &= ~DRY_TER_TTE_MASK;
            DRY_TRACE("Disabling temperature tamper circuitry.\n");
            DRY_TRACE("Temperature Tamper Flag (DRY_SR[TTF]) assertion will not set DRY_SR[DTF].\n");      
        }
        else
        {
            DRY_TER |= DRY_TER_TTE_MASK;      
            DRY_TRACE("Enabling temperature tamper circuitry.\n");      
            DRY_TRACE("Temperature Tamper Flag (DRY_SR[TTF]) assertion will set DRY_SR[DTF].\n");
            DRY_TRACE("Please also enable clock tamper detect.\n");      
        }
        break;
    case 7:
        if (enable_disable == 0) {
            DRY_TER &= ~DRY_TER_STE_MASK;
            DRY_TRACE("Security Module Tamper Flag (DRY_SR[STF]) assertion will not set DRY_SR[DTF].\n");
        }
        else
        {
            DRY_TER |= DRY_TER_STE_MASK;
            DRY_TRACE("Security Module Tamper Flag (DRY_SR[STF] assertion will set DRY_SR[DTF].\n");
        }
        break;
    case 8:
        if (enable_disable == 0) {
            DRY_TER &= ~DRY_TER_FSE_MASK;
            DRY_TRACE("Flash Security Flag (DRY_SR[FSF]) assertion will not set DRY_SR[DTF].\n");
        }
        else
        {
            DRY_TER |= DRY_TER_FSE_MASK;
            DRY_TRACE("Flash Security Flag (DRY_SR[FSF]) assertion will set DRY_SR[DTF].\n");
        }
        break;
    case 9:
        if (enable_disable == 0) {
            DRY_TER &= ~DRY_TER_TME_MASK;
            DRY_TRACE("Test Mode Flag (DRY_SR[TMF]) assertion will not set DRY_SR[DTF].\n");
        }
        else
        {
            DRY_TER |= DRY_TER_TME_MASK;
            DRY_TRACE("Test Mode Flag (DRY_SR[TMF]) assertion will set DRY_SR[DTF].\n");
        }
    }
    return;
}//end dryice_int_tamper_config
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void dryice_pin_tamper_config(int pin_number, int enable_disable)
{
    int32 corresponding_tamper_pin;

    DRY_TRACE("\n-------------------------------------------\n");
    DRY_TRACE("  DryIce Pin Tamper Flag Setting DTF Enable/Disable Utility\n");
    DRY_TRACE("-------------------------------------------\n");
    DRY_TRACE("\n");
    if (((DRY_LR & DRY_LR_TEL_MASK) == 0x0) || ((DRY_WAC & DRY_WAC_TEW_MASK) == 0x0) || ((DRY_RAC & DRY_RAC_TER_MASK) == 0x0))
    {
        DRY_TRACE("Cannot read or write tamper enable settings. DRY_TER is either locked or its read or write access is blocked. Remove the lock or the read/write access block and try again.\n");
        return;
    }    

    //Determine value to write to TER[TPE]
    corresponding_tamper_pin = power(pin_number);    

    if ((pin_number < 0) || (pin_number > 2))
    {
        DRY_TRACE("Invalid pin tamper source:\n");
        return;
    }

    if ((enable_disable != 0) && (enable_disable != 1))
    {
        DRY_TRACE("Invalid tamper enable/disable switch:\n");
        return;
    }

    if (enable_disable == 0) {
        DRY_TER &= ~DRY_TER_TPE(corresponding_tamper_pin);
        DRY_TRACE("Tamper Event on Pin %i will not set either SR[TPF%i] or SR[DTF].\n",pin_number,pin_number);
    }
    else
    {
        DRY_TER |= DRY_TER_TPE(corresponding_tamper_pin);
        DRY_TRACE("Tamper Event on Pin %i will set both SR[TPF%i] and SR[DTF].\n",pin_number,pin_number);
    }
    return;
}// end dryice_pin_tamper_config
////////////////////////////////////////////////////////////////////////////////
//void glitch_filter_prescaler_switch(int tamper_pin, int desired_glitch_filter_setting)
//{
//    switch (tamper_pin)
//    {
//    case 0:
//        if (((DRY_RAC & DRY_RAC_GFR(0x1))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot read DRY_PGFR0 because its read access is blocked. Remove the read access block and try again.\n");
//            return;
//        }    
//        if (((DRY_WAC & DRY_WAC_GFW(0x1))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR0 because its write access is blocked. Remove the write access block and try again.\n");
//            return;
//        }
//        if (((DRY_LR & DRY_LR_GFL(0x1))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR0 because it is locked. Remove the lock and try again.\n");
//            return;
//        }        
//        if (desired_glitch_filter_setting == 0) {
//            DRY_TRACE("Current DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 &= ~DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 0 glitch filter disabled.\n");
//            DRY_TRACE("New DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        else
//        {
//            DRY_TRACE("Current DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 |= DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 0 glitch filter enabled.\n");
//            DRY_TRACE("New DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        break;
//    case 1:
//        if (((DRY_RAC & DRY_RAC_GFR(0x2))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot read DRY_PGFR1 because its read access is blocked. Remove the read access block and try again.\n");
//            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (((DRY_WAC & DRY_WAC_GFW(0x2))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR1 because its write access is blocked. Remove the write access block and try again.\n");
//            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
//            DRY_TRACE("\n");
//            return;
//        }
//        if (((DRY_LR & DRY_LR_GFL(0x2))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR1 because it is locked. Remove the lock and try again.\n");
//            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (desired_glitch_filter_setting == 0) {
//            DRY_TRACE("Current DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 &= ~DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 1 glitch filter disabled.\n");
//            DRY_TRACE("New DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        else
//        {
//            DRY_TRACE("Current DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 |= DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 1 glitch filter enabled.\n");
//            DRY_TRACE("New DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        break;
//    case 2:
//        if (((DRY_RAC & DRY_RAC_GFR(0x4))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot read DRY_PGFR2 because its read access is blocked. Remove the read access block and try again.\n");
//            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (((DRY_WAC & DRY_WAC_GFW(0x4))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR2 because its write access is blocked. Remove the write access block and try again.\n");
//            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
//            DRY_TRACE("\n");
//            return;
//        }
//        if (((DRY_LR & DRY_LR_GFL(0x4))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR2 because it is locked. Remove the lock and try again.\n");
//            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (desired_glitch_filter_setting == 0) {
//            DRY_TRACE("Current DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 &= ~DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 2 glitch filter disabled.\n");
//            DRY_TRACE("New DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        else
//        {
//            DRY_TRACE("Current DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 |= DRY_PGFR_GFE_MASK;
//            DRY_TRACE("Tamper Pin 2 glitch filter enabled.\n");
//            DRY_TRACE("New DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        break;
//    }        
//}
////////////////////////////////////////////////////////////////////////////////
//void glitch_filter_prescaler_select(int tamper_pin, int desired_glitch_filter_prescaler)
//{
//    DRY_TRACE("\n-------------------------------------------\n");
//    DRY_TRACE("  DryIce Pin Glitch Filter Clock Select Utility\n");
//    DRY_TRACE("-------------------------------------------\n");
//    DRY_TRACE("\n");  
//    switch (tamper_pin)
//    {
//    case 0:
//        if (((DRY_RAC & DRY_RAC_GFR(0x1))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot read DRY_PGFR0 because its read access is blocked. Remove the read access block and try again.\n");
//            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (((DRY_WAC & DRY_WAC_GFW(0x1))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR0 because its write access is blocked. Remove the write access block and try again.\n");
//            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
//            DRY_TRACE("\n");
//            return;
//        }
//        if (((DRY_LR & DRY_LR_GFL(0x1))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR0 because it is locked. Remove the lock and try again.\n");
//            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
//            DRY_TRACE("\n");
//            return;
//        }        
//        if ((DRY_PGFR0 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter prescaler setting when glitch filter is enabled. Please disable it first.\n");
//            return;
//        }    
//        if (desired_glitch_filter_prescaler == 0) {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 &= ~DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 0 glitch filter sourced by 512 Hz clock.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//            DRY_PGFR0 |= DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 0 glitch filter sourced by 32.768 kHz clock.\n");
//            DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        }
//        break;
//    case 1:
//        if (((DRY_RAC & DRY_RAC_GFR(0x2))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot read DRY_PGFR1 because its read access is blocked. Remove the read access block and try again.\n");
//            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (((DRY_WAC & DRY_WAC_GFW(0x2))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR1 because its write access is blocked. Remove the write access block and try again.\n");
//            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
//            DRY_TRACE("\n");
//            return;
//        }
//        if (((DRY_LR & DRY_LR_GFL(0x2))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR1 because it is locked. Remove the lock and try again.\n");
//            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        if ((DRY_PGFR1 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter prescaler setting when glitch filter is enabled. Please disable it first.\n");
//            return;
//        }    
//        if (desired_glitch_filter_prescaler == 0) {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 &= ~DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 1 glitch filter sourced by 512 Hz clock.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//            DRY_PGFR1 |= DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 1 glitch filter sourced by 32.768 kHz clock.\n");
//            DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);
//        }
//        break;
//    case 2:
//        if (((DRY_RAC & DRY_RAC_GFR(0x4))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot read DRY_PGFR2 because its read access is blocked. Remove the read access block and try again.\n");
//            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (((DRY_WAC & DRY_WAC_GFW(0x4))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR2 because its write access is blocked. Remove the write access block and try again.\n");
//            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
//            DRY_TRACE("\n");
//            return;
//        }
//        if (((DRY_LR & DRY_LR_GFL(0x4))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR2 because it is locked. Remove the lock and try again.\n");
//            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
//            DRY_TRACE("\n");
//            return;
//        }         
//        if ((DRY_PGFR2 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter prescaler setting when glitch filter is enabled. Please disable it first.\n");
//            return;
//        }
//        if (desired_glitch_filter_prescaler == 0) {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 &= ~DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 2 glitch filter sourced by 512 Hz clock.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        else
//        {
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//            DRY_PGFR2 |= DRY_PGFR_GFP_MASK;
//            DRY_TRACE("Tamper Pin 2 glitch filter sourced by 32.768 kHz clock.\n");
//            DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);
//        }
//        break;
//    }      
//}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//void glitch_filter_width_select(int tamper_pin, int desired_glitch_filter_width)
//{
//#ifdef CFG_DBG_HW_DRYICE 
//    int current_glitch_filter_width;
//#endif
//
//    switch (tamper_pin)
//    {
//    case 0:
//        if (((DRY_RAC & DRY_RAC_GFR(0x1))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot read DRY_PGFR0 because its read access is blocked. Remove the read access block and try again.\n");
//            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (((DRY_WAC & DRY_WAC_GFW(0x1))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR0 because its write access is blocked. Remove the write access block and try again.\n");
//            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
//            DRY_TRACE("\n");
//            return;
//        }
//        if (((DRY_LR & DRY_LR_GFL(0x1))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR0 because it is locked. Remove the lock and try again.\n");
//            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0); 
//        if ((DRY_PGFR0 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter width when glitch filter is enabled. Please disable it first.\n");
//            return;
//        }
//#ifdef CFG_DBG_HW_DRYICE 
//        current_glitch_filter_width = (DRY_PGFR0 & DRY_PGFR_GFW_MASK);
//        DRY_TRACE("Previous Tamper Pin 0 glitch filter width set to (%i + 1) * 2.\n",current_glitch_filter_width);    
//#endif
//        DRY_PGFR0 &= ~DRY_PGFR_GFW(0x3F);
//        DRY_PGFR0 |= DRY_PGFR_GFW(desired_glitch_filter_width);
//        DRY_TRACE("Tamper Pin 0 glitch filter width set to (%i + 1) * 2.\n",desired_glitch_filter_width);
//        DRY_TRACE("DRY_PGFR0 is %08X\n",DRY_PGFR0);
//        break;
//    case 1:
//        if (((DRY_RAC & DRY_RAC_GFR(0x2))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot read DRY_PGFR1 because its read access is blocked. Remove the read access block and try again.\n");
//            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (((DRY_WAC & DRY_WAC_GFW(0x2))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR1 because its write access is blocked. Remove the write access block and try again.\n");
//            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
//            DRY_TRACE("\n");
//            return;
//        }
//        if (((DRY_LR & DRY_LR_GFL(0x2))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR1 because it is locked. Remove the lock and try again.\n");
//            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
//            DRY_TRACE("\n");
//            return;
//        }     
//        DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);        
//        if ((DRY_PGFR1 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter width when glitch filter is enabled. Please disable it first.\n");
//            return;
//        }
//#ifdef CFG_DBG_HW_DRYICE 
//        current_glitch_filter_width = (DRY_PGFR1 & DRY_PGFR_GFW_MASK);
//        DRY_TRACE("Previous Tamper Pin 1 glitch filter width set to (%i + 1) * 2.\n",current_glitch_filter_width);    
//#endif
//        DRY_PGFR1 &= ~DRY_PGFR_GFW(0x3F);
//        DRY_PGFR1 |= DRY_PGFR_GFW(desired_glitch_filter_width);
//        DRY_TRACE("Tamper Pin 1 glitch filter width set to (%i + 1) * 2.\n",desired_glitch_filter_width);
//        DRY_TRACE("DRY_PGFR1 is %08X\n",DRY_PGFR1);    
//        break;
//    case 2:
//        if (((DRY_RAC & DRY_RAC_GFR(0x4))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot read DRY_PGFR2 because its read access is blocked. Remove the read access block and try again.\n");
//            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
//            DRY_TRACE("\n");
//            return;
//        }    
//        if (((DRY_WAC & DRY_WAC_GFW(0x4))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR2 because its write access is blocked. Remove the write access block and try again.\n");
//            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
//            DRY_TRACE("\n");
//            return;
//        }
//        if (((DRY_LR & DRY_LR_GFL(0x4))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0))
//        {
//            DRY_TRACE("Cannot write DRY_PGFR2 because it is locked. Remove the lock and try again.\n");
//            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
//            DRY_TRACE("\n");
//            return;
//        }     
//        DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);        
//        if ((DRY_PGFR2 & DRY_PGFR_GFE_MASK) == DRY_PGFR_GFE_MASK)
//        {
//            DRY_TRACE("Cannot change glitch filter width when glitch filter is enabled. Please disable it first.\n");
//            return;
//        }
//#ifdef CFG_DBG_HW_DRYICE 
//        current_glitch_filter_width = (DRY_PGFR2 & DRY_PGFR_GFW_MASK);
//        DRY_TRACE("Previous Tamper Pin 2 glitch filter width set to (%i + 1) * 2.\n",current_glitch_filter_width);    
//#endif
//        DRY_PGFR2 &= ~DRY_PGFR_GFW(0x3F);
//        DRY_PGFR2 |= DRY_PGFR_GFW(desired_glitch_filter_width);
//        DRY_TRACE("Tamper Pin 2 glitch filter width set to (%i + 1) * 2.\n",desired_glitch_filter_width);
//        DRY_TRACE("DRY_PGFR2 is %08X\n",DRY_PGFR2);    
//        break;
//    }      
//}
////////////////////////////////////////////////////////////////////////////////
void dryice_active_tamper_config(int active_tamper_reg, int desired_polynomial, int desired_shift_reg_value)
{
    int32 atr_reg_contents1,atr_reg_contents2,atr_reg_contents3;


    DRY_TRACE("\n-------------------------------------------\n");
    DRY_TRACE("  DryIce Pin Active Tamper Config Utility\n");
    DRY_TRACE("-------------------------------------------\n");
    DRY_TRACE("\n");

    if ((DRY_CR & DRY_CR_DEN_MASK) != DRY_CR_DEN_MASK)
    {
        DRY_TRACE("DryIce prescaler and clock are disabled. Please enable them first before modifying the active tamper registers.\n\n");
        DRY_TRACE("\n");
        return;
    }  

    switch (active_tamper_reg)
    {
    case 0:
        if (((DRY_RAC & DRY_RAC_ATR(0x1))>>(DRY_RAC_ATR_SHIFT+0) == 0x0))
        {
            DRY_TRACE("Cannot read DRY_ATR0 because its read access is blocked. Remove the read access block and try again.\n");
            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
            DRY_TRACE("\n");
            return;
        }    
        if (((DRY_WAC & DRY_WAC_ATW(0x1))>>(DRY_WAC_ATW_SHIFT+0) == 0x0))
        {
            DRY_TRACE("Cannot write DRY_ATR0 because its write access is blocked. Remove the write access block and try again.\n");
            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
            DRY_TRACE("\n");
            return;
        }
        if (((DRY_LR & DRY_LR_ATL(0x1))>>(DRY_LR_ATL_SHIFT+0) == 0x0))
        {
            DRY_TRACE("Cannot write DRY_ATR0 because it is locked. Remove the lock and try again.\n");
            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
            DRY_TRACE("\n");
            return;
        }            
        DRY_TRACE("\nCurrent DRY_ATR0 is %08X\n",DRY_ATR0);
        atr_reg_contents1 = DRY_ATR_ATP(desired_polynomial);
        atr_reg_contents2 = DRY_ATR_ATSR(desired_shift_reg_value);
        atr_reg_contents3 = atr_reg_contents1 | atr_reg_contents2;    
        DRY_TRACE("Writing new values to DRY_ATR0.\n");    
        DRY_ATR0 = atr_reg_contents3;    
        DRY_TRACE("New DRY_ATR0 is %32b",DRY_ATR0);    
        DRY_TRACE(" (%08X)\n",DRY_ATR0);
        break;

    case 1:
        if (((DRY_RAC & DRY_RAC_ATR(0x2))>>(DRY_RAC_ATR_SHIFT+1) == 0x0))
        {
            DRY_TRACE("Cannot read DRY_ATR1 because its read access is blocked. Remove the read access block and try again.\n");
            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
            DRY_TRACE("\n");
            return;
        }
        if (((DRY_WAC & DRY_WAC_ATW(0x2))>>(DRY_WAC_ATW_SHIFT+1) == 0x0))
        {
            DRY_TRACE("Cannot write DRY_ATR1 because its write access is blocked. Remove the write access block and try again.\n");
            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
            DRY_TRACE("\n");
            return;
        }
        if (((DRY_LR & DRY_LR_ATL(0x2))>>(DRY_LR_ATL_SHIFT+1) == 0x0))
        {
            DRY_TRACE("Cannot write DRY_ATR1 because it is locked. Remove the lock and try again.\n");
            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
            DRY_TRACE("\n");
            return;
        }        
        DRY_TRACE("\n Current DRY_ATR1 is %08X\n",DRY_ATR1);
        atr_reg_contents1 = DRY_ATR_ATP(desired_polynomial);
        atr_reg_contents2 = DRY_ATR_ATSR(desired_shift_reg_value);
        atr_reg_contents3 = atr_reg_contents1 | atr_reg_contents2;    
        DRY_TRACE("Writing new values to DRY_ATR1.\n");    
        DRY_ATR1 = atr_reg_contents3;    
        DRY_TRACE("New DRY_ATR1 is %32b",DRY_ATR1); 
        DRY_TRACE(" (%08X)\n",DRY_ATR1);    
        break;
    }
    DRY_TRACE("\n");  
    return;
}// end dryice_active_tamper_config
////////////////////////////////////////////////////////////////////////////////
void dryice_display_active_tamper_regs(void)
{
#ifdef CFG_DBG_HW_DRYICE 
    int shiftreg0;
    int shiftreg1;
    int active_tamper0_output, active_tamper1_output, xor_active_tamper_outputs;
#endif
    int cnt=1;
    int current_value;
    current_value = 0;

    DRY_TRACE("\n-------------------------------------------\n");
    DRY_TRACE("  DryIce Active Tamper Register Values\n");
    DRY_TRACE("-------------------------------------------\n");
    DRY_TRACE("\n");
    if ((DRY_CR & DRY_CR_DEN_MASK) != DRY_CR_DEN_MASK)
    {
        DRY_TRACE("DryIce prescaler and clock are disabled, so active tamper registers are static. Please enable DryIce prescaler and clock first.\n\n");
        DRY_TRACE("\n");
        return;
    }

    DRY_TRACE("\nPress any key to exit.\n\n");
    DRY_TRACE("-----Active Tamper 1 Output----------------------------------------+\n");
    DRY_TRACE("-----Active Tamper 0 Output----+                                   |\n");
    DRY_TRACE("                               |                                   |\n");
    DRY_TRACE("                               |                                   |\n");
    DRY_TRACE("                               V                                   V\n");
    while(1)
    {
        if(current_value != DRY_ATR0)
        {
#ifdef CFG_DBG_HW_DRYICE 
            shiftreg0 = DRY_ATR0 & 0x0000FFFF;
            active_tamper0_output = (shiftreg0 & 0x01);

            shiftreg1 = DRY_ATR1 & 0x0000FFFF;
            active_tamper1_output = (shiftreg1 & 0x01);

            xor_active_tamper_outputs = active_tamper0_output ^ active_tamper1_output;      
            DRY_TRACE("%03i. ", cnt);
#endif
            DRY_TRACE("ATR0[ATSR]=%016b (%04X); ATR1[ATSR]=%016b (%04X); Active Tamper 0 output XOR Active Tamper 1 output: %1b\n",shiftreg0,shiftreg0,shiftreg1,shiftreg1,xor_active_tamper_outputs);      
            cnt+=1;
            current_value = DRY_ATR0;
        }
        if(char_present()){
            break;
        }
    }     
    DRY_TRACE("\n");  
    return;
}// end dryice_display_active_tamper_regs

////////////////////////////////////////////////////////////////////////////////

void dryice_enable_clock_and_prescaler(int desired_dryice_clock_switch_setting)
{
#ifdef CFG_DBG_HW_DRYICE 
    int current_dryice_clock_switch_setting;
#endif


    DRY_TRACE("\n--------------------------------------------------------\n");
    DRY_TRACE("  DryIce Enable/Disable Prescaler and 32.768 kHz Clock \n");
    DRY_TRACE("--------------------------------------------------------\n");
    DRY_TRACE("\n");  

    if ((desired_dryice_clock_switch_setting < 0) || (desired_dryice_clock_switch_setting > 1))
    {
        DRY_TRACE("Invalid DryIce clock and prescaler option:\n");
        return;    
    }
#ifdef CFG_DBG_HW_DRYICE 
    current_dryice_clock_switch_setting = (DRY_CR & DRY_CR_DEN_MASK);
    DRY_TRACE("Previous DryIce clock and prescaler setting was \t%i\n",current_dryice_clock_switch_setting>>DRY_CR_DEN_SHIFT);
#endif
    if (desired_dryice_clock_switch_setting == 0)
    {
        DRY_CR &= ~DRY_CR_DEN_MASK;
    }
    else
    {
        DRY_CR |= DRY_CR_DEN_MASK;
    }
#ifdef CFG_DBG_HW_DRYICE 
    current_dryice_clock_switch_setting = (DRY_CR & DRY_CR_DEN_MASK);
    DRY_TRACE("New DryIce clock and prescaler setting is \t\t%i\n",current_dryice_clock_switch_setting>>DRY_CR_DEN_SHIFT);
    DRY_TRACE("\n");  
#endif
    return;  
}

void dryice_enable_TamperForceSystem(int mode)
{
    DRY_TRACE("\n-|DryIce Enable/Disable TAMPER FORCE SYSTEM RESET!\n");
    if ((mode < 0) || (mode > 1))
    {
        DRY_TRACE("Invalid DryIce clock and prescaler option:\n");
        return;    
    }
    if (mode == 0)
    {
        DRY_CR &= ~DRY_CR_TFSR_MASK;
    } else
    {
        DRY_CR |= DRY_CR_TFSR_MASK;
    }
    return;  
}


