@echo off
setlocal ENABLEDELAYEDEXPANSION
if DEFINED _echo   echo on
if DEFINED verbose echo on

REM ********************************************************************
REM
REM This script creates symbolcd.inf in the %NTTREE%\symbolcd directory.
REM Symbolcd.inf is a list of the files to copy to the symbols cd.
REM
REM ********************************************************************

if NOT "%_NTTREE%" == "" goto GotNtTree

if "%PROCESSOR_ARCHITECTURE%"=="x86"   goto X86
if "%PROCESSOR_ARCHITECTURE%"=="ALPHA" goto ALPHA
echo PROCESSOR_ARCHITECTURE not defined.
goto EOF

:X86
set _NTTREE=%_nt386tree%
if DEFINED IA64 set _NTTREE=%_ntia64tree%
goto GotNtTree

:ALPHA
set _NTTREE=%_ntalphatree%
if DEFINED AXP64 set _NTTREE=%_ntaxp64tree%
goto GotNtTree

:GotNtTree

if "%binaries%" == "" set binaries=%_NTTREE%

if "%2" == "" (
    set lang=usa
    set AddOnCab=no
    set SymCabName=symbols
    set mybinaries=%binaries%
) else (
    set lang=%2
    set AddOnCab=no
    set SymCabName=symbols
    set mybinaries=%relbins%\%2
    set intlbins.txt=%_NTDRIVE%%_NTROOT%\public\tools\intlbins.txt
)

if /i "%1" == "clean" goto ExecuteAll
if /i "%1" == "cabs"  (
    set Cabs=TRUE
    goto ExecuteAll
)
goto Usage

:ExecuteAll

set NTTREE=%mybinaries%
set langroot=%NTTREE%\symbolcd\%lang%


REM 
REM Set some variables
REM

REM
REM Decide which build we are working on
REM debug vs. retail and i386 vs. alpha
REM

if /i "%PROCESSOR_ARCHITECTURE%" == "x86"   (
    if /i "%lang%" == "NEC_98" (
        set SUBDIR=NEC98
    ) else (
        set SUBDIR=i386
    )
)
if /i "%PROCESSOR_ARCHITECTURE%" == "alpha" (
    set SUBDIR=alpha
)

if /i "%NTDEBUG%" == "ntsd" (
    set FRECHK=Debug
) else (
    set FRECHK=Retail
)

REM
REM symerr.txt - error message pop-up
REM
set symerr.txt=%NTTREE%\symbolcd\%lang%\symerr.txt

REM
REM symbad.txt - don't write errors to the error log
REM for files in symbad.txt
REM
set symbad.txt=%_NTDRIVE%%_NTROOT%\public\tools\symbad.txt

REM
REM symlist.txt -  list of all files that we should
REM check symbols for 
REM

set symlist.txt=%langroot%\symlist.txt

REM
REM symlist.dbgonly.txt -  list of all files that we should
REM check symbols for, but only take the DBG's, don't take the
REM PDB's. 
REM

set symlist.dbgonly.txt=%langroot%\symlist.dbgonly.txt

REM
REM symbolcd.txt - list of all the files to put into
REM symbol cabs.  This is the result of running 
REM symchk on symlist.txt.
REM

set symbolcd.txt=%langroot%\symbolcd.txt

REM
REM symerror.log - files that had errors when symchk
REM was run on symlist.txt
REM

set symerror.log=%langroot%\symerror.log
set symerror.winnt32.log=%langroot%\symerror.winnt32.log
set symerror.noexport.log=%langroot%\symerror.noexport.log

REM
REM cabgenerr.log - error file when generating cabs
REM

set cabgenerr.log=%langroot%\cabgenerr.log

REM
REM exclude.txt - files that are not to be shipped
REM

set exclude.txt=%langroot%\exclude.txt

REM
REM symmake - program that creates the makefile and
REM ddfs for cab creation
REM
set symmake=%NTTREE%\symbolCD\symmake.exe

REM
REM symcopy = copies files from source tree and
REM
set symcopy=%NTTREE%\symbolCD\symcopy.cmd

REM
REM DDFDir - directory that contains the makefile and
REM ddfs for cab creation
REM
set DDFDir=%langroot%\ddf
set USDDFDir=%NTTREE%\symbolCD\usa\ddf

REM
REM Perl script that takes union and intersection of lists
REM
set MakeList=%_NTDRIVE%%_NTROOT%\public\tools\makelist.pl

REM
REM Perl
REM
set perl=perl.exe

REM
REM SYMCD - root of the symbol CD.  Everything under here
REM will appear as is on the symbol CD.
REM
set SYMCD=%NTTREE%\symbolCD\CD

REM
REM InfDestDir - final destination directory for the infs
REM and the final cab 
REM
set InfDestDir=%SYMCD%\Symbols\%SUBDIR%\%FRECHK%

REM
REM CabDestDir - directory where the individual cabs are
REM placed before they get merged into one cab
REM
set CabDestDir=%langroot%\cabs

REM
REM RedistDir has cabs that are on the CD, but are not
REM installed
REM
set RedistDir=%NTTREE%\symbolCD\redist

REM
REM If this is incremental, only create the cabs
REM

if /i "%Cabs%" == "TRUE" goto CreateCabs

REM
REM Otherwise, do a clean build of everything
REM 

REM
REM Delete existing files
REM

REM Delete the final cab
if EXIST %InfDestDir%\%SymCabName%.cab (
    del /q %InfDestDir%\%SymCabName%.cab
)

REM Delete the final inf
if EXIST %InfDestDir%\%SymCabName%.inf (
    del /q %InfDestDir%\%SymCabName%.inf
)

REM Delete the catalog file
if EXIST %InfDestDir%\%SymCabName%.CAT (
    del /q %InfDestDir%\%SymCabName%.CAT
)

REM Delete the %langroot% directory 
if EXIST %langroot% (
    rd /s /q %langroot%
)

REM Make the necessary directories
if not exist %InfDestDir% md %InfDestDir%
md %langroot%
md %DDFDir%
md %DDFDir%\temp
md %CabDestDir%

REM
REM Create the exclude list. Right now exclusion list consists of the
REM list of symbol files we are not supposed to ship.  Symbad.txt is not
REM included in the exclusion list because symbad.txt might not be up to
REM data and some of the files in symbad could have correct symbols.  We
REM want to get as many symbols as possible, as long as they pass symbol
REM checking. 
REM 

:CreateExcludeList

copy %_NTDRIVE%%_NTROOT%\public\tools\symdontship.txt %exclude.txt%


REM
REM Create the list of files.  This is the list of retail files that
REM we will attempt to find symbols for.
REM 

:CreateFileList

echo Creating the list of files to check symbols for 

REM Get the list of subdirectories that have dosnet and excdosnet in them.
REM And, make it compliant with the international builds
call getprods.cmd %lang%

REM Set the first one to be workstation, which is the current directory
set inflist= .
if defined _SRV set inflist=%inflist% srvinf
if defined _ENT set inflist=%inflist% entinf
if defined _DTC set inflist=%inflist% dtcinf

REM Take the union of dosnet.inf and excdosnet.inf for all subdirectorie
for %%a in (%inflist%) do (
    if /i EXIST %NTTREE%\%%a (
        %perl% %MakeList% -n %NTTREE%\%%a\dosnet.inf -o %DDFDir%\temp\dosnet.lst
        %perl% %MakeList% -q %NTTREE%\%%a\excdosnt.inf -o %DDFDir%\temp\drivers.lst
        %perl% %MakeList% -u %DDFDir%\temp\drivers.lst %DDFDir%\temp\dosnet.lst -o %DDFDir%\temp\%%a.lst
        if NOT exist %DDFDir%\temp\all.lst (
            copy %DDFDir%\temp\%%a.lst %DDFDir%\temp\all.lst
        ) else (
            %perl% %MakeList% -u %DDFDir%\temp\all.lst %DDFDir%\temp\%%a.lst -o %DDFDir%\temp\all.lst
        )
    )
)

REM People who own cabs have supplied lists of the files in the cabs
REM These lists are in the directory symbolcd\cablists
REM Add them to the end of the list we just created

echo Adding the files that are in cabs
if EXIST %NTTREE%\symbolcd\cablists (
    dir /a-d /b %NTTREE%\symbolcd\cablists > %DDFDir%\temp\cablists.lst
)

if EXIST %DDFDir%\temp\cablists.lst (
    for /f %%a in (%DDFDir%\temp\cablists.lst) do (
        type %NTTREE%\symbolcd\cablists\%%a >> %DDFDir%\temp\all.lst
    )
)


REM
REM Compute international lists
REM 
REM all.lst - list of files that need DBG and PDB info
REM dbgonly.lst - list of files that need DBG's only
REM

if /i "%AddOnCab%" == "no" goto EndIntlList

REM
REM To create all.lst subtract the US's all.lst
REM

echo Computing list of %lang% files that need DBG's and PDB's
copy %DDFDir%\temp\all.lst %DDFDir%\temp\allx.lst
%perl% %MakeList% -d %DDFDir%\temp\allx.lst %USDDFDir%\temp\all.lst -o %DDFDir%\temp\all.lst

REM
REM Now add the files that the international build relinked
REM

echo Adding files in %binaries%
dir /a-d /b %binaries% >> %DDFDir%\temp\all.lst > nul 2>&1 

for /F "tokens=1,2 delims= " %%a in (%intlbins.txt%) do (
    call :AddLanguageDir %%a %%b
)
echo Finished creating %DDFDir%\temp\all.lst

REM
REM Now %DDFDir%\temp\all.lst contains a list of all the binaries that
REM we will need to get DBG's and PDB's for.  All of the language specific
REM binaries are in this list.
REM

REM
REM Dbgonly.lst contains all the binaries that we just need dbg's for.
REM
REM To create dbgonly.lst subtract the new all.lst from the original 
REM all.lst (this was renamed to allx.lst)
REM

echo Computing list of files that only need DBG's
%perl% %MakeList% -d %DDFDir%\temp\allx.lst %DDFDir%\temp\all.lst -o %DDFDir%\temp\dbgonly.lst

REM
REM Compute which files are in dbgonly.lst are in %binaries%
REM

echo Computing which files that require dbg's only are in %NTTREE%
for /f %%b in (%DDFDir%\temp\dbgonly.lst) do (
    if /i EXIST %NTTREE%\%%b (
        echo %NTTREE%\%%b >> %DDFDir%\temp\dbgonly2.lst
    )
)
sort %DDFDir%\temp\dbgonly2.lst > %symlist.dbgonly.txt%

echo Finished creating %symlist.dbgonly.txt%

goto EndIntlList

:AddLanguageDir %1 %2
if /i "%2" == "%lang%" (
    echo Adding files in %binaries%\%1
    dir /a-d /b %binaries%\%1 >> %DDFDir%\temp\all.lst > nul 2>&1 
    echo Adding files in %binaries%\usa\%1
    dir /a-d /b %binaries%\usa\%1 >> %DDFDir%\temp\all.lst > nul 2>&1 
)
goto :EOF

:EndIntlList


REM
REM Find out which of these files are in %binaries%
REM Put the output in %symlist.txt%
REM

echo Computing which files are in %binaries%
for /f %%b in (%DDFDir%\temp\all.lst) do (
    if /i EXIST %NTTREE%\%%b (
        echo %NTTREE%\%%b >> %DDFDir%\temp\all2.lst 
    )
)
sort %DDFDir%\temp\all2.lst > %symlist.txt%
echo Finished creating %symlist.txt%

:SymbolCheck

REM
REM Create the list of files to copy onto the symbolcd.  
REM Result is %symbolcd.txt%
REM
REM /c output list of symbols.  THis is used as input to symmake for
REM    creating the makefile and ddfs.
REM /l input list of files to find symbols for
REM %NTTREE% - directory where files in %symlist.txt% are located
REM /s symbol search path
REM /e don't look for symbols for these files (i.e., files we should not
REM    ship symbols for
REM /x Don't write errors for these files to the error log
REM %symerror.log% Errors during symbol checking --- these should be 
REM    examined because the errors are genuine.
REM

echo Examining file headers to compute the list of symbol files ...
symchk.exe /c %symbolcd.txt% /l %symlist.txt% %NTTREE%\* /s %NTTREE%\symbols\retail /e %exclude.txt% /x %symbad.txt% > %symerror.log%


:SymbolCheckDbgOnly

REM
REM Do symbol checking for the dbgonly files
REM /d means only take the DBG's 
REM
if /i "%AddOnCab%" == "no" goto EndSymbolCheckDbgOnly

echo Examining file headers to compute the list of dbg only symbol files
symchk.exe /d /c %symbolcd.txt% /l %symlist.dbgonly.txt% %NTTREE%\* /s %NTTREE%\symbols\retail /e %exclude.txt% /x %symbad.txt% >> %symerror.log%


:EndSymbolCheckDbgOnly

:winnt32

REM
REM Recursively add files in the winnt32 and noexport subdirectory
REM
REM Don't use a list, just go through the subdirectories
REM

echo Adding files in the winnt32 subdirectory
symchk.exe /r /c %symbolcd.txt% %NTTREE%\winnt32\* /s %NTTREE%\symbols\winnt32 /x %symbad.txt% > %symerror.winnt32.log%

echo Adding files in the noexport subdirectory
symchk.exe /r /c %symbolcd.txt% %NTTREE%\noexport\* /s %NTTREE%\symbols\noexport /x %symbad.txt% > %symerror.noexport.log%

echo Adding *.sym files in the %NTTREE%\system32 directory
echo on
dir /b %NTTREE%\system32\*.sym > %DDFDir%\temp\symfiles.txt
for /f %%a in (%DDFDir%\temp\symfiles.txt) do (
    echo %%a,%%a,system32\%%a,16bit>> %symbolcd.txt%
)
echo off
echo Finished creating %symbolcd.txt%

REM
REM 
REM

:CopyFiles
echo Copying source files to correct place on CD
call %symcopy%

:CreateMakefile

REM
REM /i symbol inf that lists the symbol files to be cabbed
REM /o directory to place the DDF files and the makefile
REM /s Number of files to put into a cab
REM /m Tell inf that all cabs will be merged into %SymCabName%.inf
REM

REM
REM Create the makefile and all the DDF's
REM This also creates 
REM     %DDFDir%\symcabs.txt - a list of all the cabs
REM

echo Creating the makefile and the DDF's for creating the cabs

REM
REM This is the command if we are merging all the cabs into one
REM

%symmake% /m /c %SymCabName% /i %symbolcd.txt% /o %DDFDir% /s 400 /t %NTTREE% /x %CabDestDir% /y %InfDestDir% 

REM
REM Create the catalog
REM 

REM
REM Put the name of the catalog file into the header
REM Symmake already output %SymCabName%.CDF
REM

:CreateCatalog

echo Creating header for %SymCabName%.CDF
%perl% %MakeList% -h %SymCabName% -o %DDFDir%\%SymCabName%.CDF > nul 2>&1 

echo Adding files to %SymCabName%.CDF
echo ^<HASH^>%InfDestDir%\%SymCabName%.inf=%InfDestDir%\%SymCabName%.inf>> %DDFDir%\%SymCabName%.CDF
type %DDFDir%\%SymCabName%.CDF.noheader >> %DDFDir%\%SymCabName%.CDF

echo Creating %InfDestDir%\%SymCabName%.CAT
start "SymCabGen - Creating %SymCabName%.CAT" /MIN cmd /c "%NTTREE%\SymbolCD\SymCabGen.cmd %SymCabName% %DDFDir% CAT %InfDestDir%" %langroot%\%SymCabName%.CAT.log

:CreateCabs

REM
REM Create the Cabs
REM

if NOT EXIST %DDFDir%\cabs md %DDFDir%\cabs
if /i EXIST %DDFDir%\temp\*.txt del /f /q %DDFDir%\temp\*.txt

for /F "tokens=1" %%a in (%DDFDir%\symcabs.txt) do (
    sleep 1
    start "SymCabGen %%a" /MIN cmd /c "%NTTREE%\SymbolCD\SymCabGen.cmd %%a %DDFDir% CAB %CabDestDir% %langroot%\%%a.err" 
)

echo Waiting for symbol cabs to finish
:wait
sleep 5
if EXIST %DDFDir%\temp\*.txt goto wait


:Verify

REM
REM Verify that all the cabs are there
REM

if /i EXIST %DDFDir%\temp\*.txt del /f /q %DDFDir%\temp\*.txt

set AllCreated=TRUE
echo Verifying that all the cabs got created
for /F "tokens=1" %%a in (%DDFDir%\symcabs.txt) do (
    if NOT EXIST %CabDestDir%\%%a (
        set AllCreated=FALSE
        echo %CabDestDir%\%%a didn't get created ... Trying again
        sleep 1
        start "SymCabGen %%a" /MIN cmd /c "%NTTREE%\SymbolCD\SymCabGen.cmd %%a %CabDestDir%"
    )
)

if /i "%AllCreated%" == "TRUE" goto Final

REM
REM Wait for cabs to finish
REM

echo Trying again for symbol cabs to finish
:wait2
sleep 5
if EXIST %DDFDir%\temp\*.txt goto wait2

REM
REM This time print an error message if the cabs do not exist
REM

:FinalVerify
echo Verifying that all the cabs got created

set AllCreated=TRUE

if /i EXIST %cabgenerr.log% del /f /q %cabgenerr.log%
for /F "tokens=1" %%a in (%DDFDir%\symcabs.txt) do (
    if /i NOT EXIST %CabDestDir%\%%a (
        set AllCreated=FALSE
        echo %CabDestDir%\%%a was not created >> %cabgenerr.log%
    )
)

if /i EXIST %cabgenerr.log% (
    REM Create %symerr.txt%
    echo ****ATTENTION****>%symerr.txt%
    echo. >> %symerr.txt%
    echo At least one of the symbol cabs did not get created.>> %symerr.txt%>> %symerr.txt%
    echo. >> %symerr.txt%
    echo Check %NTTREE%\symbolcd\%lang%\cabgenerr.log to find out which ones didn't get created.>> %symerr.txt%
    echo. >> %symerr.txt%
    echo Try rerunning the following command ...>> %symerr.txt%
    echo. >> %symerr.txt%
    echo      ^"symbolcd.cmd cabs^">> %symerr.txt%
    echo. >> %symerr.txt%
    echo If that doesn't work try running>> %symerr.txt%
    echo. >> %symerr.txt%
    echo      ^"symbolcd.cmd clean^">> %symerr.txt%
    echo. >> %symerr.txt%
    echo If this fails to create the cabs then contact BarbKess, or the DBG alias.>> %symerr.txt%
    echo. >> %symerr.txt%
    start notepad %symerr.txt%
    goto errend
)

:Final

REM
REM Combine all the cabs into one cab
REM

set first=yes
for /F "tokens=1" %%a in (%DDFDir%\symcabs.txt) do (
    REM Make a copy of it
    REM copy %CabDestDir%\%%a %InfDestDir%\%%a

    if "!first!" == "yes" (
        set MergeCommand=load %CabDestDir%\%%a
        set first=no
    ) else (
        set MergeCommand=!MergeCommand! load %CabDestDir%\%%a merge
    )
)
set MergeCommand=%MergeCommand% save %InfDestDir%\%SymCabName%.cab

echo Merging cabs into %InfDestDir%\%SymCabName%.cab
cabbench.exe %MergeCommand%

REM
REM Copy the ADC no-install cab
REM
:CopyRedist

if EXIST %RedistDir%\*.cab (
    echo Copying cabs from %RedistDir% to %InfDestDir% 
    copy %RedistDir%\*.cab %InfDestDir%
)

REM
REM Check for errors
REM
for /f "tokens=3 delims= " %%a in (%symerror.log%) do  (
    if "%%a" == "FAILED" (
        copy %symerror.log% %symerror.log%.tmp
echo. >> %symerror.log%.tmp
echo Please contact the owner of any FAILED files.  The owner has >> %symerror.log%.tmp
echo possibly checked in a binary that was already split and >> %symerror.log%.tmp
echo didn't check in the correct .dbg and/or .pdb file. >> %symerror.log%.tmp
echo. >>symerror.log%.tmp
echo For more info contact the DBG alias and/or BarbKess >> %symerror.log%.tmp
echo. >>%symerror.log%.tmp
echo One or more files failed symbol checking and will not be >> %symerror.log%.tmp
echo on the symbols CD. You only need to pay attention to the >> %symerror.log%.tmp
echo individual files reported as FAILED.  >> %symerror.log%.tmp
echo. >>%symerror.log%.tmp
echo The statistic "SYMCHK: FAILED files" aboves may be high because >>%symerror.log%.tmp
echo the files that are in symbad.txt are reported as failing. >> %symerror.log%.tmp
echo Files that are individually written to the output>> %symerror.log%.tmp
echo failed and are not in symbad.txt. >> %symerror.log%.tmp
echo. >>%symerror.log%.tmp
echo The error file is %symerror.log% >>%symerror.log%.tmp
start notepad %symerror.log%.tmp
goto errend
    )
)

goto end

:Usage
echo Usage:  symbolcd clean [^<language^>]
echo         symcolcd cabs [^<language^>]
echo.
echo clean    Do a clean build of the cabs and all their
echo          associated lists and infs
echo cabs     Update the out of date cabs
echo.
echo language Language must be one of the languages in codes.txt.
echo          If language is not given on the command line, 
echo          usa is the default.
echo.
goto errend

:errend
endlocal
REM Set errorlevel to 1 in case of errors during execution.
REM Used by the build rule script "symbols.cmd" after calling "symbolcd.cmd".
seterror.exe 1
goto :EOF

:end
endlocal
REM Set errorlevel to 0 when the script finishes successfully.
REM Used by the build rule script "symbols.cmd" after calling "symbolcd.cmd".
seterror.exe 0
goto :EOF
