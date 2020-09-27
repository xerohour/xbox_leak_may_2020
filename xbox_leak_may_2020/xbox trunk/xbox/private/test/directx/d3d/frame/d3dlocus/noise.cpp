/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    noise.cpp

Author:

    Matt Bronder

Description:

    Perlin noise generation.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// CPerlinNoise
//******************************************************************************

UINT CPerlinNoise::m_puPrimes[3][PRIME_TABLE_ENTRIES] = {0};

//******************************************************************************
//
// Method:
//
//     CPerlinNoise
//
// Description:
//
//     Construct a CPerlinNoise object.
//
// Arguments:
//
//     float fPersistence               - Persistence of the noise (the
//                                        amplitude of each frequency)
//
//     UINT uOctaves                    - Number of octaves to add together
//
//     DWORD dwSeed                     - Seed value for the noise
//
//     INTERPOLATIONTYPE irp            - Function to use in interpolating
//                                        between noise sample points
//
//     BOOL bClamp                      - Indicates whether to scale the 
//                                        computed noise function so it falls
//                                        entirely within its given range or to 
//                                        allow the positive interference of 
//                                        successive octaves to naturally exceed 
//                                        the the given range and then clamp the 
//                                        function to that range.
//
// Return Value:
//
//     None.
//
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
//
// Method:
//
//     ~CPerlinNoise
//
// Description:
//
//     Destruct a CPerlinNoise object.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CPerlinNoise::~CPerlinNoise() {
}

//******************************************************************************
//
// Method:
//
//     SetPersistence
//
// Description:
//
//     Set the persistence value for the noise function.
//
// Arguments:
//
//     float fPersistence               - Persistence of the noise (the
//                                        amplitude of each frequency)
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CPerlinNoise::SetPersistence(float fPersistence) {

    m_fPersistence = fPersistence;
}

//******************************************************************************
//
// Method:
//
//     SetOctaves
//
// Description:
//
//     Set the number of octaves to use in the noise function
//
// Arguments:
//
//     UINT uOctaves                    - Number of octaves to add together
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CPerlinNoise::SetOctaves(UINT uOctaves) {

    if (uOctaves > MAX_OCTAVES) {
        return;
    }

    m_uOctaves = uOctaves;

    InitPrimesList(uOctaves, m_dwSeed);
}

//******************************************************************************
//
// Method:
//
//     SetSeed
//
// Description:
//
//     Set the pseudo-random seed value to use in sampling noise values at
//     discrete points.
//
// Arguments:
//
//     DWORD dwSeed                     - Seed value for the noise
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CPerlinNoise::SetSeed(DWORD dwSeed) {

    m_dwSeed = dwSeed;

    InitPrimesList(m_uOctaves, dwSeed);
}

//******************************************************************************
//
// Method:
//
//     SetInterpolation
//
// Description:
//
//     Identify the interpolation function to be used in calculating noise 
//     values between sample points.
//
// Arguments:
//
//     INTERPOLATIONTYPE irp            - Function to use in interpolating
//                                        between noise sample points
//
// Return Value:
//
//     None.
//
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
//
// Method:
//
//     EnableClamp
//
// Description:
//
//     Indicate whether to scale or clamp the noise function to the given range.
//
// Arguments:
//
//     BOOL bEnable                     - Indicates whether to scale the 
//                                        computed noise function so it falls
//                                        entirely within its given range or to 
//                                        allow the positive interference of 
//                                        successive octaves to naturally exceed 
//                                        the the given range and then clamp the 
//                                        function to that range.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CPerlinNoise::EnableClamp(BOOL bEnable) {

    m_bClamp = bEnable;
}

//******************************************************************************
//
// Method:
//
//     SetRange
//
// Description:
//
//     Set the upper and lower limits for the noise function.
//
// Arguments:
//
//     float fMin                       - Minimum value for the noise
//
//     float fMax                       - Maximum value for the noise
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CPerlinNoise::SetRange(float fMin, float fMax) {

    if (fMax > fMin) {

        m_fRangeBase = fMin;
        m_fRangeMagnitude = fMax - fMin;
    }
}

//******************************************************************************
//
// Method:
//
//     InterpolateLinear
//
// Description:
//
//     Calculate the value of a point lying a given distance on a line between
//     two given end points.
//
// Arguments:
//
//     float f1                         - Value of the first end point
//
//     float f2                         - Value of the second end point
//
//     float fBeta                      - Percentage of the distance between the 
//                                        points (where 0.0 is f1 and 1.0 is f2)
//
// Return Value:
//
//     Interpolated value between the given points.
//
//******************************************************************************
float CPerlinNoise::InterpolateLinear(float f1, float f2, float fBeta) {

    return f1 * (1.0f - fBeta) + f2 * fBeta;
}

//******************************************************************************
//
// Method:
//
//     InterpolateCosine
//
// Description:
//
//     Calculate the value of a point lying a given distance between two given
//     end points using the cosine of the percentage of the distance to provide
//     a smooth curve between the points.
//
// Arguments:
//
//     float f1                         - Value of the first end point
//
//     float f2                         - Value of the second end point
//
//     float fBeta                      - Percentage of the distance between the 
//                                        points (where 0.0 is f1 and 1.0 is f2)
//
// Return Value:
//
//     Interpolated value between the given points.
//
//******************************************************************************
float CPerlinNoise::InterpolateCosine(float f1, float f2, float fBeta) {

    float f = (1.0f - (float)cos(fBeta * 3.14159265359f)) * 0.5f;
    return f1 * (1.0f - f) + f2 * f;
}

//******************************************************************************
//
// Method:
//
//     Rand32
//
// Description:
//
//     Return a pseudo-random 32 bit number corresponding to the given seed.
//
// Arguments:
//
//     DWORD dwSeed                     - Seed value
//
// Return Value:
//
//     The random number corresponding to the given seed.
//
//******************************************************************************
DWORD CPerlinNoise::Rand32(DWORD dwSeed) {

    dwSeed = (dwSeed << 13) ^ dwSeed;
    return ((dwSeed * (dwSeed * dwSeed * 9973 + 769999) + 1376309969) & 0xFFFFFFFF);
}

//******************************************************************************
//
// Method:
//
//     IsPrime
//
// Description:
//
//     Evaluate the given integer and determine whether or not it is a prime
//     number.
//
// Arguments:
//
//     UINT u                           - Unsigned integer to evaluate
//
// Return Value:
//
//     TRUE if the number is a prime, FALSE if not.
//
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
//
// Method:
//
//     InitPrimesList
//
// Description:
//
//     Initialize a table of prime numbers to enable separate pseudo-random
//     noise functions to be used for each octave of perlin noise generation.
//
// Arguments:
//
//     UINT uOctaves                    - Number of octaves used in noise
//                                        generation.
//
//     DWORD dwSeed                     - Seed to use in constructing the table
//
// Return Value:
//
//     None.
//
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
//
// Method:
//
//     CPerlinNoise1D
//
// Description:
//
//     Construct a CPerlinNoise1D object.
//
// Arguments:
//
//     float fPersistence               - Persistence of the noise (the
//                                        amplitude of each frequency)
//
//     UINT uOctaves                    - Number of octaves to add together
//
//     DWORD dwSeed                     - Seed value for the noise
//
//     INTERPOLATIONTYPE irp            - Function to use in interpolating
//                                        between noise sample points
//
//     BOOL bClamp                      - Indicates whether to scale the 
//                                        computed noise function so it falls
//                                        entirely within its given range or to 
//                                        allow the positive interference of 
//                                        successive octaves to naturally exceed 
//                                        the the given range and then clamp the 
//                                        function to that range.
//
//     BOOL bSmooth                     - Indicates whether or not to use a
//                                        filter to smooth the noise
//
// Return Value:
//
//     None.
//
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
//
// Method:
//
//     ~CPerlinNoise1D
//
// Description:
//
//     Destruct a CPerlinNoise1D object.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CPerlinNoise1D::~CPerlinNoise1D() {
}

//******************************************************************************
//
// Method:
//
//     EnableSmoothing
//
// Description:
//
//     Enable or disable the use of a filter to smooth the noise.
//
// Arguments:
//
//     BOOL bEnable                     - TRUE to enable the filter, FALSE
//                                        to disable it
//
// Return Value:
//
//     None.
//
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
//
// Method:
//
//     GetIntensity
//
// Description:
//
//     Calculate the intensity (amplitude) of the noise function at the given
//     coordinate.
//
// Arguments:
//
//     float x                          - x coordinate
//
// Return Value:
//
//     The intensity of the noise at the given coordinate.
//
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
//
// Method:
//
//     InterpolatedNoise1D
//
// Description:
//
//     Calculate the noise value that corresponds to the given coordinate
//     for the given octave.  Noise values are calculated
//     for the integer points that bound the given coordinate and the noise
//     value for the coordinate is then interpolated from the results based
//     on where the coordinate lies between the bounding points and on the
//     interpolation function set for the object.
//
// Arguments:
//
//     float x                          - x coordinate
//
// Return Value:
//
//     The interpolated value of the noise at the given coordinate.
//
//******************************************************************************
float CPerlinNoise1D::InterpolatedNoise1D(float x) {

    int     nx;
    float   fx;

    nx = (int)floor(x);
    fx = x - (float)nx;

    return (this->*m_pfnInterpolate)((this->*m_pfnNoise)(nx), (this->*m_pfnNoise)(nx+1), fx);
}

//******************************************************************************
//
// Method:
//
//     Noise1D
//
// Description:
//
//     Calculate the noise value that corresponds to the given coordinate
//     for the given octave.
//
// Arguments:
//
//     int x                            - x coordinate
//
// Return Value:
//
//     The value of the noise at the given coordinate.
//
//******************************************************************************
float CPerlinNoise1D::Noise1D(int x) {

    DWORD   dwSeed;

    dwSeed = (x << 13) ^ x;

    return (1.0f - ((dwSeed * (dwSeed * dwSeed * m_plPrimesList[m_uOctave].uPrime1 + m_plPrimesList[m_uOctave].uPrime2) + m_plPrimesList[m_uOctave].uPrime3) & 0x7FFFFFFF) / 1073741824.0f);
}

//******************************************************************************
//
// Method:
//
//     SmoothNoise1D
//
// Description:
//
//     Calculate the filtered noise value that corresponds to the given 
//     coordinate for the given octave.  Noise values are calculated
//     for neighboring coordinates and then used to bias the noise for the
//     given coordinate.
//
// Arguments:
//
//     int x                            - x coordinate
//
//     int y                            - y coordinate
//
// Return Value:
//
//     The value of the filtered noise at the given coordinate.
//
//******************************************************************************
float CPerlinNoise1D::SmoothNoise1D(int x) {

    return Noise1D(x) / 2.0f + Noise1D(x-1) / 4.0f + Noise1D(x+1) / 4.0f;
}

//******************************************************************************
// CPerlinNoise2D
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CPerlinNoise2D
//
// Description:
//
//     Construct a CPerlinNoise2D object.
//
// Arguments:
//
//     float fPersistence               - Persistence of the noise (the
//                                        amplitude of each frequency)
//
//     UINT uOctaves                    - Number of octaves to add together
//
//     DWORD dwSeed                     - Seed value for the noise
//
//     INTERPOLATIONTYPE irp            - Function to use in interpolating
//                                        between noise sample points
//
//     BOOL bClamp                      - Indicates whether to scale the 
//                                        computed noise function so it falls
//                                        entirely within its given range or to 
//                                        allow the positive interference of 
//                                        successive octaves to naturally exceed 
//                                        the the given range and then clamp the 
//                                        function to that range.
//
//     BOOL bSmooth                     - Indicates whether or not to use a
//                                        filter to smooth the noise
//
// Return Value:
//
//     None.
//
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
//
// Method:
//
//     ~CPerlinNoise2D
//
// Description:
//
//     Destruct a CPerlinNoise2D object.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CPerlinNoise2D::~CPerlinNoise2D() {
}

//******************************************************************************
//
// Method:
//
//     EnableSmoothing
//
// Description:
//
//     Enable or disable the use of a filter to smooth the noise.
//
// Arguments:
//
//     BOOL bEnable                     - TRUE to enable the filter, FALSE
//                                        to disable it
//
// Return Value:
//
//     None.
//
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
//
// Method:
//
//     GetIntensity
//
// Description:
//
//     Calculate the intensity (amplitude) of the noise function at the given
//     coordinates.
//
// Arguments:
//
//     float x                          - x coordinate
//
//     float y                          - y coordinate
//
// Return Value:
//
//     The intensity of the noise at the given coordinates.
//
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
//
// Method:
//
//     InterpolatedNoise2D
//
// Description:
//
//     Calculate the noise value that corresponds to the given two 
//     dimesnional coordinates for the given octave.  Noise values are calculated
//     for the integer points that bound the given coordinates and the noise
//     value for the coordinates are then interpolated from the results based
//     on where the coordinates lie between the bounding points and on the
//     interpolation function set for the object.
//
// Arguments:
//
//     float x                          - x coordinate
//
//     float y                          - y coordinate
//
// Return Value:
//
//     The interpolated value of the noise at the given coordinates.
//
//******************************************************************************
float CPerlinNoise2D::InterpolatedNoise2D(float x, float y) {

    int     nx, ny;
    float   fx, fy;
    float   i1, i2;

    nx = (int)floor(x);
    ny = (int)floor(y);
    fx = x - (float)nx;
    fy = y - (float)ny;

    i1 = (this->*m_pfnInterpolate)((this->*m_pfnNoise)(nx, ny  ), (this->*m_pfnNoise)(nx+1, ny  ), fx);
    i2 = (this->*m_pfnInterpolate)((this->*m_pfnNoise)(nx, ny+1), (this->*m_pfnNoise)(nx+1, ny+1), fx);

    return (this->*m_pfnInterpolate)(i1, i2, fy);
}

//******************************************************************************
//
// Method:
//
//     Noise2D
//
// Description:
//
//     Calculate the noise value that corresponds to the given two dimesnional
//     coordinates for the given octave.
//
// Arguments:
//
//     int x                            - x coordinate
//
//     int y                            - y coordinate
//
// Return Value:
//
//     The value of the noise at the given coordinates.
//
//******************************************************************************
float CPerlinNoise2D::Noise2D(int x, int y) {

    DWORD   dwSeed;

    dwSeed = x + y * 57;
    dwSeed = (dwSeed << 13) ^ dwSeed;

    return (1.0f - ((dwSeed * (dwSeed * dwSeed * m_plPrimesList[m_uOctave].uPrime1 + m_plPrimesList[m_uOctave].uPrime2) + m_plPrimesList[m_uOctave].uPrime3) & 0x7FFFFFFF) / 1073741824.0f);
}

//******************************************************************************
//
// Method:
//
//     SmoothNoise2D
//
// Description:
//
//     Calculate the filtered noise value that corresponds to the given two 
//     dimesnional coordinates for the given octave.  Noise values are calculated
//     for neighboring coordinates and then used to bias the noise for the
//     given coordinates.
//
// Arguments:
//
//     int x                            - x coordinate
//
//     int y                            - y coordinate
//
// Return Value:
//
//     The value of the filtered noise at the given coordinates.
//
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
//
// Method:
//
//     CPerlinNoise3D
//
// Description:
//
//     Construct a CPerlinNoise3D object.
//
// Arguments:
//
//     float fPersistence               - Persistence of the noise (the
//                                        amplitude of each frequency)
//
//     UINT uOctaves                    - Number of octaves to add together
//
//     DWORD dwSeed                     - Seed value for the noise
//
//     INTERPOLATIONTYPE irp            - Function to use in interpolating
//                                        between noise sample points
//
//     BOOL bClamp                      - Indicates whether to scale the 
//                                        computed noise function so it falls
//                                        entirely within its given range or to 
//                                        allow the positive interference of 
//                                        successive octaves to naturally exceed 
//                                        the the given range and then clamp the 
//                                        function to that range.
//
//     BOOL bSmooth                     - Indicates whether or not to use a
//                                        filter to smooth the noise
//
// Return Value:
//
//     None.
//
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
//
// Method:
//
//     ~CPerlinNoise3D
//
// Description:
//
//     Destruct a CPerlinNoise3D object.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CPerlinNoise3D::~CPerlinNoise3D() {
}

//******************************************************************************
//
// Method:
//
//     EnableSmoothing
//
// Description:
//
//     Enable or disable the use of a filter to smooth the noise.
//
// Arguments:
//
//     BOOL bEnable                     - TRUE to enable the filter, FALSE
//                                        to disable it
//
// Return Value:
//
//     None.
//
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
//
// Method:
//
//     GetIntensity
//
// Description:
//
//     Calculate the intensity (amplitude) of the noise function at the given
//     coordinates.
//
// Arguments:
//
//     float x                          - x coordinate
//
//     float y                          - y coordinate
//
//     float z                          - z coordinate
//
// Return Value:
//
//     The intensity of the noise at the given coordinates.
//
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
//
// Method:
//
//     InterpolatedNoise3D
//
// Description:
//
//     Calculate the noise value that corresponds to the given three
//     dimesnional coordinates for the given octave.  Noise values are calculated
//     for the integer points that bound the given coordinates and the noise
//     value for the coordinates are then interpolated from the results based
//     on where the coordinates lie between the bounding points and on the
//     interpolation function set for the object.
//
// Arguments:
//
//     float x                          - x coordinate
//
//     float y                          - y coordinate
//
//     float z                          - z coordinate
//
// Return Value:
//
//     The interpolated value of the noise at the given coordinates.
//
//******************************************************************************
float CPerlinNoise3D::InterpolatedNoise3D(float x, float y, float z) {

    int     nx, ny, nz;
    float   fx, fy, fz;
    float   i1, i2, i3, i4, i5, i6;

    nx = (int)floor(x);
    ny = (int)floor(y);
    nz = (int)floor(z);
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
//
// Method:
//
//     Noise3D
//
// Description:
//
//     Calculate the noise value that corresponds to the given three dimesnional
//     coordinates for the given octave.
//
// Arguments:
//
//     int x                            - x coordinate
//
//     int y                            - y coordinate
//
//     int z                            - z coordinate
//
// Return Value:
//
//     The value of the noise at the given coordinates.
//
//******************************************************************************
float CPerlinNoise3D::Noise3D(int x, int y, int z) {

    DWORD   dwSeed;

    dwSeed = x + y * 57 + z * 117;
    dwSeed = (dwSeed << 13) ^ dwSeed;

    return (1.0f - ((dwSeed * (dwSeed * dwSeed * m_plPrimesList[m_uOctave].uPrime1 + m_plPrimesList[m_uOctave].uPrime2) + m_plPrimesList[m_uOctave].uPrime3) & 0x7FFFFFFF) / 1073741824.0f);
}

//******************************************************************************
//
// Method:
//
//     SmoothNoise3D
//
// Description:
//
//     Calculate the filtered noise value that corresponds to the given three
//     dimesnional coordinates for the given octave.  Noise values are calculated
//     for neighboring coordinates and then used to bias the noise for the
//     given coordinates.
//
// Arguments:
//
//     int x                            - x coordinate
//
//     int y                            - y coordinate
//
//     int z                            - z coordinate
//
// Return Value:
//
//     The value of the filtered noise at the given coordinates.
//
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
