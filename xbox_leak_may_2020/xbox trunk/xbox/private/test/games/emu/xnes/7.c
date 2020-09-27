#include "mapinc.h"



void Mapper7_write(uint16 A,uint8 V)
{
      ROM_BANK32(0x8000,V&0xF);
      onemir((V>>4)&1);
}

void Mapper7_init(void)
{
        onemir(0);
        SetWriteHandler(0x8000,0xFFFF,(void *)Mapper7_write);
}

