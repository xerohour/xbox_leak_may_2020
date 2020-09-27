#include "mapinc.h"



#define MMC1_reg mapbyte1
#define MMC1_buf mapbyte2[0]
#define MMC1_sft mapbyte3[0]
#define lastn    mapbyte2[1]

static void MMC1CHR(void)
{
    if(MMC1_reg[0]&0x10)
    {
     VROM_BANK4(0x0000,MMC1_reg[1]);
     VROM_BANK4(0x1000,MMC1_reg[2]);
    }
    else
    {
     VROM_BANK4(0x0000,MMC1_reg[1]);
     VROM_BANK4(0x1000,(MMC1_reg[1]+1));
    }
}

static void MMC1PRG(void)
{
        uint8 offs;

        offs=MMC1_reg[1]&0x10;
        switch(MMC1_reg[0]&0xC)
        {
          case 0xC: ROM_BANK16(0x8000,(MMC1_reg[3]+offs));
                    ROM_BANK16(0xC000,((pmask16&0xF)+offs));
                    break;
          case 0x8: ROM_BANK16(0xC000,(MMC1_reg[3]+offs));
                    ROM_BANK16(0x8000,offs);
                    break;
          case 0x0:
          case 0x4:
                    ROM_BANK16(0x8000,(MMC1_reg[3]+offs));
                    ROM_BANK16(0xc000,(MMC1_reg[3]+offs+1));
                    break;
        }
}

void Mapper1_write(uint16 A,uint8 V)
{
        int n=(A>>13)-4;

        if (V&0x80) {
         MMC1_sft=MMC1_buf=0;
//         if(n==0) MMC1_reg[0]|=0xC;
         return;
        }

        if(lastn!=n)
        {
         MMC1_sft=MMC1_buf=0;
        }
        lastn=n;

        MMC1_buf|=(V&1)<<(MMC1_sft++);

  if (MMC1_sft==5) {
        if(n==3) V&=0xF;
        else     V&=0x1F;

        MMC1_reg[n]=V=MMC1_buf;
        MMC1_sft = MMC1_buf=0;
        switch(n){
        case 0:
                switch(V&3)
                {
                 case 2: MIRROR_SET(0);break;
                 case 3: MIRROR_SET(1);break;
                 case 0: onemir(0);break;
                 case 1: onemir(2);break;
                }
                MMC1CHR();
                MMC1PRG();
                break;
        case 1:
                MMC1CHR();
                MMC1PRG();
                break;
        case 2:
                MMC1CHR();
                break;
        case 3:
                MMC1PRG();
                break;
        }
  }
}


void Mapper1_init(void)
{
        int i;
        for(i=0;i<4;i++) MMC1_reg[i]=0;
        MMC1_sft = MMC1_buf =0;
        MMC1_reg[0]=0xC;
        ROM_BANK16(0xC000,(pmask16&15)); /* last 16K */
	SetWriteHandler(0x8000,0xFFFF,(void *)Mapper1_write);
}

