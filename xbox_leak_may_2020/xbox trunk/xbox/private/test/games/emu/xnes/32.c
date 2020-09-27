#include "mapinc.h"


#define IREMCon mapbyte1[0]

void Mapper32_write(uint16 A,uint8 V)
{
switch(A>>12)
 {
 case 0x8:
          if(IREMCon) ROM_BANK8(0xc000,V);
          else ROM_BANK8(0x8000,V);
          break;
 case 0x9:MIRROR_SET2(V&1);
          IREMCon=(V>>1)&1;
          MIRROR_SET(V&1);
          break;
 case 0xa:ROM_BANK8(0xA000,V);
          break;
 }

if((A&0xF000)==0xb000)
 {
 switch(A&0x000f)
  {
  case 0:VROM_BANK1(0x000,V);break;
  case 1:VROM_BANK1(0x400,V);break;
  case 2:VROM_BANK1(0x800,V);break;
  case 3:VROM_BANK1(0xc00,V);break;
  case 4:VROM_BANK1(0x1000,V);break;
  case 5:VROM_BANK1(0x1400,V);break;
  case 6:VROM_BANK1(0x1800,V);break;
  case 7:VROM_BANK1(0x1c00,V);break;
  }
 }
}
void Mapper32_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper32_write);
}
