@echo off
if DEFINED _echo   echo on
if DEFINED verbose echo on
setlocal enableextensions

REM ---------------------------------------------------------------------------
REM  CRYPTO.CMD  -  script for CSP signing. 
REM
REM  Overview
REM
REM  This script handles CSP signing, which refers to the type of signing
REM  applied to cryptographic service providers and security packages.
REM  CSP signing is separate from PRS signing, and only affects a handful
REM  of files.  Similar to PRS signing, CSP signing involves test signing
REM  daily builds and real signing RTM builds.  The latter is sometimes
REM  more specifically referred to as vault signing, since the real 
REM  signature gets generated from a hardware device secured in a vault.
REM  CSP signatures are inserted into the image itself.
REM
REM  This script performs several tasks:
REM  1) Inserts a cert into Terminal Services files.
REM  2) CSP (test) signs CSPs and security provider files.
REM  3) Creates encrypted installers that contain high encryption versions
REM     of some CSPs and security providers.
REM  4) Verifies the signatures on the signed files are valid.
REM
REM  Error handling in this script
REM
REM  This script sets errorlevel=1 and returns to the caller on error.
REM  Likewise, the calling script should check errorlevel and take the
REM  appropriate action on error, which may involve returning to its
REM  caller in the same way via "goto errend".  This paradigm allows
REM  errors to be propagated up from the lowest to highest level.
REM  For example:
REM
REM    call crypto.cmd
REM    if errorlevel 1 (
REM       call errmsg.cmd "crypto.cmd failed"
REM       goto errend
REM    )
REM      
REM    :errend
REM    seterror.exe 1
REM    goto :EOF
REM
REM  All errors are logged to %LOGFILE%
REM ---------------------------------------------------------------------------

REM Server that doles out test signatures via signcsp.exe
set CryptoServer=enigma

for %%a in (./ .- .) do if ".%1." == "%%a?." goto Usage

REM  
REM  Define script name used for logging.
REM

for %%i in (%0) do set script_name=%%~ni.cmd

REM
REM Save the command line
REM

for %%i in (%0) do set cmdline=%%~f0 %*

REM
REM Define LOGFILE, to be used by the logging scripts.
REM As this script is typically invoked from a wrapper (congeal.cmd),
REM LOGFILE is often already defined. Do not overwrite LOGFILE if already defined.
REM

if defined LOGFILE goto logfile_defined
for %%i in (%script_name%) do (
  set LOGFILE=%tmp%\%%~ni.log
)
if exist %LOGFILE% del /f %LOGFILE%
:logfile_defined

REM
REM Mark the beginning of script's execution.
REM

call logmsg.cmd /t "START '%cmdline%'" %logfile%

REM Define "mybinaries" as the directory of binaries to be processed by this
REM script. On US build machines, the files to be processed reside in %binaries%.
REM On international build machines the files to be processed reside in %relbins%,
REM the directory containing the localized version of the files in %binaries%.

if not defined mybinaries set mybinaries=%binaries%

if "%mybinaries%" == "" goto UseNTTREE
goto GotDirs

:UseNTTREE

REM
REM 64-bit razzle doesn't set binaries by default
REM

if "%PROCESSOR_ARCHITECTURE%"=="x86"   goto X86
if "%PROCESSOR_ARCHITECTURE%"=="ALPHA" goto ALPHA
echo PROCESSOR_ARCHITECTURE not defined.
goto End

:X86
set mybinaries=%_nt386tree%
if DEFINED IA64 set mybinaries=%_ntia64tree%
if DEFINED IA64 set SignCsps=1
goto GotDirs

:ALPHA
set mybinaries=%_ntalphatree%
if DEFINED AXP64 set mybinaries=%_ntaxp64tree%
set SignCsps=1
goto GotDirs

:GotDirs

for %%i in (mybinaries _ntbindir) do if not defined %%i (
  call errmsg.cmd "'%%i' not defined"
  goto errend
)

REM To facilitate specifying which processing happens per file, per language,
REM per architecture, etc, let the table file define and drive this.
set table=%_ntbindir%\public\tools\crypto.txt
if not exist %table% (
  call errmsg.cmd "File '%table%' not found"
  goto errend
)

REM International builds perform a subset of the normal operations in this script
if not exist %_ntbindir%\public\tools\isintlbld.cmd (
  call errmsg.cmd "File '%_ntbindir%\public\tools\isintlbld.cmd' not found"
  goto errend
)
call isintlbld.cmd
if errorlevel 1 (
  set isintlbld=
) else (
  set isintlbld=1
)

REM Process command line keyword arguments
if /i "%1" == "VerifyConnection" (
  call :VerifyConnection %CryptoServer% & if errorlevel 1 goto errend
  goto end
)
if /i "%1" == "VerifyFiles" goto VerifyFiles
if not "%1"=="" (
  call errmsg.cmd "'%1' is not a recognized keyword argument"
  goto errend
)

REM By policy, signed files are binary identical across languages
if defined isintlbld goto SignItDone

echo.
echo ------------------------------------------------
echo  Add cert to Terminal Services files
echo ------------------------------------------------

REM Some TS files need to have a cert added prior to being
REM stuffed into their respective encrypted installer
REM This is a temporary workaround until RTM.

for /f "eol=; tokens=1,2,6,7 delims= " %%a in (%table%) do @if /i "%%c"=="yes" call :TSCert1File %mybinaries%\%%b\%%a %%c %%d & if errorlevel 1 goto errend

echo.
echo ------------------------------------------------
echo  CSP-sign binaries
echo ------------------------------------------------

REM Platform partially determines the need to test sign
set myplatform=
if "%PROCESSOR_ARCHITECTURE%"=="x86"                    set myplatform=i386
if "%PROCESSOR_ARCHITECTURE%"=="x86"   if defined IA64  set myplatform=ia64
if "%PROCESSOR_ARCHITECTURE%"=="ALPHA"                  set myplatform=alpha
if "%PROCESSOR_ARCHITECTURE%"=="ALPHA" if defined AXP64 set myplatform=axp64
if not defined myplatform (
  call errmsg.cmd "unrecognized platform"
  goto errend
)

pushd \
for /f "eol=; tokens=1,2,5,10 delims= " %%a in (%table%) do @if /i "%%c"=="yes" call :Sign1File %mybinaries%\%%b\%%a %%c %%d %%b & if errorlevel 1 goto errend
net use \\%CryptoServer%\ipc$ /d
popd
:SignItDone

echo.
echo ------------------------------------------------
echo  Create encrypted installers
echo ------------------------------------------------

REM  For US, create all encrypted installers listed in crypto.txt.
REM  For international langauges, only create those that:
REM  1) get localized, AND
REM  2) don't get signed, AND
REM  3) are in product-types shipped for the given language.
REM
REM  Note: 3) NYI because language is not known in this context

for /f "eol=; tokens=1,2,3,4,5,7 delims= " %%a in (%table%) do @if /i not "%%c"=="-" call :GenEI %%a %%b %%c %%d %%e %%f & if errorlevel 1 goto errend


echo.
echo ------------------------------------------------
echo  Validate TS certs and CSP signatures
echo ------------------------------------------------
:VerifyFiles

REM These checks apply whether or not files were just updated by the verifier or not.
set verify_err=

REM Check for missing TS cert
for /f "eol=; tokens=1,2,6 delims= " %%a in (%table%) do @if /i "%%c"=="yes" tscrtvfy.exe %mybinaries%\%%b\%%a & if errorlevel 1 call errmsg.cmd "tscrtvfy.exe %mybinaries%\%%b\%%a failed" & set verify_err=1

REM Check for unsigned files
for /f "eol=; tokens=1,2,5 delims= " %%a in (%table%) do @if /i "%%c"=="yes" showsig.exe %mybinaries%\%%b\%%a & if errorlevel 1 call errmsg.cmd "showsig.exe %mybinaries%\%%b\%%a failed" & set verify_err=1

if defined verify_err (
  call errmsg.cmd "Errors found in validation - see %logfile% for details"
  call logmsg.cmd /t "DONE '%cmdline%'" %logfile%
  goto errend
)
call logmsg.cmd /t "DONE '%cmdline%'" %logfile%
goto end


REM ------------------------------------------------------
REM  GenEI - Function to generate one encrypted installer.
REM          An encrypted installer (EI) contains a 128-bit
REM          binary inside it as an encrypted resource.
REM          Sets errorlevel=1 on failure
REM ------------------------------------------------------
:GenEI
  REM Parameters
  REM %1:  128-bit filename
  REM %2:  path to %1 relative to %binaries%
  REM %3:  encrypted installer filename to create
  REM %4:  localizable? { yes no }
  REM %5:  sign?        { yes no }
  REM %6:  product type { wks srv }

  REM Only generate EIs for files that require them.
  if /i "%3"=="-" goto :EOF

  REM International builds only generate EIs for binaries that get
  REM localized AND do not get signed
  if defined isintlbld if /i not "%4"=="yes"  goto :EOF
  if defined isintlbld if /i "%5"=="yes" goto :EOF

  REM Properly speaking, we'd restrict ourselves to valid producttype-language
  REM combinations to avoid creating server-specific files for languages that
  REM only ship workstation.  Since language is not available (it would be
  REM if this were moved to bldrules), we go ahead and process all files even
  REM though they may not be in every language product.

  if not exist %mybinaries%\dump\instsch.dll (
    call errmsg.cmd "%mybinaries%\dump\instsch.dll not found"
    goto errend
  )

  set cmd=copy %mybinaries%\dump\instsch.dll %mybinaries%\%3
  %cmd%
  if errorlevel 1 (
    call errmsg.cmd "%cmd% failed"
    goto errend
  )

  rem
  rem Disable adding resources to 64-bit images until the code is able
  rem to do this for cross platform images, i.e., resources can be added
  rem to 64-bit images on a 32-bit system.
  rem As an artifact of this, 64-bit builds get the inst*.dll files
  rem present, but they're disfunctional.
  rem

  if defined AXP64 goto :EOF
  if defined IA64  goto :EOF

  pushd \
  REM Example: encaddrs.exe %mybinaries%\instips5.dll %mybinaries%\noexport\ipsec.sys
  set cmd=encaddrs.exe %mybinaries%\%3 %mybinaries%\noexport\%1
  %cmd%
  if errorlevel 1 (
    call errmsg.cmd "%cmd% failed"
    goto errend
  )
  popd
goto :EOF


REM ------------------------------------------------
REM  TSCert1File: Function to add a Terminal Services cert to one file
REM               Sets errorlevel=1 on failure
REM ------------------------------------------------
:TSCert1File
  REM Parameters
  REM %1:  full path to filename we'll consider adding a TS cert to
  REM %2:  does this file require a TS cert?
  REM %3:  product type

  REM Only consider files requiring the TS cert
  if /i not "%2"=="yes" goto :EOF

  if not exist %1 (
    call errmsg.cmd "%1 not found"
    goto errend
  )

  REM Add the TS cert if the image doesn't already have one
  tscrtvfy.exe %1
  if "%errorlevel%"=="0" (
    call logmsg.cmd "%1 already has a valid TS cert"
    goto :EOF
  )

  set cmd=tscrtadd.exe %1
  %cmd%
  if not "%errorlevel%"=="0" (
    call errmsg.cmd "%cmd% failed"
    goto errend
  ) else (
    call logmsg.cmd "%cmd% succeeded"
  )
goto :EOF


REM ------------------------------------------------
REM  Sign1File: Function to test sign one file
REM             Sets errorlevel=1 on failure
REM ------------------------------------------------
:Sign1File
  REM Parameters
  REM %1:  full path to file to consider for test signing (ex: d:\binaries\rsabase.dll)
  REM %2:  does this file require signing?                {yes no}
  REM %3:  platforms requiring test signing for %1        (ex: i386,ia64,axp64)
  REM %4:  path to binary from %mybinaries%               (ex: noexport or .)

  REM International builds don't sign any files
  if defined isintlbld goto :EOF

  REM Only consider files that are meant to be signed
  if /i not "%2"=="yes" goto :EOF

  REM Only sign files on the appropriate platform(s) as defined by %table%
  set arg=%3
  set valid_platforms=%arg::= %
  for %%i in (%valid_platforms%) do if /i "%%i"=="%myplatform%" goto sign1file_continue
  call logmsg.cmd "not signing file '%1' for platform '%myplatform%' per policy in %table%"
  goto :EOF

  :sign1file_continue
  if not exist %1 (
    call errmsg.cmd "%1 not found"
    goto errend
  )

  REM Prevent the image from being bound in the future.
  REM This is the last change to the image prior to test signing.
  imagecfg.exe -n %1

  :Sign1File_loop
  set cmd=signcsp.exe %1
  echo Running '%cmd%'
  %cmd%
  if errorlevel 1 (
    sleep 15
    echo **********************************************************************
    echo * Error: '%cmd%' failed.
    echo * Assuming we cannot connect to \\%CryptoServer% - please go to a cmd window
    echo * on this computer and make the network connection while we loop:
    echo *     net use \\%CryptoServer% /u:^<domain^>\^<user^> ^<password^>
    echo **********************************************************************
    goto Sign1File_loop
  )
  touch.exe %1

  REM We've updated the image without updating image header fields
  REM in the .dbg file.  Update the .dbg file so symchk.exe doesn't
  REM doesn't complain about mismatched file size, base address, 
  REM checksum, or time and date stamp during symbol CD creation.
  REM This problem will all become irrelevant once we rid ourselves
  REM of COFF (.dbg) symbols.

  REM BUGBUG: rsaenh.dll gets renamed to rsaenhs.dll somewhere, so symbols
  REM         only exist for rsaenh.dll.  However, encpack.sed and media scripts
  REM         refer to rsaenhs.dll.  Needless to say, you'll never update
  REM         the rsaenhs.dbg because it doesn't exist.
  set dbgsubpath=%4
  if "%dbgsubpath%"=="." set dbgsubpath=retail
  set dbgpath=%mybinaries%\symbols\%dbgsubpath%
  set cmd=upddbg.exe -u -s %dbgpath% %1
  call logmsg.cmd "Running %cmd%"
  %cmd%
  if errorlevel 1 (
    call logmsg.cmd "%cmd% failed; not a critical error"
  )

  REM  Verify the binary was signed.  Note that showsig.exe gets updated
  REM  occasionally to change its validate criteria based on the following:
  REM  1) a test signature OR real signature  (during the development cycle)
  REM  2) a real signature only               (just prior to RTM)
  REM  Test signatures are from the crypto server; real signatures are from the vault.

  showsig.exe %1
  if ERRORLEVEL 1 (
    call errmsg.cmd "showsig.exe %1 failed"
    goto errend
  )
goto :EOF


REM ------------------------------------------------
REM  VerifyConnection: Function to verify connection to crypto server.
REM                    It's useful to know at the beginning of
REM                    congeal, which invokes this script, that
REM                    that the connection exists to avoid looping
REM                    on error later on in congeal.
REM                    Sets errorlevel=1 on failure
REM ------------------------------------------------
:VerifyConnection
  REM Parameters
  REM %1:  server name to verify connection to  (ex: %CryptoServer%)

  if defined isintlbld (
    call logmsg.cmd "VerifyConnection only applicable when signing"
    goto :EOF
  )
  REM  Make sure we have a valid connection to %1. The
  REM  connection must be made by hand due to security concerns.
  net use | findstr -i %1 | findstr OK >nul
  if errorlevel 1 (
    call errmsg.cmd "Network connection to %1 not found"
    echo.
    echo  You'll need to connect to the crypto server before
    echo  running this command by doing the following with
    echo  your personal ntdev account on all build machines:
    echo.
    echo    net use \\%1\ipc$ /u:^<domain^>\^<user^> ^<password^>
    echo.
    echo  ALL WINDOWS 2000 BUILDS MUST SIGN THE CSP AND SECURITY PROVIDER 
    echo  FILES ELSE CryptoAPI AND OTHER FUNCTIONALITY WILL BREAK.  THE 
    echo  SIGNATURES GENERATED HERE ARE FOR TESTING PURPOSES AND ARE NOT 
    echo  MEANT FOR THE SHIPPING PRODUCT.  THE CRYPTO TEAM RE-SIGNS BINARIES
    echo  THAT ACTUALLY SHIP.  THE SIGNATURE WILL BE INVALIDATED IF THE 
    echo  BINARY IS MODIFIED ^(INCLUDING BEING REBUILT, LOCALIZED, REBASED, 
    echo  BOUND, OPTIMIZED^) IN ANY WAY AFTER IT IS SIGNED.
    echo.
    echo  ALWAYS:
    echo     1. Add all signed binaries to the ExcludeDll list in
    echo        public\tools\bindsys.cmd to exclude them from being bound.
    echo     2. Add all signed binaries to public\tools\never.reb to exclude
    echo        from being rebased.
    echo.
    echo  THE CAPI BVT WILL VERIFY THAT CryptoAPI FUNCTIONALITY WORKS
    echo  ON A RUNNING SYSTEM, BUT WILL NOT VERIFY THAT ALL CSPs AND
    echo  SECURITY PROVIDER FILES ARE PROPERLY SIGNED.
    goto errend
  ) else (
    call logmsg.cmd "Network connection to %1 found"
  )
goto :EOF

REM ------------------------------------------------
REM  Display Usage:
REM ------------------------------------------------
:Usage

echo.
echo This script performs several tasks:
echo  1) Inserts a cert into Terminal Services files.
echo  2) CSP (test) signs CSPs and security provider files.
echo  3) Creates encrypted installers that contain high encryption
echo     versions of some CSPs and security providers.
echo  4) Verifies that signatures on the signed files are valid.
echo Not all tasks are appropriate for all languages, architectures,
echo files, builds, etc.  Policies for driving this are read from 
echo crypto.txt.
echo.
echo Usage:  %~nx0 [VerifyConnection ^| VerifyFiles]
echo.
echo   VerifyConnection   Verifies that a connection to the crypto
echo                      server %CryptoServer% is established.  This is
echo                      useful to verify early so the calling 
echo                      program does not loop on failure later on 
echo                      when attempting to test sign files.  No 
echo                      other processing is performed.
echo   VerifyFiles        Verifies signatures of files that are supposed
echo                      to be signed.  Does not update any files or do
echo                      any other processing.  This verification step
echo                      is performed by default during normal processing.

:errend
seterror.exe 1
goto :EOF

:end
seterror.exe 0
goto :EOF
