#include "bldsetup.h"

#include "xplatform.h"
#include <limits.h>
#include "typedef.hpp"
#include "motioncomp_wmv.h"
#include "opcodes.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"

#pragma warning( disable : 4731 ) // turn off ebp in inline assembly warning


#if defined(_WMV_TARGET_X86_) || defined(_Embedded_x86)
#if defined(_WIN32) || defined(_XBOX) || defined(_Embedded_x86)

Void_WMV g_MotionCompZeroMotion_MMX_WMV (
    U8_WMV* ppxliCurrQYMB, 
    U8_WMV* ppxliCurrQUMB, 
    U8_WMV* ppxliCurrQVMB,
    const U8_WMV* ppxliRefYMB, 
    const U8_WMV* ppxliRefUMB, 
    const U8_WMV* ppxliRefVMB,
    I32_WMV iWidthY,
    I32_WMV iWidthUV
)
{
#if defined(_WIN32) || defined(_XBOX)
__asm {
    mov			eax, dword ptr [ppxliCurrQYMB]
	mov			ebx, eax
    add         ebx, 8
    mov			ecx, dword ptr [ppxliRefYMB]
	mov			edx, ecx
    add         edx, 8
    mov         esi, iWidthY
    mov         edi, 16
LoopY:
    movq        mm0, [ecx]
    add         ecx, esi
    movq        [eax], mm0
    add         eax, esi
    movq        mm1, [edx]
    add         edx, esi
    movq        [ebx], mm1
    add         ebx, esi
    dec         edi
    jne         LoopY

    mov			eax, dword ptr [ppxliCurrQUMB]
	mov			ebx, dword ptr [ppxliCurrQVMB]
    mov			ecx, dword ptr [ppxliRefUMB]
	mov			edx, dword ptr [ppxliRefVMB]    
    mov         esi, iWidthUV
    mov         edi, 8
LoopUV:
    movq        mm0, [ecx]
    add         ecx, esi
    movq        [eax], mm0
    add         eax, esi
    movq        mm1, [edx]
    add         edx, esi
    movq        [ebx], mm1
    add         ebx, esi
    dec         edi
    jne         LoopUV
    emms
    }
#else
// if defined(_Embedded_x86)
asm volatile 
    (
    "mov %0, %%eax \n\t"
    "mov %1, %%ecx \n\t"
    "mov %2, %%esi \n\t"
    "mov $1, %%ebx \n\t"
    "mov	%%ecx, %%edx     \n\t"
    "add        $8, %%edx        \n\t"
    "mov        $16, %%edi       \n\t"
"LoopYa:                         \n\t"
    "movq       (%%ecx), %%mm0           \n\t"
    "add        %%esi, %%ecx           \n\t"
    "movq       %%mm0, (%%eax)           \n\t"
    "movq        (%%edx), %%mm1           \n\t"
    "add         %%esi, %%edx           \n\t"
    "movq        %%mm1, (%%eax, %%ebx, 8)           \n\t"
    "add         %%esi, %%eax           \n\t"
    "dec         %%edi           \n\t"
    "jne         LoopYa           \n\t"
    :
    : "m"  (ppxliCurrQYMB),
      "m"  (ppxliRefYMB),       
      "m"   (iWidthY)
    : "%eax", "%ecx", "%esi", "%ebx", "%edx", "%edi"
    );

asm volatile
  (
    "mov %0, %%eax   \n\t"
    "mov %1, %%ebx   \n\t"
    "mov %2, %%ecx   \n\t"
    "mov %3, %%edx   \n\t"
    "mov %4, %%esi   \n\t"
    "mov         $8, %%edi          \n\t"
"LoopUVa:                           \n\t"
    "movq        (%%ecx), %%mm0     \n\t"
    "add         %%esi, %%ecx       \n\t"
    "movq        %%mm0, (%%eax)     \n\t"
    "add         %%esi, %%eax       \n\t"
    "movq        (%%edx), %%mm1     \n\t"
    "add         %%esi, %%edx       \n\t"
    "movq        %%mm1, (%%ebx)     \n\t"
    "add         %%esi, %%ebx       \n\t"
    "dec         %%edi              \n\t"
    "jne         LoopUVa            \n\t"
    "emms                           \n\t"
    :
    :  "m" (ppxliCurrQUMB),
       "m" (ppxliCurrQVMB),
       "m" (ppxliRefUMB),
       "m" (ppxliRefVMB),   
       "m" (iWidthUV)
    : "%eax", "%ecx", "%esi", "%ebx", "%edx", "%edi"
    );
#endif
}

//ppxliErrorQMB is used as a 16bit int
Void_WMV g_MotionCompAndAddError_MMX (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCurrQMB,
    const Buffer __huge* ppxliErrorQMB, const U8_WMV* ppxlcRef,
    I32_WMV iWidthFrm, Bool_WMV bXEven, Bool_WMV bYEven, I32_WMV iMixedPelMV	
)
{
#if defined(_WIN32) || defined(_XBOX)
    if (bYEven) {
        if (bXEven) {  //!bXSubPxl && !bYSubPxl
			__asm {
				mov			eax, dword ptr [ppxlcCurrQMB]
				mov			ebx, dword ptr [ppxlcRef]
				mov			ecx, 8
				mov			edx, dword ptr [ppxliErrorQMB]
				mov			edi, dword ptr [iWidthFrm]
				pxor		mm7, mm7

			mainLoop1:
				movq		mm0, [ebx] // pred
				movq		mm1, [edx] // error
				movq		mm2, mm0
				punpcklbw	mm2, mm7
				paddw		mm1, mm2
				packuswb	mm1, mm7
				movd		[eax], mm1 // curr

				movq		mm1, [edx + 8]
				punpckhbw	mm0, mm7
				paddw		mm1, mm0
				packuswb	mm1, mm7
				movd		[eax + 4], mm1

				add			ebx, edi
				add			eax, edi
				add			edx, 16

				dec			ecx
				jne			mainLoop1
                emms
			}
        }
        else {  //bXSubPxl && !bYSubPxl
			static const __int64	x0101010101010101 = 0x0001000100010001;
			__asm {
				mov		eax, dword ptr [ppxlcCurrQMB]
				mov     ebx, dword ptr [ppxlcRef]
				mov     ecx, 8
				mov     edx, dword ptr [ppxliErrorQMB]
				mov		edi, dword ptr [iWidthFrm]
				movq	mm6, x0101010101010101
				pxor	mm7, mm7

			mainLoop2:
				movq		mm0, [ebx] // pred
				movq		mm1, [ebx + 1] // pred + 1
				movq		mm2, mm0
				movq		mm3, mm1
				punpcklbw	mm2, mm7
				punpcklbw	mm3, mm7
				paddw		mm2, mm3
				paddw		mm2, mm6
				psrlw		mm2, 1

				movq		mm3, [edx] // error
				paddw		mm2, mm3
				packuswb	mm2, mm7
				movd		[eax], mm2 // curr


				punpckhbw	mm0, mm7
				punpckhbw	mm1, mm7
				paddw		mm0, mm1
				paddw		mm0, mm6
				psrlw		mm0, 1

				movq		mm1, [edx + 8] // error
				paddw		mm0, mm1
				packuswb	mm0, mm7
				movd		[eax + 4], mm0 // curr

				add			ebx, edi
				add			eax, edi
				add			edx, 16

				dec			ecx
				jne			mainLoop2
                emms
			}   
        }
    }
    else {
        if (bXEven) {  //!bXSubPxl&& bYSubPxl
			static const __int64	x0001000100010001 = 0x0001000100010001;
			__asm {
				mov		eax, dword ptr [ppxlcCurrQMB]
				mov     ebx, dword ptr [ppxlcRef]
				mov     ecx, 8
				mov     edx, dword ptr [ppxliErrorQMB]
				mov		edi, dword ptr [iWidthFrm]
				movq	mm6, x0001000100010001
				pxor	mm7, mm7

			mainLoop3:
				movq		mm0, [ebx] // pred
				movq		mm1, [ebx + edi] // pred + 1
				movq		mm2, mm0
				movq		mm3, mm1
				punpcklbw	mm2, mm7
				punpcklbw	mm3, mm7
				paddw		mm2, mm3
				paddw		mm2, mm6
				psrlw		mm2, 1

				movq		mm3, [edx] // error
				paddw		mm2, mm3
				packuswb	mm2, mm7
				movd		[eax], mm2 // curr


				punpckhbw	mm0, mm7
				punpckhbw	mm1, mm7
				paddw		mm0, mm1
				paddw		mm0, mm6
				psrlw		mm0, 1

				movq		mm1, [edx + 8] // error
				paddw		mm0, mm1
				packuswb	mm0, mm7
				movd		[eax + 4], mm0 // curr

				add			ebx, edi
				add			eax, edi
				add			edx, 16

				dec			ecx
				jne			mainLoop3
                emms
			}
        }
        else { // bXSubPxl && bYSubPxl
			static const __int64	x0002000200020002 = 0x0002000200020002;
			__asm {
				mov			eax, dword ptr [ppxlcCurrQMB]
				mov			ebx, dword ptr [ppxlcRef]
				mov			ecx, 8
				mov			edx, dword ptr [ppxliErrorQMB]
				mov			edi, dword ptr [iWidthFrm]
				pxor		mm7, mm7

			mainLoop4:
				movq		mm0, [ebx] // pred
				movq		mm1, [ebx + 1] // pred + 1
				movq		mm2, [ebx + edi] // pred
				movq		mm3, [ebx + edi + 1] // pred + 1

				movq		mm4, mm0
				movq		mm5, mm1
				movq		mm6, mm2
				punpcklbw	mm4, mm7
				punpcklbw	mm5, mm7
				punpcklbw	mm6, mm7
				paddw		mm4, mm5
				paddw		mm4, mm6
				movq		mm5, mm3
				punpcklbw	mm5, mm7
				paddw		mm4, mm5
				paddw		mm4, x0002000200020002
				psrlw		mm4, 2

				movq		mm6, [edx] // error
				pxor		mm7, mm7

				paddw		mm4, mm6
				packuswb	mm4, mm7
				movd		[eax], mm4 // curr


				punpckhbw	mm0, mm7
				punpckhbw	mm1, mm7
				punpckhbw	mm2, mm7
				punpckhbw	mm3, mm7
				paddw		mm0, mm1
				paddw		mm0, mm2
				paddw		mm0, mm3
				paddw		mm0, x0002000200020002
				psrlw		mm0, 2

				movq		mm1, [edx + 8] // error
				pxor		mm7, mm7
				paddw		mm0, mm1
				packuswb	mm0, mm7
				movd		[eax + 4], mm0 // curr

				add			ebx, edi
				add			eax, edi
				add			edx, 16

				dec			ecx
				jne			mainLoop4
                emms
			}
        }
    }
#else // _Embedded_x86
    if (bYEven) {
        if (bXEven) {  //!bXSubPxl && !bYSubPxl
			asm volatile
            (
				"mov %0, %%eax    \n\t"
				"mov %1, %%ebx    \n\t"
				"mov %2, %%edx    \n\t"
				"mov %3, %%edi    \n\t"
				"mov			$1, %%esi    \n\t"
				"mov			$8, %%ecx    \n\t"
				"pxor		%%mm7, %%mm7      \n\t"
			"mainLoop1a:                       \n\t"
				"movq		(%%ebx), %%mm0          \n\t" // pred
				"movq		(%%edx), %%mm1          \n\t" // error
				"movq		%%mm0, %%mm2         \n\t"
				"punpcklbw	%%mm7, %%mm2         \n\t"
				"paddw		%%mm2, %%mm1         \n\t"
				"packuswb	%%mm7, %%mm1         \n\t"
				"movd		%%mm1, (%%eax)        \n\t" // curr

//				"movq		(%%edx, %%esi, 8), %%mm1 \n\t"
				"movq		8(%%edx), %%mm1 \n\t"
				"punpckhbw	%%mm7, %%mm0         \n\t"
				"paddw		%%mm0, %%mm1         \n\t"
				"packuswb	%%mm7, %%mm1         \n\t"

				"movd		%%mm1, 4(%%eax)         \n\t"
//				"movd		%%mm1, (%%eax, %%esi, 4)         \n\t"

				"add			%%edi, %%ebx         \n\t"
				"add			%%edi, %%eax         \n\t"
				"add			$16, %%edx         \n\t"

				"dec			%%ecx         \n\t"
				"jne			mainLoop1a         \n\t"
                "emms       \n\t"
                :
				:   "m"  (ppxlcCurrQMB),
				    "m"		(ppxlcRef),
				    "m"   (ppxliErrorQMB),
				    "m"   (iWidthFrm)
                :    "%eax", "%ebx", "%edx", "%edi", "%ecx", "%esi"
			);
        }
        else {  //bXSubPxl && !bYSubPxl
			static const __int64	x0101010101010101 = 0x0001000100010001;
//		    asm volatile ("\n\t movq	%0, %%mm6 \n ": : "m" (x0101010101010101));
			asm volatile
            (
                "mov  %0, %%eax              \n\t"
                "mov  %1, %%ebx              \n\t"
                "mov  %2, %%edx              \n\t"
                "mov  %3, %%edi              \n\t"
                "movq  %4, %%mm6              \n\t"
				"mov    $1, %%esi             \n\t"
				"mov    $8, %%ecx             \n\t"
//				"movq	$0x0001000100010001, %%mm6 \n\t"
				"pxor	%%mm7, %%mm7            \n\t"

			"mainLoop2:            \n\t"
				"movq		(%%ebx), %%mm0             \n\t" // pred
				"movq		1(%%ebx), %%mm1         \n\t" // pred + 1
//				"movq		(%%ebx , %%esi, 1), %%mm1         \n\t" // pred + 1
				"movq		%%mm0, %%mm2            \n\t"
				"movq		%%mm1, %%mm3            \n\t"
				"punpcklbw	%%mm7, %%mm2            \n\t"
				"punpcklbw	%%mm7, %%mm3            \n\t"
				"paddw		%%mm3, %%mm2            \n\t"
				"paddw		%%mm6, %%mm2            \n\t"
				"psrlw		$1, %%mm2            \n\t"

				"movq		(%%edx), %%mm3    \n\t" // error
				"paddw		%%mm3, %%mm2            \n\t"
				"packuswb	%%mm7, %%mm2            \n\t"
				"movd		%%mm2, (%%eax)  \n\t" // curr

				"punpckhbw	%%mm7, %%mm0            \n\t"
				"punpckhbw	%%mm7, %%mm1            \n\t"
				"paddw		%%mm1, %%mm0            \n\t"
				"paddw		%%mm6, %%mm0            \n\t"
				"psrlw		$1, %%mm0            \n\t"

//				"movq		(%%edx, %%esi, 8),  %%mm1            \n\t" // error
				"movq		8(%%edx),  %%mm1            \n\t" // error
				"paddw		%%mm1, %%mm0            \n\t"
				"packuswb	%%mm7, %%mm0            \n\t"
//				"movd		%%mm0, (%%eax, %%esi, 4)  \n\t" // curr
				"movd		%%mm0, 4(%%eax)  \n\t" // curr

				"add			%%edi, %%ebx             \n\t"
				"add			%%edi, %%eax           \n\t"
				"add			$16, %%edx            \n\t"

				"dec			%%ecx            \n\t"
				"jne			mainLoop2       \n\t"
                "emms       \n\t"
				:
                :    "m" (ppxlcCurrQMB),
				     "m" (ppxlcRef),
				     "m" (ppxliErrorQMB),
					 "m" (iWidthFrm),
			         "m" (x0101010101010101)
                :    "%eax", "%ebx", "%edx", "%edi", "%ecx", "%esi"
			);
        }
    }
    else {
        if (bXEven) {  //!bXSubPxl&& bYSubPxl
			static const __int64	x0001000100010001 = 0x0001000100010001;
//		    asm volatile ("\n\t movq	%0, %%mm6 \n ": : "m" (x0001000100010001));
			asm volatile
            (
				"mov    $1, %%esi            \n\t"
				"mov    $8, %%ecx            \n\t"
                "movq	%4, %%mm6            \n\t"
				"pxor	%%mm7, %%mm7   \n\t"

			"mainLoop3:                            \n\t"
				"movq		(%%ebx), %%mm0          \n\t" // pred
				"movq		(%%ebx, %%edi,1), %%mm1          \n\t" // pred + 1
				"movq		%%mm0, %%mm2          \n\t"
				"movq		%%mm1, %%mm3          \n\t"
				"punpcklbw	%%mm7, %%mm2          \n\t"
				"punpcklbw	%%mm7, %%mm3          \n\t"
				"paddw		%%mm3, %%mm2          \n\t"
				"paddw		%%mm6, %%mm2          \n\t"
				"psrlw		$1, %%mm2          \n\t"

				"movq		(%%edx), %%mm3          \n\t" // error
				"paddw		%%mm3, %%mm2          \n\t"
				"packuswb	%%mm7, %%mm2          \n\t"
				"movd		%%mm2, (%%eax)          \n\t" // curr


				"punpckhbw	%%mm7, %%mm0          \n\t"
				"punpckhbw	%%mm7, %%mm1          \n\t"
				"paddw		%%mm1, %%mm0          \n\t"
				"paddw		%%mm6, %%mm0          \n\t"
				"psrlw		$1, %%mm0          \n\t"

				"movq		8(%%edx), %%mm1          \n\t" // error
//				"movq		(%%edx, %%esi, 8), %%mm1          \n\t" // error
				"paddw		%%mm1, %%mm0          \n\t"
				"packuswb	%%mm7, %%mm0          \n\t"
//				"movd		%%mm0, (%%eax, %%esi, 4)           \n\t"// curr
				"movd		%%mm0, 4(%%eax)           \n\t"// curr

				"add		%%edi, %%ebx          \n\t"
				"add		%%edi, %%eax          \n\t"
				"add		$16, %%edx          \n\t"

				"dec			%%ecx          \n\t"
				"jne			mainLoop3          \n\t"
                "emms       \n\t"
                :
				:   "a" (ppxlcCurrQMB),
				    "b"   (ppxlcRef),
				    "d" (ppxliErrorQMB),
				    "D"	(iWidthFrm),
                    "m"	(x0001000100010001)
                :    "%eax", "%ebx", "%edx", "%edi", "%ecx", "%esi"
			);
        }
        else { // bXSubPxl && bYSubPxl
			static const __int64	x0002000200020002 = 0x0002000200020002;
			asm volatile
            (
                "mov        %0, %%eax \n\t"
                "mov        %1, %%ebx \n\t"
                "mov        %2, %%edx \n\t"
                "mov        %3, %%edi \n\t"
				"mov		$1, %%esi              \n\t"
				"mov		$8, %%ecx              \n\t"
				"pxor		%%mm7, %%mm7              \n\t"

			"mainLoop4:              \n\t"
				"movq		(%%ebx), %%mm0               \n\t" // pred
				"movq		1(%%ebx), %%mm1              \n\t" // pred + 1
				"movq		(%%ebx, %%edi,1), %%mm2              \n\t" // pred
				"movq		1(%%ebx, %%edi, 1), %%mm3              \n\t" // pred + 1
				"movq		%%mm0, %%mm4              \n\t"
				"movq		%%mm1, %%mm5              \n\t"
				"movq		%%mm2, %%mm6              \n\t"
				"punpcklbw	%%mm7, %%mm4              \n\t"
				"punpcklbw	%%mm7, %%mm5              \n\t"
				"punpcklbw	%%mm7, %%mm6              \n\t"
				"paddw		%%mm5, %%mm4              \n\t"
				"paddw		%%mm6, %%mm4              \n\t"
				"movq		%%mm3, %%mm5              \n\t"
				"punpcklbw	%%mm7, %%mm5              \n\t"
				"paddw		%%mm5, %%mm4              \n\t"
				"paddw		%4, %%mm4              \n\t"
//				"paddw		x0002000200020002, %%mm4              \n\t"
				"psrlw		$2, %%mm4              \n\t"

				"movq		(%%edx), %%mm6              \n\t" // error
				"pxor		%%mm7, %%mm7              \n\t"

				"paddw		%%mm6, %%mm4              \n\t"
				"packuswb	%%mm7, %%mm4              \n\t"
				"movd		%%mm4, (%%eax)               \n\t"// curr


				"punpckhbw	%%mm7, %%mm0              \n\t"
				"punpckhbw	%%mm7, %%mm1              \n\t"
				"punpckhbw	%%mm7, %%mm2              \n\t"
				"punpckhbw	%%mm7, %%mm3              \n\t"
				"paddw		%%mm1, %%mm0              \n\t"
				"paddw		%%mm2, %%mm0              \n\t"
				"paddw		%%mm3, %%mm0              \n\t"
				"paddw		%4, %%mm0              \n\t"
//				"paddw		x0002000200020002, %%mm0              \n\t"
				"psrlw		$2, %%mm0              \n\t"

				"movq		8(%%edx), %%mm1              \n\t" // error
				"pxor		%%mm7, %%mm7              \n\t"
				"paddw		%%mm1, %%mm0              \n\t"
				"packuswb	%%mm7, %%mm0              \n\t"
				"movd		%%mm0, 4(%%eax)               \n\t"// curr

				"add		%%edi, %%ebx              \n\t"
				"add		%%edi, %%eax              \n\t"
				"add		$16, %%edx              \n\t"

				"dec			%%ecx              \n\t"
				"jne			mainLoop4       \n\t"
                "emms       \n\t"
				:
                :   "m"	(ppxlcCurrQMB),
				    "m"	(ppxlcRef),
				    "m" (ppxliErrorQMB),
				    "m" (iWidthFrm),
                    "m" (x0002000200020002)
//                :    "%eax", "%ebx", "%edx", "%edi"
                :    "%eax", "%ebx", "%edx", "%edi", "%ecx", "%esi"
			);
        }
    }
#endif

}

Void_WMV g_MotionComp_MMX(
    tWMVDecInternalMember *pWMVDec,
	U8_WMV*             ppxlcPredMB,
	const U8_WMV*       ppxlcRefMB,
    I32_WMV             iWidthFrm,
    Bool_WMV            bInterpolateX,
    Bool_WMV            bInterpolateY,
    I32_WMV             iMixedPelMV	
)
{


  	static const Int64 x01 = (Int64)0x0101010101010101;
    static const Int64 x7f = (Int64)0x7f7f7f7f7f7f7f7f;
    static const Int64 x3F = (Int64)0x3F3F3F3F3F3F3F3F;
    static const Int64 x03 = (Int64)0x0303030303030303;
    static const Int64 x02 = (Int64)0x0202020202020202;

#if defined(_WIN32) || defined(_XBOX)
     if (bInterpolateY) {
        if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
            _asm {
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB
            mov         eax,iWidthFrm
            mov         ebx,iWidthFrm
            mov         ecx,8
            cmp         ecx,16
            jz          CopyLoop2
CopyLoop1:
            movq        mm0,[esi]
            add         esi,eax
            movq        [edi],mm0
            add         edi,ebx
            dec         ecx
            jnz         CopyLoop1
            jmp         End
CopyLoop2:
            movq        mm0,[esi]
            movq        [edi],mm0
            movq        mm0,[esi+8]
            add         esi,eax
            movq        [edi+8],mm0
            add         edi,ebx
            dec         ecx
            jnz         CopyLoop2
            }
        }
        else {  //bXSubPxl && !bYSubPxl
            _asm {
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB
            mov         edx,1
            mov         eax,iWidthFrm
            mov         ebx,iWidthFrm
            movq        mm7,x7f
            movq        mm6,x01
            mov         ecx,8
            cmp         ecx,16
            jz          HorizantalLoop2
HorizantalLoop1:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]

            //avg mm0,mm1
            movq        mm5,mm0
            por         mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi],mm0
            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         HorizantalLoop1
            jmp         End

HorizantalLoop2:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]

            //avg mm0,mm1
            movq        mm5,mm0
            por         mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi],mm0

            movq        mm0,[esi+8]
            movq        mm1,[esi+edx+8]

            //avg mm0,mm1
            movq        mm5,mm0
            por         mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi+8],mm0

            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         HorizantalLoop2
	        }
        }
    }
    else {
        if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
            _asm {
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB
            mov         eax,iWidthFrm
            mov         edx,1
            imul        edx,eax
            mov         eax,iWidthFrm
            mov         ebx,iWidthFrm
            movq        mm7,x7f
            movq        mm6,x01
            mov         ecx,8
            cmp         ecx,16
            jz          VerticalLoop2
VerticalLoop1:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]

            //avg mm0,mm1
            movq        mm5,mm0
            por         mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi],mm0
            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         VerticalLoop1
            jmp         End

VerticalLoop2:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]

            //avg mm0,mm1
            movq        mm5,mm0
            por         mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi],mm0

            movq        mm0,[esi+8]
            movq        mm1,[esi+edx+8]

            //avg mm0,mm1
            movq        mm5,mm0
            por         mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi+8],mm0

            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         VerticalLoop2
	        }
        }
        else { // bXSubPxl && bYSubPxl
            _asm {
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB

            mov         eax,1
            cmp         eax,-1
            jnz         PositiveXMV
            dec         esi
PositiveXMV:
            movq        mm7,x3F
            movq        mm6,x03
            mov         eax,iWidthFrm
            mov         edx,1
            imul        edx,eax
            mov         ecx,iWidthFrm
            mov         ebx,iWidthFrm
//            mov         [esp-4],ebp
            push        ebp; // for xbox->fix esp crash because the stack can't be accessed
            mov         ebp,8
            cmp         ebp,16
            jz          BilinearLoop2
BilinearLoop1:
            movq        mm0,[esi]
            movq        mm4,mm0
            pand        mm4,mm6
            movq        mm1,[esi+1]
            movq        mm5,mm1
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm2,[esi+edx]
            movq        mm5,mm2
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm3,[esi+edx+1]
            movq        mm5,mm3
            pand        mm5,mm6
            paddw       mm4,mm5
            paddw       mm4,x02
            psrlw       mm4,2
            pand        mm4,mm6
            psrlw       mm0,2
            pand        mm0,mm7
            psrlw       mm1,2
            pand        mm1,mm7
            paddw       mm0,mm1
            psrlw       mm2,2
            pand        mm2,mm7
            paddw       mm0,mm2
            psrlw       mm3,2
            pand        mm3,mm7
            paddw       mm0,mm3
            paddw       mm0,mm4
            movq        [edi],mm0

            add         esi,ecx
            add         edi,ebx
            dec         ebp
            jnz         BilinearLoop1
//            mov         ebp,[esp-4]
            pop         ebp 
            jmp         End

BilinearLoop2:
            movq        mm0,[esi]
            movq        mm4,mm0
            pand        mm4,mm6
            movq        mm1,[esi+1]
            movq        mm5,mm1
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm2,[esi+edx]
            movq        mm5,mm2
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm3,[esi+edx+1]
            movq        mm5,mm3
            pand        mm5,mm6
            paddw       mm4,mm5
            paddw       mm4,x02
            psrlw       mm4,2
            pand        mm4,mm6
            psrlw       mm0,2
            pand        mm0,mm7
            psrlw       mm1,2
            pand        mm1,mm7
            paddw       mm0,mm1
            psrlw       mm2,2
            pand        mm2,mm7
            paddw       mm0,mm2
            psrlw       mm3,2
            pand        mm3,mm7
            paddw       mm0,mm3
            paddw       mm0,mm4
            movq        [edi],mm0

            movq        mm0,[esi+8]
            movq        mm4,mm0
            pand        mm4,mm6
            movq        mm1,[esi+9]
            movq        mm5,mm1
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm2,[esi+edx+8]
            movq        mm5,mm2
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm3,[esi+edx+9]
            movq        mm5,mm3
            pand        mm5,mm6
            paddw       mm4,mm5
            paddw       mm4,x02
            psrlw       mm4,2
            pand        mm4,mm6
            psrlw       mm0,2
            pand        mm0,mm7
            psrlw       mm1,2
            pand        mm1,mm7
            paddw       mm0,mm1
            psrlw       mm2,2
            pand        mm2,mm7
            paddw       mm0,mm2
            psrlw       mm3,2
            pand        mm3,mm7
            paddw       mm0,mm3
            paddw       mm0,mm4
            movq        [edi+8],mm0

            add         esi,ecx
            add         edi,ebx
            dec         ebp
            jnz         BilinearLoop2
//            mov         ebp,[esp-4]
            pop         ebp; // for x-box
            }
        }
    }
End: _asm emms;
#else // _Embedded_x86
     if (bInterpolateY) {
        if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
            asm volatile
            (
            "cmp         $16, %%ecx \n\t"
            "jz          CopyLoop2a  \n\t"

"CopyLoop1a:             \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "add         %%eax, %%esi     \n\t"
            "movq        %%mm0, (%%edi)     \n\t"
            "add         %%ebx,%%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         CopyLoop1a     \n\t"
            "jmp         Enda     \n\t"

"CopyLoop2a:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        %%mm0, (%%edi)     \n\t"
            "movq        8(%%esi), %%mm0     \n\t" 
            "add         %%eax,%%esi     \n\t"
            "movq        %%mm0, 8(%%edi)     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         CopyLoop2a     \n\t"
             :
             : "S"  (ppxlcRefMB),
               "D"  (ppxlcPredMB),
               "a"  (iWidthFrm),
               "b" (iWidthFrm),
               "c" (8)
             : "%esi", "%edi", "%eax", "%ebx", "%ecx", "%edx", "%ebp"
            );
        }     
        else {  //bXSubPxl && !bYSubPxl     
            asm volatile
            (
             "mov %0, %%esi         \n\t"
             "mov %1, %%edi         \n\t"
             "mov %2, %%edx         \n\t"
             "mov %3, %%eax         \n\t"
             "mov %4, %%ebx         \n\t"
             "mov %5, %%ecx         \n\t"
            "movq        %6, %%mm7     \n\t"
//            "movq        x7f, %%mm7     \n\t"
            "movq        %7, %%mm6     \n\t"
//            "movq        x01, %%mm6     \n\t"
            "cmp         $16, %%ecx     \n\t"
            "jz          HorizantalLoop2a     \n\t"
" HorizantalLoop1a:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm1     \n\t"
     
            //avg %%mm0,%%mm1     \n\t"
            "movq        %%mm0,%%mm5     \n\t"
            "por         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"
    
            "movq        %%mm0, (%%edi)     \n\t"
            "add         %%eax, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         HorizantalLoop1a     \n\t"
            "jmp         Enda     \n\t"

"HorizantalLoop2a:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm1     \n\t"
     
            //avg %%mm0,%%mm1     \n\t"
            "movq        %%mm0,%%mm5     \n\t"
            "por         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"
     
            "movq        %%mm0, (%%edi)     \n\t"
     
            "movq        8(%%esi), %%mm0     \n\t"
            "movq        8(%%esi, %%edx, 1), %%mm1     \n\t"

            //avg %%mm0,%%mm1      // Here , IW
            "movq        %%mm0,%%mm5     \n\t"
            "por         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"
 
            "movq        %%mm0, 8(%%edi)     \n\t"

            "add         %%eax, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         HorizantalLoop2a     \n\t"
            :
            :   "m"  (ppxlcRefMB),
                "m"  (ppxlcPredMB),
                "m"  (1),
                "m"  (iWidthFrm),
                "m"   (iWidthFrm),
                "m"  (8),
                "m"  (x7f),
                "m"  (x01)
            :  "%esi", "%edi", "%edx", "%eax", "%ebx", "%ecx", "%ebp"
            );
        }    
    }    
    else {
        if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
            asm volatile 
            ( 
            "mov %0, %%esi  \n\t"
            "mov %1, %%edi  \n\t"
            "mov %2, %%edx  \n\t"
            "mov %3, %%eax  \n\t"
            "mov %4, %%ebx  \n\t"
            "mov %5, %%ecx  \n\t"
//            "mov         $1, %%ebp     \n\t"
            "imul        %%eax, %%edx     \n\t"
            "mov         %3, %%eax     \n\t"
            "movq        %6, %%mm7     \n\t"
//            "movq        x7f, %%mm7     \n\t"
            "movq        %7, %%mm6     \n\t"
//            "movq        x01, %%mm6     \n\t"
            "cmp         $16, %%ecx     \n\t"
            "jz          VerticalLoop2a     \n\t"
"VerticalLoop1a:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm1     \n\t"  // esi + edi

            //avg %%mm0,%%mm1
            "movq        %%mm0,%%mm5     \n\t"
            "por         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"

            "movq        %%mm0, (%%edi)     \n\t"
            "add         %%eax, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         VerticalLoop1a     \n\t"
            "jmp         Enda     \n\t"
     
"VerticalLoop2a:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm1     \n\t"  // esi + edi

            //avg %%mm0,%%mm1     
            "movq        %%mm0,%%mm5     \n\t"
            "por         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"

            "movq        %%mm0, (%%edi)     \n\t"

            "movq        8(%%esi), %%mm0     \n\t"
            "movq        8(%%esi, %%edx, 1), %%mm1     \n\t" // esi+edx + 8

            //avg %%mm0,%%mm1     
            "movq        %%mm0,%%mm5     \n\t"
            "por         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"

            "movq        %%mm0, 8(%%edi)     \n\t"

            "add         %%eax, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         VerticalLoop2a     \n\t"
            :
            :   "m"  (ppxlcRefMB),
                "m"  (ppxlcPredMB),     
                "m"   (1),
                "m"   (iWidthFrm),  
                "m"   (iWidthFrm),
                "m"   (8),
                "m"   (x7f),
                "m"   (x01)
            :   "%esi", "%edi", "%edx", "%eax", "%ebx", "%ecx", "%ebp"
	    );     
        }     
        else { // bXSubPxl && bYSubPxl

//            asm volatile ("mov  %0, %%esi : :  "m" (ppxlcRefMB));
            asm volatile 
            (
            "mov  %0, %%esi  \n\t"
            "mov  %1, %%edi  \n\t"
            "mov  %2, %%eax  \n\t"
            "cmp         $-1, %%eax     \n\t"
            "jnz         PositiveXMV     \n\t"
            "dec         %%esi     \n\t"
"PositiveXMV:     \n\t"
            "movq        %3, %%mm7     \n\t"
            "movq        %4, %%mm6     \n\t"
            "mov         %5, %%eax     \n\t"
            "mov         %7, %%edx     \n\t"
//            "movq        x3F, %%mm7     \n\t"
//            "movq        x03, %%mm6     \n\t"
//            "mov         iWidthFrm, %%eax     \n\t"
//            "mov         1, %%edx     \n\t"
            "imul        %%eax, %%edx     \n\t"
            "mov         %5, %%ecx \n\t"
            "mov         %6, %%ebx  \n\t"
//            "mov         iWidthFrm, %%ecx \n\t"
//            "mov         iWidthFrm, %%ebx  \n\t"
//            mov         [esp-4],ebp     \n\t"
            "push        %%ebp  \n\t" // for xbox->fix esp crash because the stack can't be accessed     \n\t"
            "mov         %8, %%ebp  \n\t"
//            "mov         8, %%ebp  \n\t"
            "cmp         $16, %%ebp     \n\t"
            "jz          BilinearLoop2a     \n\t"
"BilinearLoop1a:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        %%mm0,%%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t" // here
            "movq        1(%%esi), %%mm1     \n\t"
            "movq        %%mm1,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm2     \n\t"
            "movq        %%mm2,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "movq        1(%%esi, %%edx,1), %%mm3     \n\t"
            "movq        %%mm3,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "paddw       %9, %%mm4 \n\t"
//            "paddw       x02, %%mm4 \n\t"
            "psrlw       $2, %%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t"
            "psrlw       $2, %%mm0     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "psrlw       $2, %%mm1     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"
            "psrlw       $2, %%mm2     \n\t"
            "pand        %%mm7,%%mm2     \n\t"
            "paddw       %%mm2,%%mm0     \n\t"
            "psrlw       $2, %%mm3     \n\t"
            "pand        %%mm7,%%mm3     \n\t"
            "paddw       %%mm3,%%mm0     \n\t"
            "paddw       %%mm4,%%mm0     \n\t"
            "movq        %%mm0, (%%edi)     \n\t"

            "add         %%ecx, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec          %%ebp     \n\t"
            "jnz         BilinearLoop1a     \n\t"
//            mov         ebp,[esp-4]     \n\t"
            "pop         %%ebp      \n\t"
            "jmp         Enda     \n\t"

"BilinearLoop2a:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        %%mm0,%%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t" // here
            "movq        1(%%esi), %%mm1     \n\t"
            "movq        %%mm1,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm2     \n\t"
            "movq        %%mm2,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "movq        1(%%esi, %%edx, 1), %%mm3     \n\t"
            "movq        %%mm3,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "paddw       %9, %%mm4 \n\t"
//            "paddw       x02, %%mm4 \n\t"
            "psrlw       $2, %%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t"
            "psrlw       $2, %%mm0     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "psrlw       $2, %%mm1     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"
            "psrlw       $2, %%mm2     \n\t"
            "pand        %%mm7,%%mm2     \n\t"
            "paddw       %%mm2,%%mm0     \n\t"
            "psrlw       $2, %%mm3     \n\t"
            "pand        %%mm7,%%mm3     \n\t"
            "paddw       %%mm3,%%mm0     \n\t"
            "paddw       %%mm4,%%mm0     \n\t"
            "movq        %%mm0, (%%edi)     \n\t"

            "movq        8(%%esi), %%mm0     \n\t"
            "movq        %%mm0,%%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t" // here
            "movq        9(%%esi), %%mm1     \n\t" // esi + 9
            "movq        %%mm1,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "movq        8(%%esi, %%edx, 1), %%mm2     \n\t" // esi+ edx +8
            "movq        %%mm2,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "movq        9(%%esi, %%edx, 1), %%mm3     \n\t" //esi+edx+9
            "movq        %%mm3,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "paddw       %9, %%mm4 \n\t"
//            "paddw       x02, %%mm4 \n\t"
            "psrlw       $2, %%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t"
            "psrlw       $2, %%mm0     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "psrlw       $2, %%mm1     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"
            "psrlw       $2, %%mm2     \n\t"
            "pand        %%mm7,%%mm2     \n\t"
            "paddw       %%mm2,%%mm0     \n\t"
            "psrlw       $2, %%mm3     \n\t"
            "pand        %%mm7,%%mm3     \n\t"
            "paddw       %%mm3,%%mm0     \n\t"
            "paddw       %%mm4,%%mm0     \n\t"
            "movq        %%mm0, 8(%%edi)     \n\t"

            "add         %%ecx, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ebp     \n\t"
            "jnz         BilinearLoop2a     \n\t"
//            mov         ebp,[esp-4]     \n\t"
            "pop         %%ebp \n\t" // for x-box
            :
            :   "m"  (ppxlcRefMB),
                "m"  (ppxlcPredMB),     
                "m"   (1),
                "m"  (x3F),
                "m"  (x03),
                "m"  (iWidthFrm),
                "m"  (iWidthFrm),
                "m"  (1),
                "m"  (8),
                "m"  (x02)
            :   "%esi", "%edi", "%edx", "%eax", "%ebx", "%ecx", "%ebp"
            );
        }
    }
    asm volatile
    (
"Enda: \n\t"
        "emms \n\t"
    );
#endif
}

//ppxliErrorQMB is used as a 16bit int
Void_WMV g_MotionCompAndAddErrorRndCtrl_MMX (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCurrQMB,
    const Buffer __huge* ppxliErrorQMB, const U8_WMV* ppxlcRef,
    I32_WMV iWidthFrm, Bool_WMV bXEven, Bool_WMV bYEven, I32_WMV iMixedPelMV	
)
{
#if defined(_WIN32) || defined(_XBOX)
    if (bYEven) {
        if (bXEven) {  //!bXSubPxl && !bYSubPxl
			__asm {
				mov			eax, dword ptr [ppxlcCurrQMB]
				mov			ebx, dword ptr [ppxlcRef]
				mov			ecx, 8
				mov			edx, dword ptr [ppxliErrorQMB]
				mov			edi, dword ptr [iWidthFrm]
				pxor		mm7, mm7

			mainLoop1:
				movq		mm0, [ebx] // pred
				movq		mm1, [edx] // error
				movq		mm2, mm0
				punpcklbw	mm2, mm7
				paddw		mm1, mm2
				packuswb	mm1, mm7
				movd		[eax], mm1 // curr

				movq		mm1, [edx + 8]
				punpckhbw	mm0, mm7
				paddw		mm1, mm0
				packuswb	mm1, mm7
				movd		[eax + 4], mm1

				add			ebx, edi
				add			eax, edi
				add			edx, 16

				dec			ecx
				jne			mainLoop1
                emms;

			}
        }
        else {  //bXSubPxl && !bYSubPxl
			//static const __int64	x0101010101010101 = 0x0001000100010001;
			__asm {
				mov		eax, dword ptr [ppxlcCurrQMB]
				mov     ebx, dword ptr [ppxlcRef]
				mov     ecx, 8
				mov     edx, dword ptr [ppxliErrorQMB]
				mov		edi, dword ptr [iWidthFrm]
				//movq	mm6, x0101010101010101
				pxor	mm7, mm7

			mainLoop2:
				movq		mm0, [ebx] // pred
				movq		mm1, [ebx + 1] // pred + 1
				movq		mm2, mm0
				movq		mm3, mm1
				punpcklbw	mm2, mm7
				punpcklbw	mm3, mm7
				paddw		mm2, mm3
				//paddw		mm2, mm6
				psrlw		mm2, 1

				movq		mm3, [edx] // error
				paddw		mm2, mm3
				packuswb	mm2, mm7
				movd		[eax], mm2 // curr


				punpckhbw	mm0, mm7
				punpckhbw	mm1, mm7
				paddw		mm0, mm1
				//paddw		mm0, mm6
				psrlw		mm0, 1

				movq		mm1, [edx + 8] // error
				paddw		mm0, mm1
				packuswb	mm0, mm7
				movd		[eax + 4], mm0 // curr

				add			ebx, edi
				add			eax, edi
				add			edx, 16

				dec			ecx
				jne			mainLoop2
                emms;

			}
        }
    }
    else {
        if (bXEven) {  //!bXSubPxl&& bYSubPxl
			//static const __int64	x0001000100010001 = 0x0001000100010001;
			__asm {
				mov		eax, dword ptr [ppxlcCurrQMB]
				mov     ebx, dword ptr [ppxlcRef]
				mov     ecx, 8
				mov     edx, dword ptr [ppxliErrorQMB]
				mov		edi, dword ptr [iWidthFrm]
				//movq	mm6, x0001000100010001
				pxor	mm7, mm7

			mainLoop3:
				movq		mm0, [ebx] // pred
				movq		mm1, [ebx + edi] // pred + 1
				movq		mm2, mm0
				movq		mm3, mm1
				punpcklbw	mm2, mm7
				punpcklbw	mm3, mm7
				paddw		mm2, mm3
				//paddw		mm2, mm6
				psrlw		mm2, 1

				movq		mm3, [edx] // error
				paddw		mm2, mm3
				packuswb	mm2, mm7
				movd		[eax], mm2 // curr


				punpckhbw	mm0, mm7
				punpckhbw	mm1, mm7
				paddw		mm0, mm1
				//paddw		mm0, mm6
				psrlw		mm0, 1

				movq		mm1, [edx + 8] // error
				paddw		mm0, mm1
				packuswb	mm0, mm7
				movd		[eax + 4], mm0 // curr

				add			ebx, edi
				add			eax, edi
				add			edx, 16

				dec			ecx
				jne			mainLoop3
                emms;

			}
        }
        else { // bXSubPxl && bYSubPxl
			static const __int64	x0001000100010001 = 0x0001000100010001;
			__asm {
				mov			eax, dword ptr [ppxlcCurrQMB]
				mov			ebx, dword ptr [ppxlcRef]
				mov			ecx, 8
				mov			edx, dword ptr [ppxliErrorQMB]
				mov			edi, dword ptr [iWidthFrm]
				pxor		mm7, mm7

			mainLoop4:
				movq		mm0, [ebx] // pred
				movq		mm1, [ebx + 1] // pred + 1
				movq		mm2, [ebx + edi] // pred
				movq		mm3, [ebx + edi + 1] // pred + 1

				movq		mm4, mm0
				movq		mm5, mm1
				movq		mm6, mm2
				punpcklbw	mm4, mm7
				punpcklbw	mm5, mm7
				punpcklbw	mm6, mm7
				paddw		mm4, mm5
				paddw		mm4, mm6
				movq		mm5, mm3
				punpcklbw	mm5, mm7
				paddw		mm4, mm5
				paddw		mm4, x0001000100010001
				psrlw		mm4, 2

				movq		mm6, [edx] // error
				pxor		mm7, mm7

				paddw		mm4, mm6
				packuswb	mm4, mm7
				movd		[eax], mm4 // curr


				punpckhbw	mm0, mm7
				punpckhbw	mm1, mm7
				punpckhbw	mm2, mm7
				punpckhbw	mm3, mm7
				paddw		mm0, mm1
				paddw		mm0, mm2
				paddw		mm0, mm3
				paddw		mm0, x0001000100010001
				psrlw		mm0, 2

				movq		mm1, [edx + 8] // error
				pxor		mm7, mm7
				paddw		mm0, mm1
				packuswb	mm0, mm7
				movd		[eax + 4], mm0 // curr

				add			ebx, edi
				add			eax, edi
				add			edx, 16

				dec			ecx
				jne			mainLoop4
                emms;
			}
        }
    }
#else // _Embedded_x86
    if (bYEven) {
        if (bXEven) {  //!bXSubPxl && !bYSubPxl
            asm volatile
            (
                "mov %0, %%eax    \n\t"
                "mov %1, %%ebx    \n\t"
                "mov %2, %%edx    \n\t"
                "mov %3, %%edi    \n\t"
                "mov            $8, %%ecx    \n\t"
                "pxor       %%mm7, %%mm7      \n\t"
            "mainLoop1:                       \n\t"
                "movq       (%%ebx), %%mm0          \n\t" // pred
                "movq       (%%edx), %%mm1          \n\t" // error
                "movq       %%mm0, %%mm2         \n\t"
                "punpcklbw  %%mm7, %%mm2         \n\t"
                "paddw      %%mm2, %%mm1         \n\t"
                "packuswb   %%mm7, %%mm1         \n\t"
                "movd       %%mm1, (%%eax)        \n\t" // curr

                "movq       8(%%edx), %%mm1 \n\t"
                "punpckhbw  %%mm7, %%mm0         \n\t"
                "paddw      %%mm0, %%mm1         \n\t"
                "packuswb   %%mm7, %%mm1         \n\t"

                "movd       %%mm1, 4(%%eax)         \n\t"

                "add            %%edi, %%ebx         \n\t"
                "add            %%edi, %%eax         \n\t"
                "add            $16, %%edx         \n\t"

                "dec            %%ecx         \n\t"
                "jne            mainLoop1         \n\t"
                "emms   \n\t"
                :
                :   "m"  (ppxlcCurrQMB),
                    "m"     (ppxlcRef),
                    "m"   (ppxliErrorQMB),
                    "m"   (iWidthFrm)
                :    "%eax", "%ebx", "%edx", "%edi", "%ecx", "%esi", "%ebp"
            );
        }
        else {  //bXSubPxl && !bYSubPxl
            static const __int64    x0101010101010101 = 0x0001000100010001;
//          asm volatile ("\n\t movq    %0, %%mm6 \n ": : "m" (x0101010101010101));
            asm volatile
            (
                "mov  %0, %%eax              \n\t"
                "mov  %1, %%ebx              \n\t"
                "mov  %2, %%edx              \n\t"
                "mov  %3, %%edi              \n\t"
//                "movq  %4, %%mm6              \n\t"
                "mov    $8, %%ecx             \n\t"
//              "movq   $0x0001000100010001, %%mm6 \n\t"
                "pxor   %%mm7, %%mm7            \n\t"

           "mainLoop2a:            \n\t"
                "movq       (%%ebx), %%mm0             \n\t" // pred
                "movq       1(%%ebx), %%mm1         \n\t" // pred + 1
                "movq       %%mm0, %%mm2            \n\t"
                "movq       %%mm1, %%mm3            \n\t"
                "punpcklbw  %%mm7, %%mm2            \n\t"
                "punpcklbw  %%mm7, %%mm3            \n\t"
                "paddw      %%mm3, %%mm2            \n\t"
//                "paddw      %%mm6, %%mm2            \n\t"
                "psrlw      $1, %%mm2            \n\t"

                "movq       (%%edx), %%mm3    \n\t" // error
                "paddw      %%mm3, %%mm2            \n\t"
                "packuswb   %%mm7, %%mm2            \n\t"
                "movd       %%mm2, (%%eax)  \n\t" // curr

                "punpckhbw  %%mm7, %%mm0            \n\t"
                "punpckhbw  %%mm7, %%mm1            \n\t"
                "paddw      %%mm1, %%mm0            \n\t"
//                "paddw      %%mm6, %%mm0            \n\t"
                "psrlw      $1, %%mm0            \n\t"

                "movq       8(%%edx),  %%mm1            \n\t" // error
                "paddw      %%mm1, %%mm0            \n\t"
                "packuswb   %%mm7, %%mm0            \n\t"
                "movd       %%mm0, 4(%%eax)  \n\t" // curr

                "add            %%edi, %%ebx             \n\t"
                "add            %%edi, %%eax           \n\t"
                "add            $16, %%edx            \n\t"

                "dec            %%ecx            \n\t"
                "jne            mainLoop2a       \n\t"
                "emms   \n\t"
                :
                :    "m" (ppxlcCurrQMB),
                     "m" (ppxlcRef),
                     "m" (ppxliErrorQMB),
                     "m" (iWidthFrm),
                     "m" (x0101010101010101)
                :    "%eax", "%ebx", "%edx", "%edi", "%ecx", "%esi", "%ebp"
            );
        }
    }
    else {
        if (bXEven) {  //!bXSubPxl&& bYSubPxl
            static const __int64    x0001000100010001 = 0x0001000100010001;
//          asm volatile ("\n\t movq    %0, %%mm6 \n ": : "m" (x0001000100010001));
            asm volatile
            (
                "mov    $8, %%ecx            \n\t"
                "movq   %4, %%mm6            \n\t"
                "pxor   %%mm7, %%mm7   \n\t"

            "mainLoop3a:                            \n\t"
                "movq       (%%ebx), %%mm0          \n\t" // pred
                "movq       (%%ebx, %%edi,1), %%mm1          \n\t" // pred + 1
                "movq       %%mm0, %%mm2          \n\t"
                "movq       %%mm1, %%mm3          \n\t"
                "punpcklbw  %%mm7, %%mm2          \n\t"
                "punpcklbw  %%mm7, %%mm3          \n\t"
                "paddw      %%mm3, %%mm2          \n\t"
//                "paddw      %%mm6, %%mm2          \n\t"
                "psrlw      $1, %%mm2          \n\t"

                "movq       (%%edx), %%mm3          \n\t" // error
                "paddw      %%mm3, %%mm2          \n\t"
                "packuswb   %%mm7, %%mm2          \n\t"
                "movd       %%mm2, (%%eax)          \n\t" // curr


                "punpckhbw  %%mm7, %%mm0          \n\t"
                "punpckhbw  %%mm7, %%mm1          \n\t"
                "paddw      %%mm1, %%mm0          \n\t"
//                "paddw      %%mm6, %%mm0          \n\t"
                "psrlw      $1, %%mm0          \n\t"

                "movq       8(%%edx), %%mm1          \n\t" // error
                "paddw      %%mm1, %%mm0          \n\t"
                "packuswb   %%mm7, %%mm0          \n\t"
                "movd       %%mm0, 4(%%eax)           \n\t"// curr

                "add        %%edi, %%ebx          \n\t"
                "add        %%edi, %%eax          \n\t"
                "add        $16, %%edx          \n\t"

                "dec            %%ecx          \n\t"
                "jne            mainLoop3a          \n\t"
                "emms   \n\t"
                :
                :   "a" (ppxlcCurrQMB),
                    "b"   (ppxlcRef),
                    "d" (ppxliErrorQMB),
                    "D" (iWidthFrm),
                    "m" (x0001000100010001)
                :    "%eax", "%ebx", "%edx", "%edi", "%ecx", "%esi", "%ebp"
            );
        }
        else { // bXSubPxl && bYSubPxl
            static const __int64    x0001000100010001 = 0x0001000100010001;
            asm volatile
            (
                "mov        %0, %%eax \n\t"
                "mov        %1, %%ebx \n\t"
                "mov        %2, %%edx \n\t"
                "mov        %3, %%edi \n\t"
                "mov        $1, %%esi              \n\t"
                "mov        $8, %%ecx              \n\t"
                "pxor       %%mm7, %%mm7              \n\t"

            "mainLoop4a:              \n\t"
                "movq       (%%ebx), %%mm0               \n\t" // pred
                "movq       1(%%ebx), %%mm1              \n\t" // pred + 1
                "movq       (%%ebx, %%edi,1), %%mm2              \n\t" // pred
              "movq         1(%%ebx, %%edi, 1), %%mm3              \n\t" // pred + 1
                "movq       %%mm0, %%mm4              \n\t"
                "movq       %%mm1, %%mm5              \n\t"
                "movq       %%mm2, %%mm6              \n\t"
                "punpcklbw  %%mm7, %%mm4              \n\t"
                "punpcklbw  %%mm7, %%mm5              \n\t"
                "punpcklbw  %%mm7, %%mm6              \n\t"
                "paddw      %%mm5, %%mm4              \n\t"
                "paddw      %%mm6, %%mm4              \n\t"
                "movq       %%mm3, %%mm5              \n\t"
                "punpcklbw  %%mm7, %%mm5              \n\t"
                "paddw      %%mm5, %%mm4              \n\t"
                "paddw      %4, %%mm4              \n\t"
//              "paddw      x0001000100010001, %%mm4              \n\t"
                "psrlw      $2, %%mm4              \n\t"

                "movq       (%%edx), %%mm6              \n\t" // error
                "pxor       %%mm7, %%mm7              \n\t"

                "paddw      %%mm6, %%mm4              \n\t"
                "packuswb   %%mm7, %%mm4              \n\t"
                "movd       %%mm4, (%%eax)               \n\t"// curr


                "punpckhbw  %%mm7, %%mm0              \n\t"
                "punpckhbw  %%mm7, %%mm1              \n\t"
                "punpckhbw  %%mm7, %%mm2              \n\t"
                "punpckhbw  %%mm7, %%mm3              \n\t"
                "paddw      %%mm1, %%mm0              \n\t"
                "paddw      %%mm2, %%mm0              \n\t"
                "paddw      %%mm3, %%mm0              \n\t"
                "paddw      %4, %%mm0              \n\t"
//              "paddw      x0001000100010001, %%mm0              \n\t"
                "psrlw      $2, %%mm0              \n\t"

                "movq       8(%%edx), %%mm1              \n\t" // error
                "pxor       %%mm7, %%mm7              \n\t"
                "paddw      %%mm1, %%mm0              \n\t"
                "packuswb   %%mm7, %%mm0              \n\t"
                "movd       %%mm0, 4(%%eax)               \n\t"// curr

                "add        %%edi, %%ebx              \n\t"
                "add        %%edi, %%eax              \n\t"
                "add        $16, %%edx              \n\t"

                "dec            %%ecx              \n\t"
                "jne            mainLoop4a       \n\t"
                "emms   \n\t"
                :
                :   "m" (ppxlcCurrQMB),
                    "m" (ppxlcRef),
                    "m" (ppxliErrorQMB),
                    "m" (iWidthFrm),
                    "m" (x0001000100010001)
//                :    "%eax", "%ebx", "%edx", "%edi"
                :    "%eax", "%ebx", "%edx", "%edi", "%ecx", "%esi", "%ebp"
            );
        }
    }
#endif
}

Void_WMV g_MotionCompRndCtrl_MMX(
    tWMVDecInternalMember *pWMVDec,
	U8_WMV*             ppxlcPredMB,
	const U8_WMV*       ppxlcRefMB,
    I32_WMV             iWidthFrm,
    Bool_WMV            bInterpolateX,
    Bool_WMV            bInterpolateY,
    I32_WMV             iMixedPelMV	
)
{
  	static const Int64 x01 = (Int64)0x0101010101010101;
    static const Int64 x7f = (Int64)0x7f7f7f7f7f7f7f7f;
    static const Int64 x3F = (Int64)0x3F3F3F3F3F3F3F3F;
    static const Int64 x03 = (Int64)0x0303030303030303;
    //static const Int64 x02 = (Int64)0x0202020202020202;

#if defined(_WIN32) || defined(_XBOX)
     if (bInterpolateY) {
        if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
            _asm {
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB
            mov         eax,iWidthFrm
            mov         ebx,iWidthFrm
            mov         ecx,8
            cmp         ecx,16
            jz          CopyLoop2
CopyLoop1:
            movq        mm0,[esi]
            add         esi,eax
            movq        [edi],mm0
            add         edi,ebx
            dec         ecx
            jnz         CopyLoop1
            jmp         End
CopyLoop2:
            movq        mm0,[esi]
            movq        [edi],mm0
            movq        mm0,[esi+8]
            add         esi,eax
            movq        [edi+8],mm0
            add         edi,ebx
            dec         ecx
            jnz         CopyLoop2
            }
        }
        else {  //bXSubPxl && !bYSubPxl
            _asm {
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB
            mov         edx,1
            mov         eax,iWidthFrm
            mov         ebx,iWidthFrm
            movq        mm7,x7f
            movq        mm6,x01
            mov         ecx,8
            cmp         ecx,16
            jz          HorizantalLoop2
HorizantalLoop1:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]

            //avg mm0,mm1
            movq        mm5,mm0
            //por         mm5,mm1
            pand        mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi],mm0
            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         HorizantalLoop1
            jmp         End

HorizantalLoop2:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]

            //avg mm0,mm1
            movq        mm5,mm0
            //por         mm5,mm1
            pand        mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi],mm0

            movq        mm0,[esi+8]
            movq        mm1,[esi+edx+8]

            //avg mm0,mm1
            movq        mm5,mm0
            //por         mm5,mm1
            pand        mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi+8],mm0

            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         HorizantalLoop2
	        }
        }
    }
    else {
        if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
            _asm {
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB
            mov         eax,iWidthFrm
            mov         edx,1
            imul        edx,eax
            mov         eax,iWidthFrm
            mov         ebx,iWidthFrm
            movq        mm7,x7f
            movq        mm6,x01
            mov         ecx,8
            cmp         ecx,16
            jz          VerticalLoop2
VerticalLoop1:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]

            //avg mm0,mm1
            movq        mm5,mm0
            //por         mm5,mm1
            pand        mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi],mm0
            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         VerticalLoop1
            jmp         End

VerticalLoop2:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]

            //avg mm0,mm1
            movq        mm5,mm0
            //por         mm5,mm1
            pand        mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi],mm0

            movq        mm0,[esi+8]
            movq        mm1,[esi+edx+8]

            //avg mm0,mm1
            movq        mm5,mm0
            //por         mm5,mm1
            pand        mm5,mm1
            psrlw       mm0,1
            psrlw       mm1,1
            pand        mm5,mm6
            pand        mm0,mm7
            pand        mm1,mm7
            paddw       mm0,mm5
            paddw       mm0,mm1

            movq        [edi+8],mm0

            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         VerticalLoop2
	        }
        }
        else { // bXSubPxl && bYSubPxl
            _asm {            
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB

            mov         eax,1
            cmp         eax,-1
            jnz         PositiveXMV
            dec         esi
PositiveXMV:
            movq        mm7,x3F
            movq        mm6,x03
            mov         eax,iWidthFrm
            mov         edx,1
            imul        edx,eax
            mov         ecx,iWidthFrm
            mov         ebx,iWidthFrm
            //mov         [esp-4],ebp
            push        ebp // for x-box
            mov         ebp,8
            cmp         ebp,16
            jz          BilinearLoop2
BilinearLoop1:
            movq        mm0,[esi]
            movq        mm4,mm0
            pand        mm4,mm6
            movq        mm1,[esi+1]
            movq        mm5,mm1
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm2,[esi+edx]
            movq        mm5,mm2
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm3,[esi+edx+1]
            movq        mm5,mm3
            pand        mm5,mm6
            paddw       mm4,mm5
            paddw       mm4,x01
            psrlw       mm4,2
            pand        mm4,mm6
            psrlw       mm0,2
            pand        mm0,mm7
            psrlw       mm1,2
            pand        mm1,mm7
            paddw       mm0,mm1
            psrlw       mm2,2
            pand        mm2,mm7
            paddw       mm0,mm2
            psrlw       mm3,2
            pand        mm3,mm7
            paddw       mm0,mm3
            paddw       mm0,mm4
            movq        [edi],mm0

            add         esi,ecx
            add         edi,ebx
            dec         ebp
            jnz         BilinearLoop1
            //mov         ebp,[esp-4]
            pop         ebp
            jmp         End

BilinearLoop2:
            movq        mm0,[esi]
            movq        mm4,mm0
            pand        mm4,mm6
            movq        mm1,[esi+1]
            movq        mm5,mm1
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm2,[esi+edx]
            movq        mm5,mm2
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm3,[esi+edx+1]
            movq        mm5,mm3
            pand        mm5,mm6
            paddw       mm4,mm5
            paddw       mm4,x01
            psrlw       mm4,2
            pand        mm4,mm6
            psrlw       mm0,2
            pand        mm0,mm7
            psrlw       mm1,2
            pand        mm1,mm7
            paddw       mm0,mm1
            psrlw       mm2,2
            pand        mm2,mm7
            paddw       mm0,mm2
            psrlw       mm3,2
            pand        mm3,mm7
            paddw       mm0,mm3
            paddw       mm0,mm4
            movq        [edi],mm0

            movq        mm0,[esi+8]
            movq        mm4,mm0
            pand        mm4,mm6
            movq        mm1,[esi+9]
            movq        mm5,mm1
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm2,[esi+edx+8]
            movq        mm5,mm2
            pand        mm5,mm6
            paddw       mm4,mm5
            movq        mm3,[esi+edx+9]
            movq        mm5,mm3
            pand        mm5,mm6
            paddw       mm4,mm5
            paddw       mm4,x01
            psrlw       mm4,2
            pand        mm4,mm6
            psrlw       mm0,2
            pand        mm0,mm7
            psrlw       mm1,2
            pand        mm1,mm7
            paddw       mm0,mm1
            psrlw       mm2,2
            pand        mm2,mm7
            paddw       mm0,mm2
            psrlw       mm3,2
            pand        mm3,mm7
            paddw       mm0,mm3
            paddw       mm0,mm4
            movq        [edi+8],mm0

            add         esi,ecx
            add         edi,ebx
            dec         ebp
            jnz         BilinearLoop2
            //mov         ebp,[esp-4]
            pop         ebp // for x-box
            }
        }
    }
End: _asm emms;
#else // _Embedded_x86
     if (bInterpolateY) {
        if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
            asm volatile
            (
            "cmp         $16, %%ecx \n\t"
            "jz          CopyLoop2  \n\t"

"CopyLoop1:             \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "add         %%eax, %%esi     \n\t"
            "movq        %%mm0, (%%edi)     \n\t"
            "add         %%ebx,%%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         CopyLoop1     \n\t"
            "jmp         Endb     \n\t"

"CopyLoop2:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        %%mm0, (%%edi)     \n\t"
            "movq        8(%%esi), %%mm0     \n\t"
            "add         %%eax,%%esi     \n\t"
            "movq        %%mm0, 8(%%edi)     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         CopyLoop2     \n\t"
             :
             : "S"  (ppxlcRefMB),
               "D"  (ppxlcPredMB),
               "a"  (iWidthFrm),
               "b" (iWidthFrm),
               "c" (8)
             : "%esi", "%edi", "%eax", "%ebx", "%ecx", "%edx", "%ebp"
            );
        } 
        else {  //bXSubPxl && !bYSubPxl
            asm volatile
            (
             "mov %0, %%esi         \n\t"
             "mov %1, %%edi         \n\t"
             "mov %2, %%edx         \n\t"
             "mov %3, %%eax         \n\t"
             "mov %4, %%ebx         \n\t"
             "mov %5, %%ecx         \n\t"
//            "mov         $1, %%ebp   \n\t" // IW
            "movq        %6, %%mm7     \n\t"
//            "movq        x7f, %%mm7     \n\t"
            "movq        %7, %%mm6     \n\t"
//            "movq        x01, %%mm6     \n\t"
            "cmp         $16, %%ecx     \n\t"
            "jz          HorizantalLoop2     \n\t"
" HorizantalLoop1:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm1     \n\t"

            //avg %%mm0,%%mm1     \n\t"
            "movq        %%mm0,%%mm5     \n\t"
//            "por         %%mm1,%%mm5     \n\t"
            "pand         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"

            "movq        %%mm0, (%%edi)     \n\t"
            "add         %%eax, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         HorizantalLoop1     \n\t"
            "jmp         Endb     \n\t"

"HorizantalLoop2:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm1     \n\t"

            //avg %%mm0,%%mm1     \n\t"
            "movq        %%mm0,%%mm5     \n\t"
//            "por         %%mm1,%%mm5     \n\t"
            "pand         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"

            "movq        %%mm0, (%%edi)     \n\t"
    
            "movq        8(%%esi), %%mm0     \n\t"
            "movq        8(%%esi, %%edx, 1), %%mm1     \n\t"

            //avg %%mm0,%%mm1      // Here , IW
            "movq        %%mm0,%%mm5     \n\t"
//            "por         %%mm1,%%mm5     \n\t"
            "pand         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"

            "movq        %%mm0, 8(%%edi)     \n\t"

            "add         %%eax, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         HorizantalLoop2     \n\t"
            :
            :   "m"  (ppxlcRefMB),
                "m"  (ppxlcPredMB),
                "m"  (1),
                "m"  (iWidthFrm),
                "m"   (iWidthFrm),
                "m"  (8),
                "m"  (x7f),
                "m"  (x01)
            :  "%esi", "%edi", "%edx", "%eax", "%ebx", "%ecx", "%ebp"
            );
        }
    }
    else {
        if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
            asm volatile
            (
            "mov %0, %%esi  \n\t"
            "mov %1, %%edi  \n\t"
            "mov %2, %%edx  \n\t"
            "mov %3, %%eax  \n\t"
            "mov %4, %%ebx  \n\t"
            "mov %5, %%ecx  \n\t"
//            "mov         $1, %%ebp     \n\t"
            "imul        %%eax, %%edx     \n\t"
            "mov         %3, %%eax     \n\t"
            "movq        %6, %%mm7     \n\t"
//            "movq        x7f, %%mm7     \n\t"
            "movq        %7, %%mm6     \n\t"
//            "movq        x01, %%mm6     \n\t"
            "cmp         $16, %%ecx     \n\t"
            "jz          VerticalLoop2     \n\t"
"VerticalLoop1:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm1     \n\t"  // esi + edi

            //avg %%mm0,%%mm1
            "movq        %%mm0,%%mm5     \n\t"
//            "por         %%mm1,%%mm5     \n\t"
            "pand         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"

            "movq        %%mm0, (%%edi)     \n\t"
            "add         %%eax, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         VerticalLoop1     \n\t"
            "jmp         Endb     \n\t"
    
"VerticalLoop2:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm1     \n\t"  // esi + edi

            //avg %%mm0,%%mm1
            "movq        %%mm0,%%mm5     \n\t"
//            "por         %%mm1,%%mm5     \n\t"
            "pand         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"

            "movq        %%mm0, (%%edi)     \n\t"

            "movq        8(%%esi), %%mm0     \n\t"
            "movq        8(%%esi, %%edx,1), %%mm1     \n\t"

            //avg %%mm0,%%mm1
            "movq        %%mm0,%%mm5     \n\t"
//            "por         %%mm1,%%mm5     \n\t"
            "pand         %%mm1,%%mm5     \n\t"
            "psrlw       $1, %%mm0     \n\t"
            "psrlw       $1, %%mm1     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm5,%%mm0     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"

            "movq        %%mm0, 8(%%edi)     \n\t"

            "add         %%eax, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ecx     \n\t"
            "jnz         VerticalLoop2     \n\t"
            :
            :   "m"  (ppxlcRefMB),
                "m"  (ppxlcPredMB),
                "m"   (1),
                "m"   (iWidthFrm),
                "m"   (iWidthFrm),
                "m"   (8),
                "m"   (x7f),
                "m"   (x01)
            :   "%esi", "%edi", "%edx", "%eax", "%ebx", "%ecx", "%ebp"
            );
        }   
        else { // bXSubPxl && bYSubPxl

//            asm volatile ("mov  %0, %%esi : :  "m" (ppxlcRefMB));
            asm volatile
            (
            "mov  %0, %%esi  \n\t"
            "mov  %1, %%edi  \n\t"
            "mov  %2, %%eax  \n\t"
            "cmp         $-1, %%eax     \n\t"
            "jnz         PositiveXMVa     \n\t"
            "dec         %%esi     \n\t"
"PositiveXMVa:     \n\t"
            "movq        %3, %%mm7     \n\t"
            "movq        %4, %%mm6     \n\t"
            "mov         %5, %%eax     \n\t"
            "mov         %7, %%edx     \n\t"
//            "movq        x3F, %%mm7     \n\t"
//            "movq        x03, %%mm6     \n\t"
//            "mov         iWidthFrm, %%eax     \n\t"
//            "mov         1, %%edx     \n\t"
            "imul        %%eax, %%edx     \n\t"
            "mov         %5, %%ecx \n\t"
            "mov         %6, %%ebx  \n\t"
//            "mov         iWidthFrm, %%ecx \n\t"
//            "mov         iWidthFrm, %%ebx  \n\t"
//            mov         [esp-4],ebp     \n\t"
            "push        %%ebp  \n\t" // for xbox->fix esp crash because the stack can't be accessed     \n\t"
            "mov         %8, %%ebp  \n\t"
//            "mov         8, %%ebp  \n\t"
            "cmp         $16, %%ebp     \n\t"
            "jz          BilinearLoop2     \n\t"
"BilinearLoop1:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        %%mm0,%%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t" // here
            "movq        1(%%esi), %%mm1     \n\t"
            "movq        %%mm1,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm2     \n\t"
            "movq        %%mm2,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "mov         %%edx, %%eax    \n\t"
            "movq        1(%%esi, %%edx,1), %%mm3     \n\t"
            "movq        %%mm3,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "paddw       %9, %%mm4 \n\t"
//            "paddw       x02, %%mm4 \n\t"
            "psrlw       $2, %%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t"
            "psrlw       $2, %%mm0     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "psrlw       $2, %%mm1     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"
            "psrlw       $2, %%mm2     \n\t"
            "pand        %%mm7,%%mm2     \n\t"
            "paddw       %%mm2,%%mm0     \n\t"
            "psrlw       $2, %%mm3     \n\t"
            "pand        %%mm7,%%mm3     \n\t"
            "paddw       %%mm3,%%mm0     \n\t"
            "paddw       %%mm4,%%mm0     \n\t"
            "movq        %%mm0, (%%edi)     \n\t"

            "add         %%ecx, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec          %%ebp     \n\t"
            "jnz         BilinearLoop1     \n\t"
//            mov         ebp,[esp-4]     \n\t"
            "pop         %%ebp      \n\t"
            "jmp         Endb     \n\t"

"BilinearLoop2:     \n\t"
            "movq        (%%esi), %%mm0     \n\t"
            "movq        %%mm0,%%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t" // here
            "movq        1(%%esi), %%mm1     \n\t"
            "movq        %%mm1,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "movq        (%%esi, %%edx, 1), %%mm2     \n\t"
            "movq        %%mm2,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "movq        1(%%esi, %%edx, 1), %%mm3     \n\t"
            "movq        %%mm3,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "paddw       %9, %%mm4 \n\t"
//            "paddw       x02, %%mm4 \n\t"
            "psrlw       $2, %%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t"
            "psrlw       $2, %%mm0     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "psrlw       $2, %%mm1     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"
            "psrlw       $2, %%mm2     \n\t"
            "pand        %%mm7,%%mm2     \n\t"
            "paddw       %%mm2,%%mm0     \n\t"
            "psrlw       $2, %%mm3     \n\t"
            "pand        %%mm7,%%mm3     \n\t"
            "paddw       %%mm3,%%mm0     \n\t"
            "paddw       %%mm4,%%mm0     \n\t"
            "movq        %%mm0, (%%edi)     \n\t"

            "movq        8(%%esi), %%mm0     \n\t"
            "movq        %%mm0,%%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t" // here
            "movq        9(%%esi), %%mm1     \n\t" // esi + 9
            "movq        %%mm1,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "movq        8(%%esi, %%edx, 1), %%mm2     \n\t" // esi+ edx +8
            "movq        %%mm2,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "movq        9(%%esi, %%edx, 1), %%mm3     \n\t" //esi+edx+9
            "movq        %%mm3,%%mm5     \n\t"
            "pand        %%mm6,%%mm5     \n\t"
            "paddw       %%mm5,%%mm4     \n\t"
            "paddw       %9, %%mm4 \n\t"
//            "paddw       x02, %%mm4 \n\t"
            "psrlw       $2, %%mm4     \n\t"
            "pand        %%mm6,%%mm4     \n\t"
            "psrlw       $2, %%mm0     \n\t"
            "pand        %%mm7,%%mm0     \n\t"
            "psrlw       $2, %%mm1     \n\t"
            "pand        %%mm7,%%mm1     \n\t"
            "paddw       %%mm1,%%mm0     \n\t"
            "psrlw       $2, %%mm2     \n\t"
            "pand        %%mm7,%%mm2     \n\t"
            "paddw       %%mm2,%%mm0     \n\t"
            "psrlw       $2, %%mm3     \n\t"
            "pand        %%mm7,%%mm3     \n\t"
            "paddw       %%mm3,%%mm0     \n\t"
            "paddw       %%mm4,%%mm0     \n\t"
            "movq        %%mm0, 8(%%edi)     \n\t"

            "add         %%ecx, %%esi     \n\t"
            "add         %%ebx, %%edi     \n\t"
            "dec         %%ebp     \n\t"
            "jnz         BilinearLoop2     \n\t"
//            mov         ebp,[esp-4]     \n\t"
            "pop         %%ebp \n\t" // for x-box
            :
            :   "m"  (ppxlcRefMB),
                "m"  (ppxlcPredMB),
                "m"   (1),
                "m"  (x3F),
                "m"  (x03),
                "m"  (iWidthFrm),
                "m"  (iWidthFrm),
                "m"  (1),
                "m"  (8),
                "m"  (x01)
            :   "%esi", "%edi", "%edx", "%eax", "%ebx", "%ecx", "%ebp"
            );
        }
    }
    asm volatile
    (
"Endb: \n\t"
        "emms \n\t"
    );
#endif
}
#endif
#endif

#if defined(_WMV_TARGET_X86_)
#if defined(_WIN32) || defined(_XBOX) || defined(_Embedded_x86)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Katmai (P3) Versions ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Void_WMV g_MotionComp_KNI(
    tWMVDecInternalMember *pWMVDec,
	U8_WMV*             ppxlcPredMB,
	const U8_WMV*       ppxlcRefMB,
    I32_WMV             iWidthFrm,
    Bool_WMV            bInterpolateX,
    Bool_WMV            bInterpolateY,
    I32_WMV             iMixedPelMV	
)
{
  	static const Int64 x01 = (Int64)0x0101010101010101;
    static const Int64 x7f = (Int64)0x7f7f7f7f7f7f7f7f;
    static const Int64 x3F = (Int64)0x3F3F3F3F3F3F3F3F;
    static const Int64 x03 = (Int64)0x0303030303030303;
    static const Int64 x02 = (Int64)0x0202020202020202;

     if (bInterpolateY) {
        if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
            _asm {
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB
            mov         eax,iWidthFrm
            mov         ebx,iWidthFrm
            mov         ecx,8
            cmp         ecx,16
            jz          CopyLoop2
CopyLoop1:
            movq        mm0,[esi]
            add         esi,eax
            movq        [edi],mm0
            add         edi,ebx
            dec         ecx
            jnz         CopyLoop1
            jmp         End
CopyLoop2:
            movq        mm0,[esi]
            movq        [edi],mm0
            movq        mm0,[esi+8]
            add         esi,eax
            movq        [edi+8],mm0
            add         edi,ebx
            dec         ecx
            jnz         CopyLoop2
            }
        }
        else {  //bXSubPxl && !bYSubPxl
            _asm {
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB
            mov         edx,1
            mov         eax,iWidthFrm
            mov         ebx,iWidthFrm
            movq        mm7,x7f
            movq        mm6,x01
            mov         ecx,8
            cmp         ecx,16
            jz          HorizantalLoop2
HorizantalLoop1:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]
            pavgb       mm0_mm1
            movq        [edi],mm0
            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         HorizantalLoop1
            jmp         End

HorizantalLoop2:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]
            pavgb       mm0_mm1
            movq        [edi],mm0

            movq        mm0,[esi+8]
            movq        mm1,[esi+edx+8]
            pavgb       mm0_mm1
            movq        [edi+8],mm0

            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         HorizantalLoop2
	        }
        }
    }
    else {
        if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
            _asm {
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB
            mov         eax,iWidthFrm
            mov         edx,1
            imul        edx,eax
            mov         eax,iWidthFrm
            mov         ebx,iWidthFrm
            movq        mm7,x7f
            movq        mm6,x01
            mov         ecx,8
            cmp         ecx,16
            jz          VerticalLoop2
VerticalLoop1:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]
            pavgb       mm0_mm1
            movq        [edi],mm0
            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         VerticalLoop1
            jmp         End

VerticalLoop2:
            movq        mm0,[esi]
            movq        mm1,[esi+edx]
            pavgb       mm0_mm1
            movq        [edi],mm0

            movq        mm0,[esi+8]
            movq        mm1,[esi+edx+8]
            pavgb       mm0_mm1
            movq        [edi+8],mm0

            add         esi,eax
            add         edi,ebx
            dec         ecx
            jnz         VerticalLoop2
	        }
        }
        else { // bXSubPxl && bYSubPxl
            _asm {
            // for x-box->fix esp crash because the stack can't be accessed
            push        eax;
            push        eax;
            push        eax;
            push        eax;
            // -------------
            mov         esi,ppxlcRefMB
            mov         edi,ppxlcPredMB

            mov         eax,1
            cmp         eax,-1
            jnz         PositiveXMV
            dec         esi
PositiveXMV:
            mov         eax,1
            mov         edx,iWidthFrm
            cmp         eax,-1
            jnz         PositiveYMV
            sub         esi,edx
PositiveYMV:
            mov         [esp+8],esi // IW
            mov         [esp+12],edi // IW

            movq        mm7,x3F
            movq        mm6,x03
            mov         ecx,iWidthFrm
            mov         ebx,iWidthFrm
            mov         [esp+4],ebp // IW
            mov         ebp,8

            movq        mm0,[esi]
            movq        mm1,[esi+1]

            movq        mm4,mm0
            pand        mm4,mm6
            pxor        mm0,mm4

            movq        mm5,mm1
            pand        mm5,mm6
            pxor        mm1,mm5
            paddw       mm4,mm5

            pavgb       mm0_mm1

BilinearLoop1:

            movq        mm1,[esi+edx]
            movq        mm3,[esi+edx+1]

            movq        mm7,mm1
            pand        mm7,mm6
            pxor        mm1,mm7

            movq        mm5,mm3
            pand        mm5,mm6
            pxor        mm3,mm5
            paddw       mm7,mm5
            paddw       mm4,mm7
            paddw       mm4,x02
            psrlw       mm4,2
            pand        mm4,mm6

            pavgb       mm1_mm3
            movq        mm2,mm1
            pavgb       mm0_mm1
            paddw       mm0,mm4

            movq        [edi],mm0

            movq        mm0,mm2
            movq        mm4,mm7

            add         esi,ecx
            add         edi,ebx
            dec         ebp
            jnz         BilinearLoop1

            mov         ebp,[esp+4] // IW
            mov         ebp,8
            cmp         ebp,16
            jnz         Done

            mov         esi,[esp+8] // IW
            add         esi,8
            mov         edi,[esp+12] // IW
            add         edi,8

            movq        mm0,[esi]
            movq        mm1,[esi+1]

            movq        mm4,mm0
            pand        mm4,mm6
            pxor        mm0,mm4

            movq        mm5,mm1
            pand        mm5,mm6
            pxor        mm1,mm5
            paddw       mm4,mm5

            pavgb       mm0_mm1

BilinearLoop2:

            movq        mm1,[esi+edx]
            movq        mm3,[esi+edx+1]

            movq        mm7,mm1
            pand        mm7,mm6
            pxor        mm1,mm7

            movq        mm5,mm3
            pand        mm5,mm6
            pxor        mm3,mm5
            paddw       mm7,mm5
            paddw       mm4,mm7
            paddw       mm4,x02
            psrlw       mm4,2
            pand        mm4,mm6

            pavgb       mm1_mm3
            movq        mm2,mm1
            pavgb       mm0_mm1
            paddw       mm0,mm4

            movq        [edi],mm0

            movq        mm0,mm2
            movq        mm4,mm7

            add         esi,ecx
            add         edi,ebx
            dec         ebp
            jnz         BilinearLoop2

Done:       mov         ebp,[esp+4] // IW
            // for x-box
            pop         eax;
            pop         eax;
            pop         eax;
            pop         eax;
            // ------------
            }

        }
    }
End: _asm emms;
}
#endif
#endif

#pragma warning( default : 4731 )  // turn on ebp in inline assembly warning
