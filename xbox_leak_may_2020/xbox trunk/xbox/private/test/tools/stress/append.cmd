@echo off

for /f "delims=" %%d in ('date /t') do set DATE=%%d
for /f "delims=" %%t in ('time /t') do set TIME=%%t

echo. >> users.txt
echo ------------------%DATE%-%TIME%------------------------------- >> users.txt
echo. >> users.txt