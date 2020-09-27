TARGETTYPE=LIBRARY
TARGETPATH=..\..\lib

SOURCES=\
	strerror1.c \
	strerror2.c \
	strerror3.c \
	_stricol.c \
	strlwr1.c \
	strlwr2.c \
	strlwr3.c \
	strupr1.c \
	strupr2.c \
	strupr3.c \
	memccpy1.c \
	memccpy2.c \
	memccpy3.c \
	memccpy4.c \
	memccpy5.c \
	memchr1.c \
	memchr2.c \
	memchr3.c \
	memchr4.c \
	memchr5.c \
	memcmp1.c \
	memcmp2.c \
	memcmp3.c \
	memcmp4.c \
	memcpy1.c \
	memcpy2.c \
	memcpy3.c \
	memcpy4.c \
	memicm5.c \
	memicmp1.c \
	memicmp2.c \
	memicmp3.c \
	memicmp4.c \
	memicmp5.c \
	memmove1.c \
	memmove2.c \
	memmove3.c \
	memmove4.c \
	memset1.c \
	memset2.c \
	memset3.c \
	memset4.c \
	strcat1.c \
	strcat2.c \
	strchr1.c \
	strchr2.c \
	strchr3.c \
	strchr4.c \
	strcmp1.c \
	strcmp2.c \
	strcmp3.c \
	strcmp4.c \
	strcoll1.c \
	strcoll2.c \
	strcoll3.c \
	strcoll4.c \
	strcpy1.c \
	strcpy2.c \
	strcpy3.c \
	strcspn1.c \
	strcspn2.c \
	strcspn3.c \
	strcspn4.c \
	strdup1.c \
	strdup2.c \
	strdup3.c \
	stricmp1.c \
	stricmp2.c \
	strlen1.c \
	strlen2.c \
	strncat1.c \
	strncat2.c \
	strncmp1.c \
	strncmp2.c \
	strncmp3.c \
	strncpy1.c \
	strncpy2.c \
	strncpy3.c \
	strnicm1.c \
	strnicm2.c \
	strnicm3.c \
	strnicm4.c \
	strnset1.c \
	strnset2.c \
	strnset3.c \
	strpbrk1.c \
	strpbrk2.c \
	strpbrk3.c \
	strpbrk4.c \
	strrchr1.c \
	strrchr2.c \
	strrchr3.c \
	strrchr4.c \
	strrev1.c \
	strrev2.c \
	strrev3.c \
	strset1.c \
	strset2.c \
	strset3.c \
	strstr1.c \
	strstr2.c \
	strtok1.c \
	strtok2.c \
	strtol1.c \
	strtol2.c \
	strtoul1.c \
	strtoul2.c \
	strxfrm1.c \
	strxfrm2.c \
	tstrchr2.c \
	tstrncat.c \
	tstrncpy.c \
	tstrstr2.c \
	drv.c

INCLUDES=$(BASEDIR)\private\test\crttests\test\clib;$(SDK_INC_PATH)

MSC_WARNING_LEVEL=/W2
MSC_OPTIMIZATION=/Od

SOURCES_USED=..\sources.inc
