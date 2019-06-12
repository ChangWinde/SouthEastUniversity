/*******************************************************************************
 *
 *  HAL_Wheel.c - Driver for the scroll wheel
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
 * @file       HAL_Wheel.c
 * @addtogroup HAL_Wheel
 * @{
 ******************************************************************************/
#include "msp430.h"
#include "HAL_Wheel.h"

#define WHEEL_PORT_DIR P8DIR
#define WHEEL_PORT_OUT P8OUT
#define WHEEL_ENABLE  BIT0
#define ADC_PORT_SEL  P6SEL
#define ADC_INPUT_A5  BIT5

uint16_t positionData;
uint16_t positionDataOld;

/***************************************************************************//**
 * @brief   Set up the wheel
 * @param   None
 * @return  None
 ******************************************************************************/

void Wheel_init(void)
{
    WHEEL_PORT_DIR |= WHEEL_ENABLE;
    WHEEL_PORT_OUT |= WHEEL_ENABLE;                    // Enable wheel

    ADC12CTL0 = ADC12SHT02 + ADC12ON;                  // Sampling time, ADC12 on
    ADC12CTL1 = ADC12SHP;                              // Use sampling timer
    ADC12MCTL0 = ADC12INCH_5;                          // Use A5 (wheel) as input
    ADC12CTL0 |= ADC12ENC;                             // Enable conversions
    ADC_PORT_SEL |= ADC_INPUT_A5;                      // P6.5 ADC option select (A5)
}

/***************************************************************************//**
 * @brief   Determine the wheel's position
 * @param   None
 * @return  Wheel position (0~7)
 ******************************************************************************/

uint8_t Wheel_getPosition(void)
{
    uint8_t position = 0;

    Wheel_getValue();
    //determine which position the wheel is in
    if (positionData > 0x0806)
        position = 7 - (positionData - 0x0806) / 260;  //scale the data for 8 different positions
    else
        position = positionData / 260;

    return position;
}

/***************************************************************************//**
 * @brief   Determine the raw voltage value across the potentiometer
 * @param   None
 * @return  Value
 ******************************************************************************/

uint16_t Wheel_getValue(void)
{
    //measure ADC value
    ADC12IE = 0x01;                                    // Enable interrupt
    ADC12CTL0 |= ADC12SC;                              // Start sampling/conversion
    __bis_SR_register(LPM0_bits + GIE);                // LPM0, ADC12_ISR will force exit
    ADC12IE = 0x00;                                    // Disable interrupt

    //add hysteresis on wheel to remove fluctuations
    if (positionData > positionDataOld)
        if ((positionData - positionDataOld) > 10)
            positionDataOld = positionData;            //use new data if change is beyond
                                                       // fluctuation threshold
        else
            positionData = positionDataOld;            //use old data if change is not beyond
                                                       // fluctuation threshold
    else
    if ((positionDataOld - positionData) > 10)
        positionDataOld = positionData;                //use new data if change is beyond
                                                       // fluctuation threshold
    else
        positionData = positionDataOld;                //use old data if change is not beyond
                                                       // fluctuation threshold

    return positionData;
}

/***************************************************************************//**
 * @brief   Disable wheel
 * @param   None
 * @return  none
 ******************************************************************************/

void Wheel_disable(void)
{
    WHEEL_PORT_OUT &= ~WHEEL_ENABLE;                   //disable wheel
    ADC12CTL0 &= ~ADC12ENC;                            // Disable conversions
    ADC12CTL0 &= ~ADC12ON;                             // ADC12 off
}

/***************************************************************************//**
 * @brief   Enable wheel
 * @param   None
 * @return  none
 ******************************************************************************/

void Wheel_enable(void)
{
    WHEEL_PORT_OUT |= WHEEL_ENABLE;                    //disable wheel
    ADC12CTL0 |= ADC12ON;                              // ADC12 on
    ADC12CTL0 |= ADC12ENC;                             // Enable conversions
}

/***************************************************************************//**
 * @brief Handles ADC interrupts.
 *
 *        Stores result of single ADC conversion for reading position of the scroll wheel.
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    switch (__even_in_range(ADC12IV, ADC12IV_ADC12IFG15))
    {
        // Vector  ADC12IV_NONE:  No interrupt
        case  ADC12IV_NONE:
            break;

        // Vector  ADC12IV_ADC12OVIFG:  ADC overflow
        case  ADC12IV_ADC12OVIFG:
            break;

        // Vector  ADC12IV_ADC12TOVIFG:  ADC timing overflow
        case  ADC12IV_ADC12TOVIFG:
            break;

        // Vector  ADC12IV_ADC12IFG0: ADC12IFG0:
        case  ADC12IV_ADC12IFG0:
            positionData = ADC12MEM0;                  // ADC12MEM = A0 > 0.5AVcc?
            __bic_SR_register_on_exit(LPM0_bits);      // Exit active CPU
            break;

        // Vector  ADC12IV_ADC12IFG1:  ADC12IFG1
        case  ADC12IV_ADC12IFG1:
            break;

        // Vector ADC12IV_ADC12IFG2:  ADC12IFG2
        case ADC12IV_ADC12IFG2:
            break;

        // Vector ADC12IV_ADC12IFG3:  ADC12IFG3
        case ADC12IV_ADC12IFG3:
            break;

        // Vector ADC12IV_ADC12IFG4:  ADC12IFG4
        case ADC12IV_ADC12IFG4:
            break;

        // Vector ADC12IV_ADC12IFG5:  ADC12IFG5
        case ADC12IV_ADC12IFG5:
            break;

        // Vector ADC12IV_ADC12IFG6:  ADC12IFG6
        case ADC12IV_ADC12IFG6:
            break;

        // Vector ADC12IV_ADC12IFG7:  ADC12IFG7
        case ADC12IV_ADC12IFG7:
            break;

        // Vector ADC12IV_ADC12IFG8:  ADC12IFG8
        case ADC12IV_ADC12IFG8:
            break;

        // Vector ADC12IV_ADC12IFG9:  ADC12IFG9
        case ADC12IV_ADC12IFG9:
            break;

        // Vector ADC12IV_ADC12IFG10:  ADC12IFG10
        case ADC12IV_ADC12IFG10:
            break;

        // Vector ADC12IV_ADC12IFG11:  ADC12IFG11
        case ADC12IV_ADC12IFG11:
            break;

        // Vector ADC12IV_ADC12IFG12:  ADC12IFG12
        case ADC12IV_ADC12IFG12:
            break;

        // Vector ADC12IV_ADC12IFG13:  ADC12IFG13
        case ADC12IV_ADC12IFG13:
            break;

        // Vector ADC12IV_ADC12IFG14:  ADC12IFG14
        case ADC12IV_ADC12IFG14:
            break;

        // Vector ADC12IV_ADC12IFG15:  ADC12IFG15
        case ADC12IV_ADC12IFG15:
            break;

        default:
            break;
    }
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
