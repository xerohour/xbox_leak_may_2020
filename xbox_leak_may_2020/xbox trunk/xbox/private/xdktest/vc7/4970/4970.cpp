#include <xtl.h>
#include <stdio.h>

float DivideBy27(float x)
{
    return(x/27.0f);
}

void __cdecl main()
{
	char szTemp[1024];

	float foo = 1.0f;
	for (int i = 0; i < 4; i++)
		foo = DivideBy27(foo);

    sprintf(szTemp, "%.12e\n", foo);
	OutputDebugString(szTemp);

	// debug produces       1.881676325866e-006
	// release /O2 produces 1.881676423159e-006
	// release /O1 produces 1.881676479237e-006

	while (1)
		;
} 
