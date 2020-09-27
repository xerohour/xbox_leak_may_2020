#include "mapinc.h"



void Mapper34_write(uint16 A,uint8 V)
{
switch(A)
 {
 case 0x7FFD:ROM_BANK32(0x8000,V);break;
 case 0x7FFE:VROM_BANK4(0x0000,V);break;
 case 0x7fff:VROM_BANK4(0x1000,V);break;
 }
if(A>=0x8000)
 ROM_BANK32(0x8000,V);
}

void Mapper34_init(void)
{
  SetWriteHandler(0x7ffd,0xffff,(void *)Mapper34_write);
}
