@echo off
setlocal

if "%1" == "-s" goto stress
if "%1" == "-S" goto stress
if "%1" == "/s" goto stress
if "%1" == "/S" goto stress

set __dir=bvt
goto stressdone

:stress
set __dir=stress

:stressdone

set __IMAGE=%__dir%.iso
set __DEST=%_NTBINDIR%\private\test\buildxbe\cdfiles\%__dir%

@echo on
del /f /q %__DEST%\*.*
copy /y %_NT386TREE%\dump\harness.xbe %__DEST%\default.xbe
copy /y %_NTBINDIR%\private\test\buildlab\ini\%__DIR%\testini.ini %__DEST%\testini.ini

if "%__dir%"=="stress" goto stressfiles
copy /y   \\danrose\content\xbox_media\2592.wav %__DEST%
copy /y   \\danrose\content\xbox_media\2845.wav %__DEST%
copy /y   \\danrose\content\xbox_media\2723.wav %__DEST%     
copy /y   \\danrose\content\xbox_media\2615.wav %__DEST%
copy /y   \\danrose\content\xbox_media\2603.wav %__DEST%
copy /y   \\danrose\content\xbox_media\2674.wav %__DEST%
copy /y   \\danrose\content\xbox_media\167.wav  %__DEST%
copy /y   \\danrose\content\xbox_media\677.wav  %__DEST%
copy /y   \\danrose\content\xbox_media\682.wav  %__DEST%
copy /y   \\danrose\content\xbox_media\681.wav  %__DEST%
copy /y   \\danrose\content\xbox_media\1947.wav %__DEST%
copy /y   \\danrose\content\xbox_media\2301.wav %__DEST%
copy /y   \\danrose\content\xbox_media\1892.wav %__DEST%
copy /y   \\danrose\content\xbox_media\1891.wav %__DEST%
copy /y   \\danrose\content\xbox_media\1596.wav %__DEST%
copy /y   \\danrose\content\xbox_media\1210.wav %__DEST%
copy /y   \\danrose\content\xbox_media\1226.wav %__DEST%
copy /y   \\danrose\content\xbox_media\5.wav    %__DEST%     
copy /y   \\danrose\content\xbox_media\664.wav  %__DEST%   
copy /y   \\danrose\content\xbox_media\663.wav  %__DEST%  
copy /y   \\danrose\content\xbox_media\666.wav  %__DEST%  
copy /y   \\danrose\content\xbox_media\665.wav  %__DEST%  
copy /y   \\danrose\content\xbox_media\991.wav  %__DEST%  
copy /y   \\danrose\content\xbox_media\823.wav  %__DEST%   
copy /y   \\danrose\content\xbox_media\831.wav  %__DEST%  
copy /y   \\danrose\content\xbox_media\816.wav  %__DEST%   
copy /y   \\danrose\content\xbox_media\2770.wav %__DEST% 
copy /y   \\danrose\content\xbox_media\2768.wav %__DEST% 
copy /y   \\danrose\content\xbox_media\2772.wav %__DEST%  
copy /y   \\danrose\content\xbox_media\2761.wav %__DEST%   
copy /y   \\danrose\content\xbox_media\1939.wav %__DEST%
copy /y   \\danrose\content\xbox_media\2066.wav %__DEST%
copy /y   \\danrose\content\xbox_media\1240.wav %__DEST%
copy /y   \\danrose\content\xbox_media\755.wav  %__DEST%
copy /y   \\danrose\content\xbox_media\190.wav  %__DEST%
copy /y   \\danrose\content\xbox_media\2513.wav %__DEST%
copy /y   \\danrose\content\xbox_media\1252.wav %__DEST%
copy /y   \\danrose\content\xbox_media\1308.wav %__DEST% 
copy /y   \\danrose\content\xbox_media\1448.wav %__DEST%
copy /y   \\danrose\content\xbox_media\1720.wav %__DEST%
copy /y   \\danrose\content\xbox_media\1958.wav %__DEST%
copy /y   \\danrose\content\xbox_media\1357.wav %__DEST%
copy /y   \\danrose\content\xbox_media\2057.wav %__DEST%
copy /y   \\danrose\content\xbox_media\77.wav   %__DEST%
copy /y   \\danrose\content\xbox_media\696.wav  %__DEST%
copy /y   \\danrose\content\xbox_media\1413.wav %__DEST%
copy /y   \\danrose\content\xbox_media\2633.wav %__DEST%
copy /y   \\danrose\content\xbox_media\179.wav  %__DEST%
copy /y   \\danrose\content\xbox_media\1306.wav %__DEST%
copy /y   \\danrose\content\xbox_media\2000.wav %__DEST%
copy /y   \\danrose\content\xbox_media\2111.wav %__DEST%
goto image

:stressfiles

for /f %%f in ('randfile.exe \\danrose\content\xbox_media 7') do copy /y %%f %__DEST%

@echo off

:image
cdimage -xbox -l%__dir% %__DEST% %_NT386TREE%\%__IMAGE%

goto end

:usage
echo "usage: makecd [-s]"

:end
endlocal
