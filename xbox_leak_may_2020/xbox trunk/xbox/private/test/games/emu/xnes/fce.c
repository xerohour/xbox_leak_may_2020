/*
        Portable Family Computer Emulator
	1998 by BERO bero@geocities.co.jp
        FCE Ultra
        1998-2000 Xodnizel xodnizel@home.com
*/

#pragma warning( disable : 4244 )
#pragma warning( disable : 4018 )

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/stat.h>

#include "m6502.h"
#include "sound.h"
#include "fce.h"
#include "svga.h"
#include "mapper.h"
#include "general.h"
#include "config.h"
#include "endian.h"
#include "version.h"
#include "memory.h"
#include "nsf.h"
#include "fds.h"
#include "gg.h"
#include "ines.h"
#include "state.h"

#define Pal     (VRAM+0x3f00)

void RefreshLine(void);
static void PRefreshLine(void);
static void RefreshSprite(void);

byte VRAMBuffer;

int GameAttributes = 0;
void (*GameInterface)(int h);
int scanline;

int MapperNo = 0;

byte *vnapage[4]={0};
word VPAL2[4]={0};

char nothing[8192];

int toevent;
int PPUEvent;

byte (*ARead[0x10000])(word A);
void (*BWrite[0x10000])(word A, byte V);


void *GetWriteHandler(int32 a)
{
	return BWrite[a];
}

void SetReadHandler(int32 start, int32 end, byte (*func)(uint16 A))
{
	int x;
	for(x = start; x <= end; x++)
		ARead[x] = func;
}

void SetWriteHandler(int32 start, int32 end, void (*func)(uint16 A, uint8 V))
{
	int x;
	for(x = start; x <= end; x++)
		BWrite[x] = func;
}

 

byte vsdip = 0;


/* Bit 0: 0 = Game Genie loaded for game.
		  1 = Game Genie not loaded for game.
   Bit 1: 0 = No Game wGenie.
		  1 = Game Genie.
*/
byte genie = 0;

byte BaseDirectory[2048];

byte vtoggle=0;
byte XOffset=0;

pair __TempAddr;
pair __RefreshAddr;

static unsigned long scanlines_per_frame=262;
static unsigned long aha=241;

extern byte *XBuf;

byte VPAL[8];
byte PPU[4];


byte *Page[9] = {0}, *VPage[12] = {0};
byte *MMC5SPRVPage[12] = {0};
byte *MMC5BGVPage[12] = {0};
byte RAM[0x800], SPRAM[0x100], SPRBUF[0x100], WRAM[0x2000], VRAM[0x4000];
byte *ROM = NULL, *VROM = NULL;


byte joy_readbit = 0, joy2_readbit = 0, joy1or = 0, joy2or = 0;
word joy_1, joy_2;

word ROM_size;
word VROM_size;
byte ROM_type;


byte Mirroring;
byte PAL = 0;
unsigned int IPeriod = 256;

#define MMC5SPRVRAMADR(V)	&MMC5SPRVPage[(V) >> 10][(V)]
#define MMC5BGVRAMADR(V)	&MMC5BGVPage[(V) >> 10][(V)]
#define	VRAMADR(V)			&VPage[(V) >> 10][(V)]
 
void BNull(word A, byte V)
{
}

byte ANull(word A)
{
	return 0xFF;
}

byte AROM(word A)
{ 
	return Page[A >> 13][A];
}

void BRAM(word A, byte V)
{  
	RAM[A & 0x7FF] = V;
}

byte ARAM(word A)
{
	return RAM[A & 0x7FF];
}

void BWRAM(word A, byte V)
{
	WRAM[A-0x6000]=V;
}

byte AWRAM(word A)
{
	return WRAM[A - 0x6000];
}

           
byte A2000(word A)
{
	return PPU[0];
}

byte A2001(word A)
{
	return PPU[1];
}

byte A2002(word A)
{
	byte ret;
	ret = PPU_status;
	vtoggle=0;
	PPU_status &= 0x7F;
			
	if(!SpriteON && scanline == 0) 
		ret |= 0x40;

	return ret;
}

byte A2003(word A)
{
	return PPU[3];
}

byte A2004(word A)
{
	return SPRAM[PPU[3]];
}

byte A2007(word A)
{
	byte ret;
	static int32 VAddrB;

	VAddrB = RefreshAddr;
	__RefreshAddr.B.h &= 0x3F;
	ret = VRAMBuffer;
	if(__RefreshAddr.B.h >= 0x30)
	{
		if(__RefreshAddr.B.h == 0x3F)
		{
			VRAMBuffer = 0;
			ret = VRAM[0x3f00 + (__RefreshAddr.B.l & 0x1f)];
		}
	}
	else if(__RefreshAddr.B.h < 0x20) 
	{
		VRAMBuffer = VPage[RefreshAddr >> 10][RefreshAddr];
		if(PPU_hook) 
			PPU_hook(RefreshAddr);
	}
	else 
		VRAMBuffer = vnapage[((__RefreshAddr.B.h & 0xF) >> 2)][RefreshAddr & 0x3FF];
			
	RefreshAddr = VAddrB;
	if(INC32) 
		RefreshAddr += 32;
	else 
		RefreshAddr++;

	return ret;
}

byte A4016(word A)
{
	byte ret;
	ret = ((joy_1 >> (joy_readbit)) & 1) | 0x40;
             
	if(joy_readbit == 19) 
		ret = 1;
		
	joy_readbit++;		
	return ret | joy1or;
}

byte A4017(word A)
{
	byte ret;

	ret = ((joy_2 >> (joy2_readbit)) & 1);
	if(joy2_readbit == 18)
		ret = 1;
		
	joy2_readbit++;
   
	return ret | joy2or;
}



void B2000(word A, byte V)
{
	PPU[0]=V;
	__TempAddr.B.h &= 0xF3;
	__TempAddr.B.h |= (V & 3) << 2;
}

void B2001(word A, byte V)
{
	if(PAL) 
		V &= 0x1F;
             
	SetNESDeemph(V >> 5, 0);
	PPU[1] = V;
}

void B2003(word A, byte V)
{
	PPU[3] = V;
}

void B2004(word A, byte V)
{
	SPRAM[PPU[3]++] = V;
}

void B2005(word A, byte V)
{
	if(!vtoggle)
	{
		__TempAddr.B.l &= 0xE0;
		__TempAddr.B.l |= V >> 3;
		XOffset = V & 7;
	}
	else
	{
		TempAddr &= 0x8C1F;
		TempAddr |= ((V&~0x7) << 2);
		TempAddr |= (V & 7) << 12;
	}
	
	vtoggle ^= 1;
}

void B2006(word A, byte V)
{
	if(!vtoggle)
	{
		__TempAddr.B.h = V & 0x3f;
	}
	else
	{
		__TempAddr.B.l = V;
		RefreshAddr = TempAddr;
	}
      
	vtoggle ^= 1;
}

void B2007(word A, byte V)
{  
	static int32 VAddrB;

	VAddrB = RefreshAddr;
	__RefreshAddr.B.h &= 0x3f;
                        
	if(__RefreshAddr.B.h == 0x3F)
	{
		// This is so most likely quite wrong.
		if(!(__RefreshAddr.B.l & 0xf))
			VRAM[0x3f00] = VRAM[0x3f04] = VRAM[0x3f08] = VRAM[0x3f0C] =
				VRAM[0x3f10] = VRAM[0x3f14] = VRAM[0x3f18] = VRAM[0x3f1c] = V&0x3f;
		else if(RefreshAddr & 3) 
			VRAM[0x3f00 + (RefreshAddr & 0x1f)] = V & 0x3f;
	}
	else
	{
		if(__RefreshAddr.B.h < 0x20)
		{
			if(VPAL[__RefreshAddr.B.h >> 2])
				VPage[__RefreshAddr.B.h >> 2][RefreshAddr] = V;
	
			if(PPU_hook) 
				PPU_hook(RefreshAddr);
		}
	}
    
	if((__RefreshAddr.B.h & 0x30) == 0x20) 
	{                         
		if(!(VPAL2[(__RefreshAddr.B.h & 0xF) >> 2] & 0x800))
		{
			vnapage[((__RefreshAddr.B.h & 0xF) >> 2)][RefreshAddr & 0x3FF] = V;
		} 
	}

	RefreshAddr = VAddrB;
                  
	if(INC32) 
		RefreshAddr += 32;
	else 
		RefreshAddr++;
}


void B4014(word A, byte V)
{                        
	PPU[3] = 0;
	memcpy(SPRAM, &Page[V >> 5][V << 8], 256);
	M.ICounta += 512;
}

void B4016(word A, byte V)
{
	if((PSG[0x16] & 1) && (!(V & 1)))
	{
		joy_readbit = joy2_readbit = 0;
	}
	
	PSG[0x16] = V;
}

/*	This is called at the beginning of each scanline	*/

void Loop6502(void)
{
	int x;
	static int SkipFrames = 3;


	if(PCMIRQCount)
	{
		PCMIRQCount--;
		
		if(!PCMIRQCount)
		{
			vdis = 1;
			if(PSG[0x10] & 0x80 && !(PSG[0x10] & 0x40))
			{
				extern byte PCMIRQ; 
				PCMIRQ=0x80; 
				TriggerIRQ();
			}
		}
	}


	if(scanline <= 239 && scanline && (SkipFrames == 0))
	{
		XBuf += (scanline << 8) + (scanline << 4) + 8;

		if(ScreenON || SpriteON)
		{
			if(ScreenON)
			{
				if(scanline >= 8 || PAL)
				{
					RefreshLine();
					if(!(PPU[1] & 2))
					dwmemset(XBuf, (Pal[0] | 64), 8);
				}
				else
				{
					if(PPU_hook)
						PRefreshLine();
				}
			}
			else
				dwmemset(XBuf, (Pal[0] | 64), 264);        


			if (SpriteON)
				RefreshSprite();

			if(PPU[1] & 0x01)
			{
				for(x = 0; x < 64; x++)
					*(unsigned long *)&XBuf[x<<2] = 
						(*(unsigned long*)&XBuf[x<<2]) & 0xF0F0F0F0;
			}
     
			if(PPU[1] & 0xE0)
			{
				for(x = 0; x < 64; x++)
					*(unsigned long *)&XBuf[x<<2] =
						(*(unsigned long*)&XBuf[x<<2]) | 0xC0C0C0C0;
			}
		}
		else
			dwmemset(XBuf, Pal[0], 256);        

		XBuf-=(scanline<<8)+(scanline<<4)+8;
	}

	if(scanline == 239)
	{
		unsigned long J; 
		uint8 g;
		J = Joysticks();
		if(GameAttributes & GA_VSUNI)
			J = (J & 0xFFFF0c0C) | ((J & 0xF3) << 8) | ((J & 0xF300) >> 8);

		joy_1 = (J & 0xFF) | ((J & 0xFF0000) >> 8);
		J &= 0xFF00FF00;
		joy_2 = (J >> 8) | (J >> 16);
                
		if(Exit)
		{
			TrashFCE();
			TrashMachine();
			exit(0);
		}
		

		SkipFrames--;
		if(SkipFrames == -1)
			SkipFrames = 3;
		
		if(SkipFrames == 0)
			PutImage();

		RawC = (RawC + 1) & 31;
		RawWrite[RawC] = 0;
		g = 0;
                
		for(J = 0; J < 32; J++)
			if(RawWrite[J] > 15) 
				g |= 1;

		if(g && !RawPCMOn) 
		{
			RawPCMCount.DW.h = timestamp;
			RawPCMCount.DW.l = 0;
		}
         
		RawPCMOn = g;
	}
}


#define PAL(c)  ((c)+cc)


static void PRefreshLine(void)
{
	unsigned long vofs;
	byte X1;

	vofs = 0;
	if (BGAdrHI) 
		vofs = 0x1000;
	
	vofs += (RefreshAddr >> 12) & 7;
	
	for(X1 = 33; X1; X1--)
	{
		register byte no;
		register byte zz2;

		zz2 = (byte)((RefreshAddr >> 10) & 3);
		no  = vnapage[zz2][(RefreshAddr & 0x3ff)];
		PPU_hook((no << 4) + vofs);
		if((RefreshAddr & 0x1f) == 0x1f)
			RefreshAddr ^= 0x41F;
		else
			RefreshAddr++;
	}
}



/*              Update scanline in 32 pixel blocks - 4 tiles */

/*              Total of 33 tiles(32 + 1 extra) */

void RefreshLine(void)
{
#define vofs __vofs.DW
	
	dpair __vofs;
	byte X1;
	register byte *P; 
	vofs = 0;
	Pal[0] |= 64;
	Pal[4] |= 64;
	Pal[8] |= 64;
	Pal[0xC] |= 64;
	
	P = XBuf;
	__vofs.B.h = PPU[0];
	P -= XOffset;
	__vofs.B.h &= 0x10;
	vofs += (__RefreshAddr.B.h >> 4) & 7;

	if(MapperNo == 5 && mapbyte4[2] == 1)
	{
        for(X1 = 33; X1; X1--, P += 8){
			byte *C;                                   
			register byte cc;
			register byte zz,zz2;
			unsigned long vadr;  

			C = VROM;
			zz = (byte)(__RefreshAddr.B.l & 0x1F);
			zz2 = (byte)((__RefreshAddr.B.h >> 2) & 3);
			
			//cc=(byte)((((vnapage[zz2][0x3c0+(zz>>2)+((RefreshAddr&0x380)>>4)])>> ((zz&2) + ((__RefreshAddr.B.l&0x40)>>4))&3)<<2));
			vadr = (vnapage[zz2][RefreshAddr & 0x3ff] << 4) + vofs;
			C += (((MapperExRAM[RefreshAddr & 0x3ff]) & 0x3f & vmask4) * 4096) + (vadr & 0xfff);
			vadr = (MapperExRAM[RefreshAddr & 0x3ff] & 0xC0) >> 4;
			cc = vadr;
			
#include "fceline.h"

			if((__RefreshAddr.B.l & 0x1f) == 0x1f)
				RefreshAddr ^= 0x41F;
			else
				__RefreshAddr.B.l++;
		}
	}
	else if(PPU_hook)
	{
        for(X1 = 33; X1; X1--, P += 8){
			byte *C;                                   
			register byte cc;
			register byte zz,zz2;
			unsigned long vadr;
			
			zz = (byte)(__RefreshAddr.B.l & 0x1F);
			zz2 = (byte)((__RefreshAddr.B.h >> 2) & 3);
			cc = (byte)((((vnapage[zz2][0x3c0 + (zz >> 2) + ((RefreshAddr & 0x380) >> 4)]) >> ((zz & 2) + ((__RefreshAddr.B.l & 0x40) >> 4)) & 3) << 2));
			vadr = (vnapage[zz2][RefreshAddr & 0x3ff] << 4) + vofs;
			//PPU_hook(vadr);
			C = VRAMADR(vadr);
			//PPU_hook(vadr);
/*
#ifdef C80x86
#include "fcelineasm.h"
			PPU_hook(vadr);
#else
*/
#include "fceline.h"
//#endif
			if((__RefreshAddr.B.l & 0x1f) == 0x1f)
				RefreshAddr ^= 0x41F;
			else
				__RefreshAddr.B.l++;
		}
	}
	else
	{
        for(X1 = 33; X1; X1--, P += 8)
		{
			byte *C;
			register byte cc;
			register byte zz,zz2;
			unsigned long vadr;

			zz = (byte)(__RefreshAddr.B.l & 0x1F);
			zz2 = (byte)((__RefreshAddr.B.h >> 2) & 3);
			cc = (byte)((((vnapage[zz2][0x3c0 + (zz >> 2) + ((RefreshAddr & 0x380) >> 4)]) >> ((zz & 2) + ((__RefreshAddr.B.l & 0x40) >> 4)) & 3) << 2));
			vadr = (vnapage[zz2][RefreshAddr & 0x3ff] << 4) + vofs;
			
			if(MapperNo == 5)
				C = MMC5BGVRAMADR(vadr);
			else
				C = VRAMADR(vadr);
#include "fceline.h"
			
			if((__RefreshAddr.B.l & 0x1f) == 0x1f)
				RefreshAddr ^= 0x41F;
			else
				__RefreshAddr.B.l++;
		}
	}
#undef vofs
	Pal[0] &= 63;
	Pal[4] &= 63;
	Pal[8] &= 63;
	Pal[0xC] &= 63;
}

/*	This is called at the beginning of each hblank */
void hcheck(void)
{
	if(!scanline) 
		if(ScreenON || SpriteON) 
			RefreshAddr = TempAddr;
		
	scanline = (scanline + 1) % scanlines_per_frame;
		
	if(scanline == 241) 
	{
		if(MapperNo == 666)
		{
			if(VBlankON)
				TriggerNMINSF();
		}
		else
		{
			toevent = 204;
			PPUEvent = 0;
		}
	}
		
	if(MapHBIRQHook) 
		MapHBIRQHook();
       
	if(scanline == scanlines_per_frame - 1)
		PPU_status &= 0x1f;
		
		
	if((ScreenON || SpriteON) && scanline <= 239)
	{		
		RefreshAddr &= 0xFBE0;
		__RefreshAddr.B.h |= __TempAddr.B.h & 0x04;
		__RefreshAddr.B.l |= __TempAddr.B.l & 0x1f; 
			
		if((__RefreshAddr.B.h & 0x70) == 0x70)
		{
			__RefreshAddr.B.h ^= 0x70;
			if((RefreshAddr & 0x3E0) == 0x3A0)
			{
				RefreshAddr ^= 0x3A0;
				RefreshAddr ^= 0x800;
			}
			else
			{
				if((RefreshAddr & 0x3E0) == 0x3e0)
					RefreshAddr ^= 0x3e0;
				else 
					RefreshAddr += 0x20;
			}
		}
		else
			RefreshAddr += 0x1000;
	}
}

#define	V_FLIP	0x80
#define	H_FLIP	0x40
#define	SP_BACK	0x20

typedef struct {
	byte y, no, atr, x;
} SPR;


uint8 sprlinebuf[256 + 8];        

static void RefreshSprite(void)
{
	int n, vofs;
	byte H, nosprites, SpriteBlurp;
	SPR *spr;
	byte *P;
	
	nosprites = SpriteBlurp = 0;
	H = 8;
	
	vofs = (unsigned int)(PPU[0] & 0x8 & (((PPU[0] & 0x20) ^ 0x20) >> 2)) << 9;
	H += (PPU[0] & 0x20) >> 2;
	spr = (SPR*)SPRAM;
	dwmemset(sprlinebuf, 128, 256);
	
	if(!PPU_hook)
        for(n = 0; n < 64; n++, spr++)
        {
			int t;
			byte y;
			
			y = spr->y + 1;
			t = (int)scanline - y;
			
			if(t >= H) 
				continue;
			
			if(t < 0 || y == 0)
				continue;
			
			if(nosprites < 8)
			{
				if(!n) 
					SpriteBlurp = 1;

				*(unsigned long *)&SPRBUF[nosprites << 2] = *(unsigned long *)&SPRAM[n << 2];
				nosprites++;
			}
			else
			{
				PPU_status |= 0x20;
				break;
			}
        }
	else
		for(n = 0; n < 64; n++, spr++)
		{
			int t;
			byte y;
			
			y = spr->y + 1;
			t = (int)scanline - y;
			
			if(t >= H)
				continue;

			if(t < 0 || y == 0)
				continue;
			
			if(nosprites < 8)
			{
				if(!n) 
					SpriteBlurp = 1;

				*(unsigned long *)&SPRBUF[nosprites << 2] = *(unsigned long *)&SPRAM[n << 2];
				if(PPU_hook)
				{
					if(Sprite16)
						PPU_hook(((SPRBUF[(nosprites << 2) + 1] & 1) << 12)+
							((SPRBUF[(nosprites << 2) + 1] & 0xFE) << 4));
					else
						PPU_hook((SPRBUF[(nosprites << 2) + 1] << 4) + vofs);
				}
				nosprites++;
			}
			else
			{
				PPU_status |= 0x20;
				break;
			}
		}
		
		
	if(!nosprites)
		return;

	nosprites--;
	spr = (SPR*)SPRBUF + nosprites;
	P = XBuf;
	
	for(n = nosprites; n >= 0; n--, spr--)
	{	
		register byte J, c1, c2;
		byte x, no, atr;//,cc;
		byte *C;
		byte *VB;
		int t;
		char inc;
		unsigned int vadr;

		t = (int)scanline - (spr->y) - 1;
		x = spr->x;
		no = spr->no;
		atr = spr->atr;
		//cc = ((atr&3)<<2);
		if(Sprite16)
			vadr = ((no & 1) << 12) + ((no & 0xFE) << 4);
		else
			vadr = (no << 4) + vofs;
		
		if(MapperNo == 5) 
			C = MMC5SPRVRAMADR(vadr);
		else 
			C = VRAMADR(vadr);
			
		P += x;
		inc = 1;
		if(atr & V_FLIP) 
		{
			inc = -1;
			C += 7;
			C += (PPU[0] & 0x20) >> 1;
		}

		C += t * inc;
		if(t >= 8)
			C += inc << 3;
			
		c1 = ((C[0] >> 1) & 0x55) | (C[8] & 0xAA);
		c2 = (C[0] & 0x55) | ((C[8] << 1) & 0xAA);
		J = C[0] | C[8];
			
		if(J)
		{
			if(n == 0 && SpriteBlurp)
			{  
				toevent = x; 
				PPUEvent = 0x40;
				//PPU_status|=0x40;
			}
			VB = (VRAM + 0x3f10) + ((atr & 3) << 2);
			if(atr & SP_BACK) 
			{
				if(atr & H_FLIP)
				{
					if(J & 0x02)
						sprlinebuf[x + 1] = VB[c1 & 3] | 0x40;
					if(J & 0x01)  
						sprlinebuf[x] = VB[c2 & 3] | 0x40;

					c1 >>= 2;
					c2 >>= 2;

					if(J & 0x08)
						sprlinebuf[x + 3] = VB[c1 & 3] | 0x40;
					if(J & 0x04)
						sprlinebuf[x + 2] = VB[c2 & 3] | 0x40;

					c1 >>= 2;                                            
					c2 >>= 2;
						
					if(J & 0x20)
						sprlinebuf[x + 5] = VB[c1 & 3] | 0x40;
					if(J & 0x10) 
						sprlinebuf[x + 4] = VB[c2 & 3] | 0x40;

					c1 >>= 2;
					c2 >>= 2;
						
					if(J & 0x80)
						sprlinebuf[x + 7] = VB[(c1)] | 0x40;
					if(J & 0x40) 
						sprlinebuf[x + 6] = VB[(c2)] | 0x40;
				} 
				else
				{
					if(J & 0x02)
						sprlinebuf[x + 6] = VB[c1 & 3] | 0x40;
					if(J & 0x01)
						sprlinebuf[x + 7] = VB[c2 & 3] | 0x40;

					c1 >>= 2;           
					c2 >>= 2;

					if(J & 0x08)
						sprlinebuf[x + 4] = VB[c1 & 3] | 0x40;
					if(J & 0x04)
						sprlinebuf[x + 5] = VB[c2 & 3] | 0x40;

					c1 >>= 2;
					c2 >>= 2;

					if(J & 0x20)
						sprlinebuf[x + 2] = VB[c1 & 3] | 0x40;
					if(J & 0x10)  
						sprlinebuf[x + 3] = VB[c2 & 3] | 0x40;

					c1 >>= 2;
					c2 >>= 2;

					if(J & 0x80) 
						sprlinebuf[x] = VB[c1] | 0x40;
					if(J & 0x40)  
						sprlinebuf[x + 1] = VB[c2] | 0x40;
				}
			}
			else 
			{
				if(atr & H_FLIP)
				{
					if (J&0x02) {sprlinebuf[x+1]=VB[(c1&3)];}
					if (J&0x01) {sprlinebuf[x]=VB[(c2&3)];}
					c1>>=2;
					c2>>=2;
					if (J&0x08) {sprlinebuf[x+3]=VB[(c1&3)];}
					if (J&0x04) {sprlinebuf[x+2]=VB[(c2&3)];}
					c1>>=2;
					c2>>=2;
					if (J&0x20) {sprlinebuf[x+5]=VB[c1&3];}
					if (J&0x10) {sprlinebuf[x+4]=VB[c2&3];}
					c1>>=2;
					c2>>=2;
					if (J&0x80) {sprlinebuf[x+7]=VB[(c1)];}
					if (J&0x40) {sprlinebuf[x+6]=VB[(c2)];}
				}
				else
				{                    
					if (J&0x02) {sprlinebuf[x+6]=VB[(c1&3)];}
					if (J&0x01) {sprlinebuf[x+7]=VB[(c2&3)];}
					c1>>=2;
					c2>>=2;
					if (J&0x08) {sprlinebuf[x+4]=VB[(c1&3)];}
					if (J&0x04) {sprlinebuf[x+5]=VB[(c2&3)];}
					c1>>=2;                
					c2>>=2;
					if (J&0x20) {sprlinebuf[x+2]=VB[(c1&3)];}
					if (J&0x10) {sprlinebuf[x+3]=VB[(c2&3)];}
					c1>>=2;
					c2>>=2;
					if (J&0x80) {sprlinebuf[x]=VB[(c1)];}
					if (J&0x40) {sprlinebuf[x+1]=VB[(c2)];}
				}
			}
		}
		P -= x;
	}

	n = ((PPU[1] & 4) ^ 4) << 1;

	for(; n < 256; n++)
	{
		if(sprlinebuf[n] != 128)
		{
			if(!(sprlinebuf[n] & 0x40))       // Normal sprite
				P[n] = sprlinebuf[n];
			else if(P[n] & 64)        // behind bg sprite
				P[n] = sprlinebuf[n];
		}
	}
}



void ResetMapper(void)
{
	int x;

	for(x = 0; x < 8; x++)
	{
		Page[x] = nothing - 8192 * x;
		VPage[x] = nothing - 0x400 * x;
	}
	
	Page[0] = Page[8] = RAM;
	Page[3] = WRAM - 0x6000;
	
	
	SetReadHandler(0, 0xFFFF, (void *)ANull);
	SetWriteHandler(0, 0xFFFF, (void *)BNull);
	
	SetReadHandler(0, 0x1FFF, (void *)ARAM);
	SetWriteHandler(0, 0x1FFF, (void *)BRAM);
	
	SetReadHandler(0x6000, 0x7FFF, (void *)AWRAM);
	SetWriteHandler(0x6000, 0x7FFF, (void *)BWRAM);
	
	SetReadHandler(0x8000, 0xFFFF, (void *)AROM);
	
	for(x = 0x2000; x < 0x4000; x += 8)
	{
        ARead[x] = (void *)A2000;
        BWrite[x] = (void *)B2000;
        ARead[x+1] = (void *)A2001;
        BWrite[x+1] = (void *)B2001;
        ARead[x+2] = (void *)A2002;
        ARead[x+3] = (void *)A2003;
        BWrite[x+3] = (void *)B2003;
        ARead[x+4] = (void *)A2004;
        BWrite[x+4] = (void *)B2004;
        ARead[x+5] = (void *)A2002;
        BWrite[x+5] = (void *)B2005;
        ARead[x+6] = (void *)A2002;
        BWrite[x+6] = (void *)B2006;
        ARead[x+7] = (void *)A2007;
        BWrite[x+7] = (void *)B2007;
	}
	
	SetReadHandler(0x4000, 0x4013, (void *)Read_PSG);
	SetWriteHandler(0x4000, 0x4013, (void *)Write_PSG);
	
	BWrite[0x4014] = (void *)B4014;
	BWrite[0x4015] = (void *)Write_PSG;
	BWrite[0x4016] = (void *)B4016;
	BWrite[0x4017] = (void *)Write_PSG;
	ARead[0x4015] = (void *)Read_PSG;
	ARead[0x4016] = (void *)A4016;
	ARead[0x4017] = (void *)A4017;
}

void ResetNES(void)
{
	VRAMBuffer = PPU[0] = PPU[1] = PPU[2] = PPU[3] = 0;
	toevent = -1;
	ResetMapper();
	GameInterface(GI_RESET);
	ResetSound();
	Reset6502();
}

int InitFCE(char *name)
{
	byte z;
	FILE *fp;
	//puts("\nStarting FCE Ultra...\n");
	
	fp = fopen(name,"rb");
	if (fp == NULL)
	{
		sprintf(TempArray, "File \"%s\" not found.\n", name);
		PrintFatalError(TempArray);
		return 0;
	}
	
	if((z = iNESLoad(name,fp)))
	{
		switch(z)
		{
			default:        
			case 1: 
				switch(NSFLoad(fp))
				{
					case 1: 
						switch(FDSLoad(name,fp))
						{
							case 1: 
								sprintf(TempArray,"File \"%s\" is in an unrecognized format.",name);
								goto mrevil;
								break;
							case 3: 
								sprintf(TempArray,"FDS BIOS ROM image not found.");
								goto mrevil;
								break;
							case 4: 
								sprintf(TempArray,"Error reading FDS BIOS ROM image.");
								goto mrevil;
								break;
							case 2: 
								goto ilicktoads;
						}
						break;
					case 2: 
						goto ilicktoads;
				} 
				break;

			case 2: 
ilicktoads: 
				sprintf(TempArray,"Error reading from file \"%s\"",name);
				goto mrevil;
		}
		goto noevil;
mrevil:
		PrintFatalError(TempArray);
		fclose(fp);
		return 0;
	}
noevil:
	fclose(fp);
	
	PowerNES();
	return 1;
}

void RunFCE(void)
{
	Run6502();
}
 

void TrashFCE(void)
{
	HaltSound(1);
	GameInterface(GI_CLOSE);
}

void InitPAL(void)
{
	if(MapperNo != 666) 
		maxline = 240;

	PAL = 1;
	IPeriod = (78 * 3);
	scanlines_per_frame = 313;
	aha = 241;
}

//#ifdef ALTERNATE_MAIN
//#define main fceumain
//#endif

// arguments to the emulator
char *argv[64];
int argc;

//int main(int argc, char *argv[])
int fceumain(void)
{
	// strip off filename from path of first argument to get directory of
	// executable. This is used to find the config file (in the same directory).
//	GetBaseDirectory(BaseDirectory, argv[0]);

	// if there is only one argument, we have an error.
//	if(argc > 1)
		// save the base filename of the game (without the extension)
//		GetFileBase(argv[argc - 1]);
//	else 
		// output usage info and exit
//		CheckArgs();

#ifndef WINDOWS
//        sprintf(TempArray,"%s\\fceu.cfg",BaseDirectory);
//        LoadConfig(TempArray);
#endif
//	DoArgs(argc, argv);
 
 
	//if(InitFCE(argv[argc - 1])) 
	if(InitFCE("d:\\default.nes"))
	{
		if(!InitMachine()) 
		{
			TrashMachine();
			return 1;
		}
        
		RunFCE();
		TrashFCE();
		TrashMachine();
	}

	return 0;
}

void PowerNES(void) 
{
	M.HPeriod = 85;
	M.IPeriod = IPeriod;
	
	RefreshAddr = TempAddr = 0;
	vtoggle = 0;
	joy_readbit = joy2_readbit = joy_1 = joy_2 = 0;
	scanline = 263;
	
	memset(VRAM, 0x00, 0x4000);
	memset(RAM, 0x00, 0x800);
	memset(SPRAM, 0x00, 0x100);
	
	GameInterface(GI_POWER);
	ResetNES();
}
