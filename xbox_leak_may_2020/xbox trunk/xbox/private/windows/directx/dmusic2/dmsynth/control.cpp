//      Copyright (c) 1996-1999 Microsoft Corporation
//      CControlLogic.cpp
//

#ifdef DMSYNTH_MINIPORT
#include "common.h"
#else
#include "simple.h"
#include <mmsystem.h>
#include <dmusicc.h>
#include <dmusics.h>
#include "synth.h"
#include "misc.h"
#include "csynth.h"
#include "debug.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//
// Manage the global critical section. #pragma's disable the warning about
// not compiling with -GX when using exception handling, which we don't
// care about.
//
//
// The critical section must be global because it protects global
// data in the CMIDIRecorder class. These functions are called from 
// DllMain().
//

CPart::CPart()

{
    Init(NULL,NULL);
}

VREL CPart::m_svrPanToVREL[128];

void CPart::Init(CControlLogic *pControl, CSynth *pSynth)

{
    m_fSustain = FALSE;
    m_dwProgram = 0;
    m_rtLastTime = 0;
    m_bModWheel = 0;
    m_bPressure = 0;
    m_bVolume = 100;
    m_bPan = 64;
    m_bExpression = 127;
    memset(m_bMixBinVolume,127,8);
    m_nPitchBend = 0;
    m_bReverbSend = 40;
    m_bChorusSend = 0;
    m_bFilter = 64;
    m_pControl = pControl;
    m_pSynth = pSynth;

    static BOOL fBeenHereBefore = FALSE;
    if (fBeenHereBefore) return;
    fBeenHereBefore = TRUE;
    WORD nI;
    for (nI = 1; nI < 128; nI++)
    {
        double flTemp;
        flTemp = nI;
        flTemp /= 127.0;
        flTemp = log10(flTemp);
        flTemp *= 1000.0;
        m_svrPanToVREL[nI] = (long) flTemp;
    }  
    m_svrPanToVREL[0] = -2500;
}


CRITICAL_SECTION CControlLogic::s_CriticalSection;
DWORD CControlLogic::m_dwCSRefCount = 0;

CControlLogic::CControlLogic()

{
    if (m_dwCSRefCount == 0)
    {
        ::InitializeCriticalSection(&s_CriticalSection);
    } 
    m_dwCSRefCount++;
    m_pSynth = NULL;
    m_pInstruments = NULL;
    DWORD nIndex;
    GMReset();
    m_fGSActive = FALSE;
    m_fXGActive = FALSE;
}

CControlLogic::~CControlLogic()
{
    if (m_dwCSRefCount == 1)
    {
        DeleteCriticalSection(&s_CriticalSection);
    }
    m_dwCSRefCount--;
}

void CControlLogic::GMReset()

{
    static int nPartToChannel[16] = { 
        9,0,1,2,3,4,5,6,7,8,10,11,12,13,14,15 
    };
    int nX;
    for (nX = 0; nX < 16; nX++)
    {
        int nY;
        CPart *pPart = &m_Part[nX];
        pPart->m_nData = 0;
        pPart->m_prFineTune = 0;
        pPart->m_bDrums = 0;
        for (nY = 0; nY < 12; nY++)
        {
            pPart->m_prScaleTune[nY] = 0;
        }
        pPart->m_nCurrentRPN = (short) 0x3FFF;
        pPart->m_prCoarseTune = 0; 
        pPart->m_bPartToChannel = (BYTE)nPartToChannel[nX];
        pPart->m_fMono = FALSE;
        pPart->m_dwPriority = DAUD_STANDARD_VOICE_PRIORITY + 
            ((16 - nX) * DAUD_CHAN15_VOICE_PRIORITY_OFFSET);
        pPart->m_bBankH = 0;
        pPart->m_bBankL = 0;
    }
    m_Part[0].m_bDrums = 1;
    m_vrMasterVolume = 0;
    m_fGSActive = FALSE;
    m_fXGActive = FALSE;
}

HRESULT CControlLogic::Init(CInstManager *pInstruments, CSynth *pSynth)

{
    m_pSynth = pSynth;
    m_pInstruments = pInstruments;
    m_vrGainAdjust = 0;
    DWORD dwX;
    for (dwX = 0; dwX < 16; dwX++)
    {
        m_Part[dwX].Init(this,pSynth);
    }
    CVoice::Init();
    return S_OK;
}

void CControlLogic::SetGainAdjust(VREL vrGainAdjust)
{
    m_vrGainAdjust = vrGainAdjust;
}

static VREL g_vrMIDIToVREL[128] = 
{
    -9600, -8415, -7211, -6506, -6006, -5619, -5302, -5034, 
    -4802, -4598, -4415, -4249, -4098, -3959, -3830, -3710, 
    -3598, -3493, -3394, -3300, -3211, -3126, -3045, -2968, 
    -2894, -2823, -2755, -2689, -2626, -2565, -2506, -2449, 
    -2394, -2341, -2289, -2238, -2190, -2142, -2096, -2050, 
    -2006, -1964, -1922, -1881, -1841, -1802, -1764, -1726, 
    -1690, -1654, -1619, -1584, -1551, -1518, -1485, -1453, 
    -1422, -1391, -1361, -1331, -1302, -1273, -1245, -1217, 
    -1190, -1163, -1137, -1110, -1085, -1059, -1034, -1010, 
    -985, -961, -938, -914, -891, -869, -846, -824, 
    -802, -781, -759, -738, -718, -697, -677, -657, 
    -637, -617, -598, -579, -560, -541, -522, -504, 
    -486, -468, -450, -432, -415, -397, -380, -363, 
    -347, -330, -313, -297, -281, -265, -249, -233, 
    -218, -202, -187, -172, -157, -142, -127, -113, 
    -98, -84, -69, -55, -41, -27, -13, 0
};

VREL VelocityToVolume(WORD nVelocity)

{
    return (g_vrMIDIToVREL[nVelocity]);
}

static VREL g_vrMIDIPercentToVREL[128] = 
{
    -9600, -4207, -3605, -3253, -3003, -2809, -2651, -2517, 
    -2401, -2299, -2207, -2124, -2049, -1979, -1915, -1855, 
    -1799, -1746, -1697, -1650, -1605, -1563, -1522, -1484, 
    -1447, -1411, -1377, -1344, -1313, -1282, -1253, -1224, 
    -1197, -1170, -1144, -1119, -1095, -1071, -1048, -1025, 
    -1003, -982, -961, -940, -920, -901, -882, -863, 
    -845, -827, -809, -792, -775, -759, -742, -726, 
    -711, -695, -680, -665, -651, -636, -622, -608, 
    -595, -581, -568, -555, -542, -529, -517, -505, 
    -492, -480, -469, -457, -445, -434, -423, -412, 
    -401, -390, -379, -369, -359, -348, -338, -328, 
    -318, -308, -299, -289, -280, -270, -261, -252, 
    -243, -234, -225, -216, -207, -198, -190, -181, 
    -173, -165, -156, -148, -140, -132, -124, -116, 
    -109, -101, -93, -86, -78, -71, -63, -56, 
    -49, -42, -34, -27, -20, -13, -6, 0 
};

VREL MIDIToPercent(WORD nMIDI)

{
    return (g_vrMIDIPercentToVREL[nMIDI]);
}


void CPart::AllSoundsOff(REFERENCE_TIME rtTime)

{
    CVoice *pVoice = m_pSynth->m_VoicesInUse.GetHead();
    for (;pVoice != NULL; pVoice = pVoice->GetNext())
    {
        if (pVoice->m_pPart == this)
        {
            pVoice->QuickStopVoice(rtTime);
        }
    }
}

void CPart::SetSustain(REFERENCE_TIME rtTime,BOOL fSustain)

{                
    m_fSustain = fSustain;
    if (!fSustain)
    {
        CVoice * pVoice = m_pSynth->m_VoicesInUse.GetHead();
        for (;pVoice != NULL;pVoice = pVoice->GetNext())
        {
            if (pVoice->m_fSustainOn &&
                (pVoice->m_pPart == this))
            {
                pVoice->StopVoice(rtTime);
            }
        }
    } 
}

void CPart::AllNotesOff(REFERENCE_TIME rtTime)
{
    CVoice *pVoice = m_pSynth->m_VoicesInUse.GetHead();
    for (;pVoice != NULL; pVoice = pVoice->GetNext())
    {
        if (pVoice->m_fNoteOn && !pVoice->m_fSustainOn &&
            (pVoice->m_pPart == this))
        {
            if (m_fSustain)
            {
                pVoice->m_fSustainOn = TRUE;
            }
            else
            {
                pVoice->StopVoice(rtTime);
            }
        }
    }
}

void CPart::GetVolume(VREL vrVoice, long lDefaultPan,VREL *pvrVolume,VREL *pvrLeft,VREL *pvrRight,VREL *pvrReverb,VREL *pvrChorus)

{
    VREL vrVolume = VelocityToVolume(m_bVolume) +
        VelocityToVolume(m_bExpression);
    vrVolume += m_pSynth->m_vrGainAdjust;
    vrVolume += m_pControl->m_vrMasterVolume;
    vrVolume += vrVoice;
    if (pvrVolume)
    {
        *pvrVolume = vrVolume;
    }
    if (pvrLeft && pvrRight)
    {
        lDefaultPan += m_bPan;

        if (lDefaultPan < 0) 
            lDefaultPan = 0;

        if (lDefaultPan > 127) 
            lDefaultPan = 127;

        *pvrLeft = m_svrPanToVREL[127 - lDefaultPan] + vrVolume;
        *pvrRight = m_svrPanToVREL[lDefaultPan] + vrVolume;
    } 
    if (pvrReverb)
    {
        *pvrReverb = vrVolume + g_vrMIDIPercentToVREL[m_bReverbSend];
    }
    if (pvrChorus)
    {
        *pvrChorus = vrVolume + g_vrMIDIPercentToVREL[m_bChorusSend];
    }
}

void CPart::GetVolume(VREL vrVolume, BYTE pbControllers[], VREL pvrVolume[])

{
    vrVolume += VelocityToVolume(m_bVolume) +
        VelocityToVolume(m_bExpression);
    vrVolume += m_pSynth->m_vrGainAdjust;
    vrVolume += m_pControl->m_vrMasterVolume;
    DWORD dwMixBin;
    for (dwMixBin = 0; pbControllers[dwMixBin]; dwMixBin++)
    {
        pvrVolume[dwMixBin] = vrVolume + 
            VelocityToVolume(m_bMixBinVolume[pbControllers[dwMixBin]-1]);
    }
}

void CPart::GetModWheel(DWORD *pdwModWheel)

{
    *pdwModWheel = m_bModWheel;
}

void CPart::GetPressure(DWORD *pdwPressure)

{
    *pdwPressure = m_bPressure;
}

void CPart::GetPitch(PREL *prPitch)

{
    *prPitch = m_nPitchBend + m_prFineTune;
}

void CPart::GetFilter(PREL *prFilter)

{
    *prFilter = m_bFilter - 64; 
    *prFilter *= 100; // Convert to pitch cents.
}

void CPart::SendVolume(REFERENCE_TIME rtTime)

{
    CVoice *pVoice = m_pSynth->m_VoicesInUse.GetHead();
    for (;pVoice != NULL; pVoice = pVoice->GetNext())
    {
        if (pVoice->m_pPart == this)
        {
            pVoice->SendVolume(rtTime);
        }
    }
}

void CPart::SendPitch(REFERENCE_TIME rtTime)

{
    CVoice *pVoice = m_pSynth->m_VoicesInUse.GetHead();
    for (;pVoice != NULL; pVoice = pVoice->GetNext())
    {
        if (pVoice->m_pPart == this)
        {
            pVoice->SendPitch(rtTime);
        }
    }
}


void CPart::SendFilter(REFERENCE_TIME rtTime)

{
    CVoice *pVoice = m_pSynth->m_VoicesInUse.GetHead();
    for (;pVoice != NULL; pVoice = pVoice->GetNext())
    {
        if (pVoice->m_pPart == this)
        {
            pVoice->SendFilter(rtTime,FALSE);
        }
    }
}

void CPart::SendLFOs(REFERENCE_TIME rtTime)

{
    CVoice *pVoice = m_pSynth->m_VoicesInUse.GetHead();
    for (;pVoice != NULL; pVoice = pVoice->GetNext())
    {
        if (pVoice->m_pPart == this)
        {
            pVoice->SendLFOs(rtTime);
        }
    }
}

void CPart::PlayNoteOff(REFERENCE_TIME rtTime,BYTE bNote)

{
    CVoice * pVoice = m_pSynth->m_VoicesInUse.GetHead();
    DWORD dwNoteID = 0; // Use to track multiple voices on one note.
    for (;pVoice != NULL;pVoice = pVoice->GetNext())
    {
        if (pVoice->m_fNoteOn && !pVoice->m_fSustainOn &&
            (pVoice->m_nKey == (WORD) bNote) &&
            (pVoice->m_pPart == this)) 
        {
            if (!dwNoteID || (dwNoteID == pVoice->m_dwNoteID))
            {
                dwNoteID = pVoice->m_dwNoteID;
                if (m_fSustain)
                {
                    pVoice->m_fSustainOn = TRUE;
                }
                else
                {
                    pVoice->StopVoice(rtTime);
                }
            }
        }
    }
}

void CPart::PlayNoteOn(REFERENCE_TIME rtTime,BYTE bNote, BYTE bVelocity)

{
    DWORD dwProgram = m_dwProgram;
    if (m_bDrums)
    {
        dwProgram |= F_INSTRUMENT_DRUMS;
    }
    if (m_fMono)
    {
        CVoice * pVoice = m_pSynth->m_VoicesInUse.GetHead();
        for (;pVoice != NULL;pVoice = pVoice->GetNext())
        {
            if (pVoice->m_fNoteOn && (pVoice->m_pPart == this)) 
            {
                pVoice->StopVoice(rtTime);
            }
        }
    }
    // While we are working with the instrument, including copying
    // the data over from the region, we have to make sure it
    // can not be removed from the instrument list.
    EnterCriticalSection(&m_pControl->m_pInstruments->m_CriticalSection);
    CInstrument * pInstrument = 
        m_pControl->m_pInstruments->GetInstrument(dwProgram,bNote,bVelocity);
    if (!pInstrument) 
    {
        if (dwProgram & F_INSTRUMENT_DRUMS)
        {
            dwProgram = F_INSTRUMENT_DRUMS;
            pInstrument = 
                m_pControl->m_pInstruments->GetInstrument(dwProgram,bNote,bVelocity);
        }
        else if (m_pControl->m_fXGActive)
        {
            if ((dwProgram & 0x7F0000) == 0x7F0000) // Drum?
            {
                dwProgram &= 0x7F007F;              // Enforce 0 LSB
                pInstrument = 
                    m_pControl->m_pInstruments->GetInstrument(dwProgram,bNote,bVelocity);
                if (!pInstrument)
                {
                    dwProgram = 0x7F0000;
                    pInstrument = 
                        m_pControl->m_pInstruments->GetInstrument(dwProgram,bNote,bVelocity);
                }
            }
            else
            {
                dwProgram &= 0x7F;  // Fall back to GM set.
                pInstrument = 
                    m_pControl->m_pInstruments->GetInstrument(dwProgram,bNote,bVelocity);
            }
        }
    }
    if (pInstrument != NULL)
    {
        DWORD dwNotesLost = 1;  // Assume note will be lost, will be decremented if played 
        CSourceRegion * pRegion = NULL;
        static DWORD sdwNoteID = 0; // Generate a unique id that will be placed in all voices that play this note.
        sdwNoteID++;                // This will be used to keep the voices associated so we can stop them all at once later.
        while ( pRegion = pInstrument->ScanForRegion(bNote,bVelocity,pRegion) ) 
        {
            CVoice * pVoice = m_pSynth->m_VoicesInUse.GetHead();
            if (!pRegion->m_bAllowOverlap)
            {
                for (;pVoice != NULL; pVoice = pVoice->GetNext())
                {
                    if ((pVoice->m_pPart == this) &&
                        (pVoice->m_nKey == bNote) &&
                        (pVoice->m_pRegion  == pRegion))
                    {
                        pVoice->QuickStopVoice(rtTime);
                    }
                }    
            }

            if (pRegion->m_bGroup != 0)
            {
                pVoice = m_pSynth->m_VoicesInUse.GetHead();
                for (;pVoice != NULL;pVoice = pVoice->GetNext())
                {
                    if ((pVoice->m_dwGroup == pRegion->m_bGroup) &&
                        (pVoice->m_pPart == this) &&
                        (pVoice->m_dwProgram == dwProgram))
                    {
                        pVoice->QuickStopVoice(rtTime);
                    }
                }
            }
        
            pVoice = m_pSynth->m_VoicesFree.RemoveHead();
            if (pVoice == NULL)
            {
                pVoice = m_pSynth->m_VoicesExtra.RemoveHead();
            }

            if (pVoice == NULL)
            {
                pVoice = m_pSynth->StealVoice(m_dwPriority);
                
                // The voice IDs are used by the VoiceServiceThread in DMusic
                // to refill the streaming wave buffers....
                // Since the voice is stolen this voice could really belong to
                // a streaming wave in which case preserving the voice ID will 
                // break the refill code. (NOTE!! This is different from stealing
                // voices for waves. Waves will ultimately preserve the voice ID as 
                // they pass it to StartWave where it gets assigned to the voice's 
                // m_dwVoiceId member).

                if(pVoice)
                {
                    // Set the voice ID to something unacceptable
                    pVoice->m_dwVoiceId = 0xffffffff;
                }
            }

            if (pVoice != NULL)
            {
                PREL prPitch = m_prScaleTune[bNote % 12];
                if (!m_bDrums) 
                {
                    if (m_pControl->m_fXGActive)
                    {
                        if ((dwProgram & 0x7F0000) != 0x7F0000)
                        {
                            prPitch += m_prCoarseTune;
                        }
                    }
                    else
                    {
                        prPitch += m_prCoarseTune;
                    }
                }

                pVoice->m_pPart = this;
                pVoice->m_nKey = bNote;
                pVoice->m_dwProgram = dwProgram;
                pVoice->m_dwPriority = m_dwPriority;
                pVoice->m_pControl = m_pControl;
                pVoice->m_pRegion = pRegion;
                pVoice->m_dwNoteID = sdwNoteID;
                VREL vrVolume = VelocityToVolume(m_bVolume) +
                    VelocityToVolume(m_bExpression);

                if (pVoice->StartVoice(m_pSynth,
                    pRegion, rtTime, 
                    &m_BusIds,
                    (WORD)bNote,
                    (WORD)bVelocity,
                    m_pControl->m_vrMasterVolume, 
                    vrVolume,
                    prPitch,
                    m_nPitchBend))
                {
                    pVoice->m_fInUse = TRUE;
                    m_pSynth->QueueVoice(pVoice);
                    dwNotesLost = 0;    // Note played remove notelost assumpstion 
                }
                else
                {
                    m_pSynth->m_VoicesFree.AddHead(pVoice);
                }
            } 
        } 
    } 
    else 
    {
        Trace(2,"Warning: No instrument/region was found for patch # %lx, note %ld\n",
            dwProgram, (long) bNote);
    }
    LeaveCriticalSection(&m_pControl->m_pInstruments->m_CriticalSection);
}

void CPart::RecordMIDI(REFERENCE_TIME rtTimeIn, BYTE bStatus, BYTE bData1, BYTE bData2)

{
    REFERENCE_TIME rtTime;
    // We don't allow events to any particular part to be out of order. 
    // This simplifies the implementation of continuous controllers
    // enormously. 
    if (rtTimeIn > m_rtLastTime)
    {
        rtTime = rtTimeIn;
        m_rtLastTime = rtTime;
    }
    else
    {
        rtTime = m_rtLastTime;
    }
    switch (bStatus)
    {
    case MIDI_NOTEOFF :
        PlayNoteOff(rtTime, bData1);
        break;
    case MIDI_NOTEON :
        PlayNoteOn(rtTime, bData1, bData2);
        break;
    case MIDI_CCHANGE :
        switch (bData1)
        {
        case CC_BANKSELECTH :
            m_bBankH = bData2;
            break;
        case CC_MODWHEEL :
            m_bModWheel = bData2;
            SendLFOs(rtTime);
            break;
        case CC_VOLUME :
            m_bVolume = bData2;
            SendVolume(rtTime);
            break;
        case CC_PAN :
            m_bPan = bData2;
            SendVolume(rtTime);
            break;
        case CC_EXPRESSION :
            m_bExpression = bData2;
            SendVolume(rtTime);
            break;
        case CC_BANKSELECTL :
            m_bBankL = bData2;
            break;
        case CC_RESETALL:
            if (bData2)
            {
                m_bVolume = 100;
                m_bPan = 64;
            }
            m_bExpression = 127;
            m_nPitchBend = 0;
            m_bModWheel = 0;  
            m_bPressure = 0;
            m_bPressure = 0;
            m_bReverbSend = 40;
            m_bChorusSend = 0;
            m_bFilter = 64;
            memset(m_bMixBinVolume,127,8);
            bData2 = 0;
            SendVolume(rtTime);
            SendLFOs(rtTime);
            SendFilter(rtTime);
            SendPitch(rtTime);
            // fall through into Sustain Off case....

        case CC_SUSTAIN :
            SetSustain(rtTime,(BOOL)bData2);
            break;
        case CC_ALLSOUNDSOFF:
            AllSoundsOff(rtTime);
            break;
        case CC_ALLNOTESOFF:
            AllNotesOff(rtTime);
            break;
        case CC_DATAENTRYMSB:
            m_nData &= ~(0x7F << 7);
            m_nData |= bData2 << 7;
            switch (m_nCurrentRPN)
            {
            case RPN_PITCHBEND:
                // Special case for a range of 82 semitones, because this must
                // really be an attempt to have the pitch bend map directly to
                // pitch cents.
//                    if (bData == 82)
//                    {
//                        m_PitchBend.m_prRange = 8192;
//                    }
//                    else
//                    {
//                        m_PitchBend.m_prRange = bData * 100;
//                    }
                break;
            case RPN_FINETUNE:
                {
                    long lTemp = m_nData;
                    lTemp -= 8192;
                    lTemp *= 100;
                    lTemp /= 8192;
                    m_prFineTune = lTemp;
                }
                break;
            case RPN_COARSETUNE:
                m_prCoarseTune = 100 * (bData2 - 64);
                break;        
            }
            break;
        case CC_DATAENTRYLSB:
            m_nData &= ~0x7F;
            m_nData |= bData2;
            switch (m_nCurrentRPN)
            {
            case RPN_PITCHBEND: // Don't do anything, Roland ignores lsb
                break;
            case RPN_FINETUNE:
                {
                    long lTemp = m_nData;
                    lTemp -= 8192;
                    lTemp *= 100;
                    lTemp /= 8192;
                    m_prFineTune = lTemp;
                }
                break;
            case RPN_COARSETUNE: // Ignore lsb
                break;            
            }
            break;
        case CC_NRPN_LSB :
        case CC_NRPN_MSB :
            m_nCurrentRPN = 0x3FFF;
            break;
        case CC_RPN_LSB:
            m_nCurrentRPN = (m_nCurrentRPN & 0x3f80) + bData2;
            break; 
        case CC_RPN_MSB:
            m_nCurrentRPN = (m_nCurrentRPN & 0x7f) + (bData2 << 7);
            break;
        case CC_MONOMODE :
            m_fMono = TRUE;
            AllSoundsOff(rtTime);
            break;
        case CC_POLYMODE :
            m_fMono = FALSE;
            AllSoundsOff(rtTime);
            break;
        case CC_REVERB :
            m_bReverbSend = bData2;
            SendVolume(rtTime);
            break;
        case CC_CHORUS :
            m_bChorusSend = bData2;
            SendVolume(rtTime);
            break;
        case CC_CUTOFFFREQ:
            m_bFilter = bData2;
            SendFilter(rtTime);
            break;
        case CC_MIXBINVOLUME:
        case CC_MIXBINVOLUME+1:
        case CC_MIXBINVOLUME+2:
        case CC_MIXBINVOLUME+3:
        case CC_MIXBINVOLUME+4:
        case CC_MIXBINVOLUME+5:
        case CC_MIXBINVOLUME+6:
        case CC_MIXBINVOLUME+7:
            m_bMixBinVolume[bData1 - CC_MIXBINVOLUME] = bData2;
            SendVolume(rtTime);
            break;
        default:
            break;
        }
        break;

    case MIDI_PCHANGE :
        m_dwProgram = bData1 | 
            (m_bBankH << 16) | 
            (m_bBankL << 8);
        break;
    case MIDI_PBEND :
        {
            WORD nBend;
            nBend = bData2 << 7;
            nBend |= bData1;
            m_nPitchBend = nBend - 0x2000;
            SendPitch(rtTime);
//            SendFilter(stTime);
        }
        break;

    case MIDI_MTOUCH:
        m_bPressure = bData1;
        SendLFOs(rtTime);
        break;
    }
}


BOOL CControlLogic::RecordMIDI(REFERENCE_TIME rtTimeIn,BYTE bStatus, BYTE bData1, BYTE bData2)

{
    WORD nPreChannel = bStatus & 0xF; 
    bStatus = bStatus & 0xF0;
    DWORD dwPart;

    ::EnterCriticalSection(&s_CriticalSection);

    for (dwPart = 0;dwPart < 16; dwPart++)
    {
        CPart *pPart = &m_Part[dwPart];
        if (nPreChannel == pPart->m_bPartToChannel)
        {
            pPart->RecordMIDI(rtTimeIn,bStatus,bData1,bData2);
        }
    }
    ::LeaveCriticalSection(&s_CriticalSection);

    return TRUE;
}

HRESULT CControlLogic::RecordSysEx(DWORD dwSysExLength,BYTE *pSysExData, REFERENCE_TIME rtTime)

{
    HRESULT hSuccess = S_OK;
    int nPart;
    int nTune;
    DWORD dwAddress;
    BOOL fClearAll = FALSE;
    BOOL fResetPatches = FALSE;
    if (dwSysExLength < 6) 
    {
        Trace(4,"Warning: Unknown sysex message sent to synth.\n");
        return E_FAIL;
    }

    EnterCriticalSection(&s_CriticalSection);

    switch (pSysExData[1])  // ID number
    {
    case 0x7E : // General purpose ID
        if (pSysExData[3] == 0x09) 
        {
            GMReset();
            fClearAll = TRUE;
            fResetPatches = TRUE;
        }
        break;
    case 0x7F : // Real time ID
        if (pSysExData[3] == 0x04)
        {
            if (pSysExData[4] == 1) // Master Volume
            {
                m_vrMasterVolume = VelocityToVolume(pSysExData[6]);
            }
        }
        break;
    case 0x41 : // Roland
        if (dwSysExLength < 11) 
        {
            Trace(4,"Warning: Unknown sysex message sent to synth.\n");
            LeaveCriticalSection(&s_CriticalSection);
            return E_FAIL;
        }
        if (pSysExData[3] != 0x42) break;
        if (pSysExData[4] != 0x12) break;
        nPart = pSysExData[6] & 0xF;
        dwAddress = (pSysExData[5] << 16) |
            ((pSysExData[6] & 0xF0) << 8) | pSysExData[7];
        switch (dwAddress)
        {
        case 0x40007F :     // GS Reset.
            GMReset();
            m_fXGActive = FALSE;
            fClearAll = TRUE;
            m_fGSActive = TRUE;
            fResetPatches = TRUE;
            break;
        case 0x401002 :     // Set Receive Channel.
            if (m_fGSActive)
            {
                if (pSysExData[8])
                {
                    m_Part[nPart].m_bPartToChannel = (BYTE) pSysExData[8] - 1;
                }
            }
            break;
        case 0x401015 :     // Use for Rhythm.
            if (m_fGSActive)
            {
                m_Part[nPart].m_bDrums = pSysExData[8];
                fClearAll = TRUE;
            }
            break;
        case 0x401040 :     // Scale Tuning.
            if (m_fGSActive)
            {
                for (nTune = 0;nTune < 12; nTune++)
                {
                    if (pSysExData[9 + nTune] & 0x80) break;
                    m_Part[nPart].m_prScaleTune[nTune] = 
                        (PREL) pSysExData[8 + nTune] - (PREL) 64;
                }
            }
            break;
        }
        break;
    case 0x43 : // Yamaha
        if ((pSysExData[3] == 0x4C) &&
            (pSysExData[4] == 0) &&
            (pSysExData[5] == 0) &&
            (pSysExData[6] == 0x7E) &&
            (pSysExData[7] == 0))
        {   // XG System On
            m_fXGActive = TRUE;
            m_fGSActive = FALSE;
            GMReset();
            m_fXGActive = TRUE;
            m_Part[0].m_bDrums = 0;
            m_Part[0].m_bBankH = 127;
            fClearAll = TRUE;
            fResetPatches = TRUE;
        }
        break;
    }
    if (fClearAll)
    {
        for (nPart = 0;nPart < 16;nPart++)
        {     
            CPart *pPart = &m_Part[nPart];
            pPart->AllSoundsOff(rtTime);
            pPart->SetSustain(rtTime,FALSE);
            pPart->m_bVolume = 100;
            pPart->m_bPan = 64;
            pPart->m_bExpression = 127;
            pPart->m_nPitchBend = 0;
            pPart->m_bModWheel = 0;
            pPart->m_bPressure = 0;
            pPart->m_bReverbSend = 40;
            pPart->m_bChorusSend = 0;
            pPart->m_bFilter = 64;
        }
    }
    if (fResetPatches)
    {
        for (nPart = 0;nPart < 16;nPart++)
        {     
            if ((nPart == 0) && (m_fXGActive))
            {
                m_Part[nPart].m_dwProgram = 0x7F0000;
            }
            else
            {
                m_Part[nPart].m_dwProgram = 0;
            }
        }
    }

    LeaveCriticalSection(&s_CriticalSection);
    return hSuccess;
}

HRESULT CControlLogic::SetChannelPriority(DWORD dwChannel,DWORD dwPriority)

{
    DWORD dwPart;
    for (dwPart = 0;dwPart < 16; dwPart++)
    {
        if (m_Part[dwPart].m_bPartToChannel == dwChannel)
        {
            m_Part[dwPart].m_dwPriority = dwPriority;
        }
    }
    return S_OK;
}

HRESULT CControlLogic::GetChannelPriority(DWORD dwChannel,LPDWORD pdwPriority)

{
    DWORD dwPart;
    for (dwPart = 0;dwPart < 16; dwPart++)
    {
        if (m_Part[dwPart].m_bPartToChannel == dwChannel)
        {
            *pdwPriority = m_Part[dwPart].m_dwPriority;
            break;
        }
    }
    return S_OK;
}

static DWORD g_dwVoiceID = 1;

BOOL CControlLogic::RecordWaveEvent(
    REFERENCE_TIME rtTime, BYTE bChannel, VREL vrVolume, PREL prPitchIn, 
    SAMPLE_TIME stVoiceStart, SAMPLE_TIME stLoopStart, SAMPLE_TIME stLoopEnd,
    IDirectSoundWave *pIWave, DWORD *pdwVoiceID)

{
    WORD nPreChannel = bChannel;
    BOOL bReturn = TRUE;
    DWORD dwPart;

    ::EnterCriticalSection(&s_CriticalSection);


    CSourceWave *pWave;
    if (SUCCEEDED(pIWave->QueryInterface(IID_CSourceWave,(void **) &pWave)))
    {
        *pdwVoiceID = g_dwVoiceID++;
        for (dwPart = 0;dwPart < 16; dwPart++)
        {
            CPart *pPart = &m_Part[dwPart];;
            if (nPreChannel == pPart->m_bPartToChannel)
            {
                EnterCriticalSection(&m_pInstruments->m_CriticalSection);
                CVoice * pVoice = m_pSynth->m_VoicesFree.RemoveHead();

                if (pVoice == NULL)
                {
                    pVoice = m_pSynth->m_VoicesExtra.RemoveHead();
                }

                if (pVoice == NULL)
                {
                    pVoice = m_pSynth->StealVoice(pPart->m_dwPriority);
                }

                if (pVoice != NULL)
                {
                    pVoice->m_nKey = 0xffff;                // set to unused values 
                    pVoice->m_dwProgram = 0xffffffff;       // set to unused values 
                    pVoice->m_pPart = pPart;
                    pVoice->m_dwPriority = pPart->m_dwPriority;
                    pVoice->m_pControl = this;
                    pVoice->m_pRegion  = NULL;
                    BOOL fSuccess;
                    if ( pWave->m_pnWave ) 
                    {
                        fSuccess = pVoice->StartWave(m_pSynth,
                            pWave,
                            *pdwVoiceID, 
                            rtTime, 
                            &pPart->m_BusIds,
                            vrVolume,0,    
                            prPitchIn,0,
                            (DWORD) stVoiceStart,
                            (DWORD) stLoopStart,
                            (DWORD) stLoopEnd);
                    }
                    else
                    {
                        fSuccess = pVoice->StartStreamedWave(m_pSynth,
                            pWave,
                            *pdwVoiceID, 
                            rtTime, 
                            &pPart->m_BusIds,
                            vrVolume,0,    
                            prPitchIn,0,
                            (DWORD) stVoiceStart,
                            (DWORD) stLoopStart,
                            (DWORD) stLoopEnd);
                    }
                    if (fSuccess)
                    {
                        pVoice->m_fInUse = TRUE;
                        m_pSynth->QueueVoice(pVoice);
                    }
                    else
                    {
                        m_pSynth->m_VoicesFree.AddHead(pVoice);
                    }
                } 
                else 
                {
                    Trace(1,"Error: No voice avaible for synt\n");
                }
                LeaveCriticalSection(&m_pInstruments->m_CriticalSection);
            }
        }
        pWave->Release();
    }
    ::LeaveCriticalSection(&s_CriticalSection);

    return bReturn;
}

HRESULT CControlLogic::AssignChannelToOutput(DWORD dwChannel, IDirectSoundBuffer *pBuffer,DWORD dwMixBins, BYTE* pbControllers )
{
    DWORD dwPart;
    for (dwPart = 0;dwPart < 16; dwPart++)
    {
        if (m_Part[dwPart].m_bPartToChannel == dwChannel)
        {
            //
            // Assign new mixbins and buffer to this channel 
            //
            m_Part[dwPart].m_BusIds.AssignOutput(pBuffer, dwMixBins, pbControllers);
        }
    }

    return S_OK;
}
/*
/////////////////////////////////////////////////////////////////
CWaveDataList    CWaveIn::m_sFreeList;
DWORD            CWaveIn::m_sUsageCount = 0;

CWaveData::CWaveData() 
{
    m_stTime = 0;
}

CWaveIn::CWaveIn()
{
    m_sUsageCount++;
    m_stCurrentTime = 0;
}

CWaveIn::~CWaveIn()

{
//    ClearWave(0x7FFFFFFF);
    m_sUsageCount--;
    // If there are no instances of CMIDIRecorder left, get rid of the free pool.
    if (!m_sUsageCount)
    {
        CWaveData *pWD;
        while (pWD = m_sFreeList.RemoveHead())
        {
            delete pWD;
        }
    }
}

BOOL CWaveIn::ClearWave(REFERENCE_TIME rtTime)

{
    CWaveData *pWD;

    for (;pWD = m_EventList.GetHead();)
    {
        if (pWD->m_stTime < stTime)
        {
            m_EventList.RemoveHead();
            m_stCurrentTime = pWD->m_stTime;
            m_lCurrentData  = pWD->m_WaveEventData;
            if (pWD->m_WaveEventData.m_pWaveArt)
            {
                pWD->m_WaveEventData.m_pWaveArt->Release();
                pWD->m_WaveEventData.m_pWaveArt = NULL;
            }
            m_sFreeList.AddHead(pWD);
            
        }
        else break;
    }
    return m_EventList.IsEmpty();
}*/

/*BOOL CWaveIn::RemoveWave(DWORD dwID)

{
    CWaveData *pWD = m_EventList.GetHead();
    CWaveData *pWDNext = NULL;

    for (; pWD; pWD = pWDNext)
    {
        pWDNext = pWD->GetNext();
        if (pWD->m_WaveEventData.m_dwVoiceId == dwID)
        {
            m_EventList.Remove(pWD);
            if (pWD->m_WaveEventData.m_pWaveArt)
            {
                pWD->m_WaveEventData.m_pWaveArt->Release();
                pWD->m_WaveEventData.m_pWaveArt = NULL;
            }
            m_sFreeList.AddHead(pWD);
        }
    }
    return m_EventList.IsEmpty();
}

BOOL CWaveIn::RemoveWaveByStopTime(DWORD dwID, REFERENCE_TIME rtStopTime)

{
    CWaveData *pWD = m_EventList.GetHead();
    CWaveData *pWDNext = NULL;

    for (; pWD; pWD = pWDNext)
    {
        pWDNext = pWD->GetNext();
        if (pWD->m_WaveEventData.m_dwVoiceId == dwID && pWD->m_stTime >= rtStopTime)
        {
            m_EventList.Remove(pWD);
            if (pWD->m_WaveEventData.m_pWaveArt)
            {
                pWD->m_WaveEventData.m_pWaveArt->Release();
                pWD->m_WaveEventData.m_pWaveArt = NULL;
            }
            m_sFreeList.AddHead(pWD);
        }
    }
    return m_EventList.IsEmpty();
}*/

/*BOOL CWaveIn::RecordWave(REFERENCE_TIME rtTime, CWaveEvent* pWave)

{
    CWaveData *pWD = m_sFreeList.RemoveHead();
    if (!pWD)
    {
        pWD = new CWaveData;
    }    
    CWaveData *pScan = m_EventList.GetHead();
    CWaveData *pNext;

    if (pWD)
    {
        pWD->m_stTime        = rtTime;
        pWD->m_WaveEventData = *pWave;
        pWD->m_WaveEventData.m_stTime = rtTime;
        if (pScan == NULL)
        {
            m_EventList.AddHead(pWD);
        }
        else
        {
            if (pScan->m_stTime > stTime)
            {
                m_EventList.AddHead(pWD);
            }
            else
            {
                for (;pScan != NULL; pScan = pNext)
                {
                    pNext = pScan->GetNext();
                    if (pNext == NULL)
                    {
                        pScan->SetNext(pWD);
                    }
                    else
                    {
                        if (pNext->m_stTime > stTime)
                        {
                            pWD->SetNext(pNext);
                            pScan->SetNext(pWD);
                            break;
                        }
                    }
                }
            }
        }
        return (TRUE);
    }
    Trace(1,"Error: Wave Event pool empty.\n");

    return (FALSE);
}*/

/*BOOL CWaveIn::GetWave(REFERENCE_TIME rtTime, CWaveEvent *pWave)

{
    CWaveData *pWD = m_EventList.GetHead();

    if (pWD != NULL)
    {
        if (pWD->m_stTime <= stTime)
        {
            *pWave =  pWD->m_WaveEventData;
            m_EventList.RemoveHead();
            m_sFreeList.AddHead(pWD);
            return (TRUE);
        }
    }
    return (FALSE);
}*/

#ifndef XBOX
/////////////////////////////////////////////////////////////////
CBusIds::CBusIds()
{
    m_dwBusIds[0] = DSBUSID_LEFT;
    m_dwBusIds[1] = DSBUSID_RIGHT;
    m_dwBusIds[2] = DSBUSID_REVERB_SEND;
    m_dwBusIds[3] = DSBUSID_CHORUS_SEND;
    m_dwBusCount = NUM_DEFAULT_BUSES;
}

CBusIds::~CBusIds()
{
}

HRESULT CBusIds::Initialize()
{
    HRESULT hr = S_OK;

    m_dwBusIds[0] = DSBUSID_LEFT;
    m_dwBusIds[1] = DSBUSID_RIGHT;
    m_dwBusIds[2] = DSBUSID_REVERB_SEND;
    m_dwBusIds[3] = DSBUSID_CHORUS_SEND;
    m_dwBusCount = NUM_DEFAULT_BUSES;

    return hr;
}



HRESULT CBusIds::AssignBuses(LPDWORD pdwBusIds, DWORD dwBusCount)
{
    HRESULT hr = S_OK;

    if ( pdwBusIds && dwBusCount > 0 )
    {
        if ( dwBusCount > MAX_DAUD_CHAN )
            dwBusCount = MAX_DAUD_CHAN;

        memcpy(m_dwBusIds,pdwBusIds,sizeof(DWORD)*dwBusCount);
        m_dwBusCount = dwBusCount;
    }

    return hr;
}
#endif
