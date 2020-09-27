/****************************************/
/*		FCE Ultra						*/
/*										*/
/*		video.c							*/
/*										*/
/*  Some generic high-level video		*/
/*  related functions.					*/
/****************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "m6502.h"
#include "video.h"
#include "fce.h"
#include "svga.h"
#include "version.h"
#include "general.h"

uint8 *XBuf;

uint8 InitVirtualVideo(void)
{
	unsigned long m;
	
//	printf("Allocating buffers...");
	XBuf = (uint8*) (malloc((256+16) * (240+16) + 8));
	if(XBuf==NULL)
	{
//		printf("FAILED\n");
		PrintFatalError("Failed allocating buffers!");
		return (0);
	}
	
	// if pointer size is 4 bytes, make sure the
	// buffer is aligned on an 8 byte boundary.
	if(sizeof(uint8*)==4)
	{
		m=(unsigned long) XBuf;
		m+=8;
		m&=0xFFFFFFF8;
		(unsigned long)XBuf=m;
	} 
	putc('\n',stdout);
	memset(XBuf,128,272*256);
	return(1);
}

#include "pcxhead.h"

void writepcx(void)
{
	uint8 *tmp;
	uint8 r,g,b;
	int x,u,y;
	FILE *pp=NULL;
	uint8 *gfxbuffer=NULL;
	
	gfxbuffer=(uint8 *)malloc(256*256);
	if(!gfxbuffer) return;
	
	for(u=0;u<999;u++)
	{
		//sprintf(TempArray,"%s"PSS"snaps"PSS"snap%d.pcx",BaseDirectory,u);
		sprintf(TempArray,"%s\\snaps\\snap%d.pcx",BaseDirectory,u);
		pp=fopen(TempArray,"rb");
		if(pp==NULL) break;
        fclose(pp);
	}
	pp=fopen(TempArray,"wb");
	if(pp==NULL)
	{
		sprintf(errmsg,"Error saving screen snapshot.");howlong=180;
		return;
	}
	fwrite(pcxheader,1,128,pp);
	
	tmp=XBuf+8;
	if(!PAL) 
	{
		for(x=0;x<256*8;x++) putc(128,pp);
		y=8;
		tmp+=272*8;
	}
	else
	{
		y=1;
		for(x=0;x<256;x++) putc(128,pp);
		tmp+=272;
	}
	for(;y<240;y++)
	{
		for(x=0;x<256;x++)
		{
			if(*tmp>=0xc0) putc(0xC1,pp);
			putc(*tmp,pp);
			tmp++;
		}
		tmp+=16;
	}
	putc(0xC,pp);
	for(x=0;x<256;x++)
	{
		GetPalette(x,&r,&g,&b);
		putc(r,pp);
		putc(g,pp);
		putc(b,pp);
	}
	fclose(pp);
	free(gfxbuffer);
	sprintf(errmsg,"Screen snapshot %d saved.",u);howlong=180;
}

