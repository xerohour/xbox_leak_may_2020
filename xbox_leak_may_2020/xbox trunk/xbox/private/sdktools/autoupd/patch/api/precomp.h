
#define BUILDING_PATCHAPI 1

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#pragma warning( push, 3 )
#include <windows.h>
#include <imagehlp.h>
#pragma warning( pop )

#pragma warning( disable: 4001 )    // single line comments
#pragma warning( disable: 4115 )    // type definition in parentheses
#pragma warning( disable: 4200 )    // zero-sized array in struct/union
#pragma warning( disable: 4201 )    // nameless struct/union
#pragma warning( disable: 4204 )    // non-constant initializer
#pragma warning( disable: 4206 )    // empty file after preprocessing
#pragma warning( disable: 4209 )    // benign redefinition
#pragma warning( disable: 4213 )    // cast on l-value
#pragma warning( disable: 4214 )    // bit field other than int
#pragma warning( disable: 4514 )    // unreferenced inline function

#include <stdlib.h>                 // for debug printf

#include "md5.h"
#include "misc.h"
#include "redblack.h"
#include "patchapi.h"
#include "patchprv.h"
#include "patchlzx.h"
#include "pestuff.h"
#include "psym.h"

typedef void t_encoder_context;
typedef void t_decoder_context;

#include <encapi.h>
#include <decapi.h>


