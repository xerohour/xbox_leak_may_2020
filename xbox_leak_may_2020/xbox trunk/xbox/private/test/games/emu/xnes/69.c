#include "mapinc.h"


#define sunselect mapbyte1[0]
#define sungah    mapbyte1[1]
#define sunindex  mapbyte1[2]

static int32 inc;
void SUN5BWRAM(register word A, register byte V)
{
Page[3][A]=V;
}

byte SUN5AWRAM(register word A)
{
if(sungah&0x40 && (!(sungah&0x80)))
 return 0xAF;                   // I *should* return what's on the bus,
                                // since $6000-$7fff is in an open bus state
 return Page[A>>13][A];
}


void Mapper69_write(uint16 A,uint8 V)
{
switch(A&0xE000)
 {
 case 0xC000:sunindex=V%14;break;
 case 0xE000:MapExpSound=(void *)AYSound;
             if(sunindex==0xD)
             {
              byte tab[16]={15,15,15,15,0,0,0,0,15,15,15,15,0,0,0,0};
              MapperExRAM[0x20]=tab[V&15];
             }
             else if(sunindex==0x6) znreg=0xFFFF;
             MapperExRAM[sunindex]=V;
             break;
 case 0x8000:sunselect=V;break;
 case 0xa000:
             switch(sunselect&0x0f)
             {
             case 0:VROM_BANK1(0x000,V);break;
             case 1:VROM_BANK1(0x400,V);break;
             case 2:VROM_BANK1(0x800,V);break;
             case 3:VROM_BANK1(0xc00,V);break;
             case 4:VROM_BANK1(0x1000,V);break;
             case 5:VROM_BANK1(0x1400,V);break;
             case 6:VROM_BANK1(0x1800,V);break;
             case 7:VROM_BANK1(0x1c00,V);break;
             case 8:
                        sungah=V;
                        if(V&0x40)
                        {
                         if(V&0x80) // Select WRAM
                         {
                          Page[3]=WRAM-0x6000;
                         }
                         else       //
                         {
                          Page[3]=nothing-0x6000;
                         }
                        }
                        else ROM_BANK8(0x6000,V);break;
             case 9:ROM_BANK8(0x8000,V);break;
             case 0xa:ROM_BANK8(0xa000,V);break;
             case 0xb:ROM_BANK8(0xc000,V);break;
             case 0xc:
                    switch(V&3)
                    {
                    case 0:MIRROR_SET2(1);break;
                    case 1:MIRROR_SET2(0);break;
                    case 2:onemir(0);break;
                    case 3:onemir(2);break;
                    }
             break;
             case 0xd:IRQa=V;break;
             case 0xe:IRQCount&=0xFF00;IRQCount|=V;break;
             case 0xf:IRQCount&=0x00FF;IRQCount|=V<<8;break;
             }
             break;
 }
}

int AYSound(int32 *Wave)
{
    int x,V;
    uint32 freq;
    unsigned char amp;

    for(x=0;x<3;x++)
    {
     if(!(MapperExRAM[0x7]&(1<<x)))
     {
      long vcoo;

      freq=(MapperExRAM[x<<1]|((MapperExRAM[(x<<1)+1]&15)<<8))+1;
      inc=(long double)((unsigned long)((SndRate OVERSAMPLE)<<12))/
        ((long double)PSG_base/freq);
      if(MapperExRAM[0x8+x]&0x10) amp=MapperExRAM[0x20]&15;
                           else   amp=MapperExRAM[0x8+x]&15;
              amp-=amp>>1;
              vcoo=vcount[x];
             if(amp)
              for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
               {
                if(vcoo>=inc>>1)
                  Wave[V>>4]-=amp;
                else
                  Wave[V>>4]+=amp;
                vcoo+=0x1000;
                if(vcoo>=inc) vcoo-=inc;
                }
              vcount[x]=vcoo;
     }
    }

        amp=0;
        for(V=0;V<3;V++)
         {
          if(!(MapperExRAM[0x7]&(8<<V)))
          { if(MapperExRAM[0x8+V]&0x10) amp+=MapperExRAM[0x20]&15;
                                else  amp+=MapperExRAM[0x8+V]&15;
          }
         }
     amp-=amp>>1;
     if(amp)
       {
        freq=PSG_base/(MapperExRAM[0x6]+1);
        if(freq>44100)
         inc=((freq<<11)/(SndRate OVERSAMPLE))<<4;
        else
         inc=(freq<<15)/(SndRate OVERSAMPLE);

         for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
          {
             static int mixer;

             if(vcount[3]>=32768)
             {
               unsigned char feedback;
               mixer=0;
               if(znreg&1) mixer+=amp;
               else mixer-=amp;
               feedback=((znreg>>13)&1)^((znreg>>14)&1);
               znreg=(znreg<<1)+(feedback);
               vcount[3]-=32768;
             }
             Wave[V>>4]+=mixer;
             vcount[3]+=inc;
           }
       }
        #ifdef moo
        vcount[4]+=cycles_per_update;
        if(vcount[4]>=((MapperExRAM[0xC]|(MapperExRAM[0xB]<<8))<<8))
        {
        vcount[4]-=(MapperExRAM[0xC]|(MapperExRAM[0xB]<<8))<<8;
        }
        #endif

 return 0;
}
static void SunIRQHook(int a)
{
  if(IRQa)
  {
   IRQCount-=a;
   if(IRQCount<=0)
   {TriggerIRQ();IRQa=0;IRQCount=0xFFFF;}
  }
}

void Mapper69_StateRestore(int version)
{
   if(version>=19)
   {
     if(mapbyte1[1]&0x40)
     {
     if(mapbyte1[1]&0x80) // Select WRAM
      Page[3]=WRAM-0x6000;
     else       //
      Page[3]=nothing-0x6000;
     }
    else ROM_BANK8(0x6000,(mapbyte1[1]&pmask8));
   }
   else
    mapbyte1[1]=0xC0;
}

void Mapper69_init(void)
{
SetWriteHandler(0x8000,0xffff,(void *)Mapper69_write);
SetWriteHandler(0x6000,0x7fff,(void *)SUN5BWRAM);
SetReadHandler(0x6000,0x7fff,(void *)SUN5AWRAM);
MapIRQHook=(void *)SunIRQHook;
MapStateRestore=(void *)Mapper69_StateRestore;
}

