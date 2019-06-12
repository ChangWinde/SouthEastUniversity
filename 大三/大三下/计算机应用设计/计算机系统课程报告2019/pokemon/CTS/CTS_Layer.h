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
 * @file   CTS_Layer.h
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
 * @par    Supported API Calls:
 *              - TI_CAPT_Init_Baseline()
 *              - TI_CAPT_Update_Baseline()
 *              - TI_CAPT_Reset_Tracking()
 *              - TI_CAPT_Update_Tracking_DOI()
 *              - TI_CAPT_Update_Tracking_Rate()
 *              - TI_CAPT_Update_Baseline()
 *              - TI_CAPT_Raw()
 *              - TI_CAPT_Custom()
 *              - TI_CAPT_Button()
 *              - TI_CAPT_Buttons()
 *              - TI_CAPT_Slider()
 *              - TI_CAPT_Wheel()
 ******************************************************************************/

#ifndef CTS_LAYER
#define CTS_LAYER

#include "CTS_HAL.h"

//! \name Status Register Definitions
//! @{
//
//! (bit 0): Event Flag: this indicates that a threshold crossing occured
#define EVNT            0x01 
//! (bit 1): Direction of Interest: This indicates if the measurement is looking
//! for an increasing (set) or decreasing (clr) capacitance.
#define DOI_MASK        0x02 
#define DOI_INC         0x02  
#define DOI_DEC         0x00  
//! (bit 2): Past Event Flag: this indicates that a prior element within the 
//! sensor group has detected a threshold crossing
#define PAST_EVNT       0x04  
//! (bits 4-5): Tracking Rate in Direction of Interest: this indicates at what  
//! rate the baseline will adjust to the current measurement when the 
//! when the measurement is changing in the direction of interst but does not
//! result in a threshold crossing:
//!     \n Very Slow
//!     \n Slow
//!     \n Medium
//!     \n Fast
#define TRIDOI_VSLOW    0x00   
#define TRIDOI_SLOW     0x10   
#define TRIDOI_MED      0x20  
#define TRIDOI_FAST     0x30  
//! (bits 5-6): Tracking Rate Against Direction of Interest: this indicates at   
//! what rate the baseline will adjust to the current measurement when the 
//! when the measurement is changing against the direction of interst:
//!     \n Fast
//!     \n Medium
//!     \n Slow
//!     \n Very Slow
#define TRADOI_FAST     0x00   
#define TRADOI_MED      0x40   
#define TRADOI_SLOW     0x80  
#define TRADOI_VSLOW    0xC0  

//! @}


// API Calls
void TI_CAPT_Init_Baseline(const struct Sensor*);
void TI_CAPT_Update_Baseline(const struct Sensor*, uint8_t);

void TI_CAPT_Reset_Tracking(void);
void TI_CAPT_Update_Tracking_DOI(uint8_t);
void TI_CAPT_Update_Tracking_Rate(uint8_t);

void TI_CAPT_Raw(const struct Sensor*, uint16_t*);

void TI_CAPT_Custom(const struct Sensor *, uint16_t*);

uint8_t TI_CAPT_Button(const struct Sensor *);
const struct Element * TI_CAPT_Buttons(const struct Sensor *);
uint16_t TI_CAPT_Slider(const struct Sensor*);
uint16_t TI_CAPT_Wheel(const struct Sensor*);

// Internal Calls
uint8_t Dominant_Element (const struct Sensor*, uint16_t*);

#endif
