/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : lcd_hw.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 7/31/2014 1:53:36 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "drv_inc.h"
#include "lcd_hw.h"


int hw_lcd_spi_init(void)
{
    SPI_InitDef spi_init;

    memset(&spi_init,0,sizeof(SPI_InitDef));
    spi_init.spix = LCD_SPIn;
    memset(spi_init.pcs,PTxInvid,SPI_PCS_MAX);
    spi_init.pcs[LCD_SPI_PCSn] = LCD_PTxy_PCSn;
    spi_init.sck  = LCD_PTxy_SCLK;
    spi_init.mosi = LCD_PTxy_MOSI;
    spi_init.miso = PTxInvid;
    spi_init.mode = SPI_MODE_MASTER;
    spi_init.attr = SCK_BR_DIV_4|MODE_MSBit//SCK_BR_DIV_2|SCK_PBR_DIV_2|SCK_DBR_MUL_2|MODE_MSBit
                    |CPOL_CLK_HIGH | CPHA_DATA_CAPTURED_FollowingEdge 
                    |FRAME_SIZE_8;
    spi_init.TxFIFO_switch = FALSE;
    spi_init.RxFIFO_switch = FALSE;
    spi_init.TxCompleteIrq = FALSE;
    spi_init.TxQueueEndIrq = FALSE;
    spi_init.TxFIFO_UnderflowIrq = FALSE;
    spi_init.RxFIFO_OverflowIrq  = FALSE;
    spi_init.TxFIFO_FillIrq  = FALSE;
    spi_init.RxFIFO_DrainIrq = FALSE;
    spi_init.TxFIFO_IrqMode  = FALSE;
    spi_init.RxFIFO_IrqMode  = FALSE;
    spi_init.p_asc  = 0;
    spi_init.t_asc  = 1;

    return hw_spi_init(&spi_init);
}

void hw_lcd_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init;

#if defined(LCD_SOFT_RESET)
    //INT->Reset
    gpio_init.GPIOx = LCD_GPIO_RESET;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF|PORT_Mode_Out_DSL;
    gpio_init.PORT_Pin = LCD_PINx_RESET;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(LCD_PINx_RESET);
    hw_gpio_init(&gpio_init);
    hw_gpio_set_bits(LCD_GPIO_RESET,(1<<LCD_PINx_RESET));
#endif

//    gpio_init.GPIOx = BT_SLEEP_GPIO;
//    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF|PORT_Mode_Out_DSL|PORT_Mode_IN_PU;
//    gpio_init.PORT_Pin = BT_SLEEP_PINx;
//    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(BT_SLEEP_PINx);
//    hw_gpio_init(&gpio_init);

    //RS(AD)
    gpio_init.GPIOx = LCD_GPIO_RS;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF|PORT_Mode_Out_DSL;
    gpio_init.PORT_Pin = LCD_PINx_RS;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(LCD_PINx_RS);
    hw_gpio_init(&gpio_init);
    hw_gpio_set_bits(LCD_GPIO_RS,(1<<LCD_PINx_RS));

    //Backligth
    gpio_init.GPIOx = LCD_GPIO_BACKLIGHT;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF|PORT_Mode_Out_DSL;
    gpio_init.PORT_Pin = LCD_PINx_BACKLIGHT;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(LCD_PINx_BACKLIGHT);
    hw_gpio_init(&gpio_init);
//    hw_gpio_set_bits(LCD_GPIO_BACKLIGHT,(1<<LCD_PINx_BACKLIGHT));
    hw_gpio_reset_bits(LCD_GPIO_BACKLIGHT,(1<<LCD_PINx_BACKLIGHT));
}

void hw_lcd_reset(uint8_t level)
{
#if defined(LCD_SOFT_RESET)
    if (level == 0)
        hw_gpio_reset_bits(LCD_GPIO_RESET,(1<<LCD_PINx_RESET));
    else
        hw_gpio_set_bits(LCD_GPIO_RESET,(1<<LCD_PINx_RESET));
#endif
}

void hw_lcd_backlight(uint8_t level)
{
    if (level == LCD_BACKLIGHT_OFF)
        hw_gpio_reset_bits(LCD_GPIO_BACKLIGHT,(1<<LCD_PINx_BACKLIGHT));
    else
        hw_gpio_set_bits(LCD_GPIO_BACKLIGHT,(1<<LCD_PINx_BACKLIGHT));
}

void hw_lcd_write_cmd(uint8_t cmd)
{
    hw_gpio_reset_bits(LCD_GPIO_RS,(1<<LCD_PINx_RS));
    hw_spi_master_write(LCD_SPIn,LCD_PCS_PIN,&cmd,1); 
}

void hw_lcd_write_bytedata(uint8_t data)
{
    hw_gpio_set_bits(LCD_GPIO_RS,(1<<LCD_PINx_RS));
    hw_spi_master_write(LCD_SPIn,LCD_PCS_PIN,&data,1); 
}

void hw_lcd_write_data(const uint8_t *data,uint32_t length)
{
    hw_gpio_set_bits(LCD_GPIO_RS,(1<<LCD_PINx_RS));
    hw_spi_master_write(LCD_SPIn,LCD_PCS_PIN,data,length); 
}

//SEGment:SEG0~SEG131
void hw_lcd_set_column(uint8_t column)
{
	if (column > 131)
	{
		column -= 132;
	}
    hw_lcd_write_cmd(LCD_CMD_COL_ADDRH|(column>>4));
	hw_lcd_write_cmd(LCD_CMD_COL_ADDRL|(column&0x0F));
}

//COM:COM0~COM64
void hw_lcd_set_line(uint8_t line)
{
    hw_lcd_write_cmd(LCD_CMD_START_LINE(line)); //Set start line
}

//page:0~7
void hw_lcd_set_page(uint8_t page)
{
	hw_lcd_write_cmd(LCD_CMD_PAGE_ADDR(page&0x07));	
}

/**
 * \brief   CMD:0xA6~A7.(正反色显示)
 *			Normal/Reverse Display:Reverses the Display ON/OFF status         
 *			without re-writing the contents of the display data RAM.
 * \param:mode   0-LCD_CMD_NORMAL 1-LCD_CMD_REVERSE(反色显示)  
 */
void hw_lcd_dismode(uint8_t mode)
{
	if (mode == LCD_NORMAL)
	{
		hw_lcd_write_cmd(LCD_CMD_DISP_NORMAL);
	} else
	{
		hw_lcd_write_cmd(LCD_CMD_DISP_REVERSE);
	}	
}

/**
 * \brief          CMD:0x81. DoubleByteCommand:electronic volume.
 *	Brightness Control: adjust the brightness of the liquid crystal display.
 * \param volume   electronic volume:[0-63].When the electronic volume 
 *				   function is not used, set D5 -D0 to 100000(64)       
 */
void hw_lcd_contrast(uint8_t volume)
{
	hw_lcd_write_cmd(LCD_CMD_REFVOL_1ST);
	hw_lcd_write_cmd(LCD_CMD_REFVOL_2ND(volume));
}


void hw_lcd_init(void)
{
    hw_lcd_spi_init();
    hw_lcd_gpio_init();
    hw_lcd_write_cmd(LCD_CMD_RESET); //Software reset

    hw_lcd_reset(0);
    s_DelayUs(6);
    hw_lcd_reset(1);

    hw_lcd_write_cmd(LCD_CMD_RESET); //Software reset
    hw_lcd_write_cmd(LCD_CMD_BIAS_A2); //Bias select a2
    hw_lcd_write_cmd(LCD_CMD_COM_REVERSE); //COM deriction
    hw_lcd_write_cmd(LCD_CMD_ADC_NORMAL); //SEG deriction
    //Power control
    hw_lcd_write_cmd(LCD_CMD_POWER_CTRL
                     |LCD_CMD_POWER_ConverterCircuitMASK);
    hw_lcd_write_cmd(LCD_CMD_POWER_CTRL
                     |LCD_CMD_POWER_ConverterCircuitMASK
                     |LCD_CMD_POWER_RegulatorCircuitMASK); 
    hw_lcd_write_cmd(LCD_CMD_POWER_CTRL
                     |LCD_CMD_POWER_ConverterCircuitMASK
                     |LCD_CMD_POWER_RegulatorCircuitMASK
                     |LCD_CMD_POWER_FollowerCircuitMASK); 

//hw_lcd_write_cmd(0xF8); //Set booster
//hw_lcd_write_cmd(0x00);

    hw_lcd_write_cmd(LCD_CMD_V0_SET(4)); //V0 control
//    hw_lcd_write_cmd(LCD_CMD_REFVOL_1ST);
//    hw_lcd_write_cmd(LCD_CMD_REFVOL_2ND(0x27));//28:9.0V
    hw_lcd_contrast(0x27);

    hw_lcd_set_line(0); //Set start line
    hw_lcd_write_cmd(LCD_CMD_DISPLAY_ON);   //display on

}



