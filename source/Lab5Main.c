/*******************************************************************************
* EECE344 Lab 5
*
* In this lab, I will be designing a sinewave generator with controls so it can
* be turned on or off. It uses a timeslice scheduler along with event-based
* interrupt responses for multitasking.
*
* Khoi Le, 16/11/2022
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
#include "K65TWR_I2C.h"
#include "MMA8451Q.h"
#include "K65TWR_TSI.h"
#include "WaveGenDMA.h"
#include "Clock.h"

/*******************************************************************************
* Define constants and type
*******************************************************************************/
#define START_ADDS (INT8U*)0x00000000U
#define END_ADDS (INT8U*)0x001FFFFFU
#define SLICE_PERIOD 10
typedef enum {DISARMED, ARMED, ALARM} STATES_T;

/*******************************************************************************
* lab5ControlTask() - PRIVATE
*   parameter: none
*   description: this function handles the user interface and controls the alarm
*******************************************************************************/
static void lab5ControlTask(void);

/*******************************************************************************
* LEDTask() - PRIVATE
*   parameter: none
*   description: control the LEDs flashing
*******************************************************************************/
static void LEDTask(void);

/*******************************************************************************
* AccelTask() - PRIVATE
*   parameter: none
*   description: tampering notification using on-board accelerometer. detect
*   some form of movement that indicates that someone is tampering with the
*   alarm system.
*******************************************************************************/
static void AccelTask(void);

/*******************************************************************************
* Code
*******************************************************************************/
static STATES_T CurState;
static STATES_T PrevState;
static INT8U Led8Indi = 0;
static INT8U Led9Indi = 0;
static INT8U Counter1 = 0;
static INT8U Counter2 = 0;

void main(void){

    INT16U sum;

    K65TWR_BootClock();
    BIOOpen(BIO_BIT_RATE_115200);
    SysTickDlyInit();
    LcdDispInit();
    KeyInit();
    GpioDBugBitsInit();
    GpioLED8Init();
    GpioLED9Init();
    TSIInit();
    TSIChCalibration((INT8U)11);
    TSIChCalibration((INT8U)12);
    (void)MMA8451Init();
    WaveGenDMAInit();
    ClockInit();

    sum = MemCSumGet(START_ADDS,END_ADDS);
    LcdDispClear();
    LcdCursorMove(2, 1);
    LcdDispHexWord((INT32U)sum, 4);
    LcdCursorMode(0, 0);
    CurState = DISARMED;
    PrevState = ARMED;
    while(TRUE){
        SysTickWaitEvent(SLICE_PERIOD);
        KeyTask();
        TSITask();
        lab5ControlTask();
        LEDTask();
        AccelTask();
        ClockTask();
    }
}

/*******************************************************************************
* lab5ControlTask() - PRIVATE
*   parameter: none
*   description: this function handles the user interface and controls the alarm
*******************************************************************************/
static void lab5ControlTask(void){
    INT8C key_char;
    INT8U mode;
    DB2_TURN_ON();
    switch(CurState){
    case DISARMED:
        mode = 0;
        if(PrevState == ARMED || PrevState == ALARM){
            LED8_TURN_OFF();
            LED9_TURN_OFF();
            Led8Indi = 0;
            Led9Indi = 0;
            Counter1 = 0;
            Counter2 = 0;
            LcdDispLineClear(1);
            LcdCursorMove(1, 1);
            LcdDispString("DISARMED");
            PrevState = DISARMED;
            WaveGenDMAEnable(mode);
        } else{}
        key_char = KeyGet();
        if(key_char == DC1){
            CurState = ARMED;
        } else{}
        if(key_char == DC3){
            LcdDispLineClear(2);
            LcdCursorMove(2, 1);
            LcdDispHexWord((INT32U)MemCSumGet(START_ADDS,END_ADDS), 4);
        } else{}
        break;
    case ARMED:
        if(PrevState == DISARMED || PrevState == ALARM){
            Counter1 = 0;
            LcdDispLineClear(1);
            LcdCursorMove(1, 1);
            LcdDispString("ARMED");
            PrevState = ARMED;
        } else{}
        key_char = KeyGet();
        if(key_char == DC4){
            CurState = DISARMED;
        } else{}
        if(key_char == DC3){
            LcdDispLineClear(2);
            LcdCursorMove(2, 1);
            LcdDispHexWord((INT32U)MemCSumGet(START_ADDS,END_ADDS), 4);
        } else{}
        break;
    case ALARM:
        mode = 1;
        if(PrevState == ARMED || PrevState == DISARMED){
            LED8_TURN_OFF();
            LED9_TURN_OFF();
            Counter1 = 0;
            Counter2 = 0;
            LcdDispLineClear(1);
            LcdCursorMove(1, 1);
            LcdDispString("ALARM");
            PrevState = ALARM;
            WaveGenDMAEnable(mode);
        } else{}
        key_char = KeyGet();
        if(key_char == DC4){
            CurState = DISARMED;
        } else{}
        if(key_char == DC3){
            LcdDispLineClear(2);
            LcdCursorMove(2, 1);
            LcdDispHexWord((INT32U)MemCSumGet(START_ADDS,END_ADDS), 4);
        } else{}
        break;
    default:
        CurState = DISARMED;
        break;
    }
    DB2_TURN_OFF();
}

/*******************************************************************************
* LEDTask() - PRIVATE
*   parameter: none
*   description: control the LEDs flashing
*******************************************************************************/
static void LEDTask(void){
    INT16U cur_sense_flags;
    DB4_TURN_ON();
    cur_sense_flags = TSIGetSensorFlags();
    switch(CurState){
    case DISARMED:
        Counter1++;
        Counter2++;
        if((cur_sense_flags & (1<<BRD_PAD1_CH)) != 0){
            if(Counter1 <= 27){
                LED8_TURN_ON();
            } else if(Counter1 <= 49){
                LED8_TURN_OFF();
            } else if(Counter1 >= 50){
                Counter1 = 0;
            }
        }else{
            LED8_TURN_OFF();
        }
        if((cur_sense_flags & (1<<BRD_PAD2_CH)) != 0){
            if(Counter2 <= 25){
                LED9_TURN_ON();
            } else{
                LED9_TURN_OFF();
                if(Counter2 >= 50){
                    Counter2 = 0;
                } else{}
            }
        } else{
            LED9_TURN_OFF();
        }
        break;
    case ARMED:
        if((cur_sense_flags & (1<<BRD_PAD1_CH)) != 0){
            Led8Indi = 1;
            CurState = ALARM;
        }else{}
        if((cur_sense_flags & (1<<BRD_PAD2_CH)) != 0){
            Led9Indi = 1;
            CurState = ALARM;
        }else{}
        Counter1++;
        if(Counter1 <= 25){
            LED8_TURN_ON();
            LED9_TURN_OFF();
        } else{
            LED9_TURN_ON();
            LED8_TURN_OFF();
            if(Counter1 >= 50){
                Counter1 = 0;
            } else{}
        }
        break;
    case ALARM:
        if((cur_sense_flags & (1<<BRD_PAD1_CH)) != 0){
            Led8Indi = 1;
        }else{}
        if((cur_sense_flags & (1<<BRD_PAD2_CH)) != 0){
            Led9Indi = 1;
        }else{}
        if(Led8Indi == 1){
            Counter1++;
            if(Counter1 <= 5){
                LED8_TURN_ON();
            } else{
                LED8_TURN_OFF();
                if(Counter1 >= 10){
                    Counter1 = 0;
                } else{}
            }
        } else{
            Counter1 = 0;
        }
        if(Led9Indi == 1){
            Counter2++;
            if(Counter2 <= 5){
                LED9_TURN_ON();
            } else{
                LED9_TURN_OFF();
                if(Counter2 >= 10){
                    Counter2 = 0;
                } else{}
            }
        } else{
            Counter2 = 0;
        }
        break;
    default:
        CurState = DISARMED;
        break;
    }
    DB4_TURN_OFF();
}

/*******************************************************************************
* AccelTask() - PRIVATE
*   parameter: none
*   description: tampering notification using on-board accelerometer. detect
*   some form of movement that indicates that someone is tampering with the
*   alarm system.
*******************************************************************************/
static void AccelTask(void){
    INT8S x;
    INT8S y;
    INT8S z;
    DB5_TURN_ON();
    x = (INT8S)MMA8451RegRd(MMA8451_OUT_X_MSB);
    y = (INT8S)MMA8451RegRd(MMA8451_OUT_Y_MSB);
    z = (INT8S)MMA8451RegRd(MMA8451_OUT_Z_MSB);
    if(x >= 16 || y >= 16 || z <= 48){
        LcdDispLineClear(2);
        LcdCursorMove(2, 1);
        LcdDispString("TAMPERING ALARM");
    }else{}
    DB5_TURN_OFF();
}
