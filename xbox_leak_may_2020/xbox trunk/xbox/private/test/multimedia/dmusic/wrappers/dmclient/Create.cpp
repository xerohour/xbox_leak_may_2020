#include "DMClient.h"


/******************************************************************************************
******************************************************************************************/
HRESULT CreateIDirectMusicPerformance8(CtIDirectMusicPerformance8 **pptPerformance8)
{
HRESULT hr = S_OK;

CtIDirectMusicPerformance8 *ptPerformance8 = NULL;
ptPerformance8 = new CtIDirectMusicPerformance8;
if (!ptPerformance8)
    hr = E_FAIL;


//Init our test class.
ptPerformance8->InitTestClass();

*pptPerformance8 = ptPerformance8;
return hr;
};



HRESULT CreateIDirectMusicLoader8(CtIDirectMusicLoader8 **pptLoader8)
{
HRESULT hr = S_OK;

CtIDirectMusicLoader8 *ptLoader8 = NULL;
ptLoader8 = new CtIDirectMusicLoader8;
if (!ptLoader8)
    hr = E_FAIL;


//Init our test class.
ptLoader8->InitTestClass();

*pptLoader8 = ptLoader8;
return hr;
};


/******************************************************************************************
******************************************************************************************/
HRESULT CreateIDirectMusicSegment8(CtIDirectMusicSegment8 **pptSegment8)
{
HRESULT hr = S_OK;

CtIDirectMusicSegment8 *ptSegment8 = NULL;
ptSegment8 = new CtIDirectMusicSegment8;
if (!ptSegment8)
    hr = E_FAIL;


//Init our test class.
ptSegment8->InitTestClass();

*pptSegment8 = ptSegment8;
return hr;
};



/******************************************************************************************
This is the same as the above function, except it creates the wrapper segment based on 
an XBox pointer.  This is needed when an XBox pointer is returned.
******************************************************************************************/
HRESULT CreateIDirectMusicSegment8(LPVOID pvXBoxSegment8, CtIDirectMusicSegment8 **pptSegment8)
{
HRESULT hr = S_OK;

CtIDirectMusicSegment8 *ptSegment8 = NULL;
ptSegment8 = new CtIDirectMusicSegment8;
if (!ptSegment8)
    hr = E_FAIL;


//Init our test class.
ptSegment8->InitTestClass(pvXBoxSegment8);

*pptSegment8 = ptSegment8;
return hr;
};







/*
void __stdcall Hello(void)
{
HRESULT hr = S_OK;
};

void _stdcall Hello(void)
{
HRESULT hr = S_OK;
};
*/

void Hello(void)
{
HRESULT hr = S_OK;
};

