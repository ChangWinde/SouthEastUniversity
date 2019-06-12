// (c)2010 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: usb.c, File Version 1.02 2010/06/17                                |
|  Author: RSTO                                                               |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  RSTO         2008/09/03   born                                             |
|  RSTO         2008/12/23   enhancements of CDC API                          |
|  RSTO         2009/01/12   enhancements for USB serial number               |
|  RSTO         2009/05/15   added USB_connectionState()                      |
|  RSTO         2009/07/17   added __data16 qualifier for USB buffers         |
|  RSTO         2009/08/04   workaround for PLL start up problem              |
|  MSP,Biju     2009/10/20   Changes for composite support                    |
|  RSTO         2009/10/21   updated USB_InitSerialStringDescriptor()         |
|  RSTO         2009/11/05   updated USB_connectionState()                    |
|  MSP,Biju     2010/07/15   Updated for MSC                                  |
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| Include files                                                               |
+----------------------------------------------------------------------------*/

#include "../USB_Common/device.h"
#include "../USB_Common/types.h"              // Basic Type declarations
#include "../USB_Common/defMSP430USB.h"
#include "../USB_Common/usb.h"  // USB-specific Data Structures
#include "../USB_CDC_API/UsbCdc.h"
#include "../USB_HID_API/UsbHidReq.h"
#include "../USB_MSC_API/UsbMscScsi.h"
#include <descriptors.h>

#include <HAL_UCS.h>
#include <HAL_TLV.h>
#include <string.h>

/*----------------------------------------------------------------------------+
 | Internal Constant Definition                                               |
 +----------------------------------------------------------------------------*/
#define NO_MORE_DATA    0xFFFF
#define EPBCT_NAK       0x80
#define EPCNF_TOGLE     0x20

#define DIRECTION_IN	0x80
#define DIRECTION_OUT	0x00

/*----------------------------------------------------------------------------+
| Internal Variables                                                          |
+----------------------------------------------------------------------------*/

static BYTE bConfigurationNumber;      // Set to 1 when USB device has been
                                // configured, set to 0 when unconfigured

static BYTE bInterfaceNumber;   // interface number

WORD wBytesRemainingOnIEP0;     // For endpoint zero transmitter only
                                // Holds count of bytes remaining to be
                                // transmitted by endpoint 0.  A value
                                // of 0 means that a 0-length data packet
                                // A value of 0xFFFF means that transfer
                                // is complete.

WORD wBytesRemainingOnOEP0;     // For endpoint zero transmitter only
                                // Holds count of bytes remaining to be
                                // received by endpoint 0.  A value
                                // of 0 means that a 0-length data packet
                                // A value of 0xFFFF means that transfer
                                // is complete.

static PBYTE pbIEP0Buffer;      // A buffer pointer to input end point 0
                                // Data sent back to host is copied from
                                // this pointed memory location

static PBYTE pbOEP0Buffer;      // A buffer pointer to output end point 0
                                // Data sent from host is copied to
                                // this pointed memory location

static BYTE bHostAskMoreDataThanAvailable=0;

BYTE abUsbRequestReturnData[USB_RETURN_DATA_LENGTH];
BYTE abUsbRequestIncomingData[USB_RETURN_DATA_LENGTH];

__no_init BYTE abramSerialStringDescriptor[34];

BYTE bStatusAction;
BYTE bFunctionSuspended=FALSE;  // TRUE if function is suspended
BYTE bEnumerationStatus = 0;    //is 0 if not enumerated

static BYTE bRemoteWakeup;

WORD wUsbEventMask;             //used by USB_getEnabledEvents() and USB_setEnabledEvents()

#ifdef _MSC_
void MscResetData();
extern BOOL bMcsCommandSupported;
extern BOOL isMSCConfigured;

extern BYTE bMscResetRequired;
#endif

/*----------------------------------------------------------------------------+
| Global Variables                                                            |
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| Hardware Related Structure Definition                                       |
+----------------------------------------------------------------------------*/

#ifdef __IAR_SYSTEMS_ICC__

#pragma location = 0x2380
__no_init tDEVICE_REQUEST __data16 tSetupPacket;

#pragma location = 0x0920
__no_init tEDB0 __data16 tEndPoint0DescriptorBlock;

#pragma location = 0x23C8
__no_init tEDB __data16 tInputEndPointDescriptorBlock[7];

#pragma location = 0x2388
__no_init tEDB __data16 tOutputEndPointDescriptorBlock[7];

#pragma location = 0x2378
__no_init BYTE __data16 abIEP0Buffer[EP0_MAX_PACKET_SIZE];

#pragma location = 0x2370
__no_init BYTE __data16 abOEP0Buffer[EP0_MAX_PACKET_SIZE];

#pragma location = OEP1_X_BUFFER_ADDRESS
 __no_init BYTE __data16 pbXBufferAddressEp1[EP_MAX_PACKET_SIZE];

#pragma location = OEP1_Y_BUFFER_ADDRESS
 __no_init BYTE __data16 pbYBufferAddressEp1[EP_MAX_PACKET_SIZE];

#pragma location = IEP1_X_BUFFER_ADDRESS
 __no_init BYTE __data16 pbXBufferAddressEp81[EP_MAX_PACKET_SIZE];

#pragma location = IEP1_Y_BUFFER_ADDRESS
 __no_init BYTE __data16 pbYBufferAddressEp81[EP_MAX_PACKET_SIZE];

#pragma location = OEP2_X_BUFFER_ADDRESS
 __no_init BYTE __data16 pbXBufferAddressEp2[EP_MAX_PACKET_SIZE];

#pragma location = OEP2_Y_BUFFER_ADDRESS
 __no_init BYTE __data16 pbYBufferAddressEp2[EP_MAX_PACKET_SIZE];

#pragma location = IEP2_X_BUFFER_ADDRESS
 __no_init BYTE __data16 pbXBufferAddressEp82[EP_MAX_PACKET_SIZE];

#pragma location = IEP2_Y_BUFFER_ADDRESS
 __no_init BYTE __data16 pbYBufferAddressEp82[EP_MAX_PACKET_SIZE];

#pragma location = OEP3_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp3[EP_MAX_PACKET_SIZE];

#pragma location = OEP3_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp3[EP_MAX_PACKET_SIZE];

#pragma location = IEP3_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp83[EP_MAX_PACKET_SIZE];

#pragma location = IEP3_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp83[EP_MAX_PACKET_SIZE];

#pragma location = OEP4_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp4[EP_MAX_PACKET_SIZE];

#pragma location = OEP4_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp4[EP_MAX_PACKET_SIZE];

#pragma location = IEP4_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp84[EP_MAX_PACKET_SIZE];

#pragma location = IEP4_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp84[EP_MAX_PACKET_SIZE];

#pragma location = OEP5_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp5[EP_MAX_PACKET_SIZE];

#pragma location = OEP5_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp5[EP_MAX_PACKET_SIZE];

#pragma location = IEP5_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp85[EP_MAX_PACKET_SIZE];

#pragma location = IEP5_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp85[EP_MAX_PACKET_SIZE];

#pragma location = OEP6_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp6[EP_MAX_PACKET_SIZE];

#pragma location = OEP6_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp6[EP_MAX_PACKET_SIZE];

#pragma location = IEP6_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp86[EP_MAX_PACKET_SIZE];

#pragma location = IEP6_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp86[EP_MAX_PACKET_SIZE];

#pragma location = OEP7_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp7[EP_MAX_PACKET_SIZE];

#pragma location = OEP7_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp7[EP_MAX_PACKET_SIZE];

#pragma location = IEP7_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp87[EP_MAX_PACKET_SIZE];

#pragma location = IEP7_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp87[EP_MAX_PACKET_SIZE];



#endif

#ifdef __TI_COMPILER_VERSION__
extern __no_init tDEVICE_REQUEST tSetupPacket;
extern __no_init tEDB0 tEndPoint0DescriptorBlock;
extern __no_init tEDB tInputEndPointDescriptorBlock[7];
extern __no_init tEDB tOutputEndPointDescriptorBlock[7];
extern __no_init BYTE abIEP0Buffer[EP0_MAX_PACKET_SIZE];
extern __no_init BYTE abOEP0Buffer[EP0_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp1[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp1[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp81[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp81[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp2[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp2[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp82[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp82[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp3[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp3[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp83[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp83[EP_MAX_PACKET_SIZE];

extern __no_init BYTE pbXBufferAddressEp4[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp4[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp84[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp84[EP_MAX_PACKET_SIZE];

extern __no_init BYTE pbXBufferAddressEp5[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp5[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp85[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp85[EP_MAX_PACKET_SIZE];

#endif

VOID CdcResetData();
VOID HidResetData();

VOID USB_InitSerialStringDescriptor(VOID);
VOID USB_initMemcpy(VOID);

//----------------------------------------------------------------------------
BYTE USB_init(VOID)
{
    WORD bGIE  = __get_SR_register() &GIE;  //save interrupt status
    // atomic operation - disable interrupts
    __disable_interrupt();               // Disable global interrupts

    // configuration of USB module
    USBKEYPID   =     0x9628;            // set KEY and PID to 0x9628 -> access to configuration registers enabled

    USBPHYCTL   =     PUSEL;             // use DP and DM as USB terminals (not needed because an external PHY is connected to port 9)

    USBPWRCTL   =     VUSBEN + SLDOAON; // enable primary and secondary LDO (3.3 and 1.8 V)
    {
    	volatile unsigned int i;
    	for (i =0; i < USB_MCLK_FREQ/1000*2/10; i++);      // wait some time for LDOs (1ms delay)
    }

    USBPWRCTL   =   VUSBEN + SLDOAON + VBONIE;  // enable interrupt VBUSon
    USBKEYPID   =    0x9600;            // access to configuration registers disabled

    //reset events mask
    wUsbEventMask = 0;

    //init Serial Number
#if (USB_STR_INDEX_SERNUM != 0)
    USB_InitSerialStringDescriptor();
#endif

    // init memcpy() function: DMA or non-DMA
    USB_initMemcpy();
#ifdef _MSC_
    MscResetCtrlLun();
#endif

    __bis_SR_register(bGIE); //restore interrupt status
    return kUSB_succeed;
}

//----------------------------------------------------------------------------
// This function will be compiled only if
#if (USB_STR_INDEX_SERNUM != 0)
VOID USB_InitSerialStringDescriptor(VOID)
{
    BYTE i,j,hexValue;
    PBYTE pbSerNum;
    BYTE bBytes;

    j=1;                   // we start with second byte, first byte (lenght) will be filled later
    pbSerNum=0;
    abramSerialStringDescriptor[j++] = DESC_TYPE_STRING;

    // TLV access Function Call
    Get_TLV_Info(TLV_DIERECORD, 0, (uint8_t *)&bBytes, (uint16_t **)&pbSerNum); //The die record used for serial number
    if (bBytes == 0)    // no serial number available
    {
        // use 00 as serial number = no serial number available
        abramSerialStringDescriptor[0] = 4;      //length
        abramSerialStringDescriptor[j++] = 0;    // no serial number available
        abramSerialStringDescriptor[j++] = 0;    // no serial number available
    }
    else
    {
        for(i=0; (i<bBytes)&&(i<8); i++,pbSerNum++)
        {
            hexValue = (*pbSerNum & 0xF0)>> 4;
            if(hexValue < 10 ) abramSerialStringDescriptor[j++] = (hexValue + '0');
            else abramSerialStringDescriptor[j++] = (hexValue + 55);
            abramSerialStringDescriptor[j++] = 0x00;  // needed for UNI-Code

            hexValue = (*pbSerNum & 0x0F);
            if(hexValue < 10 ) abramSerialStringDescriptor[j++] = (hexValue + '0');
            else abramSerialStringDescriptor[j++] = (hexValue + 55);
            abramSerialStringDescriptor[j++] = 0x00;    // needed for UNI-Code
        }
        abramSerialStringDescriptor[0] = i*4 +2;        // calculate the length
    }
}
#endif

//----------------------------------------------------------------------------

BYTE USB_enable()
{
    volatile unsigned int i;
    volatile unsigned int j = 0;

    if (!(USBPWRCTL & USBBGVBV))            // check USB Bandgap and VBUS valid
    {
        return kUSB_generalError;
    }

    if ((USBCNF & USB_EN) &&
        (USBPLLCTL & UPLLEN))
    {
        return kUSB_succeed;                // exit if PLL is already enalbed
    }

    USBKEYPID = 0x9628;                     // set KEY and PID to 0x9628 -> access to configuration registers enabled
    XT2_Start(XT2DRIVE_3);
    USBPLLDIVB = USB_XT_FREQ;               // Settings desired frequency

    if (USB_PLL_XT == 2)
    {
        USBPLLCTL = UPCS0 + UPFDEN + UPLLEN;// Select XT2 as Ref / Select PLL for USB / Discrim. on, enable PLL
    }
    else
    {
        USBPLLCTL = UPFDEN + UPLLEN;        // Select XT1 as Ref / Select PLL for USB / Discrim. on, enable PLL
    }

    //Wait some time till PLL is settled
    do
    {
        USBPLLIR    =     0x0000;           // make sure no interrupts can occur on PLL-module

#ifdef __MSP430F6638
        //wait 1 ms till enable USB
        for (i =0; i < USB_MCLK_FREQ/1000*1/10; i++);
#else
        //wait 1/2 ms till enable USB
        for (i =0; i < USB_MCLK_FREQ/1000* 1/2 /10; i++);
#endif

        if (j++ > 10)
        {
            USBKEYPID   =    0x9600;        // access to configuration registers disabled
            return kUSB_generalError;
        }
    }while (USBPLLIR != 0);

    USBCNF     |=    USB_EN;                // enable USB module
    USBKEYPID   =    0x9600;                // access to configuration registers disabled
    return kUSB_succeed;
}

/*
Disables the USB module and PLL.
*/
BYTE USB_disable(VOID)
{
    USBKEYPID = 0x9628;        // set KEY and PID to 0x9628 -> access to configuration registers enabled
    USBCNF    = 0;             // disable USB module
    USBPLLCTL &= ~UPLLEN;      // disable PLL
    USBKEYPID = 0x9600;        // access to configuration registers disabled
    bEnumerationStatus = 0x00; // device is not enumerated
    bFunctionSuspended = FALSE;// device is not suspended
    return kUSB_succeed;
}

/*
Enables/disables various USB events.
*/
BYTE USB_setEnabledEvents(WORD events)
{
    wUsbEventMask = events;
    return kUSB_succeed;
}

/*
Returns which events are enabled and which are disabled.
*/
WORD USB_getEnabledEvents()
{
    return wUsbEventMask;
}

/*
Reset USB-SIE and global variables.
*/
BYTE USB_reset()
{
    int i;
    USBKEYPID = 0x9628;                   // set KEY and PID to 0x9628 -> access to configuration registers enabled

    //reset should be on the bus after this!
    bEnumerationStatus = 0x00;            // Device not enumerated yet
    bFunctionSuspended = FALSE;           // Device is not in suspend mode

    bRemoteWakeup = DISABLE;

    bConfigurationNumber    = 0x00;       // device unconfigured
    bInterfaceNumber        = 0x00;

    // FRSTE handling:
    // Clear FRSTE in the RESRIFG interrupt service routine before re-configuring USB control registers.
    // Set FRSTE at the beginning of SUSRIFG, SETUP, IEPIFG.EP0 and OEPIFG.EP0 interrupt service routines.
    USBCTL = 0;                           // Function Reset Connection disable (FRSTE)

    wBytesRemainingOnIEP0   = NO_MORE_DATA;
    wBytesRemainingOnOEP0   = NO_MORE_DATA;
    bStatusAction           = STATUS_ACTION_NOTHING;

    //The address reset normally will be done automatically during bus function reset
    USBFUNADR   =     0x00;               // reset address of USB device (unconfigured)

    /* Set settings for EP0 */
    // NAK both 0 endpoints and enable endpoint 0 interrupt
    tEndPoint0DescriptorBlock.bIEPBCNT = EPBCNT_NAK;
    tEndPoint0DescriptorBlock.bOEPBCNT = EPBCNT_NAK;
    tEndPoint0DescriptorBlock.bIEPCNFG = EPCNF_USBIE | EPCNF_UBME | EPCNF_STALL;    // 8 byte data packet
    tEndPoint0DescriptorBlock.bOEPCNFG = EPCNF_USBIE | EPCNF_UBME | EPCNF_STALL;    // 8 byte data packet

    USBOEPIE = USB_OUTEP_INT_EN;
    USBIEPIE = USB_INEP_INT_EN;

    // loop for initialization all of used enpoints
    for(i=0; i < (CDC_NUM_INTERFACES + HID_NUM_INTERFACES + MSC_NUM_INTERFACES); i++)
    {
        BYTE edbIndex = stUsbHandle[i].edb_Index;

        /* Set settings for IEPx */
        tInputEndPointDescriptorBlock[edbIndex].bEPCNF   = EPCNF_USBIE | EPCNF_UBME | EPCNF_DBUF; //double buffering
        tInputEndPointDescriptorBlock[edbIndex].bEPBBAX  = (BYTE)(((stUsbHandle[i].iep_X_Buffer - START_OF_USB_BUFFER) >> 3) & 0x00ff);
        tInputEndPointDescriptorBlock[edbIndex].bEPBBAY  = (BYTE)(((stUsbHandle[i].iep_Y_Buffer - START_OF_USB_BUFFER) >> 3) & 0x00ff);
        tInputEndPointDescriptorBlock[edbIndex].bEPBCTX  = EPBCNT_NAK;
        tInputEndPointDescriptorBlock[edbIndex].bEPBCTY  = EPBCNT_NAK;
        tInputEndPointDescriptorBlock[edbIndex].bEPSIZXY = MAX_PACKET_SIZE;

        /* Set settings for OEPx */
        tOutputEndPointDescriptorBlock[edbIndex].bEPCNF   = EPCNF_USBIE | EPCNF_UBME | EPCNF_DBUF ; //double buffering
        tOutputEndPointDescriptorBlock[edbIndex].bEPBBAX  = (BYTE)(((stUsbHandle[i].oep_X_Buffer - START_OF_USB_BUFFER) >> 3) & 0x00ff);
        tOutputEndPointDescriptorBlock[edbIndex].bEPBBAY  = (BYTE)(((stUsbHandle[i].oep_Y_Buffer - START_OF_USB_BUFFER) >> 3) & 0x00ff);
        tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX  = 0x00;
        tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY  = 0x00;
        tOutputEndPointDescriptorBlock[edbIndex].bEPSIZXY = MAX_PACKET_SIZE;

#       ifdef _CDC_
        /* Additional interrupt end point for CDC */
        if(stUsbHandle[i].dev_Class == CDC_CLASS)
        {
            // The decriptor tool always generates the managemnet endpoint before the data endpoint
            tInputEndPointDescriptorBlock[edbIndex-1].bEPCNF   = EPCNF_USBIE | EPCNF_UBME | EPCNF_DBUF; //double buffering
            tInputEndPointDescriptorBlock[edbIndex-1].bEPBBAX  = (BYTE)(((stUsbHandle[i].intepEP_X_Buffer - START_OF_USB_BUFFER) >> 3) & 0x00ff);
            tInputEndPointDescriptorBlock[edbIndex-1].bEPBBAY  = (BYTE)(((stUsbHandle[i].intepEP_Y_Buffer - START_OF_USB_BUFFER) >> 3) & 0x00ff);
            tInputEndPointDescriptorBlock[edbIndex-1].bEPBCTX  = EPBCNT_NAK;
            tInputEndPointDescriptorBlock[edbIndex-1].bEPBCTY  = EPBCNT_NAK;
            tInputEndPointDescriptorBlock[edbIndex-1].bEPSIZXY = MAX_PACKET_SIZE;
        }
#       endif
    }

#   ifdef _HID_
        HidResetData();                     // reset HID specific data structures
#   endif // _HID_

#   ifdef _MSC_
        isMSCConfigured = FALSE;
        MscResetData();
#   endif

#   ifdef _CDC_
        CdcResetData();                     // reset CDC specific data structures
#   endif // _CDC_

    USBCTL = FEN;                       // enable function
    USBIFG = 0;                         // make sure no interrupts are pending

    USBIE = SETUPIE | RSTRIE | SUSRIE;  // enable USB specific interrupts (setup, reset, suspend)
    USBKEYPID = 0x9600;                 // access to configuration registers disabled
    return kUSB_succeed;
}

/*
Instruct USB module to make itself available to the PC for connection, by pulling PUR high.
*/
BYTE USB_connect()
{
    USBKEYPID = 0x9628;   // set KEY and PID to 0x9628 -> access to configuration registers enabled
    USBCNF |= PUR_EN;     // generate rising edge on DP -> the host enumerates our device as full speed device
    USBPWRCTL |= VBOFFIE; // enable interrupt VUSBoff
    USBKEYPID = 0x9600;   // access to configuration registers disabled

    // after this the enumeration may take place
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();

    return kUSB_succeed;
}

/*
Force a disconnect from the PC by pulling PUR low.
*/
BYTE USB_disconnect()
{
    USBKEYPID = 0x9628;     // set KEY and PID to 0x9628 -> access to configuration registers enabled
    USBCNF &= ~PUR_EN;      // disconnect pull up resistor - logical disconnect from HOST
    USBPWRCTL &= ~VBOFFIE;  // disable interrupt VUSBoff
    USBKEYPID = 0x9600;     // access to configuration registers disabled
    bEnumerationStatus = 0; // not enumerated
    bFunctionSuspended = FALSE;     // device is not suspended
    return kUSB_succeed;
}

/*
Force a remote wakeup of the USB host.
*/
BYTE USB_forceRemoteWakeup()
{
    if (bFunctionSuspended == FALSE) // device is not suspended
    {
        return kUSB_NotSuspended;
    }
    if(bRemoteWakeup == ENABLE)
    {
    	volatile unsigned int i;
        USBCTL |= RWUP;             // USB - Device Remote Wakeup Request - this bit is self-cleaned
        return kUSB_succeed;
    }
    return kUSB_generalError;
}

/*
Returns the status of the USB connection.
*/
BYTE USB_connectionInfo()
{
    BYTE retVal = 0;
    if (USBPWRCTL & USBBGVBV)
    {
        retVal |= kUSB_vbusPresent;
    }

    if (bEnumerationStatus == ENUMERATION_COMPLETE)
    {
        retVal |= kUSB_Enumerated;
    }

    if (USBCNF & PUR_EN)
    {
        retVal |= kUSB_purHigh;
    }

    if (bFunctionSuspended == TRUE)
    {
        retVal |= kUSB_suspended;
    }
    else
    {
        retVal |= kUSB_NotSuspended;
    }
    return retVal;
}

/*
Returns the state of the USB connection.
*/
BYTE USB_connectionState()
{
    // If no VBUS present
    if (!(USBPWRCTL & USBBGVBV))
    {
        return ST_USB_DISCONNECTED;
    }

    // If VBUS present, but PUR is low
    if ((USBPWRCTL & USBBGVBV)&&(!(USBCNF & PUR_EN)))
    {
        return ST_USB_CONNECTED_NO_ENUM;
    }

    // If VBUS present, PUR is high, and enumeration is complete, and not suspended
    if ((USBPWRCTL & USBBGVBV) && (USBCNF & PUR_EN)
        && (bEnumerationStatus == ENUMERATION_COMPLETE)
        && (!(bFunctionSuspended == TRUE)))
    {
        return ST_ENUM_ACTIVE;
    }

    // If VBUS present, PUR is high, and enumeration is NOT complete, and  suspended
    if ((USBPWRCTL & USBBGVBV) && (USBCNF & PUR_EN)
        && (!(bEnumerationStatus == ENUMERATION_COMPLETE))
        && (bFunctionSuspended == TRUE))
    {
        return ST_NOENUM_SUSPENDED;
    }

    // If VBUS present, PUR is high, and enumeration is complete, and  suspended
    if ((USBPWRCTL & USBBGVBV) && (USBCNF & PUR_EN)
        && (bEnumerationStatus == ENUMERATION_COMPLETE)
        && (bFunctionSuspended == TRUE))
    {
        return ST_ENUM_SUSPENDED;
    }

    // If VBUS present, PUR is high, but no enumeration yet
    if ((USBPWRCTL & USBBGVBV) && (USBCNF & PUR_EN)
        && (!(bEnumerationStatus == ENUMERATION_COMPLETE)))
    {
        return ST_ENUM_IN_PROGRESS;
    }

    return ST_ERROR;
}

//----------------------------------------------------------------------------

BYTE USB_suspend(VOID)
{

    bFunctionSuspended  = TRUE;
    USBKEYPID = 0x9628;         // set KEY and PID to 0x9628 -> access to configuration registers enabled
    USBCTL |= FRSTE;            // Function Reset Connection Enable
    USBIFG &= ~SUSRIFG;         // clear interrupt flag

    if(USB_DISABLE_XT_SUSPEND)
    {
        if (USB_PLL_XT == 2)
        {
            USBPLLCTL &= ~UPLLEN;         // disable PLL
            UCSCTL6   |= XT2OFF;         // disable XT2
        }
        else
        {
            USBPLLCTL &= ~UPLLEN;           // disable PLL
            UCSCTL6 |= XT1OFF;
        }
    }

    USBIE = RESRIE;             // disable USB specific interrupts (setup, suspend, reset), enable resume.
                                // If the reset occured during device in suspend, the resume-interrupt will come, after - reset interrupt
    USBKEYPID = 0x9600;         // access to configuration registers disabled

    return kUSB_succeed;
}

//----------------------------------------------------------------------------

BYTE USB_resume(VOID)
{
    USB_enable();                       // enable PLL

    USBIFG &= ~(RESRIFG | SUSRIFG);     // clear interrupt flags
    USBIE = SETUPIE | RSTRIE | SUSRIE;  // enable USB specific interrupts (setup, reset, suspend)

    bFunctionSuspended  = FALSE;
    return kUSB_succeed;
}

//----------------------------------------------------------------------------

VOID usbStallEndpoint0(VOID)
{
    tEndPoint0DescriptorBlock.bIEPCNFG |= EPCNF_STALL;
    tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;
}

//----------------------------------------------------------------------------

VOID usbClearOEP0ByteCount(VOID)
{
    tEndPoint0DescriptorBlock.bOEPBCNT = 0x00;
}

//----------------------------------------------------------------------------

VOID usbStallOEP0(VOID)
{
    // in standard USB request, there is not control write request with data stage
    // control write, stall output endpoint 0
    // wLength should be 0 in all cases
    tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;
}

//----------------------------------------------------------------------------

VOID usbSendNextPacketOnIEP0(VOID)
{
    BYTE bPacketSize,bIndex;

    // First check if there are bytes remaining to be transferred
    if(wBytesRemainingOnIEP0 != NO_MORE_DATA)
    {
        if(wBytesRemainingOnIEP0 > EP0_PACKET_SIZE)
        {
            // More bytes are remaining than will fit in one packet
            // there will be More IN Stage
            bPacketSize = EP0_PACKET_SIZE;
            wBytesRemainingOnIEP0 -= EP0_PACKET_SIZE;
            bStatusAction = STATUS_ACTION_DATA_IN;
        }
        else if (wBytesRemainingOnIEP0 < EP0_PACKET_SIZE)
        {
            // The remaining data will fit in one packet.
            // This case will properly handle wBytesRemainingOnIEP0 == 0
            bPacketSize = (BYTE)wBytesRemainingOnIEP0;
            wBytesRemainingOnIEP0 = NO_MORE_DATA;        // No more data need to be Txed
            bStatusAction = STATUS_ACTION_NOTHING;
        }
        else
        {
            bPacketSize = EP0_PACKET_SIZE;
            if(bHostAskMoreDataThanAvailable == TRUE)
            {
                wBytesRemainingOnIEP0 = 0;
                bStatusAction = STATUS_ACTION_DATA_IN;
            }
            else
            {
                wBytesRemainingOnIEP0 = NO_MORE_DATA;
                bStatusAction = STATUS_ACTION_NOTHING;
            }
        }

        for(bIndex=0; bIndex<bPacketSize; bIndex++)
        {
            abIEP0Buffer[bIndex] = *pbIEP0Buffer;
            pbIEP0Buffer++;
        }
        tEndPoint0DescriptorBlock.bIEPBCNT = bPacketSize;
    }
    else
    {
        bStatusAction = STATUS_ACTION_NOTHING;
    }
}

//----------------------------------------------------------------------------

VOID usbSendDataPacketOnEP0(PBYTE pbBuffer)
{
    WORD wTemp;

    pbIEP0Buffer = pbBuffer;
    wTemp = tSetupPacket.wLength;

    // Limit transfer size to wLength if needed
    // this prevent USB device sending 'more than require' data back to host
    if(wBytesRemainingOnIEP0 >= wTemp)
    {
        wBytesRemainingOnIEP0 = wTemp;
        bHostAskMoreDataThanAvailable = FALSE;
    }
    else
    {
        bHostAskMoreDataThanAvailable = TRUE;
    }
    usbSendNextPacketOnIEP0();
}

//----------------------------------------------------------------------------
VOID usbReceiveNextPacketOnOEP0(VOID)
{
    BYTE bIndex,bByte;

    bByte = tEndPoint0DescriptorBlock.bOEPBCNT & EPBCNT_BYTECNT_MASK;

    if(wBytesRemainingOnOEP0 >= (WORD)bByte)
    {
        for(bIndex=0;bIndex<bByte;bIndex++)
        {
            *pbOEP0Buffer = abOEP0Buffer[bIndex];
            pbOEP0Buffer++;
        }
        wBytesRemainingOnOEP0 -= (WORD)bByte;

        // clear the NAK bit for next packet
        if(wBytesRemainingOnOEP0 > 0)
        {
            usbClearOEP0ByteCount();
            bStatusAction = STATUS_ACTION_DATA_OUT;
        }
        else
        {
            usbStallOEP0();
            bStatusAction = STATUS_ACTION_NOTHING;
        }
    }
    else
    {
        usbStallOEP0();
        bStatusAction = STATUS_ACTION_NOTHING;
    }
}

//----------------------------------------------------------------------------

VOID usbReceiveDataPacketOnEP0(PBYTE pbBuffer)
{

    pbOEP0Buffer = pbBuffer;

    wBytesRemainingOnOEP0 = tSetupPacket.wLength;
    bStatusAction = STATUS_ACTION_DATA_OUT;

    usbClearOEP0ByteCount();
}

//----------------------------------------------------------------------------

VOID usbSendZeroLengthPacketOnIEP0(VOID)
{
    wBytesRemainingOnIEP0 = NO_MORE_DATA;
    bStatusAction = STATUS_ACTION_NOTHING;
    tEndPoint0DescriptorBlock.bIEPBCNT = 0x00;
}

//----------------------------------------------------------------------------

VOID usbClearEndpointFeature(VOID)
{
    BYTE bEndpointNumber;

    // EP is from EP1 to EP7 while C language start from 0
    bEndpointNumber = (tSetupPacket.wIndex & EP_DESC_ADDR_EP_NUM);
    if(bEndpointNumber == 0x00) usbSendZeroLengthPacketOnIEP0();
    else
    {
        bEndpointNumber--;
        if(bEndpointNumber < MAX_ENDPOINT_NUMBER)
        {
            if((tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN) == EP_DESC_ADDR_DIR_IN)
            {
#ifdef _MSC_
                if (!bMscResetRequired) {
#endif
                  tInputEndPointDescriptorBlock[bEndpointNumber].bEPCNF &= ~(EPCNF_STALL | EPCNF_TOGGLE );
#ifdef _MSC_
                }
#endif
#               ifdef _MSC_
                    if (stUsbHandle[MSC0_INTFNUM].edb_Index == bEndpointNumber)
                    {
                        MscReadControl.bCurrentBufferXY = 0; //Set current buffer to X
                        bMcsCommandSupported = TRUE;
                    }
#               endif
            }
            else
            {
#ifdef _MSC_
                if (!bMscResetRequired) {
#endif
                    tOutputEndPointDescriptorBlock[bEndpointNumber].bEPCNF &= ~(EPCNF_STALL | EPCNF_TOGGLE );
#ifdef _MSC_
                }
#endif
#               ifdef _MSC_
                    if (stUsbHandle[MSC0_INTFNUM].edb_Index == bEndpointNumber)
                    {
                        MscWriteControl.bCurrentBufferXY = 0; //Set current buffer to X
                        bMcsCommandSupported = TRUE;
                    }
#               endif
            }
            usbSendZeroLengthPacketOnIEP0();
        }
    }
}

//----------------------------------------------------------------------------

VOID usbGetConfiguration(VOID)
{
    usbClearOEP0ByteCount();                    // for status stage
    wBytesRemainingOnIEP0 = 1;
    usbSendDataPacketOnEP0((PBYTE)&bConfigurationNumber);
}

//----------------------------------------------------------------------------

VOID usbGetDeviceDescriptor(VOID)
{
    usbClearOEP0ByteCount();
    wBytesRemainingOnIEP0 = SIZEOF_DEVICE_DESCRIPTOR;
    usbSendDataPacketOnEP0((PBYTE) &abromDeviceDescriptor);
}

//----------------------------------------------------------------------------

VOID usbGetConfigurationDescriptor(VOID)
{
    usbClearOEP0ByteCount();
    wBytesRemainingOnIEP0 = sizeof(abromConfigurationDescriptorGroup);
    usbSendDataPacketOnEP0((PBYTE)&abromConfigurationDescriptorGroup);
}

//----------------------------------------------------------------------------

VOID usbGetStringDescriptor(VOID)
{
    WORD bIndex;
    BYTE bVal = (BYTE)tSetupPacket.wValue;

    usbClearOEP0ByteCount();                    // for status stage
#if (USB_STR_INDEX_SERNUM != 0)

    if(bVal == 0x03)
    {
        wBytesRemainingOnIEP0 = abramSerialStringDescriptor[0];
        usbSendDataPacketOnEP0((PBYTE)&abramSerialStringDescriptor);
    }
    else
#endif
    {
        bIndex = 0x00;
        while(bVal-- >  0x00) bIndex += abromStringDescriptor[bIndex];
        wBytesRemainingOnIEP0 = abromStringDescriptor[bIndex];
        usbSendDataPacketOnEP0((PBYTE)&abromStringDescriptor[bIndex]);
    }
}

//----------------------------------------------------------------------------

VOID usbGetInterface(VOID)
{

    // not fully supported, return one byte, zero
    usbClearOEP0ByteCount();                    // for status stage
    wBytesRemainingOnIEP0 = 0x02;
    abUsbRequestReturnData[0] = 0x00;           // changed to report alternative setting byte
    abUsbRequestReturnData[1] = bInterfaceNumber;
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
}

//----------------------------------------------------------------------------

VOID usbGetDeviceStatus(VOID)
{
    if((abromConfigurationDescriptorGroup.abromConfigurationDescriptorGenric.mattributes &
        CFG_DESC_ATTR_SELF_POWERED) == CFG_DESC_ATTR_SELF_POWERED)
    {
        abUsbRequestReturnData[0] = DEVICE_STATUS_SELF_POWER;
    }
    if(bRemoteWakeup == ENABLE)
    {
        abUsbRequestReturnData[0] |= DEVICE_STATUS_REMOTE_WAKEUP;
    }
    usbClearOEP0ByteCount();                    // for status stage

    // Return self power status and remote wakeup status
    wBytesRemainingOnIEP0 = 2;
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
}

//----------------------------------------------------------------------------

VOID usbGetInterfaceStatus(VOID)
{
    // check bIndexL for index number (not supported)
    usbClearOEP0ByteCount();                    // for status stage

    // Return two zero bytes
    wBytesRemainingOnIEP0 = 2;
    abUsbRequestReturnData[0] = 0x00;           // changed to support multiple interfaces
    abUsbRequestReturnData[1] = bInterfaceNumber;
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
}

//----------------------------------------------------------------------------

VOID usbGetEndpointStatus(VOID)
{
    BYTE bEndpointNumber;

    // Endpoint number is bIndexL
    bEndpointNumber = tSetupPacket.wIndex & EP_DESC_ADDR_EP_NUM;
    if(bEndpointNumber == 0x00)
    {
        if((tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN) == EP_DESC_ADDR_DIR_IN)
        {
            // input endpoint 0
            abUsbRequestReturnData[0] = (BYTE)(tEndPoint0DescriptorBlock.bIEPCNFG & EPCNF_STALL);
        }
        else
        {
            // output endpoint 0
            abUsbRequestReturnData[0] = (BYTE)(tEndPoint0DescriptorBlock.bOEPCNFG & EPCNF_STALL);
        }
        abUsbRequestReturnData[0] = abUsbRequestReturnData[0] >> 3; // STALL is on bit 3
        usbClearOEP0ByteCount();                    // for status stage
        wBytesRemainingOnIEP0 = 0x02;
        usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
    }
    else
    {
        bEndpointNumber--;
        // EP is from EP1 to EP7 while C language start from 0
        // Firmware should NOT response if specified endpoint is not supported. (charpter 8)
        if(bEndpointNumber < MAX_ENDPOINT_NUMBER)
        {
            if(tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN)
            {
                // input endpoint
                abUsbRequestReturnData[0] = (BYTE)(tInputEndPointDescriptorBlock[bEndpointNumber].bEPCNF & EPCNF_STALL);
            }else
            {
                // output endpoint
                abUsbRequestReturnData[0] = (BYTE)(tOutputEndPointDescriptorBlock[bEndpointNumber].bEPCNF & EPCNF_STALL);
            }
        }   // no response if endpoint is not supported.
        abUsbRequestReturnData[0] = abUsbRequestReturnData[0] >> 3; // STALL is on bit 3
        usbClearOEP0ByteCount();
        wBytesRemainingOnIEP0 = 0x02;
        usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
    }
}

//----------------------------------------------------------------------------
VOID usbSetAddress(VOID)
{
    usbStallOEP0();                             // control write without data stage

    // bValueL contains device address
    if(tSetupPacket.wValue < 128)
    {
        // hardware will update the address after status stage
        // therefore, firmware can set the address now.
        USBFUNADR = tSetupPacket.wValue;
        usbSendZeroLengthPacketOnIEP0();
    }
    else
    {
        usbStallEndpoint0();
    }
}

//----------------------------------------------------------------------------

VOID usbSetConfiguration(VOID)
{
    usbStallOEP0();                             // control write without data stage

    // configuration number is in bValueL
    // change the code if more than one configuration is supported
    bConfigurationNumber = tSetupPacket.wValue;
    usbSendZeroLengthPacketOnIEP0();

    if (bConfigurationNumber == 1)
    {
        bEnumerationStatus = ENUMERATION_COMPLETE;      // set device as enumerated
    }
    else
    {
        bEnumerationStatus = 0; //device is not configured == config # is zero
    }
}

//----------------------------------------------------------------------------

VOID usbClearDeviceFeature(VOID)
{
    // bValueL contains feature selector
    if(tSetupPacket.wValue == FEATURE_REMOTE_WAKEUP)
    {
        bRemoteWakeup = DISABLE;
        usbSendZeroLengthPacketOnIEP0();
    }
    else
    {
        usbStallEndpoint0();
    }
}

//----------------------------------------------------------------------------

VOID usbSetDeviceFeature(VOID)
{
    // bValueL contains feature selector
    if(tSetupPacket.wValue == FEATURE_REMOTE_WAKEUP)
    {
        bRemoteWakeup = ENABLE;
        usbSendZeroLengthPacketOnIEP0();
    }
    else
    {
        usbStallEndpoint0();
    }
}

//----------------------------------------------------------------------------

VOID usbSetEndpointFeature(VOID)
{
    BYTE bEndpointNumber;

    // wValue contains feature selector
    // bIndexL contains endpoint number
    // Endpoint number is in low byte of wIndex
    if(tSetupPacket.wValue == FEATURE_ENDPOINT_STALL)
    {
        bEndpointNumber = tSetupPacket.wIndex & EP_DESC_ADDR_EP_NUM;
        if(bEndpointNumber == 0x00) usbSendZeroLengthPacketOnIEP0();  // do nothing for endpoint 0
        else
        {
            bEndpointNumber--;
            // Firmware should NOT response if specified endpoint is not supported. (charpter 8)
            if(bEndpointNumber < MAX_ENDPOINT_NUMBER)
            {
                if(tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN)
                {
                    // input endpoint
                    tInputEndPointDescriptorBlock[bEndpointNumber].bEPCNF |= EPCNF_STALL;
                }
                else
                {
                    // output endpoint
                    tOutputEndPointDescriptorBlock[bEndpointNumber].bEPCNF |= EPCNF_STALL;
                }
                usbSendZeroLengthPacketOnIEP0();
            } // no response if endpoint is not supported.
        }
    }
    else
    {
        usbStallEndpoint0();
    }
}

//----------------------------------------------------------------------------

VOID usbSetInterface(VOID)
{
    // bValueL contains alternative setting
    // bIndexL contains interface number
    // change code if more than one interface is supported
    usbStallOEP0();                             // control write without data stage
    bInterfaceNumber = tSetupPacket.wIndex;
#ifdef _MSC_
    tInputEndPointDescriptorBlock[stUsbHandle[MSC0_INTFNUM].edb_Index].bEPCNF &= ~(EPCNF_TOGGLE);
    tOutputEndPointDescriptorBlock[stUsbHandle[MSC0_INTFNUM].edb_Index].bEPCNF &= ~(EPCNF_TOGGLE);
    MscReadControl.bCurrentBufferXY = 0; //Set current buffer to X
    MscWriteControl.bCurrentBufferXY = 0; //Set current buffer to X
#endif
    usbSendZeroLengthPacketOnIEP0();
}

//----------------------------------------------------------------------------

VOID usbInvalidRequest(VOID)
{
    // check if setup overwrite is set
    // if set, do nothing since we might decode it wrong
    // setup packet buffer could be modified by hardware if another setup packet
    // was sent while we are deocding setup packet
    if ((USBIFG & STPOWIFG) == 0x00)
    {
      usbStallEndpoint0();
    }
}

typedef VOID (*tpF)(VOID);

BYTE usbDecodeAndProcessUsbRequest(VOID)
{
    BYTE  bMask,bResult,bTemp;
    const BYTE* pbUsbRequestList;
    BYTE bWakeUp = FALSE;
    ptDEVICE_REQUEST ptSetupPacket = &tSetupPacket;
    BYTE  bRequestType,bRequest;
    tpF lAddrOfFunction;

    // point to beginning of the matrix
    pbUsbRequestList = (PBYTE)&tUsbRequestList[0];

    while(1)
    {
        bRequestType = *pbUsbRequestList++;
        bRequest     = *pbUsbRequestList++;

        if(((bRequestType == 0xff) && (bRequest == 0xff)) ||
            (tSetupPacket.bmRequestType == (USB_REQ_TYPE_INPUT | USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_DEVICE)) ||
            (tSetupPacket.bmRequestType == (USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_DEVICE)))
        {
            pbUsbRequestList -= 2;
            break;
        }

        if((bRequestType == tSetupPacket.bmRequestType) && (bRequest == tSetupPacket.bRequest))
        {
            // compare the first two
            bResult = 0xc0;
            bMask   = 0x20;
            // first two bytes matched, compare the rest
            for(bTemp = 2; bTemp < 8; bTemp++)
            {
                if (*((BYTE*)ptSetupPacket + bTemp) == *pbUsbRequestList)
                {
                    bResult |= bMask;
                }
                pbUsbRequestList++;
                bMask = bMask >> 1;
            }
            // now we have the result
            if((*pbUsbRequestList & bResult) == *pbUsbRequestList)
            {
                pbUsbRequestList -= 8;
                break;
            }
            else
            {
                pbUsbRequestList += (sizeof(tDEVICE_REQUEST_COMPARE)-8);
            }
        }
        else
        {
            pbUsbRequestList += (sizeof(tDEVICE_REQUEST_COMPARE)-2);
        }
    }

    // if another setup packet comes before we have the chance to process current
    // setup request, we return here without processing the request
    // this check is not necessary but still kept here to reduce response(or simulation) time

    if((USBIFG & STPOWIFG) != 0x00)
    {
        return bWakeUp;
    }

    // now we found the match and jump to the function accordingly.
    lAddrOfFunction = ((tDEVICE_REQUEST_COMPARE*)pbUsbRequestList)->pUsbFunction;

    // call function
    (*lAddrOfFunction)();

    // perform enumeration complete event:
    // when SetAddress was called and USBADDR is not zero
    if ((lAddrOfFunction == &usbSetAddress) && (USBFUNADR != 0))
    {
        bWakeUp = USB_handleEnumCompleteEvent();
    }
    return bWakeUp;
}

/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
