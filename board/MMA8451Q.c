/****************************************************************************************
 * DESCRIPTION: A driver module for the MMA8451Q Accelerometer on the I2C.
 *              As connected on the K65 Tower Board
 * 11/18/2021 Todd Morton Removed basic I2C functions
****************************************************************************************/
#include "MCUType.h"
#include "K65TWR_I2C.h"
#include "MMA8451Q.h"
/****************************************************************************************
* Function prototypes (Private)
****************************************************************************************/

/****************************************************************************************
* MMA8451Init - Initialize MMA8451Q
*               returns who am i id
****************************************************************************************/
INT8U MMA8451Init(void){
    INT8U accel_id;
    I2CInit();
    accel_id = MMA8451RegRd(MMA8451_WHO_AM_I); //Should be 0x1A
    MMA8451PLInit(); //portrait/landscape detect
    return accel_id;
}
/****************************************************************************************
* MMA8451RegWr - Write to MMA8451 register. Blocks until Xmit is complete.
* Parameters:
*   waddr is the address of the MMA8451 register to write
*   wdata is the value to be written to waddr
****************************************************************************************/
void MMA8451RegWr(INT8U waddr, INT8U wdata){
    I2CStart();                     /* Create I2C start                                */
    I2CWr((MMA8451_ADDR<<1)|WR);    /* Send MMA8451 address & W/R' bit                 */
    I2CWr(waddr);                   /* Send register address                           */
    I2CWr(wdata);                   /* Send write data                                 */
    I2CStop();                      /* Create I2C stop                                 */
}
/****************************************************************************************
* MMA8451RegRd - Read from MMA8451 register. Blocks until read is complete
* Parameters:
*   raddr is the register address to read
*   return value is the value read
****************************************************************************************/
INT8U MMA8451RegRd(INT8U raddr){
    INT8U rdata;
    I2CStart();                     /* Create I2C start                                */
    I2CWr((MMA8451_ADDR<<1)|WR);    /* Send MMA8451 address & W/R' bit                 */
    I2CWr(raddr);                   /* Send register address                           */
    I2CSendRepeatedStart();         /* Repeated Start                                  */
    I2CWr((MMA8451_ADDR<<1)|RD);    /* Send MMA8451 address & W/R' bit                 */
    rdata = I2CRd();                /* Send to read MMA8451 return value               */
    return rdata;
}
/****************************************************************************************
* MMA8451PLInit - Initialize 8451 for portrait/landscape detection.
* Parameters:
****************************************************************************************/
void MMA8451PLInit(void){
    INT8U treg;
    treg = MMA8451RegRd(MMA8451_CTRL_REG1);
    treg = treg & 0xfeu;            /* Clear active bit to put in standby mode         */
    MMA8451RegWr(MMA8451_CTRL_REG1,treg);
    MMA8451RegWr(MMA8451_PL_CFG,0xe0u); //Eneable PL detection
    treg = treg | 0x01u;            /* Set active bit to put in active mode         */
    MMA8451RegWr(MMA8451_CTRL_REG1,treg);
}
