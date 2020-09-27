#include "mapinc.h"



void Mapper119_write(uint16 A,uint8 V)
{
	        switch(A&0xE001){
        case 0x8000:
         if((V&0x40) != (MMC3_cmd&0x40))
         {
            byte swa;
            swa=PRGBankList[0];
            ROM_BANK8(0x8000,PRGBankList[2]);
            ROM_BANK8(0xc000,swa);
         }
        if(VROM_size)
         if((V&0x80) != (MMC3_cmd&0x80))
         {
            byte swa,swa2,x;
            for(x=0;x<4;x++)
            {
             swa=CHRBankList[4+x];
             swa2=VPAL[4+x];
             if(VPAL[x])
              {VRAM_BANK1(0x1000,CHRBankList[x]&7);}
             else
              {VROM_BANK1(0x1000,CHRBankList[x]&vmask1);}
             if(swa2)
              {VRAM_BANK1(0x0000,swa&7);}
             else
              {VROM_BANK1(0x0000,swa&vmask1);}
            }
         }
        MMC3_cmd = V;
        cbase=((V^0x80)&0x80)<<5;
        break;

        case 0x8001:
                switch(MMC3_cmd&0x07){
                case 0: V&=0xFE;
                        if(V&0x40)
                         {V&=7;VRAM_BANK1(cbase^0x1000,V);VRAM_BANK1(cbase^0x1400,(V+1));}
                        else
                         {VROM_BANK1(cbase^0x1000,V);VROM_BANK1(cbase^0x1400,(V+1));}
                        break;
                case 1: V&=0xFE;
                        if(V&0x40)
                         {V&=7;VRAM_BANK1(cbase^0x1800,V);VRAM_BANK1(cbase^0x1C00,(V+1));}
                        else
                         {VROM_BANK1(cbase^0x1800,V);VROM_BANK1(cbase^0x1C00,(V+1));}
                        break;
                case 2:
                        if(V&0x40)
                         {V&=7;VRAM_BANK1(cbase^0x000,V);}
                        else
                         {VROM_BANK1(cbase^0x000,V);}
                        break;
                case 3:
                        if(V&0x40)
                         {V&=7;VRAM_BANK1(cbase^0x400,V);}
                        else
                         {VROM_BANK1(cbase^0x400,V);}
                        break;
                case 4:
                        if(V&0x40)
                         {V&=7;VRAM_BANK1(cbase^0x800,V);}
                        else
                         {VROM_BANK1(cbase^0x800,V);}
                        break;
                case 5:
                        if(V&0x40)
                         {V&=7;VRAM_BANK1(cbase^0xc00,V);}
                        else
                         {VROM_BANK1(cbase^0xc00,V);}
                        break;
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
	#include "mmc3irq.h"
 }
}

void Mapper119_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper119_write);
  MapHBIRQHook=(void *)MMC3_hb;
}

