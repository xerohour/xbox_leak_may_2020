//-----------------------------------------------------------------------------
// Name: Tut01_CreateDevice Xbox Graphics Tutorial
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The CreateDevice tutorial shows simply how to create a Direct3DDevice. There
   is no worthy visual output, since focus is simply on the creation.


Required files and media
========================
   This tutorial requires no media to run.


Programming Notes
=================
   The focus of this tutorial is the D3DPRESENT_PARAMETERS structure, and the
   call to CreateDevice(). One a device is created, a render loop looks like:
      while(1)
      {
         pd3dDevice->Clear(...)
         pd3dDevice->BeginScene()
         RenderEntireScene();
         pd3dDevice->EndScene()
         pd3dDevice->Present(...)
      }
