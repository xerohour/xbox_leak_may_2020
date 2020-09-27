#include "mapinc.h"


void Mapper2_write(uint16 A,uint8 V)
{
        ROM_BANK16(0x8000,V);
}

void Mapper2_init(void)
{
  SetWriteHandler(0x8000,0xFFFF,(void *)Mapper2_write);
}
