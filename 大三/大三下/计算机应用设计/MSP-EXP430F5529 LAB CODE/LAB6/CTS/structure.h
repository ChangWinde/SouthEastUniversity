#ifndef CAP_TOUCH_STRUCTURE
#define CAP_TOUCH_STRUCTURE
//******************************************************************************
// structure.h
//
//
//******************************************************************************
//******************************************************************************
//******************************************************************************
// The following elements need to be configured by the user.
//******************************************************************************

#include "msp430.h"
#include <stdint.h>

/* Public Globals */
extern const struct Element element0;     //
extern const struct Element element1;     //
extern const struct Element element2;     //
extern const struct Element element3;     //
extern const struct Element element4;     //

extern const struct Sensor slider;    // structure of info for a given
                                          // group. There needs to be one
                                          // instance for each group of sensors.

//****** RAM ALLOCATION ********************************************************
// TOTAL_NUMBER_OF_ELEMENTS represents the total number of elements used, even if
// they are going to be segmented into seperate groups.  This defines the
// RAM allocation for the baseline tracking.  If only the TI_CAPT_Raw function
// is used, then this definition should be removed to conserve RAM space.
#define TOTAL_NUMBER_OF_ELEMENTS 5
// If the RAM_FOR_FLASH definition is removed, then the appropriate HEAP size
// must be allocated. 2 bytes * MAXIMUM_NUMBER_OF_ELEMENTS_PER_SENSOR + 2 bytes
// of overhead.
#define RAM_FOR_FLASH
//****** Structure Array Definition ********************************************
// This defines the array size in the sensor strucure.  In the event that
// RAM_FOR_FLASH is defined, then this also defines the amount of RAM space
// allocated (global variable) for computations.
#define MAXIMUM_NUMBER_OF_ELEMENTS_PER_SENSOR  5
//****** Choosing a  Measurement Method ****************************************
// These variables are references to the definitions found in structure.c and
// must be generated per the application.
// possible values for the method field

// OSCILLATOR DEFINITIONS
//#define RO_COMPAp_TA0_WDTp  		64
//#define RO_PINOSC_TA0_WDTp  		65
//#define RO_PINOSC_TA0       		66
//#define RO_COMPAp_TA1_WDTp  		67
//#define RO_COMPB_TA0_WDTA			68
//#define RO_COMPB_TA1_WDTA         69

// RC DEFINITIONS
//#define RC_PAIR_TA0       		01

// FAST RO DEFINITIONS
//#define fRO_PINOSC_TA0_SW           25
//#define fRO_COMPB_TA0_SW            26
#define fRO_COMPB_TA1_SW            27
//#define fRO_COMPAp_TA0_SW            28
//#define fRO_COMPAp_SW_TA0            29
//#define fRO_COMPAp_TA1_SW            30

//****** WHEEL and SLIDER ******************************************************
// Are wheel or slider representations used?
#define SLIDER
#define ILLEGAL_SLIDER_WHEEL_POSITION		0xFFFF
//#define WHEEL

//******************************************************************************
// End of user configuration section.
//******************************************************************************
//******************************************************************************
//******************************************************************************

//possible timer source clock dividers, different from clock module dividers
#define TIMER_TxCLK 	0x0000       // ID_0, IDX_0
#define TIMER_ACLK  	0x0100
#define TIMER_SMCLK 	0x0200
#define TIMER_INCLK 	0x0300

#define TIMER_SOURCE_DIV_0 0x0000
#define TIMER_SOURCE_DIV_1 0x0040
#define TIMER_SOURCE_DIV_2 0x0080
#define TIMER_SOURCE_DIV_3 0x00C0

#define GATE_WDT_ACLK      0x0004
#define GATE_WDT_SMCLK     0x0000

#define WDTp_GATE_32768    0x0000  // watchdog source/32768
#define WDTp_GATE_8192     0x0001  // watchdog source/8192
#define WDTp_GATE_512      0x0002  // watchdog source/512
#define WDTp_GATE_64       0x0003  // watchdog source/64

#define WDTA_GATE_2G       0x0000  // watchdog source/2G
#define WDTA_GATE_128M     0x0001  // watchdog source/128M
#define WDTA_GATE_8192K    0x0002  // watchdog source/8192K
#define WDTA_GATE_512K     0x0003  // watchdog source/512K
#define WDTA_GATE_32768    0x0004  // watchdog source/32768
#define WDTA_GATE_8192     0x0005  // watchdog source/8192
#define WDTA_GATE_512      0x0006  // watchdog source/512
#define WDTA_GATE_64       0x0007  // watchdog source/64

// The below variables are used to excluded portions of code not needed by
// the method chosen by the user. Uncomment the type used prior to compilation.
// Multiple types can be chosen as needed.
// What Method(s) are used in this application?

#ifdef RO_COMPAp_TA0_WDTp
    #define RO_TYPE
    #define RO_COMPAp_TYPE
    #define WDT_GATE
    #define HAL_DEFINITION
    //what devices have Pxsel2 ??
    // msp430f2112, 2122, 2132
    // msp430G2112, G2212, G2312, G2412, G2152, G2252, G2352, G2452
    // SEL2REGISTER
    #ifdef __MSP430F2112
      #define SEL2REGISTER
    #endif
    #ifdef __MSP430F2122
      #define SEL2REGISTER
    #endif
    #ifdef __MSP430F2132
      #define SEL2REGISTER
    #endif
    #ifdef __MSP430G2112
      #define SEL2REGISTER
    #endif
    #ifdef __MSP430G2212
      #define SEL2REGISTER
    #endif
    #ifdef __MSP430G2312
      #define SEL2REGISTER
    #endif
    #ifdef __MSP430G2412
      #define SEL2REGISTER
    #endif
    #ifdef __MSP430G2152
      #define SEL2REGISTER
    #endif
    #ifdef __MSP430G2252
      #define SEL2REGISTER
    #endif
    #ifdef __MSP430G2352
      #define SEL2REGISTER
    #endif
    #ifdef __MSP430G2452
      #define SEL2REGISTER
    #endif
#endif

#ifdef RO_PINOSC_TA0_WDTp
    #define RO_TYPE
    #define RO_PINOSC_TYPE
    #define WDT_GATE
    #define HAL_DEFINITION
#endif

#ifdef RO_PINOSC_TA0
    #define RO_TYPE
    #define RO_PINOSC_TYPE
    #define ACCUMULATE_TYPE
    #define HAL_DEFINITION
#endif

#ifdef RO_COMPAp_TA1_WDTp
    #define RO_TYPE
    #define RO_COMPAp_TYPE
    #define WDT_GATE
    #define HAL_DEFINITION
#endif

#ifdef RO_COMPB_TA0_WDTA
    #define RO_TYPE
    #define RO_COMPB_TYPE
    #define WDT_GATE
    #define HAL_DEFINITION
#endif

#ifdef RO_COMPB_TA1_WDTA
    #define RO_TYPE
    #define RO_COMPB_TYPE
    #define WDT_GATE
    #define HAL_DEFINITION
#endif

#ifdef RO_COMPAp_TA0_SW
    #define RO_TYPE
    #define RO_COMPAp_TYPE
    #define HAL_DEFINITION
#endif

#ifdef RO_PINOSC_TA0_SW
    #define RO_TYPE
    #define RO_PINOSC_TYPE
    #define HAL_DEFINITION
#endif

#ifdef RC_PAIR_TA0
    #define RC_TYPE
    #define RC_PAIR_TYPE
    #define ACCUMULATE_TYPE
    #define HAL_DEFINITION
#endif

#ifdef RC_PAIR_TA1
    #define RC_TYPE
    #define RC_PAIR_TYPE
    #define HAL_DEFINITION
#endif

#ifdef RC_PAIR_TD0
    #define RC_TYPE
    #define RC_PAIR_TYPE
    #define HAL_DEFINITION
#endif

#ifdef fRO_PINOSC_TA0_SW
	#define RO_TYPE
    #define RO_PINOSC_TYPE
	#define HAL_DEFINITION
#endif

#ifdef fRO_COMPB_TA0_SW
    #define RO_TYPE
    #define RO_COMPB_TYPE
	#define HAL_DEFINITION
#endif

#ifdef fRO_COMPB_TA1_SW
    #define RO_TYPE
    #define RO_COMPB_TYPE
	#define HAL_DEFINITION
#endif

#ifdef fRO_COMPAp_TA0_SW
    #define RO_TYPE
    #define RO_COMPAp_TYPE
	#define HAL_DEFINITION
#endif

#ifdef fRO_COMPAp_TA0_SW
    #define RO_TYPE
    #define RO_COMPAp_TYPE
	#define HAL_DEFINITION
#endif

#ifdef SLIDER
	#define SLIDER_WHEEL
#endif

#ifdef WHEEL
	#define SLIDER_WHEEL
#endif

#define RO_MASK         0xC0        // 1100 0000
#define RC_FRO_MASK     0x3F        // 0011 1111

//******************************************************************************
// The sensor structure identifies port or comparator input definitions for each
// sensor.
//******************************************************************************
struct Element{

#ifdef RO_PINOSC_TYPE
// These register address definitions are needed for each sensor only
// when using the PinOsc method
  uint8_t *inputPxselRegister;    // PinOsc: port selection address
  uint8_t *inputPxsel2Register;   // PinOsc: port selection 2 address
#endif

#ifdef RC_PAIR_TYPE
// these fields are specific to the RC type.
  uint8_t *inputPxoutRegister;    // RC: port output address: PxOUT
  uint8_t *inputPxinRegister;     // RC: port input address: PxIN
  uint8_t *inputPxdirRegister;    // RC+PinOsc: port direction address
  uint8_t *referencePxoutRegister;// RC: port output address: PxOUT
  uint8_t *referencePxdirRegister;// RC: port direction address: PxDIR
  uint8_t referenceBits;           // RC: port bit definition
#endif
 		
  uint16_t inputBits;                 // Comp_RO+FastRO+RC+PinOsc: bit
                                      // definition
                                      //
                                      // for comparator input bit
                                      // location in CACTL2 or CBCTL0

  uint16_t threshold;                   // specific threshold for each button
  uint16_t maxResponse;                 // Special Case: Slider max counts
};

//******************************************************************************
// The following structure definitons are application independent and are not
// intended to be modified.
//
// The CT_handler 'groups' the sensor based upon function and capacitive
// measurement method.
//******************************************************************************

struct Sensor{
  // the method acts as the switch to determine which HAL is called
  uint8_t halDefinition;           // COMPARATOR_TYPE (RO), RC, etc
                                   // RO_COMPA, RO_COMPB, RO_PINOSC
                                   // RC_GPIO, RC_COMPA, RC_COMPB
                                   // FAST_SCAN_RO

  uint8_t numElements;             // number of elements within group
  uint8_t baseOffset;              // the offset within the global
                                   // base_cnt array

  struct Element const *arrayPtr[MAXIMUM_NUMBER_OF_ELEMENTS_PER_SENSOR];
                                   // an array of pointers

//******************************************************************************
// Reference structure definitions for comparator types, for the RC method the
// reference is defined within the element.

#ifdef RO_COMPAp_TYPE
  uint8_t * refPxoutRegister;      // RO+FastRO: port output address
  uint8_t * refPxdirRegister;      // RO+FastRO: port direction address
  uint8_t refBits;                 // RO+FastRO: port bit definition

  uint8_t * txclkDirRegister;      // PxDIR
  uint8_t * txclkSelRegister;      // PxSEL
  uint8_t txclkBits;               // Bit field for register

  uint8_t *caoutDirRegister;      // PxDIR
  uint8_t *caoutSelRegister;      // PxSEL
  uint8_t caoutBits;               // Bit field for register

  // This is only applicable to the RO_COMPAp_TYPE
#ifdef SEL2REGISTER
  uint8_t *caoutSel2Register;
  uint8_t *txclkSel2Register;
#endif

  uint8_t refCactl2Bits;          // RO: CACTL2 input definition,
                                          // CA0 (P2CA0),CA1(P2CA4),
                                          // CA2(P2CA0+P2CA4)
  uint8_t capdBits;
#endif

#ifdef RO_COMPB_TYPE
  uint8_t *cboutTAxDirRegister;  // CBOUT_TA0CLK
  uint8_t *cboutTAxSelRegister;  // CBOUT_TA0CLK
  uint8_t cboutTAxBits;           // Bit field for register
  uint16_t cbpdBits;
#endif

//*****************************************************************************
// Timer definitions
//  The basic premise is to count a number of clock cycles within a time
//  period, where either the clock source or the timer period is a function
//  of the element capacitance.
//
// RC Method:
//          Period: accumulationCycles * charge and discharge time of RC
//          circuit where C is captouch element
//
//          clock source: measGateSource/sourceScale
// RO Method:
//          Period: accumulationCycles*measGateSource/sourceScale
//                  (with WDT sourceScale = 1, accumulationCycles is WDT control
//                   register settings)
//
//          clock source: relaxation oscillator where freq is a function of C
//
// fRO Method:
//          Period: accumulationCycles * 1/freq, freq is a function of C
//
//          clock source: measGateSource/sourceScale

  uint16_t measGateSource;         // RC+FastRO: measurement timer source,
                                   // {ACLK, TACLK, SMCLK}
                                   // Comp_RO+PinOsc: gate timer source,
                                   // {ACLK, TACLK, SMCLK}
  uint16_t sourceScale;            // Comp_RO+FastRO+PinOsc: gate timer,
                                   // TA/TB/TD, scale: 1,1/2,1/4,1/8
                                   // RC+FastRO: measurement timer, TA/TB/TD
                                   // scale: 16, 8, 4, 2, 1, ½, ¼, 1/8

  uint16_t accumulationCycles;

//*****************************************************************************
// Other definitions

#ifdef SLIDER_WHEEL
  uint8_t points;                   // Special Case: Number of points
                                    // along slider or wheel
  uint8_t sensorThreshold;
#endif

};

//******************************************************************************
// The scheduler structure manages each handler group (CT_Handler object) using
// one timer resource.
// TBD
//******************************************************************************
struct CT_scheduler{
  void *callBacks;
  // pointer to function in the main application (can be an array of functions
  // if multiple groups). The order is important! The pointer here must point
  // to the first CT_Handler object!

  struct CT_Handler *handler;             // order of this and *call_backs
                                          // must be the same.

  uint8_t Enable;                   // Each bit in this value will show
                                          // if the given group element
                                          // is being sheduled & measured.

  uint8_t delayTimer;              // delay timer, {SW,WDT, TimerA,
                                          // TimerB, TimerD}

  uint8_t delayTimerSrc;          // delay timer source,
                                          // {ACLK, TACLK, SMCLK}

  uint8_t delayTimerScale;        // delay time scale (/1,/2,/4,/8)

  uint8_t delayTime;               // wdt: delay {32768,8192,512,64}
 		                          // RO+FastRO+PinOsc(TimerA,B,D):
                                          // delay time {257+x*256}
 		
};

#ifndef TOTAL_NUMBER_OF_ELEMENTS
 #warning "WARNING: TOTAL_NUMBER_OF_ELEMENTS is not defined in structure.h. Only TI_CAPT_RAW function is enabled."
#endif

#ifndef RAM_FOR_FLASH
 #warning "WARNING: The HEAP must be set appropriately.  Please refer to SLAA490 for details."
#endif

#ifndef HAL_DEFINITION
 #warning "WARNING: At least one HAL definition must be made in structure.h."
#endif

#endif


