//#define __HAMMERHEAD__
//define __TWOPASS__

#include "xtl.h"
#include "Draw.h"
//#include "Device.h"
#include "dukeprod.h"
#include "dp.h"

#define countof(_x_) (sizeof(_x_) / sizeof(_x_[0]))

#define nc 8192
typedef struct {
	float x1;
	float y1;
	float x2;
	float y2;
	BOXSTATUS stat;
} BOXSTRUCT;

typedef struct {
	signed short minx;
	signed short miny;
	signed short maxx;
	signed short maxy;
	BOOL button;
	int index;
} TPTESTSTRUCT;


#pragma warning(disable : 4035)
__int64 __inline rdtsc() {_asm {rdtsc}}
int __inline rdtsci(){_asm{rdtsc}}
BYTE  __inline rolb( BYTE narf, int r){_asm{mov  al, narf}_asm{mov ecx,r}_asm{rol  al,cl}}
WORD  __inline rolw( WORD narf, int r){_asm{mov  ax, narf}_asm{mov ecx,r}_asm{rol  ax,cl}}
DWORD __inline rold(DWORD narf, int r){_asm{mov eax, narf}_asm{mov ecx,r}_asm{rol eax,cl}}
BYTE  __inline rorb( BYTE narf, int r){_asm{mov  al, narf}_asm{mov ecx,r}_asm{ror  al,cl}}
WORD  __inline rorw( WORD narf, int r){_asm{mov  ax, narf}_asm{mov ecx,r}_asm{ror  ax,cl}}
DWORD __inline rord(DWORD narf, int r){_asm{mov eax, narf}_asm{mov ecx,r}_asm{ror eax,cl}}
#pragma warning(default : 4035)

void drawbox(BOXSTRUCT* b)
{
	DWORD col; 

	switch(b->stat)
	{
	case notyet:
		col = 0xff0000ff;
		break;
	case testme:
		col = (rdtsci()&0x8000000) ? 0xffffffff : 0xff707070;	//blink grey, .4 second intravals
		break;
	case pending:
		col = 0xffffff00;
		break;
	case failed:
		col = 0xffff0000;
		break;
	case passed:
		col = 0xff00ff00;
		break;
	}

	drBox(b->x1, b->y1, b->x2, b->y2, col);
	
}

#define DRAWALL() { for(int __i_ = 0; __i_ < numtocheck; __i_++) drawbox(&bs[__i_]); }


/*
//template: to create a new test:
	copy this, 
	name it appropriately, 
	add a reference to dukeprod.cpp
	change the locations in the bs[] array
	add test code where the // is.


BOXSTATUS TEMPLATE(BOXSTATUS newstatus)
{
	static BOXSTRUCT bs[] = {
		{10,10,11,20,notyet},
		{12,10,15,20,notyet},
		{16,10,19,20,notyet},
		{20,10,23,20,notyet},
		{24,10,25,20,notyet}
	};
	static const int numtocheck = countof(bs);

	int i;
	BOOL doh = FALSE;

	switch(newstatus)
	{
	case notyet:
		for(i = 0; i < numtocheck; i++) {
			bs[i].stat = notyet;
		}
		DRAWALL();
		return notyet;
	case testme:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) {
				bs[i].stat = testme;
				doh = TRUE;
			}
		}
		if(!doh) return passed;

		//add test-checking code here

		DRAWALL();
		return testme;
	case failed:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) bs[i].stat = failed;
		}
		DRAWALL();
		return failed;
	case passed:
		DRAWALL();
		return passed;
	default:
		_asm int 3;
		return failed;
	}
}
*/

BOXSTATUS LeftTriggerTest(BOXSTATUS newstatus)
{
	static BYTE roller = 0;
	static BOXSTRUCT bs[] = {
		{20,70,23,90,notyet},
		{24,70,31,90,notyet},
		{32,70,39,90,notyet},
		{40,70,47,90,notyet},
		{48,70,51,90,notyet},
		{24,62,28,66,notyet}
	};

	static const int numtocheck = countof(bs);

	BYTE data = gData->bAnalogButtons[GPL];
	int i;
	BOOL doh = FALSE;

	switch(newstatus)
	{
	case notyet:
		for(i = 0; i < numtocheck; i++) {
			bs[i].stat = notyet;
		}
		roller = 0;
		DRAWALL();
		return notyet;
	case testme:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) {
				bs[i].stat = testme;
				doh = TRUE;
			}
		}
		if(!doh) return passed;

#ifdef __HAMMERHEAD__
		if(data == 0)	
			bs[0].stat = passed;
		bs[1].stat = passed;
		bs[2].stat = passed;
		bs[3].stat = passed;
		if(data == 0xff)
			bs[4].stat = passed;
		if(bs[4].stat == passed) {
			if(data == 0 || bs[5].stat == passed) {		//retract!
				bs[5].stat = passed;
			} else {
				bs[5].stat = pending;
			}
		}
#else 
		if(data == 0)	
			bs[0].stat = passed;
		if(data > 0x00 && data <= 0x55)		
			bs[1].stat = passed;
		if(data > 0x55 && data <= 0xAA)		
			bs[2].stat = passed;
		if(data > 0xAA && data <  0xff)
			bs[3].stat = passed;
		if(data == 0xff)
			bs[4].stat = passed;
		roller |= (rolb(data, 1) ^ data);
		if(roller == 0xff) {
			if(bs[4].stat == passed) {
				if(data == 0 || bs[5].stat == passed) {	//retract!
					bs[5].stat = passed;
				} else {
					bs[5].stat = pending;
				}
			}
		}
#endif

		DRAWALL();
		drPrintf(0,0,L"Left Trigger: %02hx", data);
		return testme;
	case failed:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) bs[i].stat = failed;
		}
		DRAWALL();
		return failed;
	case passed:
		DRAWALL();
		return passed;
	default:
		_asm int 3;
		return failed;
	}
}

BOXSTATUS RightTriggerTest(BOXSTATUS newstatus)
{
	static BYTE roller = 0;
	static BOXSTRUCT bs[] = {
		{520,70,523,90,notyet},
		{524,70,531,90,notyet},
		{532,70,539,90,notyet},
		{540,70,547,90,notyet},
		{548,70,551,90,notyet},
		{524,62,528,66,notyet}
	};

	static const int numtocheck = sizeof(bs) / sizeof(bs[0]);

	BYTE data = gData->bAnalogButtons[GPR];
	int i;
	BOOL doh = FALSE;

	switch(newstatus)
	{
	case notyet:
		for(i = 0; i < numtocheck; i++) {
			bs[i].stat = notyet;
		}
		roller = 0;
		DRAWALL();
		return notyet;
	case testme:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) {
				bs[i].stat = testme;
				doh = TRUE;
			}
		}
		if(!doh) return passed;

#ifdef __HAMMERHEAD__
		if(data == 0)	
			bs[0].stat = passed;
		bs[1].stat = passed;
		bs[2].stat = passed;
		bs[3].stat = passed;
		if(data == 0xff)
			bs[4].stat = passed;
		if(bs[4].stat == passed) {
			if(data == 0 || bs[5].stat == passed) {		//retract!
				bs[5].stat = passed;
			} else {
				bs[5].stat = pending;
			}
		}
#else 
		if(data == 0)	
			bs[0].stat = passed;
		if(data > 0x00 && data <= 0x55)		
			bs[1].stat = passed;
		if(data > 0x55 && data <= 0xAA)		
			bs[2].stat = passed;
		if(data > 0xAA && data <  0xff)
			bs[3].stat = passed;
		if(data == 0xff)
			bs[4].stat = passed;
		roller |= (rolb(data, 1) ^ data);
		if(roller == 0xff) {
			if(bs[4].stat == passed) {
				if(data == 0 || bs[5].stat == passed) {	//retract!
					bs[5].stat = passed;
				} else {								
					bs[5].stat = pending;
				}
			}
		}
#endif
		DRAWALL();
		drPrintf(0,0,L"Right Trigger: %02hx", data);
		return testme;
	case failed:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) bs[i].stat = failed;
		}
		DRAWALL();
		return failed;
	case passed:
		DRAWALL();
		return passed;
	default:
		_asm int 3;
		return failed;
	}
}



BOXSTATUS LeftThumbStick1Test(BOXSTATUS newstatus)
{
	static TPTESTSTRUCT tp[] = {				//in-order checkoff
		{   -nc,   -nc,    nc,    nc,0, 9},
		{-16000,-16000, 16000, 16000,0, 4},

		{-16000, 16001, 16000, 32767,0, 1},
		{-32768, 16001,-16001, 32767,0, 0},
		{-32768,-16000,-16001, 16000,0, 3},
		{-32768,-32768,-16001,-16001,0, 6},
		{-16000,-32768, 16000,-16001,0, 7},
		{ 16001,-32768, 32767,-16001,0, 8},
		{ 16001,-16000, 32767, 16000,0, 5},
		{ 16001, 16001, 32767, 32767,0, 2},

#ifdef __TWOPASS__
		{-16000, 16001, 16000, 32767,1, 1},
		{-32768, 16001,-16001, 32767,1, 0},
		{-32768,-16000,-16001, 16000,1, 3},
		{-32768,-32768,-16001,-16001,1, 6},
		{-16000,-32768, 16000,-16001,1, 7},
		{ 16001,-32768, 32767,-16001,1, 8},
		{ 16001,-16000, 32767, 16000,1, 5},
		{ 16001, 16001, 32767, 32767,1, 2},

		{-16000,-16000, 16000, 16000,1, 4},
#endif
		//{     0,     0,     0,     0,1, 9},
		{   -nc,   -nc,    nc,    nc, 1, 9},
		{   -nc,   -nc,    nc,    nc, 0, 9}
		//{     0,     0,     0,     0,0, 9}
	};

	/*
//#pragma warning (disable : 4309; disable : 4305)//either this, or (short)... ugh.
	static TPTESTSTRUCT tp2[] = {					//any-order checkoff
		{(short)0x8000,(short)0x8000,(short)0x8000,(short)0x7fff,0,10},
		{(short)0x8001,(short)0x8000,(short)0xa000,(short)0x7fff,0,11},
		{(short)0xa001,(short)0x8000,(short)0xc000,(short)0x7fff,0,12},
		{(short)0xc001,(short)0x8000,(short)0xe000,(short)0x7fff,0,13},
		{(short)0xe001,(short)0x8000,(short)0xffff,(short)0x7fff,0,14},
		{(short)0x0000,(short)0x8000,(short)0x0000,(short)0x7fff,0,15},
		{(short)0x0001,(short)0x8000,(short)0x2000,(short)0x7fff,0,16},
		{(short)0x2001,(short)0x8000,(short)0x4000,(short)0x7fff,0,17},
		{(short)0x4001,(short)0x8000,(short)0x6000,(short)0x7fff,0,18},
		{(short)0x6001,(short)0x8000,(short)0x7ffe,(short)0x7fff,0,19},
		{(short)0x7fff,(short)0x8000,(short)0x7fff,(short)0x7fff,0,20},

		{(short)0x8000,(short)0x8000,(short)0x7fff,(short)0x8000,0,21},
		{(short)0x8000,(short)0x8001,(short)0x7fff,(short)0xa000,0,22},
		{(short)0x8000,(short)0xa001,(short)0x7fff,(short)0xc000,0,23},
		{(short)0x8000,(short)0xc001,(short)0x7fff,(short)0xe000,0,24},
		{(short)0x8000,(short)0xe001,(short)0x7fff,(short)0xffff,0,25},
		{(short)0x8000,(short)0x0000,(short)0x7fff,(short)0x0000,0,26},
		{(short)0x8000,(short)0x0001,(short)0x7fff,(short)0x2000,0,27},
		{(short)0x8000,(short)0x2001,(short)0x7fff,(short)0x4000,0,28},
		{(short)0x8000,(short)0x4001,(short)0x7fff,(short)0x6000,0,29},
		{(short)0x8000,(short)0x6001,(short)0x7fff,(short)0x7ffe,0,30},
		{(short)0x8000,(short)0x700f,(short)0x7fff,(short)0x7fff,0,31}
	};
//#pragma warning(default : 4309; default : 4305)
*/

	static BOXSTRUCT bs[] = {
		{40,160,59,179,notyet},				//0
		{60,160,79,179,notyet},				//1
		{80,160,99,179,notyet},
		{40,180,59,199,notyet},				//3	
		{60,180,79,199,notyet},				//4
		{80,180,99,199,notyet},
		{40,200,59,219,notyet},				//6
		{60,200,79,219,notyet},
		{80,200,99,219,notyet},	

		{67,187,73,193,notyet},	//centered	//9
/*
		{42,150,46,154,notyet},	//   0x80	//10
		{47,150,51,154,notyet},	//to 0xa0
		{52,150,56,154,notyet},	//to 0xc0
		{57,150,61,154,notyet},	//to 0xe0
		{62,150,66,154,notyet},	//to 0xff
		{67,150,71,154,notyet},	//   0x00
		{72,150,76,154,notyet},	//to 0x20
		{77,150,81,154,notyet},	//to 0x40
		{82,150,86,154,notyet},	//to 0x60
		{87,150,91,154,notyet},	//to 0x7e
		{92,150,96,154,notyet},	//   0x7f	//20

		{105,212,109,216,notyet},	//   0x7f	//31
		{105,207,109,211,notyet},	//to 0x7e
		{105,202,109,206,notyet},	//to 0x60
		{105,197,109,201,notyet},	//to 0x40
		{105,192,109,196,notyet},	//to 0x20
		{105,187,109,191,notyet},	//   0x00
		{105,182,109,186,notyet},	//to 0xff
		{105,177,109,181,notyet},	//to 0xe0
		{105,172,109,176,notyet},	//to 0xc0
		{105,167,109,171,notyet},	//to 0xa0
		{105,162,109,166,notyet},	//   0x80	//21
*/
		{110,177,126,193,notyet}    //button	//32
	};

	static const int numtocheck = sizeof(bs) / sizeof(bs[0]);

	int i;
	BOOL doh = FALSE;
	static int c = 0;

	signed short x = gData->sThumbLX;
	signed short y = gData->sThumbLY;
	BOOL bp = (gData->wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? TRUE : FALSE;

	switch(newstatus)
	{
	case notyet:
		for(i = 0; i < numtocheck; i++) {
			bs[i].stat = notyet;
		}
		DRAWALL();
		c = 0;
		return notyet;
	case testme:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) {
				doh = TRUE;
			}
		}
		if(!doh) return passed;

		if(c >= sizeof(tp) / sizeof(tp[0])) {
			bs[sizeof(bs)/sizeof(bs[0])-1].stat = passed;
		} else {
			if(x > tp[c].maxx || x < tp[c].minx || 
				y > tp[c].maxy || y < tp[c].miny ||
				bp != tp[c].button || ((tp[c].maxx | tp[c].minx | tp[c].maxy | tp[c].miny) && (!(x | y))) )
			{
				bs[tp[c].index].stat = testme;
				if(c!=(numtocheck-2)) bs[sizeof(bs)/sizeof(bs[0])-1].stat = tp[c].button ? testme : pending;
				else bs[sizeof(bs)/sizeof(bs[0])-1].stat=pending;
			} else {
				for(i = c + 1; i < sizeof(tp) / sizeof(tp[0]); i++)
				{
					if(tp[i].index == tp[c].index) {
						bs[tp[c].index].stat = pending;
						goto notdone;
					}
				}
				bs[tp[c].index].stat = passed;
notdone:

//				bs[tp[c].index].stat = tp[c].button ? passed : pending;
				c++;
			}
		}

/*
		for(i = 0; i < sizeof(tp2) / sizeof(tp2[0]); i++)
		{
			if(x > tp2[i].maxx || x < tp2[i].minx ||
				y > tp2[i].maxy || y < tp2[i].miny)
			{
				if(bs[tp2[i].index].stat != passed) bs[tp2[i].index].stat = testme;
			} else {
				bs[tp2[i].index].stat = passed;
			}
		}
*/

		::drPrintf(0,0,L"Left Thumb-Stick: X: %6hd   Y: %6hd  B: %s", 
			x, y, bp ? L"TRUE" : L"FALSE");
		DRAWALL();
		return testme;
	case failed:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) bs[i].stat = failed;
		}
		DRAWALL();
		return failed;
	case passed:
		DRAWALL();
		return passed;
	default:
		_asm int 3;
		return failed;
	}
}

BOXSTATUS DPadTest(BOXSTATUS newstatus)
{
	static struct {
		BYTE d[4];
	} bd[] = {
		{1,0,0,0},
		{1,0,1,0},
		{0,0,1,0},
		{0,1,1,0},
		{0,1,0,0},
		{0,1,0,1},
		{0,0,0,1},
		{1,0,0,1},
		{0,0,0,0}
	};
	static BOXSTRUCT bs[] = {
		{115,335,205,425,notyet},	//all?
		{150,340,170,360,notyet},	//up
		{120,340,140,360,notyet},	//upleft
		{120,370,140,390,notyet},	//left
		{120,400,140,420,notyet},	//downleft
		{150,400,170,420,notyet},	//down
		{180,400,200,420,notyet},	//downright
		{180,370,200,390,notyet},	//right
		{180,340,200,360,notyet},	//upright
		{150,370,170,390,notyet} 	//centered
	};
	static const int numtocheck = sizeof(bs) / sizeof(bs[0]);

	int i;
	static int c = 0;
	BOOL doh = FALSE;
	BYTE data[] = {
		gData->wButtons & XINPUT_GAMEPAD_DPAD_UP    ? 1 : 0,
		gData->wButtons & XINPUT_GAMEPAD_DPAD_DOWN  ? 1 : 0,
		gData->wButtons & XINPUT_GAMEPAD_DPAD_LEFT  ? 1 : 0,
		gData->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1 : 0
	};


	switch(newstatus)
	{
	case notyet:
		for(i = 0; i < numtocheck; i++) {
			bs[i].stat = notyet;
		}
		DRAWALL();
		c = 1;
		return notyet;
	case testme:
		for(i = 1; i < numtocheck; i++) {
			if(bs[i].stat != passed) {
				doh = TRUE;
			}
		}
		if(doh) {
			goto doh;
		}

		if(bs[0].stat == failed) {
			return failed;
		}

		return passed;
doh:
		if((data[0] && data[1]) || (data[2] && data[3])) {
			bs[0].stat = failed;
		}

		for (i = 0; i < 4; i++)	{
			if(data[i] != bd[c-1].d[i]) {
				bs[c].stat = testme;
				goto doh2;
			}
		}
		bs[c].stat = passed;
		c++;

doh2:

		DRAWALL();
		drPrintf(0,0,L"Up: %d  Down: %d  Left: %d  Right: %d",data[0],data[1],data[2],data[3]);
		return testme;
	case failed:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) bs[i].stat = failed;
		}
		DRAWALL();
		return failed;
	case passed:
		DRAWALL();
		return passed;
	default:
		_asm int 3;
		return failed;
	}
}


BOXSTATUS StartSelectTest(BOXSTATUS newstatus)
{
	static BOXSTRUCT bs[] = {
		{240,400,250,410,notyet},
		{280,400,290,410,notyet}
	};
	static const int numtocheck = sizeof(bs) / sizeof(bs[0]);
	static int c = 0;
	int i;
	BOOL doh = TRUE;
	BYTE data[] = {
		gData->wButtons & XINPUT_GAMEPAD_BACK     ? 1 : 0,
		gData->wButtons & XINPUT_GAMEPAD_START    ? 1 : 0,
	};

	switch(newstatus)
	{
	case notyet:
		for(i = 0; i < numtocheck; i++) {
			bs[i].stat = notyet;
		}
		DRAWALL();
		c = 0;
		return notyet;
	case testme:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) {
				doh = TRUE;
			}
		}
		if(!doh) return passed;

		switch (c) {
		case 0:
			if(data[0] == 0 || data[1] == 1) {
				bs[0].stat = testme;
			} else {
				bs[0].stat = pending;
				c++;
			}
			break;
		case 1:
			if(data[0] == 0) {
				bs[0].stat = passed;
				bs[1].stat = testme;
				c++;
			}
			break;
		case 2:
			if(data[0] == 1 || data[1] == 0) {
				bs[1].stat = testme;
			} else {
				bs[1].stat = pending;
				c++;
			}
			break;
		case 3:
			if(data[1] == 0) {
				bs[1].stat = passed;
				return passed;
			}
			break;
		}

		DRAWALL();
		drPrintf(0,0,L"Start: %d   Back: %d", data[0], data[1]);
		return testme;
	case failed:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) bs[i].stat = failed;
		}
		DRAWALL();
		return failed;
	case passed:
		DRAWALL();
		return passed;
	default:
		_asm int 3;
		return failed;
	}
}


BOXSTATUS RightThumbStick1Test(BOXSTATUS newstatus)
{
	static TPTESTSTRUCT tp[] = {				//in-order checkoff
		{     -nc,     -nc,     nc,     nc,0, 9},
		{-16000, -16000,  16000,  16000,0, 4},

		{-16000, 16001, 16000, 32767,0, 1},
		{-32768, 16001,-16001, 32767,0, 0},
		{-32768,-16000,-16001, 16000,0, 3},
		{-32768,-32768,-16001,-16001,0, 6},
		{-16000,-32768, 16000,-16001,0, 7},
		{ 16001,-32768, 32767,-16001,0, 8},
		{ 16001,-16000, 32767, 16000,0, 5},
		{ 16001, 16001, 32767, 32767,0, 2},

#ifdef __TWOPASS__
		{-16000, 16001, 16000, 32767,1, 1},
		{-32768, 16001,-16001, 32767,1, 0},
		{-32768,-16000,-16001, 16000,1, 3},
		{-32768,-32768,-16001,-16001,1, 6},
		{-16000,-32768, 16000,-16001,1, 7},
		{ 16001,-32768, 32767,-16001,1, 8},
		{ 16001,-16000, 32767, 16000,1, 5},
		{ 16001, 16001, 32767, 32767,1, 2},

		{ -16000, -16000,  16000,  16000,1, 4},
#endif
		{   -nc,   -nc,    nc,    nc,1, 9},
		{   -nc,   -nc,    nc,    nc,0, 9}
		
		//{     0,     0,     0,     0,1, 9},
		//{     0,     0,     0,     0,0, 9}
	};

	/*
//#pragma warning (disable : 4309; disable : 4305)//either this, or (short)... ugh.
	static TPTESTSTRUCT tp2[] = {					//any-order checkoff
		{(short)0x8000,(short)0x8000,(short)0x8000,(short)0x7fff,0,10},
		{(short)0x8001,(short)0x8000,(short)0xa000,(short)0x7fff,0,11},
		{(short)0xa001,(short)0x8000,(short)0xc000,(short)0x7fff,0,12},
		{(short)0xc001,(short)0x8000,(short)0xe000,(short)0x7fff,0,13},
		{(short)0xe001,(short)0x8000,(short)0xffff,(short)0x7fff,0,14},
		{(short)0x0000,(short)0x8000,(short)0x0000,(short)0x7fff,0,15},
		{(short)0x0001,(short)0x8000,(short)0x2000,(short)0x7fff,0,16},
		{(short)0x2001,(short)0x8000,(short)0x4000,(short)0x7fff,0,17},
		{(short)0x4001,(short)0x8000,(short)0x6000,(short)0x7fff,0,18},
		{(short)0x6001,(short)0x8000,(short)0x7ffe,(short)0x7fff,0,19},
		{(short)0x7fff,(short)0x8000,(short)0x7fff,(short)0x7fff,0,20},

		{(short)0x8000,(short)0x8000,(short)0x7fff,(short)0x8000,0,21},
		{(short)0x8000,(short)0x8001,(short)0x7fff,(short)0xa000,0,22},
		{(short)0x8000,(short)0xa001,(short)0x7fff,(short)0xc000,0,23},
		{(short)0x8000,(short)0xc001,(short)0x7fff,(short)0xe000,0,24},
		{(short)0x8000,(short)0xe001,(short)0x7fff,(short)0xffff,0,25},
		{(short)0x8000,(short)0x0000,(short)0x7fff,(short)0x0000,0,26},
		{(short)0x8000,(short)0x0001,(short)0x7fff,(short)0x2000,0,27},
		{(short)0x8000,(short)0x2001,(short)0x7fff,(short)0x4000,0,28},
		{(short)0x8000,(short)0x4001,(short)0x7fff,(short)0x6000,0,29},
		{(short)0x8000,(short)0x6001,(short)0x7fff,(short)0x7ffe,0,30},
		{(short)0x8000,(short)0x700f,(short)0x7fff,(short)0x7fff,0,31}
	};
//#pragma warning(default : 4309; default : 4305)
*/

	static BOXSTRUCT bs[] = {
		{340,360,359,379,notyet},				//0
		{360,360,379,379,notyet},				//1
		{380,360,399,379,notyet},
		{340,380,359,399,notyet},				//3	
		{360,380,379,399,notyet},				//4
		{380,380,399,399,notyet},
		{340,400,359,419,notyet},				//6
		{360,400,379,419,notyet},
		{380,400,399,419,notyet},	
		{367,387,373,393,notyet},	//centered	//9
/*
		{342,350,346,354,notyet},	//   0x80	//10
		{347,350,351,354,notyet},	//to 0xa0
		{352,350,356,354,notyet},	//to 0xc0
		{357,350,361,354,notyet},	//to 0xe0
		{362,350,366,354,notyet},	//to 0xff
		{367,350,371,354,notyet},	//   0x00
		{372,350,376,354,notyet},	//to 0x20
		{377,350,381,354,notyet},	//to 0x40
		{382,350,386,354,notyet},	//to 0x60
		{387,350,391,354,notyet},	//to 0x7e
		{392,350,396,354,notyet},	//   0x7f	//20

		{405,412,409,416,notyet},	//   0x7f	//31
		{405,407,409,411,notyet},	//to 0x7e
		{405,402,409,406,notyet},	//to 0x60
		{405,397,409,401,notyet},	//to 0x40
		{405,392,409,396,notyet},	//to 0x20
		{405,387,409,391,notyet},	//   0x00
		{405,382,409,386,notyet},	//to 0xff
		{405,377,409,381,notyet},	//to 0xe0
		{405,372,409,376,notyet},	//to 0xc0
		{405,367,409,371,notyet},	//to 0xa0
		{405,362,409,366,notyet},	//   0x80	//21
		*/

		{410,377,426,393,notyet}    //button	//32
	};

	static const int numtocheck = sizeof(bs) / sizeof(bs[0]);

	int i;
	BOOL doh = FALSE;
	static int c = 0;

	signed short x = gData->sThumbRX;
	signed short y = gData->sThumbRY;
	BOOL bp = (gData->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? TRUE : FALSE;

	switch(newstatus)
	{
	case notyet:
		for(i = 0; i < numtocheck; i++) {
			bs[i].stat = notyet;
		}
		DRAWALL();
		c = 0;
		return notyet;
	case testme:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) {
//				bs[i].stat = testme;
				doh = TRUE;
			}
		}
		if(!doh) return passed;

		if(c >= sizeof(tp) / sizeof(tp[0])) {
			bs[sizeof(bs)/sizeof(bs[0])-1].stat = passed;
		} else {
			if(x > tp[c].maxx || x < tp[c].minx || 
				y > tp[c].maxy || y < tp[c].miny ||
				bp != tp[c].button || ((tp[c].maxx | tp[c].minx | tp[c].maxy | tp[c].miny) && (!(x | y))) )
			{
				bs[tp[c].index].stat = testme;
				if(c!=(numtocheck-2)) bs[sizeof(bs)/sizeof(bs[0])-1].stat = tp[c].button ? testme : pending;
				else bs[sizeof(bs)/sizeof(bs[0])-1].stat=pending;
			} else {
				for(i = c + 1; i < sizeof(tp) / sizeof(tp[0]); i++)
				{
					if(tp[i].index == tp[c].index) {
						bs[tp[c].index].stat = pending;
						goto notdone;
					}
				}
				bs[tp[c].index].stat = passed;
notdone:

//				bs[tp[c].index].stat = tp[c].button ? passed : pending;
				c++;
			}
		}

/*
		for(i = 0; i < sizeof(tp2) / sizeof(tp2[0]); i++)
		{
			if(x > tp2[i].maxx || x < tp2[i].minx ||
				y > tp2[i].maxy || y < tp2[i].miny)
			{
				if(bs[tp2[i].index].stat != passed) bs[tp2[i].index].stat = testme;
			} else {
				bs[tp2[i].index].stat = passed;
			}
		}
*/

		::drPrintf(0,0,L"Right Thumb-Stick: X: %6hd   Y: %6hd  B: %s", 
			x, y, bp ? L"TRUE" : L"FALSE");
		DRAWALL();
		return testme;
	case failed:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) bs[i].stat = failed;
		}
		DRAWALL();
		return failed;
	case passed:
		DRAWALL();
		return passed;
	default:
		_asm int 3;
		return failed;
	}
}

BOXSTATUS ButtonTest(BOXSTATUS newstatus)
{
	static BOXSTRUCT bs[] = {
		{410,210,420,212,notyet},
		{410,212,420,216,notyet},
		{410,216,420,220,notyet},
		{410,220,420,224,notyet},
		{410,224,420,228,notyet},
		{424,210,428,228,notyet},

		{450,210,460,212,notyet},
		{450,212,460,216,notyet},
		{450,216,460,220,notyet},
		{450,220,460,224,notyet},
		{450,224,460,228,notyet},
		{464,210,468,228,notyet},

		{490,210,500,212,notyet},
		{490,212,500,216,notyet},
		{490,216,500,220,notyet},
		{490,220,500,224,notyet},
		{490,224,500,228,notyet},
		{504,210,508,228,notyet},

		{410,260,420,262,notyet},
		{410,262,420,266,notyet},
		{410,266,420,270,notyet},
		{410,270,420,274,notyet},
		{410,274,420,278,notyet},
		{424,260,428,278,notyet},

		{450,260,460,262,notyet},
		{450,262,460,266,notyet},
		{450,266,460,270,notyet},
		{450,270,460,274,notyet},
		{450,274,460,278,notyet},
		{464,260,468,278,notyet},

		{490,260,500,262,notyet},
		{490,262,500,266,notyet},
		{490,266,500,270,notyet},
		{490,270,500,274,notyet},
		{490,274,500,278,notyet},
		{504,260,508,278,notyet}
	};
	static const int numtocheck = sizeof(bs) / sizeof(bs[0]);

	static int c = 0;
//	if(c >= 6) c = 0;

	int i;
	BOOL doh = FALSE;
	BOOL doh2 = FALSE;

	static BYTE otherdif[6] = {0,0,0,0,0,0};
	static BYTE roller = 0;

	BYTE data[] = {
		gData->bAnalogButtons[GPX],
		gData->bAnalogButtons[GPY],
		gData->bAnalogButtons[GPW],
		gData->bAnalogButtons[GPA],
		gData->bAnalogButtons[GPB],
		gData->bAnalogButtons[GPK]
	};

	switch(newstatus)
	{
	case notyet:
		for(i = 0; i < numtocheck; i++) {
			bs[i].stat = notyet;
		}

		roller = 0;
		for(i = 1; i < 6; i++) {
			otherdif[i] = 0x00;
		}
		otherdif[0] = 0xff;
		c = 0;

		DRAWALL();
		return notyet;
	case testme:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) {
				doh = TRUE;
			}
		}
		if(c < 6) {
			for(i = 0; i < 6; i++) {
				if(bs[c * 6 + i].stat != passed) {
					bs[c * 6 + i].stat = testme;
				}
			}
		}
		if(!doh) return passed;

#ifdef __HAMMERHEAD__
		if(data[c] == 0) 
			bs[c * 6 + 4].stat = passed;
		bs[c * 6 + 3].stat = passed;
		bs[c * 6 + 2].stat = passed;
		bs[c * 6 + 1].stat = passed;
		if (data[c] == 0xff) 
			bs[c * 6 + 0].stat = passed;
#else
		if(data[c] == 0) 
			bs[c * 6 + 4].stat = passed;
		else if (data[c] < 0x55) 
			bs[c * 6 + 3].stat = passed;
		else if (data[c] < 0xaa) 
			bs[c * 6 + 2].stat = passed;
		else if (data[c] < 0xff) 
			bs[c * 6 + 1].stat = passed;
		if (data[c] == 0xff) 
			bs[c * 6 + 0].stat = passed;
#endif
		

		doh2 = FALSE;
		for(i = 0; i < 6; i++) {
			otherdif[i] |= data[c] ^ data[i];
			if(otherdif[i] != 0xff) {
				doh2 = TRUE;
			}
		}

		roller |= rolb(data[c], 1) ^ data[c];
		if(!doh2 
			#ifndef __HAMMERHEAD__
				&& roller == 0xff
			#endif
			) {		
			bs[c * 6 + 5].stat = pending;
		}

		for(i = 0; i < 6; i++) {
			if(bs[i * 6 + 5].stat == pending) {
				if(bs[i * 6 + 4].stat == passed && data[i] == 0)
					bs[i * 6 + 5].stat = passed;
				else
					bs[i * 6 + 5].stat = pending;
			}
		}

		for(i = 0; i < 5; i++) {
			if(bs[c * 6 + i].stat != passed) goto doh2;
		}
		if(bs[c * 6 + 5].stat != passed && bs[c * 6 + 5].stat != pending) {
			goto doh2;
		}

		c++;
		for(i = 0; i < 6; i++) {
			otherdif[i] = 0;		
		}
		roller = 0;
		otherdif[c] = 0xff;
doh2:
		DRAWALL();
		drPrintf(0,0,L"X:%02x  Y:%02x  K:%02x  W:%02x  A:%02x  B:%02x  c:%d  ro:%02x  od: %02x %02x %02x %02x %02x %02x",
			data[2], data[3], data[4], data[5], data[0], data[1], c, roller, 
			otherdif[0], otherdif[1], otherdif[2], otherdif[3], otherdif[4], otherdif[5]);
		return testme;
	case failed:
		for(i = 0; i < numtocheck; i++) {
			if(bs[i].stat != passed) bs[i].stat = failed;
		}
		DRAWALL();
		return failed;
	case passed:
		DRAWALL();
		return passed;
	default:
		_asm int 3;
		return failed;
	}
}