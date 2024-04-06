#ifndef K65TWR_TSI_H_
#define K65TWR_TSI_H_

#define BRD_PAD1_CH  12U
#define BRD_PAD2_CH  11U

void TSIInit(void);
void TSIChCalibration(INT8U channel);
INT16U TSIGetSensorFlags(void);
void TSITask(void);

#endif
