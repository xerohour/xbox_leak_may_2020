#include "mapinc.h"



void Mapper226_write(uint16 A,uint8 V)
{
 MIRROR_SET((A>>13)&1);
 VROM_BANK8(A&0x7F);
 if(A&0x1000)
  {
   if(A&0x40)
    {
     ROM_BANK16(0x8000,(((A>>7))<<1)+1);
     ROM_BANK16(0xC000,(((A>>7))<<1)+1);
    }
    else
    {
     ROM_BANK16(0x8000,(((A>>7))<<1));
     ROM_BANK16(0xC000,(((A>>7))<<1));
    }
  }
  else
  {
   ROM_BANK32(0x8000,A>>7);
  }
}

void Mapper226_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper226_write);
}

