// DLSLoadSaveUtils.Cpp : header file
//

#include "stdafx.h"

#ifndef DLSLOADSAVEUTILS_H
#include "DLSLoadSaveUtils.h"
#endif

BOOL notinrange(long a, long b, long range)
{
    if (a > b)
    {
        return ((a - b) > range);
    }
    return ((b - a) > range);
}

void makeconnection(CONNECTION *pChunk,
					USHORT usSource, 
					USHORT usControl,
                    USHORT usDestination, 
					USHORT usTransform, 
					long lScale)
{
    pChunk->usSource = usSource;
    pChunk->usControl = usControl;
    pChunk->usDestination = usDestination;
    pChunk->usTransform = usTransform;
    pChunk->lScale = lScale;
}


unsigned long GetFilePos(IStream* pIStream)
{
    LARGE_INTEGER   dlibSeekTo;
    ULARGE_INTEGER  dlibNewPos;
    dlibSeekTo.HighPart = 0;
    dlibSeekTo.LowPart = 0;
    pIStream->Seek(dlibSeekTo, STREAM_SEEK_CUR, &dlibNewPos);
    return (dlibNewPos.LowPart);
}

void SetFilePos(IStream* pIStream, unsigned long ulPos)
{
    LARGE_INTEGER   dlibSeekTo;
    dlibSeekTo.HighPart = 0;
    dlibSeekTo.LowPart = ulPos;
    pIStream->Seek(dlibSeekTo, STREAM_SEEK_SET, NULL);
}

DWORD TimeCents2Mils(TCENT tcTime)

{
    DWORD dwMils;
    double dTime = tcTime;
    dTime /= (65536 * 1200);
    dTime = pow(2.0,dTime);
    dTime *= 1000;
    dwMils = (DWORD) (dTime + 0.5);
	if (dwMils > 40000) dwMils = 40000;
    return(dwMils);
}

DWORD PitchCents2Hertz(PCENT pcPitch)

{
    DWORD dwHertz;
    double dTemp = pcPitch;
    dTemp /= 65536;
    dTemp -= 6900;
    dTemp /= 1200;
    dTemp = pow(2.0,dTemp);
    dTemp *= 440;
    dTemp *= 1000.0;
    dwHertz = (DWORD) dTemp;
    return(dwHertz);
}

PCENT Hertz2PitchCents(DWORD dwHertz)
{
	double dTemp = dwHertz;
	dTemp /= 440;
	dTemp /= 1000;
	dTemp = log(dTemp);
	dTemp /= log(2);
	dTemp *= 1200;
	dTemp += 6900;
	dTemp *= 65536;
	return (PCENT)dTemp;
}

DWORD PitchCents2PitchFract(PCENT pcRate,DWORD dwSampleRate)

{
    double fTemp = pcRate;
    fTemp /= 65536;
    fTemp -= 6900;
    fTemp /= 1200;
    fTemp = pow(2.0,fTemp);
    fTemp *= 7381975040.0; // (440*256*16*4096);
    fTemp /= dwSampleRate;
    return (DWORD) (fTemp);
}

TCENT Mils2TimeCents(DWORD dwMils)
{
    double dTime;
    if (dwMils < 1) dwMils = 1;
    dTime = dwMils;
	dTime -= 0.5;
    dTime /= 1000.0;
    dTime = log10(dTime) / 0.301;
    dTime *= (1200 * 65536);
    return (TCENT)dTime;
}
