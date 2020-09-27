#pragma once

#ifndef countof
#define countof(a) (sizeof(a) / sizeof((a)[0]))
#endif

//random functions: between min & max.
//for FRND, [min, max).
#define FRND(min, max) ((((float)rand() / (float)RAND_MAX) * (fabs((max) - (min)))) + (min))
//for RND, [min, max].
#define RND(min, max) ((rand() % (abs((max) - (min)) + 1)) + (min))
//for RNDE, [min, max).
#define RNDE(min, max) (((rand() * abs((max) - (min)) / RAND_MAX)) + (min))
//for [False,True]
#define RNDTF() ((rand() & 1) ? true : false)

namespace XGRAPHICS {
	//a handy auto-resizing buffer class, including "printf", and other nifty features
	class Buffer {
		DWORD m_size;
		char* m_buf;
		DWORD m_used;
	public:
		Buffer();
		~Buffer();
		HRESULT Initialize(DWORD size);
		HRESULT GrowTo(DWORD size);
		HRESULT Printf(const char* msg,...);
		HRESULT Append(LPCVOID data, DWORD length);
		HRESULT Append(char c);
		HRESULT Append(const char* c);
		HRESULT Append(Buffer& buffer);
		char* GetText();
		char* GetTextEnd();
		DWORD GetUsed();
		void Clear();
	};
};
typedef XGRAPHICS::Buffer XBuffer;


//the three expected return values from a LIPOTEST.
enum LIPOTESTRETURN {
	ltrTestFailed, ltrTestPassed, ltrInvalid
};

//LIPOTEST is a callback function for LipoVsh. It should perform a test on the shader,
//  and return ltrTestFailed if LipoVsh should accept the change it just made to the shader
//  or ltrTestPassed if it should reject the change...
//  or ltrInvalid if the change it made made the shader not assemblable (or something similar.)
//In most cases, TestPassed and Invalid will result in the same behavior, except for the first time
//  the test is run.
//An example of its usage: An app would create a vertex shader, and call LipoVsh on it, passing 
//  a test which would try to assemble the shader, and return ltrTestFailed if the shader optimizer
//  broke the resulting shader. IT would return ltrTestPassed if the shader optimizer succeeded, or
//  ltrInvalid if the shader couldn't be assembled for other reasons.
//  LipoVsh would then shrink the test down (retesting it after every shrinkage) in order to get the 
//  smallest shader possible that still breaks the optimizer. The app would then save that shader to disk,
//  and if it was really an impressive app, would send mail to the developer in charge of the optimizer, 
//  and attach the file that broke it. :)
typedef LIPOTESTRETURN (*LIPOTEST)(XBuffer* pText, void* pData); 

/*
The smallest shader that still repro's (the test returns ltrTestFailed) will be returned in 
  a new buffer, which will be pointed to by *ppOutput.

Return: 
	if a parameter is NULL (other than pData), return E_POINTER
	if it runs out of memory, it returns E_OUTOFMEMORY
	if the original shader isn't valid, return E_INVALIDARG
	if the original shader doesn't repro (if the test returns ltrTestPassed), it returns S_FALSE.
	if it successfully shrinks the shader, it returns S_OK
	if something internally bad happens, it returns S_UNEXPECTED
*/
HRESULT LipoVsh(
	XBuffer*  pInput,    //input buffer (constant)
	XBuffer** ppOutput,  //pointer to receive smallest output
	LIPOTEST pfnTest,   //pointer to function to test the lipo'd shader
	void*    pData,     //pointer to data to pass to the test function
	bool     bNSquared, //true to try removing 2 lines at a time
	bool     bTryAgain  //true to keep trying to repeat if it succeeds the first time
);



/*
Create a random vertex shader, of length randomly selected between minlength and maxlength, inclusive.
It will start the shader with the specified number of NOPs, which can prevent some of the randomness that occurs in the first few cycles of the shader
dwType can be either hardware format (0x7378, etc), or SASM-format (SASMT_VERTEXSTATESHADER, etc.).
if bWriteConsts is false, it will not write to constant registers
bIsXvs represents whether it is an xbox-specific shader (196 constants, two extra instructions)
bAllowPair, if true, will allow up to 3 instructions to be paired with the + operator
bScreenspace, if true, will add #pragma screenspace to the beginning of the shader
ppOutput should be the address of a XBuffer*. That will be set to point to a new xbuffer that will contain the new shader.
*/
HRESULT GenerateRandomVertexShader(
	int minlength, 
	int maxlength, 
	DWORD dwNopsAtBeginning,
	DWORD dwType, 
	bool bWriteConsts,
	bool bIsXvs,
	bool bAllowIndex,
	bool bAllowPair,
	bool bScreenspace,
	bool bAllowNop,
	XBuffer** ppOutput
);