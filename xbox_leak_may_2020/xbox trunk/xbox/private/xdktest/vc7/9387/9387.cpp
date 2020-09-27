/*

The following code prints [10.000000,10.000000,25.000000] instead of
[10.000000,25.000000,50.000000] when global optimizations and frame-pointer
omission are turned on (/Og and /Oy).  I don't have that much detailed
knowledge of calling conventions, so this might be by design.  However,
it seems wrong to me because:

1) 5, 25, and 55 are loaded up before the function call and those values
   are correctly changed to 10, 25, 50 after the function call leading me
   to believe that the function itself is OK.
2) The MSDN docs say "Frame pointer optimization (the /Oy compiler option)
   is not recommended, but it is automatically suppressed for a naked function."


It looks like the variables in the D3VECTOR struct are referenced out of
alignment after the function call returns.  

*/

#include <stdio.h>
#include <xtl.h> // needed for OutputDebugString -- probably would be windows.h for non-Xbox code

typedef struct {
    float x;
    float y;
    float z;
} D3VECTOR;

_declspec(naked) void __fastcall clampVectorSSEUnaligned(D3VECTOR *value, float *min, float *max)
// EAX  : Address of max..
// ECX	: Address of value (D3VECTOR - unaligned)...
// EDX  : Address of min...
// XMM0 : Contents of value (post loading - N/A, Z, Y, X)...
// XMM1 : Contents of min...
// XMM2 : Contents of max...
// XMM3 : NOT USED
// XMM4 : NOT USED
// XMM5 : NOT USED
// XMM6 : NOT USED
// XMM7 : NOT USED
{
	_asm
	{
		MOVSS	XMM0, DWORD PTR [ECX]			// Store parameter 'value (x)' in XMM0 (N/A, N/A, N/A, X)..
		SHUFPS	XMM0, XMM0, 39H					// Rotate the register (X, N/A, N/A, N/A)...
		MOVSS	XMM1, DWORD PTR [ECX + 4]		// Store parameter 'value (y)' in XMM0 (X, N/A, N/A, Y)...
		MOVSS	XMM0, XMM1
		SHUFPS	XMM0, XMM0, 39H					// Rotate the register (Y, X, N/A, N/A)...
		MOVSS	XMM1, DWORD PTR [ECX + 8]		// Store parameter 'value (z)' in XMM0 (Y, X, N/A, Z)...
		MOVSS	XMM0, XMM1
		SHUFPS	XMM0, XMM0, 4EH					// Shuffle the register (N/A, Z, Y, X)...

		MOVSS	XMM1, DWORD PTR [EDX]			// Store parameter 'min' in XMM0 (N/A, N/A, N/A, min)..
		SHUFPS	XMM1, XMM1, 00H					// Broadcast the value to the register (min, min, min, min)...
		MOVSS	XMM2, DWORD PTR [EAX]			// Store parameter 'max' in XMM0 (N/A, N/A, N/A, max)....
		SHUFPS	XMM2, XMM2, 00H					// Broadcast the value to the register (max, max, max, max)...

		MAXPS	XMM0, XMM1
		// Clamp min values...
		MINPS	XMM0, XMM2
		// Clamp max values...

		MOVSS	DWORD PTR [ECX], XMM0			// Save the results X -> [ECX]...
		SHUFPS	XMM0, XMM0, 39H					// Rotate the register (X, N/A, Z, Y)...
		MOVSS	DWORD PTR [ECX + 4], XMM0		// Save the results Y -> [ECX + 4]...
		SHUFPS	XMM0, XMM0, 39H					// Rotate the register (Y, X, N/A, Z)...
		MOVSS	DWORD PTR [ECX + 8], XMM0		// Save the results Z -> [ECX + 8]...
	}

	// Return (C 'return;' is not allowed)...
	_asm	RET
}

#pragma optimize ( "g", on )

#pragma optimize ( "y", off )

void ShowExpectedResults()
{
	D3VECTOR v;
	v.x = 5.0f;
	v.y = 25.0f;
	v.z = 55.0f;

	float fMin = 10.0f;
	float fMax = 50.0f;
	clampVectorSSEUnaligned(&v, &fMin, &fMax);

	char buf[100] = "";
	sprintf(buf, "Expected output is [%hf,%hf,%hf]\n", v.x, v.y, v.z);
	OutputDebugStringA(buf);
}

#pragma optimize ( "y", on )

void ShowBuggyResults()
{
	D3VECTOR v;
	v.x = 5.0f;
	v.y = 25.0f;
	v.z = 55.0f;

	float fMin = 10.0f;
	float fMax = 50.0f;
	clampVectorSSEUnaligned(&v, &fMin, &fMax);

	char buf[100] = "";
	sprintf(buf, "Actual output is [%hf,%hf,%hf]\n", v.x, v.y, v.z);
	OutputDebugStringA(buf);
}

#pragma optimize ( "", on )

void __cdecl main()
{
	ShowExpectedResults();
	ShowBuggyResults();
	while (1)
		Sleep(500);
}

