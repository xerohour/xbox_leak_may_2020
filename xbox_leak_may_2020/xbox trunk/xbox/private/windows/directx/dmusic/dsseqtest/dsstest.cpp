// Copyright Microsoft, Inc. All Rights Reserved.

#include "dsoundsequencer.h"

extern "C" ULONG __cdecl DbgPrint(PCH Format, ...);

HRESULT ReportError(HRESULT hr,  const char *filename, int linenumber ){
	if ( FAILED(hr) ) {
		DbgPrint("%s(%d) : Failure 0x%08x\n", filename, linenumber, hr);
		__asm int 3
	}
	return hr;
}

#define REPORT(X) ReportError(X, __FILE__, __LINE__)

class SoundSource {
public:
    virtual ~SoundSource() {}
    virtual void GenerateData(LPVOID pData, DWORD lengthInBytes){}
};

// Converts a floating point value to a long.
static _declspec(naked) long FloatToLong(float f)
{
    _asm
    {
        // Note that this does a truncate, not a floor:

        cvttss2si eax, [esp+4]
        ret 4
    }
}

// Cosine.
static _declspec(naked) float Sin(float e)
{
    _asm
    {
        fld [esp+4]
        fsin
        ret 4
    }
}
// 44KHz stereo 16-bit samples

class ToneSource : public SoundSource{
public:
    void GenerateData(LPVOID pData, DWORD toneFreq, DWORD lengthInBytes, double volume){
        GenerateTone(0, toneFreq, 48000, 2, pData, lengthInBytes, volume);
    }
private:
    void
    GenerateTone
    (
        DWORD                   dwSampleBase,
        DWORD                   dwToneFrequency,
        DWORD                   dwPlaybackFrequency,
        DWORD                   dwChannels,
        LPVOID                  pvBuffer,
        DWORD                   dwBufferSize,
        double                  volume // 0.0 .. 1.0
    )
    {
        static const double     pi          = 3.1415926535;
        static const double     iscale      = 2.0 * pi / (double)(dwPlaybackFrequency - 2);
        static const double     datascale   = volume * 32767.0 / 1.0;
        const double             freq        = (float)dwToneFrequency;
        short *                 psBuffer    = (short *)pvBuffer;
        DWORD                   dwSamples   = dwBufferSize / dwChannels / 2;
        short                   sSample;
        DWORD                   a, b;

        for(a = 0; a < dwSamples; a++)
        {
            float phi = (float) (freq * (a + dwSampleBase) * iscale);
            sSample = (short) FloatToLong((float)(Sin(phi) * datascale));

            for(b = 0; b < dwChannels; b++)
            {
                psBuffer[a * dwChannels + b] = sSample;
            }
        }
    }
};

HRESULT PlayNote(LPDIRECTSOUNDSEQUENCER pSequencer, REFERENCE_TIME rtStart, REFERENCE_TIME rtDuration,
                 void* pToneBuffer, size_t dwToneBufferSizeBytes)
{
    HRESULT hr = S_OK;
    LPDIRECTSOUNDSEQUENCERBUFFER pBuffer = 0;
    DSBUFFERDESC dsbd;
    WAVEFORMATEX wfxFormat;

    memset(&dsbd, 0, sizeof(dsbd));
    memset(&wfxFormat, 0, sizeof(wfxFormat));
    dsbd.dwFlags = DSBCAPS_LOCDEFER | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
    dsbd.lpwfxFormat = &wfxFormat;
    wfxFormat.cbSize = sizeof(wfxFormat);
    wfxFormat.nAvgBytesPerSec = 48000 * 2;
    wfxFormat.nBlockAlign = 2;
    wfxFormat.nChannels = 1;
    wfxFormat.nSamplesPerSec = 48000;
    wfxFormat.wBitsPerSample = 16;
    wfxFormat.wFormatTag = WAVE_FORMAT_PCM;

    if(SUCCEEDED(hr)){
        // DbgPrint("DirectSoundSequencer::CreateBuffer\n");
        hr = pSequencer->CreateBuffer(&dsbd, &pBuffer);
        REPORT(hr);
    }

    if(SUCCEEDED(hr)){
        hr = pBuffer->SetBufferData(0, pToneBuffer, dwToneBufferSizeBytes);
        REPORT(hr);
    }

    if(SUCCEEDED(hr)){
        hr = pBuffer->Play(rtStart, 0, 0, DSBPLAY_LOOPING);
        REPORT(hr);
    }

    if(SUCCEEDED(hr)){
        hr = pBuffer->Stop(rtStart + rtDuration);
        REPORT(hr);
    }

    if(pBuffer) {
        pBuffer->Release();
    }

    return hr;
}


void __cdecl main()
{
    DbgPrint("Start of DirectSoundSequencer test\n");
    HRESULT hr = 0;

    LPDIRECTSOUNDSEQUENCER pSequencer = 0;

    if(SUCCEEDED(hr)){
        DbgPrint("DirectSoundCreateSequencer\n");
        hr = DirectSoundCreateSequencer(0, 500, &pSequencer);
        REPORT(hr);
    }

    const int kToneFreq = 440;
    const int kToneBufferSizeSamples = 48000/(2*kToneFreq);
    const int kToneBufferSizeBytes = kToneBufferSizeSamples*sizeof(WORD);

    WORD* pToneBuffer;

    pToneBuffer = (WORD*) malloc(kToneBufferSizeBytes);
    
    if(!pToneBuffer){
        hr = E_OUTOFMEMORY;
        REPORT(hr);
    }

    ToneSource toneSource;

    if(SUCCEEDED(hr)){
        toneSource.GenerateData(pToneBuffer, kToneFreq, kToneBufferSizeBytes, 0.1);
    }


    REFERENCE_TIME now = 0;
    pSequencer->SetTime(0);
    for(int i = 0; i < 100; i++){
        REFERENCE_TIME rtStart = now + 10000;
        REFERENCE_TIME rtDuration = 2500;
        for(int j = 0; j < 300; j++){
            PlayNote(pSequencer, rtStart, rtDuration, pToneBuffer, kToneBufferSizeBytes);
        }
        Sleep(1000);
        now += 10000;
    }

    if(pSequencer) {
        pSequencer->Release();
    }

    if(pToneBuffer){
        free(pToneBuffer);
    }

    DbgPrint("Exiting dsstest\n");
    XLaunchNewImage( NULL, NULL );
}