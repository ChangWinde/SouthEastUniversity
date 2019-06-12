/*******************************************************************************
 * CTS_HAL.c - Hardware abstraction of various combinations of modules to 
 *                   perform a capacitance measurement.
 *
 *  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/ 
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
 * @file   CTS_HAL.c
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


/***************************************************************************//**
 * @addtogroup CTS_HAL
 * @{
 ******************************************************************************/

#include "CTS_HAL.h"

#ifdef RO_COMPAp_TA0_WDTp
/***************************************************************************//**
 * @brief   RO method capactiance measurement using CompA+, TimerA0, and WDT+
 *
 *      \n  Schematic Description of CompA+ forming relaxation oscillator and
 *          coupling (connection) between the relaxation oscillator and TimerA0.
 *      \n  <- Output
 *      \n  -> Input
 *      \n  R  Resistor (typically 100Kohms)
 *      \n
 *      \n               +-<-Px.y (reference)
 *      \n               |
 *      \n               R
 *      \n               |
 *      \n           +---+-->COMPA+
 *      \n           |   |
 *      \n           R   R                
 *      \n           |   |
 *      \n          GND  |
 *      \n               |                       
 *      \n               +-->TACLK
 *      \n               |
 *      \n  element-+-R--+-<-CAOUT                               
 *      \n          |
 *      \n          +------->COMPA- 
 *      \n
 *      \n  The WDT+ interval represents the measurement window.  The number of 
 *          counts within the TA0R that have accumulated during the measurement
 *          window represents the capacitance of the element.
 * 
 * @param   group Address of the structure describing the Sensor to be measured
 * @param   counts Address to where the measurements are to be written
 * @return  none
 ******************************************************************************/
void TI_CTS_RO_COMPAp_TA0_WDTp_HAL(const struct Sensor *group, uint16_t *counts)
{ 
    uint8_t i;
//** Context Save
//  Status Register: 
//  WDTp: IE1, WDTCTL
//  TIMERA0: TACTL, TACCTL1
//  COMPAp: CACTL1, CACTL2, CAPD
//  Ports: caoutDIR, caoutSel, txclkDIR, txclkSel, caoutSel2, txclkSel2, refout, refdir 
    uint8_t contextSaveSR; 
    uint8_t contextSaveIE1;
    uint16_t contextSaveWDTCTL;
    uint16_t contextSaveTACTL,contextSaveTACCTL1,contextSaveTACCR1;
    uint8_t contextSaveCACTL1,contextSaveCACTL2,contextSaveCAPD;
    uint8_t contextSaveCaoutDir,contextSaveCaoutSel;  
    uint8_t contextSavetxclkDir,contextSavetxclkSel;    
    uint8_t contextSaveRefDir,contextSaveRefOutSel;  
    #ifdef SEL2REGISTER
    uint8_t contextSaveCaoutSel2,contextSaveTxclkSel2; 
    
    contextSaveCaoutSel2 = *(group->caoutSel2Register);
    contextSaveTxclkSel2 = *(group->txclkSel2Register);       
    #endif    
    contextSaveSR = __get_SR_register();
    contextSaveIE1 = IE1;
    contextSaveWDTCTL = WDTCTL;
    contextSaveWDTCTL &= 0x00FF;
    contextSaveWDTCTL |= WDTPW;        
    contextSaveTACTL = TACTL;
    contextSaveTACCTL1 = TACCTL1;
    contextSaveTACCR1 = TACCR1;
    contextSaveCACTL1 = CACTL1;
    contextSaveCACTL2 = CACTL2;
    contextSaveCAPD = CAPD;
    contextSaveCaoutDir = *(group->caoutDirRegister);
    contextSaveCaoutSel = *(group->caoutSelRegister);  
    contextSavetxclkDir = *(group->txclkDirRegister);
    contextSavetxclkSel = *(group->txclkSelRegister);    
    contextSaveRefDir = *(group->refPxdirRegister);
    contextSaveRefOutSel = *(group->refPxoutRegister);  
    
    TACTL = TASSEL_0+MC_2;                // TACLK, cont mode
    TACCTL1 = CM_3+CCIS_2+CAP;            // Pos&Neg,GND,Cap
	  
    *(group->caoutDirRegister) |= group->caoutBits;
    *(group->txclkDirRegister) &= ~group->txclkBits;
    *(group->caoutSelRegister) |= group->caoutBits;
    *(group->txclkSelRegister) |= group->txclkBits;
    
    #ifdef SEL2REGISTER
    *(group->caoutSel2Register) |= group->caoutBits;
    *(group->txclkSel2Register) |= group->txclkBits;
    #endif
	  
    *(group->refPxdirRegister) |= group->refBits;
    *(group->refPxoutRegister) |= group->refBits;
    CACTL1 |= CAON;                       // Turn on comparator
    CAPD |= (group->capdBits); 
    IE1 |= WDTIE;                         // enable WDT interrupt
	
    for (i = 0; i<(group->numElements); i++)
    {
        CACTL2= group->refCactl2Bits + (group->arrayPtr[i])->inputBits;
	//**  Setup Gate Timer *****************************************************
	// Set duration of sensor measurment
	WDTCTL = WDTPW+WDTTMSEL+ group->measGateSource + group->accumulationCycles; 
	TACTL |= TACLR;                     // Clear Timer_A TAR
    if(group->measGateSource == GATE_WDT_ACLK)
    {
        __bis_SR_register(LPM3_bits+GIE);   // Wait for WDT interrupt
    }
    else
    {
	    __bis_SR_register(LPM0_bits+GIE);   // Wait for WDT interrupt
    }
	TACCTL1 ^= CCIS0;                   // Create SW capture of CCR1
	counts[i] = TACCR1;                 // Save result
	WDTCTL = WDTPW + WDTHOLD;           // Stop watchdog timer
    }
	  // End Sequence
      
    //** Context Restore
    //  WDTp: IE1, WDCTL
    //  TIMERA0: TACTL, TACCTL1
    //  COMPAp: CACTL1, CACTL2, CAPD
    //  Ports: caoutDIR, caoutSel, txclkDIR, txclkSel, caoutSel2, txclkSel2, refout, refdir  
    #ifdef SEL2REGISTER  
    *(group->caoutSel2Register) = contextSaveCaoutSel2;
    *(group->txclkSel2Register) = contextSaveTxclkSel2;       
    #endif    
    __bis_SR_register(contextSaveSR);   
    if(!(contextSaveSR & GIE))
    {
        __bic_SR_register(GIE);   // Wait for WDT interrupt        
    }
    IE1 = contextSaveIE1;
    WDTCTL = contextSaveWDTCTL;
    TACTL = contextSaveTACTL;
    TACCTL1 = contextSaveTACCTL1;
    TACCR1 = contextSaveTACCR1;
    CACTL1 = contextSaveCACTL1;
    CACTL2 = contextSaveCACTL2;
    CAPD = contextSaveCAPD;
    *(group->caoutDirRegister) = contextSaveCaoutDir;
    *(group->caoutSelRegister) = contextSaveCaoutSel;  
    *(group->txclkDirRegister) = contextSavetxclkDir;
    *(group->txclkSelRegister) = contextSavetxclkSel;    
    *(group->refPxdirRegister) = contextSaveRefDir;
    *(group->refPxoutRegister) = contextSaveRefOutSel;  
}
#endif

#ifdef fRO_COMPAp_TA0_SW
/***************************************************************************//**
 * @brief   RO method capactiance measurement using CompA+, TimerA0, and SW loop
 *
 *      \n  Schematic Description of CompA+ forming relaxation oscillator.
 *      \n  <- Output
 *      \n  -> Input
 *      \n  R  Resistor (typically 100Kohms)
 *      \n
 *      \n               +-<-Px.y (reference)
 *      \n               |
 *      \n               R
 *      \n               |
 *      \n           +---+-->COMPA+
 *      \n           |   |
 *      \n           R   R                
 *      \n           |   |
 *      \n          GND  |
 *      \n               |  
 *      \n               +-->TACLK                     
 *      \n               |
 *      \n  element-+-R--+-<-CAOUT                               
 *      \n          |
 *      \n          +------->COMPA- 
 *      \n
 *      \n  The timer counts to TA0CCR0 representing the measurement window. The
 *          number of counts within the SW loop that have accumulated during the
 *          measurement window represents the capacitance of the element.
 * 
 * @param   group Address of the structure describing the Sensor to be measured
 * @param   counts Address to where the measurements are to be written
 * @return  none
 ******************************************************************************/
void TI_CTS_fRO_COMPAp_TA0_SW_HAL(const struct Sensor *group, uint16_t *counts)
{ 
    uint8_t i;
    uint16_t j;
//** Context Save
//  Status Register: 
//  TIMERA0: TACTL, TACCTL0
//  COMPAp: CACTL1, CACTL2, CAPD
//  Ports: caoutDIR, caoutSel, txclkDIR, txclkSel, caoutSel2, txclkSel2, refout, refdir 
    uint16_t contextSaveTACTL,contextSaveTACCTL0,contextSaveTACCR0;
    uint8_t contextSaveCACTL1,contextSaveCACTL2,contextSaveCAPD;
    uint8_t contextSaveCaoutDir,contextSaveCaoutSel;  
    uint8_t contextSavetxclkDir,contextSavetxclkSel;    
    uint8_t contextSaveRefDir,contextSaveRefOutSel;  
    #ifdef SEL2REGISTER
    uint8_t contextSaveCaoutSel2,contextSaveTxclkSel2; 
    
    contextSaveCaoutSel2 = *(group->caoutSel2Register);
    contextSaveTxclkSel2 = *(group->txclkSel2Register);       
    #endif    
    contextSaveTACTL = TACTL;
    contextSaveTACCTL0 = TACCTL0;
    contextSaveTACCR0 = TACCR0;
    contextSaveCACTL1 = CACTL1;
    contextSaveCACTL2 = CACTL2;
    contextSaveCAPD = CAPD;
    contextSaveCaoutDir = *(group->caoutDirRegister);
    contextSaveCaoutSel = *(group->caoutSelRegister);  
    contextSavetxclkDir = *(group->txclkDirRegister);
    contextSavetxclkSel = *(group->txclkSelRegister);    
    contextSaveRefDir = *(group->refPxdirRegister);
    contextSaveRefOutSel = *(group->refPxoutRegister);      
    
//** Setup Measurement timer***************************************************
 
    // Configure Timer TA0
    TACCR0 =(group->accumulationCycles);
    TACCTL0 &= ~CAP;
    // setup connections between CAOUT and TA0
    *(group->caoutDirRegister) |= group->caoutBits;
    *(group->txclkDirRegister) &= ~group->txclkBits;
    *(group->caoutSelRegister) |= group->caoutBits;
    *(group->txclkSelRegister) |= group->txclkBits;
    
    #ifdef SEL2REGISTER
    *(group->caoutSel2Register) |= group->caoutBits;
    *(group->txclkSel2Register) |= group->txclkBits;
    #endif
    // setup reference
    *(group->refPxdirRegister) |= group->refBits;
    *(group->refPxoutRegister) |= group->refBits;
    CACTL1 |= CAON;                       // Turn on comparator
    CAPD |= (group->capdBits); 
    
    for (i = 0; i<(group->numElements); i++)
    {
       j=0;
       CACTL2= group->refCactl2Bits + (group->arrayPtr[i])->inputBits;
        //**  Setup Gate Timer **************
        // Set duration of sensor measurment
        TACTL = TASSEL_0+TACLR+MC_1;        // TACLK, reset, up mode
        TACTL &= ~TAIFG;                    // clear IFG
        while(!(TACTL & TAIFG))
        {
            j++;
        } // end accumulation
        counts[i] = j;   
    }
    // End Sequence
    //** Context Restore
    //  TIMERA0: TACTL, TACCTL1
    //  COMPAp: CACTL1, CACTL2, CAPD
    //  Ports: caoutDIR, caoutSel, txclkDIR, txclkSel, caoutSel2, txclkSel2, refout, refdir  
    #ifdef SEL2REGISTER  
    *(group->caoutSel2Register) = contextSaveCaoutSel2;
    *(group->txclkSel2Register) = contextSaveTxclkSel2;       
    #endif    
    TACTL = contextSaveTACTL;
    TACCTL0 = contextSaveTACCTL0;
    TACCR0 = contextSaveTACCR0;
    CACTL1 = contextSaveCACTL1;
    CACTL2 = contextSaveCACTL2;
    CAPD = contextSaveCAPD;
    *(group->caoutDirRegister) = contextSaveCaoutDir;
    *(group->caoutSelRegister) = contextSaveCaoutSel;  
    *(group->txclkDirRegister) = contextSavetxclkDir;
    *(group->txclkSelRegister) = contextSavetxclkSel;    
    *(group->refPxdirRegister) = contextSaveRefDir;
    *(group->refPxoutRegister) = contextSaveRefOutSel;  
}
#endif

#ifdef fRO_COMPAp_SW_TA0
/***************************************************************************//**
 * @brief   RO method capactiance measurement using CompA+, TimerA0, and SW loop
 *
 *      \n     Schematic Description of CompA+ forming relaxation oscillator.
 *      \n     <- Output
 *      \n     -> Input
 *      \n    R  Resistor (typically 100Kohms)
 *      \n 
 *      \n                  +-<-Px.y (reference)
 *      \n                  |
 *      \n                  R
 *      \n                  |
 *      \n              +---+-->COMPA+
 *      \n              |   |
 *      \n              R   R                
 *      \n              |   |
 *      \n             GND  |
 *      \n                  |  
 *      \n                  |
 *      \n     element-+-R--+-<-CAOUT                               
 *      \n             |
 *      \n             +------->COMPA- 
 *      \n 
 *      \n     The SW loop counts to 'n' accumulationCycles, representing the 
 *      \n     measurement window. The number of timer counts within TA0R register
 *      \n     represents the capacitance of the element.
 * 
 * @param   group Address of the structure describing the Sensor to be measured
 * @param   counts Address to where the measurements are to be written
 * @return  none
 ******************************************************************************/
void TI_CTS_fRO_COMPAp_SW_TA0_HAL(const struct Sensor *group, uint16_t *counts)
{ 
    uint8_t i;
    uint16_t j;
    //** Context Save
//  Status Register: 
//  TIMERA0: TACTL, TACCTL0, TACCTL1
//  COMPAp: CACTL1, CACTL2, CAPD
//  Ports: caoutDIR, caoutSel, caoutSel2, refout, refdir 
    uint16_t contextSaveTACTL,contextSaveTACCTL0,contextSaveTACCTL1;
    uint16_t contextSaveTACCR0,contextSaveTACCR1;
    uint8_t contextSaveCACTL1,contextSaveCACTL2,contextSaveCAPD;
    uint8_t contextSaveCaoutDir,contextSaveCaoutSel;  
    uint8_t contextSaveRefDir,contextSaveRefOutSel;  
    #ifdef SEL2REGISTER
    uint8_t contextSaveCaoutSel2,contextSaveTxclkSel2; 
    
    contextSaveCaoutSel2 = *(group->caoutSel2Register);
    #endif    
    contextSaveTACTL = TACTL;
    contextSaveTACCTL0 = TACCTL0;
    contextSaveTACCTL1 = TACCTL1;
    contextSaveTACCR0 = TACCR0;
    contextSaveTACCR1 = TACCR1;
    contextSaveCACTL1 = CACTL1;
    contextSaveCACTL2 = CACTL2;
    contextSaveCAPD = CAPD;
    contextSaveCaoutDir = *(group->caoutDirRegister);
    contextSaveCaoutSel = *(group->caoutSelRegister);  
    contextSaveRefDir = *(group->refPxdirRegister);
    contextSaveRefOutSel = *(group->refPxoutRegister);      
//** Setup Measurement timer***************************************************
 
    // Configure Timer TA0
    TACCTL0 = CM_3+CCIS_2+CAP;            // Pos&Neg,GND,Cap
    TACCTL1 = CM_3+CCIS_2+CAP;            // Pos&Neg,GND,Cap

    // setup connections between CAOUT and TA0
    *(group->caoutDirRegister) |= group->caoutBits;
    *(group->caoutSelRegister) |= group->caoutBits;
    
    #ifdef SEL2REGISTER
    *(group->caoutSel2Register) |= group->caoutBits;
    #endif
    // setup reference
    *(group->refPxdirRegister) |= group->refBits;
    *(group->refPxoutRegister) |= group->refBits;
    CACTL1 |= CAON;                       // Turn on comparator
    CAPD |= (group->capdBits); 
    for (i = 0; i<(group->numElements); i++)
    {
        CACTL2= group->refCactl2Bits + (group->arrayPtr[i])->inputBits;
        //**  Setup Gate Timer **************
        // Set duration of sensor measurment
        TACTL = group->measureGateSource+group->sourceScale+TACLR+MC_2;
        TACCTL0 ^= CCIS0;                     // Create SW capture of CCR0
        for(j = group->accumulationCycles; j > 0; j--)
        {
            CACTL1 &= ~CAIFG;
            while(!(CACTL1 & CAIFG));
        }
        TACCTL1 ^= CCIS0;                     // Create SW capture of CCR1
        counts[i] = TACCR1;               // Save result
        counts[i] -= TACCR0;               // Save result
        TACCTL0 &= ~CCIFG;
        TACCTL1 &= ~CCIFG;
    }
    // End Sequence
    //** Context Restore
    //  WDTp: IE1, WDCTL
    //  TIMERA0: TACTL, TACCTL0, TACCTL1, TACCR0, TACCR1
    //  COMPAp: CACTL1, CACTL2, CAPD
    //  Ports: caoutDIR, caoutSel, txclkDIR, caoutSel2, refout, refdir  
    #ifdef SEL2REGISTER  
    *(group->caoutSel2Register) = contextSaveCaoutSel2;
    #endif    
    TACTL = contextSaveTACTL;
    TACCTL0 = contextSaveTACCTL0;
    TACCTL1 = contextSaveTACCTL1;
    TACCR0 = contextSaveTACCR0;
    TACCR1 = contextSaveTACCR1;
    CACTL1 = contextSaveCACTL1;
    CACTL2 = contextSaveCACTL2;
    CAPD = contextSaveCAPD;
    *(group->caoutDirRegister) = contextSaveCaoutDir;
    *(group->caoutSelRegister) = contextSaveCaoutSel;  
    *(group->refPxdirRegister) = contextSaveRefDir;
    *(group->refPxoutRegister) = contextSaveRefOutSel;  
}
#endif

#ifdef RO_COMPAp_TA1_WDTp
/***************************************************************************//**
 * @brief   RO method capactiance measurement using CompA+, TimerA1, and WDT+
 *
 *          Schematic Description of CompA+ forming relaxation oscillator and
 *          coupling (connection) between the relaxation oscillator and TimerA0.
 *     \n      <- Output
 *     \n      -> Input
 *     \n      R  Resistor (typically 100Kohms)
 * \n 
 *     \n                   +-<-Px.y (reference)
 *      \n                  |
 *      \n                  R
 *      \n                  |
 *      \n              +---+-->COMPA+
 *      \n              |   |
 *      \n              R   R                
 *      \n              |   |
 *       \n            GND  |
 *      \n                  |                       
 *      \n                  +-->TA1CLK
 *      \n                  |
 *      \n     element-+-R--+-<-CAOUT                               
 *      \n             |
 *      \n             +------->COMPA- 
 * \n 
 *          The WDT+ interval represents the measurement window.  The number of 
 *          counts within the TA0R that have accumulated during the measurement
 *          window represents the capacitance of the element.
 * 
 * @param   group Address of the structure describing the Sensor to be measured
 * @param   counts Address to where the measurements are to be written
 * @return  none
 ******************************************************************************/
void TI_CTS_RO_COMPAp_TA1_WDTp_HAL(const struct Sensor *group, uint16_t *counts)
{ 
    uint8_t i;
//** Context Save
//  Status Register: 
//  WDTp: IE1, WDTCTL
//  TIMERA0: TA1CTL, TA1CCTL1
//  COMPAp: CACTL1, CACTL2, CAPD
//  Ports: caoutDIR, caoutSel, txclkDIR, txclkSel, caoutSel2, txclkSel2, refout, refdir 
    uint8_t contextSaveSR; 
    uint8_t contextSaveIE1;
    uint16_t contextSaveWDTCTL;
    uint16_t contextSaveTA1CTL,contextSaveTA1CCTL1;
    uint16_t contextSaveTA1CCR1;
    uint8_t contextSaveCACTL1,contextSaveCACTL2,contextSaveCAPD;
    uint8_t contextSaveCaoutDir,contextSaveCaoutSel;  
    uint8_t contextSavetxclkDir,contextSavetxclkSel;    
    uint8_t contextSaveRefDir,contextSaveRefOutSel;  
    #ifdef SEL2REGISTER
    uint8_t contextSaveCaoutSel2,contextSaveTxclkSel2; 
    
    contextSaveCaoutSel2 = *(group->caoutSel2Register);
    contextSaveTxclkSel2 = *(group->txclkSel2Register);       
    #endif    
    contextSaveSR = __get_SR_register();
    contextSaveIE1 = IE1;
    contextSaveWDTCTL = WDTCTL;
    contextSaveWDTCTL &= 0x00FF;
    contextSaveWDTCTL |= WDTPW;        
    contextSaveTA1CTL = TA1CTL;
    contextSaveTA1CCTL1 = TA1CCTL1;
    contextSaveTA1CCR1 = TA1CCR1;
    contextSaveCACTL1 = CACTL1;
    contextSaveCACTL2 = CACTL2;
    contextSaveCAPD = CAPD;
    contextSaveCaoutDir = *(group->caoutDirRegister);
    contextSaveCaoutSel = *(group->caoutSelRegister);  
    contextSavetxclkDir = *(group->txclkDirRegister);
    contextSavetxclkSel = *(group->txclkSelRegister);    
    contextSaveRefDir = *(group->refPxdirRegister);
    contextSaveRefOutSel = *(group->refPxoutRegister);  
//** Setup Measurement timer***************************************************
// Choices are TA0,TA1,TB0,TB1,TD0,TD1 these choices are pushed up into the 
// capacitive touch layer.
 
    TA1CTL = TASSEL_0+MC_2;                // TA1CLK, cont mode
    TA1CCTL1 = CM_3+CCIS_2+CAP;            // Pos&Neg,GND,Cap
      
    *(group->caoutDirRegister) |= group->caoutBits;
    *(group->txclkDirRegister) &= ~group->txclkBits;
    *(group->caoutSelRegister) |= group->caoutBits;
    *(group->txclkSelRegister) |= group->txclkBits;
    
    #ifdef SEL2REGISTER
    *(group->caoutSel2Register) |= group->caoutBits;
    *(group->txclkSel2Register) |= group->txclkBits;
    #endif
      
    *(group->refPxdirRegister) |= group->refBits;
    *(group->refPxoutRegister) |= group->refBits;
    CACTL1 |= CAON;                       // Turn on comparator
    CAPD |= (group->capdBits); 
    IE1 |= WDTIE;                         // enable WDT interrupt
    
    for (i = 0; i<(group->numElements); i++)
    {
       CACTL2= group->refCactl2Bits + (group->arrayPtr[i])->inputBits;
    //**  Setup Gate Timer *****************************************************
    // Set duration of sensor measurment
       WDTCTL = WDTPW+WDTTMSEL+ group->measGateSource + group->accumulationCycles; 
       TA1CTL |= TACLR;                     // Clear Timer_A TAR
       if(group->measGateSource == GATE_WDT_ACLK)
       {
            __bis_SR_register(LPM3_bits+GIE);   // Wait for WDT interrupt
       }
       else
       {
	        __bis_SR_register(LPM0_bits+GIE);   // Wait for WDT interrupt
       }
       TA1CCTL1 ^= CCIS0;                   // Create SW capture of CCR1
       counts[i] = TA1CCR1;                 // Save result
       WDTCTL = WDTPW + WDTHOLD;           // Stop watchdog timer
    }
      // End Sequence
    //** Context Restore
    //  WDTp: IE1, WDCTL
    //  TIMERA0: TACTL, TACCTL1
    //  COMPAp: CACTL1, CACTL2, CAPD
    //  Ports: caoutDIR, caoutSel, txclkDIR, txclkSel, caoutSel2, txclkSel2, refout, refdir  
    #ifdef SEL2REGISTER  
    *(group->caoutSel2Register) = contextSaveCaoutSel2;
    *(group->txclkSel2Register) = contextSaveTxclkSel2;       
    #endif    
    __bis_SR_register(contextSaveSR);   
    if(!(contextSaveSR & GIE))
    {
      __bic_SR_register(GIE);   // Wait for WDT interrupt        
    }
    IE1 = contextSaveIE1;
    WDTCTL = contextSaveWDTCTL;
    TA1CTL = contextSaveTA1CTL;
    TA1CCTL1 = contextSaveTA1CCTL1;
    TA1CCR1 = contextSaveTA1CCR1;
    CACTL1 = contextSaveCACTL1;
    CACTL2 = contextSaveCACTL2;
    CAPD = contextSaveCAPD;
    *(group->caoutDirRegister) = contextSaveCaoutDir;
    *(group->caoutSelRegister) = contextSaveCaoutSel;  
    *(group->txclkDirRegister) = contextSavetxclkDir;
    *(group->txclkSelRegister) = contextSavetxclkSel;    
    *(group->refPxdirRegister) = contextSaveRefDir;
    *(group->refPxoutRegister) = contextSaveRefOutSel;  
}
#endif

#ifdef fRO_COMPAp_TA1_SW
/***************************************************************************//**
 * @brief   RO method capactiance measurement using CompA+, TimerA1, and SW loop
 *
 *          Schematic Description of CompA+ forming relaxation oscillator.
 *     \n      <- Output
 *     \n      -> Input
 *     \n      R  Resistor (typically 100Kohms)
 * \n 
 *      \n                  +-<-Px.y (reference)
 *      \n                  |
 *      \n                  R
 *      \n                  |
 *      \n              +---+-->COMPA+
 *      \n              |   |
 *      \n              R   R                
 *      \n              |   |
 *      \n             GND  |
 *      \n                  |  
 *      \n                  +-->TA1CLK                     
 *      \n                  |
 *      \n     element-+-R--+-<-CAOUT                               
 *      \n             |
 *      \n             +------->COMPA- 
 * \n 
 *      \n     The timer counts to TA1CCR0 representing the measurement window. The
 *          number of counts within the SW loop that have accumulated during the
 *          measurement window represents the capacitance of the element.
 * 
 * @param   group Address of the structure describing the Sensor to be measured
 * @param   counts Address to where the measurements are to be written
 * @return  none
 ******************************************************************************/
void TI_CTS_fRO_COMPAp_TA1_SW_HAL(const struct Sensor *group, uint16_t *counts)
{ 
    uint8_t i;
    uint16_t j;
    //** Context Save
//  Status Register: 
//  TIMERA0: TA1CTL, TA1CCTL0
//  COMPAp: CACTL1, CACTL2, CAPD
//  Ports: caoutDIR, caoutSel, txclkDIR, txclkSel, caoutSel2, txclkSel2, refout, refdir 
    uint16_t contextSaveTA1CTL,contextSaveTA1CCTL0,contextSaveTA1CCR0;
    uint8_t contextSaveCACTL1,contextSaveCACTL2,contextSaveCAPD;
    uint8_t contextSaveCaoutDir,contextSaveCaoutSel;  
    uint8_t contextSavetxclkDir,contextSavetxclkSel;    
    uint8_t contextSaveRefDir,contextSaveRefOutSel;  
    #ifdef SEL2REGISTER
    uint8_t contextSaveCaoutSel2,contextSaveTxclkSel2; 
    
    contextSaveCaoutSel2 = *(group->caoutSel2Register);
    contextSaveTxclkSel2 = *(group->txclkSel2Register);       
    #endif    
    contextSaveTA1CTL = TA1CTL;
    contextSaveTA1CCTL0 = TA1CCTL0;
    contextSaveTA1CCR0 = TA1CCR0;
    contextSaveCACTL1 = CACTL1;
    contextSaveCACTL2 = CACTL2;
    contextSaveCAPD = CAPD;
    contextSaveCaoutDir = *(group->caoutDirRegister);
    contextSaveCaoutSel = *(group->caoutSelRegister);  
    contextSavetxclkDir = *(group->txclkDirRegister);
    contextSavetxclkSel = *(group->txclkSelRegister);    
    contextSaveRefDir = *(group->refPxdirRegister);
    contextSaveRefOutSel = *(group->refPxoutRegister);      
//** Setup Measurement timer***************************************************
 
    // Configure Timer TA0
    TA1CCR0 =(group->accumulationCycles);
    // setup connections between CAOUT and TA0
    *(group->caoutDirRegister) |= group->caoutBits;
    *(group->txclkDirRegister) &= ~group->txclkBits;
    *(group->caoutSelRegister) |= group->caoutBits;
    *(group->txclkSelRegister) |= group->txclkBits;
    
    #ifdef SEL2REGISTER
    *(group->caoutSel2Register) |= group->caoutBits;
    *(group->txclkSel2Register) |= group->txclkBits;
    #endif
    // setup reference
    *(group->refPxdirRegister) |= group->refBits;
    *(group->refPxoutRegister) |= group->refBits;
    CACTL1 |= CAON;                       // Turn on comparator
    CAPD |= (group->capdBits); 
    
    for (i = 0; i<(group->numElements); i++)
    {
        j=0;
        CACTL2= group->refCactl2Bits + (group->arrayPtr[i])->inputBits;
        //**  Setup Gate Timer **************
        // Set duration of sensor measurment
        TA1CTL = TASSEL_0+TACLR+MC_1;     // TA1CLK, Reset, up mode
        TA1CTL &= ~TAIFG;                 // clear IFG        
        while(!(TACTL & TAIFG))
        {
            j++;
        } // end accumulation
        counts[i] = j;   
    }
    // End Sequence
    //** Context Restore
    //  WDTp: IE1, WDCTL
    //  TIMERA0: TACTL, TACCTL1
    //  COMPAp: CACTL1, CACTL2, CAPD
    //  Ports: caoutDIR, caoutSel, txclkDIR, txclkSel, caoutSel2, txclkSel2, refout, refdir  
    #ifdef SEL2REGISTER  
    *(group->caoutSel2Register) = contextSaveCaoutSel2;
    *(group->txclkSel2Register) = contextSaveTxclkSel2;       
    #endif    
    TA1CTL = contextSaveTA1CTL;
    TA1CCTL0 = contextSaveTA1CCTL0;
    TA1CCR0 = contextSaveTA1CCR0;
    CACTL1 = contextSaveCACTL1;
    CACTL2 = contextSaveCACTL2;
    CAPD = contextSaveCAPD;
    *(group->caoutDirRegister) = contextSaveCaoutDir;
    *(group->caoutSelRegister) = contextSaveCaoutSel;  
    *(group->txclkDirRegister) = contextSavetxclkDir;
    *(group->txclkSelRegister) = contextSavetxclkSel;    
    *(group->refPxdirRegister) = contextSaveRefDir;
    *(group->refPxoutRegister) = contextSaveRefOutSel;  
}
#endif

/***************************************************************************//**
 * @brief   RC method capactiance measurement using a Pair of GPIO and TimerA0
 *
 *          Schematic Description of two GPIO forming RC measruement.
 *    \n       <- Output
 *    \n       -> Input
 *    \n       R  Resistor (typically 1Mohms)
 * \n 
 *     \n                   +-<-Px.y (reference)
 *     \n                   |
 *     \n                   R
 *     \n                   |
 *     \n         Element---+-->Pa.b                               
 *     \n      
 *     \n      Charge and Discharge Cycle
 *     \n                       + 
 *     \n                   +    +
 *     \n               +          +
 *     \n            +                +
 *     \n          +                     +
 *     \n      Start Timer                  After n cycles Stop Timer
 *     \n   The TAR reister value is the number of SMCLK periods within n charge
 *          and discharge cycles.  This value is directly proportional to the 
 *          capacitance of the element measured. 'n' is defined by the variable 
 *          accumulation_cycles.
 * 
 * @param   group Address of the structure describing the Sensor to be measured
 * @param   counts Address to where the measurements are to be written
 * @return  none
 ******************************************************************************/
#ifdef RC_PAIR_TA0            
void TI_CTS_RC_PAIR_TA0_HAL(const struct Sensor *group,uint16_t *counts)
{
    uint8_t i;
	uint16_t j;
    
    //** Context Save
//  TIMERA0: TA0CTL
//  Port: inputPxout, inputPxdir, referencePxout, referencePxdir

    uint8_t contextSaveinputPxout,contextSaveinputPxdir,contextSavereferencePxout;
    uint8_t contextSavereferencePxdir;

    #ifdef __MSP430_HAS_SFR__
    uint16_t contextSaveTA0CTL,contextSaveTA0CCR0;

    contextSaveTA0CTL = TA0CTL;
    contextSaveTA0CCR0 = TA0CCR0;
    #else
    uint16_t contextSaveTACTL,contextSaveTACCR0;
    
    contextSaveTACTL = TACTL; 
    contextSaveTACCR0 = TACCR0; 
    #endif

//** Setup Measurement timer****************************************************
// Choices are TA0,TA1,TB0,TB1,TD0,TD1 these choices are pushed up into the 
// capacitive touch layer.
    #ifdef __MSP430_HAS_SFR__
    TA0CCR0 = 0xFFFF;                           
    #else
    TACCR0 = 0xFFFF;                           
    #endif
    for (i = 0; i<(group->numElements); i++)
    {
        // Context Save
        contextSaveinputPxout = *((group->arrayPtr[i])->inputPxoutRegister);
        contextSaveinputPxdir = *((group->arrayPtr[i])->inputPxdirRegister);
        contextSavereferencePxout = *((group->arrayPtr[i])->referencePxoutRegister);
        contextSavereferencePxdir = *((group->arrayPtr[i])->referencePxdirRegister);
        j = (group->accumulationCycles);
        #ifdef __MSP430_HAS_SFR__
	    TA0CTL = TASSEL_2+TACLR;                // SMCLK, up mode
        #else
        TACTL = TASSEL_2+TACLR;                // SMCLK, up mode
        #endif
        while(j--)
	    {
        //******************************************************************************
	    // Positive cycle
        //    SENSOR ---+---- Input (low to high)
	    //              R
	    //              +---- Rerefence (high)
            //******************************************************************************
	    // Input low
            *((group->arrayPtr[i])->inputPxoutRegister) &= ~((group->arrayPtr[i])->inputBits);
	    *((group->arrayPtr[i])->inputPxdirRegister) |= (group->arrayPtr[i])->inputBits;
            // Reference High
            *((group->arrayPtr[i])->referencePxdirRegister) |= (group->arrayPtr[i])->referenceBits;
            *((group->arrayPtr[i])->referencePxoutRegister) |= ((group->arrayPtr[i])->referenceBits);
            // Wait until low
	    while((*((group->arrayPtr[i])->inputPxinRegister)) & ((group->arrayPtr[i])->inputBits));
            // Change to an input
            *((group->arrayPtr[i])->inputPxdirRegister) &= ~(group->arrayPtr[i])->inputBits;
	    //**************************************************************************
	    // This mechanism is traditianally an LPM with the ISR calculating the 
	    // delta between when the first snapshot and the ISR event.  If this is
	    // included within the library the entire port ISR would not be available
            // to the calling application.  In this example the polling is done with the
            // CPU at expense of power and MIPS but preserves the port ISR for other 
	    // interruptible functions.
	    //**************************************************************************
            #ifdef __MSP430_HAS_SFR__
            TA0CTL |= MC_1;                     // start timer    
            #else
            TACTL |= MC_1;                     // start timer    
            #endif
	    //wait until voltage reaches Vih of port
	    while(!((*((group->arrayPtr[i])->inputPxinRegister) & (group->arrayPtr[i])->inputBits)));
            #ifdef __MSP430_HAS_SFR__
            TA0CTL &= ~ MC_3;                     // stop timer        
            #else
            TACTL &= ~ MC_3;                     // stop timer        
            #endif
			//******************************************************************************
			// Negative cycle
			//    SENSOR ---+---- Input (high to low)
			//              R
			//              +---- Rerefence (low)
			//******************************************************************************
	    // Input High
            *((group->arrayPtr[i])->inputPxoutRegister) |= ((group->arrayPtr[i])->inputBits);  
            *((group->arrayPtr[i])->inputPxdirRegister) |= (group->arrayPtr[i])->inputBits; 
	    // Reference Low
	    *((group->arrayPtr[i])->referencePxoutRegister) &= ~((group->arrayPtr[i])->referenceBits);
	    // Change to an input
	    *((group->arrayPtr[i])->inputPxdirRegister) &= ~((group->arrayPtr[i])->inputBits);
            #ifdef __MSP430_HAS_SFR__
            TA0CTL |= MC_1;                     // start timer  
            #else
            TACTL |= MC_1;                     // start timer  
            #endif
	    //wait until voltage reaches Vil of port  
	    while((*((group->arrayPtr[i])->inputPxinRegister)) & ((group->arrayPtr[i])->inputBits));
            #ifdef __MSP430_HAS_SFR__
            TA0CTL &= ~ MC_3;                     // stop timer        
            #else
            TACTL &= ~ MC_3;                     // stop timer        
            #endif
        } // END accumulation loop for a single element
        #ifdef __MSP430_HAS_SFR__
        counts[i] = TA0R;    
        #else
        counts[i] = TAR;    
        #endif
        // Context Restore
        *((group->arrayPtr[i])->inputPxoutRegister) = contextSaveinputPxout;
        *((group->arrayPtr[i])->inputPxdirRegister) = contextSaveinputPxdir;     
        *((group->arrayPtr[i])->referencePxoutRegister) = contextSavereferencePxout;
        *((group->arrayPtr[i])->referencePxdirRegister) = contextSavereferencePxdir;             
    } // END FOR loop which cycles through elements within sensor
    
    //** Context Restore
    #ifdef __MSP430_HAS_SFR__
    TA0CTL = contextSaveTA0CTL;
    TA0CCR0 = contextSaveTA0CCR0;
    #else
    TACTL = contextSaveTACTL;
    TACCR0 = contextSaveTACCR0;
    #endif

}
#endif

/***************************************************************************//**
 * @brief   fRO method capactiance measurement using the PinOsc and TimerA0
 *          
 *     \n      Charge and Discharge Cycle
 *     \n                       + 
 *     \n                   +    +
 *     \n               +          +
 *     \n            +                +
 *     \n          +                     +
 *     \n      Start Timer                  After n cycles Stop Timer
 *     \n   The TAR reister value is the number of SW loops (function of MCLK) 
 *          within n charge and discharge cycles.  This value is directly 
 *          proportional to the capacitance of the element measured. 'n' is 
 *          defined by the variable accumulation_cycles.
 * 
 * @param   group Address of the structure describing the Sensor to be measured
 * @param   counts Address to where the measurements are to be written
 * @return  none
 ******************************************************************************/
#ifdef fRO_PINOSC_TA0_SW            
void TI_CTS_fRO_PINOSC_TA0_SW_HAL(const struct Sensor *group,uint16_t *counts)
{ 
    uint8_t i;
    uint16_t j;
//** Context Save
//  TIMERA0: TA0CTL
//  Ports: PxSEL, PxSEL2 
    uint16_t contextSaveTA0CTL, contextSaveTA0CCTL0;
    uint8_t contextSaveSel,contextSaveSel2;
    
    contextSaveTA0CTL = TA0CTL;
    contextSaveTA0CCTL0 = TA0CCTL0;
    // Setup Measurement timer
    TACCR0 =(group->accumulationCycles);
    for (i =0; i< (group->numElements); i++)
    {
        j = 0;
        // Context Save
        contextSaveSel = *((group->arrayPtr[i])->inputPxselRegister);
        contextSaveSel2 = *((group->arrayPtr[i])->inputPxsel2Register);
        // start single oscillation (rise then fall and trigger on fall)
        *((group->arrayPtr[i])->inputPxselRegister) &= ~((group->arrayPtr[i])->inputBits);
        *((group->arrayPtr[i])->inputPxsel2Register) |= ((group->arrayPtr[i])->inputBits);  
        TA0CTL = TASSEL_3+TACLR+MC_1;      // INCLK, reset, up mode   
        TA0CTL &= ~TAIFG;                  // clear IFG                
        // start timer in up mode
        while(!(TA0CTL & TAIFG))
        {
            j++;
        } // end accumulation
        counts[i] = j;   
        TA0CTL &= ~MC_1;
        // Context Restore
        *((group->arrayPtr[i])->inputPxselRegister) = contextSaveSel;
        *((group->arrayPtr[i])->inputPxsel2Register) = contextSaveSel2;                        
    }
    // End Sequence
    // Context Restore
    TA0CTL = contextSaveTA0CTL;
    TA0CCTL0 = contextSaveTA0CCTL0;
}
#endif 

#ifdef RO_PINOSC_TA0_WDTp 
/***************************************************************************//**
 * @brief   RO method capactiance measurement using PinOsc IO, TimerA0, and WDT+
 *
 *          Schematic Description: 
 * 
 *     \n   element-----+->Px.y
 * 
 *     \n   The WDT+ interval represents the measurement window.  The number of 
 *          counts within the TA0R that have accumulated during the measurement
 *          window represents the capacitance of the element.
 * 
 * @param   group Pointer to the structure describing the Sensor to be measured
 * @param   counts Pointer to where the measurements are to be written
 * @return  none
 ******************************************************************************/
void TI_CTS_RO_PINOSC_TA0_WDTp_HAL(const struct Sensor *group,uint16_t *counts)
{ 
    uint8_t i;

//** Context Save
//  Status Register: 
//  WDTp: IE1, WDTCTL
//  TIMERA0: TA0CTL, TA0CCTL1
//  Ports: PxSEL, PxSEL2 
    uint8_t contextSaveSR; 
    uint8_t contextSaveIE1;
    uint16_t contextSaveWDTCTL;
    uint16_t contextSaveTA0CTL,contextSaveTA0CCTL1,contextSaveTA0CCR1;
    uint8_t contextSaveSel,contextSaveSel2;

    contextSaveSR = __get_SR_register();
    contextSaveIE1 = IE1;
    contextSaveWDTCTL = WDTCTL;
    contextSaveWDTCTL &= 0x00FF;
    contextSaveWDTCTL |= WDTPW;        
    contextSaveTA0CTL = TA0CTL;
    contextSaveTA0CCTL1 = TA0CCTL1;
    contextSaveTA0CCR1 = TA0CCR1;
    
//** Setup Measurement timer***************************************************
// Choices are TA0,TA1,TB0,TB1,TD0,TD1 these choices are pushed up into the 
// capacitive touch layer.
 
 // Configure and Start Timer
    TA0CTL = TASSEL_3+MC_2;                // TACLK, cont mode
    TA0CCTL1 = CM_3+CCIS_2+CAP;            // Pos&Neg,GND,Cap
    IE1 |= WDTIE;                         // enable WDT interrupt
    for (i = 0; i<(group->numElements); i++)
    {
        // Context Save
        contextSaveSel = *((group->arrayPtr[i])->inputPxselRegister);
        contextSaveSel2 = *((group->arrayPtr[i])->inputPxsel2Register);
	      // Configure Ports for relaxation oscillator
	      *((group->arrayPtr[i])->inputPxselRegister) &= ~((group->arrayPtr[i])->inputBits);
	      *((group->arrayPtr[i])->inputPxsel2Register) |= ((group->arrayPtr[i])->inputBits);
        //**  Setup Gate Timer ********************************************************
	      // Set duration of sensor measurment
	      //WDTCTL = (WDTPW+WDTTMSEL+group->measGateSource+group->accumulationCycles);
        WDTCTL = (WDTPW+WDTTMSEL+(group->measGateSource)+(group->accumulationCycles));  
        TA0CTL |= TACLR;                     // Clear Timer_A TAR
        if(group->measGateSource == GATE_WDT_ACLK)
        {
            __bis_SR_register(LPM3_bits+GIE);   // Wait for WDT interrupt
        }
        else
        {
	          __bis_SR_register(LPM0_bits+GIE);   // Wait for WDT interrupt
        }
	      TA0CCTL1 ^= CCIS0;                   // Create SW capture of CCR1
	      counts[i] = TA0CCR1;                 // Save result
	      WDTCTL = WDTPW + WDTHOLD;           // Stop watchdog timer
        // Context Restore
        *((group->arrayPtr[i])->inputPxselRegister) = contextSaveSel;
        *((group->arrayPtr[i])->inputPxsel2Register) = contextSaveSel2;
    }
    // End Sequence
    // Context Restore
    __bis_SR_register(contextSaveSR);   
    if(!(contextSaveSR & GIE))
    {
        __bic_SR_register(GIE);   //        
    }
    IE1 = contextSaveIE1;
    WDTCTL = contextSaveWDTCTL;
    TA0CTL = contextSaveTA0CTL;
    TA0CCTL1 = contextSaveTA0CCTL1;
    TA0CCR1 = contextSaveTA0CCR1;
}
#endif

#ifdef RO_PINOSC_TA0 
/***************************************************************************//**
 * @brief   RO method capactiance measurement using PinOsc IO, and TimerA0
 *
 *          Schematic Description: 
 * 
 *     \n      element-----+->Px.y
 * 
 *     \n   The measurement window is accumulation_cycles/ACLK. The ACLK is
 *          used to generate a capture event via the internal connection CCIOB. 
 *          The counts within the TA0R that have accumulated during the 
 *          measurement window represents the capacitance of the element.
 * 
 * @param   group Pointer to the structure describing the Sensor to be measured
 * @param   counts Pointer to where the measurements are to be written
 * @return  none
 ******************************************************************************/
void TI_CTS_RO_PINOSC_TA0_HAL(const struct Sensor *group,uint16_t *counts)
{ 
    uint8_t i;
	uint16_t j;
    //** Context Save
//  TIMERA0: TA0CTL, TA0CCTL0
//  Ports: PxSEL, PxSEL2 
    uint16_t contextSaveTA0CTL,contextSaveTA0CCTL0,contextSaveTA0CCR0;
    uint8_t contextSaveSel,contextSaveSel2;

    contextSaveTA0CTL = TA0CTL;
    contextSaveTA0CCTL0 = TA0CCTL0;
    contextSaveTA0CCR0 = TA0CCR0;

	//** Setup Measurement timer***************************************************
	// Choices are TA0,TA1,TB0,TB1,TD0,TD1 these choices are pushed up into the 
	// capacitive touch layer.
 
	// Configure and Start Timer
	TA0CTL = TASSEL_3+MC_2;                // TACLK, cont mode
	for (i =0; i< (group->numElements); i++)
	{
        // Context Save
        contextSaveSel = *((group->arrayPtr[i])->inputPxselRegister);
        contextSaveSel2 = *((group->arrayPtr[i])->inputPxsel2Register);
	    // Configure Ports for relaxation oscillator
	    j = (group->accumulationCycles);
	    *((group->arrayPtr[i])->inputPxselRegister) &= ~((group->arrayPtr[i])->inputBits);
	    *((group->arrayPtr[i])->inputPxsel2Register) |= ((group->arrayPtr[i])->inputBits);
	    TA0CCTL0 = CM_3+CCIS_1+CAP;            // Pos&Neg,ACLK (CCI0B),Cap
	    while(!(TA0CCTL0 & CCIFG));             // wait for capture event  
	    TA0CTL |= TACLR;                       // Clear Timer_A TAR
		while(j--)
		{
            TA0CCTL0 = CM_3+CCIS_1+CAP;            // Pos&Neg,ACLK (CCI0B),Cap
            while(!(TA0CCTL0 & CCIFG));             // wait for capture event 
		} 
	    counts[i] = TA0CCR0;                    // Save result
        TA0CTL = TASSEL_3+MC_2;                        
        // Context Restore
        *((group->arrayPtr[i])->inputPxselRegister) = contextSaveSel;
        *((group->arrayPtr[i])->inputPxsel2Register) = contextSaveSel2;
    }
    // End Sequence
    // Context Restore
    TA0CTL = contextSaveTA0CTL;
    TA0CCTL0 = contextSaveTA0CCTL0;
    TA0CCR0 = contextSaveTA0CCR0;
}
#endif

#ifdef RO_COMPB_TA0_WDTA
/***************************************************************************//**
 * @brief   RO method capactiance measurement using CompB, TimerA0, and WDTA
 *
 *    \n       Schematic Description of CompB forming relaxation oscillator and
 *          coupling (connection) between the relaxation oscillator and TimerA0.
 *     \n      <- Output
 *     \n      -> Input
 *     \n      R  Resistor (typically 100Kohms)
 * 
 *     \n      element---R----<-CBOUT/TA0CLK                               
 * 
 *     \n   The WDTA interval represents the measurement window.  The number of 
 *          counts within the TA0R that have accumulated during the measurement
 *          window represents the capacitance of the element.
 * 
 * @param   group Address of the structure describing the Sensor to be measured
 * @param   counts Address to where the measurements are to be written
 * @return  none
 ******************************************************************************/
void TI_CTS_RO_COMPB_TA0_WDTA_HAL(const struct Sensor *group,uint16_t *counts)
{ 
   uint8_t i;
    
//** Context Save
//  Status Register: 
//  WDTA: IE1, WDTCTL
//  TIMERA0: TA0CTL, TA0CCTL1
//  COMPAp: CACTL1, CACTL2, CAPD
//  Ports: CboutDIR, CboutSel 
    uint8_t contextSaveSR; 
    uint16_t contextSaveSFRIE1;
    uint16_t contextSaveWDTCTL;
    uint16_t contextSaveTA0CTL,contextSaveTA0CCTL1,contextSaveTA0CCR1;
    uint16_t contextSaveCBCTL0,contextSaveCBCTL1;
    uint16_t contextSaveCBCTL2,contextSaveCBCTL3;
    uint8_t contextSaveCboutDir,contextSaveCboutSel;  

    contextSaveSR = __get_SR_register();
    contextSaveSFRIE1 = SFRIE1;
    contextSaveWDTCTL = WDTCTL;
    contextSaveWDTCTL &= 0x00FF;
    contextSaveWDTCTL |= WDTPW;        
    contextSaveTA0CTL = TA0CTL;
    contextSaveTA0CCTL1 = TA0CCTL1;
    contextSaveTA0CCR1 = TA0CCR1;
    
    contextSaveCBCTL0 = CBCTL0;
    contextSaveCBCTL1 = CBCTL1;
    contextSaveCBCTL2 = CBCTL2;
    contextSaveCBCTL3 = CBCTL3;
    contextSaveCboutDir = *(group->cboutTAxDirRegister);
    contextSaveCboutSel = *(group->cboutTAxSelRegister);  

    //** Setup Measurement timer***************************************************
    // connect CBOUT with TA0
    *(group->cboutTAxDirRegister) |= (group->cboutTAxBits);
    *(group->cboutTAxSelRegister) |= (group->cboutTAxBits);

    CBCTL2 = CBREF14+CBREF13 + CBREF02;
  
    // Configure Timer TA0
    TA0CTL = TASSEL_0+MC_2;                // TACLK, cont mode
    TA0CCTL1 = CM_3+CCIS_2+CAP;            // Pos&Neg,GND,Cap 
  
    // Turn on Comparator
    CBCTL1 = CBON + CBF;                      // Turn on comparator with filter
                                              // Vcc to resistor ladder

    CBCTL3 |= (group->cbpdBits);               // set CPD bits to disable 
                                             // I/O buffer
    SFRIE1 |= WDTIE;                           // enable WDT interrupt
    CBCTL2 |= CBRS_1;                          // Turn on reference
    for (i = 0; i<(group->numElements); i++)
    {
        CBCTL0 = CBIMEN + (group->arrayPtr[i])->inputBits;
                                                 
        //**  Setup Gate Timer ********************************************************
        // Set duration of sensor measurment
        WDTCTL = WDTPW+WDTTMSEL+ group->measGateSource + group->accumulationCycles;
        TA0CTL |= TACLR;                     // Clear Timer_A TAR
        if(group->measGateSource == GATE_WDT_ACLK)
        {
            __bis_SR_register(LPM3_bits+GIE);   // Wait for WDT interrupt
        }
        else
        {
	          __bis_SR_register(LPM0_bits+GIE);   // Wait for WDT interrupt
        }
	    TA0CCTL1 ^= CCIS0;                   // Create SW capture of CCR1
	    counts[i] = TA0CCR1;                 // Save result
	    WDTCTL = WDTPW + WDTHOLD;           // Stop watchdog timer
    }  
    // End Sequence
    //** Context Restore
    //  WDTA: IE1, WDCTL
    //  TIMERA0: TACTL, TACCTL1
    //  COMPB: CBCTL0, CBCTL1, CBCTL2, CBCTL3
    //  Ports: CboutDIR, CboutSel 
    __bis_SR_register(contextSaveSR);   
    if(!(contextSaveSR & GIE))
    {
      __bic_SR_register(GIE);   // Wait for WDT interrupt        
    }
    SFRIE1 = contextSaveSFRIE1;
    WDTCTL = contextSaveWDTCTL;
    TA0CTL = contextSaveTA0CTL;
    TA0CCTL1 = contextSaveTA0CCTL1;
    TA0CCR1 = contextSaveTA0CCR1;
    CBCTL0 = contextSaveCBCTL0;
    CBCTL1 = contextSaveCBCTL1;
    CBCTL2 = contextSaveCBCTL2;
    CBCTL3 = contextSaveCBCTL3;
    *(group->cboutTAxDirRegister) = contextSaveCboutDir;
    *(group->cboutTAxSelRegister) = contextSaveCboutSel;  
}
#endif

#ifdef RO_COMPB_TA1_WDTA
/***************************************************************************//**
 * @brief   RO method capactiance measurement using CompB, TimerA1, and WDTA
 *
 *          Schematic Description of CompB forming relaxation oscillator and
 *          coupling (connection) between the relaxation oscillator and TimerA1.
 *          <- Output
 *          -> Input
 *          R  Resistor (typically 100Kohms)
 * 
 *          element---R----<-CBOUT/TA1CLK                               
 * 
 *          The WDTA interval represents the measurement window.  The number of 
 *          counts within the TA1R that have accumulated during the measurement
 *          window represents the capacitance of the element.
 * 
 * @param   group Address of the structure describing the Sensor to be measured
 * @param   counts Address to where the measurements are to be written
 * @return  none
 ******************************************************************************/
void TI_CTS_RO_COMPB_TA1_WDTA_HAL(const struct Sensor *group, uint16_t *counts)
{ 
    uint8_t i=0;
    
    //** Context Save
//  Status Register: 
//  WDTA: IE1, WDTCTL
//  TIMERA1: TA1CTL, TA1CCTL1
//  COMPB: CBCTL0, CBCTL1, CBCTL2, CBCTL3
//  Ports: CboutDIR, CboutSel 
    uint8_t contextSaveSR; 
    uint16_t contextSaveSFRIE1;
    uint16_t contextSaveWDTCTL;
    uint16_t contextSaveTA1CTL,contextSaveTA1CCTL1,contextSaveTA1CCR1;
    uint16_t contextSaveCBCTL0,contextSaveCBCTL1;
    uint16_t contextSaveCBCTL2,contextSaveCBCTL3;
    uint8_t contextSaveCboutDir,contextSaveCboutSel;  

    contextSaveSR = __get_SR_register();
    contextSaveSFRIE1 = SFRIE1;
    contextSaveWDTCTL = WDTCTL;
    contextSaveWDTCTL &= 0x00FF;
    contextSaveWDTCTL |= WDTPW;        
    contextSaveTA1CTL = TA1CTL;
    contextSaveTA1CCTL1 = TA1CCTL1;
    contextSaveTA1CCR1 = TA1CCR1;
    
    contextSaveCBCTL0 = CBCTL0;
    contextSaveCBCTL1 = CBCTL1;
    contextSaveCBCTL2 = CBCTL2;
    contextSaveCBCTL3 = CBCTL3;
    contextSaveCboutDir = *(group->cboutTAxDirRegister);
    contextSaveCboutSel = *(group->cboutTAxSelRegister);  

    //** Setup Measurement timer***************************************************
    // connect CBOUT with TA1
    *(group->cboutTAxDirRegister) |= (group->cboutTAxBits);
    *(group->cboutTAxSelRegister) |= (group->cboutTAxBits);
    // Setup Comparator
    CBCTL2 = CBREF14+CBREF13 + CBREF02;
    // Configure Timer TA1
    TA1CTL = TASSEL_0+MC_2;                // TACLK, cont mode
    TA1CCTL1 = CM_3+CCIS_2+CAP;            // Pos&Neg,GND,Cap 
    // Turn on Comparator
    CBCTL1 = CBON + CBF;                       // Turn on comparator with filter
    CBCTL3 |= (group->cbpdBits);               // set CPD bits to disable 
    SFRIE1 |= WDTIE;                           // enable WDT interrupt
    CBCTL2 |= CBRS_1;                          // Turn on reference
    for (i = 0; i<(group->numElements); i++)
    {
        CBCTL0 = CBIMEN + (group->arrayPtr[i])->inputBits;
        //**  Setup Gate Timer ********************************************************
        // Set duration of sensor measurment
	    WDTCTL = WDTPW+WDTTMSEL+WDTCNTCL+ group->measGateSource + group->accumulationCycles;
	    TA1CTL |= TACLR;                     // Clear Timer_A TAR
        if(group->measGateSource == GATE_WDT_ACLK)
        {
            __bis_SR_register(LPM3_bits+GIE);   // Wait for WDT interrupt
        }
        else
        {
		    __bis_SR_register(LPM0_bits+GIE);   // Wait for WDT interrupt
        }
	    TA1CCTL1 ^= CCIS0;                   // Create SW capture of CCR1
	    counts[i] = TA1CCR1;                 // Save result
	    WDTCTL = WDTPW + WDTHOLD;           // Stop watchdog timer
	    CBCTL3 &= ~((group->arrayPtr[i])->inputBits);
	}
    // End Sequence
    //** Context Restore
    //  WDTA: IE1, WDCTL
    //  TIMERA0: TACTL, TACCTL1
    //  COMPB: CBCTL0, CBCTL1, CBCTL2, CBCTL3
    //  Ports: CboutDIR, CboutSel 
    __bis_SR_register(contextSaveSR);   
    if(!(contextSaveSR & GIE))
    {
      __bic_SR_register(GIE);   // Wait for WDT interrupt        
    }
    SFRIE1 = contextSaveSFRIE1;
    WDTCTL = contextSaveWDTCTL;
    TA1CTL = contextSaveTA1CTL;
    TA1CCTL1 = contextSaveTA1CCTL1;
    TA1CCR1 = contextSaveTA1CCR1;
    CBCTL0 = contextSaveCBCTL0;
    CBCTL1 = contextSaveCBCTL1;
    CBCTL2 = contextSaveCBCTL2;
    CBCTL3 = contextSaveCBCTL3;
    *(group->cboutTAxDirRegister) = contextSaveCboutDir;
    *(group->cboutTAxSelRegister) = contextSaveCboutSel;  
}
#endif

#ifdef fRO_COMPB_TA0_SW
/***************************************************************************//**
 * @brief   fRO method capactiance measurement using CompB, TimerA0 
 *
 *          Schematic Description of CompB forming relaxation oscillator and
 *          coupling (connection) between the relaxation oscillator and TimerA0.
 *          <- Output
 *          -> Input
 *          R  Resistor (typically 100Kohms)
 * 
 *          element---R----<-CBOUT                               
 * 
 *          The TAR reister value is the number of SW loops (function of MCLK) 
 *          within n charge and discharge cycles.  This value is directly 
 *          proportional to the capacitance of the element measured. 'n' is 
 *          defined by the variable accumulation_cycles.
 * 
 * @param   group Address of the structure describing the Sensor to be measured
 * @param   counts Address to where the measurements are to be written
 * @return  none
 ******************************************************************************/
void TI_CTS_fRO_COMPB_TA0_SW_HAL(const struct Sensor *group, uint16_t *counts)
{ 
    uint8_t i;
    uint16_t j;
    
//** Context Save
//  TIMERA0: TA0CTL, TA0CCR1
//  COMPB: CBCTL0, CBCTL1, CBCTL2, CBCTL3
//  Ports: CboutDIR, CboutSel 

    uint16_t contextSaveTA0CTL,contextSaveTA0CCR0;
    uint16_t contextSaveCBCTL0,contextSaveCBCTL1;
    uint16_t contextSaveCBCTL2,contextSaveCBCTL3;
    uint8_t contextSaveCboutDir,contextSaveCboutSel;  

    contextSaveTA0CTL = TA0CTL;
    contextSaveTA0CCR0 = TA0CCR0;
    
    contextSaveCBCTL0 = CBCTL0;
    contextSaveCBCTL1 = CBCTL1;
    contextSaveCBCTL2 = CBCTL2;
    contextSaveCBCTL3 = CBCTL3;
    contextSaveCboutDir = *(group->cboutTAxDirRegister);
    contextSaveCboutSel = *(group->cboutTAxSelRegister);  
    //** Setup Measurement timer***************************************************
    // connect CBOUT with TA0
    *(group->cboutTAxDirRegister) |= (group->cboutTAxBits);
    *(group->cboutTAxSelRegister) |= (group->cboutTAxBits);
    CBCTL2 = CBREF14+CBREF13 + CBREF02;
    // Configure Timer TA0
    TA0CCR0 =(group->accumulationCycles);

    // Turn on Comparator
    CBCTL1 = CBON + CBF;                       // Turn on comparator with filter
                                               // Vcc to resistor ladder
    CBCTL3 |= (group->cbpdBits);               // set CPD bits to disable 
                                               // I/O buffer
    CBCTL2 |= CBRS_1;                          // Turn on reference
    for (i = 0; i<(group->numElements); i++)
    {
        j=0;
        CBCTL0 = CBIMEN + (group->arrayPtr[i])->inputBits;                                         
        //**  Setup Gate Timer ********************************************************
        // Set duration of sensor measurment
        TA0CTL = TASSEL_0+TACLR+MC_1;             // TACLK
        TA0CTL &= ~TAIFG;             // TACLK        
        while(!(TA0CTL & TAIFG))
        {
            j++;
        } // end accumulation
        counts[i] = j;   
    }
    // End Sequence
    //** Context Restore
    //  TIMERA0: TACTL, TACCTL1
    //  COMPB: CBCTL0, CBCTL1, CBCTL2, CBCTL3
    //  Ports: CboutDIR, CboutSel 
    TA0CTL = contextSaveTA0CTL;
    TA0CCR0 = contextSaveTA0CCR0;
    CBCTL0 = contextSaveCBCTL0;
    CBCTL1 = contextSaveCBCTL1;
    CBCTL2 = contextSaveCBCTL2;
    CBCTL3 = contextSaveCBCTL3;
    *(group->cboutTAxDirRegister) = contextSaveCboutDir;
    *(group->cboutTAxSelRegister) = contextSaveCboutSel;  
}
#endif

#ifdef fRO_COMPB_TA1_SW
/***************************************************************************//**
 * @brief   fRO method capactiance measurement using CompB, TimerA1 
 *
 *          Schematic Description of CompB forming relaxation oscillator and
 *          coupling (connection) between the relaxation oscillator and TimerA1.
 *          <- Output
 *          -> Input
 *          R  Resistor (typically 100Kohms)
 * 
 *          element---R----<-CBOUT                               
 * 
 *          The TAR reister value is the number of SW loops (function of MCLK) 
 *          within n charge and discharge cycles.  This value is directly 
 *          proportional to the capacitance of the element measured. 'n' is 
 *          defined by the variable accumulation_cycles.
 * 
 * @param   group Address of the structure describing the Sensor to be measured
 * @param   counts Address to where the measurements are to be written
 * @return  none
 ******************************************************************************/
void TI_CTS_fRO_COMPB_TA1_SW_HAL(const struct Sensor *group,uint16_t *counts)
{ 
    uint8_t i;
    uint16_t j;
//** Context Save
//  TIMERA0: TA1CTL, TA1CCTL1
//  COMPB: CBCTL0, CBCTL1, CBCTL2, CBCTL3
//  Ports: CboutDIR, CboutSel 

    uint16_t contextSaveTA1CTL,contextSaveTA1CCR0;
    uint16_t contextSaveCBCTL0,contextSaveCBCTL1;
    uint16_t contextSaveCBCTL2,contextSaveCBCTL3;
    uint8_t contextSaveCboutDir,contextSaveCboutSel;  

    contextSaveTA1CTL = TA1CTL;
    contextSaveTA1CCR0 = TA1CCR0;
    
    contextSaveCBCTL0 = CBCTL0;
    contextSaveCBCTL1 = CBCTL1;
    contextSaveCBCTL2 = CBCTL2;
    contextSaveCBCTL3 = CBCTL3;
    contextSaveCboutDir = *(group->cboutTAxDirRegister);
    contextSaveCboutSel = *(group->cboutTAxSelRegister);  
    //** Setup Measurement timer***************************************************
    // connect CBOUT with TA1
    *(group->cboutTAxDirRegister) |= (group->cboutTAxBits);
    *(group->cboutTAxSelRegister) |= (group->cboutTAxBits);
    CBCTL2 = CBREF14+CBREF13 + CBREF02;
    // Configure Timer TA1
    TA1CCR0 =(group->accumulationCycles);

    // Turn on Comparator
    CBCTL1 = CBON + CBF;                       // Turn on comparator with filter
                                               // Vcc to resistor ladder
    CBCTL3 |= (group->cbpdBits);               // set CPD bits to disable 
                                               // I/O buffer
    CBCTL2 |= CBRS_1;                          // Turn on reference
    for (i = 0; i<(group->numElements); i++)
    {
        j=0;
        CBCTL0 = CBIMEN + (group->arrayPtr[i])->inputBits;                                         
        //**  Setup Gate Timer ********************************************************
        // Set duration of sensor measurment
        TA1CTL = TASSEL_0+TACLR+MC_1;          // TA1CLK, reset, up mode
        TA1CTL &= ~TAIFG;                      // clear ifg
        while(!(TA1CTL & TAIFG))
        {
            j++;
        } // end accumulation
        counts[i] = j;   
        //P1SEL &=~BIT4; 
    }
    // End Sequence
    //** Context Restore
    //  TIMERA0: TACTL, TACCTL1
    //  COMPB: CBCTL0, CBCTL1, CBCTL2, CBCTL3
    //  Ports: CboutDIR, CboutSel 
    TA1CTL = contextSaveTA1CTL;
    TA1CCR0 = contextSaveTA1CCR0;
    CBCTL0 = contextSaveCBCTL0;
    CBCTL1 = contextSaveCBCTL1;
    CBCTL2 = contextSaveCBCTL2;
    CBCTL3 = contextSaveCBCTL3;
    *(group->cboutTAxDirRegister) = contextSaveCboutDir;
    *(group->cboutTAxSelRegister) = contextSaveCboutSel;  
}
#endif

#ifdef WDT_GATE
// Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
    __bic_SR_register_on_exit(LPM3_bits);           // Exit LPM3 on reti
}
#endif

/***************************************************************************//**
 * @}
 ******************************************************************************/
