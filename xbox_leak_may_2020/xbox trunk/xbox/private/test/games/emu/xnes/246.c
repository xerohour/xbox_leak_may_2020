#include "mapinc.h"


void Mapper246_write(uint16 A,uint8 V)
{
 switch(A&0xF007)
 {
 case 0x6000:ROM_BANK8(0x8000,V);break;
 case 0x6001:ROM_BANK8(0xA000,V);break;
 case 0x6002:ROM_BANK8(0xC000,V);break;
 case 0x6003:ROM_BANK8(0xE000,V);break;
 case 0x6004:VROM_BANK2(0x0000,V);break;
 case 0x6005:VROM_BANK2(0x0800,V);break;
 case 0x6006:VROM_BANK2(0x1000,V);break;
 case 0x6007:VROM_BANK2(0x1800,V);break;
 }
}

void Mapper246_init(void)
{
  SetWriteHandler(0x4020,0x67ff,(void *)Mapper246_write);
  SetWriteHandler(0x8000,0xffff,(void *)Mapper246_write);
}

