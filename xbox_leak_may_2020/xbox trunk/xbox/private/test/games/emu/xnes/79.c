#include "mapinc.h"



void Mapper79_write(uint16 A,uint8 V)
{
 if(A<0x8000 && ((A^0x4100)==0))
 {
 ROM_BANK32(0x8000,(V>>3)&1);
 }
VROM_BANK8(V);
}

void Mapper79_init(void)
{
SetWriteHandler(0x8000,0xffff,(void *)Mapper79_write);
SetWriteHandler(0x4020,0x5fff,(void *)Mapper79_write);
}

