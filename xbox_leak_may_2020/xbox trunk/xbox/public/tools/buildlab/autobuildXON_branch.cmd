rem This is kicked off by the 'Scheduled Tasks' command @ 4:10am daily.

@ECHO OFF
set _XOB=1
D:\xonline-%_BUILDBRANCH%\private\developr\xblds\xorazzle.cmd d:\scripts\DailyXON.cmd %1 %2 %3
