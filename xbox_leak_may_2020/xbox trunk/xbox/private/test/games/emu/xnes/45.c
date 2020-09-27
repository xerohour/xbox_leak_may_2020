#include "mapinc.h"


#define MQW mapbyte4
#define MQP mapbyte4[5]


static int atable[16]={0,0,0,0,0,0,0,0,1,3,7,0xF,0x1F,0x3F,0x7F,0xFF};

static __inline void VROM_BANK1MOD(uint32 A,uint32 V)
{
 V&=atable[MQW[2]&0xF];
 V|=MQW[0];
 V+=(MQW[2]&0x10)<<4;
 VROM_BANK1(A,V);

}

static __inline void ROM_BANK8MOD(unsigned short A, byte V)
{
 V&=(MQW[3]&0x3F)^0xFF;
 V&=0x3F;
 V|=MQW[1];
 ROM_BANK8(A,V);
}


void Mapper45_q(uint16 A, uint8 V)
{
 int x;

 MQW[MQP]=V;
 MQP=(MQP+1)&3;

 for(x=0;x<4;x++)
  ROM_BANK8MOD(0x8000+x*8192,mapbyte2[x]);
 for(x=0;x<8;x++)
  VROM_BANK1MOD(0x0000+x*1024,mapbyte3[x]);

}


void Mapper45_write(uint16 A,uint8 V)
{
        switch(A&0xE001){
        case 0x8000:
         if((V&0x40) != (MMC3_cmd&0x40))
         {
          byte swa;
          swa=PRGBankList[0];
          ROM_BANK8(0x8000,PRGBankList[2]);
          ROM_BANK8(0xc000,swa);
          swa=mapbyte2[0];
          mapbyte2[0]=mapbyte2[2];
          mapbyte2[2]=swa;
         }
        if(VROM_size)
         if((V&0x80) != (MMC3_cmd&0x80))
         {
            int x;
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
                
            for(x=0;x<4;x++)
            {
             swa=mapbyte3[4+x];
             mapbyte3[4+x]=mapbyte3[x];
             mapbyte3[x]=swa;
            }
         }
        MMC3_cmd = V;
        cbase=((V^0x80)&0x80)<<5;
        break;

        case 0x8001:
                switch(MMC3_cmd&0x07){
                case 0: V&=0xFE;mapbyte3[(cbase>>10)^0x4]=V;mapbyte3[(cbase>>10)^0x5]=V+1;VROM_BANK1MOD((cbase^0x1000),V);VROM_BANK1MOD((cbase^0x1400),V+1);break;
                case 1: V&=0xFE;mapbyte3[(cbase>>10)^0x6]=V;mapbyte3[(cbase>>10)^0x7]=V+1;VROM_BANK1MOD((cbase^0x1800),V);VROM_BANK1MOD((cbase^0x1c00),V+1);break;
                case 2: VROM_BANK1MOD(cbase^0x000,V);mapbyte3[(cbase>>10)]=V;break;
                case 3: VROM_BANK1MOD(cbase^0x400,V);mapbyte3[(cbase>>10)^1]=V;break;
                case 4: VROM_BANK1MOD(cbase^0x800,V);mapbyte3[(cbase>>10)^2]=V;break;
                case 5: VROM_BANK1MOD(cbase^0xC00,V);mapbyte3[(cbase>>10)^3]=V;break;
                case 6: 
                        if (MMC3_cmd&0x40) {mapbyte2[2]=V&0x3f;ROM_BANK8MOD(0xC000,V);}
                        else {mapbyte2[0]=V&0x3f;ROM_BANK8MOD(0x8000,V);}
                        break;
                case 7: mapbyte2[1]=V&0x3f;
                        ROM_BANK8MOD(0xA000,V);
                        break;
               }
               break;

        case 0xA000:
        MIRROR_SET(V&1);
        break;

        case 0xc000:IRQLatch=V;IRQlow&=~1;
                    if(resetmode==1)
                     {IRQCount=IRQLatch;}
                    break;
        case 0xc001:resetmode=1;IRQlow&=~1;
                    IRQCount=IRQLatch;
                    break;
        case 0xE000:IRQa=0;IRQlow&=~1;
                    if(resetmode==1)
                     IRQCount=IRQLatch;
                    break;
        case 0xE001:IRQa=1;IRQlow&=~1;
                    if(resetmode==1)
                     IRQCount=IRQLatch;
                    break;



 }
}

void Mapper45_init(void)
{
 int x;
 for(x=0;x<8;x++)
  mapbyte3[x]=x;
 mapbyte2[0]=0;
 mapbyte2[1]=1;
 mapbyte2[2]=pmask8-1;
 mapbyte2[3]=pmask8;

 SetWriteHandler(0x6000,0x6000,(void *)Mapper45_q);
 SetWriteHandler(0x8000,0xFFFF,(void *)Mapper45_write);
 MapHBIRQHook=(void *)MMC3_hb;
}
