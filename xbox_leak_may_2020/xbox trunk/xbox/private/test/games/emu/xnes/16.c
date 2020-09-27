#include "mapinc.h"


void BandaiIRQHook(int a)
{
  if(IRQa)
  {
   IRQCount-=a;
   if(IRQCount<0)
   {
    TriggerIRQ();
    IRQa=0;
    IRQCount=0xFFFF;
   }
  }
}


void Mapper16_write(uint16 A,uint8 V)
{
        switch(A&0xF){
        case 0x0: VROM_BANK1(0x0000,V);break;
        case 0x1: VROM_BANK1(0x0400,V);break;
        case 0x2: VROM_BANK1(0x0800,V);break;
        case 0x3: VROM_BANK1(0x0C00,V);break;
        case 0x4: VROM_BANK1(0x1000,V);break;
        case 0x5: VROM_BANK1(0x1400,V);break;
        case 0x6: VROM_BANK1(0x1800,V);break;
        case 0x7: VROM_BANK1(0x1C00,V);break;
        case 0x8: ROM_BANK16(0x8000,V);break;
        case 0x9: switch(V&3){
                  case 0x00:MIRROR_SET2(1);break;
                  case 0x01:MIRROR_SET2(0);break;
                  case 0x02:onemir(0);break;
                  case 0x03:onemir(2);break;
                  }
                  break;
        case 0xA:IRQa=V&1;break;
        case 0xB:
                 {IRQCount&=0xFF00;IRQCount|=V;}break;
        case 0xC:
                 {IRQCount&=0xFF;IRQCount|=V<<8;}break;
        case 0xD:break;/* EEPROM control port */
        }
}

void Mapper16_init(void)
{
MapIRQHook=(void *)BandaiIRQHook;
SetWriteHandler(0x4020,0xFFFF,(void *)Mapper16_write);
}

