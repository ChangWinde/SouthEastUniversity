//****************************************************************************//
//  Inclination Game
//  Puzzle.h
//
//  Describtion:
//    Simple example to demonstrate the functionality of the Excel
//    state machine generation tool
//
//  Target MCU:   MSP430F5529
//
//  Implemented with Application report: SLAA402
//
//  Texas Instruments Freising
//  August 2009
//****************************************************************************//

#ifndef PUZZLE_H
#define PUZZLE_H

//****************************************************************************//
// Function prototypes
//****************************************************************************//

//====================================================================

/**
 * initialize the timer for a periodic interrupt every 16 ms
 *
 */
void TimerA1Init(void);

//====================================================================

/**
 * initialize the game
 *
 */
void StartPuzzle(void);

//====================================================================

/**
 * Stop the TimerA1 to save energy
 *
 */
void TimerA1Stop(void);

//****************************************************************************//

#endif /* PUZZLE_H */

//****************************************************************************//
