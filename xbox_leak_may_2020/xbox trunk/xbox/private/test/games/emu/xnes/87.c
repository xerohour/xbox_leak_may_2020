#include "mapinc.h"


void Mapper87_write(uint16 A,uint8 V)
{
VROM_BANK8(V>>1);
}

void Mapper87_init(void)
{
  SetWriteHandler(0x6000,0xffff,(void *)Mapper87_write);
}

