#include "mapinc.h"

#define M44g mapbyte3[0]




static __inline void VROM_BANK1MOD(uint32 A,uint32 V) 
{
        if(M44g>=6)
        {
         V&=0xFF;
         V|=(M44g&6)<<7;
        }
        else
        {
         V&=0x7F;
         V|=M44g<<7;
        }
        VROM_BANK1(A,V);
}

static __inline void ROM_BANK8MOD(unsigned short A, byte V)
{

        if(M44g>=6)
        {
         V&=0x1f;
         V|=(M44g&6)<<4;
        }
        else
        {
         V&=0xF;
         V|=M44g<<4;
        }
        ROM_BANK8(A,V);  
}

void Mapper44_write(uint16 A,uint8 V)
{
        switch(A&0xE001){
        case 0x8000:
         if((V&0x40) != (MMC3_cmd&0x40))
         {uint32 swa;swa=PRGBankList[0];ROM_BANK8(0x8000,PRGBankList[2]);
          ROM_BANK8(0xc000,swa);}
        if(VROM_size)
         if((V&0x80) != (MMC3_cmd&0x80))
         {
            uint32 swa;
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
                case 0: V&=0xFE;VROM_BANK1MOD((cbase^0x1000),V);VROM_BANK1MOD((cbase^0x1400),V+1);break;
                case 1: V&=0xFE;VROM_BANK1MOD((cbase^0x1800),V);VROM_BANK1MOD((cbase^0x1c00),V+1);break;
                case 2: VROM_BANK1MOD(cbase^0x000,V); break;
                case 3: VROM_BANK1MOD(cbase^0x400,V); break;
                case 4: VROM_BANK1MOD(cbase^0x800,V); break;
                case 5: VROM_BANK1MOD(cbase^0xC00,V); break;
                case 6: if (MMC3_cmd&0x40) ROM_BANK8MOD(0xC000,V);
                        else ROM_BANK8MOD(0x8000,V);
                        break;
                case 7: ROM_BANK8MOD(0xA000,V);
                        break;
               }
               break;

        case 0xA000:
        MIRROR_SET(V&1);
        break;
        case 0xA001:
                    {
                    int x;
                    V&=7;
                    M44g=V;
                    for(x=0;x<4;x++)
                     ROM_BANK8MOD(0x8000+(x*8192),PRGBankList[x]);
                    if(V>=6)
                     ROM_BANK16(0xc000,pmask16);
                    for(x=0;x<8;x++)
                     VROM_BANK1MOD((x*1024),CHRBankList[x]);
                    }
                    break;

	#include "mmc3irq.h"
 }
}

void Mapper44_init(void)
{
 ROM_BANK16(0xc000,7);
 SetWriteHandler(0x8000,0xFFFF,(void *)Mapper44_write);
 MapHBIRQHook=(void *)MMC3_hb;
}
