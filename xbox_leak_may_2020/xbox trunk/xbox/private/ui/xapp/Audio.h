#define TRANSPORT_STOP	0
#define TRANSPORT_PLAY	1
#define TRANSPORT_PAUSE	2

class CAudioBuf;

class CAudioClip : public CTimeDepNode
{
	DECLARE_NODE(CAudioClip, CTimeDepNode)
public:
	CAudioClip();
	~CAudioClip();

//	TCHAR* m_description;
//	float m_pitch;
	TCHAR* m_url;

	float m_fade;
	float m_volume;
	float m_pan;
	float m_frequency;

    bool m_sendProgress;
    bool m_pause_on_moving;
	float m_progress;
	bool m_removeVoice;
	int m_transportMode;
	int m_lastTransportMode;

	void Play();
	void Pause();
	void PlayOrPause();
	void Stop();

	int getMinutes();
	int getSeconds();

	void* GetSampleBuffer();
	int GetSampleBufferSize();

protected:
	bool m_bDirty;
    bool m_bUnpauseNeeded;
	float m_lastVolume;
	float m_lastPan;
	float m_lastFrequency;

	CAudioBuf* m_pSound;

	void Init();
	void Cleanup();

	bool OpenWaveFile();

	XBOXADPCMWAVEFORMAT m_format;

	void Advance(float nSeconds);
	void OnIsActiveChanged();
	bool OnSetProperty(const PRD* pprd, const void* pvValue);

//	int m_nMinutesLast;
//	int m_nSecondsLast;

	DECLARE_NODE_PROPS()
	DECLARE_NODE_FUNCTIONS()
};
