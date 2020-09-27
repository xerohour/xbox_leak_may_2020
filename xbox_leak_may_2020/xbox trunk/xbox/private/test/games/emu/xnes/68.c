#include "mapinc.h"


#define boogaa mapbyte1[0]
#define boogaa2 mapbyte1[1]

void Mapper68_write(uint16 A,uint8 V)
{
switch(A&0xF000)
 {

 case 0x8000: VROM_BANK2(0x0000,V);break;
 case 0x9000: VROM_BANK2(0x0800,V);break;
 case 0xA000: VROM_BANK2(0x1000,V);break;
 case 0xB000: VROM_BANK2(0x1800,V);break;
 case 0xc000:
              if(VROM_size)
              {
              V|=128;
              V&=vmask1;
              vnapage[0]=VROM+(V<<10);
              VPAL2[0]=V|0x8800;
              boogaa=V;
              }
              break;
 case 0xd000:
              if(VROM_size)
              {
              V|=128;
              V&=vmask1;
              vnapage[2]=VROM+(V<<10);
              VPAL2[2]=V|0x8800;
              }
              boogaa2=V;
              break;

 case 0xe000:
              if(!(V&0x10))
              {
               switch(V&3)
                {
                case 0:MIRROR_SET2(1);break;
                case 1:MIRROR_SET2(0);break;
                case 2:onemir(0);break;
                case 3:onemir(2);break;
                }

              }
              else if(VROM_size)
              {
                vnapage[0]=VROM+(boogaa<<10);
                vnapage[1]=VROM+(boogaa2<<10);
                vnapage[2]=VROM+(boogaa2<<10);
                vnapage[3]=VROM+(boogaa<<10);
                VPAL2[0]=boogaa|0x8800;
                VPAL2[1]=boogaa2|0x8800;
                VPAL2[2]=boogaa2|0x8800;
                VPAL2[3]=boogaa|0x8800;
              }
              break;
 case 0xf000: ROM_BANK16(0x8000,V);break;
 }
}

void Mapper68_init(void)
{
SetWriteHandler(0x8000,0xffff,(void *)Mapper68_write);
}
