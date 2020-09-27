#pragma once

#include <xtl.h>
#include <stdlib.h>
#include <draw.h>

#include "donkeyconstants.h"

enum TRACK_SIDE { TRACK_SIDE_LEFT, TRACK_SIDE_RIGHT };
enum SPRITE_MOVE { MOVE_UP, MOVE_DOWN, MOVE_SIDEWAYS };

class CSprite {

public:

	CSprite( float fy, ULONG ulWidth, ULONG ulHeight, const BYTE* pData, bool bRandom );
	~CSprite();

	void Draw( void );
	void Move( SPRITE_MOVE move );

	void Reset( void );

public:

	bool bRandom;
	float fx;
	float fy;
	ULONG ulWidth;
	ULONG ulHeight;
	const BYTE* pData;

	float fStartY;

	TRACK_SIDE side;

};