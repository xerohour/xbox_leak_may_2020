#include "mapinc.h"



void Mapper76_write(uint16 A,uint8 V)
{
        switch(A&0xE001){
        case 0x8000:
         MMC3_cmd = V;
         break;
        case 0x8001:
                switch(MMC3_cmd&0x07){
                case 2: VROM_BANK2(0x000,V);break;
                case 3: VROM_BANK2(0x800,V);break;
                case 4: VROM_BANK2(0x1000,V);break;
                case 5: VROM_BANK2(0x1800,V);break;
                case 6:
                        if (MMC3_cmd&0x40) ROM_BANK8(0xC000,V);
                        else ROM_BANK8(0x8000,V);
                        break;
                case 7: ROM_BANK8(0xA000,V);
                        break;
               }
               break;
        case 0xA000:
        MIRROR_SET(V&1);
        break;
 }
}

void Mapper76_init(void)
{
SetWriteHandler(0x8000,0xffff,(void *)Mapper76_write);
}

