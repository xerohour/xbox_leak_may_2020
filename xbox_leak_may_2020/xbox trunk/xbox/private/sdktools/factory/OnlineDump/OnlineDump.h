#include <ntos.h>  //ntos has to come before xtl.h
#include <xtl.h>
#include <xboxp.h>


CONST DWORD APPEND_FILE = 0;
CONST DWORD NEW_FILE    = 1;


#define BREAK_INTO_DEBUGGER     _asm { int 3 }
// Prototypes


CONST WORD TOP   = 1;
CONST WORD BOTTOM= 0;
  

class COnlineDump: public CXCrypto {

public:

      COnlineDump( VOID ){}
      ~COnlineDump( VOID ){}

      DWORD
      DetermineBitMask(IN DWORD dwBitMask);

      VOID DrawMessage(const PCHAR pcszTextLine1, const PCHAR pcszTextLine2, DWORD dwColor);

      BOOL
      ReadEEPROM ( BYTE* pbData );
      
      BOOL 
      FileToDrive( LPCSTR lpFileName,
                   LPVOID pvFileContents,
                   DWORD dwFileSize,
                   DWORD dwWriteFlags );
   
      VOID DebugOut (PCHAR szMessage,...);
};
