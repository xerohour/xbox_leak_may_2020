/*++

Copyright (c) Microsoft Coroporation. All rights reserved.

Module Name:

    cmdutils.cpp

Abstract:

    This module implements various utility routines to send commands to
    coverage debug extension (coverage.dxt).  It can be used for collecting
    code coverage data.

--*/

#include "xbcovcmdp.h"

DWORD
CoverageFormatCommand(
    IN  ULONG  Command,
    OUT PSTR   Buffer,
    IN  SIZE_T BufferSize,
    IN  PCSTR   Format OPTIONAL,
    IN  va_list argptr OPTIONAL
    )
/*++

Routine Description:

    This routine formats command being sent to Xbox into text string format.
    The optional command parameters also can be specified.

Arguments:

    Command - Command ID, e.g. CoverageCmdSave

    Buffer - Output buffer to receive formatted command

    BufferSize - Size in bytes of output buffer

    Format - Format string, see printf for more information

    argptr - Optional parameter (use with Format)

Return Value:

    NOERROR or HRESULT of ERROR_INSUFFICIENT_BUFFER

--*/
{
    int BytesStored;
    SIZE_T CmdLen;
    CONST SIZE_T MinBufferSize = sizeof(DM_COVERAGE_COMMAND) - 1 +
        sizeof(COVERAGE_COMMAND_HEADER) - 1 + sizeof('\0');

    ASSERT( Command < CoverageCmdMax );

    //
    // Verify that buffer size is bug enough to hold BBCOVER command +
    // command id + NULL terminator
    //

    if ( BufferSize < MinBufferSize ) {
        return ERROR_INSUFFICIENT_BUFFER;
    }

    //
    // Copy BBCOVER command, command id header and append the command id
    //

    strcpy( Buffer, DM_COVERAGE_COMMAND );
    strcat( Buffer, COVERAGE_COMMAND_HEADER );
    CmdLen = MinBufferSize - sizeof('\0');
    CmdLen += strlen( _ultoa(Command, &Buffer[CmdLen], 10) );

    //
    // Format optional parameter if format string is not null
    //

    if ( Format ) {

        //
        // Append space to the command before trying to append other
        // optional parameters
        //

        Buffer[ CmdLen++ ] = ' ';

        BytesStored = _vsnprintf( &Buffer[CmdLen], BufferSize-CmdLen, Format,
                                 argptr );

        //
        // Check to see if we overflow the buffer
        //

        if ( BytesStored < 0 ) {
            return ERROR_INSUFFICIENT_BUFFER;
        }
    }

    return ERROR_SUCCESS;
}

DWORD
CoverageSendCommand(
    IN  ULONG  Command,
    OUT PVOID  Buffer OPTIONAL,
    IN OUT SIZE_T *BufferSize OPTIONAL,
    IN  PCSTR Format OPTIONAL,
    ...
    )
/*++

Routine Description:

    This routine formats coverage command and sends to Xbox.  The response
    also copied to buffer if specified.

Arguments:

    Command - Coverage command ID, e.g. CoverageCmdSave

    Buffer - Optional buffer to receive response

    BufferSize - Size in bytes of response buffer

    Format - String format of optional parameters

Return Value:

    Return value from Xbox

--*/
{
    DWORD Error;
    CHAR CmdBuffer[512];
    va_list argptr;
    HRESULT hr;
    SIZE_T BufferLen;

    va_start( argptr, Format );

    Error = CoverageFormatCommand( Command, CmdBuffer, sizeof(CmdBuffer),
                                   Format, argptr );

    va_end( argptr );

    if ( Error == ERROR_SUCCESS ) {

        //
        // If buffer size is less than 8 digit hex + sizeof('\0') + internal
        // area used by xbdm (DM_RESPONSE_MSG_OFFSET), we will redirect it
        //

        if ( Buffer == NULL || BufferSize == NULL ||
             (BufferSize &&
              *BufferSize < (8 + sizeof('\0') + DM_RESPONSE_MSG_OFFSET)) ) {
            Buffer = CmdBuffer;
            BufferLen = sizeof(CmdBuffer);
            BufferSize = &BufferLen;
        }

        hr = DmSendCommand( NULL, CmdBuffer, (PCHAR)Buffer, BufferSize );

        if ( FAILED(hr) ) {
            Error = GET_RESPONSE_ERR( (PSTR)Buffer );
        }
    }

    return Error;
}

