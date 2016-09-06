/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __FSL_USB_FEATURES_H__
#define __FSL_USB_FEATURES_H__

#if defined(CPU_MK10DN512VLK10) || defined(CPU_MK10DN512VLL10) || defined(CPU_MK10DX128VLQ10) || defined(CPU_MK10DX256VLQ10) || \
    defined(CPU_MK10DN512VLQ10) || defined(CPU_MK10DN512VMB10) || defined(CPU_MK10DN512VMC10) || defined(CPU_MK10DX128VMD10) || \
    defined(CPU_MK10DX256VMD10) || defined(CPU_MK10DN512VMD10) || defined(CPU_MK10DX128VMP5) || defined(CPU_MK10DN128VMP5) || \
    defined(CPU_MK10DX64VMP5) || defined(CPU_MK10DN64VMP5) || defined(CPU_MK10DX32VMP5) || defined(CPU_MK10DN32VMP5) || \
    defined(CPU_MK10DX128VLH5) || defined(CPU_MK10DN128VLH5) || defined(CPU_MK10DX64VLH5) || defined(CPU_MK10DN64VLH5) || \
    defined(CPU_MK10DX32VLH5) || defined(CPU_MK10DN32VLH5) || defined(CPU_MK10DX128VFM5) || defined(CPU_MK10DN128VFM5) || \
    defined(CPU_MK10DX64VFM5) || defined(CPU_MK10DN64VFM5) || defined(CPU_MK10DX32VFM5) || defined(CPU_MK10DN32VFM5) || \
    defined(CPU_MK10DX128VFT5) || defined(CPU_MK10DN128VFT5) || defined(CPU_MK10DX64VFT5) || defined(CPU_MK10DN64VFT5) || \
    defined(CPU_MK10DX32VFT5) || defined(CPU_MK10DN32VFT5) || defined(CPU_MK10DX128VLF5) || defined(CPU_MK10DN128VLF5) || \
    defined(CPU_MK10DX64VLF5) || defined(CPU_MK10DN64VLF5) || defined(CPU_MK10DX32VLF5) || defined(CPU_MK10DN32VLF5) || \
    defined(CPU_MK10DX64VLH7) || defined(CPU_MK10DX128VLH7) || defined(CPU_MK10DX256VLH7) || defined(CPU_MK10DX64VLK7) || \
    defined(CPU_MK10DX128VLK7) || defined(CPU_MK10DX256VLK7) || defined(CPU_MK10DX128VLL7) || defined(CPU_MK10DX256VLL7) || \
    defined(CPU_MK10DX64VMB7) || defined(CPU_MK10DX128VMB7) || defined(CPU_MK10DX256VMB7) || defined(CPU_MK10DX128VML7) || \
    defined(CPU_MK10DX256VML7) || defined(CPU_MK10FN1M0VLQ12) || defined(CPU_MK10FX512VLQ12) || defined(CPU_MK10FN1M0VMD12) || \
    defined(CPU_MK10FX512VMD12) || defined(CPU_MK20DN512VLK10) || defined(CPU_MK20DN512VLL10) || defined(CPU_MK20DX128VLQ10) || \
    defined(CPU_MK20DX256VLQ10) || defined(CPU_MK20DN512VLQ10) || defined(CPU_MK20DN512VMB10) || defined(CPU_MK20DX256VMC10) || \
    defined(CPU_MK20DN512VMC10) || defined(CPU_MK20DX128VMD10) || defined(CPU_MK20DX256VMD10) || defined(CPU_MK20DN512VMD10) || \
    defined(CPU_MK20DX128VMP5) || defined(CPU_MK20DN128VMP5) || defined(CPU_MK20DX64VMP5) || defined(CPU_MK20DN64VMP5) || \
    defined(CPU_MK20DX32VMP5) || defined(CPU_MK20DN32VMP5) || defined(CPU_MK20DX128VLH5) || defined(CPU_MK20DN128VLH5) || \
    defined(CPU_MK20DX64VLH5) || defined(CPU_MK20DN64VLH5) || defined(CPU_MK20DX32VLH5) || defined(CPU_MK20DN32VLH5) || \
    defined(CPU_MK20DX128VFM5) || defined(CPU_MK20DN128VFM5) || defined(CPU_MK20DX64VFM5) || defined(CPU_MK20DN64VFM5) || \
    defined(CPU_MK20DX32VFM5) || defined(CPU_MK20DN32VFM5) || defined(CPU_MK20DX128VFT5) || defined(CPU_MK20DN128VFT5) || \
    defined(CPU_MK20DX64VFT5) || defined(CPU_MK20DN64VFT5) || defined(CPU_MK20DX32VFT5) || defined(CPU_MK20DN32VFT5) || \
    defined(CPU_MK20DX128VLF5) || defined(CPU_MK20DN128VLF5) || defined(CPU_MK20DX64VLF5) || defined(CPU_MK20DN64VLF5) || \
    defined(CPU_MK20DX32VLF5) || defined(CPU_MK20DN32VLF5) || defined(CPU_MK20DX64VLH7) || defined(CPU_MK20DX128VLH7) || \
    defined(CPU_MK20DX256VLH7) || defined(CPU_MK20DX64VLK7) || defined(CPU_MK20DX128VLK7) || defined(CPU_MK20DX256VLK7) || \
    defined(CPU_MK20DX128VLL7) || defined(CPU_MK20DX256VLL7) || defined(CPU_MK20DX64VMB7) || defined(CPU_MK20DX128VMB7) || \
    defined(CPU_MK20DX256VMB7) || defined(CPU_MK20DX128VML7) || defined(CPU_MK20DX256VML7) || defined(CPU_MK20FN1M0VLQ12) || \
    defined(CPU_MK20FX512VLQ12) || defined(CPU_MK20FN1M0VMD12) || defined(CPU_MK20FX512VMD12) || defined(CPU_MK21FX512VLQ12) || \
    defined(CPU_MK21FN1M0VLQ12) || defined(CPU_MK21FX512VLQ12WS) || defined(CPU_MK21FN1M0VLQ12WS) || defined(CPU_MK21FX512VMC12) || \
    defined(CPU_MK21FN1M0VMC12) || defined(CPU_MK21FX512VMC12WS) || defined(CPU_MK21FN1M0VMC12WS) || defined(CPU_MK21FX512VMD12) || \
    defined(CPU_MK21FN1M0VMD12) || defined(CPU_MK21FX512VMD12WS) || defined(CPU_MK21FN1M0VMD12WS) || defined(CPU_MK22FX512VLH12) || \
    defined(CPU_MK22FN1M0VLH12) || defined(CPU_MK22FX512VLK12) || defined(CPU_MK22FN1M0VLK12) || defined(CPU_MK22FX512VLL12) || \
    defined(CPU_MK22FN1M0VLL12) || defined(CPU_MK22FX512VLQ12) || defined(CPU_MK22FN1M0VLQ12) || defined(CPU_MK22FX512VMC12) || \
    defined(CPU_MK22FN1M0VMC12) || defined(CPU_MK22FX512VMD12) || defined(CPU_MK22FN1M0VMD12) || defined(CPU_MK30DN512VLK10) || \
    defined(CPU_MK30DN512VLL10) || defined(CPU_MK30DX128VLQ10) || defined(CPU_MK30DX256VLQ10) || defined(CPU_MK30DN512VLQ10) || \
    defined(CPU_MK30DN512VMB10) || defined(CPU_MK30DN512VMC10) || defined(CPU_MK30DX128VMD10) || defined(CPU_MK30DX256VMD10) || \
    defined(CPU_MK30DN512VMD10) || defined(CPU_MK30DX64VLH7) || defined(CPU_MK30DX128VLH7) || defined(CPU_MK30DX256VLH7) || \
    defined(CPU_MK30DX64VLK7) || defined(CPU_MK30DX128VLK7) || defined(CPU_MK30DX256VLK7) || defined(CPU_MK30DX128VLL7) || \
    defined(CPU_MK30DX256VLL7) || defined(CPU_MK30DX64VMB7) || defined(CPU_MK30DX128VMB7) || defined(CPU_MK30DX256VMB7) || \
    defined(CPU_MK30DX128VML7) || defined(CPU_MK30DX256VML7) || defined(CPU_MK40DN512VLK10) || defined(CPU_MK40DN512VLL10) || \
    defined(CPU_MK40DX128VLQ10) || defined(CPU_MK40DX256VLQ10) || defined(CPU_MK40DN512VLQ10) || defined(CPU_MK40DN512VMB10) || \
    defined(CPU_MK40DN512VMC10) || defined(CPU_MK40DX128VMD10) || defined(CPU_MK40DX256VMD10) || defined(CPU_MK40DN512VMD10) || \
    defined(CPU_MK40DX64VLH7) || defined(CPU_MK40DX128VLH7) || defined(CPU_MK40DX256VLH7) || defined(CPU_MK40DX64VLK7) || \
    defined(CPU_MK40DX128VLK7) || defined(CPU_MK40DX256VLK7) || defined(CPU_MK40DX128VLL7) || defined(CPU_MK40DX256VLL7) || \
    defined(CPU_MK40DX64VMB7) || defined(CPU_MK40DX128VMB7) || defined(CPU_MK40DX256VMB7) || defined(CPU_MK40DX128VML7) || \
    defined(CPU_MK40DX256VML7) || defined(CPU_MK50DX256CLL10) || defined(CPU_MK50DN512CLL10) || defined(CPU_MK50DN512CLQ10) || \
    defined(CPU_MK50DX256CMC10) || defined(CPU_MK50DN512CMC10) || defined(CPU_MK50DN512CMD10) || defined(CPU_MK50DX256CMD10) || \
    defined(CPU_MK50DX128CLH7) || defined(CPU_MK50DX256CLK10) || defined(CPU_MK50DX128CLK7) || defined(CPU_MK50DX256CLK7) || \
    defined(CPU_MK50DX256CLL7) || defined(CPU_MK50DX256CMB10) || defined(CPU_MK50DX128CMB7) || defined(CPU_MK50DX256CMB7) || \
    defined(CPU_MK50DX256CML7) || defined(CPU_MK51DX256CLL10) || defined(CPU_MK51DN512CLL10) || defined(CPU_MK51DN256CLQ10) || \
    defined(CPU_MK51DN512CLQ10) || defined(CPU_MK51DX256CMC10) || defined(CPU_MK51DN512CMC10) || defined(CPU_MK51DN256CMD10) || \
    defined(CPU_MK51DN512CMD10) || defined(CPU_MK51DX128CLH7) || defined(CPU_MK51DX256CLK10) || defined(CPU_MK51DX128CLK7) || \
    defined(CPU_MK51DX256CLK7) || defined(CPU_MK51DX256CLL7) || defined(CPU_MK51DX256CMB10) || defined(CPU_MK51DX128CMB7) || \
    defined(CPU_MK51DX256CMB7) || defined(CPU_MK51DX256CML7) || defined(CPU_MK52DN512CLQ10) || defined(CPU_MK52DN512CMD10) || \
    defined(CPU_MK53DN512CLQ10) || defined(CPU_MK53DX256CLQ10) || defined(CPU_MK53DN512CMD10) || defined(CPU_MK53DX256CMD10) || \
    defined(CPU_MK60DN256VLL10) || defined(CPU_MK60DX256VLL10) || defined(CPU_MK60DN512VLL10) || defined(CPU_MK60DN256VLQ10) || \
    defined(CPU_MK60DX256VLQ10) || defined(CPU_MK60DN512VLQ10) || defined(CPU_MK60DN256VMC10) || defined(CPU_MK60DX256VMC10) || \
    defined(CPU_MK60DN512VMC10) || defined(CPU_MK60DN256VMD10) || defined(CPU_MK60DX256VMD10) || defined(CPU_MK60DN512VMD10) || \
    defined(CPU_MK60FN1M0VLQ12) || defined(CPU_MK60FX512VLQ12) || defined(CPU_MK60FN1M0VLQ15) || defined(CPU_MK60FX512VLQ15) || \
    defined(CPU_MK60FN1M0VMD12) || defined(CPU_MK60FX512VMD12) || defined(CPU_MK60FN1M0VMD15) || defined(CPU_MK60FX512VMD15) || \
    defined(CPU_MK61FN1M0VMD12) || defined(CPU_MK61FX512VMD12) || defined(CPU_MK61FN1M0VMD15) || defined(CPU_MK61FX512VMD15) || \
    defined(CPU_MK61FN1M0VMD12WS) || defined(CPU_MK61FX512VMD12WS) || defined(CPU_MK61FN1M0VMD15WS) || defined(CPU_MK61FX512VMD15WS) || \
    defined(CPU_MK61FN1M0VMF12) || defined(CPU_MK61FX512VMF12) || defined(CPU_MK61FN1M0VMF15) || defined(CPU_MK61FX512VMF15) || \
    defined(CPU_MK61FN1M0VMJ12) || defined(CPU_MK61FX512VMJ12) || defined(CPU_MK61FN1M0VMJ15) || defined(CPU_MK61FX512VMJ15) || \
    defined(CPU_MK61FN1M0VMJ12WS) || defined(CPU_MK61FX512VMJ12WS) || defined(CPU_MK61FN1M0VMJ15WS) || defined(CPU_MK61FX512VMJ15WS) || \
    defined(CPU_MK63FN1M0VMD12) || defined(CPU_MK63FN1M0VMD12WS) || defined(CPU_MK64FN1M0VMD12) || defined(CPU_MK64FX512VMD12) || \
    defined(CPU_MK70FN1M0VMF12) || defined(CPU_MK70FX512VMF12) || defined(CPU_MK70FN1M0VMF15) || defined(CPU_MK70FX512VMF15) || \
    defined(CPU_MK70FN1M0VMJ12) || defined(CPU_MK70FX512VMJ12) || defined(CPU_MK70FN1M0VMJ15) || defined(CPU_MK70FX512VMJ15) || \
    defined(CPU_MK70FN1M0VMJ12WS) || defined(CPU_MK70FX512VMJ12WS) || defined(CPU_MK70FN1M0VMJ15WS) || defined(CPU_MK70FX512VMJ15WS) || \
    defined(CPU_MK11DX128VLK5) || defined(CPU_MK11DX256VLK5) || defined(CPU_MK11DN512VLK5) || defined(CPU_MK11DX128VLK5WS) || \
    defined(CPU_MK11DX256VLK5WS) || defined(CPU_MK11DN512VLK5WS) || defined(CPU_MK11DX128VMC5) || defined(CPU_MK11DX256VMC5) || \
    defined(CPU_MK11DN512VMC5) || defined(CPU_MK11DX128VMC5WS) || defined(CPU_MK11DX256VMC5WS) || defined(CPU_MK11DN512VMC5WS) || \
    defined(CPU_MK12DX128VLH5) || defined(CPU_MK12DX256VLH5) || defined(CPU_MK12DN512VLH5) || defined(CPU_MK12DX128VLK5) || \
    defined(CPU_MK12DX256VLK5) || defined(CPU_MK12DN512VLK5) || defined(CPU_MK12DX128VMC5) || defined(CPU_MK12DX256VMC5) || \
    defined(CPU_MK12DN512VMC5) || defined(CPU_MK12DX128VLF5) || defined(CPU_MK12DX256VLF5) || defined(CPU_MK21DX128VLK5) || \
    defined(CPU_MK21DX256VLK5) || defined(CPU_MK21DN512VLK5) || defined(CPU_MK21DX128VLK5WS) || defined(CPU_MK21DX256VLK5WS) || \
    defined(CPU_MK21DN512VLK5WS) || defined(CPU_MK21DX128VMC5) || defined(CPU_MK21DX256VMC5) || defined(CPU_MK21DN512VMC5) || \
    defined(CPU_MK21DX128VMC5WS) || defined(CPU_MK21DX256VMC5WS) || defined(CPU_MK21DN512VMC5WS) || defined(CPU_MK22DX128VLH5) || \
    defined(CPU_MK22DX256VLH5) || defined(CPU_MK22DN512VLH5) || defined(CPU_MK22DX128VLK5) || defined(CPU_MK22DX256VLK5) || \
    defined(CPU_MK22DN512VLK5) || defined(CPU_MK22DX128VMC5) || defined(CPU_MK22DX256VMC5) || defined(CPU_MK22DN512VMC5) || \
    defined(CPU_MK22DX128VLF5) || defined(CPU_MK22DX256VLF5) || defined(CPU_MK22FN512VDC12) || defined(CPU_MK22FN512VMC12)
#else
    #error "No valid CPU defined!"
#endif

#endif /* __FSL_USB_FEATURES_H__*/
/*******************************************************************************
 * EOF
 *******************************************************************************/