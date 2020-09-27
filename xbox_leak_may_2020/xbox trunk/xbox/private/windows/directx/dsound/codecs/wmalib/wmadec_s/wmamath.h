#ifndef __WMAMATH_H__
#define __WMAMATH_H__

static _declspec(naked) long FloatToLong(float x)
{
    __asm
    {
        cvttss2si eax, [esp+4]
        ret 4
    }
}

#define ftoi(x) (int)FloatToLong(x)

#endif // __WMAMATH_H__

