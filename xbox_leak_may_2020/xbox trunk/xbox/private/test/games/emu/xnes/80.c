#include "mapinc.h"



void Mapper80_write(uint16 A,uint8 V)
{
switch(A)
 {
  case 0x7ef0: VROM_BANK1(0x000,V);VROM_BANK1(0x400,(V+1));break;
  case 0x7ef1: VROM_BANK1(0x800,V);VROM_BANK1(0xC00,(V+1));break;

  case 0x7ef2: VROM_BANK1(0x1000,V);break;
  case 0x7ef3: VROM_BANK1(0x1400,V);break;
  case 0x7ef4: VROM_BANK1(0x1800,V);break;
  case 0x7ef5: VROM_BANK1(0x1c00,V);break;
  case 0x7efa:
  case 0x7efb: ROM_BANK8(0x8000,V);break;
  case 0x7efd:
  case 0x7efc: ROM_BANK8(0xA000,V);break;
  case 0x7efe:
  case 0x7eff: ROM_BANK8(0xC000,V);break;
 }
}

void Mapper80_init(void)
{
SetWriteHandler(0x4020,0x7fff,(void *)Mapper80_write);
}

