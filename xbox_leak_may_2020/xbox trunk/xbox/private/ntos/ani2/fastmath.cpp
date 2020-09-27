/*--
Copyright (c) 2000  Microsoft Corporation - Xbox SDK

Module Name:

    fastmath.cpp
--*/

#include "precomp.h"
//#include "Debug.h"

//----------------------------------------------------------------------------
// Katmai and MMX(TM) constants implementation
#define _MM_ALIGN16 __declspec(align(16))

#define SC_OPT

#if !defined(M128)
#define M128(x) (*(__m128*)&(x))
#endif

typedef unsigned long DWORD;

#define _M128_CONST(Name, Val) \
static const _MM_ALIGN16 float _m128const_##Name[4] = { Val, Val, Val, Val }

#define _M128_CONST4(Name, Val0, Val1, Val2, Val3) \
static const _MM_ALIGN16 float _m128const_##Name[4] = { Val0, Val1, Val2, Val3 }

#define M128_EXTERN_CONST(Name, Val) \
static const _MM_ALIGN16 float _m128const_##Name[4] = { Val, Val, Val, Val }; \
const __m128* p##Name = (__m128*)_m128const_##Name

#define M128_EXTERN_CONST_TYPE(Name, Val, Type) \
static const _MM_ALIGN16 Type _m128const_##Name[4] = { Val, Val, Val, Val }; \
const __m128* p##Name = (__m128*)_m128const_##Name

#define M128_CONST(Name, Val) \
static const _MM_ALIGN16 float _m128const_##Name[4] = { Val, Val, Val, Val }; \
const __m128 Name = M128(_m128const_##Name)

M128_EXTERN_CONST(am_0, 0.0f);
M128_EXTERN_CONST(am_1, 1.0f);
M128_EXTERN_CONST(am_minus_1, -1.0f);
M128_EXTERN_CONST(am_0p5, 0.5f);
M128_EXTERN_CONST(am_1p5, 1.5f);
M128_EXTERN_CONST(am_3_over_2, 3.0f / 2.0f);
M128_EXTERN_CONST(am_pi, PI);
M128_EXTERN_CONST(am_pi_over_2, (PI / 2.0f));
M128_EXTERN_CONST(am_2_over_pi, (2.0f / PI));
M128_EXTERN_CONST_TYPE(am_sign_mask, 0x80000000, DWORD);
M128_EXTERN_CONST_TYPE(am_inv_sign_mask, ~0x80000000, DWORD);
M128_EXTERN_CONST_TYPE(am_min_pos_norm, 0x00800000, DWORD);
M128_EXTERN_CONST_TYPE(am_mant_mask, 0x7f800000, DWORD);
M128_EXTERN_CONST_TYPE(am_inv_mant_mask, ~0x7f800000, DWORD);


//----------------------------------------------------------------------------
// Katmai and MMX(TM) constants implementation
static const float p0 = 0.15707963267948963959e1f;
static const float p1 = -0.64596409750621907082e0f;
static const float p2 = 0.7969262624561800806e-1f;
static const float p3 = -0.468175413106023168e-2f;

static const float t0 = -0.91646118527267623468e-1f;
static const float t1 = -0.13956945682312098640e1f;
static const float t2 = -0.94393926122725531747e2f;
static const float t3 =  0.12888383034157279340e2f;
static const float s0 =  0.12797564625607904396e1f;
static const float s1 =  0.21972168858277355914e1f;
static const float s2 =  0.68193064729268275701e1f;
static const float s3 =  0.28205206687035841409e2f;

static const float p0exp = 1.26177193074810590878e-4f;
static const float p1exp = 3.02994407707441961300e-2f;

static const float q0 = 3.00198505138664455042e-6f;
static const float q1 = 2.52448340349684104192e-3f;
static const float q2 = 2.27265548208155028766e-1f;
static const float q3 = 2.00000000000000000009e0f;

static const float rln2 = 1.4426950408889634073599f;
static const float c1 = 6.93145751953125e-1f;
static const float c2 = 1.42860682030941723212e-6f;

	const float at3613 = 2.7692309f;
	const float at2511 = 2.2727273f;
	const float at36 = 36.0f;
	const float at25 = 25.0f;
	const float at16 = 16.0f;
	const float at11 = 11.0f;
	const float at9 = 9.0f;
	const float at7 = 7.0f;
	const float at5 = 5.0f;
	const float at4 = 4.0f;
	const float at3 = 3.0f;
	const float at1 = 1.0f;
	const float at_p2 = PI_DIV_2;
	const float mp2 = -PI_DIV_2;

	const float as2 = FLOAT_SMALL;

	const float SQ2 = 1.4142136f;
	const float SQ3 = 0.3333333f;
	const float SQ5 = 1.4000000f;
	const float SQ7 = 0.1428571f;
	const float LOG2 = 0.3465736f;

    static const float log_p0 = -7.89580278884799154124e-1f;
    static const float log_p1 = 1.63866645699558079767e1f;
    static const float log_p2 = -6.41409952958715622951e1f;

    static const float log_q0 = -3.56722798256324312549e1f;
    static const float log_q1 = 3.12093766372244180303e2f;
    static const float log_q2 = -7.69691943550460008604e2f;

    static const float log_rsqrt2 = 7.07106781186547524401e-1f;
    static const float log_c0 = 0.693147180559945f;

    static const float fmax = 88.0f;
    static const float fmin = -88.0f;

    static const float pow_p0 = -7.89580278884799154124e-1f;
    static const float pow_p1 = 1.63866645699558079767e1f;
    static const float pow_p2 = -6.41409952958715622951e1f;

    static const float pow_q0 = -3.56722798256324312549e1f;
    static const float pow_q1 = 3.12093766372244180303e2f;
    static const float pow_q2 = -7.69691943550460008604e2f;

    static const float pow_rsqrt2 = 7.07106781186547524401e-1f;
    static const float pow_c0 = 1.44269504088896340735992f;

    static const float pow_r0 = 2.30933477057345225087e-2f;
    static const float pow_r1 = 2.02020656693165307700e1f;
    static const float pow_r2 = 1.51390680115615096133e3f;

    static const float pow_s0 = 2.33184211722314911771e2f;
    static const float pow_s1 = 4.36821166879210612817e3f;

    static const float pow_fmax = 128.0f;
    static const float pow_fmin = -127.0f;

	const float th1 = 1.0f;
	const float th2p = 2.0f;
	const float th2m = -2.0f;
	const float th3 = 0.3333333f;


	const float sh1 = 1.0f;
	const float sh5 = 0.5f;
	const float sh6 = 0.1666667f;

	const float t_as[49] = {
		0.9698000f,
		0.9691796f, 0.9685330f, 0.9678589f, 0.9671551f, 0.9664199f, 0.9656509f, 0.9648460f, 0.9640023f,
		0.9631173f, 0.9621876f, 0.9612098f, 0.9601802f, 0.9590943f, 0.9579477f, 0.9567349f, 0.9554501f,
		0.9540865f, 0.9526370f, 0.9510929f, 0.9494448f, 0.9476817f, 0.9457912f, 0.9437591f, 0.9415686f,
		0.9392007f, 0.9366328f, 0.9338384f, 0.9307863f, 0.9274390f, 0.9237517f, 0.9196697f, 0.9151261f,
		0.9100379f, 0.9043011f, 0.8977833f, 0.8903134f, 0.8816667f, 0.8715416f, 0.8595238f, 0.8450292f,
		0.8272059f, 0.8047620f, 0.7756411f, 0.7363636f, 0.6805556f, 0.5952381f, 0.4500000f, 0.1666667f
	};

//----------------------------------------------------------------------------
// atan
float fast_atan
(
    float x
)
//--------------------------------------
{
#if defined(USE_C)

    // 154 cycles
    return atanf(x);

#else
#if defined(SC_OPT)
	_asm {
		mov		eax, x
		movss	xmm0, x
		cmp		eax, 0bf800000h
		jnc		minus1
		cmp		eax, 3f800000h
		jnc		plus1
		movss	xmm1, xmm0
		mulss	xmm1, xmm1
		movss	xmm2, xmm1
		movss	xmm3, xmm1
		movss	xmm4, xmm1
		movss	xmm5, xmm1
		mulss	xmm2, at2511
		mulss	xmm3, at16
		mulss	xmm4, at9
		mulss	xmm5, at4
		addss	xmm2, at9
		rcpss	xmm6, xmm2
		mulss	xmm6, xmm3
		addss	xmm6, at7
		rcpss	xmm2, xmm6
		mulss	xmm2, xmm4
		addss	xmm2, at5
		rcpss	xmm6, xmm2
		mulss	xmm6, xmm5
		addss	xmm6, at3
		rcpss	xmm2, xmm6
		mulss	xmm1, xmm2
		addss	xmm1, at1
		rcpss	xmm2, xmm1
		movss	xmm7, xmm2
		addss	xmm2, xmm2
		mulss	xmm7, xmm7
		mulss	xmm7, xmm1
		subss	xmm2, xmm7

		mulss	xmm0, xmm2
		movss	x, xmm0
	}
	return x;
	_asm {
		ALIGN 16
minus1:
		rcpss	xmm0, xmm0
		movss	xmm1, xmm0
		mulss	xmm1, xmm1
		movss	xmm2, xmm1
		movss	xmm3, xmm1
		movss	xmm4, xmm1
		movss	xmm5, xmm1
		movss	xmm6, xmm1
		mulss	xmm2, at3613
		mulss	xmm3, at25
		mulss	xmm4, at16
		mulss	xmm5, at9
		mulss	xmm6, at4
		addss	xmm2, at11
		rcpss	xmm2, xmm2
		mulss	xmm2, xmm3
		addss	xmm2, at9
		rcpss	xmm2, xmm2
		mulss	xmm2, xmm4
		addss	xmm2, at7
		rcpss	xmm2, xmm2
		movss	xmm3, mp2
		mulss	xmm2, xmm5
		addss	xmm2, at5
		rcpss	xmm2, xmm2
		mulss	xmm2, xmm6
		addss	xmm2, at3
		rcpss	xmm2, xmm2
		mulss	xmm1, xmm2
		addss	xmm1, at1
		rcpss	xmm2, xmm1
		movss	xmm7, xmm2
		addss	xmm2, xmm2
		mulss	xmm7, xmm7
		mulss	xmm7, xmm1
		subss	xmm2, xmm7
		mulss	xmm0, xmm2
		subss	xmm3, xmm0
		movss	x, xmm3
	}
	return x;
	_asm {
		ALIGN 16
plus1:
		rcpss	xmm0, xmm0
		movss	xmm1, xmm0
		mulss	xmm1, xmm1
		movss	xmm2, xmm1
		movss	xmm3, xmm1
		movss	xmm4, xmm1
		movss	xmm5, xmm1
		movss	xmm6, xmm1
		mulss	xmm2, at3613
		mulss	xmm3, at25
		mulss	xmm4, at16
		mulss	xmm5, at9
		mulss	xmm6, at4
		addss	xmm2, at11
		rcpss	xmm2, xmm2
		mulss	xmm2, xmm3
		addss	xmm2, at9
		rcpss	xmm2, xmm2
		mulss	xmm2, xmm4
		addss	xmm2, at7
		rcpss	xmm2, xmm2
		movss	xmm3, at_p2
		mulss	xmm2, xmm5
		addss	xmm2, at5
		rcpss	xmm2, xmm2
		mulss	xmm2, xmm6
		addss	xmm2, at3
		rcpss	xmm2, xmm2
		mulss	xmm1, xmm2
		addss	xmm1, at1
		rcpss	xmm2, xmm1
		movss	xmm7, xmm2
		addss	xmm2, xmm2
		mulss	xmm7, xmm7
		mulss	xmm7, xmm1
		subss	xmm2, xmm7
		mulss	xmm0, xmm2
		subss	xmm3, xmm0
		movss	x, xmm3
	}
    return x;
#else	// SC_OPT
    // 60 cycles
    __asm
    {
        movss   xmm0, x

        movss   xmm1, xmm0
        rcpss   xmm4, xmm0
        orps    xmm1, _m128const_am_sign_mask
        movss   xmm6, xmm4
        comiss  xmm1, _m128const_am_minus_1
        jc      l_big  // 'c' is 'lt' for comiss

        movss   xmm3, t0
        movss   xmm2, xmm0
        mulss   xmm2, xmm2

        movss   xmm1, s0
        addss   xmm1, xmm2

        movss   xmm7, s1
        rcpss   xmm1, xmm1
        mulss   xmm1, xmm3
        movss   xmm3, t1
        addss   xmm7, xmm2
        addss   xmm1, xmm7

        movss   xmm7, s2
        rcpss   xmm1, xmm1
        mulss   xmm1, xmm3
        movss   xmm3, t2
        addss   xmm7, xmm2
        addss   xmm1, xmm7

        movss   xmm7, s3
        rcpss   xmm1, xmm1
        mulss   xmm1, xmm3
        movss   xmm3, t3
        addss   xmm7, xmm2
        mulss   xmm0, xmm3
        addss   xmm1, xmm7

        rcpss   xmm1, xmm1
        mulss   xmm0, xmm1

        jmp l_done

l_big:
        movss   xmm3, t0
        mulss   xmm6, xmm6

        movss   xmm5, s0
        addss   xmm5, xmm6

        movss   xmm7, s1
        rcpss   xmm5, xmm5
        mulss   xmm5, xmm3
        movss   xmm3, t1
        addss   xmm7, xmm6
        addss   xmm5, xmm7

        movss   xmm7, s2
        rcpss   xmm5, xmm5
        mulss   xmm5, xmm3
        movss   xmm3, t2
        addss   xmm7, xmm6
        addss   xmm5, xmm7

        movss   xmm7, s3
        rcpss   xmm5, xmm5
        mulss   xmm5, xmm3
        movss   xmm3, t3
        addss   xmm7, xmm6
        mulss   xmm4, xmm3
        addss   xmm5, xmm7

        movss   xmm2, _m128const_am_sign_mask
        rcpss   xmm5, xmm5
        mulss   xmm5, xmm4

        movss   xmm7, _m128const_am_pi_over_2
        andps   xmm0, xmm2
        orps    xmm0, xmm7
        subss   xmm0, xmm5

l_done:
        movss   x, xmm0
    }
    return x;
#endif	// !SC_OPT

#endif // !USE_C
}


//----------------------------------------------------------------------------
// atan2
float fast_atan2
(
    float x,
    float y
)
//--------------------------------------
{
#if defined(USE_C)

    // 154 cycles
    return atan2f(x, y);

#else
#if defined(SC_OPT)
	_asm {
		movss	xmm0, x
		rcpss	xmm1, y
		movss	xmm2, xmm1
		addss	xmm1, xmm1
		mulss	xmm2, xmm2
		mulss	xmm2, y
		subss	xmm1, xmm2
		mulss	xmm0, xmm1
		movss	x, xmm0
	}
	return fast_atan( x );
#else	// SC_OPT

    // 77 cycles
    fast_atan(x * y);

    __asm
    {
        // We assume fast_atan leaves the return value in xmm0

        xorps   xmm7, xmm7
        movss   xmm1, y //[esp - 20 - 8]
        comiss  xmm1, xmm7
        movss   xmm4, x //[esp - 20 - 4]
        jnc     l_pos   // 'nc' is 'ge' for comiss

        andps   xmm4, _m128const_am_sign_mask
        orps    xmm4, _m128const_am_pi

        addss   xmm0, xmm4

l_pos:
        movss   x, xmm0
    }

    return x;

#endif	// !SC_OPT
#endif // !USE_C
}

//----------------------------------------------------------------------------
// acos
float fast_acos
(
    float x
)
//--------------------------------------
{
#if defined(USE_C)
    // 273 cycles
    return acosf(x);
#else
	_asm {
		mov		eax, x;
		test	eax, 080000000h
		jnz		acminus
		or		eax, eax		; == 0.0
		jz		acretz
		cmp		eax, 3f800000h	; >= 1.0
		jnc		acretp
		jmp		acculc
acminus:
		and		eax, 7fffffffh	;Just in case.  it may be not need.
		jz		acretz			; == -0.0
		cmp		eax, 0bf800000h	; <= -1.0
		jnc		acretm
acculc:
	}
	return PI_DIV_2 - fast_asin( x );
acretz:
	return PI_DIV_2;
acretp:
	return 0.0f;
acretm:
	return PI;
#endif
}


//----------------------------------------------------------------------------
// asin
float fast_asin
(
    float x
)
//--------------------------------------
{
#if defined(USE_C)
    // 279 cycles
    return asinf(x);
#else
	const unsigned long as1 = FP_ONE_BITS;
	const unsigned long* pt_as = (unsigned long*) &t_as[0];

	_asm {
		mov		eax, x;
		test	eax, 080000000h
		jnz		asminus
		or		eax, eax		; == 0.0
		jz		asretz
		cmp		eax, 3f800000h	; >= 1.0
		jnc		asretp
		cmp		eax, 3f3504f3h	; >= SQRT2 / 2
		jnc		asrett
		jmp		asculc
asminus:
		and		eax, 7fffffffh	;Just in case.  it may be not need.
		jz		asretz			; == -0.0
		cmp		eax, 0bf800000h	; <= -1.0
		jnc		asretm
		cmp		eax, 0bf3504f3h	; <= -SQRT2 / 2
		jnc		asrett
asculc:
		movss	xmm0, as1		;xmm0 = factor
		movss	xmm1, xmm0		;xmm1 = sum
		movss	xmm2, xmm0		;xmm2 = power
		movss	xmm3, x			;xmm3 = x
		movss	xmm4, xmm3
		mulss	xmm4, xmm4		;xmm4 = y
		movss	xmm5, as2		;xmm5 = FLOAT_SMALL
		mov		edx, pt_as
		mov		ecx, 48
asloop:
		movss	xmm6, dword ptr[edx + ecx * 4]
		mulss	xmm0, xmm6
		mulss	xmm2, xmm4
		movss	xmm6, xmm0
		mulss	xmm6, xmm2
		addss	xmm1, xmm6
		comiss	xmm6, xmm5
		dec		ecx
		ja		asloop
		mulss	xmm1, xmm3
		movss	x, xmm1
	}
	return x;
asretz:
	return 0.0f;
asretp:
	return PI_DIV_2;
asretm:
	return -PI_DIV_2;
asrett:
	_asm {
		movss	xmm1, x
		mulss	xmm1, xmm1
		movss	xmm0, as1
		subss	xmm0, xmm1
		rsqrtss	xmm1, xmm0
		movss	xmm2, xmm0
		mulss	xmm0, xmm1
		mulss	xmm0, xmm1
		mulss	xmm0, xmm1
		mulss	xmm0, _m128const_am_0p5
		mulss	xmm1, _m128const_am_3_over_2
		subss	xmm1, xmm0
		mulss	xmm1, x
		movss	x, xmm1
	}
	// quality of fast_atan is too bad.
	return fast_atan( x );
#endif
}


//----------------------------------------------------------------------------
// log
float fast_log
(
    float x
)
//--------------------------------------
{
#if defined(USE_C)

    // 106 cycles
    return logf(x);

#else
#if defined(SC_OPT)

    // 58 cycles
	_asm {
		mov			eax, x
		mov			edx, eax
		and			eax, 7fffffh
		movss		xmm2, SQ2
		and			edx, 7f800000h
		or			eax, 3f800000h
		shr			edx, 22
		mov			x, eax
		movss		xmm1, x
		movss		xmm0, xmm1
		movss		xmm3, SQ5
		addss		xmm1, xmm2
		subss		xmm0, xmm2
		sub			edx, 253

		rcpss		xmm2, xmm1
		movss		xmm3, xmm2			;Newton-Raphson
		addss		xmm3, xmm3
		mulss		xmm2, xmm2
		mulss		xmm2, xmm1
		subss		xmm3, xmm2			;complete Newton-Raphson
		mulss		xmm0, xmm3

		movss		xmm2, SQ7
		movss		xmm1, xmm0
		mulss		xmm0, xmm0
		mulss		xmm2, xmm0
		addss		xmm3, xmm0
		mulss		xmm2, xmm3
		addss		xmm2, SQ3
		mulss		xmm0, xmm1
		mulss		xmm0, xmm2
		cvtsi2ss	xmm3, edx
		addss		xmm0, xmm1
		mulss		xmm3, LOG2
		addss		xmm0, xmm0
		addss		xmm0, xmm3
		movss		x, xmm0
	}
#else	// !SC_OPT

    // 66 cycles
    __asm
    {
        movss   xmm0, x

        maxss   xmm0, _m128const_am_min_pos_norm  // Cut off denormalized stuff
        movss   xmm7, _m128const_am_inv_mant_mask
        movss   xmm1, _m128const_am_1
        movss   [esp - 4], xmm0

        andps   xmm0, xmm7
        orps    xmm0, xmm1  // xmm1 == 1.0
        comiss  xmm0, log_rsqrt2
        movss   xmm7, xmm0

        jc      l_lt  // 'c' is 'lt' for comiss
//l_ge:
        xor     ecx, ecx
        movss   xmm2, xmm1  // xmm1 == 1.0
        jmp     l_continue

l_lt:
        mov     ecx, 1
        movss   xmm2, _m128const_am_0p5

l_continue:
        addss   xmm7, xmm2
        subss   xmm0, xmm2
        mov     edx, x
        rcpss   xmm7, xmm7
        mulss   xmm0, xmm7
        addss   xmm0, xmm0

        shr     edx, 23

        movss   xmm2, xmm0
        sub     edx, 0x7f
        mulss   xmm2, xmm2

        movss   xmm4, log_p0
        movss   xmm6, log_q0

        mulss   xmm4, xmm2
        movss   xmm5, log_p1
        sub     edx, ecx
        mulss   xmm6, xmm2
        movss   xmm7, log_q1

        addss   xmm4, xmm5
        addss   xmm6, xmm7

        movss   xmm5, log_p2
        mulss   xmm4, xmm2
        cvtsi2ss    xmm1, edx
        movss   xmm7, log_q2
        mulss   xmm6, xmm2

        addss   xmm4, xmm5
        addss   xmm6, xmm7

        movss   xmm5, log_c0
        mulss   xmm4, xmm2
        rcpss   xmm6, xmm6

        mulss   xmm4, xmm0
        mulss   xmm1, xmm5
        mulss   xmm4, xmm6

        addss   xmm0, xmm1
        addss   xmm0, xmm4

        movss   x, xmm0
    }
#endif	// SC_OPT
    return x;
#endif // !USE_C
}


//----------------------------------------------------------------------------
// log10
float fast_log10
(
    float x
)
//--------------------------------------
{
#if defined(USE_C)

    // 106 cycles
    return log10f(x);

#else
    // 74 cycles
	// fixed coefficient 7/3/2000 Shinji Chiba
	return fast_log( x ) * 0.4342945f;

#endif // !USE_C
}


//----------------------------------------------------------------------------
// exp
float fast_exp
(
    float x
)
//--------------------------------------
{
#if defined(USE_C)

    // 151 cycles
    return expf(x);

#else

    // 90 cycles
    __asm
    {
        movss   xmm0, x

        maxss   xmm0, fmin
        minss   xmm0, fmax

        movss   xmm1, rln2
        mulss   xmm1, xmm0
        movss   xmm7, _m128const_am_0
        addss   xmm1, _m128const_am_0p5
        xor     ecx, ecx

        mov     edx, 1
        comiss  xmm1, xmm7
        cvttss2si   eax, xmm1
        cmovc   ecx, edx    // 'c' is 'lt' for comiss
        sub     eax, ecx

        cvtsi2ss    xmm1, eax
        add     eax, 0x7f

        movss   xmm2, xmm1
        mulss   xmm1, c1
        and     eax, 0xff   // Optional, just for sanity
        mulss   xmm2, c2
        subss   xmm0, xmm1
        shl     eax, 23
        subss   xmm0, xmm2

        movss   xmm2, xmm0
        mov     x, eax
        mulss   xmm2, xmm2

        movss   xmm6, q0
        movss   xmm4, p0exp

        mulss   xmm6, xmm2
        movss   xmm7, q1
        mulss   xmm4, xmm2
        movss   xmm5, p1exp

        addss   xmm6, xmm7
        addss   xmm4, xmm5

        movss   xmm7, q2
        mulss   xmm6, xmm2
        mulss   xmm4, xmm2

        addss   xmm6, xmm7
        mulss   xmm4, xmm0

        movss   xmm7, q3
        mulss   xmm6, xmm2
        addss   xmm4, xmm0
        addss   xmm6, xmm7
        movss   xmm0, x

        subss   xmm6, xmm4
        rcpss   xmm6, xmm6
        movss   xmm7, _m128const_am_1
        mulss   xmm4, xmm6
        addss   xmm4, xmm4
        addss   xmm4, xmm7

        mulss   xmm0, xmm4

        movss   x, xmm0
    }

    return x;

#endif // !USE_C
}


float fast_sqrt( float x )
{
	_asm {
		mov		eax, x
		or		eax, eax
		jz		SQRTZERO
		movss	xmm0, x
		// approximate sqrt reciprocal -- |Max Error| <= 1.5x2^-12
		rsqrtss	xmm1, xmm0                  // 1/(x^.5)

		// this does the Newton-Raphson iteration to get up
		// to 22 bits of precision
		movss	xmm2, xmm0					// x
		mulss	xmm0, xmm1					// 9 * 1/sqr(9)
		mulss	xmm0, xmm1					// 9 * 1/sqr(9) * 1/sqr(9)
		mulss	xmm0, xmm1					// 9 * 1/sqr(9) * 1/sqr(9) * 1/sqr(9)
		mulss	xmm0, _m128const_am_0p5		// 1/2 * 9 * 1/sqr(9) * 1/sqr(9) * 1/sqr(9)
		mulss	xmm1, _m128const_am_1p5		// 3/2 * 1/sqr(9)
		subss	xmm1, xmm0					// 3/2 * 1/sqr(9) - 1/2 * 9 * 1/sqr(9) * 1/sqr(9) * 1/sqr(9)
		mulss	xmm1, xmm2					// x * 1/(x^.5)
		movss	x, xmm1
SQRTZERO:
	}
	return x;
}

//----------------------------------------------------------------------------
// sqrt
float fast_inversesqrt
(
    float x
)
//--------------------------------------
{
#if defined(USE_C)

    // 61 cycles
    return 1.0f / sqrtf(x);

#else
    // 35 cycles
	_asm {
		movss	xmm0, x
		rsqrtss	xmm1, xmm0
		mulss	xmm0, xmm1
		mulss	xmm0, xmm1
		mulss	xmm0, xmm1
		mulss	xmm0, _m128const_am_0p5
		mulss	xmm1, _m128const_am_3_over_2
		subss	xmm1, xmm0
		movss	x, xmm1
	}
	return x;

#endif // !USE_C
}


//----------------------------------------------------------------------------
// fabs
float fast_fabs
(
    float x
)
//--------------------------------------
{

#if defined(USE_C)

    // 7 cycles
    return fabsf(x);

#else

    // 6 cyles
    __asm
    {
        mov     eax, x          // starting with
        and     eax, 0x7fffffff // And out the sign bit
        mov     x, eax          // result in x
    }

    return x;

#endif // !USE_C

}


//----------------------------------------------------------------------------
// sincos computes both sin and cos simultaneously.
void fast_sincos
(
    float        x,
    SinCosPair*  v
)
//--------------------------------------
{

#if defined(USE_C)

    // 133 cycles
    v->fCos = fast_cos(x);
    v->fSin = fast_sin(x);

#else

    // 68 cycles
    __asm
    {
        movss   xmm0, x

        andps   xmm0, _m128const_am_inv_sign_mask
        mulss   xmm0, _m128const_am_2_over_pi

        mov     eax, x          // sin
        and     eax, 0x80000000 // sin

        movaps  xmm4, xmm0      // sin

        addss   xmm0, _m128const_am_1

        cvttss2si   ecx, xmm0
        cvttss2si   esi, xmm4   // sin

        mov     edx, ecx
        shl     edx, (31 - 1)

        mov     edi, esi        // sin
        shl     edi, (31 - 1)   // sin

        cvtsi2ss    xmm1, ecx
        and     edx, 0x80000000

        cvtsi2ss    xmm5, esi   // sin
        and     edi, 0x80000000 // sin

        and     ecx, 0x1
        subss   xmm0, xmm1

        jz      l_contcos
        movss   xmm1, _m128const_am_1
        subss   xmm1, xmm0
        movss   xmm0, xmm1
l_contcos:

        and     esi, 0x1        // sin
        subss   xmm4, xmm5      // sin

        jz      l_contsin       // sin
        movss   xmm5, _m128const_am_1      // sin
        subss   xmm5, xmm4      // sin
        movss   xmm4, xmm5      // sin
l_contsin:                      // sin

        mov     ecx, v

        movss   xmm1, xmm0
        mulss   xmm0, xmm0

        movss   xmm5, xmm4      // sin
        mulss   xmm4, xmm4      // sin

        mov     [ecx]v.fCos, edx
        movss   xmm2, xmm0
        mulss   xmm0, p3

        xor     eax, edi        // sin
        movss   xmm6, xmm4      // sin
        mulss   xmm4, p3        // sin

        addss   xmm0, p2

        mov     [ecx]v.fSin, eax    // sin
        addss   xmm4, p2         // sin

        mulss   xmm0, xmm2
        movss   xmm3, [ecx]v.fCos

        mulss   xmm4, xmm6      // sin
        movss   xmm7, [ecx]v.fSin  // sin

        addss   xmm0, p1
        addss   xmm4, p1        // sin

        mulss   xmm0, xmm2
        orps    xmm1, xmm3

        mulss   xmm4, xmm6      // sin
        orps    xmm5, xmm7      // sin

        addss   xmm0, p0
        addss   xmm4, p0        // sin

        mulss   xmm0, xmm1
        mulss   xmm4, xmm5      // sin

        movss   dword ptr [ecx]v.fCos, xmm0
        movss   dword ptr [ecx]v.fSin, xmm4
    }

#endif // !USE_C

}

//----------------------------------------------------------------------------
// sin
float fast_sin
(
    float x
)
//--------------------------------------
{

#if defined(USE_C)

    // 111 cycles
    return sinf(x);

#else

    // 62 cycles
    __asm
    {
        movss   xmm0, x

        andps   xmm0, _m128const_am_inv_sign_mask   // xmm0 = abs(x)
        mov     eax, x                              // eax = x
        mulss   xmm0, _m128const_am_2_over_pi       // xmm0 = abs(x) * 2/PI
        and     eax, 0x80000000                     // eax = sign(x)

        cvttss2si   ecx, xmm0                       // ecx = int(xmm0)
        mov     edx, ecx                            // edx = ecx
        shl     edx, (31 - 1)                       // edx = edx << 30
        cvtsi2ss    xmm1, ecx                       // xmm1 = ecx
        and     edx, 0x80000000                     // edx = sign(edx)

        and     ecx, 0x1                            // ecx = ecx & 0x1 (set ZF according to result)
        subss   xmm0, xmm1                          // xmm0 = xmm0 - xmm1

        jz      l_cont                              // jump if 0 / ZF = 1

        movss   xmm1, _m128const_am_1               // xmm1 = 1
        subss   xmm1, xmm0                          // xmm1 = xmm1 - xmm0
        movss   xmm0, xmm1                          // xmm0 = xmm1

l_cont:
        movss   xmm1, xmm0                          // xmm1 = xmm0
        mulss   xmm0, xmm0                          // xmm0 = xmm0 * xmm0
        xor     eax, edx                            // eax = edx | eax
        movss   xmm2, xmm0                          // xmm2 = xmm0
        mulss   xmm0, p3                            // xmm0 = xmm0 * p3
        mov     x, eax                              // x = eax
        addss   xmm0, p2                            // xmm0 = xmm0 + p2
        mulss   xmm0, xmm2                          // xmm0 = xmm0 * xmm2
        movss   xmm3, x                             // xmm3 = x
        addss   xmm0, p1                            // xmm0 = xmm0 + p1
        mulss   xmm0, xmm2                          // xmm0 = xmm0 * xmm2
        orps    xmm1, xmm3                          // xmm1 = xmm1 | xmm3
        addss   xmm0, p0                            // xmm0 = xmm0 + p0
        mulss   xmm0, xmm1                          // xmm0 = xmm0 * xmm1

        movss   x, xmm0
    }

    return x;

#endif // !USE_C

}


//----------------------------------------------------------------------------
// cos
float fast_cos
(
    float x
)
//--------------------------------------
{

#if defined(USE_C)

    // 97 cycles
    return cosf(x);

#else


    // 68 cycles
    __asm
    {
        movss   xmm0, x

        andps   xmm0, _m128const_am_inv_sign_mask   // abs(x)
        mulss   xmm0, _m128const_am_2_over_pi       // x * (2 / pi)
        addss   xmm0, _m128const_am_1               // x * (2 / pi) + 1

        cvttss2si   ecx, xmm0                       // Trancate into ecx.
        mov     edx, ecx                            // Store ecx.
        shl     edx, (31 - 1)                       // Shift left 30 bits.
        cvtsi2ss    xmm1, ecx                       // Store ecx into xmm1.
        and     edx, 0x80000000                     // Get sign bit.

        and     ecx, 0x1
        subss   xmm0, xmm1

        jz      l_cont
        movss   xmm1, _m128const_am_1
        subss   xmm1, xmm0
        movss   xmm0, xmm1
l_cont:
        movss   xmm1, xmm0
        mulss   xmm0, xmm0
        mov     x, edx
        movss   xmm2, xmm0
        mulss   xmm0, p3
        addss   xmm0, p2
        mulss   xmm0, xmm2
        movss   xmm3, x
        addss   xmm0, p1
        mulss   xmm0, xmm2
        orps    xmm1, xmm3
        addss   xmm0, p0
        mulss   xmm0, xmm1

        movss   x, xmm0
    }

    return x;

#endif // !USE_C
}


//----------------------------------------------------------------------------
// fast_atan
float fast_tan
(
    float x
)
//--------------------------------------
{
#if defined(USE_C)

    // 148 cycles
    return tanf(x);

#else

    // 75 cycles
    __asm
    {
        movss   xmm0, x

        andps   xmm0, _m128const_am_inv_sign_mask
        mulss   xmm0, _m128const_am_2_over_pi
        movss   xmm7, xmm0
        addss   xmm0, _m128const_am_1

        cvttss2si   ecx, xmm0
        mov     edx, ecx
        shl     edx, (31 - 1)
        cvtsi2ss    xmm1, ecx
        and     edx, 0x80000000

        and     ecx, 0x1
        subss   xmm0, xmm1

        jz      l_cont
        movss   xmm1, _m128const_am_1
        subss   xmm1, xmm0
        movss   xmm0, xmm1
l_cont:
        mov     eax, x
        and     eax, 0x80000000

        cvttss2si   ecx, xmm7
        xor     eax, edx
        mov     edx, ecx
        shl     edx, (31 - 1)
        cvtsi2ss    xmm3, ecx
        and     edx, 0x80000000

        and     ecx, 0x1
        subss   xmm7, xmm3

        jz      l_cont2
        movss   xmm3, _m128const_am_1
        subss   xmm3, xmm7
        movss   xmm7, xmm3
l_cont2:
        movss   xmm1, xmm0
        movss   xmm3, xmm7
        movss   xmm6, p3
        mulss   xmm0, xmm0
        mulss   xmm7, xmm7
        xor     eax, edx
        movss   xmm2, xmm0
        movss   xmm4, xmm7
        mulss   xmm0, xmm6
        mulss   xmm7, xmm6
        movss   xmm6, p2
        mov     x, eax
        addss   xmm0, xmm6
        addss   xmm7, xmm6
        mulss   xmm0, xmm2
        movss   xmm6, p1
        mulss   xmm7, xmm4
        movss   xmm5, x
        addss   xmm0, xmm6
        addss   xmm7, xmm6
        mulss   xmm0, xmm2
        mulss   xmm7, xmm4
        movss   xmm6, p0
        orps    xmm3, xmm5
        addss   xmm0, xmm6
        addss   xmm7, xmm6
        mulss   xmm0, xmm1
        mulss   xmm7, xmm3
        rcpss   xmm0, xmm0

        mulss   xmm0, xmm7

        movss   x, xmm0
    }

    return x;

#endif // !USE_C
}


//----------------------------------------------------------------------------
// pow
float fast_pow
(
    float x,
    float y
)
//--------------------------------------
{
#if defined(USE_C)

    // 303 cycles
    return powf(x, y);

#else

    // 133 cycles
    __asm
    {
        movss   xmm0, x
        movss   xmm1, y

        xorps   xmm7, xmm7
        comiss  xmm7, xmm0
        movss   xmm7, _m128const_am_inv_mant_mask
        maxss   xmm0, _m128const_am_min_pos_norm  // Cut off denormalized stuff.
        jnc     l_zerobase
        movss   xmm3, _m128const_am_1
        movss   x, xmm0

        andps   xmm0, xmm7
        orps    xmm0, xmm3  // xmm3 == 1.0
        comiss  xmm0, pow_rsqrt2
        movss   xmm7, xmm0

        jc      l_lt  // 'c' is 'lt' for comiss
//l_ge:
        xor     ecx, ecx
        movss   xmm2, xmm3  // xmm3 == 1.0
        jmp     l_continue

l_lt:
        mov     ecx, 1
        movss   xmm2, _m128const_am_0p5

l_continue:
        addss   xmm7, xmm2
        subss   xmm0, xmm2
        mov     edx, x
        rcpss   xmm7, xmm7
        mulss   xmm0, xmm7
        addss   xmm0, xmm0

        shr     edx, 23

        movss   xmm4, pow_p0
        movss   xmm6, pow_q0

        sub     edx, 0x7f
        movss   xmm2, xmm0
        mulss   xmm2, xmm2

        mulss   xmm4, xmm2
        sub     edx, ecx
        movss   xmm5, pow_p1
        mulss   xmm6, xmm2
        cvtsi2ss    xmm3, edx
        movss   xmm7, pow_q1

        addss   xmm4, xmm5
        mulss   xmm3, xmm1
        addss   xmm6, xmm7

        movss   xmm5, pow_p2
        mulss   xmm4, xmm2
        movss   xmm7, pow_q2
        mulss   xmm6, xmm2

        addss   xmm4, xmm5
        mulss   xmm1, pow_c0
        addss   xmm6, xmm7

        mulss   xmm4, xmm2
        rcpss   xmm6, xmm6

        mulss   xmm6, xmm0
        movss   xmm5, _m128const_am_0p5
        mulss   xmm4, xmm6
        addss   xmm0, xmm4
        xorps   xmm7, xmm7
        mulss   xmm0, xmm1

        addss   xmm0, xmm3

        maxss   xmm0, pow_fmin
        minss   xmm0, pow_fmax

        xor     ecx, ecx
        addss   xmm5, xmm0

        mov     edx, 1
        comiss  xmm5, xmm7
        cvttss2si   eax, xmm5
        cmovc   ecx, edx  // 'c' is 'lt' for comiss
        sub     eax, ecx

        cvtsi2ss    xmm5, eax
        add     eax, 0x7f

        subss   xmm0, xmm5

        movss   xmm2, xmm0
        mulss   xmm2, xmm2

        movss   xmm6, pow_s0
        movss   xmm4, pow_r0

        mulss   xmm6, xmm2
        movss   xmm7, pow_s1
        and     eax, 0xff  // Optional, just for sanity
        mulss   xmm4, xmm2
        movss   xmm5, pow_r1

        shl     eax, 23
        addss   xmm6, xmm7
        addss   xmm4, xmm5

        movss   xmm5, pow_r2
        mulss   xmm4, xmm2

        addss   xmm4, xmm5

        mulss   xmm4, xmm0

        mov     x, eax
        subss   xmm6, xmm4
        movss   xmm7, _m128const_am_1
        rcpss   xmm6, xmm6
        mulss   xmm4, xmm6
        movss   xmm0, x
        addss   xmm4, xmm4
        addss   xmm4, xmm7

        mulss   xmm0, xmm4

        jmp l_done

l_zerobase:
        xorps   xmm0, xmm0

l_done:
        movss   x, xmm0
    }

    return x;

#endif // !USE_C
}

float fast_hypot( float x, float y )
{
#if defined(USE_C)
	return (float) _hypot( x, y );
#else
	// 15.38x faster
	_asm {
		movss		xmm0, x
		movss		xmm1, y
		mulss		xmm0, xmm0
		mulss		xmm1, xmm1
		addss		xmm0, xmm1
		rsqrtss		xmm1, xmm0
		movss		xmm2, xmm0
		mulss		xmm0, xmm1
		mulss		xmm0, xmm1
		mulss		xmm0, xmm1
		mulss		xmm0, _m128const_am_0p5
		mulss		xmm1, _m128const_am_3_over_2
		subss		xmm1, xmm0
		mulss		xmm1, xmm2
		movss		x, xmm1
	}
	return x;
#endif // !USE_C
}

float fast_ceil( float x )
{
#if defined(USE_C)
	return ceilf( x );
#else
	// 2.01x faster
	unsigned long m32;
	_asm {
		movss		xmm0, x
		stmxcsr		m32
		mov			edx, m32
		mov			eax, 0ffff9fffh
		and			eax, edx
		or			eax, 4000h
		mov			m32, eax
		ldmxcsr		m32
		cvtss2si	eax, xmm0
		cvtsi2ss	xmm0, eax
		mov			m32, edx
		ldmxcsr		m32
		movss		x, xmm0
	}
	return x;
#endif // !USE_C
}

float fast_floor( float x )
{
#if defined(USE_C)
	return floorf( x );
#else
	// 1.99x faster
	unsigned long m32;
	_asm {
		movss		xmm0, x
		stmxcsr		m32
		mov			edx, m32
		mov			eax, 0ffff9fffh
		and			eax, edx
		or			eax, 2000h
		mov			m32, eax
		ldmxcsr		m32
		cvtss2si	eax, xmm0
		cvtsi2ss	xmm0, eax
		mov			m32, edx
		ldmxcsr		m32
		movss		x, xmm0
	}
	return x;
#endif // !USE_C
}

float fast_tanh( float x )
{
#if defined(USE_C)
	return tanhf( x );
#else
	// 3.26x faster
	_asm {
		mov			eax, x
		cmp			eax, 0ba83126fh
		ja			rettanhm
		cmp			eax, 3a83126fh
		ja			rettanhp

		movss		xmm1, x
		movss		xmm0, th1
		mulss		xmm1, xmm1
		mulss		xmm1, th3
		subss		xmm0, xmm1
		mulss		xmm0, x
		movss		x, xmm0
	}
	return x;
	_asm {
		ALIGN 16
rettanhm:
		movss		xmm0, x
		mulss		xmm0, th2p
		maxss		xmm0, fmin
		minss		xmm0, fmax
		movss		xmm1, rln2
		mulss		xmm1, xmm0
		movss		xmm6, _m128const_am_0
		addss		xmm1, _m128const_am_0p5
		xor			ecx, ecx
		mov			edx, 1
		comiss		xmm1, xmm6
		cvttss2si	eax, xmm1
		cmovc		ecx, edx
		sub			eax, ecx
		cvtsi2ss	xmm1, eax
		add			eax, 7fh
		movss		xmm2, xmm1
		mulss		xmm1, c1
		and			eax, 0ffh
		mulss		xmm2, c2
		subss		xmm0, xmm1
		shl			eax, 23
		subss		xmm0, xmm2
		movss		xmm2, xmm0
		mov			x, eax
		mulss		xmm2, xmm2
		movss		xmm5, q0
		movss		xmm3, p0exp
		mulss		xmm5, xmm2
		movss		xmm6, q1
		mulss		xmm3, xmm2
		movss		xmm4, p1exp
		addss		xmm5, xmm6
		addss		xmm3, xmm4
		movss		xmm6, q2
		mulss		xmm5, xmm2
		mulss		xmm3, xmm2
		addss		xmm5, xmm6
		mulss		xmm3, xmm0
		movss		xmm6, q3
		mulss		xmm5, xmm2
		addss		xmm3, xmm0
		addss		xmm5, xmm6
		movss		xmm0, x
		subss		xmm5, xmm3
		rcpss		xmm5, xmm5
		movss		xmm6, _m128const_am_1
		mulss		xmm3, xmm5
		addss		xmm3, xmm3
		addss		xmm3, xmm6
		mulss		xmm0, xmm3
		addss		xmm0, th1
		rcpss		xmm1, xmm0
		movss		xmm2, xmm1
		addss		xmm2, xmm2
		mulss		xmm1, xmm1
		mulss		xmm1, xmm0
		subss		xmm2, xmm1
		mulss		xmm2, th2p
		movss		xmm1, th1
		subss		xmm1, xmm2
		movss		x, xmm1
	}
	return x;
	_asm {
		ALIGN 16
rettanhp:
		movss		xmm0, x
		mulss		xmm0, th2m
		maxss		xmm0, fmin
		minss		xmm0, fmax
		movss		xmm1, rln2
		mulss		xmm1, xmm0
		movss		xmm6, _m128const_am_0
		addss		xmm1, _m128const_am_0p5
		xor			ecx, ecx
		mov			edx, 1
		comiss		xmm1, xmm6
		cvttss2si	eax, xmm1
		cmovc		ecx, edx
		sub			eax, ecx
		cvtsi2ss	xmm1, eax
		add			eax, 7fh
		movss		xmm2, xmm1
		mulss		xmm1, c1
		and			eax, 0ffh
		mulss		xmm2, c2
		subss		xmm0, xmm1
		shl			eax, 23
		subss		xmm0, xmm2
		movss		xmm2, xmm0
		mov			x, eax
		mulss		xmm2, xmm2
		movss		xmm5, q0
		movss		xmm3, p0exp
		mulss		xmm5, xmm2
		movss		xmm6, q1
		mulss		xmm3, xmm2
		movss		xmm4, p1exp
		addss		xmm5, xmm6
		addss		xmm3, xmm4
		movss		xmm6, q2
		mulss		xmm5, xmm2
		mulss		xmm3, xmm2
		addss		xmm5, xmm6
		mulss		xmm3, xmm0
		movss		xmm6, q3
		mulss		xmm5, xmm2
		addss		xmm3, xmm0
		addss		xmm5, xmm6
		movss		xmm0, x
		subss		xmm5, xmm3
		rcpss		xmm5, xmm5
		movss		xmm6, _m128const_am_1
		mulss		xmm3, xmm5
		addss		xmm3, xmm3
		addss		xmm3, xmm6
		mulss		xmm0, xmm3
		addss		xmm0, th1
		rcpss		xmm1, xmm0
		movss		xmm2, xmm1
		addss		xmm2, xmm2
		mulss		xmm1, xmm1
		mulss		xmm1, xmm0
		subss		xmm2, xmm1
		mulss		xmm2, th2p
		subss		xmm2, th1
		movss		x, xmm2
	}
	return x;
#endif // !USE_C
}

float fast_cosh( float x )
{
#if defined(USE_C)
	return coshf( x );
#else
	// 3.96x faster

	_asm {
		movss		xmm0, x
		maxss		xmm0, fmin
		minss		xmm0, fmax
		movss		xmm1, rln2
		mulss		xmm1, xmm0
		movss		xmm6, _m128const_am_0
		addss		xmm1, _m128const_am_0p5
		xor			ecx, ecx
		mov			edx, 1
		comiss		xmm1, xmm6
		cvttss2si	eax, xmm1
		cmovc		ecx, edx
		sub			eax, ecx
		cvtsi2ss	xmm1, eax
		add			eax, 7fh
		movss		xmm2, xmm1
		mulss		xmm1, c1
		and			eax, 0ffh
		mulss		xmm2, c2
		subss		xmm0, xmm1
		shl			eax, 23
		subss		xmm0, xmm2
		movss		xmm2, xmm0
		mov			x, eax
		mulss		xmm2, xmm2
		movss		xmm5, q0
		movss		xmm3, p0exp
		mulss		xmm5, xmm2
		movss		xmm6, q1
		mulss		xmm3, xmm2
		movss		xmm4, p1exp
		addss		xmm5, xmm6
		addss		xmm3, xmm4
		movss		xmm6, q2
		mulss		xmm5, xmm2
		mulss		xmm3, xmm2
		addss		xmm5, xmm6
		mulss		xmm3, xmm0
		movss		xmm6, q3
		mulss		xmm5, xmm2
		addss		xmm3, xmm0
		addss		xmm5, xmm6
		movss		xmm0, x
		subss		xmm5, xmm3
		rcpss		xmm5, xmm5
		movss		xmm6, _m128const_am_1
		mulss		xmm3, xmm5
		addss		xmm3, xmm3
		addss		xmm3, xmm6
		mulss		xmm0, xmm3

		rcpss		xmm1, xmm0
		movss		xmm2, xmm1
		addss		xmm2, xmm2
		mulss		xmm1, xmm1
		mulss		xmm1, xmm0
		subss		xmm2, xmm1
		addss		xmm0, xmm2
		mulss		xmm0, sh5
		movss		x, xmm0
	}
	return x;
#endif // !USE_C
}

float fast_sinh( float x )
{
#if defined(USE_C)
	return sinhf( x );
#else
	// 3.30x faster

	_asm {
		mov			eax, x
		and			eax, 7fffffffh
		cmp			eax, 3a83126fh
		jg			culcsinh
		movss		xmm0, x
		mulss		xmm0, xmm0
		mulss		xmm0, sh6
		addss		xmm0, sh1
		mulss		xmm0, x
		movss		x, xmm0
	}
	return x;
	_asm {
		ALIGN 16
culcsinh:
		movss		xmm0, x
		maxss		xmm0, fmin
		minss		xmm0, fmax
		movss		xmm1, rln2
		mulss		xmm1, xmm0
		movss		xmm6, _m128const_am_0
		addss		xmm1, _m128const_am_0p5
		xor			ecx, ecx
		mov			edx, 1
		comiss		xmm1, xmm6
		cvttss2si	eax, xmm1
		cmovc		ecx, edx
		sub			eax, ecx
		cvtsi2ss	xmm1, eax
		add			eax, 7fh
		movss		xmm2, xmm1
		mulss		xmm1, c1
		and			eax, 0ffh
		mulss		xmm2, c2
		subss		xmm0, xmm1
		shl			eax, 23
		subss		xmm0, xmm2
		movss		xmm2, xmm0
		mov			x, eax
		mulss		xmm2, xmm2
		movss		xmm5, q0
		movss		xmm3, p0exp
		mulss		xmm5, xmm2
		movss		xmm6, q1
		mulss		xmm3, xmm2
		movss		xmm4, p1exp
		addss		xmm5, xmm6
		addss		xmm3, xmm4
		movss		xmm6, q2
		mulss		xmm5, xmm2
		mulss		xmm3, xmm2
		addss		xmm5, xmm6
		mulss		xmm3, xmm0
		movss		xmm6, q3
		mulss		xmm5, xmm2
		addss		xmm3, xmm0
		addss		xmm5, xmm6
		movss		xmm0, x
		subss		xmm5, xmm3
		rcpss		xmm5, xmm5
		movss		xmm6, _m128const_am_1
		mulss		xmm3, xmm5
		addss		xmm3, xmm3
		addss		xmm3, xmm6
		mulss		xmm0, xmm3

		rcpss		xmm1, xmm0
		movss		xmm2, xmm1
		addss		xmm2, xmm2
		mulss		xmm1, xmm1
		mulss		xmm1, xmm0
		subss		xmm2, xmm1
		subss		xmm0, xmm2
		mulss		xmm0, sh5
		movss		x, xmm0
	}
	return x;
#endif // !USE_C
}
