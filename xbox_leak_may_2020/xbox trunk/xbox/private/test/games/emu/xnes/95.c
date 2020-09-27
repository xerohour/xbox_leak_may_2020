#include "mapinc.h"


#define dbarray mapbyte1
void dragonbust_ppu(uint32 A)
{
 static int last=-1;
 static byte z;
 A>>=13;

 z=dbarray[A];

 if(z!=last)
 {
  onemir(z);
  last=z;
 }
}


void Mapper95_write(uint16 A,uint8 V)
{
        switch(A&0xF001){

        case 0x8000:
        MMC3_cmd = V;
        break;

        case 0x8001:
                switch(MMC3_cmd&7){
                case 0: dbarray[0]=dbarray[1]=(V&0x20)>>4;onemir((V&0x20)>>4);V>>=1;VROM_BANK2(0x0000,V);break;
                case 1: dbarray[2]=dbarray[3]=(V&0x20)>>4;onemir((V&0x20)>>4);V>>=1;V&=vmask2;VROM_BANK2(0x0800,V);break;
                case 2: dbarray[4]=(V&0x20)>>4;onemir((V&0x20)>>4);VROM_BANK1(0x1000,V); break;
                case 3: dbarray[5]=(V&0x20)>>4;onemir((V&0x20)>>4);VROM_BANK1(0x1400,V); break;
                case 4: dbarray[6]=(V&0x20)>>4;onemir((V&0x20)>>4);VROM_BANK1(0x1800,V); break;
                case 5: dbarray[7]=(V&0x20)>>4;onemir((V&0x20)>>4);VROM_BANK1(0x1C00,V); break;
                case 6:
                        ROM_BANK8(0x8000,V);
                        break;
                case 7:
                        ROM_BANK8(0xA000,V);
                        break;
                }
                break;
}
}

void Mapper95_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper95_write);
  PPU_hook=(void *)dragonbust_ppu;
}

