// TxFactory.h - Create IVsTextImage, IVsTextSpanSet

#pragma once
//#include "textfind.h" // interfaces defined in textfind.idl

//================================================================
//
// CreateTextImage - Create an instance of IVsTextImage
//
// If pIPersistImage is non-NULL, the IVsTextImage uses that implementation
// to handle persistence and memory allocation.
//
// If pIPersistImage is NULL, it uses it's own implementation based on VSMem
//
HRESULT WINAPI 
CreateTextImage
(
    IVsTextImage **          ppImage, 
    IVsPersistentTextImage * pIPersistImage = NULL  // optional IVsPersistentTextImage
);


//================================================================
//
// CreateNoEditTextImage - Create a lightweight noneditable instance of IVsTextImage
//
// Uses VSMem to implement IVsPersistentTextImage
//
HRESULT WINAPI 
CreateNoEditTextImage 
(
    IVsTextImage ** ppImage
);


//================================================================
//
// CreateTextSpanSet - Create an instance of IVsTextSpanSet
//
HRESULT WINAPI 
CreateTextSpanSet
(
    IVsTextSpanSet ** ppSpanSet
);

