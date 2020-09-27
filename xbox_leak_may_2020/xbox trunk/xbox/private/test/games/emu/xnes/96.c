#include "mapinc.h"



void Mapper96_write(uint16 A,uint8 V)
{
ROM_BANK32(0x8000,V);
}

void Mapper96_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper96_write);
}

