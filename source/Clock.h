/*******************************************************************************
* Clock.h
*
* This module contains all function prototypes for Clock.c
*
* Khoi Le, 12/01/2022
*******************************************************************************/

#ifndef CLOCKH
#define CLOCKH

/*******************************************************************************
* ClockInit() - PUBLIC
*   parameter: none
*   description: Initialization anything needed to generate the Clock. This
*   function must be call before the any Clock function
*******************************************************************************/
void ClockInit(void);

/*******************************************************************************
* ClockTask() - PUBLIC
*   parameter: none
*   description: diplay the time on the LCD with ISO 8601 extended time display
*   format
*******************************************************************************/
void ClockTask(void);

#endif
