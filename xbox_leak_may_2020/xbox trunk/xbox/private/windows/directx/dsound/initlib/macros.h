/***************************************************************************
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       macros.h
 *  Content:    Misc. helper macros.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  08/06/01    dereks  Created.
 *
 ***************************************************************************/

#ifndef __MACROS_H__
#define __MACROS_H__

#define DEFINEPTYPE(name) \
    typedef name *P##name

#define DEFINELPTYPE(name) \
    typedef name *LP##name

#define DEFINELPCTYPE(name) \
    typedef const name *LPC##name

#ifdef __cplusplus                  

#define DEFINEREFTYPE(name)  \
    typedef const name &REF##name

#else // __cplusplus                

#define DEFINEREFTYPE(name) \
    typedef const name *REF##name

#endif // __cplusplus               

#define DEFINETYPEFORMS(name) \
    DEFINEPTYPE(name); DEFINELPTYPE(name); DEFINELPCTYPE(name); DEFINEREFTYPE(name)

#define DEFINETYPE(name, type) \
    typedef type name; DEFINETYPEFORMS(name)
                                    
#define BEGIN_DEFINE_STRUCT() \
    typedef struct {

#define BEGIN_DEFINE_STRUCT_(name) \
    typedef struct name {

#define END_DEFINE_STRUCT(name) \
    } name; DEFINETYPEFORMS(name)

#define BEGIN_DEFINE_UNION() \
    typedef union {

#define BEGIN_DEFINE_UNION_(name) \
    typedef union name {

#define END_DEFINE_UNION(name) \
    } name; DEFINETYPEFORMS(name)

#define BEGIN_DEFINE_ENUM()  \
    typedef enum {

#define BEGIN_DEFINE_ENUM_(name) \
    typedef enum name {

#define END_DEFINE_ENUM(name) \
    } name; DEFINETYPEFORMS(name)

#define END_DEFINE_ENUM_() \
    }

#define DIVUP(a, b) \
    (((a) + ((b) - 1)) / (b))

#define BLOCKALIGN(a, b) \
    (((a) / (b)) * (b))

#define BLOCKALIGNPAD(a, b) \
    (DIVUP(a, b) * (b))

#define MAKEBOOL(a) \
    (!!(a))

#define NUMELMS(a) \
    (sizeof(a) / sizeof((a)[0]))

#define HRESULT_FROM_POINTER(p) \
    ((p) ? S_OK : E_OUTOFMEMORY)

#define HRFROMP(p) \
    HRESULT_FROM_POINTER(p)

#ifdef __cplusplus

#define IS_NULL_GUID(guid) \
    (!&(guid) || IsEqualGUID(guid, GUID_NULL))

#else // __cplusplus

#define IS_NULL_GUID(pguid) \
    (!(pguid) || IsEqualGUID(pguid, &GUID_NULL))

#endif // __cplusplus

#define IS_VALID_HANDLE_VALUE(h) \
    ((h) && (INVALID_HANDLE_VALUE != (h)))

#define CLOSE_HANDLE(h) \
    { \
        if(IS_VALID_HANDLE_VALUE(h)) \
        { \
            CloseHandle(h); \
            (h) = NULL; \
        } \
    }

#define INTERLOCKED_EXCHANGE(a, b) \
    InterlockedExchange((LPLONG)&(a), (LONG)(b))

#define INTERLOCKED_INCREMENT(a) \
    InterlockedIncrement((LPLONG)&(a))

#define INTERLOCKED_DECREMENT(a) \
    InterlockedDecrement((LPLONG)&(a))

#define CHECKRANGE(a, min, max) \
    { \
        if((a) < (min)) \
        { \
            (a) = (min); \
        } \
        else if((a) > (max)) \
        { \
            (a) = (max); \
        } \
    }

DEFINELPCTYPE(LONG);
DEFINELPCTYPE(DWORD);
DEFINEREFTYPE(DS3DLISTENER);
DEFINEREFTYPE(DS3DBUFFER);
DEFINEREFTYPE(D3DXVECTOR3);
DEFINEREFTYPE(XMEDIAPACKET);
DEFINELPCTYPE(XMEDIAPACKET);
DEFINEREFTYPE(DSI3DL2LISTENER);
DEFINEREFTYPE(DSI3DL2BUFFER);

#endif // __MACROS_H__
