#include "mapinc.h"

static int32 inc;
void Mapper24_write(uint16 A,uint8 V)
{
        switch (A&0xF003){
        case 0x8000:V&=pmask16;ROM_BANK16(0x8000,V);break;
        case 0x9000:VPSG[0]=V;break;
        case 0x9001:VPSG[2]=V;break;
        case 0x9002:VPSG[3]=V;break;
        case 0xa000:VPSG[4]=V;break;
        case 0xa001:VPSG[6]=V;break;
        case 0xa002:VPSG[7]=V;break;
        case 0xb000:VPSG2[0]=V;break;
        case 0xb001:VPSG2[1]=V;break;
        case 0xb002:VPSG2[2]=V;break;
        case 0xB003:
        switch(V&0xF)
         {
         case 0x0:MIRROR_SET2(1);break;
         case 0x4:MIRROR_SET2(0);break;
         case 0x8:onemir(0);break;
         case 0xC:onemir(2);break;
         }
        break;
        case 0xC000:ROM_BANK8(0xC000,V);break;
        case 0xD000:VROM_BANK1(0x0000,V);break;
        case 0xD001:VROM_BANK1(0x0400,V);break;
        case 0xD002:VROM_BANK1(0x0800,V);break;
        case 0xD003:VROM_BANK1(0x0c00,V);break;
        case 0xE000:VROM_BANK1(0x1000,V);break;
        case 0xE001:VROM_BANK1(0x1400,V);break;
        case 0xE002:VROM_BANK1(0x1800,V);break;
        case 0xE003:VROM_BANK1(0x1c00,V);break;
        case 0xF000:IRQLatch=V;break;
        case 0xF001:IRQa=V&2;
                    vrctemp=V&1;
                    if(V&2) {IRQCount=IRQLatch;}
                    break;
        case 0xf002:IRQa=vrctemp;break;
        case 0xF003:break;
  }
}

static void KonamiIRQHook(int a)
{
  static int count=0;
  if(IRQa)
   {
    count+=(a<<1)+a;
    if(count>=341)
    {
     doagainbub:count-=341;IRQCount++;
     if(IRQCount&0x100) {count=0;TriggerIRQ();IRQCount=IRQLatch;}
     if(count>=341) goto doagainbub;
    }
 }
}

int VRC6Sound(int32 *Wave)
{
    uint8 amp;
    int32 freq;
    int V;
    if(VPSG[0x3]&0x80)
    {
     unsigned long dcycs;
     amp=VPSG[0]&15;
     freq=(((VPSG[0x2]|((VPSG[0x3]&15)<<8))+1));
     inc=(long double)((unsigned long)((SndRate OVERSAMPLE)<<12))/
((long double)PSG_base/freq);
     switch(VPSG[0]&0x70)
     {
      default:
      case 0x00:dcycs=inc>>4;break;
      case 0x10:dcycs=inc>>3;break;
      case 0x20:dcycs=(inc*3)>>4;break;
      case 0x30:dcycs=inc>>2;break;
      case 0x40:dcycs=(inc*5)>>4;break;
      case 0x50:dcycs=(inc*6)>>4;break;
      case 0x60:dcycs=(inc*7)>>4;break;
      case 0x70:dcycs=inc>>1;break;
     }
             for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
              {
               if(vcount[0]>=dcycs)
                 Wave[V>>4]-=amp;
               else
                 Wave[V>>4]+=amp;
               vcount[0]+=0x1000;
               if(vcount[0]>=inc) vcount[0]-=inc;
               }
    }
    if(VPSG[0x7]&0x80)
    {
     unsigned long dcycs;
     amp=VPSG[4]&15;
     freq=(((VPSG[0x6]|((VPSG[0x7]&15)<<8))+1));
     inc=(long double)((unsigned long)((SndRate OVERSAMPLE)<<12))/((long double)PSG_base/freq);
     switch(VPSG[4]&0x70)
     {
      default:
      case 0x00:dcycs=inc>>4;break;
      case 0x10:dcycs=inc>>3;break;
      case 0x20:dcycs=(inc*3)>>4;break;
      case 0x30:dcycs=inc>>2;break;
      case 0x40:dcycs=(inc*5)>>4;break;
      case 0x50:dcycs=(inc*6)>>4;break;
      case 0x60:dcycs=(inc*7)>>4;break;
      case 0x70:dcycs=inc>>1;break;
     }
             for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
              {
               if(vcount[1]>=dcycs)
                 Wave[V>>4]-=amp;
               else
                 Wave[V>>4]+=amp;
               vcount[1]+=0x1000;
               if(vcount[1]>=inc) vcount[1]-=inc;
               }
    }

   if(VPSG2[2]&0x80)
   {
    static long double saw1phaseacc=0;
    unsigned long freq3;
    static byte b3=0;
    static long phaseacc=0;
    static long duff;
    freq3=(VPSG2[1]+((VPSG2[2]&15)<<8)+1)<<1;
    duff=(((phaseacc>>3)&0x1f)-15);
    for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
    {
     saw1phaseacc-=(double)nesincsize;
     if(saw1phaseacc<=0)
     {
     saw1phaseacc+=freq3;
     phaseacc+=VPSG2[0]&0x3f;
     b3++;
     if(b3==7)
      {
      b3=0;
      phaseacc=0;
      }
      duff=(((phaseacc>>3)&0x1f)-15);
     }
     Wave[V>>4]+=duff;
    }
   }
 return 0;
}

void Mapper24_init(void)
{
        SetWriteHandler(0x8000,0xffff,(void *)Mapper24_write);
        MapIRQHook=(void *)KonamiIRQHook;
	MapExpSound=(void *)VRC6Sound;
}

