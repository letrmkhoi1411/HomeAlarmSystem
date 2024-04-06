/*******************************************************************************
* WaveGenDMA.c
*
* This module contains all function needed for producing the sound wave from
* a lookup table using DMA.
*
* Khoi Le, 12/01/2022
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
#include "WaveGenDMA.h"

/*******************************************************************************
* Private Resources
*******************************************************************************/
static const INT16U wgdWaveTable[64] =
{0x07FF,0x0CA4,0x0D60,0x0AA6,0x0836,0x085A,0x09A5,0x09AD,
 0x0877,0x07F1,0x08C9,0x097C,0x08D5,0x07E1,0x0845,0x0989,
 0x0999,0x0819,0x0793,0x0A36,0x0E4C,0x0F94,0x0C45,0x0763,
 0x0544,0x06AC,0x089B,0x0889,0x0746,0x0711,0x0828,0x08C2,
 0x07FF,0x073B,0x07D6,0x08ED,0x08B8,0x0775,0x0763,0x0952,
 0x0ABA,0x089B,0x03B9,0x006A,0x01B2,0x05C8,0x086B,0x07E5,
 0x0665,0x0675,0x07B9,0x081D,0x0729,0x0682,0x0735,0x080D,
 0x0787,0x0651,0x0659,0x07A4,0x07C8,0x0558,0x029E,0x035A};

static const INT16U wgdWaveTableOff[64] =
{0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,
 0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,
 0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,
 0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,
 0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,
 0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,
 0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,
 0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF,0x07FF};

/******************************************************************************
* Function Code
******************************************************************************/

/*******************************************************************************
* WaveGenDMAInit() - PUBLIC
*   parameter: none
*   description: Initialization anything needed to generate the sound wave,
*   which are PIT, DAC, and DMA. This function must be call before
*   WaveGenDMAEnable()
*******************************************************************************/
void WaveGenDMAInit(void){
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;

    SIM->SCGC2 |= SIM_SCGC2_DAC0(1);
    DAC0->C0 |= DAC_C0_DACEN(1);
    DAC0->C0 |= DAC_C0_DACRFS(1);
    DAC0->C0 |= DAC_C0_DACTRGSEL(1);
    DAC0->C1 |= DAC_C1_DMAEN(1);

    SIM->SCGC6 |= SIM_SCGC6_PIT(1); //Turn on PIT clock
    PIT->MCR = PIT_MCR_MDIS(0);     //Enable PIT clock
    PIT->CHANNEL[0].LDVAL = 3124;   //set Tep to 52us (Buss clock = 60MHz)
    // Enable ints, start Timer 0
    PIT->CHANNEL[0].TCTRL = (PIT_TCTRL_TIE(1)|PIT_TCTRL_TEN(1));
}

/*******************************************************************************
* WaveGenDMAEnable(INT8U mode) - PUBLIC
*   parameter: mode - 1 to enable wave, 0 to disable wave
*   description: the function takes the parameter (mode) to either enable or
*   disable the sound wave
*******************************************************************************/
void WaveGenDMAEnable(INT8U mode){
    if(mode == 1){
        DMAMUX->CHCFG[WAVE_DMA_OUT_CH] &= ~(DMAMUX_CHCFG_ENBL_MASK|DMAMUX_CHCFG_TRIG_MASK);
        DMA0->TCD[WAVE_DMA_OUT_CH].SADDR = DMA_SADDR_SADDR(wgdWaveTable);
        DMA0->TCD[WAVE_DMA_OUT_CH].ATTR = DMA_ATTR_SMOD(0) | DMA_ATTR_SSIZE(SIZE_CODE_16BIT)
                                        | DMA_ATTR_DMOD(0) | DMA_ATTR_DSIZE(SIZE_CODE_16BIT);
        DMA0->TCD[WAVE_DMA_OUT_CH].SOFF = DMA_SOFF_SOFF(WAVE_BYTES_PER_SAMPLE);
        DMA0->TCD[WAVE_DMA_OUT_CH].SLAST = DMA_SLAST_SLAST(-(WAVE_BYTES_PER_BLOCK));
        DMA0->TCD[WAVE_DMA_OUT_CH].DADDR = DMA_DADDR_DADDR(&DAC0->DAT[0].DATL);
        DMA0->TCD[WAVE_DMA_OUT_CH].DOFF = DMA_DOFF_DOFF(0);
        DMA0->TCD[WAVE_DMA_OUT_CH].DLAST_SGA = DMA_DLAST_SGA_DLASTSGA(0);
        DMA0->TCD[WAVE_DMA_OUT_CH].NBYTES_MLNO = DMA_NBYTES_MLNO_NBYTES(WAVE_BYTES_PER_SAMPLE);
        DMA0->TCD[WAVE_DMA_OUT_CH].CITER_ELINKNO = DMA_CITER_ELINKNO_ELINK(0)|
                                                   DMA_CITER_ELINKNO_CITER(WAVE_SAMPLES_PER_BLOCK);
        DMA0->TCD[WAVE_DMA_OUT_CH].BITER_ELINKNO = DMA_BITER_ELINKNO_ELINK(0)|
                                                   DMA_BITER_ELINKNO_BITER(WAVE_SAMPLES_PER_BLOCK);
        DMA0->TCD[WAVE_DMA_OUT_CH].CSR = DMA_CSR_ESG(0) | DMA_CSR_MAJORELINK(0) |
                                         DMA_CSR_BWC(3) | DMA_CSR_INTHALF(0) |
                                         DMA_CSR_INTMAJOR(0) | DMA_CSR_DREQ(0) |
                                         DMA_CSR_START(0);
        DMAMUX->CHCFG[WAVE_DMA_OUT_CH] = DMAMUX_CHCFG_ENBL(1)|DMAMUX_CHCFG_TRIG(1)|DMAMUX_CHCFG_SOURCE(60);
        DMA0->SERQ = DMA_SERQ_SERQ(WAVE_DMA_OUT_CH);
    } else if(mode == 0){
        DMAMUX->CHCFG[WAVE_DMA_OUT_CH] &= ~(DMAMUX_CHCFG_ENBL_MASK|DMAMUX_CHCFG_TRIG_MASK);
        DMA0->TCD[WAVE_DMA_OUT_CH].SADDR = DMA_SADDR_SADDR(wgdWaveTableOff);
        DMA0->TCD[WAVE_DMA_OUT_CH].ATTR = DMA_ATTR_SMOD(0) | DMA_ATTR_SSIZE(SIZE_CODE_16BIT)
                                        | DMA_ATTR_DMOD(0) | DMA_ATTR_DSIZE(SIZE_CODE_16BIT);
        DMA0->TCD[WAVE_DMA_OUT_CH].SOFF = DMA_SOFF_SOFF(WAVE_BYTES_PER_SAMPLE);
        DMA0->TCD[WAVE_DMA_OUT_CH].SLAST = DMA_SLAST_SLAST(-(WAVE_BYTES_PER_BLOCK));
        DMA0->TCD[WAVE_DMA_OUT_CH].DADDR = DMA_DADDR_DADDR(&DAC0->DAT[0].DATL);
        DMA0->TCD[WAVE_DMA_OUT_CH].DOFF = DMA_DOFF_DOFF(0);
        DMA0->TCD[WAVE_DMA_OUT_CH].DLAST_SGA = DMA_DLAST_SGA_DLASTSGA(0);
        DMA0->TCD[WAVE_DMA_OUT_CH].NBYTES_MLNO = DMA_NBYTES_MLNO_NBYTES(WAVE_BYTES_PER_SAMPLE);
        DMA0->TCD[WAVE_DMA_OUT_CH].CITER_ELINKNO = DMA_CITER_ELINKNO_ELINK(0)|
                                                   DMA_CITER_ELINKNO_CITER(WAVE_SAMPLES_PER_BLOCK);
        DMA0->TCD[WAVE_DMA_OUT_CH].BITER_ELINKNO = DMA_BITER_ELINKNO_ELINK(0)|
                                                   DMA_BITER_ELINKNO_BITER(WAVE_SAMPLES_PER_BLOCK);
        DMA0->TCD[WAVE_DMA_OUT_CH].CSR = DMA_CSR_ESG(0) | DMA_CSR_MAJORELINK(0) |
                                         DMA_CSR_BWC(3) | DMA_CSR_INTHALF(0) |
                                         DMA_CSR_INTMAJOR(0) | DMA_CSR_DREQ(0) |
                                         DMA_CSR_START(0);
        DMAMUX->CHCFG[WAVE_DMA_OUT_CH] = DMAMUX_CHCFG_ENBL(1)|DMAMUX_CHCFG_TRIG(1)|DMAMUX_CHCFG_SOURCE(60);
        DMA0->SERQ = DMA_SERQ_SERQ(WAVE_DMA_OUT_CH);
    } else{}
}
