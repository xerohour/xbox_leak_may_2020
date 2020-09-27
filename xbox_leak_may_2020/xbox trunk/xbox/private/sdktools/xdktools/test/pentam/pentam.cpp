#include "pentam.h"

// ---------------------------------------------------------------------------
// RenderBoard()
// Renders the board at the specified location.
// ---------------------------------------------------------------------------
void  CGameBoard::RenderBoard(CDraw *draw, int left, int top, BOOL bPaused)
{
	// clear the board background
	draw->FillRect(left, top, BOARD_WIDTH * SQUARE_SIZE + 1, BOARD_HEIGHT * SQUARE_SIZE + 1, 0xFF202020);
	
	if(bPaused)
	{
		for(int x=0;x<BOARD_WIDTH;x++)
		{
			for(int y=0;y<BOARD_HEIGHT;y++)
			{
				draw->FillRect(
					left + x * SQUARE_SIZE + 2, 
					top  + y * SQUARE_SIZE + 2, 
					SQUARE_SIZE - 2, 
					SQUARE_SIZE - 2, 
					g_dwSquareColorDK[COLOR_YELLOW]);
				
				draw->FillRect(
					left + x * SQUARE_SIZE + 1, 
					top  + y * SQUARE_SIZE + 1, 
					SQUARE_SIZE - 1, 
					1, 
					g_dwSquareColorLT[COLOR_YELLOW]);
				
				draw->FillRect(
					left + x * SQUARE_SIZE + 1, 
					top  + y * SQUARE_SIZE + 1, 
					1, 
					SQUARE_SIZE - 1, 
					g_dwSquareColorLT[COLOR_YELLOW]);
			}
		}
		
		return;
	}
	
	for(int x=0;x<BOARD_WIDTH;x++)
	{
		for(int y=0;y<BOARD_HEIGHT;y++)
		{
			unsigned char color = m_board[x][y];
			if(color)
			{
				draw->FillRect(
					left + x * SQUARE_SIZE + 2, 
					top  + y * SQUARE_SIZE + 2, 
					SQUARE_SIZE - 2, 
					SQUARE_SIZE - 2, 
					g_dwSquareColorDK[color]);
				
				draw->FillRect(
					left + x * SQUARE_SIZE + 1, 
					top  + y * SQUARE_SIZE + 1, 
					SQUARE_SIZE - 1, 
					1, 
					g_dwSquareColorLT[color]);
				
				draw->FillRect(
					left + x * SQUARE_SIZE + 1, 
					top  + y * SQUARE_SIZE + 1, 
					1, 
					SQUARE_SIZE - 1, 
					g_dwSquareColorLT[color]);
			}
		}
	}

	// Draw Cursor : top
	draw->FillRect(
		left + m_dwCursorX * SQUARE_SIZE, 
		top  + m_dwCursorY * SQUARE_SIZE, 
		SQUARE_SIZE << 1, 
		2, 
		0xffffffff);

	// Draw Cursor : bottom
	draw->FillRect(
		left + m_dwCursorX * SQUARE_SIZE, 
		top  + m_dwCursorY * SQUARE_SIZE + SQUARE_SIZE - 1,
		SQUARE_SIZE << 1, 
		2, 
		0xffffffff);

	// Draw Cursor : left
	draw->FillRect(
		left + m_dwCursorX * SQUARE_SIZE, 
		top  + m_dwCursorY * SQUARE_SIZE,
		2, 
		SQUARE_SIZE, 
		0xffffffff);

	// Draw Cursor : right
	draw->FillRect(
		left + m_dwCursorX * SQUARE_SIZE + (SQUARE_SIZE << 1) - 1,
		top  + m_dwCursorY * SQUARE_SIZE,
		2, 
		SQUARE_SIZE, 
		0xffffffff);
}

// ---------------------------------------------------------------------------
// ClearBoard()
// Clears the board data.
// ---------------------------------------------------------------------------
void  CGameBoard::ClearBoard()
{
	memset(m_board,COLOR_EMPTY,sizeof(m_board));
	m_dwScore = 0;
}

// ---------------------------------------------------------------------------
// AddRow()
// Adds a row to the bottom of the board. Pushes existing rows up. Returns
// TRUE if there was room, and FALSE if the board was full (end of game).
// ---------------------------------------------------------------------------
BOOL  CGameBoard::AddRow()
{
	int x;
	int y;

	for(x=0;x<BOARD_WIDTH;x++)
	{
		if(m_board[x][0])
		{
			return FALSE;
		}
	}

	for(x=0;x<BOARD_WIDTH;x++)
	{
		for(y=0;y<BOARD_HEIGHT;y++)
		{
			if( y < BOARD_HEIGHT - 1 )
			{
				m_board[x][y]=m_board[x][y+1];
			}
			else
			{
				m_board[x][y]=(unsigned char)
				((rand() * (COLOR_COUNT - 2)) / RAND_MAX) + 1;
			}
		}
	}

	MoveCursor(0,-1,MOVE_RELATIVE);

	return TRUE;
}

// ---------------------------------------------------------------------------
// CheckBoard()
// Scans each row and column to see if any squares can be cleared. If any 
// squares are cleared, the score is updated and the function returns TRUE.
// Otherwise, it returns FALSE. Returns after the first clear. Keep calling
// until the function returns FALSE to clear the whole board. I set it up 
// this way so that I could animate each cleared set. For now, they just 
// flash white, then vanish.
// ---------------------------------------------------------------------------
BOOL  CGameBoard::CheckBoard()
{
	int x;
	int y;

	// Delete cleared squares
	for(y=0;y<BOARD_HEIGHT;y++)
	{
		for(x=0;x<BOARD_WIDTH;x++)
		{
			if(m_board[x][y] == COLOR_CLEARED_SQUARE)
			{
				m_board[x][y] = COLOR_EMPTY;
			}
		}
	}

	// Enforce gravity
	for(x=0;x<BOARD_WIDTH;x++)
	{
		int i=BOARD_HEIGHT-1;
		for(y=i;y>=0;y--)
		{
			if(m_board[x][y] != COLOR_EMPTY)
			{
				if(i!=y)
				{
					m_board[x][i]=m_board[x][y];
				}
				i--;
			}
		}

		for(;i>=0;i--)
		{
			m_board[x][i]=COLOR_EMPTY;
		}
	}

	// Scan rows
	for(y=0;y<BOARD_HEIGHT;y++)
	{
		int nMatchCount = 0;
		int last_color  = COLOR_EMPTY;
		for(x=0;x<BOARD_WIDTH;x++)
		{
			int this_color = m_board[x][y];
			if(this_color)
			{
				if(this_color == last_color)
				{
					nMatchCount++;
					if(nMatchCount == 2)
					{
						m_board[x  ][y]=COLOR_CLEARED_SQUARE;
						m_board[x-1][y]=COLOR_CLEARED_SQUARE;
						m_board[x-2][y]=COLOR_CLEARED_SQUARE;
					}
					else if(nMatchCount > 2)
					{
						m_board[x][y]=COLOR_CLEARED_SQUARE;
					}
				}
				else
				{
					if(nMatchCount >= 2)
					{
						m_dwScore += nMatchCount * SCORE_PER_SQUARE;
						m_dwScore += (nMatchCount == 4)?SCORE_BONUS_QUAD :0;
						m_dwScore += (nMatchCount >= 5)?SCORE_BONUS_PENTA:0;
						return TRUE;
					}
					nMatchCount = 0;
					last_color  = COLOR_EMPTY;
				}
			}
			else
			{
				if(nMatchCount >= 2)
				{
					m_dwScore += nMatchCount * SCORE_PER_SQUARE;
					m_dwScore += (nMatchCount == 4)?SCORE_BONUS_QUAD :0;
					m_dwScore += (nMatchCount >= 5)?SCORE_BONUS_PENTA:0;
					return TRUE;
				}
				nMatchCount = 0;
				last_color  = COLOR_EMPTY;
			}
			last_color = this_color;
		}

		if(nMatchCount >= 2)
		{
			m_dwScore += nMatchCount * SCORE_PER_SQUARE;
			m_dwScore += (nMatchCount == 4)?SCORE_BONUS_QUAD :0;
			m_dwScore += (nMatchCount >= 5)?SCORE_BONUS_PENTA:0;
			return TRUE;
		}
	}
	
	// Scan columns
	for(x=0;x<BOARD_WIDTH;x++)
	{
		int nMatchCount = 0;
		int last_color  = COLOR_EMPTY;
		for(y=0;y<BOARD_HEIGHT;y++)
		{
			int this_color = m_board[x][y];
			if(this_color)
			{
				if(this_color == last_color)
				{
					nMatchCount++;
					if(nMatchCount == 2)
					{
						m_board[x][y  ]=COLOR_CLEARED_SQUARE;
						m_board[x][y-1]=COLOR_CLEARED_SQUARE;
						m_board[x][y-2]=COLOR_CLEARED_SQUARE;
					}
					else if(nMatchCount > 2)
					{
						m_board[x][y]=COLOR_CLEARED_SQUARE;
					}
				}
				else
				{
					if(nMatchCount >= 2)
					{
						m_dwScore += nMatchCount * SCORE_PER_SQUARE;
						m_dwScore += (nMatchCount == 4)?SCORE_BONUS_QUAD :0;
						m_dwScore += (nMatchCount >= 5)?SCORE_BONUS_PENTA:0;
						return TRUE;
					}
					nMatchCount = 0;
					last_color  = COLOR_EMPTY;
				}
			}
			else
			{
				if(nMatchCount >= 2)
				{
					m_dwScore += nMatchCount * SCORE_PER_SQUARE;
					m_dwScore += (nMatchCount == 4)?SCORE_BONUS_QUAD :0;
					m_dwScore += (nMatchCount >= 5)?SCORE_BONUS_PENTA:0;
					return TRUE;
				}
				nMatchCount = 0;
				last_color  = COLOR_EMPTY;
			}
			last_color = this_color;
		}

		if(nMatchCount >= 2)
		{
			m_dwScore += nMatchCount * SCORE_PER_SQUARE;
			m_dwScore += (nMatchCount == 4)?SCORE_BONUS_QUAD :0;
			m_dwScore += (nMatchCount >= 5)?SCORE_BONUS_PENTA:0;
			return TRUE;
		}
	}

	return FALSE;
}

// ---------------------------------------------------------------------------
// MoveCursor()
// Move the game cursor.
// ---------------------------------------------------------------------------
void  CGameBoard::MoveCursor(int dx, int dy, int mode)
{
	switch(mode)
	{
	case MOVE_RELATIVE:
		m_dwCursorX += dx;
		m_dwCursorY += dy;
		break;
	case MOVE_ABSOLUTE:
		m_dwCursorX = dx;
		m_dwCursorY = dy;
		break;
	case MOVE_CENTER:
	default:
		m_dwCursorX = BOARD_WIDTH  / 2 - 1;
		m_dwCursorY = BOARD_HEIGHT / 2 - 1;
		break;
	}
	
	if( m_dwCursorX < 0 )
	{
		m_dwCursorX = 0;
	}
	
	if( m_dwCursorX > BOARD_WIDTH - 2 )
	{
		m_dwCursorX = BOARD_WIDTH - 2;
	}

	if( m_dwCursorY < 0 )
	{
		m_dwCursorY = 0;
	}
	
	if( m_dwCursorY > BOARD_HEIGHT - 1 )
	{
		m_dwCursorY = BOARD_HEIGHT - 1;
	}
}

// ---------------------------------------------------------------------------
// SwapSelected()
// Swaps the two pieces that are under the cursor.
// ---------------------------------------------------------------------------
void  CGameBoard::SwapSelected()
{
	unsigned char temp = m_board[m_dwCursorX][m_dwCursorY];
	m_board[m_dwCursorX][m_dwCursorY] = m_board[m_dwCursorX + 1][m_dwCursorY];
	m_board[m_dwCursorX + 1][m_dwCursorY] = temp;
}

// ---------------------------------------------------------------------------
// GetScore()
// Returns the current score.
// ---------------------------------------------------------------------------
DWORD CGameBoard::GetScore()
{

	return m_dwScore;
}



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
// Init()
// Initialize application data.
// ---------------------------------------------------------------------------
void Init()
{
	g_dwCurrentTick = GetTickCount();
	g_dwTickLastMove1 = g_dwCurrentTick;
	g_dwTickLastMove2 = g_dwCurrentTick;
	srand(g_dwCurrentTick);

	g_dwCountDown = 0;

	g_bPlayer1Active = FALSE;
	g_bPlayer2Active = FALSE;

	g_bStartPressedLastFrame1 = FALSE;
	memset(g_cButtonsPressedLastFrame1,0,sizeof(g_cButtonsPressedLastFrame1));

	g_bStartPressedLastFrame2 = FALSE;
	memset(g_cButtonsPressedLastFrame2,0,sizeof(g_cButtonsPressedLastFrame2));

	g_pDraw = new CDraw(SCREEN_WIDTH, SCREEN_HEIGHT);
	g_pDraw->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFF000000);
	
	g_pPlayer1 = new CGameBoard();
	g_pPlayer2 = new CGameBoard();
	
	g_dwGameState = STATE_DEMO;
	
	g_hGamepad1 = XInputOpen(XDEVICE_TYPE_GAMEPAD,0,XDEVICE_NO_SLOT,NULL);
	g_hGamepad2 = XInputOpen(XDEVICE_TYPE_GAMEPAD,1,XDEVICE_NO_SLOT,NULL);
}

// ---------------------------------------------------------------------------
// ShutDown()
// Clean up before exit.
// ---------------------------------------------------------------------------
void ShutDown() // Will this ever happen?  8-)
{
	XInputClose(g_hGamepad1);
	XInputClose(g_hGamepad2);
}

// ---------------------------------------------------------------------------
// NewGame()
// Initialize game data.
// ---------------------------------------------------------------------------
void NewGame()
{
	g_pPlayer1->ClearBoard();
	g_pPlayer2->ClearBoard();
	for(int i=0;i<INIT_ROW_COUNT;i++)
	{
		g_pPlayer1->AddRow();
		g_pPlayer2->AddRow();
	}

	m_dwTickLastAddRow1 = GetTickCount();
	m_dwTickLastAddRow2 = GetTickCount();

	while(g_pPlayer1->CheckBoard());
	g_pPlayer1->MoveCursor(0,0,MOVE_CENTER);

	while(g_pPlayer2->CheckBoard());
	g_pPlayer2->MoveCursor(0,0,MOVE_CENTER);
}

// ---------------------------------------------------------------------------
// DemoMode()
// Render stuff to keep the player happy between games.
// ---------------------------------------------------------------------------
void DemoMode()
{
	g_pPlayer1->RenderBoard(g_pDraw,PLAYER1_BOARD_X,PLAYER1_BOARD_Y,TRUE);
	g_pPlayer2->RenderBoard(g_pDraw,PLAYER2_BOARD_X,PLAYER2_BOARD_Y,TRUE);

	g_dwCountDownLastTick = GetTickCount();
	g_dwCountDown = 10;

	if(g_hGamepad1)
	{
		WORD wButtons = g_Gamepad1State.Gamepad.wButtons;

		if((wButtons & XINPUT_GAMEPAD_START) && !g_bStartPressedLastFrame1)
		{
			g_dwGameState = STATE_PRE_PLAY;
			g_bStartPressedLastFrame1 = TRUE;
			g_bPlayer1Active = TRUE;
			g_bPlayer2Active = FALSE;
			NewGame();
			return;
		}
	}

	if(g_hGamepad2)
	{
		WORD wButtons = g_Gamepad2State.Gamepad.wButtons;

		if((wButtons & XINPUT_GAMEPAD_START) && !g_bStartPressedLastFrame2)
		{
			g_dwGameState = STATE_PRE_PLAY;
			g_bStartPressedLastFrame2 = TRUE;
			g_bPlayer1Active = FALSE;
			g_bPlayer2Active = TRUE;
			NewGame();
			return;
		}
	}
}

// ---------------------------------------------------------------------------
// PauseMode()
// Paused game.
// ---------------------------------------------------------------------------
void PauseMode()
{
	g_pPlayer1->RenderBoard(g_pDraw,PLAYER1_BOARD_X,PLAYER1_BOARD_Y,TRUE);
	g_pPlayer2->RenderBoard(g_pDraw,PLAYER2_BOARD_X,PLAYER2_BOARD_Y,TRUE);

	if(g_hGamepad1)
	{
		WORD wButtons = g_Gamepad1State.Gamepad.wButtons;

		if((wButtons & XINPUT_GAMEPAD_START) && !g_bStartPressedLastFrame1)
		{
			g_dwGameState = STATE_PLAY;
			g_bStartPressedLastFrame1 = TRUE;
			return;
		}
	}

	if(g_hGamepad2)
	{
		WORD wButtons = g_Gamepad2State.Gamepad.wButtons;

		if((wButtons & XINPUT_GAMEPAD_START) && !g_bStartPressedLastFrame2)
		{
			g_dwGameState = STATE_PLAY;
			g_bStartPressedLastFrame2 = TRUE;
			return;
		}
	}
}

// ---------------------------------------------------------------------------
// PrePlayMode()
// Will other player join?
// ---------------------------------------------------------------------------
void PrePlayMode()
{
	g_pPlayer1->RenderBoard(g_pDraw,PLAYER1_BOARD_X,PLAYER1_BOARD_Y,TRUE);
	g_pPlayer2->RenderBoard(g_pDraw,PLAYER2_BOARD_X,PLAYER2_BOARD_Y,TRUE);

	if(GetTickCount() - g_dwCountDownLastTick >= COUNTDOWN_DELAY)
	{
		g_dwCountDownLastTick = GetTickCount();

		if(--g_dwCountDown <= 0)
		{
			g_dwGameState = STATE_PLAY;
			return;
		}
	}

	WCHAR buf[80];
	wsprintf(buf,L"Waiting...(%d)",g_dwCountDown);
	
	if(g_bPlayer1Active)
	{
		g_pDraw->DrawText(L"Ready!",PLAYER1_BOARD_X,PLAYER1_BOARD_Y-20,0xff00ff00,0xff000000,0);
	}
	else
	{
		g_pDraw->DrawText(buf,PLAYER1_BOARD_X,PLAYER1_BOARD_Y-20,0xff00ff00,0xff000000,0);

		if(g_hGamepad1)
		{
			WORD wButtons = g_Gamepad1State.Gamepad.wButtons;

			if((wButtons & XINPUT_GAMEPAD_START) && !g_bStartPressedLastFrame1)
			{
				g_dwGameState = STATE_PLAY;
				g_bStartPressedLastFrame1 = TRUE;
				g_bPlayer1Active = TRUE;
				return;
			}
		}
	}


	if(g_bPlayer2Active)
	{
		g_pDraw->DrawText(L"Ready!",PLAYER2_BOARD_X,PLAYER2_BOARD_Y-20,0xff00ff00,0xff000000,0);
	}
	else
	{
		g_pDraw->DrawText(buf,PLAYER2_BOARD_X,PLAYER2_BOARD_Y-20,0xff00ff00,0xff000000,0);
		if(g_hGamepad2)
		{
			WORD wButtons = g_Gamepad2State.Gamepad.wButtons;

			if((wButtons & XINPUT_GAMEPAD_START) && !g_bStartPressedLastFrame2)
			{
				g_dwGameState = STATE_PLAY;
				g_bStartPressedLastFrame2 = TRUE;
				g_bPlayer2Active = TRUE;
				return;
			}
		}
	}

	if(g_bPlayer1Active && g_bPlayer2Active)
	{
		g_dwGameState = STATE_PLAY;
	}
}

// ---------------------------------------------------------------------------
// PlayMode()
// The game!
// ---------------------------------------------------------------------------
void PlayMode()
{
	while(g_pPlayer1->CheckBoard() || g_pPlayer2->CheckBoard())
	{
		g_pPlayer1->RenderBoard(g_pDraw,PLAYER1_BOARD_X,PLAYER1_BOARD_Y,g_bPlayer1Active?FALSE:TRUE);
		g_pPlayer2->RenderBoard(g_pDraw,PLAYER2_BOARD_X,PLAYER2_BOARD_Y,g_bPlayer2Active?FALSE:TRUE);
		g_pDraw->Present();
		DWORD current_tick = GetTickCount();
		while(GetTickCount() - current_tick < CLEAR_DELAY);
	}

	if(g_bPlayer1Active && (GetTickCount() - m_dwTickLastAddRow1 > ADDROW_DELAY))
	{
		if(!g_pPlayer1->AddRow())
		{
			g_dwGameState = STATE_POST_PLAY;
		}
		m_dwTickLastAddRow1 = GetTickCount();
	}

	if(g_bPlayer2Active && (GetTickCount() - m_dwTickLastAddRow2 > ADDROW_DELAY))
	{
		if(!g_pPlayer2->AddRow())
		{
			g_dwGameState = STATE_POST_PLAY;
		}
		m_dwTickLastAddRow2 = GetTickCount();
	}

	if(g_hGamepad1 && g_bPlayer1Active)
	{
		WORD wButtons = g_Gamepad1State.Gamepad.wButtons;

		if((g_Gamepad1State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]) && !g_cButtonsPressedLastFrame1[XINPUT_GAMEPAD_A])
		{
			g_pPlayer1->SwapSelected();
		}

		if((wButtons & XINPUT_GAMEPAD_START) && !g_bStartPressedLastFrame1)
		{
			g_dwGameState = STATE_PAUSE;
			g_bStartPressedLastFrame1 = TRUE;
			return;
		}

		if(g_dwCurrentTick - g_dwTickLastMove1 > MOVE_DELAY)
		{
			g_pPlayer1->MoveCursor(
				(wButtons & XINPUT_GAMEPAD_DPAD_LEFT)?-1:
					((wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)?1:0),
				(wButtons & XINPUT_GAMEPAD_DPAD_UP)?-1:
					((wButtons & XINPUT_GAMEPAD_DPAD_DOWN)?1:0),
				MOVE_RELATIVE);
			g_dwTickLastMove1 = g_dwCurrentTick;
		}
	}

	if(g_hGamepad2 && g_bPlayer2Active)
	{
		WORD wButtons = g_Gamepad2State.Gamepad.wButtons;

		if((g_Gamepad2State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]) && !g_cButtonsPressedLastFrame2[XINPUT_GAMEPAD_A])
		{
			g_pPlayer2->SwapSelected();
		}

		if((wButtons & XINPUT_GAMEPAD_START) && !g_bStartPressedLastFrame2)
		{
			g_dwGameState = STATE_PAUSE;
			g_bStartPressedLastFrame2 = TRUE;
			return;
		}

		if(g_dwCurrentTick - g_dwTickLastMove2 > MOVE_DELAY)
		{
			g_pPlayer2->MoveCursor(
				(wButtons & XINPUT_GAMEPAD_DPAD_LEFT)?-1:
					((wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)?1:0),
				(wButtons & XINPUT_GAMEPAD_DPAD_UP)?-1:
					((wButtons & XINPUT_GAMEPAD_DPAD_DOWN)?1:0),
				MOVE_RELATIVE);
			g_dwTickLastMove2 = g_dwCurrentTick;
		}
	}

	g_pPlayer1->RenderBoard(g_pDraw,PLAYER1_BOARD_X,PLAYER1_BOARD_Y,g_bPlayer1Active?FALSE:TRUE);
	g_pPlayer2->RenderBoard(g_pDraw,PLAYER2_BOARD_X,PLAYER2_BOARD_Y,g_bPlayer2Active?FALSE:TRUE);
}

// ---------------------------------------------------------------------------
// FrameUpdate()
// Called on every frame.
// ---------------------------------------------------------------------------
BOOL FrameUpdate()
{
	g_dwCurrentTick = GetTickCount();

	XInputGetState( g_hGamepad1,&g_Gamepad1State );
	XInputGetState( g_hGamepad2,&g_Gamepad2State );
	
	g_pDraw->FillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0xff000000);

	switch(g_dwGameState)
	{
	case STATE_DEMO:
		DemoMode();
		g_pDraw->DrawText(L"DemoMode",0,0,0xffff0000,0xff000000,0);
		break;
	case STATE_PRE_PLAY:
		PrePlayMode();
		g_pDraw->DrawText(L"PrePlay",0,0,0xffff0000,0xff000000,0);
		break;
	case STATE_PLAY:
		PlayMode();
		g_pDraw->DrawText(L"PlayMode",0,0,0xffff0000,0xff000000,0);
		break;
	case STATE_PAUSE:
		PauseMode();
		g_pDraw->DrawText(L"PauseMode",0,0,0xffff0000,0xff000000,0);
		break;
	case STATE_POST_PLAY:
		g_pDraw->DrawText(L"PostPlay",0,0,0xffff0000,0xff000000,0);
		g_dwGameState = STATE_DEMO;
		break;
	case STATE_INIT:
		// do nothing
		break;
	case STATE_QUIT:
	default:
		// ERROR! Should never happen.
		return FALSE;
	}


	WCHAR buf[30];
	
	wsprintf(buf,L"Score: %d",g_pPlayer1->GetScore());
	if(g_bPlayer1Active && g_dwGameState != STATE_PRE_PLAY)
	{
		g_pDraw->DrawText(buf,PLAYER1_BOARD_X,PLAYER1_BOARD_Y-20,0xff00ff00,0xff000000,0);
	}

	wsprintf(buf,L"Score: %d",g_pPlayer2->GetScore());
	if(g_bPlayer2Active && g_dwGameState != STATE_PRE_PLAY)
	{
		g_pDraw->DrawText(buf,PLAYER2_BOARD_X,PLAYER2_BOARD_Y-20,0xff00ff00,0xff000000,0);
	}
	
	g_pDraw->Present();

	// gamepad state housekeeping
	g_bStartPressedLastFrame1 = g_Gamepad1State.Gamepad.wButtons & XINPUT_GAMEPAD_START;
	memcpy(g_cButtonsPressedLastFrame1,g_Gamepad1State.Gamepad.bAnalogButtons,8);

	g_bStartPressedLastFrame2 = g_Gamepad2State.Gamepad.wButtons & XINPUT_GAMEPAD_START;
	memcpy(g_cButtonsPressedLastFrame2,g_Gamepad2State.Gamepad.bAnalogButtons,8);

	if(!(g_Gamepad1State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_ALL))
	{
		g_dwTickLastMove1 = 0; // delay is only for "auto repeat"
	}

	if(!(g_Gamepad2State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_ALL))
	{
		g_dwTickLastMove2 = 0; // delay is only for "auto repeat"
	}


	return TRUE;
}

// ---------------------------------------------------------------------------
// wWinMain()
// App's point of entry.
// ---------------------------------------------------------------------------
extern "C" int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hprev, LPSTR lpcmd, int nShow)
{
	Init();

	while (FrameUpdate());

	ShutDown();	// Will this ever happen?  8-)
	return 0;
}
