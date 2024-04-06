/*******************************************************************************
* WaveGenDMA.h
*
* This module contains all function prototypes for WaveGenDMA.c
*
* Khoi Le, 12/01/2022
*******************************************************************************/

#ifndef WAVEGENDMAH
#define WAVEGENDMAH

/*******************************************************************************
* Definition of sample stream macros/constants
*******************************************************************************/
#define WAVE_DMA_OUT_CH             0
#define NUM_BLOCKS                  1
#define WAVE_BYTES_PER_SAMPLE       2
#define WAVE_SAMPLES_PER_BLOCK      64
#define WAVE_BYTES_PER_BLOCK        128
#define WAVE_BYTES_PER_BUFFER       (NUM_BLOCKS*BYTES_PER_BLOCK)
#define SIZE_CODE_16BIT             001

/*******************************************************************************
* WaveGenDMAEnable(INT8U mode) - PUBLIC
*   parameter: mode - 1 to enable wave, 0 to disable wave
*   description: the function takes the parameter (mode) to either enable or
*   disable the sound wave
*******************************************************************************/
void WaveGenDMAEnable(INT8U mode);

/*******************************************************************************
* WaveGenDMAInit() - PUBLIC
*   parameter: none
*   description: Initialization anything needed to generate the sound wave,
*   which are PIT, DAC, and DMA. This function must be call before
*   WaveGenDMAEnable()
*******************************************************************************/
void WaveGenDMAInit(void);

#endif
