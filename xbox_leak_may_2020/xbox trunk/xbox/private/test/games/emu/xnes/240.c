#include "mapinc.h"



void Mapper240_write(uint16 A,uint8 V)
{
 if(A<0x8000)
 {
  ROM_BANK32(0x8000,V>>4);
  VROM_BANK8(V&0xF);
 }
}

void Mapper240_init(void)
{
  SetWriteHandler(0x4020,0x5fff,(void *)Mapper240_write);
  SetWriteHandler(0x8000,0xffff,(void *)Mapper240_write);
}

