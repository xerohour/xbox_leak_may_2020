#include "sprite.h"

CSprite::CSprite( float fy, ULONG ulWidth, ULONG ulHeight, const BYTE* pData, bool bRandom )
{
	this->fStartY = fy;
	this->pData = pData;
	this->ulWidth = ulWidth;
	this->ulHeight = ulHeight;
	this->fx = LEFT_LANE;

	this->bRandom = bRandom;

	Reset();
}

CSprite::~CSprite( void )
{
	// NO CODE HERE
}

void CSprite::Reset()
{
	fy = fStartY;
	fx = true == bRandom ? 0 == rand() % 2 ? LEFT_LANE : RIGHT_LANE : fx;

	side = LEFT_LANE == fx ? TRACK_SIDE_LEFT : TRACK_SIDE_RIGHT;
}

void CSprite::Draw( void )
{
	drBitBlt( fx, fy, ulWidth, ulHeight, pData );
}

void CSprite::Move( SPRITE_MOVE move )
{
	switch ( move )
	{
	case MOVE_UP:
		fy -= MOVE_UP_AMOUNT;
		break;

	case MOVE_DOWN:
		fy += MOVE_DOWN_AMOUNT;
		break;

	case MOVE_SIDEWAYS:

		if ( side == TRACK_SIDE_LEFT )
		{
			fx = RIGHT_LANE;
			side = TRACK_SIDE_RIGHT;
		} else {
			fx = LEFT_LANE;
			side = TRACK_SIDE_LEFT;
		}

		break;

	default:
		break;
	}
}