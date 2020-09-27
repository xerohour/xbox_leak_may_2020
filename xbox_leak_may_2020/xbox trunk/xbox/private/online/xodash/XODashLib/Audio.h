#pragma once
#define TRANSPORT_STOP	0
#define TRANSPORT_PLAY	1
#define TRANSPORT_PAUSE	2

class CAudioBuf;

class CAudioClip
{

public:
	CAudioClip();
	~CAudioClip();

	TCHAR* m_url;

	void SetUrl(const TCHAR* AudioFile);

	void Play(bool bLoop = false);
	void Pause();
	void PlayOrPause();
	void Stop();

	int getMinutes();
	int getSeconds();

	void* GetSampleBuffer();
	int GetSampleBufferSize();

	HRESULT Initialize();
	HRESULT Cleanup();

protected:
	bool m_bDirty;
	float m_lastVolume;
	float m_lastPan;
	float m_lastFrequency;
	float m_lastTransportMode;
	float m_fade;
	float m_volume;
	float m_pan;
	float m_frequency;

	int m_transportMode;

	CAudioBuf* m_pSound;


	bool OpenWaveFile();

	XBOXADPCMWAVEFORMAT m_format;

	void OnIsActiveChanged();

	bool m_bLoop;
	bool m_isActive;
};
