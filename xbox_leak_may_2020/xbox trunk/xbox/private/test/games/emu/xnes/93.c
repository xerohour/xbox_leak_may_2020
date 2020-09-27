#include "mapinc.h"



void Mapper93_write(uint16 A,uint8 V)
{
ROM_BANK16(0x8000,V>>4);
MIRROR_SET(V&1);
}

void Mapper93_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper93_write);
}

