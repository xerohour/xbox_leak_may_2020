;Files
;Flavor,type,Source root,Source path,source file,Target root,Target path,target file,Flags
B,file,TEST,,relnotes.txt,XDK,,,
B,file,TEST,,XDKUpdateInstall.htm,XDK,,,
B,file,TEST,,relnotes.txt,SYSTEM_DIR,,,

;Removes
;flavor,type,target root,target path,target file,flags
B,remove,XDK,,foo.c,
B,removedir,XDK,foobar,,

;Source (for copies)
;flavor,command,source id,source root,source path,source file,target root
B,source,FOOCPP,TEST,,foo.cpp,XDK

;Copy
;flavor,command,id,target path,target file,flags
B,copy,FOOCPP,samples,,
B,copy,FOOCPP,samples\slacker,rename.cpp,

;Registry
;flavor,command,root key,subkey,value name,value,flags
B,addreg,HKLM,software\Microsoft\XboxSDK,Slacker,New Value,
B,addreg,HKLM,software\Microsoft\XboxSDK,StupidDWORD,1234,D
B,addreg,HKLM,software\Microsoft\XboxSDK,Bogus,bogus,
B,delreg,HKLM,software\Microsoft\XboxSDK,Bogus,,

;Shortcut
;flavor,command,target root,target path,target file,shortcut path,shortcut file,description
B,shortcut,XDK,,relnotes.txt,,relnotes.lnk,Xbox SDK Release Notes
