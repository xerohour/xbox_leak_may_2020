rem This is kicked off by the 'Scheduled Tasks' command @ 4:10am daily.

@ECHO OFF
set _ALLOFIT=1
if "%fBuildBranch%"=="0" (
echo Building MAIN
D:\xboxRET\private\developr\xblds\RETrazzle.cmd d:\scripts\DailyRET.cmd %1 %2 %3
)
if "%fBuildBranch%"=="1" (
echo Building Branch %CurrentBranch%
G:\xbox-%CurrentBranch%RET\private\developr\xblds\%CurrentBranch%RetRazzle.cmd d:\scripts\DailyRET.cmd %1 %2 %3
)
