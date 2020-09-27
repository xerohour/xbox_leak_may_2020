#include "mapinc.h"

#define TKSMIR mapbyte3
#define PPUCHRBus mapbyte2[0]
static void TKSPPU(uint32 A)
{
 static int last=-1;
 static byte z;

 A>>=13;
 PPUCHRBus=A;
 z=TKSMIR[A];

 if(z!=last)
 {
 // tksmir(z);
 //MIRROR_SET(z>>1);
  onemir(z);
  last=z;
 }
}

static __inline void tksmir(byte v)
{
onemir(TKSMIR[PPUCHRBus]);
}


void Mapper118_write(uint16 A,uint8 V)
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
            swa=TKSMIR[4];
            TKSMIR[4]=TKSMIR[0];
            TKSMIR[0]=swa;

            swa=CHRBankList[5];
            VROM_BANK1(0x1400,CHRBankList[1]);
            VROM_BANK1(0x0400,swa);
            swa=TKSMIR[5];
            TKSMIR[1]=TKSMIR[1];
            TKSMIR[1]=swa;

            swa=CHRBankList[6];
            VROM_BANK1(0x1800,CHRBankList[2]);
            VROM_BANK1(0x0800,swa);
            swa=TKSMIR[6];
            TKSMIR[6]=TKSMIR[2];
            TKSMIR[2]=swa;

            swa=CHRBankList[7];
            VROM_BANK1(0x1c00,CHRBankList[3]);
            VROM_BANK1(0x0c00,swa);
            swa=TKSMIR[7];
            TKSMIR[7]=TKSMIR[3];
            TKSMIR[3]=swa;
         }
        MMC3_cmd = V;
        cbase=((V^0x80)&0x80)<<5;
        break;

        case 0x8001:
                switch(MMC3_cmd&0x07){
                case 0:TKSMIR[(cbase>>10)^4]=TKSMIR[(cbase>>10)^5]=(V&0x80)>>6;tksmir((V&0x80)>>6);V&=0xFE;VROM_BANK1(cbase^0x1000,V);VROM_BANK1(cbase^0x1400,(V+1));break;
                case 1:TKSMIR[(cbase>>10)^6]=TKSMIR[(cbase>>10)^7]=(V&0x80)>>6;tksmir((V&0x80)>>6);V&=0xFE;VROM_BANK1(cbase^0x1800,V);VROM_BANK1(cbase^0x1C00,(V+1));break;
                case 2:TKSMIR[(cbase>>10)]=(V&0x80)>>6;tksmir((V&0x80)>>6);VROM_BANK1(cbase^0x000,V); break;
                case 3:TKSMIR[(cbase>>10)^0x1]=(V&0x80)>>6;tksmir((V&0x80)>>6);VROM_BANK1(cbase^0x400,V); break;
                case 4:TKSMIR[(cbase>>10)^0x2]=(V&0x80)>>6;tksmir((V&0x80)>>6);VROM_BANK1(cbase^0x800,V); break;
                case 5:TKSMIR[(cbase>>10)^0x3]=(V&0x80)>>6;tksmir((V&0x80)>>6);VROM_BANK1(cbase^0xC00,V); break;
                case 6:
                        if (MMC3_cmd&0x40) ROM_BANK8(0xC000,V);
                        else ROM_BANK8(0x8000,V);
                        break;
                case 7: ROM_BANK8(0xA000,V);
                        break;
               }
               break;
	#include "mmc3irq.h"
 }
}

void Mapper118_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper118_write);
  MapHBIRQHook=(void *)MMC3_hb;
  PPU_hook=(void *)TKSPPU;
}

