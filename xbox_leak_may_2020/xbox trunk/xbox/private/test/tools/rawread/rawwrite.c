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
#endif // WIN32

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
int PutImage(LPEVERYTHING);
int EnterMainLoop(LPEVERYTHING);
int NextFragment(LPEVERYTHING);
int OpenFragment(BYTE, LPEVERYTHING, BOOL);
int CloseFragment(LPEVERYTHING);
int WipeClustersToCurrent(LPEVERYTHING, DWORD);
int VerifyWriteSector(LPDRIVEINFO, LPVOID, LPVOID, DWORD, DWORD, WORD);
int DumpInfo(LPEVERYTHING);

#ifdef WIN32

LPARAM CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

#endif // WIN32


#ifdef WIN32

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
    EVERYTHING                  info;
    IMAGESTRUCT                 is;
    IMAGERES                    ir;
    DRIVEINFO                   di;
    WNDCLASSEX                  wc;
    HACCEL                      hAccel;
    HWND                        hWnd;
    MSG                         msg;

    // Initialize data
    MemSet(&is, 0, sizeof(is));
    MemSet(data.lpImage, 0, sizeof(ir));
    MemSet(&di, 0, sizeof(di));

    ir.fhImage = INVALID_FILE_HANDLE;
    
    GetRegOptions(&is);
    
    info.lpData->lpImage = &is;
    info.lpData->lpImage = data.lpImage;
    info.lpData->lpDrive = &di;
    
    // Init disk util
    InitDiskUtil();

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
    wc.lpszClassName = "RawWrite:Main";
    wc.hIconSm = LoadImage(hInst, MAKEINTRESOURCE(IDI_IMAGE), IMAGE_ICON, 16, 16, 0);

    RegisterClassEx(&wc);

    // Load the accelerator table
    hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR));
    
    // Create the main window
    ir.hWnd = hWnd = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_RAWWRITE), NULL, wc.lpfnWndProc, (LPARAM)&info);

    // Enter message loop
    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
        if(!TranslateAccelerator(hWnd, hAccel, &msg) && !IsDialogMessage(hWnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    // Release disk utiil
    FreeDiskUtil();

    // Return success
    return msg.wParam;
}


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
            
            // Initialize control values
            SetDlgItemText(hWnd, IDC_FILE, lpInfo->lpData->lpImage->szFile);

            SetCheckFlag(hWnd, IDC_OPTION_WIPE, OPTIONS_WIPE, lpInfo->lpData->lpImage->wFlags);
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

                    // Get options
                    GetCheckFlag(hWnd, IDC_OPTION_WIPE, OPTIONS_WIPE, &(lpInfo->lpData->lpImage->wFlags));
                    GetCheckFlag(hWnd, IDC_OPTION_FINDBAD, OPTIONS_FINDBAD, &(lpInfo->lpData->lpImage->wFlags));
                    GetCheckFlag(hWnd, IDC_OPTION_VERIFY, OPTIONS_VERIFY, &(lpInfo->lpData->lpImage->wFlags));
                    GetCheckFlag(hWnd, IDC_OPTION_NOSTOPONERROR, OPTIONS_FORCE, &(lpInfo->lpData->lpImage->wFlags));

                    // Save settings to the registry
                    SetRegOptions(lpInfo->lpData->lpImage);
                    
                    // Go
                    if(nRet = PrepImage(lpInfo->lpData->lpImage, lpInfo->lpData->lpImage, lpInfo->lpData->lpDrive))
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

    data.lpDrive->bDrive = 0xFF;
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
    wc.lpszClassName = "RawWrite:Main";
    wc.hIconSm = LoadImage(hInst, MAKEINTRESOURCE(IDI_IMAGE), IMAGE_ICON, 16, 16, 0);

    RegisterClassEx(&wc);

    // Load the accelerator table
    hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR));
    
    // Create the main window
    data.lpImage->hWnd = hWnd = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_RAWWRITE), NULL, wc.lpfnWndProc, (LPARAM)&data);

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
        if(argv[i][0] == '/' || argv[i][0] == '-')
        {
            for(z = 1; z < StrLen(argv[i]); z++)
            {
                switch(toupper(argv[i][z]))
                {
                    case SWITCH_HELP:
                        return GoRawWriteHelp();
                       
                    case SWITCH_FORCE:
                        data.lpImage->wFlags |= OPTIONS_FORCE;
                        break;
                    
                    case SWITCH_IGNORETYPE:
                        data.lpImage->wFlags |= OPTIONS_IGNORETYPE;
                        break;
                    
                    case SWITCH_WIPE:
                        data.lpImage->wFlags |= OPTIONS_WIPE;
                        break;
                    
                    case SWITCH_NOSTATUS:
                        data.lpImage->wFlags |= OPTIONS_NOSTATUS;
                        break;
                    
                    case SWITCH_VERIFY:
                        data.lpImage->wFlags |= OPTIONS_VERIFY;
                        break;
                    
                    case SWITCH_FINDBAD:
                        data.lpImage->wFlags |= OPTIONS_FINDBAD;
                        break;
                    
                    case SWITCH_DUMPINFO:
                        data.lpImage->wFlags |= OPTIONS_DUMPINFO;
                        break;
                   
                    default:
                        printf("Erroneous switch:  %c" CRLF CRLF, argv[i][z]);
                        return GoRawWriteHelp();
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
    
    // Did we get the right number of arguments?  If we're just dumping
    // data, the user is allowed to leave out either the file name or
    // drive letter.
    if(argc != 3)
    {
        if(!((data.lpImage->wFlags & OPTIONS_DUMPINFO) && (argc == 2)))
        {
            return GoRawWriteHelp();
        }
    }
    
    // First argument should be a filename
    if(!(data.lpImage->wFlags & OPTIONS_DUMPINFO) || !IsDrivePath(argv[1], &data.lpDrive->bDrive))
    {
        GetFullPath(data.lpImage->aszFragNames[0], argv[1], MAX_PATH);
    }
    
    // Second argument should be a drive path
    if(argc > 2)
    {
        if(!IsDrivePath(argv[2], &data.lpDrive->bDrive))
        {
            return GoRawWriteHelp();
        }
    }

    // If the drive is compressed, bail
    if(data.lpDrive->bDrive != 0xFF)
    {
        if(GetDriveMapping(data.lpDrive->bDrive, NULL, NULL))
        {
            return ErrorMsg(data.lpImage, ERROR_COMPRESSEDDRIVE);
        }
    }
 
    // Can't put the image file on the image drive
    if(data.lpDrive->bDrive != 0xFF)
    {
        if(toupper(data.lpImage->aszFragNames[0][0]) == data.lpDrive->bDrive + 'A')
        {
            return ErrorMsg(data.lpImage, ERROR_DRIVEWRITE);
        }
    }

#endif // WIN32
    
    // Get drive info
    if(data.lpDrive->bDrive != 0xFF)
    {
        if(nRet = GetDriveInfo(data.lpDrive->bDrive, data.lpDrive))
        {
            return ErrorMsg(data.lpImage, nRet);
        }
    }

    // Attempt to lock the drive
    if((data.lpDrive->bDrive != 0xFF) && !(data.lpImage->wFlags & OPTIONS_NOLOCK))
    {
        if(!LockVolume(data.lpDrive->bDrive) && !(data.lpImage->wFlags & OPTIONS_FORCE))
        {
            if(!AskUser(data.lpImage, "Unable to lock drive %c:.  Continue anyway?", data.lpDrive->bDrive + 'A'))
            {
                return ErrorMsg(data.lpImage, ERROR_LOCKFAIL);
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
        nRet = PutImage(&data);

#ifdef WIN32

        // Close the progress dialog
        data.lpImage->hWnd = CloseProgressDialog(data.lpImage->hWnd);

#endif // WIN32

    }

    // Unlock the drive
    if((data.lpDrive->bDrive != 0xFF) && !(data.lpImage->wFlags & OPTIONS_NOLOCK))
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
    MemFree(data.lpImage);
    MemFree(data.lpDrive);

    // Return

#ifndef WIN32

    printf(CRLF);

#endif // WIN32

    return nRet;
}


int PutImage(LPEVERYTHING lpData)
{
    BGBOOTSECTOR                bsRaw;
    RRBOOTSECTOR                bsImage;
    int                         nRet;
    BOOL                        fBSRead = FALSE;
    DWORD                       dwCurrentSector;
    DWORD                       dwRead;
    DWORD                       dwDriveSectors;
    
    // Scan for bad clusters
    if(nRet = FindBadClusters(lpData, 2, lpData->lpDrive->bs.dwTotalClusters, &(lpData->lpDrive->adwBadClusters), &(lpData->lpDrive->dwBadClusters), FALSE))
    {
        return nRet;
    }

    // Open the first fragment
    if(nRet = OpenFragment(0, lpData, FALSE) && nRet != ERROR_INVALIDIMAGE)
    {
        return nRet;
    }

    // It's possible that the user is trying to dump an invalid image.
    // Let's give them the benefit of the doubt and try to dump it anyway.
    if(nRet == ERROR_INVALIDIMAGE)
    {
        if(!(lpData->lpImage->wFlags & OPTIONS_IGNORETYPE))
        {
            if(!AskUser(lpData->lpImage, "This is not a valid " RAWREAD "/" RAWWRITE " image." CRLF "Attempt to process the image anyway?"))
            {
                return ERROR_INVALIDIMAGE;
            }
        }
        
        // Generate a fake LEADSECTOR based on the boot sector in the image
        MemSet(&(lpData->lpImage->ls), 0, sizeof(lpData->lpImage->ls));
        
        FileSetPos(lpData->lpImage->fhImage, 0, SEEK_SET);            

        if(FileRead(lpData->lpImage->fhImage, &bsRaw, sizeof(bsRaw)) != sizeof(bsRaw))
        {
            return ERROR_FILEREAD;
        }

        if(nRet = ConvertBootSector(&bsRaw, &bsImage))
        {
            return nRet;
        }

        fBSRead = TRUE;
        
        lpData->lpImage->ls.dwClusterCount = bsImage.dwTotalClusters;
        lpData->lpImage->ls.bFragCount = 1;
        lpData->lpImage->ls.dwClustersPerFrag = lpData->lpImage->ls.dwClusterCount;
        lpData->lpImage->ls.dwLastCluster = lpData->lpImage->ls.dwClusterCount - 1;
        lpData->lpImage->ls.fCompleted = TRUE;
    }

    // Read the boot sector from the image
    if(!fBSRead)
    {
        if(FileRead(lpData->lpImage->fhImage, &bsRaw, sizeof(bsRaw)) != sizeof(bsRaw))
        {
            return ERROR_FILEREAD;
        }

        if(nRet = ConvertBootSector(&bsRaw, &bsImage))
        {
            return nRet;
        }
    }
    
    // Is the drive big enough to hold this image?
    if(!(lpData->lpImage->wFlags & OPTIONS_FORCE))
    {
        dwDriveSectors = lpData->lpDrive->bs.dwTotalSectors - (lpData->lpDrive->dwBadClusters * lpData->lpDrive->bs.dwSectorsPerCluster);
        
        if(bsImage.dwUsableSectors > dwDriveSectors)
        {
            return ERROR_DRIVETOOSMALL;
        }
        else if(bsImage.dwUsableSectors < dwDriveSectors)
        {
            if(!AskUser(lpData->lpImage, "The target drive has %lu more usable sectors than the original drive." CRLF "These extra sectors will be unusable until the target drive is " CRLF "repartitioned.  Continue anyway?", dwDriveSectors - bsImage.dwUsableSectors))
            {
                return ERROR_USERCANCEL;
            }
        }
    }
    
    // Are the filesystem types compatible?
    if(!(lpData->lpImage->wFlags & OPTIONS_FORCE))
    {
        if(StrCmpI(bsImage.szFSType, lpData->lpDrive->bs.szFSType))
        {
            if(!AskUser(lpData->lpImage, "The file system types of the source image and target drive are different.  The" CRLF "target drive will, in essence, be reformatted.  Continue anyway?"))
            {
                return ERROR_WRONGFSTYPE;
            }
        }
    }
    
    // Some fields of the boot sector need to be preserved
    bsRaw.bpb.wSectorsPerTrack = lpData->lpDrive->bs.bsOriginal.bpb.wSectorsPerTrack;
    bsRaw.bpb.wHeadCount = lpData->lpDrive->bs.bsOriginal.bpb.wHeadCount;
    bsRaw.bpb.dwHiddenSectors = lpData->lpDrive->bs.bsOriginal.bpb.dwHiddenSectors;

    // Overwrite our copy of the drive's boot sector with the image's
    ConvertBootSector(&bsRaw, &(lpData->lpDrive->bs));
    
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
    
    if(nRet = VerifyWriteSector(lpData->lpDrive, &(lpData->lpDrive->bs.bsOriginal), lpData->lpImage->lpbReadBuffer, 0, 1, lpData->lpImage->wFlags))
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
            dwRead = bsImage.dwPreDataSectors - dwCurrentSector;
        }

        if(FileRead(lpData->lpImage->fhImage, lpData->lpImage->lpbWriteBuffer, dwRead * lpData->lpDrive->bs.dwSectorSize) != dwRead * lpData->lpDrive->bs.dwSectorSize)
        {
            return ERROR_FILEREAD;
        }

        if(nRet = VerifyWriteSector(lpData->lpDrive, lpData->lpImage->lpbWriteBuffer, lpData->lpImage->lpbReadBuffer, dwCurrentSector, dwRead, lpData->lpImage->wFlags))
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


int EnterMainLoop(LPEVERYTHING lpData)
{
    DWORD                       dwBadCluster;
    DWORD                       dwCluWrite;
    DWORD                       dwRead;
    DWORD                       dwWrite;
    DWORD                       dwCheck;
    WORD                        wCompressed;
    DWORD                       dwTotalCompressed;
    WORD                        wUncompressed;
    DWORD                       dwTotalUncompressed;
    BOOL                        fSet;
    LPBYTE                      lpbWrite;
    LPBYTE                      lpbRead;
    DWORD                       dwWipe;
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
        dwWipe = 0;

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
            dwWipe++;
        }

        // Wipe any unused clusters
        if(lpData->lpImage->wFlags & OPTIONS_WIPE && dwWipe)
        {
            if(nRet = WipeClustersToCurrent(lpData, dwWipe))
            {
                return nRet;
            }
        }

        // This is done for UI niceness.  It would be cleaner to change
        // the above while() to check for this.
        if(lpData->lpImage->dwCurrentCluster >= lpData->lpImage->ls.dwClusterCount)
        {
            break;
        }

        // Determine the number of clusters to read
        dwCluWrite = READWRITE_BUFFER_SIZE / (lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize);
        
        // Make sure we don't read past the end of this fragment
        dwCluWrite = min(dwCluWrite, lpData->lpImage->ls.dwLastCluster + 1 - lpData->lpImage->dwCurrentCluster);
        
        // If there's any unused clusters within the next write buffer, stop just before them
        if(lpData->lpImage->ls.dwAllocBitmapSize)
        {
            dwCheck = 0;
            
            while(dwCheck < dwCluWrite)
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
            
            dwCluWrite = min(dwCluWrite, dwCheck);
        }
        
        // If there's any bad clusters within the next read buffer, stop just before them
        if(nNextBad < lpData->lpDrive->dwBadClusters)
        {
            dwCluWrite = min(dwCluWrite, lpData->lpDrive->adwBadClusters[nNextBad] - lpData->lpImage->dwCurrentCluster);
        }
        
        // Undocumented switch to force 1 cluster at a time
        if(lpData->lpImage->wFlags & OPTIONS_1CLUSTER)
        {
            dwCluWrite = min(dwCluWrite, 1);
        }
        
        // Calculate the current sector
        dwCurrentSector = ClusterToSector(&(lpData->lpDrive->bs), lpData->lpImage->dwCurrentCluster + 2);
    
        // Read the cluster(s) from the image
        if(dwCluWrite > 0)
        {
            // Read the clusters from the image file
            dwWrite = dwCluWrite * lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize;
            dwRead = FileRead(lpData->lpImage->fhImage, lpData->lpImage->lpbReadBuffer, dwWrite);

            // Copy the read data to the write buffer
            if(lpData->lpImage->ls.dwCompBitmapSize)
            {
                lpbRead = lpData->lpImage->lpbReadBuffer;
                lpbWrite = lpData->lpImage->lpbWriteBuffer;

                dwTotalCompressed = 0;
                dwTotalUncompressed = 0;

                for(dwCheck = 0; dwCheck < dwCluWrite; dwCheck++)
                {
                    wCompressed = wUncompressed = (WORD)(lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize);
                    
                    if(nRet = GetBitmapBit(&(lpData->lpImage->ibmpCompression), lpData->lpImage->dwCurrentCluster - lpData->lpImage->ls.dwFirstCluster + dwCheck, &fSet))
                    {
                        return nRet;
                    }

                    if(fSet)
                    {
                        // This is a compressed cluster
                        if(!UncompressData(lpbRead, &wCompressed, lpbWrite, wUncompressed))
                        {
                            return ERROR_UNCOMPRESS;
                        }

                        DPF("Uncompressed cluster %lu from %u to %u bytes", dwCheck + lpData->lpImage->dwCurrentCluster, wCompressed, wUncompressed);
                    }
                    else
                    {
                        // The data is not compressed.  Just copy it to the write buffer
                        DPF("Cluster %lu is not compressed", dwCheck + lpData->lpImage->dwCurrentCluster);
                        MemCopy(lpbWrite, lpbRead, wUncompressed);
                    }

                    lpbRead += wCompressed;
                    dwTotalCompressed += wCompressed;

                    lpbWrite += wUncompressed;
                    dwTotalUncompressed += wUncompressed;
                }

                DPF("Uncompressed a total of %lu to %lu bytes", dwTotalCompressed, dwTotalUncompressed);

                // Verify that we uncompressed the correct amount of data
                if(dwTotalUncompressed != dwWrite)
                {
                    return ERROR_UNCOMPRESS;
                }

                // Reposition the file cursor
                DPF("Backing up %ld bytes", dwTotalCompressed - dwRead);
                
                if(FileSetPos(lpData->lpImage->fhImage, dwTotalCompressed - dwRead, SEEK_CUR) == -1)
                {
                    return ERROR_FILEREAD;
                }
            }
            else
            {
                // The data is not compressed.  Verify that we read the correct 
                // amount of data.
                if(dwWrite != dwRead)
                {
                    return ERROR_FILEREAD;
                }

                // Copy the data to the write buffer
                MemCopy(lpData->lpImage->lpbWriteBuffer, lpData->lpImage->lpbReadBuffer, (UINT)dwRead);
            }
        }

        // Special case any odd clusters (i.e. bad, unallocated)
        if(dwCluWrite < 1)
        {
            // Is the corresponding cluster on the drive bad?
            if(nNextBad < lpData->lpDrive->dwBadClusters && lpData->lpDrive->adwBadClusters[nNextBad] == lpData->lpImage->dwCurrentCluster)
            {
                nNextBad++;
                
                // Determine the cluster size
                if(lpData->lpImage->ls.dwCompBitmapSize)
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
                    // The size of the compressed cluster is stored in the first
                    // two bytes of the data.
                    if(FileRead(lpData->lpImage->fhImage, &wCompressed, sizeof(WORD)) != sizeof(WORD))
                    {
                        return ERROR_FILEREAD;
                    }
                }
                else
                {
                    // Just go with the default
                    wCompressed = (WORD)(lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize);
                }
                
                // Seek past the cluster
                if(FileSetPos(lpData->lpImage->fhImage, wCompressed, SEEK_CUR) == -1)
                {
                    return ERROR_FILEREAD;
                }
            }
            else
            {
                // Is the cluster unused?
                if(lpData->lpImage->ls.dwAllocBitmapSize)
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
                    // The cluster isn't allocated, and therefore is not
                    // actually in the image.  Just move along...
                }
            }
        }
        
        // Write the cluster(s) to the drive
        if(dwCluWrite)
        {
            if(nRet = VerifyWriteSector(lpData->lpDrive, lpData->lpImage->lpbWriteBuffer, lpData->lpImage->lpbReadBuffer, dwCurrentSector, dwCluWrite * lpData->lpDrive->bs.dwSectorsPerCluster, lpData->lpImage->wFlags))
            {
                if(nRet == ERROR_DRIVEWRITE)
                {
                    // Uh-oh.  We couldn't write to one of the clusters.  Break the loop
                    // down to 1 cluster at a time and determine which cluster it was.
                    for(dwBadCluster = 0; dwBadCluster < dwCluWrite; dwBadCluster++)
                    {
                        lpbWrite = lpData->lpImage->lpbWriteBuffer + (dwBadCluster * lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize);

                        if(nRet = VerifyWriteSector(lpData->lpDrive, lpbWrite, lpData->lpImage->lpbReadBuffer, dwCurrentSector + (dwBadCluster * lpData->lpDrive->bs.dwSectorsPerCluster), 1, lpData->lpImage->wFlags))
                        {
                            if(nRet == ERROR_DRIVEWRITE)
                            {

#ifdef WIN32

                                if(!(lpData->lpImage->wFlags & OPTIONS_FORCE))
                                {
                                    ImageMessageBox(lpData->lpImage->hWnd, MB_ICONEXCLAMATION, "Unable to write to cluster %lu.... ", lpData->lpImage->dwCurrentCluster + 2 + dwBadCluster);
                                }

#else // WIN32

                                printf(CRLF "Unable to write to cluster %lu.  ", lpData->lpImage->dwCurrentCluster + 2 + dwBadCluster);
                        
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
                                    return ERROR_DRIVEWRITE;
                                }

#ifndef WIN32
                        
                                if(!(lpData->lpImage->wFlags & OPTIONS_NOSTATUS))
                                {
                                    printf(SZCLUSTER, lpData->lpImage->dwCurrentCluster + dwBadCluster + 2 + 1);
                                }

#endif // WIN32
                            
                            }
                            else
                            {
                                return nRet;
                            }
                        }
                    }
                }
                else
                {
                    return nRet;
                }
            }
        }
        
        // It's possible we didn't actually write any clusters.  This
        // will happen if we were looking for allocated clusters only
        // and the next cluster to write was unused.
        dwCluWrite = max(dwCluWrite, 1);
        
        lpData->lpImage->dwCurrentCluster += dwCluWrite;
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


int NextFragment(LPEVERYTHING lpData)
{
    int						nRet;
    
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
        
        if(nRet = OpenFragment(++(lpData->lpImage->ls.bSequence), lpData, FALSE))
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
    

int OpenFragment(BYTE bFrag, LPEVERYTHING lpData, BOOL fQuiet)
{
    DWORD                       dwFirstData;
    int                         nRet;

    // If we're spanning media, prompt the user to insert the next disk
    if(lpData->lpImage->wFlags & OPTIONS_SPAN)
    {
        InsertDisk(lpData->lpImage, lpData->lpImage->aszFragNames[bFrag], (char)toupper(lpData->lpImage->aszFragNames[bFrag][0]));
    }

    do
    {
        // Open the file
        if((lpData->lpImage->fhImage = OpenFragmentForRead(bFrag, lpData->lpImage)) == INVALID_FILE_HANDLE)
        {
            return ERROR_FILEOPEN;
        }
        
        // Read the image header
        if(nRet = ReadImageHeader(lpData->lpImage->fhImage, &(lpData->lpImage->ls), &dwFirstData, FALSE))
        {
            return nRet;
        }
        
        // Is this actually the next image in the sequence?
        if(lpData->lpImage->ls.bSequence != bFrag)
        {
            FileClose(lpData->lpImage->fhImage);
            lpData->lpImage->fhImage = INVALID_FILE_HANDLE;
                
            ErrorMsg(lpData->lpImage, ERROR_OUTOFSEQUENCE);

            if(nRet = GetFragNames(lpData->lpImage, bFrag, 1))
            {
                return nRet;
            }
        }
    }
    while(lpData->lpImage->ls.bSequence != bFrag);

#ifndef WIN32

    // Tell the user what we're going to do
    if(!fQuiet)
    {
        printf("Writing image:  %s" CRLF, lpData->lpImage->aszFragNames[bFrag]);
        printf("To drive:       %c: (%s)" CRLF, lpData->lpDrive->bDrive + 'A', lpData->lpDrive->bs.szVolumeLabel);
        printf("Fragment:       %u of %u" CRLF, bFrag + 1, lpData->lpImage->ls.bFragCount);
        printf(CRLF);
    }

#endif // WIN32

    // Validate the cluster offset
    if(lpData->lpImage->ls.dwFirstCluster != lpData->lpImage->dwCurrentCluster)
    {
        return ERROR_BADCLUSTEROFFSET;
    }
    
    // Initialize the bitmaps
    if(lpData->lpImage->ls.dwCompBitmapSize)
    {
        if(nRet = InitBitmap(&(lpData->lpImage->ibmpCompression), lpData->lpImage->ls.dwCompBitmapSize, lpData->lpImage->fhImage, 2, TRUE))
        {
            return nRet;
        }
    }
    
    if(lpData->lpImage->ls.dwAllocBitmapSize)
    {
        if(nRet = InitBitmap(&(lpData->lpImage->ibmpAllocation), lpData->lpImage->ls.dwAllocBitmapSize, lpData->lpImage->fhImage, 2 + lpData->lpImage->ls.dwCompBitmapSize, TRUE))
        {
            return nRet;
        }
    }
    
    // Seek to the start of the data area
    FileSetPos(lpData->lpImage->fhImage, dwFirstData, SEEK_SET);

    // Return success
    return ERROR_NONE;
}


int CloseFragment(LPEVERYTHING lpData)
{
    int                         nRet;
    
    // Copy the name of the next fragment into the array
    GetFullPath(lpData->lpImage->aszFragNames[lpData->lpImage->ls.bSequence + 1], lpData->lpImage->ls.szNextFrag, MAX_PATH);
    
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
    
    // Close the file handle
    FileClose(lpData->lpImage->fhImage);
    lpData->lpImage->fhImage = INVALID_FILE_HANDLE;
    
    // Return success
    return ERROR_NONE;
}


int VerifyWriteSector(LPDRIVEINFO lpDrive, LPVOID lpvWriteBuffer, LPVOID lpvVerifyBuffer, DWORD dwSector, DWORD dwCount, WORD wFlags)
{
    if(!WriteSectorEx(lpDrive->bDrive, lpvWriteBuffer, dwSector, dwCount, lpDrive->bs.dwSectorSize))
    {
        return ERROR_DRIVEWRITE;
    }
    
    if(wFlags & OPTIONS_VERIFY)
    {
        if(!ReadSectorEx(lpDrive->bDrive, lpvVerifyBuffer, dwSector, dwCount, lpDrive->bs.dwSectorSize))
        {
            return ERROR_DRIVEREAD;
        }

        if(MemCmp(lpvVerifyBuffer, lpvWriteBuffer, (UINT)(dwCount * lpDrive->bs.dwSectorSize)))
        {
            return ERROR_VERIFYDISK;
        }
    }

    return ERROR_NONE;
}


int WipeClustersToCurrent(LPEVERYTHING lpData, DWORD dwCount)
{
    DWORD                       dwCurrentCluster;
    DWORD                       dwCurrentSector;
    DWORD                       dwWipe;
    
    dwCurrentCluster = lpData->lpImage->dwCurrentCluster - dwCount;
    dwCurrentSector = ClusterToSector(&(lpData->lpDrive->bs), dwCurrentCluster + 2);
    
    MemSet(lpData->lpImage->lpbWriteBuffer, 0, (UINT)READWRITE_BUFFER_SIZE);

    while(dwCount)
    {
        if(!(lpData->lpImage->wFlags & OPTIONS_NOSTATUS))
        {

#ifdef WIN32

            SetProgressPos(lpData->lpImage->hWnd, dwCurrentCluster + 2, lpData->lpImage->ls.dwClusterCount + 2);

#else // WIN32

            printf(SZBACK SZCLUSTER, dwCurrentCluster + 2);

#endif // WIN32

        }

        dwWipe = min(dwCount, READWRITE_BUFFER_SIZE / (lpData->lpDrive->bs.dwSectorsPerCluster * lpData->lpDrive->bs.dwSectorSize));
        DPF("Wiping clusters %lu to %lu", dwCurrentCluster, dwWipe);

        if(!WriteSectorEx(lpData->lpDrive->bDrive, lpData->lpImage->lpbWriteBuffer, dwCurrentSector, dwWipe * lpData->lpDrive->bs.dwSectorsPerCluster, lpData->lpDrive->bs.dwSectorSize))
        {
            return ERROR_DRIVEWRITE;
        }

        dwCurrentCluster += dwWipe;
        dwCurrentSector += dwWipe * lpData->lpDrive->bs.dwSectorsPerCluster;
        dwCount -= dwWipe;
    }

    return ERROR_NONE;
}


int DumpInfo(LPEVERYTHING lpData)
{
    BGBOOTSECTOR                bsRaw;
    RRBOOTSECTOR                bs;
    int                         nRet;
    BYTE                        bFrag;
    char                        szText[0xFF];
    
    if(lpData->lpDrive->bDrive != 0xFF)
    {
        DumpBootSector("Target drive boot sector", &(lpData->lpDrive->bs));
    }

    if(lpData->lpImage->aszFragNames[0][0])
    {
        bFrag = 0;
        
        do
        {
            if(nRet = OpenFragment(bFrag, lpData, TRUE))
            {
                return nRet;
            }

            if(!bFrag)
            {
                if(FileRead(lpData->lpImage->fhImage, &bsRaw, sizeof(bsRaw)) != sizeof(bsRaw))
                {
                    return ERROR_FILEREAD;
                }

                if(nRet = ConvertBootSector(&bsRaw, &bs))
                {
                    return nRet;
                }
        
                DumpBootSector("Source drive boot sector", &bs);
            }

            sprintf(szText, "Image header for fragment %u", bFrag);
            DumpLeadSector(szText, &(lpData->lpImage->ls));

            if(nRet = CloseFragment(lpData))
            {
                return nRet;
            }
    
            lpData->lpImage->dwCurrentCluster = lpData->lpImage->ls.dwLastCluster + 1;
        } 
        while(++bFrag < lpData->lpImage->ls.bFragCount);
    }

    return ERROR_NONE;
}
