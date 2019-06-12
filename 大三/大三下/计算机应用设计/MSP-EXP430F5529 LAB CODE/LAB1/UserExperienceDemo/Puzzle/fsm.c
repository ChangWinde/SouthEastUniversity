//****************************************************************************//
//  MSP430 state machine
//  fsm.c
//
//  Describtion:
//    A simple state machine for the MSP430
//    Do not change code in here!!!
//
//  Generated with Excel Table
//  Date:  08/26/2008        Time:  15:00:56
//
//****************************************************************************//

#include <stdint.h>
#include <stdio.h>

#include "fsm.h"

//****************************************************************************//
// Global variables
//****************************************************************************//
uint8_t ActState;

const FSM_STATE_TABLE StateTable[NR_STATES][NR_EVENTS] =
{
    NULL, INIT_GAME, NULL, INIT_GAME, NULL, INIT_GAME, NULL, INIT_GAME, FSM_StartGame, EMPTY_FIELD9,
    FSM_StopGame, INIT_GAME,
    FSM_PushLeft, EMPTY_FIELD2, NULL, EMPTY_FIELD1, FSM_PushUp, EMPTY_FIELD4, NULL, EMPTY_FIELD1,
    FSM_InitGame, INIT_GAME, NULL, EMPTY_FIELD1,
    FSM_PushLeft, EMPTY_FIELD3, FSM_PushRight, EMPTY_FIELD1, FSM_PushUp, EMPTY_FIELD5, NULL,
    EMPTY_FIELD2, FSM_InitGame, INIT_GAME, NULL, EMPTY_FIELD2,
    NULL, EMPTY_FIELD3, FSM_PushRight, EMPTY_FIELD2, FSM_PushUp, EMPTY_FIELD6, NULL, EMPTY_FIELD3,
    FSM_InitGame, INIT_GAME, NULL, EMPTY_FIELD3,
    FSM_PushLeft, EMPTY_FIELD5, NULL, EMPTY_FIELD4, FSM_PushUp, EMPTY_FIELD7, FSM_PushDown,
    EMPTY_FIELD1, FSM_InitGame, INIT_GAME, NULL, EMPTY_FIELD4,
    FSM_PushLeft, EMPTY_FIELD6, FSM_PushRight, EMPTY_FIELD4, FSM_PushUp, EMPTY_FIELD8, FSM_PushDown,
    EMPTY_FIELD2, FSM_InitGame, INIT_GAME, NULL, EMPTY_FIELD5,
    NULL, EMPTY_FIELD6, FSM_PushRight, EMPTY_FIELD5, FSM_PushUp, EMPTY_FIELD9, FSM_PushDown,
    EMPTY_FIELD3, FSM_InitGame, INIT_GAME, NULL, EMPTY_FIELD6,
    FSM_PushLeft, EMPTY_FIELD8, NULL, EMPTY_FIELD7, NULL, EMPTY_FIELD7, FSM_PushDown, EMPTY_FIELD4,
    FSM_InitGame, INIT_GAME, NULL, EMPTY_FIELD7,
    FSM_PushLeft, EMPTY_FIELD9, FSM_PushRight, EMPTY_FIELD7, NULL, EMPTY_FIELD8, FSM_PushDown,
    EMPTY_FIELD5, FSM_InitGame, INIT_GAME, NULL, EMPTY_FIELD8,
    NULL, EMPTY_FIELD9, FSM_PushRight, EMPTY_FIELD8, NULL, EMPTY_FIELD9, FSM_PushDown, EMPTY_FIELD6,
    FSM_InitGame, INIT_GAME, FSM_StopGame, STOP_GAME,
    NULL, STOP_GAME, NULL, STOP_GAME, NULL, STOP_GAME, NULL, STOP_GAME, FSM_InitGame, INIT_GAME,
    NULL, STOP_GAME
};


//****************************************************************************//
// Initialize state machine
//****************************************************************************//
void FSM_Init(void)
{
    ActState = INIT_GAME;
}

//****************************************************************************//
// Event function "Left"
//****************************************************************************//
void FSM_Left(void)
{
    if (StateTable[ActState][LEFT].ptrFunct != NULL)
        StateTable[ActState][LEFT].ptrFunct();

    ActState = StateTable[ActState][LEFT].NextState;
}

//****************************************************************************//
// Event function "Right"
//****************************************************************************//
void FSM_Right(void)
{
    if (StateTable[ActState][RIGHT].ptrFunct != NULL)
        StateTable[ActState][RIGHT].ptrFunct();

    ActState = StateTable[ActState][RIGHT].NextState;
}

//****************************************************************************//
// Event function "Up"
//****************************************************************************//
void FSM_Up(void)
{
    if (StateTable[ActState][UP].ptrFunct != NULL)
        StateTable[ActState][UP].ptrFunct();

    ActState = StateTable[ActState][UP].NextState;
}

//****************************************************************************//
// Event function "Down"
//****************************************************************************//
void FSM_Down(void)
{
    if (StateTable[ActState][DOWN].ptrFunct != NULL)
        StateTable[ActState][DOWN].ptrFunct();

    ActState = StateTable[ActState][DOWN].NextState;
}

//****************************************************************************//
// Event function "Button"
//****************************************************************************//
void FSM_Button(void)
{
    if (StateTable[ActState][BUTTON].ptrFunct != NULL)
        StateTable[ActState][BUTTON].ptrFunct();

    ActState = StateTable[ActState][BUTTON].NextState;
}

//****************************************************************************//
// Event function "Win"
//****************************************************************************//
void FSM_Win(void)
{
    if (StateTable[ActState][WIN].ptrFunct != NULL)
        StateTable[ActState][WIN].ptrFunct();

    ActState = StateTable[ActState][WIN].NextState;
}

//****************************************************************************//
