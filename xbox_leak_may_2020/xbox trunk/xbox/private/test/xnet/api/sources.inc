!IF 0

Copyright (c) Microsoft Corporation

Module Name:

    sources.

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.

!ENDIF



TARGETPATH=$(BASEDIR)\private\test\lib

PRECOMPILED_INCLUDE=..\precomp.h
PRECOMPILED_PCH=precomp.pch
PRECOMPILED_OBJ=precomp.obj
PRECOMPILED_CXX=1

USECXX_FLAG=/Tp

INCLUDES=\
    $(INCLUDES); \
    $(BASEDIR)\private\test\inc; \
    ..\; \

SOURCES=\
    htonl.c                  \
    htons.c                  \
    ntohl.c                  \
    ntohs.c                  \
    inet_addr.c              \
    wsastartup.c             \
    wsacleanup.c             \
    socket.c                 \
    bind.c                   \
    listen.c                 \
    accept.c                 \
    connect.c                \
    send.c                   \
    wsasend.c                \
    sendto.c                 \
    wsasendto.c              \
    recv.c                   \
    wsarecv.c                \
    recvfrom.c               \
    wsarecvfrom.c            \
    getsockname.c            \
    getpeername.c            \
    ioctlsocket.c            \
    wsagetoverlappedresult.c \
    wsacanceloverlappedio.c  \
    select.c                 \
    fdsmacros.c              \
    shutdown.c               \
    closesocket.c            \
    getsockopt.c             \
    setsockopt.c             \
    XNetInAddrToString.c     \
    XNetRandom.c             \
    XNetCreateKey.c          \
    XNetRegisterKey.c        \
    XNetUnregisterKey.c      \
    XNetXnAddrToInAddr.c     \
    XNetInAddrToXnAddr.c     \
    XNetDnsLookup.c          \
    XNetDnsRelease.c         \
