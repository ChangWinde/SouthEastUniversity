FatFs Module Sample Projects                             (C)ChaN, 2011


  This archive contains sample projects for function/compatibility test
  of FatFs module with platform dependent low level disk I/O modules.


DIRECTORIES

  Directory  Platform
  <generic>  Generic microcontrollers
  <avr>      ATMEL AVR (ATmega64, 8-bit)
  <pic24>    Microchip PIC24 (PIC24FJ64GA002, 16-bit)
  <lpc2k>    NXP LPC2000 (LPC2388, 32-bit)
  <v850>     NEC V850ES (UPD70F3716, 32-bit)
  <h8>       Renesas H8/300H (HD64F3694, 16-bit)
  <sh7262>   Renesas SH-2A (R5S72620W14FPU, 32-bit)
  <rx62n>    Renesas RX62N (R5F562N8BDFB, 32-bit)
  <win32>    Windows (Visual C++), ANSI/Unicode

  Each sample project contains support of following media.

            MMC/SDC  MMC/SDC    CFC      ATA     NAND
             (SPI)   (native)  (8bit)   (16bit)
  Generic     o
  AVR         o                  o        o
  PIC24       o
  LPC2000               o                         o
  V850ES      o
  H8/300H     o
  SH7262      o
  RX62N       o

  The disk I/O modules will able to be used as reference design for any other
  file system module as well. You will able to find various implementations
  on the web other than these samples, such as STM32, MSP430, PIC32, Nios 2,
  68HC11, and Z8, at least, so far as I know.



AGREEMENTS

  These sample projects for FatFs module are free software and there is no
  warranty. You can use, modify and redistribute it for personal, non-profit
  or commercial product without any restriction under your responsibility.



REVISION HISTORY

  Apr 29, 2006  First release.
  Aug 19, 2006  MMC module: Fixed a bug that disk_initialize() never time-out
                when card does not go ready.
  Oct 12, 2006  CF module: Fixed a bug that disk_initialize() can fail at 3.3V.
  Oct 22, 2006  Added a sample project for V850ES.
  Feb 04, 2007  All modules: Modified for FatFs module R0.04.
                MMC module: Fixed a bug that disk_ioctl() returns incorrect disk size.
  Apr 03, 2007  All modules: Modified for FatFs module R0.04a.
                MMC module: Supported high capacity SD memory cards.
  May 05, 2007  MMC modules: Fixed a bug that GET_SECTOR_COUNT via disk_ioctl() fails on MMC.
  Aug 26, 2007  Added some ioctl sub-functions.
  Oct 13, 2007  MMC modules: Fixed send_cmd() sends incorrect command packet.
  Dec 12, 2007  Added a sample project for Microchip PIC.
  Feb 03, 2008  All modules: Modified for FatFs module R0.05a.
  Apr 01, 2008  Modified main() for FatFs module R0.06.
  Oct 18, 2008  Added a sample project for NXP LPC2000.
  Apr 01, 2009  Modified for FatFs module R0.07.
  Apr 18, 2009  Modified for FatFs module R0.07a.
  Jun 25, 2009  Modified for FatFs module R0.07c.
  Jul 13, 2009  Added sample project for Unicoede API.
  May 15, 2010  Modified for FatFs module R0.08.
                Added a sample project for Renesas SH2A.
  Aug 16, 2010  Modified for FatFs module R0.08a.
                LPC2000: Added NAND flash driver.
  Oct 14, 2010  Added a sample project for generic uC.
  Nov 25, 2010  AVR: Fixed socket controls of MMC drivers. Added bitbanging MMC driver.
  Jan 15, 2011  Modified for FatFs module R0.08b.
  Feb 06, 2011  Added a sample project for Renesas RX62N.
