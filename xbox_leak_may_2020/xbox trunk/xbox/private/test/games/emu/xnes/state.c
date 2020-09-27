#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "version.h"
#include "m6502.h"
#include "sound.h"
#include "fce.h"
#include "mapper.h"
#include "svga.h"
#include "endian.h"
#include "fds.h"
#include "general.h"

/*	Really need to clean this up. */

byte StateName[2048];
byte StateFile[2048];
byte CurrentState=0;

void SaveState(void)
{
	FILE *st=NULL;
	int x;
	byte version;
	int32 nada;
	nada=0;
	
	if(genie&1)
	{
		sprintf(errmsg,"Cannot save FCS in GG screen.");
		howlong=180;
		return;
	}
	sprintf(TempArray,"%s\\fcs\\%s.fc%d",BaseDirectory,StateFile,CurrentState);
	st=fopen(TempArray,"wb");
	if(st!=NULL)
	{
		HaltSound(0);
		fwrite(&M.PC.B.l,1,1,st);
		fwrite(&M.PC.B.h,1,1,st);
		fwrite(&M.A,1,1,st);
		fwrite(&M.P,1,1,st);
		fwrite(&M.X,1,1,st);
		fwrite(&M.Y,1,1,st);
		fwrite(&M.S,1,1,st);
		
		version=VERSION_NUMERIC;
		fwrite(&version,1,1,st);
		
		write32(nada,st);
		write32(nada,st);
		fwrite(&nada,1,1,st);
		fwrite(&nada,1,1,st);
		fwrite(&nada,1,1,st);
		fwrite(&nada,1,1,st);
		write32(nada,st);
		fwrite(&nada,1,1,st);
		fwrite(&nada,1,1,st);
		fwrite(&nada,1,1,st);
		fwrite(&nada,1,1,st);
		for(x=0;x<8;x++)
			writeupper8of16(CHRBankList[x],st);
		fwrite(PRGBankList,4,1,st);
		for(x=0;x<8;x++)
			writelower8of16(CHRBankList[x],st);
		fwrite(VRAM,1,0x4000,st);
		fwrite(PPU,1,4,st);
		
		fwrite(SPRAM,1,0x100,st);
		fwrite(WRAM,1,8192,st);
		fwrite(RAM,1,0x800,st);
		write16(scanline,st);
		write16(RefreshAddr,st);
		fwrite(&VRAMBuffer,1,1,st);
		fwrite(&IRQa,1,1,st);
		write32(IRQCount,st);
		write32(IRQLatch,st);
		fwrite(&Mirroring,1,1,st);
		fwrite(PSG,1,0x17,st);
		
		fwrite(decvolume,1,3,st);
		
		fwrite(DecCountTo1,1,3,st);
		fwrite(SweepCount,1,2,st);
		write32(nada,st);
		fwrite(&nada,1,3,st);
		write32(nada,st);
		for(x=0;x<5;x++)
			write32(count[x],st);
		for(x=0;x<2;x++)
			write32(vcount[x],st);
		fwrite(&sqnon,1,1,st);
		for(x=2;x<16;x++)
			write32(vcount[x],st);
		fwrite(&triangleor,1,1,st);
		write32(PCMSizeIndex,st);
		write32(PCMAddressIndex,st);
		fwrite(&PCMBuffer,1,1,st);
		fwrite(&PCMBitIndex,1,1,st);
		
		fwrite(&PSG[0x17],1,1,st);
		fwrite(&IRQlow,1,1,st);
		write32(fhcnt,st);
		fwrite(&fcnt,1,1,st);
		fwrite(RAM,1,193-48-56-1-8-2-1-6,st);
		fwrite(&joy_1,1,1,st);
		fwrite(&joy_2,1,1,st);
		fwrite(&joy_readbit,1,1,st);
		fwrite(&joy2_readbit,1,1,st);
		fwrite(&joy1or,1,1,st);
		fwrite(&joy2or,1,1,st);
		fwrite(&XOffset,1,1,st);
		fwrite(VPAL,1,8,st);
		fwrite(mapbyte1,1,8,st);
		fwrite(mapbyte2,1,8,st);
		fwrite(mapbyte3,1,8,st);
		fwrite(mapbyte4,1,8,st);
		for(x=0;x<4;x++)
			write16(mapword1[x],st);
		for(x=0;x<4;x++)
			write16(VPAL2[x],st);
		fwrite(MapperExRAM,1,32768,st);
		fwrite(&vtoggle,1,1,st);
		write16(TempAddr,st);
		write16(RefreshAddr,st);
		
		SaveStateStatus[CurrentState]=1;
		FDSStateSave(st);
		UnHaltSound();
		fclose(st);
		sprintf(errmsg,"State %d saved.",CurrentState);howlong=180;
	}
	else sprintf(errmsg,"State %d save error.",CurrentState);howlong=180;
}
byte *StateBuffer;
unsigned short intostate;


static void afread(void *ptr, size_t _size, size_t _nelem)
{
	memcpy(ptr,StateBuffer+intostate,_size*_nelem);
	intostate+=_size*_nelem;
}


static void areadlower8of16(int8 *d)
{
#ifdef LSB_FIRST
	*d=StateBuffer[intostate++];
#else
	d[1]=StateBuffer[intostate++];
#endif
}


static void areadupper8of16(int8 *d)
{
#ifdef LSB_FIRST
	d[1]=StateBuffer[intostate++];
#else
	*d=StateBuffer[intostate++];
#endif
}


static void aread16(int8 *d)
{
#ifdef LSB_FIRST
	*d=StateBuffer[intostate++];
	d[1]=StateBuffer[intostate++];
#else
	d[1]=StateBuffer[intostate++];
	*d=StateBuffer[intostate++];
#endif
}


static void aread32(int8 *d)
{
#ifdef LSB_FIRST
	*d=StateBuffer[intostate++];
	d[1]=StateBuffer[intostate++];
	d[2]=StateBuffer[intostate++];
	d[3]=StateBuffer[intostate++];
#else
	d[3]=StateBuffer[intostate++];
	d[2]=StateBuffer[intostate++];
	d[1]=StateBuffer[intostate++];
	*d=StateBuffer[intostate++];
#endif
}


void LoadState(void)
{
	int x;
	FILE *st=NULL;
	int32 nada;
	byte version;
	nada=0;
	
	StateBuffer=malloc(59999);
	if(StateBuffer==NULL)
	{
		sprintf(errmsg,"Unable to allocate buffer.");howlong=180;
		return;
	}
	
	sprintf(TempArray,"%s\\fcs\\%s.fc%d",BaseDirectory,StateFile,
		CurrentState);
	
	st=fopen(TempArray,"rb");
	if(st!=NULL)
	{
		if(!fread(StateBuffer,59999,1,st))
		{
			sprintf(errmsg,"State %d load error.",CurrentState);howlong=180;
			fclose(st);
			SaveStateStatus[CurrentState]=0;
			return;
		}
	}
	else
	{
		sprintf(errmsg,"State %d load error.",CurrentState);howlong=180;
		SaveStateStatus[CurrentState]=0;
		return;
	}
	if(genie&1)
	{
		genie=0;
		ResetNES();
	}
	
	SaveStateStatus[CurrentState]=1;
	intostate=0;
	HaltSound(0);
	afread(&M.PC.B.l,1,1);
	afread(&M.PC.B.h,1,1);
	afread(&M.A,1,1);
	afread(&M.P,1,1);
	afread(&M.X,1,1);
	afread(&M.Y,1,1);
	afread(&M.S,1,1);
	afread(&version,1,1);
	afread(&nada,1,1);
	afread(&nada,1,1);
	afread(&nada,1,1);
	afread(&nada,1,1);
	aread32((int8 *)&M.ICount);
	afread(&nada,1,1);
	afread(&nada,1,1);
	afread(&nada,1,1);
	afread(&nada,1,1);
	aread32((int8 *)&nada);
	afread(&nada,1,1);
	afread(&nada,1,1);
	afread(&nada,1,1);
	afread(&nada,1,1);
	
	for(x=0;x<8;x++)
		areadupper8of16((int8 *)&CHRBankList[x]);
	afread(PRGBankList,4,1);
	if(MapperNo!=777)
	{
		ROM_BANK8(0x8000,PRGBankList[0]);
		ROM_BANK8(0xA000,PRGBankList[1]);
		ROM_BANK8(0xC000,PRGBankList[2]);
		ROM_BANK8(0xE000,PRGBankList[3]);
	}
	for(x=0;x<8;x++)
		areadlower8of16((int8 *)&CHRBankList[x]);
	if(MapperNo!=777)
	{
		VROM_BANK1(0x0000,CHRBankList[0]);
		VROM_BANK1(0x0400,CHRBankList[1]);
		VROM_BANK1(0x0800,CHRBankList[2]);
		VROM_BANK1(0x0c00,CHRBankList[3]);
		VROM_BANK1(0x1000,CHRBankList[4]);
		VROM_BANK1(0x1400,CHRBankList[5]);
		VROM_BANK1(0x1800,CHRBankList[6]);
		VROM_BANK1(0x1c00,CHRBankList[7]);
	}
	afread(VRAM,1,0x4000);
	for(x=0x3f00;x<0x3f20;x++)
		VRAM[x]&=0x3f;
	if(MapperNo!=777)
	{
		if(VROM_size==0)
		{
			VRAM_BANK1(0x0000,0);
			VRAM_BANK1(0x0400,1);
			VRAM_BANK1(0x0800,2);
			VRAM_BANK1(0x0c00,3);
			VRAM_BANK1(0x1000,4);
			VRAM_BANK1(0x1400,5);
			VRAM_BANK1(0x1800,6);
			VRAM_BANK1(0x1c00,7);
		}
	}
	afread(PPU,1,4);
	afread(SPRAM,1,0x100);
	afread(WRAM,1,8192);
	afread(RAM,1,0x800);
	aread16((int8 *)&scanline);
	aread16((int8 *)&RefreshAddr);
	afread(&VRAMBuffer,1,1);
	
	afread(&IRQa,1,1);
	aread32((int8 *)&IRQCount);
	aread32((int8 *)&IRQLatch);
	afread(&Mirroring,1,1);
	switch(Mirroring)
	{
	case 0:MIRROR_SET2(0);break;
	case 1:MIRROR_SET2(1);break;
	case 2:DoFourscreen();break;
	case 0x10:onemir(0);break;
	case 0x11:onemir(1);break;
	case 0x12:onemir(2);break;
	case 0x13:onemir(3);break;
	}
	
	afread(PSG,1,0x17);
	PSG[0x11]&=0x7F;
	if(version>=20)
	{
		afread(decvolume,1,3);
		if(version>=40)
		{
			for(x=0;x<3;x++)
				afread(&DecCountTo1[x],1,1);
			for(x=0;x<2;x++)
				afread(&SweepCount[x],1,1);
			aread32((int8 *)&nada);
			for(x=0;x<3;x++)
				afread(&nada,1,1);
		}
		else
			for(x=0;x<3;x++)
				aread32((int8 *)&nada);
			aread32((int8 *)&nada);
			for(x=0;x<5;x++)
				aread32((int8 *)&count[x]);
			for(x=0;x<2;x++)
				aread32((int8 *)&vcount[x]);
			afread(&sqnon,1,1);
			for(x=2;x<16;x++)
				aread32((int8 *)&vcount[x]);
			afread(&triangleor,1,1);
			if(version>=30)
			{
				uint32 temp[2];
				aread32((int8 *)&temp[1]);
				aread32((int8 *)&temp[0]);
				afread(&PCMBuffer,1,1);
				afread(&PCMBitIndex,1,1);
				temp[0]&=0x7FFF;
				PCMAddressIndex=temp[0];
				temp[1]&=0x1fff;
				PCMSizeIndex=temp[1];
				if(version>=31)
				{
					afread(&PSG[0x17],1,1);
					afread(&IRQlow,1,1);
					aread32((int8 *)&fhcnt);
					afread(&fcnt,1,1);
					afread(MapperExRAM,1,193-48-56-1-8-2-1-6);
				}
				else
					afread(MapperExRAM,1,193-48-56-1-8-2);
			}
			else
			{
				PSG[0x15]&=0xF;
				afread(MapperExRAM,1,193-48-56-1);
			}
	}
	else
	{
		afread(MapperExRAM,1,193);
		PSG[0x15]&=0xF;
		sqnon=PSG[0x15];
	}
	if(version<=30)
	{
		IRQlow=0;
		PSG[0x17]|=0x40;
	}
	
	afread(&joy_1,1,1);
	afread(&joy_2,1,1);
	afread(&joy_readbit,1,1);
	afread(&joy2_readbit,1,1);
	afread(&joy1or,1,1);
	afread(&joy2or,1,1);
	afread(&XOffset,1,1);
	afread(VPAL,1,8);
	
	if(MapperNo!=777)
		for(x=0;x<8;x++)
			if(VPAL[x]) VRAM_BANK1((x*0x400),(CHRBankList[x]&7));
			
			afread(mapbyte1,1,8);
			afread(mapbyte2,1,8);
			afread(mapbyte3,1,8);
			afread(mapbyte4,1,8);
			for(x=0;x<4;x++)
				aread16((int8 *)&mapword1[x]);
			
			for(x=0;x<4;x++)
				aread16((int8 *)&VPAL2[x]);
			
			for(x=0;x<4;x++)
				if(VPAL2[x]&0x8000)	//	>= .15 format?
				{
					if(VPAL2[x]&0x800)
						vnapage[x]=VROM+((VPAL2[x]&vmask1)*1024);
					else
					{
						vnapage[x]=VRAM+0x2000+((VPAL2[x]&3)*1024);
					}
				}
				afread(MapperExRAM,1,32768);
				afread(&vtoggle,1,1);
				aread16((int8 *)&TempAddr);
				aread16((int8 *)&RefreshAddr);
				
				if(MapStateRestore) MapStateRestore(version);
				
				UnHaltSound();
				free(StateBuffer);
				sprintf(errmsg,"State %d loaded.",CurrentState);howlong=180;
				if(MapperNo==777) FDSStateLoad(st);
				fclose(st);
}

char SaveStateStatus[10]={-1};

void CheckStates(void)
{
	FILE *st=NULL;
	int ssel;
	if(SaveStateStatus[0]==-1)
		for(ssel=0;ssel<10;ssel++)
		{
			sprintf(TempArray,"%s\\fcs\\%s.fc%d",BaseDirectory,StateFile,ssel);
			st=fopen(TempArray,"rb");
			if(st)
			{
				SaveStateStatus[ssel]=1;
				fclose(st);
			}
			else
				SaveStateStatus[ssel]=0;
		}
}

