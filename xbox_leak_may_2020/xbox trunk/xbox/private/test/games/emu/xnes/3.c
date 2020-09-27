#include "mapinc.h"


void Mapper3_write(uint16 A,uint8 V)
{
        VROM_BANK8(V);
}

void Mapper3_init(void)
{
SetWriteHandler(0x8000,0xFFFF,(void *)Mapper3_write);
}
