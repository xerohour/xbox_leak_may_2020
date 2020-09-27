/****************************************************************/
/*			FCE Ultra											*/
/*																*/
/*	This file contains routines for reading/writing the			*/
/*	configuration file, and for getting arguments				*/
/*																*/
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m6502.h"
#include "svga.h"
#include "fce.h"
#include "config.h"
#include "netplay.h"

extern int joy[4] = {1, 1, 0, 0};
extern byte JoySwap;
int joyvar[32];

void GetValue(FILE *fp, char *str, int *v)
{
	char buf[256], buf2[32];
	while(fgets(buf, 256, fp))
	{
		int v2;
		sscanf(buf, "%[^=] %*[=] %d", buf2, &v2);
		if(!strcmp(str, buf2)) 
			*v = v2;
	}
	fseek(fp, 0, SEEK_SET);
	//rewind(fp);
}

void GetValueString(FILE *fp, char *str, char *v)
{
	char buf[256], buf2[32];
	while(fgets(buf, 256, fp))
	{
		char v2[1024];
		sscanf(buf, "%[^=] %*[=] %s", buf2, v2);
		if(!strcmp(str, buf2)) 
		{
			strcpy(v, v2);
		}
	}
	fseek(fp, 0, SEEK_SET);
	//rewind(fp);
}


void SetValue(FILE *fp, char *str, int v)
{
	fprintf(fp, "%s=%d\n", str, v);
}

void SetValueString(FILE *fp, char *str, char *str2)
{
	fprintf(fp, "%s=%s\n", str, str2);
}


void SaveConfig(char *filename)
{
	FILE *fp;
	int x;
	char gorf[16];

	fp = fopen(filename, "wb");
	if(fp == NULL) 
		return;

	SetValue(fp, "vgamode", vmode);
	SetValue(fp, "sound", soundon);
	SetValue(fp, "joy1", joy[0]);
	SetValue(fp, "joy2", joy[1]);
	SetValue(fp, "joy3", joy[2]);
	SetValue(fp, "joy4", joy[3]);

	for(x = 0; x < 32; x++)
	{
		sprintf(gorf, "joyvar%d", x);
		SetValue(fp, gorf, joyvar[x]);
	}

	SetValue(fp, "ntsccol", ntsccol);
	SetValue(fp, "ntsctint", ntsctint);
	SetValue(fp, "ntschue", ntschue);
	SetValue(fp, "ssync", ssync&1);
	SetValue(fp, "dowait", dowait);
//#ifdef WINDOWS
//	SetValueString(fp,"driver",d=GetDriverConfig());
//#endif
	fclose(fp);
}

void LoadConfig(char *filename)
{
	FILE *fp;
	char gorf[16];
	int x;
	
	fp = fopen(filename, "rb");
	if(fp == NULL)
		return;
	
	GetValue(fp, "vgamode", &vmode);
	GetValue(fp, "sound", &soundon);
	GetValue(fp, "joy1", &joy[0]);
	GetValue(fp, "joy2", &joy[1]);
	GetValue(fp, "joy3", &joy[2]);
	GetValue(fp, "joy4", &joy[3]);

	for(x = 0; x < 32; x++)
	{
		joyvar[x] = 0;
		sprintf(gorf, "joyvar%d", x);
		GetValue(fp, gorf, joyvar + x);
	}

	GetValue(fp, "ntsccol", &ntsccol);
	GetValue(fp, "ntsctint", &ntsctint);
	GetValue(fp, "ntschue", &ntschue);

	x = ssync;
	GetValue(fp, "ssync", &x);
	ssync &= 2;
	ssync |= (x & 1);
	
	x = dowait;
	GetValue(fp, "dowait", &x);
	dowait = x;

//#ifdef WINDOWS
//	GetValueString(fp,"driver",SetDriverConfig());
//#endif
	fclose(fp);
}

void CheckArgs(void)
{
//	printf("\nUsage is as follows:\nfceu <options> filename\n\n");
//	puts("Options:");
//#ifdef WINDOWS
//	puts("-vmode x        Select video mode(all are 8 bpp).
//		1 = 320x240(full screen)    6 = 640x480(scaled: 2x,2y)
//		2 = 512x384(centered)       7 = 1024x768(scaled: 4x,3y)
//		3 = 640x480(centered)       8 = 1280x1024(scaled: 5x,4y) 
//		4 = 640x480(with scanlines) 9 = 1600x1200(scaled: 6x,5y)
//		5 = 640x480(T.V. emulation)                                  
//		");
//#else                            z
//		puts("
//		-vmode x        Select video mode(all are 8 bpp).
//		1 = 256x240                 4 = 640x480(with scanlines)
//		2 = 256x256                 5 = 640x480(T.V. emulation)
//		3 = 256x256(with scanlines) 6 = 256x224(with scanlines)");
//#endif                 
//		puts("-ntsccol x      Emulate an NTSC's TV's colors.
//		0 = Disabled.
//		1 = Enabled.
//		-pal            Emulate a PAL NES.
//		-novsync x      Disable speed limiting.
//		0 = Disabled(speed limiting).
//		1 = Enabled(no speed limiting).
//		-ssync x        Synchronize video refreshes to internal sound timer.
//		0 = Disabled.
//		1 = Enabled.
//		-sound x	Sound.
//		0 = Disabled.
//		1 = Enabled.
//		-joyx y 	Joystick mapped to virtual joystick x[1-4].
//		0 = Disabled, reset configuration.
//		Otherwise, y[1-inf) = joystick number.
//		-gg             Enable Game Genie emulation.");
		
//#ifdef NETWORK
//		puts("-connect   s	Connect to server 's' for TCP/IP network play.
//		-server    x    Be a host/server for TCP/IP network play.
//		\"x\" is an integer that specifies how many frames should be
//		between virtual joystick refreshes.  Default is 3.  It is
//		not required.");
//#endif
		exit(1);
}

void DoArgs(int argc, char *argv[])
{
	int argctemp,x;
	char *ppoint;
	for(argctemp = 1; argctemp < argc - 1; argctemp++)
	{
/*
#ifdef NETWORK
		ppoint =(char *)strstr(argv[argctemp],"-server");
		if(ppoint!=NULL && (argctemp+1<argc))
		{
			netplay=1;
			if(argctemp!=(argc-2)) // Make sure that we're not using the ROM
				// image file name as a value!
			{
				int t=3;
				sscanf(argv[argctemp+1],"%d",&t);
				netskip=t;
			}
		}
		ppoint =(char *)strstr(argv[argctemp],"-connect");
		if(ppoint!=NULL && (argctemp+1<argc))
		{
			netplay=2;
			sscanf(argv[argctemp+1],"%255s",netplayhost);
			JoySwap=8;
		}
#endif
*/
		ppoint =(char *)strstr(argv[argctemp],"-vgamode");
		if(!ppoint) ppoint =(char *)strstr(argv[argctemp],"-vmode");
		if(ppoint!=NULL && (argctemp+1<argc))
			vmode=*argv[argctemp+1]-48;
		ppoint =(char *)strstr(argv[argctemp],"-ntsccol");
		if(ppoint!=NULL && (argctemp+1<argc))
			ntsccol=*argv[argctemp+1]-48;
		ppoint =(char *)strstr(argv[argctemp],"-sound");
		if(ppoint!=NULL && (argctemp+1<argc))
			soundon=*argv[argctemp+1]-48;
		ppoint =(char *)strstr(argv[argctemp],"-gg");
		if(ppoint!=NULL) 
			genie=3;
		ppoint =(char *)strstr(argv[argctemp],"-novsync");
		if(ppoint!=NULL && (argctemp+1<argc))
			dowait=((*argv[argctemp+1]-48)&1)^1;
		ppoint =(char *)strstr(argv[argctemp],"-ssync");
		if(ppoint!=NULL && (argctemp+1<argc))
		{ssync&=2;ssync|=(*argv[argctemp+1]-48)&1;}
		
		if(((!strcmp(argv[argctemp],"-joy1")) || (!strcmp(argv[argctemp],"-joy"))) && (argctemp+1<argc))
		{
			sscanf(argv[argctemp+1],"%d",&joy[0]);
			if(!joy[0]) memset(joyvar,0,32);
		}
		for(x=0;x<4;x++)
		{
			char t[6];
			sprintf(t,"-joy%d",x+1);
			ppoint =(char *)strstr(argv[argctemp],t);
			if(ppoint!=NULL && (argctemp+1<argc))
			{
				sscanf(argv[argctemp+1],"%d",&joy[x]);
				if(!joy[x]) memset(&joyvar[x*8],0,8*sizeof(int));
			}
		}
		ppoint=(char *)strstr(argv[argctemp],"-pal");
		if(ppoint!=NULL) 
			InitPAL();
	}
}
