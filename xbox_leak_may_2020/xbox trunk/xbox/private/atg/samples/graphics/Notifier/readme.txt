//-----------------------------------------------------------------------------
// Name: Notifier Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   This sample illustrates usage of the following API's:
      SetVerticalBlankCallback(D3DCALLBACK pCallback);
      BlockUntilVerticalBlank();
      InsertFence();
      BlockOnFence(DWORD dwFence);
      InsertCallback(D3DCALLBACKTYPE Type, D3DCALLBACK pCallback, DWORD dwContext);


Programming Notes
=================
   The Fence and InsertCallback API's can be used to determine if a resource
   is in use by the GPU. They can also be used to determine when some
   geometry has been completely processed by the pipeline.