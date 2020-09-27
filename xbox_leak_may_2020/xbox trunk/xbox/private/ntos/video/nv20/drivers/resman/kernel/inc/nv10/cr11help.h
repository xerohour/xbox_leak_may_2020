//-------------------------------------------------------------

#include <cr11_ref.h>

//-------------------------------------------------------------
#define max(a,b)	(((a) > (b)) ? (a) : (b))
#define min(a,b)	(((a) < (b)) ? (a) : (b))
//-------------------------------------------------------------
// Macros for combining tiling information into Crush north bridge registers

#define WriteNBTBASE( _pitch, _offset, _size, _valid ) \
    ( \
          CR_DRF_NUM(_XL,_TBASE,_PITCH, DRF_VAL(_PGRAPH,_TSIZE,_PITCH,(_pitch))) \
        | CR_DRF_NUM(_XL,_TBASE,_REGION,_valid) \
        | CR_DRF_NUM(_XL,_TBASE,_ADR,   DRF_VAL(_PGRAPH,_TILE, _ADR, (_offset))) \
    )

#define WriteNBTLIMIT( _pitch, _offset, _size, _valid ) \
    ((_offset) + (_size) - 1)

//-------------------------------------------------------------
// Macros for extracting tiling information from Crush north bridge registers

#define ReadNBPitch( _tbase, _tlimit ) \
    DRF_NUM(_PGRAPH,_TSIZE,_PITCH,CR_DRF_VAL(_XL,_TBASE,_PITCH,(_tbase)))

#define ReadNBOffset( _tbase, _tlimit ) \
    DRF_NUM(_PGRAPH,_TILE, _ADR,CR_DRF_VAL(_XL,_TBASE,_ADR,(_tbase)))

#define ReadNBSize( _tbase, _tlimit ) \
    ((_tlimit) + 1 - ReadNBOffset(_tbase, _tlimit))

#define ReadNBValid( _tbase, _tlimit ) \
    (CR_DRF_VAL( _XL, _TBASE, _REGION, _tbase))

//-------------------------------------------------------------
// Macros for combining tiling information into NV1X _PFB & _PGRAPH registers

#define FormatPFBTile( _pitch, _offset, _size, _valid ) \
    ((_offset) | DRF_NUM(_PGRAPH, _TILE, _REGION, _valid))

#define FormatPFBTSize( _pitch, _offset, _size, _valid ) \
    (_pitch)

#define FormatPFBTLimit( _pitch, _offset, _size, _valid ) \
    ((_offset) + (_size) -1)

//-------------------------------------------------------------

//-------------------------------------------------------------
