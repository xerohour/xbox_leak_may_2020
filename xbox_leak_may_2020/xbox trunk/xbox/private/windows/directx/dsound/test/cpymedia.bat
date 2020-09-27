@echo off

xbmkdir xc:\tdata
xbmkdir xc:\tdata\00000000
xbmkdir xc:\tdata\00000000\media
xbmkdir xc:\tdata\00000000\media\audio

xbcp -f -s -d %1 %2 %3 %4 %5 %6 %7 %8 %9 \\dereks\media\*.* xc:\tdata\00000000\media\audio
