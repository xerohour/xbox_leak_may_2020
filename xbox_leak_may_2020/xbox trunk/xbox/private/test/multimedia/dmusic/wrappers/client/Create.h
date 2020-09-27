#pragma once

#include "macros.h"
#include "ctperf8.h"
#include "ctseg8.h"

/******************************************************************************************
******************************************************************************************/
template <class T>
CreateInstance(T **pptObject)
{
    HRESULT hr = S_OK;
    T* ptObject = NULL;

    ptObject = new T;
    if (!ptObject)
        hr = E_FAIL;

    //Init our test class.
    ptObject->InitTestClass();
    if (FAILED(hr))
        SAFEDELETE(ptObject);

    *pptObject = ptObject;
    return hr;
};

template <class T>
CreateInstance(LPVOID pObject, T **pptObject)
{
    HRESULT hr = S_OK;
    T* ptObject = NULL;

    ptObject = new T;
    if (!ptObject)
        hr = E_FAIL;

    //Init our test class.
    hr = ptObject->InitTestClass(pObject);
    if (FAILED(hr))
        SAFEDELETE(ptObject);

    *pptObject = ptObject;
    return hr;
}


/*
HRESULT CreateIDirectMusicPerformance8(CtIDirectMusicPerformance8 **pptPerformance8);
HRESULT CreateIDirectMusicSegment8(LPVOID pvXBoxSegment8, CtIDirectMusicSegment8 **pptSegment8);
HRESULT CreateIDirectMusicSegment8(CtIDirectMusicSegment8 **pptSegment8);
*/
