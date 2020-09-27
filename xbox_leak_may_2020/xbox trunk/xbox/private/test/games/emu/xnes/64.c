#include "mapinc.h"


void Mapper64_write(uint16 A,uint8 V)
{
 switch(A&0xF003)
 {
        case 0xa000:MIRROR_SET(V&1);break;
        case 0x8000:
         MMC3_cmd = V;
         if(MMC3_cmd&0x80) cbase=0x1000;
         else cbase=0x000;
        break;
	#include "mmc3irq.h"

        case 0x8001:
                switch(MMC3_cmd&15){
                case 0: VROM_BANK1(cbase^0x0000,V);VROM_BANK1(cbase^0x0400,V+1);break;
                case 1: VROM_BANK1(cbase^0x0800,V);VROM_BANK1(cbase^0x0C00,V+1);break;
                case 2: VROM_BANK1(cbase^0x1000,V); break;
                case 3: VROM_BANK1(cbase^0x1400,V); break;
                case 4: VROM_BANK1(cbase^0x1800,V); break;
                case 5: VROM_BANK1(cbase^0x1C00,V); break;
                case 6:
                        if (MMC3_cmd&0x40) ROM_BANK8(0xa000,V);
                        else ROM_BANK8(0x8000,V);
                        break;
                case 7:
                        if (MMC3_cmd&0x40) ROM_BANK8(0xC000,V);
                        else ROM_BANK8(0xA000,V);
                        break;
                case 8: VROM_BANK1(cbase^0x0400,V);break;
                case 9: VROM_BANK1(cbase^0x0c00,V);break;
                case 15:
                        if (MMC3_cmd&0x40) ROM_BANK8(0x8000,V);
                        else ROM_BANK8(0xC000,V);
                        break;
                default: break;
               }
               break;
  }	
}

void Mapper64_init(void)
{
 MapHBIRQHook=(void *)MMC3_hb;
 SetWriteHandler(0x8000,0xffff,(void *)Mapper64_write);
}
