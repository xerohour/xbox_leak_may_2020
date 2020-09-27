#include "mapinc.h"



void Mapper70_write(uint16 A,uint8 V)
{
ROM_BANK16(0x8000,V>>4);
VROM_BANK8(V&0xF);
MIRROR_SET((V>>3)&1);
}

void Mapper70_init(void)
{
SetWriteHandler(0x8000,0xffff,(void *)Mapper70_write);

}
