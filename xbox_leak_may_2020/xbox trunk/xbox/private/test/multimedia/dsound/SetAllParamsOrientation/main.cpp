#include "xtl.h"
#include "dsstdfx.h"
#include "xbsound.h"
#include "stdio.h"
#include <xdbg.h>
#define M_PI       3.14159265358979323846

void 
__cdecl
main()
{
    IDirectSound8 *pDSound = NULL;
    HRESULT hr = DirectSoundCreate(NULL, &pDSound, NULL);

    DSEFFECTIMAGELOC EffectLoc;
    EffectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
    EffectLoc.dwCrosstalkIndex = I3DL2_CHAIN_XTALK;
    hr = XAudioDownloadEffectsImage("d:\\media\\dsstdfx.bin", &EffectLoc, XAUDIO_DOWNLOADFX_EXTERNFILE, NULL);

    CWaveFile WaveFile;
    hr = WaveFile.Open("d:\\media\\Dolphin4.wav");

    DWORD cbFormat = 0;
    hr = WaveFile.GetFormat(NULL, 0, &cbFormat);
    WAVEFORMATEX *pFormat = (WAVEFORMATEX *) new char[cbFormat];
    hr = WaveFile.GetFormat(pFormat, cbFormat, NULL);

    DWORD cbData;
    WaveFile.GetDuration(&cbData);
    void *pData;
    pData = new char[cbData];
    DWORD cbRead = 0;
    hr = WaveFile.ReadSample(0, pData, cbData, &cbRead);
    WaveFile.Close();

    DSBUFFERDESC BufferDesc;
    ZeroMemory( &BufferDesc, sizeof( DSBUFFERDESC ) );
    BufferDesc.dwSize = sizeof( BufferDesc );
    BufferDesc.dwFlags = DSBCAPS_CTRL3D;
    BufferDesc.lpwfxFormat = pFormat;

    IDirectSoundBuffer8 *pBuffer = NULL;
    DirectSoundUseFullHRTF();
    DSI3DL2LISTENER i3dl2 = { DSI3DL2_ENVIRONMENT_PRESET_NOREVERB };
    hr = pDSound->SetI3DL2Listener(&i3dl2, DS3D_IMMEDIATE);

    hr = DirectSoundCreateBuffer(&BufferDesc, &pBuffer);
    hr = pBuffer->SetBufferData(pData, cbRead);

    hr = pBuffer->SetPosition(2.0, 0, 0, DS3D_IMMEDIATE);
    hr = pBuffer->Play(0, 0, DSBPLAY_LOOPING);
    DWORD dwTimeStart = GetTickCount();


    DS3DLISTENER listener;
    listener.dwSize = sizeof(listener);
    listener.vPosition.x = DS3D_DEFAULTPOSITION_X;
    listener.vPosition.y = DS3D_DEFAULTPOSITION_Y;
    listener.vPosition.z = DS3D_DEFAULTPOSITION_Z;
    listener.vVelocity.x = DS3D_DEFAULTVELOCITY_X;
    listener.vVelocity.y = DS3D_DEFAULTVELOCITY_Y;
    listener.vVelocity.z = DS3D_DEFAULTVELOCITY_Z;
    listener.vOrientTop.x = DS3D_DEFAULTORIENTTOP_X;
    listener.vOrientTop.y = DS3D_DEFAULTORIENTTOP_Y;
    listener.vOrientTop.z = DS3D_DEFAULTORIENTTOP_Z;
    listener.flDistanceFactor = DS3D_DEFAULTDISTANCEFACTOR;
    listener.flRolloffFactor = DS3D_DEFAULTROLLOFFFACTOR;
    listener.flDopplerFactor = DS3D_DEFAULTDOPPLERFACTOR;

    bool fSetAll = true;
    for (;;)
    {
        const DWORD dwRotationTime = 5000; // spin once every five seconds
        float fRoationTime = dwRotationTime;

        DWORD dwTimeElapsed = GetTickCount() - dwTimeStart;

        const float pi2 = (float)M_PI * 2.0f;
        float fRadians = (dwTimeElapsed % dwRotationTime) / fRoationTime * pi2;

        float fX = sinf(fRadians);
        float fZ = cosf(fRadians);

        if (dwTimeElapsed % 100 == 0)
            DbgPrint("(%g, 0.0, %g), %g Pi Radians\n", fX, fZ, (float)(fRadians / M_PI));


        if (fSetAll)
        {
            listener.vOrientFront.x = fX;
            listener.vOrientFront.y = 0;
            listener.vOrientFront.z = fZ;
            hr = pDSound->SetAllParameters(&listener, DS3D_IMMEDIATE);
        }
        else
        {
            hr = pDSound->SetOrientation(
                            fX, 0, fZ,
                            DS3D_DEFAULTORIENTTOP_X, DS3D_DEFAULTORIENTTOP_Y, DS3D_DEFAULTORIENTTOP_Z,
                            DS3D_IMMEDIATE);
        }
    }
}
