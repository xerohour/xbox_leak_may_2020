//-----------------------------------------------------------------------------
// File: XBStrip.h
//
// Desc: Tristrip routines (which convert a mesh into a list of optimized
//       triangle strips).
//
// Hist: 02.01.01 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define OPTIMIZE_FOR_CACHE      0x00
#define OPTIMIZE_FOR_INDICES    0x01
#define OUTPUT_TRISTRIP         0x00
#define OUTPUT_TRILIST          0x02




//-----------------------------------------------------------------------------
// Name: Stripify()
// Desc: Main stripify routine. Stripifies a mesh and returns the number of 
//       strip indices contained in ppStripIndices.
// Note: Caller must make sure to call delete[] on the ppStripIndices array
//       when finished with it.
//-----------------------------------------------------------------------------
DWORD Stripify( DWORD  dwNumTriangles,   // Number of triangles
                WORD*  pTriangles,       // Ptr to triangle indices
                DWORD* pdwNumIndices,    // Number of output indices
                WORD** ppStripIndices,   // Output indices
                DWORD  dwFlags = 0 );    // Flags controlling optimizer.



//-----------------------------------------------------------------------------
// Name: ComputeVertexPermutation()
// Desc: Re-arrange vertices so that they occur in the order that they are
//       first used. Instead of actually moving vertex data around, this
//       function returns an array that specifies where (in the new vertex
//       array) each old vertex should go. It also re-maps the strip indices
//       to use the new vertex locations.
// Note: Caller must make sure to call delete[] on the pVertexPermutation array
//       when finished with it.
//-----------------------------------------------------------------------------
VOID ComputeVertexPermutation( DWORD  dwNumStripIndices,     // Number of strip indices
                               WORD*  pStripIndices,         // Ptr to strip indices
                               DWORD  dwNumVertices,         // Number of verticess in
                               WORD** ppVertexPermutation ); // Map from orignal index to remapped index
