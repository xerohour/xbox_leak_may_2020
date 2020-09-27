#ifndef DECRYPTION_H
#define DECRYPTION_H

#define VIONA_VERSION			1
#undef ONLY_EXTERNAL_VISIBLE

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"

#define ONLY_EXTERNAL_VISIBLE	1

#include "library\hardware\mpeg2dec\generic\mpeg2dec.h"

Error GlobalDoAu (MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE * key, BYTE * resultKey);

#endif
