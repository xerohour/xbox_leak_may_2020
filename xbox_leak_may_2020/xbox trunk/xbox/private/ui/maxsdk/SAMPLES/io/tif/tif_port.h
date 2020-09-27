#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <buildver.h>

#define KINETIX_TIFF_CHANGES
#define MICROSOFT
#define NO_FSTAT
#define m386
#define BSDTYPES
#define MAXFNAME  13
typedef FILE *FILEHNDL;	
#define NOFILE 0
#ifndef EOS
#define EOS      '\0'
#endif

typedef unsigned long ulong;

#define  alloc(size) malloc(size)
#define index(str,char) strchr(str,char)

#define read(file, buf, size)	fread(file->_fd,buf,1,size)
#define write(file, buf, size) fwrite(file->_fd,buf,1,size)
#define myseek(file, off, type) fseek(file->_fd, (unsigned long)off, type)
#define c_open(name, mode) fopen(name,mode)
#define c_create(name, mode) fopen(name,mode)
#define open(name, mode,prot) fopen(name,mode)
#define close(file) fclose(file)

#define Attr_archive "wb"
#define For_updating "ab"
#define For_reading	"rb"


#undef NULL
#define NULL 0

