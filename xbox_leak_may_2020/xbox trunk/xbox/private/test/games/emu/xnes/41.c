#include "mapinc.h"


#define calreg mapbyte1[0]
#define calchr mapbyte1[1]

void Mapper41_write(uint16 A,uint8 V)
{
 if(A<0x8000)
 {
 ROM_BANK32(0x8000,A&7);
 MIRROR_SET((A>>5)&1);
 calreg=A;
 calchr&=0x3;
 calchr|=(A>>1)&0xC;
 VROM_BANK8(calchr);
 }
 else if(calreg&0x4)
 {
 calchr&=0xC;
 calchr|=A&3;
 VROM_BANK8(calchr);
 }
}

void Mapper41_init(void)
{
 ROM_BANK32(0x8000,0);
 SetWriteHandler(0x8000,0xffff,(void *)Mapper41_write);
 SetWriteHandler(0x6000,0x67ff,(void *)Mapper41_write);
}
