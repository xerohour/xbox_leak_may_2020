#include "mapinc.h"


#define reg1 mapbyte1[0]
#define reg2 mapbyte1[1]
#define reg3 mapbyte1[2]
#define reg4 mapbyte1[3]

byte A110in1read(word A)
{
switch(A&0x3)
 {
  case 0:return reg1;break;
  case 1:return reg2;break;
  case 2:return reg3;break;
  case 3:return reg4;break;
 }
return 0xF;
}
void A110in1regwr(word A, byte V)
{
switch(A&0x3)
 {
  case 0:reg1=V&0xF;break;
  case 1:reg2=V&0xF;break;
  case 2:reg3=V&0xF;break;
  case 3:reg4=V&0xF;break;
 }
}

void Mapper225_write(uint16 A,uint8 V)
{
 int banks=0;

 MIRROR_SET((A>>13)&1);
 if(A&0x4000)
  banks=1;
 else
  banks=0;

  VROM_BANK8(((A&0x003f)+(banks<<6)));
 if(A&0x1000)
  {
   if(A&0x40)
    {
     ROM_BANK16(0x8000,((((((A>>7)&0x1F)+(banks<<5)))<<1)+1));
     ROM_BANK16(0xC000,((((((A>>7)&0x1F)+(banks<<5)))<<1)+1));
    }
    else
    {
     ROM_BANK16(0x8000,(((((A>>7)&0x1F)+(banks<<5)))<<1));
     ROM_BANK16(0xC000,(((((A>>7)&0x1F)+(banks<<5)))<<1));
    }
  }
  else
  {
    ROM_BANK32(0x8000,((((A>>7)&0x1F)+(banks<<5))));
  }
}

void Mapper225_init(void)
{
  SetWriteHandler(0x8000,0xffff,(void *)Mapper225_write);
  SetReadHandler(0x5800,0x5fff,(void *)A110in1read);
  SetWriteHandler(0x5800,0x5fff,(void *)A110in1regwr);
}

