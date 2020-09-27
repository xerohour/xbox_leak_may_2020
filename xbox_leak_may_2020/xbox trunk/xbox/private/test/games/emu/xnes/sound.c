/********************************************************/
/*******		sound.c				*/
/*******						*/
/*******  Sound emulation code and waveform synthesis 	*/
/*******  routines.  A few ideas were inspired		*/
/*******  by code from Marat Fayzullin's EMUlib		*/
/*******						*/
/********************************************************/		

#pragma warning( disable : 4244 )
#pragma warning( disable : 4018 )

#include <stdlib.h>
#include <stdio.h>
//#include <unistd.h>
#include <string.h>
#include "m6502.h"
#include "fce.h"
#include "svga.h"
#include "mapper.h"
#include "sound.h"


squ SoundQueue[32768];
uint32 SQWrPtr=0;
uint32 SQRdPtr=0;

int SndRate    = 0;
static int32 Wave[SND_BUFSIZE];
static int32 *Wave2=0;
static char ready_for_sound=0;


#ifdef FPS
unsigned long ploinker=0;
#endif


uint8 triangleor=0xFF,tricoop=0;
uint8 PSG[0x18];

uint8 decvolume[3];
int32 count[5]; 
static int32 inc;
int32 vcount[16];
uint8 sqnon=0;


uint16 nreg=1;

int32 lengthcount[4]; 

static const uint8 Slengthtable[0x20]=
{
	0x5,0x7f,0xA,0x1,0x14,0x2,0x28,0x3,0x50,0x4,0x1E,0x5,0x7,0x6,0x0E,0x7,
		0x6,0x08,0xC,0x9,0x18,0xa,0x30,0xb,0x60,0xc,0x24,0xd,0x8,0xe,0x10,0xf
};
static uint32 lengthtable[0x20];

static const uint32 SNoiseFreqTable[0x10]=
{
	4,8,16,32,64,96,128,160,202,254,380,508,762,1016,2034,2046
};
static uint32 NoiseFreqTable[0x10];

double nesincsize;

rpd RawPCMData[8192];
long RawPCMIndex=0;
long RawPCMIndexRead=0;

ullong RawPCMCount;
//unsigned long long RawPCMIncSize;
ullong RawPCMIncSize; // changed by kevin

uint8 RawC=0;
int32 RawWrite[32];
uint8 RawPCMOn=1;

static const uint8 NTSCPCMTable[0x10]=
{
	0xd6,0xbe,0xaa,0xa0,0x8f,0x7f,0x71,0x6b,
		0x5f,0x50,0x47,0x40,0x35,0x2a,0x24,0x1b
};
static uint32 PCMTable[16];

unsigned long PSG_base;

// $4010        -        Frequency
// $4011        -        Actual data outputted
// $4012        -        Address register: $c000 + V*64
// $4013        -        Size register:  Size in bytes = (V+1)*64


int32 PCMIRQCount;

uint8 PCMBitIndex=0;
uint32 PCMAddressIndex=0;
int32 PCMSizeIndex=0;
uint8 PCMBuffer=0; 
int vdis=0;

void CalcPCM()
{
	uint32 freq;
	uint32 honk;
	uint32 cycles;
	
	freq=(NTSCPCMTable[PSG[0x10]&0xF]<<4);
	cycles=(((PSG[0x13]<<4)+1));
	cycles*=freq/14; 
	honk=((PSG[0x13]<<4)+1)*freq;
	honk-=cycles; 
	if(PAL) honk/=107;
	else honk/=(double)113.66666666;
	PCMIRQCount=honk+1;
	vdis=0;
}

void PrepDPCM()
{
	PCMAddressIndex=0x4000+(PSG[0x12]<<6); 
	PCMSizeIndex=(PSG[0x13]<<4)+1;
	PCMBitIndex=count[4]=0;  
	PCMBuffer=Page[(0x8000+PCMAddressIndex)>>13][0x8000+PCMAddressIndex];
}

byte sweepon[2]={0,0};
long sweepcount[2]={0,0};
long sweepfreq[2]={0,0};


byte PCMIRQ=0;

uint8 SweepCount[2];
uint8 DecCountTo1[3];

uint8 fcnt=0;
int32 fhcnt=0;

void Write_PSG(word A, byte V)
{ 
	A&=0x1f;
	//if(A<=0x3) printf("$%04x:$%02x\n",A,V);
	switch(A)
	{
	case 0x1:
		if(!sweepon[0])
			sweepfreq[0]=PSG[0x2]|((PSG[0x3]&7)<<8);
		sweepon[0]=V&0x80;
		break;
	case 0x2:
		sweepfreq[0]&=0xFF00;
		sweepfreq[0]|=V;
		break;
	case 0x3:
		if(PSG[0x15]&1)
		{
			sweepon[0]=PSG[1]&0x80;
			sweepfreq[0]=PSG[0x2]|((V&7)<<8);
			sweepcount[0]=1<<29;
			decvolume[0]=15;
			lengthcount[0]=lengthtable[(V>>3)&0x1f];
			sqnon|=1;
			DecCountTo1[0]=(PSG[0]&0xF)+1;
			SweepCount[0]=((PSG[0x1]>>4)&7)+1;
		}
		break;
		
	case 0x5:
		if(!sweepon[1])
			sweepfreq[1]=PSG[0x6]|((PSG[0x7]&7)<<8);
		sweepon[1]=V&0x80;
		break;
	case 0x6:
		sweepfreq[1]&=0xFF00;
		sweepfreq[1]|=V;
		break;
	case 0x7:
		if(PSG[0x15]&2)
		{
			sweepon[1]=PSG[0x5]&0x80;
			sweepfreq[1]=PSG[0x6]|((V&7)<<8);
			sweepcount[1]=1<<29;
			decvolume[1]=15;
			lengthcount[1]=lengthtable[(V>>3)&0x1f];
			sqnon|=2;
			DecCountTo1[1]=(PSG[0x4]&0xF)+1;
			SweepCount[1]=((PSG[0x5]>>4)&7)+1;
		}
		break;
	case 0x8:                      
		tricoop=V&0x7F;
		break;
	case 0xb://printf("$%04x:$%02x\n",A,V);
		triangleor=255;
		tricoop=PSG[0x8]&0x7f;
		sqnon|=4;
		lengthcount[2]=lengthtable[(V>>3)&0x1f];
		break; 
	case 0xF:
		if(PSG[0x15]&8)
		{          
			sqnon|=8;
			decvolume[2]=15;
			lengthcount[3]=lengthtable[(V>>3)&0x1f];
			DecCountTo1[2]=(PSG[0xC]&0xF)+1;
		}
		break;
	case 0x11:
		if(PSG[0x15]&0x10) return;
		V&=0x7f;
		RawWrite[RawC]++;
		RawPCMData[RawPCMIndex].d=V-64;
		RawPCMData[RawPCMIndex].ts=timestamp;
		RawPCMData[RawPCMIndex].new=1;
		RawPCMIndex=(RawPCMIndex+1)&8191;
		break;
	case 0x13:
		sqnon|=0x10;
		break;
	case 0x15: 
		sqnon&=V;
		if(V&0x10)
		{
            if(!(PSG[0x15]&0x10)) 
			{PrepDPCM();CalcPCM();}
			else if(vdis)
			{CalcPCM();}
		}
		break;
	case 0x17:
		V&=0xC0;
		fhcnt=7457*3;
		fcnt=3;
		FrameSoundUpdate();
		IRQlow&=~4;
		// printf("$%04x:$%02x\n",A,V);
		break;
	}
	PSG[A]=V;
}

#define QueueFlush() while(SQRdPtr!=SQWrPtr){SoundQueue[SQRdPtr].func(SoundQueue[SQRdPtr].A,SoundQueue[SQRdPtr].V);SQRdPtr=(SQRdPtr+1)&32767;}

byte Read_PSG(word A)
{
	A&=0x1F;
	switch(A)
	{
	default:
		return PSG[A];
	case 0x15:  
		{
			byte ret;
			ret=((sqnon&0x1f)&PSG[0x15])|PCMIRQ;
			PCMIRQ=0;
			if(IRQlow&4) ret|=0x40;
			IRQlow&=~4;
			return ret;
		}
	}
}

void FrameSoundUpdate(void)
{
	int P;
	// Linear counter:  Bit 0-6 of $4008
	// Length counter:  Bit 4-7 of $4003, $4007, $400b, $400f
	fcnt=(fcnt+1)&3;
	if(fcnt==3) 
		if(PSG[0x17]&0x80) fhcnt+=7457*3;
		if(fcnt==3)
		{
			if(!(PSG[0x17]&0xC0)) {TriggerIRQ();IRQlow|=4;}
		}
		switch((fcnt&1))
		{
		case 0:       /* Envelope decay, linear counter, length counter, freq sweep */
			if(PSG[0x15]&4 && sqnon&4)
				if(!(PSG[8]&0x80))
				{
					if(lengthcount[2]>0)
					{
						lengthcount[2]--;
						if(lengthcount[2]<=0) sqnon&=~4;
					}
				}
				
				for(P=0;P<2;P++)
				{
					if(PSG[0x15]&(P+1) && sqnon&(P+1))
					{
						if(!(PSG[P<<2]&0x20))
						{
							if(lengthcount[P]>0)
							{
								lengthcount[P]--;
								if(lengthcount[P]<=0) sqnon&=~(P+1);
							}
						}
						/* Frequency Sweep Code Here */
						/* xxxx 0000 */
						/* xxxx = hz.  120/(x+1)*/
						if(PSG[(P<<2)+0x1]&0x80 && sweepon[P]&0x80)
						{
							int32 mod;
							mod=0;
							
							if(SweepCount[P]>0) SweepCount[P]--; 
							if(SweepCount[P]<=0)
							{
								SweepCount[P]=((PSG[(P<<2)+0x1]>>4)&7)+1; //+1;
								if(sweepfreq[P])
								{
									if(PSG[(P<<2)+0x1]&0x8)
									{
										mod-=((P+sweepfreq[P])>>(PSG[(P<<2)+0x1]&7));          
										sweepfreq[P]+=mod;
										if(sweepfreq[P]&0x800)
										{sweepfreq[P]=0;sweepon[P]=2;}
										if(!sweepfreq[P]) sweepon[P]=sweepfreq[P]=0;
#ifdef moo
										if((mod+sweepfreq[P])&0x800)
										{sweepfreq[P]=0;sweepon[P]=0;}
										else
										{sweepfreq[P]+=mod; if(!sweepfreq[P]) sweepon[P]=0;}
#endif
									}
									else
									{
										mod+=sweepfreq[P]>>(PSG[(P<<2)+0x1]&7);
										if((mod+sweepfreq[P])&0x800)
										{
											if(mod==sweepfreq[P])
												sweepon[P]=0;
											else
												sweepon[P]=2;
											sweepfreq[P]=0;
										}
										else
										{sweepfreq[P]+=mod; if(!sweepfreq[P]) sweepon[P]=0;}
										
										//printf("erk %d: %d, %d\n",P,mod,sweepfreq[P]);
									}
								}
								
							}
							
						} 
					}
				}
				
				if(PSG[0x15]&0x8 && sqnon&8)
				{if(!(PSG[0xC]&0x20))
				{
					if(lengthcount[3]>0)
					{
						lengthcount[3]--;
						if(lengthcount[3]<=0) sqnon&=~8;
					}
				}}
				
		case 1:	/* Envelope decay + linear counter */
			if(!(PSG[8]&0x80) && PSG[0x15]&0x4 && sqnon&4)
			{
				if(tricoop) tricoop--;
				if(tricoop<=0) triangleor=0;
			}
			for(P=0;P<2;P++)
			{
				if(PSG[0x15]&(P+1) && sqnon&(P+1))
				{
					if(DecCountTo1[P]>0) DecCountTo1[P]--;
					if(DecCountTo1[P]<=0)
					{
						DecCountTo1[P]=(PSG[P<<2]&0xF)+1;
						if(decvolume[P] || PSG[P<<2]&0x20)
						{
							decvolume[P]--;
							if((PSG[P<<2]&0x20) && decvolume[P]==0)
								DecCountTo1[P]+=16;
							decvolume[P]&=15;
						}
					}
				}
			}
			if(PSG[0x15]&0x8 && sqnon&8)
			{
				if(DecCountTo1[2]>0) DecCountTo1[2]--;
				if(DecCountTo1[2]<=0)
				{
					DecCountTo1[2]=(PSG[0xC]&0xF)+1;
					if(decvolume[2] || PSG[0xC]&0x20)
					{
						decvolume[2]--;
						if((PSG[0xC]&0x20) && decvolume[2]==0)
							DecCountTo1[2]+=16;
						decvolume[2]&=15;
					}
				}
			}
			break;
 }
 
}


static byte haltthingy=0;
static int reduce=0;

void HaltSound(int how)
{
	byte *ht;
	if(!soundon) return;
	ht=&haltthingy;
	haltthingy=how?99:1;
loskf:
	if(*ht!=2) goto loskf;
}

void UnHaltSound(void)
{
	byte *ht;
	if(!soundon) return;
	ht=&haltthingy;
sexyme:
	if(*ht==2) haltthingy=3;
	if(*ht) goto sexyme;
}

void FillSoundBuffer(unsigned char *Buf)
{
	long V,P,freq;
	if(!ready_for_sound) return;
	
#ifdef FPS
    ploinker++;
#endif
	if(haltthingy)
	{
		if(haltthingy==3) haltthingy=0;
		else 
		{
			if(haltthingy==1)
			{
				QueueFlush();
				haltthingy=2;
			}
			else if(haltthingy==99)
			{
				for(V=0;V<256;V++)
					Buf[V]=0x80;
				haltthingy=2;
			}
			return;
		}
	}
    QueueFlush();
    if(!(PSG[0x15]&0x10))
	{
        if(!RawPCMOn)
        {
			int32 out;
			out=(int32)(PSG[0x11]-64)<<4;
			for(V=0;V<SND_BUFSIZE;V++)
				Wave[V]+=out;
        }
        else
        { 	 
			if(RawPCMCount.DW.h<=timestamp-90090 || RawPCMCount.DW.h>=timestamp+90090)
			{RawPCMCount.DW.h=timestamp;RawPCMCount.DW.l=0;}	  
			for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
			{
				Wave[V>>4]+=RawPCMData[RawPCMIndexRead].d;
				RawPCMData[RawPCMIndexRead].new=0;

				// Kevin
				RawPCMCount.DW.h += RawPCMIncSize.DW.h;
				if((RawPCMCount.DW.l + RawPCMIncSize.DW.l) < RawPCMCount.DW.l)
					RawPCMCount.DW.h++;
				RawPCMCount.DW.l += RawPCMIncSize.DW.l;
				//RawPCMCount.LL+=RawPCMIncSize;


				if(RawPCMCount.DW.h>=RawPCMData[(RawPCMIndexRead+1)&8191].ts+90090 && RawPCMData[(RawPCMIndexRead+1)&8191].new)
					RawPCMIndexRead=(RawPCMIndexRead+1)&8191;
			}
		}
	}
    else
    {     
		inc=PCMTable[PSG[0x10]&0xF];
		
		for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
		{     
			if(count[4]>=inc)
			{
				if(!(PSG[0x15]&0x10))
					goto endseq;
				PCMBuffer=Page[(0x8000+PCMAddressIndex)>>13][0x8000+PCMAddressIndex];
				if((PCMBuffer>>PCMBitIndex)&1)
				{
					unsigned char bah;
					bah=PSG[0x11];
					bah+=2;
					bah-=(bah>>6)&2; 
					PSG[0x11]=bah;
				}
				else
				{
					char bah;
					bah=PSG[0x11];
					bah-=2;
					if(bah<0) bah+=2;
					PSG[0x11]=bah;
				}
				PCMBitIndex=(PCMBitIndex+1);
				if(PCMBitIndex&8)
				{        
					PCMSizeIndex--;
					if(!PCMSizeIndex)
					{
						if(PSG[0x10]&0x40)
							PrepDPCM();
						else
							PSG[0x15]&=~0x10;
					}
					else
					{
						PCMAddressIndex=(PCMAddressIndex+1)&0x7fff;
						PCMBitIndex=0;
					}
				}
endseq:
				count[4]-=inc;
			}
			Wave[V>>4]+=PSG[0x11]-64;
			count[4]+=0x400000;
		}
    }
	
	for(P=0;P<2;P++)
	{
		if(PSG[0x15]&(P+1) && sqnon&(P+1))
		{     
			long vcoo;
			unsigned long dcycs;
			unsigned char amplitude;
			long envelope;
			
			if(PSG[(P<<2)+0x1]&0x80 && sweepon[P])
				freq=sweepfreq[P];
			else
				freq=(((PSG[(P<<2)+0x2]|((PSG[(P<<2)+0x3]&7)<<8))));
			
			if(PSG[P<<2]&0x10)
				amplitude=PSG[P<<2]&15;
			else
				amplitude=decvolume[P];
			envelope=(amplitude<<16)/15;
			
			if(freq<8 || freq>0x7ff) goto enda;
			freq+=1;
			inc=(long double)((unsigned long)((SndRate OVERSAMPLE)<<12))/((long double)PSG_base/freq);
			
			switch(PSG[P<<2]&0xC0)
			{
			default:
			case 0x00:dcycs=inc>>3;break;
			case 0x40:dcycs=inc>>2;break;
			case 0x80:dcycs=inc>>1;break;
			case 0xC0:dcycs=(inc+inc+inc)>>2;break;
			}
			if(PSG[P<<2]&0x10)
				amplitude=PSG[P<<2]&15;
			else
				amplitude=decvolume[P];
			vcoo=count[P];
			for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
			{               
				if(vcoo>=dcycs) 
					Wave[V>>4]-=amplitude; 
				else 
					Wave[V>>4]+=amplitude; 
				vcoo+=0x1000;
				if(vcoo>=inc) vcoo-=inc;
			}
			count[P]=vcoo; 
			
enda:
			if(1)
				;
		}
	}
	
	
	if(PSG[0x15]&0x4 && sqnon&4)
	{
		static long double triacc=0; 
		static byte tc=0,tcout=0;
		
		
		freq=(((PSG[0xa]|((PSG[0xb]&7)<<8))+1));
		
		if(triangleor && PSG[0x8]!=0x80 && freq>1)
			for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
			{               
				triacc=triacc-(double)nesincsize;
				if(triacc<=0)
				{
					triacc+=freq;
					tc=(tc+1)&0x1F;
					tcout=(tc&0xF);
					if(tc&0x10) tcout^=0xF;
					tcout<<=1;
				}
				Wave[V>>4]+=tcout-15;
			}
	}
	
	if(PSG[0x15]&0x8 && sqnon&8)
	{ 
		char amptab[2];
		unsigned char amplitude;
		
		if(PSG[0xC]&0x10)
			amplitude=(PSG[0xC]&0xF);
		else            
			amplitude=decvolume[2]&0xF;
		
		inc=NoiseFreqTable[PSG[0xE]&0xF]; 
		amptab[0]=((amplitude)-(amplitude>>1));
		amptab[1]=0-(amplitude>>1);     
		
		if(amplitude) 
			for(V=0;V<SND_BUFSIZE OVERSAMPLE;V++)
			{     
				Wave[V>>4]+=amptab[nreg&1];
				if(count[3]>=inc)
				{                          
					if(PSG[0xE]&0x80)  // "short" noise
					{
						unsigned char feedback;      
						feedback=((nreg>>8)&1)^((nreg>>14)&1);
						nreg=(nreg<<1)+feedback;
						nreg&=0x7fff;
					}
					else
					{
						unsigned char feedback;
						feedback=((nreg>>13)&1)^((nreg>>14)&1);
						nreg=(nreg<<1)+feedback;
						nreg&=0x7fff;
					}
					count[3]-=inc;
				}
				count[3]+=0x1000;
			}
	}
	
	/*      Here the data is downsampled and converted.
	*/     
	if(MapExpSound)
	{
        reduce=MapExpSound(Wave);
        if(MapperNo==666 && Wave2)
			memcpy(Wave2,Wave,SND_BUFSIZE*sizeof(int32));
        if(reduce)
        {
			for(P=0;P<SND_BUFSIZE;P++)
			{
				int mixer;
				mixer=0;
				mixer=Wave[P];
				Wave[P]=0;
				if(mixer<-4096) mixer=-4096;
				else if(mixer>4095) mixer=4095;
				Buf[P]=(mixer>>(OVERSAMPLESHIFT+1))^128;
			}
        } 
		else
        {
			for(P=0;P<SND_BUFSIZE;P++)
			{
				int mixer;
				mixer=Wave[P];
				Wave[P]=0;
				if(mixer<-2048) mixer=-2048;
				else if(mixer>2047) mixer=2047;
				Buf[P]=(mixer>>OVERSAMPLESHIFT)^128;
			}
		}
	}
	else
	{
        if(MapperNo==666 && Wave2)
			memcpy(Wave2,Wave,SND_BUFSIZE*sizeof(int32));
        for(P=0;P<SND_BUFSIZE;P++)
		{
			int mixer;
			mixer=Wave[P];
			Wave[P]=0;
			Buf[P]=(mixer>>OVERSAMPLESHIFT)^128;    
		}
	}
}

int GetSoundBuffer(int32 **W)
{
	if(!Wave2) Wave2=malloc(SND_BUFSIZE*sizeof(int32));
	*W=Wave2;
	if(!Wave2) *W=Wave;
	return reduce;
}

void ResetSound(void)
{
	int x;
	for(x=0;x<0x16;x++)
		if((x&0x13)!=1) {Write_PSG(x,0);}
		PSG[0x17]=0;
		fhcnt=7457*3;
		fcnt=3;
        reduce=0;
}
void SetSoundVariables(int Rate)
{
	int x;  
	long double temp2;
	
	SndRate=Rate;
	temp2=(long double)(PAL?312*106*50:NTSC_CPU)/(SndRate OVERSAMPLE);

	//<<TODO>> Fix This!!!!!!!
//	RawPCMIncSize=temp2*0x100000000;
	nesincsize=(double)(PAL?PAL_CPU:NTSC_CPU)/(double)(SndRate OVERSAMPLE);
	PSG_base=PAL?110840:111861;
	for(x=0;x<2048;x++)
		RawPCMData[x].ts=-1;
	for(x=0;x<0x10;x++)
	{
		long double z;
		z=NTSCPCMTable[x]<<4;
		z=(NTSC_CPU*8)/z;
		z=((long double)SndRate OVERSAMPLE)/(long double)z;
		z*=524288*8;
		PCMTable[x]=z;
	}
	for(x=0;x<0x10;x++)
	{
		long double z;
		z=SNoiseFreqTable[x];
		z=(PAL?PAL_CPU:NTSC_CPU)/z;
		z=(long double)((unsigned long)((SndRate OVERSAMPLE)<<12))/z;
		NoiseFreqTable[x]=z;
	}
	for(x=0;x<0x20;x++)
		lengthtable[x]=Slengthtable[x]<<1;
	ready_for_sound=1;
}
