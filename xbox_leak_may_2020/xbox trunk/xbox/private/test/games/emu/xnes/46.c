#include "mapinc.h"



#define A64reg  mapbyte1[0]
#define A64wr   mapbyte1[1]

void Mapper46_writel(uint16 A, uint8 V)
{
  A64reg=V;
  ROM_BANK32(0x8000,(A64wr&1)+((A64reg&0xF)<<1));
  VROM_BANK8(((A64wr>>4)&7)+((A64reg&0xF0)>>1));
}

void Mapper46_write(uint16 A,uint8 V)
{
  A64wr=V;
  ROM_BANK32(0x8000,(V&1)+((A64reg&0xF)<<1));
  VROM_BANK8(((V>>4)&7)+((A64reg&0xF0)>>1));
}

void Mapper46_init(void)
{
 MIRROR_SET(0);
 ROM_BANK32(0x8000,0);
 SetWriteHandler(0x8000,0xffff,(void *)Mapper46_write);
 SetWriteHandler(0x6000,0x7fff,(void *)Mapper46_writel);
}
