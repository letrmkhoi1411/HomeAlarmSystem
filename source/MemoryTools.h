/*******************************************************************************
* EECE344
*
* Memory Tools header
*
* Khoi Le, 10/10/2022
*******************************************************************************/

#ifndef MEMORYTOOLSH
#define MEMORYTOOLSH

/*******************************************************************************
* MemCSumGet() calculate the sum of the content of a memory block
* It returns the sum in 16 bit un-sign integer
*******************************************************************************/
INT16U MemCSumGet(INT8U *startaddr, INT8U *endaddr);

#endif
