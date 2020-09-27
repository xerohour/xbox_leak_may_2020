#include "mapinc.h"


static void latchcheck(uint32 VAddra)
{
 pair vap;
 vap.W=VAddra;

if(vap.B.h>=0x20 || ((vap.B.h&0xF)!=0xf)) return;
else if(vap.B.h<0x10)
     {
        if((vap.B.l&0xF0)==0xD0)
         {
         VROM_BANK4(0x0000,MMC4reg[0]);
         latcha1=0xFD;
         }
        else if((vap.B.l&0xF0)==0xE0)
         {
         VROM_BANK4(0x0000,MMC4reg[1]);
         latcha1=0xFE;
         }
     }
else
     {
        if((vap.B.l&0xF0)==0xD0)
         {
         VROM_BANK4(0x1000,MMC4reg[2]);
         latcha2=0xFD;
         }
        else if((vap.B.l&0xF0)==0xE0)
         {
         VROM_BANK4(0x1000,MMC4reg[3]);
         latcha2=0xFE;
         }
     }
}

void Mapper9_write(uint16 A,uint8 V)
{
        switch(A&0xF000){
        case 0xA000:
                ROM_BANK8(0x8000,V);
                break;
        case 0xB000:
                V&=vmask4;
                if (latcha1==0xFD) { VROM_BANK4(0x0000,V);}
                MMC4reg[0]=V;
                break;
        case 0xC000:
                V&=vmask4;
                if (latcha1==0xFE) {VROM_BANK4(0x0000,V);}
                MMC4reg[1]=V;
                break;
        case 0xD000:
                V&=vmask4;
                if (latcha2==0xFD) {VROM_BANK4(0x1000,V);}
                MMC4reg[2]=V;
                break;
        case 0xE000:
                V&=vmask4;
                if (latcha2==0xFE) {VROM_BANK4(0x1000,V);}
                MMC4reg[3]=V;
                break;
        case 0xF000:
                MIRROR_SET(V&1);
                break;
        }
}

void Mapper9_init(void)
{
        latcha1=0xFE;
        latcha2=0xFE;
        ROM_BANK8(0xA000,pmask8&~2);
        ROM_BANK8(0x8000,0);
        SetWriteHandler(0x8000,0xFFFF,(void *)Mapper9_write);
        PPU_hook=(void *)latchcheck;
}

