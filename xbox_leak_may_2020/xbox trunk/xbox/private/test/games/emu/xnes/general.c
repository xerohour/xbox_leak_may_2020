#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <fcntl.h>
//#include <unistd.h>

#include "m6502.h"
#include "svga.h"
#include "fce.h"
#include "general.h"
#include "state.h"

char TempArray[2048];

void GetFileBase(char *f)
{
	char *tp1,*tp3;
	
#ifdef UNIX
	tp1=((char *)strrchr(f,'/'));
#else
	// find the last '\' in the string
	tp1=((char *)strrchr(f,'\\'));
	tp3=((char *)strrchr(f,'/'));
	if(tp1<tp3) tp1=tp3;
#endif
	// if there are no '\' (just a file), then use the entire filename
	if(!tp1) 
		tp1=f;
	
	if((tp3=strrchr(f,'.'))!=NULL)
	{
		memcpy(StateFile,tp1,tp3-tp1);
		StateFile[tp3-tp1]=0;
	}
	else
		strcpy(StateFile,tp1);
}

#ifndef WINDOWS
void GetBaseDirectory(char *bd, char *f)
{
#ifndef UNIX
	char *a,*b;
	a=strrchr(f,'\\');
	b=strrchr(f,'/');
	
	if(a>b)
	{memcpy(bd,f,a-f);bd[a-f]=0;}
	else if(b>a)
	{memcpy(bd,f,b-f);bd[b-f]=0;}
	else
	{bd[0]='.';bd[1]=0;} 
#else
	char *ol;
	ol=getenv("HOME");
	bd[0]=0;
	if(ol) {strncpy(bd,ol,2048);strcat(bd,"/.fceultra");}
#endif
}
#endif

void CreateDirs(char *BaseDirectory)
{
#ifdef WINDOWS
	mkdir(BaseDirectory);
	sprintf(TempArray,"%s\\fcs",BaseDirectory);
	mkdir(TempArray);
	sprintf(TempArray,"%s\\snaps",BaseDirectory);
	mkdir(TempArray);
#elif UNIX
	mkdir(BaseDirectory,S_IRWXU|S_IRGRP|S_IROTH);
	sprintf(TempArray,"%s/fcs",BaseDirectory);
	mkdir(TempArray,S_IRWXU|S_IRGRP|S_IROTH);
	sprintf(TempArray,"%s/snaps",BaseDirectory);
	mkdir(TempArray,S_IRWXU|S_IRGRP|S_IROTH);
#endif
}

