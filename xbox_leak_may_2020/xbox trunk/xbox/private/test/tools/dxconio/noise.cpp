
#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <math.h>
#include "noise.h"

namespace DXCONIO {

//******************************************************************************
// CPerlinNoise
//******************************************************************************

UINT CPerlinNoise::m_puPrimes[3][PRIME_TABLE_ENTRIES] = {0};

//******************************************************************************
CPerlinNoise::CPerlinNoise(float fPersistence, UINT uOctaves, DWORD dwSeed, 
                                            INTERPOLATIONTYPE irp, BOOL bClamp) 
{
    if (!m_puPrimes[0][0]) {

        UINT i, j, k;
        UINT uBase[] = {
                10000,
                770000,
                1376310000,
        };

        // Find the first PRIME_TABLE_ENTRIES prime numbers that proceed 3 base numbers
        for (i = 0; i < 3; i++) {
            for (j = 0, k = uBase[i]; j < PRIME_TABLE_ENTRIES; k++) {
                if (IsPrime(k)) {
                    m_puPrimes[i][j++] = k;
                }
            }
        }
    }

    m_fPersistence = fPersistence;
    m_uOctaves = uOctaves;
    m_dwSeed = dwSeed;
    switch (irp) {
        case INTERPOLATE_LINEAR:
            m_pfnInterpolate = CPerlinNoise::InterpolateLinear;
            break;
        case INTERPOLATE_COSINE:
        default:
            m_pfnInterpolate = CPerlinNoise::InterpolateCosine;
            break;
    }
    m_bClamp = bClamp;
    m_fRangeBase = 0.0f;
    m_fRangeMagnitude = 1.0f;

    if (uOctaves > MAX_OCTAVES) {
        DebugBreak();
    }

    InitPrimesList(uOctaves, dwSeed);
}

//******************************************************************************
CPerlinNoise::~CPerlinNoise() {
}

//******************************************************************************
void CPerlinNoise::SetPersistence(float fPersistence) {

    m_fPersistence = fPersistence;
}

//******************************************************************************
void CPerlinNoise::SetOctaves(UINT uOctaves) {

    if (uOctaves > MAX_OCTAVES) {
        return;
    }

    m_uOctaves = uOctaves;

    InitPrimesList(uOctaves, m_dwSeed);
}

//******************************************************************************
void CPerlinNoise::SetSeed(DWORD dwSeed) {

    m_dwSeed = dwSeed;

    InitPrimesList(m_uOctaves, dwSeed);
}

//******************************************************************************
void CPerlinNoise::SetInterpolation(INTERPOLATIONTYPE irp) {

    switch (irp) {
        case INTERPOLATE_LINEAR:
            m_pfnInterpolate = CPerlinNoise::InterpolateLinear;
            break;
        case INTERPOLATE_COSINE:
            m_pfnInterpolate = CPerlinNoise::InterpolateCosine;
            break;
    }
}

//******************************************************************************
void CPerlinNoise::EnableClamp(BOOL bEnable) {

    m_bClamp = bEnable;
}

//******************************************************************************
void CPerlinNoise::SetRange(float fMin, float fMax) {

    if (fMax > fMin) {

        m_fRangeBase = fMin;
        m_fRangeMagnitude = fMax - fMin;
    }
}

//******************************************************************************
float CPerlinNoise::InterpolateLinear(float f1, float f2, float fBeta) {

    return f1 * (1.0f - fBeta) + f2 * fBeta;
}

//******************************************************************************
float CPerlinNoise::InterpolateCosine(float f1, float f2, float fBeta) {

    float f = (1.0f - (float)cos(fBeta * 3.14159265359f)) * 0.5f;
    return f1 * (1.0f - f) + f2 * f;
}

//******************************************************************************
DWORD CPerlinNoise::Rand32(DWORD dwSeed) {

    dwSeed = (dwSeed << 13) ^ dwSeed;
    return ((dwSeed * (dwSeed * dwSeed * 9973 + 769999) + 1376309969) & 0xFFFFFFFF);
}

//******************************************************************************
BOOL CPerlinNoise::IsPrime(UINT u) {

    UINT i = 1, j, k = 0;

    do {
        j = k;
        k = i;
        i = (k + (u / k));
        i >>= 1;
    } while(j != i);

    for (j = 2; j <= i; j++) {
        if (u % j == 0) {
            return FALSE;
        }
    }

    return TRUE;
}

//******************************************************************************
void CPerlinNoise::InitPrimesList(UINT uOctaves, DWORD dwSeed) {

    DWORD dwRandFunc, dwRand;
    UINT  i;

    for (i = 0, dwRandFunc = dwSeed; i < uOctaves; i++) {

        dwRand = Rand32(dwRandFunc);
        m_plPrimesList[i].uPrime1 = m_puPrimes[0][dwRand % PRIME_TABLE_ENTRIES];
        dwRand = Rand32(dwRand);
        m_plPrimesList[i].uPrime2 = m_puPrimes[1][dwRand % PRIME_TABLE_ENTRIES];
        dwRand = Rand32(dwRand);
        m_plPrimesList[i].uPrime3 = m_puPrimes[2][dwRand % PRIME_TABLE_ENTRIES];
        dwRandFunc = Rand32(dwRandFunc);
    }
}

//******************************************************************************
// CPerlinNoise1D
//******************************************************************************

//******************************************************************************
CPerlinNoise1D::CPerlinNoise1D(float fPersistence, UINT uOctaves, DWORD dwSeed, 
                    INTERPOLATIONTYPE irp, BOOL bClamp, BOOL bSmooth) :
                    CPerlinNoise(fPersistence, uOctaves, dwSeed, irp, bClamp)
{
    if (bSmooth) {
        m_pfnNoise = CPerlinNoise1D::SmoothNoise1D;
    }
    else {
        m_pfnNoise = CPerlinNoise1D::Noise1D;
    }
}

//******************************************************************************
CPerlinNoise1D::~CPerlinNoise1D() {
}

//******************************************************************************
void CPerlinNoise1D::EnableSmoothing(BOOL bEnable) {

    if (bEnable) {
        m_pfnNoise = CPerlinNoise1D::SmoothNoise1D;
    }
    else {
        m_pfnNoise = CPerlinNoise1D::Noise1D;
    }
}

//******************************************************************************
float CPerlinNoise1D::GetIntensity(float x) {

    float fIntensity = 0.0f, fAmplitude, fFrequency, fTotalAmp = 0.0f;

    fAmplitude = 1.0f;

    for (m_uOctave = 0; m_uOctave < m_uOctaves; m_uOctave++) {

//        fFrequency = (float)pow(2.0, (double)m_uOctave);
        fFrequency = (float)(0x1 << m_uOctave);
//        fAmplitude = (float)pow(m_fPersistence, (double)m_uOctave);
        fIntensity = fIntensity + InterpolatedNoise1D(x * fFrequency) * fAmplitude;
        fTotalAmp += fAmplitude;
        fAmplitude *= m_fPersistence;
    }

    if (m_bClamp) {
        fTotalAmp = 1.0f;
        if (fIntensity < -1.0f) {
            fIntensity = -1.0f;
        }
        else if (fIntensity > 1.0f) {
            fIntensity = 1.0f;
        }
    }

    return m_fRangeBase + m_fRangeMagnitude * ((fIntensity + fTotalAmp) / (fTotalAmp * 2.0f));
}

//******************************************************************************
float CPerlinNoise1D::InterpolatedNoise1D(float x) {

    int     nx;
    float   fx;

    nx = (int)x;
    fx = x - (float)nx;

    return (this->*m_pfnInterpolate)((this->*m_pfnNoise)(nx), (this->*m_pfnNoise)(nx+1), fx);
}

//******************************************************************************
float CPerlinNoise1D::Noise1D(int x) {

    DWORD   dwSeed;

    dwSeed = (x << 13) ^ x;

    return (1.0f - ((dwSeed * (dwSeed * dwSeed * m_plPrimesList[m_uOctave].uPrime1 + m_plPrimesList[m_uOctave].uPrime2) + m_plPrimesList[m_uOctave].uPrime3) & 0x7FFFFFFF) / 1073741824.0f);
}

//******************************************************************************
float CPerlinNoise1D::SmoothNoise1D(int x) {

    return Noise1D(x) / 2.0f + Noise1D(x-1) / 4.0f + Noise1D(x+1) / 4.0f;
}

//******************************************************************************
// CPerlinNoise2D
//******************************************************************************

//******************************************************************************
CPerlinNoise2D::CPerlinNoise2D(float fPersistence, UINT uOctaves, DWORD dwSeed, 
                    INTERPOLATIONTYPE irp, BOOL bClamp, BOOL bSmooth) :
                    CPerlinNoise(fPersistence, uOctaves, dwSeed, irp, bClamp)
{
    if (bSmooth) {
        m_pfnNoise = CPerlinNoise2D::SmoothNoise2D;
    }
    else {
        m_pfnNoise = CPerlinNoise2D::Noise2D;
    }
}

//******************************************************************************
CPerlinNoise2D::~CPerlinNoise2D() {
}

//******************************************************************************
void CPerlinNoise2D::EnableSmoothing(BOOL bEnable) {

    if (bEnable) {
        m_pfnNoise = CPerlinNoise2D::SmoothNoise2D;
    }
    else {
        m_pfnNoise = CPerlinNoise2D::Noise2D;
    }
}

//******************************************************************************
float CPerlinNoise2D::GetIntensity(float x, float y) {

    float fIntensity = 0.0f, fAmplitude, fFrequency, fTotalAmp = 0.0f;

    fAmplitude = 1.0f;

    for (m_uOctave = 0; m_uOctave < m_uOctaves; m_uOctave++) {

//        fFrequency = (float)pow(2.0, (double)m_uOctave);
        fFrequency = (float)(0x1 << m_uOctave);
//        fAmplitude = (float)pow(m_fPersistence, (double)m_uOctave);
        fIntensity = fIntensity + InterpolatedNoise2D(x * fFrequency, y * fFrequency) * fAmplitude;
        fTotalAmp += fAmplitude;
        fAmplitude *= m_fPersistence;
    }

    if (m_bClamp) {
        fTotalAmp = 1.0f;
        if (fIntensity < -1.0f) {
            fIntensity = -1.0f;
        }
        else if (fIntensity > 1.0f) {
            fIntensity = 1.0f;
        }
    }

    return m_fRangeBase + m_fRangeMagnitude * ((fIntensity + fTotalAmp) / (fTotalAmp * 2.0f));
}

//******************************************************************************
float CPerlinNoise2D::InterpolatedNoise2D(float x, float y) {

    int     nx, ny;
    float   fx, fy;
    float   i1, i2;

    nx = (int)x;
    ny = (int)y;
    fx = x - (float)nx;
    fy = y - (float)ny;

    i1 = (this->*m_pfnInterpolate)((this->*m_pfnNoise)(nx, ny  ), (this->*m_pfnNoise)(nx+1, ny  ), fx);
    i2 = (this->*m_pfnInterpolate)((this->*m_pfnNoise)(nx, ny+1), (this->*m_pfnNoise)(nx+1, ny+1), fx);

    return (this->*m_pfnInterpolate)(i1, i2, fy);
}

//******************************************************************************
float CPerlinNoise2D::Noise2D(int x, int y) {

    DWORD   dwSeed;

    dwSeed = x + y * 57;
    dwSeed = (dwSeed << 13) ^ dwSeed;

    return (1.0f - ((dwSeed * (dwSeed * dwSeed * m_plPrimesList[m_uOctave].uPrime1 + m_plPrimesList[m_uOctave].uPrime2) + m_plPrimesList[m_uOctave].uPrime3) & 0x7FFFFFFF) / 1073741824.0f);
}

//******************************************************************************
float CPerlinNoise2D::SmoothNoise2D(int x, int y) {

    float fCorners, fSides, fCenter;

    fCorners = (Noise2D(x-1, y-1) + Noise2D(x+1, y-1) + 
                Noise2D(x-1, y+1) + Noise2D(x+1, y+1)) / 16.0f;
    fSides =   (Noise2D(x-1, y) + Noise2D(x+1, y) +
                Noise2D(x, y-1) + Noise2D(x, y+1)) / 8.0f;
    fCenter =   Noise2D(x, y) / 4.0f;

    return fCorners + fSides + fCenter;
}

//******************************************************************************
// CPerlinNoise3D
//******************************************************************************

//******************************************************************************
CPerlinNoise3D::CPerlinNoise3D(float fPersistence, UINT uOctaves, DWORD dwSeed, 
                    INTERPOLATIONTYPE irp, BOOL bClamp, BOOL bSmooth) :
                    CPerlinNoise(fPersistence, uOctaves, dwSeed, irp, bClamp)
{
    if (bSmooth) {
        m_pfnNoise = CPerlinNoise3D::SmoothNoise3D;
    }
    else {
        m_pfnNoise = CPerlinNoise3D::Noise3D;
    }
}

//******************************************************************************
CPerlinNoise3D::~CPerlinNoise3D() {
}

//******************************************************************************
void CPerlinNoise3D::EnableSmoothing(BOOL bEnable) {

    if (bEnable) {
        m_pfnNoise = CPerlinNoise3D::SmoothNoise3D;
    }
    else {
        m_pfnNoise = CPerlinNoise3D::Noise3D;
    }
}

//******************************************************************************
float CPerlinNoise3D::GetIntensity(float x, float y, float z) {

    float fIntensity = 0.0f, fAmplitude, fFrequency, fTotalAmp = 0.0f;

    fAmplitude = 1.0f;

    for (m_uOctave = 0; m_uOctave < m_uOctaves; m_uOctave++) {

//        fFrequency = (float)pow(2.0, (double)m_uOctave);
        fFrequency = (float)(0x1 << m_uOctave);
//        fAmplitude = (float)pow(m_fPersistence, (double)m_uOctave);
        fIntensity = fIntensity + InterpolatedNoise3D(x * fFrequency, y * fFrequency, z * fFrequency) * fAmplitude;
        fTotalAmp += fAmplitude;
        fAmplitude *= m_fPersistence;
    }

    if (m_bClamp) {
        fTotalAmp = 1.0f;
        if (fIntensity < -1.0f) {
            fIntensity = -1.0f;
        }
        else if (fIntensity > 1.0f) {
            fIntensity = 1.0f;
        }
    }

    return m_fRangeBase + m_fRangeMagnitude * ((fIntensity + fTotalAmp) / (fTotalAmp * 2.0f));
}

//******************************************************************************
float CPerlinNoise3D::InterpolatedNoise3D(float x, float y, float z) {

    int     nx, ny, nz;
    float   fx, fy, fz;
    float   i1, i2, i3, i4, i5, i6;

    nx = (int)x;
    ny = (int)y;
    nz = (int)z;
    fx = x - (float)nx;
    fy = y - (float)ny;
    fz = z - (float)nz;

    i3 = (this->*m_pfnInterpolate)((this->*m_pfnNoise)(nx,   ny,   nz),   
                                   (this->*m_pfnNoise)(nx+1, ny  , nz),   fx);
    i4 = (this->*m_pfnInterpolate)((this->*m_pfnNoise)(nx,   ny+1, nz),   
                                   (this->*m_pfnNoise)(nx+1, ny+1, nz),   fx);

    i5 = (this->*m_pfnInterpolate)((this->*m_pfnNoise)(nx,   ny,   nz+1), 
                                   (this->*m_pfnNoise)(nx+1, ny,   nz+1), fx);
    i6 = (this->*m_pfnInterpolate)((this->*m_pfnNoise)(nx,   ny+1, nz+1), 
                                   (this->*m_pfnNoise)(nx+1, ny+1, nz+1), fx);

    i1 = (this->*m_pfnInterpolate)(i3, i4, fy);
    i2 = (this->*m_pfnInterpolate)(i5, i6, fy);

    return (this->*m_pfnInterpolate)(i1, i2, fz);
}

//******************************************************************************
float CPerlinNoise3D::Noise3D(int x, int y, int z) {

    DWORD   dwSeed;

    dwSeed = x + y * 57 + z * 117;
    dwSeed = (dwSeed << 13) ^ dwSeed;

    return (1.0f - ((dwSeed * (dwSeed * dwSeed * m_plPrimesList[m_uOctave].uPrime1 + m_plPrimesList[m_uOctave].uPrime2) + m_plPrimesList[m_uOctave].uPrime3) & 0x7FFFFFFF) / 1073741824.0f);
}

//******************************************************************************
float CPerlinNoise3D::SmoothNoise3D(int x, int y, int z) {

    float fEnds, fCorners, fSides, fCenter;

    fEnds =    (Noise3D(x-1, y-1, z-1) + Noise3D(x+1, y-1, z-1) +
                Noise3D(x-1, y+1, z-1) + Noise3D(x+1, y+1, z-1) +
                Noise3D(x-1, y-1, z+1) + Noise3D(x+1, y-1, z+1) +
                Noise3D(x-1, y+1, z+1) + Noise3D(x+1, y+1, z+1)) / 64.0f;
    fCorners = (Noise3D(x-1, y-1, z) + Noise3D(x+1, y-1, z) + 
                Noise3D(x-1, y+1, z) + Noise3D(x+1, y+1, z) +
                Noise3D(x-1, y, z-1) + Noise3D(x+1, y, z-1) +
                Noise3D(x, y-1, z-1) + Noise3D(x, y+1, z-1) +
                Noise3D(x-1, y, z+1) + Noise3D(x+1, y, z+1) +
                Noise3D(x, y-1, z+1) + Noise3D(x, y+1, z+1)) / 32.0f;
    fSides =   (Noise3D(x-1, y, z) + Noise3D(x+1, y, z) +
                Noise3D(x, y-1, z) + Noise3D(x, y+1, z) +
                Noise3D(x, y, z-1) + Noise3D(x, y, z+1)) / 16.0f;
    fCenter =   Noise3D(x, y, z) / 8.0f;

    return fEnds + fCorners + fSides + fCenter;
}

} // namespace DXCONIO
