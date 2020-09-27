
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "m6502.h"
#include "fce.h"
#include "mapper.h"
#include "svga.h"
#include "video.h"
#include "sound.h"
#include "nsf.h"
#include "nsfbgnew.h"
#include "general.h"

#pragma warning( disable : 4244 )

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

byte SongReload=1;
byte CurrentSong;

static int sinetable[32];


static byte NSFROM[]=
{
	
	/* 0x00 */
	
	0x08,0x48,0x8A,0x48,0x98,0x48,          /* Store regs           */
		0xA9,0xFF,
		0x8D,0xF2,0x5F,				/* NMI has occured	*/
		0x68,0xA8,0x68,0xAA,0x68,0x28,
		0x40,     /* Restore regs         */
		
		/* 0x12 */
		
		0xAD,0xF2,0x5F,				/* See if an NMI occured */
		0xF0,0xFB,				/* If it hasn't, loop    */
		
		0xA9,0x00,
		0x8D,0xF2,0x5F,				/* Clear play pending reg*/
		
		
		0xAD,0xF0,0x5F,                         /* See if we need to init. */
		0xF0,0x09,                              /* If 0, go to JMP         */
		
		0xAD,0xF1,0x5F,                         /* Confirm and load A      */
		0xAE,0xF3,0x5F,                         /* Load X with PAL/NTSC byte */
		
		0x20,0x00,0x00,                         /* JSR to init routine     */
		
		0x20,0x00,0x00,                         /* JSR to play routine  */
		
		0x4C,0x12,0x20                          /* Loop                 */
};



static int NSFMaxBank;

static byte *NSFmmap;
static int NSFSize;
static byte BSon;
static word PlayAddr;
static word InitAddr;
static word LoadAddr;

NSF_HEADER NSFHeader;

void NSFGI(int h)
{
	switch(h)
	{
	case GI_CLOSE:
		free(NSFmmap);break;
	case GI_RESET: NSF_init();break;
	}
}

int NSFLoad(FILE *fp)
{
	int x;
	ssync|=2;
	fseek(fp,0,SEEK_SET);
	fread(&NSFHeader,1,0x80,fp);
	if (memcmp(NSFHeader.ID,"NESM\x1a",5))
		return 1;
	NSFHeader.SongName[31]=NSFHeader.Artist[31]=NSFHeader.Copyright[31]=0;
	
	LoadAddr=NSFHeader.LoadAddressLow;
	LoadAddr|=NSFHeader.LoadAddressHigh<<8;
	
	InitAddr=NSFHeader.InitAddressLow;
	InitAddr|=NSFHeader.InitAddressHigh<<8;
	
	PlayAddr=NSFHeader.PlayAddressLow;
	PlayAddr|=NSFHeader.PlayAddressHigh<<8;
	
	NSFmmap=(byte*)malloc(32768*2);
	
	fseek(fp,0,SEEK_END);
	NSFSize=ftell(fp)-0x80;
	
	ROM=(byte *)malloc(NSFSize+(LoadAddr&0xfff)+32768);
	
	NSFMaxBank=((NSFSize+32768)/4096)-1;
	
	fseek(fp,0x80,SEEK_SET);
	memset(ROM,0x00,NSFSize+(LoadAddr&0xfff));
	fread(ROM+(LoadAddr&0xfff),1,NSFSize,fp);
	
	BSon=0;
	for(x=0;x<8;x++)
		BSon|=NSFHeader.BankSwitch[x];
	if(BSon)
	{
		for(x=0;x<8;x++)
		{
			if(NSFHeader.BankSwitch[x]<=NSFMaxBank)
				memcpy(NSFmmap+(x*4096),
				ROM+(NSFHeader.BankSwitch[x]*4096),4096);
		}
	}
	else
		memcpy(NSFmmap+(LoadAddr&0x7000),ROM,32768-(LoadAddr&0x7000));
	
	MapperNo=666;
	MIRROR_SET2(0);
	VROM_size=0;
	for(x=0;;x++)
	{
		if(NSFROM[x]==0x20)
		{
			NSFROM[x+1]=InitAddr&0xFF;
			NSFROM[x+2]=InitAddr>>8;
			NSFROM[x+4]=PlayAddr&0xFF;
			NSFROM[x+5]=PlayAddr>>8;
			break;
		}
	}
	
	CurrentSong=NSFHeader.StartingSong;
	if(NSFHeader.VideoSystem&1) InitPAL();
	for(x=0;x<32;x++)
	{
		static double fruit=0;
		if(x&15)
			sinetable[x]=sin(fruit)*8;
		else
			sinetable[x]=0;
		fruit+=(double)M_PI*2/32;
	}
	GameInterface=(void *)NSFGI;
	MapStateRestore=0;
	PPU_hook=0;
	MapHBIRQHook=0;
	MapExpSound=0;
	return 0;
}


void NSF_init(void)
{
	Page[4]=Page[5]=Page[6]=Page[7]=NSFmmap-0x8000;
	Page[1]=NSFROM-0x2000;
	B2000(0x2000,0x80);
	memset(WRAM,0x00,8192);  
	SetWriteHandler(0x2000,0x3fff,(void *)BNull);
	SetReadHandler(0x2000,0x3fff,(void *)AROM);
	
	SetWriteHandler(0x4020,0x5fff,(void *)NSF_write);
	SetReadHandler(0x4020,0x5fff,(void *)NSF_read);
	
	if(NSFHeader.SoundChip&1) { 
		SetWriteHandler(0x9000,0xb002,(void *)Mapper24_write);
		MapExpSound=(void *)VRC6Sound;
	} else if (NSFHeader.SoundChip&2) {
		SetWriteHandler(0x9010,0x9010,(void *)Mapper85_write);
		SetWriteHandler(0x9010,0x9030,(void *)Mapper85_write);
		MapExpSound=(void *)VRC7Sound;
	} else if (NSFHeader.SoundChip&8) {
		SetWriteHandler(0x5000,0x5015,(void *)Mapper5_write);
		SetWriteHandler(0x5205,0x5206,(void *)Mapper5_write);
		SetReadHandler(0x5205,0x5206,(void *)MMC5_read);
		
		SetWriteHandler(0x5c00,0x5fef,(void *)MMC5_ExRAMWr);
		SetReadHandler(0x5c00,0x5fef,(void *)MMC5_ExRAMRd);
		MapExpSound=(void *)MMC5Sound;
	} else if (NSFHeader.SoundChip&0x10) {
		SetWriteHandler(0x4800,0x4fff,(void *)Mapper19_write);
		SetReadHandler(0x4800,0x4fff,(void *)Namco_Read);
		
		SetWriteHandler(0xf800,0xffff,(void *)Mapper19_write);
		SetReadHandler(0xf800,0xffff,(void *)Namco_Read);
		
		MapExpSound=(void *)NamcoSound;
	} else if (NSFHeader.SoundChip&0x20) {
		SetWriteHandler(0xc000,0xffff,(void *)Mapper69_write);
		MapExpSound=(void *)AYSound;
	}
}

static byte DoUpdateStuff=0;
void NSF_write(word A, byte V)
{
	switch(A)
	{
	case 0x5FF2:if((M.PC.W&0xF000)==0x2000) DoUpdateStuff=V;break;
		
	case 0x5FF8:
	case 0x5FF9:
	case 0x5FFA:
	case 0x5FFB:
	case 0x5FFC:
	case 0x5FFD:
	case 0x5FFE:
	case 0x5FFF:if(!BSon) return;
		A&=7;
		if(V>NSFMaxBank) return;
		memcpy(NSFmmap+(A*4096),ROM+(V*4096),4096);
		break;
	}
}

byte NSF_read(word A)
{
	int x;
	if((M.PC.W&0xF000)==0x2000)
		switch(A)
	{
 case 0x5ff0:x=SongReload;SongReload=0;return x;
 case 0x5ff1:
	 {
		 memset(RAM,0x00,0x800);
		 memset(WRAM,0x00,8192);
		 Write_PSG(0x15,0xF);
		 for(x=0;x<0x14;x++)
		 {Write_PSG(x,0);}
		 Write_PSG(0x15,0x0);
		 for(x=0;x<0x14;x++)
		 {Write_PSG(x,0);}
		 Write_PSG(0x11,0x40);
		 Write_PSG(0x15,0xF);
		 Write_PSG(0x17,0x40);
		 if(BSon)
		 {
			 for(x=0;x<8;x++)
			 {
				 if(NSFHeader.BankSwitch[x]<=NSFMaxBank)
					 memcpy(NSFmmap+(x*4096),
					 ROM+(NSFHeader.BankSwitch[x]*4096),4096);
			 }
		 }
		 return (CurrentSong-1);
	 }
 case 0x5FF2:return DoUpdateStuff;
 case 0x5FF3:return PAL;
	}
	return 0;
}

static int32 *Bufpl;
void DrawNSF(void)
{
	static int z=0;
	int x,y;
	
	for(y=0;y<240;y++)
	{
		int offs;
		offs=sinetable[((z+y)>>2)&31];
		memcpy(XBuf+(y<<8)+(y<<4),NSFBG+8+offs+(y<<8)+(y<<4),256);
	}
	z=(z+1)&127;
	
	DrawTextTrans(XBuf+10*272+4+(((31-strlen(NSFHeader.SongName))<<2)), 272, NSFHeader.SongName, 255);
	DrawTextTrans(XBuf+30*272+4+(((31-strlen(NSFHeader.Artist))<<2)), 272, NSFHeader.Artist, 255);
	DrawTextTrans(XBuf+50*272+4+(((31-strlen(NSFHeader.Copyright))<<2)), 272, NSFHeader.Copyright, 255);
	
	DrawTextTrans(XBuf+90*272+4+(((31-strlen("Song:"))<<2)), 272, "Song:", 255);
	sprintf(TempArray,"<%d/%d>",CurrentSong,NSFHeader.TotalSongs);
	DrawTextTrans(XBuf+102*272+4+(((31-strlen(TempArray))<<2)), 272, TempArray, 255);
	
	if(GetSoundBuffer(&Bufpl))
	{
		for(x=0;x<256;x++)
			XBuf[x+(224-(((((Bufpl[x]>>(OVERSAMPLESHIFT+1))^128)&255)*3)>>3))*272]=255;
	}
	else
	{
		for(x=0;x<256;x++)
			XBuf[x+(224-((((Bufpl[x]>>(OVERSAMPLESHIFT)^128)&255)*3)>>3))*272]=255;
	}
}

void NSFControl(int z)
{
	switch(z)
	{
	case 0:SongReload=0xFF;break;
	case 1:if(CurrentSong<NSFHeader.TotalSongs) CurrentSong++; 
		SongReload=0xFF;break;
	case 2:if(CurrentSong>1) CurrentSong--;
		SongReload=0xFF;break;
	}
}
