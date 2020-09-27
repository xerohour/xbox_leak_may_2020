#include <xtl.h>

typedef unsigned int Value_Type;

struct Test
{
	static Test __fastcall Get_Value();
	Value_Type m_value;
};

void __fastcall Show_The_Bug()
{
	// begin bug-> (build Release for repro)

	Test a = Test::Get_Value();
	// lea ecx,[esp]
	// call 00400000

	Test b = Test::Get_Value();
	// lea ecx,[esp]            // both values are written into the same address
	// call 00400000

	// mov eax,dword ptr [esp]    // compares the value to itself
	// cmp eax,dword ptr [esp]
	// jne 0040003A
    
	// <- end bug

	if(a.m_value == b.m_value)
	{
		OutputDebugStringA("Bug\n");    // this happens in the release build
	}
	else
	{
		OutputDebugStringA("No bug\n");
	}
}

Value_Type some_value = 0;

__declspec( naked ) Test __fastcall Test::Get_Value()
{
    __asm
    {
        mov        eax, some_value
        inc        [some_value]
        mov        [ecx], eax
        mov        eax, ecx
        ret
    }
}

void __cdecl main()
{
    Show_The_Bug();

    while(true)
    {
    }
}

