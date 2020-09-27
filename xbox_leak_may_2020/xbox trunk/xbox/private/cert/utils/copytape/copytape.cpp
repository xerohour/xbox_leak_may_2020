/*
 *
 * copytape.cpp
 *
 * Tape copy and labeling tool
 *
 */

#include "precomp.h"

BOOL FMountTape(CTapeDevice *ptap, LPCSTR szName, BOOL fTOD, BOOL fWrite)
{
    HANDLE h;

    if(!szName)
        return FALSE;
    if(0 == strncmp(szName, "\\\\?", 3) || 0 == strncmp(szName, "\\\\.", 3))
        /* Tape device */
        return ptap->FMount(szName, fWrite);
    if(fWrite)
        h = CreateFile(szName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0,
            NULL);
    else
        h = CreateFile(szName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0,
            NULL);
    if(h == INVALID_HANDLE_VALUE)
        return FALSE;
    return ptap->FMount(h, fWrite);
}

int __cdecl main(int cArg, char **rgszArg)
{
    LPCSTR szInputName = NULL;
    LPCSTR szPrinterName;
    CIniFile cifInput;
    CIniBag *pbagInput;
    CVarBag *pbagReg;
    CVarBag *pbagT;
    CLabelPrinter *ppr = NULL;
    CTapeDevice tapSrc;
    CTapeDevice *prgtapDst;
    HKEY hkey;
    int ilyr = -1;
    DWORD dwOff;
    BOOL fRedLabels = FALSE;
    DWORD dwTapeCRC, dwMediaCRC, dwVideoCRC;
    DWORD dwInputCRC;
    LPCSTR szXMID;
    char szSubmission[16];
    DWORD dwSubmission;
    int i;
    DWORD cb;
    DWORD dwBlkSize;
    DWORD dwDataPerPhy;
    BYTE rgb[32768];
    char szName[32];
    LPSTR *rgszTapeNames;

    /* First look for our reg data so we know whether we'll be able to find
     * any printers */
    if(ERROR_SUCCESS == RegOpenKey(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Xbox DVD-X2 DLT Utility", &hkey))
    {
        pbagReg = PbagFromRegKey(hkey);
        RegCloseKey(hkey);
    } else
        pbagReg = NULL;

    while(++rgszArg, --cArg) {
        if(_stricmp(*rgszArg, "-input") == 0) {
            if(!--cArg) {
noarg:
                fprintf(stderr, "error: missing argument for %s\n", *rgszArg);
                return 1;
            }
            szInputName = *++rgszArg;
        } else if(0 == _stricmp(*rgszArg, "-layer")) {
            if(!--cArg)
                goto noarg;
            if(**++rgszArg == '-' || 0 == sscanf(*rgszArg, "%d", &ilyr)) {
                fprintf(stderr, "error: invalid parameter to -layer\n");
                return 1;
            }
        } else if(0 == _stricmp(*rgszArg, "-label")) {
            if(ppr) {
                fprintf(stderr, "error: -label can only be specified once\n");
                return 1;
            }
            /* First make sure we have a printer */
            if(!pbagReg || !(szPrinterName =
                pbagReg->FindSzProp("LabelPrinter")))
            {
                fprintf(stderr, "error: unable to get printer name\n");
                return 1;
            }
            if(!pbagReg->FFindDwProp("PrinterOffset", &dwOff))
                dwOff = 0;            
            ppr = CLabelPrinter::PprCreate(szPrinterName, -(int)dwOff);
            if(!ppr) {
                fprintf(stderr, "error: unable to access printer %s\n",
                    szPrinterName);
                return 1;
            }
            /* Now see if we can find a RED or GREEN */
            if(cArg > 1) {
                if(0 == _stricmp(rgszArg[1], "GREEN")) {
                    ++rgszArg;
                    --cArg;
                    if(!szInputName)
                        szInputName = "OUTPUT.INI";
                } else if(0 == _stricmp(rgszArg[1], "RED")) {
                    ++rgszArg;
                    --cArg;
                    ++fRedLabels;
                    if(!szInputName)
                        szInputName = "XBCERT.INI";
                }
            }
        } else if(**rgszArg == '-') {
            fprintf(stderr, "error: unknown argument %s\n", *rgszArg);
usage:
            fprintf(stderr, "usage: xboxcopytape [-input inifile] [-label [RED | GREEN] -layer [0 | 1] ] source dest...\n");
            return 1;
        } else
            break;
    }

    /* Make sure have all the data we think is necessary */
    if(ppr) {
        if(ilyr < 0) {
            fprintf(stderr, "error: -label requires -layer\n");
            return 1;
        }
        if(!szInputName) {
            fprintf(stderr, "error: -label requires input file name\n");
            return 1;
        }
        /* Make sure we can read our input file */
        if(!cifInput.FReadFile(szInputName, TRUE)) {
            fprintf(stderr, "error: could not process input file %s\n",
                szInputName);
            return 1;
        }

        pbagInput = cifInput.PbagRoot();
        /* Check for greensign output first */
        pbagT = pbagInput->FindProp("RSM");
        if(pbagT) {
            sprintf(szName, "Layer%dCRC", ilyr);
            if(!pbagT->FFindDwProp(szName, &dwTapeCRC) ||
                !pbagT->FFindDwProp("XboxMediaCRC", &dwMediaCRC) ||
                !pbagT->FFindDwProp("VideoCRC", &dwVideoCRC))
            {
badinput:
                fprintf(stderr, "error: unable to process input data\n");
                return 1;
            }
            pbagT = pbagInput->FindProp("Xbox");
            if(!pbagT)
                goto badinput;
            szXMID = pbagT->FindSzProp("XMID");
            if(!szXMID)
                goto badinput;
            dwSubmission = 0;
        } else {
            pbagT = pbagInput->FindProp("Xbox");
            if(!pbagT || !pbagT->FFindDwProp("Submission", &dwSubmission))
                dwSubmission = 0;
            pbagT = pbagInput->FindProp("CRC");
            sprintf(szName, "Layer%dCRC", ilyr);
            if(!pbagT->FFindDwProp(szName, &dwTapeCRC) ||
                    !pbagT->FFindDwProp("XboxMediaCRC", &dwMediaCRC) ||
                    !pbagT->FFindDwProp("VideoCRC", &dwVideoCRC))
                goto badinput;
            pbagT = pbagInput->FindProp("Disk Manufacturing Information");
            if(!pbagT)
                goto badinput;
            szXMID = pbagT->FindSzProp("XMID");
            if(!szXMID)
                goto badinput;
        }
        if(fRedLabels && dwSubmission)
            sprintf(szSubmission, "-R%d", dwSubmission);
        else
            szSubmission[0] = 0;
    }

    if(cArg < 2)
        goto usage;

    /* Mount the source tape */
    if(!FMountTape(&tapSrc, *rgszArg, TRUE, FALSE)) {
        fprintf(stderr, "error: could not mount %s\n", *rgszArg);
        return 1;
    }

    prgtapDst = new CTapeDevice[--cArg];
    rgszTapeNames = ++rgszArg;
    for(i = 0; i < cArg; ++i) {
        if(!FMountTape(&prgtapDst[i], rgszTapeNames[i], TRUE, TRUE)) {
            fprintf(stderr, "error: could not mount %s\n", rgszTapeNames[i]);
            goto fatal;
        }
    }

    /* Now copy all of the files */
    while(tapSrc.FReadFile(szName, &dwBlkSize, &dwDataPerPhy)) {
        for(i = 0; i < cArg; ++i) {
            if(!prgtapDst[i].FWriteFile(szName, dwBlkSize, dwDataPerPhy)) {
writeerr:
                fprintf(stderr, "error: could not write to %s\n",
                    rgszTapeNames[i]);
                goto fatal;
            }
        }

        while(cb = tapSrc.CbRead(sizeof rgb, rgb)) {
            for(i = 0; i < cArg; ++i) {
                if(prgtapDst[i].CbWrite(cb, rgb) != cb)
                    goto writeerr;
            }
        }

        tapSrc.FCloseFile();
        for(i = 0; i < cArg; ++i) {
            if(!prgtapDst[i].FCloseFile())
                goto writeerr;
        }
    }

    if(!tapSrc.FAtEnd()) {
        fprintf(stderr, "error: reading from source tape\n");
        goto fatal;
    }

    /* Grab the tape CRC and unmount the tapes */
    dwInputCRC = tapSrc.DwCheckSum();
    tapSrc.FUnmount();
    for(i = 0; i < cArg; ++i) {
        if(!prgtapDst[i].FUnmount())
            fprintf(stderr, "warning: failed to unmount %d\n", rgszTapeNames[i]);
    }

    printf("Tape checksum is 0x%08X\n", dwInputCRC);
    if(ilyr >= 0 && dwInputCRC != dwTapeCRC) {
        fprintf(stderr, "error: tape checksum (0x%08X) does not match expected value!\n",
            dwInputCRC);
        goto fatal;
    }

    /* Print one label per copy */
    if(ppr) {
        for(i = 0; i < cArg; ++i) {
            if(ppr->FStartLabel()) {
                ppr->PrintLine("XMID: %.*s\n", 8, szXMID);
                ppr->PrintLine("DLT P/N: %.*s%s-L%d\n", 8, szXMID,
                    szSubmission, ilyr);
                ppr->PrintLine("Format: DVD-X2    Layer: %d\n", ilyr);
                ppr->PrintLine("DLT CRC: %08X\n", dwTapeCRC);
                ppr->PrintLine("Xbox Media CRC: %08X\n", dwMediaCRC);
                ppr->PrintLine("DVD Video CRC: %08X\n", dwVideoCRC);
                ppr->EndLabel();
            } else
                fprintf(stderr, "warning: could not print label\n");
        }
    }

    /* Clean up the printer */
    if(ppr)
        delete ppr;

    return 0;
fatal:
    if(ppr)
        delete ppr;
    for(i = 0; i < cArg; ++i)
        prgtapDst[i].FRewind();
    delete[] prgtapDst;
    return 1;
}
