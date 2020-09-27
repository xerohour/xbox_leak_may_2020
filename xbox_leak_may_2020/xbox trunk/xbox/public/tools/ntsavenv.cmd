@if "%_echo%"=="" echo off
echo Saving modified files \nt\private\developr\%USERNAME% to \\kernel\developr\%USERNAME%
tc /t \nt\private\developr\%USERNAME% \\kernel\developr\%USERNAME%
