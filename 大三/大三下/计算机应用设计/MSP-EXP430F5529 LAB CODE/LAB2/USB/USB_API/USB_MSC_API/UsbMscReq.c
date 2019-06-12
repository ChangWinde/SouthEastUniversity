// (c)2010 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
  |                                                                             |
  |                              Texas Instruments                              |
  |                                                                             |
  |                          MSP430 USB-Example (MSC  Driver)                   |
  |                                                                             |
  +-----------------------------------------------------------------------------+
  |  Source: Usb_Msc_Req.c, File Version 1.02                                   |
  |  Description: This file contains the APIs specific to MSC class             |
  |               (Class specific requests)                                     |
  |  Author: Biju,MSP                                                           |
  |                                                                             |
  |  WHO          WHEN         WHAT                                             |
  |  ---          ----------   ------------------------------------------------ |
  |  MSP          2010/02/01   born                                             |
  |  Biju,MSP     2010/07/15   CV bug fix                                       |                                                                             |
  +----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
  | Includes                                                                    |
  +----------------------------------------------------------------------------*/

#include <descriptors.h>

#ifdef _MSC_

#include "../USB_Common/types.h"      // Basic Type declarations
#include "../USB_Common/device.h"
#include "../USB_Common/defMSP430USB.h"
#include "../USB_Common/usb.h"        // USB-specific Data Structures
#include "../USB_MSC_API/UsbMscScsi.h"
#include "../USB_MSC_API/UsbMscReq.h"
#include "../USB_MSC_API/UsbMsc.h"

extern __no_init tEDB __data16 tInputEndPointDescriptorBlock[];
extern __no_init tEDB __data16 tOutputEndPointDescriptorBlock[];
BOOL isMSCConfigured = FALSE;
extern BYTE bMscResetRequired;

/*----------------------------------------------------------------------------+
  | Functions                                                                   |
  +----------------------------------------------------------------------------*/
VOID USBMSC_reset(VOID)
{
    Msc_ResetStateMachine();
    Msc_ResetFlags();
    Msc_ResetStruct();
    isMSCConfigured = TRUE;

    bMscResetRequired = FALSE;
    tInputEndPointDescriptorBlock[stUsbHandle[MSC0_INTFNUM].edb_Index].bEPCNF
      &= ~(EPCNF_STALL | EPCNF_TOGGLE );
    tOutputEndPointDescriptorBlock[stUsbHandle[MSC0_INTFNUM].edb_Index].bEPCNF
      &= ~(EPCNF_STALL  | EPCNF_TOGGLE );
    usbSendZeroLengthPacketOnIEP0();            // status stage for control transfer
}

//----------------------------------------------------------------------------
VOID Get_MaxLUN(VOID)
{
    BYTE maxLunNumber = MSC_MAX_LUN_NUMBER - 1;
    wBytesRemainingOnIEP0 = 1;
    isMSCConfigured = TRUE;
    usbSendDataPacketOnEP0((PBYTE)&maxLunNumber);
}

#endif // _MSC_
/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
