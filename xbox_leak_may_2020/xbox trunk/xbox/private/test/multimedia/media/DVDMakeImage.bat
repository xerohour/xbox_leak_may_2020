@echo off

if  "%1"=="" goto ERROR
if  "%2"=="" goto ERROR

set XBENAME=MEDIA.XBE
set MKDIR=mkdir
set COPY=xcopy


REM 1) Choose where your XBE will go (for example XE:\MediaStress)
REM -------------------------------------------------------------
set ROOT=%1

REM 1) Choose your settings file!
REM -------------------------------------------------------------
set SETTINGS=%2
if not exist "%2" goto ERROR_NOEXIST_SETTINGS


REM 2) Copy your XBE to that Directory 
REM %COPY% /f %_NT386TREE%\dump\%XBENAME% %ROOT%\
%COPY% /y %_NT386TREE%\dump\%XBENAME% %ROOT%\


REM 3) Copy your media into the "media" subdirectory.
%MKDIR% %ROOT%\Media
REM %COPY% /f /y /r /t /d %_NTDRIVE%%_NTROOT%\private\test\multimedia\media\media\*.* %ROOT%\Media
%COPY% /y /e %_NTDRIVE%%_NTROOT%\private\test\multimedia\media\media\*.* %ROOT%\Media

REM 4) Copy over the INI file you've created and name it TESTINI.INI
REM    NOTE: All file references in this INI file must reference D:\MEDIA.
REM %COPY% /y /f %SETTINGS% %ROOT%\testini.ini
%COPY% %SETTINGS% %ROOT%\testini.ini


REM *********************************** NOTES **************************************************
REM
REM All file references in your INI file must be to D:\MEDIA, because the media (content) needed
REM by the app is in the MEDIA subdirectory relative to the app.

goto END


:ERROR
echo * Usage:    DVDMakeImage.bat [Destination] [Settings]
echo * Examples:
echo *     
echo *           To make an image in C:\TEMP using DVD.INI as your testini.ini file
echo *           DVDMakeImage.bat C:\TEMP DVD.INI
echo *          
echo *           Note: You may only make images on the hard disk; you must then copy
echo *                 or burn them to the DVD or XBox manually.
goto END

:ERROR_NOEXIST_SETTINGS
@echo Error: %SETTINGS% does not exist!
goto END

:END
