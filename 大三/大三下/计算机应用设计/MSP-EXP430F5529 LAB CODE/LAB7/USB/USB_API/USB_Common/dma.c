// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (HID/CDC Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: dma.c, File Version 1.02 2009/12/03                                |
|  Author: RSTO                                                               |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  RSTO         2009/03/03   born                                             |
|  RSTO         2009/04/08   Redefine memcpy()                                |
|  RSTO         2009/04/16   use 16 bit access to DMA regs                    |
|  RSTO         2009/09/18   fixed trigger selection for DMA with bit set     |
|  RSTO         2009/11/03   do not transfer via DMA if length is zero        |
|  MSP,Biju     2009/12/03   Review comments addressed, file versioning       |
|                            started                                          |
|  RSTO         2010/01/08   added support for large mem model                |
+----------------------------------------------------------------------------*/

#include "../USB_Common/device.h"
#include "../USB_Common/types.h"              // Basic Type declarations
#include "../USB_Common/defMSP430USB.h"
#include <descriptors.h>
#include <string.h>

#ifdef __REGISTER_MODEL__
/* for IAR */
#	if __REGISTER_MODEL__ == __REGISTER_MODEL_REG20__
#		define __DMA_ACCESS_REG__ (void __data20 *)
#	else
#		define __DMA_ACCESS_REG__ (unsigned short)
#	endif
#else
/* for CCS */
#	define __DMA_ACCESS_REG__ (__SFR_FARPTR)(unsigned long)
#endif

//function pointers
VOID *(*USB_TX_memcpy)(VOID * dest, const VOID * source, size_t count);
VOID *(*USB_RX_memcpy)(VOID * dest, const VOID * source, size_t count);

VOID * memcpyDMA0(VOID * dest, const VOID * source, size_t count);
VOID * memcpyDMA1(VOID * dest, const VOID * source, size_t count);
VOID * memcpyDMA2(VOID * dest, const VOID * source, size_t count);

// NOTE: this functin works only with data in the area <64k (small memory model)
VOID * memcpyV(VOID * dest, const VOID * source, size_t count)
{
    WORD i;
    volatile BYTE bTmp;
    for (i=0; i<count; i++)
    {
        bTmp = *((BYTE*)source +i);
        *((BYTE*)dest  +i) = bTmp;
    }
    return dest;
}

//this function inits the DMA
VOID USB_initMemcpy(VOID)
{
    USB_TX_memcpy = memcpyV;
    USB_RX_memcpy = memcpyV;

    switch (USB_DMA_CHAN)
    {
    case 0:
        DMACTL0 &= ~DMA0TSEL_31;         // DMA0 is triggered by DMAREQ
        DMACTL0 |= DMA0TSEL_0;           // DMA0 is triggered by DMAREQ
        DMA0CTL = (DMADT_1 + DMASBDB + DMASRCINCR_3 +   // configure block transfer (byte-wise) with increasing source
                       DMADSTINCR_3 );                  // and destination address
        DMACTL4 |= ENNMI;               // enable NMI interrupt
        USB_TX_memcpy = memcpyDMA0;
        USB_RX_memcpy = memcpyDMA0;
        break;
    case 1:
        DMACTL0 &= ~DMA1TSEL_31;         // DMA1 is triggered by DMAREQ
        DMACTL0 |= DMA1TSEL_0;           // DMA1 is triggered by DMAREQ
        DMA1CTL = (DMADT_1 + DMASBDB + DMASRCINCR_3 +   // configure block transfer (byte-wise) with increasing source
                       DMADSTINCR_3 );                  // and destination address
        DMACTL4 |= ENNMI;               // enable NMI interrupt
        USB_TX_memcpy = memcpyDMA1;
        USB_RX_memcpy = memcpyDMA1;
        break;
    case 2:
       DMACTL0 &= ~DMA2TSEL_31;         // DMA2 is triggered by DMAREQ
       DMACTL0 |= DMA2TSEL_0;           // DMA2 is triggered by DMAREQ
       DMA2CTL = (DMADT_1 + DMASBDB + DMASRCINCR_3 +   // configure block transfer (byte-wise) with increasing source
                       DMADSTINCR_3 );                 // and destination address
       DMACTL4 |= ENNMI;               // enable NMI interrupt
       USB_TX_memcpy = memcpyDMA2;
       USB_RX_memcpy = memcpyDMA2;
       break;
    }
}

// this functions starts DMA transfer to/from USB memory into/from RAM
// Using DMA0
// Support only for data in <64k memory area.
VOID * memcpyDMA0(VOID * dest, const VOID *  source, size_t count)
{
    if (count == 0)         // do nothing if zero bytes to transfer
    {
        return dest;
    }

    DMA0DA = __DMA_ACCESS_REG__ dest;   // set destination for DMAx
    DMA0SA = __DMA_ACCESS_REG__ source; // set source for DMAx
    DMA0SZ = count;         // how many bytes to transfer

    DMA0CTL |= DMAEN;       // enable DMAx
    DMA0CTL |= DMAREQ;      // trigger DMAx

    //wait for DMA transfer finished
    while (!(DMA0CTL & DMAIFG));

    DMA0CTL &= ~DMAEN;      // disable DMAx
    return dest;
}

// this functions starts DMA transfer to/from USB memory into/from RAM
// Using DMA1
// Support only for data in <64k memory area.
VOID * memcpyDMA1(VOID * dest, const VOID * source, size_t count)
{
    if (count == 0)         // do nothing if zero bytes to transfer
    {
        return dest;
    }

    DMA1DA = __DMA_ACCESS_REG__ dest;   // set destination for DMAx
    DMA1SA = __DMA_ACCESS_REG__ source; // set source for DMAx
    DMA1SZ = count;         // how many bytes to transfer

    DMA1CTL |= DMAEN;       // enable DMAx
    DMA1CTL |= DMAREQ;      // trigger DMAx

    //wait for DMA transfer finished
    while (!(DMA1CTL & DMAIFG));

    DMA1CTL &= ~DMAEN;      // disable DMAx
    return dest;
}

// this functions starts DMA transfer to/from USB memory into/from RAM
// Using DMA2
// Support only for data in <64k memory area.
VOID * memcpyDMA2(VOID * dest, const VOID * source, size_t count)
{
    if (count == 0)         // do nothing if zero bytes to transfer
    {
        return dest;
    }

    DMA2DA = __DMA_ACCESS_REG__ dest;   // set destination for DMAx
    DMA2SA = __DMA_ACCESS_REG__ source; // set source for DMAx
    DMA2SZ = count;         // how many bytes to transfer

    DMA2CTL |= DMAEN;       // enable DMAx
    DMA2CTL |= DMAREQ;      // trigger DMAx

    //wait for DMA transfer finished
    while (!(DMA2CTL & DMAIFG));

    DMA2CTL &= ~DMAEN;      // disable DMAx
    return dest;
}





/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
