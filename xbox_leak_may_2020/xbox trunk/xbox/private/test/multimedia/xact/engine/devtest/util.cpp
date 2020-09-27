#include <xtl.h>
#include <xdbg.h>
#include <xactp.h>
#include "devtest.h"
#include <dsstdfx.h>


HRESULT LoadBinaryData(const CHAR *pszName, PVOID *ppvData, PDWORD pdwSize)
{

    DWORD dwBytesRead;

    HANDLE hFile = CreateFile(
        pszName,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {

        DbgPrint("Failed to load %s\n",pszName);
        return E_FAIL;
    }

    *pdwSize = SetFilePointer(hFile,0,0,FILE_END);


    *ppvData = new BYTE[*pdwSize];
    if (*ppvData == NULL) {

        return E_OUTOFMEMORY;

    }

    SetFilePointer(hFile,0,0,FILE_BEGIN);

    BOOL bSuccess = ReadFile(hFile,
        *ppvData,
        *pdwSize,
        &dwBytesRead,
        NULL);

    if (!bSuccess) {

        DbgPrint("Failed to read %s\n",pszName);
        return E_FAIL;

    }

    return S_OK;
}

VOID CreateRandomEvent(PXACT_TRACK_EVENT pEvent)
{

    FLOAT fTemp;    
    
    //
    // normalize rand value
    //
    
    fTemp = (FLOAT)rand()/(FLOAT)RAND_MAX;
    
    //
    // create an arbitrary event
    //
    
    switch (pEvent->Header.wType) {
    case eXACTEvent_Play:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_PLAY);
        break;
    case eXACTEvent_PlayWithPitchAndVolumeVariation:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_PLAY_WITH_PITCH_VOLUME_VAR);
        pEvent->EventData.PlayWithPitchAndVolumeVariation.PlayDesc.WaveSource.wWaveIndex = 1;
        pEvent->EventData.PlayWithPitchAndVolumeVariation.PlayDesc.WaveSource.wBankIndex = 0;
        pEvent->EventData.PlayWithPitchAndVolumeVariation.VarDesc.Pitch.sPitchHi=8;
        pEvent->EventData.PlayWithPitchAndVolumeVariation.VarDesc.Pitch.sPitchLo=-8;
        pEvent->EventData.PlayWithPitchAndVolumeVariation.VarDesc.Volume.sVolHi=0;
        pEvent->EventData.PlayWithPitchAndVolumeVariation.VarDesc.Volume.sVolLo=-2000;
        break;
        
    case eXACTEvent_Stop:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_STOP);
        break;
        
    case eXACTEvent_SetFrequency:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETFREQUENCY);
        fTemp = fTemp * 48000.0f;
        fTemp = max(DSBFREQUENCY_MIN,fTemp);
        pEvent->EventData.SetFrequency.wFrequency = (WORD)fTemp;
        break;
        
    case eXACTEvent_SetVolume:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETVOLUME);            
        fTemp = fTemp*(-2000.0f);
        pEvent->EventData.SetVolume.sVolume = (SHORT)fTemp;
        break;
        
    case eXACTEvent_SetHeadroom:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETHEADROOM);
        fTemp = fTemp*(2);
        pEvent->EventData.SetHeadroom.wHeadroom = (WORD)fTemp;
        break;
        
    case eXACTEvent_SetLFO:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETLFO);
        pEvent->EventData.SetLFO.Desc.dwLFO = DSLFO_PITCH;
        pEvent->EventData.SetLFO.Desc.dwDelay = 10;
        fTemp = fTemp*1000;
        pEvent->EventData.SetLFO.Desc.dwDelta = (DWORD)fTemp;
        pEvent->EventData.SetLFO.Desc.lPitchModulation = 0x1f;
        break;
    case eXACTEvent_SetEG:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETEG);
        pEvent->EventData.SetEG.Desc.dwEG = DSEG_AMPLITUDE;
        pEvent->EventData.SetEG.Desc.dwMode = DSEG_MODE_DELAY;
        pEvent->EventData.SetEG.Desc.dwDelay = 1;
        pEvent->EventData.SetEG.Desc.dwAttack = 100;
        pEvent->EventData.SetEG.Desc.dwHold = 0xa00;
        pEvent->EventData.SetEG.Desc.dwDecay = 100;
        pEvent->EventData.SetEG.Desc.dwSustain = 200;
        pEvent->EventData.SetEG.Desc.dwRelease = 1;
        
        break;
    case eXACTEvent_SetFilter:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETFILTER);
        pEvent->EventData.SetFilter.Desc.dwMode = DSFILTER_MODE_PARAMEQ;
        pEvent->EventData.SetFilter.Desc.dwQCoefficient = DSFILTER_MODE_PARAMEQ;
        
        //
        // set the filter for Fc=701Hz, Gain = 20dB, Q = 1
        //
        
        pEvent->EventData.SetFilter.Desc.adwCoefficients[0] = 0xbe1d;
        pEvent->EventData.SetFilter.Desc.adwCoefficients[1] = 0x8000;
        pEvent->EventData.SetFilter.Desc.adwCoefficients[2] = 0xc8dc;
        pEvent->EventData.SetFilter.Desc.adwCoefficients[3] = 0x1fff;
        
        break;
    case eXACTEvent_LoopStart:    
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_LOOPSTART);
        break;

    case eXACTEvent_LoopEnd:    
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_LOOPEND);
        break;
        
    default:
        break;
    }

}