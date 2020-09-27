// cbounce.cpp

#include "draw.h"
#include "bounce.h"

#define BallSpeed (.12f)
#define SCRWIDTH 640
#define SCRHEIGHT 480

typedef struct BALL
{
	float x;
	float y;
	float height;
	float width;
	DWORD color;
	float dx;
	float dy;
} BALL;

static BALL b = {SCRWIDTH / 2, SCRHEIGHT / 8, 5, 5, 0xffffff00, -BallSpeed, BallSpeed};

float fabs(float a) { return a < 0 ? -a : a; }
float sign(float a) { return (a == 0) ? (float)0 : (a < 0) ? (float)-1 : (float)1; }

CDraw *g_pDraw;

CBounce::CBounce()
{
    g_pDraw = new CDraw(SCRWIDTH, SCRHEIGHT);
    b.x = SCRWIDTH * .75; b.y = SCRHEIGHT * .9; b.dx = -BallSpeed; b.dy = BallSpeed;

    // clear the screen
	g_pDraw->FillRect(0, 0, SCRWIDTH, SCRHEIGHT, 0xFF000000);
    g_pDraw->Present();
}

void CBounce::DrawStuff()
{
	//g_pDraw->DrawText(__FILE__ " (compiled " __DATE__ " " __TIME__ ")", 0, 0, 0xFFFFFF);

    // erase the ball
    g_pDraw->FillRect((int) b.x, (int) b.y, (int) b.width, (int) b.height, 0xFF000000);

	if (b.y <= 0)
	{
		b.dy = fabs(b.dy);	// make ball bounce off the top of the screen
	}
	else if (b.y >= SCRHEIGHT - b.height)
	{
		b.dy = -fabs(b.dy);	// bounce off the bottom
	}

	if (b.x < 0)			// if ball is off the left edge
	{
		b.dx = fabs(b.dx);	//change x direction
	}
	else if (b.x >= SCRWIDTH - b.width - 1)	// if the ball is off the right edge
	{
		b.dx = -fabs(b.dx);		//change x direction
	}

	// move the ball
	b.x += b.dx;	
	b.y += b.dy;

    // draw the ball
    g_pDraw->FillRect((int) b.x, (int) b.y, (int) b.width, (int) b.height, b.color);

    g_pDraw->Present();
}
