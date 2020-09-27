#include "mapinc.h"

/*      I'm getting the feeling this is another "jam two different bank
        switching hardwares into one mapper".
*/        

/* HES 4-in-1 */
void Mapper113_write(uint16 A,uint8 V)
{
        ROM_BANK32(0x8000,(V>>3)&7);
        VROM_BANK8(V&7);
}

      
/*      Deathbots */
void Mapper113_writeh(uint16 A,uint8 V)
{
        ROM_BANK32(0x8000,V&0xF);
}


void Mapper113_init(void)
{
 ROM_BANK32(0x8000,0);
 SetWriteHandler(0x4020,0x7fff,(void *)Mapper113_write);
 SetWriteHandler(0x8000,0xffff,(void *)Mapper113_writeh);
}
