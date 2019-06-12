/*******************************************************************************
 *
 *  HAL_Dogs102x6.h
 *
 *  Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
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

#ifndef HAL_DOGS102X6_H
#define HAL_DOGS102X6_H

#include <stdint.h>

// Screen size
#define DOGS102x6_X_SIZE   102         // Display Size in dots: X-Axis
#define DOGS102x6_Y_SIZE    64         // Display Size in dots: Y-Axis

// Screen printing styles
#define DOGS102x6_DRAW_NORMAL   0x00   // Display dark pixels on a light background
#define DOGS102x6_DRAW_INVERT   0x01   // Display light pixels on a dark background

#define contrastSetpointAddress     0x1880
#define brightnessSetpointAddress   0x1882

extern uint8_t dogs102x6Memory[];      // Provide direct access to the frame buffer

extern void Dogs102x6_init(void);
extern void Dogs102x6_backlightInit(void);
extern void Dogs102x6_disable(void);
extern void Dogs102x6_writeCommand(uint8_t* sCmd, uint8_t i);
extern void Dogs102x6_writeData(uint8_t* sData, uint8_t i);
extern void Dogs102x6_setAddress(uint8_t pa, uint8_t ca);
extern uint8_t Dogs102x6_getContrast(void);
extern uint8_t Dogs102x6_getBacklight(void);
extern void Dogs102x6_setContrast(uint8_t newContrast);
extern void Dogs102x6_setBacklight(uint8_t brightness);
extern void Dogs102x6_setInverseDisplay(void);
extern void Dogs102x6_clearInverseDisplay(void);
extern void Dogs102x6_scrollLine(uint8_t lines);
extern void Dogs102x6_setAllPixelsOn(void);
extern void Dogs102x6_clearAllPixelsOn(void);
extern void Dogs102x6_clearScreen(void);
extern void Dogs102x6_charDraw(uint8_t row, uint8_t col, uint16_t f, uint8_t style);
extern void Dogs102x6_charDrawXY(uint8_t x, uint8_t y, uint16_t f, uint8_t style);
extern void Dogs102x6_stringDraw(uint8_t row, uint8_t col, char *word, uint8_t style);
extern void Dogs102x6_stringDrawXY(uint8_t x, uint8_t y, char *word, uint8_t style);
extern void Dogs102x6_clearRow(uint8_t row);
extern void Dogs102x6_pixelDraw(uint8_t x, uint8_t y, uint8_t style);
extern void Dogs102x6_horizontalLineDraw(uint8_t x1, uint8_t x2, uint8_t y, uint8_t style);
extern void Dogs102x6_verticalLineDraw(uint8_t y1, uint8_t y2, uint8_t x, uint8_t style);
extern void Dogs102x6_lineDraw(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t style);
extern void Dogs102x6_circleDraw(uint8_t x, uint8_t y, uint8_t radius, uint8_t style);
extern void Dogs102x6_imageDraw(const uint8_t IMAGE[], uint8_t row, uint8_t col);
extern void Dogs102x6_clearImage(uint8_t height, uint8_t width, uint8_t row, uint8_t col);

#endif /* HAL_DOGS102x6_H */
