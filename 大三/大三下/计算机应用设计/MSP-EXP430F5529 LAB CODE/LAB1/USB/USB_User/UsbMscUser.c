// (c)2010 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
  |                                                                             |
  |                              Texas Instruments                              |
  |                                                                             |
  |                          MSP430 USB-Example (MSC Driver)                    |
  |                                                                             |
  +-----------------------------------------------------------------------------+
  |  Source: Msc_user.c, File Version 1.00                                      |
  |  Description: This file contains the structures,APIs to be populated by user|
  |               User should populate based on his media. Also user can define |
  |               his media read/write APIs here.                               |
  |  Author: MSP                                                                |
  |                                                                             |
  |  WHO          WHEN         WHAT                                             |
  |  ---          ----------   ------------------------------------------------ |
  |  MSP, Biju    2010/06/16   Created                                          |
  +----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
  | Includes                                                                    |
  +----------------------------------------------------------------------------*/

#include "../USB_Common/types.h"
#include "../USB_config/descriptors.h"

#ifdef _MSC_

#include "UsbMscUser.h"
#include "../USB_MSC_API/UsbMscScsi.h"
//#include "../USB_MSC_Application/UsbMscUserData.h"
#include <string.h>

/*MSC Config structure having some info on the media */
/*Update this based on your media */
struct config_struct USBMSC_config = {
    {
        {
        0x00,
        0x00,
        0x80,
        "TI Mass",
        "Storage Lun0",
        "0x00",
        },
    }
};



#endif // _MSC_

/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
