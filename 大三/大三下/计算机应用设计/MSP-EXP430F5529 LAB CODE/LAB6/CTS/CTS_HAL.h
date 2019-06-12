/*******************************************************************************
 *
 *  CTS_HAL.h
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/
 
/***************************************************************************//**
 * @file   CTS_HAL.h
 *
 * @brief       
 *
 * @par    Project:
 *             MSP430 Capacitive Touch Library 
 *
 * @par    Developed using:
 *             IAR Version : 5.10.6 [Kickstart] (5.10.6.30180)
 *             CCS Version : 4.2.1.00004, w/support for GCC extensions (--gcc)
 *
 *
 * @version     1.0.0 Initial Release
 *
 * @par    Supported Hardware Configurations:
 *              - TI_CTS_RO_COMPAp_TA0_WDTp_HAL()
 *              - TI_CTS_fRO_COMPAp_TA0_SW_HAL()
 *              - TI_CTS_fRO_COMPAp_SW_TA0_HAL()
 *              - TI_CTS_RO_COMPAp_TA1_WDTp_HAL()
 *              - TI_CTS_fRO_COMPAp_TA1_SW_HAL()
 *              - TI_CTS_RC_PAIR_TA0_HAL()
 *              - TI_CTS_RO_PINOSC_TA0_WDTp_HAL()
 *              - TI_CTS_RO_PINOSC_TA0_HAL()
 *              - TI_CTS_fRO_PINOSC_TA0_SW_HAL()
 *              - TI_CTS_RO_COMPB_TA0_WDTA_HAL()
 *              - TI_CTS_RO_COMPB_TA1_WDTA_HAL()
 *              - TI_CTS_fRO_COMPB_TA0_SW_HAL()
 *              - TI_CTS_fRO_COMPB_TA1_SW_HAL()
 ******************************************************************************/

#ifndef CAP_TOUCH_HAL
#define CAP_TOUCH_HAL

#include "structure.h"

void TI_CTS_RO_COMPAp_TA0_WDTp_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_fRO_COMPAp_TA0_SW_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_fRO_COMPAp_SW_TA0_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_RO_COMPAp_TA1_WDTp_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_fRO_COMPAp_TA1_SW_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_RC_PAIR_TA0_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_RO_PINOSC_TA0_WDTp_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_RO_PINOSC_TA0_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_fRO_PINOSC_TA0_SW_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_RO_COMPB_TA0_WDTA_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_fRO_COMPB_TA0_SW_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_RO_COMPB_TA1_WDTA_HAL(const struct Sensor *, uint16_t *);

void TI_CTS_fRO_COMPB_TA1_SW_HAL(const struct Sensor *, uint16_t *);

#endif
