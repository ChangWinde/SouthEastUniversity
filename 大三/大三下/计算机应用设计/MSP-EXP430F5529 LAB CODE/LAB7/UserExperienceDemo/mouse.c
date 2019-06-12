/*******************************************************************************
 *
 mouse.c - Uses the MSP430 as a mouse for the PC using the HID stack
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

/***************************************************************************//**
 * @file       Mouse.c
 * @addtogroup Mouse
 * @{
 ******************************************************************************/
#include <stdint.h>
#include "HAL_UCS.h"
#include "HAL_PMM.h"
#include "HAL_Board.h"
#include "HAL_Buttons.h"
#include "HAL_Cma3000.h"
#include "HAL_Dogs102x6.h"
#include "mouse.h"
#include "device.h"
#include "types.h" // Basic Type declarations
#include "descriptors.h"
#include "usb.h"   // USB-specific functions
#include "UsbHid.h"
#include "MassStorageCommon.h"

// This image has been created by ImageDog using 'small_mouse_2.png'
static const uint8_t mouseImage[] =
{
    0x20,          // Image width in pixels
    0x04,          // Image height in rows (1 row = 8 pixels)
    0x00, 0x38, 0x7c, 0xc6, 0xc6, 0xc3, 0xc3, 0xc1, 0x01, 0x01, 0x02, 0x03, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0f, 0x10, 0x20, 0xc0, 0x81, 0x02, 0x04, 0x09, 0x12, 0xa4, 0xa8, 0x40, 0x40,
    0x40, 0x40, 0x40, 0xc0, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x7f, 0x1f, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0x18, 0x04, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x07, 0x07, 0x0f, 0x0e, 0x1e, 0x3c, 0xf8, 0xf0, 0xc0, 0x00, 0x00
};

typedef struct
{
    uint8_t buttons;
    uint8_t dX;
    uint8_t dY;
    uint8_t dZ;
} MOUSE_REPORT;

MOUSE_REPORT mouseReport = { 0, 0, 0, 0 };  // HID report, to be sent to the PC.



uint8_t sendNewMousePosition = FALSE;       // Flag by which timer tells main
                                            // loop to send a new report

const char *const usbMouseText[] = {
    "Tilt board to    ",
    "move mouse on PC.",
    "Push S1 to click.",
    "Push S2 to quit. "
};

/***************************************************************************//**
 * @brief  Uses the MSP430 as a mouse for the PC using the HID stack.
 * @param  none
 * @return none
 ******************************************************************************/

void Mouse(void)
{
    uint8_t i;

    buttonsPressed = 0;

    Dogs102x6_clearScreen();
    for (i = 0; i < 4; i++)
    {
        Dogs102x6_stringDraw(i, 0, (char *)usbMouseText[i], DOGS102x6_DRAW_NORMAL);
    }
    Dogs102x6_imageDraw(mouseImage, 4, 35);           //绘制鼠标图案

    USB_init();                                       //初始化USB模块

    //使能所有USB事件
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

    // 当每次发送USB鼠标位置报告时，LED1进行闪烁
    Board_ledOff(LED1);                            //关闭LED1


    //Timer_A初始化，利用定时器TA2定时产生中断，唤醒主回路（每1/60秒）
    TA2CCTL0 = CCIE;                                // CCR0 中断使能
    TA2CCR0 = 547;                                  // 547/32768 定时每16.7ms发送一次鼠标位置报告
    TA2CTL = TASSEL_1 + TACLR;                      // ACLK, 清除定时计数器

    while (!(buttonsPressed & BUTTON_S2))
    {
        switch (USB_connectionState())
        {
            case ST_USB_DISCONNECTED:
                TA2CTL &= ~MC_1;
                __bis_SR_register(LPM3_bits + GIE); // Enter LPM3
                break;

            case ST_USB_CONNECTED_NO_ENUM:
                break;

            case ST_ENUM_ACTIVE:
                msc_Loop();

                // 开启定时器
                TA2CTL |= MC_1;

                // 进低功耗模式0，直到利用定时器中断唤醒CPU，继续执行程序
                __bis_SR_register(LPM0_bits + GIE);

                // 定时器已唤醒CPU，发送USB鼠标位置报告（主回路）
                if (sendNewMousePosition)              //若鼠标发送标志位为1，进行发送报告
                {
                    sendNewMousePosition = FALSE;      //清除鼠标发送标志位
                    // 创建鼠标位置报告
                    if (buttonsPressed & BUTTON_S1)
                    {
                        mouseReport.buttons = 1;
                        buttonsPressed &= ~BUTTON_S1;
                    }
                    else
                        mouseReport.buttons = 0;

                    mouseReport.dX = -Cma3000_xAccel;

                    mouseReport.dY = Cma3000_yAccel;  //发送加速度数据作为鼠标坐标

                    // 发送鼠标位置报告
                    USBHID_sendReport((void *)&mouseReport, HID0_INTFNUM);

                    Board_ledToggle(LED1);            // 发转LED1状态，表示一次发送完成
                }
                break;

            case ST_NOENUM_SUSPENDED:
                break;

            case ST_ENUM_SUSPENDED:
                TA2CTL &= ~MC_1;
                Board_ledOff(LED1);
                __bis_SR_register(LPM3_bits + GIE);   // Enter LPM3, until a resume or VBUS-off
                                                      // event
                break;

            case ST_ENUM_IN_PROGRESS:
                break;

            case ST_ERROR:
                break;

            default:;
        }
    }

    buttonsPressed = 0;
    USB_disable();

    // Disable accelerometer
    Cma3000_disable();

    TA2CTL &= ~MC0;                       // Stop timer
    __disable_interrupt();                // Disable interrupts globally
    SFRIE1 &= ~OFIE;
    Init_FLL_Settle(25000, 762);          // Return to normal clock settings
    SFRIE1 |= OFIE;
    Board_ledOff(LED_ALL);
    Dogs102x6_clearScreen();
}

/***************************************************************************//**
 * @brief  Handles TimerA2 interrupts to update mouse position
 * @param  none
 * @return none
 ******************************************************************************/
// TimerA2 interrupt service routine
#pragma vector=TIMER2_A0_VECTOR
__interrupt void TIMER2_A0_ISR(void)
{
    // 加速度计初始化
    Cma3000_init();

    // 读取加速度值偏移量
    Cma3000_readAccel_offset();

    // 鼠标发送标志位置1
    sendNewMousePosition = TRUE;

    // 退出低功耗模式
    __bic_SR_register_on_exit(LPM0_bits);
}



/***************************************************************************//**
 * @}
 ******************************************************************************/




