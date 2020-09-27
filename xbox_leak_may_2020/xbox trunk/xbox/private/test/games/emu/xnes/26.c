#include "mapinc.h"



void Mapper26_write(uint16 A,uint8 V)
{
        switch (A){
        case 0x8000:V&=pmask16;ROM_BANK16(0x8000,V);break;
        case 0x9000:VPSG[0]=V;break;
        case 0x9002:VPSG[2]=V;break;
        case 0x9001:VPSG[3]=V;break;
        case 0xa000:VPSG[4]=V;break;
        case 0xa002:VPSG[6]=V;break;
        case 0xa001:VPSG[7]=V;break;
        case 0xb000:VPSG2[0]=V;break;
        case 0xb002:VPSG2[1]=V;break;
        case 0xb001:VPSG2[2]=V;break;

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
        case 0xD001:VROM_BANK1(0x0800,V);break;
        case 0xD002:VROM_BANK1(0x0400,V);break;
        case 0xD003:VROM_BANK1(0x0c00,V);break;
        case 0xE000:VROM_BANK1(0x1000,V);break;
        case 0xE001:VROM_BANK1(0x1800,V);break;
        case 0xE002:VROM_BANK1(0x1400,V);break;
        case 0xE003:VROM_BANK1(0x1c00,V);break;
        case 0xF000:IRQLatch=V;break;
        case 0xF002:IRQa&=1;
                    IRQa|=V&2;
                    vrctemp=V&1;
                    if(V&2) {IRQCount=IRQLatch;}
                    break;
        case 0xf001:IRQa&=2;IRQa|=vrctemp;break;
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

void Mapper26_init(void)
{
        SetWriteHandler(0x8000,0xffff,(void *)Mapper26_write);
        MapIRQHook=(void *)KonamiIRQHook;
	MapExpSound=(void *)VRC6Sound;
}

