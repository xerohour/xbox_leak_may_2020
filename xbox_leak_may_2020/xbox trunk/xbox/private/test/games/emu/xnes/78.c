#include "mapinc.h"



void Mapper78_write(uint16 A,uint8 V)
{
ROM_BANK16(0x8000,V);
VROM_BANK8(V>>4);
}

void Mapper78_init(void)
{
SetWriteHandler(0x8000,0xffff,(void *)Mapper78_write);
}

