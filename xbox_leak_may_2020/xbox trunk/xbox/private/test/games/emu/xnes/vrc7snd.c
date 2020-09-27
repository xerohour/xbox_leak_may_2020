#include <stdio.h>
#include <stdlib.h>
#include "mapinc.h"
#include "fmopl.h"
#include "vrc7q.h"

FM_OPL *fmob=0;
static void initopl(int how);

void writeopl(byte A, byte V)
{
 MapperExRAM[A]=V;
 if(fmob>0) {OPLWrite(fmob,0,A);OPLWrite(fmob,1,V);}
}


void vrc7translate(byte A, byte V)
{
 if(!fmob) initopl(0);
 AddQueue(VRC7_dosound,A,V);
}


void LoadOPL(void)
{
 int x;
 int y;
 for(x=y=0;x<0xF6;x++)
  y|=MapperExRAM[x];
 if(y)
 { 
  initopl(1); 
  for(x=0;x<0xF6;x++)
   writeopl(x,MapperExRAM[x]);
 }
}

void UpdateOPL(int32 *d)
{
if(fmob>0) YM3812UpdateOne(fmob, d, 256);
}

void trashopl(void)
{
int x;
for(x=0x1;x<0xF6;x++)
 writeopl(x,0);
}

static void initopl(int how)
{
int x;

fmob=OPLCreate(OPL_TYPE_WAVESEL,1789772*2,SndRate);
if(!fmob ) return;

OPLResetChip(fmob);

if(!how)
{
 for(x=0x1;x<0xF6;x++)
  {writeopl(x,0);}

  writeopl(0xBD,0xC0);
  writeopl(1,0x20);      // Enable waveform type manipulation
 }
}

//#define u8 unsigned char


// vrc7q.c ------ moved by kevin
u8 Instrument[15][8] = {
	{0x03,0x01,0x14,0x80,0xC2,0x90,0x43,0x14},	// Currently working on this one
	{0x13,0x41,0x10,0x0B,0xFF,0xF2,0x32,0xD6},
	{0x01,0x01,0x10,0x08,0xF0,0xF4,0x00,0x04},	// 90% perfect
	{0x21,0x41,0x1B,0x08,0x66,0x80,0x30,0x85},
	{0x22,0x21,0x20,0x03,0x75,0x70,0x24,0x14},
	{0x02,0x01,0x06,0x00,0xF0,0xF2,0x03,0x95},	// Do not touch! 98% perfect!
	{0x21,0x41,0x18,0x10,0x93,0xE0,0x21,0x15},
	{0x01,0x22,0x13,0x00,0xF0,0x82,0x00,0x15},
	{0x05,0x01,0x22,0x00,0x60,0xE3,0xA0,0xF5},	// 90% perfect
	{0x85,0x01,0x20,0x00,0xD7,0xA2,0x22,0xF5},	// 90% perfect
	{0x07,0x81,0x2B,0x05,0xF4,0xF2,0x14,0xF4},	// 95% perfect
	{0x21,0x41,0x20,0x18,0xF3,0x80,0x13,0x95},
	{0x01,0x02,0x20,0x00,0xF9,0x92,0x41,0x75},	// Do not touch! 98% perfect!
	{0x21,0x62,0x0E,0x00,0x84,0x85,0x45,0x15},	// 90% perfect
	{0x21,0x62,0x0E,0x00,0xA1,0xA0,0x34,0x16}	// Do not touch! 98% perfect!
};

#define Chan1x MapperExRAM
#define Chan2x (MapperExRAM+6)
#define Chan3x (MapperExRAM+12)

u8 InstTrans[6] = {0x00,0x01,0x02,0x08,0x09,0x0A};

#define OPL2_setreg writeopl

void VRC7_LoadInstrument(u8 Chan)
{
	u8 x = InstTrans[Chan];
	u8 y = (Chan3x[Chan] >> 4) & 0xF;
	if(y)
        { 
	 y-=1;
	 OPL2_setreg((u8)(0x20+x),(u8)(Instrument[y][0]));
	 OPL2_setreg((u8)(0x23+x),(u8)(Instrument[y][1]));
	 OPL2_setreg((u8)(0x40+x),(u8)(Instrument[y][2]));
	 OPL2_setreg((u8)(0x43+x),(u8)((Instrument[y][3] & 0xC0) 
	 | ((Chan3x[Chan] << 2) & 0x3C)));	// quiet
	 OPL2_setreg((u8)(0xe0+x),(u8)((Instrument[y][3] >> 3) & 0x01));
	 OPL2_setreg((u8)(0xe3+x),(u8)((Instrument[y][3] >> 4) & 0x01));
	 OPL2_setreg((u8)(0xC0+Chan),(u8)((Instrument[y][3] << 1) & 0x0E));
	 OPL2_setreg((u8)(0x60+x),(u8)(Instrument[y][4]));
	 OPL2_setreg((u8)(0x63+x),(u8)(Instrument[y][5]));
	 OPL2_setreg((u8)(0x80+x),(u8)(Instrument[y][6]));
	 OPL2_setreg((u8)(0x83+x),(u8)(Instrument[y][7]));
	}
	else
	{
	 #define ex (MapperExRAM+256)
	 OPL2_setreg((u8)(0x20+x),(u8)(ex[0]));
         OPL2_setreg((u8)(0x23+x),(u8)(ex[1]));
         OPL2_setreg((u8)(0x40+x),(u8)(ex[2]));
         OPL2_setreg((u8)(0x43+x),(u8)((ex[3] & 0xC0)
         | ((Chan3x[Chan] << 2) & 0x3C)));       // quiet
         OPL2_setreg((u8)(0xe0+x),(u8)((ex[3] >> 3) & 0x01));
         OPL2_setreg((u8)(0xe3+x),(u8)((ex[3] >> 4) & 0x01));
         OPL2_setreg((u8)(0xC0+Chan),(u8)((ex[3] << 1) & 0x0E));
         OPL2_setreg((u8)(0x60+x),(u8)(ex[4]));
         OPL2_setreg((u8)(0x63+x),(u8)(ex[5]));
         OPL2_setreg((u8)(0x80+x),(u8)(ex[6]));
         OPL2_setreg((u8)(0x83+x),(u8)(ex[7]));
	 #undef ex
	}

}

void VRC7_dosound (u8 Reg,u8 Value)
{
	u8 x = Reg & 0x0F, y;
	switch ((Reg & 0xF0) >> 4)
	{
	case 0:
		if (x & 0x08) break;
		MapperExRAM[256+x] = Value;
		for (y = 0; y < 6; y++)
			if (((Chan3x[y] >> 4) & 0xF) == 0)
				VRC7_LoadInstrument(y);
		break;
	case 1:
		Chan1x[x] = Value;
		OPL2_setreg((u8)(0xA0 + x),(u8)((Chan1x[x] << 1) & 0xFE));
		OPL2_setreg((u8)(0xB0 + x),(u8)(((Chan1x[x] >> 7) & 0x01) | ((Chan2x[x] << 1) & 0x3E)));
		break;
	case 2:
	        {
		 u8 temp;
		 temp=Chan3x[x]>>4;
 		 if (((!Value) & 0x20) && (Chan2x[x] & 0x20))
		  {
		   if(temp)
                    OPL2_setreg((u8)(0x23 + InstTrans[x]),
                     (u8)(Instrument[(Chan3x[x] >> 4)-1][1] & 0xDF));
		   else
                    OPL2_setreg((u8)(0x23 + InstTrans[x]),
                     (u8)(MapperExRAM[256+1] & 0xDF));
		  }
		Chan2x[x] = Value;
		OPL2_setreg((u8)(0xB0 + x),(u8)(((Chan1x[x] >> 7) & 0x01) | ((Chan2x[x] << 1) & 0x3E)));
		break;
		}
	case 3:
		Chan3x[x] = Value;
		VRC7_LoadInstrument(x);
		break;
	}
} 
