//-----------------------------------------------------------------------------
// Name: Tut02_Vertices Xbox Graphics Tutorial
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Vertices tutorial shows simply how to render vertices.


Required files and media
========================
   This tutorial requires no media to run.


Programming Notes
=================
   Vertices are stored inside a vertex buffer, which is rendered via the
   following suite of calls:
      pd3dDevice->SetStreamSource( 0, g_pVB, sizeof(CUSTOMVERTEX) );
      pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
      pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );
   Vertices can have a flexible format, at described by it's FVF code.
