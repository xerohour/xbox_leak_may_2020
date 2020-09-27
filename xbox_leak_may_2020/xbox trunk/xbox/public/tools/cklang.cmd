@echo off
if defined _echo echo on
if defined verbose echo on
setlocal ENABLEEXTENSIONS

REM ---------------------------------------------------------------------------
REM cklang.cmd - Validates the given language according to
REM         %_ntroot%\public\tools\codes.txt.
REM
REM usage: cklang [NOSITE] [NOARCH] [BLDENV] <lang> [<class>]
REM
REM     NOSITE  If specified, omits the check to make sure the given language
REM             is valid for the current value of the SITE environment variable.
REM             Otherwise, the check of language versus SITE is automatic.
REM
REM     NOARCH  If specified, omits the check to make sure the given language
REM             is valid for the current value of the PROCESSOR_ARCHITECTURE
REM             variable.  Otherwise, the check of language vs. architecture
REM             is automatic.
REM
REM     BLDENV  If specified, makes sure the given language is valid for the
REM             current build environment (US or INTL).  If the current
REM             environment is US, only the language USA is valid.  If the 
REM             current environment is INTL, the language USA is NOT valid.
REM
REM     lang    Language to validate.  Should be a 2- or 3- letter code as
REM             given in codes.txt, e.g. jpn or fr.
REM
REM     class   Language class, as listed in codes.txt.  If this is specified,
REM             this script will check whether the given language is a member
REM             of the class.
REM ---------------------------------------------------------------------------

REM Define exitcode values
set EXIT_SUCCESS=0
set EXIT_ERROR_BROKEN_DEP=1
set EXIT_ERROR_LANG_NOT_FOUND=2
set EXIT_ERROR_WRONG_CLASS=3
set EXIT_ERROR_WRONG_SITE=4
set EXIT_ERROR_WRONG_ARCH=5
set EXIT_ERROR_WRONG_BLDENV=6

REM Initialize exitcode
set exitcode=%EXIT_SUCCESS%

REM Define SCRIPT_NAME. Used by the logging scripts.
for %%i in (%0) do set script_name=%%~ni.cmd

REM Provide usage.
for %%a in (./ .- .) do if ".%1." == "%%a?." goto Usage

REM Verify the environment
for %%i in (_ntdrive _ntroot) do (
    if not defined %%i (
        call errmsg.cmd "Environment variable %%i is not defined." %logfile%
        set exitcode=%EXIT_ERROR_BROKEN_DEP%
        goto :end
    )
)

REM codes.txt is an input
set codesfile=%_ntdrive%%_ntroot%\public\tools\codes.txt
if not exist %codesfile% (
    call errmsg.cmd "Unable to find input file %codesfile%." %logfile%
    set exitcode=%EXIT_ERROR_BROKEN_DEP%
    goto :end
)


set lang=
set class=
set nosite=
set noarch=
set bldenv=

REM Parse the command line
:ParseCmdLine
    if "%1" == "" (
        if "%lang%" == "" (
            goto :Usage
        ) else (
            goto :MainProcedure
        )
    )
    if /i "%1" == "NOSITE" (
        if "%nosite%" == "" (
            set nosite=1
            shift /1
            goto :ParseCmdLine
        ) else (
            goto :Usage
        )
    )
    if /i "%1" == "NOARCH" (
        if "%noarch%" == "" (
            set noarch=1
            shift /1
            goto :ParseCmdLine
        ) else (
            goto :Usage
        )
    )
    if /i "%1" == "BLDENV" (
        if "%bldenv%" == "" (
            set bldenv=1
            shift /1
            goto :ParseCmdLine
        ) else (
            goto :Usage
        )
    )
    if "%lang%" == "" (
        set lang=%1
        shift /1
        goto :ParseCmdLine
    )
    if "%class%" == "" (
        set class=%1
        shift /1
        goto :ParseCmdLine
    )


:MainProcedure

REM Validate the language
call :ValidateLang

set exitcode=%ERRORLEVEL%

goto :end

REM ------------------------
REM Procedure: ValidateLang
REM ------------------------
:ValidateLang
  REM Check for valid languages in codes.txt.
  REM Also, the language must match the site
  REM specified in the environment.

  REM Check for entries that match %lang% in codes.txt
  for /f "eol=; tokens=1,6,8" %%i in ('findstr /bi /c:"%lang% " %codesfile%') do (
      set my_lang=%%i
      set my_class=%%j
      set my_site=%%k
  )

  REM Verify that language is listed in codes.txt
  if "%my_lang%" == "" (
      seterror.exe %EXIT_ERROR_LANG_NOT_FOUND%
      goto :EOF
  )

  REM Verify that the language found is the one we're looking for
  if /i NOT "%lang%" == "%my_lang%" (
      call errmsg.cmd "Given language %lang% not equal to found language %my_lang%" %logfile%
      seterror.exe %EXIT_ERROR_LANG_NOT_FOUND%
      goto :EOF
  )

  REM If class was specified, check it
  if DEFINED class (
      if /i "%class%" == "%lang%" (
          seterror.exe %EXIT_SUCCESS%
          goto :EOF
      )
      if /i NOT "%class%" == "%my_class%" (
          seterror.exe %EXIT_ERROR_WRONG_CLASS%
          goto :EOF
      )
  )

  REM Verify that language matches the SITE
  if NOT DEFINED nosite (
      if /i NOT "%site%" == "%my_site%" (
          seterror.exe %EXIT_ERROR_WRONG_SITE%
          goto :EOF
      )
  )

  REM Verify that language matches current architecture
  if NOT DEFINED noarch (
      if /i "%lang%" == "nec_98" (
          if /i not "%processor_architecture%" == "x86" (
              seterror.exe %EXIT_ERROR_WRONG_ARCH%
              goto :EOF
          )
      )
  )

  REM Verify that language matches the build environment
  if DEFINED bldenv (
      call isintlbld.cmd
      if ERRORLEVEL 2 (
          REM This is an INTL build environment, but the site is wrong
          seterror.exe %EXIT_ERROR_WRONG_SITE%
          goto :EOF
      )
      if ERRORLEVEL 1 (
          REM This is a US build environment - language should be USA
          if /i "%lang%" NEQ "USA" (
              seterror.exe %EXIT_ERROR_WRONG_BLDENV%
              goto :EOF
          )
      ) else (
          REM This is an INTL build environment - language should NOT be USA
          if /i "%lang%" EQU "USA" (
              seterror.exe %EXIT_ERROR_WRONG_BLDENV%
              goto :EOF
          )
      )
  )

  seterror.exe %EXIT_SUCCESS%
goto :EOF


:Usage
REM ----------------------------------
REM Display usage
REM ----------------------------------
echo.
echo %SCRIPT_NAME% - Validates the given language according to
echo %%_ntroot%%\public\tools\codes.txt.
echo.
echo Usage: %SCRIPT_NAME% [NOSITE] [NOARCH] [BLDENV] ^<language^> [^<class^>]
echo.
echo     NOSITE  If specified, omits the check to make sure the given language
echo             is valid for the current value of the SITE environment
echo             variable. Otherwise, the check of language versus SITE is
echo             automatic.
echo.
echo     NOARCH  If specified, omits the check to make sure the given language
echo             is valid for the current value of the PROCESSOR_ARCHITECTURE
echo             variable.  Otherwise, the check of language vs. architecture
echo             is automatic.
echo.
echo     BLDENV  If specified, makes sure the given language is valid for the
echo             current build environment (US or INTL).  If the current
echo             environment is US, only the language USA is valid.  If the
echo             current environment is INTL, the language USA is NOT valid.
echo.
echo     lang    Language to validate.  Should be a 2- or 3- letter code as
echo             given in codes.txt, e.g. jpn or fr.
echo.
echo     class   Language class, as listed in codes.txt.  If this is specified,
echo             this script will check whether the given language is a member
echo             of the class.
echo.
echo ERRORLEVEL is set upon exiting as follows:
echo.
echo     %EXIT_ERROR_WRONG_BLDENV%:      Language is not valid for the current build environment.
echo.
echo     %EXIT_ERROR_WRONG_ARCH%:      Language is not valid for the current architecture.
echo.
echo     %EXIT_ERROR_WRONG_SITE%:      Language is not valid for the current SITE.
echo.
echo     %EXIT_ERROR_WRONG_CLASS%:      Language is not a member of the given class.
echo.
echo     %EXIT_ERROR_LANG_NOT_FOUND%:      Language is not listed in codes.txt.
echo.
echo     %EXIT_ERROR_BROKEN_DEP%:      One or more of this script's dependencies or necessary
echo             environment variables is incorrect or not found.
echo.
echo     %EXIT_SUCCESS%:      Language is listed in codes.txt and fulfills all of the
echo             specified conditions.
echo.
echo   ex.
echo     %SCRIPT_NAME% usa
echo     %SCRIPT_NAME% NOSITE ger
echo     %SCRIPT_NAME% BLDENV ger
echo     %SCRIPT_NAME% ger EU
echo     %SCRIPT_NAME% ARCH nec_98
echo.


:end
endlocal & seterror.exe %exitcode%
