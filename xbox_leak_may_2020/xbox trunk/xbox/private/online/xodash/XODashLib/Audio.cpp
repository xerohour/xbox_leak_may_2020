#include "std.h"
#include "Audio.h"

#include "FileUtil.h"
#include "AudioPump.h"

#include "xoconst.h"
#include "xcdplay.h"


CAudioClip::CAudioClip() :
	m_fade(0.0f),
	m_volume(1.0f),
	m_pan(0.0f),
	m_frequency(0.0f),
	m_url(NULL),
	m_transportMode(TRANSPORT_STOP)

{
	m_bDirty = true;
	m_lastTransportMode = -1;
	m_lastVolume = 1.0f;
	m_lastPan = 0.0f;
	m_lastFrequency = 0.0f;

	m_pSound = NULL;
}

CAudioClip::~CAudioClip()
{
	Cleanup();

	delete [] m_url;

}

void CAudioClip::OnIsActiveChanged()
{
	if (m_bDirty)
		Initialize();

	if (m_isActive)
		Play();
	else
		Stop();
}


HRESULT CAudioClip::Cleanup()
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
	return S_OK;
}



HRESULT CAudioClip::Initialize()
{
	HRESULT hr = S_OK;
	ASSERT(m_bDirty);

	// Out with the old...
	Cleanup();
	m_bDirty = false;

	if (m_url == NULL || m_url[0] == 0)
		return S_FALSE;

	do
	{
		// In with the new...
		if (_tcsnicmp(m_url, _T("cd:"), 3) == 0)
		{
			int nTrack = _ttoi(m_url + 3); // 0=nothing; >0 is track number to play

			if (nTrack > 0)
			{
	#ifdef DETECT_MEMORY_LEAKS
				StartLeakTest();
	#endif
				m_pSound = new XCDPlayer;		
				hr = ((XCDPlayer*) m_pSound) ->Initialize(nTrack - 1, &m_format.wfx);			
				BREAKONFAIL(hr, "CAudioClip::Initialize - fail to init XCDRPlayer");
	
			}
		}
		else if (_tcsnicmp(m_url, _T("st:"), 3) == 0)
		{
			DWORD dwSongID = _tcstoul(m_url + 3, NULL, 0); // SongID

			m_pSound = new CWMAPump;
			hr = ((CWMAPump*) m_pSound) ->Initialize(dwSongID, &m_format.wfx);
			BREAKONFAIL(hr, "CAudioClip::Initialize - fail to init CWA");
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
					m_pSound = new CWMAPump;
					hr = ((CWMAPump*) m_pSound) ->Initialize(m_url, NULL, &m_format.wfx);
					BREAKONFAIL(hr, "CAudioClip::Initialize - fail to init CWA");
				}
				else
				{
					DbgPrint("Invalid AudioClip url: %s\n", m_url);
				}
			}
		}
	}while(0);


	if (FAILED(hr))
	{
		delete m_pSound;
		m_pSound = NULL;
	}
	return hr;
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
    _tcscpy( szBuf, SOUNDS_PATH );
    _tcscat( szBuf, m_url );
	HANDLE hFile = XAppCreateFile(szBuf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
 	    DbgPrint("\001Cannot open wave file: %s\n", m_url);
		return false;
	}

	DWORD dwRead;
	WAVFILE1 header;
    RIFFHEADER RiffHeader;

	if (!ReadFile(hFile, &header, sizeof (header), &dwRead, NULL) || dwRead != sizeof (header))
	{
		DbgPrint("\001%s is not a valid wave file...\n", m_url);
		CloseHandle(hFile);
		return false;
	}

	if (header.riff[0] != 'R' || header.riff[1] != 'I' || header.riff[2] != 'F' || header.riff[3] != 'F' ||
		header.wave[0] != 'W' || header.wave[1] != 'A' || header.wave[2] != 'V' || header.wave[3] != 'E' ||
		header.fmt[0] != 'f' || header.fmt[1] != 'm' || header.fmt[2] != 't' || header.fmt[3] != ' ')
	{
		DbgPrint("\001%s is not a valid wave file (bad header chunk)\n", m_url);
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
		DbgPrint("\001%s is not a valid wave file (bad format chunk)\n", m_url);
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
        DbgPrint("\001%s is not a valid wave file (bad data chunk)\n", m_url);
        CloseHandle(hFile);
        return false;
    }

	HRESULT hr = S_OK;
	ASSERT(m_pSound == NULL);
	if (dwDataSize > 65536)
	{
		CFilePump* pSound = new CFilePump;
		m_pSound = pSound;
		hr = pSound->Initialize(hFile, dwDataSize, &m_format.wfx);
		if (FAILED(hr))
		{
			DbgPrint("Could not initialize CFilePump for %s\n", m_url);
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
		hr = m_pSound->Initialize(&m_format.wfx, dwDataSize);
		if(FAILED(hr))
		{
			DbgPrint("Could not initialize CAudioBuf for %s\n", m_url);
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
			DbgPrint("\001%s is not a valid wave file...\n", m_url);
			delete m_pSound;
			m_pSound = NULL;

			return false;
		}
	}

	return true;
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

void CAudioClip::Play(bool bLoop /*false*/)
{
	if (m_bDirty)
		Initialize();
	m_bLoop = bLoop;
	if (m_pSound != NULL)
	{
		m_pSound->SetAttenuation((1.0f - m_volume) * 100.0f);
		m_pSound->SetPan(m_pan);
		m_pSound->SetFrequency(m_frequency);

		if (m_pSound->Play(m_bLoop))
		{
			m_transportMode = TRANSPORT_PLAY;
			m_isActive = true;
		}
        else DbgPrint("Play failed!\n");
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
		Initialize();

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
		Initialize();

	if (m_pSound == NULL)
		return 0;

	return (int)m_pSound->GetPlaybackTime() / 60;
}

int CAudioClip::getSeconds()
{
	if (m_bDirty)
		Initialize();

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


void CAudioClip::SetUrl(const TCHAR* pszAudioFile)
{
	if(m_url)
	{
		delete [] m_url;
	}
	m_bDirty = true;
	if(pszAudioFile)
	{
		m_url = new TCHAR[_tcslen(pszAudioFile)+1];
		_tcscpy(m_url, pszAudioFile);
	}

}
