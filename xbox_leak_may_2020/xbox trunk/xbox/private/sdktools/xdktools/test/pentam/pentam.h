#include "buildnt.h"
#include <xbox.h>
#include <xtl.h>
#include "draw.h"

#ifndef __pentam_h_
#define __pentam_h_

int const SCREEN_WIDTH  = 640;
int const SCREEN_HEIGHT = 480;
int const BOARD_WIDTH   =   6;
int const BOARD_HEIGHT  =  12;
int const SQUARE_SIZE   =  32;

int const PLAYER1_BOARD_X =  64;
int const PLAYER1_BOARD_Y =  50;

int const PLAYER2_BOARD_X = 384;
int const PLAYER2_BOARD_Y =  50;

int const INIT_ROW_COUNT =  5;

int const SCORE_PER_SQUARE  =  5;
int const SCORE_BONUS_QUAD  =  5;
int const SCORE_BONUS_PENTA = 15;

enum
{
	STATE_INIT,
	STATE_DEMO,
	STATE_PRE_PLAY,
	STATE_PLAY,
	STATE_PAUSE,
	STATE_POST_PLAY,
	STATE_QUIT,
	STATE_COUNT
};

enum
{
	MOVE_RELATIVE,
	MOVE_ABSOLUTE,
	MOVE_CENTER
};

enum {
	COLOR_EMPTY,  // bogus value. [square not occupied]
	COLOR_RED,
	COLOR_YELLOW,
	COLOR_CYAN,
	COLOR_GREEN,
	COLOR_MAGENTA,
	COLOR_CLEARED_SQUARE,
	COLOR_COUNT
};

DWORD g_dwSquareColorLT[COLOR_COUNT] = 
{
	0xff000000,  // bogus value. [square not occupied]
	0xffff0000,
	0xffffff00,
	0xff00ffff,
	0xff00ff00,
	0xffff00ff,
	0xffffffff
};

DWORD g_dwSquareColorDK[COLOR_COUNT] = 
{
	0xff000000,  // bogus value. [square not occupied]
	0xff800000,
	0xff808000,
	0xff008080,
	0xff008000,
	0xff800080,
	0xffffffff
};

class CGameBoard 
{
	unsigned char m_board[BOARD_WIDTH][BOARD_HEIGHT];
	long  m_dwCursorX;
	long  m_dwCursorY;
	DWORD m_dwScore;

	public:
		void  RenderBoard(CDraw *draw, int left, int top, BOOL bPaused);
		void  ClearBoard();
		BOOL  AddRow();
		BOOL  CheckBoard();
		void  MoveCursor(int dx, int dy, int mode);
		void  SwapSelected();
		DWORD GetScore();
};

int         g_dwGameState;
CDraw      *g_pDraw;
DWORD       g_dwCurrentTick;
HANDLE      g_hGamepad1;
HANDLE      g_hGamepad2;

CGameBoard *g_pPlayer1;
CGameBoard *g_pPlayer2;

BOOL g_bPlayer1Active;
BOOL g_bPlayer2Active;

XINPUT_STATE g_Gamepad1State;
XINPUT_STATE g_Gamepad2State;

DWORD m_dwTickLastAddRow1;
DWORD m_dwTickLastAddRow2;

DWORD g_dwTickLastMove1;
DWORD g_dwTickLastMove2;

BOOL g_bStartPressedLastFrame1;
BYTE g_cButtonsPressedLastFrame1[8];

BOOL g_bStartPressedLastFrame2;
BYTE g_cButtonsPressedLastFrame2[8];

DWORD g_dwCountDown;
DWORD g_dwCountDownLastTick;

int const XINPUT_GAMEPAD_DPAD_ALL = 
	XINPUT_GAMEPAD_DPAD_LEFT  | 
	XINPUT_GAMEPAD_DPAD_RIGHT | 
	XINPUT_GAMEPAD_DPAD_UP    | 
	XINPUT_GAMEPAD_DPAD_DOWN;

int const MOVE_DELAY      =  200;
int const CLEAR_DELAY     =   50;
int const ADDROW_DELAY    = 5000;
int const COUNTDOWN_DELAY = 1000;

#endif
