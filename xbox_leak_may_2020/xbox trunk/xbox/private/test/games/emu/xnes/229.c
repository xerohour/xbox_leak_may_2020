#include "mapinc.h"



void Mapper229_write(uint16 A,uint8 V)
{
if(A>=0x8000)
{
MIRROR_SET((A>>5)&1);
if(!(A&0x1e))
 {
 ROM_BANK32(0x8000,0);
 }
else
 {
 ROM_BANK16(0x8000,A&0x1f);
 ROM_BANK16(0xC000,A&0x1f);
 }
 VROM_BANK8((A&vmask));
}

}

void Mapper229_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper229_write);
}

