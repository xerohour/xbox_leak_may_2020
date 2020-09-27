#include "mapinc.h"


#define dopol mapbyte1[0]
byte gorfus=0xFF;
static int32 inc;
void NamcoIRQHook(int a)
{
  if(IRQa)
  {
   IRQCount+=a;
   if(IRQCount>=0x10000+8)
   {
    TriggerIRQ();
    IRQlow|=1;
    IRQa=0;
    IRQCount=0xFFFF;
   }
  }
}


byte Namco_Read(word A)
{
byte ret=0;
switch(A&0xF800){
        case 0x4800:ret=MapperExRAM[dopol&0x7f];
        if(dopol&0x80)
         dopol=(dopol&0x80)|((dopol++)&0x7f);
        break;
        case 0x5000:ret=IRQCount&0xFF;break;
        case 0x5800:ret=IRQCount>>8;break;
        }
return ret;
}


void Mapper19_write(uint16 A,uint8 V)
{
        switch(A&0xF800){
        case 0x4800:MapperExRAM[dopol&0x7f]=V;
		    if(dopol&0x40) MapExpSound=(void *)NamcoSound;
                    if(dopol&0x80)
                     dopol=(dopol&0x80)|((dopol++)&0x7f);
                    break;
        case 0xf800:dopol=V;break;
        case 0x5000: IRQCount&=0xFF00;IRQCount|=V;IRQlow&=~1;break;
        case 0x5800: IRQCount&=0x00ff;IRQCount|=V<<8;
                     IRQa=V&0x80;
                     //IRQCount=IRQLatch;
                     IRQlow&=~1;
                     break;
        case 0x8000:
                     if(!(gorfus&0x40) && (V&0xE0)) VRAM_BANK1(0x0000,V&7);
                     else{VROM_BANK1(0x0000,V);}
                     break;
        case 0x8800:
                     if(!(gorfus&0x40) && (V&0xE0)) VRAM_BANK1(0x0400,V&7);
                     else{VROM_BANK1(0x0400,V);}
                     break;
        case 0x9000:
                     if(!(gorfus&0x40) && (V&0xE0)) VRAM_BANK1(0x0800,V&7);
                     else{VROM_BANK1(0x0800,V);}
                     break;
        case 0x9800:
                     if(!(gorfus&0x40) && (V&0xE0)) VRAM_BANK1(0x0C00,V&7);
                     else{VROM_BANK1(0x0C00,V);}
                     break;
        case 0xa000:
                     if(!(gorfus&0x80) && (V&0xE0)) VRAM_BANK1(0x1000,V&7);
                     else{VROM_BANK1(0x1000,V);}
                     break;
        case 0xa800:
                     if(!(gorfus&0x80) && (V&0xE0)) VRAM_BANK1(0x1400,V&7);
                     else{VROM_BANK1(0x1400,V);}
                     break;
        case 0xb000:
                     if(!(gorfus&0x80) && (V&0xE0)) VRAM_BANK1(0x1800,V&7);
                     else{VROM_BANK1(0x1800,V);}
                     break;
        case 0xb800:
                     if(!(gorfus&0x80) && (V&0xE0)) VRAM_BANK1(0x1c00,V&7);
                     else{VROM_BANK1(0x1c00,V);}
                     break;
        case 0xc000:if(V>=0xE0)
                     {
                     vnapage[0]=VRAM+0x2000+((V&1)<<11);
                     VPAL2[0]=((V&1)<<1)|0x8000;
                     }
                    else
                    {
                     V&=vmask1;
                     vnapage[0]=VROM+(V<<10);
                     VPAL2[0]=V|0x8800;
                    }
                    break;
        case 0xc800:if(V>=0xE0)
                     {
                     vnapage[1]=VRAM+0x2000+((V&1)<<11);
                     VPAL2[1]=((V&1)<<1)|0x8000;
                     }
                    else
                     {
                    V&=vmask1;vnapage[1]=VROM+(V<<10);
                    VPAL2[1]=V|0x8800;
                     }
                    break;
        case 0xD000:if(V>=0xE0)
                     {
                     vnapage[2]=VRAM+0x2000+((V&1)<<11);
                     VPAL2[2]=((V&1)<<1)|0x8000;
                     }
                    else
                    {
                    V&=vmask1;vnapage[2]=VROM+(V<<10);
                    VPAL2[2]=V|0x8800;
                    }
                    break;
        case 0xD800:if(V>=0xE0)
                     {
                     vnapage[3]=VRAM+0x2000+((V&1)<<11);
                     VPAL2[3]=((V&1)<<1)|0x8000;
                     }
                    else
                    {
                    V&=vmask1;
                    vnapage[3]=VROM+(V<<10);
                    VPAL2[3]=V|0x8800;
                    }
                    break;

        case 0xE000:
        ROM_BANK8(0x8000,V);
        break;
        case 0xE800:
         ROM_BANK8(0xA000,V);
         break;
        case 0xF000:
         ROM_BANK8(0xC000,V);
         break;
        }
}
int NamcoSound(int32 *Wave)
{
      int P,V;
      static byte PlayIndex[8];
      unsigned long freq;
      for(P=0;P<8;P++)
      {
       if((MapperExRAM[0x44+(P<<3)]&0xE0) && (MapperExRAM[0x47+(P<<3)]&0xF))
       {
        byte duff;
        int duff2;
        long envelope;
        long vco;
        vco=vcount[P];
        freq=MapperExRAM[0x40+(P<<3)];
        freq|=MapperExRAM[0x42+(P<<3)]<<8;
        freq|=(MapperExRAM[0x44+(P<<3)]&3)<<16;
        if(!freq) continue;
        inc=(long double)(SndRate<<15)/(((long double)freq*(440*
         (2-((MapperExRAM[0x44+(P<<3)]>>4)&1)))/(long double)15467));
        envelope=((MapperExRAM[0x47+(P<<3)]&0xF)<<18)/15;
        duff=MapperExRAM[((MapperExRAM[0x46+(P<<3)]+PlayIndex[P])&0x3F)>>1];
        if((MapperExRAM[0x46+(P<<3)]+PlayIndex[P])&1)
         duff>>=4;
        duff&=0xF;
        duff2=((duff-8)*envelope)>>16;
        for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
        {
         if(vco>=inc)
         {
          PlayIndex[P]++;
          if(PlayIndex[P]>=((8-((MapperExRAM[0x44+(P<<3)]>>2)&(3))))<<2)
           PlayIndex[P]=0;
          vco-=inc;
          duff=MapperExRAM[((MapperExRAM[0x46+(P<<3)]+PlayIndex[P])&0x3F)>>1];
          if((MapperExRAM[0x46+(P<<3)]+PlayIndex[P])&1)
           duff>>=4;
          duff&=0xF;
          duff2=((duff-8)*envelope)>>16;
         }
          Wave[V>>4]+=duff2;
          vco+=0x8000;
        }
        vcount[P]=vco;
       }
      }
  return 1;
}
void Mapper19_init(void)
{
        VROM_BANK8(vmask);
        SetWriteHandler(0x8000,0xffff,(void *)Mapper19_write);
        SetWriteHandler(0x4020,0x5fff,(void *)Mapper19_write);
        SetReadHandler(0x4800,0x5fff,(void *)Namco_Read);
        MapIRQHook=(void *)NamcoIRQHook;
}

