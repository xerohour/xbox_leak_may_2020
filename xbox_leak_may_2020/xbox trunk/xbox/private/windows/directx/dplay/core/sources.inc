!include ..\..\sources.inc

SYNCHRONIZE_DRAIN=1

PRECOMPILED_CXX=1
PRECOMPILED_INCLUDE=..\dncorei.h
PRECOMPILED_PCH=dncorei.pch
PRECOMPILED_OBJ=dncorei.obj

!if $(FREEBUILD)
C_DEFINES=$(C_DEFINES) -DASSUME_VALID_PARAMETERS -DCINTERFACE
!else
C_DEFINES=$(C_DEFINES) -DDEBUG -DCINTERFACE
MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) /Od
!endif

INCLUDES=..; \
    $(INCLUDES)

SOURCES= ..\appdesc.cpp \
	 ..\async.cpp \
	 ..\asyncop.cpp \
	 ..\cancel.cpp \
	 ..\caps.cpp \
         ..\classfac.cpp \
	 ..\client.cpp \
	 ..\common.cpp \
	 ..\connect.cpp \
	 ..\connection.cpp \
	 ..\disconnect.cpp \
         ..\dncore.cpp \
	 ..\enum_sp.cpp \
	 ..\enumhosts.cpp \
         ..\globals.cpp \
         ..\groupcon.cpp \
	 ..\groupmem.cpp \
         ..\handles.cpp \
	 ..\handletable.cpp \
         ..\memoryfpm.cpp \
	 ..\migration.cpp \
         ..\msghandler.cpp \
	 ..\nametable.cpp \
	 ..\ntentry.cpp \
         ..\ntoplist.cpp \
	 ..\paramval.cpp \
	 ..\peer.cpp \
         ..\pools.cpp \
         ..\protocol.cpp \
	 ..\queuedmsg.cpp \
         ..\receive.cpp \
	 ..\request.cpp \
	 ..\server.cpp \
         ..\servprov.cpp \
         ..\spmessages.cpp \
         ..\syncevent.cpp \
	 ..\user.cpp \
         ..\voice.cpp \
	 ..\worker.cpp

USE_NOLIBS=1
