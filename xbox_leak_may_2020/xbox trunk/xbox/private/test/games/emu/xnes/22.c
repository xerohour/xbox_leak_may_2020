#include "mapinc.h"

#define K4buf mapbyte2



void Mapper22_write(uint16 A,uint8 V)
{
        if(A<=0xAFFF)
         {
          switch(A&0xF000)
          {
          case 0x8000:ROM_BANK8(0x8000,V);break;
          case 0xa000:ROM_BANK8(0xA000,V);break;
          case 0x9000:switch(V&3){
                      case 0x00:MIRROR_SET2(1);break;
                      case 0x01:MIRROR_SET2(0);break;
                      case 0x02:onemir(0);break;
                      case 0x03:onemir(2);break;
                      }
                      break;
          }
         }
        else
         {
          switch(A&0xF003){
           case 0xb000:K4buf[0]&=0xF0;K4buf[0]|=V&0x0F;VROM_BANK1(0x000,((K4buf[0]>>1)));break;
           case 0xb002:K4buf[0]&=0x0F;K4buf[0]|=V<<4;VROM_BANK1(0x000,((K4buf[0]>>1)));break;
           case 0xb001:K4buf[1]&=0xF0;K4buf[1]|=V&0x0F;VROM_BANK1(0x400,((K4buf[1]>>1)));break;
           case 0xb003:K4buf[1]&=0x0F;K4buf[1]|=V<<4;VROM_BANK1(0x400,((K4buf[1]>>1)));break;
           case 0xc000:K4buf[2]&=0xF0;K4buf[2]|=V&0x0F;VROM_BANK1(0x800,((K4buf[2]>>1)));break;
           case 0xc002:K4buf[2]&=0x0F;K4buf[2]|=V<<4;VROM_BANK1(0x800,((K4buf[2]>>1)));break;
           case 0xc001:K4buf[3]&=0xF0;K4buf[3]|=V&0x0F;VROM_BANK1(0xc00,((K4buf[3]>>1)));break;
           case 0xc003:K4buf[3]&=0x0F;K4buf[3]|=V<<4;VROM_BANK1(0xc00,((K4buf[3]>>1)));break;
           case 0xd000:K4buf[4]&=0xF0;K4buf[4]|=V&0x0F;VROM_BANK1(0x1000,((K4buf[4]>>1)));break;
           case 0xd002:K4buf[4]&=0x0F;K4buf[4]|=V<<4;VROM_BANK1(0x1000,((K4buf[4]>>1)));break;
           case 0xd001:K4buf[5]&=0xF0;K4buf[5]|=V&0x0F;VROM_BANK1(0x1400,((K4buf[5]>>1)));break;
           case 0xd003:K4buf[5]&=0x0F;K4buf[5]|=V<<4;VROM_BANK1(0x1400,((K4buf[5]>>1)));break;
           case 0xe000:K4buf[6]&=0xF0;K4buf[6]|=V&0x0F;VROM_BANK1(0x1800,((K4buf[6]>>1)));break;
           case 0xe002:K4buf[6]&=0x0F;K4buf[6]|=V<<4;VROM_BANK1(0x1800,((K4buf[6]>>1)));break;
           case 0xe001:K4buf[7]&=0xF0;K4buf[7]|=V&0x0F;VROM_BANK1(0x1c00,((K4buf[7]>>1)));break;
           case 0xe003:K4buf[7]&=0x0F;K4buf[7]|=V<<4;VROM_BANK1(0x1c00,((K4buf[7]>>1)));break;
         }
        }
}


void Mapper22_init(void)
{
	SetWriteHandler(0x8000,0xffff,(void *)Mapper22_write);
}
