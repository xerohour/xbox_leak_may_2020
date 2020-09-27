#include "mapinc.h"



void Mapper112_write(uint16 A,uint8 V)
{
switch(A)
{
 case 0xe000:MIRROR_SET(V&1);break;
 case 0x8000:mapbyte1[0]=V;break;
 case 0xa000:switch(mapbyte1[0])
            {
            case 0:ROM_BANK8(0x8000,V);break;
            case 1:ROM_BANK8(0xA000,V);break;
                case 2: V&=0xFE;VROM_BANK1(0,V);
                        VROM_BANK1(0x400,(V+1));break;
                case 3: V&=0xFE;VROM_BANK1(0x800,V);
                        VROM_BANK1(0xC00,(V+1));break;
            case 4:VROM_BANK1(0x1000,V);break;
            case 5:VROM_BANK1(0x1400,V);break;
            case 6:VROM_BANK1(0x1800,V);break;
            case 7:VROM_BANK1(0x1c00,V);break;
            }
            break;
 }
}

void Mapper112_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper112_write);
}

