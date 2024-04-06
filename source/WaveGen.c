/*******************************************************************************
* WaveGen.c
*
* This module contains all function needed for the PIT and DAC to produce the
* sound wave from a lookup table.
*
* Khoi Le, 01/12/2022
*******************************************************************************/

/*******************************************************************************
* Includes
*******************************************************************************/
#include "MCUType.h"
#include "BasicIO.h"
#include "K65TWR_ClkCfg.h"
#include "MemoryTools.h"
#include "K65TWR_GPIO.h"
#include "SysTickDelay.h"
#include "LCD.h"
#include "Key.h"
#include "WaveGen.h"

/*******************************************************************************
* Private Resources
*******************************************************************************/
static void PitInit(void);
static void DACInit(void);
static INT8U wgTableIndex;
static INT8U wgEnable;
static const INT16U wgWaveTable[64] =
{0x07FF,0x0CA4,0x0D60,0x0AA6,0x0836,0x085A,0x09A5,0x09AD,
 0x0877,0x07F1,0x08C9,0x097C,0x08D5,0x07E1,0x0845,0x0989,
 0x0999,0x0819,0x0793,0x0A36,0x0E4C,0x0F94,0x0C45,0x0763,
 0x0544,0x06AC,0x089B,0x0889,0x0746,0x0711,0x0828,0x08C2,
 0x07FF,0x073B,0x07D6,0x08ED,0x08B8,0x0775,0x0763,0x0952,
 0x0ABA,0x089B,0x03B9,0x006A,0x01B2,0x05C8,0x086B,0x07E5,
 0x0665,0x0675,0x07B9,0x081D,0x0729,0x0682,0x0735,0x080D,
 0x0787,0x0651,0x0659,0x07A4,0x07C8,0x0558,0x029E,0x035A};

/*******************************************************************************
* PIT0_IRQHandler() - PIT0 interrupt service routine, function prototype
*******************************************************************************/
void PIT0_IRQHandler(void);

/*******************************************************************************
* DACInit() - PRIVATE
*   parameter: none
*   description: Initialization DAC. This function must be called before using
*   DAC
*******************************************************************************/
static void DACInit(void){
    SIM->SCGC2 |= SIM_SCGC2_DAC0(1);
    DAC0->C0 |= DAC_C0_DACEN(1);
    DAC0->C0 |= DAC_C0_DACRFS(1);
    DAC0->C0 |= DAC_C0_DACTRGSEL(1);
}

/*******************************************************************************
* PitInit() - PRIVATE
*   parameter: none
*   description: Initialization PIT. This function must be called before using
*   PIT Handler function
*******************************************************************************/
static void PitInit(void){
    SIM->SCGC6 |= SIM_SCGC6_PIT(1); //Turn on PIT clock
    PIT->MCR = PIT_MCR_MDIS(0);     //Enable PIT clock
    PIT->CHANNEL[0].LDVAL = 3124;   //set Tep to 52us (Buss clock = 60MHz)
    // Enable ints, start Timer 0
    PIT->CHANNEL[0].TCTRL = (PIT_TCTRL_TIE(1)|PIT_TCTRL_TEN(1));
    //Enable PIT interrupt
    NVIC_EnableIRQ(PIT0_IRQn);
}

/*******************************************************************************
* PIT0_IRQHandler() - PIT0 interrupt service routine
*   parameter: none
*   description: when periodic interrupt happens, this function will clear the
*   timer interrupt flag. Then it will take a sample from wgWaveTable LUT and
*   send the sample out to DAC to convert the sample to analog signal.
*******************************************************************************/
void PIT0_IRQHandler(void){
    INT16U sample;
    PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
    if(wgEnable == 1){
        if(wgTableIndex<=63){
            sample = wgWaveTable[wgTableIndex];
            wgTableIndex++;
        } else {
            wgTableIndex = 0;
            sample = wgWaveTable[wgTableIndex];
            wgTableIndex ++;
        }
    } else if(wgEnable == 0){
        sample = wgWaveTable[0];
    } else{}
    DAC0->DAT[0].DATL = DAC_DATL_DATA0(sample);
    DAC0->DAT[0].DATH = DAC_DATH_DATA1((sample>>8));
}

/*******************************************************************************
* WaveGenInit() - PUBLIC
*   parameter: none
*   description: Initialization anything needed to generate the sinwave, which
*   are PIT and DAC. This function must be call before WaveGenEnable()
*******************************************************************************/
void WaveGenInit(void){
    PitInit();
    DACInit();
}

/*******************************************************************************
* WaveGenEnable(INT8U mode) - PUBLIC
*   parameter: mode - 1 to enable sound wave, 0 to disable sound wave
*   description: the function takes the parameter(mode) to either enable or
*   disable the sound wave
*******************************************************************************/
void WaveGenEnable(INT8U mode){
    if(mode == 1){
        wgTableIndex = 0;
        wgEnable = 1;
    } else if(mode == 0){
        wgTableIndex = 0;
        wgEnable = 0;
    } else{}
}
