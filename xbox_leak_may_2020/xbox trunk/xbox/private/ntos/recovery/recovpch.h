#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntddscsi.h>
#include <stdio.h>
#include <xtl.h>
#include "ntos.h"
#include <scsi.h>
#include <stdio.h>
#include <xdbg.h>

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#define CONSTANT_OBJECT_STRING(s)   { sizeof( s ) - sizeof( OCHAR ), sizeof( s ), s }

#ifdef HDSETUPAPP
#define RECOVERYAPP 1
#define RECOVERYNOFLASH 1
#endif // HDSETUPAPP

#ifdef RECOVERYAPP

#define XFONT_TRUETYPE
#include <xfont.h>

HRESULT
FlashKernelImage(
    IN  PVOID  ImageBuffer,
    IN  SIZE_T ImageSize,
    OUT LPSTR  szResp,
    IN  DWORD  cchResp
    );

#endif // RECOVERYAPP

