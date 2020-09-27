#include "mapinc.h"



void Mapper184_write(uint16 A,uint8 V)
{
VROM_BANK4(0x0000,V);
VROM_BANK4(0x1000,(V>>4));
}

void Mapper184_init(void)
{
  SetWriteHandler(0x6000,0xffff,(void *)Mapper184_write);
}

