ECHO Building %_BUILDVER%\%_BUILDTYPE

ECHO Cleaning...
cd %_NTDRIVE%%_NTROOT%
del /q /s /a-r *.*

ECHO Deleting Tree...
rd /s /q %_NT386TREE%

cd %_NTDRIVE%%_NTROOT%\private
build -cZ
copy /y build.* build_pass1.*
build -zE
