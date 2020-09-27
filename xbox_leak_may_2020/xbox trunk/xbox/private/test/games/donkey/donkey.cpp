#include <xtl.h>

#include <DSoundTest.h>
#include <device.h>
#include <ccl.h>
#include <draw.h>
#include <time.h>

#include "donkeyconstants.h"
#include "sprite.h"
#include "bitmaps.h"

static ULONG g_ulDonkeyScore = 0;
static ULONG g_ulDriverScore = 0;
static ULONG g_ulTurnCount = 0;

static CSprite g_donkey( DONKEY_START_Y, DONKEY_WIDTH, DONKEY_HEIGHT, g_pDonkey, true );
static CSprite g_driver( DRIVER_START_Y, DRIVER_WIDTH, DRIVER_HEIGHT, g_pCar, false ); 

static CDSoundTest g_sound;
static CDSoundTest g_background;

void SeedRandomNumberGenerator( void )
{
	ULONG ulSeed = (ULONG) time( NULL );
	srand( ulSeed );
}

void InitSound( void )
{
	g_sound.CreateDSound();

	g_background.OpenAndPlay( BACKGROUND_SOUND );
}

void InitGame( void )
{
	SeedRandomNumberGenerator();

    gpInit( 0 );
	drInit( SCREEN_WIDTH, SCREEN_HEIGHT );

	InitSound();
}

void CloseGame( void )
{
    gpDeinit();
	drDeinit();
}

BOOL IsButtonPressed( void )
{
	static ULONG ulCount = 0;

	ulCount++;

	if ( ulCount > BUTTON_WAIT_STATE )
	{
		ulCount = 1;
	}

	if ( 1 != ulCount )
		return FALSE;

    gpUpdateDeviceStateInfo(); 
    gpPoll();

	return cclCheckButtons( -1, bANYBUTTON );
}

void DrawBackGround( void )
{
	static float fStartY = ROAD_START_Y;

	drBox( LEFT_BACK_LEFT_X, LEFT_BACK_LEFT_Y, LEFT_BACK_RIGHT_X, LEFT_BACK_RIGHT_Y, WHITE );
	drBox( RIGHT_BACK_LEFT_X, RIGHT_BACK_LEFT_Y, RIGHT_BACK_RIGHT_X, RIGHT_BACK_RIGHT_Y, WHITE );

	drSetSize( FONT_WIDTH, FONT_HEIGHT, RED, BLACK );

	drPrintf( DONKEY_STRING_X, DONKEY_STRING_Y, DONKEY_TEXT, g_ulDonkeyScore );
	drPrintf( DRIVER_STRING_X, DRIVER_STRING_Y, DRIVER_TEXT, g_ulDriverScore );

	for ( float fy = fStartY; fy < SCREEN_HEIGHT; fy += ROAD_INCREMENT )
	{
		drLine( ROAD_X_POS, fy, ROAD_X_POS, fy + ROAD_LENGTH, WHITE );
	}

	fStartY += ROAD_ANIMATION_VAL;

	if ( ROAD_INCREMENT == fStartY )
	{
		fStartY = ROAD_START_Y;
	}

}

void DrawGame( void )
{
	drCls();

	DrawBackGround();

	g_donkey.Draw();
	g_driver.Draw();
	
	drShowScreen();
}

void ActOnInput( void )
{
	if ( TRUE == IsButtonPressed() )
	{
		g_sound.OpenAndPlay( CARMOVE_SOUND );
		g_driver.Move( MOVE_SIDEWAYS );
	}
}

void CalculateTurn( void )
{
	if ( NUM_DRIVER_TURNS < g_ulTurnCount )
	{
		g_donkey.Reset();
		g_driver.Reset();
		g_ulTurnCount = 0;
		g_ulDriverScore++;
	}
}

void CalculateStatus( void )
{
	if ( g_donkey.fy + g_donkey.ulHeight > g_driver.fy + g_driver.ulHeight )
	{

		g_donkey.Reset();
		g_driver.Move( MOVE_UP );
		g_ulTurnCount++;

	} else if ( g_donkey.fy + g_donkey.ulHeight >= g_driver.fy &&
			    g_donkey.fy + g_donkey.ulHeight <= g_driver.fy + g_driver.ulHeight &&
				g_donkey.fx == g_driver.fx )
	{
		g_sound.OpenAndPlay( DONKEYHIT_SOUND );

		g_donkey.Reset();
		g_driver.Reset();
		g_ulDonkeyScore++;
		g_ulTurnCount = 0;

	} else {

		g_donkey.Move( MOVE_DOWN );

	}
}

void CalculateSleepAmount( void )
{
	if ( g_ulDriverScore <= SLEEP_FACTOR * SLEEP_AMOUNT )
	{
		Sleep( SLEEP_AMOUNT - SLEEP_FACTOR * g_ulDriverScore );
	}
}

void __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow ) 
{

	InitGame();

	while ( true )
	{
		CalculateSleepAmount();
		DrawGame();
		ActOnInput();
		CalculateStatus();
		CalculateTurn();
	}

	CloseGame();
}