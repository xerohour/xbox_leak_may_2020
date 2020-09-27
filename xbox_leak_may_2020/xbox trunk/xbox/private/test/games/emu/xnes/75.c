#include "mapinc.h"


#define map75sel mapbyte1[0]
#define map75ar  mapbyte2

void Mapper75_write(uint16 A,uint8 V)
{
switch(A&0xF000)
 {
 case 0x8000:ROM_BANK8(0x8000,V);break;
 case 0x9000:
             VROM_BANK4(0x0000,map75ar[0]|((V&2)<<3));
             VROM_BANK4(0x1000,map75ar[1]|((V&4)<<2));
             map75sel=V;MIRROR_SET(V&1);break;
 case 0xa000:ROM_BANK8(0xa000,V);break;
 case 0xc000:ROM_BANK8(0xc000,V);break;
 case 0xe000:V&=0xF;map75ar[0]=V;V|=(map75sel&2)<<3;VROM_BANK4(0x0000,V);break;
 case 0xf000:V&=0xF;map75ar[1]=V;V|=(map75sel&4)<<2;VROM_BANK4(0x1000,V);break;
 }
}

void Mapper75_init(void)
{
SetWriteHandler(0x8000,0xffff,(void *)Mapper75_write);
}

