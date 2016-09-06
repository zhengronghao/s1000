/***************** (h) COPYRIGHT 2015 START Computer equipment *****************
 * File Name          : peripheral.h
 * bfief              : 
 * Author             : luocs 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 12/28/2015 2:27:52 PM
 * Description        : 
 *******************************************************************************/
#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__ 

#include "fsl_bitaccess.h"


/*
 * MK21FN1M0VMC10 CRC
 *
 * Cyclic Redundancy Check
 *
 * Registers defined in this header file:
 * - CRC_DATAL - CRC_DATAL register.
 * - CRC_DATAH - CRC_DATAH register.
 * - CRC_DATALL - CRC_DATALL register.
 * - CRC_DATALU - CRC_DATALU register.
 * - CRC_DATAHL - CRC_DATAHL register.
 * - CRC_DATAHU - CRC_DATAHU register.
 * - CRC_DATA - CRC Data register
 * - CRC_GPOLY - CRC Polynomial register
 * - CRC_GPOLYL - CRC_GPOLYL register.
 * - CRC_GPOLYH - CRC_GPOLYH register.
 * - CRC_GPOLYLL - CRC_GPOLYLL register.
 * - CRC_GPOLYLU - CRC_GPOLYLU register.
 * - CRC_GPOLYHL - CRC_GPOLYHL register.
 * - CRC_GPOLYHU - CRC_GPOLYHU register.
 * - CRC_CTRL - CRC Control register
 * - CRC_CTRLHU - CRC_CTRLHU register.
 */

#define CRC_INSTANCE_COUNT (1U) /*!< Number of instances of the CRC module. */
#define CRC_IDX (0U) /*!< Instance number for CRC. */

/*******************************************************************************
 * CRC_DATALL - CRC_DATALL register.
 ******************************************************************************/

/*!
 * @brief CRC_DATALL - CRC_DATALL register. (RW)
 *
 * Reset value: 0xFFU
 */
/*!
 * @name Constants and macros for entire CRC_DATALL register
 */
/*@{*/
#define CRC_RD_DATALL(base)      (CRC_DATALL_REG(base))
#define CRC_WR_DATALL(base, value) (CRC_DATALL_REG(base) = (value))
#define CRC_RMW_DATALL(base, mask, value) (CRC_WR_DATALL(base, (CRC_RD_DATALL(base) & ~(mask)) | (value)))
#define CRC_SET_DATALL(base, value) (BME_OR8(&CRC_DATALL_REG(base), (uint8_t)(value)))
#define CRC_CLR_DATALL(base, value) (BME_AND8(&CRC_DATALL_REG(base), (uint8_t)(~(value))))
#define CRC_TOG_DATALL(base, value) (BME_XOR8(&CRC_DATALL_REG(base), (uint8_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_DATAL - CRC_DATAL register.
 ******************************************************************************/

/*!
 * @brief CRC_DATAL - CRC_DATAL register. (RW)
 *
 * Reset value: 0xFFFFU
 */
/*!
 * @name Constants and macros for entire CRC_DATAL register
 */
/*@{*/
#define CRC_RD_DATAL(base)       (CRC_DATAL_REG(base))
#define CRC_WR_DATAL(base, value) (CRC_DATAL_REG(base) = (value))
#define CRC_RMW_DATAL(base, mask, value) (CRC_WR_DATAL(base, (CRC_RD_DATAL(base) & ~(mask)) | (value)))
#define CRC_SET_DATAL(base, value) (BME_OR16(&CRC_DATAL_REG(base), (uint16_t)(value)))
#define CRC_CLR_DATAL(base, value) (BME_AND16(&CRC_DATAL_REG(base), (uint16_t)(~(value))))
#define CRC_TOG_DATAL(base, value) (BME_XOR16(&CRC_DATAL_REG(base), (uint16_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_DATA - CRC Data register
 ******************************************************************************/

/*!
 * @brief CRC_DATA - CRC Data register (RW)
 *
 * Reset value: 0xFFFFFFFFU
 *
 * The CRC Data register contains the value of the seed, data, and checksum.
 * When CTRL[WAS] is set, any write to the data register is regarded as the seed
 * value. When CTRL[WAS] is cleared, any write to the data register is regarded as
 * data for general CRC computation. In 16-bit CRC mode, the HU and HL fields are
 * not used for programming the seed value, and reads of these fields return an
 * indeterminate value. In 32-bit CRC mode, all fields are used for programming
 * the seed value. When programming data values, the values can be written 8 bits,
 * 16 bits, or 32 bits at a time, provided all bytes are contiguous; with MSB of
 * data value written first. After all data values are written, the CRC result
 * can be read from this data register. In 16-bit CRC mode, the CRC result is
 * available in the LU and LL fields. In 32-bit CRC mode, all fields contain the
 * result. Reads of this register at any time return the intermediate CRC value,
 * provided the CRC module is configured.
 */
/*!
 * @name Constants and macros for entire CRC_DATA register
 */
/*@{*/
#define CRC_RD_DATA(base)        (CRC_DATA_REG(base))
#define CRC_WR_DATA(base, value) (CRC_DATA_REG(base) = (value))
#define CRC_RMW_DATA(base, mask, value) (CRC_WR_DATA(base, (CRC_RD_DATA(base) & ~(mask)) | (value)))
#define CRC_SET_DATA(base, value) (BME_OR32(&CRC_DATA_REG(base), (uint32_t)(value)))
#define CRC_CLR_DATA(base, value) (BME_AND32(&CRC_DATA_REG(base), (uint32_t)(~(value))))
#define CRC_TOG_DATA(base, value) (BME_XOR32(&CRC_DATA_REG(base), (uint32_t)(value)))
/*@}*/

/*
 * Constants & macros for individual CRC_DATA bitfields
 */

/*!
 * @name Register CRC_DATA, field LL[7:0] (RW)
 *
 * When CTRL[WAS] is 1, values written to this field are part of the seed value.
 * When CTRL[WAS] is 0, data written to this field is used for CRC checksum
 * generation.
 */
/*@{*/
/*! @brief Read current value of the CRC_DATA_LL field. */
#define CRC_RD_DATA_LL(base) ((CRC_DATA_REG(base) & CRC_DATA_LL_MASK) >> CRC_DATA_LL_SHIFT)
#define CRC_BRD_DATA_LL(base) (BME_UBFX32(&CRC_DATA_REG(base), CRC_DATA_LL_SHIFT, CRC_DATA_LL_WIDTH))

/*! @brief Set the LL field to a new value. */
#define CRC_WR_DATA_LL(base, value) (CRC_RMW_DATA(base, CRC_DATA_LL_MASK, CRC_DATA_LL(value)))
#define CRC_BWR_DATA_LL(base, value) (BME_BFI32(&CRC_DATA_REG(base), ((uint32_t)(value) << CRC_DATA_LL_SHIFT), CRC_DATA_LL_SHIFT, CRC_DATA_LL_WIDTH))
/*@}*/

/*!
 * @name Register CRC_DATA, field LU[15:8] (RW)
 *
 * When CTRL[WAS] is 1, values written to this field are part of the seed value.
 * When CTRL[WAS] is 0, data written to this field is used for CRC checksum
 * generation.
 */
/*@{*/
/*! @brief Read current value of the CRC_DATA_LU field. */
#define CRC_RD_DATA_LU(base) ((CRC_DATA_REG(base) & CRC_DATA_LU_MASK) >> CRC_DATA_LU_SHIFT)
#define CRC_BRD_DATA_LU(base) (BME_UBFX32(&CRC_DATA_REG(base), CRC_DATA_LU_SHIFT, CRC_DATA_LU_WIDTH))

/*! @brief Set the LU field to a new value. */
#define CRC_WR_DATA_LU(base, value) (CRC_RMW_DATA(base, CRC_DATA_LU_MASK, CRC_DATA_LU(value)))
#define CRC_BWR_DATA_LU(base, value) (BME_BFI32(&CRC_DATA_REG(base), ((uint32_t)(value) << CRC_DATA_LU_SHIFT), CRC_DATA_LU_SHIFT, CRC_DATA_LU_WIDTH))
/*@}*/

/*!
 * @name Register CRC_DATA, field HL[23:16] (RW)
 *
 * In 16-bit CRC mode (CTRL[TCRC] is 0), this field is not used for programming
 * a seed value. In 32-bit CRC mode (CTRL[TCRC] is 1), values written to this
 * field are part of the seed value when CTRL[WAS] is 1. When CTRL[WAS] is 0, data
 * written to this field is used for CRC checksum generation in both 16-bit and
 * 32-bit CRC modes.
 */
/*@{*/
/*! @brief Read current value of the CRC_DATA_HL field. */
#define CRC_RD_DATA_HL(base) ((CRC_DATA_REG(base) & CRC_DATA_HL_MASK) >> CRC_DATA_HL_SHIFT)
#define CRC_BRD_DATA_HL(base) (BME_UBFX32(&CRC_DATA_REG(base), CRC_DATA_HL_SHIFT, CRC_DATA_HL_WIDTH))

/*! @brief Set the HL field to a new value. */
#define CRC_WR_DATA_HL(base, value) (CRC_RMW_DATA(base, CRC_DATA_HL_MASK, CRC_DATA_HL(value)))
#define CRC_BWR_DATA_HL(base, value) (BME_BFI32(&CRC_DATA_REG(base), ((uint32_t)(value) << CRC_DATA_HL_SHIFT), CRC_DATA_HL_SHIFT, CRC_DATA_HL_WIDTH))
/*@}*/

/*!
 * @name Register CRC_DATA, field HU[31:24] (RW)
 *
 * In 16-bit CRC mode (CTRL[TCRC] is 0), this field is not used for programming
 * a seed value. In 32-bit CRC mode (CTRL[TCRC] is 1), values written to this
 * field are part of the seed value when CTRL[WAS] is 1. When CTRL[WAS] is 0, data
 * written to this field is used for CRC checksum generation in both 16-bit and
 * 32-bit CRC modes.
 */
/*@{*/
/*! @brief Read current value of the CRC_DATA_HU field. */
#define CRC_RD_DATA_HU(base) ((CRC_DATA_REG(base) & CRC_DATA_HU_MASK) >> CRC_DATA_HU_SHIFT)
#define CRC_BRD_DATA_HU(base) (BME_UBFX32(&CRC_DATA_REG(base), CRC_DATA_HU_SHIFT, CRC_DATA_HU_WIDTH))

/*! @brief Set the HU field to a new value. */
#define CRC_WR_DATA_HU(base, value) (CRC_RMW_DATA(base, CRC_DATA_HU_MASK, CRC_DATA_HU(value)))
#define CRC_BWR_DATA_HU(base, value) (BME_BFI32(&CRC_DATA_REG(base), ((uint32_t)(value) << CRC_DATA_HU_SHIFT), CRC_DATA_HU_SHIFT, CRC_DATA_HU_WIDTH))
/*@}*/

/*******************************************************************************
 * CRC_DATALU - CRC_DATALU register.
 ******************************************************************************/

/*!
 * @brief CRC_DATALU - CRC_DATALU register. (RW)
 *
 * Reset value: 0xFFU
 */
/*!
 * @name Constants and macros for entire CRC_DATALU register
 */
/*@{*/
#define CRC_RD_DATALU(base)      (CRC_DATALU_REG(base))
#define CRC_WR_DATALU(base, value) (CRC_DATALU_REG(base) = (value))
#define CRC_RMW_DATALU(base, mask, value) (CRC_WR_DATALU(base, (CRC_RD_DATALU(base) & ~(mask)) | (value)))
#define CRC_SET_DATALU(base, value) (BME_OR8(&CRC_DATALU_REG(base), (uint8_t)(value)))
#define CRC_CLR_DATALU(base, value) (BME_AND8(&CRC_DATALU_REG(base), (uint8_t)(~(value))))
#define CRC_TOG_DATALU(base, value) (BME_XOR8(&CRC_DATALU_REG(base), (uint8_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_DATAHL - CRC_DATAHL register.
 ******************************************************************************/

/*!
 * @brief CRC_DATAHL - CRC_DATAHL register. (RW)
 *
 * Reset value: 0xFFU
 */
/*!
 * @name Constants and macros for entire CRC_DATAHL register
 */
/*@{*/
#define CRC_RD_DATAHL(base)      (CRC_DATAHL_REG(base))
#define CRC_WR_DATAHL(base, value) (CRC_DATAHL_REG(base) = (value))
#define CRC_RMW_DATAHL(base, mask, value) (CRC_WR_DATAHL(base, (CRC_RD_DATAHL(base) & ~(mask)) | (value)))
#define CRC_SET_DATAHL(base, value) (BME_OR8(&CRC_DATAHL_REG(base), (uint8_t)(value)))
#define CRC_CLR_DATAHL(base, value) (BME_AND8(&CRC_DATAHL_REG(base), (uint8_t)(~(value))))
#define CRC_TOG_DATAHL(base, value) (BME_XOR8(&CRC_DATAHL_REG(base), (uint8_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_DATAH - CRC_DATAH register.
 ******************************************************************************/

/*!
 * @brief CRC_DATAH - CRC_DATAH register. (RW)
 *
 * Reset value: 0xFFFFU
 */
/*!
 * @name Constants and macros for entire CRC_DATAH register
 */
/*@{*/
#define CRC_RD_DATAH(base)       (CRC_DATAH_REG(base))
#define CRC_WR_DATAH(base, value) (CRC_DATAH_REG(base) = (value))
#define CRC_RMW_DATAH(base, mask, value) (CRC_WR_DATAH(base, (CRC_RD_DATAH(base) & ~(mask)) | (value)))
#define CRC_SET_DATAH(base, value) (BME_OR16(&CRC_DATAH_REG(base), (uint16_t)(value)))
#define CRC_CLR_DATAH(base, value) (BME_AND16(&CRC_DATAH_REG(base), (uint16_t)(~(value))))
#define CRC_TOG_DATAH(base, value) (BME_XOR16(&CRC_DATAH_REG(base), (uint16_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_DATAHU - CRC_DATAHU register.
 ******************************************************************************/

/*!
 * @brief CRC_DATAHU - CRC_DATAHU register. (RW)
 *
 * Reset value: 0xFFU
 */
/*!
 * @name Constants and macros for entire CRC_DATAHU register
 */
/*@{*/
#define CRC_RD_DATAHU(base)      (CRC_DATAHU_REG(base))
#define CRC_WR_DATAHU(base, value) (CRC_DATAHU_REG(base) = (value))
#define CRC_RMW_DATAHU(base, mask, value) (CRC_WR_DATAHU(base, (CRC_RD_DATAHU(base) & ~(mask)) | (value)))
#define CRC_SET_DATAHU(base, value) (BME_OR8(&CRC_DATAHU_REG(base), (uint8_t)(value)))
#define CRC_CLR_DATAHU(base, value) (BME_AND8(&CRC_DATAHU_REG(base), (uint8_t)(~(value))))
#define CRC_TOG_DATAHU(base, value) (BME_XOR8(&CRC_DATAHU_REG(base), (uint8_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_GPOLYLL - CRC_GPOLYLL register.
 ******************************************************************************/

/*!
 * @brief CRC_GPOLYLL - CRC_GPOLYLL register. (RW)
 *
 * Reset value: 0xFFU
 */
/*!
 * @name Constants and macros for entire CRC_GPOLYLL register
 */
/*@{*/
#define CRC_RD_GPOLYLL(base)     (CRC_GPOLYLL_REG(base))
#define CRC_WR_GPOLYLL(base, value) (CRC_GPOLYLL_REG(base) = (value))
#define CRC_RMW_GPOLYLL(base, mask, value) (CRC_WR_GPOLYLL(base, (CRC_RD_GPOLYLL(base) & ~(mask)) | (value)))
#define CRC_SET_GPOLYLL(base, value) (BME_OR8(&CRC_GPOLYLL_REG(base), (uint8_t)(value)))
#define CRC_CLR_GPOLYLL(base, value) (BME_AND8(&CRC_GPOLYLL_REG(base), (uint8_t)(~(value))))
#define CRC_TOG_GPOLYLL(base, value) (BME_XOR8(&CRC_GPOLYLL_REG(base), (uint8_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_GPOLY - CRC Polynomial register
 ******************************************************************************/

/*!
 * @brief CRC_GPOLY - CRC Polynomial register (RW)
 *
 * Reset value: 0x00001021U
 *
 * This register contains the value of the polynomial for the CRC calculation.
 * The HIGH field contains the upper 16 bits of the CRC polynomial, which are used
 * only in 32-bit CRC mode. Writes to the HIGH field are ignored in 16-bit CRC
 * mode. The LOW field contains the lower 16 bits of the CRC polynomial, which are
 * used in both 16- and 32-bit CRC modes.
 */
/*!
 * @name Constants and macros for entire CRC_GPOLY register
 */
/*@{*/
#define CRC_RD_GPOLY(base)       (CRC_GPOLY_REG(base))
#define CRC_WR_GPOLY(base, value) (CRC_GPOLY_REG(base) = (value))
#define CRC_RMW_GPOLY(base, mask, value) (CRC_WR_GPOLY(base, (CRC_RD_GPOLY(base) & ~(mask)) | (value)))
#define CRC_SET_GPOLY(base, value) (BME_OR32(&CRC_GPOLY_REG(base), (uint32_t)(value)))
#define CRC_CLR_GPOLY(base, value) (BME_AND32(&CRC_GPOLY_REG(base), (uint32_t)(~(value))))
#define CRC_TOG_GPOLY(base, value) (BME_XOR32(&CRC_GPOLY_REG(base), (uint32_t)(value)))
/*@}*/

/*
 * Constants & macros for individual CRC_GPOLY bitfields
 */

/*!
 * @name Register CRC_GPOLY, field LOW[15:0] (RW)
 *
 * Writable and readable in both 32-bit and 16-bit CRC modes.
 */
/*@{*/
/*! @brief Read current value of the CRC_GPOLY_LOW field. */
#define CRC_RD_GPOLY_LOW(base) ((CRC_GPOLY_REG(base) & CRC_GPOLY_LOW_MASK) >> CRC_GPOLY_LOW_SHIFT)
#define CRC_BRD_GPOLY_LOW(base) (CRC_RD_GPOLY_LOW(base))

/*! @brief Set the LOW field to a new value. */
#define CRC_WR_GPOLY_LOW(base, value) (CRC_RMW_GPOLY(base, CRC_GPOLY_LOW_MASK, CRC_GPOLY_LOW(value)))
#define CRC_BWR_GPOLY_LOW(base, value) (CRC_WR_GPOLY_LOW(base, value))
/*@}*/

/*!
 * @name Register CRC_GPOLY, field HIGH[31:16] (RW)
 *
 * Writable and readable in 32-bit CRC mode (CTRL[TCRC] is 1). This field is not
 * writable in 16-bit CRC mode (CTRL[TCRC] is 0).
 */
/*@{*/
/*! @brief Read current value of the CRC_GPOLY_HIGH field. */
#define CRC_RD_GPOLY_HIGH(base) ((CRC_GPOLY_REG(base) & CRC_GPOLY_HIGH_MASK) >> CRC_GPOLY_HIGH_SHIFT)
#define CRC_BRD_GPOLY_HIGH(base) (CRC_RD_GPOLY_HIGH(base))

/*! @brief Set the HIGH field to a new value. */
#define CRC_WR_GPOLY_HIGH(base, value) (CRC_RMW_GPOLY(base, CRC_GPOLY_HIGH_MASK, CRC_GPOLY_HIGH(value)))
#define CRC_BWR_GPOLY_HIGH(base, value) (CRC_WR_GPOLY_HIGH(base, value))
/*@}*/

/*******************************************************************************
 * CRC_GPOLYL - CRC_GPOLYL register.
 ******************************************************************************/

/*!
 * @brief CRC_GPOLYL - CRC_GPOLYL register. (RW)
 *
 * Reset value: 0xFFFFU
 */
/*!
 * @name Constants and macros for entire CRC_GPOLYL register
 */
/*@{*/
#define CRC_RD_GPOLYL(base)      (CRC_GPOLYL_REG(base))
#define CRC_WR_GPOLYL(base, value) (CRC_GPOLYL_REG(base) = (value))
#define CRC_RMW_GPOLYL(base, mask, value) (CRC_WR_GPOLYL(base, (CRC_RD_GPOLYL(base) & ~(mask)) | (value)))
#define CRC_SET_GPOLYL(base, value) (BME_OR16(&CRC_GPOLYL_REG(base), (uint16_t)(value)))
#define CRC_CLR_GPOLYL(base, value) (BME_AND16(&CRC_GPOLYL_REG(base), (uint16_t)(~(value))))
#define CRC_TOG_GPOLYL(base, value) (BME_XOR16(&CRC_GPOLYL_REG(base), (uint16_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_GPOLYLU - CRC_GPOLYLU register.
 ******************************************************************************/

/*!
 * @brief CRC_GPOLYLU - CRC_GPOLYLU register. (RW)
 *
 * Reset value: 0xFFU
 */
/*!
 * @name Constants and macros for entire CRC_GPOLYLU register
 */
/*@{*/
#define CRC_RD_GPOLYLU(base)     (CRC_GPOLYLU_REG(base))
#define CRC_WR_GPOLYLU(base, value) (CRC_GPOLYLU_REG(base) = (value))
#define CRC_RMW_GPOLYLU(base, mask, value) (CRC_WR_GPOLYLU(base, (CRC_RD_GPOLYLU(base) & ~(mask)) | (value)))
#define CRC_SET_GPOLYLU(base, value) (BME_OR8(&CRC_GPOLYLU_REG(base), (uint8_t)(value)))
#define CRC_CLR_GPOLYLU(base, value) (BME_AND8(&CRC_GPOLYLU_REG(base), (uint8_t)(~(value))))
#define CRC_TOG_GPOLYLU(base, value) (BME_XOR8(&CRC_GPOLYLU_REG(base), (uint8_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_GPOLYH - CRC_GPOLYH register.
 ******************************************************************************/

/*!
 * @brief CRC_GPOLYH - CRC_GPOLYH register. (RW)
 *
 * Reset value: 0xFFFFU
 */
/*!
 * @name Constants and macros for entire CRC_GPOLYH register
 */
/*@{*/
#define CRC_RD_GPOLYH(base)      (CRC_GPOLYH_REG(base))
#define CRC_WR_GPOLYH(base, value) (CRC_GPOLYH_REG(base) = (value))
#define CRC_RMW_GPOLYH(base, mask, value) (CRC_WR_GPOLYH(base, (CRC_RD_GPOLYH(base) & ~(mask)) | (value)))
#define CRC_SET_GPOLYH(base, value) (BME_OR16(&CRC_GPOLYH_REG(base), (uint16_t)(value)))
#define CRC_CLR_GPOLYH(base, value) (BME_AND16(&CRC_GPOLYH_REG(base), (uint16_t)(~(value))))
#define CRC_TOG_GPOLYH(base, value) (BME_XOR16(&CRC_GPOLYH_REG(base), (uint16_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_GPOLYHL - CRC_GPOLYHL register.
 ******************************************************************************/

/*!
 * @brief CRC_GPOLYHL - CRC_GPOLYHL register. (RW)
 *
 * Reset value: 0xFFU
 */
/*!
 * @name Constants and macros for entire CRC_GPOLYHL register
 */
/*@{*/
#define CRC_RD_GPOLYHL(base)     (CRC_GPOLYHL_REG(base))
#define CRC_WR_GPOLYHL(base, value) (CRC_GPOLYHL_REG(base) = (value))
#define CRC_RMW_GPOLYHL(base, mask, value) (CRC_WR_GPOLYHL(base, (CRC_RD_GPOLYHL(base) & ~(mask)) | (value)))
#define CRC_SET_GPOLYHL(base, value) (BME_OR8(&CRC_GPOLYHL_REG(base), (uint8_t)(value)))
#define CRC_CLR_GPOLYHL(base, value) (BME_AND8(&CRC_GPOLYHL_REG(base), (uint8_t)(~(value))))
#define CRC_TOG_GPOLYHL(base, value) (BME_XOR8(&CRC_GPOLYHL_REG(base), (uint8_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_GPOLYHU - CRC_GPOLYHU register.
 ******************************************************************************/

/*!
 * @brief CRC_GPOLYHU - CRC_GPOLYHU register. (RW)
 *
 * Reset value: 0xFFU
 */
/*!
 * @name Constants and macros for entire CRC_GPOLYHU register
 */
/*@{*/
#define CRC_RD_GPOLYHU(base)     (CRC_GPOLYHU_REG(base))
#define CRC_WR_GPOLYHU(base, value) (CRC_GPOLYHU_REG(base) = (value))
#define CRC_RMW_GPOLYHU(base, mask, value) (CRC_WR_GPOLYHU(base, (CRC_RD_GPOLYHU(base) & ~(mask)) | (value)))
#define CRC_SET_GPOLYHU(base, value) (BME_OR8(&CRC_GPOLYHU_REG(base), (uint8_t)(value)))
#define CRC_CLR_GPOLYHU(base, value) (BME_AND8(&CRC_GPOLYHU_REG(base), (uint8_t)(~(value))))
#define CRC_TOG_GPOLYHU(base, value) (BME_XOR8(&CRC_GPOLYHU_REG(base), (uint8_t)(value)))
/*@}*/

/*******************************************************************************
 * CRC_CTRL - CRC Control register
 ******************************************************************************/

/*!
 * @brief CRC_CTRL - CRC Control register (RW)
 *
 * Reset value: 0x00000000U
 *
 * This register controls the configuration and working of the CRC module.
 * Appropriate bits must be set before starting a new CRC calculation. A new CRC
 * calculation is initialized by asserting CTRL[WAS] and then writing the seed into
 * the CRC data register.
 */
/*!
 * @name Constants and macros for entire CRC_CTRL register
 */
/*@{*/
#define CRC_RD_CTRL(base)        (CRC_CTRL_REG(base))
#define CRC_WR_CTRL(base, value) (CRC_CTRL_REG(base) = (value))
#define CRC_RMW_CTRL(base, mask, value) (CRC_WR_CTRL(base, (CRC_RD_CTRL(base) & ~(mask)) | (value)))
#define CRC_SET_CTRL(base, value) (BME_OR32(&CRC_CTRL_REG(base), (uint32_t)(value)))
#define CRC_CLR_CTRL(base, value) (BME_AND32(&CRC_CTRL_REG(base), (uint32_t)(~(value))))
#define CRC_TOG_CTRL(base, value) (BME_XOR32(&CRC_CTRL_REG(base), (uint32_t)(value)))
/*@}*/

/*
 * Constants & macros for individual CRC_CTRL bitfields
 */

/*!
 * @name Register CRC_CTRL, field TCRC[24] (RW)
 *
 * Width of CRC protocol.
 *
 * Values:
 * - 0b0 - 16-bit CRC protocol.
 * - 0b1 - 32-bit CRC protocol.
 */
/*@{*/
/*! @brief Read current value of the CRC_CTRL_TCRC field. */
#define CRC_RD_CTRL_TCRC(base) ((CRC_CTRL_REG(base) & CRC_CTRL_TCRC_MASK) >> CRC_CTRL_TCRC_SHIFT)
#define CRC_BRD_CTRL_TCRC(base) (BME_UBFX32(&CRC_CTRL_REG(base), CRC_CTRL_TCRC_SHIFT, CRC_CTRL_TCRC_WIDTH))

/*! @brief Set the TCRC field to a new value. */
#define CRC_WR_CTRL_TCRC(base, value) (CRC_RMW_CTRL(base, CRC_CTRL_TCRC_MASK, CRC_CTRL_TCRC(value)))
#define CRC_BWR_CTRL_TCRC(base, value) (BME_BFI32(&CRC_CTRL_REG(base), ((uint32_t)(value) << CRC_CTRL_TCRC_SHIFT), CRC_CTRL_TCRC_SHIFT, CRC_CTRL_TCRC_WIDTH))
/*@}*/

/*!
 * @name Register CRC_CTRL, field WAS[25] (RW)
 *
 * When asserted, a value written to the CRC data register is considered a seed
 * value. When deasserted, a value written to the CRC data register is taken as
 * data for CRC computation.
 *
 * Values:
 * - 0b0 - Writes to the CRC data register are data values.
 * - 0b1 - Writes to the CRC data register are seed values.
 */
/*@{*/
/*! @brief Read current value of the CRC_CTRL_WAS field. */
#define CRC_RD_CTRL_WAS(base) ((CRC_CTRL_REG(base) & CRC_CTRL_WAS_MASK) >> CRC_CTRL_WAS_SHIFT)
#define CRC_BRD_CTRL_WAS(base) (BME_UBFX32(&CRC_CTRL_REG(base), CRC_CTRL_WAS_SHIFT, CRC_CTRL_WAS_WIDTH))

/*! @brief Set the WAS field to a new value. */
#define CRC_WR_CTRL_WAS(base, value) (CRC_RMW_CTRL(base, CRC_CTRL_WAS_MASK, CRC_CTRL_WAS(value)))
#define CRC_BWR_CTRL_WAS(base, value) (BME_BFI32(&CRC_CTRL_REG(base), ((uint32_t)(value) << CRC_CTRL_WAS_SHIFT), CRC_CTRL_WAS_SHIFT, CRC_CTRL_WAS_WIDTH))
/*@}*/

/*!
 * @name Register CRC_CTRL, field FXOR[26] (RW)
 *
 * Some CRC protocols require the final checksum to be XORed with 0xFFFFFFFF or
 * 0xFFFF. Asserting this bit enables on the fly complementing of read data.
 *
 * Values:
 * - 0b0 - No XOR on reading.
 * - 0b1 - Invert or complement the read value of the CRC Data register.
 */
/*@{*/
/*! @brief Read current value of the CRC_CTRL_FXOR field. */
#define CRC_RD_CTRL_FXOR(base) ((CRC_CTRL_REG(base) & CRC_CTRL_FXOR_MASK) >> CRC_CTRL_FXOR_SHIFT)
#define CRC_BRD_CTRL_FXOR(base) (BME_UBFX32(&CRC_CTRL_REG(base), CRC_CTRL_FXOR_SHIFT, CRC_CTRL_FXOR_WIDTH))

/*! @brief Set the FXOR field to a new value. */
#define CRC_WR_CTRL_FXOR(base, value) (CRC_RMW_CTRL(base, CRC_CTRL_FXOR_MASK, CRC_CTRL_FXOR(value)))
#define CRC_BWR_CTRL_FXOR(base, value) (BME_BFI32(&CRC_CTRL_REG(base), ((uint32_t)(value) << CRC_CTRL_FXOR_SHIFT), CRC_CTRL_FXOR_SHIFT, CRC_CTRL_FXOR_WIDTH))
/*@}*/

/*!
 * @name Register CRC_CTRL, field TOTR[29:28] (RW)
 *
 * Identifies the transpose configuration of the value read from the CRC Data
 * register. See the description of the transpose feature for the available
 * transpose options.
 *
 * Values:
 * - 0b00 - No transposition.
 * - 0b01 - Bits in bytes are transposed; bytes are not transposed.
 * - 0b10 - Both bits in bytes and bytes are transposed.
 * - 0b11 - Only bytes are transposed; no bits in a byte are transposed.
 */
/*@{*/
/*! @brief Read current value of the CRC_CTRL_TOTR field. */
#define CRC_RD_CTRL_TOTR(base) ((CRC_CTRL_REG(base) & CRC_CTRL_TOTR_MASK) >> CRC_CTRL_TOTR_SHIFT)
#define CRC_BRD_CTRL_TOTR(base) (BME_UBFX32(&CRC_CTRL_REG(base), CRC_CTRL_TOTR_SHIFT, CRC_CTRL_TOTR_WIDTH))

/*! @brief Set the TOTR field to a new value. */
#define CRC_WR_CTRL_TOTR(base, value) (CRC_RMW_CTRL(base, CRC_CTRL_TOTR_MASK, CRC_CTRL_TOTR(value)))
#define CRC_BWR_CTRL_TOTR(base, value) (BME_BFI32(&CRC_CTRL_REG(base), ((uint32_t)(value) << CRC_CTRL_TOTR_SHIFT), CRC_CTRL_TOTR_SHIFT, CRC_CTRL_TOTR_WIDTH))
/*@}*/

/*!
 * @name Register CRC_CTRL, field TOT[31:30] (RW)
 *
 * Defines the transpose configuration of the data written to the CRC data
 * register. See the description of the transpose feature for the available transpose
 * options.
 *
 * Values:
 * - 0b00 - No transposition.
 * - 0b01 - Bits in bytes are transposed; bytes are not transposed.
 * - 0b10 - Both bits in bytes and bytes are transposed.
 * - 0b11 - Only bytes are transposed; no bits in a byte are transposed.
 */
/*@{*/
/*! @brief Read current value of the CRC_CTRL_TOT field. */
#define CRC_RD_CTRL_TOT(base) ((CRC_CTRL_REG(base) & CRC_CTRL_TOT_MASK) >> CRC_CTRL_TOT_SHIFT)
#define CRC_BRD_CTRL_TOT(base) (BME_UBFX32(&CRC_CTRL_REG(base), CRC_CTRL_TOT_SHIFT, CRC_CTRL_TOT_WIDTH))

/*! @brief Set the TOT field to a new value. */
#define CRC_WR_CTRL_TOT(base, value) (CRC_RMW_CTRL(base, CRC_CTRL_TOT_MASK, CRC_CTRL_TOT(value)))
#define CRC_BWR_CTRL_TOT(base, value) (BME_BFI32(&CRC_CTRL_REG(base), ((uint32_t)(value) << CRC_CTRL_TOT_SHIFT), CRC_CTRL_TOT_SHIFT, CRC_CTRL_TOT_WIDTH))
/*@}*/

/*******************************************************************************
 * CRC_CTRLHU - CRC_CTRLHU register.
 ******************************************************************************/

/*!
 * @brief CRC_CTRLHU - CRC_CTRLHU register. (RW)
 *
 * Reset value: 0x00U
 */
/*!
 * @name Constants and macros for entire CRC_CTRLHU register
 */
/*@{*/
#define CRC_RD_CTRLHU(base)      (CRC_CTRLHU_REG(base))
#define CRC_WR_CTRLHU(base, value) (CRC_CTRLHU_REG(base) = (value))
#define CRC_RMW_CTRLHU(base, mask, value) (CRC_WR_CTRLHU(base, (CRC_RD_CTRLHU(base) & ~(mask)) | (value)))
#define CRC_SET_CTRLHU(base, value) (BME_OR8(&CRC_CTRLHU_REG(base), (uint8_t)(value)))
#define CRC_CLR_CTRLHU(base, value) (BME_AND8(&CRC_CTRLHU_REG(base), (uint8_t)(~(value))))
#define CRC_TOG_CTRLHU(base, value) (BME_XOR8(&CRC_CTRLHU_REG(base), (uint8_t)(value)))
/*@}*/

/*
 * Constants & macros for individual CRC_CTRLHU bitfields
 */

/*!
 * @name Register CRC_CTRLHU, field TCRC[0] (RW)
 *
 * Values:
 * - 0b0 - 16-bit CRC protocol.
 * - 0b1 - 32-bit CRC protocol.
 */
/*@{*/
/*! @brief Read current value of the CRC_CTRLHU_TCRC field. */
#define CRC_RD_CTRLHU_TCRC(base) ((CRC_CTRLHU_REG(base) & CRC_CTRLHU_TCRC_MASK) >> CRC_CTRLHU_TCRC_SHIFT)
#define CRC_BRD_CTRLHU_TCRC(base) (BME_UBFX8(&CRC_CTRLHU_REG(base), CRC_CTRLHU_TCRC_SHIFT, CRC_CTRLHU_TCRC_WIDTH))

/*! @brief Set the TCRC field to a new value. */
#define CRC_WR_CTRLHU_TCRC(base, value) (CRC_RMW_CTRLHU(base, CRC_CTRLHU_TCRC_MASK, CRC_CTRLHU_TCRC(value)))
#define CRC_BWR_CTRLHU_TCRC(base, value) (BME_BFI8(&CRC_CTRLHU_REG(base), ((uint8_t)(value) << CRC_CTRLHU_TCRC_SHIFT), CRC_CTRLHU_TCRC_SHIFT, CRC_CTRLHU_TCRC_WIDTH))
/*@}*/

/*!
 * @name Register CRC_CTRLHU, field WAS[1] (RW)
 *
 * Values:
 * - 0b0 - Writes to CRC data register are data values.
 * - 0b1 - Writes to CRC data reguster are seed values.
 */
/*@{*/
/*! @brief Read current value of the CRC_CTRLHU_WAS field. */
#define CRC_RD_CTRLHU_WAS(base) ((CRC_CTRLHU_REG(base) & CRC_CTRLHU_WAS_MASK) >> CRC_CTRLHU_WAS_SHIFT)
#define CRC_BRD_CTRLHU_WAS(base) (BME_UBFX8(&CRC_CTRLHU_REG(base), CRC_CTRLHU_WAS_SHIFT, CRC_CTRLHU_WAS_WIDTH))

/*! @brief Set the WAS field to a new value. */
#define CRC_WR_CTRLHU_WAS(base, value) (CRC_RMW_CTRLHU(base, CRC_CTRLHU_WAS_MASK, CRC_CTRLHU_WAS(value)))
#define CRC_BWR_CTRLHU_WAS(base, value) (BME_BFI8(&CRC_CTRLHU_REG(base), ((uint8_t)(value) << CRC_CTRLHU_WAS_SHIFT), CRC_CTRLHU_WAS_SHIFT, CRC_CTRLHU_WAS_WIDTH))
/*@}*/

/*!
 * @name Register CRC_CTRLHU, field FXOR[2] (RW)
 *
 * Values:
 * - 0b0 - No XOR on reading.
 * - 0b1 - Invert or complement the read value of CRC data register.
 */
/*@{*/
/*! @brief Read current value of the CRC_CTRLHU_FXOR field. */
#define CRC_RD_CTRLHU_FXOR(base) ((CRC_CTRLHU_REG(base) & CRC_CTRLHU_FXOR_MASK) >> CRC_CTRLHU_FXOR_SHIFT)
#define CRC_BRD_CTRLHU_FXOR(base) (BME_UBFX8(&CRC_CTRLHU_REG(base), CRC_CTRLHU_FXOR_SHIFT, CRC_CTRLHU_FXOR_WIDTH))

/*! @brief Set the FXOR field to a new value. */
#define CRC_WR_CTRLHU_FXOR(base, value) (CRC_RMW_CTRLHU(base, CRC_CTRLHU_FXOR_MASK, CRC_CTRLHU_FXOR(value)))
#define CRC_BWR_CTRLHU_FXOR(base, value) (BME_BFI8(&CRC_CTRLHU_REG(base), ((uint8_t)(value) << CRC_CTRLHU_FXOR_SHIFT), CRC_CTRLHU_FXOR_SHIFT, CRC_CTRLHU_FXOR_WIDTH))
/*@}*/

/*!
 * @name Register CRC_CTRLHU, field TOTR[5:4] (RW)
 *
 * Values:
 * - 0b00 - No Transposition.
 * - 0b01 - Bits in bytes are transposed, bytes are not transposed.
 * - 0b10 - Both bits in bytes and bytes are transposed.
 * - 0b11 - Only bytes are transposed; no bits in a byte are transposed.
 */
/*@{*/
/*! @brief Read current value of the CRC_CTRLHU_TOTR field. */
#define CRC_RD_CTRLHU_TOTR(base) ((CRC_CTRLHU_REG(base) & CRC_CTRLHU_TOTR_MASK) >> CRC_CTRLHU_TOTR_SHIFT)
#define CRC_BRD_CTRLHU_TOTR(base) (BME_UBFX8(&CRC_CTRLHU_REG(base), CRC_CTRLHU_TOTR_SHIFT, CRC_CTRLHU_TOTR_WIDTH))

/*! @brief Set the TOTR field to a new value. */
#define CRC_WR_CTRLHU_TOTR(base, value) (CRC_RMW_CTRLHU(base, CRC_CTRLHU_TOTR_MASK, CRC_CTRLHU_TOTR(value)))
#define CRC_BWR_CTRLHU_TOTR(base, value) (BME_BFI8(&CRC_CTRLHU_REG(base), ((uint8_t)(value) << CRC_CTRLHU_TOTR_SHIFT), CRC_CTRLHU_TOTR_SHIFT, CRC_CTRLHU_TOTR_WIDTH))
/*@}*/

/*!
 * @name Register CRC_CTRLHU, field TOT[7:6] (RW)
 *
 * Values:
 * - 0b00 - No Transposition.
 * - 0b01 - Bits in bytes are transposed, bytes are not transposed.
 * - 0b10 - Both bits in bytes and bytes are transposed.
 * - 0b11 - Only bytes are transposed; no bits in a byte are transposed.
 */
/*@{*/
/*! @brief Read current value of the CRC_CTRLHU_TOT field. */
#define CRC_RD_CTRLHU_TOT(base) ((CRC_CTRLHU_REG(base) & CRC_CTRLHU_TOT_MASK) >> CRC_CTRLHU_TOT_SHIFT)
#define CRC_BRD_CTRLHU_TOT(base) (BME_UBFX8(&CRC_CTRLHU_REG(base), CRC_CTRLHU_TOT_SHIFT, CRC_CTRLHU_TOT_WIDTH))

/*! @brief Set the TOT field to a new value. */
#define CRC_WR_CTRLHU_TOT(base, value) (CRC_RMW_CTRLHU(base, CRC_CTRLHU_TOT_MASK, CRC_CTRLHU_TOT(value)))
#define CRC_BWR_CTRLHU_TOT(base, value) (BME_BFI8(&CRC_CTRLHU_REG(base), ((uint8_t)(value) << CRC_CTRLHU_TOT_SHIFT), CRC_CTRLHU_TOT_SHIFT, CRC_CTRLHU_TOT_WIDTH))
/*@}*/


#endif


