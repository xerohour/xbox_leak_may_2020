@echo off
::Update Net Use Paths

for /f %%a in (\\xbuilds\release\usa\latest.txt) do set BuildVer=%%a
echo BuildVer %BuildVer%

set btype=checked
if "%1"=="-e" set btype=free

net use * /d /y
net use x: \\xstress\stress
net use i: \\xbuilds\release\usa\%buildver%\%btype%\public\idw
net use r: \\xbuilds\release\usa\%buildver%\%btype%
net use t: \\xbuilds\release\usa\%buildver%\%btype%\public\tools
set _NT386TREE=R:\

PATH=i:\;r:\;t:\;x:\;%PATH%

