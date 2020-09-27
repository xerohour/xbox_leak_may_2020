@echo off
d:
cd \xbox\private\windows\directx\dplay

start /wait windiff . D:\nt\MULTIM~1\DirectX\dplay\dnet -T -Sdx diff\diff.lst
start /wait windiff . D:\nt\MULTIM~1\DirectX\dplay\dnet -T -Slrx diff\diff2.lst

cd diff
copy diff.lst diff.txt
type diff2.lst >> diff.txt

for /F "skip=1 tokens=1,2,3*" %%1 in (diff.lst) do echo . >> diff.txt & echo . >> diff.txt & echo ()---()---()---()---()---()---()---()---()---(%%1)---()---()---()---()---()---()---()---()---()---() >> diff.txt & echo . >> diff.txt & echo . >> diff.txt & diff ..\%%1 D:\nt\MULTIM~1\DirectX\dplay\dnet\%%1 >> diff.txt

del diff.lst
del diff2.lst
notepad diff.txt

echo on