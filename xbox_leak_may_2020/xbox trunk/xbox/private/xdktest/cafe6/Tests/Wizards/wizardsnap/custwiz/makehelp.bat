@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by APWIZTES.HPJ. >hlp\apwiztes.hm
echo. >>hlp\apwiztes.hm
echo // Dialogs (IDD_*) >>hlp\apwiztes.hm
makehm IDD_,HIDD_,0x20000 resource.h >>hlp\apwiztes.hm
echo. >>hlp\apwiztes.hm

REM -- Make help for Project APWIZTES

call hc31 apwiztes.hpj
if exist windebug copy apwiztes.hlp windebug
if exist winrel copy apwiztes.hlp winrel
