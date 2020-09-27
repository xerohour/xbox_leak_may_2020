/*
**
** xbsetcfg
**
** Set the remote Xbox machine
**
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <xboxdbg.h>

// fake this to get the av.h header to be included.
typedef unsigned long NTSTATUS;
    
#include <av.h>

#include "xbfile.hxx"
#include "xbox.h"
#include "xboxp.h"

int __cdecl main(int cArg, char **rgszArg)
{
    char *szExe = *rgszArg;
    DWORD dwConfigValue = 0;
    DWORD dwPAL60Flags = 0;
    BOOL fSeenV = FALSE;
    DWORD dwTempVal;

    HRESULT hr;

    /* Process arguments */
    ++rgszArg;
    while(--cArg) {
        if (**rgszArg != '-' && **rgszArg != '/')
        {
            goto usage;
        }
        char *szArg = *rgszArg++;
        while(*++szArg) {
            switch(*szArg) {
            case 'x':
            case 'X':
                if(!--cArg)
                    goto usage;
                if(FAILED(DmSetXboxName(*rgszArg++))) {
                    fprintf(stderr, "error in Xbox name\n");
                    exit(1);
                }
                break;

            case 'v':
            case 'V':
            {
                if(!--cArg)
                    goto usage;

                if (dwConfigValue)
                    goto usage;

                fSeenV = TRUE;

                char* pszNextArg = *rgszArg++;

                if (0 == _stricmp(pszNextArg, "NTSC-M"))
                {
                    dwConfigValue = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
                }
                else if (0 == _stricmp(pszNextArg, "NTSC-J"))
                {
                    dwConfigValue = AV_STANDARD_NTSC_J | AV_FLAGS_60Hz;
                }
                else if (0 == _stricmp(pszNextArg, "PAL-I"))
                {
                    dwConfigValue = AV_STANDARD_PAL_I | AV_FLAGS_50Hz;
                }
                else if (0 == _stricmp(pszNextArg, "PAL-M"))
                {
                    dwConfigValue = AV_STANDARD_PAL_M | AV_FLAGS_60Hz;
                }
                else
                {
                    fprintf(stderr, "invalid arg %s: must be NTSC-M, NTSC-J, or PAL-I\n", pszNextArg);
                    exit(1);
                }
                break;
            }

            case 'D':
            {
                char szDir[MAX_PATH + 1];
                char szName[MAX_PATH + 1];
                char szCommand[1024];
                char *pch;
                char *pchArg = *rgszArg;

                if(1 != --cArg)
                    goto usage;
                FIL fil(*rgszArg);
                if(!fil.m_fXbox) {
                    fprintf(stderr, "error: illegal filename %s\n", pchArg);
                    exit(1);
                }
                strcpy(szName, fil.m_szName);
                fil.m_szName[0] = 0;
                fil.FillFullName(szDir);
                fil.m_szName[0] = szName[0];
                sprintf(szCommand, "deftitle dir=\"%s\" name=\"%s\"",
                    szDir, szName);
                hr = DmSendCommand(NULL, szCommand, NULL, NULL);
                if(FAILED(hr))
                    fil.Fatal(hr, 0);
                goto done;
            }

            case 'l':
            case 'L':
                if(0 != --cArg)
                    goto usage;
                hr = DmSendCommand(NULL, "deftitle launcher", NULL, NULL);
                goto done;

            case 'd':
                if(0 != --cArg)
                    goto usage;
                hr = DmSendCommand(NULL, "deftitle none", NULL, NULL);
                goto done;

            case 'W':
            case 'w':
                if (!fSeenV)
                    goto usage;

                dwConfigValue |= AV_FLAGS_WIDESCREEN;
                break;

            case '6':
                if (!fSeenV)
                    goto usage;

                if((dwConfigValue & AV_STANDARD_MASK) != AV_STANDARD_PAL_I) {
                    fprintf(stderr, "error: -6 valid only with PAL-I\n");
                    exit(1);
                }
                dwPAL60Flags = AV_FLAGS_60Hz;
                break;

            case '4':
                if (!fSeenV)
                    goto usage;

                if(((dwConfigValue & AV_STANDARD_MASK) != AV_STANDARD_NTSC_M) &&
                   ((dwConfigValue & AV_STANDARD_MASK) != AV_STANDARD_NTSC_J)) {
                    fprintf(stderr, "error: -4 valid only with NTSC-M or NTSC-J\n");
                    exit(1);
                }
                dwConfigValue |= AV_FLAGS_HDTV_480p;
                break;

            case '7':
                if (!fSeenV)
                    goto usage;

                if(((dwConfigValue & AV_STANDARD_MASK) != AV_STANDARD_NTSC_M) &&
                   ((dwConfigValue & AV_STANDARD_MASK) != AV_STANDARD_NTSC_J)) {
                    fprintf(stderr, "error: -7 valid only with NTSC-M or NTSC-J\n");
                    exit(1);
                }
                dwConfigValue |= AV_FLAGS_HDTV_720p;
                break;

            case '1':
                if (!fSeenV)
                    goto usage;

                if(((dwConfigValue & AV_STANDARD_MASK) != AV_STANDARD_NTSC_M) &&
                   ((dwConfigValue & AV_STANDARD_MASK) != AV_STANDARD_NTSC_J)) {
                    fprintf(stderr, "error: -1 valid only with NTSC-M or NTSC-J\n");
                    exit(1);
                }
                dwConfigValue |= AV_FLAGS_HDTV_1080i;
                break;

            case 'a':
            case 'A':
            {
#define TEMP_STEREO_FLAG 0x00008000
                
                DWORD dwAudioFlags = 0;

                if(1 != --cArg)
                    goto usage;

                char* pszNextArg = *rgszArg++;

                while (*pszNextArg && !isspace(*pszNextArg))
                {
                    switch (toupper(*pszNextArg))
                    {
                        case 'D':
                            if (XC_AUDIO_FLAGS_BASIC(dwAudioFlags) & (XC_AUDIO_FLAGS_MONO | TEMP_STEREO_FLAG))
                            {
                                fprintf(stderr, "invalid arg after -a: only R can be specified with D\n");
                                exit(1);
                            }
                            dwAudioFlags |= (XC_AUDIO_FLAGS_ENABLE_AC3 | XC_AUDIO_FLAGS_SURROUND);
                            break;
                            
                        case 'T':
                            dwAudioFlags |= XC_AUDIO_FLAGS_ENABLE_DTS;
                            break;
                            
                        case 'S':
                            if (dwAudioFlags & XC_AUDIO_FLAGS_ENABLE_AC3)
                            {
                                fprintf(stderr, "invalid arg after -a: only R can be specified with D\n");
                                exit(1);
                            }
                            
                            if (XC_AUDIO_FLAGS_BASIC(dwAudioFlags))
                            {
                                fprintf(stderr, "invalid arg after -a: only one of R, S, or M can be specified\n");
                                exit(1);
                            }

                            //
                            // Treat stereo like a bit flag for now (even though it is really 0)
                            //
                            
                            dwAudioFlags |= TEMP_STEREO_FLAG;
                            break;
                            
                        case 'R':
                            if (XC_AUDIO_FLAGS_BASIC(dwAudioFlags) & ~XC_AUDIO_FLAGS_SURROUND)
                            {
                                fprintf(stderr, "invalid arg after -a: only one of R, S, or M can be specified\n");
                                exit(1);
                            }
                            
                            dwAudioFlags |= XC_AUDIO_FLAGS_SURROUND;
                            break;
                            
                        case 'M':
                            if (dwAudioFlags & XC_AUDIO_FLAGS_ENABLE_AC3)
                            {
                                fprintf(stderr, "invalid arg after -a: only R can be specified with D\n");
                                exit(1);
                            }
                            
                            if (XC_AUDIO_FLAGS_BASIC(dwAudioFlags))
                            {
                                fprintf(stderr, "invalid arg after -a: only one of R, S, or M can be specified\n");
                                exit(1);
                            }

                            dwAudioFlags |= XC_AUDIO_FLAGS_MONO;
                            break;

                        default:
                            fprintf(stderr, "invalid arg after -a: only D, T, R, S, or M can be specified\n");
                            exit(1);
                    }
                    
                    pszNextArg++;
                }

                if (0 == XC_AUDIO_FLAGS_BASIC(dwAudioFlags))
                {
                    fprintf(stderr, "invalid arg after -a: R, S, or M must be specified\n");
                    exit(1);
                }

                //
                // Mask off our fake stereo bit flag (it is really 0)
                //
                
                dwAudioFlags &= ~TEMP_STEREO_FLAG;
                            
                hr = DmSetConfigValue(XC_AUDIO_FLAGS, REG_DWORD, &dwAudioFlags, sizeof(dwAudioFlags));
                goto done;
            }

            case 'n':
            case 'N':
            {
                DWORD dwLanguage = 0;

                if(1 != --cArg)
                    goto usage;

                char* pszNextArg = *rgszArg++;

                // This better be a one character argument, if not, goto usage
                if ((*(pszNextArg + 1) != '\0') &&
                    (!isspace(*(pszNextArg + 1))))
                {
                    fprintf(stderr, "invalid arg after -n: 1, 2, 3, 4, 5, or 6 must be specified\n");
                    exit(1);
                }

                switch (*pszNextArg)
                {
                    case '1':
                        dwLanguage = XC_LANGUAGE_ENGLISH;
                        break;
                        
                    case '2':
                        dwLanguage = XC_LANGUAGE_JAPANESE;
                        break;
                        
                    case '3':
                        dwLanguage = XC_LANGUAGE_GERMAN;
                        break;
                        
                    case '4':
                        dwLanguage = XC_LANGUAGE_FRENCH;
                        break;
                        
                    case '5':
                        dwLanguage = XC_LANGUAGE_SPANISH;
                        break;
                        
                    case '6':
                        dwLanguage = XC_LANGUAGE_ITALIAN;
                        break;
                }
                            

                if (0 == dwLanguage)
                {
                    fprintf(stderr, "invalid arg after -n: 1, 2, 3, 4, 5, or 6 must be specified\n");
                    exit(1);
                }

                hr = DmSetConfigValue(XC_LANGUAGE, REG_DWORD, &dwLanguage, sizeof(dwLanguage));
                goto done;
            }

            case 't':
            case 'T':
            {
                //
                // Set the clock on the xbox
                // also if the time zone or language information is invalid
                // initialize them to their default values
                //
                FILETIME filetime;
                CHAR cmdbuf[64];
                if(0 != --cArg)
                    goto usage;
                GetSystemTimeAsFileTime(&filetime);
                sprintf(cmdbuf, "setsystime clockhi=0x%08x clocklo=0x%08x tz=1",
                    filetime.dwHighDateTime, filetime.dwLowDateTime);
                hr = DmSendCommand(NULL, cmdbuf, NULL, NULL);
                goto done;
            }

            default:
                goto usage;
            }
        }
    }

    if ((cArg < 0)) {
usage:
        fprintf(stderr, "usage: %s [-x xboxname]\n", szExe);
        fprintf(stderr, "\n");
        fprintf(stderr, "To change the video flags:\n");
        fprintf(stderr, "        -v      {NTSC-M|NTSC-J|PAL-I} [-w] [-6] [-4] [-7] [-1]\n");
        fprintf(stderr, "                 -w == enable widescreen support\n");
        fprintf(stderr, "                 -6 == enable PAL-60\n");
        fprintf(stderr, "                 -4 == enable 480p (HDTV only)\n");
        fprintf(stderr, "                 -7 == enable 720p (HDTV only)\n");
        fprintf(stderr, "                 -1 == enable 1080i (HDTV only)\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "To change the audio flags:\n");
        fprintf(stderr, "        -a      {D|T|{R|S|M}}\n");
        fprintf(stderr, "                 D == enable Dolby Digital output\n");
        fprintf(stderr, "                 T == enable DTS output\n");
        fprintf(stderr, "                 R == set 2 channel output to Dolby Surround\n");
        fprintf(stderr, "                 S == set 2 channel output to Stereo (only if DD not enabled)\n");
        fprintf(stderr, "                 M == set 2 channel output to Mono (only if DD not enabled)\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "To change the default language:\n");
        fprintf(stderr, "        -n      {1|2|3|4|5|6}\n");
        fprintf(stderr, "                 1 == English\n");
        fprintf(stderr, "                 2 == Japanese\n");
        fprintf(stderr, "                 3 == German\n");
        fprintf(stderr, "                 4 == French\n");
        fprintf(stderr, "                 5 == Spanish\n");
        fprintf(stderr, "                 6 == Italian\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "To change the default title:\n");
        fprintf(stderr, "        -l      set the default title to be the launcher\n");
        fprintf(stderr, "        -d      set the default title to be the dashboard\n");
        exit(1);
    }

    if(!(dwConfigValue & AV_REFRESH_MASK))
        goto usage;

    dwTempVal = dwPAL60Flags | (dwConfigValue & (AV_HDTV_MODE_MASK | AV_FLAGS_WIDESCREEN));

    hr = DmSetConfigValue(XC_VIDEO_FLAGS, REG_DWORD, &dwTempVal, sizeof(dwTempVal));

    if (SUCCEEDED(hr))
    {
        dwTempVal = (dwConfigValue & (AV_STANDARD_MASK | AV_REFRESH_MASK));
        hr = DmSetConfigValue(XC_FACTORY_AV_REGION, REG_DWORD, &dwTempVal, sizeof(dwTempVal));
    }

done:
    if (FAILED(hr))
    {
        Fatal(hr, 0);
    }

    return 0;
}

