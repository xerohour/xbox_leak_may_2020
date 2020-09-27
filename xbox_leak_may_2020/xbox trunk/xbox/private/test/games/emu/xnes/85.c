#include "mapinc.h"

#pragma warning( disable : 4013 )

static void VROM_BANK1VRC7(uint32 A, uint32 V)
{
 if(!VROM_size){  V&=7;CHRBankList[(A)>>10]=V;VPage[(A)>>10]=
&VRAM[(V)<<10]-(A);VPAL[(A)>>10]=1;}else{V&=vmask1;
CHRBankList[(A)>>10]=V;VPage[(A)>>10]=&VROM[(V)<<10]-(A);}
}


void vrc7translate(byte A, byte V);
void UpdateOPL(int32 *d);
void Mapper85_write(uint16 A,uint8 V)
{
static byte indox=0;
A|=(A&8)<<1;
switch(A&0xF030)
        {
        case 0x8000:ROM_BANK8(0x8000,V);break;
        case 0x8010:ROM_BANK8(0xa000,V);break;
        case 0x9000:ROM_BANK8(0xc000,V);break;
        case 0x9010:indox=V;break;
        case 0x9030:vrc7translate(indox,V);MapExpSound=(void *)VRC7Sound;break;
        case 0xa000:VROM_BANK1VRC7(0x000,V);break;
        case 0xa010:VROM_BANK1VRC7(0x400,V);break;
        case 0xb000:VROM_BANK1VRC7(0x800,V);break;
        case 0xb010:VROM_BANK1VRC7(0xc00,V);break;
        case 0xc000:VROM_BANK1VRC7(0x1000,V);break;
        case 0xc010:VROM_BANK1VRC7(0x1400,V);break;
        case 0xd000:VROM_BANK1VRC7(0x1800,V);break;
        case 0xd010:VROM_BANK1VRC7(0x1c00,V);break;
       case 0xe000:
        switch(V&3)
        {
        case 0:MIRROR_SET2(1);break;
        case 1:MIRROR_SET2(0);break;
        case 2:onemir(0);break;
        case 3:onemir(2);break;
        }
        break;
        case 0xE010:
                    IRQLatch=V;
                    break;
        case 0xF000:
                    IRQa=V&2;
                    vrctemp=V&1;
                    if(V&2) {IRQCount=IRQLatch;}
                    break;
        case 0xf010:if(vrctemp) IRQa=1;
                    else IRQa=0;
                    break;
        }
}

static void KonamiIRQHook(int a)
{
  static int count=0;
  if(IRQa)
   {
    count+=(a<<1)+a;
    if(count>=342)
    {
     doagainbub:count-=342;IRQCount++;
     if(IRQCount&0x100) {count=0;TriggerIRQ();IRQCount=IRQLatch;}
     if(count>=342) goto doagainbub;
    }
 }
}
void Mapper85_StateRestore(int version)
{
 LoadOPL();

}
int VRC7Sound(int32 *Wave)
{
 UpdateOPL(Wave);
 return 0;
}

void Mapper85_init(void)
{
  MapIRQHook=(void *)KonamiIRQHook;
  SetWriteHandler(0x8000,0xffff,(void *)Mapper85_write);
  MapStateRestore=(void *)Mapper85_StateRestore;
}
