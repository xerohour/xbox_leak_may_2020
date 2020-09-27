#include "mapinc.h"



void Mapper8_write(uint16 A,uint8 V)
{
        ROM_BANK16(0x8000,V>>3);
        VROM_BANK8(V&7);
}

void Mapper8_init(void)
{
	ROM_BANK32(0x8000,0);
        SetWriteHandler(0x8000,0xFFFF,(void *)Mapper8_write);
}

