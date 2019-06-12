//***************************************************************************//
//                                                                           //
//                         MSP-EXP430F5529 LAB CODE                          //
//                                                                           //
//                              lab4 - EchoUSB                               //
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
#include <stdlib.h>
#include <string.h>
#include "msp430.h"
#include "HAL_PMM.h"
#include "HAL_UCS.h"
#include "HAL_Buttons.h"
#include "HAL_Dogs102x6.h"
#include "HAL_Board.h"
#include "HAL_Menu.h"
#include "MassStorage.h"
#include "descriptors.h"
#include "usb.h"                            // USB-specific functions
#include "UsbCdc.h"
#include "lab4.h"
#include "MassStorageCommon.h"
#include "usbConstructs.h"
#include "diskio.h"

#define BUFFER_SIZE         130
uint8_t dataBuffer[BUFFER_SIZE];            // User buffer to hold the data

/***************************************************************************//**
 * @brief  Echoes everything the user types in the terminal back to the
 *         terminal, and prints to the LCD.
 * @param  none
 * @return none
 ******************************************************************************/

void lab4(void)
{
    WORD bytesReceived;
    uint8_t row = 2;
    uint8_t col = 0;
    uint8_t i = 0;

    Dogs102x6_clearScreen();
    buttonsPressed = 0;
    Dogs102x6_stringDraw(0, 0, "===LAB4:EchoUSB==", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(3, 0, " Press button S1 ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(4, 0, "  to start LAB4. ", DOGS102x6_DRAW_NORMAL);
    while (!(buttonsPressed & BUTTON_S1)) ;                  //等待S1键被按下

    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(0, 0, "Type on PC.      ", DOGS102x6_DRAW_INVERT);
    Dogs102x6_stringDraw(1, 0, "Press S2 to quit.", DOGS102x6_DRAW_INVERT);

    SFRIE1 &= ~OFIE;
    disk_initialize(0);                   // Initialize Disk Drive #0

    SFRIE1 |= OFIE;

    ClockUSB();                           //USB时钟初始化
    USB_init();                           //USB初始化

    // 使能各种USB事件处理程序
    USB_setEnabledEvents(kUSB_allUsbEvents);

    msc_Init();
    // If USB is already connected when the program starts up, then there won't
    // be a USB_handleVbusOnEvent(). So we need to check for it, and manually
    // connect if the host is already present.
    if (USB_connectionInfo() & kUSB_vbusPresent)
    {
        if (USB_enable() == kUSB_succeed)
        {
            USB_reset();
            USB_connect();
        }
    }

    while (!(buttonsPressed & BUTTON_S2))
    {
        // Check the USB state and loop accordingly
        switch (USB_connectionState())
        {
            case  ST_USB_DISCONNECTED:
                // Enter LPM3 until USB is connected
                __bis_SR_register(LPM3_bits + GIE);
                __no_operation();
                break;

            case  ST_USB_CONNECTED_NO_ENUM:
                __no_operation();
                break;

            case  ST_ENUM_ACTIVE:
            	 msc_Loop();
                //得到接收数据字节数
                bytesReceived = cdcReceiveDataInBuffer(dataBuffer, BUFFER_SIZE,
                                                       CDC0_INTFNUM);

                if (bytesReceived){
                    // 后台发送数据
                    cdcSendDataInBackground(dataBuffer, bytesReceived,
                                            CDC0_INTFNUM, 0);

                    // 数据LCD显示
                    for (i = 0; i < bytesReceived; i++)
                    {
                        Dogs102x6_charDraw(row,
                                           col,
                                           dataBuffer[i],
                                           DOGS102x6_DRAW_NORMAL);
                        col += 6;
                        if (col > 101)
                        {
                            if (row < 7)
                            {
                                row++;
                                col = 0;
                            }
                            else
                            {
                                row = 2;
                                col = 0;
                            }
                        }
                    }
                }
                break;

            case ST_NOENUM_SUSPENDED:
                __bis_SR_register(LPM3_bits + GIE);
                __no_operation();
                break;

            case  ST_ENUM_SUSPENDED:
                Board_ledOff(LED1);
                // Enter LPM3 until a resume or VBUS-off event
                __bis_SR_register(LPM3_bits + GIE);
                __no_operation();
                break;

            case ST_ENUM_IN_PROGRESS:
                __no_operation();
                break;

            case  ST_ERROR:
                __no_operation();
                break;

            default:;
        }
    }

    USB_disable();                        //禁用USB模块
    SFRIE1 &= ~OFIE;
    Init_FLL_Settle(25000, 762);          // 返回25MHZ系统时钟设置
    SFRIE1 |= OFIE;
    buttonsPressed = 0;
    Dogs102x6_clearScreen();
}

/***************************************************************************//**
 * @brief  Handles UNMI interrupts
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR(void)
{
    switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG))
    {
        case SYSUNIV_NONE:
            __no_operation();
            break;
        case SYSUNIV_NMIIFG:
            __no_operation();
            break;
        case SYSUNIV_OFIFG:
            UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT2OFFG); // Clear OSC flaut Flags fault flags
            SFRIFG1 &= ~OFIFG;                          // Clear OFIFG fault flag
            break;
        case SYSUNIV_ACCVIFG:
            __no_operation();
            break;
        case SYSUNIV_BUSIFG:
            // If bus error occurred - the cleaning of flag and re-initializing of USB is required.
            SYSBERRIV = 0;                              // clear bus error flag
            USB_disable();                              // Disable
    }
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
