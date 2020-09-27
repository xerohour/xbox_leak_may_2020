/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    App to test TV output

--*/

#if 1

#include <ntos.h>
#include <av.h>
#include <smcdef.h>
#include <xtl.h>

#else 

#include <xtl.h>

#endif 

#include <xgraphics.h>
#include <xfont.h>
#include <stdio.h>
#include <xbdm.h>

//typedef unsigned long NTSTATUS;

#include <av.h>

void    WINAPI D3DDevice_EnableCC(BOOL Enable);
void    WINAPI D3DDevice_SendCC(BOOL Field, BYTE cc1, BYTE cc2);
void    WINAPI D3DDevice_GetCCStatus(BOOL *pField1, BOOL *pField2);

IDirect3DSurface8 * g_pBack;
IDirect3DSurface8 * g_pOverlay;

HANDLE              g_hEvent;
CRITICAL_SECTION    g_Mutex;

DWORD               g_iMode;
DWORD               g_VpsTime;
DWORD               g_VpsCount;
DWORD               g_FpsTime;
LONG                g_FpsCount;
D3DDISPLAYMODE      g_Mode;
BOOL                g_Overlay;
DWORD               g_ModeTime;
BOOL                g_EnableCC;
DWORD               g_SavedAvInfo;

DWORD               g_ShowSweep;
DWORD               g_SweepTime;
double              g_SweepRate = 2.0; // seconds per sweep (full cycle)
double              g_SweepAttenuation = 0.02;
DWORD               g_SweepWidth = 30; // Lines, max 60

#define BUFFER_SIZE 60

DWORD               g_LineBuffer[BUFFER_SIZE][1920];
DWORD               g_LineNumber[BUFFER_SIZE];

HANDLE g_InputHandle;

BOOL g_A;
BOOL g_B;
BOOL g_X;
BOOL g_Y;
BOOL g_Black;
BOOL g_White;

IDirect3DSurface8 * g_ScreenOdd;
IDirect3DSurface8 * g_ScreenEven;

extern DWORD D3D__AvInfo;
extern volatile DWORD *D3D__GpuReg;

ULONG
_cdecl
DbgPrint(
    PCH Format,
    ...
    );

void GetScreenChecksum(
    DWORD *pRed,
    DWORD *pGreen,
    DWORD *pBlue
    );

//============================================================================
// Helpers to create the test pattern.
//============================================================================

double SecondsPerPixel()
{
    double secondsperpixel;
    double pixelsperline;
    double secondsperline;

    pixelsperline = g_Mode.Width;

    // These values are measured.
    switch(D3D__AvInfo & AV_STANDARD_MASK)
    {
    case AV_STANDARD_PAL_I:

        if (g_Mode.RefreshRate == 50)
        {
            if (g_Mode.Flags & D3DPRESENTFLAG_10X11PIXELASPECTRATIO)
            {
                secondsperline = 46.428e-6;
            }
            else
            {
                secondsperline = 51.326e-6;
            }

            break;
        }

        // fall through

    case AV_STANDARD_NTSC_M:
    case AV_STANDARD_NTSC_J:
    case AV_STANDARD_PAL_M:

        if (g_Mode.Flags & D3DPRESENTFLAG_10X11PIXELASPECTRATIO)
        {
            secondsperline = 47.633e-6;
        }
        else
        {
            secondsperline = 52.426e-6;
        }

        break;

    }

    secondsperpixel = secondsperline / pixelsperline;

    return secondsperpixel;
}

// This function returns the natural frequency in 
double Kw (double fFreq)
{
    double radiansperpixel;
    double radiansperwave;
    double wavespersecond;
    double secondsperpixel;

    radiansperwave  = 2.0 * 3.1415;
    wavespersecond  = fFreq;
    secondsperpixel = SecondsPerPixel();

    radiansperpixel = wavespersecond * secondsperpixel * radiansperwave;

    return radiansperpixel;

//    return (2.0*3.1415*fFreq/(30*525/1.001)/640.0);
}

DWORD IREtoDW (double dfIre)
{
    return ((DWORD)(dfIre*255.0/100.0));
}

DWORD IREtoRGB(double dfIre)
{
    DWORD c;

    c = IREtoDW(dfIre) & 0xFF;

    return 0xFF000000 
           | (c << 16)
           | (c << 8)
           | c;
}

DWORD AbsIREtoDW(double dfIre)
{
    if ((D3D__AvInfo & AV_STANDARD_MASK) == AV_STANDARD_NTSC_M)
    {
        return IREtoDW((dfIre - 7.5) / .925);
    }
    else
    {
        return IREtoDW(dfIre);
    }
}


//============================================================================
// This signal is a composite of several useful test signals:
// Top Picture Extents
// 75% Color Bars
// 100% Color Bars
// Picture Center and Horizontal Extents
// Multiburst
// LUMA Step
// Bottom Picture Extents
//============================================================================

void CompositeTest(D3DSurface *pSurface, double d, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwR, dwG, dwB, dwY;
    DWORD           x;
    DWORD           y;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    double MultiburstAdjust;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust  = Rect.Pitch / 4 - Width;

    MultiburstAdjust = (double)Width / 640;

    pData = (LPDWORD)Rect.pBits;

    // Vertical (top) Picture Extents
    for (y = 0; y < Height/12; y++)
    {
        for (x = 0; x < Width; x++)
        {
            if ((x%10 == 0) && (y > Height/12*3/4) || (x%100 == 0) && (y > Height/12*1/2))
            {
                dwR = IREtoDW(0.0);
                dwG = IREtoDW(0.0);
                dwB = IREtoDW(0.0);
            }
            else
            {
                dwR = IREtoDW(100.0);
                dwG = IREtoDW(100.0);
                dwB = IREtoDW(100.0);
            }
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }

        pData += PitchAdjust;
    }

    // 75% Color Bars
    
    for (; y < Height / 4; y++)
    {
        for (x = 0; x < 1 * Width / 8; x++)
        {
            dwR = IREtoDW(75.0);
            dwG = IREtoDW(75.0);
            dwB = IREtoDW(75.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 2 * Width / 8; x++)
        {
            dwR = IREtoDW(75.0);
            dwG = IREtoDW(75.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 3 * Width / 8; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(75.0);
            dwB = IREtoDW(75.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 4 * Width / 8; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(75.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 5 * Width / 8; x++)
        {
            dwR = IREtoDW(75.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(75.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 6* Width / 8; x++)
        {
            dwR = IREtoDW(75.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 7* Width / 8; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(75.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < Width; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }

        pData += PitchAdjust;
    }

    // 100% Color Bars
    
    for (; y < Height / 12 * 5; y++)
    {
        for (x = 0; x < 1 * Width / 8; x++)
        {
            dwR = IREtoDW(100.0);
            dwG = IREtoDW(100.0);
            dwB = IREtoDW(100.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 2 * Width / 8; x++)
        {
            dwR = IREtoDW(100.0);
            dwG = IREtoDW(100.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 3 * Width / 8; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(100.0);
            dwB = IREtoDW(100.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 4 * Width / 8; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(100.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 5 * Width / 8; x++)
        {
            dwR = IREtoDW(100.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(100.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 6* Width / 8; x++)
        {
            dwR = IREtoDW(100.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 7* Width / 8; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(100.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < Width; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }

        pData += PitchAdjust;
    }

    // Picture Center
    
    for (; y < Height / 2; y++)
    {
        for (x = 0; x < 20; x++)
        {
            *pData++ = IREtoRGB(100.0);
        }
        for (; x < Width / 2 - Height / 12; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < Width / 2; x++)
        {
            dwR = IREtoDW(10.0);
            dwG = IREtoDW(10.0);
            dwB = IREtoDW(10.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < Width / 2 + Height / 12; x++)
        {
            dwR = IREtoDW(90.0);
            dwG = IREtoDW(90.0);
            dwB = IREtoDW(90.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < Width - 20; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < Width; x++)
        {
            *pData++ = IREtoRGB(100.0);
        }
        pData += PitchAdjust;
    }

    for (; y < Height / 12 * 7; y++)
    {
        for (x = 0; x < 20; x++)
        {
            *pData++ = IREtoRGB(100.0);
        }
        for (; x < Width / 2 - Height / 12; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < Width / 2; x++)
        {
            dwR = IREtoDW(90.0);
            dwG = IREtoDW(90.0);
            dwB = IREtoDW(90.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < Width / 2 + Height / 12; x++)
        {
            dwR = IREtoDW(10.0);
            dwG = IREtoDW(10.0);
            dwB = IREtoDW(10.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < Width - 20; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < Width; x++)
        {
            *pData++ = IREtoRGB(100.0);
        }
        pData += PitchAdjust;
    }

    // Multiburst

#define MB(x) ((DWORD)((double)x * MultiburstAdjust))

    for (; y < Height / 4 * 3; y++)
    {
       for (x = 0; x < MB(55); x++)
        {
            dwY = AbsIREtoDW(70.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
        for (; x < MB(106); x++)
        {
            dwY = AbsIREtoDW(10.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
        for (; x < MB(123); x++)
        {
            dwY = AbsIREtoDW(40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }

        for (; x < MB(195); x++)
        {           
            dwY = AbsIREtoDW( (30.0 * sin(Kw(500e3) * (double)x)) + 40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
        for (; x < MB(208); x++)
        {
            dwY = AbsIREtoDW(40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }

        for (; x < MB(280); x++)
        {
            dwY = AbsIREtoDW( (30.0 * sin(Kw(1e6) * (double)x)) + 40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
        for (; x < MB(293); x++)
        {
            dwY = AbsIREtoDW(40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }


        for (; x < MB(365); x++)
        {
            dwY = AbsIREtoDW( (30.0 * sin(Kw(2e6) * (double)x)) + 40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
        for (; x < MB(378); x++)
        {
            dwY = AbsIREtoDW(40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }


        for (; x < MB(450); x++)
        {
            dwY = AbsIREtoDW( (30.0 * sin(Kw(3e6) * (double)x)) + 40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
        for (; x < MB(464); x++)
        {
            dwY = AbsIREtoDW(40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }


        for (; x < MB(535); x++)
        {
            dwY = AbsIREtoDW( (30.0 * sin(Kw(3.58e6) * (double)x)) + 40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
        for (; x < MB(549); x++)
        {
            dwY = AbsIREtoDW(40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }


        for (; x < MB(621); x++)
        {
            dwY = AbsIREtoDW( (30.0 * sin(Kw(4.2e6) * (double)x)) + 40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
        for (; x < MB(640); x++)
        {
            dwY = AbsIREtoDW(40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }

        for (; x < Width; x++)
        {
            *pData++ = 0;
        }

        pData += PitchAdjust;
    }

#undef MB

    // 5-Step

    for (; y < Height / 12 * 11; y++)
    {
       for (x = 0; x < 1 * Width / 5; x++)
        {
            dwY = IREtoDW(20.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
       for (; x < 2 * Width / 5; x++)
        {
            dwY = IREtoDW(40.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
       for (; x < 3 * Width / 5; x++)
        {
            dwY = IREtoDW(60.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
       for (; x < 4 * Width / 5; x++)
        {
            dwY = IREtoDW(80.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }
       for (; x < 5 * Width / 5; x++)
        {
            dwY = IREtoDW(100.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }

        pData += PitchAdjust;
    }

    // Bottom Picture Extents
    for (; y < Height; y++)
    {
       for (x = 0; x < Width; x++)
        {
            dwY = IREtoDW(100.0);
            *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
        }

        pData += PitchAdjust;
    }

    D3DSurface_UnlockRect(pSurface); 
}

//============================================================================
// Centering test.
//============================================================================

void Centering(D3DSurface *pSurface, double dfIRE, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwVal;
    DWORD           x;
    DWORD           y;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust = Rect.Pitch / 4;

    pData = (LPDWORD)Rect.pBits;

    for (y = 0; y < Height; y++)
    {
        for (x = 0; x < 32; x++)
        {
            *(pData + x) = IREtoRGB(100.0);
        }

        for (x = Width / 2 - 64; x < Width / 2 - 32; x++)
        {
            *(pData + x) = IREtoRGB(50.0);
        }

        for (x = Width / 2 - 32; x < Width / 2; x++)
        {
            *(pData + x) = IREtoRGB(0.0);
        }

        for (x = Width / 2; x < Width / 2 + 32; x++)
        {
            *(pData + x) = IREtoRGB(100.0);
        }

        for (x = Width / 2 + 32; x < Width / 2 + 64; x++)
        {
            *(pData + x) = IREtoRGB(50.0);
        }

        for (x = Width - 32; x < Width; x++)
        {
            *(pData + x) = IREtoRGB(100.0);
        }

        pData += PitchAdjust;
    }

    D3DSurface_UnlockRect(pSurface); 
}

//============================================================================
// Vertical Centering test.
//============================================================================

void VCentering(D3DSurface *pSurface, double dfIRE, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwVal;
    DWORD           x;
    DWORD           y;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    DWORD Start;
    DWORD Step;
    DWORD Tick;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust = Rect.Pitch / 4;

    Start = Width / 4;
    Step  = Width / 2 / 10;

    pData = (LPDWORD)Rect.pBits;

    for (x = 0; x < Width; x++)
    {
        *pData++ = IREtoRGB(100.0);
    }

    pData = (LPDWORD)Rect.pBits + Rect.Pitch / 4 * Height / 2;

    for (x = 0; x < Width; x++)
    {
        *pData++ = IREtoRGB(100.0);
    }

    pData = (LPDWORD)Rect.pBits + Rect.Pitch / 4 * (Height - 1);

    for (x = 0; x < Width; x++)
    {
        *pData++ = IREtoRGB(100.0);
    }

    pData = (LPDWORD)Rect.pBits;

    for (y = 0; y < Height / 2; y++)
    {
        for (x = 0; x < 10; x++)
        {
            *(pData + Start + Step * x) = IREtoRGB(100.0);
        }

        if (!(y % 5))
        {           
            Tick = (y / 5) % 10 * Step + Start - 12;

            for (x = Tick ; x < Tick + 24; x++)
            {
                *(pData + x) = IREtoRGB(100.0);
            }
        }

        pData += PitchAdjust;
    }

    for (y = Height / 2; y < Height; y++)
    {
        for (x = 0; x < 10; x++)
        {
            *(pData + Start + Step * x) = IREtoRGB(100.0);
        }

        if (!((Height - y) % 5))
        {           
            Tick = ((Height - y ) / 5) % 10 * Step + Start - 12;

            for (x = Tick ; x < Tick + 24; x++)
            {
                *(pData + x) = IREtoRGB(100.0);
            }
        }

        pData += PitchAdjust;
    }

    D3DSurface_UnlockRect(pSurface); 
}



//============================================================================
// Cross hatch test pattern.
//============================================================================

void CrossHatch(D3DSurface *pSurface, double d, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT Rect;

    DWORD Width;
    DWORD Height;
    DWORD Pitch;

    DWORD Space;

    DWORD *pBits;
    DWORD *pCurrent;
    DWORD *pEnd;

    DWORD y;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    Pitch  = Rect.Pitch / 4;

    Space = (DWORD)d    ;

    pBits = (DWORD *)Rect.pBits;

    for (y = 0; y < Height; y++)
    {
        pCurrent = pBits + Pitch * y;
        pEnd = pCurrent + Width;

        for (pCurrent += y % Space; pCurrent < pEnd; pCurrent += Space)
        {
            *pCurrent = IREtoRGB(100.0);
        }

        pCurrent = pBits + Pitch * y;
        pEnd = pCurrent + Width;

        for (pEnd -= y % Space; pEnd >= pCurrent; pEnd -= Space)
        {
            *pEnd = IREtoRGB(100.0);
        }
    }

    D3DSurface_UnlockRect(pSurface);
}

//============================================================================
// Red the whole screen.  We could use Clear for this but why bother?
//============================================================================

void RedField(D3DSurface *pSurface, double dfIRE, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwVal;
    DWORD           x;
    DWORD           y;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust = Rect.Pitch / 4 - Width;

    pData = (LPDWORD)Rect.pBits;

    dwVal = IREtoDW(dfIRE);

    for (y = 0; y < Height; y++)
    {
        for (x = 0; x < Width; x++)
        {
            *pData++ = D3DCOLOR_ARGB(0, dwVal, 0, 0);
        }

        pData += PitchAdjust;
    }

    D3DSurface_UnlockRect(pSurface); 
}

//============================================================================
// Green the whole screen.  We could use Clear for this but why bother?
//============================================================================

void GreenField(D3DSurface *pSurface, double dfIRE, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwVal;
    DWORD           x;
    DWORD           y;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust = Rect.Pitch / 4 - Width;

    pData = (LPDWORD)Rect.pBits;

    dwVal = IREtoDW(dfIRE);

    for (y = 0; y < Height; y++)
    {
        for (x = 0; x < Width; x++)
        {
            *pData++ = D3DCOLOR_ARGB(0, 0, dwVal, 0);
        }

        pData += PitchAdjust;
    }

    D3DSurface_UnlockRect(pSurface); 
}

//============================================================================
// Blue the whole screen.  We could use Clear for this but why bother?
//============================================================================

void BlueField(D3DSurface *pSurface, double dfIRE, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwVal;
    DWORD           x;
    DWORD           y;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust = Rect.Pitch / 4 - Width;

    pData = (LPDWORD)Rect.pBits;

    dwVal = IREtoDW(dfIRE);

    for (y = 0; y < Height; y++)
    {
        for (x = 0; x < Width; x++)
        {
            *pData++ = D3DCOLOR_ARGB(0, 0, 0, dwVal);
        }

        pData += PitchAdjust;
    }

    D3DSurface_UnlockRect(pSurface); 
}

//============================================================================
// Grey the whole screen.  We could use Clear for this but why bother?
//============================================================================

void GreyField(D3DSurface *pSurface, double dfIRE, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwVal;
    DWORD           x;
    DWORD           y;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust = Rect.Pitch / 4 - Width;

    pData = (LPDWORD)Rect.pBits;

    dwVal = IREtoDW(dfIRE);

    // A non-zero d2 indicates that we need to adjust the IRE to be
    // actually value...ignoring any pedistal.
    //
    if (d2 != 0.0 && (D3D__AvInfo & AV_STANDARD_MASK) == AV_STANDARD_NTSC_M)
    {
        dwVal = IREtoDW((dfIRE - 7.5) / .925);
    }

    for (y = 0; y < Height; y++)
    {
        for (x = 0; x < Width; x++)
        {
            *pData++ = D3DCOLOR_ARGB(0, dwVal, dwVal, dwVal);
        }

        pData += PitchAdjust;
    }

    D3DSurface_UnlockRect(pSurface); 
}

//============================================================================
// Line interlace test.
//============================================================================

void LineInterlace(D3DSurface *pSurface, double d, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT Rect;

    DWORD Width;
    DWORD Height;
    DWORD Pitch;

    DWORD *pBits;
    DWORD *pCurrent;
    DWORD *pEnd;

    DWORD x, y, i;
    DWORD Pulse, Rest;

    DWORD Color;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    Pitch  = Rect.Pitch / 4;

    Rest = Width / 34;
    Pulse = Rest * 2;

    pBits = (DWORD *)Rect.pBits;

    for (y = 0; y < Height; y++)
    {
        pCurrent = pBits + Pitch * y;

        for (i = 0; i < 11; i++)
        {
            for (x = 0; x < Rest; x++)
            {
                *(pCurrent++) = IREtoRGB(50.0);
            }

            Color = (y & (0x400 >> i)) ? IREtoRGB(100.0) : IREtoRGB(0.0);

            for (x = 0; x < Pulse; x++)
            {
                *(pCurrent++) = Color;
            }
        }

        for (x = 0; x < Rest; x++)
        {
            *(pCurrent++) = IREtoRGB(50.0);
        }
    }

    D3DSurface_UnlockRect(pSurface);
}

//============================================================================
// Interlaced bluefield test.
//============================================================================

void InterlaceField(D3DSurface *pSurface, double dfIRE, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT Rect;

    DWORD Width;
    DWORD Height;
    DWORD Pitch;

    DWORD Space;

    DWORD *pBits;
    DWORD *pCurrent;
    DWORD *pEnd;

    DWORD y;

    BOOL EvenField = (DWORD)dfIRE;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height * 2;
    Pitch  = Rect.Pitch / 4;

    Space = (DWORD)50;

    pBits = (DWORD *)Rect.pBits;

#if 1

    for (y = 0; y < Height; y ++)
    {
        pCurrent = pBits + Pitch * y;
        pEnd = pCurrent + Width;

        for (; pCurrent < pEnd; pCurrent++)
        {
            if (EvenField)
            {
                *pCurrent = IREtoRGB(50.0);
            }
            else
            {
                *pCurrent = IREtoRGB(100.0);
            }
        }
    }

#else

    if (EvenField)
    {
        y = 1;
    }
    else
    {
        y = 0;
    }

    for (; y < Height; y += 2)
    {
        pCurrent = pBits + Pitch * (y / 2);
        pEnd = pCurrent + Width;

        for (pCurrent += y % Space; pCurrent < pEnd; pCurrent += Space)
        {
            *pCurrent = IREtoRGB(100.0);
        }

        pCurrent = pBits + Pitch * (y / 2);
        pEnd = pCurrent + Width;

        for (pEnd -= y % Space; pEnd >= pCurrent; pEnd -= Space)
        {
            *pEnd = IREtoRGB(100.0);
        }
    }

#endif

    D3DSurface_UnlockRect(pSurface);
}

//============================================================================
// Luma ramp.
//============================================================================

void LumaRamp(D3DSurface *pSurface, double d, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwVal;
    DWORD           x;
    DWORD           y;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    DWORD ColorBase;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust  = Rect.Pitch / 4 - Width;

    if (d == 1.0)
    {
        ColorBase = 0x00010000;
    }
    else if (d == 2.0)
    {
        ColorBase = 0x00000100;
    }
    else if (d == 3.0)
    {
        ColorBase = 0x00000001;
    }
    else
    {
        ColorBase = 0x00010101;
    }

    pData = (LPDWORD)Rect.pBits;

    for (y = 0; y < Height; y++)
    {
        for (x = 0; x < Width; x++)
        {
            if (y < Height / 2)
            {
                dwVal = IREtoDW(100.0 * x / (Width - 1));
            }
            else
            {
                dwVal = IREtoDW(100.0 * (Width - 1 - x) / (Width - 1));
            }

            *pData++ = ColorBase * dwVal;
        }

        pData += PitchAdjust;
    }
    
    D3DSurface_UnlockRect(pSurface); 
}

//============================================================================
// Smpte bars
//============================================================================

void SmpteBars(D3DSurface *pSurface, double WhiteIntensity, double Intensity, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwR, dwG, dwB;
    DWORD           x;
    DWORD           y;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust  = Rect.Pitch / 4 - Width;

    pData = (LPDWORD)Rect.pBits;

    for (y = 0; y < Height * 3 / 4; y++)
    {
        for (x = 0; x < 1 * Width / 8; x++)
        {
            dwR = IREtoDW(WhiteIntensity);
            dwG = IREtoDW(WhiteIntensity);
            dwB = IREtoDW(WhiteIntensity);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 2 * Width / 8; x++)
        {
            dwR = IREtoDW(Intensity);
            dwG = IREtoDW(Intensity);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 3 * Width / 8; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(Intensity);
            dwB = IREtoDW(Intensity);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 4 * Width / 8; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(Intensity);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 5 * Width / 8; x++)
        {
            dwR = IREtoDW(Intensity);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(Intensity);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 6* Width / 8; x++)
        {
            dwR = IREtoDW(Intensity);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 7* Width / 8; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(Intensity);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 8 * Width / 8; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }

        pData += PitchAdjust;
    }

    for (; y < Height; y++)
    {
        for (x = 0; x < 1 * Width / 6; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(10.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 2 * Width / 6; x++)
        {
            dwR = IREtoDW(100.0);
            dwG = IREtoDW(100.0);
            dwB = IREtoDW(100.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 3 * Width / 6; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(10.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 4 * Width / 6; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 5 * Width / 6; x++)
        {
            dwR = IREtoDW(4.2);
            dwG = IREtoDW(4.2);
            dwB = IREtoDW(4.2);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }
        for (; x < 6 * Width / 6; x++)
        {
            dwR = IREtoDW(0.0);
            dwG = IREtoDW(0.0);
            dwB = IREtoDW(0.0);
            *pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
        }

        pData += PitchAdjust;
    }   

    D3DSurface_UnlockRect(pSurface); 
}

//============================================================================
// Multiburst test pattern.
//============================================================================

void MultiBurst(D3DSurface *pSurface, double d, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwVal, dwY;
    double          x;
    DWORD           y;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    double MultiburstAdjust;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust  = Rect.Pitch / 4 - Width;

    MultiburstAdjust = (double)Width / 640;

    pData = (LPDWORD)Rect.pBits;

#define MB(x) ((DWORD)((double)x * MultiburstAdjust))

    for (y = 0; y < 480; y++)
    {
        if (y<20)
        {
            for (x = 0; x < MB(640); x= x+1.0)
            {
                dwVal = AbsIREtoDW(70.0);
                *pData++ = D3DCOLOR_ARGB(0, dwVal, dwVal, dwVal);
            }
        }
        else if (y<460)
        {
            // Square
            for (x = 0; x < MB(22); x++)
            {
                dwY = AbsIREtoDW(70.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (; x < MB(55); x++)
            {
                dwY = AbsIREtoDW(10.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (; x < MB(65); x++)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // .5MHz
            for (; x < MB(120); x++)
            {           
                dwY = AbsIREtoDW( (30.0 * sin(Kw(500e3) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (; x < MB(130); x++)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 1MHz
            for (; x < MB(185); x++)
            {
                dwY = AbsIREtoDW( (30.0 * sin(Kw(1e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (; x < MB(195); x++)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 2MHz
            for (; x < MB(250); x++)
            {
                dwY = AbsIREtoDW( (30.0 * sin(Kw(2e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (; x < MB(260); x++)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 3MHz
            for (; x < MB(315); x++)
            {
                dwY = AbsIREtoDW( (30.0 * sin(Kw(3e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (; x < MB(325); x++)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 3.586MHz
            for (; x < MB(380); x++)
            {
                dwY = AbsIREtoDW( (30.0 * sin(Kw(3.58e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (; x < MB(390); x++)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 4.2MHz
            for (; x < MB(445); x++)
            {
                dwY = AbsIREtoDW( (30.0 * sin(Kw(4.2e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (; x < MB(455); x++)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 5MHz
            for (; x < MB(510); x++)
            {
                dwY = AbsIREtoDW( (30.0 * sin(Kw(5.0e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (; x < MB(520); x++)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 5.5MHz
            for (; x < MB(575); x++)
            {
                dwY = AbsIREtoDW( (30.0 * sin(Kw(5.5e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (; x < MB(585); x++)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            
            // 6.75MHz
            for (; x < MB(640); x++)
            {
                dwY = AbsIREtoDW( (30.0 * sin(Kw(6.75e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }            
            
            for (; x < Width; x++)
            {
                *pData++ = 0;
            }
        }
        else
        {
            for (x=0; x < MB(640); x= x+1.0)
            {
                dwVal = AbsIREtoDW(10.0);
                *pData++ = D3DCOLOR_ARGB(0, dwVal, dwVal, dwVal);
            }
        }

        pData += PitchAdjust;
    }   

#undef MB

    D3DSurface_UnlockRect(pSurface); 
}

//============================================================================
// Multipulse test pattern.
//
// The MultiPulse test signal consists of a series of short modulated pulses 
// of varying duration, subcarrier frequency, and amplitude, specifically: 
//
// 1. A white flag bar (100 IRE bar signal of duration 5.9 microseconds) 
// 2. The 2T pulse (2T luminance-only pulse of amplitude 100 IRE , 
//    where T =125 nanoseconds), and 
// 3. A 25T modulated pulse (25T sine-squared pulse of amplitude 100 IRE, 
//    with 1.0MHz modulation, where T = 125 nanoseconds), and 
// 4. Four 12.5T modulated pulses (12.5T sine-squared pulses of amplitude 
//    100 IRE with 2.0, 3.0, 3.58, and 4.2 MHz modulation, respectively, 
//    where T = 125 nanoseconds). 
//
// These signal components are supported with standard synchronizing and 
// blanking signals. The signal is specified in the ANSI T1.502-1988 Standard
// for Telecommunications. 
//
// The IRE levels for the pulses is set to the range [-3, 70]
//============================================================================
void MultiPulse(D3DSurface *pSurface, double d, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwVal, dwY;
    double          x;
    DWORD           y;
    double          xx;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    double MultiburstAdjust;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust  = Rect.Pitch / 4 - Width;

    MultiburstAdjust = (double)Width / 640;

    pData = (LPDWORD)Rect.pBits;

#define MB(x) ((DWORD)((double)(x) * MultiburstAdjust))

    // The multipulse is similar to the multiburst except for the frequencies
    // used and the following:
    // In the multipulse signal, the sinusoidal signales are modulated by a
    // sin^2 pulse and then added to the same sin^2 pulse.
#define SINSQ(x) (0.5 * (1.0 - cos(2.0 * (x))))
#define MP(x) (sin(x) * SINSQ(x) + SINSQ(x))

    for (y = 0; y < 480; y++)
    {
        if (y<20)
        {
            for (x = 0; x < MB(640); x= x+1.0)
            {
                dwVal = AbsIREtoDW(70.0);
                *pData++ = D3DCOLOR_ARGB(0, dwVal, dwVal, dwVal);
            }
        }
        else if (y<460)
        {
            // White Flag Bar 100 IRE for 5.9e-6 seconds
            for(x=0, xx=0; xx < MB(5.9e-6 / SecondsPerPixel()); ++xx, ++x)
            {           
                dwY = AbsIREtoDW(100.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (xx=0; xx < 10; ++xx, ++x)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            
            // Luminance only pulse ? at 100 IRE for 250e-9 seconds
            for(xx=0.0; xx < MB(250.0e-9 / SecondsPerPixel()); ++xx, ++x)
            {           
                dwY = AbsIREtoDW(70.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (xx=0; xx < 10; ++xx, ++x)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 1MHz pulse at 100 IRE for 3125e-9 seconds
            for (xx=0.0; xx < MB(3125.0e-9 / SecondsPerPixel()); ++xx, ++x)
            {           
                dwY = AbsIREtoDW((15.0 * MP(Kw(1.0e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (xx=0; xx < 10; ++xx, ++x)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 2MHz pulse at 100 IRE for 1562.5e-9 seconds
            for (xx=0.0; xx < MB(1562.5e-9 / SecondsPerPixel()); ++xx, ++x)
            {
                dwY = AbsIREtoDW((15.0 * MP(Kw(2.0e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (xx=0; xx < 10; ++xx, ++x)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 3MHz pulse at 100 IRE for 1562.5e-9 seconds
            for (xx=0.0; xx < MB(1562.5e-9 / SecondsPerPixel()); ++xx, ++x)
            {
                dwY = AbsIREtoDW((15.0 * MP(Kw(3.0e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (xx=0; xx < 10; ++xx, ++x)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 3.58MHz pulse at 100 IRE for 1562.5e-9 seconds
            for (xx=0.0; xx < MB(1562.5e-9 / SecondsPerPixel()); ++xx, ++x)
            {
                dwY = AbsIREtoDW((15.0 * MP(Kw(3.58e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }
            for (xx=0; xx < 10; ++xx, ++x)
            {
                dwY = AbsIREtoDW(40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }

            // 4.1MHz pulse at 100 IRE for 1562.5e-9 seconds
            for (xx=0.0; xx < MB(1562.5e-9 / SecondsPerPixel()); ++xx, ++x)
            {
                dwY = AbsIREtoDW((15.0 * MP(Kw(4.1e6) * (double)x)) + 40.0);
                *pData++ = D3DCOLOR_ARGB(0, dwY, dwY, dwY);
            }            
            
            for (; x < Width; ++x)
            {
                *pData++ = 0;
            }
        }
        else
        {
            for (x=0; x < MB(640); x= x+1.0)
            {
                dwVal = AbsIREtoDW(10.0);
                *pData++ = D3DCOLOR_ARGB(0, dwVal, dwVal, dwVal);
            }
        }

        pData += PitchAdjust;
    }   

#undef MB
#undef MP
#undef SINSQ

    D3DSurface_UnlockRect(pSurface); 
}


//============================================================================
// (sin x)/x test pattern.
//
// Because using Kw(x) in this method only produces a small fraction
// of the signal, I chose to use 6 cycles (just an arbitrary number).
//============================================================================

void SinXOverX(D3DSurface *pSurface, double d, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           dwVal;
    double          x;
    DWORD           y;
    double          fx;
    DWORD           PitchAdjust;
    double          MultiburstAdjust;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    PitchAdjust = Rect.Pitch / 4 - Desc.Width;
    pData       = (LPDWORD)Rect.pBits;
    
    MultiburstAdjust = (double)Desc.Width / 640;

    pData = (LPDWORD)Rect.pBits;

#define MB(x) ((DWORD)((double)(x) * MultiburstAdjust))

    for(y = 0; y < 480; ++y)
    {
        if(y<20)
        {
            for (x=0.0; x < MB(640); x+=1.0)
            {
                dwVal       = AbsIREtoDW(70.0);
                *pData++    = D3DCOLOR_ARGB(0, dwVal, dwVal, dwVal);
            }
        }
        else if(y<460)
        {
            for (x=0.0; x < MB(640); x+=1.0)
            {
                fx = x / (double)MB(640) * 3.14159265359 * 2.0 * 6.0;
                if(fx != 0.0)
                    dwVal   = AbsIREtoDW((30.0 * (sin(fx) / fx)) + 40.0);
                else
                    dwVal   = AbsIREtoDW(100.0);
                *pData++    = D3DCOLOR_ARGB(0, dwVal, dwVal, dwVal);
            }
        }
        else
        {
            for (x=0.0; x < MB(640); x+=1.0)
            {
                dwVal       = AbsIREtoDW(10.0);
                *pData++    = D3DCOLOR_ARGB(0, dwVal, dwVal, dwVal);
            }
        }
        pData += PitchAdjust;
    }   

    D3DSurface_UnlockRect(pSurface); 
}

//============================================================================
// Blue the whole screen.  We could use Clear for this but why bother?
//============================================================================

void DrawCircle(DWORD *pStart, DWORD Pitch, DWORD Color, DWORD xCenter, DWORD yCenter, double dRadius)
{
    long xOffset;
    long yOffset;

    long x, y;

    yOffset = (long)dRadius;

    for (y = -yOffset; y <= yOffset; y++)
    {
        xOffset = (long)sqrt(dRadius * dRadius - (double)(y * y));

        for (x = -xOffset; x <= xOffset; x++)
        {
            *(pStart + (yCenter + y) * Pitch / 4 + (xCenter + x)) = Color;
        }
    }   
}

void Circles(D3DSurface *pSurface, double d1, double d2, DWORD Time)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    DWORD           x;
    DWORD           y;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust = Rect.Pitch / 4 - Width;

    pData = (LPDWORD)Rect.pBits;

    for (y = 0; y < Height; y++)
    {
        for (x = 0; x < Width; x++)
        {
            *pData++ = D3DCOLOR_ARGB(0, 0, 0, 0);
        }

        pData += PitchAdjust;
    }

    // Draw red circles
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00FF0000,  70,  40, 32.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00FF0000, 170,  40, 16.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00FF0000, 270,  40,  8.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00FF0000, 370,  40,  4.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00FF0000, 470,  40,  2.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00FF0000, 570,  40,  1.0);

    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00BF0000,  70, 112, 32.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00BF0000, 170, 112, 16.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00BF0000, 270, 112,  8.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00BF0000, 370, 112,  4.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00BF0000, 470, 112,  2.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x00BF0000, 570, 112,  1.0);

    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000FF00,  70, 184, 32.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000FF00, 170, 184, 16.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000FF00, 270, 184,  8.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000FF00, 370, 184,  4.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000FF00, 470, 184,  2.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000FF00, 570, 184,  1.0);

    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000BF00,  70, 256, 32.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000BF00, 170, 256, 16.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000BF00, 270, 256,  8.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000BF00, 370, 256,  4.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000BF00, 470, 256,  2.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x0000BF00, 570, 256,  1.0);

    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000FF,  70, 328, 32.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000FF, 170, 328, 16.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000FF, 270, 328,  8.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000FF, 370, 328,  4.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000FF, 470, 328,  2.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000FF, 570, 328,  1.0);

    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000BF,  70, 400, 32.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000BF, 170, 400, 16.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000BF, 270, 400,  8.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000BF, 370, 400,  4.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000BF, 470, 400,  2.0);
    DrawCircle((LPDWORD)Rect.pBits, Rect.Pitch, 0x000000BF, 570, 400,  1.0);

    D3DSurface_UnlockRect(pSurface); 
}

//==============================================================================
// Helpers to read/write the CRTC and TV encoder registers.
//==============================================================================

/*
 * The list of interesting registers.
 */

const UCHAR g_CRTCMap[] = 
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
    0x18, 0x19, 0x1A, 0x1B, 0x20, 0x25, 0x2D, 0x33, 
    0x39, 0x41
};

const UCHAR g_TVMap[] =
{
    0x00, 0x02, 0x04, 0x06, 0x2E, 
    0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E,
    0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E,
    0x50, 0x52, 0x54, 0x56, 0x58, 0x5A, 0x5C, 0x5E,
    0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C, 0x6E,
    0x70, 0x72, 0x74, 0x76, 0x78, 0x7A, 0x7C, 0x7E,
    0x80, 0x82, 0x84, 0x86, 0x88, 0x8A, 0x8C, 0x8E,
    0x90, 0x92, 0x94, 0x96, 0x98, 0x9A, 0x9C, 0x9E,
    0xA0, 0xA2, 0xA4, 0xA6, 0xA8, 0xAA, 0xAC, 0xAE,
    0xB0, 0xB2, 0xB4, 0xB6, 0xB8, 0xBA, 0xBC, 0xBE,
    0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
    0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE,
    0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE,
    0xF0, 0xF2, 0xF4, 0xF6, 0xF8, 0xFA, 0xFC, 0xFF
};

/*
 * Copies of the current register state and pending changes.
 */

//----------------------------------------------------------------------------
// Helpers to read/write CRTC and TV encoder registers.
//


// Register base is always at one spot.
#define REGISTER_BASE                ((void *)0xFD000000)

//
// Functions to access hardware registers
//

/* 
 * From smb.c
 */

#define MCPIOBASE   0xC000

extern __inline UCHAR IO_RD08(USHORT address)
{
  UCHAR result;

  __asm {
      mov dx, address;
      in al, dx;
      mov result, al;
  }

  return(result);
}

extern __inline USHORT IO_RD16(USHORT address)
{
  USHORT result;

  __asm {
      mov dx, address;
      in ax, dx;
      mov result, ax;
  }

  return(result);
}

extern __inline void IO_WR08(USHORT address, UCHAR data)
{
  __asm {
      mov dx, address;
      mov al, data;
      out dx, al;
  }
}

extern __inline void IO_WR16(USHORT address, USHORT data)
{
  __asm {
      mov dx, address;
      mov ax, data;
      out dx, ax;
  }
}

#if 1

UCHAR
SMB_RD(VOID * Ptr, UCHAR Reg)
{
    ULONG Value;

    HalReadSMBusByte(TV_ENCODER_ID, Reg, &Value);

    return (UCHAR)Value;
}

VOID
SMB_WR(VOID *Ptr, UCHAR Reg, UCHAR Val)
{
    HalWriteSMBusByte(TV_ENCODER_ID, Reg, Val);
}

USHORT
SMB_RDW(VOID * Ptr, UCHAR Reg)
{
    ULONG Value;

    HalReadSMBusWord(TV_ENCODER_ID, Reg, &Value);

    return (USHORT)Value;
}

VOID
SMB_WRW(VOID *Ptr, UCHAR Reg, USHORT Val)
{
    HalWriteSMBusWord(TV_ENCODER_ID, Reg, Val);
}

#else

#define SmbusReadPortUCHAR(port)           (IO_RD08((USHORT)(MCPIOBASE + port)))
#define SmbusReadPortUSHORT(port)          (IO_RD16((USHORT)(MCPIOBASE + port)))
#define SmbusWritePortUCHAR(port, data)    (IO_WR08((USHORT)(MCPIOBASE + port),(UCHAR)(data)))
#define SmbusWritePortUSHORT(port, data)   (IO_WR16((USHORT)(MCPIOBASE + port),(USHORT)(data)))

// MCP SMB defines
#define MCP_SMB_STATUS                                   0x00000000
#define MCP_SMB_STATUS_HST_STS_BUSY                          (1<<3)
#define MCP_SMB_STATUS_HCYC_STS_ACTIVE                       (1<<4)
#define MCP_SMB_STATUS_HCYC_STS_SUCCESS                      (1<<4)
#define MCP_SMB_STATUS_SM_BSY_BUSY                          (1<<11)

#define MCP_SMB_CTRL                                     0x00000002
#define MCP_SMB_CTRL_CYC_TYPE_RD_WR_BYTE                     (2<<0)
#define MCP_SMB_CTRL_HOST_STC_START                          (1<<3)

#define MCP_SMB_HSTA                                     0x00000004
#define MCP_SMB_HDATA                                    0x00000006
#define MCP_SMB_HCMD                                     0x00000008


// should we add a timeout to the while loop?
void
AvpWaitForSMBusIdle()
{
    volatile USHORT status;

    do {
        status = SmbusReadPortUSHORT(MCP_SMB_STATUS);
    } while (status & (MCP_SMB_STATUS_HST_STS_BUSY |
                       MCP_SMB_STATUS_SM_BSY_BUSY  ));
}



void
AvpReadDeviceReg(
    PVOID RegisterBase,
    UCHAR  SMBaddr,
    UCHAR  Reg,
    UCHAR* ReadData
    )
{
    USHORT status;

    // reset status bits
    status = SmbusReadPortUSHORT(MCP_SMB_STATUS);
    SmbusWritePortUSHORT(MCP_SMB_STATUS, status);    // write 1's to clear

    // load the slave addr, subaddress
    SmbusWritePortUSHORT(MCP_SMB_HSTA,  SMBaddr + 1);
    SmbusWritePortUCHAR(MCP_SMB_HCMD,   Reg);

    // initiate the transfer
    SmbusWritePortUSHORT(MCP_SMB_CTRL, (MCP_SMB_CTRL_CYC_TYPE_RD_WR_BYTE |
                                        MCP_SMB_CTRL_HOST_STC_START));
    AvpWaitForSMBusIdle();

    // return completion status
    status = SmbusReadPortUSHORT(MCP_SMB_STATUS);

    if (status & MCP_SMB_STATUS_HCYC_STS_SUCCESS)
    {
        *ReadData = SmbusReadPortUCHAR(MCP_SMB_HDATA);
    }
}



UCHAR
AvpWriteDeviceReg(
    PVOID  RegisterBase,
    UCHAR SMBaddr,
    UCHAR Reg,
    UCHAR WriteValue
    )
{
    USHORT status;

    // reset status bits
    status = SmbusReadPortUSHORT(MCP_SMB_STATUS);
    SmbusWritePortUSHORT(MCP_SMB_STATUS, status);    // write 1's to clear

    // load the slave addr, subaddress and data
    SmbusWritePortUSHORT(MCP_SMB_HSTA,  SMBaddr);
    SmbusWritePortUCHAR(MCP_SMB_HCMD,   Reg);
    SmbusWritePortUSHORT(MCP_SMB_HDATA, WriteValue);

    // initiate the transfer
    SmbusWritePortUSHORT(MCP_SMB_CTRL, (MCP_SMB_CTRL_CYC_TYPE_RD_WR_BYTE |
                                        MCP_SMB_CTRL_HOST_STC_START));
    AvpWaitForSMBusIdle();

    // return completion status
    status = SmbusReadPortUSHORT(MCP_SMB_STATUS);
    return !(status & MCP_SMB_STATUS_HCYC_STS_SUCCESS);
}

UCHAR
SMB_RD(VOID * Ptr, UCHAR Reg)
{
    UCHAR Value;

    AvpReadDeviceReg(Ptr, TV_ENCODER_ID, Reg, &Value);

    return Value;
}

VOID
SMB_WR(VOID *Ptr, UCHAR Reg, UCHAR Val)
{
    AvpWriteDeviceReg(Ptr, TV_ENCODER_ID, Reg, Val);
}

#endif


VOID
AvpDelay(
    PVOID RegisterBase,
    ULONG nanosec
    )
{
   ULONG start;
   ULONG elapsed;

   volatile ULONG *Timer = (ULONG *)((UCHAR *)RegisterBase + 0x00009400);

   start = *Timer;

   do {
       elapsed = *Timer - start;
   } while (elapsed < nanosec);
}

/*
 * From avp.h.
 */

ULONG
REG_RD32(VOID* Ptr, ULONG Addr)
{
    return *((volatile ULONG*)((UCHAR*)(Ptr) + (Addr)));
}

VOID
REG_WR32(VOID* Ptr, ULONG Addr, ULONG Val)
{
    *((volatile ULONG*)((UCHAR*)(Ptr) + (Addr))) = (ULONG)(Val);
}

VOID
REG_OR32(VOID* Ptr, ULONG Addr, ULONG Val)
{
    ULONG Value;
    
    Value = REG_RD32(Ptr, Addr);
    REG_WR32(Ptr, Addr, Value | Val);
}

UCHAR
REG_RD08(VOID* Ptr, ULONG Addr)
{
    return *((volatile UCHAR*)((UCHAR*)(Ptr) + (Addr)));
}

VOID
REG_WR08(VOID* Ptr, ULONG Addr, UCHAR Val)
{
    *((volatile UCHAR*)((UCHAR*)(Ptr) + (Addr))) = (UCHAR)(Val);
}

VOID
CRTC_WR(VOID* Ptr, UCHAR i, UCHAR d)            
{
    REG_WR08(Ptr, 0x006013d4, (i));
    REG_WR08(Ptr, 0x006013d5, (d));
}

UCHAR
CRTC_RD(VOID* Ptr, UCHAR i)            
{
    UCHAR Value;

    REG_WR08(Ptr, 0x006013d4, (i));
    Value = REG_RD08(Ptr, 0x006013d5);

    return Value;
}

//==============================================================================
// Test table, contains all of the above tests.
//==============================================================================

/*
 * Parsed commands.
 */

ULONG g_Argc;
char *g_Argv[1024];

/*
 * Response.
 */

ULONG g_cchBuffer;
char  g_szBuffer[4096];

/*
 * Test patterns.
 */

typedef void (*COMMAND)(D3DSurface *pSurface, double d, double d2, DWORD Time);

typedef struct _TESTS
{
    COMMAND m_Command;
    char *m_szName;
    char *m_szDescription;
    double m_DefaultValue1;
    double m_DefaultValue2;
    BOOL m_TwoField;
} TESTS;

TESTS g_Tests[] =
{
    { CompositeTest, "co", "composite",                      0.0,   0.0, FALSE },
    { SmpteBars,     "s17","smptebars 100.75",             100.0,  75.0, FALSE },
    { SmpteBars,     "s77","smptebars 75.75",               75.0,  75.0, FALSE },
    { SmpteBars,     "s11","smptebars 100.100",            100.0, 100.0, FALSE },
    { Centering,     "ce", "centering",                      0.0,   0.0, FALSE },
    { VCentering,    "vc", "vertical centering",             0.0,   0.0, FALSE },
    { GreyField,     "g0", "greyfield 0",                    0.0,   0.0, FALSE },
    { GreyField,     "g5", "greyfield 50",                  50.0,   1.0, FALSE },
    { GreyField,     "g1", "greyfield 100",                100.0,   0.0, FALSE },
    { MultiBurst,    "mb",  "multiburst",                    0.0,   0.0, FALSE },
    { MultiPulse,    "mp",  "multipulse",                    0.0,   0.0, FALSE },
    { SinXOverX,     "sx", "(sin x)/x",                      0.0,   0.0, FALSE },
    { CrossHatch,    "cr", "cross hatch <distance = 50>",   50.0,   0.0, FALSE },
    { RedField,      "rf", "redfield <intensity = 100%>",  100.0,   0.0, FALSE },
    { GreenField,    "gf", "greenfield <intensity = 100%>",100.0,   0.0, FALSE },
    { BlueField,     "bf", "bluefield <intensity = 100%>", 100.0,   0.0, FALSE },
    { InterlaceField,"if", "interlace field",                0.0,   0.0, TRUE  },
    { LineInterlace, "li", "line interlace",                 0.0,   0.0, FALSE },
    { LumaRamp,      "lr", "lumaramp",                       0.0,   0.0, FALSE },
    { LumaRamp,      "rr", "red ramp",                       1.0,   0.0, FALSE },
    { LumaRamp,      "gr", "green ramp",                     2.0,   0.0, FALSE },
    { LumaRamp,      "br", "blue ramp",                      3.0,   0.0, FALSE },
    { Circles,       "ci", "circles",                        0.0,   0.0, FALSE },
};

ULONG g_TestPattern;

//----------------------------------------------------------------------------
// Initializes a screen.
//
void
CreateScreen(
    ULONG Screen,
    double Value,
    double Value2
    )
{
    if (g_ScreenOdd)
    {
        D3DSurface_Release(g_ScreenOdd);
        g_ScreenOdd = NULL;
    }

    if (g_ScreenEven)
    {
        D3DSurface_Release(g_ScreenEven);
        g_ScreenEven = NULL;
    }

    D3DDevice_CreateImageSurface(g_Mode.Width, 
                                 g_Mode.Height, 
                                 D3DFMT_LIN_X8R8G8B8, 
                                 &g_ScreenOdd);

    D3DDevice_SetRenderTarget(g_ScreenOdd, NULL);
    D3DDevice_Clear(0, NULL, D3DCLEAR_TARGET, 0xFF000000, 1.0, 0);

    g_Tests[Screen].m_Command(g_ScreenOdd, Value, Value2, 0);

    if (g_Tests[Screen].m_TwoField)
    {
        D3DDevice_CreateImageSurface(g_Mode.Width, 
                                     g_Mode.Height, 
                                     D3DFMT_LIN_X8R8G8B8, 
                                     &g_ScreenEven);

        D3DDevice_SetRenderTarget(g_ScreenEven, NULL);
        D3DDevice_Clear(0, NULL, D3DCLEAR_TARGET, 0xFF000000, 1.0, 0);

        g_Tests[Screen].m_Command(g_ScreenEven, (double)(!(DWORD)Value), 0.0, 0);
    }
    else
    {
        g_ScreenEven = g_ScreenOdd;

        D3DSurface_AddRef(g_ScreenEven);
    }

    memset(g_LineNumber, 0, sizeof(g_LineNumber));
}

//----------------------------------------------------------------------------
// Print helper.
//
void
ResPrintf(
    char* szFormat,
    ...
    )
{
    ULONG cch;
    va_list va;

    va_start(va, szFormat);

    cch = vsprintf(g_szBuffer + g_cchBuffer, szFormat, va);

    g_cchBuffer += cch;

    if (g_cchBuffer >= sizeof(g_szBuffer))
    {
        _asm int 3;
    }

    va_end(va);
}

//----------------------------------------------------------------------------
// Print helper.
//
void 
ResPrintMode(
    ULONG iMode
    )
{
    D3DDISPLAYMODE Mode;
    char *szFormat;

    Direct3D_EnumAdapterModes(0, iMode * 4, &Mode);

    ResPrintf("%d: %dx%d %sat %d Hz (%s)", 
              iMode, 
              Mode.Width, 
              Mode.Height, 
              (Mode.Flags & D3DPRESENTFLAG_10X11PIXELASPECTRATIO) ? "1.1:1 " : "",
              Mode.RefreshRate, 
              (Mode.Flags & D3DPRESENTFLAG_INTERLACED) ? (Mode.Flags & D3DPRESENTFLAG_FIELD) ? "field" : "interlaced" : "progressive");

    if (Mode.Flags & D3DPRESENTFLAG_WIDESCREEN)
    {
        ResPrintf(", 16:9");
    }

    ResPrintf("\r\n");
}

//----------------------------------------------------------------------------
// Change which test is being displayed.
//
void Show()
{
    ULONG i;
    double Value1, Value2;

    i = sizeof(g_Tests) / sizeof(TESTS);

    if (g_Argc == 2 || g_Argc == 3)
    {
        for (i = 0; i < sizeof(g_Tests) / sizeof(TESTS); i++)
        {
            if (!strncmp(g_Tests[i].m_szName, g_Argv[1], strlen(g_Tests[i].m_szName)))
            {
                break;
            }
        }
    }

    if (i == sizeof(g_Tests) / sizeof(TESTS))
    {
        ResPrintf("\r\n!show <test> [<value>]\r\n\r\n");

        for (i = 0; i < sizeof(g_Tests) / sizeof(TESTS); i++)
        {
            ResPrintf("    %3s - %s\r\n", g_Tests[i].m_szName, g_Tests[i].m_szDescription);
        }
    }
    else
    {
        g_TestPattern = i;

        Value1 = g_Tests[i].m_DefaultValue1;
        Value2 = g_Tests[i].m_DefaultValue2;
       
        CreateScreen(i, Value1, Value2);
    }
}

// Helper
char *Enabled(BOOL Value)
{
    if (Value)
    {
        return "enabled";
    }
    else
    {
        return "disabled";
    }
}

//----------------------------------------------------------------------------
// Dump display setting information
//
void Info()
{

#if 1

    ULONG i;

    ULONG htotal, hdisplayend, hblankstart, hblankend, hretracestart, hretraceend;
    ULONG vtotal, vdisplayend, vblankstart, vblankend, vretracestart, vretraceend;

    double pclock, fract;

    ULONG Value;

    /*
     * Main clock
     */

#ifndef FOCUS

    i = SMB_RD(REGISTER_BASE, 0xA0) & 0x1F;
    fract = (double)(((ULONG)SMB_RD(REGISTER_BASE, 0x9E) << 8) | (ULONG)SMB_RD(REGISTER_BASE, 0x9C)) / 65536.0;

    pclock = 13.500 * ((double)i + fract);

    if (SMB_RD(REGISTER_BASE, 0x2A) & 0x02)
    {
        pclock /= 10.0;
    }
    else
    {
        pclock /= 6.0;
    }

    // How precise is the clock?
    pclock = floor(pclock * 1000000.0) / 1000000.0;

    ResPrintf("Clock           - %.6f\r\n\r\n", pclock);

#endif

    /*
     * CRTC 
     */

    // htotal - total # of characters.
    htotal  = CRTC_RD(REGISTER_BASE, 0x00);
    htotal |= (CRTC_RD(REGISTER_BASE, 0x2D) & 0x01) << 8;

    htotal = htotal + 5;

    // hdisplayend - character count of the last displayed character (total - 1)
    hdisplayend  = CRTC_RD(REGISTER_BASE, 0x01);
    hdisplayend |= (CRTC_RD(REGISTER_BASE, 0x2D) & 0x02) << 7;

    // hblankstart - character count of the start of the blank.
    //
    // !!! this is doced to be the same at hdisplayend, but that would imply 
    //    that this value is one before the start of the blank.
    //
    hblankstart  = CRTC_RD(REGISTER_BASE, 0x02);
    hblankstart |= (CRTC_RD(REGISTER_BASE, 0x2D) & 0x04) << 6;

    // hblankend - character count to the end of the blank.
    if (CRTC_RD(REGISTER_BASE, 0x1A) & 0x04)
    {
        hblankend  = hblankstart & 0xFFFFFF80;
        hblankend |= (CRTC_RD(REGISTER_BASE, 0x25) & 0x10) << 2;
    }
    else
    {
        hblankend  = hblankstart & 0xFFFFFFC0;
    }

    hblankend |= (CRTC_RD(REGISTER_BASE, 0x03) & 0x1F);
    hblankend |= (CRTC_RD(REGISTER_BASE, 0x05) & 0x80) >> 2;

    // hretracestart - character count to the start of the retrace.
    hretracestart  = CRTC_RD(REGISTER_BASE, 0x04);
    hretracestart |= (CRTC_RD(REGISTER_BASE, 0x2D) & 0x08) << 5;

    // hretraceend
    hretraceend  = hretracestart & 0xFFFFFFE0;
    hretraceend |= (CRTC_RD(REGISTER_BASE, 0x05) & 0x1F);

    // vtotal
    vtotal  = CRTC_RD(REGISTER_BASE, 0x06);
    vtotal |= (CRTC_RD(REGISTER_BASE, 0x07) & 0x01) << 8;
    vtotal |= (CRTC_RD(REGISTER_BASE, 0x07) & 0x20) << 4;
    vtotal |= (CRTC_RD(REGISTER_BASE, 0x25) & 0x01) << 10;
    //vtotal |= (CRTC_RD(REGISTER_BASE, 0x41) & 0x01) << 11;

    vtotal += 2;

    // vdisplayend
    vdisplayend  = CRTC_RD(REGISTER_BASE, 0x12);
    vdisplayend |= (CRTC_RD(REGISTER_BASE, 0x07) & 0x02) << 7;
    vdisplayend |= (CRTC_RD(REGISTER_BASE, 0x07) & 0x40) << 3;
    vdisplayend |= (CRTC_RD(REGISTER_BASE, 0x25) & 0x02) << 9;
    //vdisplayend |= (CRTC_RD(REGISTER_BASE, 0x41) & 0x04) << 9;

    //vblankstart
    vblankstart  = CRTC_RD(REGISTER_BASE, 0x15);
    vblankstart |= (CRTC_RD(REGISTER_BASE, 0x07) & 0x08) << 5;
    vblankstart |= (CRTC_RD(REGISTER_BASE, 0x09) & 0x20) << 4;
    vblankstart |= (CRTC_RD(REGISTER_BASE, 0x25) & 0x08) << 7;
    //vblankstart |= (CRTC_RD(REGISTER_BASE, 0x41) & 0x40) << 5;
   
    // vblankend
    // --- I have no idea where the high bits come from.  Just use vtotal for now.
    vblankend  = vtotal & 0xFFFFFF00;
    vblankend |= CRTC_RD(REGISTER_BASE, 0x16);

    // retracestart
    vretracestart  = CRTC_RD(REGISTER_BASE, 0x10);
    vretracestart |= (CRTC_RD(REGISTER_BASE, 0x07) & 0x04) << 6;
    vretracestart |= (CRTC_RD(REGISTER_BASE, 0x07) & 0x80) << 2;
    vretracestart |= (CRTC_RD(REGISTER_BASE, 0x25) & 0x04) << 8;
    //vretracestart |= (CRTC_RD(REGISTER_BASE, 0x41) & 0x10) << 7;

    // retraceend
    vretraceend   = vretracestart & 0xFFFFFFF0;
    vretraceend |= CRTC_RD(REGISTER_BASE, 0x11) & 0xF;

    // Show it.
    ResPrintf("CRTC settings:\r\n\r\n");

    ResPrintf("HTOTAL          - %d, programmed as %d\r\n", htotal, htotal - 5);
    ResPrintf("HDISPLAYEND     - %d\r\n", hdisplayend);
    ResPrintf("HBLANKSTART     - %d\r\n", hblankstart);
    ResPrintf("HBLANKEND       - %d\r\n", hblankend);
    ResPrintf("HRETRACESTART   - %d\r\n", hretracestart);
    ResPrintf("HRETRACEEND     - %d\r\n\r\n", hretraceend);

    ResPrintf("VTOTAL          - %d, programmed as %d\r\n", vtotal, vtotal - 2);
    ResPrintf("VDISPLAYEND     - %d\r\n", vdisplayend);
    ResPrintf("VBLANKSTART     - %d\r\n", vblankstart);
    ResPrintf("VBLANKEND       - %d\r\n", vblankend);
    ResPrintf("VRETRACESTART   - %d\r\n", vretracestart);
    ResPrintf("VRETRACEEND     - %d\r\n\r\n", vretraceend);

    ResPrintf("H active        - %d\r\n", (hdisplayend + 1) * 8);
    ResPrintf("H front porch   - %d\r\n", (hretracestart - hdisplayend) * 8);
    ResPrintf("H sync          - %d\r\n", (hretraceend - hretracestart) * 8);
    ResPrintf("H back porch    - %d\r\n", (hblankend - hretraceend) * 8);
    ResPrintf("H blank (f+s+b) - %d\r\n", (hblankend - hblankstart) * 8);
    ResPrintf("H total         - %d\r\n\r\n", htotal * 8);

    ResPrintf("V active        - %d\r\n", vdisplayend + 1);
    ResPrintf("V front porch   - %d\r\n", vretracestart - vdisplayend);
    ResPrintf("V sync          - %d\r\n", vretraceend - vretracestart);
    ResPrintf("V back porch    - %d\r\n", vblankend - vretraceend);
    ResPrintf("V blank (f+s+b) - %d\r\n", vblankend - vblankstart);
    ResPrintf("V total         - %d\r\n", vtotal);

    ResPrintf("\r\n");

    /*
     * FP
     */

    // FP_DEBUG_0
    Value = REG_RD32(REGISTER_BASE, 0x680880);

    ResPrintf("NV_PRAMDAC_FP_DEBUG_0 (0x00680880 = 0x%08X)\r\n\r\n", Value);

    ResPrintf("XSCALE          : %s\r\n", Enabled(Value & 0x00000001));
    ResPrintf("XSCALE_STEP_AUTO: %s\r\n", Enabled(Value & 0x00000004));
    ResPrintf("YSCALE          : %s\r\n", Enabled(Value & 0x00000010));
    ResPrintf("YSCALE_STEP_AUTO: %s\r\n", Enabled(Value & 0x00000010));
    ResPrintf("XINTERP         : %s\r\n", (Value & 0x00000100) ? "bilinear" : "truncate");
    ResPrintf("YINTERP         : %s\r\n", (Value & 0x00001000) ? "bilinear" : "truncate");

    switch(Value & 0x00030000)
    {
    case 0x00000000:
        ResPrintf("VCNTR           : none\r\n");
        break;
    case 0x00010000:
        ResPrintf("VCNTR           : vcntr\r\n");
        break;
    case 0x00020000:
        ResPrintf("VCNTR           : newpix\r\n");
        break;
    case 0x00030000:
        ResPrintf("VCNTR           : both\r\n");
        break;
    }

    ResPrintf("XWEIGHT         : %s\r\n", (Value & 0x00100000) ? "round" : "truncate");
    ResPrintf("YWEIGHT         : %s\r\n", (Value & 0x01000000) ? "round" : "truncate");
    ResPrintf("PWRDOWN         : %s\r\n", (Value & 0x10000000) ? "fpclk" : "none");
    ResPrintf("PWRDOWN_TMDS_PLL: %s\r\n", Enabled(Value & 0x20000000));

    // FP_DEBUG_6
    Value = REG_RD32(REGISTER_BASE, 0x680898);

    ResPrintf("\r\nNV_PRAMDAC_FP_DEBUG_6 (0x00680898 = 0x%08X)\r\n\r\n", Value);

    if (Value == 0x10000000)
    {
        ResPrintf("XSCALE_VALUE    : noscale\r\n");
    }
    else
    {
        ResPrintf("XSCALE_VALUE    : 0x%5X\r\n", Value >> 10);
    }

    // FP_DEBUG_7
    Value = REG_RD32(REGISTER_BASE, 0x68089C);

    ResPrintf("\r\nNV_PRAMDAC_FP_DEBUG_7 (0x0068089C = 0x%08X)\r\n\r\n", Value);

    if (Value == 0x10000000)
    {
        ResPrintf("YSCALE_VALUE    : noscale\r\n");
    }
    else
    {
        ResPrintf("YSCALE_VALUE    : 0x%5X\r\n", Value >> 10);
    }

    // FP_TVO_SETUP
    Value = REG_RD32(REGISTER_BASE, 0x6808C0);

    ResPrintf("\r\nNV_PRAMDAC_FP_TVO_SETUP (0x006808C0 = 0x%08X)\r\n\r\n", Value);

    ResPrintf("TYPE            : %s\r\n", (Value & 0x00000001) ? "master" : "slave");

    switch(Value & 0x00000030)
    {
    case 0x00000000:
        ResPrintf("DATA_FORMAT     : MODE_1X00\r\n");
        break;
    case 0x00000010:
        ResPrintf("DATA_FORMAT     : MODE_0110\r\n");
        break;
    case 0x00000020:
        ResPrintf("DATA_FORMAT     : MODE_0000\r\n");
        break;
    case 0x00000030:
        ResPrintf("DATA_FORMAT     : reserved\r\n");
        break;
    }

    ResPrintf("DATA_OUT        : %s\r\n", (Value & 0x00000100) ? "tvo" : "fp");

    // FP_TVO_BLANK_COLOR
    Value = REG_RD32(REGISTER_BASE, 0x6808C4);

    ResPrintf("\r\nNV_PRAMDAC_FP_TVO_BLANK_COLOR (0x006808C4 = 0x%08X)\r\n\r\n", Value);

    ResPrintf("BLANK_COLOR_VAL : %06X\r\n", (Value & 0x00FFFFFF));
   
    switch(Value & 0xC0000000)
    {
    case 0x00000000:
        ResPrintf("BLANK_COLOR_EN  : default\r\n");
        break;
    case 0x40000000:
        ResPrintf("BLANK_COLOR_EN  : undefined\r\n");
        break;
    case 0x80000000:
        ResPrintf("BLANK_COLOR_EN  : blank\r\n");
        break;
    case 0xC0000000:
        ResPrintf("BLANK_COLOR_EN  : always\r\n");
        break;
    }

    // FP_INACTIVE_PXL_COLOR
    Value = REG_RD32(REGISTER_BASE, 0x68084C);

    ResPrintf("\r\nNV_PRAMDAC_FP_INACTIVE_PXL_COLOR (0x0068084C = 0x%08X)\r\n", Value);

    // COMPOSITE
    Value = REG_RD32(REGISTER_BASE, 0x680630);

    ResPrintf("\r\nNV_PRAMDAC_COMPOSITE (0x00680630 = 0x%08X)\r\n\r\n", Value);

    switch(Value & 0x3)
    {
    case 0:
        ResPrintf("COMPOSITE_MODE  : RGB\r\n");
        break;
    case 1:
        ResPrintf("COMPOSITE_MODE  : reserved\r\n");
        break;
    case 2:
        ResPrintf("COMPOSITE_MODE  : YCrCb_601\r\n");
        break;
    case 3:
        ResPrintf("COMPOSITE_MODE  : YCrCb_709\r\n");
        break;
    }

    // TG_CONTROL
    Value = REG_RD32(REGISTER_BASE, 0x680848);

    ResPrintf("\r\nNV_PRAMDAC_FP_TG_CONTROL (0x00680848 = 0x%08X)\r\n\r\n", Value);

    switch(Value & 0x3)
    {
    case 0x0:
        ResPrintf("VSYNC           : neg\r\n");
        break;
    case 0x1:
        ResPrintf("VSYNC           : pos\r\n");
        break;
    case 0x2:
        ResPrintf("VSYNC           : disabled\r\n");
        break;
    case 0x3:
        ResPrintf("VSYNC           : reserved\r\n");
        break;
    }

    switch(Value & 0x30)
    {
    case 0x00:
        ResPrintf("HSYNC           : neg\r\n");
        break;
    case 0x10:
        ResPrintf("HSYNC           : pos\r\n");
        break;
    case 0x20:
        ResPrintf("HSYNC           : disabled\r\n");
        break;
    case 0x30:
        ResPrintf("HSYNC           : reserved\r\n");
        break;
    }

    switch(Value & 0x300)
    {
    case 0x000:
        ResPrintf("MODE            : scale\r\n");
        break;
    case 0x100:
        ResPrintf("MODE            : center\r\n");
        break;
    case 0x200:
        ResPrintf("MODE            : native\r\n");
        break;
    case 0x300:
        ResPrintf("MODE            : undefined\r\n");
        break;
    }

    switch(Value & 0x3000)
    {
    case 0x0000:
        ResPrintf("CENTER          : none\r\n");
        break;
    case 0x1000:
        ResPrintf("CENTER          : horizontal\r\n");
        break;
    case 0x2000:
        ResPrintf("CENTER          : vertical\r\n");
        break;
    case 0x3000:
        ResPrintf("CENTER          : both\r\n");
        break;
    }

    switch(Value & 0x30000)
    {
    case 0x00000:
        ResPrintf("NATIVE          : none\r\n");
        break;
    case 0x10000:
        ResPrintf("NATIVE          : horizontal\r\n");
        break;
    case 0x20000:
        ResPrintf("NATIVE          : vertical\r\n");
        break;
    case 0x30000:
        ResPrintf("NATIVE          : both\r\n");
        break;
    }

    ResPrintf("READ            : %s\r\n", (Value & 0x00100000) ? "prog" : "actual");
    ResPrintf("WIDTH           : %s\r\n", (Value & 0x01000000) ? "12" : "24");
    
    switch(Value & 0x30000000)
    {
    case 0x0000000:
        ResPrintf("DISPEN          : neg\r\n");
        break;
    case 0x10000000:
        ResPrintf("DISPEN          : pos\r\n");
        break;
    case 0x20000000:
        ResPrintf("DISPEN          : disabled\r\n");
        break;
    case 0x30000000:
        ResPrintf("DISPEN          : reserved\r\n");
        break;
    }

    ResPrintf("FPCLK_RATIO     : %s\r\n", (Value & 0x80000000) ? "db2" : "db2");


    // Display values.

    ResPrintf("\r\n");
    ResPrintf("HDISPLAY_END (0x00680820) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680820), REG_RD32(REGISTER_BASE, 0x680820));
    ResPrintf("HTOTAL       (0x00680824) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680824), REG_RD32(REGISTER_BASE, 0x680824));
    ResPrintf("HCRTC        (0x00680828) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680828), REG_RD32(REGISTER_BASE, 0x680828));
    ResPrintf("HSYNC_START  (0x0068082C) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x68082C), REG_RD32(REGISTER_BASE, 0x68082C));
    ResPrintf("HSYNC_END    (0x00680830) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680830), REG_RD32(REGISTER_BASE, 0x680830));
    ResPrintf("HVALID_START (0x00680834) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680834), REG_RD32(REGISTER_BASE, 0x680834));
    ResPrintf("HVALID_END   (0x00680838) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680838), REG_RD32(REGISTER_BASE, 0x680838));
    ResPrintf("\r\n");
    ResPrintf("VDISPLAY_END (0x00680800) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680800), REG_RD32(REGISTER_BASE, 0x680800));
    ResPrintf("VTOTAL       (0x00680804) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680804), REG_RD32(REGISTER_BASE, 0x680804));
    ResPrintf("VCRTC        (0x00680808) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680808), REG_RD32(REGISTER_BASE, 0x680808));
    ResPrintf("VSYNC_START  (0x0068080C) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x68080C), REG_RD32(REGISTER_BASE, 0x68080C));
    ResPrintf("VSYNC_END    (0x00680810) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680810), REG_RD32(REGISTER_BASE, 0x680810));
    ResPrintf("VVALID_START (0x00680814) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680814), REG_RD32(REGISTER_BASE, 0x680814));
    ResPrintf("VVALID_END   (0x00680818) : %d (0x%X)\r\n", REG_RD32(REGISTER_BASE, 0x680818), REG_RD32(REGISTER_BASE, 0x680818));

#ifndef FOCUS

    /*
     * TV encoder
     */

    ResPrintf("\r\n");

    Value = SMB_RD(REGISTER_BASE, 0x78)
            | ((SMB_RD(REGISTER_BASE, 0x86) & 0x70) << 4);

    ResPrintf("H_ACTIVE                  : %d (0x%X)\r\n", Value, Value);

    Value = SMB_RD(REGISTER_BASE, 0x8C)
            | ((SMB_RD(REGISTER_BASE, 0x8E) & 0x08) << 5)
            | ((SMB_RD(REGISTER_BASE, 0x38) & 0x01) << 9);

    ResPrintf("H_BLANKI                  : %d (0x%X)\r\n", Value, Value);

    Value = SMB_RD(REGISTER_BASE, 0x80)
            | ((SMB_RD(REGISTER_BASE, 0x9A) & 0xC0) << 2);

    ResPrintf("H_BLANK0                  : %d (0x%X)\r\n", Value, Value);

    Value = SMB_RD(REGISTER_BASE, 0x8A)
            | ((SMB_RD(REGISTER_BASE, 0x8e) & 0x07) << 8);

    ResPrintf("H_CLKI                    : %d (0x%X)\r\n", Value, Value);

    Value = SMB_RD(REGISTER_BASE, 0x76)
            | ((SMB_RD(REGISTER_BASE, 0x86) & 0x0F) << 8);

    ResPrintf("H_CLKO                    : %d (0x%X)\r\n", Value, Value);

    ResPrintf("\r\n");

    Value = SMB_RD(REGISTER_BASE, 0x94)
            | ((SMB_RD(REGISTER_BASE, 0x96) & 0x0C) << 6);

    ResPrintf("V_ACTIVE (input)          : %d (0x%X)\r\n", Value, Value);

    Value = SMB_RD(REGISTER_BASE, 0x84)
            | ((SMB_RD(REGISTER_BASE, 0x86) & 0x80) << 1);

    ResPrintf("V_ACTIVEO                 : %d (0x%X)\r\n", Value, Value);

    Value = SMB_RD(REGISTER_BASE, 0x92);

    ResPrintf("V_BLANKI                  : %d (0x%X)\r\n", Value, Value);

    Value = SMB_RD(REGISTER_BASE, 0x82);

    ResPrintf("V_BLANKO (# blank + 1)    : %d (0x%X)\r\n", Value, Value);

    Value = SMB_RD(REGISTER_BASE, 0x90)
            | ((SMB_RD(REGISTER_BASE, 0x96) & 0x3) << 8)
            | ((SMB_RD(REGISTER_BASE, 0x38) & 0x2) << 9);

    ResPrintf("V_LINESI                  : %d (0x%X)\r\n", Value, Value);

    Value = SMB_RD(REGISTER_BASE, 0x98)
            | ((SMB_RD(REGISTER_BASE, 0x9A) & 0x1F) << 8);

    ResPrintf("V_SCALE                   : %d (0x%X)\r\n", Value, Value);

#endif

#endif 0

}

//----------------------------------------------------------------------------
// Change/display the graphics mode.
//
BOOL Mode()
{
    ULONG i, c;
    BOOL retval;

    c = Direct3D_GetAdapterModeCount(0) / 4;

    if (g_Argc != 2 || strcmp(g_Argv[1], "+") && !isdigit(*g_Argv[1]))
    {
        ResPrintf("\r\n!m [<mode to set>]\r\n\r\n");
        ResPrintf("    + - advance to next mode\r\n\r\n");

        for (i = 0; i < c; i++)
        {
            ResPrintf("    ");
            ResPrintMode(i);
        }
        
        retval = FALSE;
    }
    else 
    {
        if (!strcmp(g_Argv[1], "+"))
        {
            g_iMode = (g_iMode + 1) % c;
        }
        else
        {
            g_iMode = atoi(g_Argv[1]) % c;
        }

        retval = TRUE;
    }

    return retval;
}

//----------------------------------------------------------------------------
// Change the current AV pack.
//
BOOL AvPack()
{
    ULONG c;
    BOOL retval;

    if (g_Argc != 2 || strcmp(g_Argv[1], "+") && !isdigit(*g_Argv[1]))
    {
        ResPrintf("\r\n!a [<av pack to set>]\r\n\r\n");
        ResPrintf("    + - advance to next pack\r\n\r\n");
        ResPrintf("    1: Standard\r\n");
        ResPrintf("    2: RFU\r\n");
        ResPrintf("    3: SCART\r\n");
        ResPrintf("    4: HDTV\r\n");
        ResPrintf("    5: VGA\r\n");
        ResPrintf("    6: SVIDEO\r\n");

        retval = FALSE;
    }
    else
    {
        D3D__AvInfo &= 0xFFFFFF00;

        if (!strcmp(g_Argv[1], "+"))
        {
            c = D3D__AvInfo & 0xFF;

            D3D__AvInfo |= (c + 1) % AV_PACK_MAX;
        }
        else
        {
            D3D__AvInfo |= atoi(g_Argv[1]) % AV_PACK_MAX;
        }

        // Skip bad modes.
        c = D3D__AvInfo & 0xFF;

        if (c == 0)
        {
            D3D__AvInfo++;
        }

        g_iMode = 0;
        retval = TRUE;
    }

    return retval;
}

BOOL Standard()
{
    ULONG c;
    BOOL retval;

    if (g_Argc != 2 || strcmp(g_Argv[1], "+") && !isdigit(*g_Argv[1]))
    {
        ResPrintf("\r\n!r [<region to set>]\r\n\r\n");
        ResPrintf("    + - advance to next standard\r\n\r\n");
        ResPrintf("    1: NTSC-M\r\n");
        ResPrintf("    2: NSTC-J\r\n");
        ResPrintf("    3: PAL-I\r\n");
        ResPrintf("    4: PAL-M\r\n");

        retval = FALSE;
    }
    else
    {
        D3D__AvInfo &= 0xFFFF00FF;

        if (!strcmp(g_Argv[1], "+"))
        {
            c = (D3D__AvInfo & 0xFF00) >> 8;

            D3D__AvInfo |= ((c + 1) % AV_STANDARD_MAX) << 8;
        }
        else
        {
            D3D__AvInfo |= (atoi(g_Argv[1]) % AV_STANDARD_MAX) << 8;
        }

        // Remove unsupported modes.
        c = D3D__AvInfo & 0xFF00;

        if (c == 0)
        {
            D3D__AvInfo += 0x100;
        }

        // Set to the default refresh rate.
        c = D3D__AvInfo & 0xFF00;

        if (c == AV_STANDARD_PAL_I)
        {
            D3D__AvInfo = (D3D__AvInfo & ~AV_REFRESH_MASK) | AV_FLAGS_50Hz;
        }
        else
        {
            D3D__AvInfo = (D3D__AvInfo & ~AV_REFRESH_MASK) | AV_FLAGS_60Hz;
        }

        g_iMode = 0;
        retval = TRUE;
    }

    return retval;
}

void Change()
{
    ULONG Register;
    ULONG Value;
    ULONG Active, FrontPorch, Sync, BackPorch;
    ULONG Total, DisplayEnd, BlankStart, BlankEnd, RetraceStart, RetraceEnd;

    if (g_Argc < 4)
    {
ChangeUsage:
        ResPrintf("\r\n!c <type> <parameters>\r\n\r\n");
        ResPrintf("    * Register numbers are always in hex, value is either hex (0x...) or decimal.\r\n\r\n");
        ResPrintf("    crtc <register number> <value>\r\n");
        ResPrintf("    tv <register number> <value>\r\n");
        ResPrintf("    reg <register number> <value>\r\n\r\n");

        ResPrintf("    vcrtctime <active> <front porch> <sync> <back porch>\r\n");
        ResPrintf("    hcrtctime <active> <front porch> <sync> <back porch>\r\n");
    }
    else
    {
        if (!strcmp(g_Argv[1], "crtc"))
        {
            Register = strtoul(g_Argv[2], NULL, 16);
            Value    = strtoul(g_Argv[3], NULL, 0);

            CRTC_WR(REGISTER_BASE, (UCHAR)Register, (UCHAR)Value);
            ResPrintf("CR%02X = 0x%02X\r\n", Register, Value);
        }
        else if (!strcmp(g_Argv[1], "reg"))
        {
            Register = strtoul(g_Argv[2], NULL, 16);
            Value    = strtoul(g_Argv[3], NULL, 0);

            REG_WR32(REGISTER_BASE, Register, Value);
            ResPrintf("Register 0x%08X = 0x%08X\r\n", Register, Value);
        }
        else if (!strcmp(g_Argv[1], "tv"))
        {
            Register = strtoul(g_Argv[2], NULL, 16);
            Value    = strtoul(g_Argv[3], NULL, 0);

            SMB_WR(REGISTER_BASE, (UCHAR)Register, (UCHAR)Value);
            ResPrintf("TV%02X = 0x%02X\r\n", Register, Value);
        }
        else if (!strcmp(g_Argv[1], "vcrtctime"))
        {
            if (g_Argc != 6)
            {
                goto ChangeUsage;
            }

            Active     = strtoul(g_Argv[2], NULL, 0);
            FrontPorch = strtoul(g_Argv[3], NULL, 0);
            Sync       = strtoul(g_Argv[4], NULL, 0);
            BackPorch  = strtoul(g_Argv[5], NULL, 0);
            
            Total        = Active + FrontPorch + Sync + BackPorch;
            DisplayEnd   = Active;
            BlankStart   = Active;
            BlankEnd     = Active + FrontPorch + Sync + BackPorch;
            RetraceStart = Active + FrontPorch;
            RetraceEnd   = RetraceStart + Sync;

            // vtotal is # of lines - 2.
            Total -= 2;

            CRTC_WR(REGISTER_BASE, 0x06, (UCHAR)(Total & 0xFF));
            CRTC_WR(REGISTER_BASE, 0x07, (CRTC_RD(REGISTER_BASE, 0x07) & ~0x01) | (((UCHAR)(Total >> 8) & 0x01) << 0));
            CRTC_WR(REGISTER_BASE, 0x07, (CRTC_RD(REGISTER_BASE, 0x07) & ~0x20) | (((UCHAR)(Total >> 9) & 0x01) << 5));
            CRTC_WR(REGISTER_BASE, 0x25, (CRTC_RD(REGISTER_BASE, 0x25) & ~0x01) | (((UCHAR)(Total >> 10) & 0x01) << 0));
            CRTC_WR(REGISTER_BASE, 0x41, (CRTC_RD(REGISTER_BASE, 0x41) & ~0x01) | (((UCHAR)(Total >> 11) & 0x01) << 0));

            DisplayEnd -= 1;

            CRTC_WR(REGISTER_BASE, 0x12, (UCHAR)DisplayEnd & 0xFF);
            CRTC_WR(REGISTER_BASE, 0x07, (CRTC_RD(REGISTER_BASE, 0x07) & ~0x02) | (((UCHAR)(DisplayEnd >> 8) & 0x01) << 1));
            CRTC_WR(REGISTER_BASE, 0x07, (CRTC_RD(REGISTER_BASE, 0x07) & ~0x40) | (((UCHAR)(DisplayEnd >> 9) & 0x01) << 6));
            CRTC_WR(REGISTER_BASE, 0x25, (CRTC_RD(REGISTER_BASE, 0x25) & ~0x02) | (((UCHAR)(DisplayEnd >> 10) & 0x01) << 1));
            CRTC_WR(REGISTER_BASE, 0x41, (CRTC_RD(REGISTER_BASE, 0x41) & ~0x04) | (((UCHAR)(DisplayEnd >> 11) & 0x01) << 2));

            BlankStart -= 1;

            CRTC_WR(REGISTER_BASE, 0x15, (UCHAR)BlankStart & 0xFF);
            CRTC_WR(REGISTER_BASE, 0x07, (CRTC_RD(REGISTER_BASE, 0x07) & ~0x08) | (((UCHAR)(BlankStart >> 8) & 0x01) << 3));
            CRTC_WR(REGISTER_BASE, 0x09, (CRTC_RD(REGISTER_BASE, 0x09) & ~0x20) | (((UCHAR)(BlankStart >> 9) & 0x01) << 5));
            CRTC_WR(REGISTER_BASE, 0x25, (CRTC_RD(REGISTER_BASE, 0x25) & ~0x08) | (((UCHAR)(BlankStart >> 10) & 0x01) << 3));
            CRTC_WR(REGISTER_BASE, 0x41, (CRTC_RD(REGISTER_BASE, 0x41) & ~0x40) | (((UCHAR)(BlankStart >> 11) & 0x01) << 6));

            BlankEnd -= 1;

            CRTC_WR(REGISTER_BASE, 0x16, (UCHAR)BlankEnd & 0xFF);

            RetraceStart -= 1;

            CRTC_WR(REGISTER_BASE, 0x10, (UCHAR)RetraceStart & 0xFF);
            CRTC_WR(REGISTER_BASE, 0x07, (CRTC_RD(REGISTER_BASE, 0x07) & ~0x04) | (((UCHAR)(RetraceStart >> 8) & 0x01) << 2));
            CRTC_WR(REGISTER_BASE, 0x07, (CRTC_RD(REGISTER_BASE, 0x07) & ~0x80) | (((UCHAR)(RetraceStart >> 9) & 0x01) << 7));
            CRTC_WR(REGISTER_BASE, 0x25, (CRTC_RD(REGISTER_BASE, 0x25) & ~0x04) | (((UCHAR)(RetraceStart >> 10) & 0x01) << 2));
            CRTC_WR(REGISTER_BASE, 0x41, (CRTC_RD(REGISTER_BASE, 0x41) & ~0x10) | (((UCHAR)(RetraceStart >> 11) & 0x01) << 4));

            RetraceEnd -= 1;

            CRTC_WR(REGISTER_BASE, 0x11, (CRTC_RD(REGISTER_BASE, 0x11) & ~0x0F) | (((UCHAR)(RetraceEnd >> 0) & 0x0F) << 0));
        }
        else if (!strcmp(g_Argv[1], "hcrtctime"))
        {
            if (g_Argc != 6)
            {
                goto ChangeUsage;
            }

            Active     = strtoul(g_Argv[2], NULL, 0);
            FrontPorch = strtoul(g_Argv[3], NULL, 0);
            Sync       = strtoul(g_Argv[4], NULL, 0);
            BackPorch  = strtoul(g_Argv[5], NULL, 0);
            
            Total        = Active + FrontPorch + Sync + BackPorch;
            DisplayEnd   = Active;
            BlankStart   = Active;
            BlankEnd     = Active + FrontPorch + Sync + BackPorch;
            RetraceStart = Active + FrontPorch;
            RetraceEnd   = RetraceStart + Sync;

            Total = Total / 8 - 5;

            CRTC_WR(REGISTER_BASE, 0x00, (UCHAR)(Total & 0xFF));
            CRTC_WR(REGISTER_BASE, 0x2D, (CRTC_RD(REGISTER_BASE, 0x2D) & ~0x01) | (((UCHAR)(Total >> 8) & 0x01) << 0));

            DisplayEnd = DisplayEnd / 8 - 1;

            CRTC_WR(REGISTER_BASE, 0x01, (UCHAR)DisplayEnd & 0xFF);
            CRTC_WR(REGISTER_BASE, 0x2D, (CRTC_RD(REGISTER_BASE, 0x2D) & ~0x02) | (((UCHAR)(DisplayEnd >> 8) & 0x01) << 1));

            BlankStart = BlankStart / 8 - 1;

            CRTC_WR(REGISTER_BASE, 0x02, (UCHAR)BlankStart & 0xFF);
            CRTC_WR(REGISTER_BASE, 0x2D, (CRTC_RD(REGISTER_BASE, 0x2D) & ~0x04) | (((UCHAR)(BlankStart >> 8) & 0x01) << 2));

            BlankEnd = BlankEnd / 8 - 1;

            CRTC_WR(REGISTER_BASE, 0x03, (CRTC_RD(REGISTER_BASE, 0x03) & ~0x1F) | (((UCHAR)(BlankEnd >> 0) & 0x1F) << 0));
            CRTC_WR(REGISTER_BASE, 0x05, (CRTC_RD(REGISTER_BASE, 0x05) & ~0x80) | (((UCHAR)(BlankEnd >> 5) & 0x1F) << 7));

            if (CRTC_RD(REGISTER_BASE, 0x1A) & 0x04)
            {
                CRTC_WR(REGISTER_BASE, 0x25, (CRTC_RD(REGISTER_BASE, 0x25) & ~0x10) | (((UCHAR)(BlankEnd >> 6) & 0x01) << 4));
            }

            RetraceStart = RetraceStart / 8 - 1;

            CRTC_WR(REGISTER_BASE, 0x04, (UCHAR)RetraceStart & 0xFF);
            CRTC_WR(REGISTER_BASE, 0x2D, (CRTC_RD(REGISTER_BASE, 0x2D) & ~0x08) | (((UCHAR)(RetraceStart >> 8) & 0x01) << 3));

            RetraceEnd = RetraceEnd / 8 - 1;

            CRTC_WR(REGISTER_BASE, 0x05, (CRTC_RD(REGISTER_BASE, 0x05) & ~0x1F) | (((UCHAR)(RetraceEnd >> 0) & 0x1F) << 0));
        }
        else
        {
            goto ChangeUsage;
        }
    }
}

void Dump()
{

#if 0

    SMB_RD(REGISTER_BASE, 4);

#else

    ULONG i;

    for (i = 0; i < sizeof(g_CRTCMap); i++)
    {
        ResPrintf("CR%02X = %02X\r\n", g_CRTCMap[i], CRTC_RD(REGISTER_BASE, g_CRTCMap[i]));
    }

    ResPrintf("\r\n");

#ifndef FOCUS

    for (i = 0; i < sizeof(g_TVMap); i++)
    {
        ResPrintf("TV%02X = %02X\r\n", g_TVMap[i], SMB_RD(REGISTER_BASE, g_TVMap[i]));
    }

#else

    for (i = 0; i < 0xBE; i++)
    {
        ResPrintf("TV%02X = %02X\r\n", i, SMB_RD(REGISTER_BASE, (UCHAR)i));
    }


#endif

#endif 0

}

void Macrovision()
{
    ULONG Value;
    ULONG Results;

    if (g_Argc != 2 || !isdigit(*g_Argv[1]))
    {
        ResPrintf("!v <mode>\r\n\r\n");
        ResPrintf("    0: off\r\n");
        ResPrintf("    1: AGC\r\n");
        ResPrintf("    2: two stripes plus AGC\r\n");
        ResPrintf("    3: four stripes plus AGC\r\n");
    }
    else
    {
        Value = atoi(g_Argv[1]);

        D3DDevice_BlockUntilVerticalBlank();
        AvSendTVEncoderOption(REGISTER_BASE, AV_OPTION_MACROVISION_MODE, Value, &Results);

        D3DDevice_BlockUntilVerticalBlank();
        AvSendTVEncoderOption(REGISTER_BASE, AV_OPTION_MACROVISION_COMMIT, Value, &Results);
    }
}

void OverscanColor()
{
    ULONG Value;
    ULONG Results;

    if (g_Argc != 2 || !isdigit(*g_Argv[1]))
    {
        ResPrintf("!l <color>\r\n\r\n");
    }
    else
    {
        Value = strtoul(g_Argv[1], NULL, 16);

        D3DDevice_SetOverscanColor(Value);
        Value = D3DDevice_GetOverscanColor();

        ResPrintf("Color = 0x%08X\r\n", Value);
    }
}

void FlickerFilter()
{
    ULONG Value;
    ULONG Results;

    if (g_Argc != 2 || !isdigit(*g_Argv[1]))
    {
        ResPrintf("!v <mode>\r\n\r\n");
        ResPrintf("    0: off\r\n");
        ResPrintf("    1 - 4: # of taps\r\n");
        ResPrintf("    5: adaptive\r\n");
    }
    else
    {
        Value = atoi(g_Argv[1]);

        if (Value > 6)
        {

            Value = 5;
        }

        D3DDevice_SetFlickerFilter(Value);
    }
}

//----------------------------------------------------------------------------
// Parse a command string into its little subparts.
//
char *ParseCommand(
    char *szCommand
    )
{
    char **szArgv;
    char *szMax;
    char *szNext;

    unsigned i, c, to;

    g_Argc = 0;
    memset(g_Argv, 0, sizeof(g_Argv));

    if (!szCommand)
    {
        return NULL;
    }

    // Skip over the prefix.
    szCommand++;

    // Lowercase and whack backspaces.
    c  = strlen(szCommand);
    to = 0;

    for (i = 0; i < c; i++)
    {
        if (szCommand[i] == 0x08)
        {
            if (to)
            {
                to--;
            }
        }
        else if (szCommand[i] == '!')
        {
            break;
        }
        else
        {
            szCommand[to] = (char)tolower(szCommand[i]);
            to++;
        }
    }

    // Do we have multiple commands?
    if (i < c)
    {
        szNext = szCommand + i;
    }
    else
    {
        szNext = NULL;
    }

    // Remember the end.
    szMax = szCommand + to;

    // Walk the commands, splitting each one out.
    szArgv = g_Argv;

    for (; szCommand < szMax; szCommand++)
    {
        if (iswspace(*szCommand))
        {
            if (*szArgv)
            {
                *szCommand = 0;
                szArgv++;
            }
        }
        else
        {
            if (!*szArgv)
            {
                *szArgv = szCommand;
            }
        }
    }

    if (*szArgv)
    {
        *szCommand = 0;
        szArgv++;
    }

    g_Argc = szArgv - g_Argv;

    return szNext;
}

//----------------------------------------------------------------------------
// Output handler.  Called by the debug monitor to dump our text buffer.
//
HRESULT __stdcall 
RXCmdCont(
    PDM_CMDCONT pdmcc, 
    LPSTR szResponse, 
    DWORD cchResponse
    )
{
    pdmcc->DataSize = g_cchBuffer;
    g_cchBuffer = 0;

    if (pdmcc->DataSize)
    {
        return XBDM_NOERR;
    }
    else
    {
        return XBDM_ENDOFLIST;  
    }
}

//----------------------------------------------------------------------------
// Input handler.  This gets called by the debug monitor to have us process
// a command.
//
#define COMMAND_PREFIX  ""

HRESULT __stdcall 
RXCmdProcessorProc(
    LPCSTR szConstCommand, 
    LPSTR szResp,
    DWORD cchResp, 
    PDM_CMDCONT pdmcc
    )
{    
    BOOL ResetDisplay;
    D3DPRESENT_PARAMETERS d3dpp;

    ULONG i, c;
    ULONG Results;
    UCHAR Value;

    ULONG Red, Green, Blue;

    char *szCommand;

    ResetDisplay = FALSE;
    szCommand = (char *)szConstCommand;

    EnterCriticalSection(&g_Mutex);

    while (szCommand)
    {
        szCommand = ParseCommand(szCommand);

        if (!g_Argc)
        {
            continue;
        }

        // parse the command.
        switch (*g_Argv[0])
        {
        case 'e':

            D3D__AvInfo = 0;

            g_iMode = 0;
            ResetDisplay = TRUE;

            break;

        case 's':

            Show();
            break;

        case 'm':

            ResetDisplay = Mode();
            break;

        case 'a':

            ResetDisplay = AvPack();
            break;

        case 't':

            ResetDisplay = Standard();
            break;

        case 'w':

            if (D3D__AvInfo & AV_FLAGS_WIDESCREEN)
            {
                D3D__AvInfo &= ~AV_FLAGS_WIDESCREEN;
            }
            else
            {
                D3D__AvInfo |= AV_FLAGS_WIDESCREEN;
            }

            g_iMode = 0;
            ResetDisplay = TRUE;

            break;

        case 'o':

            g_Overlay = !g_Overlay;
            break;

        case 'h':

            if (g_Argc != 2 || !isdigit(*g_Argv[1]))
            {
                ResPrintf("\r\n!h [<mode bits>]\r\n\r\n");
                ResPrintf("    ** use !? for general command help **\r\n\r\n");
                ResPrintf("    bit 1: 720p\r\n");
                ResPrintf("    bit 2: 1080i\r\n");
                ResPrintf("    bit 3: 480i\r\n");
            }
            else
            {
                D3D__AvInfo &= ~AV_HDTV_MODE_MASK;

                D3D__AvInfo |= (atoi(g_Argv[1]) & 0x7) << 17;
            }

            g_iMode = 0;
            ResetDisplay = TRUE;

            break;

        case 'p':

            if (D3D__AvInfo & AV_FLAGS_50Hz)
            {
                if (D3D__AvInfo & AV_FLAGS_60Hz)
                {
                    D3D__AvInfo &= ~AV_FLAGS_60Hz;
                }
                else
                {
                    D3D__AvInfo |= AV_FLAGS_60Hz;
                }
            }

            g_iMode = 0;
            ResetDisplay = TRUE;

            break;

        case 'i':

            Info();
            break;

        case 'c':

            Change();
            break;

        case 'd':

            Dump();
            break;

        case 'v':

            Macrovision();
            break;

        case 'l':
   
            OverscanColor();
            break;

        case 'f':

            FlickerFilter();
            break;

        case 'j':

            if (g_Argc != 2 || !isdigit(*g_Argv[1]))
            {
                ResPrintf("\r\n!j <percent>\r\n\r\n");
            }
            else
            {
                g_SweepAttenuation = (double)atoi(g_Argv[1]) / 100;
            }

            break;

        default:

            ResPrintf("\r\n!<command> <parameters>\r\n\r\n");
            ResPrintf("    '!<command> help' for command-specific usage.\r\n\r\n");
            ResPrintf("    d[e]tect    - detect the current AV pack\r\n");
            ResPrintf("    [s]how      - display a new test pattern\r\n");
            ResPrintf("    [m]ode      - set/view video modes\r\n");
            ResPrintf("    [a]vpack    - set/view video packs\r\n");
            ResPrintf("    s[t]andard  - set/view video standard\r\n");
            ResPrintf("    [w]ide      - toggle widescreen support\r\n");
            ResPrintf("    [h]dtv      - set supported HDTV modes\r\n");
            ResPrintf("    [p]al60     - toggle pal-60 support, must be in PAL-I mode\r\n");
            ResPrintf("    [i]nfo      - dump CRTC and tv encoder information\r\n");
            ResPrintf("    [c]hange    - change the value of a crtc or tv encoder register\r\n");
            ResPrintf("    [d]ump      - dump register values\r\n");
            ResPrintf("    macro[v]ion - set the macrovision mode\r\n");
            ResPrintf("    [o]verlay   - toggle the overlay\r\n");
            ResPrintf("    [f]licker   - set flicker filter\r\n");
            ResPrintf("    add[j]ust   - set sweep adjuement (%)\r\n");
            ResPrintf("    co[l]or     - set the overscan color\r\n");

            break;
        }
    }

    if (ResetDisplay)
    {
        Direct3D_EnumAdapterModes(0, g_iMode * 4, &g_Mode);

        ZeroMemory(&d3dpp, sizeof(d3dpp));

        d3dpp.BackBufferWidth            = g_Mode.Width;
        d3dpp.BackBufferHeight           = g_Mode.Height;
        d3dpp.BackBufferFormat           = g_Mode.Format;
        d3dpp.Flags                      = g_Mode.Flags;
        d3dpp.BackBufferCount            = 1;
        d3dpp.EnableAutoDepthStencil     = TRUE;
        d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
        d3dpp.FullScreen_RefreshRateInHz = g_Mode.RefreshRate;
        //d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_TWO;

        // Clear out any cached modes.
        //AvSendTVEncoderOption(NULL, AV_OPTION_ZERO_MODE, 0, NULL);

        D3DDevice_Reset(&d3dpp);

        // Recreate the surfaces.
        CreateScreen(g_TestPattern, 
                     g_Tests[g_TestPattern].m_DefaultValue1, 
                     g_Tests[g_TestPattern].m_DefaultValue2);

        g_EnableCC = TRUE;

        g_FpsCount = 0;
        g_VpsCount = 0;
        g_FpsTime = GetTickCount();
        g_VpsTime = g_FpsTime;
    }

    SetEvent(g_hEvent);

    if (!g_cchBuffer)
    {
        ResPrintf("\r\nAV Pack    - ");
        switch(D3D__AvInfo & 0xFF)
        {
        case 1:
                ResPrintf("1: Standard\r\n");
                break;
        case 2:
                ResPrintf("2: RFU\r\n");
                break; 
        case 3:
                ResPrintf("3: SCART\r\n");
                break; 
        case 4:
                ResPrintf("4: HDTV\r\n");
                break; 
        case 5:
                ResPrintf("5: VGA\r\n");
                break;
        case 6: 
                ResPrintf("6: SVideo\r\n");
                break;
        }

#if 0
        AvpReadDeviceReg(REGISTER_BASE, SMC_SLAVE_ADDRESS, SMC_COMMAND_VIDEO_MODE, &Value);
        ResPrintf("AvPack reg - %X\r\n", Value);
#endif 0

        ResPrintf("Standard   - ");
        switch((D3D__AvInfo & 0xFF00) >> 8)
        {
        case 1:
            ResPrintf("1: NTSC-M\r\n");
            break;
        case 2:            
            ResPrintf("2: NTSC-J\r\n");
            break;
        case 3:
            ResPrintf("3: PAL-I\r\n");
            break;
        case 4:
            ResPrintf("4: PAL-M\r\n");
            break;
        }

        ResPrintf("Mode       - ");
        ResPrintMode(g_iMode);

        ResPrintf("Widescreen - ");
        
        if (D3D__AvInfo & AV_FLAGS_WIDESCREEN)
        {
            ResPrintf("enabled\r\n");
        }
        else
        {
            ResPrintf("not enabled\r\n");
        }

        ResPrintf("HTDV modes - ");

        if (!(D3D__AvInfo & AV_HDTV_MODE_MASK))
        {
            ResPrintf("none\r\n");
        }
        else
        {
            ResPrintf("480p ");

            if (D3D__AvInfo & AV_FLAGS_HDTV_720p)
            {
                ResPrintf("720p ");
            }

            if (D3D__AvInfo & AV_FLAGS_HDTV_1080i)
            {
                ResPrintf("1080i ");
            }

            ResPrintf("\r\n");
        }

        ResPrintf("PAL-60     - ");

        if (D3D__AvInfo & AV_FLAGS_50Hz)
        {
            if (D3D__AvInfo & AV_FLAGS_60Hz)
            {
                ResPrintf("enabled\r\n");
            }
            else
            {
                ResPrintf("disabled\r\n");
            }
        }
        else
        {
            ResPrintf("ignored, not in PAL-I mode\r\n");
        }

        c = GetTickCount();

        ResPrintf("Blank Rate - ");
          
        if (c - g_VpsTime > 2000)
        {
            ResPrintf("%.2f vps\r\n", (double)g_VpsCount / (double)(c - g_VpsTime) * 1000.0);
        }
        else
        {
            ResPrintf("<sampling>\r\n");
        }

        ResPrintf("Frame Rate - ");
          
        if (c - g_FpsTime > 2000)
        {
            ResPrintf("%.2f fps\r\n", (double)g_FpsCount / (double)(c - g_FpsTime) * 1000.0);
        }
        else
        {
            ResPrintf("<sampling>\r\n");
        }

        GetScreenChecksum(&Red, &Green, &Blue);

        ResPrintf("Checksum - Red: %08X, Green: %08X, Blue: %08X\r\n", Red, Green, Blue);
    }

    pdmcc->HandlingFunction = RXCmdCont;
    pdmcc->DataSize = 0;
    pdmcc->Buffer = g_szBuffer;
    pdmcc->BufferSize = g_cchBuffer;
    pdmcc->CustomData = NULL;
    pdmcc->BytesRemaining = g_cchBuffer;

    LeaveCriticalSection(&g_Mutex);

    return XBDM_MULTIRESPONSE;
}

//==============================================================================
// Frame counter.
//==============================================================================

DWORD g_DoChecksum;
DWORD g_Channel;
DWORD g_Value[3];

void __cdecl FrameCounter(D3DVBLANKDATA *pData)
{
    InterlockedIncrement(&g_VpsCount);

    if (g_DoChecksum)
    {
        if (g_Channel > 0)
        {
            g_Value[g_Channel - 1] = *(volatile DWORD *)0xFD680840 & 0x00FFFFFF;
        }

        if (g_Channel < 3)
        {
            *(volatile DWORD *)0xFD680844 = 0x411 | (g_Channel << 8);
            *(volatile DWORD *)0xFD680844 = 0x410 | (g_Channel << 8);

            g_Channel++;
        }
        else
        {
            g_Channel = 0;
            g_DoChecksum = 0;
        }
    }
}

//------------------------------------------------------------------------------
// Calculates the frame's checksum.
//
void GetScreenChecksum(
    DWORD *pRed,
    DWORD *pGreen,
    DWORD *pBlue
    )
{
    g_DoChecksum = 1;

    while (g_DoChecksum)
    {
        D3DDevice_BlockUntilVerticalBlank();
    }

    *pRed   = g_Value[0];
    *pGreen = g_Value[1];
    *pBlue  = g_Value[2];
}


//==============================================================================
// Text
//==============================================================================

char *g_6[32] =
{
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
    "00000111111111111111111111110000",
    "00011111111111111111111111111000",
    "00011100000000000000000000111000",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111111111111111111111111000000",
    "00111111111111111111111111110000",
    "00111000000000000000000000111000",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00011100000000000000000000111000",
    "00011111111111111111111111111000",
    "00000111111111111111111111100000",
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
};

char *g_7[32] =
{
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
    "00111111111111111111111111111100",
    "00111111111111111111111111111100",
    "00000000000000000000000000011100",
    "00000000000000000000000000011100",
    "00000000000000000000000000111000",
    "00000000000000000000000001110000",
    "00000000000000000000000011100000",
    "00000000000000000000000111000000",
    "00000000000000000000001110000000",
    "00000000000000000000011100000000",
    "00000000000000000000111000000000",
    "00000000000000000001110000000000",
    "00000000000000000011100000000000",
    "00000000000000000111000000000000",
    "00000000000000001110000000000000",
    "00000000000000011100000000000000",
    "00000000000000111000000000000000",
    "00000000000001110000000000000000",
    "00000000000011100000000000000000",
    "00000000000111000000000000000000",
    "00000000001110000000000000000000",
    "00000000011100000000000000000000",
    "00000000111000000000000000000000",
    "00000001110000000000000000000000",
    "00000011100000000000000000000000",
    "00000111000000000000000000000000",
    "00001110000000000000000000000000",
    "00011100000000000000000000000000",
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
};

char *g_W[32] =
{
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000000000000000011100",
    "00111000000000011000000000011100",
    "00111000000000011000000000011100",
    "00111000000000111100000000011100",
    "00111000000000111100000000011100",
    "00111000000001111110000000011100",
    "00111000000001111110000000011100",
    "00111000000011100111000000011100",
    "00111000000011100111000000011100",
    "00111000000111000011100000011100",
    "00111000000111000011100000011100",
    "00111000001110000001110000011100",
    "00111000001110000001110000011100",
    "00111000011100000000111000011100",
    "00111000011100000000111000011100",
    "00111000111000000000011100011100",
    "00111000111000000000011100011100",
    "00111001110000000000001110011100",
    "00111001110000000000001110011100",
    "00111011100000000000000111011100",
    "00111011100000000000000111011100",
    "00111111000000000000000011111100",
    "00111110000000000000000001111100",
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
};

char *g_F[32] =
{
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
    "00111111111111111111111111111100",
    "00111111111111111111111111111100",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111111111111111111000000000000",
    "00111111111111111111000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00111000000000000000000000000000",
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
};

char *g_I[32] =
{
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
    "00111111111111111111111111111100",
    "00111111111111111111111111111100",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00000000000000111100000000000000",
    "00111111111111111111111111111100",
    "00111111111111111111111111111100",
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
};

char *g_11[32] =
{
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
    "00001111100000000000011111000000",
    "00011111100000000000111111000000",
    "00111011100000000001110111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000000000000111000000",
    "00000011100000011000000111000000",
    "00111111111110111101111111111100",
    "00111111111110011001111111111100",
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
};

void DrawChar(
    UINT x,
    UINT y,
    char **ppMap
    )
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT  Rect;
    LPDWORD         pData;
    UINT i;
    char *pPixel;

    DWORD Width;
    DWORD Height;
    DWORD PitchAdjust;

    D3DSurface_GetDesc(g_pBack, &Desc);
    D3DSurface_LockRect(g_pBack, &Rect, NULL, D3DLOCK_TILED);

    Width = Desc.Width;
    Height = Desc.Height;
    PitchAdjust  = Rect.Pitch / 4 - 32;

    pData = (LPDWORD)Rect.pBits + x + y * Rect.Pitch / 4;

    for (i = 0; i < 32; i++)
    {
        pPixel = ppMap[i];

        while (*pPixel)
        {
            *pData = (*pPixel == '0') ? 0 : 0xFFFFFFFF;

            pData++;
            pPixel++;
        }

        pData += PitchAdjust;
    }

    D3DSurface_UnlockRect(g_pBack); 
}

void DrawMode()
{
    UINT x;

    x = 100;

    if (g_Mode.Width == 640)
    {
        DrawChar(x, 100, g_6);
        x += 32;
    }
    else if (g_Mode.Width == 720)
    {
        DrawChar(x, 100, g_7);
        x += 32;
    }

    if (g_Mode.Flags & D3DPRESENTFLAG_10X11PIXELASPECTRATIO)
    {
        DrawChar(x, 100, g_11);
        x += 32;
    }

    if (g_Mode.Flags & D3DPRESENTFLAG_FIELD)
    {
        DrawChar(x, 100, g_F);
        x += 32;
    }
    else if (g_Mode.Flags & D3DPRESENTFLAG_INTERLACED)
    {
        DrawChar(x, 100, g_I);
        x += 32;
    }
    
    if (g_Mode.Flags & D3DPRESENTFLAG_WIDESCREEN)
    {
        DrawChar(x, 100, g_W);
        x += 32;
    }
}

//==============================================================================
// Graphics
//==============================================================================

//------------------------------------------------------------------------------
// Intialize D3D.
//
void
Init()
{
    D3DPRESENT_PARAMETERS d3dpp;
    D3DLOCKED_RECT rect;
    DWORD *pBits;
    DWORD i;
    DWORD x, y;
    DWORD r, g, b;
    UCHAR y0, u, v;

    DWORD InputDevices;

    // Initialize the work event.
    g_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // Initialize the g_Mutex.
    InitializeCriticalSection(&g_Mutex);

    // Initialize the controller.
    XInitDevices(0, NULL);

    // Make D3D go.

    Direct3D_EnumAdapterModes(0, 0, &g_Mode);

    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = g_Mode.Width;
    d3dpp.BackBufferHeight          = g_Mode.Height;
    d3dpp.BackBufferFormat          = g_Mode.Format;
    d3dpp.Flags                     = g_Mode.Flags;
    d3dpp.BackBufferCount           = 1;
    d3dpp.EnableAutoDepthStencil    = TRUE;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;

    // Create the device.
    Direct3D_CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        NULL);

    // Enable all of the funky stuff by default.
    g_SavedAvInfo = D3D__AvInfo;
    D3D__AvInfo |= AV_FLAGS_WIDESCREEN | AV_FLAGS_60Hz;

    // Reset with the new first mode.
    Direct3D_EnumAdapterModes(0, 0, &g_Mode);

    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = g_Mode.Width;
    d3dpp.BackBufferHeight          = g_Mode.Height;
    d3dpp.BackBufferFormat          = g_Mode.Format;
    d3dpp.Flags                     = g_Mode.Flags;
    d3dpp.BackBufferCount           = 1;
    d3dpp.EnableAutoDepthStencil    = TRUE;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;

    // Create the device.
    D3DDevice_Reset(&d3dpp);

    g_EnableCC = TRUE;

    // Get the buffer to draw into.
    D3DDevice_GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &g_pBack);

    // Set up the overlay surface.
    D3DDevice_CreateImageSurface(256, 256, D3DFMT_YUY2, &g_pOverlay);

    D3DSurface_LockRect(g_pOverlay, &rect, 0, D3DLOCK_TILED);

    pBits = (DWORD *)rect.pBits;

    for (y = 0; y < 256; y++)
    {
        for (x = 0; x < 256; x += 2)
        {
            if (x & 0x10)
            {
                r = x & 0xF0;
                g = 0;
                b = y;
            }
            else
            {
                r = 255;
                g = 255;
                b = 255;
            }

            y0 = (UCHAR)(0.29 * r + 0.57 * g + 0.14 * b);
            u = (UCHAR)(128.0 - 0.14 * r - 0.29 * g + 0.43 * b);            
            v = (UCHAR)(128.0 + 0.36 * r - 0.29 * g - 0.07 * b);

            *pBits = v << 24 
                     | y0 << 16
                     | u << 8
                     | y0;

            pBits++;
        }

        pBits += 128;
    }

    D3DSurface_UnlockRect(g_pOverlay);

    // Create the screen.
    CreateScreen(0, g_Tests[0].m_DefaultValue1, g_Tests[0].m_DefaultValue2);

    // Set up the fps counter.
    D3DDevice_BlockUntilVerticalBlank();

    g_FpsTime = GetTickCount();
    g_VpsTime = g_FpsTime;

    g_FpsCount = 0;
    g_VpsCount = 0;

    D3DDevice_SetVerticalBlankCallback(FrameCounter); 

    // Set up the command processor.
    DmRegisterCommandProcessor(COMMAND_PREFIX, RXCmdProcessorProc);
}

//------------------------------------------------------------------------------
// Use the cache to attenuate some bits.
//
void
Attenuate(
    BYTE *pBits,
    DWORD Pitch,
    DWORD Width,
    DWORD Start,
    DWORD Size
    )
{   
    DWORD X, Y, Line, Free, Pixel, Color, NewPixel;
    DWORD *pLine;

    if (Size)
    {
         Start++;
    }

    // Flush everything out of the cache that we're not going to keep.
    for (Line = 0; Line < BUFFER_SIZE; Line++)
    {
        if (g_LineNumber[Line] && (g_LineNumber[Line] < Start || g_LineNumber[Line] >= Start + Size))
        {
            memcpy(pBits + Pitch * (g_LineNumber[Line] - 1), g_LineBuffer[Line], Width * 4);
            g_LineNumber[Line] = 0;
        }
    }

    // Suck up anything we need.  This is as about inefficient as you can get.

    for (Y = Start; Y < Start + Size; Y++)
    {
        Free = -1;

        for (Line = 0; Line < Size; Line++)
        {
            if (g_LineNumber[Line] == Y)
            {
                break;
            }

            if (g_LineNumber[Line] == 0 && Free == -1)
            {
                Free = Line;
            }
        }

        // Not already there.  Add it.
        if (Line == Size)
        {
            if (Free == -1)
            {
                _asm int 3;
            }

            pLine = (DWORD *)(pBits + Pitch * (Y - 1));

            // Save the old.
            memcpy(g_LineBuffer[Free], pLine, Width * 4);
            g_LineNumber[Free] = Y;

            // Attenuate it.
            for (X = 0; X < Width; X++)
            {
                Pixel = *(pLine + X);

                NewPixel = 0;

                if (g_ShowSweep & 0x01)
                {
                    Color = (Pixel >> 0) & 0xFF;

                    if (Color >= 0x80)
                    {
                        Color -= (DWORD)(0xFF * g_SweepAttenuation);
                    }
                    else
                    {
                        Color += (DWORD)(0xFF * g_SweepAttenuation);
                    }

                    NewPixel |= Color << 0;
                }
                else
                {
                    NewPixel |= Pixel & 0x0000FF;
                }

                if (g_ShowSweep & 0x02)
                {
                    Color = (Pixel >> 8) & 0xFF;

                    if (Color >= 0x80)
                    {
                        Color -= (DWORD)(0xFF * g_SweepAttenuation);
                    }
                    else
                    {
                        Color += (DWORD)(0xFF * g_SweepAttenuation);
                    }

                    NewPixel |= Color << 8;
                }
                else
                {
                    NewPixel |= Pixel & 0x00FF00;
                }

                if (g_ShowSweep & 0x04)
                {
                    Color = (Pixel >> 16) & 0xFF;

                    if (Color >= 0x80)
                    {
                        Color -= (DWORD)(0xFF * g_SweepAttenuation);
                    }
                    else
                    {
                        Color += (DWORD)(0xFF * g_SweepAttenuation);
                    }

                    NewPixel |= Color << 16;
                }
                else
                {
                    NewPixel |= Pixel & 0xFF0000;
                }

                *(pLine + X) = NewPixel;
            }
        }
    }
                
}

//------------------------------------------------------------------------------
// Draw one frame.
//
void
Paint()
{    
    D3DSurface *pSurface;
    D3DFIELD_STATUS FieldStatus;
    
    DWORD Now, Elapsed;
    DWORD Start, Size, X, Y, Range;

    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT Rect;

    DWORD Pitch;
    DWORD Width;

    DWORD *pData;

    D3DDevice_GetDisplayFieldStatus(&FieldStatus);
    
    if (FieldStatus.Field == D3DFIELD_ODD)
    {
        pSurface = g_ScreenOdd;
    }
    else
    {
        pSurface = g_ScreenEven;
    }

    if (g_ScreenOdd == g_ScreenEven)
    {
        D3DSurface_GetDesc(pSurface, &Desc);
        D3DSurface_LockRect(pSurface, &Rect, NULL, D3DLOCK_TILED);

        Pitch = Rect.Pitch;
        Width = Desc.Width;

        // Draw the sweep.
        if (g_ShowSweep)
        {
            //_asm int 3;

            Now = GetTickCount();

            if (!g_SweepTime)
            {
                g_SweepTime = Now;
            }

            // Where are we in a sweep?
            Elapsed = (Now - g_SweepTime) % (DWORD)(g_SweepRate * 1000);
            Range = Desc.Height - g_SweepWidth;

            Start = (DWORD)labs((long)(Range * 2 * Elapsed / (g_SweepRate * 1000)) - (long)Range);
            Size = g_SweepWidth;
        }
        else
        {
            Start = 0;
            Size = 0;
        }

        Attenuate((BYTE *)Rect.pBits, Pitch, Width, Start, Size);

        D3DSurface_UnlockRect(g_pBack);
    }

    // Copy the rectangle.
    D3DDevice_CopyRects(pSurface, NULL, 0, g_pBack, NULL);

    // Draw the mode.
    if (g_ModeTime + 2000 > GetTickCount())
    {
        DrawMode();
    }

    // Show it.
    D3DDevice_Swap(0);
}

//------------------------------------------------------------------------------
// Handle controller input.
//
void ProcessInput()
{
    XINPUT_STATE InputState;
    DWORD A, B, X, Y, Black, White;
    D3DPRESENT_PARAMETERS d3dpp;
    USHORT Data;

    if (!(XGetDevices(XDEVICE_TYPE_GAMEPAD) & XDEVICE_PORT0_MASK))        
    {
        if (g_InputHandle)
        {
            XInputClose(g_InputHandle);
            g_InputHandle = NULL;
        }

        return;
    }

    if (!g_InputHandle)
    {
        g_InputHandle = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, 0, NULL);
    }

    // Query the input state
    if (XInputGetState(g_InputHandle, &InputState) != ERROR_SUCCESS)
    {
        return;
    }

    A     = InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] & 0x80;
    B     = InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] & 0x80;
    X     = InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] & 0x80;
    Y     = InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] & 0x80;
    Black = InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] & 0x80;
    White = InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] & 0x80;

    if (A)
    {
        if (!g_A)
        {
            g_TestPattern = (g_TestPattern + 1) % (sizeof(g_Tests) / sizeof(TESTS));

            CreateScreen(g_TestPattern, g_Tests[g_TestPattern].m_DefaultValue1, g_Tests[g_TestPattern].m_DefaultValue2);
        }
    }

    g_A = A;

    if (B)
    {
        if (!g_B)
        {
            g_iMode = (g_iMode + 1) % (Direct3D_GetAdapterModeCount(0) / 4);

            Direct3D_EnumAdapterModes(0, g_iMode * 4, &g_Mode);

            ZeroMemory(&d3dpp, sizeof(d3dpp));

            d3dpp.BackBufferWidth            = g_Mode.Width;
            d3dpp.BackBufferHeight           = g_Mode.Height;
            d3dpp.BackBufferFormat           = g_Mode.Format;
            d3dpp.Flags                      = g_Mode.Flags;
            d3dpp.BackBufferCount            = 1;
            d3dpp.EnableAutoDepthStencil     = TRUE;
            d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
            d3dpp.FullScreen_RefreshRateInHz = g_Mode.RefreshRate;

            // Clear out any cached modes.
            //AvSendTVEncoderOption(NULL, AV_OPTION_ZERO_MODE, 0, NULL);

            D3DDevice_Reset(&d3dpp);

            // Recreate the surfaces.
            CreateScreen(g_TestPattern, g_Tests[g_TestPattern].m_DefaultValue1, g_Tests[g_TestPattern].m_DefaultValue2);

            g_EnableCC = TRUE;

            //AvSendTVEncoderOption(NULL, AV_OPTION_ENABLE_CC, 0, NULL);

            g_ModeTime = GetTickCount();

            g_FpsCount = 0;
            g_VpsCount = 0;
            g_FpsTime = GetTickCount();
            g_VpsTime = g_FpsTime;
        }
    }

    g_B = B;

    if (X)
    {
        if (!g_X)
        {
            if (!g_ShowSweep)
            {
                g_ShowSweep = 0x7;
                g_SweepTime = 0;
            }
            else if (g_ShowSweep == 0x07)
            {
                g_ShowSweep = 0x4;
            }
            else if (g_ShowSweep == 0x04)
            {
                g_ShowSweep = 0x2;
            }
            else if (g_ShowSweep == 0x02)
            {
                g_ShowSweep = 0x1;
            }
            else
            {
                g_ShowSweep = 0;
            }
        }
    }

    g_X = X;

    if (Y)
    {
        if (!g_Y)
        {
            if (D3D__AvInfo & AV_HDTV_MODE_MASK)
            {
                D3D__AvInfo = D3D__AvInfo & ~AV_HDTV_MODE_MASK | g_SavedAvInfo & AV_HDTV_MODE_MASK;
            }
            else
            {
                D3D__AvInfo |= AV_HDTV_MODE_MASK;
            }
        }
    }

    g_Y = Y;

    if (Black)
    {
        if (!g_Black)
        {
#ifdef FOCUS

            AvpDelay(REGISTER_BASE, 50 * 1000 * 1000);

            Data = SMB_RDW(REGISTER_BASE, 0x0C);
            SMB_WRW(REGISTER_BASE, 0x0C, Data | 0x8000);

            AvpDelay(REGISTER_BASE, 50 * 1000 * 1000);

            Data = SMB_RDW(REGISTER_BASE, 0x0C);
            SMB_WRW(REGISTER_BASE, 0x0C, Data & ~0x8000);

            AvpDelay(REGISTER_BASE, 50 * 1000 * 1000);

            Data = SMB_RDW(REGISTER_BASE, 0x0E);
            SMB_WRW(REGISTER_BASE, 0x0E, Data & ~0x0400);

            AvpDelay(REGISTER_BASE, 50 * 1000 * 1000);

            Data = SMB_RDW(REGISTER_BASE, 0x0E);
            SMB_WRW(REGISTER_BASE, 0x0E, Data | 0x0400);

#endif
        }
    }

    g_Black = Black;

    if (White)
    {
        if (!g_White)
        {
        }
    }

    g_White = White;
}

//==============================================================================
// Main goo.
//==============================================================================

//------------------------------------------------------------------------------
// Main entrypoint.
//
void __cdecl main()
{
    RECT SourceRect, DestRect;
    D3DSURFACE_DESC Desc;

    Init();

    for(;;)
    {
        ProcessInput();

        Paint();

        if (g_EnableCC)
        {
            D3DDevice_EnableCC(TRUE);
            g_EnableCC = FALSE;
        }

        g_FpsCount++;

        if (g_Overlay)
        {
            D3DSurface_GetDesc(g_pBack, &Desc);

            SourceRect.left = 0;
            SourceRect.top  = 0;
            SourceRect.right = 256;
            SourceRect.bottom = 256;

#if 0
            DestRect.left   = Desc.Width / 2 - 160;
            DestRect.right  = Desc.Width / 2 + 160;
            DestRect.top    = Desc.Height / 2 - 160;
            DestRect.bottom = Desc.Height / 2 + 160;
#else
            DestRect.left   = 0;
            DestRect.right  = Desc.Width;
            DestRect.top    = 0;
            DestRect.bottom = Desc.Height / 2;
#endif
            while(!D3DDevice_GetOverlayUpdateStatus())
                ;

            D3DDevice_UpdateOverlay(g_pOverlay, &SourceRect, &DestRect, FALSE, 0x00000000);
        }
        else
        {
            D3DDevice_EnableOverlay(FALSE);
        }

        D3DDevice_BlockUntilVerticalBlank();

        //WaitForSingleObject(g_hEvent, INFINITE);
    }
}

