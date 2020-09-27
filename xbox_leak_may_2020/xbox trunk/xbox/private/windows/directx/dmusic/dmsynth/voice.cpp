//      Voice.cpp
//      Copyright (c) 1996-1999 Microsoft Corporation
//

#ifdef DMSYNTH_MINIPORT
#include "common.h"
#include <math.h>
#include "muldiv32.h"
#else
#include "debug.h"
#include "simple.h"
#include <mmsystem.h>
#include <dmusicc.h>
#include <dmusics.h>
#include "synth.h"
#include <math.h>
#include <stdio.h>
#include "csynth.h"
#endif
#include "fparms.h" // Generated filter parameter arrays
#include "validate.h"
#ifdef _X86_
#define MMX_ENABLED 1
#endif

#ifdef DBG
extern DWORD sdwDebugLevel;
#endif

#ifdef XBOX
#include "..\dmime\cmixbins.h"
#endif

#ifdef XMIX

static REFERENCE_TIME STimeToSeqTime(STIME stTime){
    // stTime is in sample time - 48000 units per second
    // REFERENCE_TIME is in NT system time = 10,000,000 per second
    REFERENCE_TIME result = stTime;
    result = result * 10000000 / 48000;
    return result;
}

#endif

PFRACT CDigitalAudio::m_spfCents[201];
PFRACT CDigitalAudio::m_spfSemiTones[97];
VFRACT CDigitalAudio::m_svfDbToVolume[(MAXDB - MINDB) * 10 + 1];

void CDigitalAudio::Init()
{
    double flTemp;
    VREL    vrdB;

    for (vrdB = MINDB * 10;vrdB <= MAXDB * 10;vrdB++)
    {
        flTemp = vrdB;
        flTemp /= 100.0;
        flTemp = pow(10.0,flTemp);
        flTemp = pow(flTemp,0.5);   // square root.
        flTemp *= 4095.0; // 2^12th, but avoid overflow...
        m_svfDbToVolume[vrdB - (MINDB * 10)] = (long) flTemp;
    }

    PREL prRatio;

    for (prRatio = -100;prRatio <= 100;prRatio++)
    {
        flTemp = prRatio;
        flTemp /= 1200.0;
        flTemp = pow(2.0,flTemp);
        flTemp *= 4096.0;
        m_spfCents[prRatio + 100] = (long) flTemp;
    }
    
    for (prRatio = -48;prRatio <= 48;prRatio++)
    {
        flTemp = prRatio;
        flTemp /= 12.0;
        flTemp = pow(2.0,flTemp);
        flTemp *= 4096.0;
        m_spfSemiTones[prRatio + 48] = (long) flTemp;
    }
}

VFRACT CDigitalAudio::VRELToVFRACT(VREL vrVolume)
{
    vrVolume /= 10;

    if (vrVolume < MINDB * 10) 
        vrVolume = MINDB * 10;
    else if (vrVolume >= MAXDB * 10) 
        vrVolume = MAXDB * 10;

    return (m_svfDbToVolume[vrVolume - MINDB * 10]);
}

PFRACT CDigitalAudio::PRELToPFRACT(PREL prPitch)
{
    PFRACT pfPitch = 0;
    PREL prOctave;
    if (prPitch > 100)
    {
        prOctave = prPitch / 100;
        prPitch = prPitch % 100;
        pfPitch = m_spfCents[prPitch + 100];
        pfPitch <<= prOctave / 12;
        prOctave = prOctave % 12;
        pfPitch *= m_spfSemiTones[prOctave + 48];
        pfPitch >>= 12;
    }
    else if (prPitch < -100)
    {
        prOctave = prPitch / 100;
        prPitch = (-prPitch) % 100;
        pfPitch = m_spfCents[100 - prPitch];
        pfPitch >>= ((-prOctave) / 12);
        prOctave = (-prOctave) % 12;
        pfPitch *= m_spfSemiTones[48 - prOctave];
        pfPitch >>= 12;
    }
    else
    {
        pfPitch = m_spfCents[prPitch + 100];
    }
    return (pfPitch);
}


CVoice::CVoice()
{
    m_pBuffer = NULL;
    m_pDLSWave = NULL;
    m_pWave = NULL;
    memset(&m_WaveFormat, 0, sizeof(m_WaveFormat));
    m_pStream = NULL;
    memset(m_dwPacketIndex,0,sizeof(m_dwPacketIndex));
    memset(m_pvBuffer,0,sizeof(m_pvBuffer));
    m_dwBufferSize = 0;
    m_wCurrentPacket = 0;
    m_wPacketsSubmitted = 0;
    m_pControl = NULL;
    m_dwPriority = 0;
    m_nPart = 0;
    m_nKey = 0;
    m_fInUse = FALSE;
    m_fSustainOn = FALSE;
    m_fNoteOn = FALSE;
    m_fTag = FALSE;
    m_rtStartTime = 0;
    m_rtStopTime = 0x7fffffffffffffff;
    m_fAllowOverlap = FALSE;
    m_pRegion = NULL;
    m_dwLoopType = 0;
}


CVoice::~CVoice()
{
    ClearVoice();
}

void CVoice::Init()
{
    static BOOL fBeenHereBefore = FALSE;
    if (fBeenHereBefore) return;
    fBeenHereBefore = TRUE;
    CDigitalAudio::Init();
}

LPDIRECTSOUNDSEQUENCER CVoice::GetSequencer()
{
    return m_pSynth->GetSequencer();
}

void CVoice::StopVoice(REFERENCE_TIME rtTime)
{
    if (m_fNoteOn)
    {
        m_dwGroup = 0;
        m_fNoteOn = FALSE;
        m_fSustainOn = FALSE;
        m_rtStopTime = rtTime;
        if(m_pBuffer)
        {
            DWORD dwFlags = DSBSTOPEX_ENVELOPE;
            if (m_dwLoopType == WLOOP_TYPE_RELEASE)
            {
                dwFlags |= DSBSTOPEX_RELEASEWAVEFORM;
            }
            HRESULT hr = m_pBuffer->StopEx(rtTime,dwFlags);
            if(FAILED(hr) && rtTime)
            { 
                // If we ran out of memory, try to stop immediately to avoid a stuck-on note.
                m_pBuffer->StopEx(0,dwFlags);
            }
        }
        else 
        {
            StopStream(rtTime);
        }
    }
}

void CVoice::QuickStopVoice(REFERENCE_TIME rtTime)
{
    m_fSustainOn = FALSE;
    m_rtStopTime = rtTime;
    m_dwGroup = 0;
    if (m_pBuffer)
    {
        // Only kill the sound if it is on. We currently get a click if we try to call StopEx() a second time.
        if (m_fNoteOn) 
        {
            m_pBuffer->ClearAtOrAfter(rtTime);
            // Wave already has a release time of 1.
            if (!m_fWave) 
            {
                m_Articulation.m_VolumeEG.m_Registers.dwRelease = 1;
                m_pBuffer->SetEG(rtTime, &m_Articulation.m_VolumeEG.m_Registers);
            }
            if (FAILED(m_pBuffer->StopEx(rtTime,DSBSTOPEX_ENVELOPE)))
            {
                m_pBuffer->StopEx(0,DSBSTOPEX_ENVELOPE);
            }
        }
        else
        // If the sound is already in the release phase, hack to fade it out by hand.
        {
            if (!m_fTag)
            {
                m_pBuffer->ClearAtOrAfter(rtTime);
            }
            for (DWORD dwI = 0; dwI < 4; dwI++)
            {
                m_vrStartVolume -= 1200; // Drop the volume 12dB.
                SendVolume(rtTime);
                rtTime += 10000;    // Increment forward 1 ms. 
            }
        }
    }
    else
    {
        StopStream(rtTime);
    }
    m_fTag = TRUE;
    m_fNoteOn = FALSE;
}

void CVoice::StopStream(REFERENCE_TIME rtTime)
{
    ClearVoice();
//    if(m_pStream)
//    {
        // Unfortunately, we need to wait until after the Flush is processed so we can
        // release our buffer data. So we ignore the rtTime argument and stop immediately.

//        m_pStream->Clear();  // Remove any queued commands
//        m_pStream->Flush(0); // Stop audio, set status of pending packets to Flushed.
        DirectSoundDoWork(); // So the packet status gets updated.

//        ProcessCompletedPackets(TRUE); // Releases packets.

//        m_pStream->Release();

//        m_pStream = NULL;
//        m_fInUse = false;
//    }
}


HRESULT CVoice::AllocateBuffer(XBOXADPCMWAVEFORMAT* pwfxFormat)
{
    HRESULT hr = S_OK;

    if(m_pBuffer && !(pwfxFormat->wfx.cbSize == m_WaveFormat.wfx.cbSize
        && pwfxFormat->wfx.nAvgBytesPerSec == m_WaveFormat.wfx.nAvgBytesPerSec
        && pwfxFormat->wfx.nBlockAlign == m_WaveFormat.wfx.nBlockAlign
        && pwfxFormat->wfx.nChannels == m_WaveFormat.wfx.nChannels
        && pwfxFormat->wfx.nSamplesPerSec == m_WaveFormat.wfx.nSamplesPerSec
        && pwfxFormat->wfx.wBitsPerSample == m_WaveFormat.wfx.wBitsPerSample
        && pwfxFormat->wfx.wFormatTag == m_WaveFormat.wfx.wFormatTag))
    {
        hr = m_pBuffer->SetFormat(0, &pwfxFormat->wfx);

        if (SUCCEEDED(hr))
        {
            m_WaveFormat = *pwfxFormat;
        }
        else
        {
            DbgPrint("Failed to set buffer format!\n");

            m_pBuffer->Release();
            m_pBuffer = 0;
            memset(&m_WaveFormat, 0, sizeof(m_WaveFormat));
        }
    }
    if(!m_pBuffer)
    {
        m_WaveFormat = *pwfxFormat;
        DSBUFFERDESC dsbd;
        memset(&dsbd, 0, sizeof(dsbd));
        dsbd.lpwfxFormat = &m_WaveFormat.wfx;
        dsbd.dwFlags = DSBCAPS_CTRLFREQUENCY
            | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY;
        hr = GetSequencer()->CreateBuffer(&dsbd, &m_pBuffer);
        if (FAILED(hr))
        {
            DbgPrint("Failed buffer creation!\n");
        }
    }
    return hr;
}

// Convert from frequency ratio to PREL.

__inline long RatioToPREL(float flRatio)
{
    const float             fl4096  = 1200.0f;
    long                    lPitch;

    __asm 
    {
        fld     fl4096
        fld     flRatio
        fyl2x
        fistp   lPitch
    }

    return lPitch;
}

void CVoice::SendPitch(REFERENCE_TIME rtTime)

{
    PREL prPitch;
    m_pPart->GetPitch(&prPitch);
    prPitch += m_prStartPitch;
    if (prPitch != m_prLastPitch)
    {
        m_prLastPitch = prPitch;
        // Convert from PREL format (1200 per octave) to NVidia (4096 per octave.)
        prPitch = (prPitch * 1024) + 512;
        prPitch /= 300;

        if(prPitch < DSBPITCH_MIN){
            Trace(3,"Clamping pitch to DSBPITCH_MIN\n");
            prPitch = DSBPITCH_MIN;
        }
        if(prPitch > DSBPITCH_MAX){
            Trace(3,"Clamping pitch to DSBPITCH_MAX\n");
            prPitch = DSBPITCH_MAX;
        }

        if (m_fStreamed)
        {
            if (m_pStream)
            {
                m_pStream->SetPitch(rtTime,prPitch);
            }
        }
        else
        {
            if (m_pBuffer)
            {
                m_pBuffer->SetPitch(rtTime,prPitch);
            }
        }
    }
}

void CVoice::SendLFOs(REFERENCE_TIME rtTime)

{
    long lPressure, lModWheel;
    m_pPart->GetPressure((DWORD *) &lPressure);
    m_pPart->GetModWheel((DWORD *) &lModWheel);

    // First, take the control pressure to pitch parameter and scale by control pressure.
    VREL vrTemp = (lPressure * m_Articulation.m_LFO.m_prCPPitchScale) / 127;
    // Then, add mod wheel to pitch parameter and scale it.
    vrTemp += (lModWheel * m_Articulation.m_LFO.m_prMWPitchScale) / 127;
    // Then, add to the default pitch control.
    vrTemp += m_Articulation.m_LFO.m_prPitchScale;
    vrTemp <<= 16;      // Scale up to full 32 bit DLS2 file format.
    vrTemp /= 614400;   // Then convert to NVidia format.
    FORCEBOUNDS(vrTemp,-128,127);
    m_Articulation.m_LFO.m_Registers.lPitchModulation = vrTemp;

    // Now, do the same for LF02, which only does vibrato.
    vrTemp = (lPressure * m_Articulation.m_LFO2.m_prCPPitchScale) / 127;
    vrTemp += (lModWheel * m_Articulation.m_LFO2.m_prMWPitchScale) / 127;
    vrTemp += m_Articulation.m_LFO2.m_prPitchScale;
    vrTemp <<= 16;      // Scale up to full 32 bit DLS2 file format.
    vrTemp /= 614400;   // Then convert to NVidia format.
    FORCEBOUNDS(vrTemp,-128,127);
    m_Articulation.m_LFO2.m_Registers.lPitchModulation = vrTemp;

    // Do the same for volume on first LFO. 
    vrTemp = (lPressure * m_Articulation.m_LFO.m_vrCPVolumeScale) / 127;
    vrTemp += (lModWheel * m_Articulation.m_LFO.m_vrMWVolumeScale) / 127;
    vrTemp += m_Articulation.m_LFO.m_vrVolumeScale;
    vrTemp <<= 16;      // Scale up to full 32 bit DLS2 file format.
    vrTemp /= 38400;    // Then convert to NVidia format.
    FORCEBOUNDS(vrTemp,-128,127);
    m_Articulation.m_LFO.m_Registers.lAmplitudeModulation = vrTemp;

    // And, filter on the the first LFO.
    vrTemp = (lPressure * m_Articulation.m_LFO.m_prCPCutoffScale) / 127;
    vrTemp += (lModWheel * m_Articulation.m_LFO.m_prMWCutoffScale) / 127;
    vrTemp += m_Articulation.m_LFO.m_prCutoffScale;
    vrTemp <<= 16;      // Scale up to full 32 bit DLS2 file format.
    vrTemp /= 4915200;  // Then convert to NVidia format.
    FORCEBOUNDS(vrTemp,-128,127);
    m_Articulation.m_LFO.m_Registers.lFilterCutOffRange = vrTemp;

    if (m_pBuffer)
    {
        m_pBuffer->SetLFO(rtTime, &m_Articulation.m_LFO.m_Registers);
        m_pBuffer->SetLFO(rtTime, &m_Articulation.m_LFO2.m_Registers);
    }
    else if (m_pStream)
    {
        m_pStream->SetLFO(rtTime, &m_Articulation.m_LFO.m_Registers);
        m_pStream->SetLFO(rtTime, &m_Articulation.m_LFO2.m_Registers);
    }
}

unsigned short g_nFilter[128] = {
    0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
    0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
    0x8000, 0x8000, 0x8020, 0x819d, 0x8303, 0x8455, 0x8595, 0x8756,
    0x886f, 0x89fe, 0x8af9, 0x8c5f, 0x8e1e, 0x8f57, 0x9081, 0x91f9,
    0x935a, 0x94a7, 0x95e2, 0x9756, 0x98b3, 0x99fe, 0x9b36, 0x9c99,
    0x9e1e, 0x9f57, 0xa0b2, 0xa1f9, 0xa35a, 0xa4a7, 0xa608, 0xa756,
    0xa8b3, 0xa9fe, 0xab55, 0xacb6, 0xae1d, 0xaf57, 0xb0b1, 0xb20f,
    0xb36f, 0xb4bb, 0xb61b, 0xb767, 0xb8c4, 0xba0d, 0xbb63, 0xbcc3,
    0xbe1d, 0xbf70, 0xc0bd, 0xc21a, 0xc36e, 0xc4c4, 0xc61a, 0xc76f,
    0xc8c2, 0xca13, 0xcb69, 0xccc1, 0xce1a, 0xcf6d, 0xd0c0, 0xd216,
    0xd36a, 0xd4bf, 0xd615, 0xd769, 0xd8bc, 0xda10, 0xdb65, 0xdcbc,
    0xde11, 0xdf62, 0xe0b6, 0xe208, 0xe35d, 0xe4b0, 0xe601, 0xe753,
    0xe8a6, 0xe9f7, 0xeb48, 0xec99, 0xede9, 0xef36, 0xf085, 0xf1d2,
    0xf31f, 0xf46a, 0xf5b4, 0xf6fc, 0xf842, 0xf988, 0xfacb, 0xfc0c,
    0xfd4b, 0xfe85, 0xffbe, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
};

static unsigned short CalcCutOff(PREL prPitch)

{
    prPitch += 6400;
    if (prPitch < 0) prPitch = 0;
    if (prPitch > 12700) prPitch = 12700;
    long lIndex = prPitch / 100;
    long lTemp = g_nFilter[lIndex] + 
        ((prPitch % 100) * (g_nFilter[lIndex+1] - g_nFilter[lIndex])) / 100;
    return (short) lTemp;
}

unsigned short g_nResonance[23] = {
    0x8000, 0x7214, 0x65ac, 0x5a9d, 0x50c3, 0x47fa, 0x4026, 0x392c,
    0x32f5, 0x2d6a, 0x287a, 0x2413, 0x2026, 0x1ca7, 0x198a, 0x16c3,
    0x1449, 0x1214, 0x101d, 0xe5c, 0xccc, 0xb68, 0xa2a,
};


static unsigned short CalcResonance(DWORD dwQ)

{
    if (dwQ > 22) dwQ = 22;
    return g_nResonance[dwQ];
}

/* Code to generate the tables:

#include "stdafx.h"
#include "stdio.h"
#include "math.h"

long m_spfCents[201];
long m_spfSemiTones[97];

void Init()
{
    long prRatio;
    double flTemp;

    for (prRatio = -100;prRatio <= 100;prRatio++)
    {
        flTemp = prRatio;
        flTemp /= 1200.0;
        flTemp = pow(2.0,flTemp);
        flTemp *= 4096.0;
        m_spfCents[prRatio + 100] = (long) flTemp;
    }
    
    for (prRatio = -48;prRatio <= 48;prRatio++)
    {
        flTemp = prRatio;
        flTemp /= 12.0;
        flTemp = pow(2.0,flTemp);
        flTemp *= 4096.0;
        m_spfSemiTones[prRatio + 48] = (long) flTemp;
    }
}

long PRELToPFRACT(long prPitch)
{
    long pfPitch = 0;
    long prOctave;
    if (prPitch > 100)
    {
//        if (prPitch > 4800)
//        {
//            prPitch = 4800;
//        }
        prOctave = prPitch / 100;
        prPitch = prPitch % 100;
        pfPitch = m_spfCents[prPitch + 100];
        pfPitch <<= prOctave / 12;
        prOctave = prOctave % 12;
        pfPitch *= m_spfSemiTones[prOctave + 48];
        pfPitch >>= 12;
    }
    else if (prPitch < -100)
    {
//        if (prPitch < -4800)
//        {
//            prPitch = -4800;
//        }
        prOctave = prPitch / 100;
        prPitch = (-prPitch) % 100;
        pfPitch = m_spfCents[100 - prPitch];
        pfPitch >>= ((-prOctave) / 12);
        prOctave = (-prOctave) % 12;
        pfPitch *= m_spfSemiTones[48 - prOctave];
        pfPitch >>= 12;
    }
    else
    {
        pfPitch = m_spfCents[prPitch + 100];
    }
    return (pfPitch);
}


unsigned long FreqToHardwareCoeff( float fFreq )
{
    float fNormCutoff = fFreq / 48000.0f;

    // Filter is ineffective out of these ranges, so why
    // bother even trying?
    if( fFreq < 30.0f )
        return 0x8000;
    if( fFreq > 8000.0f )
        return 0xffff;

    double fFC = float( 2.0f * sin( 3.1415926535897932384626433832795 * fNormCutoff ) );
    fFC = 4096.0f * log( fFC ) / log( 2.0f );
    long lOctaves = (long) fFC;

    return (unsigned long)lOctaves & 0xFFFF;
}

unsigned long dBToHardwareCoeff( long lResonance )
{
    float fResonance = (float)lResonance;

    if( fResonance > 22.5f )
        fResonance = 22.5f;

    double fQ = pow( 10.0, -0.05*fResonance);
    unsigned int dwQ = (unsigned int)(fQ*(1<<15));
    if( dwQ > 0xFFFF )
        dwQ = 0xFFFF;

    return (unsigned long)dwQ;
}

void GenerateQ()

{
    printf("short g_nResonance[23] = {\n    ");
    long dwIndent = 0;
    float flResonance;
    for (flResonance = 0; flResonance < 23; flResonance++)
    {
        if (dwIndent == 8)
        {
            printf("\n    ");
            dwIndent = 0;
        }
        dwIndent++;
        unsigned int pQ = dBToHardwareCoeff((long)flResonance);
        printf("0x%lx, ",(long)pQ);
    }
    printf("\n};\n");
}

void GenerateCutoff()

{
    Init();
    printf("short g_nFilter[128] = {\n    ");
    unsigned long dwIndent = 0;
    long prFilter;
    for (prFilter = (-64 * 100); prFilter < (64 * 100); prFilter += 100)
    {
        if (dwIndent == 8)
        {
            printf("\n    ");
            dwIndent = 0;
        }
        dwIndent++;
        // Now, we need to convert to a real frequency. The PREL values are relative
        // to A440, so get the sample rate by multiplying this relative pitch against 440Hz.
        unsigned long dwSampleRate = PRELToPFRACT(prFilter); 
        dwSampleRate = (440 * dwSampleRate) >> 12;
        unsigned int pF, pQ;
        pF = FreqToHardwareCoeff((float) dwSampleRate);
        char output[100];
        printf("0x%x, ",(long)pF);
    }
    printf("\n};\n");
}




int main(int argc, char* argv[])
{
	GenerateCutoff();
    GenerateQ();
	return 0;
}

*/

void CVoice::SendFilter(REFERENCE_TIME rtTime,BOOL fStart)

{
    DSFILTERDESC FilterDesc;
    // If this is a wave or DLS instrument that doesn't use filter, turn off filter if this
    // is the start.
    if (m_fUseFilter)
    {
        PREL prFilter;
        m_pPart->GetFilter(&prFilter);
        prFilter += m_prStartFilter;
        if (abs(prFilter - m_prLastFilter) >= 50)
        {
            DSFILTERDESC FilterDesc;
            FilterDesc.dwMode = DSFILTER_MODE_DLS2;
            FilterDesc.adwCoefficients[0] = CalcCutOff(prFilter-6900);
            FilterDesc.adwCoefficients[1] = CalcResonance(m_wFilterQ);
            FilterDesc.adwCoefficients[2] = 0;
            FilterDesc.adwCoefficients[3] = 0;
            FilterDesc.dwQCoefficient = 0;
            m_prLastFilter = prFilter;
            if (m_pBuffer)
            {
                m_pBuffer->SetFilter(rtTime,&FilterDesc);
            }
            else if (m_pStream)
            {
                m_pStream->SetFilter(rtTime,&FilterDesc);
            }
        }
        else
        {
            if (fStart)
            {
                m_prLastFilter = 0;
                FilterDesc.dwMode = DSFILTER_MODE_BYPASS;
                FilterDesc.adwCoefficients[0] = 0;
                FilterDesc.adwCoefficients[1] = 0;
                FilterDesc.adwCoefficients[2] = 0;
                FilterDesc.adwCoefficients[3] = 0;
                FilterDesc.dwQCoefficient = 0;
                if (m_pBuffer)
                {
                    m_pBuffer->SetFilter(rtTime,&FilterDesc);
                }
                else if (m_pStream)
                {
                    m_pStream->SetFilter(rtTime,&FilterDesc);
                }
            }
        }
/*      Testing parametric eq...
        // Now, we need to convert to a real frequency. The PREL values are relative
        // to A440, so get the samplerate by multiplying this relative pitch against 440Hz.
        static double fSampleRate = 100;
        fSampleRate *= 1.1;
        if (fSampleRate > 14000) fSampleRate = 50;
        FilterDesc.dwMode = DSFILTER_MODE_PARAMEQ;
        unsigned int pF, pQ;
        DlsToChamberlin(fSampleRate,10,&pF,&pQ);
        FilterDesc.adwCoefficients[0] = 1;
        FilterDesc.adwCoefficients[1] = 1;
        FilterDesc.adwCoefficients[2] = pF;
        FilterDesc.adwCoefficients[3] = pQ;
        FilterDesc.dwQCoefficient = 3;
        m_pBuffer->SetFilter(rtTime,&FilterDesc);*/

    }
}

void CVoice::SendVolume(REFERENCE_TIME rtTime)

{
    BOOL fBuffer = (m_pBuffer && !m_fStreamed);
    if (fBuffer || (m_fStreamed && m_pStream))
    {
        if (m_BusIds.HasBuffer())
        {
            // If we are sending to a buffer, then we just call SetVolume since there are no mixbins.
            VREL vrVolume;
            m_pPart->GetVolume(m_vrStartVolume,0,&vrVolume,NULL,NULL,NULL,NULL);
            CLAMP_VOLUME(vrVolume);
            if (abs(vrVolume - m_vrLastVolumes[0]) >= 10)
            {
                if (fBuffer)
                {
                    m_pBuffer->SetVolume(rtTime,vrVolume);
                }
                else 
                {
                    m_pStream->SetVolume(rtTime,vrVolume);
                }
                m_vrLastVolumes[0] = vrVolume;
            }
        }
        else if (m_BusIds.m_bControllers[0]) // Is this a multi-mixbin buffer?
        { 
            VREL vrVolumes[8];
            BOOL fChanged = false;
            m_pPart->GetVolume(m_vrStartVolume,m_BusIds.m_bControllers,vrVolumes);
            for (DWORD dwX = 0; m_BusIds.m_bControllers[dwX];dwX++)
            {
                CLAMP_VOLUME(vrVolumes[dwX]);
                if (abs(vrVolumes[dwX] - m_vrLastVolumes[dwX]) >= 10)
                {
                    m_vrLastVolumes[dwX] = vrVolumes[dwX];
                    fChanged = true;
                }
            }
            if (fChanged)
            {
                CMIXBINS MixBins;
                MixBins.CreateFromMask(m_BusIds.m_dwMixBins);
                MixBins.PokeMixBinVolumesBasedOnMask(m_BusIds.m_dwMixBins, (long *)vrVolumes);
                if (fBuffer)
                {
                    m_pBuffer->SetMixBinVolumes(rtTime,MixBins.GetMixBins());
                }
                else 
                {
                    m_pStream->SetMixBinVolumes(rtTime,MixBins.GetMixBins());
                }
            }
        } 
        else if ((m_BusIds.m_dwMixBins & ((1 << DSMIXBIN_FRONT_LEFT) | (1 << DSMIXBIN_FRONT_RIGHT))) == 
                ((1 << DSMIXBIN_FRONT_LEFT) | (1 << DSMIXBIN_FRONT_RIGHT)))
        {
            VREL vrVolumes[4];
            m_pPart->GetVolume(m_vrStartVolume,m_lDefaultPan,NULL,&vrVolumes[0],&vrVolumes[1],&vrVolumes[2],&vrVolumes[3]);
            CLAMP_VOLUME(vrVolumes[0]);
            CLAMP_VOLUME(vrVolumes[1]);
            CLAMP_VOLUME(vrVolumes[2]);
            CLAMP_VOLUME(vrVolumes[3]);
            if ((abs(vrVolumes[0] - m_vrLastVolumes[0]) >= 10) ||
                (abs(vrVolumes[1] - m_vrLastVolumes[1]) >= 10) ||
                (abs(vrVolumes[2] - m_vrLastVolumes[2]) >= 10) ||
                (abs(vrVolumes[3] - m_vrLastVolumes[3]) >= 10))
            {
                CMIXBINS MixBins;
                MixBins.CreateFromMask(m_BusIds.m_dwMixBins);
                MixBins.PokeMixBinVolumesBasedOnMask(m_BusIds.m_dwMixBins, vrVolumes);                
                if (fBuffer)
                {
                    m_pBuffer->SetMixBinVolumes(rtTime,MixBins.GetMixBins());
                }
                else 
                {
                    m_pStream->SetMixBinVolumes(rtTime,MixBins.GetMixBins());
                }
                m_vrLastVolumes[0] = vrVolumes[0];
                m_vrLastVolumes[1] = vrVolumes[1];
                m_vrLastVolumes[2] = vrVolumes[2];
                m_vrLastVolumes[3] = vrVolumes[3];
            }
        }   
        // Then, the case of the one side of a stereo wave
        else 
        {
            VREL vrVolumes[3];
            m_pPart->GetVolume(m_vrStartVolume,m_lDefaultPan,&vrVolumes[0],NULL,NULL,&vrVolumes[1],&vrVolumes[2]);
            CLAMP_VOLUME(vrVolumes[0]);
            CLAMP_VOLUME(vrVolumes[1]);
            CLAMP_VOLUME(vrVolumes[2]);
            if ((abs(vrVolumes[0] - m_vrLastVolumes[0]) >= 10) ||
                (abs(vrVolumes[1] - m_vrLastVolumes[1]) >= 10) ||
                (abs(vrVolumes[2] - m_vrLastVolumes[2]) >= 10))
            {
                CMIXBINS MixBins;
                MixBins.CreateFromMask(m_BusIds.m_dwMixBins);
                MixBins.PokeMixBinVolumesBasedOnMask(m_BusIds.m_dwMixBins, (long *)vrVolumes);
                if (fBuffer)
                {
                    m_pBuffer->SetMixBinVolumes(rtTime,MixBins.GetMixBins());
                }
                else
                {
                    m_pStream->SetMixBinVolumes(rtTime,MixBins.GetMixBins());
                }
                m_vrLastVolumes[0] = vrVolumes[0];
                m_vrLastVolumes[1] = vrVolumes[1];
                m_vrLastVolumes[2] = vrVolumes[2];
            }
        }      
    }
}

BOOL CVoice::StartVoice(CSynth *pSynth,
                        CSourceRegion *pRegion, REFERENCE_TIME rtStartTime,
                        CBusIds * pBusIds,
                        WORD nKey,WORD nVelocity,
                        VREL vrVolume,
                        VREL vrMIDIVolume,
                        PREL prPitch,
                        PREL prMIDIPitch)
{
    m_pSynth = pSynth;  
    m_fWave = FALSE;
    m_fStreamed = FALSE;
    m_BusIds = *pBusIds;
    
    memset(&m_vrLastVolumes[0],0,sizeof(VREL)*8);
    m_prLastPitch = m_prLastFilter = 0;
    if (pRegion->m_pArticulation == NULL)
    {
        return FALSE;
    }

    // If this is multichannel, that means this is a component of a larger wave. 
    // So, force it to play on the appropriate channel.
    if (!m_BusIds.HasBuffer() && pRegion->IsMultiChannel())
    {
        if (pRegion->m_dwChannel & WAVELINK_CHANNEL_RIGHT)
        {
            // Right side.
            m_BusIds.m_dwMixBins &= ~(1 << DSMIXBIN_FRONT_LEFT);
        }
        else
        {
            // Left side.
            m_BusIds.m_dwMixBins &= ~(1 << DSMIXBIN_FRONT_RIGHT);
        }
    }


    m_Articulation = *pRegion->m_pArticulation;
    m_dwLoopType = pRegion->m_Sample.m_dwLoopType;

    m_dwGroup = pRegion->m_bGroup;
    m_fAllowOverlap = pRegion->m_bAllowOverlap;

    m_vrStartVolume = VelocityToVolume(nVelocity);

    m_vrStartVolume += pRegion->m_vrAttenuation;

    m_lDefaultPan = m_Articulation.m_sDefaultPan;
  
    // Now, scale the attack, hold, and decay times as appropriate by the velocity and midi note.
    m_Articulation.m_VolumeEG.m_Registers.dwAttack *= 
        CDigitalAudio::PRELToPFRACT(nVelocity * m_Articulation.m_VolumeEG.m_trVelAttackScale / 127);
    m_Articulation.m_VolumeEG.m_Registers.dwAttack /= 4096;
    FORCEBOUNDS(m_Articulation.m_VolumeEG.m_Registers.dwAttack,0,3750);

    m_Articulation.m_PitchEG.m_Registers.dwAttack *= 
        CDigitalAudio::PRELToPFRACT(nVelocity * m_Articulation.m_PitchEG.m_trVelAttackScale / 127);
    m_Articulation.m_PitchEG.m_Registers.dwAttack /= 4096;
    FORCEBOUNDS(m_Articulation.m_PitchEG.m_Registers.dwAttack,0,3750);

    m_Articulation.m_VolumeEG.m_Registers.dwHold  *= 
        CDigitalAudio::PRELToPFRACT(nKey * m_Articulation.m_VolumeEG.m_trKeyDecayScale / 127);
    m_Articulation.m_VolumeEG.m_Registers.dwHold  /= 4096;
    FORCEBOUNDS(m_Articulation.m_VolumeEG.m_Registers.dwHold,0,3750);

    m_Articulation.m_PitchEG.m_Registers.dwHold  *= 
        CDigitalAudio::PRELToPFRACT(nKey * m_Articulation.m_PitchEG.m_trKeyDecayScale / 127);
    m_Articulation.m_PitchEG.m_Registers.dwHold  /= 4096;
    FORCEBOUNDS(m_Articulation.m_PitchEG.m_Registers.dwHold,0,3750);

    m_Articulation.m_VolumeEG.m_Registers.dwDecay *= 
        CDigitalAudio::PRELToPFRACT(nKey * m_Articulation.m_VolumeEG.m_trKeyDecayScale / 127);
    m_Articulation.m_VolumeEG.m_Registers.dwDecay /= 4096;
    FORCEBOUNDS(m_Articulation.m_VolumeEG.m_Registers.dwDecay,0,3750);

    m_Articulation.m_PitchEG.m_Registers.dwDecay *= 
        CDigitalAudio::PRELToPFRACT(nKey * m_Articulation.m_PitchEG.m_trKeyDecayScale / 127);
    m_Articulation.m_PitchEG.m_Registers.dwDecay /= 4096;
    FORCEBOUNDS(m_Articulation.m_PitchEG.m_Registers.dwDecay,0,3750);

    // Make sure we have a pointer to the wave ready:
    if ((pRegion->m_Sample.m_pWave == NULL) || (pRegion->m_Sample.m_pWave->m_pnWave == NULL))
    {
        return (FALSE);     // Do nothing if no sample.
    }

    m_fNoteOn = TRUE;
    m_fTag = FALSE;
    m_fSustainOn = FALSE;
    m_rtStartTime = rtStartTime;
    m_rtStopTime = 0x7fffffffffffffff;
//    m_DigitalAudio.m_Source = pRegion->m_Sample;

    // save a reference to this wave so we can release our reference when the voice
    // stops playing
    pRegion->m_Sample.m_pWave->AddRef();

    //
    // if the dls wave we are about to associate with the voice
    // is different than the existing one, release our reference so we dont leak
    //

    if (m_pDLSWave && (pRegion->m_Sample.m_pWave != m_pDLSWave)) {
        m_pDLSWave->Release();
    }

    m_pDLSWave = pRegion->m_Sample.m_pWave;

//    m_pCurrentBuffer = 0;
//    m_DigitalAudio.m_pWaveArt = NULL;
    XBOXADPCMWAVEFORMAT *pWaveFormat = &pRegion->m_Sample.m_pWave->m_WaveFormat;

    if (SUCCEEDED(AllocateBuffer(pWaveFormat)))
    {
        DWORD dwSamplesPerBlock = 1;
        if (m_WaveFormat.wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
        {
            dwSamplesPerBlock = 64;
        }
        // Now that we have a buffer, set the output.
        if (pBusIds->HasBuffer())
        {
            IDirectSoundBuffer* pBuffer = pBusIds->GetBuffer();
            m_pBuffer->SetOutputBuffer(0,pBuffer);
            pBuffer->Release();
        }
        else
        {
    //        Trace(-1,"%ld: Setting Mix Bins %lx on buffer %lx\n",(long) rtStartTime,pBusIds->m_dwMixBins,m_pBuffer);
            CMIXBINS MixBins;
            MixBins.CreateFromMask(m_BusIds.m_dwMixBins);
            m_pBuffer->SetMixBins(0,MixBins.GetMixBins());
        }

        // Pitch to frequency

        prPitch += pRegion->m_prTuning;
        CSourceSample* pSample = &pRegion->m_Sample;
        long lKey = (long) nKey;
        prPitch += pSample->m_prFineTune;
        prPitch += ((lKey - pSample->m_bMIDIRootKey) * 100);
        prPitch += RatioToPREL((float)pWaveFormat->wfx.nSamplesPerSec / (float) 48000);
        m_prStartPitch = prPitch;   // Store this as the base pitch.

        SendPitch(0);
        SendVolume(0);

        m_fUseFilter = (m_Articulation.m_Filter.m_prCutoff != 0x7FFF);
        m_prStartFilter = m_Articulation.m_Filter.m_prCutoff + 
            ((nVelocity * m_Articulation.m_Filter.m_prVelScale) / 127) + 
            ((nKey * m_Articulation.m_Filter.m_prKeyScale) / 127);
        m_wFilterQ = m_Articulation.m_Filter.m_vrQ / 10;
        SendFilter(0,TRUE);
        
        m_pBuffer->SetBufferData(0, pRegion->m_Sample.m_pWave->m_pnWave, pRegion->m_Sample.m_pWave->m_dwSampleDataSize);

        m_pBuffer->SetEG(0, &m_Articulation.m_VolumeEG.m_Registers);
        m_pBuffer->SetEG(0, &m_Articulation.m_PitchEG.m_Registers);

        SendLFOs(0);

        DWORD dwflag = 0;
        if(!pRegion->m_Sample.m_bOneShot)
        {
            // If there are loop points, convert to byte positions. 
            DWORD dwByteStart = (pRegion->m_Sample.m_dwLoopStart * pWaveFormat->wfx.nBlockAlign) / dwSamplesPerBlock;
            DWORD dwByteEnd = (pRegion->m_Sample.m_dwLoopEnd * pWaveFormat->wfx.nBlockAlign) / dwSamplesPerBlock;
            if(dwByteEnd > pRegion->m_Sample.m_pWave->m_dwSampleDataSize)
            {
                dwByteEnd = pRegion->m_Sample.m_pWave->m_dwSampleDataSize;
            }
            if (dwByteEnd > dwByteStart)
            {
                m_pBuffer->SetLoopRegion(0, dwByteStart, dwByteEnd - dwByteStart);
                dwflag = DSBPLAY_LOOPING;
            }
        }
    

        m_pBuffer->Play(rtStartTime, 0, 0, dwflag);
        return (TRUE);
    }
    return (FALSE);
}


BOOL CVoice::StartWave(CSynth *pSynth,
                       CSourceWave *pWave,
                       DWORD dwVoiceId,
                       REFERENCE_TIME rtStartTime,
                       CBusIds * pBusIds,
                       VREL vrVolume,
                       VREL vrMIDIVolume,
                       PREL prPitch,
                       PREL prMIDIPitch,
                       DWORD dwVoiceStart,
                       DWORD dwLoopStart,
                       DWORD dwLoopEnd
                       )
{
    m_pSynth = pSynth;
    m_fWave = TRUE;
    m_fStreamed = FALSE;
    m_BusIds = *pBusIds;
    memset(&m_vrLastVolumes[0],0,sizeof(VREL)*8);
    m_prLastPitch = m_prLastFilter = 0;
    m_Articulation = pWave->m_Articulation;

    // If this is multichannel, that means this is a component of a larger wave. 
    // So, force it to play on the appropriate channel.
/*    if (!m_BusIds.HasBuffer() && pWaveArt->m_WaveArtDl.usOptions & F_WAVELINK_MULTICHANNEL)
    {
        if (pWaveArt->m_WaveArtDl.ulBus == 1)
        {
            // Right side.
            m_BusIds.m_dwMixBins &= ~(1 << DSMIXBIN_FRONT_LEFT);
        }
        else
        {
            // Left side.
            m_BusIds.m_dwMixBins &= ~(1 << DSMIXBIN_FRONT_RIGHT);
        }
    }*/

    m_fNoteOn = TRUE;
    m_fTag = FALSE;
    m_rtStartTime = rtStartTime;
    m_rtStopTime = 0x7fffffffffffffff; 
    m_dwGroup = 0;
    m_lDefaultPan = 0;
    m_fAllowOverlap = FALSE;    
    m_fSustainOn = FALSE;
    m_dwVoiceId = dwVoiceId;

    pWave->m_bActive = TRUE;
    pWave->AddRef();
    m_pWave = pWave;

    if (SUCCEEDED(AllocateBuffer(&pWave->m_WaveFormat)))
    {

        // Now that we have a buffer, set the output.
        if (pBusIds->HasBuffer())
        {
            IDirectSoundBuffer* pBuffer = pBusIds->GetBuffer();
            m_pBuffer->SetOutputBuffer(0,pBuffer);
            pBuffer->Release();
        }
        else
        {
            CMIXBINS MixBins;
            MixBins.CreateFromMask(m_BusIds.m_dwMixBins);
            m_pBuffer->SetMixBins(0,MixBins.GetMixBins());

        }

        prPitch += RatioToPREL((float)pWave->m_WaveFormat.wfx.nSamplesPerSec / (float) 48000);
        m_prStartPitch = prPitch;   // Store this as the base pitch.
        SendPitch(0);
        m_vrStartVolume = vrVolume;
        SendVolume(0);
        DWORD dwSampleLength = pWave->m_dwSampleCount;
        DWORD dwSamplesPerBlock = 1;
        if (m_WaveFormat.wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
        {
            dwSamplesPerBlock = 64;
        }
        dwVoiceStart = (dwVoiceStart * m_WaveFormat.wfx.nBlockAlign) / dwSamplesPerBlock;
        dwSampleLength = (dwSampleLength * m_WaveFormat.wfx.nBlockAlign) / dwSamplesPerBlock;
        dwSampleLength -= dwVoiceStart;
        dwLoopEnd = (dwLoopEnd * m_WaveFormat.wfx.nBlockAlign) / dwSamplesPerBlock;
        dwLoopStart = (dwLoopStart * m_WaveFormat.wfx.nBlockAlign) / dwSamplesPerBlock;

        m_pBuffer->SetBufferData(0, &pWave->m_pnWave[dwVoiceStart/2], dwSampleLength);

        m_fUseFilter = (m_Articulation.m_Filter.m_prCutoff != 0x7FFF);
        m_prStartFilter = m_Articulation.m_Filter.m_prCutoff;
        m_wFilterQ = m_Articulation.m_Filter.m_vrQ / 10;
        SendFilter(0,TRUE);
        
        m_pBuffer->SetEG(0, &m_Articulation.m_VolumeEG.m_Registers);
        m_pBuffer->SetEG(0, &m_Articulation.m_PitchEG.m_Registers);

        SendLFOs(0);

        DWORD dwflag = 0;
        if ((dwLoopEnd > dwVoiceStart) && (dwLoopStart >= dwVoiceStart))
        {
            dwLoopEnd -= dwVoiceStart;
            dwLoopStart -= dwVoiceStart;
            if(dwLoopEnd > dwSampleLength)
            {
                dwLoopEnd = dwSampleLength;
            }
            if (dwLoopEnd > dwLoopStart)
            {
                m_pBuffer->SetLoopRegion(rtStartTime, dwLoopStart, (dwLoopEnd - dwLoopStart));
                dwflag = DSBPLAY_LOOPING;
            }
        }
        m_pBuffer->Play(rtStartTime, 0, 0, dwflag);
        return (TRUE);
    }
    return (FALSE);
}

BOOL CVoice::StartStreamedWave(CSynth *pSynth,
                       CSourceWave *pWave,
                       DWORD dwVoiceId,
                       REFERENCE_TIME rtStartTime,
                       CBusIds * pBusIds,
                       VREL vrVolume, VREL vrMIDIVolume,
                       PREL prPitch, PREL prMIDIPitch,
                       DWORD dwVoiceStart,
                       DWORD dwLoopStart,
                       DWORD dwLoopEnd)
{
    m_pSynth = pSynth;
    m_fWave = TRUE;
    m_fStreamed = TRUE;
    m_BusIds = *pBusIds;
    memset(&m_vrLastVolumes[0],0,sizeof(VREL)*8);
    m_prLastPitch = m_prLastFilter = 0;
    m_Articulation = pWave->m_Articulation;

    // If this is multichannel, that means this is a component of a larger wave. 
    // So, force it to play on the appropriate channel.
/*    if (!m_BusIds.HasBuffer() && pWaveArt->m_WaveArtDl.usOptions & F_WAVELINK_MULTICHANNEL)
    {
        if (pWaveArt->m_WaveArtDl.ulBus == 1)
        {
            // Right side.
            m_BusIds.m_dwMixBins &= ~(1 << DSMIXBIN_FRONT_LEFT);
        }
        else
        {
            // Left side.
            m_BusIds.m_dwMixBins &= ~(1 << DSMIXBIN_FRONT_RIGHT);
        }
    }*/


    // Initialize an envelope for wave playing 
    //

    m_fNoteOn = TRUE;
    m_fTag = FALSE;
    m_rtStartTime = rtStartTime;
    m_rtStopTime = 0x7fffffffffffffff; 
    m_dwGroup = 0;
    m_lDefaultPan = 0;
    m_fAllowOverlap = FALSE;    
    m_fSustainOn = FALSE;
    m_dwVoiceId = dwVoiceId;

    if(m_pStream)
    {
        m_pStream->Release();
        m_pStream = 0;
    }
    
    DSSTREAMDESC dssd;

    ZeroMemory(&dssd, sizeof(dssd));
    
    dssd.dwFlags = DSSTREAMCAPS_CTRLFREQUENCY | DSSTREAMCAPS_CTRLVOLUME | DSSTREAMCAPS_ACCURATENOTIFY;
    dssd.dwMaxAttachedPackets = VOICE_NUM_PACKETS;
    dssd.lpwfxFormat = (LPWAVEFORMATEX) &pWave->m_WaveFormat;
    m_WaveFormat = pWave->m_WaveFormat;


    if (FAILED(GetSequencer()->CreateStream(&dssd, &m_pStream)))
    {
        return FALSE;
    }

    if (pBusIds->HasBuffer())
    {
        IDirectSoundBuffer* pBuffer = pBusIds->GetBuffer();
        m_pStream->SetOutputBuffer(0,pBuffer);
        pBuffer->Release();
    }
    else
    {
        CMIXBINS MixBins;
        MixBins.CreateFromMask(m_BusIds.m_dwMixBins);
        m_pStream->SetMixBins(0,MixBins.GetMixBins());
    }

    prPitch += RatioToPREL((float)pWave->m_WaveFormat.wfx.nSamplesPerSec / (float) 48000);
    m_prStartPitch = prPitch;   // Store this as the base pitch.
    SendPitch(0);

    m_vrStartVolume = vrVolume;
    SendVolume(0);

    m_fUseFilter = (m_Articulation.m_Filter.m_prCutoff != 0x7FFF);
    m_prStartFilter = m_Articulation.m_Filter.m_prCutoff;
    m_wFilterQ = m_Articulation.m_Filter.m_vrQ / 10;
    SendFilter(0,TRUE);
    
    m_pStream->SetEG(0, &m_Articulation.m_VolumeEG.m_Registers);
    m_pStream->SetEG(0, &m_Articulation.m_PitchEG.m_Registers);

    SendLFOs(0);

    DWORD dwSamplesPerBlock = 1;
    if (m_WaveFormat.wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
    {
        dwSamplesPerBlock = 64;
    }
    
    m_dwStreamPosition = (dwVoiceStart * m_WaveFormat.wfx.nBlockAlign) / dwSamplesPerBlock;
    m_dwLoopEnd = (dwLoopEnd * m_WaveFormat.wfx.nBlockAlign) / dwSamplesPerBlock;
    m_dwLoopStart = (dwLoopStart * m_WaveFormat.wfx.nBlockAlign) / dwSamplesPerBlock;
    
    // Allocate the buffers.
    m_dwBufferSize = (DWORD)(pWave->m_rtReadAheadTime / 10000);
    if (m_dwBufferSize < 50) m_dwBufferSize = 50;
    m_dwBufferSize *= m_WaveFormat.wfx.nSamplesPerSec;
    m_dwBufferSize += 500;
    m_dwBufferSize /= 1000;
    if (m_WaveFormat.wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
    {
        m_dwBufferSize += 32;
        m_dwBufferSize /= 64;
    }
    m_dwBufferSize *= m_WaveFormat.wfx.nBlockAlign;
    DWORD dwPacket = 0;
    for (; dwPacket < VOICE_NUM_PACKETS; dwPacket++)
    {
        if ((m_WaveFormat.wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM) ||
            (m_WaveFormat.wfx.nChannels > 2))

        {        
            m_pvBuffer[dwPacket] = DirectMusicPhysicalAllocI(m_dwBufferSize);
        }
        else
        {
            m_pvBuffer[dwPacket] = new BYTE[m_dwBufferSize];
        }
        assert(m_pvBuffer[dwPacket]);
        if(!m_pvBuffer[dwPacket])
        {
            return false; // Out of physical memory. Eep!
        }
    }

    m_pWave = pWave;
    pWave->AddRef();
    m_wPacketsSubmitted = 0;
    m_wCurrentPacket = 0;
    memset(m_dwPacketIndex,0,sizeof(m_dwPacketIndex));

    for (; m_wCurrentPacket < VOICE_NUM_PACKETS; )
    {
        if (FAILED(WritePacket(rtStartTime,m_wCurrentPacket++)))
        {
            break;
        }
    }
    m_wCurrentPacket %= VOICE_NUM_PACKETS;
    return (TRUE);
}

HRESULT CVoice::WritePacket(REFERENCE_TIME rtTime,DWORD dwPacket)

{
    HRESULT hr = E_FAIL;
    if (m_dwStreamPosition < m_pWave->m_dwSampleDataSize)
    {
        DWORD dwPacketSize = m_dwBufferSize;
        if (m_dwLoopEnd && ((m_dwStreamPosition + dwPacketSize) > m_dwLoopEnd))
        {
            dwPacketSize = m_dwLoopEnd - m_dwStreamPosition;
            hr = m_pWave->Read(m_dwStreamPosition,(BYTE*) m_pvBuffer[dwPacket],dwPacketSize);
            m_dwStreamPosition = m_dwLoopStart;
        }
        else 
        {
            if ((m_dwStreamPosition + dwPacketSize) > m_pWave->m_dwSampleDataSize)
            {
                // If this is longer than the rest of the wave, read just what's needed.
                dwPacketSize = m_pWave->m_dwSampleDataSize - m_dwStreamPosition;
            }
            hr = m_pWave->Read(m_dwStreamPosition,(BYTE*) m_pvBuffer[dwPacket],dwPacketSize);
            m_dwStreamPosition += dwPacketSize;
        }

        if (SUCCEEDED(hr))
        {
            XMEDIAPACKET xmp;
            memset(&xmp,0,sizeof(xmp));
            xmp.pvBuffer = m_pvBuffer[dwPacket];
            xmp.dwMaxSize = dwPacketSize;      
            hr = m_pStream->Process(rtTime, &xmp, &m_dwPacketIndex[dwPacket]);
            m_wPacketsSubmitted++;
        }

    }

    return hr;
}

bool CVoice::StreamDoWork(REFERENCE_TIME rtNow)
{
    DWORD dwPacketStatus;
    if (rtNow > m_rtStartTime)
    {
        rtNow = 0;
    }
    else
    {
        rtNow = m_rtStartTime;
    }
    HRESULT hr = S_OK;
    for (;SUCCEEDED(hr);)
    {
        m_pStream->GetPacketStatus(m_dwPacketIndex[m_wCurrentPacket], &dwPacketStatus);
        if (dwPacketStatus == XMEDIAPACKET_STATUS_SUCCESS)
        {
            // Packet is no longer pending, so we can write to it again...
            hr = WritePacket(rtNow,m_wCurrentPacket++);
            m_wCurrentPacket %= VOICE_NUM_PACKETS;
        }
        else
        {
            break;
        }
    }
    return SUCCEEDED(hr);
}

/*SAMPLE_POSITION CVoice::GetCurrentPos()
{
    // TODO: Figure out why returning m_stStreamPosition sometimes causes the higher levels
    // to get ahead of us and try to refresh an active buffer.
    // (See Xbox bug 5956 DMusic: Streaming waves play back incorrectly; tons of debug spew.)

    // Trace(-1,"Reporting stream position %d\n", (DWORD) m_stStreamPosition);
    return m_dwStreamPosition;
}*/
    
void CVoice::ClearVoice()
{
    m_fInUse = FALSE;
    if(m_pBuffer)
    {
        m_pBuffer->Clear();
        m_pBuffer->SetBufferData(0,NULL,0);
        m_pBuffer->SetOutputBuffer(0,NULL);
        // Release the buffer so it will go back to the dsound pool.
        m_pBuffer->Release(); 
        m_pBuffer = NULL;
    }
    if(m_pStream)
    {
        m_pStream->Clear();
        m_pStream->Flush(0);
        // Release the stream so it will go back to the dsound pool. For some reason,
        // this was not enabled before. Need to make sure this is stable...
        m_pStream->Release();
        m_pStream = NULL;
    }
    if (m_pWave)
    {
        m_pWave->Release();
        m_pWave = NULL;
    }
    if (m_pDLSWave)
    {
        m_pDLSWave->Release();
        m_pDLSWave = NULL;
    }

    for(int i = 0; i < VOICE_NUM_PACKETS; i++)
    {
        if(m_pvBuffer[i])
        {
            if ((m_WaveFormat.wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM) ||
                (m_WaveFormat.wfx.nChannels > 2))

            {
                DirectMusicPhysicalFreeI(m_pvBuffer[i]);
            }
            else
            {
                delete [] m_pvBuffer[i];
            }
            m_pvBuffer[i] = NULL;
        }
    }
}

void CVoice::DoWork(CSynth* pSynth)

{
    if (!m_fNoteOn) 
    {
        if(m_pBuffer)
        {
            DWORD dwStatus;
            m_pBuffer->GetStatus(&dwStatus);
            if( dwStatus == 0 ) // Sound stopped playing
            {
                ClearVoice();
            }
            else
            {
                // If this has been playing for 40 seconds after the stop time, it's somehow stuck on. Kill it.
                REFERENCE_TIME now;
                pSynth->m_MasterClock.GetTime(&now);
                if (now > (m_rtStopTime + (10000 * 40 * 1000)))
                {
                    ClearVoice();
                }
            }
        }
    }
    else
    {
        if (m_pStream && m_fInUse)
        {
            REFERENCE_TIME now;
            pSynth->m_MasterClock.GetTime(&now);
            m_fInUse = StreamDoWork(now); 
            if (!m_fInUse) 
            {
                ClearVoice();
            }
        }
    }
}

