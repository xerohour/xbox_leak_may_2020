#pragma once

#include "globals.h"
#include "Help_ComCounter.h"

HRESULT CreateComCounter(CComCounter **ppComCounter)
{
CComCounter *pComCounter = NULL;
pComCounter = new CComCounter();
    if (pComCounter)
    {
        *ppComCounter= pComCounter;
        return S_OK;
    }
    else
    {
        return E_OUTOFMEMORY;
    }
}