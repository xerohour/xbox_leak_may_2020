/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xbpscapture.cpp
 *  Content:    Pixel Shader Debugger
 *
 ***************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include "d3d8-xbox.h"

/*
**
** xbpssnapshot
**
** Captures a pixel shader snapshot
**
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include "resource.h"
#include <xboxverp.h>
#include "xbfile.hxx"

#define SHADER_CAPTURE_BUFFER_SIZE 32768

// Identifiers for data blocks in the capture buffer
#define PSS_EOF         0x00000000
#define PSS_PIXEL       0x01000000
#define PSS_FINAL       0x02000000
#define PSS_PRIMITIVE   0x03000000
#define PSS_STAGE       0x04000000
#define PSS_FINALSTAGE  0x05000000
#define PSS_IMAGE       0x06000000
#define PSS_NOSHADER    0x07000000
#define PSS_STAGE0      0x08000000

DWORD g_dwFlags = 0;
DWORD g_dwMarker;

char g_HTMLFilename[MAX_PATH];
char g_BMPFilename[MAX_PATH];
char g_BMP2Filename[MAX_PATH];
char g_BMP3Filename[MAX_PATH];
char g_TmpPath[MAX_PATH];

BYTE buf[SHADER_CAPTURE_BUFFER_SIZE];

DWORD UsedInputRegisters;
DWORD UsedOutputRegisters;

void GenerateHTML(BYTE *buf, FILE *fp);

BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    SetDlgItemText(hwnd, IDC_StatusText, "Initializing...");
    GetTempPath(1024, g_TmpPath);
    SetDlgItemInt(hwnd, IDC_EDIT1, 320, FALSE);
    SetDlgItemInt(hwnd, IDC_EDIT2, 240, FALSE);
    SetDlgItemText(hwnd, IDC_EDIT3, g_TmpPath);
    CheckDlgButton(hwnd, IDC_CHECK1, FALSE); 
    SetDlgItemInt(hwnd, IDC_EDIT4, 0, FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_EDIT4), FALSE);
    SetDlgItemText(hwnd, IDC_StatusText, "Ready to Capture");
    return TRUE;
}

DWORD WINAPI ThreadProc(LPVOID pParam)
{
    char cmd[1024];
    sprintf(cmd, "\"%s\"", pParam);
    system(cmd);
    return TRUE;
}

DWORD *FindOpcode(DWORD op, BYTE *buf)
{
    DWORD *dwbuf = (DWORD*)buf;
    while((*dwbuf & 0xff000000) != PSS_EOF)
    {
        if((*dwbuf & 0xff000000) == op)
            return dwbuf;   // found
        dwbuf += *dwbuf & 0x00ffffff;   // move to next opcode
        if(((DWORD)dwbuf - (DWORD)buf) > SHADER_CAPTURE_BUFFER_SIZE)
            return NULL;    // not found and no EOF
    }
    return NULL;    // not found
}

void WriteOutputFiles(BYTE *buf)
{
    FILE *fp;
    BYTE *bptr;
    HRESULT hr;
    char errorbuf[MAX_PATH];
    
    // generate filenames
    
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    char time[1024];
    sprintf(time, "%02d-%02d-%02d-%02d-%02d", 
            sysTime.wMonth, sysTime.wDay, sysTime.wHour, 
            sysTime.wMinute, sysTime.wSecond);
    sprintf(g_HTMLFilename, "%sxbpscpta%s.htm", g_TmpPath, time);
    sprintf(g_BMPFilename, "%sxbpscptb%s.bmp", g_TmpPath, time);
    sprintf(g_BMP2Filename, "%sxbpscptc%s.bmp", g_TmpPath, time);
    sprintf(g_BMP3Filename, "%sxbpscptd%s.bmp", g_TmpPath, time);
    
    DWORD *ptr = FindOpcode(PSS_IMAGE, buf);
    if(ptr)
    {
        // write out the 64x64 vicinity image
        fp = fopen(g_BMPFilename, "wb");
        if(!fp)
        {
            sprintf(errorbuf, "Unable to open file %s.", g_BMPFilename);
            MessageBox(NULL, (const char*)errorbuf, "File Write Error", MB_OK | MB_TASKMODAL);
            return;
        }
        DWORD dwLen = (*ptr & 0xffffff) - 1;
        DWORD dwX, dwY;
        ptr++;
        dwX = *ptr & 0xffff;
        dwY = (*ptr >> 16) & 0xffff;
        
        // write the .bmp header
        BITMAPFILEHEADER bmfh;
        BITMAPINFOHEADER bmih;

        ZeroMemory( &bmfh, sizeof(BITMAPFILEHEADER) );
        bmfh.bfType = 'MB';
        bmfh.bfSize = (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwX*3 * dwY);
        bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), fp);
        
        ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
        bmih.biSize = sizeof(BITMAPINFOHEADER);
        bmih.biWidth = dwX;
        bmih.biHeight = dwY;
        bmih.biPlanes = 1;
        bmih.biBitCount = 24;
        fwrite(&bmih, 1, sizeof(BITMAPINFOHEADER), fp);
        
        ptr++;
        
        // draw an xor box around the center area
        for(dwX=29; dwX <= 35; dwX++)
        {
            for(dwY=29; dwY <= 35; dwY++)
            {
                if((dwX == 29) || (dwX == 35) || (dwY == 29) || (dwY == 35))
                {
                    bptr = (BYTE *)ptr + (dwX + dwY*64)*3;
                    *bptr     ^= 0xff;
                    *(bptr+1) ^= 0xff;
                    *(bptr+2) ^= 0xff;
                }
            }
        }
        // write out the bits
        fwrite(ptr, sizeof(DWORD), dwLen, fp);

        fclose(fp);
    
        // write out the 5x5 near vicinity image
        // (assumes that ptr points to start of image bits)
        struct
        {
            BYTE b,g,r;
        } img[128][128];
        INT i,j;
        
        // clear the image
        for(i=0; i<128; i++)
            for(j=0; j<128; j++)
                img[j][i].b = img[j][i].g = img[j][i].r = 0;

        // draw a white border around the center pixel
        for(i=51; i<=76; i++)
        {
            for(j=51; j<=76; j++)
            {
                img[j][i].b = 0xff;
                img[j][i].g = 0xff;
                img[j][i].r = 0xff;
            }
        }
        
        // draw very large pixels
        INT pi, pj;
        for(pj = 34; pj > 29; pj--)
        {
            for(pi = 30; pi < 35; pi++)
            {
                bptr = (BYTE *)ptr + (pj*64 + pi)*3;
                
                INT sx = (pi-29)*25-24+1;
                INT sy = (pj-29)*25-24+1;

                for(i=sx; i<(sx+24); i++)
                {
                    for(j=sy; j<(sy+24); j++)
                    {
                        img[j][i].b = *bptr;
                        img[j][i].g = *(bptr+1);
                        img[j][i].r = *(bptr+2);
                    }
                }
            }
        }
        fp = fopen(g_BMP2Filename, "wb");
        if(!fp)
        {
            sprintf(errorbuf, "Unable to open file %s.", g_BMP2Filename);
            MessageBox(NULL, (const char*)errorbuf, "File Write Error", MB_OK | MB_TASKMODAL);
            return;
        }
        // write the .bmp header
        ZeroMemory( &bmfh, sizeof(BITMAPFILEHEADER) );
        bmfh.bfType = 'MB';
        bmfh.bfSize = (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 128*128*3);
        bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), fp);
        
        ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
        bmih.biSize = sizeof(BITMAPINFOHEADER);
        bmih.biWidth = 128;
        bmih.biHeight = 128;
        bmih.biPlanes = 1;
        bmih.biBitCount = 24;
        fwrite(&bmih, 1, sizeof(BITMAPINFOHEADER), fp);
        
        // write out the bits
        fwrite(img, sizeof(BYTE), 128*128*3, fp);

        fclose(fp);
    }

    // get the screen shot
    hr = DmScreenShot(g_BMP3Filename);  // don't report an error if it occurs


    fp = fopen(g_HTMLFilename, "w");
    if(!fp)
    {
        sprintf(errorbuf, "Unable to open file %s.", g_HTMLFilename);
        MessageBox(NULL, (const char*)errorbuf, "File Write Error", MB_OK | MB_TASKMODAL);
        return;
    }
    GenerateHTML(buf, fp);                                          // generate the html
    fclose(fp);
    CreateThread(NULL, 0, &ThreadProc, g_HTMLFilename, 0, NULL);    // display the html
}

void DeleteFiles(char *cPattern)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    char Filename[MAX_PATH];

    sprintf(Filename, "%s%s", g_TmpPath, cPattern);
    hFind = FindFirstFile(Filename, &FindFileData);
    while(hFind != INVALID_HANDLE_VALUE)
    {
        sprintf(Filename, "%s%s", g_TmpPath, FindFileData.cFileName);
        DeleteFile(Filename);
        BOOL bSuccess = FindNextFile(hFind, &FindFileData);
        if(!bSuccess)
        {
            FindClose(hFind);
            break;
        }
    }
}

void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch(id)
    {
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    case IDOK:
        HRESULT hr;
        SetDlgItemText(hwnd, IDC_StatusText, "Capturing one frame...");
        
        g_dwMarker = GetDlgItemInt(hwnd, IDC_EDIT4, NULL, FALSE);
        hr = DmPixelShaderSnapshot(GetDlgItemInt(hwnd, IDC_EDIT1, NULL, FALSE), 
                                   GetDlgItemInt(hwnd, IDC_EDIT2, NULL, FALSE),
                                    g_dwFlags,
                                    g_dwMarker,
                                    buf);
        if(FAILED(hr)) 
        {
            if(hr == XBDM_FILEERROR)
                hr = HrLastError();
            MessageBox(NULL, (const char*)buf, "xbpscapture Error", MB_OK | MB_TASKMODAL);
        }
        else
        {
            SetDlgItemText(hwnd, IDC_StatusText, "Writing HTML Output");
            GetDlgItemText(hwnd, IDC_EDIT3, g_TmpPath, MAX_PATH);
            WriteOutputFiles(buf);
        }
        SetDlgItemText(hwnd, IDC_StatusText, "Ready to Capture");
        
        break;

    case IDC_DeleteTempButton:
        
        SetDlgItemText(hwnd, IDC_StatusText, "Deleting xbpscpta*.htm");
        DeleteFiles("xbpscpta*.htm");
        SetDlgItemText(hwnd, IDC_StatusText, "Deleting xbpscptb*.bmp");
        DeleteFiles("xbpscptb*.bmp");
        SetDlgItemText(hwnd, IDC_StatusText, "Deleting xbpscptc*.bmp");
        DeleteFiles("xbpscptc*.bmp");
        SetDlgItemText(hwnd, IDC_StatusText, "Deleting xbpscptd*.bmp");
        DeleteFiles("xbpscptd*.bmp");
        SetDlgItemText(hwnd, IDC_StatusText, "Ready to Capture");
        break;

    case IDC_CHECK1:
        if(IsDlgButtonChecked(hwnd, IDC_CHECK1) == BST_CHECKED)
        {
            // enable marker window
            EnableWindow(GetDlgItem(hwnd, IDC_EDIT4), TRUE);
            g_dwFlags |= 0x1;  // set marker flag
        }
        else
        {
            EnableWindow(GetDlgItem(hwnd, IDC_EDIT4), FALSE);
            g_dwFlags &= ~0x1;  // reset marker flag
        }
        break;
    }
}

BOOL CALLBACK Dlg_Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    HANDLE_MSG(hDlg, WM_COMMAND, Dlg_OnCommand);
    HANDLE_MSG(hDlg, WM_INITDIALOG, Dlg_OnInitDialog);
    case WM_HELP: printf("Help!\n"); break;
    default:
        return FALSE;
    }
    return TRUE;
}

int __cdecl main(int cArg, char **rgszArg)
{
    // if command line parameters, run silently
    if(cArg > 1)
    {
        DWORD dwX, dwY;
        HRESULT hr;
        
        dwX = atoi(rgszArg[1]);
        dwY = atoi(rgszArg[2]);
        hr = DmPixelShaderSnapshot(dwX, dwY, 0, 0, buf);
        
        if(FAILED(hr)) 
        {
            if(hr == XBDM_FILEERROR)
                hr = HrLastError();
            MessageBox(NULL, (const char*)buf, "xbpscapture Error", MB_OK | MB_TASKMODAL);
        }
        else
        {
            WriteOutputFiles(buf);
        }
        Sleep(5000);
        return 0;
    }
    
    // display the UI
    DialogBox(NULL, MAKEINTRESOURCE(IDD_PSCAPTURE), NULL, Dlg_Proc);
    return 0;
}

DWORD smearalpha(DWORD in)
{
    return (((in >>  0) & 0xff000000) |
            ((in >>  8) & 0x00ff0000) |
            ((in >> 16) & 0x0000ff00) |
            ((in >> 24) & 0x000000ff));
}

struct inputRegister
{
    char name[32];
    BOOL isZero;
    BOOL isOne;
    BOOL isMinusOne;
}inRGB[7], inA[4];

void getInputReg(struct inputRegister *pIR, BYTE val, BOOL isRGB)
{
    BYTE reg, im, chan;
    char *pName;
    char *pSuffix;

    im = val & 0xe0;
    chan = (val >> 4) & 0x1;
    reg = (val & 0xf);

    pIR->isZero = ((reg == PS_REGISTER_ZERO) &&
                   ((im == PS_INPUTMAPPING_UNSIGNED_IDENTITY) ||
                    (im == PS_INPUTMAPPING_SIGNED_IDENTITY)));
    pIR->isOne = ((reg == PS_REGISTER_ZERO) &&
                  ((im == PS_INPUTMAPPING_UNSIGNED_INVERT) ||
                   (im == PS_INPUTMAPPING_EXPAND_NEGATE)));
    pIR->isMinusOne = ((reg == PS_REGISTER_ZERO) && (im == PS_INPUTMAPPING_EXPAND_NORMAL));
    
    switch(reg)
    {
    case PS_REGISTER_ZERO: 
        UsedInputRegisters |= (1 << PS_REGISTER_ZERO); 
        pName = "zero"; 
        break;
    case PS_REGISTER_C0:
        UsedInputRegisters |= (1 << PS_REGISTER_C0);
        pName = "c0"; 
        break;
    case PS_REGISTER_C1:
        UsedInputRegisters |= (1 << PS_REGISTER_C1);
        pName = "c1"; 
        break;
    case PS_REGISTER_FOG:
        UsedInputRegisters |= (1 << PS_REGISTER_FOG);
        pName = "fog"; 
        break;
    case PS_REGISTER_V0:
        UsedInputRegisters |= (1 << PS_REGISTER_V0);
        pName = "v0"; 
        break;
    case PS_REGISTER_V1:
        UsedInputRegisters |= (1 << PS_REGISTER_V1);
        pName = "v1"; 
        break;
    case PS_REGISTER_T0:
        UsedInputRegisters |= (1 << PS_REGISTER_T0);
        pName = "t0"; 
        break;
    case PS_REGISTER_T1:
        UsedInputRegisters |= (1 << PS_REGISTER_T1);
        pName = "t1"; 
        break;
    case PS_REGISTER_T2:
        UsedInputRegisters |= (1 << PS_REGISTER_T2);
        pName = "t2"; 
        break;
    case PS_REGISTER_T3:
        UsedInputRegisters |= (1 << PS_REGISTER_T3);
        pName = "t3"; 
        break;
    case PS_REGISTER_R0:
        UsedInputRegisters |= (1 << PS_REGISTER_R0);
        pName = "r0"; 
        break;
    case PS_REGISTER_R1:
        UsedInputRegisters |= (1 << PS_REGISTER_R1);
        pName = "r1"; 
        break;
    case PS_REGISTER_V1R0_SUM:
        UsedInputRegisters |= (1 << PS_REGISTER_V1R0_SUM);
        UsedInputRegisters |= (1 << PS_REGISTER_V1);
        UsedInputRegisters |= (1 << PS_REGISTER_R0);
        pName = "sum"; 
        break;
    case PS_REGISTER_EF_PROD:
        UsedInputRegisters |= (1 << PS_REGISTER_EF_PROD);
        pName = "prod"; 
        break;
    }

    if(isRGB)
    {
        if(chan == 1)
            pSuffix = ".a";
        else
            pSuffix = ".rgb";
    }
    else
    {
        if(chan == 1)
            pSuffix = ".a";
        else
            pSuffix = ".b";
    }

    switch(im)
    {
    case PS_INPUTMAPPING_UNSIGNED_IDENTITY:
        sprintf(pIR->name, "%s%s_sat", pName, pSuffix);
        break;
    case PS_INPUTMAPPING_UNSIGNED_INVERT:
        sprintf(pIR->name, "(1-%s%s)", pName, pSuffix);
        break;
    case PS_INPUTMAPPING_EXPAND_NORMAL:
        sprintf(pIR->name, "%s%s_bx2", pName, pSuffix);
        break;
    case PS_INPUTMAPPING_EXPAND_NEGATE:
        sprintf(pIR->name, "-%s%s_bx2)", pName, pSuffix);
        break;
    case PS_INPUTMAPPING_HALFBIAS_NORMAL:
        sprintf(pIR->name, "%s%s_bias", pName, pSuffix);
        break;
    case PS_INPUTMAPPING_HALFBIAS_NEGATE:
        sprintf(pIR->name, "-%s%s_bias", pName, pSuffix);
        break;
    case PS_INPUTMAPPING_SIGNED_IDENTITY:
        sprintf(pIR->name, "%s%s", pName, pSuffix);
        break;
    case PS_INPUTMAPPING_SIGNED_NEGATE:
        sprintf(pIR->name, "-%s%s", pName, pSuffix);
        break;
    }
}

char * getoutput(DWORD val)
{
    switch(val)
    {
    case PS_REGISTER_DISCARD:
        UsedOutputRegisters |= (1 << PS_REGISTER_DISCARD);
        return("discard");
    case PS_REGISTER_C0:
        UsedOutputRegisters |= (1 << PS_REGISTER_C0);
        return("c0");
    case PS_REGISTER_C1:
        UsedOutputRegisters |= (1 << PS_REGISTER_C1);
        return("c1");
    case PS_REGISTER_FOG:
        UsedOutputRegisters |= (1 << PS_REGISTER_FOG);
        return("fog");
    case PS_REGISTER_V0:
        UsedOutputRegisters |= (1 << PS_REGISTER_V0);
        return("v0");
    case PS_REGISTER_V1:
        UsedOutputRegisters |= (1 << PS_REGISTER_V1);
        return("v1");
    case PS_REGISTER_T0:
        UsedOutputRegisters |= (1 << PS_REGISTER_T0);
        return("t0");
    case PS_REGISTER_T1:
        UsedOutputRegisters |= (1 << PS_REGISTER_T1);
        return("t1");
    case PS_REGISTER_T2:
        UsedOutputRegisters |= (1 << PS_REGISTER_T2);
        return("t2");
    case PS_REGISTER_T3:
        UsedOutputRegisters |= (1 << PS_REGISTER_T3);
        return("t3");
    case PS_REGISTER_R0:
        UsedOutputRegisters |= (1 << PS_REGISTER_R0);
        return("r0");
    case PS_REGISTER_R1:
        UsedOutputRegisters |= (1 << PS_REGISTER_R1);
        return("r1");
    case PS_REGISTER_V1R0_SUM:
        UsedOutputRegisters |= (1 << PS_REGISTER_V1R0_SUM);
        return("sum");
    case PS_REGISTER_EF_PROD:
        UsedOutputRegisters |= (1 << PS_REGISTER_EF_PROD);
        return("prod");
    }
    return("");
}

void combineAndPrint(char *pRGB, char *pA, FILE *fp)
{
    char stripRGB[1024], stripA[1024], *pStr;

    // strip .rgb from first input string
    strcpy(stripRGB, pRGB);
    pStr = strstr(stripRGB, ".rgb");
    while(pStr)
    {
        strcpy(pStr, pStr+4);
        pStr = strstr(stripRGB, ".rgb");
    }
    
    // strip .a from second input string
    strcpy(stripA, pA);
    pStr = strstr(stripA, ".a");
    while(pStr)
    {
        strcpy(pStr, pStr+2);
        pStr = strstr(stripA, ".a");
    }

    // if the strings are the same, print only one of them
    if(strcmp(stripRGB, stripA) == 0)
    {
        pStr = strstr(stripRGB, "muxargument0");
        if(pStr)
        {
            UsedInputRegisters |= (1 << PS_REGISTER_R0);
            memcpy(pStr, "(r0.a > 0.5)", 12);
        }
        fprintf(fp, "<p class=Normal>%s</p>\n", stripRGB);
    }
    else
    {
        pStr = strstr(pRGB, "muxargument0");
        if(pStr)
        {
            UsedInputRegisters |= (1 << PS_REGISTER_R0);
            memcpy(pStr, "(r0.a > 0.5)", 12);
        }
        fprintf(fp, "<p class=Normal>%s</p>\n", pRGB);
        pStr = strstr(pA, "muxargument0");
        if(pStr)
        {
            UsedInputRegisters |= (1 << PS_REGISTER_R0);
            memcpy(pStr, "(r0.a > 0.5)", 12);
        }
        fprintf(fp, "<p class=Normal>%s</p>\n", pA);
    }
}

void GetOutputModifier(DWORD dwVal, char **pre, char **post)
{
    *pre = *post = "\0";
    switch((dwVal >> 12) & 0x38)
    {
    case PS_COMBINEROUTPUT_BIAS:
        *pre = "(";
        *post = ") - 0.5";
        break;
    case PS_COMBINEROUTPUT_SHIFTLEFT_1:
        *pre = "(";
        *post = ") * 2";
        break;
    case PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS:
        *pre = "((";
        *post = ") - 0.5) * 2";
        break;
    case PS_COMBINEROUTPUT_SHIFTLEFT_2:
        *pre = "(";
        *post = ") * 4";
        break;
    case PS_COMBINEROUTPUT_SHIFTRIGHT_1:
        *pre = "(";
        *post = ") / 2";
        break;
    }
}

void PrintCombinerDescription(FILE *fp, DWORD *pComb, BOOL bIsFinal)
{
    // initialize record of which registers are used
    UsedInputRegisters = 0;
    UsedOutputRegisters = 0;

    if(!bIsFinal)
    {
        getInputReg(&(inRGB[0]), (BYTE)((*pComb >> 24) & 0xff), TRUE);
        getInputReg(&(inRGB[1]), (BYTE)((*pComb >> 16) & 0xff), TRUE);
        getInputReg(&(inRGB[2]), (BYTE)((*pComb >>  8) & 0xff), TRUE);
        getInputReg(&(inRGB[3]), (BYTE)((*pComb >>  0) & 0xff), TRUE);
    
        getInputReg(&(inA[0]), (BYTE)((*(pComb+1) >> 24) & 0xff), FALSE);
        getInputReg(&(inA[1]), (BYTE)((*(pComb+1) >> 16) & 0xff), FALSE);
        getInputReg(&(inA[2]), (BYTE)((*(pComb+1) >>  8) & 0xff), FALSE);
        getInputReg(&(inA[3]), (BYTE)((*(pComb+1) >>  0) & 0xff), FALSE);
        
        // get AB
        BOOL abdot = (*(pComb+2) >> 12) & PS_COMBINEROUTPUT_AB_DOT_PRODUCT;
        char abRGB[64], abA[64];
        if(abdot)
        {
            sprintf(abRGB, "%s . %s", inRGB[0].name, inRGB[1].name);
        }
        else
        {
            if(inRGB[0].isOne)
                sprintf(abRGB, "%s", inRGB[1].name);
            else if(inRGB[1].isOne)
                sprintf(abRGB, "%s", inRGB[0].name);
            else if(inRGB[0].isMinusOne)
                sprintf(abRGB, "-%s", inRGB[1].name);
            else if(inRGB[1].isMinusOne)
                sprintf(abRGB, "-%s", inRGB[0].name);
            else if(inRGB[0].isZero || inRGB[1].isZero)
                strcpy(abRGB, "0");
            else
                sprintf(abRGB, "%s * %s", inRGB[0].name, inRGB[1].name);
        }
        if(inA[0].isOne)
            sprintf(abA, "%s", inA[1].name);
        else if(inA[1].isOne)
            sprintf(abA, "%s", inA[0].name);
        else if(inA[0].isMinusOne)
            sprintf(abA, "-%s", inA[1].name);
        else if(inA[1].isMinusOne)
            sprintf(abA, "-%s", inA[0].name);
        else if(inA[0].isZero || inA[1].isZero)
            strcpy(abA, "0");
        else
            sprintf(abA, "%s * %s", inA[0].name, inA[1].name);
        
        // get CD
        BOOL cddot = (*(pComb+2) >> 12) & PS_COMBINEROUTPUT_CD_DOT_PRODUCT;
        char cdRGB[64], cdA[64];
        if(cddot)
        {
            sprintf(cdRGB, "%s . %s", inRGB[2].name, inRGB[3].name);
        }
        else
        {
            if(inRGB[2].isOne)
                sprintf(cdRGB, "%s", inRGB[3].name);
            else if(inRGB[3].isOne)
                sprintf(cdRGB, "%s", inRGB[2].name);
            else if(inRGB[2].isMinusOne)
                sprintf(cdRGB, "-%s", inRGB[3].name);
            else if(inRGB[3].isMinusOne)
                sprintf(cdRGB, "-%s", inRGB[2].name);
            else if(inRGB[2].isZero || inRGB[3].isZero)
                strcpy(cdRGB, "0");
            else
                sprintf(cdRGB, "%s * %s", inRGB[2].name, inRGB[3].name);
        }
        if(inA[2].isOne)
            sprintf(cdA, "%s", inA[3].name);
        else if(inA[3].isOne)
            sprintf(cdA, "%s", inA[2].name);
        else if(inA[2].isMinusOne)
            sprintf(cdA, "-%s", inA[3].name);
        else if(inA[3].isMinusOne)
            sprintf(cdA, "-%s", inA[2].name);
        else if(inA[2].isZero || inA[3].isZero)
            strcpy(cdA, "0");
        else
            sprintf(cdA, "%s * %s", inA[2].name, inA[3].name);

        DWORD abOutRGB, cdOutRGB, sumOutRGB;
        DWORD abOutA, cdOutA, sumOutA;
        abOutRGB = ((*(pComb+2)) >> 4) & 0xf;
        cdOutRGB = ((*(pComb+2)) >> 0) & 0xf;
        sumOutRGB = ((*(pComb+2)) >> 8) & 0xf;
        abOutA = ((*(pComb+3)) >> 4) & 0xf;
        cdOutA = ((*(pComb+3)) >> 0) & 0xf;
        sumOutA = ((*(pComb+3)) >> 8) & 0xf;

        char abInstA[1024], abInstRGB[1024];
        char cdInstA[1024], cdInstRGB[1024];
        char sumInstA[1024], sumInstRGB[1024];
        char *preRGB, *postRGB, *preA, *postA;

        GetOutputModifier(*(pComb+2), &preRGB, &postRGB);
        GetOutputModifier(*(pComb+3), &preA, &postA);

        abInstA[0] = abInstRGB[0] = '\0';
        cdInstA[0] = cdInstRGB[0] = '\0';
        sumInstA[0] = sumInstRGB[0] = '\0';

        if(abOutRGB != PS_REGISTER_DISCARD)
            sprintf(abInstRGB, "%s.rgb = %s;", getoutput(abOutRGB), abRGB);
        if(abOutA != PS_REGISTER_DISCARD)
            sprintf(abInstA, "%s.a = %s;", getoutput(abOutA), abA);
        if(cdOutRGB != PS_REGISTER_DISCARD)
            sprintf(cdInstRGB, "%s.rgb = %s;", getoutput(cdOutRGB), cdRGB);
        if(cdOutA != PS_REGISTER_DISCARD)
            sprintf(cdInstA, "%s.a = %s;", getoutput(cdOutA), cdA);
        if(sumOutRGB != PS_REGISTER_DISCARD)
        {
            BOOL rgbmux = (*(pComb+2) >> 12) & PS_COMBINEROUTPUT_AB_CD_MUX;
            if(rgbmux)
                sprintf(sumInstRGB, "%s.rgb = %smuxargument0 ? %s : %s%s;", getoutput(sumOutRGB), preRGB, cdRGB, abRGB, postRGB);
            else                              
                sprintf(sumInstRGB, "%s.rgb = %s%s + %s%s;", getoutput(sumOutRGB), preRGB, abRGB, cdRGB, postRGB);
        }
        if(sumOutA != PS_REGISTER_DISCARD)
        {
            BOOL amux   = (*(pComb+3) >> 12) & PS_COMBINEROUTPUT_AB_CD_MUX;
            if(amux)
                sprintf(sumInstA, "%s.a = %smuxargument0 ? %s : %s%s;", getoutput(sumOutA), preA, cdA, abA, postA);
            else
                sprintf(sumInstA, "%s.a = %s%s + %s%s;", getoutput(sumOutA), preA, abA, cdA, postA);
        }
        combineAndPrint(abInstRGB, abInstA, fp);
        combineAndPrint(cdInstRGB, cdInstA, fp);
        combineAndPrint(sumInstRGB, sumInstA, fp);
    }
    else
    {
        getInputReg(&(inRGB[0]), (BYTE)((*pComb >> 24) & 0xff), TRUE);
        getInputReg(&(inRGB[1]), (BYTE)((*pComb >> 16) & 0xff), TRUE);
        getInputReg(&(inRGB[2]), (BYTE)((*pComb >>  8) & 0xff), TRUE);
        getInputReg(&(inRGB[3]), (BYTE)((*pComb >>  0) & 0xff), TRUE);
        getInputReg(&(inRGB[4]), (BYTE)((*(pComb+1) >> 24) & 0xff), TRUE);
        getInputReg(&(inRGB[5]), (BYTE)((*(pComb+1) >> 16) & 0xff), TRUE);
        getInputReg(&(inRGB[6]), (BYTE)((*(pComb+1) >>  8) & 0xff), FALSE);
        
        char xfc[1024], reg[64];
        if(inRGB[0].isZero)
        {
            sprintf(xfc, "out.rgb = %s", inRGB[2].name);
        }
        else if(inRGB[0].isOne)
        {
            sprintf(xfc, "out.rgb = %s", inRGB[1].name);
        }
        else
        {
            sprintf(xfc, "out.rgb = (%s * %s) + (1-%s) * %s", 
                    inRGB[0].name, inRGB[1].name, inRGB[0].name, inRGB[2].name);
        }

        if(!inRGB[3].isZero)
        {
            sprintf(reg, " + %s", inRGB[3].name);
            strcat(xfc, reg);
        }
        fprintf(fp, "<p class=Normal>%s</p>\n", xfc);

        if((UsedInputRegisters >> PS_REGISTER_EF_PROD) & 0x1)
        {
            sprintf(xfc, "prod.rgb = (%s * %s)", inRGB[4].name, inRGB[5].name);
            fprintf(fp, "<p class=Normal>%s</p>\n", xfc);
        }
        
        if((UsedInputRegisters >> PS_REGISTER_V1R0_SUM) & 0x1)
        {
            sprintf(xfc, "sum.rgb = (r0.rgb + v1.rgb)");
            fprintf(fp, "<p class=Normal>%s</p>\n", xfc);
        }

        sprintf(xfc, "out.a = %s", inRGB[6].name);
        fprintf(fp, "<p class=Normal>%s</p>\n", xfc);
    }
}

void PrintReg(DWORD *pReg, CHAR *pName, CHAR *pStyle, FILE *fp, BOOL bIsBold, DWORD *pAddr)
{
    CHAR *pTextStyle;
    CHAR signhex[64], sign[64], ulong[64], color[64], alpha[64], signint[64];
    static DWORD id=0;
    short dwA = (short)((*pReg >> 16) & 0xffff);
    short dwR = (short)((*pReg >>  0) & 0xffff);
    short dwG = (short)((*(pReg+1) >> 16) & 0xffff);
    short dwB = (short)((*(pReg+1) >>  0) & 0xffff);

    sprintf(signhex, "0x%03x 0x%03x 0x%03x 0x%03x", (USHORT)dwA & 0x1ff, (USHORT)dwR & 0x1ff, (USHORT)dwG & 0x1ff, (USHORT)dwB & 0x1ff);
    sprintf(sign, "%1.7f %1.7f %1.7f %1.7f", dwA/255.f, dwR/255.f, dwG/255.f, dwB/255.f);
    sprintf(ulong, "0x%02x%02x%02x%02x", (USHORT)dwA & 0xff, (USHORT)dwR & 0xff, (USHORT)dwG & 0xff, (USHORT)dwB & 0xff);
    sprintf(color, "%02x%02x%02x", (USHORT)dwR & 0xff, (USHORT)dwG & 0xff, (USHORT)dwB & 0xff);
    sprintf(alpha, "%02x%02x%02x", (USHORT)dwA & 0xff, (USHORT)dwA & 0xff, (USHORT)dwA & 0xff);
    sprintf(signint, "%d %d %d %d", dwA, dwR, dwG, dwB);
    
    if(bIsBold)
        pTextStyle="Bold";
    else
        pTextStyle="Normal";

    fprintf(fp,
" <tr>\n"
"  <td class=%s><p class=%s>%s</p></td>\n"
"  <td class=%s><p class=%s><SPAN ID=id%04d onmouseover=\"this.style.cursor='hand'\" onclick=\"change('%s','%s','%s','%s',id%04d)\">%s</SPAN></p></td>\n"
"  <td width=10 style='background:#%s'></td>\n"
"  <td width=30 style='background:#%s'></td>\n", pStyle, pTextStyle, pName, pStyle, pTextStyle, id, signhex, ulong, sign, signint, id, signhex, alpha, color);
    if(pAddr == 0)
    {
        fprintf(fp, " </tr>\n");
    }
    else
    {
        dwA = (short)((*pAddr >> 16) & 0xffff);
        dwR = (short)((*pAddr >>  0) & 0xffff);
        dwG = (short)((*(pAddr+1) >> 16) & 0xffff);
        dwB = (short)((*(pAddr+1) >>  0) & 0xffff);
        sprintf(sign, "  Addr=(%1.6f %1.6f %1.6f %1.6f)", dwR/255.f, dwG/255.f, dwB/255.f, dwA/255.f);
        fprintf(fp,
"  <td class=Normal><p class=Normal>%s</p></td>\n"
"  </tr>\n", sign);
    }
    id++;
}

void PrintRegisterBlock(DWORD *ptr, FILE *fp, DWORD mask)
{
    if((mask >> PS_REGISTER_R0) & 0x1)
        PrintReg(ptr, "R0:", "CyanReg", fp, FALSE, 0); 
    if((mask >> PS_REGISTER_R1) & 0x1)
        PrintReg(ptr+2, "R1:", "CyanReg", fp, FALSE, 0); 
    if((mask >> PS_REGISTER_C0) & 0x1)
        PrintReg(ptr+4, "C0:", "OReg", fp, FALSE, 0); 
    if((mask >> PS_REGISTER_C1) & 0x1)
        PrintReg(ptr+6, "C1:", "OReg", fp, FALSE, 0); 
    if((mask >> PS_REGISTER_V0) & 0x1)
        PrintReg(ptr+8, "V0:", "GreenReg", fp, FALSE, 0); 
    if((mask >> PS_REGISTER_V1) & 0x1)
        PrintReg(ptr+10, "V1:", "GreenReg", fp, FALSE, 0); 
    if((mask >> PS_REGISTER_FOG) & 0x1)
        PrintReg(ptr+20, "Fog:", "TanReg", fp, FALSE, 0); 
    if((mask >> PS_REGISTER_V1R0_SUM) & 0x1)
        PrintReg(ptr+22, "Sum:", "TanReg", fp, FALSE, 0); 
    if((mask >> PS_REGISTER_EF_PROD) & 0x1)
        PrintReg(ptr+24, "Prod:", "TanReg", fp, FALSE, 0); 
    if(mask == 0xffffffff)
    {
        if((mask >> PS_REGISTER_T0) & 0x1)
            PrintReg(ptr+12, "T0:", "TanReg", fp, FALSE, ptr+20); 
        if((mask >> PS_REGISTER_T1) & 0x1)
            PrintReg(ptr+14, "T1:", "TanReg", fp, FALSE, ptr+22); 
        if((mask >> PS_REGISTER_T2) & 0x1)
            PrintReg(ptr+16, "T2:", "TanReg", fp, FALSE, ptr+24); 
        if((mask >> PS_REGISTER_T3) & 0x1)
            PrintReg(ptr+18, "T3:", "TanReg", fp, FALSE, ptr+26); 
    }
    else
    {
        if((mask >> PS_REGISTER_T0) & 0x1)
            PrintReg(ptr+12, "T0:", "TanReg", fp, FALSE, 0); 
        if((mask >> PS_REGISTER_T1) & 0x1)
            PrintReg(ptr+14, "T1:", "TanReg", fp, FALSE, 0); 
        if((mask >> PS_REGISTER_T2) & 0x1)
            PrintReg(ptr+16, "T2:", "TanReg", fp, FALSE, 0); 
        if((mask >> PS_REGISTER_T3) & 0x1)
            PrintReg(ptr+18, "T3:", "TanReg", fp, FALSE, 0); 
    }
}

BOOL ParsePrimitives(BYTE *buf, FILE *fp)
{
    DWORD dwPrimCount=0;
    DWORD *ptr = FindOpcode(PSS_PRIMITIVE, buf);
    DWORD dwStageCount;
    DWORD dwReg;
    DWORD *pDescPtr;
    DWORD dwDPre, dwCPre, dwDPost, dwCPost;
    DWORD *pNext;
    if(!ptr) 
    {
        fprintf(fp, "<p class=Bold><B>No Primitives affected this pixel.</B></p>\n");
        return TRUE;
    }

    while(1)
    {
        switch(*ptr & 0xff000000)
        {
        case PSS_EOF:
            return TRUE;
        case PSS_PRIMITIVE:
            ptr++;
            dwCPre = *ptr++;
            dwDPre = *ptr++;
            dwCPost = *ptr++;
            dwDPost = *ptr++;
            fprintf(fp, 
"<HR>"
"<p class=Normal>&nbsp;</p>\n"
"<p class=Big><B>Primitive %d</B></p>\n"
"<table border=1 cellspacing=0 cellpadding=0 style='margin-left:.30in;border-collapse:collapse;border:none'>\n"
" <tr>\n"
"  <td class=RedReg><p class=Normal>Before:</p></td>\n"
"  <td class=RedReg><p class=Normal>depth=0x%08x</p></td>\n"
"  <td class=RedReg><p class=Normal>color=0x%08x</p></td>\n"
"  <td width=10 style='background:#%06x'></td>\n"
"  <td width=30 style='background:#%06x'></td>\n"
" </tr>\n"
" <tr>\n"
"  <td class=RedReg><p class=Normal>After:</p></td>\n"
"  <td class=RedReg><p class=Normal>depth=0x%08x</p></td>\n"
"  <td class=RedReg><p class=Normal>color=0x%08x</p></td>\n"
"  <td width=10 style='background:#%06x'></td>\n"
"  <td width=30 style='background:#%06x'></td>\n"
" </tr>\n"
"</table>\n", dwPrimCount++, dwDPre, dwCPre, smearalpha(dwCPre)&0xffffff, dwCPre&0xffffff,
              dwDPost, dwCPost, smearalpha(dwCPost)&0xffffff, dwCPost&0xffffff);
            dwStageCount=1;
            break;
        
        case PSS_NOSHADER:
            ptr++;
            fprintf(fp, 
"<p class=Normal>No pixel shader was active for this primitive.</p>\n");
            break;
        
        case PSS_STAGE0:
            ptr++;
            pDescPtr = ptr;   // save combiner description pointer
            ptr += 4;

            // print all registers
            fprintf(fp, 
"<p class=Normal>&nbsp;</p>\n"
"<p class=Normal>Initial State:</p>\n");
                fprintf(fp, 
"<table border=1 cellspacing=0 cellpadding=0 style='margin-left:.30in;border-collapse:collapse;border:none'>\n");

            if((dwPrimCount == 1))
                fprintf(fp, "<p class=Tiny>Click on register values to change the format.  Values are (A R G B)</p>\n");
    
            PrintRegisterBlock(ptr, fp, 0xffffffff & ~((1 << PS_REGISTER_FOG) |
                                                       (1 << PS_REGISTER_V1R0_SUM) |
                                                       (1 << PS_REGISTER_EF_PROD))); // display initial registers
            fprintf(fp, "</table>\n");
            
            fprintf(fp, 
"<p class=Normal>&nbsp;</p>\n"
"<p class=Bold>Combiner Stage %d:</p>\n", dwStageCount++);

            PrintCombinerDescription(fp, pDescPtr, FALSE);
            fprintf(fp, "<p class=Normal>Inputs:</p>\n");
            
            fprintf(fp, 
"<table border=1 cellspacing=0 cellpadding=0 style='margin-left:.30in;border-collapse:collapse;border:none'>\n");
            PrintRegisterBlock(ptr, fp, UsedInputRegisters);
            ptr += 28;
            fprintf(fp, "</table>\n");
            
            if((*ptr & 0xff000000) == PSS_STAGE)
            {
                fprintf(fp, "<p class=Normal>Outputs:</p>\n");
                fprintf(fp, 
"<table border=1 cellspacing=0 cellpadding=0 style='margin-left:.30in;border-collapse:collapse;border:none'>\n");
                PrintRegisterBlock(ptr+5, fp, UsedOutputRegisters);
                fprintf(fp, "</table>\n");
            }
            else if((*ptr & 0xff000000) == PSS_FINALSTAGE)
            {
                fprintf(fp, "<p class=Normal>Outputs (saturated):</p>\n");
                fprintf(fp, 
"<table border=1 cellspacing=0 cellpadding=0 style='margin-left:.30in;border-collapse:collapse;border:none'>\n");
                PrintRegisterBlock(ptr+3, fp, UsedOutputRegisters);
                fprintf(fp, "</table>\n");
            }
            break;

        case PSS_STAGE:
            ptr++;
            pDescPtr = ptr;   // save combiner description pointer
            ptr += 4;

            fprintf(fp, 
"<p class=Normal>&nbsp;</p>\n"
"<p class=Bold>Combiner Stage %d:</p>\n", dwStageCount++);

            PrintCombinerDescription(fp, pDescPtr, FALSE);
            fprintf(fp, "<p class=Normal>Inputs:</p>\n");
            
            fprintf(fp, 
"<table border=1 cellspacing=0 cellpadding=0 style='margin-left:.30in;border-collapse:collapse;border:none'>\n");
            PrintRegisterBlock(ptr, fp, UsedInputRegisters);
            ptr += 20;
            fprintf(fp, "</table>\n");
            
            if((*ptr & 0xff000000) == PSS_STAGE)
            {
                fprintf(fp, "<p class=Normal>Outputs:</p>\n");
                fprintf(fp, 
"<table border=1 cellspacing=0 cellpadding=0 style='margin-left:.30in;border-collapse:collapse;border:none'>\n");
                PrintRegisterBlock(ptr+5, fp, UsedOutputRegisters);
                fprintf(fp, "</table>\n");
            }
            else if((*ptr & 0xff000000) == PSS_FINALSTAGE)
            {
                fprintf(fp, "<p class=Normal>Outputs (saturated):</p>\n");
                fprintf(fp, 
"<table border=1 cellspacing=0 cellpadding=0 style='margin-left:.30in;border-collapse:collapse;border:none'>\n");
                PrintRegisterBlock(ptr+3, fp, UsedOutputRegisters);
                fprintf(fp, "</table>\n");
            }
            break;

        case PSS_FINALSTAGE:
            fprintf(fp, 
"<p class=Normal>&nbsp;</p>\n"
"<p class=Bold>Final Combiner Stage:</p>\n");
            ptr++;
            pDescPtr = ptr;   // save combiner description pointer
            ptr += 2;

            PrintCombinerDescription(fp, pDescPtr, TRUE);
            fprintf(fp, "<p class=Normal>Inputs:</p>\n");
            fprintf(fp, 
"<table border=1 cellspacing=0 cellpadding=0 style='margin-left:.30in;border-collapse:collapse;border:none'>\n");
            PrintRegisterBlock(ptr, fp, UsedInputRegisters);
            fprintf(fp, "</table>\n");
            
            fprintf(fp, "<p class=Normal>Output:</p>\n");
            fprintf(fp, 
"<table border=1 cellspacing=0 cellpadding=0 style='margin-left:.30in;border-collapse:collapse;border:none'>\n");
            short output[4];
            output[0] = (SHORT)((dwCPost >> 16) & 0xff);
            output[1] = (SHORT)((dwCPost >> 24) & 0xff);
            output[2] = (SHORT)((dwCPost >>  0) & 0xff);
            output[3] = (SHORT)((dwCPost >>  8) & 0xff);
            PrintReg((DWORD *)output, "Out", "RedReg", fp, FALSE, 0); 
            fprintf(fp, "</table>\n");

            ptr += 20;
            break;

        case PSS_PIXEL:
        case PSS_FINAL:
        case PSS_IMAGE:
            ptr += (*ptr & 0x00ffffff);
            break;

        default:
            return FALSE;   //unrecognized opcode
        }
    }

    return TRUE;
}

void GenerateHTML(BYTE *buf, FILE *fp)
{
    DWORD *ptr;
    DWORD dwX, dwY, dwColor, dwDepth;
    struct tm *newtime;
    time_t aclock;

    time( &aclock );                 /* Get time in seconds */
    newtime = localtime( &aclock );  /* Convert time to struct tm form*/

    // Find the pixel coordinates
    ptr = FindOpcode(PSS_PIXEL, buf);
    if(!ptr) goto error;
    ptr++;
    dwX = *ptr & 0xffff;
    dwY = (*ptr >> 16) & 0xffff;

    // print html header
    fprintf(fp,
"<html>\n"
"<head>\n"
"<title>Pixel Shader Snapshot <%d,%d></title>\n"
"<style>\n"
"<!--\n"
" p.Normal    {margin:0in;font-size:10.0pt;font-family:\"Courier New\";}\n"
" p.Bold      {margin:0in;font-size:10.0pt;font-family:\"Courier New\";font-weight:\"bold\"}\n"
" p.Tiny      {margin:0in;font-size:8.0pt;font-family:\"Arial\";}\n"
" p.Big       {margin:0in;font-size:14.0pt;font-family:\"Courier New\";}\n"
" td.RedReg   {background:#ffd0d0;padding:0in 5.4pt 0in 5.4pt;}\n"
" td.CyanReg  {background:#d0FFFF;padding:0in 5.4pt 0in 5.4pt;}\n"
" td.OReg     {background:#FFFFbb;padding:0in 5.4pt 0in 5.4pt;}\n"
" td.GreenReg {background:#d0ffd0;padding:0in 5.4pt 0in 5.4pt;}\n"
" td.TanReg   {background:#ffddaa;padding:0in 5.4pt 0in 5.4pt;} \n"
"-->\n"
"</style>\n"
"</head>\n"
"<body bgcolor=d0d0d0>\n"
"<p class=Big><B>Pixel Shader Snapshot for Pixel <%d,%d></B></p>\n"
"<SCRIPT type=\"text\\javascript\">\n"
"<!--\n"
"function change(a,b,c,d,vari)\n"
"{\n"
"  if (vari.innerHTML==a) vari.innerHTML = b;\n"
"  else if (vari.innerHTML==b) vari.innerHTML = c;\n"
"  else if (vari.innerHTML==c) vari.innerHTML = d;\n"
"  else vari.innerHTML = a;\n"
"}\n"
"// -->\n"
"function launchCapture()\n"
"{\n"
"    var cmdline = \"\"\n"
"    cmdline = \"xbpscapture.exe \" + window.event.offsetX + \" \" + window.event.offsetY\n"
"    var WshShell = new ActiveXObject(\"WScript.Shell\");\n"
"    WshShell.Run(cmdline);\n"
"}\n"
"</SCRIPT>\n"
"<p class=Normal>&nbsp;</p>\n"
"<p class=Normal>%s</p>\n"
"<img onmouseover=\"this.style.cursor='crosshair'\" onclick=\"launchCapture()\" src=\"%s\">\n"
"<table border=0 cellspacing=10 cellpadding=0 style='border-collapse:collapse;border:none'>\n"
" <tr>\n"
"  <td><p class=Normal><img width=256 height=256 src=\"%s\"></p></td>\n"
"  <td><p class=Normal><img width=256 height=256 src=\"%s\"></p></td>\n"
" </tr>\n"
" <tr>\n"
"  <td align=center><p class=Normal>64x64 pixels around sample point</p></td>\n"
"  <td align=center><p class=Normal>5x5 pixels around sample point</p></td>\n"
" </tr>\n"
" <tr>\n"
"  <td align=center><p class=Normal>(scaled 4x, center 5x5 marked)</p></td>\n"
"  <td align=center><p class=Normal>(scaled 51.2x)</p></td>\n"
" </tr>\n"
"</table>\n"
"<p class=Normal>&nbsp;</p>\n", dwX, dwY, dwX, dwY, asctime( newtime ), 
                                g_BMP3Filename, g_BMPFilename, g_BMP2Filename);

    if(g_dwFlags & 0x1)
    {
        fprintf(fp, "<p class=Bold>Pixels filtered by debug marker %d</p>\n", g_dwMarker);
    }
    
    fprintf(fp, "<p class=Normal>&nbsp;</p>\n");

    // Find the final framebuffer values
    ptr = FindOpcode(PSS_FINAL, buf);
    if(!ptr) goto error;
    ptr++;
    dwColor = *ptr++;
    dwDepth = *ptr;
    fprintf(fp,
"<table border=1 cellspacing=0 cellpadding=0 style='border-collapse:collapse;border:none'>\n"
" <tr>\n"
"  <td class=RedReg><p class=Normal>Final values for <%d,%d>:</p></td>\n"
"  <td class=RedReg><p class=Normal>depth=0x%08x</p></td>\n"
"  <td class=RedReg><p class=Normal>color=0x%08x</p></td>\n"
"  <td width=10 style='background:#%06x'></td>\n"
"  <td width=30 style='background:#%06x'></td>\n"
" </tr>\n"
"</table>\n"
"<p class=Normal>&nbsp;</p>\n", dwX, dwY, dwDepth, dwColor, smearalpha(dwColor) & 0xffffff, dwColor & 0xffffff);

    if(!ParsePrimitives(buf, fp)) goto error;
    fprintf(fp, "</body>\n</html>\n");
    return;

error:
    fprintf(fp, "Error parsing snapshot data\n");
    return;
}

