/****************************************************************************************
 * DESCRIPTION: A basic I2C driver module as connected on the K65 Tower Board
 * AUTHOR: Todd Morton
 * 11/18/2021 Todd Morton Seperated basic I2C functions from MMA8451Q module.
****************************************************************************************/
#include "MCUType.h"
#include "K65TWR_I2C.h"
/****************************************************************************************
* Function prototypes (Private)
****************************************************************************************/


/****************************************************************************************
* I2CInit - Initialize I2C0.
****************************************************************************************/
void I2CInit(void){
    SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;               /*Turn on I2C clock                */
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;              /*Turn on PORTE clock              */

    PORTE->PCR[19] = PORT_PCR_MUX(4)|PORT_PCR_ODE(1);  /* Configure GPIO for I2C0         */
    PORTE->PCR[18] = PORT_PCR_MUX(4)|PORT_PCR_ODE(1);  /* and open drain                  */

    I2C0->F  = I2C_F_ICR(0x2c);                      /* Set SCL to 104kHz               */
    I2C0->C1 |= I2C_C1_IICEN_MASK;                     /* Enable I2C                      */
}

/*********************************************************************
* i2cSendRepeatedStart(void) - Public
*
*  PARAMETERS: none.
*
*  DESCRIPTION: Sends repeated start command on I2C Bus.
*
********************************************************************/
void I2CSendRepeatedStart(void){
    I2C0->C1 |= 0x04;    //send repeated start
}

/****************************************************************************************
* I2CWr - Write one byte to I2C. Blocks until byte Xmit is complete
* Parameters:
*   dout is the data/address to send
****************************************************************************************/
void I2CWr(INT8U dout){
    I2C0->D = dout;                              /* Send data/address                   */
    while((I2C0->S & I2C_S_IICIF_MASK) == 0) {}  /* Wait for completion                 */
    I2C0->S |= I2C_S_IICIF(1);                 /* Clear IICIF flag                    */
}

/****************************************************************************************
* I2CRd - Read one byte from I2C. Blocks until byte reception is complete
* Parameters:
*   Return value is the data returned I2C peripheral
****************************************************************************************/
INT8U I2CRd(void){
    INT8U din;
    I2C0->C1 &= (INT8U)(~I2C_C1_TX_MASK);        /*Set to controller receive mode       */
    I2C0->C1 |= I2C_C1_TXAK_MASK;                /*Set to no ack on read                */
    din = I2C0->D;                               /*Dummy read to generate clock cycles  */
    while((I2C0->S & I2C_S_IICIF_MASK) == 0) {}  /* Wait for completion                 */
    I2C0->S |= I2C_S_IICIF(1);                 /* Clear IICIF flag                    */
    I2CStop();                                  /* Send Stop                           */
    din = I2C0->D;                               /* Read data that was clocked in       */
    return din;
}
/****************************************************************************************
* I2CStop - Generate a Stop sequence to free the I2C bus.
****************************************************************************************/
void I2CStop(void){
    while((I2C0->S & I2C_S_TCF_MASK) == 0){}   //Wait for transfers to be complete
    I2C0->C1 &= (INT8U)(~I2C_C1_MST_MASK);
    I2C0->C1 &= (INT8U)(~I2C_C1_TX_MASK);
}
/****************************************************************************************
* I2CStart - Generate a Start sequence to grab the I2C bus.
****************************************************************************************/
void I2CStart(void){
    while((I2C0->S & I2C_S_BUSY_MASK) != 0){}   //Wait for idle bus
    I2C0->C1 |= I2C_C1_TX_MASK;
    I2C0->C1 |= I2C_C1_MST_MASK;
}

