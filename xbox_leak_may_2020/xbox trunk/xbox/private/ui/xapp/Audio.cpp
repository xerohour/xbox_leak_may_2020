#include "std.h"

#include "xapp.h"
#include "Node.h"
#include "Lerper.h"
#include "FileUtil.h"
#include "AudioPump.h"
#include "Audio.h"
#include "Runner.h"
#include "xcdplay.h"

#if defined(XBOX) && defined(DBG)
// #define DETECT_MEMORY_LEAKS
extern "C" void StartLeakTest();
extern "C" void EndLeakTest();
#else
#endif

bool g_bRemoveVoice = false; // REVIEW: Clean this up!
extern bool g_bLevelTransition;

IMPLEMENT_NODE("AudioClip", CAudioClip, CTimeDepNode)

START_NODE_PROPS(CAudioClip, CTimeDepNode)
//	NODE_PROP(pt_string, CAudioClip, description)
//	NODE_PROP(pt_number, CAudioClip, pitch)
	NODE_PROP(pt_number, CAudioClip, volume)
	NODE_PROP(pt_number, CAudioClip, pan)
	NODE_PROP(pt_number, CAudioClip, frequency)
	NODE_PROP(pt_number, CAudioClip, fade)
	NODE_PROP(pt_string, CAudioClip, url)
	NODE_PROP(pt_integer, CAudioClip, transportMode)
	NODE_PROP(pt_boolean, CAudioClip, removeVoice)
	NODE_PROP(pt_boolean, CAudioClip, sendProgress)
	NODE_PROP(pt_boolean, CAudioClip, pause_on_moving)
	NODE_PROP(pt_number, CAudioClip, progress)
END_NODE_PROPS()

START_NODE_FUN(CAudioClip, CTimeDepNode)
	NODE_FUN_VV(Play)
	NODE_FUN_VV(Stop)
	NODE_FUN_VV(Pause)
	NODE_FUN_IV(getMinutes)
	NODE_FUN_IV(getSeconds)
	NODE_FUN_VV(PlayOrPause)
END_NODE_FUN()

CAudioClip::CAudioClip() :
//	m_description(NULL),
//	m_pitch(1.0f),
	m_fade(0.0f),
	m_volume(1.0f),
	m_pan(0.0f),
	m_frequency(0.0f),
	m_url(NULL),
	m_transportMode(TRANSPORT_STOP),
	m_removeVoice(false),
    m_sendProgress(false),
    m_pause_on_moving(false),
    m_bUnpauseNeeded(false),
	m_progress(0)
{
	m_bDirty = true;
//	m_nMinutesLast = 0;
//	m_nSecondsLast = 0;
	m_lastTransportMode = -1;
	m_lastVolume = 1.0f;
	m_lastPan = 0.0f;
	m_lastFrequency = 0.0f;

	m_pSound = NULL;
	m_visible = false;
}

CAudioClip::~CAudioClip()
{
	Cleanup();

	delete [] m_url;
//	delete [] m_description;
}

bool CAudioClip::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_volume))
	{
		if (m_fade != 0.0f)
		{
			float volume = *(float*)pvValue;
			CLerper::RemoveObject(this);
			new CLerper(this, &m_volume, volume, m_fade);
			return false;
		}
	}
	if ((int)pprd->pbOffset == offsetof(m_pan))
	{
		if (m_fade != 0.0f)
		{
			float pan = *(float*)pvValue;
			CLerper::RemoveObject(this);
			new CLerper(this, &m_pan, pan, m_fade);
			return false;
		}
	}
	else if ((int)pprd->pbOffset == offsetof(m_url))
	{
		m_bDirty = true;
	}
	else if ((int)pprd->pbOffset == offsetof(m_isActive))
	{
		m_isActive = *(bool*)pvValue;
		OnIsActiveChanged();
	}

	return true;
}

void CAudioClip::OnIsActiveChanged()
{
	if (m_bDirty)
		Init();

	if (m_isActive)
		Play();
	else
		Stop();
}


void CAudioClip::Cleanup()
{
	Stop();

	delete m_pSound;
	m_pSound = NULL;
	m_lastVolume = 1.0f;
	m_lastPan = 0.0f;
	m_lastFrequency = 0.0f;
#ifdef DETECT_MEMORY_LEAKS
    EndLeakTest();
#endif
}

void CAudioClip::Init()
{
	ASSERT(m_bDirty);

	// Out with the old...
	Cleanup();
	m_bDirty = false;

	if (m_url == NULL || m_url[0] == 0)
		return;

	// In with the new...
	if (_tcsnicmp(m_url, _T("cd:"), 3) == 0)
	{
		int nTrack = _ttoi(m_url + 3); // 0=nothing; >0 is track number to play

		if (nTrack > 0)
		{
#ifdef DETECT_MEMORY_LEAKS
            StartLeakTest();
#endif
			XCDPlayer* pPlayer = new XCDPlayer;
			m_pSound = pPlayer;
			if (!pPlayer->Initialize(nTrack - 1, &m_format.wfx))
			{
				delete pPlayer;
				m_pSound = NULL;
			}
		}
	}
	else if (_tcsnicmp(m_url, _T("st:"), 3) == 0)
	{
		DWORD dwSongID = _tcstoul(m_url + 3, NULL, 0); // SongID

		CWMAPump* pSound = new CWMAPump;
		m_pSound = pSound;
		if (!pSound->Initialize(dwSongID, &m_format.wfx))
		{
			delete m_pSound;
			m_pSound = NULL;
		}
	}
	else
	{
		const TCHAR* pch = _tcsrchr(m_url, '.');
		if (pch != NULL)
		{
			if (_tcsicmp(pch + 1, _T("wav")) == 0)
			{
				OpenWaveFile();
			}
			else if (_tcsicmp(pch + 1, _T("wma")) == 0)
			{
				CWMAPump* pSound = new CWMAPump;
				m_pSound = pSound;
				if (!pSound->Initialize(m_url, NULL, &m_format.wfx))
				{
					delete m_pSound;
					m_pSound = NULL;
				}
			}
			else
			{
				TRACE(_T("Invalid AudioClip url: %s\n"), m_url);
			}
		}
	}
}

struct WAVFILE1
{
	BYTE riff [4];
	DWORD dwSize;
	BYTE wave [4];
	BYTE fmt [4];
	DWORD dwFormatSize;
};

struct WAVFILE2
{
	BYTE data [4];
	DWORD dwDataSize;
};

typedef struct
{
    FOURCC  fccChunkId;
    DWORD   dwDataSize;
} RIFFHEADER, *LPRIFFHEADER;

#ifndef FOURCC_RIFF
#define FOURCC_RIFF 'FFIR'
#endif // FOURCC_RIFF

#ifndef FOURCC_WAVE
#define FOURCC_WAVE 'EVAW'
#endif // FOURCC_WAVE

#ifndef FOURCC_FORMAT
#define FOURCC_FORMAT ' tmf'
#endif // FOURCC_FORMAT

#ifndef FOURCC_DATA
#define FOURCC_DATA 'atad'
#endif // FOURCC_DATA

bool CAudioClip::OpenWaveFile()
{
	TCHAR szBuf [MAX_PATH];
	FindFilePath(szBuf, m_url);
	HANDLE hFile = XAppCreateFile(szBuf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("\001Cannot open wave file: %s\n"), m_url);
		return false;
	}

	DWORD dwRead;
	WAVFILE1 header;
    RIFFHEADER RiffHeader;

	if (!ReadFile(hFile, &header, sizeof (header), &dwRead, NULL) || dwRead != sizeof (header))
	{
		TRACE(_T("\001%s is not a valid wave file...\n"), m_url);
		CloseHandle(hFile);
		return false;
	}

	if (header.riff[0] != 'R' || header.riff[1] != 'I' || header.riff[2] != 'F' || header.riff[3] != 'F' ||
		header.wave[0] != 'W' || header.wave[1] != 'A' || header.wave[2] != 'V' || header.wave[3] != 'E' ||
		header.fmt[0] != 'f' || header.fmt[1] != 'm' || header.fmt[2] != 't' || header.fmt[3] != ' ')
	{
		TRACE(_T("\001%s is not a valid wave file (bad header chunk)\n"), m_url);
		CloseHandle(hFile);
		return false;
	}

	DWORD dwFormatSize = header.dwFormatSize;
	ZeroMemory(&m_format, sizeof(m_format));

	if (dwFormatSize > sizeof(m_format))
    {
		dwFormatSize = sizeof(m_format);
    }

	if (!ReadFile(hFile, &m_format, dwFormatSize, &dwRead, NULL) || dwRead != dwFormatSize)
	{
		TRACE(_T("\001%s is not a valid wave file (bad format chunk)\n"), m_url);
		CloseHandle(hFile);
		return false;
	}

    if (header.dwFormatSize != dwFormatSize)
    {
        SetFilePointer(hFile, header.dwFormatSize - dwFormatSize, 0, FILE_CURRENT);
    }

    BOOL b;
    DWORD dwDataSize = 0;

    ASSERT(m_format.wfx.wFormatTag == WAVE_FORMAT_PCM || m_format.wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM);

	do
	{
        b = ReadFile(hFile, &RiffHeader, sizeof(RiffHeader), &dwRead, NULL);
        if (b)
        {
            if (RiffHeader.fccChunkId == FOURCC_DATA)
            {
                dwDataSize = RiffHeader.dwDataSize;
                break;
            }

            b = SetFilePointer(hFile, RiffHeader.dwDataSize, 0, FILE_CURRENT);
        }
	}
	while (b);

    if (!b || dwDataSize == 0)
    {
        TRACE(_T("\001%s is not a valid wave file (bad data chunk)\n"), m_url);
        CloseHandle(hFile);
        return false;
    }

	ASSERT(m_pSound == NULL);
	if (dwDataSize > 65536)
	{
		CFilePump* pSound = new CFilePump;
		m_pSound = pSound;
		if (!pSound->Initialize(hFile, dwDataSize, &m_format.wfx))
		{
			TRACE(_T("Could not initialize CFilePump for %s\n"), m_url);
			CloseHandle(hFile);
			delete pSound;
			m_pSound = NULL;
			return false;
		}

		// NOTE: File will be closed by CFilePump when it's done...
	}
	else
	{
		m_pSound = new CAudioBuf;
		if (!m_pSound->Initialize(&m_format.wfx, dwDataSize))
		{
			TRACE(_T("Could not initialize CAudioBuf for %s\n"), m_url);
			CloseHandle(hFile);
			delete m_pSound;
			m_pSound = NULL;
			return false;
		}

		void* pvBuffer = m_pSound->Lock();
		bool bError = !ReadFile(hFile, pvBuffer, dwDataSize, &dwRead, NULL) || dwRead != dwDataSize;
		m_pSound->Unlock(pvBuffer);

		CloseHandle(hFile);

		if (bError)
		{
			TRACE(_T("\001%s is not a valid wave file...\n"), m_url);
			delete m_pSound;
			m_pSound = NULL;

			return false;
		}
	}

	return true;
}

void CAudioClip::Advance(float nSeconds)
{
    if (m_bDirty)
        Init();

    // Pause this audio clip if we are in the middle of level transition
    if (m_pause_on_moving && m_pSound && m_volume != 0.0f)
    {
        if (g_bLevelTransition)
        {
            if (!m_pSound->IsPaused())
            {
                m_pSound->Pause(true);
                m_bUnpauseNeeded = true;
            }
            return;
        }

        if (!g_bLevelTransition && m_bUnpauseNeeded)
        {
            if (m_pSound->IsPaused())
            {
                m_pSound->Pause(false);
            }

            m_bUnpauseNeeded = false;
        }
    }

	CTimeDepNode::Advance(nSeconds);

	g_bRemoveVoice = m_removeVoice; // REVIEW: Clean this up!

	if (m_pSound == NULL)
	{
		m_isActive = false;
		m_transportMode = TRANSPORT_STOP;
	}
	else if (m_isActive)
	{
		if (m_volume != m_lastVolume)
		{
			if (m_lastVolume == 0.0f && m_transportMode == TRANSPORT_PAUSE)
			{
				TRACE(_T("Un-Pausing %s due to non-zero volume...\n"), m_url);
				Pause();
			}

			m_lastVolume = m_volume;

			m_pSound->SetAttenuation((1.0f - m_volume) * 100.0f);

			if (m_volume == 0.0f && m_transportMode == TRANSPORT_PLAY)
			{
				TRACE(_T("Pausing %s due to zero volume...\n"), m_url);
				Pause();
			}
		}

		if (m_pan != m_lastPan)
		{
			m_lastPan = m_pan;

			m_pSound->SetPan(m_pan);
		}

		if (m_frequency != m_lastFrequency)
		{
			m_lastFrequency = m_frequency;

			m_pSound->SetFrequency(m_frequency);
		}

		// BLOCK: Check progress...
        if (m_sendProgress)
		{
			// OPTIMIZATION: Might want to make this optional...
			float progress = 0.0f;

			float nLength = m_pSound->GetPlaybackLength();
			if (nLength != 0.0f)
				progress = m_pSound->GetPlaybackTime() / nLength;

			if (progress != m_progress)
			{
				m_progress = progress;
				CallFunction(this, _T("OnProgressChanged"));
			}
		}

		if (m_pSound->IsPaused())
		{
			m_transportMode = TRANSPORT_PAUSE;
		}
		else if (m_pSound->IsPlaying())
		{
			m_transportMode = TRANSPORT_PLAY;
		}
		else
		{
			if (m_loop && m_isActive)
			{
				// Restart the sound...
				// REVIEW: This really shouldn't happen as the CAudioBuf object
				// should handle looping itself (to avoid pops)...
				ASSERT(FALSE);
//				TRACE(_T("looping %s\n"), m_url);
//				m_pSound->Play(m_loop);
//				m_transportMode = TRANSPORT_PLAY;
			}
			else
			{
				if (m_transportMode == TRANSPORT_PLAY)
				{
					m_transportMode = TRANSPORT_STOP;
					m_isActive = false;

					CallFunction(this, _T("OnEndOfAudio"));
				}
			}
		}
	}

	if (m_transportMode != m_lastTransportMode)
	{
		m_lastTransportMode = m_transportMode;
		CallFunction(this, _T("OnTransportModeChanged"));
	}
}

void CAudioClip::PlayOrPause()
{
	switch (m_transportMode)
	{
	case TRANSPORT_PLAY:
	case TRANSPORT_PAUSE:
		Pause();
		break;

	default:
		Play();
		break;
	}
}

void CAudioClip::Play()
{
	if (m_bDirty)
		Init();

//	if (m_transportMode == TRANSPORT_PLAY)
//		return;

	if (m_pSound != NULL)
	{
		m_pSound->SetAttenuation((1.0f - m_volume) * 100.0f);
		m_pSound->SetPan(m_pan);
		m_pSound->SetFrequency(m_frequency);

		if (m_pSound->Play(m_loop))
		{
			m_transportMode = TRANSPORT_PLAY;
			m_isActive = true;
		}
        else TRACE(_T("Play failed!\n"));
	}

}

void CAudioClip::Stop()
{
	if (m_transportMode == TRANSPORT_STOP)
		return;

	m_transportMode = TRANSPORT_STOP;

	if (m_pSound != NULL)
		m_pSound->Stop();

	delete m_pSound;
	m_pSound = NULL;

	m_isActive = false;
	m_bDirty = true; // so audio buffer is re-created and will start over
}

void CAudioClip::Pause()
{
	if (m_bDirty)
		Init();

	if (m_transportMode == TRANSPORT_PAUSE)
	{
		m_transportMode = TRANSPORT_PLAY;

		if (m_pSound != NULL)
			m_pSound->Pause(false);
	}
	else
	{
		m_transportMode = TRANSPORT_PAUSE;

		if (m_pSound != NULL)
			m_pSound->Pause(true);
	}
}

int CAudioClip::getMinutes()
{
	if (m_bDirty)
		Init();

	if (m_pSound == NULL)
		return 0;

	return (int)m_pSound->GetPlaybackTime() / 60;
}

int CAudioClip::getSeconds()
{
	if (m_bDirty)
		Init();

	if (m_pSound == NULL)
		return 0;

	return (int)m_pSound->GetPlaybackTime() % 60;
}


void* CAudioClip::GetSampleBuffer()
{
	if (m_pSound == NULL)
		return NULL;

	return m_pSound->GetSampleBuffer();
}

int CAudioClip::GetSampleBufferSize()
{
	if (m_pSound == NULL)
		return 0;

	return m_pSound->GetSampleBufferSize();
}

////////////////////////////////////////////////////////////////////////////

class CPeriodicAudioGroup : public CGroup
{
	DECLARE_NODE(CPeriodicAudioGroup, CGroup)
public:
	CPeriodicAudioGroup();
	
	void Advance(float nSeconds);

	bool m_isActive;
    bool m_pause_on_moving;
	float m_period;
	float m_periodNoise;

	XTIME m_timeOfNextEvent;
	int m_nNextClip;
    CAudioClip* m_pClip;

	DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("PeriodicAudioGroup", CPeriodicAudioGroup, CGroup)

START_NODE_PROPS(CPeriodicAudioGroup, CGroup)
	NODE_PROP(pt_boolean, CPeriodicAudioGroup, isActive)
	NODE_PROP(pt_number, CPeriodicAudioGroup, period)
	NODE_PROP(pt_number, CPeriodicAudioGroup, periodNoise)
END_NODE_PROPS()


CPeriodicAudioGroup::CPeriodicAudioGroup() :
	m_isActive(false),
    m_pause_on_moving(true),
	m_period(10.0f),
	m_periodNoise(0.0f)
{
	m_visible = false;

	m_timeOfNextEvent = 0.0f;
	m_nNextClip = -1;
    m_pClip = NULL;
}

void CPeriodicAudioGroup::Advance(float nSeconds)
{
	CGroup::Advance(nSeconds);

    // Stop this background audio group if we are in the middle of level transition
    if (m_pause_on_moving && g_bLevelTransition)
    {
        if (m_pClip && m_pClip->IsKindOf(NODE_CLASS(CAudioClip)))
        {
            m_pClip->Stop();
        }

        return;
    }

	if (!m_isActive)
	{
		m_timeOfNextEvent = 0.0f;
		return;
	}

	int nChildCount = m_children.GetLength();
	if (nChildCount == 0)
		return;

	if (m_nNextClip < 0)
		m_nNextClip = rand() % nChildCount;

	if (m_timeOfNextEvent == 0.0f)
	{
		m_timeOfNextEvent = XAppGetNow() + m_period + rnd(m_periodNoise);
		TRACE(_T("Next clip will play in %0.2f seconds...\n"), m_timeOfNextEvent - XAppGetNow());
		return;
	}
	
	if (XAppGetNow() > m_timeOfNextEvent)
	{
		m_timeOfNextEvent = 0.0f;

		if (m_nNextClip >= nChildCount)
			m_nNextClip = 0;

		m_pClip = (CAudioClip*)m_children.GetNode(m_nNextClip);
		m_nNextClip += 1;

		if (!m_pClip->IsKindOf(NODE_CLASS(CAudioClip)))
		{
			TRACE(_T("CPeriodicAudioGroup: child %d is not an AudioClip!\n"), m_nNextClip-1);
			return;
		}

		TRACE(_T("CPeriodicAudioGroup: playing clip %d (%s)\n"), m_nNextClip-1, m_pClip->m_url);
		m_pClip->Play();
	}
}
