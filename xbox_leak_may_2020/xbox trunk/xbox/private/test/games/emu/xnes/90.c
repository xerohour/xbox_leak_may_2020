#include "mapinc.h"


#define tkcom1 mapbyte1[1]
#define tkcom2 mapbyte1[2]

#define tklist1 mapbyte2
#define tklist2 mapbyte3
#define tklist3 mapbyte4

byte tekker=0x80;

byte tekread(word A)
{
 return tekker;
}
static void tekprom(void)
{
 switch(tkcom1&3)
  {
   case 1:              // 16 KB
          ROM_BANK16(0x8000,tklist1[0]);
          ROM_BANK16(0xC000,tklist1[2]);
          break;

   case 2:              //2 = 8 KB ??
   case 3:
          ROM_BANK8(0x8000,tklist1[0]);
          ROM_BANK8(0xa000,tklist1[1]);
          ROM_BANK8(0xc000,tklist1[2]);
          ROM_BANK8(0xe000,tklist1[3]);
          break;
  }
}
static void tekvrom(void)
{
 switch(tkcom1&0x18)
  {
   case 0x00:      // 8KB
           VROM_BANK8((tklist1[4]|(tklist2[4]<<8)));
   break;
   case 0x08:      // 4KB
          VROM_BANK4(0x0000,(tklist1[4]|(tklist2[4]<<8)));
          VROM_BANK4(0x1000,(tklist2[0]|(tklist3[0]<<8)));
   break;
   case 0x10:      // 2KB
          VROM_BANK2(0x0000,(tklist1[4]|(tklist2[4]<<8)));
          VROM_BANK2(0x0800,(tklist1[6]|(tklist2[6]<<8)));
          VROM_BANK2(0x1000,(tklist2[0]|(tklist3[0]<<8)));
          VROM_BANK2(0x0800,(tklist2[2]|(tklist3[2]<<8)));
   break;
   case 0x18:      // 1KB
           VROM_BANK1(0x0000,(tklist1[4]|(tklist2[4]<<8)));
           VROM_BANK1(0x0400,(tklist1[5]|(tklist2[5]<<8)));
           VROM_BANK1(0x0800,(tklist1[6]|(tklist2[6]<<8)));
           VROM_BANK1(0x0c00,(tklist1[7]|(tklist2[7]<<8)));
           VROM_BANK1(0x1000,(tklist2[0]|(tklist3[0]<<8)));
           VROM_BANK1(0x1400,(tklist2[1]|(tklist3[1]<<8)));
           VROM_BANK1(0x1800,(tklist2[2]|(tklist3[2]<<8)));
           VROM_BANK1(0x1c00,(tklist2[3]|(tklist3[3]<<8)));
   break;
 }
}

void Mapper90_write(uint16 A,uint8 V)
{
switch(A&0xF007)
 {
   case 0x8000:tklist1[0]=V;tekprom();break;
   case 0x8001:tklist1[1]=V;tekprom();break;
   case 0x8002:tklist1[2]=V;tekprom();break;
   case 0x8003:tklist1[3]=V;tekprom();break;
   case 0x9000:tklist1[4]=V;tekvrom();break;
   case 0x9001:tklist1[5]=V;tekvrom();break;
   case 0x9002:tklist1[6]=V;tekvrom();break;
   case 0x9003:tklist1[7]=V;tekvrom();break;
   case 0x9004:tklist2[0]=V;tekvrom();break;
   case 0x9005:tklist2[1]=V;tekvrom();break;
   case 0x9006:tklist2[2]=V;tekvrom();break;
   case 0x9007:tklist2[3]=V;tekvrom();break;
   case 0xa000:tklist2[4]=V;tekvrom();break;
   case 0xa001:tklist2[5]=V;tekvrom();break;
   case 0xa002:tklist2[6]=V;tekvrom();break;
   case 0xa003:tklist2[7]=V;tekvrom();break;
   case 0xa004:tklist3[0]=V;tekvrom();break;
   case 0xa005:tklist3[1]=V;tekvrom();break;
   case 0xa006:tklist3[2]=V;tekvrom();break;
   case 0xa007:tklist3[3]=V;tekvrom();break;
   case 0xb000:tklist3[4]=V;break;
   case 0xb001:tklist3[5]=V;break;
   case 0xb002:tklist3[6]=V;break;
   case 0xb003:tklist3[7]=V;break;
   case 0xc004:
   case 0xc000:IRQLatch=V;break;

   case 0xc005:
   case 0xc001:IRQlow&=~1;
               IRQCount=V;break;
   case 0xc006:
   case 0xc002:IRQlow&=~1;
               IRQa=0;
               IRQCount=IRQLatch;
               break;
   case 0xc007:
   case 0xc003:IRQa=1;break;

   case 0xd000:tkcom1=V;break;
   case 0xd001:switch(V&3){
                  case 0x00:MIRROR_SET2(1);break;
                  case 0x01:MIRROR_SET2(0);break;
                  case 0x02:onemir(0);break;
                  case 0x03:onemir(2);break;
               }
	       break;
   break;
 }
}
static void Mapper90_hb(void)
{
 if(IRQa)
 {
        if(scanline<241 && (SpriteON || ScreenON) && IRQCount) 
	{
	 IRQCount--;
         if(!IRQCount)
          {
	  IRQlow|=1;
          IRQCount=IRQLatch;
          }
        }
 }
}

void Mapper90_init(void)
{
  tekker^=0x80;
  SetWriteHandler(0x8000,0xffff,(void *)Mapper90_write);
  SetReadHandler(0x5000,0x5000,(void *)tekread);
  MapHBIRQHook=(void *)Mapper90_hb;
}

