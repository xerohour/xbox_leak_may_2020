#include <xtl.h>

#include "device.h"
#include "draw.h"
#include "ccl.h"

#include "quiz.h"
//#include "d3d8.h"
//#include "d3d8types.h"

#include "tsc.h"

#include "DsoundTest.h"

#pragma warning(disable : 4035)
__int64 __inline rdtsc() {_asm {rdtsc}}
DWORD  __inline rdtscd() {_asm {rdtsc}}
#pragma warning(default : 4035)

int gScore[4] = {0,0,0,0};
DWORD gColor[4] = {0xff0000ff, 0xff00ff00, 0xffff0000, 0xffffffff};

void InitGraphics();
void NormalGraphics();
void Winner1();
void Winner2();
void Winner3();
void Winner4();

void Winner(int player);

void (*WinnerFunc[])() = {Winner1, Winner2, Winner3, Winner4};

extern "C" int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hprev, LPSTR lpcmd, int nShow)
{
	int i;
	BUTTONS b;
	BOOL pressed[4];
	int totalpressed;

	
	b.all = bANYBUTTON;

	TscInit(0);
	gpInit(0);		//initialize device stuff
	drInit();		//initialize drawing routines

    while(1) { 
		gpUpdateDeviceStateInfo();				//update the button state info...
		gpPoll();								//query for the next button states
	
		totalpressed = 0;
		for(i = 0; i < ::gpMaxSlots(); i++) {
			pressed[i] = FALSE;
			if(::gpDeviceExists(i)) {
				if(::cclCheckButtons(i, b.all)) {
					pressed[i] = TRUE;
					totalpressed++;
				}
			}
		}

		if(totalpressed == 0) { 
			//arf.
		} else if (totalpressed == 1) {
			for(i = 0; i < gpMaxSlots(); i++) {
				if(pressed[i]) {
					Winner(i);
				}
			}
		} else {
			while(1) {
				i = rand() & 3;		//get a random number between 0 and 3 until it finds someone who has won
				if(pressed[i]) {
					Winner(i);
					break;
				}
			}
		}


		drCls();
		NormalGraphics();
		drShowScreen();							//and draw the screen
	}

	return 0;
}


BOOL AnyKeyPressed()
{
	return cclCheckButtons(-1, bANYBUTTON);
}

CHAR* GetSound( int player )
{
    CHAR* tszFile = "t:\\media\\audio\\pcm\\player.wav";

	switch ( player ) 
	{

	case 0: 
        tszFile = "t:\\media\\audio\\pcm\\uno.wav";
		break;

	case 1:
        tszFile = "t:\\media\\audio\\pcm\\dos.wav";
		break;

	case 2:
        tszFile = "t:\\media\\audio\\pcm\\tres.wav";
		break;

	case 3:
        tszFile = "t:\\media\\audio\\pcm\\quatro.wav";
		break;

	default:
		tszFile = NULL;

	}

	return tszFile;
}

void Winner(int player)
{
	CDSoundTest test;
    test.OpenAndPlay( "t:\\media\\audio\\pcm\\player.wav");

	__int64 tim;
	tim = ::TscBegin();

	bool bFirst = true;

	while(1) {
		drCls();		
		gpUpdateDeviceStateInfo();				//update the button state info...
		gpPoll();
		WinnerFunc[player]();
		drShowScreen();
		if ((::TscTicksToSeconds(::TscCurrentTime(tim)) > 3) && !AnyKeyPressed()) {
			break;
		}

		if ( true == bFirst )
		{
			bFirst = false;
			test.OpenAndPlay( GetSound( player ) );
		}

	}


}

DWORD Blend(DWORD start, DWORD end, float fade)
{
	float r, g, b, a;
	r = (1 - fade) * (start & 255) + (fade) * (end & 255);
	g = (1 - fade) * ((start >> 8) & 255) + (fade) * ((end >> 8) & 255);
	b = (1 - fade) * ((start >> 16) & 255) + (fade) * ((end >> 16) & 255);
	a = (1 - fade) * (start >> 24) + (fade) * (end >> 24);
	return drUnscaledFloatToRgba(r,g,b,a);
}

void NormalGraphics()
{
	static float fade = 1;
	static start = 0;
	static end = 1;

	fade += .0003f;
	if(fade >= 1) {
		fade = 0;
		start = end;
		end = rand() & 3;
		if(end == start) end ^= 3;
	}
	::drBox(0,0,639,479, Blend(gColor[start], gColor[end], fade));
}


void Winner1()
{
	static float hue = 0;
	hue += .1f;
	if(hue >= 360) hue -= 360;
	DWORD col = Blend(::drHslToRgb(hue, 100, 100, 1), gColor[0], .625);

	drQuad(160, 120, col, 240, 40, col, 320, 40, col, 320, 120, col);
	drQuad(240, 120, col, 320, 120, col, 320, 360, col, 240, 360, col);
	drQuad(160, 360, col, 160, 440, col, 400, 440, col, 400, 360, col);
}
void Winner2()
{
	static float hue = 0;
	hue += .1f;
	if(hue >= 360) hue -= 360;
	DWORD col = Blend(::drHslToRgb(hue, 100, 100, 1), gColor[1], .625);

	drQuad(200, 120, col, 280, 80, col, 280, 160, col, 200, 180, col);
	drQuad(280, 80, col, 280, 160, col, 360, 160, col, 360, 80, col);
	drQuad(360, 80, col, 360, 240, col, 440, 240, col, 440, 160, col);
	drQuad(360, 240, col, 440, 240, col, 360, 320, col, 200, 320, col);
	drQuad(200, 320, col, 440, 320, col, 440, 400, col, 200, 400, col);
}
void Winner3()
{
	static float hue = 0;
	hue += .1f;
	if(hue >= 360) hue -= 360;
	DWORD col = Blend(::drHslToRgb(hue, 100, 100, 1), gColor[2], .625);

	drQuad(160, 120, col, 160, 160, col, 240, 120, col, 240, 40, col);
	drQuad(240, 120, col, 240, 40, col, 320, 40, col, 320, 120, col);
	drQuad(320, 40, col, 320, 240, col, 400, 200, col, 400, 120, col);
	drQuad(320, 200, col, 320, 200, col, 240, 240, col, 320, 280, col);
	drQuad(320, 240, col, 320, 440, col, 400, 360, col, 400, 280, col);
	drQuad(240, 440, col, 240, 360, col, 320, 360, col, 320, 440, col);
	drQuad(240, 440, col, 240, 360, col, 160, 320, col, 160, 400, col);
}
void Winner4()
{
	static float hue = 0;
	hue += .1f;
	if(hue >= 360) hue -= 360;
	DWORD col = Blend(::drHslToRgb(hue, 100, 100, 1), gColor[3], .625);

	drQuad(320, 40, col, 320, 120, col, 240, 200, col, 160, 200, col);
	drQuad(160, 200, col, 480, 200, col, 480, 280, col, 160, 280, col);
	drQuad(320, 40, col, 400, 40, col, 400, 200, col, 320, 200, col);
	drQuad(400, 280, col, 320, 280, col, 320, 440, col, 400, 440, col);
}
