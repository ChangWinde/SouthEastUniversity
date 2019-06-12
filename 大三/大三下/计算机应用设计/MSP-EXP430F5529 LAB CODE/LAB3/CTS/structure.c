//******************************************************************************
// structure.c
//
// 5 elements configured as a 100 point slider.
//
//
//******************************************************************************

#include "structure.h"

// Actual demo structures
const struct Element element0 = {   //CB0, P6.0
              .inputBits = CBIMSEL_0,
              .maxResponse = 250,
              .threshold = 125
};

const struct Element element1 = {   //CB1, P6.1
              .inputBits = CBIMSEL_1,
              .maxResponse = 390,
              .threshold = 105
};
const struct Element element2 = {   //CB2, P6.2
              .inputBits = CBIMSEL_2,
              .maxResponse = 340,
              .threshold = 170
};
const struct Element element3 = {   //CB3, P6.3
              .inputBits = CBIMSEL_3,
              .maxResponse = 500,
              .threshold = 230
};
const struct Element element4 = {   //CB4, P6.4
              .inputBits = CBIMSEL_4,
              .maxResponse = 400,
              .threshold = 200
};

//*** CAP TOUCH HANDLER *******************************************************/
// This defines the grouping of sensors, the method to measure change in
// capacitance, and the function of the group

const struct Sensor slider =
               {
                  .halDefinition = fRO_COMPB_TA1_SW,
                  .numElements = 5,
                  .baseOffset = 0,
                  .cbpdBits = 0x001F, //BIT0+BIT1+BIT2+...BITE+BITF
				  .points = 100,
				  .sensorThreshold = 75,
                  // Pointer to elements
                  .arrayPtr[0] = &element0,  // point to first element
                  .arrayPtr[1] = &element1,
                  .arrayPtr[2] = &element2,
                  .arrayPtr[3] = &element3,
                  .arrayPtr[4] = &element4,

                  .cboutTAxDirRegister = (unsigned char *)&P1DIR,  // PxDIR
                  .cboutTAxSelRegister = (unsigned char *)&P1SEL,  // PxSEL
                  .cboutTAxBits = BIT6, // P1.6
                  // Timer Information
                  //.measGateSource= GATE_WDT_ACLK,     //  0->SMCLK, 1-> ACLK
                  .accumulationCycles = 50 //
               };

