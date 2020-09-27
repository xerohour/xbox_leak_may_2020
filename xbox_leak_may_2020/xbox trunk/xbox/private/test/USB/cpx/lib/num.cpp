#include <xtl.h>
#include <stdio.h>
#include "draw.h"
#include "num.h"
#include <math.h>

#define PI (3.1415926535897932384626)
#define NUMREPEATSCALE (2)

static double gRipple;


typedef struct NUM {
	bool arf[7];
} NUM;

NUM gNumbers[]={{1,1,1,0,1,1,1},	//0
				{0,0,1,0,0,1,0},	//1
				{1,0,1,1,1,0,1},	//2
				{1,0,1,1,0,1,1},	//3
				{0,1,1,1,0,1,0},	//4
				{1,1,0,1,0,1,1},	//5
				{1,1,0,1,1,1,1},	//6
				{1,0,1,0,0,1,0},	//7
				{1,1,1,1,1,1,1},	//8
				{1,1,1,1,0,1,0}};	//9

void numDrawNum(float x, float y, float sizex, float sizey, int num, DWORD col)
{
	if(gNumbers[num].arf[0]) {
		drLine(x+1,		y,			x+sizex-1,	y,				col);
		drLine(x+2,		y+1,		x+sizex-2,	y+1,			col);
	}
	if(gNumbers[num].arf[1]) {
		drLine(x,		y+2,		x,			y+sizey/2-1,	col);
		drLine(x+1,		y+3,		x+1,		y+sizey/2-2,	col);
	}
	if(gNumbers[num].arf[2]) {
		drLine(x+sizex,	y+2,		x+sizex,	y+sizey/2-1,	col);
		drLine(x+sizex-1,y+3,		x+sizex-1,	y+sizey/2-2,	col);
	}
	if(gNumbers[num].arf[3]) {
		drLine(x+2,		y+sizey/2,	x+sizex-2,	y+sizey/2,		col);
		drLine(x+3,		y+1+sizey/2,x+sizex-3,	y+1+sizey/2,	col);
	}
	if(gNumbers[num].arf[4]) {
		drLine(x,		y+1+sizey/2,x,			y+sizey-2,		col);
		drLine(x+1,		y+2+sizey/2,x+1,		y+sizey-3,		col);
	}
	if(gNumbers[num].arf[5]) {
		drLine(x+sizex,	y+1+sizey/2,x+sizex,	y+sizey-2,		col);
		drLine(x+sizex-1,y+2+sizey/2,x+sizex-1,	y+sizey-3,		col);
	}
	if(gNumbers[num].arf[6]) {
		drLine(x+2,		y+sizey-1,	x+sizex-2,	y+sizey-1,		col);
		drLine(x+1,		y+sizey,	x+sizex-1,	y+sizey,		col);
	}
}

void numRipple()
{
	gRipple += .01 * NUMREPEATSCALE;
	if(gRipple > 2*PI*NUMREPEATSCALE) gRipple -= 2*PI*NUMREPEATSCALE;
}

void numDrawNumbers(int num, float x, float y, float sizex, float sizey, DWORD col, DWORD flags)
{
	char buf[32];
	float startx, dx, starty, dy;
	float spacer = 3;
	int i;
	float tempx, tempy;
	float yoffset;

	sprintf(buf, "%d", num);

	switch(flags & 3)
	{
	case numALIGN_LEFT:
		startx = x;
		dx = sizex + spacer;
		starty = y;
		dy = 0;
		break;
	case numALIGN_RIGHT:
		dx = sizex + spacer;
		startx = x + spacer - (dx * strlen(buf));
		starty = y;
		dy = 0;
		break;
	case numALIGN_CENTER:
		dx = sizex + spacer;
		startx = x + (spacer - (dx * strlen(buf))) / 2;
		starty = y;
		dy = 0;
		break;
	case numDRAW_VERCIAL:
		startx = x;
		dx = 0;
		starty = y;
		dy = sizey + spacer;
		break;
	}

	yoffset = 0;
	for(i = 0, tempx = startx, tempy = starty; buf[i]; i++, tempx += dx, tempy += dy) {
		if(flags & numRIPPLE) {
			yoffset = sizey * .5f * (float)cos((gRipple + i) / NUMREPEATSCALE);
		}
		numDrawNum(tempx, tempy + yoffset, sizex, sizey, buf[i] - '0', col);
	}
}