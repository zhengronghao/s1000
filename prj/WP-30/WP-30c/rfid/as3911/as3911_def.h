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
 
/*! \file as3911_def.h
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3911 register and direct command definitions.
 */

#ifndef AS3911_DEF_H
#define AS3911_DEF_H

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

/*! \name AS3911 register addresses */
/**@{*/
/*! \ingroup as3911 */

/* AS3911 registers. */
#define AS3911_REG_IO_CONF1                     0x00        /*!< RW IO Configuration Register 1. */
#define AS3911_REG_IO_CONF2                     0x01        /*!< RW IO Configuration Register 2. */

#define AS3911_REG_OP_CONTROL                   0x02        /*!< RW Operation Control Register. */
#define AS3911_REG_MODE                         0x03        /*!< RW Mode Definition Register. */
#define AS3911_REG_BIT_RATE                     0x04        /*!< RW Bit Rate Definition Register. */

#define AS3911_REG_ISO14443A_NFC                0x05        /*!< RW ISO14443A and NFC 106 kBit/s Settings Register. */
#define AS3911_REG_ISO14443B_1                  0x06        /*!< RW ISO14443B Settings Register 1. */
#define AS3911_REG_ISO14443B_2                  0x07        /*!< RW ISO14443B Settings Register 2. */
#define AS3911_REG_STREAM_MODE                  0x08        /*!< RW Stream Mode Definition Register. */
#define AS3911_REG_AUX                          0x09        /*!< RW Auxiliary Definition Register. */
#define AS3911_REG_RX_CONF1                     0x0A        /*!< RW Receiver Configuration Register 1. */
#define AS3911_REG_RX_CONF2                     0x0B        /*!< RW Receiver Configuration Register 2. */
#define AS3911_REG_RX_CONF3                     0x0C        /*!< RW Receiver Configuration Register 3. */
#define AS3911_REG_RX_CONF4                     0x0D        /*!< RW Receiver Configuration Register 4. */

#define AS3911_REG_MASK_RX_TIMER                0x0E        /*!< RW Mask Receive Timer Register. */
#define AS3911_REG_NO_RESPONSE_TIMER1           0x0F        /*!< RW No-response Timer Register 1. */
#define AS3911_REG_NO_RESPONSE_TIMER2           0x10        /*!< RW No-response Timer Register 2. */
#define AS3911_REG_GPT_CONF                     0x11        /*!< RW General Purpose Timer Control Register. */
#define AS3911_REG_GPT1                         0x12        /*!< RW General Purpose Timer Register 1. */
#define AS3911_REG_GPT2                         0x13        /*!< RW General Purpose Timer Register 2. */

#define AS3911_REG_IRQ_MASK_MAIN                0x14        /*!< RW Mask Main Interrupt Register. */
#define AS3911_REG_IRQ_MASK_TIMER_NFC           0x15        /*!< RW Mask Tiemr and NFC Interrupt Register. */
#define AS3911_REG_IRQ_MASK_ERROR_WUP           0x16        /*!< RW Mask Error and Wake-up Interrupt Register. */
#define AS3911_REG_IRQ_MAIN                     0x17        /*!< R  Main Interrupt Register. */
#define AS3911_REG_IRQ_TIMER_NFC                0x18        /*!< R  Timer and NFC Interrupt Register. */
#define AS3911_REG_IRQ_ERROR_WUP                0x19        /*!< R  Error and Wake-up Interrupt Register. */
#define AS3911_REG_FIFO_RX_STATUS1              0x1A        /*!< RW FIFO RX Status Register 1. */
#define AS3911_REG_FIFO_RX_STATUS2              0x1B        /*!< RW FIFO RX Status Register 2. */
#define AS3911_REG_COLLISION_STATUS             0x1C        /*!< RW Collision Display Register. */

#define AS3911_REG_NUM_TX_BYTES1                0x1D        /*!< RW Number of Transmitted Bytes Register 1. */
#define AS3911_REG_NUM_TX_BYTES2                0x1E        /*!< RW Number of Transmitted Bytes Register 2. */

#define AS3911_REG_NFCIP1_BIT_RATE              0x1F        /*!< RW NFCIP Bit Rate Detection Display Register. */

#define AS3911_REG_AD_RESULT                    0x20        /*!< R  A/D Converter Output Register. */

#define AS3911_REG_ANT_CAL_CONF                 0x21        /*!< RW Antenna Calibration Control Register. */
#define AS3911_REG_ANT_CAL_TARGET               0x22        /*!< RW Antenna Calibration Target Register. */
#define AS3911_REG_ANT_CAL_RESULT               0x23        /*!< R  Antenna Calibration Display Register. */

#define AS3911_REG_AM_MOD_DEPTH_CONF            0x24        /*!< RW AM Modulation Depth Control Register. */
#define AS3911_REG_AM_MOD_DEPTH_RESULT          0x25        /*!< R  AM Modulation Depth Display Register. */
#define AS3911_REG_RFO_AM_ON_LEVEL              0x26        /*!< RW RFO AM Modulation (On) Level Definition Register. */
#define AS3911_REG_RFO_AM_OFF_LEVEL             0x27        /*!< RW RFO Normal (AM Off) Level Definition Register. */

#define AS3911_REG_FIELD_THRESHOLD              0x29        /*!< RW External Field Detector Threshold Register. */

#define AS3911_REG_VSS_REGULATOR_CONF           0x2A        /*!< RW Regulated Voltage Control Register. */
#define AS3911_REG_VSS_REGULATOR_RESULT         0x2B        /*!< R Regulator Display Register. */

#define AS3911_REG_RX_STATE1                    0x2C        /*!< R Receiver State Display Register 1. */
#define AS3911_REG_RX_STATE2                    0x2D        /*!< R Receiver State Display Register 2. */

#define AS3911_REG_CAP_SENSOR_CONF              0x2E        /*!< RW Capacitive Sensor Control Register. */
#define AS3911_REG_CAP_SENSOR_RESULT            0x2F        /*!< R  Capacitive Sensor Display Register. */

#define AS3911_REG_AUX_DISPLAY                  0x30        /*!< R Auxiliary Display Register. */

#define AS3911_REG_WUP_TIMER_CONF               0x31        /*!< RW Wake-up Timer Control Register. */
#define AS3911_REG_AMPLITUDE_MEASURE_CONF       0x32        /*!< RW Amplitude Measurement Configuration Register. */
#define AS3911_REG_AMPLITUDE_MEASURE_REF        0x33        /*!< RW Amplitude Measurement Referebce Register. */
#define AS3911_REG_AMPLITUDE_MEASURE_AA_RESULT  0x34        /*!< R  Amplitude Measurement Auto Averaging Display Register. */
#define AS3911_REG_AMPLITUDE_MEASURE_RESULT     0x35        /*!< R  Amplitude Measurement Display Register. */
#define AS3911_REG_PHASE_MEASURE_CONF           0x36        /*!< RW Phase Measurement Configuration Register. */
#define AS3911_REG_PHASE_MEASURE_REF            0x37        /*!< RW Phase Measurement Reference Register. */
#define AS3911_REG_PHASE_MEASURE_AA_RESULT      0x38        /*!< R  Phase Measurement Auto Averaging Display Register. */
#define AS3911_REG_PHASE_MEASURE_RESULT         0x39        /*!< R  Phase Measurement Display Register. */
#define AS3911_REG_CAPACITANCE_MEASURE_CONF     0x3A        /*!< RW Capacitance Measurement Configuration Register. */
#define AS3911_REG_CAPACITANCE_MEASURE_REF      0x3B        /*!< RW Capacitance Measurement Reference Register. */
#define AS3911_REG_CAPACITANCE_MEASURE_AA_RESULT 0x3C       /*!< R  Capacitance Measurement Auto Averaging Display Register. */
#define AS3911_REG_CAPACITANCE_MEASURE_RESULT   0x3D        /*!< R  Capacitance Measurement Display Register. */

#define AS3911_REG_WRITE_FIFO 0x80 //AS3911_SPI_CMD_WRITE_FIFO
#define AS3911_REG_READ_FIFO  0xBF //AS3911_SPI_CMD_READ_FIFO 
/**@}*/

/*! \name AS3911 test register addresses */
/**@{*/
/*! \ingroup as3911 */

/* AS3911 test registers. */
/**
 *****************************************************************************
 * RW Analog Test and Observation Register.
 *
 * \note Use as3911ReadTestRegister and as3911WriteTestRegister to access
 * this register.
 *****************************************************************************
 */
#define AS3911_REG_ANALOG_TEST                  0x01        
/**
 *****************************************************************************
 * RW Test Mode Definition Register.
 *
 * \note Use as3911ReadTestRegister and as3911WriteTestRegister to access
 * this register.
 *****************************************************************************
 */
#define AS3911_REG_TEST_MODE                    0x02
/**
 *****************************************************************************
 * RW PROM Test Mode DefinitionRegister.
 *
 * \note Use as3911ReadTestRegister and as3911WriteTestRegister to access
 * this register.
 *****************************************************************************
 */
#define AS3911_REG_PROM_TEST_MODE               0x03

/**@}*/

/*! \name AS3911 direct commands */
/**@{*/
/*! \ingroup as3911 */
/* AS3911 direct commands. */
#define AS3911_CMD_SET_DEFAULT              0x01    /*!< Put the chip into the default state (same as after power-up). */
#define AS3911_CMD_CLEAR_FIFO               0x02    /*!< Clear the FIFO and stop all activities (e.g. timers). */
#define AS3911_CMD_TRANSMIT_WITH_CRC        0x04    /*!< Transmit with CRC. */
#define AS3911_CMD_TRANSMIT_WITHOUT_CRC     0x05    /*!< Transmit without CRC. */
#define AS3911_CMD_TRANSMIT_REQA            0x06    /*!< Transmit REQA. */
#define AS3911_CMD_TRANSMIT_WUPA            0x07    /*!< Transmit WUPA. */
#define AS3911_CMD_INITIAL_RF_COLLISION     0x08    /*!< Perform NFC initial RF collision avoidance. */
#define AS3911_CMD_RESPONSE_RF_COLLISION_N  0x09    /*!< Perform NFC response RF collision avoidance. */
#define AS3911_CMD_RESPONSE_RF_COLLISION_0  0x0A    /*!< Perform NFC response RF Collision avoidance with n=0. */
#define AS3911_CMD_NORMAL_NFC_MODE          0x0B    /*!< Switch from NFC target bit rate detection mode to normal NFC mode. */
#define AS3911_CMD_MASK_RECEIVE_DATA        0x10    /*!< Disable receiver, RSSI, and AGC. */
#define AS3911_CMD_UNMASK_RECEIVE_DATA      0x11    /*!< Enable receiver, RSSI, and AGC. */
#define AS3911_CMD_MEASURE_AMPLITUDE        0x13    /*!< Measure singal amplitude on RFI inputs. */
#define AS3911_CMD_SQUELCH                  0x14    /*!< Start squelch process. */
#define AS3911_CMD_CLEAR_SQUELCH            0x15    /*!< Clear squelch gain reduction and stop any ongoing squelch process. */
#define AS3911_CMD_ADJUST_REGULATORS        0x16    /*!< Adjust the voltage regulators. */
#define AS3911_CMD_CALIBRATE_MODULATION     0x17    /*!< Calibrate modulation depth. */
#define AS3911_CMD_CALIBRATE_ANTENNA        0x18    /*!< Calibrate antenna trimming. */
#define AS3911_CMD_MEASURE_PHASE            0x19    /*!< Measure the phase difference between the RFO and RFI signals. */
#define AS3911_CMD_MEASURE_RSSI             0x1A    /*!< clear the RSSI bits and restart RSSI measurement. */
#define AS3911_CMD_TRANSPARENT_MODE         0x1C    /*!< Set the chip into transparent mode. */
#define AS3911_CMD_CALIBRATE_C_SENSOR       0x1D    /*!< Calibrate the capacitive sensor. */
#define AS3911_CMD_MEASURE_CAPACITANCE      0x1E    /*!< Measure the capacitance between CSI and CSO pins. */
#define AS3911_CMD_MEASURE_VSS              0x1F    /*!< Measure the power supply voltage. */
#define AS3911_CMD_START_GP_TIMER           0x20    /*!< Start the general purpose timer. */
#define AS3911_CMD_START_WUP_TIMER          0x21    /*!< Start the wake-up timer. */
#define AS3911_CMD_START_MASK_RECEIVE_TIMER 0x22    /*!< Start the mask-receive timer. */
#define AS3911_CMD_START_NO_RESPONSE_TIMER  0x23    /*!< Start the no-repsonse timer. */
#define AS3911_CMD_TEST_ACCESS              0x3C    /*!< Enable R/W access to the test registers. */
#define AS3911_CMD_LOAD_PPROM               0x3D    /*!< Load data from the poly fuses to RAM. */
#define AS3911_CMD_FUSE_PPROM               0x3E    /*!< Fuse poly fuses with data from the RAM. */

/**@}*/


/*! \name AS3911 FIFO parameters */
/**@{*/
/*! \ingroup as3911 */

/*! AS3911 FIFO size (bytes). */
#define AS3911_FIFO_SIZE                96
/*! AS3911 water level interrupt transmit water level if IO configuration register 1 bit fifo_lt is zero. */
#define AS3911_FIFO_TRANSMIT_WL0        32
/*! AS3911 water level interrupt transmit water level if IO configuration register 1 bit fifo_lt is one. */
#define AS3911_FIFO_TRANSMIT_WL1        16
/*! AS3911 water level interrupt receive water level if IO configuration register 1 bit fifo_lr is zero. */
#define AS3911_FIFO_RECEIVE_WL0         64
/*! AS3911 water level interrupt receive water level if IO configuration register 1 bit fifo_lr is one. */
#define AS3911_FIFO_RECEIVE_WL1         80

/**@}*/

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

#endif /* AS3911_DEF_H */
