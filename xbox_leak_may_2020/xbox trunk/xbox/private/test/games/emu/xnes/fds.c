#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m6502.h"
#include "version.h"
#include "fce.h"
#include "mapper.h"
#include "fds.h"
#include "svga.h"
#include "sound.h"
#include "general.h"

/*	TODO:  Add code to put a delay in between the time a disk is inserted
	and the when it can be successfully read/written to.  This should
	prevent writes from occuring to places they shouldn't.
*/

#pragma warning( disable : 4244 )

static void FDSPSG(word A, byte V);
static void FDSClose(void);
static byte header[16];
#define writeskip mapbyte2[0]
static byte FDSWave[64];

static char FDSSaveName[2048];

byte FDSBIOS[8192];
byte *diskdata[4];

#define SideWrite mapbyte2[1]
#define DiskPtr (*(uint32*)(mapbyte2+4))
#define dsr0 mapbyte2[2]
#define dsr1 mapbyte2[3]

#define DC_INC		1

#define DiskSeekIRQ (*(int32*)(mapbyte3+4))
#define SelectDisk mapbyte3[0]
#define InDisk	   mapbyte3[1]

void FDSReset(void)
{
	memset(mapbyte1,0,8);
	memset(mapbyte2,0,8);
	memset(mapbyte3+4,0,4);
	memset(mapbyte4,0,8);
	memset(mapword1,0,8);
	memset(mapbyte2,0,8);
}
void FDSGI(int h)
{
	switch(h)
	{
	case GI_CLOSE: FDSClose();break;
	case GI_POWER: break;
	case GI_RESET: FDSReset();FDSInit();break;
	}
	
}
void FDSInit(void)
{
	static int done=0;
	dsr0=0;
	dsr1=0x41;
	MIRROR_SET(1);
	if(!done) InDisk=255;
	else
	{
		if(InDisk!=255)
			dsr1&=0xFE;
	}
	done=1;
	Page[7]=FDSBIOS-0xe000;
	Page[4]=Page[5]=Page[6]=MapperExRAM-0x8000;
	VPage[0]=VPage[1]=VPage[2]=VPage[3]=VPage[4]=VPage[5]=VPage[6]=VPage[7]=VRAM;
	VPAL[0]=VPAL[1]=VPAL[2]=VPAL[3]=VPAL[4]=VPAL[5]=VPAL[6]=VPAL[7]=1;
	MIRROR_SET2(0);
	MapStateRestore=0;
	PPU_hook=0;
	MapHBIRQHook=0;
	MapIRQHook=(void *)FDSFix;
	SetReadHandler(0x4020,0x5fff,(void *)FDSRead);
	SetWriteHandler(0x4020,0x5fff,(void *)FDSWrite);
	SetWriteHandler(0x8000,0xdfff,(void *)FDSRAMWrite);
}

void FDSControl(int what)
{
	switch(what)
	{
	case FDS_IDISK:dsr1&=0xFE;
		if(InDisk==255)
		{
			sprintf(errmsg,"Disk %d Side %s Inserted",
				SelectDisk>>1,(SelectDisk&1)?"B":"A");
			InDisk=SelectDisk;
		}
		else
			sprintf(errmsg,"Jamming disks is a BAD IDEA");
		howlong=180;
		break;
	case FDS_EJECT:
		if(InDisk!=255)
			sprintf(errmsg,"Disk Ejected");
		else
			sprintf(errmsg,"Cannot Eject Air");
		dsr1|=1;InDisk=255;
		howlong=180;
		break;
	case FDS_SELECT:
		if(InDisk!=255)
		{
			sprintf(errmsg,"Eject disk before selecting.");
			howlong=180;
			break;
		}
		SelectDisk=((SelectDisk+1)%header[4])&3;
		sprintf(errmsg,"Disk %d Side %s Selected",
			SelectDisk>>1,(SelectDisk&1)?"B":"A");
		howlong=180;
		break;
	}
}

void FDSFix(int a)
{
	if(IRQa)
	{
		IRQCount-=a;
		if(IRQCount<=0)
		{
			IRQa=0;
			dsr0|=1;
			dsr0&=~2;
			IRQCount=0xFFFF;
			TriggerIRQ();
			//IRQlow|=1;
		}
	}
	if(DiskSeekIRQ>0) 
	{
		DiskSeekIRQ-=a;
		if(DiskSeekIRQ<=0 && mapbyte1[5]&0x80)
		{dsr0&=~1;dsr0|=2;/*IRQlow|=1;*/TriggerIRQ();}
	}
}

void DiskControl(int which)
{
	if(mapbyte1[5]&1)
	{
		switch(which)
		{
		case DC_INC:
			//IRQlow&=~1;
			if(DiskPtr<64999) DiskPtr++;
			//DiskSeekIRQ=160+100;
			//DiskSeekIRQ=140;
			//DiskSeekIRQ=160;
			DiskSeekIRQ=160;
			break;
		}
	}
}
byte FDSRead(word A)
{
	//printf("Read: $%04x, %d\n",A,DiskPtr);
	switch(A)
	{
	case 0x4030:
		{
			//byte ret;
			//ret=dsr0&0xFE;
			//if(IRQlow&1) ret|=1;
			//IRQlow&=0xFE;
			//IRQlow&=~1;
			return dsr0;
		}
		break;
	case 0x4031: if(InDisk==255) return 0xFF;
				 {
					 byte z;
					 z=diskdata[InDisk][DiskPtr];
					 DiskControl(DC_INC);
					 return z;
				 }
	case 0x4032: return dsr1;
	case 0x4033: return 0x80;	// Battery
	}
	return 0xFF;
}

void FDSRAMWrite(word A, byte V)
{
	Page[A>>13][A]=V;
}

void FDSWrite(word A, byte V)
{
	if(A>=0x4040 && A<=0x407f)
	{
		FDSWave[A&0x3f]=V;
	} 
	else if(A>=0x4080 && A<=0x4089) FDSPSG(A,V);
	else
		switch(A)
	{
  case 0x4020:IRQlow&=~1;IRQLatch&=0xFF00;IRQLatch|=V;mapbyte1[0]=V;break;
  case 0x4021:IRQlow&=~1;IRQLatch&=0xFF;IRQLatch|=V<<8;mapbyte1[1]=V;break;
  case 0x4022:IRQlow&=~1;IRQCount=IRQLatch;IRQa=V&2;mapbyte1[2]=V;break;
  case 0x4023:mapbyte1[3]=V;break;
  case 0x4024:if(InDisk==255) break;
	  if(!(mapbyte1[5]&0x4) && mapbyte1[3]&0x1)
	  {
		  if(DiskPtr>=0 && DiskPtr<65000)
		  {
			  // sprintf(errmsg,"disk write");howlong=255;
			  if(writeskip) writeskip--;
			  else if(DiskPtr>=2)
			  {
				  SideWrite|=1<<InDisk;
				  diskdata[InDisk][DiskPtr-2]=V;
			  }
		  }
	  }
	  break;
  case 0x4025:if(InDisk==255) break;
	  if(!(V&0x40))
	  {
		  if(mapbyte1[5]&0x40 && !(V&0x10))
 	        {
			  DiskSeekIRQ=200;	
			  DiskPtr-=2;
		  }
		  if(DiskPtr<0) DiskPtr=0;
	  }
	  if(!(V&0x4)) writeskip=2;
	  mapbyte1[5]=V;
	  if(V&2) {DiskPtr=0;DiskSeekIRQ=200;}
	  if(V&0x40) DiskSeekIRQ=200;
	  if(V&0x8) MIRROR_SET2(0);
 		   else MIRROR_SET2(1);
		   break;
	}
}

int FDSLoad(char *name, FILE *fp)
{
	FILE *zp;
	int x;
	fseek(fp,0,SEEK_SET);
	fread(header,16,1,fp);
	if(memcmp(header,"FDS\x1a",4)) 
	{
		if(!(memcmp(header+1,"*NINTENDO-HVC*",14)))
		{
			long t;
			t=fseek(fp,0,SEEK_END);
			t=ftell(fp);
			if(t<65500)
				return 1;
			header[4]=t/65500;
			header[0]=0;
			fseek(fp,0,SEEK_SET);
		}
		else
			return 1; 
	}
	if(header[4]>4) header[4]=4;
	if(!header[4]) header[4]|=1;
	for(x=0;x<header[4];x++)
	{
		diskdata[x]=malloc(65500);
		fread(diskdata[x],1,65500,fp);
	}

	sprintf(TempArray, "%s\\disksys.rom", BaseDirectory);
	if(!(zp=fopen(TempArray,"rb"))) return 3;
	if(fread(FDSBIOS,1,8192,zp)!=8192)
		return 4;
	
	fclose(zp);
	MapperNo=777;
	VROM_size=0;
	strcpy(FDSSaveName,name);
	GameInterface=(void *)FDSGI;
	return 0;
}



static byte dah[16];
static void FDSPSG(word A, byte V)
{
	switch(A)
	{
	case 0x4080: case 0x4081: case 0x4082: case 0x4083: dah[A&0xF]=V;break;
	case 0x4089:dah[9]=V;break;
		
	} 
}

static long vco=0;

void FDSSound(long *Wave)
{
	int32 V;
	int32 envelope;
	uint32 freq;
	long inc;
	static int32 index=0;
	
	return; 
	freq=dah[2]|(((dah[3]&0xF)<<8)+1);
	// inc=(long double)(SndRate<<15)/(long double)21477270/256/(long double)freq;
	// inc=(long double)(SndRate<<15)/(long double)50000/(long double)freq;
	inc=(long double)(SndRate<<15)/((long double)freq*(long double)28.6363636363/64);
	if(dah[0]&0x80)
		envelope=((dah[0]&0x3f)<<16)/63;
	else
		envelope=0;
	envelope=((0x3f)<<16)/63;
	for(V=0;V<256;V++)
	{
		if(vco>=inc)
		{
			vco-=inc;
			index=(index+1)&63;
		}
		Wave[V]=((((FDSWave[index]&0x3f)<<4)-32)*envelope)>>16;
		vco+=0x8000;
	}
}


void FDSStateLoad(FILE *fp)
{
	int x;
	
	for(x=0;x<header[4];x++)
		if(SideWrite&(1<<x))
		{
			//printf("Disk state read %d\n",x);
			fread(diskdata[x],1,65500,fp);
		}
		// printf("gah:  %d\n",InDisk);
		// printf("honk: %d\n",header[4]);
		if(InDisk>((header[4]-1)&3)) InDisk=255;
}

void FDSStateSave(FILE *fp)
{
	int x;
	for(x=0;x<header[4];x++)
		if(SideWrite&(1<<x))
		{
			//printf("Disk state write %d\n",x);
			fwrite(diskdata[x],1,65500,fp);
		}
}

void FDSClose(void)
{
	FILE *fp;
	int x;
	
	fp=fopen(FDSSaveName,"r+b");
	if(!fp) return;
	
	fseek(fp,0,SEEK_SET);
	if(header[0])			// Does it have a 16-byte FWNES-style header?
		fseek(fp,16,SEEK_SET);	// If so, skip it.
	else
		fseek(fp,0,SEEK_SET);
	for(x=0;x<header[4];x++)
	{
		if(fwrite(diskdata[x],1,65500,fp)!=65500) {fclose(fp);return;}
	}
	fclose(fp);
}
