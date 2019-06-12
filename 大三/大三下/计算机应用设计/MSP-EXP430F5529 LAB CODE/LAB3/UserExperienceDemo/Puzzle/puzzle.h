/*
 * puzzle.h
 *
 *  Created on: 2012-7-23
 *      Author: Administrator
 */

#ifndef PUZZLE_H_
#define PUZZLE_H_
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



#endif /* PUZZLE_H_ */
