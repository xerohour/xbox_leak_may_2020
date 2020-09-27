@echo off
rem Update Xstress

type \\xstress\stress\stress.txt > stressmail.txt
type \\xstress\stress\users_temp.txt >> stressmail.txt

call sendfile xboxos,xatgfte,xstress -f "MS Exchange Settings" -s "See Stress Run" -t stressmail.txt
