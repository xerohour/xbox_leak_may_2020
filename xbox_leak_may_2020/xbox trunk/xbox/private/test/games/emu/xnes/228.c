#include "mapinc.h"



void Mapper228_write(uint16 A,uint8 V)
{
MIRROR_SET((A>>13)&1);

VROM_BANK8(((V&3)|((A&0xF)<<2)));

if((A>>5)&1) // Page size is 16 KB
 {
  if((A>>6)&1)  // High 16 KB mirror
   {
     if(((A>>11)&3)==3)
      {
      ROM_BANK16(0x8000,(((((A>>7)&0xF)|0x20)<<1)+1));
      ROM_BANK16(0xC000,(((((A>>7)&0xF)|0x20)<<1)+1));
      }
     else
      {
      ROM_BANK16(0x8000,(((((A>>7)&0xF)|((A>>7)&0x30))<<1)+1));
      ROM_BANK16(0xC000,(((((A>>7)&0xF)|((A>>7)&0x30))<<1)+1));
      }
   }
   else         // Low 16 KB mirror
   {
     if(((A>>11)&3)==3)
      {
      ROM_BANK16(0x8000,((((A>>7)&0xF)|0x20)<<1));
      ROM_BANK16(0xC000,((((A>>7)&0xF)|0x20)<<1));
      }
     else
      {
      ROM_BANK16(0x8000,((((A>>7)&0xF)|((A>>7)&0x30))<<1));
      ROM_BANK16(0xC000,((((A>>7)&0xF)|((A>>7)&0x30))<<1));
      }
   }

 }
else         // Page size is 32 KB
 {
     if(((A>>11)&3)==3)
      {
      ROM_BANK32(0x8000,((((A>>7)&0x0f)|0x20)));
      }
      else
      {
      ROM_BANK32(0x8000,((((A>>7)&0x0f)|((A>>7)&0x30))));
      }
 }
}

void Mapper228_init(void)
{
  ROM_BANK32(0x8000,0);
  SetWriteHandler(0x8000,0xffff,(void *)Mapper228_write);
}

