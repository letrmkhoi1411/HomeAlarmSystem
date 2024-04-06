/*******************************************************************************
* WaveGen.h
*
* This module contains all function prototypes for WaveGen.c
*
* Khoi Le, 01/12/2022
*******************************************************************************/

#ifndef WAVEGENH
#define WAVEGENH

/*******************************************************************************
* WaveGenInit() - PUBLIC
*   parameter: none
*   description: Initialization anything needed to generate the sound wave,
*   which are PIT and DAC. This function must be call before WaveGenEnable()
*******************************************************************************/
void WaveGenInit(void);

/*******************************************************************************
* WaveGenEnable(INT8U mode) - PUBLIC
*   parameter: mode - 1 to enable Sin wave, 0 to disable sinwave
*   description: the function takes the parameter(mode) to either enable or
*   disable the sound wave
*******************************************************************************/
void WaveGenEnable(INT8U mode);

#endif
