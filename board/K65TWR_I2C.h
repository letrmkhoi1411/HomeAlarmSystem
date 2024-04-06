/************************************************************************
 * DESCRIPTION: Header for a driver module for the MMA8451 Accelerometer
 *              on I2C0 as wired on the K65 tower board.
 * AUTHOR: Todd Morton
 * HISTORY: Started 11/23/14
 * Revision: Revised for the K65 board. TDm
 ***********************************************************************/
#ifndef I2C_DEF
#define I2C_DEF
/************************************************************************
* Public Functions
************************************************************************/
void I2CWr(INT8U dout);
INT8U I2CRd(void);
void I2CStop(void);
void I2CStart(void);
void I2CInit(void);
void I2CSendRepeatedStart(void);

#endif
