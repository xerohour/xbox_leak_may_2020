#pragma once

#include "stdafx.h"

class SoundXBOX : public Sound
{
public:
	SoundXBOX();
	virtual ~SoundXBOX();

	HRESULT Initialize();

	virtual void set(Sound::Register reg, uInt8 val);
    virtual void mute(bool state);

private:

	LPDIRECTSOUNDBUFFER m_pBuffer;
	DWORD m_dwStart;
};