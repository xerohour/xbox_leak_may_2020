#include "mapinc.h"



void Mapper151_write(uint16 A,uint8 V)
{
switch(A&0xF000)
 {
 case 0x8000:ROM_BANK8(0x8000,V);break;
 case 0xA000:ROM_BANK8(0xA000,V);break;
 case 0xC000:ROM_BANK8(0xC000,V);break;
 case 0xe000:VROM_BANK4(0x000,V);break;
 case 0xf000:VROM_BANK4(0x1000,V);break;
 }
}

void Mapper151_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper151_write);
}

