
TARGETPATH=$(BASEDIR)\private\lib
TARGETTYPE=LIBRARY

#USER_C_FLAGS=/P /C


INCLUDES=$(BASEDIR)\private\inc\crypto; \
         $(BASEDIR)\public\sdk\inc; \
         $(BASEDIR)\private\ntos\inc; \
         $(BASEDIR)\public\sdk\inc\crt 
          
SOURCES= xcbase.c  \
         umkm.c

UMTYPE=console
              