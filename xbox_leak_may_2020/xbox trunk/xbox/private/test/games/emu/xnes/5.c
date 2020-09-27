#include "mapinc.h"

void WriteMMC5PSG(word A,byte V);

#define MMC5SPRVROM_BANK1(A,V) {if(VROM_size){MMC5SPRVPage[(A)>>10]=&VROM[(V)<<10]-(A);}}
#define MMC5BGVROM_BANK1(A,V) {if(VROM_size){MMC5BGVPage[(A)>>10]=&VROM[(V)<<10]-(A);}}

#define MMC5SPRVROM_BANK2(A,V) {if(VROM_size){MMC5SPRVPage[(A)>>10]=MMC5SPRVPage[((A)>>10)+1]=&VROM[(V)<<11]-(A);}}
#define MMC5BGVROM_BANK2(A,V) {if(VROM_size){MMC5BGVPage[(A)>>10]=MMC5BGVPage[((A)>>10)+1]==&VROM[(V)<<11]-(A);}}

#define MMC5SPRVROM_BANK4(A,V) {if(VROM_size){MMC5SPRVPage[(A)>>10]=MMC5SPRVPage[((A)>>10)+1]= MMC5SPRVPage[((A)>>10)+2]=MMC5SPRVPage[((A)>>10)+3]=&VROM[(V)<<12]-(A);}}
#define MMC5BGVROM_BANK4(A,V) {if(VROM_size){MMC5BGVPage[(A)>>10]=MMC5BGVPage[((A)>>10)+1]=MMC5BGVPage[((A)>>10)+2]=MMC5BGVPage[((A)>>10)+3]=&VROM[(V)<<12]-(A);}}

#define MMC5SPRVROM_BANK8(V) {if(VROM_size){MMC5SPRVPage[0]=MMC5SPRVPage[1]=MMC5SPRVPage[2]=MMC5SPRVPage[3]=MMC5SPRVPage[4]=MMC5SPRVPage[5]=MMC5SPRVPage[6]=MMC5SPRVPage[7]=&VROM[(V)<<13];}}
#define MMC5BGVROM_BANK8(V) {if(VROM_size){MMC5BGVPage[0]=MMC5BGVPage[1]=MMC5BGVPage[2]=MMC5BGVPage[3]=MMC5BGVPage[4]=MMC5BGVPage[5]=MMC5BGVPage[6]=MMC5BGVPage[7]=&VROM[(V)<<13];}}

static int32 inc;
byte MMC5fill[0x400];

#define MMC5IRQR        mapbyte3[4]
#define MMC5LineCounter mapbyte3[5]
#define mmc5psize mapbyte1[0]
#define mmc5vsize mapbyte1[1]

byte MMC5WRAMsize=1;
byte MMC5WRAMIndex[8];

byte MMC5ROMWrProtect[4];
static void MMC5CHRA(void);
static void MMC5CHRB(void);

#include "crc32.h"

typedef struct __cartdata {
        unsigned long crc32;
        unsigned char size;
} cartdata;
#define MMC5_NOCARTS 13
cartdata MMC5CartList[]=
{
 {0x9c18762b,2},         /* L'Empereur */
 {0x26533405,2},
 {0x6396b988,2},

 {0xaca15643,2},        /* Uncharted Waters */
 {0xfe3488d1,2},        /* Dai Koukai Jidai */

 {0x15fe6d0f,2},        /* BKAC             */
 {0x39f2ce4b,2},        /* Suikoden              */

 {0x8ce478db,2},        /* Nobunaga's Ambition 2 */
 {0xeee9a682,2},

 {0xf540677b,4},        /* Nobunaga...Bushou Fuuun Roku */

 {0x6f4e4312,4},        /* Aoki Ookami..Genchou */

 {0xf011e490,4},        /* Romance of the 3 Kingdoms 2 */
 {0x184c2124,4},        /* Sangokushi 2 */
};

void DetectMMC5WRAMSize(void)
{
 int x;
 unsigned long accum;

 gen_crc_table();

 MMC5WRAMsize=1;

 accum=0xFFFFFFFF;

 accum=update_crc(accum,ROM,ROM_size<<14);
 if(VROM_size)
  accum=update_crc(accum,VROM,VROM_size<<13);

 accum=~accum;
 for(x=0;x<MMC5_NOCARTS;x++)
  if(accum==MMC5CartList[x].crc32)
   {
   MMC5WRAMsize=MMC5CartList[x].size;
   break;
   }
 for(x=0;x<8;x++)
 {
  switch(MMC5WRAMsize)
  {
    default:
    case 1:MMC5WRAMIndex[x]=(x>3)?255:0;break;
    case 2:MMC5WRAMIndex[x]=(x&4)>>2;break;
    case 4:MMC5WRAMIndex[x]=(x>3)?255:(x&3);break;
    //case 8:MMC5WRAMIndex[x]=x;break;
  }
 }
}


static void MMC5CHRA(void)
{
int x;
switch(mapbyte1[1]&3)
 {
 case 0:MMC5SPRVROM_BANK8(mapbyte2[7]&vmask);
        VROM_BANK8(mapbyte2[7]&vmask);
        break;
 case 1:
        MMC5SPRVROM_BANK4(0x0000,mapbyte2[3]&vmask4);
        MMC5SPRVROM_BANK4(0x1000,mapbyte2[7]&vmask4);
        {VROM_BANK4(0x0000,mapbyte2[3]&vmask4);
        VROM_BANK4(0x1000,mapbyte2[7]&vmask4);}
        break;
 case 2:MMC5SPRVROM_BANK2(0x0000,mapbyte2[1]&vmask2);
        MMC5SPRVROM_BANK2(0x0800,mapbyte2[3]&vmask2);
        MMC5SPRVROM_BANK2(0x1000,mapbyte2[5]&vmask2);
        MMC5SPRVROM_BANK2(0x1800,mapbyte2[7]&vmask2);
        VROM_BANK2(0x0000,mapbyte2[1]&vmask2);
        VROM_BANK2(0x0800,mapbyte2[3]&vmask2);
        VROM_BANK2(0x1000,mapbyte2[5]&vmask2);
        VROM_BANK2(0x1800,mapbyte2[7]&vmask2);
        break;
 case 3:
        {for(x=0;x<8;x++)VROM_BANK1(x<<10,mapbyte2[x]&vmask1);}
        for(x=0;x<8;x++)
         MMC5SPRVROM_BANK1(x<<10,mapbyte2[x]&vmask1);
        break;
 }
}
static void MMC5CHRB(void)
{
int x;
switch(mapbyte1[1]&3)
 {
 case 0:MMC5BGVROM_BANK8(mapbyte3[3]&vmask);
        VROM_BANK8(mapbyte3[3]&vmask);
        break;
 case 1:
        MMC5BGVROM_BANK4(0x0000,mapbyte3[3]&vmask4);
        MMC5BGVROM_BANK4(0x1000,mapbyte3[3]&vmask4);
        {VROM_BANK4(0x0000,mapbyte3[3]&vmask4);
        VROM_BANK4(0x1000,mapbyte3[3]&vmask4);}
        break;
 case 2:MMC5SPRVROM_BANK2(0x0000,mapbyte3[1]&vmask2);
        MMC5SPRVROM_BANK2(0x0800,mapbyte3[3]&vmask2);
        MMC5SPRVROM_BANK2(0x1000,mapbyte3[1]&vmask2);
        MMC5SPRVROM_BANK2(0x1800,mapbyte3[3]&vmask2);
        VROM_BANK2(0x0000,mapbyte3[1]&vmask2);
        VROM_BANK2(0x0800,mapbyte3[3]&vmask2);
        VROM_BANK2(0x1000,mapbyte3[1]&vmask2);
        VROM_BANK2(0x1800,mapbyte3[3]&vmask2);
        break;
 case 3:
        {for(x=0;x<8;x++)VROM_BANK1(x<<10,mapbyte3[x&3]&vmask1);}
        for(x=0;x<8;x++)
         MMC5BGVROM_BANK1(x<<10,mapbyte3[x&3]&vmask1);
        break;
 }
}
void MMC5WRAM(word A, byte V)
{
   V=MMC5WRAMIndex[V&7];
   if(V==255)
   {
    Page[A>>13]=nothing-A;
   }
   else
   {
    if(V)
    {
     Page[A>>13]=MapperExRAM+(V<<13)-A;
    }
    else
     Page[A>>13]=WRAM-A;
    }
}

void MMC5PRG(void)
{
 switch(mapbyte1[0]&3)
  {
  case 0:
         if(mapbyte1[5]&0x80)
          {
           MMC5ROMWrProtect[0]=MMC5ROMWrProtect[1]=
           MMC5ROMWrProtect[2]=MMC5ROMWrProtect[3]=1;
           ROM_BANK16(0x8000,(mapbyte1[5]>>2));
           ROM_BANK16(0xC000,((mapbyte1[5]>>2)+1));
          }
         else
          {
           MMC5ROMWrProtect[0]=MMC5ROMWrProtect[1]=
           MMC5ROMWrProtect[2]=MMC5ROMWrProtect[3]=0;
           MMC5WRAM(0x8000,mapbyte1[5]&7&0xFC);
           MMC5WRAM(0xA000,(mapbyte1[5]&7&0xFC)+1);
           MMC5WRAM(0xC000,(mapbyte1[5]&7&0xFC)+2);
           MMC5WRAM(0xE000,(mapbyte1[5]&7&0xFC)+3);
          }
          break;
  case 1:
         if(mapbyte1[5]&0x80)
          {
           MMC5ROMWrProtect[0]=MMC5ROMWrProtect[1]=1;
           ROM_BANK16(0x8000,(mapbyte1[5]>>1));
          }
         else
          {
           MMC5ROMWrProtect[0]=MMC5ROMWrProtect[1]=0;
           MMC5WRAM(0x8000,mapbyte1[5]&7&0xFE);
           MMC5WRAM(0xA000,(mapbyte1[5]&7&0xFE)+1);
          }
         if(mapbyte1[7]&0x80)
          {
           MMC5ROMWrProtect[2]=MMC5ROMWrProtect[3]=1;
           ROM_BANK16(0x8000,(mapbyte1[7]>>1));
          }
         else
          {
           MMC5ROMWrProtect[2]=MMC5ROMWrProtect[3]=0;
           MMC5WRAM(0xC000,mapbyte1[7]&7&0xFE);
           MMC5WRAM(0xE000,(mapbyte1[7]&7&0xFE)+1);
          }
         break;
  case 2:
         if(mapbyte1[5]&0x80)
          {
           MMC5ROMWrProtect[0]=MMC5ROMWrProtect[1]=1;
           ROM_BANK16(0x8000,(mapbyte1[5]>>1));
          }
         else
          {
           MMC5ROMWrProtect[0]=MMC5ROMWrProtect[1]=0;
           MMC5WRAM(0x8000,mapbyte1[5]&7&0xFE);
           MMC5WRAM(0xA000,(mapbyte1[5]&7&0xFE)+1);
          }
         if(mapbyte1[6]&0x80)
          {MMC5ROMWrProtect[2]=1;ROM_BANK8(0xC000,mapbyte1[6]);}
         else
          {MMC5ROMWrProtect[2]=0;MMC5WRAM(0xC000,mapbyte1[6]&7);}
         MMC5ROMWrProtect[3]=1;
         ROM_BANK8(0xE000,mapbyte1[7]);
         break;
  case 3:
         if(mapbyte1[4]&0x80)
          {MMC5ROMWrProtect[0]=1;ROM_BANK8(0x8000,mapbyte1[4]);}
         else
          {MMC5ROMWrProtect[0]=0;MMC5WRAM(0x8000,mapbyte1[4]&7);}
         if(mapbyte1[5]&0x80)
          {MMC5ROMWrProtect[1]=1;ROM_BANK8(0xA000,mapbyte1[5]);}
         else
          {MMC5ROMWrProtect[1]=0;MMC5WRAM(0xA000,mapbyte1[5]&7);}
         if(mapbyte1[6]&0x80)
          {MMC5ROMWrProtect[2]=1;ROM_BANK8(0xC000,mapbyte1[6]);}
         else
          {MMC5ROMWrProtect[2]=0;MMC5WRAM(0xC000,mapbyte1[6]&7);}
         MMC5ROMWrProtect[3]=1;
         ROM_BANK8(0xE000,mapbyte1[7]);
         break;
  }
}

#define mul1 mapbyte3[6]
#define mul2 mapbyte3[7]


void Mapper5_write(uint16 A,uint8 V)
{
 switch(A)
  {
   default:break;
   case 0x5105:
                {
                int x;
                for(x=0;x<4;x++)
                {
                 switch((V>>(x<<1))&3)
                 {
                 case 0:VPAL2[x]=0x8000;vnapage[x]=VRAM+0x2000;break;
                 case 1:VPAL2[x]=0x8002;vnapage[x]=VRAM+0x2800;break;
                 case 2:VPAL2[x]=0x8001;vnapage[x]=MapperExRAM;break;
                 case 3:VPAL2[x]=0x8800;vnapage[x]=MMC5fill;break;
                 }
                }
               }
               mapbyte4[3]=V;
               break;
   case 0x5000:
   case 0x5001:
   case 0x5002:
   case 0x5003:
   case 0x5004:
   case 0x5005:
   case 0x5006:
   case 0x5007:
   case 0x5010:
   case 0x5011:
   case 0x5015:WriteMMC5PSG(A,V);break;

   case 0x5113:mapbyte4[6]=V;MMC5WRAM(0x6000,V&7);break;
   case 0x5100:mapbyte1[0]=V;MMC5PRG();break;
   case 0x5101:mapbyte1[1]=V;
               if(!mapbyte4[7])
                {MMC5CHRB();MMC5CHRA();}
               else
                {MMC5CHRB();MMC5CHRA();}
               break;

   case 0x5114:
   case 0x5115:
   case 0x5116:
   case 0x5117:
               mapbyte1[A&7]=V;MMC5PRG();break;

   case 0x5120:
   case 0x5121:
   case 0x5122:
   case 0x5123:
   case 0x5124:
   case 0x5125:
   case 0x5126:
   case 0x5127:mapbyte4[7]=0;
               mapbyte2[A&7]=V;MMC5CHRA();break;
   case 0x5128:
   case 0x5129:
   case 0x512a:
   case 0x512b:mapbyte4[7]=1;
               mapbyte3[A&3]=V;MMC5CHRB();break;
   case 0x5102:mapbyte4[0]=V;break;
   case 0x5103:mapbyte4[1]=V;break;
   case 0x5104:mapbyte4[2]=V;break;
   case 0x5106:if(V!=mapbyte4[4])
               {
                dwmemset(MMC5fill,V,0x3c0);
               }
               mapbyte4[4]=V;
               break;
   case 0x5107:if(V!=mapbyte4[5])
               {
                unsigned char moop;
                moop=V|(V<<2)|(V<<4)|(V<<6);
                dwmemset(MMC5fill+0x3c0,moop,0x40);
               }
               mapbyte4[5]=V;
               break;
   case 0x5203:IRQlow&=~1;IRQCount=(byte)V;break;
   case 0x5204:IRQlow&=~1;IRQa=V&0x80;break;
   case 0x5205:mul1=V;break;
   case 0x5206:mul2=V;break;
  }
}

byte MMC5_ReadROMRAM(word A)
{
         return Page[A>>13][A];
}

void MMC5_WriteROMRAM(word A, byte V)
{
       if(A>=0x8000)
        if(MMC5ROMWrProtect[(A-0x8000)>>13])
         return;
       Page[A>>13][A]=V;
}

void MMC5_ExRAMWr(word A, byte V)
{
 MapperExRAM[A&0x3ff]=V;
}

byte MMC5_ExRAMRd(word A)
{
 return MapperExRAM[A&0x3ff];
}

byte MMC5_read(word A)
{
 switch(A)
 {
  default:break;
  case 0x5204:IRQlow&=~1;
              {byte x;x=MMC5IRQR;MMC5IRQR&=0x40;if(!IRQa)MMC5IRQR&=0; return x;}
  case 0x5205:return (byte)((byte)mul1*(byte)mul2);
  case 0x5206:return ((word)(mul1*mul2))>>8;
 }
 return 0xFF;
}
void MMC5_restore(void)
{
 int x;
 MMC5PRG();
 for(x=0;x<4;x++)
 {
  switch((mapbyte4[3]>>(x<<1))&3)
   {
    case 0:VPAL2[x]=0x8000;vnapage[x]=VRAM+0x2000;break;
    case 1:VPAL2[x]=0x8002;vnapage[x]=VRAM+0x2800;break;
    case 2:VPAL2[x]=0x8001;vnapage[x]=MapperExRAM;break;
    case 3:VPAL2[x]=0x8800;vnapage[x]=MMC5fill;break;
   }
 }
 MMC5WRAM(0x6000,mapbyte4[6]&7);
 if(!mapbyte4[7])
  {
  MMC5CHRB();
  MMC5CHRA();
  }
  else
  {
   MMC5CHRB();
   MMC5CHRA();
  }
  dwmemset(MMC5fill,mapbyte4[4],0x3c0);
  {
   unsigned char moop;
   moop=mapbyte4[5]|(mapbyte4[5]<<2)|(mapbyte4[5]<<4)|(mapbyte4[5]<<6);
   dwmemset(MMC5fill+0x3c0,moop,0x40);
  }
  IRQlow&=~1;
}

void MMC5_hb(void)
{
  if(scanline==0 && (ScreenON || SpriteON)) {MMC5LineCounter=0;}
  if(MMC5LineCounter<245)
  {
   if(MMC5LineCounter==IRQCount) MMC5IRQR|=0x80;
   if((MMC5LineCounter==IRQCount && IRQa&0x80))
    {TriggerIRQ();IRQlow|=1;}
   if(ScreenON || SpriteON)
        MMC5LineCounter++;
  }
  if(MMC5LineCounter>=245) MMC5IRQR|=0x40;
}

void Mapper5_StateRestore(int version)
{
 MMC5_restore();
}

static uint32 MMC5slengthtable[0x20]=
 {
 0x5,0x7f,0xA,0x1,0x14,0x2,0x28,0x3,0x50,0x4,0x1E,0x5,0x7,0x6,0x0E,0x7,
 0x6,0x08,0xC,0x9,0x18,0xa,0x30,0xb,0x60,0xc,0x24,0xd,0x8,0xe,0x10,0xf
 };
static int32 dectab[32];
static uint32 MMC5lengthtable[0x20];

#define MMC5deccount(x)    (*(int32 *)&MapperExRAM[0x400+((x)<<2)])
#define MMC5decvolume(x)   (*(uint8 *)&MapperExRAM[0x408+(x)])
#define MMC5sqnon          (*(uint8 *)&MapperExRAM[0x40A])
#define MMC5lengthcount(x) (*(int32 *)&MapperExRAM[0x40B+((x)<<2)])
#define MMC5PSG(x)         (*(uint8 *)&MapperExRAM[0x40B+8+(x)])

void WriteMMC5PSG(word A,byte V)
{
 A&=0x1f;
 switch(A)
 {
  case 0x3:
          if(MMC5PSG(0x15)&1)
          {
          MMC5decvolume(0)=15;
          MMC5lengthcount(0)=MMC5lengthtable[(V>>3)&0x1f];
          MMC5sqnon|=1;
          MMC5deccount(0)=1<<29;
          }
          break;
  case 0x7:
          if(MMC5PSG(0x15)&2)
          {
          MMC5decvolume(1)=15;
          MMC5lengthcount(1)=MMC5lengthtable[(V>>3)&0x1f];
          MMC5sqnon|=2;
          MMC5deccount(1)=1<<29;
          }
          break;
 case 0x15:
          MMC5sqnon&=V;
          break;
 }
 MMC5PSG(A)=V;
}


int MMC5Sound(int32 *Wave)
{
 int P,V;
 int32 freq;
   for(P=0;P<2;P++)
   {
    if(MMC5PSG(0x15)&(P+1) && MMC5sqnon&(P+1))
    {
     unsigned long dcycs;
     unsigned char amplitude;
     long vcoo;

     freq=(((MMC5PSG((P<<2)+0x2)|((MMC5PSG((P<<2)+0x3)&7)<<8))));

     if(freq<8) goto mmc5enda;
     freq+=1;
     inc=(long double)((unsigned long)((SndRate OVERSAMPLE)<<12))/
((long double)PSG_base/freq);

     switch(MMC5PSG(P<<2)&0xC0)
     {
     default:
     case 0x00:dcycs=inc>>3;break;
     case 0x40:dcycs=inc>>2;break;
     case 0x80:dcycs=inc>>1;break;
     case 0xC0:dcycs=(inc+inc+inc)>>2;break;
     }
      if(MMC5PSG(P<<2)&0x10)
       amplitude=MMC5PSG(P<<2)&15;
      else
       amplitude=MMC5decvolume(P);
      vcoo=vcount[P];
             for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
              {
               if(vcoo>=dcycs)
                Wave[V>>4]-=amplitude;
               else
                Wave[V>>4]+=amplitude;
               vcoo+=0x1000;
               if(vcoo>=inc) vcoo-=inc;
               }
      vcount[P]=vcoo;
    }

  mmc5enda:

  if(!(MMC5PSG(P<<2)&0x20))
   {
    if(MMC5lengthcount(P)>0)
    {
     MMC5lengthcount(P)-=0x100000;
     if(MMC5lengthcount(P)<=0) MMC5sqnon&=~(P+1);
    }
   }

   MMC5deccount(P)-=dectab[(MMC5PSG(P<<2)&0xF) | ((MMC5PSG(P<<2)&0x20)>>1)];
   if(MMC5deccount(P)<=0)
    {
     if(MMC5decvolume(P) || MMC5PSG(P<<2)&0x20)
     {
      MMC5decvolume(P)--;
      MMC5decvolume(P)&=15;
      MMC5deccount(P)+=1<<29;
     }
    }
 }
 return 0;
}

void Mapper5_init(void)
{
int x;

for(x=0;x<0x20;x++) 
 {
  long double temp;
  temp=(long double)SndRate/SND_BUFSIZE;
  if(PAL)
   temp=(long double)temp/50;
  else
   temp=(long double)temp/60;
  temp*=0x100000;
  MMC5lengthtable[x]=(long double)MMC5slengthtable[x]*temp;
 }
  for(x=0;x<32;x++)
  {
   dectab[x]=(long double)0x20000000*256/((long double)SndRate/SND_BUFSIZE)/((x&15)+1+(x>>4));
  }

mapbyte1[4]=mapbyte1[5]=mapbyte1[6]=mapbyte1[7]=pmask8;
ROM_BANK8(0x8000,pmask8);
ROM_BANK8(0xa000,pmask8);
ROM_BANK8(0xc000,pmask8);
ROM_BANK8(0xe000,pmask8);
mapbyte1[0]=mapbyte1[1]=3;
MMC5ROMWrProtect[0]=MMC5ROMWrProtect[1]=
MMC5ROMWrProtect[2]=MMC5ROMWrProtect[3]=1;
MMC5CHRA();
MMC5CHRB();
DetectMMC5WRAMSize();
IRQlow&=~1;
//MMC5_write(0x5105,0xFF);
//MMC5_write(0x5106,0xFF);

SetWriteHandler(0x4020,0x5bff,(void *)Mapper5_write);
SetReadHandler(0x4020,0x5bff,(void *)MMC5_read);

SetWriteHandler(0x5c00,0x5fff,(void *)MMC5_ExRAMWr);
SetReadHandler(0x5c00,0x5fff,(void *)MMC5_ExRAMRd);

SetWriteHandler(0x6000,0xFFFF,(void *)MMC5_WriteROMRAM);
SetReadHandler(0x6000,0xFFFF,(void *)MMC5_ReadROMRAM);

MapHBIRQHook=(void *)MMC5_hb;
MapStateRestore=(void *)Mapper5_StateRestore;
MapExpSound=(void *)MMC5Sound;
}

