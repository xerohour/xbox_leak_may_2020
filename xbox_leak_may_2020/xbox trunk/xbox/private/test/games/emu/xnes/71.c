#include "mapinc.h"



void Mapper71_write(uint16 A,uint8 V)
{
switch(A&0xF000)
 {
 case 0xF000:
 case 0xE000:
 case 0xD000:
 case 0xC000:ROM_BANK16(0x8000,V);break;
 case 0x9000:onemir((V>>3)&2);break;
 }
}

void Mapper71_init(void)
{
SetWriteHandler(0x4020,0xffff,(void *)Mapper71_write);
}

