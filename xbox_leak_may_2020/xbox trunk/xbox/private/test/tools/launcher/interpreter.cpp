/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    interpreter.cpp

Abstract:

    Command interpreter and parser.

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    07-31-2000  Created

*/
#include "stdafx.h"
#include "launcherx.h"

namespace Launcher {


//
// general responses
//
char *responseHello = "\r\nXBox Harness Remote Window\r\n";
char *responsePrompt = "> ";
char *responseTimeout = "\r\n\r\nConnection timed out. Game Over.\r\n";
char *responseOk = "Done\r\n";
USBManager *usbmanager = NULL;

//
// Command Names, descriptions, help text, and function pointers
//
const CommandSet commands[] =
    {
    {"ATTRIB",
        "Displays the file attributes.",
        "ATTRIB [drive:\\path\\file]\r\n\r\n  [drive:\\path\\file]\r\n    Specifies drive, directory, and/or files to list.",
        ATTRIB},
    {"BARTEST",
        "Displays a series of colored bars on the video monitor.",
        "BARTEST",
        BARTEST},
    {"CALL",
        "Executes a batch program.",
        "CALL [drive:\\path\\file]\r\n\r\n  [drive:\\path\\file]\r\n    Specifies drive, directory, and file to execute.",
        CALL},
    {"COPY",
        "Copies one or more files to another location.",
        "COPY [drive:\\path\\file] [drive:\\path\\file]\r\n\r\n  [drive:\\path\\file]\r\n    Specifies drive, directory, and files to copy.",
        COPY},
    {"DEL",
        "Deletes one or more files.",
        "DEL [drive:\\path\\file]\r\n\r\n  [drive:\\path\\file]\r\n    Specifies drive, directory, and files to remove.",
        DEL},
    {"DIR",
        "Displays a list of files and subdirectories in a directory.",
        "DIR [drive:\\path\\file]\r\n\r\n  [drive:\\path\\file]\r\n    Specifies drive, directory, and/or files to list.",
        DIR},
    {"DISKIO",
        "Performs continuous reads or writes to a disk.",
        "DISKIO [IN|OUT] [drive]\r\n\r\n  IN       Specifies constant reads (default).\r\n  OUT      Specifies constant writes.\r\n  [drive]  is the letter for the mounted drive (F is default).",
        DISKIO},
    {"DUMP",
        "Displays the contents of a binary file.",
        "DUMP [drive:\\path\\file]\r\n\r\n  [drive:\\path\\file]\r\n    Specifies drive, directory, and file to display as a hex dump.",
        DUMP},
    {"FLUSH",
        "Flushes the cache.",
        "FLUSH",
        FLUSH},
    {"FORMAT",
        "Formats a FAT device.",
        "FORMAT [device name]\r\n\r\n  [device name]\r\n    \\Device\\MemoryUnit01_01 (mu port numbers are hex values, 1 based)\r\n    \\Device\\Harddisk0\\partition5",
        FORMAT},
    {"HELP",
        "Provides Help information for remote commands.",
        "HELP [command-name]",
        HELP},
    {"HOTPLUG",
        "Displays or sets whether or not devices will be initialized on insert.",
        "HOTPLUG [ON|OFF]",
        HOTPLUG},
    {"MD",
        "Creates a directory.",
        "MD [drive:][path]\r\n\r\n  [drive:][path]\r\n    Specifies drive, and directory to create.",
        MKDIR},
    {"MF",
        "Creates a file filled with a specific byte pattern.",
        "MF [drive:\\path\\file] [size] [pattern]\r\n\r\n  [drive:\\path\\file]  Specifies drive, path and file to create.\r\n  [size]         Maximum size of the file.\r\n  [pattern]      Hex pattern for the data allows up to 20 bytes.\r\n                 (eg: \"00 55 ff\")",
        MF},
    {"MODS",
        "Lists the modules callable with the RUN command.",
        "MODS",
        MODS},
    {"MOVE",
        "Moves one or more files from one directory to another directory.",
        "MOVE [drive:\\path\\file] [drive:\\path\\file]\r\n\r\n  [drive:\\path\\file]\r\n    Specifies drive, directory, and files to move.",
        MOVE},
    {"MU",
        "Handles MU specific operations",
        "MU [drive] [operation]\r\n\r\n  [drive]    The drive to perform the operation on.\r\n  [operation]\r\n    MOUNT - Mounts/remounts the specified MU drive\r\n\r\n  Diagnostic Operations (requires special build)\r\n    BADBLOCK - Displays the bad block table.\r\n    CLEARBADBLOCK - Flushes the bad block table.\r\n    MEMTEST - Performs the memory test.",
        MU},
    {"RD",
        "Removes a directory.",
        "RD [drive:][path]\r\n\r\n  [drive:][path]\r\n    Specifies drive, and directory to remove.",
        RMDIR},
    {"RAWREAD",
        "Performs a low level read from a device and writes the data to a file.",
        "RAWREAD [device] [drive:\\path\\file] [size]\r\n\r\n  [device]\r\n    Specifies the source device (eg \\Device\\MemoryUnit01_01)\r\n  [drive:\\path\\file]\r\n    Specifies the destination file.\r\n  [size]\r\n    Specifies the number of bytes to transfer.",
        RAWREAD},
    {"RAWWRITE",
        "Reads data from a file and low level writes it to a device.",
        "RAWWRITE [device] [drive:\\path\\file] [size]\r\n\r\n  [device]\r\n    Specifies the destination device (eg \\Device\\MemoryUnit01_01)\r\n  [drive:\\path\\file]\r\n    Specifies the data file.\r\n  [size]\r\n    Specifies the number of bytes to transfer.",
        RAWWRITE},
    {"REBOOT",
        "Reboots the system.",
        "REBOOT",
        REBOOT},
    {"REN",
        "Renames a file or files.",
        "REN [drive:\\path\\file] [drive:\\path\\file]\r\n\r\n  [drive:\\path\\file]\r\n    Specifies drive, directory, and files to rename.",
        MOVE},
    {"RUN",
        "Executes a Harness DLL.",
        "RUN [dll-name]\r\n\r\n  [dll-name]\r\n    Specifies the name of the harness dll to execute.\r\n\r\nUse the MODS command to see a list of callable modules.",
        RUN},
    {"SAVER",
        "Screen saver.",
        "SAVER\r\n    You must have xboxlogo.bmp in your tdata\\A7049955\\media directory",
        SAVER},
    {"SET",
        "Sets or views internal variables.",
        "SET [name=value]\r\n\r\nUse %name% to evaluate (eg format %MUF%)",
        SET},
    {"STATS",
        "Displays system resource information.",
        "STATS [/(VER|MEM|PS|NET|DISK|DEVICE|ALL)]\r\n\r\n  /ALL is default.",
        STATS},
    {"STRESS",
        "Shuts down stress.",
        "STRESS [mode]",
        STRESS},
    {"TIME",
        "Displays the system time and date.",
        "TIME",
        TIMEDATE},
    {"TYPE",
        "Displays the contents of a text file.",
        "TYPE [drive:\\path\\file]\r\n\r\n  [drive:\\path\\file]\r\n    Specifies drive, directory, and file to display.",
        TYPE},
    {"USB",
        "Dumps out the state of any connected XID devices.",
        "USB [num seconds]\r\n\r\n  [num seconds]    Specifies the duration of the dump (30 is default).",
        USB},
    {"WRITEINI",
        "Adds or edits an entry in the Harness INI file.",
        "WRITEINI section key=value\r\n\r\n  section       The INI file section, typically a dll name.\r\n  key=value     key is the name of the parameter and value is the data.\r\n\r\n  NOTE: you can use \"type t:\\testini.ini\" to view your changes.",
        WRITEINI},
    {"XMOUNT",
        "Mounts/remounts the specified MU drive.",
        "XMOUNT drive",
        XMOUNT},
    {"KILL",
        "Stops the current session and shuts down the remote server.",
        "KILL",
        NULL},
    {"QUIT|EXIT|BYE",
        "Exits this remote session and allows someone else to connect.",
        "QUIT",
        NULL}
    };

//
// these never get displayed in help
//
const CommandSet hiddenCommands[] =
    {
    {"?",
        "Provides Help information for remote commands.",
        "? [command-name]",
        HELP},
    {"ECHO",
        "Writes the input to the screen.",
        "ECHO [stuff]",
        ECHO},
    {"ERASE",
        "Deletes one or more files.",
        "ERASE [drive:\\path\\file]\r\n\r\n  [drive:\\path\\file]\r\n    Specifies drive, directory, and files to remove.",
        DEL},
    {"MKDIR",
        "Creates a directory.",
        "MKDIR [drive:][path]\r\n\r\n  [drive:][path]\r\n    Specifies drive, and directory to create.",
        MKDIR},
    {"REM",
        "Records comments (remarks) in batch files.",
        "",
        REM},
    {"RENAME",
        "Renames a file or files.",
        "RENAME [drive:\\path\\file] [drive:\\path\\file]\r\n\r\n  [drive:\\path\\file]\r\n    Specifies drive, directory, and files to rename.",
        MOVE},
    {"RMDIR",
        "Removes a directory.",
        "RMDIR [drive:][path]\r\n\r\n  [drive:][path]\r\n    Specifies drive, and directory to remove.",
        RMDIR},
    {"START",
        "Executes a Harness DLL.",
        "START [dll-name]\r\n\r\n  [dll-name]    Specifies the name of the harness dll to execute.\r\n\r\nUse the MODS command to see a list of callable modules.",
        RUN},
    {"TEST",
        "just a stub for internal tests",
        "",
        TEST}
    };

//
// command list sizes
//
const unsigned commandLength = ARRAYSIZE(commands);
const unsigned hiddenCommandLength = ARRAYSIZE(hiddenCommands);

HashTable<Variable*, 13> variables;

/*****************************************************************************

Routine Description:

    GetToken
    returns the location of the first and second tokens and the length of the first token

Arguments:

    char *str           - [in] string buffer
    char **tokenStart   - [out] start of the first token
    size_t *tokenLen    - [out] length of the first token
    char **nextToken    - [out] start of the next token

Return Value:

    char * - same as tokenStart

Notes:

    Example:
        GetToken("\t    abc  def ghij", &tokenStart, &tokenLen, &nextToken);

        printf(tokenStart) -> "abc  def ghij"
        printf(nextToken)  -> "def ghij"
        tokenLen == 3

*****************************************************************************/
char* GetToken(char *str, char **tokenStart, size_t *tokenLen, char **nextToken)
    {
    size_t leadingBlank = strspn(str, " \t\n");
    str += leadingBlank;
    size_t len = strcspn(str, " \t\n");
    size_t trailingBlank = strspn(str + len, " \t\n");

    *tokenStart = str;
    *tokenLen = len;
    *nextToken = str+len+trailingBlank;

    return str;
    }


/*****************************************************************************

Routine Description:

    ExecuteCommand
    Given a token it tries to resolve it into a command name and then call the
    appropriate function.

Arguments:

    SOCKET sock     - [in] socket all communication should be sent through
    char *token     - [in] the name of the command
    char *nextToken - [in] the string containing any parameters to the cmd

Return Value:

    DWORD

*****************************************************************************/
DWORD ExecuteCommand(SOCKET sock, char *token, char *nextToken)
    {
    usbmanager->CheckForHotplugs();

    // check for help switch
    if(strcmp(nextToken, "/?") == 0) return HELP(sock, token);

    // execute the command if its valid
    for(unsigned i=0; i<commandLength; i++)
        {
        if(strcmp(commands[i].name, token) == 0)
            {
            return commands[i].funct(sock, nextToken);
            }
        }
    for(i=0; i<hiddenCommandLength; i++)
        {
        if(strcmp(hiddenCommands[i].name, token) == 0)
            {
            return hiddenCommands[i].funct(sock, nextToken);
            }
        }

    return ERR_UNKNOWN;
    }



/*****************************************************************************

Routine Description:

    Parser

    1. Waits for input on the connected socket
    2. Pieces together a command
    3. Tries to execute that command (if its valid)
    4. Goto 1

Arguments:

    SOCKET sock         - [in] socket all communication goes through
    CSockServer *server - [in] pointer to the server that connected the socket

Return Value:

    DWORD

Notes:

    0x0D 0x0A           Enter
    0x0D                keypad Enter
    0x08                Backspace
    0x09                Tab
    0x7F                Del
    0x1B                Esc
    0x1B 0x5B 0x41      up arrow
    0x1B 0x5B 0x42      down arrow
    0x1B 0x5B 0x43      right arrow
    0x1B 0x5B 0x44      left arrow

*****************************************************************************/
DWORD Parser(SOCKET sock, CSockServer *server)
    {
    DebugPrint("LAUNCHER: connected to client\n");

    variables.SetCleanupFunct(DestroyVariable);
    variables.Add("MUF", new Variable("\\Device\\MemoryUnit01_01"));
    variables.Add("MUG", new Variable("\\Device\\MemoryUnit01_02"));
    variables.Add("MUH", new Variable("\\Device\\MemoryUnit02_01"));
    variables.Add("MUI", new Variable("\\Device\\MemoryUnit02_02"));
    variables.Add("MUJ", new Variable("\\Device\\MemoryUnit03_01"));
    variables.Add("MUK", new Variable("\\Device\\MemoryUnit03_02"));
    variables.Add("MUL", new Variable("\\Device\\MemoryUnit04_01"));
    variables.Add("MUM", new Variable("\\Device\\MemoryUnit04_02"));
    variables.Add("8MEG", new Variable("8388608"));

    const unsigned HISTORY_MAX_SIZE = 10;
    char *buffer = new char[1024];
    char *command = new char[1024];
    char *history[HISTORY_MAX_SIZE];
    char *parameters = new char[1024];
    unsigned historyLast, historySize, historyPtr;
    for(historyLast=0; historyLast<HISTORY_MAX_SIZE; historyLast++)
        {
        history[historyLast] = new char[1024];
        memset(history[historyLast], 0, 1024);
        }
    historyLast=historyPtr = HISTORY_MAX_SIZE-1;
    historySize = 0;
    usbmanager = new USBManager;
    if(!buffer || !command || !usbmanager || !parameters)
        {
        DebugPrint("LAUNCHER: Out of memory!\n");
        delete[] buffer;
        delete[] command;
        delete[] parameters;
        delete usbmanager;
        return ERR_NOMEM;
        }
    int len;
    DWORD err;
    char *t;
    char *token;
    char *nextToken;
    size_t tokenLen;
    buffer[0] = '\0';
    command[0] = '\0';

    SockPrint(sock, responseHello);

    CALL(sock, "t:\\autoexec.bat");
    CALL(sock, "z:\\autoexec.bat");

    SockPrint(sock, "\r\n\r\n%s", responsePrompt);

    do
        {
        // wait for data from client
        if(!server->IsDataAvailable(sock))
            {
            // we timed out
            SockPrint(sock, responseTimeout);
            break;
            }

        len = recv(sock, buffer, 1024, 0);
        if(len == 0 || len == SOCKET_ERROR) break;
        buffer[len] = '\0';

        //HexDump(INVALID_SOCKET, (unsigned char*)buffer, len);

        // handle escape key
        if(buffer[0] == 0x1B && len == 1)
            {
            // just a plain ol esc key - clear the current input
            command[0]='\0';
            SockPrint(sock, "\r%79s\r%s", "", responsePrompt);
            continue;
            }
        else if(buffer[0] == 0x1B)
            {
            // handle escape sequences such as arrow keys
            if(len == 3 && buffer[2] == 0x41) // up arrow
                {
                if(historySize == 0) continue;
                strcpy(command, history[historyPtr]);
                historyPtr = (historySize+historyPtr-1)%historySize;
                SockPrint(sock, "\r%79s\r%s%s", "", responsePrompt, command);
                continue;
                }
            else if(len == 3 && buffer[2] == 0x42) // down arrow
                {
                if(historySize == 0) continue;
                strcpy(command, history[historyPtr]);
                historyPtr = (historyPtr+1)%historySize;
                SockPrint(sock, "\r%79s\r%s%s", "", responsePrompt, command);
                continue;
                }
            }

        // ignore invalid inputs
        if(buffer[0] >= 127) continue;
        if(buffer[0] < 32 && buffer[0]!=8 && buffer[0]!='\r') continue;

        // add the input to the command
        strcat(command, buffer);

        // handle backspaces
        while((t = strchr(command, '\x08')) != NULL)
            {
            if(t == command) memmove(t, t+1, strlen(t));
            else memmove(t-1, t+1, strlen(t));
            }
        /*
        // handle erronous '\r' characters when pasting text in telnet client
        // but this will also prevent clients from sending commands and \r in
        // the same packet
        while((t = strchr(command, '\r')) != NULL && len>2)
            {
            memmove(t, t+1, strlen(t));
            }
            */

        // reprint the current command
        SockPrint(sock, "\r%79s\r%s%s", "", responsePrompt, command);

        // execute the command if they hit Enter
        if((t=strchr(command, '\r')) != 0)
            {
            t[0] = '\0';

            // use the last command if they just hit Enter
            if(command[0] == '\0')
                {
                strcpy(command, history[historyLast]);
                }
            else // save the command in the history
                {
                if(_stricmp(command, history[historyLast]) != 0)
                    {
                    if(historySize<HISTORY_MAX_SIZE) ++historySize;
                    historyPtr=historyLast = (historyLast+1)%HISTORY_MAX_SIZE;
                    strcpy(history[historyLast], command);
                    }
                else
                    {
                    historyPtr = historyLast;
                    }
                }

            // make it uppercase
            _strupr(command);

            // break the pieces out
            GetToken(command, &token, &tokenLen, &nextToken);
            token[tokenLen] = '\0';

            // look for exit commands
            if(strcmp(token, "EXIT")==0 || _stricmp(token, "BYE")==0 || _stricmp(token, "QUIT")==0)
                {
                SockPrint(sock, responseOk);
                break;
                }
            else if(strcmp(token, "KILL")==0)
                {
                server->SetExitFlag(TRUE);
                SockPrint(sock, responseOk);
                break;
                }

            // expand any variables
            LinePreProcessor(nextToken, parameters);

            // run the command
            err = ExecuteCommand(sock, token, parameters);

            if(err == ERR_UNKNOWN)
                {
                // others
                if(strcmp(token, "LS")==0)
                    {
                    SockPrint(sock, "This aint no freakin unix box!\r\n");
                    }

                // unknown command
                else
                    {
                    SockPrint(sock, "Unknown Command: %s\r\nType HELP for a list of supported commands\r\n", command);
                    }
                }

            command[0] = '\0';
            SockPrint(sock, responsePrompt);
            }

        } while(!server->GetExitFlag());

    delete usbmanager;
    delete[] buffer;
    delete[] command;
    delete[] parameters;
    for(historyLast=0; historyLast<HISTORY_MAX_SIZE; historyLast++)
        {
        delete[] history[historyLast];
        }

    return ERR_OK;
    }


Variable::Variable()
    {
    value = NULL;
    valueLen = 0;
    }

Variable::Variable(char *v)
    {
    valueLen = strlen(v) + 1;
    value = new char[valueLen];
    if(value) strcpy(value, v);
    }

Variable::~Variable()
    {
    if(value) delete[] value;
    value = NULL;
    }

// Called by the HashTable to clean up the variables
// As per "variables.SetCleanupFunct(DestroyVariable);"
void DestroyVariable(char *name, void *p, void *param)
    {
    if(!p) return;
    Variable *v = *(Variable**)p;
    delete v;
    }



size_t LinePreProcessor(char *input, char *output)
    {
    char *keyToken;
    char *nextToken;
    size_t offset;
    size_t tokenLen;
    size_t totalProcessed=0;
    Variable *var;
    char *params = NULL;
    char spacer; // the char we temporarily replace with the null

    if(!input || !output) return FALSE;

    output[0] = '\0';

    while((keyToken = strstr(input, "%")) != NULL)
        {
        offset = (size_t)(keyToken - input);

        // copy up to the keyword
        memcpy(output, input, offset);
        output[offset] = '\0';
        output += offset;

        // we were evaluating a function's params + and hit the end
        if(offset && *(keyToken-1)==')')
            {
            output[-1] = '\0';
            totalProcessed += offset-1;
            delete[] params;
            return totalProcessed;
            }

        // skip "%"
        keyToken += 1;
        offset += 1;

        // find end of the token
        tokenLen = strcspn(keyToken, "%(");

        // PRECEDENCE
        if(keyToken[0] == '(')
            {
            offset += LinePreProcessor(&keyToken[tokenLen+1], params) + 2; // +2 "()"
            if(variables.Get(params, var))
                {
                memcpy(output, var->value, var->valueLen);
                output += var->valueLen-1;
                }
            else if((nextToken = strstr(keyToken, "%")) != NULL)
                {
                // unknown variable, so just copy it (minus the % delimators)
                sprintf(output, "%s", params);
                }
            }

        // INLINE FUNCTIONS
        else if(keyToken[tokenLen] == '(')
            {/*
            if(!params) params = new char[4096];
            // evaluate the arguments
            offset += tokenLen;
            size_t len = LinePreProcessor(&keyToken[tokenLen+1], params) + 3; // +3 "()%"
            offset += len;

            INLINE_FUNC fn;

            spacer = keyToken[tokenLen];
            keyToken[tokenLen] = '\0';
            if(inlineFunctions.Get(keyToken, fn))
                {
                output += (this->cmdClass.*fn)(params, output);
                }
            else
                {
                // unknown function, so just copy it (with its params)
                output += sprintf(output, "%s(%s)", keyToken, params);
                }
            keyToken[tokenLen] = spacer;*/
            }

        // VARIABLES
        else
            {
            spacer = keyToken[tokenLen];
            keyToken[tokenLen] = '\0';
            if(variables.Get(keyToken, var))
                {
                offset += strlen(keyToken) + 1; // +1 for the "%"
                memcpy(output, var->value, var->valueLen);
                output += var->valueLen-1;
                }/*
            else if(strcmp(keyToken, "HTTPSTATUS") == 0)
                {
                offset += strlen(keyToken) + 1; // +1 for the "%"
                short status;
                engine->get_httpStatus(&status);
                output += sprintf(output, "%03d", status);
                }*/
            else if((nextToken = strstr(keyToken, "%")) != NULL)
                {
                // unknown variable, so just copy it (minus the % delimators)
                memcpy(output, keyToken, tokenLen+1);
                output += tokenLen;
                offset += tokenLen+1;
                }
            keyToken[tokenLen] = spacer;
            }

        input += offset;
        totalProcessed += offset;
        }

    // copy the last part of the string
    strcpy(output, input);
    delete[] params;
    return totalProcessed;
    }

} //namespace Launcher