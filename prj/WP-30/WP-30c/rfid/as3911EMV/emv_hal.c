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
 * PROJECT: AS911 firmware
 * $Revision: $
 * LANGUAGE: ANSI C
 */

/*! \file emv_hal.c
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV module RFID hardware abstraction layer.
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

/*! AS3911 operation control register transmit enable bit mask. */
#define EMV_HAL_REG_OPCONTROL_TXEN_BIT    0x08

/*!
 *****************************************************************************
 * Receiver dead time (in carrier cycles) for the ISO14443A protocol.
 * See [CCP v2.01, PCD 4.8.1.3].
 * Calculation:
 *
 * 1108: Deaftime if the last transmitted data bis was logic '1'
 *
 *  276: Time from the rising pulse of the pause of the logic '1'
 *       (i.e. the timepoint to measure the deaftime from), to the
 *       actual end of the EOF sequence (the point where the MRT starts).
 *       Please note that the AS3911 uses the ISO14443-2 definition
 *       where the EOF consists of logic '0' followed by sequence Y.
 *
 *   64: Adjustment for the MRT timer jitter. Currently the MRT timer
 *       will have any timeout between the set timeout and the set timout
 *       + 64 cycles.
 *****************************************************************************
 */
#define EMV_HAL_ISO14443A_RECEIVER_DEADTIME     (1108 - 276 - 64)

/*!
 *****************************************************************************
 * Receiver dead time (in carrier cycles) for the ISO14443B protocol.
 * See [CCP v2.01, PCD 4.8.1.3].
 *
 * 1024: TR0_MIN
 *
 *  340: Time from the rising edge of the EoS to the starting point
 *       of the MRT timer (sometime after the final high part of the
 *       EoS is completed).
 *
 *   64: Adjustment for the MRT timer jitter. Currently the MRT timer
 *       will have any timeout between the set timeout and the set timout
 *       + 64 cycles.
 *****************************************************************************
 */
#define EMV_HAL_ISO14443B_RECEIVER_DEADTIME     (1024 - 340 - 64)

/*!
 *****************************************************************************
 * Adjustment from the timeout requested by the caller to the timeout
 * value which has to be used for the AS3911.
 *****************************************************************************
 */
#define EMV_HAL_TYPE_A_TIMEOUT_ADJUSTMENT    512

/*!
 *****************************************************************************
 * Adjustment from the timeout requested by the caller to the timeout
 * value which has to be used for the AS3911.
 *****************************************************************************
 */
#define EMV_HAL_TYPE_B_TIMEOUT_ADJUSTMENT    6360

/*
******************************************************************************
* MACROS
******************************************************************************
*/

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

/*! Stores the currently active standard. */
static EmvHalStandard_t emvioActiveStandard;

static AS3911ModulationLevelMode_t emvHalTypeBModulationLevelMode = AS3911_MODULATION_LEVEL_FIXED;
static const void *emvHalTypeBModulationLevelModeData = NULL;
EmvHalTransceiveMode_t gtransceiveMode;
/**@}*/

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

/*!
 *****************************************************************************
 * \brief Convert AS3911 module error codes to EMV IO module error codes.
 *
 * \param as3911errorCode AS3911 module error code.
 * \return EMV IO module error code.
 *****************************************************************************
 */
static s16 emvioConvertErrorCode(s16 as3911ErrorCode);

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
void sleepMilliseconds(unsigned int milliseconds)
{
    //timerStart(milliseconds);
    //while(timerIsRunning())
    //    ;
    //s_BlockDelayMs(milliseconds);
//    sys_delay_ms(milliseconds);
    s_DelayMs(milliseconds);
}

void emvHalSleepMilliseconds(u16 milliseconds)
{
    sleepMilliseconds(milliseconds);
//    s_DelayMs(milliseconds);
//    s_BlockDelayMs(milliseconds);
}

s8 emvHalSetStandard(EmvHalStandard_t standard)
{
    if (EMV_HAL_TYPE_A == standard)
    {
        emvioActiveStandard = standard;

        /* Set the AS3911 to ISO14443-A, 106kBit/s rx/tx datarate. */
        as3911WriteRegister(AS3911_REG_MODE, 0x08);
        as3911WriteRegister(AS3911_REG_BIT_RATE, 0x00);

        //type a set ------chenf 20130301
//        as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONF, 0x80);
//        as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, 0x88);
//        as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, 0x00);

        /* Disable transmit without parity, disable receive without parity&crc, set
         * to default pulse width, and disable special anticollision mode.
         */
        as3911WriteRegister(AS3911_REG_ISO14443A_NFC, 0x00);


        /* OOK modulation, no tolerant processing of the first byte. */
        as3911WriteRegister(AS3911_REG_AUX, 0x00);
        //mifare 卡后恢复
        as3911ModifyRegister(AS3911_REG_NUM_TX_BYTES2, 0x07, 0);

        /* Set the receiver deadtime. */
        as3911SetReceiverDeadtime(EMV_HAL_ISO14443A_RECEIVER_DEADTIME);

        /* Set the frame delay time to the minimum allowed PCD frame delay time. */
        as3911SetFrameDelayTime(EMV_FDT_PCD_MIN);

        /* Disable dynamic adjustment of the modulation level. */
        as3911SetModulationLevelMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
        as3911WriteRegister(AS3911_REG_RX_CONF3, gtRfidProInfo.gTypeArec);
    }
    else if (EMV_HAL_TYPE_B == standard)
    {
        emvioActiveStandard = standard;

        /* Set the AS3911 to ISO14443-B, 106kBit/s rx/tx datarate. */
        as3911WriteRegister(AS3911_REG_MODE, 0x10);
        as3911WriteRegister(AS3911_REG_BIT_RATE, 0x00);

        //type b set ------chenf 20130301
//        as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONF, 0x80);
//        as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, 0x88);
//        as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, 0x00);
//        as3911ModifyRegister(AS3911_REG_AM_MOD_DEPTH_CONF, 0x7E, 0x1E);

        /* Configure AM modulation for ISO14443B. */
        as3911WriteRegister(AS3911_REG_AUX, 0x20);
        //mifare 卡后恢复
        as3911ModifyRegister(AS3911_REG_NUM_TX_BYTES2, 0x07, 0);

        /* Set the receiver deadtime. */
        as3911SetReceiverDeadtime(EMV_HAL_ISO14443B_RECEIVER_DEADTIME);

        /* Set the frame delay time to the minimum allowed PCD frame delay time. */
        as3911SetFrameDelayTime(EMV_FDT_PCD_MIN);

        /* Enable dynamic adjustment of the modulation level. */
        as3911SetModulationLevelMode(emvHalTypeBModulationLevelMode, emvHalTypeBModulationLevelModeData);
        as3911WriteRegister(AS3911_REG_RX_CONF3, gtRfidProInfo.gTypeBrec);
    }
    else
    {
        /* Handle unkown parameter error here. */
        return EMV_HAL_ERR_PARAM;
    }

    return EMV_HAL_ERR_OK;
}

s8 emvHalSetErrorHandling(EmvHalErrorHandling_t errorHandling)
{
    if (EMV_HAL_PREACTIVATION_ERROR_HANDLING == errorHandling)
    {
        /* Disable higher layer EMV exception processing. */
        as3911EnableEmvExceptionProcessing(OFF);
        as3911SetTransmissionErrorThreshold(0);

        /* Enable detailed anticollision detection. */
        // as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, 0x01, 0x01);
    }
    else if (EMV_HAL_LAYER4_ERROR_HANDLING == errorHandling)
    {
        /* Errornous frames <= 4 bytes in length shall not be considered
         * transmission errors.
         */
        as3911SetTransmissionErrorThreshold(4);
        as3911EnableEmvExceptionProcessing(ON);

        /* Disable detailed anticollision detction.
         * This autmatically enables the enhanced SOF detection.
         */
        // as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, 0x01, 0x00);
    }
    else
    {
        /* Handle unkown parameter error here. */
        return EMV_HAL_ERR_PARAM;
    }

    return EMV_HAL_ERR_OK;
}

s8 emvHalActivateField(uchar activateField)
{
    s8 error = ERR_NONE;

    if (activateField)
        error |= as3911ModifyRegister(AS3911_REG_OP_CONTROL, EMV_HAL_REG_OPCONTROL_TXEN_BIT, EMV_HAL_REG_OPCONTROL_TXEN_BIT);
    else
        error |= as3911ModifyRegister(AS3911_REG_OP_CONTROL, EMV_HAL_REG_OPCONTROL_TXEN_BIT, 0x00);

    if (ERR_NONE == error)
        return ERR_NONE;
    else
        return ERR_IO;
}

uchar emvHalFieldIsActivated()
{
    u8 regOpcontrol = 0;

    if (ERR_NONE != as3911ReadRegister(AS3911_REG_OP_CONTROL, &regOpcontrol))
        return OFF;

    if (regOpcontrol & EMV_HAL_REG_OPCONTROL_TXEN_BIT)
        return ON;
    else
        return OFF;
}

s8 emvHalResetField()
{
    s8 error = ERR_NONE;

    error |= emvHalActivateField(OFF);
    emvHalSleepMilliseconds(EMV_T_RESET);
    error |= emvHalActivateField(ON);

    if (ERR_NONE == error)
        return ERR_NONE;
    else
        return ERR_IO;
}

s8 emvHalTransceive(u8 *request, size_t requestLength, u8 *response
    , size_t maxResponseLength, size_t *responseLength, u32 timeout, EmvHalTransceiveMode_t transceiveMode)
{
    s16 error = AS3911_NO_ERROR;

    /* Adjust timeout by the delay between the start of card modulation
     * and the SoR interrupt of the AS3911.
     */
    D2(LABLE(0xEE);DATAIN((uchar)emvioActiveStandard);
       DATAIN((uchar)transceiveMode);
       );
    gtransceiveMode = transceiveMode;
    if (EMV_HAL_TYPE_A == emvioActiveStandard)
        timeout += EMV_HAL_TYPE_A_TIMEOUT_ADJUSTMENT;
    else if (EMV_HAL_TYPE_B == emvioActiveStandard)
        timeout += EMV_HAL_TYPE_B_TIMEOUT_ADJUSTMENT;

    if (EMV_HAL_TRANSCEIVE_WITH_CRC == transceiveMode)
    {
        error = as3911Transceive(request, requestLength, response, maxResponseLength,
                    responseLength, timeout, AS3911_CRC_TO_FIFO);
    }
    else if (EMV_HAL_TRANSCEIVE_WITHOUT_CRC == transceiveMode)
    {
        error = as3911Transceive(request, requestLength, response, maxResponseLength,
                    responseLength, timeout,
                    (AS3911RequestFlags_t)(AS3911_IGNORE_CRC | AS3911_TRANSMIT_WITHOUT_CRC));
    }
    else if (EMV_HAL_TRANSCEIVE_WUPA == transceiveMode)
    {
        error = as3911Transceive(NULL, 0, response, maxResponseLength,
                    responseLength, timeout,
                    AS3911_TRANSMIT_WUPA);

    }
    else
    {
        /* Handle unkown parameter error here. */
        return EMV_HAL_ERR_PARAM;
    }

    return emvioConvertErrorCode(error);
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

static s16 emvioConvertErrorCode(s16 as3911ErrorCode)
{
    if (AS3911_NO_ERROR == as3911ErrorCode)
        return EMV_HAL_ERR_OK;
    else if (AS3911_TIMEOUT_ERROR == as3911ErrorCode)
        return EMV_HAL_ERR_TIMEOUT;
    else if (AS3911_OVERFLOW_ERROR == as3911ErrorCode)
        return EMV_HAL_ERR_OVERFLOW;
    else if (AS3911_COLLISION_ERROR == as3911ErrorCode)
        return EMV_HAL_ERR_COLLISION;
    else
        return EMV_HAL_ERR_ENCODING;
}

uchar s_as3911errorCode(int error)
{
    if (EMV_HAL_ERR_OK == error)
        return EM_mifs_SUCCESS;
    else if (EMV_HAL_ERR_TIMEOUT == error)
        return EM_mifs_NOTAGERR;
    else if (EMV_HAL_ERR_OVERFLOW == error)
        return EM_mifs_OVFLERR;
    else if (EMV_HAL_ERR_COLLISION == error)
        return EM_mifs_COLLERR;
    else
        return EM_mifs_TRANSERR;
}

s8 emvHalSetAs3911TypeBModulationMode(AS3911ModulationLevelMode_t modulationLevelMode, const void *modulationLevelModeData)
{
	emvHalTypeBModulationLevelMode = modulationLevelMode;
	emvHalTypeBModulationLevelModeData = modulationLevelModeData;

	return ERR_NONE;
}

void s_as3911_SetInt(uint mode)
{
    s_Rfid_SetInt(mode);
#if 0
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_ClearITPendingBit(EXINT_MF);
	EXTI_InitStructure.EXTI_Line = EXINT_MF;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = (FunctionalState) mode;
	EXTI_Init(&EXTI_InitStructure);
#endif
}

#if 0
void s_as3911_spi_setbps(SPI_TypeDef *spi, uint bps)
{
 	SPI_InitTypeDef SPI_InitStructure;
	SPI_Cmd(spi, DISABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	--bps;
	SPI_InitStructure.SPI_BaudRatePrescaler = (u16)((bps & 0x07) << 3);  //SPI_BaudRatePrescaler_256;//
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(spi, &SPI_InitStructure);
	SPI_Cmd(spi, ENABLE);
}
#endif

void s_as3911_init(void)
{
    uint i;
//    char temp;

    EG_mifs_tWorkInfo.RFIDModule = RFID_Module_AS3911;
    s_Rfid_vHalInit();
    s_as3911_SetInt(OFF);
#if 0
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    spi_open(SPI_BUS_RFID,SPI_BPS_2250K,0);
    s_as3911_spi_setbps(SPI1, SPI_BPS_2250K);

//	//初始化RST
//	GPIO_InitStructure.GPIO_Pin = Pin_MF_RST;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//	GPIO_Init(Port_MF, &GPIO_InitStructure);
//	GPIO_SetBits(Port_MF, Pin_MF_RST);

	//初始化INT
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = Pin_MF_INT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(Port_MF, &GPIO_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRI_MF;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUBPRI_MF;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	EXTI_ClearITPendingBit(EXINT_MF);
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_InitStructure.EXTI_Line = EXINT_MF;
	EXTI_Init(&EXTI_InitStructure);
#else
//    SPI_ConfigureNPCS(RFID_SPIn, SPIBUS0_RFID,
//                SPI_CSBR_(MCK_CLK / (MCK_CLK/12)) |
//                SPI_DLYBCT_(0));//8.277M
//    set_pio_output(RFID_Reset,0);
//	enable_pio(RFID_INTERRUPT);   // USC_INT
//	set_pio_input(RFID_INTERRUPT,1);
//
//    Enable_SPI_Clk(0);
//    s_DelayMs(10);

#endif
    /* Reset the AS3911 */
    as3911ExecuteCommand(AS3911_CMD_SET_DEFAULT);

    /* MCU_CLK and LF MCU_CLK off, 27MHz XTAL */
    as3911WriteRegister(AS3911_REG_IO_CONF1, 0x0F);
//    as3911WriteRegister(AS3911_REG_IO_CONF2, 0x80);
    /* Enable Oscillator, Transmitter and receiver. */
//    as3911WriteRegister(AS3911_REG_OP_CONTROL, 0xC8);
    as3911WriteRegister(AS3911_REG_OP_CONTROL, 0xD8);

    s_DelayMs(10);

#if 0
    //等晶振稳定再开启中断
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);
	EXTI_ClearITPendingBit(EXINT_MF);
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
#endif
    //clear interrupt
    s_as3911ReadRegister(3, AS3911_REG_IRQ_MAIN, (uchar*)&i);

    //type b 设置调制深度恢复默认 各个调试表恢复默认
    as3911SetModulationLevelMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
    emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_FIXED, NULL);
    as3911SetGainMode(AS3911_GAIN_FIXED, NULL);

    as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, 0x00); //根据硬件调节0x9F
    as3911WriteRegister(AS3911_REG_ANT_CAL_CONF, 0x80); //0x21 设置最大
    as3911ModifyRegister(AS3911_REG_RX_CONF3, 0xE0, 0x00);

    s_as3911_SetInt(ON);
}

#endif

