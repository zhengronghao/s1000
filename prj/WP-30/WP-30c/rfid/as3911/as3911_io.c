/*
 *****************************************************************************
 * Copyright @ 2009 by austriamicrosystems AG                                *
 * All rights are reserved.                                                  *
 *                                                                           *
 * Reproduction in whole or in part is prohibited without the written consent*
 * of the copyright owner. Austriamicrosystems reserves the right to make    *
 * changes without notice at any time. The software is provided as is and    *
 * Austriamicrosystems makes no warranty, expressed, implied or statutory,   *
 * including but not limited to any implied warranty of merchantability or   *
 * fitness for any particular purpose, or that the use will not infringe any *
 * third party patent, copyright or trademark. Austriamicrosystems should    *
 * not be liable for any loss or damage arising from its use.                *
 *****************************************************************************
 */

/*
 * PROJECT: AS3911 firmware
 * $Revision: $
 * LANGUAGE: ANSI C
 */

/*! \file as3911_io.c
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3911 SPI communication.
 *
 * Implementation of the AS3911 SPI communication. The PIC is set to IPL 7 to disable
 * interrupts while accessing the SPI.
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "wp30_ctrl.h"

#if (defined(EM_AS3911_Module))
/*
******************************************************************************
* DEFINES
******************************************************************************
*/

#define AS3911_SPI_ADDRESS_MASK         (0x3F)
#define AS3911_SPI_CMD_READ_REGISTER    (0x40)
#define AS3911_SPI_CMD_WRITE_REGISTER   (0x00)
#define AS3911_SPI_CMD_READ_FIFO        (0xBF)
#define AS3911_SPI_CMD_WRITE_FIFO       (0x80)
#define AS3911_SPI_CMD_DIREC_CMD        (0xC0)

#define AS3911_IO_MAX_CHAINED_DIRECT_COMMANDS   (10)

/*
******************************************************************************
* MACROS
******************************************************************************
*/

//#define AS3911_SEN_ON() { _LATB8 = 0; }
//#define AS3911_SEN_OFF() { _LATB8 = 1; }
//#define as3911_SPI_RFID_ID     SPI_ID_1
/* FIXME: remove macros and references in the code once CS from the SPI module works. */
#define AS3911_SEN_ON()        //hw_gpio_reset_bits(GPIOE,GPIO_Pin_4)//SPI_SetCS(RFID_SPIn,SPIBUS0_RFID,ON)//set_pio_output(BOARD_PIN_SPI0_CS3_RF, 0)
#define AS3911_SEN_OFF()       //hw_gpio_set_bits(GPIOE,GPIO_Pin_4)//SPI_SetCS(RFID_SPIn,SPIBUS0_RFID,OFF)//set_pio_output(BOARD_PIN_SPI0_CS3_RF, 1)
#define AS3911_CloseIsr()      s_Rfid_SetInt(OFF)//s_CloseIsr()
#define AS3911_OpenIsr()       s_Rfid_SetInt(ON)//s_OpenIsr()
//#define AS3911_CloseIsr()      s_CloseIsr() //采用这个屏蔽中断速度太慢
//#define AS3911_OpenIsr()       s_OpenIsr()  //采用这个屏蔽中断速度太慢
//#define s_CloseIsr()           s_Rfid_SetInt(OFF)
//#define s_OpenIsr()            s_Rfid_SetInt(ON)
/*
******************************************************************************
* LOCAL DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL TABLES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL VARIABLE DEFINITIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/

uchar spi_exchange_block_EX(uchar data, uchar cs)
{
    if ( cs == 0 )
        return hw_spi_master_WriteRead(RFID_SPIn,data,RFID_PCS_PIN,SPI_PCS_ASSERTED);
    else
        return hw_spi_master_WriteRead(RFID_SPIn,data,RFID_PCS_PIN,SPI_PCS_INACTIVE);
}

char s_as3911WriteRegister(ulong ulLen, uchar ucRegAddr, uchar * pucData)
{
    uint i;
	vuchar data;

    if ( ucRegAddr != AS3911_REG_WRITE_FIFO) {
        //不是写FIFO指令
        ucRegAddr = AS3911_SPI_CMD_WRITE_REGISTER | (ucRegAddr & AS3911_SPI_ADDRESS_MASK); 
    }
    AS3911_CloseIsr();
	AS3911_SEN_ON();
	data = spi_exchange_block_EX(ucRegAddr,0);
	for(i=0;i<ulLen-1;i++)
	{
		data = spi_exchange_block_EX(pucData[i],0);
	}
    data = spi_exchange_block_EX(pucData[i],1);
	AS3911_SEN_OFF();
    AS3911_OpenIsr();
    return 0;
}

char s_as3911ReadRegister(ulong ulLen, uchar ucRegAddr, uchar * pucData)
{
	uint i;
	vuchar tmp;

    if ( ucRegAddr != AS3911_SPI_CMD_READ_FIFO ) {
        ucRegAddr = AS3911_SPI_CMD_READ_REGISTER | (ucRegAddr & AS3911_SPI_ADDRESS_MASK);
    }
    AS3911_CloseIsr();
	AS3911_SEN_ON();
	tmp = spi_exchange_block_EX(ucRegAddr,0);
	for(i=0;i<ulLen-1;i++)
	{
		tmp = spi_exchange_block_EX(0xFF,0);
		pucData[i] = tmp;
	}
    tmp = spi_exchange_block_EX(0xFF,1);
    pucData[i] = tmp;
	AS3911_SEN_OFF();
    AS3911_OpenIsr();

    return 0;
}

void s_as3911_SetBitMask(uchar ucRegAddr, uchar ucBitMask)
{
	uchar ucTemp;
	s_as3911ReadRegister(1, ucRegAddr, &ucTemp);
	ucTemp |= ucBitMask;
	s_as3911WriteRegister(1, ucRegAddr, &ucTemp);
}

void s_as3911_ClrBitMask(uchar ucRegAddr, uchar ucBitMask)
{
	uchar ucTemp;
	s_as3911ReadRegister(1, ucRegAddr, &ucTemp);
	ucTemp &= ~ucBitMask;
	s_as3911WriteRegister(1, ucRegAddr, &ucTemp);
}

char as3911WriteRegister(uchar address, uchar data)
{
    uchar value = data;
    return (s_as3911WriteRegister(1, address, &value));
}

char as3911ReadRegister(uchar address, uchar * data)
{
    return (s_as3911ReadRegister(1, address, data));
}

s8 as3911WriteTestRegister(u8 address, u8 data)
{
	vuchar tmp;

    AS3911_CloseIsr();
	AS3911_SEN_ON();
	tmp = spi_exchange_block_EX((AS3911_SPI_CMD_DIREC_CMD | AS3911_CMD_TEST_ACCESS),0);
    tmp = spi_exchange_block_EX(AS3911_SPI_CMD_WRITE_REGISTER | (address & AS3911_SPI_ADDRESS_MASK),0);
    tmp = spi_exchange_block_EX(data,1);
	AS3911_SEN_OFF();
    AS3911_OpenIsr();

    return ERR_NONE;
}

s8 as3911ReadTestRegister(u8 address, u8 *data)
{
	vuchar tmp;

    AS3911_CloseIsr();
	AS3911_SEN_ON();
	tmp = spi_exchange_block_EX((AS3911_SPI_CMD_DIREC_CMD | AS3911_CMD_TEST_ACCESS),0);
	tmp = spi_exchange_block_EX(AS3911_SPI_CMD_READ_REGISTER | (address & AS3911_SPI_ADDRESS_MASK),0);
    *data = spi_exchange_block_EX(0xFF,1);
	AS3911_SEN_OFF();
    AS3911_OpenIsr();

    return ERR_NONE;
}

char as3911ContinuousWrite(uchar address, uchar *data, uchar length)
{
    return (s_as3911WriteRegister(length, address, data));
}

char as3911ContinuousRead(uchar address, uchar *data, uchar length)
{
    return (s_as3911ReadRegister(length, address, data));
}

char as3911ModifyRegister(uchar address, uchar mask, uchar value)
{
//    char error = ERR_NONE;
    uchar registerValue = 0;

//    error |= as3911ReadRegister(address, &registerValue);
//    registerValue = (registerValue & ~mask) | data;
//    error |= as3911WriteRegister(address, registerValue);
//    if (ERR_NONE == error)
//        return ERR_NONE;
//    else
//        return ERR_IO;

    as3911ReadRegister(address, &registerValue);
    registerValue = (registerValue & ~mask) | value;
    as3911WriteRegister(address, registerValue);
    return 0;
}

char as3911WriteFifo(uchar *data, uchar length)
{
    if (length == 0)
        return ERR_NONE;
    return (s_as3911WriteRegister(length, AS3911_REG_WRITE_FIFO, data));
}

char as3911ReadFifo(uchar *data, uchar length)
{
    if (length == 0)
        return ERR_NONE;
    return (s_as3911ReadRegister(length, AS3911_SPI_CMD_READ_FIFO, data));
}

char as3911ExecuteCommand(uchar directCommand)
{
    directCommand = AS3911_SPI_CMD_DIREC_CMD | (directCommand & AS3911_SPI_ADDRESS_MASK);
    AS3911_CloseIsr();
	AS3911_SEN_ON();
	spi_exchange_block_EX(directCommand,1);
	AS3911_SEN_OFF();
    AS3911_OpenIsr();
    return 0;
}

char as3911ExecuteCommands(uchar *directCommands, uchar length)
{
//    s8 error = ERR_NONE;
    uchar index = 0;
    u8 as3911DirectCommands[AS3911_IO_MAX_CHAINED_DIRECT_COMMANDS];
    
    /* FIXME: Check for better error code for internal buffer overflow. */
    if (length > AS3911_IO_MAX_CHAINED_DIRECT_COMMANDS)
        return ERR_IO;
    
    for (index = 0; index < length; ++index)
        as3911DirectCommands[index] = AS3911_SPI_CMD_DIREC_CMD | (directCommands[index] & AS3911_SPI_ADDRESS_MASK);
    
    AS3911_CloseIsr();
	AS3911_SEN_ON();
    for (index = 0; index < (length-1); ++index)
        spi_exchange_block_EX(as3911DirectCommands[index],0);
    spi_exchange_block_EX(as3911DirectCommands[index],1);
	AS3911_SEN_OFF();
    AS3911_OpenIsr();
    
//    if (ERR_NONE != error)
//        return ERR_IO;
//    else
//        return ERR_NONE;
    return 0;
}
/*! 
 *****************************************************************************
 *  \brief  Executes a direct command and returns the result
 *
 *  This function executes the direct command given by \a cmd waits for
 *  \a sleeptime and returns the result read from register \a resreg.
 *
 *  \param[in] cmd: direct command to execute.
 *  \param[in] resreg: Address of the register containing the result.
 *  \param[in] sleeptime: time in milliseconds to wait before reading the result.
 *  \param[out] result: 8 bit long result
 *
 *  \return ERR_IO : Error during communication with AS3911.
 *  \return ERR_NONE : No error, result of direct command written to \a result.
 *
 *****************************************************************************
 */
s8 as3911ExecuteCommandAndGetResult(u8 cmd, u8 resreg, u8 sleeptime, u8* result)
{
    s8 err;
    u32 irqs;

    if (cmd == AS3911_CMD_ADJUST_REGULATORS)
    {
        err = as3911ExecuteCommand(cmd);
        sleepMilliseconds(sleeptime);
    }
    else if (   (cmd >= AS3911_CMD_INITIAL_RF_COLLISION && cmd <= AS3911_CMD_RESPONSE_RF_COLLISION_0)
            || (cmd == AS3911_CMD_MEASURE_AMPLITUDE)
            || (cmd >= AS3911_CMD_ADJUST_REGULATORS && cmd <= AS3911_CMD_MEASURE_PHASE)
            || (cmd >= AS3911_CMD_CALIBRATE_C_SENSOR && cmd <= AS3911_CMD_MEASURE_VDD)
            || (cmd >= 0xFD && cmd <= 0xFE )
       )
    {
        as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
        as3911GetInterrupts(AS3911_IRQ_MASK_DCT, &irqs);
        err = as3911ExecuteCommand(cmd);
        as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, sleeptime, &irqs);
        as3911DisableInterrupts(AS3911_IRQ_MASK_DCT);
    }
    else
    {
        err = as3911ExecuteCommand(cmd);
        sleepMilliseconds(sleeptime);
    }

    /* read out the result if the pointer is not NULL */
    if (result)
        err |= as3911ReadRegister(resreg, result);

    return err;
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

#endif

