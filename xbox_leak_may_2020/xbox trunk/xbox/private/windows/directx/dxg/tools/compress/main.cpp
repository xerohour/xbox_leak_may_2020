#include <windows.h>
#include <stdio.h>

DWORD g_cWidth;
DWORD g_cHeight;

#define TEXT_COLOR RGB(0x00, 0xFF, 0x00)
#define BKG_COLOR RGB(0x00, 0x00, 0x00)

#pragma pack(1)

typedef struct _BYTES1 {
    BYTE b1;
} BYTES1;

typedef struct _IMAGE_RUN1 {
    UINT fOne : 1;
    UINT Size : 3;
    UINT Intensity : 4;
} IMAGE_RUN1;

typedef union _RUN1 {
    BYTES1 bytes;
    IMAGE_RUN1 run;
} RUN1;

typedef struct _BYTES2 {
    BYTE b1;
    BYTE b2;
} BYTES2;

typedef struct _IMAGE_RUN2 {
    UINT fOne : 1;
    UINT fTwo : 1;
    UINT Size : 10;
    UINT Intensity : 4;
} IMAGE_RUN2;

typedef union _RUN2 {
    BYTES2 bytes;
    IMAGE_RUN2 run;
} RUN2;

typedef struct _BYTES3 {
    BYTE b1;
    BYTE b2;
    BYTE b3;
} BYTES3;

typedef struct _IMAGE_RUN3 {
    UINT fOne : 1;
    UINT fTwo : 1;
    UINT Size : 18;
    UINT Intensity : 4;
} IMAGE_RUN3;

typedef union _RUN3 {
    BYTES3 bytes;
    IMAGE_RUN3 run;
} RUN3;

typedef struct _IMAGE_RUN {
    UINT fOne;
    UINT fTwo;
    UINT Size;
    UINT Intensity;
} IMAGE_RUN;

#pragma pack()

IMAGE_RUN *g_pImage;

UINT IntensityFromColor(COLORREF cr)
{
    UINT r = GetRValue(cr);
    UINT g = GetGValue(cr);
    UINT b = GetBValue(cr);

    // Has the range 0 through 255
    UINT intensity8bits = max(max(r, g), b);

    // Has the range 0.0 through 1.0
    FLOAT intensityFloat = intensity8bits / 255.0f;

    // Rounded, in the range 0 through 15
    UINT intensity4bits = (UINT)(intensityFloat * 15.0f + 0.5f);

    return intensity4bits;
}

COLORREF ColorFromIntensity(UINT intensity4bits)
{
    UINT BaseR = GetRValue(TEXT_COLOR);
    UINT BaseG = GetGValue(TEXT_COLOR);
    UINT BaseB = GetBValue(TEXT_COLOR);

    UINT r = (UINT)((BaseR * intensity4bits) / 15.0f + 0.5f);
    UINT g = (UINT)((BaseG * intensity4bits) / 15.0f + 0.5f);
    UINT b = (UINT)((BaseB * intensity4bits) / 15.0f + 0.5f);

    return RGB(r, g, b);
}

void EncodeRun(IMAGE_RUN *pRun, COLORREF cr, UINT Size)
{
    if (Size <= 0x7) {
        pRun->fOne = 1;
        pRun->fTwo = 0;
    } else if (Size <= 0x3FF) {
        pRun->fOne = 0;
        pRun->fTwo = 1;
    } else {
        pRun->fOne = 0;
        pRun->fTwo = 0;
    }

    pRun->Size = Size;

    if (cr == BKG_COLOR) {
        pRun->Intensity = 0;
    } else {
        pRun->Intensity = IntensityFromColor(cr);
    }
}

int Encode(LPSTR psz)
{
    BITMAP bm;
    DWORD i, j, total, c1, c2, c3, cRuns, CurrentRun, LongestRun, cOut;
    COLORREF cr, crPrev;
    HDC hdc, hdcScreen;
    HBITMAP hbm, hbmOld;
    IMAGE_RUN *pRun;
    RUN1 Run1;
    RUN2 Run2;
    RUN3 Run3;

    hbm = (HBITMAP)LoadImage(NULL,
            psz,
            IMAGE_BITMAP,
            0,
            0,
            LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if (hbm == NULL) {
        return 0;
    }

    GetObject(hbm, sizeof(BITMAP), &bm);

    g_cWidth = bm.bmWidth;
    g_cHeight = bm.bmHeight;

    hdcScreen = GetDC(NULL);
    hdc = CreateCompatibleDC(hdcScreen);
    hbmOld = (HBITMAP)SelectObject(hdc, hbm);

    // First pass - firgure out the number of runs.

    cRuns = 0;
    LongestRun = 0;
    CurrentRun = 0;

    for (i = 0; i < (DWORD)bm.bmHeight; i++) {
        for (j = 0; j < (DWORD)bm.bmWidth; j++) {

            cr = GetPixel(hdc, j, i);

            if (i == 0 && j == 0) {
                crPrev = cr;
            }

            if (cr == crPrev) {
                CurrentRun++;
            } else {

                if (CurrentRun > LongestRun) {
                    LongestRun = CurrentRun;
                }

                CurrentRun = 1;
                cRuns++;
                crPrev = cr;
            }
        }
    }

    // Count the last run.

    cRuns++;
    if (CurrentRun > LongestRun) {
        LongestRun = CurrentRun;
    }

    // Make sure we don't overflow.

    if (LongestRun > 0x7FFFF) {
        __asm int 3;
    }

    // Allocate the memory for all of the runs we counted.

    g_pImage = (IMAGE_RUN*)LocalAlloc(LMEM_FIXED, sizeof(IMAGE_RUN) * cRuns);

    // Second pass - actually encode the data.

    cRuns = 0;
    CurrentRun = 0;

    for (i = 0; i < (DWORD)bm.bmHeight; i++) {
        for (j = 0; j < (DWORD)bm.bmWidth; j++) {

            cr = GetPixel(hdc, j, i);

            if (i == 0 && j == 0) {
                crPrev = cr;
            }

            if (cr == crPrev) {
                CurrentRun++;
            } else {

                EncodeRun(&g_pImage[cRuns], crPrev, CurrentRun);

                CurrentRun = 1;
                cRuns++;
                crPrev = cr;
            }
        }
    }

    EncodeRun(&g_pImage[cRuns], crPrev, CurrentRun);
    cRuns++;

    ReleaseDC(NULL, hdcScreen);
    SelectObject(hdc, hbmOld);
    DeleteObject(hdc);

    cOut = 0;
    FILE *fp = fopen("image.h", "w");
    for (i = 0; i < cRuns; i++) {
        pRun = &g_pImage[i];

        if (pRun->fOne) {
            Run1.run.fOne = pRun->fOne;
            Run1.run.Size = pRun->Size;
            Run1.run.Intensity = pRun->Intensity;

            fprintf(fp, "0x%02X, ", Run1.bytes.b1);
            cOut++;
        } else if (pRun->fTwo) {
            Run2.run.fOne = pRun->fOne;
            Run2.run.fTwo = pRun->fTwo;
            Run2.run.Size = pRun->Size;
            Run2.run.Intensity = pRun->Intensity;

            fprintf(fp, "0x%02X, ", Run2.bytes.b1);
            cOut++;

            if ((cOut % 12) == 0) {
                fprintf(fp, "\n");
            }

            fprintf(fp, "0x%02X, ", Run2.bytes.b2);
            cOut++;
        } else {
            Run3.run.fOne = pRun->fOne;
            Run3.run.fTwo = pRun->fTwo;
            Run3.run.Size = pRun->Size;
            Run3.run.Intensity = pRun->Intensity;

            fprintf(fp, "0x%02X, ", Run3.bytes.b1);
            cOut++;

            if ((cOut % 12) == 0) {
                fprintf(fp, "\n");
            }

            fprintf(fp, "0x%02X, ", Run3.bytes.b2);
            cOut++;

            if ((cOut % 12) == 0) {
                fprintf(fp, "\n");
            }

            fprintf(fp, "0x%02X, ", Run3.bytes.b3);
            cOut++;
        }
        if ((cOut % 12) == 0) {
            fprintf(fp, "\n");
        }
    }
    fclose(fp);

    printf("\nBitmap size is %d x %d\n", g_cWidth, g_cHeight);
    printf("Compressed data written to 'image.h'\n\n");

    // Check the image integrity.

    total = 0;
    c1 = 0;
    c2 = 0;
    c3 = 0;

    for (i = 0; i < cRuns; i++) {
        pRun = &g_pImage[i];

        if (pRun->fOne) {
            c1++;
        } else if (pRun->fTwo) {
            c2++;
        } else {
            c3++;
        }

        total += pRun->Size;
    }
    if (total != (DWORD)(bm.bmWidth * bm.bmHeight)) {
        __asm int 3;
    }

    printf("%d bytes, %d runs, %d 1bytes, %d 2bytes, %d 3bytes\n\n", cOut, cRuns, c1, c2, c3);

    return cRuns;
}

HBITMAP Decode(const IMAGE_RUN *pImage, DWORD cRuns)
{
    HDC hdcScreen, hdc;
    HBITMAP hbm, hbmOld;
    DWORD i = 0, j = 0, k, iRun;
    const IMAGE_RUN *pRun;

    hdcScreen = GetDC(NULL);
    hdc = CreateCompatibleDC(hdcScreen);
    hbm = CreateCompatibleBitmap(hdcScreen, g_cWidth, g_cHeight);

    hbmOld = (HBITMAP)SelectObject(hdc, hbm);
    
    for (iRun = 0; iRun < cRuns; iRun++) {

        pRun = &pImage[iRun];

        for (k = 0; k < pRun->Size; k++) {

            if (i >= g_cWidth) {
                i = 0;
                j++;

                if (j >= g_cHeight) {
                    __asm int 3;
                }
            }

            if (pRun->Intensity == 0) {
                SetPixel(hdc, i, j, BKG_COLOR);
            } else {
                SetPixel(hdc, i, j, ColorFromIntensity(pRun->Intensity));
            }

            i++;
        }
    }

    ReleaseDC(NULL, hdcScreen);
    SelectObject(hdc, hbmOld);
    DeleteObject(hdc);

    return hbm;
}

HBITMAP g_hbm;

void PaintBitmap(HDC hdc)
{
    HDC hdcMem = CreateCompatibleDC(hdc);
    SelectObject(hdcMem, g_hbm);
    BitBlt(hdc, 0, 0, g_cWidth, g_cHeight, hdcMem, 0, 0, SRCCOPY);
    DeleteObject(hdcMem);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg) {
    case WM_ERASEBKGND:
        PaintBitmap((HDC)wParam);
        return 1;

    case WM_CLOSE:
        PostMessage(NULL, WM_QUIT, 0, 0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int __cdecl main(int argc, char **argv)
{
    if (argc < 2) {
        return 0;
    }

    printf("Encoding...\n");
    int cRuns = Encode(argv[1]);

    printf("Decoding...\n");
    g_hbm = Decode(g_pImage, cRuns);

    printf("Displaying...\n");
    
    RECT rc;
    HWND hwnd;
    WNDCLASS wc;
    MSG msg;
    HINSTANCE hinst = GetModuleHandle(NULL);
    LPSTR psz = argv[1];

    ZeroMemory(&wc, sizeof(wc));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = psz;
    wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    rc.left = rc.top = 0;
    rc.right = g_cWidth;
    rc.bottom = g_cHeight;

    AdjustWindowRect(&rc, WS_POPUP | WS_CAPTION | WS_SYSMENU, FALSE);

    hwnd = CreateWindowEx(0, psz, psz, WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
            20, 20, rc.right - rc.left, rc.bottom - rc.top, 0, 0, hinst, 0);

    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}
