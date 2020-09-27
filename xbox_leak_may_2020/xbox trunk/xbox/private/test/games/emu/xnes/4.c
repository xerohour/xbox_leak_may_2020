#include "mapinc.h"


void Mapper4_write(uint16 A,uint8 V)
{
        switch(A&0xE001){
        case 0x8000:
         if((V&0x40) != (MMC3_cmd&0x40))
         {byte swa;swa=PRGBankList[0];ROM_BANK8(0x8000,PRGBankList[2]);
          ROM_BANK8(0xc000,swa);}
        if(VROM_size)
         if((V&0x80) != (MMC3_cmd&0x80))
         {
            byte swa;
            swa=CHRBankList[4];
            VROM_BANK1(0x1000,CHRBankList[0]);
            VROM_BANK1(0x0000,swa);
            swa=CHRBankList[5];
            VROM_BANK1(0x1400,CHRBankList[1]);
            VROM_BANK1(0x0400,swa);
            swa=CHRBankList[6];
            VROM_BANK1(0x1800,CHRBankList[2]);
            VROM_BANK1(0x0800,swa);
            swa=CHRBankList[7];
            VROM_BANK1(0x1c00,CHRBankList[3]);
            VROM_BANK1(0x0c00,swa);
         }
        MMC3_cmd = V;
        cbase=((V^0x80)&0x80)<<5;
        break;

        case 0x8001:
                switch(MMC3_cmd&0x07){
                case 0: V>>=1;VROM_BANK2((cbase^0x1000),V);break;
                case 1: V>>=1;VROM_BANK2((cbase^0x1800),V);break;
                case 2: VROM_BANK1(cbase^0x000,V); break;
                case 3: VROM_BANK1(cbase^0x400,V); break;
                case 4: VROM_BANK1(cbase^0x800,V); break;
                case 5: VROM_BANK1(cbase^0xC00,V); break;
                case 6: if (MMC3_cmd&0x40) ROM_BANK8(0xC000,V);
                        else ROM_BANK8(0x8000,V);
                        break;
                case 7: ROM_BANK8(0xA000,V);
                        break;
               }
               break;

        case 0xA000:
        MIRROR_SET(V&1);
        break;
        case 0xA001:break;
	#include "mmc3irq.h"
 }
}

void MMC3_hb(void)
{
     if((ScreenON || SpriteON)&& (scanline<241))
     {
      if(IRQCount>=0)
      {
       IRQCount--;resetmode=0;
	if(IRQCount<0)
        {
         if(IRQa)
         {
			resetmode = 1;
			TriggerIRQSync();
			IRQlow |= 1;
         }
        }
      }
     }
}
void Mapper4_StateRestore(int version)
{
if(version<=19)
 {
  if(PRGBankList[2]!=(pmask8-1))
   {
    mapbyte1[1]|=0x40;
   }
  if(mapword1[0]==0x0000)
   mapbyte1[1]|=0x80;
 }
}

void Mapper4_init(void)
{
 SetWriteHandler(0x8000,0xFFFF,(void *)Mapper4_write);
 MapHBIRQHook=(void *)MMC3_hb;
 MapStateRestore=(void *)Mapper4_StateRestore;
}
