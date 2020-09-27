@echo off
setlocal ENABLEDELAYEDEXPANSION
if DEFINED _echo   echo on
if DEFINED verbose echo on

REM ********************************************************************
REM
REM This script finds out which files in symbad.txt are either not found
REM or pass symbol checking on all four build machines.
REM
REM It creates a new symbad.txt that can be checked into \nt\public\tools
REM
REM Author: Barb Kess
REM Date:   June 1, 1999
REM
REM ********************************************************************

if /i "%PROCESSOR_ARCHITECTURE%" == "x86"   set NTTREE=%_NT386TREE%
if /i "%PROCESSOR_ARCHITECTURE%" == "ia64" set NTTREE=%_NTIA64TREE%
if /i "%PROCESSOR_ARCHITECTURE%" == "axp64" set NTTREE=%_NTAXPTREE%

set Symbad=%_NTDRIVE%%_NTROOT%\public\tools\symbad.txt
set SymDir=%NTTREE%\symbad
set ml=perl %_NTDRIVE%%_NTROOT%\public\tools\makelist.pl

if EXIST %SymDir% rd /s /q %SymDir%
md %SymDir%

REM Create a Symbad.txt that doesn't have any comments in it
for /f "eol=; tokens=1" %%a in (%Symbad%) do (
    echo %%a>> %SymDir%\symbad.txt.old2
)
sort %SymDir%\symbad.txt.old2 >%SymDir%\symbad.txt.old

REM Take all the files in symbad.txt.old and create a list for each
REM build machine of the ones that pass symbol checking

for %%a in (x86fre x86chk ia64fre ia64chk axp64fre axp64chk) do (
    echo Examining files on %%a
    for /f %%b in (%SymDir%\symbad.txt.old) do (
        if /i EXIST \\%%a\binaries\%%b (
            %NTTREE%\idw\symchk.exe \\%%a\binaries\%%b /s \\%%a\binaries\symbols\retail /v | findstr PASSED | findstr -v IGNORED >> %SymDir%\%%a.tmp
        ) else (
            REM if
            echo SYMCHK: %%b   PASSED  NOT FOUND >> %SymDir%\%%a.tmp
        )
    )

    REM Strip everything out except for the file name
    for /f "tokens=2 delims= " %%c in (%SymDir%\%%a.tmp) do (
        echo %%c>> %SymDir%\%%a.pass
    )
    REM Special case for zero-length file, make sure that a zero length
    REM file is at least there for later code
    if NOT EXIST %SymDir%\%%a.pass copy %SymDir%\%%a.tmp %SymDir%\%%a.pass

    echo All files that passed or were not found are in %SymDir%\%%a.pass
)

:combine
REM
REM Now find the intersection of all the symbols that passed
REM and put result into %SymDir%\all.passed
REM 

echo Finding the intersection of the files that passed ...
%ml% -i %SymDir%\x86fre.pass %SymDir%\x86chk.pass -o %SymDir%\all2.pass
%ml% -i %SymDir%\all2.pass %SymDir%\ia64fre.pass  -o %SymDir%\all3.pass
%ml% -i %SymDir%\all3.pass %SymDir%\ia64chk.pass  -o %SymDir%\all4.pass
%ml% -i %SymDir%\all4.pass %SymDir%\axp64fre.pass -o %SymDir%\all5.pass
%ml% -i %SymDir%\all5.pass %SymDir%\axp64chk.pass -o %SymDir%\all.pass

sort %SymDir%\all.pass > %SymDir%\all.passed

REM
REM Find the difference between the original symbad.txt and the files
REM that just passed symbol checking on all platforms.  Note: symbad.txt.old
REM is symbad.txt without the comments.
REM
REM Save the list in %SymDir%\symbad.txt.tmp
REM

echo Calculating the new symbad.txt
%ml% -d %SymDir%\symbad.txt.old %SymDir%\all.passed -o %SymDir%\symbad.txt.tmp2
sort %SymDir%\symbad.txt.tmp2 > %SymDir%\symbad.txt.tmp
del %SymDir%\symbad.txt.tmp2

REM
REM Now, save all the comments that were in the original symbad.txt
REM Copy the new symbad.txt that can be checked in to 
REM \nt\public\tools\symbad.txt
REM

echo Restoring the comments from original symbad.txt
for /f %%a in (%SymDir%\symbad.txt.tmp) do (
    findstr /i %%a %Symbad% >> %SymDir%\symbad.txt.new2 
)


REM Everything is great except that there could be duplicate lines in the file
sort %SymDir%\symbad.txt.new2 > %SymDir%\symbad.txt.new3

set prev=
for /f "tokens=1 delims=" %%a in (%SymDir%\symbad.txt.new3) do (
    if /i NOT "%%a" == "!prev!" echo %%a>> %SymDir%\symbad.txt.new
    set prev=%%a
)

REM Add acmsetup.exe and mssetup.dll back
REM This is a bug that the TS group needs to fix.  This can be
REM removed when they get their bug fixed.  They rename retail
REM in their placefil.txt.  Thus, even though these don't get
REM binplaced to binaries, build.exe thinks they do and says they
REM have symbol errors

echo acmsetup.exe    ; tsext\client\setup>> %SymDir%\symbad.txt.new
echo mssetup.dll     ; tsext\client\setup>> %SymDir%\symbad.txt.new

REM Add the 64-bit files back in
REM findstr "64-bit" %Symbad% >> %SymDir%\symbad.txt.new

REM Sort the final list
sort %SymDir%\symbad.txt.new > %SymDir%\symbad.txt.sorted
copy %SymDir%\symbad.txt.sorted %SymDir%\symbad.txt.new

echo New symbad.txt = %SymDir%\symbad.txt.new

endlocal
