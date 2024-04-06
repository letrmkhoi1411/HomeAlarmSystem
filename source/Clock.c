/*******************************************************************************
* Clock.c
*
* This module contains all function needed for producing the clock
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
#include "Clock.h"

/*******************************************************************************
* Private Resources
*******************************************************************************/

/******************************************************************************
* Function Code
******************************************************************************/

/*******************************************************************************
* ClockInit() - PUBLIC
*   parameter: none
*   description: Initialization anything needed to generate the Clock. This
*   function must be call before the any Clock function
*******************************************************************************/
void ClockInit(void){
    SIM->SCGC6 |= SIM_SCGC6_RTC(1);
    RTC->CR |= RTC_CR_OSCE(1);
    SysTickWaitEvent(100);
    RTC->WAR |= RTC_WAR_TSRW(1);
    RTC->SR |= RTC_SR_TCE(0);
    RTC->TSR |= 0;
    RTC->SR |= RTC_SR_TCE(1);

}
/*******************************************************************************
* ClockTask() - PUBLIC
*   parameter: none
*   description: diplay the time on the LCD with ISO 8601 extended time display
*   format
*******************************************************************************/
void ClockTask(void){
    INT32U clock_count;
    INT8U hour;
    INT8U minute;
    INT32U second;
    clock_count = (INT32U)RTC->TSR;
    second = clock_count;
    hour = second / 3600;
    if(hour >= 24){
        hour = hour - 24;
    }
    minute = (second % 3600) / 60;
    second = second % 60;
    LcdCursorMove(1, 9);
    LcdDispDecWord(hour, 2, LCD_DEC_MODE_LZ);
    LcdDispChar(':');
    LcdDispDecWord(minute, 2, LCD_DEC_MODE_LZ);
    LcdDispChar(':');
    LcdDispDecWord(second, 2, LCD_DEC_MODE_LZ);
}
