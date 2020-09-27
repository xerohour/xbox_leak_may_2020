#include "mapinc.h"



void Mapper11_write(uint16 A,uint8 V)
{
        ROM_BANK32(0x8000,V);
        VROM_BANK8(V>>4);
}

void Mapper11_init(void)
{
        ROM_BANK32(0x8000,0);
	SetWriteHandler(0x8000,0xFFFF,(void *)Mapper11_write);
}

