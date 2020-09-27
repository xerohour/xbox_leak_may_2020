$(O)\CREATEINSTANCE.CPP: $(O)\FILELIST.TXT
    echo Analyzing content to produce CreateInstance.cpp
    $(_NT386TREE)\idw\AudAnalyze /q $(O)\FilesToAnalyze.txt $(O)\CreateInstance.cpp

$(O)\FILELIST.TXT: 
    copy \\xdb01\stresscontent\DMusic\AudAnalyze\FileLists\$(NAME).txt $(O)\FileList.txt    
    rd /q /s media
    mkdir Media
    time /t

    REM Create the file list for AudAnalyze.exe
    for /F "eol=; tokens=1,2 delims=," %a in ($(O)\filelist.txt) do echo Media\%b%~nxa>>$(O)\FilesToAnalyze.txt

    REM Make all the destination directories.
    for /F "eol=; tokens=1,2 delims=," %a in ($(O)\filelist.txt) do mkdir Media\%b

    REM Copy down the files.
    for /F "eol=; tokens=1,2 delims=," %a in ($(O)\filelist.txt) do copy "\\xdb01\StressContent\%a" Media\%b
