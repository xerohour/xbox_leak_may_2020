////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// File: XBTga.h
//
// Desc: Support class for rendering a splash image
//
//-----------------------------------------------------------------------------
#ifndef XBTGA_H
#define XBTGA_H
#include "XBFont.h"
#include "XBUtil.h"


//-----------------------------------------------------------------------------
// Name: class CXBTga
// Desc: Class for rendering a targa image
//-----------------------------------------------------------------------------
class CXBTga
{
    LPDIRECT3DDEVICE8  m_pd3dDevice;
    LPDIRECT3DTEXTURE8 m_pTgaTexture;
	RECT m_pos;

public:
    // Constructor
    CXBTga();

    // Functions to create and destroy the internal objects
    HRESULT Create( LPDIRECT3DDEVICE8 pd3dDevice, char* pFilename, RECT pos );
    HRESULT Destroy();

    // Renders the tga
    HRESULT Render();
};

#endif
