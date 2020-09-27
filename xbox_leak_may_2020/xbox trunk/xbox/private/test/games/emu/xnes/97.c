#include "mapinc.h"



void Mapper97_write(uint16 A,uint8 V)
{
ROM_BANK16(0xC000,V&15);
switch(V>>6)
 {
 case 0:break;
 case 1:MIRROR_SET2(0);break;
 case 2:MIRROR_SET2(1);break;
 case 3:break;
 }
}

void Mapper97_init(void)
{
  ROM_BANK16(0x8000,pmask16);
  SetWriteHandler(0x8000,0xffff,(void *)Mapper97_write);
}

