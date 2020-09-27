//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       loadStuff.h
//
//--------------------------------------------------------------------------

#ifndef LOADSTUFF_H
#define LOADSTUFF_H

#include "wmc_type.h"

#if defined(_MSC_VER) || defined(LINUX_x86)
#define LITTLE_ENDIAN
#endif

#ifdef LITTLE_ENDIAN

#define GetUnalignedWord( pb, w )   (w) = *(UNALIGNED U16_WMC*)(pb); 

#define GetUnalignedDword( pb, dw ) (dw) = *(UNALIGNED U32_WMC*)(pb);

#define GetUnalignedQword( pb, qw ) (qw) = *(UNALIGNED U64_WMC*)(pb);

#define GetUnalignedDouble( pb, d ) (d) = *(UNALIGNED Double_WMC*)(pb);

#else

#define GetUnalignedWord( pb, w ) \
            (w) = ((U16_WMC) *(pb + 1) << 8) + *pb;

#define GetUnalignedDword( pb, dw ) \
            (dw) = ((U32_WMC) *(pb + 3) << 24) + \
                   ((U32_WMC) *(pb + 2) << 16) + \
                   ((U16_WMC) *(pb + 1) << 8) + *pb;
/*
#define GetUnalignedQword( pb, qw ) \
            GetUnalignedDword( pb, (qw).dwLo ); \
            GetUnalignedDword( (pb + 4), (qw).dwHi );
*/
#define GetUnalignedQword( pb, qw ) \
            GetUnalignedDword( pb, (*((U32_WMC *)(&(qw)))) ); \
            GetUnalignedDword( (pb + 4), (*(((U32_WMC *)(&(qw)))+1)) );

#define GetUnalignedDouble( pb, d ) (d) = *(UNALIGNED Double_WMC*)(pb);

#endif

#define GetUnalignedWordEx( pb, w )     GetUnalignedWord( pb, w ); (pb) += sizeof(U16_WMC);
#define GetUnalignedDwordEx( pb, dw )   GetUnalignedDword( pb, dw ); (pb) += sizeof(U32_WMC);
#define GetUnalignedQwordEx( pb, qw )   GetUnalignedQword( pb, qw ); (pb) += sizeof(U64_WMC);
#define GetUnalignedDoubleEx( pb, d )   GetUnalignedDouble( pb, d ); (pb) += sizeof(Double_WMC);

#define LoadBYTE( b, p )    b = *(U8_WMC *)p;  p += sizeof( U8_WMC )

#define LoadWORD( w, p )    GetUnalignedWordEx( p, w )
#define LoadDWORD( dw, p )  GetUnalignedDwordEx( p, dw )
#define LoadQWORD( qw, p )  GetUnalignedQwordEx( p, qw )

#define LoadGUID( g, p ) \
        { \
            LoadDWORD( g.Data1, p ); \
            LoadWORD( g.Data2, p ); \
            LoadWORD( g.Data3, p ); \
            LoadBYTE( g.Data4[0], p ); \
            LoadBYTE( g.Data4[1], p ); \
            LoadBYTE( g.Data4[2], p ); \
            LoadBYTE( g.Data4[3], p ); \
            LoadBYTE( g.Data4[4], p ); \
            LoadBYTE( g.Data4[5], p ); \
            LoadBYTE( g.Data4[6], p ); \
            LoadBYTE( g.Data4[7], p ); \
        }

#endif  // LOADSTUFF_H
