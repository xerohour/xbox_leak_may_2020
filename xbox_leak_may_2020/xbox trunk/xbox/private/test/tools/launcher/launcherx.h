/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    launcherx.h

Abstract:

    Internal header file

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    07-31-2000  Created

*/

#ifndef _LAUNCHERX_H_
#define _LAUNCHERX_H_

#include <winsockx.h>
#include <xnetref.h>
#include "launcher.h"
#include "CSockServer.h"
#include "usbmanager.h"

extern void __stdcall penterDumpStats(void);

namespace Launcher {

typedef DWORD (*CMD_FUNC)(SOCKET sock, char *nextToken);

struct CommandSet
    {
    char *name;
    char *helpShort;
    char *helpLong;
    CMD_FUNC funct;
    };


#define CLEARFLAG(var, flag)                (var &= ~(flag))
#define SETFLAG(var, flag)                  (var |=  (flag))
#define ARRAYSIZE(a)        (sizeof(a) / sizeof(a[0]))
#define KILOBYTE 1024
#define MEGABYTE (KILOBYTE*KILOBYTE)

extern USBManager *usbmanager;

//
// error codes
//
#define ERR_OK      100
#define ERR_NOMEM   101 // out of memory
#define ERR_UNKNOWN 102 // unknown command
#define ERR_SYNTAX  103 // invalid syntax
#define ERR_EXIT    104 // quit the server
#define ERR_FILE    105

//
// common responses
//
extern char *responseHello;
extern char *responsePrompt;
extern char *responseTimeout;
extern char *responseOk;

//
// parser
//
const extern CommandSet commands[];
const extern CommandSet hiddenCommands[];
const extern unsigned commandLength;
const extern unsigned hiddenCommandLength;
extern DWORD ExecuteCommand(SOCKET sock, char *token, char *nextToken);
extern char* GetToken(char *str, char **tokenStart, size_t *tokenLen, char **nextToken);
extern DWORD Parser(SOCKET sock, CSockServer *server);
extern size_t LinePreProcessor(char *input, char *output);

//
// dos commands
//
extern DWORD ATTRIB(SOCKET sock, char *nextToken);
extern DWORD CALL(SOCKET sock, char *nextToken);
extern DWORD COPY(SOCKET sock, char *nextToken);
extern DWORD DIR(SOCKET sock, char *nextToken);
extern DWORD DEL(SOCKET sock, char *nextToken);
extern DWORD ECHO(SOCKET sock, char *nextToken);
extern DWORD FORMAT(SOCKET sock, char *nextToken);
extern DWORD MKDIR(SOCKET sock, char *nextToken);
extern DWORD MOVE(SOCKET sock, char *nextToken);
extern DWORD REM(SOCKET sock, char *nextToken);
extern DWORD RMDIR(SOCKET sock, char *nextToken);
extern DWORD SET(SOCKET sock, char *nextToken);
extern DWORD TIMEDATE(SOCKET sock, char *nextToken);
extern DWORD TYPE(SOCKET sock, char *nextToken);

//
// other commands
//
extern DWORD BARTEST(SOCKET sock, char *nextToken);
extern DWORD DISKIO(SOCKET sock, char *nextToken);
extern DWORD DUMP(SOCKET sock, char *nextToken);
extern DWORD FLUSH(SOCKET sock, char *nextToken);
extern DWORD HELP(SOCKET sock, char *nextToken);
extern DWORD HOTPLUG(SOCKET sock, char *nextToken);
extern DWORD MF(SOCKET sock, char *nextToken);
extern DWORD MODS(SOCKET sock, char *nextToken);
extern DWORD MU(SOCKET sock, char *nextToken);
extern DWORD RAWREAD(SOCKET sock, char *nextToken);
extern DWORD RAWWRITE(SOCKET sock, char *nextToken);
extern DWORD REBOOT(SOCKET sock, char *nextToken);
extern DWORD RUN(SOCKET sock, char *nextToken);
extern DWORD SAVER(SOCKET sock, char *nextToken);
extern DWORD STATS(SOCKET sock, char *nextToken);
extern DWORD STRESS(SOCKET sock, char *nextToken);
extern DWORD TEST(SOCKET sock, char *nextToken);
extern DWORD USB(SOCKET sock, char *nextToken);
extern DWORD WRITEINI(SOCKET sock, char *nextToken);
extern DWORD XMOUNT(SOCKET sock, char *nextToken);


//
// utils
//
extern WCHAR* Ansi2UnicodeHack(char *str);
extern char* Unicode2AnsiHack(unsigned short *str);
extern void HexDump(SOCKET sock, const unsigned char * buffer, DWORD length, DWORD offset=0);
void FlushDiskCache(void);
char* xPathFindFileName(char *name);
bool NetHit(SOCKET sock, unsigned usec=0);
bool OpenMU(unsigned port, unsigned slot, char *device);
bool CloseMU(unsigned port, unsigned slot);
bool MUDiagnosticIoctl(SOCKET sock, unsigned port, unsigned slot, DWORD ioctl, void *output, DWORD &size);
bool ReadMUCapacity(HANDLE handle);
DWORD RawRead(char *device, char *file, DWORD offset, DWORD size);
DWORD RawWrite(char *device, char *file, DWORD offset, DWORD size);






//
// Variable hash tables etc.
//

typedef void (*HASHOBJ_FUNC)(char *, void*, void*);

/*****************************************************************************

Class Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T> class HashEntry
    {
    public:
        char *key;
        T obj;
        HashEntry <T> *next;

    public:
        HashEntry();
        HashEntry(char *k, T o);
        ~HashEntry();
    };

/*****************************************************************************

Class Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE> class HashTable
    {
    public:
        HashTable();
        ~HashTable();

        HASHOBJ_FUNC clean;
        size_t m_size;

    public:
        HashEntry<T> *table[SIZE];

        BOOL Get(char *key, T& out);
        BOOL Add(char *key, T o);
        BOOL ForAll(HASHOBJ_FUNC fn, void *param);
        void SetCleanupFunct(HASHOBJ_FUNC c) { clean = c; }
    };

class Variable
    {
    public:
        char *value;
        size_t valueLen; // size of the buffer

    public:
        Variable();
        Variable(char *v);
        ~Variable();
    };

extern void DestroyVariable(char *name, void *p, void *param);

/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T>
HashEntry<T>::HashEntry()
    {
    clean = NULL;
    key = NULL;
    next = NULL;
    }


/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T>
HashEntry<T>::HashEntry(char *k, T o)
    {
    key = new char[strlen(k)+1];
    strcpy(key, k);
    obj = o;
    next = NULL;
    }

/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T>
HashEntry<T>::~HashEntry()
    {
    if(key) delete[] key;
    if(next) delete next;
    }

/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE>
HashTable<T, SIZE>::HashTable()
    {
    m_size = SIZE;
    clean = NULL;
    for(size_t i=0; i<SIZE; i++)
        {
        table[i] = NULL;
        }
    }


/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE>
HashTable<T, SIZE>::~HashTable()
    {
    if(clean) ForAll(clean, NULL);
    for(size_t i=0; i<SIZE; i++)
        {
        if(table[i]) delete table[i];
        }
    }

/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE>
BOOL HashTable<T, SIZE>::Get(char *key, T& out)
    {
    if(!key) return FALSE;

    size_t i = (key[0] + key[1]) % SIZE;

    if(!table[i]) return FALSE;

    for(HashEntry<T> *h = table[i]; h; h=h->next)
        {
        if(strcmp(h->key, key) == 0)
            {
            out = h->obj;
            return TRUE;
            }
        }

    return FALSE;
    }

/*****************************************************************************

Routine Description:

    Add the key and the associated object. Add the new HashEntry at the end
    of the list to give precedence to items added first.

Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE>
BOOL HashTable<T, SIZE>::Add(char *key, T o)
    {
    if(!key) return FALSE;
    size_t i = (key[0] + key[1]) % SIZE;

    if(!table[i])
        {
        table[i] = new HashEntry<T>(key, o);
        return TRUE;
        }
    else
        {
        HashEntry<T> *h = table[i];
        do
            {
            // replace entry with the same key
            if(_stricmp(h->key, key) == 0)
                {
                if(clean) clean(h->key, &(h->obj), NULL);
                h->obj = o;
                return TRUE;
                }

            // otherwise add it to the end
            if(!h->next)
                {
                h->next = new HashEntry<T>(key, o);
                return TRUE;
                }

            h=h->next;
            } while(h);
        }

    return FALSE;
    }

/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE>
BOOL HashTable<T, SIZE>::ForAll(HASHOBJ_FUNC fn, void *param)
    {
    for(size_t i=0; i<SIZE; i++)
        {
        if(table[i])
            {
            for(HashEntry<T> *h = table[i]; h; h=h->next)
                {
                fn(h->key, &(h->obj), param);
                }
            }
        }
    return TRUE;
    }

} // namespace Launcher

#endif //_LAUNCHERX_H_