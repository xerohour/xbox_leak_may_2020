#include "mapinc.h"



void Mapper180_write(uint16 A,uint8 V)
{
ROM_BANK16(0xC000,V);
}

void Mapper180_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper180_write);
}

