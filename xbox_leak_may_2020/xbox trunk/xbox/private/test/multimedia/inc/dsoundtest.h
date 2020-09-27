#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <stdlib.h>
#include <waveldr.h>
#include <macros.h>
#include <dsoundtestconstants.h>
#include <stdio.h>
#include <dsutil.h>

class CDSoundTest {

public:

	/*****/ CDSoundTest             ( void                                     );
	/*****/ ~CDSoundTest            ( void                                     );

	void    Close                   ( void                                     );
	void    DestroyDSound           ( void                                     );

	DWORD   CalculateSleepTime      ( void                                     );
	DWORD   CalculateBufferSize     ( void                                     );

	HRESULT PopulateDSBD            ( void                                     );
	HRESULT PopulateBuffer          ( void                                     );
	HRESULT ReAllocate              ( void                                     );
	HRESULT Wait                    ( void                                     );
	HRESULT PlayAndWait             ( void                                     );

	HRESULT CreateBuffer            ( void                                     );
	HRESULT CreateDSound            ( void                                     );
	HRESULT CreateSoundBuffer       ( void                                     );
	HRESULT CheckMemory             ( void                                     );

	HRESULT Open                    ( LPCSTR tszFileName                       );
	HRESULT OpenWaveFile            ( LPCSTR tszFileName                       );
	HRESULT OpenAndPlay             ( LPCSTR tszFileName                       );
	HRESULT OpenRandomFile          ( LPCSTR tszPath                           );

	bool    CreateCompleteBufferSet ( void                                     ) { return CREATE_COMPLETE_BUFFER == m_fBufferDuration; };
	bool    PlayPollingSet          ( void                                     ) { return PLAY_POLLING           == m_dwSleepTime;     };
	bool    PlayToCompletionSet     ( void                                     ) { return PLAY_TO_COMPLETION     == m_dwSleepTime;     };
	bool    StillPlaying            ( void                                     ) { return DSBSTATUS_PLAYING       & m_dwStatus;        };

	HRESULT CommitDeferredSettings  ( void                                     ) { return NULL == m_pDSound       ? E_UNEXPECTED : m_pDSound->CommitDeferredSettings        (                                                                                                                         ); };
	HRESULT GetCaps                 ( void                                     ) { return NULL == m_pDSound       ? E_UNEXPECTED : m_pDSound->GetCaps                       ( m_pdsCaps                                                                                                               ); };
//	HRESULT SetSpeakerConfig        ( void                                     ) { return NULL == m_pDSound       ? E_UNEXPECTED : m_pDSound->SetSpeakerConfig              ( m_dwSpeakerConfig                                                                                                       ); };
	HRESULT GetSpeakerConfig        ( void                                     ) { return NULL == m_pDSound       ? E_UNEXPECTED : m_pDSound->GetSpeakerConfig              ( &m_dwSpeakerConfig                                                                                                      ); };
	HRESULT ListenerSetAllParameters( void                                     ) { return NULL == m_pDSound       ? E_UNEXPECTED : m_pDSound->SetAllParameters              ( m_pds3dl,             m_dwApply                                                                                         ); };
	HRESULT SetDistanceFactor       ( void                                     ) { return NULL == m_pDSound       ? E_UNEXPECTED : m_pDSound->SetDistanceFactor             ( m_pad3dVal[0],        m_dwApply                                                                                         ); };
	HRESULT SetDopplerFactor        ( void                                     ) { return NULL == m_pDSound       ? E_UNEXPECTED : m_pDSound->SetDopplerFactor              ( m_pad3dVal[0],        m_dwApply                                                                                         ); };
	HRESULT SetRolloffFactor        ( void                                     ) { return NULL == m_pDSound       ? E_UNEXPECTED : m_pDSound->SetRolloffFactor              ( m_pad3dVal[0],        m_dwApply                                                                                         ); };
	HRESULT ListenerSetPosition     ( void                                     ) { return NULL == m_pDSound       ? E_UNEXPECTED : m_pDSound->SetPosition                   ( m_pad3dVal[0],        m_pad3dVal[1],        m_pad3dVal[2], m_dwApply                                                    ); };
	HRESULT ListenerSetVelocity     ( void                                     ) { return NULL == m_pDSound       ? E_UNEXPECTED : m_pDSound->SetVelocity                   ( m_pad3dVal[0],        m_pad3dVal[1],        m_pad3dVal[2], m_dwApply                                                    ); };
	HRESULT SetOrientation          ( void                                     ) { return NULL == m_pDSound       ? E_UNEXPECTED : m_pDSound->SetOrientation                ( m_pad3dVal[0],        m_pad3dVal[1],        m_pad3dVal[2], m_pad3dVal[3],  m_pad3dVal[4], m_pad3dVal[5],  m_dwApply     ); };
	HRESULT Stop                    ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->Stop                    (                                                                                                                         ); };

#ifdef    SILVER
	HRESULT SetChannelVolume        ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetChannelVolume        ( m_pdscv                                                                                                                 ); };
#endif // SILVER

	HRESULT SetVolume               ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetVolume               ( m_lVolume                                                                                                               ); };
	HRESULT GetStatus               ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->GetStatus               ( &m_dwStatus                                                                                                             ); };
	HRESULT SetFrequency            ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetFrequency            ( m_dwFrequency                                                                                                           ); };
	HRESULT Buffer3DSetAllParameters( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetAllParameters        ( m_pds3db,             m_dwApply                                                                                         ); };
	HRESULT SetConeOutsideVolume    ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetConeOutsideVolume    ( m_lConeOutsideVolume, m_dwApply                                                                                         ); };
	HRESULT SetMaxDistance          ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetMaxDistance          ( m_pad3dVal[0],        m_dwApply                                                                                         ); };
	HRESULT SetMinDistance          ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetMinDistance          ( m_pad3dVal[0],        m_dwApply                                                                                         ); };
	HRESULT SetMode                 ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetMode                 ( m_dwMode,             m_dwApply                                                                                         ); };
	HRESULT SetNotificationPositions( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetNotificationPositions( m_dwNotifyCount,      m_paNotifies                                                                                      ); };	
	HRESULT SetCurrentPosition      ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetCurrentPosition      ( m_dwPlayCursor                                                                                                          ); };
	HRESULT GetCurrentPosition      ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->GetCurrentPosition      ( &m_dwPlayCursor,      &m_dwWriteCursor                                                                                  ); };
	HRESULT SetBufferData           ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetBufferData           ( m_pvBufferData,       m_dwBufferBytes                                                                                   ); };
    HRESULT SetLoopRegion           ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetLoopRegion           ( m_dwLoopStart,        m_dwLoopLength                                                                                    ); };
 	HRESULT SetConeAngles           ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->SetConeAngles           ( m_dwInsideConeAngle,  m_dwOutsideConeAngle, m_dwApply                                                                   ); };
	HRESULT Play                    ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->Play                    ( m_dwReserved1,        m_dwReserved2,        m_dwPlayFlags                                                               ); };
	HRESULT Lock                    ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->Lock                    ( m_dwWriteCursor,      m_dwWriteBytes,       &m_pvLock1,    &m_dwLockSize1, &m_pvLock2,    &m_dwLockSize2, m_dwLockFlags ); };
	HRESULT StopEx                  ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->StopEx                  ( m_rtTimeStamp,        m_dwFlags                                                                                         ); };
	HRESULT PlayEx                  ( void                                     ) { return NULL == m_pDSoundBuffer ? E_UNEXPECTED : m_pDSoundBuffer->PlayEx                  ( m_rtTimeStamp,        m_dwFlags                                                                                         ); };

	void    SetD3DVals              ( FLOAT*               paD3DVal            ) { m_pad3dVal            = paD3DVal;            };
	void    SetControllingUnknown   ( LPUNKNOWN            pControllingUnknown ) { m_pControllingUnknown = pControllingUnknown; };
#ifdef    SILVER
	void    SetChannelVolume        ( LPDSCHANNELVOLUME    pdscv               ) { m_pdscv               = pdscv;               };
	void    SetDeviceId             ( DWORD                dwDeviceId          ) { m_dwDeviceId          = dwDeviceId;          };

#endif // SILVER
	void    SetLockPointer1         ( LPVOID               pvLock              ) { m_pvLock1             = pvLock;              };
	void    SetLockPointer2         ( LPVOID               pvLock              ) { m_pvLock2             = pvLock;              };
	void    SetBufferData           ( LPVOID               lpvBufferData       ) { m_pvBufferData        = lpvBufferData;       };
	void    SetNotifies             ( LPCDSBPOSITIONNOTIFY paNotifies          ) { m_paNotifies          = paNotifies;          };
	void    SetTimeStamp            ( REFERENCE_TIME       rtTimeStamp         ) { m_rtTimeStamp         = rtTimeStamp;         };
	void    SetDuration             ( DWORD                dwDuration          ) { m_dwDuration          = dwDuration;          };
	void    SetBufferSize           ( DWORD                dwBufferSize        ) { m_dwBufferSize        = dwBufferSize;        };
    void    SetBufferFlags          ( DWORD                dwFlags             ) { m_dwBufferFlags       = dwFlags;             };
	void    SetLockSize1            ( DWORD                dwLockSize          ) { m_dwLockSize1         = dwLockSize;          };
	void    SetLockSize2            ( DWORD                dwLockSize          ) { m_dwLockSize2         = dwLockSize;          };
	void    SetLockFlags            ( DWORD                dwFlags             ) { m_dwLockFlags         = dwFlags;             };
	void    SetWriteCursor          ( DWORD                dwWriteCursor       ) { m_dwWriteCursor       = dwWriteCursor;       };
	void    SetWriteBytes           ( DWORD                dwWriteBytes        ) { m_dwWriteBytes        = dwWriteBytes;        };
	void    SetReadPosition         ( DWORD                dwReadPosition      ) { m_dwReadPosition      = dwReadPosition;      };
    void    SetLoopStart            ( DWORD                dwLoopStart         ) { m_dwLoopStart         = dwLoopStart;         };
    void    SetLoopLength           ( DWORD                dwLoopLength        ) { m_dwLoopLength        = dwLoopLength;        };
	void    SetSleepTime            ( DWORD                dwSleepTime         ) { m_dwSleepTime         = dwSleepTime;         };
	void    SetBufferStatus         ( DWORD                dwBufferStatus      ) { m_dwBufferStatus      = dwBufferStatus;      };
	void    SetPlayCursor           ( DWORD                dwPlayCursor        ) { m_dwPlayCursor        = dwPlayCursor;        };
	void    SetFrequency            ( DWORD                dwFrequency         ) { m_dwFrequency         = dwFrequency;         };
	void    SetChannelCount         ( DWORD                dwChannelCount      ) { m_dwChannelCount      = dwChannelCount;      };
	void    SetStatus               ( DWORD                dwStatus            ) { m_dwStatus            = dwStatus;            };
	void    SetApply                ( DWORD                dwApply             ) { m_dwApply             = dwApply;             };
	void    SetInsideConeAngle      ( DWORD                dwInsideConeAngle   ) { m_dwInsideConeAngle   = dwInsideConeAngle;   };
	void    SetOutsideConeAngle     ( DWORD                dwOutsideConeAngle  ) { m_dwOutsideConeAngle  = dwOutsideConeAngle;  };
	void    SetMode                 ( DWORD                dwMode              ) { m_dwMode              = dwMode;              };
	void    SetNumBuffers           ( DWORD                dwNumBuffers        ) { m_dwNumBuffers        = dwNumBuffers;        };
	void    SetPlayFlags            ( DWORD                dwPlayFlags         ) { m_dwPlayFlags         = dwPlayFlags;         };
	void    SetReserved1            ( DWORD                dwReserved1         ) { m_dwReserved1         = dwReserved1;         };
	void    SetReserved2            ( DWORD                dwReserved2         ) { m_dwReserved2         = dwReserved2;         };
	void    SetBufferBytes          ( DWORD                dwBufferBytes       ) { m_dwBufferBytes       = dwBufferBytes;       };
	void    SetSpeakerConfig        ( DWORD                dwSpeakerConfig     ) { m_dwSpeakerConfig     = dwSpeakerConfig;     };
	void    SetNotifyCount          ( DWORD                dwNotifyCount       ) { m_dwNotifyCount       = dwNotifyCount;       };
	void    SetFlags                ( DWORD                dwFlags             ) { m_dwFlags             = dwFlags;             };
	void    SetConeOutsideVolume    ( LONG                 lConeOutsideVolume  ) { m_lConeOutsideVolume  = lConeOutsideVolume;  };
	void    SetVolume               ( LONG                 lVolume             ) { m_lVolume             = lVolume;             };
	void    SetBufferDuration       ( float                fBufferDuration     ) { m_fBufferDuration     = fBufferDuration;     };
    void    SetStress               ( bool                 bStress             ) { m_bStress             = bStress;             };
    void    SetWaveFormat           ( LPWAVEFORMATEX       pWFX                );
	void    SetDSCaps               ( LPDSCAPS             pdsCaps             );
	void    SetListener             ( LPDS3DLISTENER       pListener           );
	void    SetBuffer               ( LPDS3DBUFFER         pBuffer             );

protected:

	LPDS3DLISTENER       m_pds3dl;
	LPDS3DBUFFER         m_pds3db;
	LPDSCAPS             m_pdsCaps;
	DSBUFFERDESC*        m_pDsbd;
	WAVEFORMATEX*        m_pWaveFormat;
	CWaveFile*           m_pWaveFile;

	LPDIRECTSOUND        m_pDSound;               
	LPDIRECTSOUNDBUFFER  m_pDSoundBuffer;         
    FLOAT*               m_pad3dVal;              
	LPUNKNOWN            m_pControllingUnknown;   

#ifdef    SILVER
	LPDSCHANNELVOLUME    m_pdscv; 
	DWORD                m_dwDeviceId;  
#endif // SILVER

    LPVOID               m_pvLock1;               
	LPVOID               m_pvLock2;               
	LPVOID               m_pvBufferData;          
	LPCDSBPOSITIONNOTIFY m_paNotifies;            
	REFERENCE_TIME       m_rtTimeStamp;           
	DWORD                m_dwDuration;            
	DWORD                m_dwBufferSize;          
	DWORD                m_dwBufferFlags;         
	DWORD                m_dwLockSize1;           
	DWORD                m_dwLockSize2;           
	DWORD                m_dwLockFlags;           
	DWORD                m_dwWriteCursor;         
	DWORD                m_dwWriteBytes;          
	DWORD                m_dwReadPosition;        
    DWORD                m_dwLoopStart;           
    DWORD                m_dwLoopLength;          
	DWORD                m_dwSleepTime;           
	DWORD                m_dwBufferStatus;        
	DWORD                m_dwPlayCursor;          
	DWORD                m_dwFrequency;           
	DWORD                m_dwChannelCount;        
	DWORD                m_dwStatus;              
	DWORD                m_dwApply;               
	DWORD                m_dwInsideConeAngle;     
	DWORD                m_dwOutsideConeAngle;    
	DWORD                m_dwMode;                
	DWORD                m_dwNumBuffers;          
	DWORD                m_dwPlayFlags;           
	DWORD                m_dwReserved1;           
	DWORD                m_dwReserved2;           
	DWORD                m_dwBufferBytes;         
	DWORD                m_dwSpeakerConfig;       
	DWORD                m_dwNotifyCount;         
	DWORD                m_dwFlags;               
	LONG                 m_lConeOutsideVolume;    
	LONG                 m_lVolume;               
	float                m_fBufferDuration;
	bool                 m_bStress;
};




