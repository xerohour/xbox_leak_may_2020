#if defined(_WIN64)

#include "cmntypes.h"
#include "gtfmath.h"

/*
;==============================================================================
;
; Function:     ComputeGTFParameters
;
; Purpose:      This function computes the data for the GTFOut structure
;               from the arguments in the GTFIn structure provided
;               according to the GTF formula.
;
;               This is currently just stubbed out for Win64.
;
; Returns:      0       failure and lpGTFOut was not filled in
;
;==============================================================================
*/
ULONG WINAPI ComputeGTFParameters
(
        LPGTFIN  lpGtfIn,
        LPGTFOUT lpGtfOut
)
{
    return(0);
}
ULONG WINAPI Divide32By32
(
        ULONG dwDividend,
        ULONG dwDivisor
)
{
    return (dwDividend / dwDivisor);
}
ULONG WINAPI Multiply32By32
(
        ULONG dwMult1,
        ULONG dwMult2
)
{
    return (dwMult1 * dwMult2);
}
#endif // _WIN64
