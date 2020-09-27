#include "mapinc.h"


static void Map47boink(uint16 A, uint8 V)
{
 int x;
 mapbyte2[0]=V&1;
 for(x=0;x<8;x++)
  VROM_BANK1(x*1024,(CHRBankList[x]&0x7F)|(mapbyte2[0]<<7));
 for(x=0;x<4;x++)
  ROM_BANK8(0x8000+x*8192,(PRGBankList[x]&0xF)|(mapbyte2[0]<<4));
}
void Mapper47_write(uint16 A,uint8 V)
{        
        switch(A&0xE001)
        {
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
                case 0: V>>=1;VROM_BANK2((cbase^0x1000),(V&0x3f)|(mapbyte2[0]<<6));break;
                case 1: V>>=1;VROM_BANK2((cbase^0x1800),(V&0x3f)|(mapbyte2[0]<<6));break;
                case 2: VROM_BANK1(cbase^0x000,(V&0x7f)|(mapbyte2[0]<<7)); break;
                case 3: VROM_BANK1(cbase^0x400,(V&0x7f)|(mapbyte2[0]<<7)); break;
                case 4: VROM_BANK1(cbase^0x800,(V&0x7f)|(mapbyte2[0]<<7)); break;
                case 5: VROM_BANK1(cbase^0xC00,(V&0x7f)|(mapbyte2[0]<<7)); break;
                case 6: if (MMC3_cmd&0x40) ROM_BANK8(0xC000,(V&0xF)|(mapbyte2[0]<<4));
                        else ROM_BANK8(0x8000,(V&0xF)|(mapbyte2[0]<<4));
                        break;
                case 7: ROM_BANK8(0xA000,(V&0xF)|(mapbyte2[0]<<4));
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

void Mapper47_init(void)
{
 ROM_BANK16(0xC000,0x7);
 SetWriteHandler(0x6000,0x6000,(void *)Map47boink);
 SetWriteHandler(0x8000,0xFFFF,(void *)Mapper47_write);
 MapHBIRQHook=(void *)MMC3_hb;
}
