#include "mapinc.h"



void Mapper66_write(uint16 A,uint8 V)
{
 VROM_BANK8(V&0xF);
 ROM_BANK32(0x8000,(V>>4));
}

void Mapper66_init(void)
{
 ROM_BANK32(0x8000,0);
 SetWriteHandler(0x6000,0xffff,(void *)Mapper66_write);
}
