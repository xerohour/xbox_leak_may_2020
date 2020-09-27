#ifdef WIN32
#include <windows.h>
#include <commctrl.h>
#include "win32\reg.h"
#include "win32\resource.h"
#include "win32\winmisc.h"
#else // WIN32
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#endif // WIN32

#include <string.h>
#include <direct.h>
#include "bitmap.h"
#include "bootsec.h"
#include "compress.h"
#include "debug.h"
#include "defs.h"
#include "diskutil.h"
#include "fatutil.h"
#include "file.h"
#include "image.h"
#include "mem.h"
#include "text.h"


// Local prototypes
int GetImage(LPEVERYTHING);
int RecoverImage(LPEVERYTHING);
int EnterMainLoop(LPEVERYTHING);
int NextFragment(LPEVERYTHING);
int OpenFragment(BYTE, LPEVERYTHING);
int CloseFragment(LPEVERYTHING);
int VerifyFileWrite(FILEHANDLE, LPVOID, LPVOID, DWORD, WORD);
int DumpInfo(LPEVERYTHING);

#ifdef WIN32

LPARAM CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static LPEVERYTHING         lpInfo;
    static BOOL                 fDrive, fFile;
    HMENU                       hSysMenu;
    char                        szFile[MAX_PATH];
    LPNM_LISTVIEW               lpnmlv;
    LV_ITEM                     lvi;
    int                         nRet;
    
    switch(uMsg)
    {
        case WM_INITDIALOG:
            lpInfo = (LPEVERYTHING)lParam;
            
            // Set the window title
            SetWindowText(hWnd, APPTITLE);
            
            // Disable 'maximize' and 'size' on the system menu
            hSysMenu = GetSystemMenu(hWnd, FALSE);
            
            EnableMenuItem(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hSysMenu, SC_SIZE, MF_BYCOMMAND | MF_GRAYED);

            // Enable dropping files on the file text box
            EnableDragDrop(GetDlgItem(hWnd, IDC_FILE), TRUE);
            
            // Initialize the fragment size spinner
            SendDlgItemMessage(hWnd, IDC_FRAGSIZE_SPIN, UDM_SETRANGE, 0, MAKELONG(UD_MAXVAL, 0));

            // Initialize control values
            SetDlgItemText(hWnd, IDC_FILE, lpInfo->lpData->lpImage->szFile);

            SendDlgItemMessage(hWnd, IDC_FRAGSIZE_SPIN, UDM_SETPOS, 0, MAKELPARAM(lpInfo->lpData->lpImage->dwMaxFragSize, 0));
            SetDlgItemInt(hWnd, IDC_FRAGSIZE, lpInfo->lpData->lpImage->dwMaxFragSize, FALSE);
            
            SetCheckFlag(hWnd, IDC_OPTION_COMPRESS, OPTIONS_COMPRESS, lpInfo->lpData->lpImage->wFlags);
            SetCheckFlag(hWnd, IDC_OPTION_ALLOCATED, OPTIONS_ALLOCATED, lpInfo->lpData->lpImage->wFlags);
            SetCheckFlag(hWnd, IDC_OPTION_SPAN, OPTIONS_SPAN, lpInfo->lpData->lpImage->wFlags);
            SetCheckFlag(hWnd, IDC_OPTION_FINDBAD, OPTIONS_FINDBAD, lpInfo->lpData->lpImage->wFlags);
            SetCheckFlag(hWnd, IDC_OPTION_VERIFY, OPTIONS_VERIFY, lpInfo->lpData->lpImage->wFlags);
            SetCheckFlag(hWnd, IDC_OPTION_NOSTOPONERROR, OPTIONS_FORCE, lpInfo->lpData->lpImage->wFlags);
    
            // Make the window visible
            GetRegWindowPos(hWnd);
            ShowWindow(hWnd, SW_RESTORE);
            UpdateWindow(hWnd);

            // Initialize the drive list
            InitDriveList(GetDlgItem(hWnd, IDC_DRIVELIST));
            
            return TRUE;
        
        case WM_SIZE:
            // Fixing a bug in Windows' dialog handler
            if(wParam == SIZE_RESTORED || wParam == SIZE_MINIMIZED)
            {
                hSysMenu = GetSystemMenu(hWnd, FALSE);

                EnableMenuItem(hSysMenu, SC_MINIMIZE, MF_BYCOMMAND | (wParam == SIZE_RESTORED) ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem(hSysMenu, SC_RESTORE, MF_BYCOMMAND | (wParam == SIZE_RESTORED) ? MF_GRAYED : MF_ENABLED);
            }

            break;
        
        case WM_DESTROY:
            // Audi
            PutRegWindowPos(hWnd);
            PostQuitMessage(0);

            break;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_FILE_REFRESHDRIVELIST:
                    // Disable the OK button
                    fDrive = FALSE;
                    EnableWindow(GetDlgItem(hWnd, IDOK), FALSE);

                    // Refresh the drive list
                    InitDriveList(GetDlgItem(hWnd, IDC_DRIVELIST));

                    break;

                case ID_FILE_EXIT:
                case IDCANCEL:
                    // Later
                    SendMessage(hWnd, WM_CLOSE, 0, 0);

                    break;

                case ID_HELP_ABOUT:
                    // About me
                    ShellAbout(hWnd, APPTITLE, "Contact DEREKS for questions or comments", LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_IMAGE)));

                    break;

                case IDC_FILE:
                    if(HIWORD(wParam) != EN_CHANGE)
                    {
                        break;
                    }

                    fFile = GetWindowTextLength((HWND)lParam);
                    EnableWindow(GetDlgItem(hWnd, IDOK), fDrive && fFile);

                    break;

                case IDC_FILE_BROWSE:
                    // Select a new image file
                    GetDlgItemText(hWnd, IDC_FILE, szFile, sizeof(szFile));
                    
                    if(BrowseForImageFile(hWnd, szFile, TRUE))
                    {
                        SetDlgItemText(hWnd, IDC_FILE, szFile);
                        SetFocus(GetDlgItem(hWnd, IDC_FILE));
                        SendDlgItemMessage(hWnd, IDC_FILE, EM_SETSEL, 0, -1);
                    }

                    break;

                case IDOK:
                    // Get selected item
                    lvi.iItem = ListView_GetNextItem(GetDlgItem(hWnd, IDC_DRIVELIST), -1, LVNI_ALL | LVNI_SELECTED);

                    if(lvi.iItem == -1)
                    {
                        ImageMessageBox(hWnd, MB_ICONEXCLAMATION, "No item selected in drive list");
                        break;
                    }

                    // Get drive number from item data
                    lvi.iSubItem = 0;
                    lvi.mask = LVIF_PARAM;

                    if(!ListView_GetItem(GetDlgItem(hWnd, IDC_DRIVELIST), &lvi))
                    {
                        ImageMessageBox(hWnd, MB_ICONEXCLAMATION, "Unable to get drive item data");
                        break;
                    }

                    lpInfo->lpData->lpDrive->bDrive = (BYTE)lvi.lParam;

                    // Get filename
                    GetDlgItemText(hWnd, IDC_FILE, lpInfo->lpData->lpImage->szFile, sizeof(lpInfo->lpData->lpImage->szFile));

                    if(!lstrlen(lpInfo->lpData->lpImage->szFile))
                    {
                        ImageMessageBox(hWnd, MB_ICONEXCLAMATION, "No image file name");
                        SetFocus(GetDlgItem(hWnd, IDC_FILE));
                        SendDlgItemMessage(hWnd, IDC_FILE, EM_SETSEL, 0, -1);
                        break;
                    }

                    // Get max fragment size
                    lpInfo->lpData->lpImage->dwMaxFragSize = GetDlgItemInt(hWnd, IDC_FRAGSIZE, NULL, FALSE);

                    // Get options
                    GetCheckFlag(hWnd, IDC_OPTION_COMPRESS, OPTIONS_COMPRESS, &(lpInfo->lpData->lpImage->wFlags));
                    GetCheckFlag(hWnd, IDC_OPTION_ALLOCATED, OPTIONS_ALLOCATED, &(lpInfo->lpData->lpImage->wFlags));
                    GetCheckFlag(hWnd, IDC_OPTION_SPAN, OPTIONS_SPAN, &(lpInfo->lpData->lpImage->wFlags));
                    GetCheckFlag(hWnd, IDC_OPTION_FINDBAD, OPTIONS_FINDBAD, &(lpInfo->lpData->lpImage->wFlags));
                    GetCheckFlag(hWnd, IDC_OPTION_VERIFY, OPTIONS_VERIFY, &(lpInfo->lpData->lpImage->wFlags));
                    GetCheckFlag(hWnd, IDC_OPTION_NOSTOPONERROR, OPTIONS_FORCE, &(lpInfo->lpData->lpImage->wFlags));

                    // Save settings to the registry
                    SetRegOptions(lpInfo->lpData->lpImage);
                    
                    // Go
                    if(nRet = PrepImage(lpInfo))
                    {
                        ErrorMsg(lpInfo->lpData->lpImage, nRet);
                    }

                    break;
            }

            break;

        case WM_NOTIFY:
            switch(wParam)
            {
                case IDC_DRIVELIST:
                    lpnmlv = (LPNM_LISTVIEW)lParam;

                    switch(lpnmlv->hdr.code)
                    {
                        case LVN_ITEMCHANGED:
                            fDrive = lpnmlv->uNewState & LVIS_SELECTED;
                            EnableWindow(GetDlgItem(hWnd, IDOK), fDrive && fFile);
                            
                            break;
                    }

                    break;
            }
            
            break;
    }

    // Allow for default processing
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#endif // WIN32


#ifdef WIN32

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)

#else // WIN32

int main(int argc, char **argv)

#endif // WIN32

{
    EVERYTHING                  data;

#ifdef WIN32

    WNDCLASSEX                  wc;
    HACCEL                      hAccel;
    HWND                        hWnd;
    MSG                         msg;

#else // WIN32

    BOOL                        fRecover    = FALSE;
    BOOL                        fGetComment = FALSE;
    char                        szFile[MAX_PATH];
    char                        szPath[MAX_PATH];
    BYTE                        bDrive;
    int                         i, z;
    int                         nRet;

#endif // WIN32
    
    // Allocate memory
    MemSet(&data, 0, sizeof(data));

    data.lpImage = (LPIMAGESTRUCT)MemAlloc(sizeof(IMAGESTRUCT));
    data.lpDrive = (LPDRIVEINFO)MemAlloc(sizeof(DRIVEINFO));

    if(!data.lpImage || !data.lpDrive)
    {
        return ErrorMsg(NULL, ERROR_OOM);
    }

    MemSet(data.lpImage, 0, sizeof(IMAGESTRUCT));
    MemSet(data.lpDrive, 0, sizeof(DRIVEINFO));

    data.lpImage->lpbReadBuffer = (LPBYTE)MemAlloc(READWRITE_BUFFER_SIZE);
    data.lpImage->lpbWriteBuffer = (LPBYTE)MemAlloc(READWRITE_BUFFER_SIZE);

    if(!data.lpImage->lpbReadBuffer || !data.lpImage->lpbWriteBuffer)
    {
        return ErrorMsg(data.lpImage, ERROR_OOM);
    }

    data.lpImage->fhImage = INVALID_FILE_HANDLE;
    
#ifdef WIN32

    GetRegOptions(&(data.image));

#endif // WIN32
    
    // Init disk util
    InitDiskUtil();

#ifdef WIN32

    // Init common controls
    InitCommonControls();

    // Register the window class
    wc.cbSize = sizeof(wc);

    GetClassInfoEx(NULL, WC_DIALOG, &wc);

    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_IMAGE));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "RawRead:Main";
    wc.hIconSm = LoadImage(hInst, MAKEINTRESOURCE(IDI_IMAGE), IMAGE_ICON, 16, 16, 0);

    RegisterClassEx(&wc);

    // Load the accelerator table
    hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR));
    
    // Create the main window
    data.lpImage->hWnd = hWnd = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_RAWREAD), NULL, wc.lpfnWndProc, (LPARAM)&data);

    // Enter message loop
    while(GetMessage(&msg, NULL, 0, 0))
    {
        if(!TranslateAccelerator(hWnd, hAccel, &msg) && !IsDialogMessage(hWnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    nRet = msg.wParam;

#else // WIN32

    // Whoo-hoo!  Look at me!
    GoPropaganda();

    // Parse for switches
    for(i = 1; i < argc; i++)
    {
        if(argv[i][0] == '/' || argv[i][0] == '-' || fGetComment)
        {
            if(fGetComment)
            {
                strncpy(data.lpImage->ls.szComment, argv[i], sizeof(data.lpImage->ls.szComment));
                fGetComment = FALSE;
            }
            else
            {
                for(z = 1; z < StrLen(argv[i]); z++)
                {
                    switch(toupper(argv[i][z]))
                    {
                        case SWITCH_HELP:
                            return GoRawReadHelp();
                               
                        case SWITCH_NOPROMPT:
                            data.lpImage->wFlags |= OPTIONS_NOPROMPT;
                            break;
                            
                        case SWITCH_SPAN:
                            data.lpImage->wFlags |= OPTIONS_SPAN;
                            break;
                            
                        case SWITCH_NOLOCK:
                            data.lpImage->wFlags |= OPTIONS_NOLOCK;
                            break;
                            
                        case SWITCH_OVERWRITE:
                            data.lpImage->wFlags |= OPTIONS_OVERWRITE;
                            break;
                            
                        case SWITCH_NORECOVER:
                            data.lpImage->wFlags |= OPTIONS_NORECOVER;
                            break;
                            
                        case SWITCH_COMPRESS:
                            data.lpImage->wFlags |= OPTIONS_COMPRESS;
                            break;
                            
                        case SWITCH_ALLOCATED:
                            data.lpImage->wFlags |= OPTIONS_ALLOCATED;
                            break;
                            
                        case SWITCH_FINDBAD:
                            data.lpImage->wFlags |= OPTIONS_FINDBAD;
                            break;
                            
                        case SWITCH_NOSTATUS:
                            data.lpImage->wFlags |= OPTIONS_NOSTATUS;
                            break;
                            
                        case SWITCH_VERIFY:
                            data.lpImage->wFlags |= OPTIONS_VERIFY;
                            break;
                               
                        case SWITCH_FORCE:
                            data.lpImage->wFlags |= OPTIONS_FORCE;
                            break;
                                
                        case SWITCH_DUMPINFO:
                            data.lpImage->wFlags |= OPTIONS_DUMPINFO;
                            break;
                               
                        case SWITCH_TEXTCOMMENT:
                            fGetComment = TRUE;
                            break;
                            
                        default:
                            printf("Erroneous switch:  %c" CRLF CRLF, argv[i][z]);
                            return GoRawReadHelp();
                    }
                }
            }
            
            for(z = i; z < argc - 1; z++)
            {
                argv[z] = argv[z + 1];
            }
            
            argc--;
            i--;
        }
    }
    
    // Did we get the right number of arguments?
    if(argc < 2 || argc > 4)
    {
        return GoRawReadHelp();
    }

    // First argument should be a drive letter followed by a colon
    if(toupper(argv[1][0]) < 'A' || toupper(argv[1][0]) > 'Z' || argv[1][1] != ':')
    {
        return GoRawReadHelp();
    }
    
    bDrive = data.lpDrive->bDrive = toupper(argv[1][0]) - 'A';

    // If the drive is compressed, grab it's host
    GetDriveMapping(bDrive, &(data.lpDrive->bDrive), NULL);
    
    // Get drive info
    if(nRet = GetDriveInfo(data.lpDrive->bDrive, data.lpDrive))
    {
        return ErrorMsg(NULL, nRet);
    }

    // Second argument should be a filename.  If this parameter is omitted, use
    // the source drive's volume label.
    if(argc >= 3)
    {
        StrCpy(szFile, argv[2]);
    }
    else
    {
        StrCpy(szFile, data.lpDrive->bs.szVolumeLabel);
    }

    // Long file names are not supported
    if(nRet = ValidateFileName(szFile))
    {
        return ErrorMsg(NULL, nRet);
    }

    // Generate the default fragment names
    GetDefaultFragNames(szFile, data.lpImage, 0, MAX_IMAGE_FRAGMENTS);

    // Can't put the image file on the image drive
    if(toupper(data.lpImage->aszFragNames[0][0]) == bDrive + 'A' || toupper(data.lpImage->aszFragNames[0][0]) == bDrive + 'A')
    {
        return ErrorMsg(NULL, ERROR_FILEOPEN);
    }
    
    // Set the current drive and directory to coincide with the first image
    // fragment.  This is an easy way to use that directory as the default
    // when getting the other fragment names.
    GetFileLocation(data.lpImage->aszFragNames[0], szPath, NULL);
    _chdrive(toupper(szPath[0]) - 'A' + 1);
    _chdir(szPath);

    // Third argument should be a max image size.  0 indicates unlimited size.
    // Default to 0 if no argument
    if(argc >= 4)
    {
        for(i = 0; i < StrLen(argv[3]); i++)
        {
            if(!isdigit(argv[3][i]))
            {
                return GoRawReadHelp();
            }
        }
        
        data.lpImage->dwMaxFragSize = (DWORD)atol(argv[3]);
    }
    
    // Does the image already exist?
    if(!(data.lpImage->wFlags & OPTIONS_DUMPINFO))
    {
        if((data.lpImage->fhImage = FileOpen(data.lpImage->aszFragNames[0], OPEN_ACCESS_READWRITE, 0, ACTION_FILE_OPEN)) != INVALID_FILE_HANDLE)
        {
            if(!(data.lpImage->wFlags & OPTIONS_NORECOVER) && CanRecoverImage(data.lpImage->fhImage, data.lpDrive->bDrive))
            {
                fRecover = AskUser(data.lpImage, "%s appears to be an aborted image." CRLF "Would you like to attempt to recover it?", data.lpImage->aszFragNames[0]);
            }
    
            if(!fRecover)
            {
                FileClose(data.lpImage->fhImage);
                data.lpImage->fhImage = INVALID_FILE_HANDLE;
            }
        }
    }
        
    // Attempt to lock the drive
    if(!(data.lpImage->wFlags & OPTIONS_NOLOCK))
    {
        if(!LockVolume(data.lpDrive->bDrive) && !(data.lpImage->wFlags & OPTIONS_FORCE))
        {
            if(!AskUser(data.lpImage, "Unable to lock drive %c:.  Continue anyway?", data.lpDrive->bDrive + 'A'))
            {
                return ErrorMsg(NULL, ERROR_LOCKFAIL);
            }
        }
    }
    
    if(data.lpImage->wFlags & OPTIONS_DUMPINFO)
    {
        nRet = DumpInfo(&data);
    }
    else
    {

#ifdef WIN32

        // Create the progress dialog
        data.lpImage->hWnd = CreateProgressDialog(data.lpImage->hWnd);
    
#endif // WIN32
    
        // Start imaging
        if(fRecover)
        {
            nRet = RecoverImage(&data);
        }
        else
        {
            nRet = GetImage(&data);
        }

#ifdef WIN32

        // Close the progress dialog
        data.lpImage->hWnd = CloseProgressDialog(data.lpImage->hWnd);

#endif // WIN32

    }

#endif // WIN32

    // Unlock the drive
    if(!(data.lpImage->wFlags & OPTIONS_NOLOCK))
    {
        UnlockVolume(data.lpDrive->bDrive);
    }

    // Release disk util
    FreeDiskUtil();

    // Display any errors
    if(nRet != ERROR_NONE)
    {
        ErrorMsg(data.lpImage, nRet);
    }
    
    // Free memory
    MemFree(data.lpImage->lpbReadBuffer);
    MemFree(data.lpImage->lpbWriteBuffer);

    MemFree(data.lpImage);
    MemFree(data.lpDrive);

    // Return

#ifndef WIN32

    printf(CRLF);

#endif // WIN32

    return nRet;
}


int GetImage(LPEVERYTHING lpData)
{
    DWORD                       dwFragCount;
    DWORD                       dwCurrentSector;
    DWORD                       dwRead;
    int                         nRet;

    // Calculate the total number of clusters in the image.
    lpData->lpImage->ls.dwClusterCount = lpData->lpDrive->bs.dwTotalClusters;
    
    // Copy the options to the LEADSECTOR
    lpData->lpImage->ls.wFlags = lpData->lpImage->wFlags;
   
    // Scan for bad clusters
    if(nRet = FindBadClusters(lpData, 2, lpData->lpDrive->bs.dwTotalClusters, &(lpData->lpDrive->adwBadClusters), &(lpData->lpDrive->dwBadClusters), FALSE))
    {
        return nRet;
    }
    
    // Scan for unallocated clusters
    if(nRet = FindUnallocatedClusters(lpData, 2, lpData->lpDrive->bs.dwTotalClusters, NULL, &(lpData->lpDrive->dwUnusedClusters), FALSE))
    {
        return nRet;
    }
    
    // Calculate the maximum number of clusters allowable in each image 
    // fragment.  If we're not fragmenting the image, set the number of 
    // clusters per fragment to the total number of clusters on the drive so 
    // that we can do safe math based on this number.  The number of clusters 
    // per fragment is really a loose calculation based on only the number of 
    // clusters that will fit into the maximum fragment size.  This number 
    // does not take into account the image header, bitmaps or pre-data 
    // sectors.  But, because we're dealing in mb, it's probably not a big 
    // deal if we're off by a couple here and there.

    // BUGBUG: this should really be fixed

    if(lpData->lpImage->dwMaxFragSize)
    {
        lpData->lpImage->ls.dwClustersPerFrag = (lpData->lpImage->dwMaxFragSize * 1024 * 1024) / lpData->lpDrive->bs.dwSectorsPerCluster / lpData->lpDrive->bs.dwSectorSize;
    }
    else
    {
        lpData->lpImage->ls.dwClustersPerFrag = lpData->lpImage->ls.dwClusterCount;
    }

    // Calculate the number of fragments this operation will require
    dwFragCount = max(1, DIVUP(lpData->lpImage->ls.dwClusterCount - lpData->lpDrive->dwUnusedClusters, lpData->lpImage->ls.dwClustersPerFrag));
        
    if(dwFragCount > MAX_IMAGE_FRAGMENTS)
    {
        return ERROR_TOOMANYFRAGMENTS;
    }

    lpData->lpImage->ls.bFragCount = (BYTE)dwFragCount;
        
    // Get the image fragment filenames
    if(!(lpData->lpImage->wFlags & OPTIONS_NOPROMPT))
    {
        if(nRet = GetFragNames(lpData->lpImage, 1, lpData->lpImage->ls.bFragCount - 1))
        {
            return nRet;
        }
    }

    // Open the first fragment
    if(nRet = OpenFragment(0, lpData))
    {
        return nRet;
    }
    
    // Write the metadata area
    dwCurrentSector = 0;

#ifdef WIN32

    SetProgressLabel(lpData->lpImage->hWnd, "Preprocessing %lu sectors...", lpData->lpDrive->bs.dwPreDataSectors);

#else // WIN32

    printf("Preprocessing %lu sectors... ", lpData->lpDrive->bs.dwPreDataSectors);

    if(!(lpData->lpImage->wFlags & OPTIONS_NOSTATUS))
    {
        printf(SZCLUSTER, dwCurrentSector);
    }

#endif // WIN32
    
    if(nRet = VerifyFileWrite(lpData->lpImage->fhImage, &(lpData->lpDrive->bs.bsOriginal), lpData->lpImage->lpbReadBuffer, sizeof(lpData->lpDrive->bs.bsOriginal), lpData->lpImage->wFlags))
    {
        return nRet;
    }
    
    dwCurrentSector++;

    while(dwCurrentSector < lpData->lpDrive->bs.dwPreDataSectors)
    {

#ifndef WIN32

        if(!(lpData->lpImage->wFlags & OPTIONS_NOSTATUS))
        {
            printf(SZBACK SZCLUSTER, dwCurrentSector);
        }

#endif // WIN32
    
        dwRead = READWRITE_BUFFER_SIZE / lpData->lpDrive->bs.dwSectorSize;

        if(dwCurrentSector + dwRead > lpData->lpDrive->bs.dwPreDataSectors)
        {
            dwRead = lpData->lpDrive->bs.dwPreDataSectors - dwCurrentSector;
        }

        if(!ReadSectorEx(lpData->lpDrive->bDrive, lpData->lpImage->lpbWriteBuffer, dwCurrentSector, dwRead, lpData->lpDrive->bs.dwSectorSize))
        {
            return ERROR_DRIVEREAD;
        }
        
        if(nRet = VerifyFileWrite(lpData->lpImage->fhImage, lpData->lpImage->lpbWriteBuffer, lpData->lpImage->lpbReadBuffer, dwRead * lpData->lpDrive->bs.dwSectorSize, lpData->lpImage->wFlags))
        {
            return nRet;
        }

        dwCurrentSector += dwRead;
    }
    
#ifndef WIN32

    if(!(lpData->lpImage->wFlags & OPTIONS_NOSTATUS))
    {
        printf(SZBACK);
    }

    printf(SZDONE CRLF);

#endif // WIN32
    
    // Enter the main loop
    return EnterMainLoop(lpData);
}


int RecoverImage(LPEVERYTHING lpData)
{
    DWORD                       dwFirstData;
    BOOL                        fSet;
    WORD                        wSize;
    DWORD                       dwRead;
    int                         nRet;
    char                        szFile[MAX_PATH];
    
    // Read the image header
    if(nRet = ReadImageHeader(lpData->lpImage->fhImage, &(lpData->lpImage->ls), &dwFirstData, FALSE))
    {
        return nRet;
    }
    
    // Override (some of) the command-line options
    if(lpData->lpImage->ls.wFlags & OPTIONS_ALLOCATED)
    {
        lpData->lpImage->wFlags |= OPTIONS_ALLOCATED;
    }
    else
    {
        lpData->lpImage->wFlags &= ~OPTIONS_ALLOCATED;
    }
    
    if(lpData->lpImage->ls.wFlags & OPTIONS_COMPRESS)
    {
        lpData->lpImage->wFlags |= OPTIONS_COMPRESS;
    }
    else
    {
        lpData->lpImage->wFlags &= ~OPTIONS_COMPRESS;
    }
    
    // Scan for bad clusters
    if(nRet = FindBadClusters(lpData, 2, lpData->lpDrive->bs.dwTotalClusters, &(lpData->lpDrive->adwBadClusters), &(lpData->lpDrive->dwBadClusters), FALSE))
    {
        return nRet;
    }
    
    // Scan for unallocated clusters
    if(nRet = FindUnallocatedClusters(lpData, 2, lpData->lpDrive->bs.dwTotalClusters, NULL, &(lpData->lpDrive->dwUnusedClusters), FALSE))
    {
        return nRet;
    }
    
    // Find the fragment we left off with

#ifdef WIN32

    SetProgressLabel(lpData->lpImage->hWnd, "Finding incomplete fragment...");

#else // WIN32

    printf("Finding incomplete fragment... ");

#endif // WIN32

    while(lpData->lpImage->ls.fCompleted && lpData->lpImage->ls.bSequence + 1 < lpData->lpImage->ls.bFragCount)
    {
        // Open the next fragment
        if(lpData->lpImage->fhImage != INVALID_FILE_HANDLE)
        {
            FileClose(lpData->lpImage->fhImage);
            lpData->lpImage->fhImage = INVALID_FILE_HANDLE;
        }

        lpData->lpImage->dwCurrentCluster = lpData->lpImage->ls.dwLastCluster + 1;
        lpData->lpImage->ls.bSequence++;
        
        SetFilePath(lpData->lpImage->ls.szNextFrag, lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence - 1], lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence]);
        
        while(TRUE)
        {
            if((lpData->lpImage->fhImage = FileOpen(lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence], OPEN_ACCESS_READONLY, 0, ACTION_FILE_OPEN)) != INVALID_FILE_HANDLE)
            {
                break;
            }

            if(!ReadImageHeader(lpData->lpImage->fhImage, &(lpData->lpImage->ls), &dwFirstData, FALSE))
            {
                break;
            }
            
            if(lpData->lpImage->fhImage != INVALID_FILE_HANDLE)
            {
                FileClose(lpData->lpImage->fhImage);
                lpData->lpImage->fhImage = INVALID_FILE_HANDLE;
            }

#ifdef WIN32

            ImageMessageBox(lpData->lpImage->hWnd, MB_ICONEXCLAMATION, "Unable to open %s", lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence]);
            StrCpy(szFile, lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence]);
            
            if(!GetFragmentName(lpData->lpImage->hWnd, lpData->lpImage->ls.bSequence + 1, lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence]))
            {
                return ERROR_FILEOPEN;
            }

            // The user won't have changed the path if this is correct
            if(!StrCmp(szFile, lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence]))
            {
                break;
            }

#else // WIN32

            printf(CRLF CRLF "Unable to open %s" CRLF, lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence]);
            printf("Please enter the correct path, or press <ENTER>" CRLF "to begin the recovery with this file:  ");
            
            gets(szFile);

#ifndef WIN32

            printf(CRLF "Finding incomplete fragment... ");

#endif // WIN32
            
            if(!StrLen(szFile))
            {
                break;
            }

            GetFullPath(lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence], szFile, MAX_PATH);
            
#endif // WIN32

        }
        
        if(lpData->lpImage->fhImage == INVALID_FILE_HANDLE)
        {
            break;
        }
    }

#ifndef WIN32

    printf(SZDONE CRLF);

#endif // WIN32
    
    // Did we find an incomplete fragment?
    if(lpData->lpImage->ls.bSequence >= lpData->lpImage->ls.bFragCount)
    {
        DPF("%lu %lu", lpData->lpImage->ls.bSequence, lpData->lpImage->ls.bFragCount);
        return ERROR_STUPIDRECOVERY;
    }
    
    // Remove the 'completed' flag from the image header
    lpData->lpImage->ls.fCompleted = FALSE;

    // Do we have a valid image?
    if(lpData->lpImage->fhImage == INVALID_FILE_HANDLE || (DWORD)FileSetPos(lpData->lpImage->fhImage, dwFirstData, SEEK_SET) != dwFirstData)
    {
        // If we're still looking at the first fragment, recovery failed.
        if(lpData->lpImage->ls.bSequence == 0)
        {
            return ERROR_RECOVERYFAILED;
        }
        
        // Get the remaining fragment names
        if(!(lpData->lpImage->wFlags & OPTIONS_NOPROMPT))
        {
            if(nRet = GetFragNames(lpData->lpImage, lpData->lpImage->ls.bSequence + 1, lpData->lpImage->ls.bFragCount - (lpData->lpImage->ls.bSequence + 1)))
            {
                return nRet;
            }
        }
        
        // Open the fragment
        if(lpData->lpImage->fhImage != INVALID_FILE_HANDLE)
        {
            FileClose(lpData->lpImage->fhImage);
            lpData->lpImage->fhImage = INVALID_FILE_HANDLE;
        }
        
        if(nRet = OpenFragment(lpData->lpImage->ls.bSequence, lpData))
        {
            return nRet;
        }
        
        // Go
        return EnterMainLoop(lpData);
    }
    
    // Get the remaining fragment names
    if(!(lpData->lpImage->wFlags & OPTIONS_NOPROMPT))
    {
        if(nRet = GetFragNames(lpData->lpImage, lpData->lpImage->ls.bSequence + 1, lpData->lpImage->ls.bFragCount - (lpData->lpImage->ls.bSequence + 1)))
        {
            return nRet;
        }
    }
    
    // Re-open the fragment with write priveliges
    if(lpData->lpImage->fhImage != INVALID_FILE_HANDLE)
    {
        FileClose(lpData->lpImage->fhImage);
        lpData->lpImage->fhImage = INVALID_FILE_HANDLE;
    }
                            
    if((lpData->lpImage->fhImage = FileOpen(lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence], OPEN_ACCESS_READWRITE, 0, ACTION_FILE_OPEN)) == INVALID_FILE_HANDLE)
    {
        return ERROR_FILEOPEN;
    }
    
#ifdef WIN32

    SetProgressLabel(lpData->lpImage->hWnd, "Verifying fragment integrity...");

#else // WIN32

    printf("Verifying fragment integrity... ");

#endif // WIN32

    // Update the image header
    if(nRet = WriteImageHeader(lpData->lpImage->fhImage, &(lpData->lpImage->ls), &dwFirstData))
    {
        return nRet;
    }
    
    // We need to find which cluster we left off with.  Initialize
    // the bitmaps to be sure we read the correct number of bytes.
    if(lpData->lpImage->ls.dwCompBitmapSize > 0)
    {
        if(nRet = InitBitmap(&(lpData->lpImage->ibmpCompression), lpData->lpImage->ls.dwCompBitmapSize, lpData->lpImage->fhImage, 2, TRUE))
        {
            return nRet;
        }
    }
    
    if(lpData->lpImage->ls.dwAllocBitmapSize > 0)
    {
        if(nRet = InitBitmap(&(lpData->lpImage->ibmpAllocation), lpData->lpImage->ls.dwAllocBitmapSize, lpData->lpImage->fhImage, 2 + lpData->lpImage->ls.dwCompBitmapSize, TRUE))
        {
            return nRet;
        }
    }
    
    // Seek past the image header
    FileSetPos(lpData->lpImage->fhImage, dwFirstData, SEEK_SET);
    
    // Seek past the pre data area
    if(lpData->lpImage->ls.bSequence == 0)
    {
        FileSetPos(lpData->lpImage->fhImage, lpData->lpDrive->bs.dwPreDataSectors * lpData->lpDrive->bs.dwSectorSize, SEEK_CUR);
    }
    
    while(lpData->lpImage->dwCurrentCluster <= lpData->lpImage->ls.dwLastCluster)
    {
        // How big is this cluster?
        wSize = (WORD)(lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize);

        if(lpData->lpImage->ls.dwAllocBitmapSize > 0)
        {
            if(nRet = GetBitmapBit(&(lpData->lpImage->ibmpAllocation), lpData->lpImage->dwCurrentCluster - lpData->lpImage->ls.dwFirstCluster, &fSet))
            {
                return nRet;
            }
        }
        else
        {
            fSet = FALSE;
        }
            
        if(fSet)
        {
            // Cluster isn't really in the image
            wSize = 0;
        }
        else
        {
            if(lpData->lpImage->ls.dwCompBitmapSize > 0)
            {
                if(nRet = GetBitmapBit(&(lpData->lpImage->ibmpCompression), lpData->lpImage->dwCurrentCluster - lpData->lpImage->ls.dwFirstCluster, &fSet))
                {
                    return nRet;
                }
            }
            else
            {
                fSet = FALSE;
            }                
                    
            if(fSet)
            {
                // The size of the compressed cluster is stored in the first 2
                // bytes of the data.
                if((dwRead = FileRead(lpData->lpImage->fhImage, &wSize, sizeof(WORD))) != sizeof(WORD))
                {
                    // Back up
                    FileSetPos(lpData->lpImage->fhImage, 0 - dwRead, SEEK_CUR);
                 
                    // This is the cluster we died on

#ifndef WIN32

                    printf(SZDONE CRLF);

#endif // WIN32

                    return EnterMainLoop(lpData);
                }
            }
        }
        
        // Can we read the cluster?
        if(wSize > 0 && (dwRead = FileRead(lpData->lpImage->fhImage, lpData->lpImage->lpbReadBuffer, wSize)) != wSize)
        {
            // Back up
           FileSetPos(lpData->lpImage->fhImage, 0 - dwRead, SEEK_CUR);
                 
            // This is the cluster we died on

#ifndef WIN32

            printf(SZDONE CRLF);

#endif // WIN32

            return EnterMainLoop(lpData);
        }
        
        // Increment the current cluster
        lpData->lpImage->dwCurrentCluster++;
    }
                                            
    // We couldn't find the cluster we left off with.  This fragment must have actually been
    // completed.
    FileClose(lpData->lpImage->fhImage);
    lpData->lpImage->fhImage = INVALID_FILE_HANDLE;
    
    lpData->lpImage->ls.bSequence++;
    
    if(lpData->lpImage->ls.bSequence >= lpData->lpImage->ls.bFragCount)
    {
        DPF("(2) %lu %lu", lpData->lpImage->ls.bSequence, lpData->lpImage->ls.bFragCount);
        return ERROR_STUPIDRECOVERY;
    }
    
    // Open the fragment and go for it

#ifndef WIN32

    printf(SZDONE CRLF);

#endif // WIN32

    if(nRet = OpenFragment(lpData->lpImage->ls.bSequence, lpData))
    {
        return nRet;
    }
    
    return EnterMainLoop(lpData);
}


int EnterMainLoop(LPEVERYTHING lpData)
{
    DWORD                       dwCluRead;
    DWORD                       dwWrite;
    DWORD                       dwBadCluster;
    DWORD                       dwCheck;
    WORD                        wCompressed;
    WORD                        wUncompressed;
    BOOL                        fSet;
    LPBYTE                      lpb;
    DWORD                       dwCurrentSector;
    UINT                        nNextBad;
    int                         nRet;
    
    // Position the next bad cluster in the list
    for(nNextBad = 0; nNextBad < lpData->lpDrive->dwBadClusters; nNextBad++)
    {
        if(lpData->lpDrive->adwBadClusters[nNextBad] >= lpData->lpImage->dwCurrentCluster)
        {
            break;
        }
    }

    // Here we go...

#ifdef WIN32

    SetProgressLabel(lpData->lpImage->hWnd, "Processing clusters %lu through %lu...", 2, lpData->lpImage->ls.dwClusterCount + 2);

#else // WIN32

    printf("Processing clusters %lu through %lu... ", lpData->lpImage->dwCurrentCluster + 2, lpData->lpImage->ls.dwLastCluster + 2);

    if(!(lpData->lpImage->wFlags & OPTIONS_NOSTATUS))
    {
        printf(SZCLUSTER, lpData->lpImage->dwCurrentCluster + 2);
    }

#endif // WIN32

    while(TRUE)
    {

#ifdef WIN32

        // Process all pending messages
        ProcessMessages(lpData->lpImage->hWnd);

        if(!IsWindow(lpData->lpImage->hWnd))
        {
            // Time to die
            return ERROR_USERCANCEL;
        }

#endif // WIN32
            
        // Update the progress meter
        if(!(lpData->lpImage->wFlags & OPTIONS_NOSTATUS))
        {

#ifdef WIN32

            SetProgressPos(lpData->lpImage->hWnd, lpData->lpImage->dwCurrentCluster + 2, lpData->lpImage->ls.dwClusterCount + 2);

#else // WIN32

            printf(SZBACK SZCLUSTER, lpData->lpImage->dwCurrentCluster + 2);

#endif // WIN32

        }

        // Skip ahead to the next allocated cluster.  While we're at it,
        // check to see if it's time to move on to a new fragment.
        while(lpData->lpImage->dwCurrentCluster < lpData->lpImage->ls.dwClusterCount)
        {
            if(nRet = NextFragment(lpData))
            {
                return nRet;
            }

            if(!lpData->lpImage->ls.dwAllocBitmapSize)
            {
                break;
            }

            if(nRet = GetBitmapBit(&(lpData->lpImage->ibmpAllocation), lpData->lpImage->dwCurrentCluster - lpData->lpImage->ls.dwFirstCluster, &fSet))
            {
                return nRet;
            }

            if(!fSet)
            {
                break;
            }

            DPF("Cluster %lu is unused", lpData->lpImage->dwCurrentCluster);

            lpData->lpImage->dwCurrentCluster++;
        }

        // This is done for UI niceness.  It would be cleaner to change
        // the above while() to check for this.
        if(lpData->lpImage->dwCurrentCluster >= lpData->lpImage->ls.dwClusterCount)
        {
            break;
        }

        // Determine the number of clusters to read
        dwCluRead = READWRITE_BUFFER_SIZE / (lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize);
        
        // Make sure we don't read past the end of this fragment
        dwCluRead = min(dwCluRead, lpData->lpImage->ls.dwLastCluster + 1 - lpData->lpImage->dwCurrentCluster);
        
        // If there's any bad clusters within the next read buffer, stop just before them
        if(nNextBad < lpData->lpDrive->dwBadClusters)
        {
            dwCluRead = min(dwCluRead, lpData->lpDrive->adwBadClusters[nNextBad] - lpData->lpImage->dwCurrentCluster);
        }
        
        // If there's any unused clusters within the next read buffer, stop just before them
        if(lpData->lpImage->ls.dwAllocBitmapSize)
        {
            dwCheck = 0;
            
            while(dwCheck < dwCluRead)
            {
                if(nRet = GetBitmapBit(&(lpData->lpImage->ibmpAllocation), lpData->lpImage->dwCurrentCluster - lpData->lpImage->ls.dwFirstCluster + dwCheck, &fSet))
                {
                    return nRet;
                }
                
                if(fSet)
                {
                    break;
                }
                
                dwCheck++;
            }
            
            dwCluRead = min(dwCluRead, dwCheck);
        }
        
        // Undocumented switch to force 1 cluster at a time
        if(lpData->lpImage->wFlags & OPTIONS_1CLUSTER)
        {
            dwCluRead = min(dwCluRead, 1);
        }
        
        // Calculate the current sector
        dwCurrentSector = ClusterToSector(&(lpData->lpDrive->bs), lpData->lpImage->dwCurrentCluster + 2);
    
        // Read the cluster(s) from the drive
        if(dwCluRead)
        {
            if(!ReadSectorEx(lpData->lpDrive->bDrive, lpData->lpImage->lpbReadBuffer, dwCurrentSector, dwCluRead * lpData->lpDrive->bs.dwSectorsPerCluster, lpData->lpDrive->bs.dwSectorSize))
            {
                // Uh-oh.  We couldn't read from one of the clusters.  Break the loop
                // down to 1 cluster at a time and determine which cluster it was.
                for(dwBadCluster = 0; dwBadCluster < dwCluRead; dwBadCluster++)
                {
                    lpb = lpData->lpImage->lpbReadBuffer + (dwBadCluster * lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize);
                    
                    if(!ReadSector(lpData->lpDrive->bDrive, lpb, dwCurrentSector + (dwBadCluster * lpData->lpDrive->bs.dwSectorsPerCluster), 1))
                    {

#ifdef WIN32

                        if(!(lpData->lpImage->wFlags & OPTIONS_FORCE))
                        {
                            ImageMessageBox(lpData->lpImage->hWnd, MB_ICONEXCLAMATION, "Unable to read from cluster %lu.... ", lpData->lpImage->dwCurrentCluster + 2 + dwBadCluster);
                        }

#else // WIN32

                        printf(CRLF "Unable to read from cluster %lu.  ", lpData->lpImage->dwCurrentCluster + 2 + dwBadCluster);
                        
                        if(lpData->lpImage->wFlags & OPTIONS_FORCE)
                        {
                            printf("Continuing... ");
                        }
                        else
                        {
                            printf(CRLF);
                        }
                        
#endif // WIN32
                        
                        if(!(lpData->lpImage->wFlags & OPTIONS_FORCE))
                        {
                            return ERROR_DRIVEREAD;
                        }
                        
#ifndef WIN32

                        if(!(lpData->lpImage->wFlags & OPTIONS_NOSTATUS))
                        {
                            printf(SZCLUSTER, lpData->lpImage->dwCurrentCluster + dwBadCluster + 2 + 1);
                        }

#endif // WIN32

                        MemSet(lpb, 0xF6, (WORD)(lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize));
                    }
                }
            }
        }
        else
        {
            // We didn't read any clusters.  Was it because of a bad cluster?  If so,
            // fill inthe cluster with 0xF6 and write it to the image.
            if(nNextBad < lpData->lpDrive->dwBadClusters && lpData->lpDrive->adwBadClusters[nNextBad] == lpData->lpImage->dwCurrentCluster)
            {
                MemSet(lpData->lpImage->lpbReadBuffer, 0xF6, (WORD)(lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize));
            
                // Make sure the buffer is written
                dwCluRead = 1;

                // Increment the bad cluster list pointer
                nNextBad++;
            }
        }
            
        // Copy the data to the output buffer
        dwWrite = 0;
        
        if(dwCluRead)
        {
            // Compress the data before writing it to the image
            if(lpData->lpImage->ls.dwCompBitmapSize)
            {
                for(dwCheck = 0, lpb = lpData->lpImage->lpbReadBuffer; dwCheck < dwCluRead; dwCheck++, lpb += lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize)
                {
                    wCompressed = wUncompressed = (WORD)(lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize);
                    
                    if(CompressData(lpb, wUncompressed, lpData->lpImage->lpbWriteBuffer + dwWrite, &wCompressed))
                    {
                        DPF("Compressed cluster %lu from %u to %u bytes", lpData->lpImage->dwCurrentCluster + dwCheck, wUncompressed, wCompressed);
                    
                        // Data was successfully compressed.  Flip the bit in the bitmap
                        // and move on.
                        if(nRet = SetBitmapBit(&(lpData->lpImage->ibmpCompression), lpData->lpImage->dwCurrentCluster - lpData->lpImage->ls.dwFirstCluster + dwCheck))
                        {
                            return nRet;
                        }
                        
                        dwWrite += wCompressed;
                    }
                    else
                    {
                        // Just copy the uncompressed data to the write buffer.
                        DPF("Cluster %lu is uncompressible", lpData->lpImage->dwCurrentCluster + dwCheck);
                        MemCopy(lpData->lpImage->lpbWriteBuffer + dwWrite, lpb, wUncompressed);
                        
                        dwWrite += wUncompressed;
                    }
                }
            }
            else
            {
                // Copy the read data to the output buffer
                dwWrite = dwCluRead * lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize;

                MemCopy(lpData->lpImage->lpbWriteBuffer, lpData->lpImage->lpbReadBuffer, (UINT)dwWrite);
            }
        }
        
        // Write the cluster(s) to the image file
        if(dwWrite)
        {
            if(nRet = VerifyFileWrite(lpData->lpImage->fhImage, lpData->lpImage->lpbWriteBuffer, lpData->lpImage->lpbReadBuffer, dwWrite, lpData->lpImage->wFlags))
            {
                return nRet;
            }
        }

        // It's possible we didn't actually read any clusters.  This
        // will happen if we were looking for allocated clusters only
        // and the next cluster to read was unused.
        dwCluRead = max(dwCluRead, 1);
        
        // Increment the current cluster pointer
        lpData->lpImage->dwCurrentCluster += dwCluRead;
    }
    
    // Close the last fragment

#ifdef WIN32

    ImageMessageBox(lpData->lpImage->hWnd, MB_ICONINFORMATION, "Image complete");

#else // WIN32

    if(!(lpData->lpImage->wFlags & OPTIONS_NOSTATUS))
    {
        printf(SZBACK);
    }

    printf("\a" SZDONE);

#endif // WIN32

    if(nRet = CloseFragment(lpData))
    {
        return nRet;
    }
    
    // Return success
    return ERROR_NONE;
}


#ifndef WIN32
#pragma optimize("", off)
#endif // WIN32

int GetDriveFreeSpace(BYTE bDrive, LPDWORD lpdwFreeSpace, LPBOOL lpfOverflow)
{
    DWORD                       dwSectorsPerCluster = 0;
    DWORD                       dwBytesPerSector = 0;
    DWORD                       dwFreeClusters = 0;
    DWORD                       dwTotalClusters = 0;
    BOOL                        fOverflow;
    BOOL                        fSuccess;

#ifdef WIN32

    char                        szRoot[] = { 'A' + bDrive, ':', '\\', 0 };

    fOverflow = FALSE;

    fSuccess = GetDiskFreeSpace(szRoot, &dwSectorsPerCluster, &dwBytesPerSector, &dwFreeClusters, &dwTotalClusters);
   
#else

    _asm
    {
        mov     fOverflow, TRUE
        mov     fSuccess, FALSE

        mov     dl, bDrive
        mov     ah, 36h
        int     21h
        
        cmp     ax, 0FFFFh
        je      failure
        
        mov     word ptr [dwSectorsPerCluster], ax
        mov     word ptr [dwFreeClusters], bx
        mov     word ptr [dwBytesPerSector], cx
        mov     word ptr [dwTotalClusters], dx
        mov     fSuccess, TRUE
        
        jo      overflow
        mov     fOverflow, FALSE
  
    overflow:
    failure:
    
    }
    
#endif // WIN32
    
    if(!fSuccess)
    {
        return ERROR_DRIVEREAD;
    }
    
    DPF("dwSectorsPerCluster = %lu", dwSectorsPerCluster);
    DPF("dwFreeClusters = %lu", dwFreeClusters);
    DPF("dwBytesPerSector = %lu", dwBytesPerSector);
    DPF("dwTotalClusters = %lu", dwTotalClusters);

    if(!fOverflow && (dwFreeClusters > (0xFFFFFFFF / dwSectorsPerCluster / dwBytesPerSector)))
    {
        fOverflow = TRUE;
    }
    
    if(!fOverflow)
    {
        *lpdwFreeSpace = dwFreeClusters * dwSectorsPerCluster * dwBytesPerSector;
    }
    
    *lpfOverflow = fOverflow;
    
    return ERROR_NONE;
}
    
#ifndef WIN32
#pragma optimize("", on)
#endif // WIN32


#ifndef WIN32
#pragma optimize("", off)
#endif // WIN32

int IsDriveRemote(BYTE bDrive, LPBOOL pfRemote)
{
    BOOL                        fSuccess = FALSE;
    BOOL                        fRemote = TRUE;

#ifndef WIN32

    _asm
    {
        mov     bl, bDrive
        mov     ax, 4409h
        int     21h

        jc      failure

        test    dx, 1000h
        jnz     remote

        mov     fRemote, FALSE

    remote:
        
        mov     fSuccess, TRUE
        
    failure:
    }
    
#endif // WIN32
    
    if(!fSuccess)
    {
        return ERROR_DRIVEREAD;
    }
    
    *pfRemote = fRemote;
    
    return ERROR_NONE;
}
    
#ifndef WIN32
#pragma optimize("", on)
#endif // WIN32


int NextFragment(LPEVERYTHING lpData)
{
    int                     nRet;
    
    if(lpData->lpImage->dwCurrentCluster > lpData->lpImage->ls.dwLastCluster)
    {

#ifndef WIN32

        if(!(lpData->lpImage->wFlags & OPTIONS_NOSTATUS))
        {
            printf(SZBACK);
        }

        printf(SZDONE CRLF);

#endif // WIN32
    
        if(nRet = CloseFragment(lpData))
        {
            return nRet;
        }
        
        if(nRet = OpenFragment(++(lpData->lpImage->ls.bSequence), lpData))
        {
            return nRet;
        }

#ifndef WIN32

        printf("Processing clusters %lu through %lu... ", lpData->lpImage->dwCurrentCluster + 2, lpData->lpImage->ls.dwLastCluster + 2);

        if(!(lpData->lpImage->wFlags & OPTIONS_NOSTATUS))
        {
            printf(SZCLUSTER, lpData->lpImage->dwCurrentCluster + 2);
        }

#endif // WIN32

    }

    return ERROR_NONE;
}


int OpenFragment(BYTE bFrag, LPEVERYTHING lpData)
{
    static const char *         pszNoLabel = "no label";
    const char *                pszLabel;
    DWORD                       dwFirstData;
    DWORD                       dwFirst;
    DWORD                       dwCount;
    DWORD                       dwUnused;
    DWORD                       dwClustersThisFrag;
    DWORD                       dwFileSize;
    DWORD                       dwFreeSpace;
    BOOL                        fOverflow;
    BOOL                        fRemote;
    int                         nRet;

    // Copy the fragment sequence number
    lpData->lpImage->ls.bSequence = bFrag;
    
    // If we're spanning media, prompt the user to insert the next disk
    if(lpData->lpImage->wFlags & OPTIONS_SPAN)
    {
        InsertDisk(lpData->lpImage, lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence], (char)toupper(lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence][0]));
    }

#ifndef WIN32

    // Tell the user what we're going to do
    if(lpData->lpDrive->bs.szVolumeLabel[0])
    {
        pszLabel = lpData->lpDrive->bs.szVolumeLabel;
    }
    else
    {
        pszLabel = pszNoLabel;
    }
    
    printf("Imaging drive:  %c: (%s)" CRLF, lpData->lpDrive->bDrive + 'A', pszLabel);
    printf("To file:        %s" CRLF, lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence]);
    printf("Fragment:       %u of %u" CRLF, lpData->lpImage->ls.bSequence + 1, lpData->lpImage->ls.bFragCount);

#endif // WIN32

    // Open the file
    if(lpData->lpImage->fhImage == INVALID_FILE_HANDLE)
    {
        if((lpData->lpImage->fhImage = OpenFragmentForWrite(lpData->lpImage->ls.bSequence, lpData->lpImage)) == INVALID_FILE_HANDLE)
        {
            return ERROR_FILEOPEN;
        }
    }
    
    // Copy the name of the next fragment
    if(lpData->lpImage->ls.bSequence + 1 < lpData->lpImage->ls.bFragCount)
    {
        GetFileLocation(lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence + 1], NULL, lpData->lpImage->ls.szNextFrag);
    }
    else
    {
        MemSet(lpData->lpImage->ls.szNextFrag, 0, sizeof(lpData->lpImage->ls.szNextFrag));
    }
    
    // Set the cluster offset for this fragment
    lpData->lpImage->ls.dwFirstCluster = lpData->lpImage->dwCurrentCluster;
    
    // Calculate the size of the bitmaps.  This involves some tricky math to
    // make up for unallocted clusters.
    dwClustersThisFrag = lpData->lpImage->ls.dwClustersPerFrag;
    
    if(lpData->lpDrive->dwUnusedClusters)
    {
        dwFirst = lpData->lpImage->dwCurrentCluster;
        dwCount = lpData->lpImage->ls.dwClustersPerFrag;
        dwUnused = 0;
    
        while(dwFirst < lpData->lpImage->ls.dwClusterCount && dwCount)
        {
            if(nRet = FindUnallocatedClusters(lpData, dwFirst + 2, dwCount, NULL, &dwUnused, TRUE))
            {
                return nRet;
            }
            
            dwClustersThisFrag += dwUnused;
            dwFirst += dwCount;
            dwCount = dwUnused;
        }
    }
    
    // Increment the cluster to stop on
    lpData->lpImage->ls.dwLastCluster = min(lpData->lpImage->dwCurrentCluster + dwClustersThisFrag, lpData->lpImage->ls.dwClusterCount) - 1;
    
     // Determine the size of the compression and allocation bitmaps (1 bit for every allocated cluster on the drive)
    if(lpData->lpImage->wFlags & OPTIONS_COMPRESS)
    {
        lpData->lpImage->ls.dwCompBitmapSize = DIVUP(DIVUP(dwClustersThisFrag, 8), CBSEC);
    }
    else
    {
        lpData->lpImage->ls.dwCompBitmapSize = 0;
    }
        
    if(dwClustersThisFrag > lpData->lpImage->ls.dwClustersPerFrag)
    {
        lpData->lpImage->ls.dwAllocBitmapSize = DIVUP(DIVUP(dwClustersThisFrag, 8), CBSEC);
    }
    else
    {
        lpData->lpImage->ls.dwAllocBitmapSize = 0;
    }

    // Calculate the fragment size (pad sector + lead sector + bitmaps + 
    // [pre-data sectors] + clusters in fragment) in sectors.  If we're 
    // compressing, assume 2:1.
    dwFileSize = (2 + lpData->lpImage->ls.dwCompBitmapSize + lpData->lpImage->ls.dwAllocBitmapSize) / 2;

    if(!lpData->lpImage->ls.bSequence)
    {
        dwFileSize += lpData->lpDrive->bs.dwPreDataSectors;
    }

    dwFileSize += (lpData->lpImage->ls.dwLastCluster - lpData->lpImage->ls.dwFirstCluster) * lpData->lpDrive->bs.dwSectorsPerCluster;

    if(lpData->lpDrive->dwUnusedClusters)
    {
        dwFileSize -= dwUnused * lpData->lpDrive->bs.dwSectorsPerCluster;
    }

    if(lpData->lpImage->wFlags & OPTIONS_COMPRESS)
    {
        dwFileSize /= 2;
    }
    
    printf("Fragment size:  %lu MB", dwFileSize / 2 / 1024);

    if(lpData->lpImage->wFlags & OPTIONS_COMPRESS)
    {
        printf(" (assuming 2:1 compression)");
    }

    printf(CRLF CRLF);

    // Make sure there's enough space for the file
    if(!(lpData->lpImage->wFlags & OPTIONS_FORCE))
    {
        if(nRet = GetDriveFreeSpace((BYTE)((toupper(lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence][0]) - 'A') + 1), &dwFreeSpace, &fOverflow))
        {
            return ERROR_DRIVEREAD;
        }
        
        if(fOverflow)
        {
            if(!AskUser(lpData->lpImage, "The amount of free space on the destination drive cannot be determined." CRLF "There may not be enough free space to write the image file." CRLF "Continue anyway?"))
            {
                return ERROR_DRIVETOOSMALL;
            }
        }
        else if(dwFreeSpace < dwFileSize)
        {
            if(!AskUser(lpData->lpImage, "The destination drive does not appear to contain enough free space." CRLF "Continue anyway?"))
            {
                return ERROR_DRIVETOOSMALL;
            }
        }
    }

    // If the fragment is on a network drive, it can't be larger than 2GB.
    if(!(lpData->lpImage->wFlags & OPTIONS_FORCE))
    {
        if(dwFileSize >= 0x400000)
        {
            if(nRet = IsDriveRemote((BYTE)((toupper(lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence][0]) - 'A') + 1), &fRemote))
            {
                if(!AskUser(lpData->lpImage, "Unable to determine if the destination drive is a mapped network share." CRLF "If it is, files whose size is greater than 2 GB cannot be created." CRLF "Continue anyway?"))
                {
                    return ERROR_DRIVETOOSMALL;
                }
            }
            else if(fRemote)
            {
                if(!AskUser(lpData->lpImage, "Files created on a mapped network share cannot be greater than 2 GB." CRLF "Continue anyway?"))
                {
                    return ERROR_DRIVETOOSMALL;
                }
            }
        }
    }                
   
    // Write the image header
    if(nRet = WriteImageHeader(lpData->lpImage->fhImage, &lpData->lpImage->ls, &dwFirstData))
    {
        return nRet;
    }
    
    // Initialize the bitmaps
    if(lpData->lpImage->ls.dwCompBitmapSize)
    {
        if(nRet = InitBitmap(&(lpData->lpImage->ibmpCompression), lpData->lpImage->ls.dwCompBitmapSize, lpData->lpImage->fhImage, 2, FALSE))
        {
            return nRet;
        }
    }
    
    if(lpData->lpImage->ls.dwAllocBitmapSize)
    {
        if(nRet = InitBitmap(&(lpData->lpImage->ibmpAllocation), lpData->lpImage->ls.dwAllocBitmapSize, lpData->lpImage->fhImage, 2 + lpData->lpImage->ls.dwCompBitmapSize, FALSE))
        {
            return nRet;
        }
    }
    
    // Save the unallocated clusters to the bitmap
    if(lpData->lpImage->ls.dwAllocBitmapSize)
    {
        if(nRet = FindUnallocatedClusters(lpData, lpData->lpImage->dwCurrentCluster + 2, dwClustersThisFrag, &(lpData->lpImage->ibmpAllocation), NULL, TRUE))
        {
            return nRet;
        }

        FlushBitmapBuffer(&(lpData->lpImage->ibmpAllocation));
    }
    
    // Seek to the start of the data area
    if((DWORD)FileSetPos(lpData->lpImage->fhImage, dwFirstData, SEEK_SET) != dwFirstData)
    {
        return ERROR_FILEWRITE;
    }

    // Return success
    return ERROR_NONE;
}


int CloseFragment(LPEVERYTHING lpData)
{
    int                         nRet;
    
    // Close the bitmaps
    if(lpData->lpImage->ls.dwCompBitmapSize)
    {
        if(nRet = DiscardBitmap(&(lpData->lpImage->ibmpCompression)))
        {
            return nRet;
        }
    }
    
    if(lpData->lpImage->ls.dwAllocBitmapSize)
    {
        if(nRet = DiscardBitmap(&(lpData->lpImage->ibmpAllocation)))
        {
            return nRet;
        }
    }
    
    // Update the 'completed' flag in the header
    lpData->lpImage->ls.fCompleted = TRUE;
    
    if(nRet = WriteImageHeader(lpData->lpImage->fhImage, &lpData->lpImage->ls, NULL))
    {
        return nRet;
    }
    
    lpData->lpImage->ls.fCompleted = FALSE;
    
    // Close the file handle
    FileClose(lpData->lpImage->fhImage);
    lpData->lpImage->fhImage = INVALID_FILE_HANDLE;
    
    // Return success
    return ERROR_NONE;
}


int VerifyFileWrite(FILEHANDLE fh, LPVOID lpvWriteBuffer, LPVOID lpvVerifyBuffer, DWORD dwSize, WORD wFlags)
{
    if(FileWrite(fh, lpvWriteBuffer, dwSize) != dwSize)
    {
        return ERROR_FILEWRITE;
    }

    if(wFlags & OPTIONS_VERIFY)
    {
        FileSetPos(fh, -(long)dwSize, SEEK_CUR);
        
        FileCommit(fh);
        
        if(FileRead(fh, lpvVerifyBuffer, dwSize) != dwSize)
        {
            return ERROR_FILEREAD;
        }

        if(MemCmp(lpvVerifyBuffer, lpvWriteBuffer, (UINT)dwSize))
        {
            return ERROR_VERIFYFILE;
        }
    }

    return ERROR_NONE;
}


int DumpInfo(LPEVERYTHING lpData)
{
    DWORD                       dwFragCount;
    DWORD                       dwFileSize;
    int							nRet;
    
    // Calculate the total number of clusters in the image.
    lpData->lpImage->ls.dwClusterCount = lpData->lpDrive->bs.dwTotalClusters;
    
    // Scan for unallocated clusters
    if(nRet = FindUnallocatedClusters(lpData, 2, lpData->lpDrive->bs.dwTotalClusters, NULL, &(lpData->lpDrive->dwUnusedClusters), FALSE))
    {
        return nRet;
    }
    
    // Calculate the maximum number of clusters allowable in each
    // image fragment.  If we're not fragmenting the image,
    // set the number of clusters per fragment to the total number
    // of clusters on the drive so that we can do safe math based
    // on this number.
    if(lpData->lpImage->dwMaxFragSize)
    {
        lpData->lpImage->ls.dwClustersPerFrag = (lpData->lpImage->dwMaxFragSize * 1024 * 1024) / lpData->lpDrive->bs.dwSectorsPerCluster / lpData->lpDrive->bs.dwSectorSize;
    }
    else
    {
        lpData->lpImage->ls.dwClustersPerFrag = lpData->lpImage->ls.dwClusterCount;
    }

    // Calculate the number of fragments this operation will require
    dwFragCount = max(1, DIVUP(lpData->lpImage->ls.dwClusterCount - lpData->lpDrive->dwUnusedClusters, lpData->lpImage->ls.dwClustersPerFrag));
        
    if(dwFragCount > MAX_IMAGE_FRAGMENTS)
    {
        return ERROR_TOOMANYFRAGMENTS;
    }
    
    lpData->lpImage->ls.bFragCount = (BYTE)dwFragCount;

     // Determine the size of the compression and allocation bitmaps (1 bit for every allocated cluster on the drive)
    lpData->lpImage->ls.dwCompBitmapSize = DIVUP(DIVUP(lpData->lpImage->ls.dwClusterCount, 8), CBSEC);
    lpData->lpImage->ls.dwAllocBitmapSize = DIVUP(DIVUP(lpData->lpImage->ls.dwClusterCount, 8), CBSEC);
    
    // Calculate the size of the image
    dwFileSize = lpData->lpImage->ls.bFragCount * (2 + lpData->lpImage->ls.dwCompBitmapSize + lpData->lpImage->ls.dwAllocBitmapSize);
    dwFileSize += lpData->lpDrive->bs.dwPreDataSectors;
    dwFileSize += lpData->lpImage->ls.dwClusterCount * lpData->lpDrive->bs.dwSectorsPerCluster;

    if(lpData->lpImage->wFlags & OPTIONS_COMPRESS)
    {
        dwFileSize /= 2;
    }
    
    // Spew
    DumpBootSector("Source drive boot sector", &(lpData->lpDrive->bs));
    DumpDPB("Source drive DOS parameter block", &(lpData->lpDrive->dpb));
    
    printf("Image breakdown" CRLF LINE);
    
    printf("Total image size:                           %lu kb", dwFileSize / 2);

    if(lpData->lpImage->wFlags & OPTIONS_COMPRESS)
    {
        printf(" (assuming 2:1 compression)");
    }

    printf(CRLF);

    printf("Header size:                                %lu bytes per fragment" CRLF, 2L * CBSEC);

    if(lpData->lpImage->wFlags & OPTIONS_COMPRESS)
    {
        printf("Compression bitmap size:                    %lu bytes per fragment" CRLF, lpData->lpImage->ls.dwCompBitmapSize * CBSEC);
    }
    
    if(lpData->lpImage->wFlags & OPTIONS_ALLOCATED)
    {
        printf("Allocation bitmap size:                     %lu bytes per fragment" CRLF, lpData->lpImage->ls.dwAllocBitmapSize * CBSEC);
    }
    
    printf("Fragment count:                             %u" CRLF, lpData->lpImage->ls.bFragCount);
    
    return ERROR_NONE;
}


