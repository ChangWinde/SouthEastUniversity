/*------------------------------------------------------------------------/
/  Bitbanging MMCv3/SDv1/SDv2 (in SPI mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2010, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
---------------------------------------------------------------------------*/

#include <intrinsics.h>         /* Include MSP430-specific intrincs */
#include "diskio.h"             /* Common include file for FatFs and disk I/O layer */
#include "HAL_SDCard.h"         /* MSP-EXP430F5529 specific SD Card driver */

/*-------------------------------------------------------------------------*/
/* Platform dependent macros and functions needed to be modified           */
/*-------------------------------------------------------------------------*/

// CPU Frequency.
#define MCLK_FREQ   25000000
#define	INIT_PORT()     SDCard_init()       /* Initialize MMC control port */
#define FAST_MODE()     SDCard_fastMode()   /* Maximize SD Card transfer speed */
#define DLY_US(n)       __delay_cycles(n * (MCLK_FREQ/1000000))  // Delay n microseconds           // KLQ

#define	CS_H()          SDCard_setCSHigh()  /* Set MMC CS "high" */
#define CS_L()          SDCard_setCSLow()   /* Set MMC CS "low" */

BYTE INS = 1;    // KLQ
#define	WP              (0)                 /* Card is write protected (yes:true, no:false, default:false) */

/*-------------------------------------------------------------------------*/
/* Platform dependent RTC Function for FatFs module                        */
/*-------------------------------------------------------------------------*/
DWORD get_fattime(void)
{
	DWORD tmr;

    // TODO: Customize to use the MSP430 RTC

	/* Pack date and time into a DWORD variable */
	tmr =	  (((DWORD)2001 - 80) << 25)    // rtcYear
			| ((DWORD)9 << 21)              // rtcMon
			| ((DWORD)11 << 16)             // rtcMday
			| (WORD)(4 << 11)               // rtcHour
			| (WORD)(30 << 5)               // rtcMin
			| (WORD)(0 >> 1);               // rtcSec

	return tmr;
}

/*--------------------------------------------------------------------------
   Module Private Functions
---------------------------------------------------------------------------*/

/* MMC/SD command (SPI mode) */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD41	(41)		/* SEND_OP_COND (ACMD) */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

/* Card type flags (CardType) */
#define CT_MMC		0x01		/* MMC ver 3 */
#define CT_SD1		0x02		/* SD ver 1 */
#define CT_SD2		0x04		/* SD ver 2 */
#define CT_SDC		(CT_SD1|CT_SD2)	/* SD */
#define CT_BLOCK	0x08		/* Block addressing */


static
DSTATUS Stat = STA_NOINIT;	/* Disk status */

static
BYTE CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */



/*-----------------------------------------------------------------------*/
/* Transmit bytes to the MMC                                             */
/*-----------------------------------------------------------------------*/

static
void xmit_mmc (
	const BYTE* buff,               /* Data to be sent */
	UINT bc                         /* Number of bytes to send */
)
{
    SDCard_sendFrame((uint8_t *)buff, bc);
}

/*-----------------------------------------------------------------------*/
/* Receive bytes from the MMC                                            */
/*-----------------------------------------------------------------------*/

static
void rcvr_mmc (
	BYTE *buff,	/* Pointer to read buffer */
	UINT bc		/* Number of bytes to receive */
)
{

    SDCard_readFrame(buff, bc);
}


/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
int wait_ready (void)    /* 1:OK, 0:Timeout */
{
    BYTE d;
    UINT tmr;


    for (tmr = 5000; tmr; tmr--) {    /* Wait for ready in timeout of 500ms */
        rcvr_mmc(&d, 1);
        if (d == 0xFF) return 1;
        DLY_US(100);
    }

    return 0;
}



/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void deselect (void)
{
    BYTE d;

    CS_H();
    rcvr_mmc(&d, 1);
}



/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready                                    */
/*-----------------------------------------------------------------------*/

static
int select (void)    /* 1:OK, 0:Timeout */
{
    CS_L();
    if (!wait_ready()) {
        deselect();
        return 0;
    }
    return 1;
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
int rcvr_datablock (    /* 1:OK, 0:Failed */
    BYTE *buff,            /* Data buffer to store received data */
    UINT btr            /* Byte count */
)
{
    BYTE d[2];
    UINT tmr;


    for (tmr = 1000; tmr; tmr--) {    /* Wait for data packet in timeout of 100ms */
        rcvr_mmc(d, 1);
        if (d[0] != 0xFF) break;
        DLY_US(100);
    }
    if (d[0] != 0xFE) return 0;        /* If not valid data token, retutn with error */

    rcvr_mmc(buff, btr);            /* Receive the data block into buffer */
    rcvr_mmc(d, 2);                    /* Discard CRC */

    return 1;                        /* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

static
int xmit_datablock (    /* 1:OK, 0:Failed */
    const BYTE *buff,    /* 512 byte data block to be transmitted */
    BYTE token            /* Data/Stop token */
)
{
    BYTE d[2];


    if (!wait_ready()) return 0;

    d[0] = token;
    xmit_mmc(d, 1);                /* Xmit a token */
    if (token != 0xFD) {        /* Is it data token? */
        xmit_mmc(buff, 512);    /* Xmit the 512 byte data block to MMC */
        rcvr_mmc(d, 2);            /* Dummy CRC (FF,FF) */
        rcvr_mmc(d, 1);            /* Receive data response */
        if ((d[0] & 0x1F) != 0x05)    /* If not accepted, return with error */
            return 0;
    }

    return 1;
}



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (        /* Returns command response (bit7==1:Send failed)*/
    BYTE cmd,        /* Command byte */
    DWORD arg        /* Argument */
)
{
    BYTE n, d, buf[6];


    if (cmd & 0x80) {    /* ACMD<n> is the command sequense of CMD55-CMD<n> */
        cmd &= 0x7F;
        n = send_cmd(CMD55, 0);
        if (n > 1) return n;
    }

    /* Select the card and wait for ready */
    deselect();
    if (!select()) return 0xFF;

    /* Send a command packet */
    buf[0] = 0x40 | cmd;            /* Start + Command index */
    buf[1] = (BYTE)(arg >> 24);        /* Argument[31..24] */
    buf[2] = (BYTE)(arg >> 16);        /* Argument[23..16] */
    buf[3] = (BYTE)(arg >> 8);        /* Argument[15..8] */
    buf[4] = (BYTE)arg;                /* Argument[7..0] */
    n = 0x01;                        /* Dummy CRC + Stop */
    if (cmd == CMD0) n = 0x95;        /* (valid CRC for CMD0(0)) */
    if (cmd == CMD8) n = 0x87;        /* (valid CRC for CMD8(0x1AA)) */
    buf[5] = n;
    xmit_mmc(buf, 6);

    /* Receive command response */
    if (cmd == CMD12) rcvr_mmc(&d, 1);    /* Skip a stuff byte when stop reading */
    n = 10;                                /* Wait for a valid response in timeout of 10 attempts */
    do
        rcvr_mmc(&d, 1);
    while ((d & 0x80) && --n);

    return d;            /* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE drv            /* Drive number (0) */
)
{
    DSTATUS s = Stat;


    if (drv || !INS) {
        s = STA_NODISK | STA_NOINIT;
    } else {
        s &= ~STA_NODISK;
        if (WP)
            s |= STA_PROTECT;
        else
            s &= ~STA_PROTECT;
    }
    Stat = s;

    return s;
}



/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
/* TI: Inserted pragma to supress IAR compiler warning incicating 'cmd'
 * is not used. If removed however the compile fails */
#ifdef __IAR_SYSTEMS_ICC__
#pragma diag_suppress=Pe550
#endif
#ifdef __TI_COMPILER_VERSION__
#pragma diag_suppress 552
#endif
	BYTE n, ty, cmd, buf[4];
#ifdef __IAR_SYSTEMS_ICC__
#pragma diag_default=Pe550
#endif
//#ifdef __TI_COMPILER_VERSION__
//#pragma diag_default 552
//#endif
    UINT tmr;
    DSTATUS s;


    INIT_PORT();                /* Initialize control port */

    DLY_US(100);

    s = disk_status(drv);        /* Check if card is in the socket */
    if (s & STA_NODISK) return s;

    CS_H();
    for (n = 10; n; n--) rcvr_mmc(buf, 1);    /* 80 dummy clocks */

    ty = 0;
    if (send_cmd(CMD0, 0) == 1) {            /* Enter Idle state */
        if (send_cmd(CMD8, 0x1AA) == 1) {    /* SDv2? */
            rcvr_mmc(buf, 4);                            /* Get trailing return value of R7 resp */
            if (buf[2] == 0x01 && buf[3] == 0xAA) {        /* The card can work at vdd range of 2.7-3.6V */
                for (tmr = 1000; tmr; tmr--) {            /* Wait for leaving idle state (ACMD41 with HCS bit) */
                    if (send_cmd(ACMD41, 1UL << 30) == 0) break;
                    DLY_US(1000);
                }
                if (tmr && send_cmd(CMD58, 0) == 0) {    /* Check CCS bit in the OCR */
                    rcvr_mmc(buf, 4);
                    ty = (buf[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;    /* SDv2 */
                }
            }
        } else {                            /* SDv1 or MMCv3 */
            if (send_cmd(ACMD41, 0) <= 1)     {
                ty = CT_SD1; cmd = ACMD41;    /* SDv1 */
            } else {
                ty = CT_MMC; cmd = CMD1;    /* MMCv3 */
            }
            for (tmr = 1000; tmr; tmr--) {            /* Wait for leaving idle state */
                if (send_cmd(ACMD41, 0) == 0) break;
                DLY_US(1000);
            }
            if (!tmr || send_cmd(CMD16, 512) != 0)    /* Set R/W block length to 512 */
                ty = 0;
        }
    }
    CardType = ty;
    deselect();

    if (ty) {      /* Initialization succeded */
        FAST_MODE();
        s &= ~STA_NOINIT;
    }
    else {       /* Initialization failed */
        s |= STA_NOINIT;
    }
    Stat = s;


    return s;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE drv,            /* Physical drive nmuber (0) */
    BYTE *buff,            /* Pointer to the data buffer to store read data */
    DWORD sector,        /* Start sector number (LBA) */
    BYTE count            /* Sector count (1..128) */
)
{
    DSTATUS s;


    s = disk_status(drv);
    if (s & STA_NOINIT) return RES_NOTRDY;
    if (!count) return RES_PARERR;
    if (!(CardType & CT_BLOCK)) sector *= 512;    /* Convert LBA to byte address if needed */

    if (count == 1) {    /* Single block read */
        if ((send_cmd(CMD17, sector) == 0)    /* READ_SINGLE_BLOCK */
            && rcvr_datablock(buff, 512))
            count = 0;
    }
    else {                /* Multiple block read */
        if (send_cmd(CMD18, sector) == 0) {    /* READ_MULTIPLE_BLOCK */
            do {
                if (!rcvr_datablock(buff, 512)) break;
                buff += 512;
            } while (--count);
            send_cmd(CMD12, 0);                /* STOP_TRANSMISSION */
        }
    }
    deselect();

    return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
    BYTE drv,            /* Physical drive nmuber (0) */
    const BYTE *buff,    /* Pointer to the data to be written */
    DWORD sector,        /* Start sector number (LBA) */
    BYTE count            /* Sector count (1..128) */
)
{
    DSTATUS s;


    s = disk_status(drv);
    if (s & STA_NOINIT) return RES_NOTRDY;
    if (s & STA_PROTECT) return RES_WRPRT;
    if (!count) return RES_PARERR;
    if (!(CardType & CT_BLOCK)) sector *= 512;    /* Convert LBA to byte address if needed */

    if (count == 1) {    /* Single block write */
        if ((send_cmd(CMD24, sector) == 0)    /* WRITE_BLOCK */
            && xmit_datablock(buff, 0xFE))
            count = 0;
    }
    else {                /* Multiple block write */
        if (CardType & CT_SDC) send_cmd(ACMD23, count);
        if (send_cmd(CMD25, sector) == 0) {    /* WRITE_MULTIPLE_BLOCK */
            do {
                if (!xmit_datablock(buff, 0xFC)) break;
                buff += 512;
            } while (--count);
            if (!xmit_datablock(0, 0xFD))    /* STOP_TRAN token */
                count = 1;
        }
    }
    deselect();

    return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE drv,        /* Physical drive nmuber (0) */
    BYTE ctrl,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    DRESULT res;
    BYTE n, csd[16];
    WORD cs;


    if (disk_status(drv) & STA_NOINIT)                    /* Check if card is in the socket */
        return RES_NOTRDY;

    res = RES_ERROR;
    switch (ctrl) {
        case CTRL_SYNC :        /* Make sure that no pending write process */
            if (select()) {
                deselect();
                res = RES_OK;
            }
            break;

        case GET_SECTOR_COUNT :    /* Get number of sectors on the disk (DWORD) */
            if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
                if ((csd[0] >> 6) == 1) {    /* SDC ver 2.00 */
                    cs= csd[9] + ((WORD)csd[8] << 8) + 1;
                    *(DWORD*)buff = (DWORD)cs << 10;
                } else {                    /* SDC ver 1.XX or MMC */
                    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                    cs = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                    *(DWORD*)buff = (DWORD)cs << (n - 9);
                }
                res = RES_OK;
            }
            break;

        case GET_BLOCK_SIZE :    /* Get erase block size in unit of sector (DWORD) */
            *(DWORD*)buff = 128;
            res = RES_OK;
            break;

        default:
            res = RES_PARERR;
    }

    deselect();

    return res;
}



// KLQ

uint8_t validateCSD(void)
{
  BYTE csd0[16], csd1[16], i;
  WORD sum=0;

  // Pull the CSD -- twice.  If the response codes are invalid, then we know the card isn't there or initialized.
  if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd0, 16))
    if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd1, 16))
    {
      // The response codes were good -- but maybe the SPI input was just floating low.  Let's evaluate the CSD data.
      // First, look for all zero or all ones.  If the SPI input is floating, these are the most likely outcomes.
      for(i=0;i<=15;i++)
        sum += csd0[i];
      if(!((sum == 0) || (sum == 4096)))
      {
        // The response was a mix of 0's and 1's.  Floating inputs could still do that -- but it's unlikely they'd
        // produce the same pattern twice.  Compare to ensure the two are identical.
        i = 0;
        while(i<=15)
        {
          if(csd0[i] != csd1[i])
            break;
          i++;
        }
        if(i>15)
          return 1;
      }
    }
  return 0;
}


// Attempt to detect the card by commanding it to return its CSD register and evaluating it.  Returns the
// result, and also updates FatFs's internal INS variable.
// The proper way to detect a card is by sensing its presence on the DAT3 signal.  The EXP board doesn't
// contain the necessary h/w, so this s/w method works instead.
uint8_t detectCard(void)
{
  // Check for a valid CSD response
  if(validateCSD())
  {
    disk_status(0); // Update the INS variable
    return 1;       // Card is present
  }

  // We didn't get a valid response.  So we now know the status is one of two things:
  // a) The card isn't there at all;
  // b) or, it was just inserted recently, and needs to be initialized

  INS = 0x01;          // Trick disk_initialize into thinking it's inserted...
  disk_initialize(0);  // Attempt to initialize it

  INS = validateCSD(); // Try again
  disk_status(0);      // Update the INS variable

  return INS;          // 1 = card is present; 0 = not present
}
