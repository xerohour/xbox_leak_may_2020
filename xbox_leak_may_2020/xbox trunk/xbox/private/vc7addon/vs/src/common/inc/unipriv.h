// unipriv.h -- UniLib private internal header

#pragma once

#undef UASSERT
#ifdef _DEBUG

#ifdef _X86_
#define __UBREAK__ _asm { int 3 }
#else
#define __UBREAK__ DebugBreak();
#endif // _X86_

#define UASSERT(exp) do { if (!(exp)) __UBREAK__; } while (FALSE)
#else
#define UASSERT(exp) do {} while (0)
#endif