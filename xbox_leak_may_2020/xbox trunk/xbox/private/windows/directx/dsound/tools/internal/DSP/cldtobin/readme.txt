CldToBin: Converts Motorola .cld files to binary or Hex text  files

Gints Klimanis
gklimanis@nvidia.com

***********************************************************

1) Use the -a option to generate your .cld files.  

asm56300 -a -Bb.cld file1.asm file2.asm file3.asm > err.txt

2) Use CldToBin (nVidia internal utility) to pack your P-memory
code into a hex binary or text file.  

3) The -24to32 option will pack your 24-bit words into the
lower 24 bits of a 32 bit container.  This is suitable for system
memory images.

Check out a.bat for example usage.
You may want to exclude some P Memory sections (256 word
Interrupt Vector Table, real-time interrupt service routine
code, etc.).  If you assemble with these sections as the last
files in the command line, it's easier to identify which 
sections to exclude.

Output to test file:
cldtobin.exe -v -noX -noY -asText b.cld out.txt
cldtobin.exe -v -noX -noY         b.cld out.bin

Output to binary file:
cldtobin.exe -v -noX -noY -excludeN 0 -excludeN 4 -asText b.cld out2.txt
cldtobin.exe -v -noX -noY -excludeN 0 -excludeN 4         b.cld out2.bin



