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
 
/*! \file as3911_io.h
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3911 SPI communication.
 */

/*! \defgroup as3911SpiIo AS3911 SPI IO
 * \ingroup as3911
 *
 * \brief This part of the AS3911 module provides access to the AS3911 via the
 * SPI.
 *
 * This module abstracts the SPI interface of the AS3911. It contains functions
 * to read or write single registers:
 * - as3911ReadRegister()
 * - as3911WriteRegister()
 *
 * to read or write a continuous block of registers in a single SPI operation
 * (using AS3911 auto increment mode):
 * - as3911ContinuousRead()
 * - as3911ContinuousWrite()
 * 
 * to modify a register (performs a read, modify, write operation):
 * - as3911ModifyRegister()
 *
 * to access the FIFO of the AS3911:
 * - as3911ReadFifo()
 * - as3911WriteFifo()
 *
 * to send direct commands to the AS3911:
 * - as3911ExecuteCommand()
 *
 * and to access the test mode registers:
 * - as3911ReadTestRegister()
 * - as3911WriteTestRegister()
 *
 * All SPI data transfers are guaranteed to be completed after any of the
 * function returns. But do note that the execution of a direct command might
 * take some time to complete even after the transmission of that command is
 * completed.
 */

#ifndef AS3911_IO_H
#define AS3911_IO_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "ams_types.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL VARIABLE DECLARATIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*! \ingroup as3911SpiIo
 *****************************************************************************
 * \brief Write to a register of the AS3911.
 *
 * \param[in] address Address of the register.
 * \param[in] data The data to write to regsiter \a address.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, register write operaiton successful.
 *****************************************************************************
 */
char as3911WriteRegister(uchar address, uchar data);

/*! \ingroup as3911SpiIo
 *****************************************************************************
 * \brief Read a register of the AS3911.
 *
 * \param[in] address Address of the register to read out.
 * \param[out] data Set to the content of register \a address.
 * 
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, register read operation successful.
 *****************************************************************************
 */
char as3911ReadRegister(uchar address, uchar * data);

/*! \ingroup as3911SpiIo
 *****************************************************************************
 * \brief Modify certain bits of a register while not touching other bits of
 * the same register.
 *
 * All register bits where the respective \a mask bits are one are set to
 * the value defined by \a value. In boolean notation this is equivalent to:
 * register = (register & ~mask) | value;
 *
 * \note This is implemented via a read modify write operation.
 *
 * \param[in] address Address of the register modify.
 * \param[in] mask Defines the bit which are modified.
 * \param[in] value New value of the bits selected for modification by \a mask.
 * 
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, register modify operation successful.
 *****************************************************************************
 */
char as3911ModifyRegister(uchar address, uchar mask, uchar value);

/*! \ingroup as3911SpiIo
 *****************************************************************************
 * \brief Write to a test register of the AS3911.
 *
 * \param[in] address Address of the test register.
 * \param[in] data The data to write to test register \a address.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, test register write opreation successful.
 *****************************************************************************
 */
s8 as3911WriteTestRegister(u8 address, u8 data);

/*! \ingroup as3911SpiIo
 *****************************************************************************
 * \brief Read a test register of the AS3911.
 *
 * \param[in] address Address of the test register to read out.
 * \param[out] data Set to the content of test register \a address.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, test register read operation successful.
 *****************************************************************************
 */
s8 as3911ReadTestRegister(u8 address, u8 *data);

/*! \ingroup as3911SpiIo
 *****************************************************************************
 * \brief Write to a continuous set of registers of the AS3911 using auto
 * increment mode.
 *
 * \param[in] address Start address of the continuous write operation.
 * \param[in] data The data to write to the registers \a address to \a address
 * + \a length - 1
 * \param[in] length Number of bytes to write to the AS3911.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, continuous write operation successful.
 *****************************************************************************
 */
char as3911ContinuousWrite(uchar address,uchar *data, uchar length);

/*! \ingroup as3911SpiIo
 *****************************************************************************
 * \brief Read a continuous set of registers of the AS3911 using auto
 * increment mode.
 *
 * \param[in] address Start address of the continuous read operation.
 * \param[out] data Set to the content of the registers \a addresss to \a address
 * + \a length - 1.
 * \param[in] length Number of bytes to read from the AS3911.
 *
 * \note The array pointed to by \a data must be large enough to hold at least
 * \a length bytes.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, continuous read operation successful.
 *****************************************************************************
 */
char as3911ContinuousRead(uchar address, uchar *data, uchar length);

/*! \ingroup as3911SpiIo
 *****************************************************************************
 * \brief Write to the FIFO of the AS3911.
 *
 * \param[in] data The data to write into the FIFO.
 * \param[in] length Number of bytes to write into the FIFO.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, FIFO write operation successful.
 *****************************************************************************
 */
char as3911WriteFifo(uchar *data, uchar length);

/*! \ingroup as3911SpiIo
 *****************************************************************************
 * \brief Read from the FIFO of the AS3911.
 *
 * \param[out] data Stores the data read from the FIFO.
 * \param[in] length Number of bytes to read from the FIFO.
 * 
 * \note The array pointed to by \a data must be large enought to
 * hold at least \a length bytes.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, FIFO read operation successful.
 *****************************************************************************
 */
char as3911ReadFifo(uchar *data, uchar length);

/*! \ingroup as3911SpiIo
 *****************************************************************************
 * \brief Send a direct command to the AS3911.
 *
 * \param[in] directCommand Direct command to send to the AS3911.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, Direct command send operation successful.
 *****************************************************************************
 */
char as3911ExecuteCommand(uchar directCommand);

/*! \ingroup as3911SpiIo
 *****************************************************************************
 * \brief Send a sequence of chained direct commands to the AS3911.
 * 
 * \note Please refer to the datasheet to see which direct commands can be
 *   chained and which not.
 *   
 * \param[in] directCommands Array of direct commands to be send.
 * \param[in] length Number of direct commands stored in \a directCommands.
 * 
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, Direct commands send successful.
 *****************************************************************************
 */
char as3911ExecuteCommands(uchar *directCommands, uchar length);
s8 as3911ExecuteCommandAndGetResult(u8 cmd, u8 resreg, u8 sleeptime, u8* result);

char s_as3911WriteRegister(ulong ulLen, uchar ucRegAddr, uchar * pucData);
char s_as3911ReadRegister(ulong ulLen, uchar ucRegAddr, uchar * pucData);
void s_as3911_SetBitMask(uchar ucRegAddr, uchar ucBitMask);
void s_as3911_ClrBitMask(uchar ucRegAddr, uchar ucBitMask);


#endif /* AS3911_IO_H */
