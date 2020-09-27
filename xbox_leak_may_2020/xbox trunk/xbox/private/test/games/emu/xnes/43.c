#include "mapinc.h"



void Mapper43_write(uint16 A,uint8 V)
{
  uint32 m;
  int z;

  if(A&0x400)
   onemir(0);
  else
   MIRROR_SET((A>>13)&1);
  m=A&0x1f;

  z=(A>>8)&3;

  switch(pmask8)
  {
   default:
   case 0xFF:
             if(z&2)
              m|=0x20;
             break;
   case 0x1FF:
             m|=z<<5;
             break;
  }

   if(A&0x800)
   {
    ROM_BANK16(0x8000,(m<<1)|((A&0x1000)>>12));
    ROM_BANK16(0xC000,(m<<1)|((A&0x1000)>>12));
   }
   else
    ROM_BANK32(0x8000,m);
}

void Mapper43_init(void)
{
 ROM_BANK32(0x8000,0);
 SetWriteHandler(0x8000,0xffff,(void *)Mapper43_write);
}
