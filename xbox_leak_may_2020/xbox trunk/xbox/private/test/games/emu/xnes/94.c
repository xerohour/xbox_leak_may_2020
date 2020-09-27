#include "mapinc.h"



void Mapper94_write(uint16 A,uint8 V)
{
ROM_BANK16(0x8000,V>>2);
}

void Mapper94_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper94_write);
}

