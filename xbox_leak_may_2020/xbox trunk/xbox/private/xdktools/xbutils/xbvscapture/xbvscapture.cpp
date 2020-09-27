/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xbvscapture.cpp
 *  Content:    Vertex Shader Debugger
 *
 ***************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include "d3d8-xbox.h"

typedef struct _D3DVsInstruction {
    unsigned long eos:1;        /* 0:00 last instruction */
    unsigned long cin:1;        /* 0:01 ctx indexed address */
    unsigned long  om:1;        /* 0:02 output mux */
    unsigned long  oc:9;        /* 0:03 output write control */
    unsigned long owm:4;        /* 0:12 output write mask */
    unsigned long swm:4;        /* 0:16 secondary register write mask */
    unsigned long  rw:4;        /* 0:20 register write */
    unsigned long rwm:4;        /* 0:24 primary register write mask */
    unsigned long cmx:2;        /* 0:28 c mux (NA,r1,v,c) */
    unsigned long crr:4;        /* 0:30 c register read */
    unsigned long cws:2;        /* 1:02 c w swizzle */
    unsigned long czs:2;        /* 1:04 c z swizzle */
    unsigned long cys:2;        /* 1:06 c y swizzle */
    unsigned long cxs:2;        /* 1:08 c x swizzle */
    unsigned long cne:1;        /* 1:10 c negate */
    unsigned long bmx:2;        /* 1:11 b mux (NA,r1,v,c) */
    unsigned long brr:4;        /* 1:13 b register read */
    unsigned long bws:2;        /* 1:17 b w swizzle */
    unsigned long bzs:2;        /* 1:19 b z swizzle */
    unsigned long bys:2;        /* 1:21 b y swizzle */
    unsigned long bxs:2;        /* 1:23 b x swizzle */
    unsigned long bne:1;        /* 1:25 b negate */
    unsigned long amx:2;        /* 1:26 a mux (NA,r0,v,c) */
    unsigned long arr:4;        /* 1:28 a register read */
    unsigned long aws:2;        /* 2:00 a w swizzle */
    unsigned long azs:2;        /* 2:02 a z swizzle */
    unsigned long ays:2;        /* 2:04 a y swizzle */
    unsigned long axs:2;        /* 2:06 a x swizzle */
    unsigned long ane:1;        /* 2:08 a negate */
    unsigned long  va:4;        /* 2:09 ibuffer address */
    unsigned long  ca:8;        /* 2:13 ctx address */
    unsigned long mac:4;        /* 2:21 MLU/ALU op */
    unsigned long ilu:3;        /* 2:25 ILU op */
} D3DVsInstruction;

static const char* kOutNames[] = {"oPos",
    "o1?", "o2?",
    "oD0", "oD1",
    "oFog", "oPts",
    "oB0", "oB1",
    "oT0", "oT1", "oT2", "oT3", "???"
};

extern "C"
HRESULT DisassembleInstruction(const D3DVsInstruction* pI, char* outBuf, DWORD outLength);

void ConvertPackedDWordsToUCode(const DWORD* pIn, D3DVsInstruction* pucode)
{
    memset(pucode,0,sizeof(D3DVsInstruction));
    DWORD x = *pIn++;
    DWORD y = *pIn++;
    DWORD z = *pIn++;
    DWORD w = *pIn++;
    // Word X -- ignore
    // Word Y
	pucode->aws = (y >> 0);
	pucode->azs = (y >> 2);
    pucode->aws = (y >> 0);
    pucode->azs = (y >> 2);
    pucode->ays = (y >> 4);
    pucode->axs = (y >> 6);
    pucode->ane = (y >> 8);
    pucode->va  = (y >> 9);
    pucode->ca  = (y >>13);
    pucode->mac = (y >>21);
    pucode->ilu = (y >>25);
    //Word Z
    pucode->crr = ((0xc & (z << 2)) | ((w >> 30) & 0x3));
    pucode->cws = (z >> 2);
    pucode->czs = (z >> 4);
    pucode->cys = (z >> 6);
    pucode->cxs = (z >> 8);
    pucode->cne = (z >>10);
    pucode->bmx = (z >>11);
    pucode->brr = (z >>13);
    pucode->bws = (z >>17);
    pucode->bzs = (z >>19);
    pucode->bys = (z >>21);
    pucode->bxs = (z >>23);
    pucode->bne = (z >>25);
    pucode->amx = (z >>26);
    pucode->arr = (z >>28);

    //Word W
    pucode->eos = (w >> 0);
    pucode->cin = (w >> 1);
    pucode->om  = (w >> 2);
    pucode->oc  = (w >> 3);
    pucode->owm = (w >>12);
    pucode->swm = (w >>16);
    pucode->rw  = (w >>20);
    pucode->rwm = (w >>24);
    pucode->cmx = (w >>28);
} 

/*
**
** xbvscapture
**
** Captures a vertex shader snapshot
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
#define VSS_EOF         0x80000000
#define VSS_VERTEX      0x81000000
#define VSS_INSTRUCTION 0x82000000
#define VSS_PROGRAM     0x84000000
#define VSS_STATS       0x85000000

typedef struct vtxreg
{
    DWORD w,z,y,x;
} VTXREG;

typedef struct vtxio
{
    DWORD regs;     // lower 16 are VAB, upper are temp
    DWORD outs;     // output registers
    DWORD cnst;     // which constant
} VTXIO;

DWORD g_dwFlags = 0;
DWORD g_dwMarker;

char g_HTMLFilename[MAX_PATH];
char g_BMPFilename[MAX_PATH];
char g_TmpPath[MAX_PATH];
INT g_ProgramLength;
struct _program
{
    VTXREG inst;
    char   buf[1024];
}Program[126];

BYTE buf[SHADER_CAPTURE_BUFFER_SIZE];

DWORD UsedInputRegisters;
DWORD UsedOutputRegisters;

VTXREG vals[256];
BYTE   idxs[500];

void GenerateHTML(BYTE *buf, FILE *fp);

BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    SetDlgItemText(hwnd, IDC_StatusText, "Initializing...");
    GetTempPath(1024, g_TmpPath);
    SetDlgItemInt(hwnd, IDC_EDIT1, 0, FALSE);
    SetDlgItemInt(hwnd, IDC_EDIT2, 100, FALSE);
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
    
    while((*dwbuf & 0xff000000) != VSS_EOF)
    {
        if((*dwbuf & 0xff000000) == op)
            return dwbuf;   // found
        
        dwbuf += *dwbuf & 0x00ffffff;   // move to next opcode
        if(((DWORD)dwbuf - (DWORD)buf) > SHADER_CAPTURE_BUFFER_SIZE)
            return NULL;    // not found and no EOF
    }
    if((op == VSS_EOF) && ((*dwbuf & 0xff000000) == VSS_EOF))
        return dwbuf;
    
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
    sprintf(g_HTMLFilename, "%sxbvscpta%s.htm", g_TmpPath, time);
    sprintf(g_BMPFilename, "%sxbvscptb%s.bmp", g_TmpPath, time);
    
    // get the screen shot
    hr = DmScreenShot(g_BMPFilename);  // don't report an error if it occurs
    
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
        hr = DmVertexShaderSnapshot(GetDlgItemInt(hwnd, IDC_EDIT1, NULL, FALSE), 
                                    GetDlgItemInt(hwnd, IDC_EDIT2, NULL, FALSE),
                                    g_dwFlags,
                                    g_dwMarker,
                                    buf);
        if(FAILED(hr)) 
        {
            if(hr == XBDM_FILEERROR)
                hr = HrLastError();
            MessageBox(NULL, (const char*)buf, "xbvscapture Error", MB_OK | MB_TASKMODAL);
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
        SetDlgItemText(hwnd, IDC_StatusText, "Deleting xbvscpta*.htm");
        DeleteFiles("xbvscpta*.htm");
        SetDlgItemText(hwnd, IDC_StatusText, "Deleting xbvscptb*.bmp");
        DeleteFiles("xbvscptb*.bmp");
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
        hr = DmVertexShaderSnapshot(dwX, dwY, 0, 0, buf);
        
        if(FAILED(hr)) 
        {
            if(hr == XBDM_FILEERROR)
                hr = HrLastError();
            MessageBox(NULL, (const char*)buf, "xbvscapture Error", MB_OK | MB_TASKMODAL);
        }
        else
        {
            WriteOutputFiles(buf);
        }
        Sleep(5000);
        return 0;
    }
    
    // display the UI
    DialogBox(NULL, MAKEINTRESOURCE(IDD_VSCAPTURE), NULL, Dlg_Proc);
    return 0;
}


void GetVertexInputsAndOutputs(VTXREG *pInst, VTXIO *pInputs, VTXIO *pOutputs)
{
    // initialize
    pInputs->regs = pOutputs->regs = 0;           // no regs selected
    pInputs->outs = pOutputs->outs = 0;           // no outputs selected
    pInputs->cnst = pOutputs->cnst = 0xffffffff;  // invalid constant

    // decode instruction
    DWORD aws = (pInst->y >>  0) & 0x3;
    DWORD azs = (pInst->y >>  2) & 0x3;
    DWORD ays = (pInst->y >>  4) & 0x3;
    DWORD axs = (pInst->y >>  6) & 0x3;
    DWORD ane = (pInst->y >>  8) & 0x1;
    DWORD va =  (pInst->y >>  9) & 0xf;
    DWORD ca =  (pInst->y >> 13) & 0xff;
    DWORD mac = (pInst->y >> 21) & 0xf;
    DWORD ilu = (pInst->y >> 25) & 0x7;

    DWORD crr = (pInst->z <<  2) & 0xc;
    DWORD cws = (pInst->z >>  2) & 0x3;
    DWORD czs = (pInst->z >>  4) & 0x3;
    DWORD cys = (pInst->z >>  6) & 0x3;
    DWORD cxs = (pInst->z >>  8) & 0x3;
    DWORD cne = (pInst->z >> 10) & 0x1;
    DWORD bmx = (pInst->z >> 11) & 0x3;
    DWORD brr = (pInst->z >> 13) & 0xf;
    DWORD bws = (pInst->z >> 17) & 0x3;
    DWORD bzs = (pInst->z >> 19) & 0x3;
    DWORD bys = (pInst->z >> 21) & 0x3;
    DWORD bxs = (pInst->z >> 23) & 0x3;
    DWORD bne = (pInst->z >> 25) & 0x1;
    DWORD amx = (pInst->z >> 26) & 0x3;
    DWORD arr = (pInst->z >> 28) & 0xf;

    DWORD eos = (pInst->w >>  0) & 0x1;
    DWORD cin = (pInst->w >>  1) & 0x1;
    DWORD om =  (pInst->w >>  2) & 0x1;
    DWORD oc =  (pInst->w >>  3) & 0x1ff;
    DWORD owm = (pInst->w >> 12) & 0xf;
    DWORD swm = (pInst->w >> 16) & 0xf;
    DWORD rw =  (pInst->w >> 20) & 0xf;
    DWORD rwm = (pInst->w >> 24) & 0xf;
    DWORD cmx = (pInst->w >> 28) & 0x3;
    crr |= (pInst->w >> 30) & 0x3;

    // decode inputs
    if(mac != 0)
    {
        switch(amx)
        {
        case 1: pInputs->regs |= (1 << (16+arr)); break;   // temp register input
        case 2: pInputs->regs |= (1 << va); break;       // vab register input
        case 3: pInputs->cnst = ca;                      // constant
        }
    }
    if((mac != 0x1) &&  // MAC_MOV
       (mac != 0x3) &&  // MAC_ADD
       (mac != 0xd))    // MAC_ARL
    {
        switch(bmx)
        {
        case 1: pInputs->regs |= (1 << (16+brr)); break;   // temp register input
        case 2: pInputs->regs |= (1 << va); break;       // vab register input
        case 3: pInputs->cnst = ca;                      // constant
        }
    }
    if((mac == 0x3) ||  // MAC_ADD
       (mac == 0x4) ||  // MAC_MAD
       (ilu != 0))
    {
        switch(cmx)
        {
        case 1: pInputs->regs |= (1 << (16+crr)); break;   // temp register input
        case 2: pInputs->regs |= (1 << va); break;       // vab register input
        case 3: pInputs->cnst = ca;                      // constant
        }
    }

    // decode outputs
    if(rwm)
        pOutputs->regs |= (1 << (16+rw));              // temp register output
    
    if(owm)
    {
        if(oc & 0x100)
            pOutputs->outs |= (1 << (oc & 0xff));
        else
            pOutputs->cnst = oc;
    }
    if(swm)
    {
        if((mac != 0) && (ilu != 0))
            pOutputs->regs |= (1 << (16+1));               // add R1 output
        else
            pOutputs->regs |= (1 << (16+rw));              // temp register output
    }
}

FLOAT sprintval(char *buf, DWORD idxptr, DWORD swiz)
{
    BYTE idx = idxs[idxptr];
    sprintf(buf, "%g %g %g %g", *(FLOAT*)&(vals[idx].x), 
                                *(FLOAT*)&(vals[idx].y), 
                                *(FLOAT*)&(vals[idx].z), 
                                *(FLOAT*)&(vals[idx].w));
    switch(swiz)
    {
    case 0: return *(FLOAT*)&(vals[idx].x);
    case 1: return *(FLOAT*)&(vals[idx].y);
    case 2: return *(FLOAT*)&(vals[idx].z);
    case 3: return *(FLOAT*)&(vals[idx].w);
    }
    return 0;
}

void CountVertices(BYTE *buf, DWORD *min, DWORD *max)
{
    DWORD *ptr;

    *min = 0xffffffff;
    *max = 0;
    ptr = FindOpcode(VSS_VERTEX, buf);
    while(ptr)
    {
        if((*ptr & 0xff000000) == VSS_VERTEX)
        {
            if(*(ptr+1) > *max) *max = *(ptr+1);
            if(*(ptr+1) < *min) *min = *(ptr+1);
        }
        else if((*ptr & 0xff000000) == VSS_EOF)
            break;

        ptr = ptr + (*ptr & 0x00ffffff);
    }
}

void GenerateHTML(BYTE *buf, FILE *fp)
{
    DWORD *ptr, inputcnt, skip;
    int i, icnt;
    DWORD dwX, dwY, dwColor, dwDepth;
    VTXREG inst;
    DWORD ARLType=0, ARLCount, ARLSwizzle;
    FLOAT ARLVal, ARLRet;
    INT a0x, idx;
    DWORD *idxdwptr;
    INT idxcnt, valcnt, idxptr;
    DWORD minVert, maxVert;
    char vertstr[256];
    char valstr[256];
    DWORD idcnt=0;
    struct tm *newtime;
    time_t aclock;

    time( &aclock );                 /* Get time in seconds */
    newtime = localtime( &aclock );  /* Convert time to struct tm form*/

    // print html header
    fprintf(fp,
"<html>\n"
"<head>\n"
"<title>Vertex Shader Debugger</title>\n"
"<style>\n"
"<!--\n"
" p.Normal    {margin:0in;font-size:10.0pt;font-family:\"Courier New\";}\n"
" p.Bold      {margin:0in;font-size:10.0pt;font-family:\"Courier New\";font-weight:\"bold\"}\n"
" p.Tiny      {margin:0in;font-size:8.0pt;font-family:\"Arial\";}\n"
" p.Big       {margin:0in;font-size:14.0pt;font-family:\"Courier New\";}\n"
" td.Outcolor {background:#fff0e0;padding:0in 5.4pt 0in 5.4pt;}\n"
" td.Incolor  {background:#e0f0ff;padding:0in 5.4pt 0in 5.4pt;}\n"
" td.OReg     {background:#FFFFbb;padding:0in 5.4pt 0in 5.4pt;}\n"
" td.GreenReg {background:#d0ffd0;padding:0in 5.4pt 0in 5.4pt;}\n"
" td.TanReg   {background:#ffddaa;padding:0in 5.4pt 0in 5.4pt;} \n"
"-->\n"
"</style>\n"
"</head>\n"
"<body bgcolor=d0d0d0>\n"
"<p class=Big><B>Vertex Shader Debugger</B></p>\n"
"<SCRIPT type=\"text\\javascript\">\n"
"<!--\n"
"function change(a,b,vari)\n"
"{\n"
"  if (vari.innerHTML==a) vari.innerHTML = b;\n"
"  else vari.innerHTML = a;\n"
"}\n"
"// -->\n"
"</SCRIPT>\n"
"<p class=Normal>%s</p>\n"
"<img src=\"%s\">\n"
"<p class=Normal>&nbsp;</p>\n", asctime( newtime ), g_BMPFilename);
    
    if(g_dwFlags & 0x1)
    {
        fprintf(fp, "<p class=Bold>Vertices filtered by debug marker %d</p>\n", g_dwMarker);
    }
    
    ptr = FindOpcode(VSS_STATS, buf);
    if(ptr)
    {
        fprintf(fp,
"<table border=0 cellspacing=0 cellpadding=0>\n"
" <tr>\n"
"  <td><p class=Bold align=center>Scene Statistics</p></td>\n"
" </tr>\n"
" <tr>\n"
"  <td><p  align=right>Vertices using a shader:</p></td>\n"
"  <td><p  align=right>%d</p></td>\n"
" </tr>\n"
" <tr>\n"
"  <td><p  align=right>Vertices using the fixed pipeline:</p></td>\n"
"  <td><p  align=right>%d</p></td>\n"
" </tr>\n"
" <tr>\n"
"  <td><p  align=right>Total vertices in scene:</p></td>\n"
"  <td><p  align=right>%d</p></td>\n"
" </tr>\n"
"</table>\n"
"<p class=Bold>&nbsp;</p>\n", *(ptr+1), *(ptr+2), *(ptr+1)+*(ptr+2));
    }

    CountVertices(buf, &minVert, &maxVert);
    if(minVert != 0xffffffff)
        fprintf(fp, "<p class=Bold>Snapshot of shader vertices %d through %d</p>\n", minVert, maxVert);
    else
        fprintf(fp, "<p class=Bold>No Vertices Captured</p>\n");

    fprintf(fp, "<p class=Bold>&nbsp;</p>\n");

    char inregstr[10][64], outregstr[10][64], invalstr[10][64], outvalstr[10][64];
    DWORD incnt, outcnt;
    
    // Find the first vertex
    ptr = FindOpcode(VSS_PROGRAM, buf);
    if(!ptr) 
    {
        ptr = FindOpcode(VSS_EOF, buf);
    }
    else
    {
        fprintf(fp,
"<p class=tiny>Click on vertex to expand.  All values shown in XYZW order.</p>\n"
"<p> &nbsp;</p>\n");
    }
    
    while((*ptr & 0xff000000) != VSS_EOF)
    {
        switch(*ptr & 0xff000000)
        {
        case VSS_STATS:
            ptr += 3;       // skip stats info
            break;

        case VSS_VERTEX:
            ptr++;
            sprintf(vertstr, "Vertex %d", *ptr);
            ptr++;
            valcnt = (*ptr >> 16) & 0xffff;
            idxcnt = (*ptr & 0xffff);
            ptr++;
            for(i=0; i<valcnt; i++)
            {
                vals[i].x = *ptr++;
                vals[i].y = *ptr++;
                vals[i].z = *ptr++;
                vals[i].w = *ptr++;
            }
            idxdwptr = (DWORD *)idxs;
            for(i=0; i<idxcnt; i++)
                *idxdwptr++ = *ptr++;
            
            sprintf(valstr, "%g %g %g %g", *(FLOAT*)&(vals[0].x), 
                                           *(FLOAT*)&(vals[0].y), 
                                           *(FLOAT*)&(vals[0].z), 
                                           *(FLOAT*)&(vals[0].w));
            fprintf(fp, 
"<SPAN id=id%04d onmouseover=\"this.style.cursor='hand'\"" 
"onclick=\"change('<P class=Big><BUTTON>+</BUTTON><B>%s</B> (V0: %s)</P>',"
"'"
"<P class=Big><BUTTON>-</BUTTON><B>%s</B> (V0: %s)</P><TABLE bgcolor=#c0c0c0>", idcnt, vertstr, valstr, vertstr, valstr);

            idxptr = 0;
            for(icnt=0; icnt<g_ProgramLength; icnt++)
            {
                if(Program[icnt].inst.w & 0x2)    // (cin)
                  idx = a0x;
                else
                  idx = 0;
                
                fprintf(fp, "<TR><TD colspan=4><p class=Bold>%s</p></TD></TR>", Program[icnt].buf);
    
                UINT j;
                for(j=0; j<10; j++)
                {
                    strcpy(inregstr[j], "");
                    strcpy(outregstr[j], "");
                    strcpy(invalstr[j], "");
                    strcpy(outvalstr[j], "");
                }
                incnt = outcnt = 0;
                
                VTXIO inputs, outputs;
                GetVertexInputsAndOutputs(&(Program[icnt].inst), &inputs, &outputs);
                
                if(((Program[icnt].inst.y >> 21) & 0xf) == 0xd)  // is this an ARL instruction?
                {
                    ARLType = ((Program[icnt].inst.z >> 26) & 0x3);  // (amx)
                    ARLSwizzle = ((Program[icnt].inst.y >> 6) & 0x3); // (axs)
                    switch(ARLType)
                    {
                    case 1: // temp
                        ARLCount = ((Program[icnt].inst.z >> 28) & 0xf);  // (arr)
                        break;
                    case 2: // vab
                        ARLCount = ((Program[icnt].inst.y >> 9) & 0xf);   // (va)
                        break;
                    case 3: // const
                        ARLCount = ((Program[icnt].inst.y >> 13) & 0xff); // (ca)
                        break;
                    }
                }
                
                // Dump inputs
                // vab
                for(i=0; i<16; i++)
                {
                    if(inputs.regs & (1<<i))
                    {
                        sprintf(inregstr[incnt], "V%d:", i);
                        ARLRet = sprintval(invalstr[incnt++], idxptr++, ARLSwizzle);
                        if((ARLType == 2) && (ARLCount == (DWORD)i))
                            ARLVal = ARLRet;
                    }
                }
            
                // constant
                if(inputs.cnst != 0xffffffff)
                {
                    sprintf(inregstr[incnt], "C%d:", inputs.cnst-96+idx);
                    ARLRet = sprintval(invalstr[incnt++], idxptr++, ARLSwizzle);
                    if((ARLType == 3) && (ARLCount == inputs.cnst))
                        ARLVal = ARLRet;
                }
                
                // temp
                for(i=0; i<16; i++)
                {
                    if(inputs.regs & (1<<(i+16)))
                    {
                        sprintf(inregstr[incnt], "R%d:", i);
                        ARLRet = sprintval(invalstr[incnt++], idxptr++, ARLSwizzle);
                        if((ARLType == 1) && (ARLCount == (DWORD)i))
                            ARLVal = ARLRet;
                    }
                }
                
                // Dump outputs
                // Output Registers
                for(i=0; i<16; i++)
                {
                    if(outputs.outs & (1<<i))
                    {
                        sprintf(outregstr[outcnt], "%s:", kOutNames[i]);
                        ARLRet = sprintval(outvalstr[outcnt++], idxptr++, ARLSwizzle);
                    }
                }
            
                // constant
                if(outputs.cnst != 0xffffffff)
                {
                    sprintf(outregstr[outcnt], "C%d:", inputs.cnst-96);
                    ARLRet = sprintval(outvalstr[outcnt++], idxptr++, ARLSwizzle);
                }
                
                // temp
                for(i=0; i<16; i++)
                {
                    if(outputs.regs & (1<<(i+16)))
                    {
                        sprintf(outregstr[outcnt], "    R%d: ", i);
                        ARLRet = sprintval(outvalstr[outcnt++], idxptr++, ARLSwizzle);
                    }
                }
                
                // Update a0.x if necessary
                if(ARLType != 0)
                {
                    a0x = (INT)(*((FLOAT *)(&(ARLVal))));
                    if((Program[icnt].inst.y >> 8) & 0x1) // (ane)
                        a0x = -a0x;
                }

                // print inputs and outputs
                for(j=0; j<10; j++)
                {
                    if((j>=incnt) && (j>=outcnt)) break;
                    char *inclr, *outclr;
                    if(j>=incnt)
                        inclr = "";
                    else
                        inclr = "class = InColor";
                    if(j>=outcnt)
                        outclr = "";
                    else
                        outclr = "class = OutColor";
                    fprintf(fp,
"<TR>"
"<TD %s><P class=Normal>%s</P></TD>"
"<TD %s><P class=Normal>%s</P></TD>"
"<TD %s><P class=Normal>%s</P></TD>"
"<TD %s><P class=Normal>%s</P></TD>"
"</TR>", outclr, outregstr[j], outclr, outvalstr[j], inclr, inregstr[j], inclr, invalstr[j]);
                }
            }
            fprintf(fp,
"</TABLE>\\"
"<p class=Normal>&nbsp;</p>\\"
"   ',id%04d)\">"
"   <P class=Big><BUTTON>+</BUTTON><B>%s</B> (V0: %s)</P></SPAN>\n", idcnt++, vertstr, valstr);

            break;

        case VSS_PROGRAM:
            a0x = 0;
            DWORD packed[4];
            D3DVsInstruction VsInst;
            g_ProgramLength = ((*ptr++ & 0x00ffffff)-1)/3;
            for(i=0; i<g_ProgramLength; i++)
            {
                Program[i].inst.x = 0;
                Program[i].inst.y = *ptr++;
                Program[i].inst.z = *ptr++;
                Program[i].inst.w = *ptr++;
                packed[0] = 0;
                packed[1] = Program[i].inst.y;
                packed[2] = Program[i].inst.z;
                packed[3] = Program[i].inst.w;
                ConvertPackedDWordsToUCode(packed, &VsInst);
                DisassembleInstruction(&VsInst, Program[i].buf, 1024);
                char *cptr;
                cptr = Program[i].buf;
                while(*cptr != '\0')
                {
                    if((*cptr == 13) || (*cptr == 10))
                        *cptr = ' ';
                    cptr++;
                }

            }
            break;

        default:
            goto error;
        }
    }
    return;

error:
    fprintf(fp, "Error parsing snapshot data\n");
    return;
}


 

