//***************************************************************************//
//                                                                           //
//                         MSP-EXP430F5529 LAB CODE                          //
//                                                                           //
//                               lab1 - CLOCK                                //
//                                                                           //
//***************************************************************************//
/*******************************************************************************
 *
 *  Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
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
#include <stdint.h>
#include "msp430.h"
#include "HAL_Buttons.h"
#include "HAL_Board.h"
#include "HAL_Dogs102x6.h"
#include "HAL_Menu.h"
#include "HAL_Wheel.h"
#include "lab1.h"

#define HOUR10    (0)
#define HOUR01    (1)
#define COLON2    (2)
#define MINUTE10  (3)
#define MINUTE01  (4)
#define COLON1    (5)
#define SECOND10  (6)
#define SECOND01  (7)

#define MONTH10   (0)
#define MONTH01   (1)
#define SLASH2    (2)
#define DAY10     (3)
#define DAY01     (4)
#define SLASH1    (5)
#define YEAR10    (6)
#define YEAR01    (7)

#define TIME_OUT        10

// This image has been created by ImageDog using 'DigitalWatch.png'
const uint8_t digitalWatch[] =
{
    0x66,    // Image width in pixels
    0x08,    // Image height in rows (1 row = 8 pixels)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfc,
    0xf8, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
    0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
    0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
    0xf8, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x7c,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x7c, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x07, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07, 0x0f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// This image has been created by ImageDog using 'small_sunglasses.png'
static const uint8_t sunglasses[] =
{
    0x30,    // Image width in pixels
    0x06,    // Image height in rows (1 row = 8 pixels)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x01, 0x03, 0x03, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x06, 0x1e, 0x1f, 0x3f, 0x7f,
    0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x1f, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x01, 0x03, 0x03, 0x0f, 0x1e, 0x1c, 0x38, 0x30, 0x70, 0xf8, 0xfc, 0x0e, 0x07, 0xf3, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0e, 0x1c, 0x3f, 0x7f, 0x7f, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x1f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0,
    0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x20, 0xf0, 0xf8, 0xf8, 0x3c, 0x0e, 0x07, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// This image has been created by ImageDog using 'lightbulb_outline_inverted_mini.png'
static const uint8_t lightbulb[] =
{
    0x20,    // Image width in pixels
    0x06,    // Image height in rows (1 row = 8 pixels)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x06, 0x0c, 0x18, 0x10, 0x10, 0x10, 0x30, 0x00,
    0x00, 0x00, 0x30, 0x11, 0x11, 0x11, 0x18, 0x0c, 0x04, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x7c, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x40, 0xe0, 0x30, 0x9d, 0xcf, 0x7f, 0x1e, 0x00, 0x80, 0xfe, 0x1c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x0c, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x80, 0x01, 0x03, 0x1c, 0x70, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x78, 0x09, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x09, 0x79, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x5f, 0x00, 0x24, 0x25, 0x25,
    0x25, 0x2d, 0x2d, 0x6d, 0x00, 0x09, 0x7f, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x30, 0x38, 0xa8, 0x2c,
    0x2c, 0x2c, 0x2c, 0x28, 0x30, 0x20, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t cos[] = {
    28, 28, 27, 27, 26, 24, 23, 21,
    19, 16, 14, 11, 9, 6, 3, 0,
    20, 20, 20, 19, 18, 17, 16, 15,
    13, 12, 10, 8, 6, 4, 2, 0,
    20, 20, 20, 19, 18, 17, 16, 15,
    13, 12, 10, 8, 6, 4, 2, 0,
    12, 12, 12, 11, 11, 10, 10, 9,
    8, 7, 6, 5, 4, 2, 1, 0
};

char time[9] = {'0', '4', ':', '3', '0', ':', '0', '0', '\0'};
char timeOld[9] = {'0', '4', ':', '3', '0', ':', '0', '0', '\0'};
char date[9] = {'0', '1', '/', '0', '1', '/', '1', '1', '\0'};

uint8_t secondBCD = 0, minuteBCD = 30, hourBCD = 4;
uint8_t secondOldBCD, minuteOldBCD, hourOldBCD;
uint8_t RTCExitSec = 0;

uint8_t ClockY = 32;
uint8_t ClockX = 51;

static const char *const clockMenuText[] = {
    "===LAB1:Clock====",
    "1. Contrast ",
    "2. Backlight ",
    "3. Digital Clock ",
    "4. Analog Clock ",
    "5. Set Time "
};

// Forward declared functions
void ContrastSetting(void);
void BacklightSetting(void);
void DisplayDigitalClock(void);
void DigitalClockUpdate(void);
void DigitalClockDraw(void);
void AnalogClockInit(void);
void DisplayAnalogClock(void);
void AnalogClockUpdate(void);
void AnalogClockDrawHand(uint8_t time, uint8_t pixel, uint8_t hand);
void SetTime(void);

/***************************************************************************//**
 * @brief  The menu of lab1.
 * @param  None
 * @return none
 ******************************************************************************/

void lab1(void)
{
	   uint8_t selection = 0;
	    buttonsPressed = 0;
	    Dogs102x6_clearScreen();                         //清屏
	    Dogs102x6_stringDraw(7, 0, "*S1=Enter S2=Esc*", DOGS102x6_DRAW_NORMAL);
	    selection = Menu_active((char **)clockMenuText, 5);   //显示菜单，并返回菜单选项
	    if (buttonsPressed & BUTTON_S2);
	    else
	       switch (selection)
	        {
	            case 1: ContrastSetting(); break;             //对比度设置程序
	            case 2: BacklightSetting(); break;            //背光设置程序
	            case 3: DisplayDigitalClock(); break;         //数字时钟程序
	            case 4: DisplayAnalogClock(); break;          //模拟时钟程序
	            case 5: SetTime(); break;                     //设置时钟程序
	            default: break;
	        }
}

/***************************************************************************//**
 * @brief  Allows the user to adjust the contrast
 * @param  none
 * @return none
 ******************************************************************************/

void ContrastSetting(void)
{
    uint8_t contrast = *((unsigned char *)contrastSetpointAddress);

    buttonsPressed = 0;
    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(0, 0, " Adjust contrast ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(1, 0, "by turning wheel.", DOGS102x6_DRAW_NORMAL);

    Dogs102x6_imageDraw(sunglasses, 2, 27);                //显示太阳镜图案
    __delay_cycles(10000000); //delay for user to read directions

    while (!buttonsPressed)
    {
        contrast = Wheel_getValue() / 130;                 //通过齿轮电位计采样得到需设置的对比度值
        Dogs102x6_setContrast(contrast);                   //重置对比度
    }

    WriteFlashSettings(contrast, contrastSetpointAddress); //将新对比度值写入FLASH地址   保存设置

    buttonsPressed = 0;
    Dogs102x6_clearScreen();
}

/***************************************************************************//**
 * @brief  Allows the user to adjust the backlight brightness
 * @param  none
 * @return none
 ******************************************************************************/

void BacklightSetting(void)
{
    uint8_t brightness = *((unsigned char *)brightnessSetpointAddress);

    buttonsPressed = 0;
    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(0, 0, "Adjust brightness", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(1, 0, "by turning wheel.", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_imageDraw(lightbulb, 2, 35);                   //显示灯泡图案

    while (!buttonsPressed)
    {
        brightness = Wheel_getValue() / 340;                 //通过齿轮电位计采样得到需设置的背光参数
        if (brightness > 12)
            brightness = 12;                                 //背光参数最大为12
        Dogs102x6_setBacklight(brightness);                  //重置背光值
    }

    WriteFlashSettings(brightness, brightnessSetpointAddress); //将新背光值写入FLASH地址，保存设置

    buttonsPressed = 0;
    Dogs102x6_clearScreen();
}

/***************************************************************************//**
 * @brief  Updates the hours, minutes and seconds on the digital clock
 * @param  None
 * @return none
 ******************************************************************************/

void DigitalClockUpdate(void)
{
    uint8_t hourBCD   = RTCHOUR;
    uint8_t minuteBCD = RTCMIN;
    uint8_t secondBCD = RTCSEC;
    uint8_t dayBCD    = RTCDAY;
    uint8_t monthBCD  = RTCMON;
    uint8_t yearBCD   = RTCYEAR;
    uint8_t i;

    for (i = 0; i < 8; i++){
        timeOld[i] = time[i];
    }

    time[HOUR10]   = '0' + (hourBCD   >> 4);
    time[HOUR01]   = '0' + (hourBCD   &  0x0F);
    time[MINUTE10] = '0' + (minuteBCD >> 4);
    time[MINUTE01] = '0' + (minuteBCD &  0x0F);
    time[SECOND10] = '0' + (secondBCD >> 4);
    time[SECOND01] = '0' + (secondBCD &  0x0F);

    date[MONTH10]  = '0' + (monthBCD  >> 4);
    date[MONTH01]  = '0' + (monthBCD  &  0x0F);
    date[DAY10]    = '0' + (dayBCD    >> 4);
    date[DAY01]    = '0' + (dayBCD    &  0x0F);
    date[YEAR10]   = '0' + (yearBCD   >> 4);
    date[YEAR01]   = '0' + (yearBCD   &  0x0F);
}

/***************************************************************************//**
 * @brief  Displays the digital clock
 * @param  None
 * @return none
 ******************************************************************************/

void DisplayDigitalClock(void)
{
    buttonsPressed = 0;
    Dogs102x6_clearScreen();
    Dogs102x6_imageDraw(digitalWatch, 0, 0);                   //绘制闹钟图案
    while (!buttonsPressed)
    {
        hourBCD = RTCHOUR;
        minuteBCD = RTCMIN;
        secondBCD = RTCSEC;                                    //读取RTC时间寄存器
        if (hourBCD != hourOldBCD || minuteBCD != minuteOldBCD ||
            secondBCD != secondOldBCD)
        {
            DigitalClockDraw();                                //显示当前数字时间
        }
        hourOldBCD = hourBCD;
        minuteOldBCD = minuteBCD;
        secondOldBCD = secondBCD;
    }
    buttonsPressed = 0;
    Dogs102x6_clearScreen();
}

/***************************************************************************//**
 * @brief  Draws the updated digital clock.
 * @param  None
 * @return none
 ******************************************************************************/

void DigitalClockDraw(void)
{
    DigitalClockUpdate();
    Dogs102x6_clearScreen();
    Dogs102x6_imageDraw(digitalWatch, 0, 0);
    Dogs102x6_stringDrawXY(27, 20, &time[HOUR10], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDrawXY(27, 39, &date[MONTH10], DOGS102x6_DRAW_NORMAL);
}

/***************************************************************************//**
 * @brief  Draws the specified hand on the analog clock.
 * @param  time  The time to be represented by the hand (0 - 59)
 * @param  pixel Draw dark or light pixels
 * @param  hand  Which hand is being drawn.
 *
 * - 1 - Second hand
 * - 2 - Minute hand
 * - 3 - Hour   hand
 *
 * @return none
 ******************************************************************************/

void AnalogClockDrawHand(uint8_t time, uint8_t pixel, uint8_t hand)
{
    int8_t handX, handY, i;

    if (time < 15)
    {
        handY = ClockY - cos[(hand << 4) +  time];
        handX = ClockX + cos[(hand << 4) + 15 - time];
    }
    else if (time < 30)
    {
        handY = ClockY + cos[(hand << 4) + 30 - time];
        handX = ClockX + cos[(hand << 4) + time - 15];
    }
    else if (time < 45)
    {
        handY = ClockY + cos[(hand << 4) + time - 30];
        handX = ClockX - cos[(hand << 4) + 45 - time];
    }
    else
    {
        handY =  ClockY - cos[(hand << 4) + 60 - time];
        handX =  ClockX - cos[(hand << 4) + time - 45];
    }

    for (i = -1; i < hand; i++)
    {
        if (hand > 0 && i == -1){
            continue;
        }
        Dogs102x6_lineDraw(ClockX + i, ClockY - i, handX + i, handY - i, pixel);
    }
}

/***************************************************************************//**
 * @brief  Initializes the analog clock for display
 * @param  none
 * @return none
 ******************************************************************************/

void AnalogClockInit(void)
{
    uint8_t i;

    Dogs102x6_clearScreen();
    Dogs102x6_circleDraw(51, 32, 30, DOGS102x6_DRAW_NORMAL);    //Draw clock face
    Dogs102x6_circleDraw(51, 32, 28, DOGS102x6_DRAW_NORMAL);

    for (i = 1; i < 12; i++)                                    //Draw hour lines on clock face
    {
        if ((i != 3) && (i != 6) && (i != 9))
        {
            AnalogClockDrawHand(i * 5, DOGS102x6_DRAW_NORMAL, 0);
        }
    }

    Dogs102x6_clearImage(4, 28, 2, ClockX - 14);                //Clear out center of clock face for
                                                                // hands
    Dogs102x6_stringDrawXY(45, 6, "12", DOGS102x6_DRAW_NORMAL); //Draw numerals on clock face
    Dogs102x6_charDrawXY(72, 30, '3', DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDrawXY(50, 52, '6', DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDrawXY(26, 30, '9', DOGS102x6_DRAW_NORMAL);
}

/***************************************************************************//**
 * @brief  Updates the analog clock display due to a time event.
 * @param  none
 * @return none
 ******************************************************************************/

void AnalogClockUpdate(void)
{
    uint8_t hour   = RTCHOUR;
    uint8_t minute = RTCMIN;
    uint8_t second = RTCSEC;

    uint8_t hourOld;
    uint8_t minuteOld;
    uint8_t secondOld;

    hourBCD   = RTCHOUR;
    minuteBCD = RTCMIN;
    secondBCD = RTCSEC;

    second = secondBCD & 0xF0;              //high digit
    secondOld = secondOldBCD & 0xF0;
    //value = 16*times - 4*times - 2 times = 10*times
    second = second - (second >> 2) - (second >> 3) + (secondBCD & 0x0F);
    secondOld = secondOld - (secondOld >> 2) - (secondOld >> 3) + (secondOldBCD & 0x0F);

    minute = minuteBCD & 0xF0;              //high digit
    minuteOld = minuteOldBCD & 0xF0;
    //value = 16*times - 4*times - 2 times = 10*times
    minute = minute - (minute >> 2) - (minute >> 3) + (minuteBCD & 0x0F);
    minuteOld = minuteOld - (minuteOld >> 2) - (minuteOld >> 3) + (minuteOldBCD & 0x0F);

    hour   = hourBCD & 0xF0;
    hourOld   = hourOldBCD & 0xF0;
    //value = 16*times - 4*times - 2 times = 10*times
    hour   = hour - (hour >> 2) - (hour >> 3) + (hourBCD & 0x0F);
    hourOld   = hourOld - (hourOld >> 2) - (hourOld >> 3) + (hourOldBCD & 0x0F);

    //Erase old hand positions, if necessary
    if (second != secondOld){
        AnalogClockDrawHand(secondOld, DOGS102x6_DRAW_INVERT,  1);
    }
    if (minute != minuteOld){
        AnalogClockDrawHand(minuteOld, DOGS102x6_DRAW_INVERT,  2);
    }
    if (hour != hourOld){
        AnalogClockDrawHand(hourOld * 5, DOGS102x6_DRAW_INVERT,  3);
    }

    //redraw numbers since they may have been partially erased by the hands
    Dogs102x6_stringDrawXY(45, 6, "12", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDrawXY(72, 30, '3', DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDrawXY(50, 52, '6', DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDrawXY(26, 30, '9', DOGS102x6_DRAW_NORMAL);

    //Draw current hand positions
    AnalogClockDrawHand(second, DOGS102x6_DRAW_NORMAL, 1);
    AnalogClockDrawHand(minute, DOGS102x6_DRAW_NORMAL, 2);
    if (hour >= 12){
        AnalogClockDrawHand((hour - 12) * 5, DOGS102x6_DRAW_NORMAL, 3);
    }
    else {
        AnalogClockDrawHand(hour * 5, DOGS102x6_DRAW_NORMAL, 3);
    }

    secondOldBCD = secondBCD;
    minuteOldBCD = minuteBCD;
    hourOldBCD   = hourBCD;
}

/***************************************************************************//**
 * @brief  Executes the "Analog Clock" menu option in the User Experience
 *         example code.
 * @param  none
 * @return none
 ******************************************************************************/

void DisplayAnalogClock(void)
{
    AnalogClockInit();               //模拟时钟初始化

    __enable_interrupt();            //中断使能
    RTCExitSec = 1;
    buttonsPressed = 0;

    while (!buttonsPressed)         //若有按键按下，则退出
    {
        buttonsPressed = 0;
        AnalogClockUpdate();        //模拟时钟更新
    }

    RTCExitSec = 0;
    buttonsPressed = 0;
    Dogs102x6_clearScreen();
}

/***************************************************************************//**
 * @brief  Executes the user interface for setting the time.
 *
 *         Use the wheel to set the current digit, and button S1 to change
 *         cycle through hours, minutes, and seconds.
 * @param  none
 * @return none
 ******************************************************************************/

void SetTime(void)
{
    uint8_t horizontalPosition = 0;
    uint8_t verticalPosition = 0;
    char text[3] = {'0', '0', '\0'};
    uint16_t wheelValue = 0;
    uint16_t intialWheelValue = 0;

    Dogs102x6_clearScreen();

    DigitalClockUpdate();                         //数字时间更新

    //显示初试时间  04:30:00
    Dogs102x6_charDraw(3, 26, time[HOUR10], DOGS102x6_DRAW_INVERT);
    Dogs102x6_charDraw(3, 32, time[HOUR01], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(3, 38, time[COLON2], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(3, 44, time[MINUTE10], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(3, 50, time[MINUTE01], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(3, 56, time[COLON1], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(3, 62, time[SECOND10], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(3, 68, time[SECOND01], DOGS102x6_DRAW_NORMAL);

    Dogs102x6_charDraw(4, 26, date[MONTH10], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(4, 32, date[MONTH01], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(4, 38, date[SLASH2], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(4, 44, date[DAY10], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(4, 50, date[DAY01], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(4, 56, date[SLASH1], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(4, 62, date[YEAR10], DOGS102x6_DRAW_NORMAL);
    Dogs102x6_charDraw(4, 68, date[YEAR01], DOGS102x6_DRAW_NORMAL);

    Buttons_interruptEnable(BUTTON_ALL);       //按键中断使能
    buttonsPressed = 0;

    intialWheelValue = Wheel_getValue();       // 得到初始齿轮电位计采样值


    text[0] = '0' + ((0xF0 & RTCHOUR) >> 4);
    text[1] = '0' + (0x0F & RTCHOUR);

    while (!(buttonsPressed & BUTTON_S2))      // 按下S2退出该段程序
    {
        switch (horizontalPosition)            // 通过设置选项值，判断进入何种设置（时[0]、分[1]、秒[2]、月[3]、日[4]、年[5]？）

        {
            case 0:
                Dogs102x6_stringDraw(3, 26, &text[0], DOGS102x6_DRAW_INVERT);  //显示小时数
                wheelValue = Wheel_getValue();                  //得到当前齿轮电位计采样值
                verticalPosition = wheelValue / 163;            //得到需设置的小时数

                if (intialWheelValue != wheelValue){            //若当前齿轮电位计采样值与初始齿轮电位计采样值不同，进行小时重置
                    if (verticalPosition < 10){
                        text[0] = '0';
                        text[1] = '0' + verticalPosition;
                    }
                    else if (verticalPosition < 20){
                        text[0] = '1';
                        text[1] = '0' + (verticalPosition - 10);
                    }
                    else if (verticalPosition < 24){
                        text[0] = '2';
                        text[1] = '0' + (verticalPosition - 20);
                    }
                    else {
                        text[0] = '2';
                        text[1] = '3';
                    }
                }

                if (buttonsPressed & BUTTON_S1){         // 若S1确认键被按下，对当前设置值进行保存，并进入到下个设置项

                    RTCHOUR = (text[0] - '0') << 4;
                    RTCHOUR = (0xF0 & RTCHOUR) | (0x0F & (text[1] - '0'));
                    horizontalPosition++;
                    intialWheelValue = Wheel_getValue(); // Get the initial wheel position for the
                                                         // next group
                    text[0] = '0' + ((0xF0 & RTCMIN) >> 4);
                    text[1] = '0' + (0x0F & RTCMIN);
                    buttonsPressed = 0;
                }
                break;

            case 1:
                Dogs102x6_stringDraw(3, 44, &text[0], DOGS102x6_DRAW_INVERT);
                wheelValue = Wheel_getValue();           // 得到当前齿轮电位计值
                verticalPosition = wheelValue / 68;

                if (intialWheelValue != wheelValue){     //若当前齿轮电位计采样值与初始齿轮电位计采样值不同，进行分钟重置
                    if (verticalPosition < 10){
                        text[0] = '0';
                        text[1] = '0' + verticalPosition;
                    }
                    else if (verticalPosition < 20){
                        text[0] = '1';
                        text[1] = '0' + (verticalPosition - 10);
                    }
                    else if (verticalPosition < 30){
                        text[0] = '2';
                        text[1] = '0' + (verticalPosition - 20);
                    }
                    else if (verticalPosition < 40){
                        text[0] = '3';
                        text[1] = '0' + (verticalPosition - 30);
                    }
                    else if (verticalPosition < 50){
                        text[0] = '4';
                        text[1] = '0' + (verticalPosition - 40);
                    }
                    else if (verticalPosition < 60){
                        text[0] = '5';
                        text[1] = '0' + (verticalPosition - 50);
                    }
                    else {
                        text[0] = '5';
                        text[1] = '9';
                    }
                }

                if (buttonsPressed & BUTTON_S1){         // 若S1确认键被按下，对当前设置值进行保存，并进入到下个设置项

                    RTCMIN = (text[0] - '0') << 4;
                    RTCMIN = (0xF0 & RTCMIN) | (0x0F & (text[1] - '0'));
                    horizontalPosition++;
                    intialWheelValue = Wheel_getValue(); // Get the initial wheel position for the
                                                         // next group
                    text[0] = '0' + ((0xF0 & RTCSEC) >> 4);
                    text[1] = '0' + (0x0F & RTCSEC);
                    buttonsPressed = 0;
                }
                break;

            case 2:
                Dogs102x6_stringDraw(3, 62, &text[0], DOGS102x6_DRAW_INVERT);
                wheelValue = Wheel_getValue();           // 得到当前齿轮电位计采样值
                verticalPosition = wheelValue / 68;

                if (intialWheelValue != wheelValue){     //若当前齿轮电位计采样值与初始齿轮电位计采样值不同，进行秒数重置
                    if (verticalPosition < 10){
                        text[0] = '0';
                        text[1] = '0' + verticalPosition;
                    }
                    else if (verticalPosition < 20){
                        text[0] = '1';
                        text[1] = '0' + (verticalPosition - 10);
                    }
                    else if (verticalPosition < 30){
                        text[0] = '2';
                        text[1] = '0' + (verticalPosition - 20);
                    }
                    else if (verticalPosition < 40){
                        text[0] = '3';
                        text[1] = '0' + (verticalPosition - 30);
                    }
                    else if (verticalPosition < 50){
                        text[0] = '4';
                        text[1] = '0' + (verticalPosition - 40);
                    }
                    else if (verticalPosition < 60){
                        text[0] = '5';
                        text[1] = '0' + (verticalPosition - 50);
                    }
                    else {
                        text[0] = '5';
                        text[1] = '9';
                    }
                }

                if (buttonsPressed & BUTTON_S1){         // 若S1确认键被按下，对当前设置值进行保存，并进入到下个设置项

                    RTCSEC = (text[0] - '0') << 4;
                    RTCSEC = (0xF0 & RTCSEC) | (0x0F & (text[1] - '0'));
                    horizontalPosition++;
                    intialWheelValue = Wheel_getValue(); // Get the initial wheel position for the
                                                         // next group
                    text[0] = '0' + ((0xF0 & RTCMON) >> 4);
                    text[1] = '0' + (0x0F & RTCMON);
                    buttonsPressed = 0;
                }
                break;

            case 3:
                Dogs102x6_stringDraw(4, 26, &text[0], DOGS102x6_DRAW_INVERT);
                wheelValue = Wheel_getValue();           //得到当前齿轮电位计采样值
                verticalPosition = wheelValue / 330;

                if (intialWheelValue != wheelValue){      //若当前齿轮电位计采样值与初始齿轮电位计采样值不同，进行月份重置
                    if (verticalPosition == 0){
                        text[0] = '0';
                        text[1] = '1';
                    }
                    else if (verticalPosition < 10){
                        text[0] = '0';
                        text[1] = '0' + verticalPosition;
                    }
                    else if (verticalPosition < 13){
                        text[0] = '1';
                        text[1] = '0' + (verticalPosition - 10);
                    }
                    else {
                        text[0] = '1';
                        text[1] = '2';
                    }
                }

                if (buttonsPressed & BUTTON_S1){         // 若S1确认键被按下，对当前设置值进行保存，并进入到下个设置项

                    RTCMON = (text[0] - '0') << 4;
                    RTCMON = (0xF0 & RTCMON) | (0x0F & (text[1] - '0'));
                    horizontalPosition++;
                    intialWheelValue = Wheel_getValue(); // Get the initial wheel position for the
                                                         // next group
                    text[0] = '0' + ((0xF0 & RTCDAY) >> 4);
                    text[1] = '0' + (0x0F & RTCDAY);
                    buttonsPressed = 0;
                }
                break;

            case 4:
                Dogs102x6_stringDraw(4, 44, &text[0], DOGS102x6_DRAW_INVERT);
                wheelValue = Wheel_getValue();           // 得到当前齿轮电位计采样值
                verticalPosition = wheelValue / 128;

                if (intialWheelValue != wheelValue){     //若当前齿轮电位计采样值与初始齿轮电位计采样值不同，进行日数重置
                    if (RTCMON == 0x02){

                        if (verticalPosition == 0){
                            text[0] = '0';
                            text[1] = '1';
                        }
                        else if (verticalPosition < 10){
                            text[0] = '0';
                            text[1] = '0' + verticalPosition;
                        }
                        else if (verticalPosition < 20){
                            text[0] = '1';
                            text[1] = '0' + (verticalPosition - 10);
                        }
                        else if (verticalPosition < 29){
                            text[0] = '2';
                            text[1] = '0' + (verticalPosition - 20);
                        }
                        else {
                            text[0] = '2';
                            text[1] = '8';
                        }
                    }
                    else if ((RTCMON == 0x01) || (RTCMON == 0x03) || (RTCMON == 0x05) ||
                             (RTCMON == 0x07) || (RTCMON == 0x08) || (RTCMON == 0x10) ||
                             (RTCMON == 0x12)){

                        if (verticalPosition == 0){
                            text[0] = '0';
                            text[1] = '1';
                        }
                        else if (verticalPosition < 10){
                            text[0] = '0';
                            text[1] = '0' + verticalPosition;
                        }
                        else if (verticalPosition < 20){
                            text[0] = '1';
                            text[1] = '0' + (verticalPosition - 10);
                        }
                        else if (verticalPosition < 30){
                            text[0] = '2';
                            text[1] = '0' + (verticalPosition - 20);
                        }
                        else if (verticalPosition < 32){
                            text[0] = '3';
                            text[1] = '0' + (verticalPosition - 30);
                        }
                        else {
                            text[0] = '3';
                            text[1] = '1';
                        }
                    }
                    else {
                        if (verticalPosition == 0){
                            text[0] = '0';
                            text[1] = '1';
                        }
                        else if (verticalPosition < 10){
                            text[0] = '0';
                            text[1] = '0' + verticalPosition;
                        }
                        else if (verticalPosition < 20){
                            text[0] = '1';
                            text[1] = '0' + (verticalPosition - 10);
                        }
                        else if (verticalPosition < 30){
                            text[0] = '2';
                            text[1] = '0' + (verticalPosition - 20);
                        }
                        else if (verticalPosition < 31){
                            text[0] = '3';
                            text[1] = '0' + (verticalPosition - 30);
                        }
                        else {
                            text[0] = '3';
                            text[1] = '0';
                        }
                    }
                }

                if (buttonsPressed & BUTTON_S1){         // 若S1确认键被按下，对当前设置值进行保存，并进入到下个设置项

                    RTCDAY = (text[0] - '0') << 4;
                    RTCDAY = (0xF0 & RTCDAY) | (0x0F & (text[1] - '0'));
                    horizontalPosition++;
                    intialWheelValue = Wheel_getValue(); // Get the initial wheel position for the
                                                         // next group
                    text[0] = '0' + ((0xF0 & RTCYEAR) >> 4);
                    text[1] = '0' + (0x0F & RTCYEAR);
                    buttonsPressed = 0;
                }
                break;

            case 5:
                Dogs102x6_stringDraw(4, 62, &text[0], DOGS102x6_DRAW_INVERT);
                wheelValue = Wheel_getValue();           // 得到当前齿轮电位计采样值
                verticalPosition = wheelValue / 130;

                if (intialWheelValue != wheelValue){
                    if (verticalPosition < 10){
                        text[0] = '0';
                        text[1] = '0' + verticalPosition;
                    }
                    else if (verticalPosition < 20){
                        text[0] = '1';
                        text[1] = '0' + (verticalPosition - 10);
                    }
                    else if (verticalPosition < 30){
                        text[0] = '2';
                        text[1] = '0' + (verticalPosition - 20);
                    }
                    else {
                        text[0] = '3';
                        text[1] = '0';
                    }
                }

                if (buttonsPressed & BUTTON_S1){          // 若S1确认键被按下，对当前设置值进行保存，并进入到下个设置项



                    RTCYEAR = 0x2000;                                        // Year 20xx
                    RTCYEAR = (0xFF00 & RTCYEAR) | ((text[0] - '0') << 4);   // Year 20x0
                    RTCYEAR = (0xFFF0 & RTCYEAR) | (0x0F & (text[1] - '0')); // Year 200x
                    horizontalPosition++;
                    buttonsPressed = 0;
                }

                break;

            default: break;
        }
        //数字时钟更新显示
        DigitalClockUpdate();

        if (horizontalPosition != 0){
            Dogs102x6_charDraw(3, 26, time[HOUR10], DOGS102x6_DRAW_NORMAL);
            Dogs102x6_charDraw(3, 32, time[HOUR01], DOGS102x6_DRAW_NORMAL);
        }
        if (horizontalPosition != 1){
            Dogs102x6_charDraw(3, 44, time[MINUTE10], DOGS102x6_DRAW_NORMAL);
            Dogs102x6_charDraw(3, 50, time[MINUTE01], DOGS102x6_DRAW_NORMAL);
        }
        if (horizontalPosition != 2){
            Dogs102x6_charDraw(3, 62, time[SECOND10], DOGS102x6_DRAW_NORMAL);
            Dogs102x6_charDraw(3, 68, time[SECOND01], DOGS102x6_DRAW_NORMAL);
        }
        if (horizontalPosition != 3){
            Dogs102x6_charDraw(4, 26, date[MONTH10], DOGS102x6_DRAW_NORMAL);
            Dogs102x6_charDraw(4, 32, date[MONTH01], DOGS102x6_DRAW_NORMAL);
        }
        if (horizontalPosition != 4){
            Dogs102x6_charDraw(4, 44, date[DAY10], DOGS102x6_DRAW_NORMAL);
            Dogs102x6_charDraw(4, 50, date[DAY01], DOGS102x6_DRAW_NORMAL);
        }
        if (horizontalPosition != 5){
            Dogs102x6_charDraw(4, 62, date[YEAR10], DOGS102x6_DRAW_NORMAL);
            Dogs102x6_charDraw(4, 68, date[YEAR01], DOGS102x6_DRAW_NORMAL);
        }

        if (horizontalPosition > 5)
            break;                                       // 完成时钟设置

    }
    buttonsPressed = 0;
    Dogs102x6_clearScreen();
}

/***************************************************************************//**
 * @brief  Initializes the RTC calendar.
 * Initial values are January 01, 2010, 12:30:05
 * @param  none
 * @return none
 ******************************************************************************/

void SetupRTC(void)
{
    RTCCTL01 = RTCMODE + RTCBCD + RTCHOLD + RTCTEV_1;

    RTCHOUR = 0x04;
    RTCMIN = 0x30;
    RTCSEC = 0x00;
    RTCDAY = 0x01;
    RTCMON = 0x01;
    RTCYEAR = 0x2011;

    RTCCTL01 &= ~RTCHOLD;

   // RTCPS1CTL = RT1IP_5;                    // Interrupt freq: 2Hz
    //RTCPS0CTL = RT0IP_7;                    // Interrupt freq: 128hz

    RTCCTL0 |= RTCRDYIE + RTCTEVIE;         // Enable interrupt
}

/***************************************************************************//**
 * @brief  RTC Interrupt Service Routine. Handles time events.
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void)
{
    switch (__even_in_range(RTCIV, RTC_RT1PSIFG))
    {
        // Vector RTC_NONE: No Interrupt pending
        case RTC_NONE:
            break;

        // Vector RTC_RTCRDYIFG: RTC ready
        case RTC_RTCRDYIFG:
            if (RTCExitSec == 1)
                __bic_SR_register_on_exit(LPM3_bits);
            break;

        // Vector RTC_RTCTEVIFG: RTC interval timer
        case RTC_RTCTEVIFG:
            break;

        // Vector RTC_RTCAIFG: RTC user alarm
        case RTC_RTCAIFG:
            break;

        // Vector RTC_RT0PSIFG: RTC prescaler 0
        case RTC_RT0PSIFG:
            break;

        // Vector RTC_RT1PSIFG: RTC prescaler 1
        case RTC_RT1PSIFG:
            break;

        // Default case
        default:
            break;
    }
    __no_operation();
}

/***************************************************************************//**
 * @brief  Writes the settings to flash
 * @param  Data to be written, and offset
 * @return none
 ******************************************************************************/

void WriteFlashSettings(uint16_t Data, uint16_t Address)
{
    uint16_t * Flash_ptr;                   // Initialize Flash pointer
    uint16_t Flash_Contents[16];            // Store Contents of Flash before Programming
    uint8_t i;

    Flash_ptr = (uint16_t *)(0x1880);       // Info C

    for (i = 0; i < 16; i++)                // We know that we will only use > 8 variables in Info C
    {
        Flash_Contents[i] = *Flash_ptr;     // Read a word from flash
        if (Flash_ptr == (uint16_t *)Address)
            Flash_Contents[i] = Data;
        *Flash_ptr++;
    }


    Flash_ptr = (uint16_t *)(0x1880);       // Info C
    FCTL3 = FWKEY;                          // Clear Lock bit
    FCTL1 = FWKEY + ERASE;                  // Set Erase bit
    *Flash_ptr = 0;                         // Dummy write to erase Flash seg
    FCTL1 = FWKEY + WRT;                    // Set WRT bit for write operation

    for (i = 0; i < 16; i++)
    {
        *Flash_ptr++ = Flash_Contents[i];   // Write a word to flash
    }

    FCTL1 = FWKEY;                          // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                   // Set LOCK bit
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
