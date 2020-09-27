#include "mapinc.h"

static void (*oldmorko)(uint16 A, uint8 V);
static void morko(uint16 A, uint8 V)
{
 VROM_BANK8((V>>2)&1);
 oldmorko(A,V);
}

void Mapper99_init(void)
{
 oldmorko=(void *)GetWriteHandler(0x4016);
 SetWriteHandler(0x4016,0x4016,(void *)morko);
}
