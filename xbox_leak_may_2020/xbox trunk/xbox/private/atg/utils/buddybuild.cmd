@echo off
if exist \\revoltbuild\buddy\%_NTUSER%.bbr (
   Echo You already have a buddy build request pending - cannot submit another
   goto done
)
if "%1"=="full" (
   echo Full > \\REVOLTBUILD\buddy\%_NTUSER%.bbr
) ELSE (
   echo Light > \\REVOLTBUILD\buddy\%_NTUSER%.bbr
)
echo Your buddybuild request has been submitted.  The results
echo will be e-mailed to you.
:done
