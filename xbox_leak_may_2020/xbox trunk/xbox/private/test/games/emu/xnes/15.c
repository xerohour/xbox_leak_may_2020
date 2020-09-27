#include "mapinc.h"




void Mapper15_write(uint16 A,uint8 V)
{
switch(A)
 {
  case 0x8000:
        if(V&0x80)
        {
        ROM_BANK8(0x8000,(V<<1)+1);
        ROM_BANK8(0xA000,(V<<1));
        ROM_BANK8(0xC000,(V<<1)+2);
        ROM_BANK8(0xE000,(V<<1)+1);
        }
        else
        {
        ROM_BANK16(0x8000,V&pmask16);
        ROM_BANK16(0xC000,(V+1)&pmask16);
        }
        MIRROR_SET((V>>6)&1);
        break;
  case 0x8001:
        //break;
        //if(V&0x80)
        //{
        //ROM_BANK8(0xC000,(V<<1)+1);
        //ROM_BANK8(0xE000,V<<1);
        //}
        //else
        //{
        //ROM_BANK16(0xC000,V);
        //}
        MIRROR_SET(0);
        ROM_BANK16(0x8000,V);
        ROM_BANK16(0xc000,pmask16);
        break;
  case 0x8002:
        if(V&0x80)
        {
        ROM_BANK8(0x8000,((V<<1)+1));
        ROM_BANK8(0xA000,((V<<1)+1));
        ROM_BANK8(0xC000,((V<<1)+1));
        ROM_BANK8(0xE000,((V<<1)+1));
        }
        else
        {
        ROM_BANK8(0x8000,(V<<1));
        ROM_BANK8(0xA000,(V<<1));
        ROM_BANK8(0xC000,(V<<1));
        ROM_BANK8(0xE000,(V<<1));
        }
        break;
  case 0x8003:
        MIRROR_SET((V>>6)&1);
        if(V&0x80)
        {
        ROM_BANK8(0xC000,(V<<1)+1);
        ROM_BANK8(0xE000,(V<<1));
        }
        else
        {
        ROM_BANK16(0xC000,V);
        }
        break;
 }
}

void Mapper15_init(void)
{
        ROM_BANK32(0x8000,0);
SetWriteHandler(0x8000,0xFFFF,(void *)Mapper15_write);
}

