/*                      SVGA High Level Routines
FCE / FCE Ultra
*/
#ifndef NOSIGNALS
//#include <signal.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
//#include <sys/stat.h>
//#include <unistd.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "m6502.h"
#include "svga.h" 
#include "fce.h"
#include "mapper.h"
#include "config.h"
#include "general.h"
#include "video.h"
#include "sound.h"
#include "version.h"
#include "nsf.h"
#include "palette.h"
#include "fds.h"
#include "netplay.h"
#include "state.h"
#include "keyscan.h"

#ifdef FPS
extern unsigned long ploinker;
#endif

char SaveStateStatus[10];

int vmode=1;
int soundon=1;

#ifndef NETWORK
#define netplay 0
#endif

static byte showhelp=0;
static byte StateShow=0;

byte Exit=0;

byte joyused=0;

byte ssync=0;
byte dowait=1,SuperSpeed=0;
byte DIPS=0;
int coinon=0;

byte howlong;
byte errmsg[65];
byte maxline=232;
byte pale=0;

static pal *palpoint[8]=
{
	palette,palettevscv,palettevssmb,palettevsmar,palettevsgrad,palettevsgoon,palettevsslalom,palettevseb
};
static pal *palo;
static int lastd=-1;
void SetNESDeemph(byte d, byte f)
{
	static double rt[7]={1.239,.794,1.019,.905,1.023,.741,.75};
	static double gt[7]={.915,1.086,.98,1.026,.908,.987,.75};
	static double bt[7]={.743,.882,.653,1.277,.979,.101,.75};
	double r,g,b;
	
	int x;
	if(!f) if(d==lastd) return;
	if(!d || MapperNo==666) return;
	
	r=rt[d-1];
	g=gt[d-1];
	b=bt[d-1];
	
    for(x=0;x<0x40;x++)
	{
		int m,n,o;
		m=palo[x].r;
		n=palo[x].g;
		o=palo[x].b;
		m*=(double)r;
		n*=(double)g;
		o*=(double)b;
		if(m>0x3f) m=0x3f;
		if(n>0x3f) n=0x3f;
		if(o>0x3f) o=0x3f;
		SetPalette(x|0xC0,m<<2,n<<2,o<<2);
	}
	lastd=d;
}

void SetNESPalette(void)
{
    int x;
    palo=palpoint[pale];
	
    if(MapperNo==666)
    {
		for(x=0;x<256;x++)
			SetPalette(x,NSFPalette[x].r<<2,NSFPalette[x].g<<2,NSFPalette[x].b<<2); 
    }
    else
		for(x=0;x<128;x++)
			SetPalette(x,palo[x&0x3f].r<<2,palo[x&0x3f].g<<2,palo[x&0x3f].b<<2); 
}

void SetGeneralPalette(void)
{
    int x;
    if(MapperNo!=666)
		for(x=0;x<6;x++)
			SetPalette(x+128,unvpalette[x].r<<2,unvpalette[x].g<<2,unvpalette[x].b<<2);
}

byte controlselect=0;
int ntsccol=0;
int ntsctint=46+10;//64-26;
int ntschue=72;//64+13;
int controllength=0;

#define HUEVAL  ((double)((double)ntschue/(double)2)+(double)300)
#define TINTVAL ((double)((double)ntsctint/(double)128))

void UpdatePalette(void)
{
	int x,z;
	int r,g,b;
	double s,y,theta;
	static int cols[16]={0,240,210,180,150,120,90,60,30,0,330,300,270,0,0,0};
	static double br1[4]={.5,.75,1,1};
	static double br2[4]={.29,.45,.73,.9};
	static double br3[4]={0,.24,.47,.77};
	
	for(x=0;x<=3;x++)
		for(z=0;z<16;z++)
		{
			s=(double)TINTVAL;
			y=(double)br2[x];
			if(z==0)  {s=0;y=(double)br1[x];}
			
			if(z==13) {s=0;y=(double)br3[x];}
			if(z==14) {y=0;s=0;}
			if(z==15) {y=0;s=0;}
			theta=(double)M_PI*(double)(((double)cols[z]+HUEVAL)/(double)180);    
			r=(int)(((double)y+(double)s*(double)sin(theta))*(double)256);
			g=(int)(((double)y-(double)((double)27/(double)53)*s*(double)sin(theta)+(double)((double)10/(double)53)*s*cos(theta))*(double)256);
			b=(int)(((double)y-(double)s*(double)cos(theta))*(double)256);  
			
			if(r>255) r=255;
			if(g>255) g=255;
			if(b>255) b=255;
			if(r<0) r=0;
			if(g<0) g=0;
			if(b<0) b=0;
			
			palette[x*16+z].r=r>>2;
			palette[x*16+z].g=g>>2;
			palette[x*16+z].b=b>>2;
		}
		SetNESPalette();
		SetNESDeemph(lastd,1);
}

#include "drawing.h"
#include "help.h"
#ifdef FPS
unsigned long bloinker=0;
#endif
void PutImage(void)
{
	unsigned long x;
	byte x2;
	char *XBaf;
	
#ifdef FPS
	bloinker++;
#endif
	if(!SuperSpeed)
		if(dowait)
			WaitForVBlank();
		
        XBaf=XBuf;        
		
        if(MapperNo==666)
        {
			XBuf+=8;
			DrawNSF();
        }
        else
        {
			DrawMessage();
			DrawDips();
			if(StateShow) DrawState();
			if(showhelp) DisplayHelp();
			if(PAL && vmode!=6) XBuf+=8;
			else XBuf+=2184;
			if(controllength) {controllength--;DrawBars();}
        }
		BlitScreen();
		XBuf=XBaf;
}
#ifndef NOSIGNALS
void SetSignals(void (*t)(int))
{
#ifndef WINDOWS
	int sigs[11]={SIGINT,SIGTERM,SIGHUP,SIGPIPE,SIGSEGV,SIGFPE,SIGKILL,SIGALRM,SIGABRT,SIGUSR1,SIGUSR2};
	int x;
	for(x=0;x<11;x++)
		signal(sigs[x],t);
#endif
}
#endif

/* TrashMachine - Uninitialise everything */
void TrashMachine(void)
{
#ifdef FPS
//	printf("%f\n",(double)bloinker/ploinker);
#endif
	
	/* Don't want to be interrupted while shutting down stuff.  Bad things */
	/* might happen. */
#ifndef NOSIGNALS
	SetSignals(SIG_IGN);
#endif
	sprintf(TempArray,"%s\\fceu.cfg",BaseDirectory);
	SaveConfig(TempArray);
	
	if(joy[0]|joy[1]|joy[2]|joy[3]) KillJoystick();
	KeyboardClose();
	ResetVideo();
	if(soundon) TrashSound();
	//if(netplay) NetworkClose();
}
#ifndef NOSIGNALS
void ProtFaultBefore(void) // Occurs during initialization
{
	char szErrorMsg[2048];
	char szBuffer[256];

	ResetVideo();
	sprintf(szErrorMsg, "Iyeee!!!  Segmentation violation(fault) detected.\n");
	sprintf(szBuffer, "This happened during initialization.  Your system may be unstable.\n");
	strcat(szErrorMsg, szBuffer);
	sprintf(szBuffer, "Thanks for trying to use FCE Ultra...\n");
	strcat(szErrorMsg, szBuffer);
	PrintFatalError(szErrorMsg);
	exit(1);
}

void CloseStuff(int signum)
{
	TrashFCE();
	TrashMachine();
//	printf("\nSignal %d has been caught and dealt with...\n",signum);	
#ifndef WINDOWS
	switch(signum)
	{
	case SIGINT:printf("How DARE you interrupt me!\n");break;
	case SIGTERM:printf("MUST TERMINATE ALL HUMANS\n");break;
	case SIGHUP:printf("Reach out and hang-up on someone.\n");break;
	case SIGPIPE:printf("The pipe has broken!  Better watch out for floods...\n");break;
	case SIGSEGV:printf("Iyeeeeeeeee!!!  A segmentation fault has occurred.  Have a fluffy day.\n");break;
#ifdef SIGBUS
	case SIGBUS:printf("I told you to be nice to the driver.\n");break;
#endif
	case SIGFPE:printf("Those darn floating points.  Ne'er know when they'll bite!\n");break;
	case SIGKILL:printf("Your OS must be very screwy if you're reading this!\n");break;
	case SIGALRM:printf("Don't throw your clock at the meowing cats!\n");break;
	case SIGABRT:printf("Abort, Retry, Ignore, Fail?\n");break;
	case SIGUSR1:
	case SIGUSR2:printf("Killing your processes is not nice.\n");break;
	}
#endif
	exit(0);
}
#endif

int InitMachine(void)
{
	if(!PreInit()) return 0;
	if(!InitVirtualVideo()) return 0;
	CreateDirs(BaseDirectory);
	
#ifndef NOSIGNALS
	SetSignals((void*)CloseStuff);
	signal(SIGSEGV,(void *)ProtFaultBefore);
#endif
	
	if(joy[0]|joy[1]|joy[2]|joy[3]) InitJoystick();
	
	if(soundon)
	{
		int r;
//		printf("Initializing sound...\n");
		if(!(r=InitSound())) 
			soundon=0;
		else
			SetSoundVariables(r);
	}
	
#ifdef NETWORK
	if(!InitNetplay()) return 0;
#endif
	
	if(vmode==6) maxline=224;
	if(!SetVideoMode(vmode)) return(0);
	if(!pale && !PAL && ntsccol) UpdatePalette();
	SetNESPalette();
	SetGeneralPalette();
	KeyboardInitialize();
#ifndef NOSIGNALS
	signal(SIGSEGV,(void *)CloseStuff);
#endif
	return 1;
}

/* Joysticks - Handles input. */

#ifdef WINDOWS
//#include "drivers/win/keyscan.h"
#else
//#include "drivers/"DRIVER_DIR"/keyscan.h"
#endif
byte JoySwap=0;

byte CommandQueue=0;
#ifdef NETWORK

/*	This is a wrapper for network play code. 	*/

uint32 AJoysticks(void);
uint32 Joysticks(void)
{
	uint32 JS;
	JS=AJoysticks();
	if(netplay) NetplayUpdate(&JS);
	return JS;
}
#endif

#define KEY(__a) keys[SCANCODE_##__a]
#define keyonly(__a,__z) {if(KEY(__a)){if(!keyonce[SCANCODE_##__a]){keyonce[SCANCODE_##__a]=1;__z}}else{keyonce[SCANCODE_##__a]=0;}}

static byte keyonce[256];
#ifdef NETWORK
uint32 AJoysticks(void)
{
#else
	uint32 Joysticks(void)
	{
#endif
		int x,y;
		static byte first=0;
		static unsigned long JS = 0;
		static byte keycount=0;
		char* keys;
		
#ifdef WINDOWS
		keycount=(keycount+1)%6;
#endif
		if(first) if(!KeyboardUpdate())
		{
			if(joy[0]|joy[1]|joy[2]|joy[3]) return JS|(uint32)GetJSOr();
			return JS;
		}
		first=1;
		JS=0;
		keys = KeyboardGetstate();
		joy1or=joy2or=0;
		if(GameAttributes&GA_VSUNI)
		{
			joy1or|=(vsdip&3)<<3;
			joy2or|=(vsdip&0xFC);
			keyonly(C,CommandQueue=19;) 
				if(coinon && (!KEY(C))) CommandQueue=20;
				if(coinon) joy1or|=0x04;
		}
		
		
		if(MapperNo!=666)
		{
			if(netplay!=2)
			{
				keyonly(F10,CommandQueue=30;)
					keyonly(V,CommandQueue=10;)
					if(GameAttributes&GA_VSUNI && DIPS&1)
					{
						keyonly(1,CommandQueue=11;)
							keyonly(2,CommandQueue=12;)
							keyonly(3,CommandQueue=13;)
							keyonly(4,CommandQueue=14;)
							keyonly(5,CommandQueue=15;)
							keyonly(6,CommandQueue=16;)
							keyonly(7,CommandQueue=17;)
							keyonly(8,CommandQueue=18;)
					}
					else if(!netplay)
					{
						byte ssel;
						ssel=0;
						keyonly(0,ssel=1;StateShow=180;CurrentState=0;)
							keyonly(1,ssel=1;StateShow=180;CurrentState=1;)
							keyonly(2,ssel=1;StateShow=180;CurrentState=2;)
							keyonly(3,ssel=1;StateShow=180;CurrentState=3;)
							keyonly(4,ssel=1;StateShow=180;CurrentState=4;)
							keyonly(5,ssel=1;StateShow=180;CurrentState=5;)
							keyonly(6,ssel=1;StateShow=180;CurrentState=6;)
							keyonly(7,ssel=1;StateShow=180;CurrentState=7;)
							keyonly(8,ssel=1;StateShow=180;CurrentState=8;)
							keyonly(9,ssel=1;StateShow=180;CurrentState=9;)
							if(ssel)
								CheckStates();
							keyonly(F5,CheckStates();StateShow=0;SaveState();)
								keyonly(F7,CheckStates();StateShow=0;LoadState();)
					}
			}  
		}
		
		if( KEY(ESCAPE) || KEY(F12) )
			Exit=1;
		
#ifdef LOCK_CONSOLE
		keyonly(L,if(LockConsole()) sprintf(errmsg,"Console locked.");howlong=180;)
			keyonly(U,if(UnlockConsole()) sprintf(errmsg,"Console unlocked.");howlong=180;)
#endif
			
			if(ntsccol && !pale && !PAL && MapperNo!=666)
			{
				keyonly(H,controlselect=1;controllength=360;)
					keyonly(T,controlselect=2;controllength=360;) 
					if(!keycount && (KEY(KEYPADPLUS) || KEY(EQUAL)))
					{ 
						if(controlselect)
						{
							if(controllength)
							{     
								switch(controlselect)
								{
								case 1:ntschue++;
									if(ntschue>128) ntschue=128;
									UpdatePalette();
									break;
								case 2:ntsctint++;
									if(ntsctint>128) ntsctint=128;
									UpdatePalette();
									break;
								}    
							}
							controllength=360;
						}
					}
					if(!keycount && (KEY(KEYPADMINUS) || KEY(MINUS)))
					{
						char which;
						if(controlselect)
						{
							if(controllength)
							{          
								which=controlselect==1?ntschue:ntsctint;
								which--; 
								if(which<0) 
									which=0;

								if(controlselect == 1)
									ntschue = which;
								else
									ntsctint = which;

								//(controlselect==1?ntschue:ntsctint)=which;    

								UpdatePalette();
							}
							controllength=360;
						}
					}
			}
			keyonly(F1,showhelp^=1;)
				keyonly(D,if(GameAttributes&GA_VSUNI){DIPS^=1;if(DIPS&1) {sprintf(errmsg,"Dip switches modifyable.");}       else {sprintf(errmsg,"Dip switches not modifyable.");}howlong=180;})
				keyonly(F9,writepcx();)
				if(KEY(GRAVE)) SuperSpeed=1;
				else SuperSpeed=0;
				if(!netplay) keyonly(CAPSLOCK,JoySwap=(JoySwap+8)%32;sprintf(errmsg,"Joystick %d selected.",(JoySwap>>3)+1);howlong=180;)
					
					if(MapperNo==777 && netplay!=2)
					{
						keyonly(S,CommandQueue=1;)
							keyonly(I,CommandQueue=2;)
							keyonly(E,CommandQueue=3;)  
					}
					
					if(MapperNo==666) 
					{
						keyonly(CURSORLEFT,NSFControl(2);)
							keyonly(CURSORRIGHT,NSFControl(1);)
							if(!keycount)
							{
								if( KEY(ENTER)) NSFControl(0);
								if( KEY(CURSORUP)) NSFControl(1);
								if( KEY(CURSORDOWN)) NSFControl(2);  
							}
					}
					else
					{
						x=y=0;
						if(KEY(LEFTALT) || KEY(X))        JS|=JOY_A<<JoySwap;
						if(KEY(LEFTCONTROL) || KEY(SPACE) || KEY(Z) ) JS |=JOY_B<<JoySwap;
						if(KEY(ENTER))       JS |= JOY_START<<JoySwap;
						if(KEY(TAB))         JS |= JOY_SELECT<<JoySwap;
						if(KEY(CURSORDOWN))  y|= JOY_DOWN;
						if(KEY(CURSORUP))    y|= JOY_UP;
						if(KEY(CURSORLEFT))  x|= JOY_LEFT;
						if(KEY(CURSORRIGHT)) x|= JOY_RIGHT;
						if(y!=(JOY_DOWN|JOY_UP)) JS|=y<<JoySwap;
						if(x!=(JOY_LEFT|JOY_RIGHT)) JS|=x<<JoySwap;
					}
					
					if(!netplay && CommandQueue) {DoCommand(CommandQueue);CommandQueue=0;}
					
					if(joy[0]|joy[1]|joy[2]|joy[3])
						return JS|(uint32)GetJSOr();
					else
						return JS;
}

void DoCommand(byte c)
{
	switch(c)
	{
	case 1:FDSControl(FDS_SELECT);break;
	case 2:FDSControl(FDS_IDISK);break;
	case 3:FDSControl(FDS_EJECT);break;
		
	case 10:DIPS^=2;break;
	case 11:vsdip^=1;DIPS|=2;break;
	case 12:vsdip^=2;DIPS|=2;break;
	case 13:vsdip^=4;DIPS|=2;break;
	case 14:vsdip^=8;DIPS|=2;break;
	case 15:vsdip^=0x10;DIPS|=2;break;
	case 16:vsdip^=0x20;DIPS|=2;break;
	case 17:vsdip^=0x40;DIPS|=2;break;
	case 18:vsdip^=0x80;DIPS|=2;break;
	case 19:coinon=1;break;
	case 20:coinon=0;break;
	case 30:ResetNES();break;
	}
}
