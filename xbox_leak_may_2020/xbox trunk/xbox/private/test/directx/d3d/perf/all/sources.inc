INCLUDES =          $(PRIVATE_INC_PATH);                               \
                    $(BASEDIR)\private\test\inc;                       \
                    $(BASEDIR)\private\test\multimedia\inc;              \
                    $(SDK_INC_PATH);              \
					$(BASEDIR)\private\ntos\inc; 



SOURCES= 			                                       \
    ..\all.cpp                                       \
	..\benmark5.cpp                                       \
	..\fillrate.cpp                                       \
	..\inifile.cpp                                       \
	..\bitfont.cpp                                       \
	..\fsaa.cpp                                       \
	..\perf.cpp                                       \
  ..\utils.cpp                                       \
..\d3dapp.cpp \
..\d3dfile.cpp \
..\d3dutil.cpp  \
..\dxutil.cpp    \
                  
