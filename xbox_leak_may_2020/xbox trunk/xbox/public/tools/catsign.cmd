@echo off
setlocal ENABLEEXTENSIONS
if DEFINED _echo   echo on
if DEFINED verbose echo on

REM --------------------------------------------------------------------------
REM  catsign.cmd - creates and signs Windows 2000 catalog files.
REM --------------------------------------------------------------------------


REM Define SCRIPT_NAME. Used by the logging scripts.
for %%i in (%0) do set script_name=%%~ni.cmd


REM ------------------------------------------------
REM  Set default Variables for script:
REM ------------------------------------------------

REM Initialize exitcode
set exitcode=0

set perl=perl

REM Set _svr, _ent, _dtc.  These indicate which products the current
REM language supports.
call getprods.cmd %lang%

REM Set inflist.  This is the list of subdirectories containing inf files.
set inflist=
if defined _SRV set inflist=%inflist% srvinf
if defined _ENT set inflist=%inflist% entinf
if defined _DTC set inflist=%inflist% dtcinf

set excludes=%_NTBINDIR%\public\tools\exclude.lst
set makelist.pl=%perl% %_NTBINDIR%\public\tools\makelist.pl

REM These are used to let us repeat a step without doing
REM the whole thing

set NoLists=No
set NoCDFs=No
set NoCATS=No
set NoSign=No
set NoBin=No
set NoTime=No

if /i "%PROCESSOR_ARCHITECTURE%" == "x86"   set nt5p=nt5prtx
if /i "%PROCESSOR_ARCHITECTURE%" == "alpha" set nt5p=nt5prta


REM ------------------------------------------------
REM  Get command-line options:
REM ------------------------------------------------

REM Provide usage.
for %%a in (./ .- .) do if ".%1." == "%%a?." goto Usage

if /i "%1" == "notime" set NoTime=Yes
if /i "%NoTime%" == "Yes" shift
if not "%1" == "" goto NotDefault
if "%bindrive%" == "" goto UseNTTREE
if "%binroot%" == "" goto UseNTTREE
goto UseBinDriveRoot
:UseNTTREE
if "%PROCESSOR_ARCHITECTURE%"=="x86"   goto X86
if "%PROCESSOR_ARCHITECTURE%"=="ALPHA" goto ALPHA
call errmsg.cmd "PROCESSOR_ARCHITECTURE not defined."
set exitcode=1
goto End

:X86
set binplacedir=%_nt386tree%
goto GotBinplaceDir

:ALPHA
set binplacedir=%_ntalphatree%
if DEFINED AXP64 set binplacedir=%_ntaxp64tree%
goto GotBinplaceDir

:UseBinDriveRoot
set binplacedir=%bindrive%%binroot%

:GotBinplaceDir
set catdir=%binplacedir%
set bin_in=%binplacedir%
set bin_out=%binplacedir%
goto GotDirs

:NotDefault
set catdir=%1
shift
set bin_in=%1
shift
set bin_out=%1
shift

:GotDirs

REM Setup the directory for the output
set catlists=%catdir%\lists
set catfiles=%catdir%\cats
set catCDFs=%catdir%\CDFs
set signedCATs=%catdir%\testsig
set tempdir=%catdir%\temp

REM Set directory where the binaries exist
if not defined binaries set binaries=%bin_in%

:SwitchLoop
if "%1" == "" goto EndLoop
for %%a in (./ .- .) do if ".%1." == "%%a?."  goto Usage

if /i "%1" == "NoList"    set NoLists=Yes&&    goto NextArg
if /i "%1" == "NoCDF"     set NoCDFs=Yes&&     goto NextArg
if /i "%1" == "NoCAT"     set NoCATS=Yes&&     goto NextArg
if /i "%1" == "NoSign"    set NoSign=Yes&&     goto NextArg
if /i "%1" == "NoBin"     set NoBin=Yes&&      goto NextArg

echo.
call errmsg.cmd "%1 not understood"
set exitcode=1
goto Usage

:NextArg
shift
goto SwitchLoop

:EndLoop

if /i "%NoLists%" == "Yes" goto EndCreateLists

REM Delete hash.tmp used by congeal
if exist %TMP%\hash.tmp del %TMP%\hash.tmp

REM ----------------------------------------------------------
REM  Prepare a place for the output files
REM ----------------------------------------------------------

if NOT exist %catdir%   md %catdir%
if NOT exist %catlists% md %catlists%
if NOT exist %catfiles% md %catfiles%
if NOT exist %catCDFs%  md %catCDFs%
if NOT exist %signedCATs% md %signedCATs%
if NOT exist %tempdir%  md %tempdir%


REM ----------------------------------------------------------
REM  Create nt5print.cat
REM ----------------------------------------------------------

:CreateNt5prt

call logmsg.cmd "Creating print lists ..."
infflist.exe %bin_in%\ntprint.inf > %catlists%\%nt5p%.lst
if errorlevel 1 (
    call errmsg.cmd "infflist.exe %bin_in%\ntprint.inf failed"
    set exitcode=1
)

:EndNt5prt

REM ----------------------------------------------------------
REM  Handle special cases
REM ----------------------------------------------------------


if exist %alt_path%\desktop.in_ (
  copy %alt_path%\desktop.in_ %tempdir%\desktop.ini
)
if exist %bin_in%\desktop.in_ (
  copy %bin_in%\desktop.in_ %tempdir%\desktop.ini
)

if /i NOT exist %bin_in%\dump\catalogs md %bin_in%\dump\catalogs

REM Make winnt.exe show up as signed
if NOT DEFINED AXP64 copy %bin_in%\winnt.exe %bin_in%\dump\catalogs
REM Make relnotes.htm show up as signed
if exist %alt_path%\relnotes.htm (
  copy %alt_path%\relnotes.htm %bin_in%\dump\catalogs
) else (
  copy %bin_in%\relnotes.htm %bin_in%\dump\catalogs
)

REM Make the selfreg infs show up as signed
if exist %alt_path%\dump\selfreg* (
  copy %alt_path%\dump\selfreg* %bin_in%\dump\catalogs
) else (
  copy %bin_in%\dump\selfreg* %bin_in%\dump\catalogs
)

REM Make sure these files exist
for %%i in (%bin_in%\dump\catalogs\winnt.exe %bin_in%\dump\catalogs\relnotes.htm %bin_in%\dump\catalogs\selfreg*) do (
    if /i NOT exist %%i (
        call errmsg.cmd "%%i not found"
        set exitcode=1
    )
)

REM ----------------------------------------------------------
REM  Create nt5inf and nt5 lists
REM ----------------------------------------------------------


call logmsg.cmd "Creating nt5 and nt5inf lists ..."
copy %excludes% %catlists%\exclude.lst

REM Take printer files and excluded files out of dosnet.lst
%makelist.pl% -n %bin_in%\dosnet.inf -o %catlists%\dosnet.lst
%makelist.pl% -d %catlists%\dosnet.lst %catlists%\%nt5p%.lst -o %catlists%\dosnet.lst
%makelist.pl% -d %catlists%\dosnet.lst %catlists%\exclude.lst -o %catlists%\dosnet.lst

REM Supports languages without a SRV product
if "%_srv%" == "" (

    REM Find the inf files in the directory that are also in dosnet.lst
    REM Here we need to get list of inf files in a different way than for
    REM languages which have subdirectories for other products.
    dir /b %bin_in%\*.inf > %catlists%\inf.lst
    %makelist.pl% -i %catlists%\inf.lst %catlists%\dosnet.lst -o %catlists%\inf.lst

    REM Since there are no product subdirectories, allinfs.lst should be
    REM the same as inf.lst
    copy /y %catlists%\inf.lst %catlists%\allinfs.lst

) else (
    for %%a in (%inflist%) do (
        if NOT exist %catlists%\%%a md %catlists%\%%a

        REM Take printer files and excluded files out of subdirectory's dosnet.lst
        %makelist.pl% -n %bin_in%\%%a\dosnet.inf -o %catlists%\%%a\dosnet.lst
        %makelist.pl% -d %catlists%\%%a\dosnet.lst %catlists%\%nt5p%.lst -o %catlists%\%%a\dosnet.lst
        %makelist.pl% -d %catlists%\%%a\dosnet.lst %catlists%\exclude.lst -o %catlists%\%%a\dosnet.lst

        REM Find the inf files in the subdirectory that are also in dosnet.lst
        %makelist.pl% -c %bin_in%\%%a -o %catlists%\%%a\inf.lst
        %makelist.pl% -i %catlists%\%%a\inf.lst %catlists%\%%a\dosnet.lst -o %catlists%\%%a\inf.lst

        REM Find the union of the infs in the subdirectories.  Put this in allinfs.lst
        if NOT exist %catlists%\allinfs.lst copy %catlists%\%%a\inf.lst %catlists%\allinfs.lst
        %makelist.pl% -u %catlists%\allinfs.lst %catlists%\%%a\inf.lst -o %catlists%\allinfs.lst
    )
)


REM Use allinfs.lst and dosnet.lst to compute a inf.lst for wks and subdirectories
for %%a in (. %inflist%) do (
    %makelist.pl% -i %catlists%\%%a\dosnet.lst %catlists%\allinfs.lst -o %catlists%\%%a\inf.lst
    %makelist.pl% -d %catlists%\%%a\dosnet.lst %catlists%\allinfs.lst -o %catlists%\%%a\dosnet.lst
)

REM Now, nt5.lst is the intersection of all the dosnet.lst's
copy %catlists%\dosnet.lst %catlists%\nt5.lst
for %%a in (%inflist%) do (
    %makelist.pl% -i %catlists%\nt5.lst %catlists%\%%a\dosnet.lst -o %catlists%\nt5.lst
)

REM Add drivers.lst to nt5.lst
%makelist.pl% -q %bin_in%\excdosnt.inf -o %catlists%\drivers.lst


for %%a in (%inflist%) do (
  (if EXIST %bin_in%\%%a (
    %makelist.pl% -q %bin_in%\%%a\excdosnt.inf -o %catlists%\drivers_%%a.lst
  ))
  (if EXIST %catlists%\drivers_%%a.lst (
    %makelist.pl% -u %catlists%\drivers_%%a.lst %catlists%\drivers.lst -o %catlists%\drivers.lst
  ))
)

%makelist.pl% -d %catlists%\drivers.lst %catlists%\%nt5p%.lst -o %catlists%\drivers.lst
%makelist.pl% -u %catlists%\drivers.lst %catlists%\nt5.lst -o %catlists%\nt5.lst

REM Compute which files need to be moved to the inf list and move them
for %%a in (. %inflist%) do (
    %makelist.pl% -d %catlists%\%%a\dosnet.lst %catlists%\nt5.lst -o %catlists%\%%a\diff.lst
    %makelist.pl% -u %catlists%\%%a\inf.lst %catlists%\%%a\diff.lst -o %catlists%\%%a\inf.lst
)

REM Make sure these lists all have been created
for %%i in (exclude.lst allinfs.lst drivers.lst) do (
    if /i NOT exist %catlists%\%%i (
        call errmsg.cmd "%catlists%\%%i not found%
        set exitcode=1
    )
)
for %%i in (. %inflist%) do (
    for %%j in (dosnet.lst inf.lst) do (
        if /i NOT exist %catlists%\%%i\%%j (
            call errmsg.cmd "%catlists%\%%i\%%j not found%
            set exitcode=1
        )
    )
)

:EndCreateLists


REM ------------------------------------------------------
REM  Put the search paths in ... create the CDF
REM ------------------------------------------------------

:CreateCDFs

if /i "%NoCDFs%" == "Yes" goto EndCreateCDFs

call logmsg.cmd "Creating nt5 and nt5inf CDFs ..."

%makelist.pl% -m %catlists%\%nt5p%.lst -p -x -s %bin_in% %alt_path% -o %catCDFS%\%nt5p%.CDF -h %nt5p%

%makelist.pl% -m %catlists%\nt5.lst -p -s %bin_in% %tempdir% %alt_path% -o %catCDFs%\nt5.CDF -h nt5 -x

%makelist.pl% -m %catlists%\inf.lst -p -x -s %bin_in% %alt_path% -o %catCDFs%\nt5inf.CDF -h nt5inf

for %%a in (%inflist%) do (
    if NOT exist %catCDFs%\%%a md %catCDFs%\%%a
)

if defined _SRV (
  %makelist.pl% -m %catlists%\srvinf\inf.lst -p -s %bin_in%\srvinf %bin_in% %alt_path%\srvinf %alt_path% -o %catCDFs%\srvinf\nt5inf.CDF -h nt5inf -x
)
if defined _ENT (
  %makelist.pl% -m %catlists%\entinf\inf.lst -p -s %bin_in%\entinf %bin_in%\srvinf %bin_in% %alt_path%\entinf %alt_path%\srvinf %alt_path% -o %catCDFs%\entinf\nt5inf.CDF -h nt5inf -x
)
if defined _DTC (
  %makelist.pl% -m %catlists%\dtcinf\inf.lst -p -s %bin_in%\dtcinf %bin_in%\entinf %bin_in%\srvinf %bin_in% %alt_path%\dtcinf %alt_path%\entinf %alt_path%\srvinf %alt_path% -o %catCDFs%\dtcinf\nt5inf.CDF -h nt5inf -x
)

REM Make sure these CDFs have been created
for %%i in (%nt5p%.CDF nt5.CDF) do (
    if /i NOT exist %catCDFs%\%%i (
        call errmsg.cmd "%catCDFs%\%%i not found%
        set exitcode=1
    )
)
for %%i in (. %inflist%) do (
    for %%j in (nt5inf.CDF) do (
        if /i NOT exist %catCDFs%\%%i\%%j (
            call errmsg.cmd "%catCDFs%\%%i\%%j not found%
            set exitcode=1
        )
    )
)

REM -----------------------------------------------------
REM  Add files in subdirectories to nt5.CDF
REM -----------------------------------------------------

call logmsg.cmd "Adding subdirectories to nt5.CDF ..."

REM Make a list of all the files in the subdirs

if DEFINED alt_path goto enum_intl

if /i EXIST %catlists%\subdirfiles.lst del %catlists%\subdirfiles.lst

for /F "eol=; tokens=1,2* delims=, " %%i in (%_NTBINDIR%\public\tools\subdirs.lst) do (

    call logmsg.cmd "Processing %%i"

    if /i "%%j" == "recursive" (
        dir /s /a-d /b %binaries%\%%i >> %catlists%\subdirfiles.lst
    )  else (
        dir /a-d /b %binaries%\%%i > %catlists%\subdirtmp.lst
        for /f "tokens=1" %%a in (%catlists%\subdirtmp.lst) do (
            if /i NOT "%%a" == "" (
                echo %binaries%\%%i\%%a>> %catlists%\subdirfiles.lst
            )
        )
    )
)

goto enum_done

:enum_intl

if /i EXIST %catlists%\subdirfiles.lst del %catlists%\subdirfiles.lst
if exist %catlists%\l1.lst del %catlists%\l1.lst
if exist %catlists%\l2.lst del %catlists%\l2.lst
if exist %catlists%\l3.lst del %catlists%\l3.lst
if exist %catlists%\l4.lst del %catlists%\l4.lst
if exist %catlists%\full.lst del %catlists%\full.lst
if exist %catlists%\final.lst del %catlists%\final.lst

for /F "eol=; tokens=1,2* delims=, " %%i in (%_ntbindir%\public\tools\subdirs.lst) do (
  call logmsg.cmd "Processing %%i"
  if /i "%%j" == "recursive" (

    REM Append every file that can be found
    dir /s /b /a-d %relbins%\%lang%\%%i>>%catlists%\l1.lst
    dir /s /b /a-d %alt_path%\%%i>>%catlists%\l2.lst
  ) else (

    REM Append every file that can be found for flat dirs.
    for %%k in (%relbins%\%lang%\%%i\*) do echo %%i\%%~nxk>>%catlists%\l1.lst
    for %%k in (%alt_path%\%%i\*) do echo %%i\%%~nxk>>%catlists%\l2.lst
  )
)

REM Filter out base path to make complete list of files with relative paths
perl -pe "$foo='%relbins%\%lang%\\';$foo=~s/\\/\\\\/g;s/$foo//i" %catlists%\l1.lst>%catlists%\l3.lst
perl -pe "$foo='%alt_path%\\';$foo=~s/\\/\\\\/g;s/$foo//i" %catlists%\l2.lst>%catlists%\l4.lst

REM Get the union of localized and alternate file lists
%makelist.pl% -u %catlists%\l3.lst %catlists%\l4.lst -o %catlists%\full.lst

REM Find which file exists where
for /f "tokens=1" %%i in (%catlists%\full.lst) do if exist %relbins%\%lang%\%%i (
  echo %relbins%\%lang%\%%i>>%catlists%\subdirfiles.lst
) else (
  echo %alt_path%\%%i>>%catlists%\subdirfiles.lst
)

:enum_done

REM Go through the list of all the files in the subdirs and
REM add them to the CDF.  Weed out the blank lines.

for /f "tokens=1" %%a in (%catlists%\subdirfiles.lst) do (
   if /i NOT "%%a" == "" (
     echo ^<hash^>%%a=%%a>> %catCDFs%\nt5.CDF
   )
)

REM -------------------------------------------------
REM  Special cased files - not in layout.inf 
REM -------------------------------------------------

REM Add remote boot files to nt5.cat
for /f %%a in (%_ntdrive%\nt\public\tools\specsign.lst) do echo ^<hash^>%bin_in%\%%a=%bin_in%\%%a>> %catCDFs%\nt5.CDF

if defined _srv (
  REM Sign wks layout.inf in server nt5inf.cat for remote boot
  echo ^<hash^>%bin_in%\layout.inf=%bin_in%\layout.inf>> %catCDFs%\srvinf\nt5inf.CDF
)

REM sign autorun.exe for all available product suites.
echo ^<hash^>%bin_in%\autorun.exe=%bin_in%\autorun.exe>> %catCDFs%\nt5inf.CDF
for %%a in (%inflist%) do (
	if EXIST %catCDFs%\%%a\nt5inf.cdf echo ^<hash^>%bin_in%\%%a\autorun.exe=%bin_in%\%%a\autorun.exe>> %catCDFs%\%%a\nt5inf.CDF
)

REM  Kick off generation of hash.txt used for updating catalogs for backprops.
if "%OFFICIAL_BUILD_MACHINE%"=="1" (
	call logmsg.cmd /t "%TMP%\hash.tmp"
	start "hash" %comspec% /c %_BLDDRIVE%\bldtools\hash.cmd
)

REM -------------------------------------------------
REM  Create the catalog files
REM -------------------------------------------------

:CreateCATs

if /i "%NoCATs%" == "Yes" goto EndCreateCATs



    call logmsg.cmd "Creating %catfiles%\%nt5p%.CAT ..."
    makecat -n -v %catCDFS%\%nt5p%.CDF > %catCDFs%\%nt5p%.log
    if errorlevel 1 (
        call errmsg.cmd "makecat -n -v %catCDFS%\%nt5p%.CDF failed"
        set exitcode=1
    )
    copy %nt5p%.CAT %catfiles%
    del %nt5p%.CAT

    call logmsg.cmd "Creating %catfiles%\nt5.CAT ..."
    makecat -n -v %catCDFS%\nt5.CDF > %catCDFs%\nt5.log
    if errorlevel 1 (
        call errmsg.cmd "makecat -n -v %catCDFS%\nt5.CDF failed"
        set exitcode=1
    )
    copy nt5.CAT %catfiles%
    del nt5.CAT

    call logmsg.cmd "Creating %catfiles%\nt5inf.CAT ..."
    makecat -n -v %catCDFS%\nt5inf.CDF > %catCDFs%\nt5inf.log
    if errorlevel 1 (
        call errmsg.cmd "makecat -n -v %catCDFS%\nt5inf.CDF failed"
        set exitcode=1
    )
    copy nt5inf.CAT %catfiles%
    del nt5inf.CAT

    for %%a in (%inflist%) do (
        if NOT exist %catfiles%\%%a md %catfiles%\%%a
        call logmsg.cmd "Creating %catfiles%\nt5inf.CAT ..."
        makecat -n -v %catCDFS%\%%a\nt5inf.CDF > %catCDFs%\nt5inf.%%a.log
        if errorlevel 1 (
            call errmsg.cmd "makecat -n -v %catCDFS%\%%a\nt5inf.CDF failed"
            set exitcode=1
        )
        copy nt5inf.CAT %catfiles%\%%a
        del nt5inf.CAT
    )

REM Make sure these CATs have been created
for %%i in (%nt5p%.CAT nt5.CAT) do (
    if /i NOT exist %catfiles%\%%i (
        call errmsg.cmd "%catfiles%\%%i not found%
        set exitcode=1
    )
)
for %%i in (. %inflist%) do (
    for %%j in (nt5inf.CAT) do (
        if /i NOT exist %catfiles%\%%i\%%j (
            call errmsg.cmd "%catfiles%\%%i\%%j not found%
            set exitcode=1
        )
    )
)

:EndCreateCATs

REM -------------------------------------------------
REM  Create the catalog files with the test signature
REM -------------------------------------------------

:TestSign

if /i "%NoSign%" == "Yes" goto EndTestSign

copy %catfiles%\%nt5p%.CAT %signedCATs%
call logmsg.cmd "Signing %signedCATs%\%nt5p%.CAT"
if /i "%NoTime%" == "No" (call ntsign.cmd %signedCATs%\%nt5p%.CAT) else (call ntsign.cmd %signedCATs%\%nt5p%.CAT notime)
if errorlevel 1 (
    call errmsg.cmd "ntsign.cmd %signedCATs%\%nt5p%.CAT failed"
    set exitcode=1
)

copy %catfiles%\nt5.CAT %signedCATs%
call logmsg.cmd "Signing %signedCATs%\nt5.CAT"
if /i "%NoTime%" == "No" (call ntsign.cmd %signedCATs%\nt5.CAT) else (call ntsign.cmd %signedCATs%\nt5.CAT notime)
if errorlevel 1 (
    call errmsg.cmd "ntsign.cmd %signedCATs%\nt5.CAT failed"
    set exitcode=1
)

copy %catfiles%\nt5inf.CAT %signedCATs%
call logmsg.cmd "Signing %signedCATs%\nt5inf.CAT"
if /i "%NoTime%" == "No" (call ntsign.cmd %signedCATs%\nt5inf.CAT) else (call ntsign.cmd %signedCATs%\nt5inf.CAT notime)
if errorlevel 1 (
    call errmsg.cmd "ntsign.cmd %signedCATs%\nt5inf.CAT failed"
    set exitcode=1
)

for %%a in (%inflist%) do (
  (if NOT exist %signedCATs%\%%a (
    md %signedCATs%\%%a
  ))
  copy %catfiles%\%%a\nt5inf.CAT %signedCATs%\%%a
  call logmsg.cmd "Signing %signedCATs%\%%a\nt5inf.CAT"
  (if /i "%NoTime%" == "No" (
     call ntsign.cmd %signedCATs%\%%a\nt5inf.CAT
   ) else (
     call ntsign.cmd %signedCATs%\%%a\nt5inf.CAT notime
  ))
  if errorlevel 1 (
    call errmsg.cmd "ntsign.cmd %signedCATs%\%%a\nt5inf.CAT failed"
    set exitcode=1
  )
)

:EndTestSign

REM ------------------------------------------------
REM  Run mp2up.cmd to make a catalog for uniproc
REM ------------------------------------------------

call logmsg.cmd "Running mp2up.cmd ..."
call mp2up.cmd

if "%ERRORLEVEL%" == "1" (
	call errmsg.cmd "mp2up.cmd failed: see %LOGFILE%"
	set exitcode=1
) else (
	call logmsg.cmd "Finished mp2up.cmd successfully."
)


REM ---------------------------
REM  Binplace the catalog files
REM ---------------------------

:BinPlace

if /i "%NoBin%" == "Yes" goto EndBinPlace

call logmsg.cmd "Binplacing %nt5p%.CAT, nt5.CAT, nt5inf.CAT"
copy %signedCATs%\%nt5p%.CAT %bin_out%
copy %signedCATs%\nt5.CAT %bin_out%
copy %signedCATs%\nt5inf.CAT %bin_out%

for %%a in (%inflist%) do (
  call logmsg.cmd "Binplacing %%a\nt5inf.CAT"
  copy %signedCATs%\%%a\nt5inf.CAT %bin_out%\%%a
)

REM Make sure these CATs have been binplaced
for %%i in (%nt5p%.CAT nt5.CAT) do (
    if /i NOT exist %bin_out%\%%i (
        call errmsg.cmd "%bin_out%\%%i not found%
        set exitcode=1
    )
)
for %%i in (. %inflist%) do (
    for %%j in (nt5inf.CAT) do (
        if /i NOT exist %bin_out%\%%i\%%j (
            call errmsg.cmd "%bin_out%\%%i\%%j not found%
            set exitcode=1
        )
    )
)


if NOT "%OFFICIAL_BUILD_MACHINE%"=="1" goto End
call IsIntlBld.cmd
if not errorlevel 1 goto End


:Test_Binplace
REM --------------------------------------------
REM  Binplace the CDF files for testing purposes
REM --------------------------------------------

REM Set the directory for binplacing the binaries
set cdf_out=%bin_out%\cdf

copy %catCDFs%\%nt5p%.CDF %cdf_out%
copy %catCDFs%\%nt5p%.log %cdf_out%
copy %catCDFs%\nt5.CDF %cdf_out%
copy %catCDFs%\nt5.log %cdf_out%
copy %catCDFs%\nt5inf.CDF %cdf_out%
copy %catCDFs%\nt5inf.log %cdf_out%

for %%a in (%inflist%) do (
  copy %catCDFs%\%%a\nt5inf.CDF %cdf_out%\nt5inf.%%a.cdf
  copy %catCDFs%\nt5inf.%%a.log %cdf_out%
)
call :CopyCdf
goto End
:end_test_binplace


REM ------------------------------------------------
REM  Copy cdf's and cdf.err's to cdf.old directory
REM ------------------------------------------------
:CopyCdf

set bldno=
set bldno_10=

for /f %%a in (%_ntbindir%\bldno.log) do set bldno=%%a

set /a bldno_10 = %bldno% - 10

if not exist %binaries%\cdf\cdf.%bldno% md %binaries%\cdf\cdf.%bldno%
copy %binaries%\cdf\*.cdf %binaries%\cdf\cdf.%bldno%\*.cdf
copy %binaries%\cdf\*.log %binaries%\cdf\cdf.%bldno%\*.log
rd /s /q %binaries%\cdf\cdf.%bldno_10%
:EndCopyCdf

goto :EOF



REM ------------------------------------------------
REM  Display Usage:
REM ------------------------------------------------

:Usage

echo.
echo
echo Usage:  catsign.cmd out_dir bin_in bin_out
echo         [NoCDF] [NoCAT] [NoSign] [NoTime] [NoBin]
echo.
echo Creates catfiles for Windows 2000
echo Creates nt5.CAT, nt5inf.CAT, nt5prtx.CAT, nt5prta.CAT
echo.
echo out_dir        Directory for all of the output
echo bin_in         Binaries directory (where ntprint.inf,
echo                    dosnet.inf exist)
echo bin_out        Directory for binplacing the CAT files
echo.
echo Note: If none of these are set (catsign is run w/o args), these will
echo       all default to %bindir%%binroot% (just like bindsys.cmd)
echo.
echo [NoList]       Don't recreate the lists
echo [NoCDF]        Don't recreate the CDFs
echo [NoCAT]        Don't create the CATS
echo [NoSign]       Don't sign with the test signature
echo [NoTime]       Don't timestamp the test signature
echo [NoBin]        Don't binplace the CAT files
echo.

:end
endlocal & seterror.exe %exitcode%
