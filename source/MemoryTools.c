/*******************************************************************************
* EECE344
*
* Memory Tools
*
* Khoi Le, 10/10/2022
*******************************************************************************/

#include "MCUType.h"
#include "MemoryTools.h"

INT16U MemCSumGet(INT8U *startaddr, INT8U *endaddr) {
    INT16U sum = 0;
    INT8U *addr_ptr = startaddr;
    while (addr_ptr < endaddr) {
        sum += (INT16U)*addr_ptr;
        addr_ptr++;
    }
    sum += (INT16U)*addr_ptr;
    return sum;
}
