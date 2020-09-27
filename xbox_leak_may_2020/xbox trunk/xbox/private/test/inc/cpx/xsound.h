#pragma once

class IXsound {
public:
	static bool InitSound( CHAR** sounds, IXsound** ixsound);
	~IXsound();
	bool XPlaySound(UINT number);
};

//bool InitSound( TCHAR** sounds, XSOUNDS* );
//bool KillSound( void );
//bool XPlaySound( PONG_SOUND pong_sound );