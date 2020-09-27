#include <ntos.h>  //ntos has to come before xtl.h
#include <xtl.h>
#include <xboxp.h>

//typedef long NTSTATUS;
//#include <init.h>
#include <xconfig.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

//#define ASSERT( exp )
#include <cryptkeys.h>
#include "..\\include\\fileformat.h"
#include "..\\include\\DataPacket.h"
#include "..\include\CXboxVideo.h"
#include "CXCrypto.h"

#define BREAK_INTO_DEBUGGER     _asm { int 3 }
// Prototypes


CONST WORD TOP   = 1;
CONST WORD BOTTOM= 0;
  

class CXEEPROMDUMP: public CXBoxVideo, public CXCrypto {


   public:

      CXEEPROMDUMP(IN LPTSTR szFontDirectory): CXBoxVideo(szFontDirectory){}
      
      CXEEPROMDUMP( VOID ){}
      ~CXEEPROMDUMP( VOID ){}

      DWORD
      DetermineBitMask(IN DWORD dwBitMask);
      
      HRESULT
      ReadEEPROM ( IN PDATA_PACKET dpData );
      
      BOOL
      EnumDirectory( IN CHAR szDrive );

      HRESULT
      ReadFileIntoBuffer (IN char* szFileName, 
                          IN BYTE* bFile,
                          IN DWORD dwSizeOfBuffer);

      VOID 
      FileToDrive( LPCSTR lpFileName,
                   LPVOID pvFileContents,
                   DWORD dwFileSize,
                   DWORD dwWriteFlags );
   
      VOID DebugOut (PCHAR szMessage,...);
};
