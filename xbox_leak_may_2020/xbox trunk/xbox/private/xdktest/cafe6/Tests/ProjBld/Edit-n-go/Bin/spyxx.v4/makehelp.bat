@echo off
REM -- Generate help map files from command and resource header files
echo // Spy++ Help Mapping File>spy.hm
echo // MAKEHELP.BAT generated Help Map file.  Used by SPY.HPJ. >>spy.hm
echo // >>spy.hm
echo // This file contains help contexts for Spy++.  It is automatically>>spy.hm
echo // generated from the actual source code by MAKEHELP.BAT.  Please do not>>spy.hm
echo // edit this file manually, since any such changes will be obliterated>>spy.hm
echo // the next time the file is rebuilt.>>spy.hm
echo // >>spy.hm
echo // Owner: PatBr>>spy.hm
echo //////////////////////////////////////////////////////////////////////////////>>spy.hm
echo. >>spy.hm
echo // Commands (ID_* and IDM_*) >>spy.hm
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>spy.hm
echo. >>spy.hm
echo // AFX Commands (ID_*) >>spy.hm
makehm ID_APP_,HIDM_APP_,0x10000 ID_WINDOW_,HIDM_WINDOW_,0x10000 ID_HELP_,HIDM_HELP_,0x10000 ID_VIEW_,HIDM_VIEW_,0x10000 d:\devtools\mfc\include\afxres.h >>spy.hm
echo. >>spy.hm
echo // Dialogs (IDD_*) >>spy.hm
makehm IDD_,HIDD_,0x20000 resource.h >>spy.hm
echo. >>spy.hm
echo // Resources (IDR_*) (to enable help for views: Processes, Threads, Windows, Messages)>>spy.hm
makehm IDR_,HIDR_,0x20000 resource.h >>spy.hm
echo. >>spy.hm
