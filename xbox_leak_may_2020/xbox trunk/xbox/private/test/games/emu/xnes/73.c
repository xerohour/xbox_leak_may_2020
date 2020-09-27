#include "mapinc.h"



void Mapper73_write(uint16 A,uint8 V)
{
switch(A&0xF000)
 {
 case 0x8000:IRQCount&=0xFFF0;IRQCount|=(V&0xF);break;
 case 0x9000:IRQCount&=0xFF0F;IRQCount|=(V&0xF)<<4;break;
 case 0xa000:IRQCount&=0xF0FF;IRQCount|=(V&0xF)<<8;break;
 case 0xb000:IRQCount&=0x0FFF;IRQCount|=(V&0xF)<<12;break;
 case 0xc000:IRQa=V&2;break;
 case 0xf000:ROM_BANK16(0x8000,V);break;
 }
}
static void Mapper73_hb(void)
{
 if(IRQa)
 {
        if(IRQCount>=0xFFFF)
                {
                IRQCount&=0xFFFF;
                IRQa=0;
                TriggerIRQ();
                }
                else
                 IRQCount+=(M.IPeriod+85)/3;
 }
}

void Mapper73_init(void)
{
SetWriteHandler(0x8000,0xffff,(void *)Mapper73_write);
MapHBIRQHook=(void *)Mapper73_hb;
}

