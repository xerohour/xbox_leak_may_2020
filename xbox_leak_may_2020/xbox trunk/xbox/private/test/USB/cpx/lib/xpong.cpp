#include <xtl.h>
#include "device.h"
#include "ccl.h"
#include "draw.h"
#include "modes.h"
#include "xpong.h"
#include "xsound.h"
#include "num.h"
#include "tsc.h"
//#include "ui.h"

//#define _DontUseSound_

#ifdef _DontUseSound_
#define XPlaySound(a) (true)
#endif

extern "C" ULONG DebugPrint(PCHAR Format, ...);

#define PaddleSpeed (.175f)
#define BallSpeed (.12f)
#define Width 640
#define Height 480

typedef struct PADDLE {
	float x;
	float y;
	float height;
	float width;
	DWORD color;
} PADDLE;

typedef struct BALL{
	float x;
	float y;
	float height;
	float width;
	DWORD color;
	float dx;
	float dy;
} BALL;


static BUTTONS AnyKey;
static CL pongcommands;	//command list that includes all commands used in xpong mode

static score[2] = {0,0};
//the paddles:
static PADDLE p[2] =   {{5, Height / 2 - 50, 100, 3, 0xffffffff},	
						{Width - 5, Height / 2, 100, 3, 0xffffffff}};

static float pdy[2];	//amount of paddle movement

static BALL ball = {Width / 2, Height / 8, 5, 5, 0xffffff00, -BallSpeed, BallSpeed};

static int gSoundError = 0;

float fabs(float a) { return a < 0 ? -a : a; }
float sign(float a) { return (a == 0) ? (float)0 : (a < 0) ? (float)-1 : (float)1; }

enum PONG_SOUND { PONG_BALL_PADDLE_HIT=0, PONG_BALL_WALL_HIT, PONG_PLAYER1_WIN, PONG_PLAYER2_WIN };

IXsound * gixs = NULL;

bool InitSound()
{
	CHAR* files[] = {"t:\\media\\ding.wav","t:\\media\\chord.wav","t:\\media\\tada.wav","t:\\media\\chimes.wav"};
	return IXsound::InitSound(files, &gixs);
}

bool KillSound( void )
{
	if(gixs)
		delete(gixs);
	return true;
}

bool XPlaySound( PONG_SOUND pong_sound )
{
	if(gixs)
		return gixs->XPlaySound((UINT)pong_sound);
	else
		return false;
}

/*************************************************************************************
Method:     MoveUp
Purpose:	this is called when the left thumb stick moves up
Params:		context --- if 1, then right thumbstick represents the other control pad
			repeat --- also irrelevant for this game
			slot --- the player who is moving
Return:     none
*************************************************************************************/
VOID MoveUp(DWORD context, BOOL repeat, BYTE slot)
{
	if(slot > 1) return;
	gpGAMEPAD*pgp;

	pgp = gpGetState(slot);

	if(!gpDeviceExists(0) || !gpDeviceExists(1)) { 
		slot = (BYTE)context;		
	}
	pdy[slot] = -(float)(context ? pgp->sThumbRY : pgp->sThumbLY) / (float)32768;

	p[slot].y = max( p[slot].y + PaddleSpeed * pdy[slot], 0);
}

/*************************************************************************************
Method:     MoveDown
Purpose:	this is called when left thumb stick moves down
Params:		context --- irrelevant
			repeat --- also irrelevant for this game
			slot --- the player who is moving
Return:     none
*************************************************************************************/
VOID MoveDown(DWORD context, BOOL repeat, BYTE slot)
{
	if(slot > 1) return;
	gpGAMEPAD*pgp;

	pgp = gpGetState(slot);

	if(!gpDeviceExists(0) || !gpDeviceExists(1)) {
		slot = (BYTE)context;
	}
	pdy[slot] = -(float)(context ? pgp->sThumbRY : pgp->sThumbLY) / (float)32768;

	p[slot].y = min( p[slot].y + PaddleSpeed * pdy[slot], Height - p[slot].height);
}

/*************************************************************************************
Method:     DrawStuff
Purpose:	draws the paddles, the ball, the score, etc
Params:		none
Return:		none
*************************************************************************************/
VOID DrawStuff(int status)
{
	int i;


	//draw the score
//	drPrintf(Width / 2 - 30, 60, L"Score  %d : %d", score[0], score[1]);
	numDrawNumbers(score[0], 310, 10, 8, 16, 0xff00ff00, numALIGN_RIGHT | (status==1?numRIPPLE:0));
	numDrawNumbers(score[1], 330, 10, 8, 16, 0xff00ff00, numALIGN_LEFT  | (status==2?numRIPPLE:0));

	//draw the paddles
	for(i = 0; i <= 1; i++)	{
		drBox(p[i].x, p[i].y, p[i].x + p[i].width, p[i].y + p[i].height, p[i].color);
	}

	//draw the ball
	drBox(ball.x, ball.y, ball.x + ball.width, ball.y + ball.height, ball.color);
}

/*************************************************************************************
Method:     AnyKeyPressed
Purpose:	return TRUE if any button is pressed on the control pad, false otherwise
*************************************************************************************/
BOOL AnyKeyPressed()
{
	int i;
	for(i = 0; i < gpMaxSlots(); i++)
	{
		if(cclCheckButtons(i, bANYBUTTON)) {
			return TRUE;
		}
	}
	return FALSE;
}

/*************************************************************************************
Method:     win
Purpose:	wave the scores up and down when someone wins
Params:		p: the player that won (0 for player1, 1 for player2)
Return:		none
*************************************************************************************/
void win(int p)
{
	INT64 timer;

	score[p]++;
	if(!XPlaySound((p==0) ? PONG_PLAYER1_WIN : PONG_PLAYER2_WIN)) {
		gSoundError++;
		if(gSoundError <= 8)
			DebugPrint("XPlaySound(PONG_PLAYER%d_WIN) failed!\n", p==1?1:2);
		if(gSoundError == 8)
			DebugPrint("XPlaySound -- halting further error messages\n");
	}

	timer = TscBegin();

	while((TscTicksToFMSeconds(TscCurrentTime(timer)) < 2500) && !AnyKeyPressed()) 
	{
		drCls();
		drPrintf(100, 80, L"Player %d wins!", p + 1);
		gpUpdateDeviceStateInfo();				//update the button state info...
		gpPoll();
		numRipple();
		DrawStuff(p+1);
		drShowScreen();
	}
	
	XPong(STATUS_LEAVE, 1);
	XPong(STATUS_ENTER, 1);
}

/*************************************************************************************
Function:   XPong
Purpose:	update the state of xpong on the screen (move paddles, ball, etc)
Params:		context --- initilize (stage 1 or 2), play, or quit (Stage 1 or 2)
			param --- unused
Return:     none
*************************************************************************************/
VOID XPong(DWORD context, /* unused */ INT param)
{
	
	switch(context)
	{
	case STATUS_NORMAL:
		pdy[0] = pdy[1] = 0;
		pongcommands.CheckControls();

		if(ball.y <= 0) {
			ball.dy = fabs(ball.dy);		//make ball bounce off the top of the screen
			if (!XPlaySound(PONG_BALL_WALL_HIT)) {
				gSoundError++;
				if(gSoundError <= 8)
					DebugPrint("XPlaySound(PONG_BALL_WALL_HIT) failed!\n");
				if(gSoundError == 8)
					DebugPrint("XPlaySound -- halting further error messages\n");
			} 
		}
		if(ball.y >= Height - ball.height) {
			ball.dy = -fabs(ball.dy);	//bounce off teh bottom
			if (!XPlaySound(PONG_BALL_WALL_HIT)) {
				gSoundError++;
				if(gSoundError <= 8)
					DebugPrint("XPlaySound(PONG_BALL_WALL_HIT) failed!\n");
				if(gSoundError == 8)
					DebugPrint("XPlaySound -- halting further error messages\n");
			}
		}

		if(ball.x < 0)						//if ball is off the left edge
		{
			win(1);
		}

		if(ball.x >= Width - ball.width - 1)	//iff the ball is off the right edge
		{
			win(0);
		}

		//move the ball
		ball.x += ball.dx;	
		ball.y += ball.dy;

		//bounce off player 1's paddle
		if(ball.x <= p[0].x + p[0].width && ball.x + ball.width >= p[0].x
			&& ball.y <= p[0].y + p[0].height && ball.y + ball.height >= p[0].y)
		{
			ball.dx = fabs(ball.dx);			//change x direction
			ball.dx += BallSpeed / 20;		//speed up the ball a little
			if(ball.dx > p[0].width) ball.dx = p[0].width;	//but not too fast...
			
			//add some "spin"
			ball.dy += BallSpeed * pdy[0] / 4;
			if (!XPlaySound(PONG_BALL_PADDLE_HIT)) {
				gSoundError++;
				if(gSoundError <= 8)
					DebugPrint("XPlaySound(PONG_BALL_PADDLE_HIT) failed!\n");
				if(gSoundError == 8)
					DebugPrint("XPlaySound -- halting further error messages\n");
			}
		}

		//bounce off player 2's paddle
		if(ball.x <= p[1].x + p[1].width && ball.x + ball.width >= p[1].x
			&& ball.y <= p[1].y + p[1].height && ball.y + ball.height >= p[1].y) 
		{
			ball.dx = -fabs(ball.dx);			             //change x direction
			ball.dx -= BallSpeed / 20;		                 //speed up the ball
			if(-ball.dx > p[1].width) ball.dx = -p[1].width; //but not too fast (you have to be able to hit it!

			//add spin
			ball.dy += BallSpeed * pdy[1] / 4;
			if (!XPlaySound(PONG_BALL_PADDLE_HIT)) {
				gSoundError++;
				if(gSoundError <= 8)
					DebugPrint("XPlaySound(PONG_BALL_PADDLE_HIT) failed!\n");
				if(gSoundError == 8)
					DebugPrint("XPlaySound -- halting further error messages\n");
			}
		}

		DrawStuff(0);	//display everything (don't copy the buffer to the screen yet though)
		break;

	case STATUS_BOOT:
		//initialization stuff...
		{
			BUTTONS ball;
			ball.all = bLTS_UP_PRESSED;
			pongcommands.RegisterControl(ball, MoveUp, 0);		//left thumb stick up = move up
			ball.all = bRTS_UP_PRESSED;
			pongcommands.RegisterControl(ball, MoveUp, 1);		//right thumb stick up = move up
			ball.all = bLTS_DN_PRESSED;
			pongcommands.RegisterControl(ball, MoveDown, 0);	//...
			ball.all = bRTS_DN_PRESSED;
			pongcommands.RegisterControl(ball, MoveDown, 1);
		}

		if(!InitSound()) {
			DebugPrint("InitSound failed!\n");
		}

		TscInit(0);

		break;

	case STATUS_ENTER:
		//initialize player locations, ball locations, etc
		p[0].y = p[1].y = Height / 2 - 50;
		p[0].x = 0;
		p[1].x = Width - 8;
		ball.x = Width * .75; ball.y = Height * .9; ball.dx = -BallSpeed; ball.dy = BallSpeed;
		break;

	case STATUS_LEAVE:
		break;

	case STATUS_QUIT:
		if(!KillSound()) {
			DebugPrint("KillSound failed!\n");
		}
		break;
	}
}



