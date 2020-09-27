//-----------------------------------------------------------------------------
// FILE: FILLRATE.CPP
//
// Desc: a stupid fillrate test
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <macros.h>
#include <stdio.h>
#include <assert.h>

#include <d3d8perf.h>

// don't use the VisibilityTest apis
// #define NO_VIS_TEST

#include "fillrate.h"
#include "Ini.h"


CFillRate::CFillRate()
{
	m_pDevice			= NULL; 
	m_pVB				= NULL;			// our vertex buffer
	m_pBackBuffer		= NULL;
	m_pIndices			= NULL;			// indices allocation
	m_hInpDevice		= NULL;			// input device

	for ( UINT i=0; i<FILLRATE_NUM_TEXTURES; i++ )
	{
		m_pTexture[i]	= NULL;
	}
	
	m_ShaderHandle		= 0;             // pixel shader handle
	m_dwFVFSize			= 0;             // FVF size
	m_cIndices			= 0;             // count of indices
	
	m_fRedrawScreen		= TRUE;			// refresh screen?
	m_fdumpZ			= FALSE;		// dump Z val

	m_wLoops			= GetPrivateProfileIntA( "FILLRATE", "Loops", 1, FILLRATE_INI_PATH );
	m_wLoop				= 0;
//	m_dwStartLoopTime	= 0;
	
	m_iCurTest			= 0;

	struct
	{
		LPCSTR szDesc;
		void (CFillRate::*pFunc)();
	} rgTests[] =
	{
		{ "Current",        DoQuickTimedTest },
		{ "MultiSample",    DoMultiSampleTimedTest },
		{ "FillRate",       DoCompleteTimedTest },
		{ "Texture Test",   DoTextureTest },
	};
	memcpy( &m_rgTests, rgTests, sizeof(m_rgTests) );
}

HRESULT CFillRate::InitGraphics()
{
	HRESULT hr = S_OK;

	if ( !InitFR( true ) )
	{
        return E_FAIL;
	}

//	m_dwStartLoopTime = timeGetTime();

	return hr;
}

HRESULT CFillRate::DestroyGraphics()
{
	HRESULT hr = S_OK;

	RELEASE( m_pVB );

	RELEASE( m_pBackBuffer );

	for ( UINT i=0; i<FILLRATE_NUM_TEXTURES; i++ )
	{
		RELEASE( m_pTexture[i] );
	}

	RELEASE( m_pDevice );

	return hr;
}


HRESULT CFillRate::DestroyInput()
{
	HRESULT hr = S_OK;

	if ( NULL != m_hInpDevice )
	{
		XInputClose( m_hInpDevice );
		m_hInpDevice = NULL;
	}

	return hr;
}


HRESULT CFillRate::ProcessInput()
{
	HRESULT hr = S_OK;

	DWORD dwDeviceMap;
	DWORD dwSuccess;
	BOOL bDeviceChanges;
	DWORD dwInsertions;
	DWORD dwRemovals;

	XINPUT_STATE xinpstate;

	bDeviceChanges = XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals );
	if ( bDeviceChanges )
	{
		if ( dwInsertions & XDEVICE_PORT0_MASK )
		{
			m_hInpDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL );
			if ( m_hInpDevice == NULL )
			{
				DbgPrint( "Error Opening Input\n" );
				return E_FAIL;
			}
		}
		
		if ( dwRemovals & XDEVICE_PORT0_MASK )
		{
			DbgPrint( "Gamepad removed from Port 0\n" );
			return E_FAIL;
		}
	}

	dwDeviceMap = XGetDevices( XDEVICE_TYPE_GAMEPAD );
	if ( !(dwDeviceMap & XDEVICE_PORT0_MASK) ) 
	{
		//DbgPrint( "No Device in Port 0\n" );
		return S_OK; // live with it!
	}
	else if ( NULL == m_hInpDevice )
	{
		m_hInpDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL );
		if ( m_hInpDevice == NULL )
		{
			DWORD dwError;
			dwError = GetLastError();
			DbgPrint( "Error Opening Input. Error#: %d\n", dwError);
			return E_FAIL;
		}
	}

	dwSuccess = XInputGetState( m_hInpDevice, &xinpstate );
	if ( dwSuccess != ERROR_SUCCESS )
	{
		DbgPrint( "Error Getting Input State\n" );
		return dwSuccess;
	}

	return hr;
}


HRESULT CFillRate::Update()
{
	HRESULT hr = S_OK;

	for ( m_iCurTest=0; m_iCurTest < FILLRATE_NUM_TESTS; m_iCurTest++ )
	{
		m_var.runtestonreload = 1;
		(this->*m_rgTests[m_iCurTest].pFunc)();
		// draw one frame with current settings
		RenderFrame(1);
		// flip that dude to the front
		CheckHR(m_pDevice->Present(NULL, NULL, NULL, NULL));
		// restore settings from ini file
		InitFR( FALSE );
		// dump any stats here
		// dump some settings
#ifdef DBG
		D3DPUSHBUFFERINFO PushBufferInfo;
		D3DPERF_GetPushBufferInfo(&PushBufferInfo);
		
		D3DPERF_Dump();
		D3DPERF_Reset();
#endif DBG
	}

	m_wLoop++;
	if ( m_wLoop >= m_wLoops )
	{
		m_bQuit = TRUE;
	}
	else
	{
		//EXECUTE( DestroyGraphics() ); //included in init
		EXECUTE( InitGraphics() );
		if ( FAILED( hr ) )
		{
			return hr;
		}
	}

	return hr;
}


HRESULT CFillRate::Render()
{
	HRESULT hr = S_OK;

#if 0
	if(m_fRedrawScreen)
	{
		// clear back buffer
		RenderFrame(1);
		
		// swap that guy to the front
		//CheckHR(m_pDevice->Present(NULL, NULL, NULL, NULL));
		
		// clear new back buffer
		RenderFrame(1);
		
		// display current options on front buffer
//		m_scrtext.cls();
//		DumpCurrentSettings();
//		m_scrtext.drawtext(m_var.verboselevel >= 1);
//		m_Font.DrawText( m_pBackBuffer, (wchar_t *)"TEST", 300, 200, 0, D3DCOLOR_XRGB( 255, 255, 255 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
		m_pDevice->Present( NULL, NULL, NULL, NULL );
		
		m_fRedrawScreen = false;
	}
	else
	{
		// 33.3 fps baby!
		Sleep(30);
		m_fRedrawScreen = true;
	}
#endif 0

	return hr;
}


//=========================================================================
// Initialize fillrate app
//=========================================================================
BOOL CFillRate::InitFR(BOOL freadINIFile)
{
	// make sure we're starting from scratch
	DestroyGraphics();

    if(freadINIFile && !ReadFRIniFile())
        dprintf("%s not found\n", FILLRATE_INI_PATH );

    // init d3d
    if(!InitD3D())
    {
        dprintf("InitD3D failed.\n");
        return false;
    }

    CheckHR(m_pDevice->SetRenderState(D3DRS_SWATHWIDTH, m_var.SwathWidth));
    CheckHR(m_pDevice->SetRenderState(D3DRS_FILLMODE, m_var.d3dfillmode));
    CheckHR(m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, m_var.colorwriteenable));

    CheckHR(m_pDevice->SetRenderState(D3DRS_ZENABLE, m_var.zenable));
    CheckHR(m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, m_var.zwriteenable));
    CheckHR(m_pDevice->SetRenderState(D3DRS_ZFUNC, m_var.d3dcmpfunc));

    CheckHR(m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, m_var.alphablendenable));
    CheckHR(m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
    CheckHR(m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

    CheckHR(m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, m_var.alphatestenable));
    CheckHR(m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS));
    CheckHR(m_pDevice->SetRenderState(D3DRS_ALPHAREF, 0x00000001));

    if(m_var.userpixelshader)
        CheckHR(m_pDevice->CreatePixelShader(&m_var.shaderdef, &m_ShaderHandle));

    for(int itex = 0; itex < 4; itex++)
    {
        RELEASE(m_pTexture[itex]);

        if(m_var.rgtex[itex].type)
        {
            assert(m_var.rgtex[itex].type < TEX_Max);
            m_var.rgtex[itex].type = min(m_var.rgtex[itex].type, TEX_Max - 1);

            static const D3DRESOURCETYPE rgrestype[] =
                { D3DRTYPE_TEXTURE, D3DRTYPE_CUBETEXTURE, D3DRTYPE_VOLUMETEXTURE };
            assert(TEX_2d == 1 && TEX_Cubemap == 2 && TEX_Volume == 3);

            m_pTexture[itex] = MyCreateTexture(
                rgrestype[m_var.rgtex[itex].type - 1],
                m_var.rgtex[itex].d3dFormat,
                m_var.rgtex[itex].texsize,
                m_var.rgtex[itex].texsize);

            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_MINFILTER, m_var.rgtex[itex].filtertype));
            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_MAGFILTER, m_var.rgtex[itex].filtertype));

            // Set the texture stage states appropriately
            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_COLORARG1, D3DTA_TEXTURE));
            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_COLOROP, m_var.ColorOp));
            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_COLORARG2, D3DTA_DIFFUSE));

            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_ALPHAOP, m_var.AlphaOp));
            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE));
        }
        else
        {
            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_COLOROP, D3DTOP_DISABLE));
            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_ALPHAOP, D3DTOP_DISABLE));

            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_MINFILTER, D3DTEXF_LINEAR));
            CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_MAGFILTER, D3DTEXF_LINEAR));
        }

        CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP));
        CheckHR(m_pDevice->SetTextureStageState(itex, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP));
    }

    CreateQuadList(m_var.overdraw);

//	m_dwStartLoopTime = 0;

    return true;
}

//=========================================================================
// Init Direct3d
//=========================================================================
BOOL CFillRate::InitD3D()
{
	//D3DPRESENT_PARAMETERS d3dppCur = {0};
    D3DPRESENT_PARAMETERS d3dpp = {0};

	HRESULT hr = S_OK;

    // Set the screen mode.
    d3dpp.BackBufferWidth                   = UINT(m_var.screenwidth);
    d3dpp.BackBufferHeight                  = UINT(m_var.screenheight);
    d3dpp.BackBufferFormat                  = m_var.BackBufferFormat;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.EnableAutoDepthStencil            = !!m_var.AutoDepthStencilFormat;
    d3dpp.AutoDepthStencilFormat            = m_var.AutoDepthStencilFormat;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz        = 60;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.Flags                             = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    if(m_var.MSType != D3DMULTISAMPLE_NONE)
        d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)((DWORD)(m_var.MSType) | (DWORD)(m_var.MSFormat));

#if 0
    // if nothing has changed - don't reinit
    if(!memcmp(&d3dppCur, &d3dpp, sizeof(D3DPRESENT_PARAMETERS)))
        return FALSE;
    d3dppCur = d3dpp;

    if(m_pDevice)
    {
        if(m_ShaderHandle)
        {
            CheckHR(m_pDevice->SetPixelShader(0));
            CheckHR(m_pDevice->DeletePixelShader(m_ShaderHandle));
            m_ShaderHandle = 0;
        }

        // make sure everything is freed
        RELEASE(m_pVB);
        for(int itex = 0; itex < 4; itex++)
            RELEASE(m_pTexture[itex]);

        RELEASE(m_pBackBuffer);
        RELEASE(m_pDevice);
    }
#endif 0

    // Create the device.
    EXECUTE(Direct3D_CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
        NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice))
    if ( FAILED ( hr ) )
	{
        return FALSE;
    }

    D3DDevice_GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
    return TRUE;
}

//=========================================================================
// Fill a volume texture with shtuff
//=========================================================================
void FillTextureBits3D(D3DLOCKED_BOX *plockVol,
    D3DFORMAT d3dFormat, DWORD dwHeight, DWORD dwWidth, DWORD dwDepth)
{
    DWORD dwBits = BitsPerPixelOfD3DFORMAT(d3dFormat);

    assert(!IsLinearFormat(d3dFormat));

    BYTE *pBits = (BYTE *)plockVol->pBits;
    for(DWORD d = 0; d < dwDepth; d++)
    {
        for(DWORD y = 0; y < dwHeight; y++)
        {
            for(DWORD x = 0; x < dwWidth; x++)
            {
                DWORD c = (~y & 1) * (y + 1) * 0xff / dwHeight;
                DWORD c2 = d * 0xff / (dwDepth - 1);

                switch(dwBits)
                {
                case 4:
                    x++;
                case 8:
                    *pBits++ = LOBYTE(c);
                    break;
                case 16:
                    *(WORD *)pBits = MAKEWORD(c2, c);
                    pBits += 2;
                    break;
                case 32:
                    *(DWORD *)pBits = MAKELONG(MAKEWORD(c2, c), MAKEWORD(c2, c));
                    pBits += 4;
                    break;
                default:
                    CheckHR(E_FAIL);
                    break;
                }
            }
        }
    }
}

//=========================================================================
// Fill a 2D cubemap face or texture with shtuff
//=========================================================================
void FillTextureBits(D3DLOCKED_RECT *plockRect,
    D3DFORMAT d3dFormat, DWORD dwHeight, DWORD dwWidth)
{
    bool fIsLinear = IsLinearFormat(d3dFormat);
    DWORD dwBits = BitsPerPixelOfD3DFORMAT(d3dFormat);
    DWORD dwLine = (DWORD)plockRect->pBits;

    for (DWORD y = 0; y < dwHeight; y++)
    {
        DWORD dwAddr = dwLine;
        DWORD c = (~y & 1) * (y + 1) * 0xff / dwHeight;

        for (DWORD x = 0; x < dwWidth; x++)
        {
            switch(dwBits)
            {
            case 4:
                *(BYTE *)dwAddr = LOBYTE(c);
                dwAddr += 1;
                x++;
                break;
            case 8:
                *(BYTE *)dwAddr = LOBYTE(c);
                dwAddr += 1;
                break;
            case 16:
                *(WORD *)dwAddr = MAKEWORD(c, c);
                dwAddr += 2;
                break;
            case 32:
                *(DWORD *)dwAddr = MAKELONG(MAKEWORD(c, c), MAKEWORD(c, c));
                dwAddr += 4;
                break;
            }
        }

        // pitch is only valid for linear textures
        if(fIsLinear)
            dwLine += plockRect->Pitch;
        else
            dwLine += dwWidth * dwBits / 8;
    }
}

//=========================================================================
// Create a texture
//=========================================================================
IDirect3DBaseTexture8 *CFillRate::MyCreateTexture(D3DRESOURCETYPE d3dtype,
    D3DFORMAT d3dFormat, DWORD dwWidth, DWORD dwHeight)
{
    IDirect3DBaseTexture8 *pTexRet = NULL;

    if(d3dtype == D3DRTYPE_TEXTURE)
    {
        D3DLOCKED_RECT lockRect;
        IDirect3DTexture8 *pTexture = NULL;

        CheckHR(m_pDevice->CreateTexture(dwWidth, dwHeight, 1, 0, d3dFormat,
            D3DPOOL_MANAGED, &pTexture));

        CheckHR(pTexture->LockRect(0, &lockRect, NULL, 0));
        FillTextureBits(&lockRect, d3dFormat, dwHeight, dwWidth);
        CheckHR(pTexture->UnlockRect(0));

        pTexRet = pTexture;
    }
    else if(d3dtype == D3DRTYPE_CUBETEXTURE)
    {
        D3DLOCKED_RECT lockRect;
        IDirect3DCubeTexture8 *pCubeTexture = NULL;

        CheckHR(m_pDevice->CreateCubeTexture(dwWidth, 1, 0, d3dFormat,
            D3DPOOL_MANAGED, &pCubeTexture));

        for(int iFace = 0; iFace < D3DCUBEMAP_FACE_NEGATIVE_Z; iFace++)
        {
            D3DCUBEMAP_FACES cmFace = (D3DCUBEMAP_FACES)iFace;

            CheckHR(pCubeTexture->LockRect(cmFace, 0, &lockRect, NULL, 0));
            FillTextureBits(&lockRect, d3dFormat, dwHeight, dwWidth);
            CheckHR(pCubeTexture->UnlockRect(cmFace, 0));
        }

        pTexRet = pCubeTexture;
    }
    else
    {
        assert(d3dtype == D3DRTYPE_VOLUMETEXTURE);

        D3DLOCKED_BOX lockVol;
        IDirect3DVolumeTexture8 *pVolumeTexture = NULL;

        CheckHR(m_pDevice->CreateVolumeTexture(dwWidth, dwHeight, dwHeight, 1, 0,
            d3dFormat, D3DPOOL_MANAGED, &pVolumeTexture));

        CheckHR(pVolumeTexture->LockBox(0, &lockVol, NULL, 0));
        FillTextureBits3D(&lockVol, d3dFormat, dwHeight, dwWidth, dwWidth);
        CheckHR(pVolumeTexture->UnlockBox(0));

        pTexRet = pVolumeTexture;
    }

   return pTexRet;
}

//=========================================================================
// Create a quadlist vb with X quads and decreasing z values
//=========================================================================
void CFillRate::CreateQuadList(int nquads)
{
    RELEASE(m_pVB);

    if(m_cIndices)
    {
        delete [] m_pIndices;
        m_cIndices = 0;
    }

    m_dwFVFSize = D3DXGetFVFVertexSize(m_var.FVF);

    if(m_var.verboselevel >= 2)
        dprintf("FVF:0x%08lx, FVFSize: %d\n", m_var.FVF, m_dwFVFSize);

    UINT VBLength = m_dwFVFSize * 4 * nquads;

    CheckHR(m_pDevice->CreateVertexBuffer(VBLength,
        D3DUSAGE_WRITEONLY, m_var.FVF, D3DPOOL_MANAGED, &m_pVB));

    float *pVerts;
    CheckHR(m_pVB->Lock(0, m_dwFVFSize, (BYTE **)&pVerts, 0));

    float texscale = m_var.rgtex[0].type && IsLinearFormat(m_var.rgtex[0].d3dFormat) ?
        m_var.rgtex[0].texsize : 1.0f;

    float dz = 1.0f / (nquads * 2);
    float zval = 1.0f - dz;
    for(int iquad = 0; iquad < nquads; iquad++, zval -= dz)
    {
        for(int y = 0; y <= 1; y++)
        {
            for(int x = 0; x <= 1; x++)
            {
                D3DXVECTOR4 pos;
                D3DXVECTOR4 tex0;

                // position
                pos.x = m_var.quadx + x * m_var.quadwidth;
                pos.y = m_var.quady + y * m_var.quadheight;
                pos.z = zval;
                pos.w = 1.0f;

                // texture coordinates
                if(m_var.rotatetex)
                {
                    tex0.x = (1.0f - y) * texscale;
                    tex0.y = x * texscale;
                }
                else
                {
                    tex0.x = x * texscale;
                    tex0.y = y * texscale;
                }
                tex0.z = (1.0f - x) * texscale;
                tex0.w = 1.0f;

                // diffuse color
                DWORD diffuse = D3DCOLOR_COLORVALUE(tex0.x,
                    tex0.y, 1.0f - tex0.x, 2.0f / 0xff);

                if(m_var.FVF & D3DFVF_XYZRHW)
                {
                    *(D3DXVECTOR4 *)pVerts = pos;
                    pVerts += 4;
                }
                else if(m_var.FVF & D3DFVF_XYZ)
                {
                    *(D3DXVECTOR3 *)pVerts = *(D3DXVECTOR3 *)&pos;
                    pVerts += 3;
                }

                if(m_var.FVF & D3DFVF_NORMAL)
                {
                    *(D3DXVECTOR3 *)pVerts = D3DXVECTOR3(0, .707f, .707f);
                    pVerts += 3;
                }

                if(m_var.FVF & D3DFVF_DIFFUSE)
                    *(DWORD *)pVerts++ = diffuse;

                if(m_var.FVF & D3DFVF_SPECULAR)
                    *(DWORD *)pVerts++ = ~diffuse;

                // Texture coordinates
                UINT uNumTexCoords = (((m_var.FVF) & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
                UINT uTextureFormats = m_var.FVF >> 16;

                if(uTextureFormats)
                {
                    for(UINT i = 0; i < uNumTexCoords; i++)
                    {
                        switch(uTextureFormats & 3)
                        {
                        case D3DFVF_TEXTUREFORMAT1:     // u
                            *pVerts++ = tex0.x;
                            break;
                        case D3DFVF_TEXTUREFORMAT2:     // u,v
                            *(D3DXVECTOR2 *)pVerts = *(D3DXVECTOR2 *)&tex0;
                            pVerts += 2;
                            break;
                        case D3DFVF_TEXTUREFORMAT3:     // s,t,r
                            *(D3DXVECTOR3 *)pVerts = *(D3DXVECTOR3 *)&tex0;
                            pVerts += 3;
                            break;
                        case D3DFVF_TEXTUREFORMAT4:     // s,t,r,q
                            *(D3DXVECTOR4 *)pVerts = tex0;
                            pVerts += 4;
                            break;
                        }

                        uTextureFormats >>= 2;
                    }
                }
                else while(uNumTexCoords--)
                {
                    // u,v
                    *(D3DXVECTOR2 *)pVerts = *(D3DXVECTOR2 *)&tex0;
                    pVerts += 2;
                }
            }
        }
    }

    CheckHR(m_pVB->Unlock());
    m_pVB->MoveResourceMemory(D3DMEM_VIDEO);

    // create out index buffer
    m_cIndices = nquads * 4;
    m_pIndices = new WORD [m_cIndices];

    WORD *pindices = m_pIndices;

    for(iquad = 0; iquad < nquads; iquad++)
    {
        *pindices++ = iquad * 4 + 0;
        *pindices++ = iquad * 4 + 1;
        *pindices++ = iquad * 4 + 3;
        *pindices++ = iquad * 4 + 2;
    }
}



//=========================================================================
// Dump the first entry in the z buffer
//=========================================================================
void CFillRate::DumpZ()
{
    D3DLOCKED_RECT lockRect;
    D3DSURFACE_DESC desc;
    IDirect3DSurface8 *pZBuffer = NULL;

    if(!m_var.AutoDepthStencilFormat)
        return;

    CheckHR(m_pDevice->GetDepthStencilSurface(&pZBuffer));

    CheckHR(pZBuffer->LockRect(&lockRect, NULL, D3DLOCK_READONLY));
    CheckHR(pZBuffer->GetDesc(&desc));

    DWORD dwVal;
    switch(desc.Format)
    {
    case D3DFMT_LIN_D16:
    case D3DFMT_D16:
        // v = z[15:0]  0.0 to 65535.0
        dprintf("D3DFMT_D16: 0x%08lx", *(WORD *)lockRect.pBits);
        break;
    case D3DFMT_LIN_D24S8:
    case D3DFMT_D24S8:
        // v = z[24:0] 0.0 to 16,777,215.0  (0 to FFFFFF)
        dwVal = (*(DWORD *)lockRect.pBits);
        dprintf("D3DFMT_D24S8: 0x%08lx:%02lx %.2f", dwVal >> 8, LOBYTE(dwVal),
            (dwVal >> 8) / 16777215.0f);
        // 0x3ffffffe, 0x7ffffffe, 0xbffffffe
        // 0.25f, 0.75f, 0.5f
        break;
    case D3DFMT_LIN_F16:
    case D3DFMT_F16:
        // e4m12
        // e = z[15:12]
        // m = z[11:0]
        // value = 2^(e-7) * (1 + m/4096)
        // value = 0.0 when e == 0 and m == 0
        dprintf("D3DFMT_F16: 0x%08lx", *(WORD *)lockRect.pBits);
        break;
    case D3DFMT_LIN_F24S8:
    case D3DFMT_F24S8:
        // e8m16
        // e = z[23:16]
        // m = z[15:0]
        // value = 2^(e-127) * (1 + m/65536)
        // value = 0.0 when e == 0 and m == 0
        dprintf("D3DFMT_F24S8: 0x%08lx", *(DWORD *)lockRect.pBits);
        break;
    }

    pZBuffer->UnlockRect();

    RELEASE(pZBuffer);
}

//=========================================================================
// render the quads X # of times
//=========================================================================
void CFillRate::RenderFrame(int overdraw)
{
    UINT coverdrawverts = overdraw * 4;

    // clear stuff
    DWORD dwCFlags = m_var.clearflags;
    if(!m_var.AutoDepthStencilFormat)
        dwCFlags &= ~(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);

    CheckHR(m_pDevice->Clear(0, NULL, dwCFlags, m_var.cclearval,
        m_var.zclearval, m_var.sclearval));

    // begin scene
    CheckHR(m_pDevice->BeginScene());

    // set textures
    for(int itex = 0; itex < 4; itex++)
        CheckHR(m_pDevice->SetTexture(itex, m_pTexture[itex]));

    CheckHR(m_pDevice->SetVertexShader(m_var.FVF));
    CheckHR(m_pDevice->SetStreamSource(0, m_pVB, m_dwFVFSize));

    CheckHR(m_pDevice->SetPixelShader(m_ShaderHandle));

    if(m_var.zenable && m_var.primez)
    {
        // prime the z buffer to 0.5f
        CheckHR(m_pDevice->SetRenderState(D3DRS_ZFUNC, m_var.primezcmpfunc));
        CheckHR(m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE));

        // draw one quad (not the first one) to prime the z buffer
        CheckHR(m_pDevice->DrawIndexedVertices(D3DPT_QUADLIST, 4, m_pIndices + 4));
        coverdrawverts -= 4;

        CheckHR(m_pDevice->SetRenderState(D3DRS_ZFUNC, m_var.d3dcmpfunc));
        CheckHR(m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, m_var.zwriteenable));
    }

    while(coverdrawverts)
    {
        UINT VertexCount = min(m_cIndices, coverdrawverts);
        CheckHR(m_pDevice->DrawIndexedVertices(D3DPT_QUADLIST, VertexCount, m_pIndices));

        coverdrawverts -= VertexCount;
    }

    if(m_fdumpZ)
        DumpZ();

	CheckHR(m_pDevice->Present( NULL, NULL, NULL, NULL ));
    CheckHR(m_pDevice->EndScene());
}


//=========================================================================
// Run through a suite of multisample/supersample tests
//=========================================================================
void CFillRate::DoMultiSampleTimedTest()
{
    D3DMULTISAMPLE_TYPE rgMSType[] =
    {
        D3DMS_NONE,
        D3DMS_2_SAMPLES_MULTISAMPLE_LINEAR,
        D3DMS_2_SAMPLES_MULTISAMPLE_QUINCUNX,
        D3DMS_4_SAMPLES_MULTISAMPLE_LINEAR,
        D3DMS_4_SAMPLES_MULTISAMPLE_GAUSSIAN,
        D3DMS_9_SAMPLES_MULTISAMPLE_GAUSSIAN,
        D3DMS_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR,
        D3DMS_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR,
        D3DMS_4_SAMPLES_SUPERSAMPLE_LINEAR,
        D3DMS_4_SAMPLES_SUPERSAMPLE_GAUSSIAN,
        D3DMS_9_SAMPLES_SUPERSAMPLE_GAUSSIAN
    };
    static const int cMSTypes = ARRAYSIZE(rgMSType);

    D3DFORMAT rgBackBuf[] = { D3DFMT_R5G6B5, D3DFMT_X8R8G8B8 };
    static const cBBufs = ARRAYSIZE(rgBackBuf);

//    m_scrtext.cls();
    for(int iBuf = 0; iBuf < cBBufs; iBuf++)
    {
		CHAR str[255];
		sprintf( str, "BackBufFmt: %-38s %-8s %-8s %s\n", GetD3DFormatStr(rgBackBuf[iBuf]), "MPix/s", "fps", "frames");
		//m_Font.DrawText(m_pBackBuffer, str, 50, 50, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
		DbgPrint( str );
        //m_scrtext.printf("BackBufFmt: %-38s %-8s %-8s %s",
        //    GetD3DFormatStr(rgBackBuf[iBuf]), "MPix/s", "fps", "frames");

        for(int iType = 0; iType < cMSTypes; iType++)
        {
            TIMETESTRESULTS ttres;

            m_var.MSType = rgMSType[iType];
            m_var.BackBufferFormat = rgBackBuf[iBuf];

            InitFR(false);
            DoTimedTest(&ttres);

            //m_scrtext.printf();
			sprintf( str, "\n    %-45s %7.2f  %8.2f  %d\n\n",
                GetMultiSampleStr(rgMSType[iType]),
                ttres.fillrate, ttres.fps, ttres.cFrames);
			//m_Font.DrawText(m_pBackBuffer, str, 50, 50, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
			DbgPrint( str );
        }
    }
}

//=========================================================================
// Do a quick test with the current settings
//=========================================================================
void CFillRate::DoQuickTimedTest()
{
    TIMETESTRESULTS ttres;

    DoTimedTest(&ttres);

//    m_scrtext.cls();
    DumpCurrentSettings();
	CHAR str[512];
	sprintf( str, "\n fillrate: %.2fMP/s\n \
					fps:      %.2f\n\
					frames:   %d\n\
					time:     %.2fms\n\
					pixels:   %I64u\n\n",
					ttres.fillrate,
					ttres.fps,
					ttres.cFrames,
					ttres.TimeTillIdle,
					ttres.cNumPixelsDrawn);


	//m_Font.DrawText(m_pBackBuffer, str, 50, 50, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
	DbgPrint( str );
}

//=========================================================================
// Texture fillrate test
//=========================================================================
void CFillRate::DoTextureTest()
{
    D3DFORMAT rgFormats[] = 
    {
        D3DFMT_A4R4G4B4,
        D3DFMT_A8R8G8B8,
        D3DFMT_DXT1,
        D3DFMT_DXT2,
        D3DFMT_P8,
        D3DFMT_LIN_A8R8G8B8
    };
    int rgTexSize[] = { 1, 16, 256 };
    float rgResults[ARRAYSIZE(rgTexSize)];

//    m_scrtext.cls();
	CHAR str[512];

	sprintf( str, "%-20s %-10s %-10s %-10s\n",
        "texturefmt", "1x1", "16x16", "256x256");
	//m_Font.DrawText( m_pBackBuffer, str, 50, 50, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
	DbgPrint( str );

    //m_scrtext.printf("%-20s %-10s %-10s %-10s",
    //    "texturefmt", "1x1", "16x16", "256x256");

    for(int id3dformat = 0; id3dformat < ARRAYSIZE(rgFormats); id3dformat++)
    {
        char szBuf[128];

        szBuf[0] = 0;
        for(int itexsize = 0; itexsize < ARRAYSIZE(rgTexSize); itexsize++)
        {
            m_var.rgtex[0].type = TEX_2d;
            m_var.rgtex[0].texsize = rgTexSize[itexsize];
            m_var.rgtex[0].d3dFormat = rgFormats[id3dformat];

            InitFR(false);
            rgResults[itexsize] = DoTimedTest();
        }

		sprintf( str, "\n%-20s %-10.2f %-10.2f %-10.2f\n\n",
            GetD3DFormatStr(m_var.rgtex[0].d3dFormat),
            rgResults[0], rgResults[1], rgResults[2]);
		//m_Font.DrawText( m_pBackBuffer, str, 50, 50, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
		DbgPrint( str );
        //m_scrtext.printf(
        //    "%-20s %-10.2f %-10.2f %-10.2f",
        //    GetD3DFormatStr(m_var.rgtex[0].d3dFormat),
        //    rgResults[0], rgResults[1], rgResults[2]);
    }
}

//=========================================================================
// Run through a suite of fillrate tests
//=========================================================================
void CFillRate::DoCompleteTimedTest()
{
    D3DFORMAT rgFormats[] = { D3DFMT_DXT1, D3DFMT_A8R8G8B8, D3DFMT_A4R4G4B4 };

//    m_scrtext.cls();
	CHAR str[512];

	sprintf( str, "zwrite zread texturefmt      texturesize fillrate  (alphablendenable:%d)\n",
        m_var.alphablendenable);
	//m_Font.DrawText(m_pBackBuffer, str, 50, 50, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
	DbgPrint( str );
        

    for(int id3dformat = 0; id3dformat < ARRAYSIZE(rgFormats); id3dformat++)
    {
        for(int texsize = 0; texsize <= 512; texsize += 256)
        {
            for(int zenable = 0; zenable < 2; zenable++)
            {
                for(int zwriteenable = 0; zwriteenable < 2; zwriteenable++)
                {
                    m_var.zenable = !!zenable;
                    m_var.zwriteenable = !!zwriteenable;

                    m_var.d3dcmpfunc = D3DCMP_NOTEQUAL;

                    m_var.rgtex[0].texsize = texsize;
                    m_var.rgtex[0].type = !!texsize;
                    m_var.rgtex[0].d3dFormat = rgFormats[id3dformat];

                    if(!m_var.zenable && zwriteenable)
                        continue;

                    if(!m_var.rgtex[0].type && id3dformat)
                        continue;

                    InitFR(false);
                    float fResult = DoTimedTest();

                    sprintf( str, "\n%-6d %-5d %-15s %-11d %.2f\n\n",
                        m_var.zwriteenable,
                        m_var.zenable,
                        texsize ? GetD3DFormatStr(m_var.rgtex[0].d3dFormat) : "",
                        texsize,
                        fResult);
					//m_Font.DrawText(m_pBackBuffer, str, 50, 50, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
					DbgPrint( str );
                }
            }
        }
    }
}

//=========================================================================
// Dump the current options
//=========================================================================
void CFillRate::DumpCurrentSettings()
{
	WCHAR str[512];
	INT iX, iY;

	iX = 35;
	iY = 100;

    swprintf( str, (wchar_t *)"Btn A) run '%s' test.", m_rgTests[m_iCurTest].szDesc);
	m_Font.DrawText(m_pBackBuffer, str, iX, iY, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
	iY+=12;

    swprintf( str, (wchar_t *)"AutoDepthStencilFormat:%s "
        "BackBufferFormat:%s ",
        m_var.AutoDepthStencilFormat ? GetD3DFormatStr(m_var.AutoDepthStencilFormat) : "none",
        GetD3DFormatStr(m_var.BackBufferFormat));
	m_Font.DrawText(m_pBackBuffer, str, iX, iY, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
	iY+=12;

    swprintf( str, (wchar_t *)"MultiSampleType:%s ",
        GetMultiSampleStr(m_var.MSType));
	m_Font.DrawText(m_pBackBuffer, str, iX, iY, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
	iY+=12;

    swprintf( str, (wchar_t *)"MultiSampleFormat:%s",
        GetMultiSampleStr(m_var.MSFormat));
	m_Font.DrawText(m_pBackBuffer, str, iX, iY, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
	iY+=12;

    swprintf( str, (wchar_t *)"zenable:%d "
        "z-write:%d "
        "d3dcmpfunc:%s",
        m_var.zenable,
        m_var.zenable && m_var.zwriteenable,
        GetD3DCMPStr(m_var.d3dcmpfunc));
	m_Font.DrawText(m_pBackBuffer, str, iX, iY, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
	iY+=12;

    swprintf( str, (wchar_t *)"alpha-blend:%d "
        "alpha-test:%d "
        "swathwidth:%d ",
        m_var.alphablendenable,
        m_var.alphatestenable,
        (m_var.SwathWidth == D3DSWATH_OFF) ? -1 : (0x8 << m_var.SwathWidth));
	m_Font.DrawText(m_pBackBuffer, str, iX, iY, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
	iY+=12;

    swprintf( str, (wchar_t *)"timeflags:%s%s",
        (m_var.timeflags & TIME_Render) ? "TIME_Render " : "",
        (m_var.timeflags & TIME_Present) ? "TIME_Present" : "");
	m_Font.DrawText(m_pBackBuffer, str, iX, iY, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
	iY+=12;

    for(int itex = 0; itex < 4; itex++)
    {
        if(m_var.rgtex[itex].type)
        {
            swprintf( str, (wchar_t *)"%d) texfmt:%s "
                "texel size (bytes):%0.1f "
                "texsize: %d",
                itex,
                GetD3DFormatStr(m_var.rgtex[itex].d3dFormat),
                BitsPerPixelOfD3DFORMAT(m_var.rgtex[itex].d3dFormat) / 8.0f,
                m_var.rgtex[itex].texsize);
			m_Font.DrawText(m_pBackBuffer, str, iX, iY, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
			iY+=12;
        }
    }

    if(m_var.userpixelshader)
    {
        swprintf( str, (wchar_t *)"CombinerCount:%d",
            m_var.shaderdef.PSCombinerCount & 0xf);
		m_Font.DrawText(m_pBackBuffer, str, iX, iY, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
		iY+=12;

        UINT TexModes = m_var.shaderdef.PSTextureModes;
        for(int texmode = 0; texmode < 4; texmode++, TexModes >>= 5)
        {
            if(TexModes & 0x1f)
            {
                swprintf( str, (wchar_t *)"  T%d Mode:%s",
                    texmode, GetTextureModeStr(TexModes & 0x1f));
				m_Font.DrawText(m_pBackBuffer, str, iX, iY, 0, D3DCOLOR_XRGB( 255, 255, 128 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
				iY+=12;
            }
        }
    }
}

//=========================================================================
//
//=========================================================================
FLOAT CFillRate::EstimateTimeForScene()
{
    CTimer timer;
    int overdraw = min(10, m_var.overdraw);

    // update the screen
    RenderFrame(1);
    //CheckHR(m_pDevice->Present(NULL, NULL, NULL, NULL));

    // wait until everything is cleared out
    m_pDevice->BlockUntilIdle();

    // start timing
    timer.Start();

    if(m_var.timeflags & TIME_Render)
        RenderFrame(10);
    if(m_var.timeflags & TIME_Present)
    {
        //CheckHR(m_pDevice->Present(NULL, NULL, NULL, NULL));
    }

    // wait until GPU is idle again
    m_pDevice->BlockUntilIdle();

    // stop timer
    timer.Stop();

    return m_var.overdraw * timer.getTime() / overdraw;
}

//=========================================================================
// Do a timed test and spit out the results
//=========================================================================
float CFillRate::DoTimedTest(TIMETESTRESULTS *pttres)
{
    CTimer timer;
    int frames = 0;
    int visibilityindex = 0;
    float TimeTillDoneDrawing = 0;

    // wait until everything is cleared out
    m_pDevice->BlockUntilIdle();

    int numberscenes = m_var.numberscenes;

    if(!numberscenes)
    {
        // if there isn't an explicit count of scenes to render
        // guesstimate the time required for each scene
        float fEstimatedTimePerScene = EstimateTimeForScene();

        // and figure out how many scenes to render
        numberscenes = int(m_var.testtime / fEstimatedTimePerScene);
    }

    // put an upper cap and lower bound on the number of scenes
    numberscenes = max(2, min(numberscenes, 2047));

    // start timing
    timer.Start();

    // render X number of scenes
    for(frames = 0; frames < numberscenes; frames++)
    {
#ifndef NO_VIS_TEST
        CheckHR(m_pDevice->BeginVisibilityTest());
#endif

        if(m_var.timeflags & TIME_Render)
            RenderFrame(m_var.overdraw);
        if(m_var.timeflags & TIME_Present)
        {
            //CheckHR(m_pDevice->Present(NULL, NULL, NULL, NULL));
        }

#ifndef NO_VIS_TEST
        CheckHR(m_pDevice->EndVisibilityTest(visibilityindex++));
#endif
    }

    TimeTillDoneDrawing = timer.getTime();

    // wait until GPU is idle again
    m_pDevice->BlockUntilIdle();

    // stop timer
    timer.Stop();
    float TimeTillIdle = timer.getTime();

    // get count of pixels drawn
    UINT64 dwNumPixelsDrawn = 0;

#ifdef NO_VIS_TEST

    dwNumPixelsDrawn = (UINT64)m_var.screenwidth * (UINT64)m_var.screenheight *
        m_var.overdraw * frames;

#else

    for(int ivis = 0; ivis < visibilityindex; ivis++)
    {
        // Check the number of pixels drawn.
        UINT dwT;
        while(m_pDevice->GetVisibilityTestResult(ivis, &dwT, NULL) == D3DERR_TESTINCOMPLETE)
            ;

        if(m_var.verboselevel >= 2)
            dprintf("pass:%d  %ld pixels\n", ivis, dwT);

        if(dwT == 0xffffffff)
            dwT = 0;
        dwNumPixelsDrawn += dwT;
    }

#endif

    /*
     * Calculate fillrate
     */
    float fillrate = 0.0f;

    if((m_var.timeflags & TIME_Present) == TIME_Present)
    {
        // if we're just calculating present fillrate the pixels count
        // should be fairly accurate.
        fillrate = dwNumPixelsDrawn / (TimeTillIdle * 1000.0f);
    }
    else
    {
        // get backbuffer size
        D3DSURFACE_DESC Desc;
        CheckHR(m_pBackBuffer->GetDesc(&Desc));

        // The visibility counters count the number of pixels drawn to the backbuffer
        // which is larger when multisampling. So your fillrate gets skewed.
        // So scale those buggers by the backbuffer size.
        float fScale = (Desc.Width * Desc.Height) / (m_var.screenwidth * m_var.screenheight);
        fillrate = dwNumPixelsDrawn / (TimeTillIdle * 1000.0f * fScale);

        if((m_var.timeflags & TIME_Render) == TIME_Render)
        {
            // For the render case verify that the fillrate numbers match what we think
            // they should be.
            float fillrate2 = m_var.screenwidth * m_var.screenheight *
                m_var.overdraw * frames / (TimeTillIdle * 1000.0f);

            if(fabsf(fillrate - fillrate2) > 1.0f)
            {
                dprintf("*** Fillrate numbers don't add up: %.2f != %.2f\n",
                    fillrate, fillrate2);
            }
        }
    }

    if(pttres)
    {
        pttres->fillrate = fillrate;
        pttres->fps = frames * 1000 / TimeTillIdle;
        pttres->cFrames = frames;
        pttres->TimeTillDoneDrawing = TimeTillDoneDrawing;
        pttres->TimeTillIdle = TimeTillIdle;
        pttres->cNumPixelsDrawn = dwNumPixelsDrawn;
    }

    return fillrate;
}

//=========================================================================
// macros to help read entries from the ini file
//=========================================================================
/*static const char g_szOpts[] = "options";
#define get_option_val(_var)          m_inifile.GetIniInt(g_szOpts, #_var, m_var._var)
#define get_option_valf(_var)         m_inifile.GetIniFloat(g_szOpts, #_var, m_var._var)
#define get_option_val3(_s, _var, _d) m_inifile.GetIniInt(_s, _var, _d)*/



#define get_option_val(_var, _d)          GetPrivateProfileIntA(szSectionName, #_var, _d, FILLRATE_INI_PATH)
#define get_option_valc(_var, _d)          GetPrivateProfileConst((LPCTSTR)szSectionName, (LPCTSTR)#_var, _d, (LPCTSTR)FILLRATE_INI_PATH)
#define get_option_valf(_var, _d)         GetPrivateProfileFloat((LPCTSTR)szSectionName, (LPCTSTR)#_var, _d, (LPCTSTR)FILLRATE_INI_PATH)
#define get_option_val3(_s, _var, _d) GetPrivateProfileConst((LPCTSTR)_s, (LPCTSTR)_var, _d, (LPCTSTR)FILLRATE_INI_PATH)

//=========================================================================
// Read ini file and update all fr settings
//=========================================================================
BOOL CFillRate::ReadFRIniFile()
{
	CHAR szSectionName[256];

	sprintf( szSectionName, "LOOP%d", m_wLoop );

    if(m_ShaderHandle)
    {
        assert(m_pDevice);
        CheckHR(m_pDevice->SetPixelShader(0));
        CheckHR(m_pDevice->DeletePixelShader(m_ShaderHandle));
        m_ShaderHandle = 0;
    }

	ZeroMemory(&m_var, sizeof(FRSETTINGS) );

	m_var.overdraw = 100;
    m_var.verboselevel              = get_option_val(				verboselevel,			1);
    m_var.runtestonreload           = !!get_option_val(				runtestonreload,		1);

	m_var.screenwidth				= get_option_valf(				screenwidth,			640.0f);
	m_var.screenheight				= get_option_valf(				screenheight,			480.0f);
	m_var.BackBufferFormat          = (D3DFORMAT)get_option_valc(	BackBufferFormat,		D3DFMT_A8R8G8B8);
    m_var.AutoDepthStencilFormat    = (D3DFORMAT)get_option_valc(	AutoDepthStencilFormat, D3DFMT_D24S8);

    m_var.MSType                    = (D3DMULTISAMPLE_TYPE)get_option_valc(MSType,			D3DMS_NONE);
    m_var.MSFormat                  = (D3DMULTISAMPLE_TYPE)get_option_valc(MSFormat,		D3DMS_PREFILTER_FORMAT_DEFAULT);

	m_var.FVF                       = get_option_valc(				FVF,					D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
    m_var.quadwidth                 = get_option_valf(				quadwidth,				640);
    m_var.quadheight                = get_option_valf(				quadheight,				480);
    m_var.quadx                     = get_option_valf(				quadx,					(m_var.screenwidth - min(m_var.screenwidth, m_var.quadwidth)) / 2);
    m_var.quady                     = get_option_valf(				quady,					(m_var.screenheight - min(m_var.screenheight, m_var.quadheight)) / 2);

    m_var.zenable                   = !!get_option_val(				zenable,				FALSE);
    m_var.zwriteenable              = !!get_option_val(				zwriteenable,			FALSE);
    m_var.d3dcmpfunc                = (D3DCMPFUNC)get_option_valc(	d3dcmpfunc,				D3DCMP_NOTEQUAL);

    m_var.primez                    = !!get_option_val(				primez,					FALSE);
    m_var.primezcmpfunc             =  (D3DCMPFUNC)get_option_valc(	primezcmpfunc,			D3DCMP_ALWAYS);

	m_var.testtime                  = get_option_valf(				testtime,				10000.0f);
    m_var.numberscenes              = get_option_val(				numberscenes,			0);
    m_var.timeflags                 = get_option_valc(				timeflags,				TIME_Render);

	m_var.clearflags                = get_option_valc(				clearflags,				D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
    m_var.zclearval                 = get_option_valf(				zclearval,				1.0f);
    m_var.sclearval                 = get_option_val(				sclearval,				0);
    m_var.cclearval                 = get_option_val(				cclearval,				D3DCOLOR_XRGB(0,0,0));

	m_var.userpixelshader           = !!get_option_val(				userpixelshader,		0x00006688);
    if ( m_var.userpixelshader )
	{
        ReadPixelShaderIni(&m_var.shaderdef);
	}

    m_var.alphablendenable          = !!get_option_val(				alphablendenable,		FALSE);
    m_var.alphatestenable           = !!get_option_val(				alphatestenable,		FALSE);
	m_var.rotatetex                 = !!get_option_val(				rotatetex,				0);

    m_var.SwathWidth                = (D3DSWATHWIDTH)get_option_valc(SwathWidth,			D3DSWATH_128);

	m_var.ColorOp					= D3DTOP_SELECTARG1;
    m_var.AlphaOp					= D3DTOP_SELECTARG1;
	
    m_var.d3dfillmode				= D3DFILL_SOLID;
    m_var.colorwriteenable          = get_option_valc(				colorwriteenable,		D3DCOLORWRITEENABLE_ALL);

    for(int itex = 0; itex < 4; itex++)
    {
        char szSect[128];
        _snprintf(szSect, sizeof(szSect), "texture%d", itex);

        m_var.rgtex[itex].type = get_option_val3(szSect, "type", TEX_None);
        m_var.rgtex[itex].texsize = get_option_val3(szSect, "texsize", 8);
        m_var.rgtex[itex].d3dFormat = (D3DFORMAT)get_option_val3(szSect, "d3dFormat", D3DFMT_DXT1);
        m_var.rgtex[itex].filtertype = (D3DTEXTUREFILTERTYPE)get_option_val3(szSect, "filtertype", D3DTEXF_LINEAR);
    }

    return true;
}

#if DBG

extern "C" { extern DWORD D3D__DeadlockTimeOutVal; }

#endif


//=========================================================================
// Initialize and create our pixel shader
//=========================================================================
void CFillRate::ReadPixelShaderIni(D3DPIXELSHADERDEF *pshaderdef)
{
    enum
    {
        SE_PSTextureModes,
        SE_PSDotMapping,
        SE_PSInputTexture,
        SE_PSCompareMode,
        SE_PSCombinerCount,
        SE_PSFinalCombinerConstant0,
        SE_PSFinalCombinerConstant1,
        SE_PSFinalCombinerInputsABCD,
        SE_PSFinalCombinerInputsEFG,
        SE_Max
    };

#define SE_ENTRY(_x) SE_##_x, #_x
    struct ShaderEntry
    {
        DWORD dwSectionID;
        LPCSTR szSectionName;
        LPCSTR rgkeys[4];
        int rgvals[4];
    } rgEntries[] =
    {
        { SE_ENTRY(PSTextureModes),             { "t0", "t1", "t2", "t3" } },
        { SE_ENTRY(PSDotMapping),               { "t0", "t1", "t2", "t3" } },
        { SE_ENTRY(PSInputTexture),             { "t2", "t3" } },
        { SE_ENTRY(PSCompareMode),              { "t0", "t1", "t2", "t3" } },
        { SE_ENTRY(PSCombinerCount),            { "count", "flags" } },
        { SE_ENTRY(PSFinalCombinerConstant0),   { "c" } },
        { SE_ENTRY(PSFinalCombinerConstant1),   { "c" } },
        { SE_ENTRY(PSFinalCombinerInputsABCD),  { "A", "B", "C", "D" } },
        { SE_ENTRY(PSFinalCombinerInputsEFG),   { "E", "F", "G", "flags" } },
    };

    enum
    {
        SSE_PSConstant0,
        SSE_PSConstant1,
        SSE_PSRGBInputs,
        SSE_PSAlphaInputs,
        SSE_PSRGBOutputs,
        SSE_PSAlphaOutputs,
        SSE_Max
    };

#define SSE_ENTRY(_x) SSE_##_x, #_x
    struct ShaderStageEntry
    {
        DWORD dwSectionID;
        LPCSTR szSectionName;
        LPCSTR rgkeys[4];
        int rgvals[4][8];
    } rgStageEntries[] =
    {
        { SSE_ENTRY(PSConstant0),       { "c" } },
        { SSE_ENTRY(PSConstant1),       { "c" } },
        { SSE_ENTRY(PSRGBInputs),       { "A", "B", "C", "D" } },
        { SSE_ENTRY(PSAlphaInputs),     { "A", "B", "C", "D" } },
        { SSE_ENTRY(PSRGBOutputs),      { "ab", "cd", "mux_sum", "flags" } },
        { SSE_ENTRY(PSAlphaOutputs),    { "ab", "cd", "mux_sum", "flags" } },
    };

    // read all the shader entries
    for(int iSE = 0; iSE < SE_Max; iSE++)
    {
        ShaderEntry *pse = &rgEntries[iSE];

        for(int ikey = 0; ikey < 4; ikey++)
        {
            if(pse->rgkeys[ikey])
            {
                int val = get_option_val3(pse->szSectionName, pse->rgkeys[ikey], -1);
                if(val != -1)
                {
                    pse->rgvals[ikey] = val;
                    if(m_var.verboselevel >= 2)
                    {
                        dprintf("%s.%s = 0x%lx\n",
                            pse->szSectionName,
                            pse->rgkeys[ikey],
                            val);
                    }
                }
            }
        }
    }

    // read all the shader stage entries
    for(int iSSE = 0; iSSE < SSE_Max; iSSE++)
    {
        char szKey[128];
        ShaderStageEntry *psse = &rgStageEntries[iSSE];

        for(int ikey = 0; ikey < 4; ikey++)
        {
            if(psse->rgkeys[ikey])
            {
                for(int istage = 0; istage < 8; istage++)
                {
                    _snprintf(szKey, sizeof(szKey), "%s%d", psse->rgkeys[ikey], istage);
                    int val = get_option_val3(psse->szSectionName, szKey, -1);
                    if(val != -1)
                    {
                        psse->rgvals[ikey][istage] = val;
                        if(m_var.verboselevel >= 2)
                        {
                            dprintf("%s.%s[%d] = 0x%lx\n",
                                psse->szSectionName,
                                psse->rgkeys[ikey],
                                istage,
                                val);
                        }
                    }
                }
            }
        }
    }

    ZeroMemory(pshaderdef, sizeof(*pshaderdef));

    /*---------------------------------------------------------------------------*/
    /*  Texture configuration - The following members of the D3DPixelShaderDef   */
    /*  structure define the addressing modes of each of the four texture stages */
    /*---------------------------------------------------------------------------*/
    pshaderdef->PSTextureModes = PS_TEXTUREMODES(
        rgEntries[SE_PSTextureModes].rgvals[0],
        rgEntries[SE_PSTextureModes].rgvals[1],
        rgEntries[SE_PSTextureModes].rgvals[2],
        rgEntries[SE_PSTextureModes].rgvals[3]);

    pshaderdef->PSDotMapping = PS_DOTMAPPING(
        rgEntries[SE_PSDotMapping].rgvals[0],
        rgEntries[SE_PSDotMapping].rgvals[1],
        rgEntries[SE_PSDotMapping].rgvals[2],
        rgEntries[SE_PSDotMapping].rgvals[3]);

    pshaderdef->PSInputTexture = PS_INPUTTEXTURE(
        rgEntries[SE_PSInputTexture].rgvals[0],
        rgEntries[SE_PSInputTexture].rgvals[1],
        rgEntries[SE_PSInputTexture].rgvals[2],  // 0 and 1 valid
        rgEntries[SE_PSInputTexture].rgvals[3]); // 0, 1, and 2 valid

    pshaderdef->PSCompareMode = PS_COMPAREMODE(
        rgEntries[SE_PSCompareMode].rgvals[0],
        rgEntries[SE_PSCompareMode].rgvals[1],
        rgEntries[SE_PSCompareMode].rgvals[2],
        rgEntries[SE_PSCompareMode].rgvals[3]);

    /*---------------------------------------------------------------------------------*/
    /*  Color combiners - The following members of the D3DPixelShaderDef structure     */
    /*  define the state for the eight stages of color combiners                       */
    /*---------------------------------------------------------------------------------*/
    pshaderdef->PSCombinerCount = PS_COMBINERCOUNT(
        rgEntries[SE_PSCombinerCount].rgvals[0],
        rgEntries[SE_PSCombinerCount].rgvals[1]);

    for(int istage = 0; istage < 8; istage++)
    {
        pshaderdef->PSConstant0[istage] = rgStageEntries[SSE_PSConstant0].rgvals[0][istage];
        pshaderdef->PSConstant1[istage] = rgStageEntries[SSE_PSConstant1].rgvals[0][istage];

        // R0_ALPHA is initialized to T0_ALPHA in stage0

        // a,b,c,d each contain a value from PS_REGISTER, PS_CHANNEL, and PS_INPUTMAPPING
        pshaderdef->PSRGBInputs[istage] = PS_COMBINERINPUTS(
            rgStageEntries[SSE_PSRGBInputs].rgvals[0][istage],      // A
            rgStageEntries[SSE_PSRGBInputs].rgvals[1][istage],      // B
            rgStageEntries[SSE_PSRGBInputs].rgvals[2][istage],      // C
            rgStageEntries[SSE_PSRGBInputs].rgvals[3][istage]);     // D

        // R0_ALPHA is initialized to T0_ALPHA in stage0
        pshaderdef->PSAlphaInputs[istage] = PS_COMBINERINPUTS(
            rgStageEntries[SSE_PSAlphaInputs].rgvals[0][istage],    // A
            rgStageEntries[SSE_PSAlphaInputs].rgvals[1][istage],    // B
            rgStageEntries[SSE_PSAlphaInputs].rgvals[2][istage],    // C
            rgStageEntries[SSE_PSAlphaInputs].rgvals[3][istage]);   // D

        // mux_sum must be DISCARD if either AB_DOT_PRODUCT or CD_DOT_PRODUCT are set
        //  ie: Dot / Dot / Discard || Dot / Mult / Discard || Mult / Dot / Discard

        // ab,cd,mux_sum contain a value from PS_REGISTER
        // flags contains values from PS_COMBINEROUTPUT
        pshaderdef->PSRGBOutputs[istage] = PS_COMBINEROUTPUTS(
            rgStageEntries[SSE_PSRGBOutputs].rgvals[0][istage],     // ab
            rgStageEntries[SSE_PSRGBOutputs].rgvals[1][istage],     // cd
            rgStageEntries[SSE_PSRGBOutputs].rgvals[2][istage],     // mux_sum
            rgStageEntries[SSE_PSRGBOutputs].rgvals[3][istage]);    // flags
        pshaderdef->PSAlphaOutputs[istage] = PS_COMBINEROUTPUTS(
            rgStageEntries[SSE_PSAlphaOutputs].rgvals[0][istage],   // ab
            rgStageEntries[SSE_PSAlphaOutputs].rgvals[1][istage],   // cd
            rgStageEntries[SSE_PSAlphaOutputs].rgvals[2][istage],   // mux_sum
            rgStageEntries[SSE_PSAlphaOutputs].rgvals[3][istage]);  // flags
    }

    pshaderdef->PSFinalCombinerConstant0 = rgEntries[SE_PSFinalCombinerConstant0].rgvals[0];
    pshaderdef->PSFinalCombinerConstant1 = rgEntries[SE_PSFinalCombinerConstant1].rgvals[0];

    // FOG ALPHA is only available in final combiner
    // V1R0_SUM and EF_PROD are only available in final combiner A,B,C,D inputs
    // V1R0_SUM_ALPHA and EF_PROD_ALPHA are not available

    // AB + (1-A)C + D
    pshaderdef->PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(
        rgEntries[SE_PSFinalCombinerInputsABCD].rgvals[0],          // A
        rgEntries[SE_PSFinalCombinerInputsABCD].rgvals[1],          // B
        rgEntries[SE_PSFinalCombinerInputsABCD].rgvals[2],          // C
        rgEntries[SE_PSFinalCombinerInputsABCD].rgvals[3]);         // D

    // E,F,G contain a value from PS_REGISTER, PS_CHANNEL, and PS_INPUTMAPPING
    // flags contains values from PS_FINALCOMBINERSETTING:
    //    PS_FINALCOMBINERSETTING_CLAMP_SUM         // V1+R0 sum clamped to [0,1]
    //    PS_FINALCOMBINERSETTING_COMPLEMENT_V1     // unsigned invert mapping
    //    PS_FINALCOMBINERSETTING_COMPLEMENT_R0     // unsigned invert mapping
    pshaderdef->PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(
        rgEntries[SE_PSFinalCombinerInputsEFG].rgvals[0],           // E
        rgEntries[SE_PSFinalCombinerInputsEFG].rgvals[1],           // F
        rgEntries[SE_PSFinalCombinerInputsEFG].rgvals[2],           // G (alpha)
        rgEntries[SE_PSFinalCombinerInputsEFG].rgvals[3]);          // flags
}

#if 0
//=========================================================================
// Callback from inifile.cpp to get float string value
//=========================================================================
bool GetIniConstf(const char *szStr, int cchStr, float *pval)
{
    return false;
}

//=========================================================================
// Callback from inifile.cpp to get int string value
//=========================================================================
bool GetIniConst(const char *szStr, int cchStr, int *pval)
{
    #undef XTAG
    #define XTAG(_tag) { _tag, #_tag }
    static const struct
    {
        int Val;
        const char *szStr;
    } rgszConsts[] =
    {
        // Pixel Shaders
        XTAG(PS_TEXTUREMODES_NONE), XTAG(PS_TEXTUREMODES_PROJECT2D), XTAG(PS_TEXTUREMODES_PROJECT3D),
        XTAG(PS_TEXTUREMODES_CUBEMAP), XTAG(PS_TEXTUREMODES_PASSTHRU), XTAG(PS_TEXTUREMODES_CLIPPLANE),
        XTAG(PS_TEXTUREMODES_BUMPENVMAP), XTAG(PS_TEXTUREMODES_BUMPENVMAP_LUM), XTAG(PS_TEXTUREMODES_BRDF),
        XTAG(PS_TEXTUREMODES_DOT_ST), XTAG(PS_TEXTUREMODES_DOT_ZW), XTAG(PS_TEXTUREMODES_DOT_RFLCT_DIFF),
        XTAG(PS_TEXTUREMODES_DOT_RFLCT_SPEC), XTAG(PS_TEXTUREMODES_DOT_STR_3D), XTAG(PS_TEXTUREMODES_DOT_STR_CUBE),
        XTAG(PS_TEXTUREMODES_DPNDNT_AR), XTAG(PS_TEXTUREMODES_DPNDNT_GB), XTAG(PS_TEXTUREMODES_DOTPRODUCT),
        XTAG(PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST), XTAG(PS_COMPAREMODE_S_LT), XTAG(PS_COMPAREMODE_S_GE),
        XTAG(PS_COMPAREMODE_T_LT), XTAG(PS_COMPAREMODE_T_GE), XTAG(PS_COMPAREMODE_R_LT),
        XTAG(PS_COMPAREMODE_R_GE), XTAG(PS_COMPAREMODE_Q_LT), XTAG(PS_COMPAREMODE_Q_GE),
        XTAG(PS_COMBINERCOUNT_MUX_LSB), XTAG(PS_COMBINERCOUNT_MUX_MSB), XTAG(PS_COMBINERCOUNT_SAME_C0),
        XTAG(PS_COMBINERCOUNT_UNIQUE_C0), XTAG(PS_COMBINERCOUNT_SAME_C1), XTAG(PS_COMBINERCOUNT_UNIQUE_C1),
        XTAG(PS_INPUTMAPPING_UNSIGNED_IDENTITY), XTAG(PS_INPUTMAPPING_UNSIGNED_INVERT), XTAG(PS_INPUTMAPPING_EXPAND_NORMAL),
        XTAG(PS_INPUTMAPPING_EXPAND_NEGATE), XTAG(PS_INPUTMAPPING_HALFBIAS_NORMAL), XTAG(PS_INPUTMAPPING_HALFBIAS_NEGATE),
        XTAG(PS_INPUTMAPPING_SIGNED_IDENTITY), XTAG(PS_INPUTMAPPING_SIGNED_NEGATE), XTAG(PS_REGISTER_ZERO),
        XTAG(PS_REGISTER_DISCARD), XTAG(PS_REGISTER_C0), XTAG(PS_REGISTER_C1),
        XTAG(PS_REGISTER_FOG), XTAG(PS_REGISTER_V0), XTAG(PS_REGISTER_V1),
        XTAG(PS_REGISTER_T0), XTAG(PS_REGISTER_T1), XTAG(PS_REGISTER_T2),
        XTAG(PS_REGISTER_T3), XTAG(PS_REGISTER_R0), XTAG(PS_REGISTER_R1),
        XTAG(PS_REGISTER_V1R0_SUM), XTAG(PS_REGISTER_EF_PROD), XTAG(PS_REGISTER_ONE),
        XTAG(PS_REGISTER_NEGATIVE_ONE), XTAG(PS_REGISTER_ONE_HALF), XTAG(PS_REGISTER_NEGATIVE_ONE_HALF),
        XTAG(PS_CHANNEL_RGB), XTAG(PS_CHANNEL_BLUE), XTAG(PS_CHANNEL_ALPHA),
        XTAG(PS_FINALCOMBINERSETTING_CLAMP_SUM), XTAG(PS_FINALCOMBINERSETTING_COMPLEMENT_V1), XTAG(PS_FINALCOMBINERSETTING_COMPLEMENT_R0),
        XTAG(PS_COMBINEROUTPUT_IDENTITY), XTAG(PS_COMBINEROUTPUT_BIAS), XTAG(PS_COMBINEROUTPUT_SHIFTLEFT_1),
        XTAG(PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS), XTAG(PS_COMBINEROUTPUT_SHIFTLEFT_2), XTAG(PS_COMBINEROUTPUT_SHIFTRIGHT_1),
        XTAG(PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA), XTAG(PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA), XTAG(PS_COMBINEROUTPUT_AB_MULTIPLY),
        XTAG(PS_COMBINEROUTPUT_AB_DOT_PRODUCT), XTAG(PS_COMBINEROUTPUT_CD_MULTIPLY), XTAG(PS_COMBINEROUTPUT_CD_DOT_PRODUCT),
        XTAG(PS_COMBINEROUTPUT_AB_CD_SUM), XTAG(PS_COMBINEROUTPUT_AB_CD_MUX),
        XTAG(PS_DOTMAPPING_ZERO_TO_ONE), XTAG(PS_DOTMAPPING_MINUS1_TO_1_D3D), XTAG(PS_DOTMAPPING_MINUS1_TO_1_GL),
        XTAG(PS_DOTMAPPING_MINUS1_TO_1), XTAG(PS_DOTMAPPING_HILO_1), XTAG(PS_DOTMAPPING_HILO_HEMISPHERE_D3D), XTAG(PS_DOTMAPPING_HILO_HEMISPHERE_GL),
        XTAG(PS_DOTMAPPING_HILO_HEMISPHERE),

        // D3DFORMATs
        XTAG(D3DFMT_A8R8G8B8), XTAG(D3DFMT_X8R8G8B8), XTAG(D3DFMT_R5G6B5), XTAG(D3DFMT_R6G5B5),
        XTAG(D3DFMT_X1R5G5B5), XTAG(D3DFMT_A1R5G5B5), XTAG(D3DFMT_A4R4G4B4), XTAG(D3DFMT_A8),
        XTAG(D3DFMT_A8B8G8R8), XTAG(D3DFMT_B8G8R8A8), XTAG(D3DFMT_R4G4B4A4), XTAG(D3DFMT_R5G5B5A1),
        XTAG(D3DFMT_R8G8B8A8), XTAG(D3DFMT_R8B8), XTAG(D3DFMT_G8B8), XTAG(D3DFMT_P8),
        XTAG(D3DFMT_L8), XTAG(D3DFMT_A8L8), XTAG(D3DFMT_AL8), XTAG(D3DFMT_L16),
        XTAG(D3DFMT_V8U8), XTAG(D3DFMT_L6V5U5), XTAG(D3DFMT_X8L8V8U8), XTAG(D3DFMT_Q8W8V8U8),
        XTAG(D3DFMT_V16U16), XTAG(D3DFMT_D16_LOCKABLE), XTAG(D3DFMT_D16), XTAG(D3DFMT_D24S8),
        XTAG(D3DFMT_F16), XTAG(D3DFMT_F24S8), XTAG(D3DFMT_UYVY), XTAG(D3DFMT_YUY2),
        XTAG(D3DFMT_DXT1), XTAG(D3DFMT_DXT2), XTAG(D3DFMT_DXT3), XTAG(D3DFMT_DXT4),
        XTAG(D3DFMT_DXT5), XTAG(D3DFMT_LIN_A1R5G5B5), XTAG(D3DFMT_LIN_A4R4G4B4), XTAG(D3DFMT_LIN_A8),
        XTAG(D3DFMT_LIN_A8B8G8R8), XTAG(D3DFMT_LIN_A8R8G8B8), XTAG(D3DFMT_LIN_B8G8R8A8), XTAG(D3DFMT_LIN_G8B8),
        XTAG(D3DFMT_LIN_R4G4B4A4), XTAG(D3DFMT_LIN_R5G5B5A1), XTAG(D3DFMT_LIN_R5G6B5), XTAG(D3DFMT_LIN_R6G5B5),
        XTAG(D3DFMT_LIN_R8B8), XTAG(D3DFMT_LIN_R8G8B8A8), XTAG(D3DFMT_LIN_X1R5G5B5), XTAG(D3DFMT_LIN_X8R8G8B8),
        XTAG(D3DFMT_LIN_A8L8), XTAG(D3DFMT_LIN_AL8), XTAG(D3DFMT_LIN_L16), XTAG(D3DFMT_LIN_L8),
        XTAG(D3DFMT_LIN_D24S8), XTAG(D3DFMT_LIN_F24S8), XTAG(D3DFMT_LIN_D16), XTAG(D3DFMT_LIN_F16),

        XTAG(D3DMULTISAMPLE_NONE), XTAG(D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX), XTAG(D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR),
        XTAG(D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR), XTAG(D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN),
        XTAG(D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT),
        XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5),
        XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8),

        XTAG(D3DMS_NONE), XTAG(D3DMS_2_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMS_2_SAMPLES_MULTISAMPLE_QUINCUNX), XTAG(D3DMS_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR),
        XTAG(D3DMS_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR), XTAG(D3DMS_4_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMS_4_SAMPLES_MULTISAMPLE_GAUSSIAN), XTAG(D3DMS_4_SAMPLES_SUPERSAMPLE_LINEAR),
        XTAG(D3DMS_4_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMS_9_SAMPLES_MULTISAMPLE_GAUSSIAN),
        XTAG(D3DMS_9_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMS_PREFILTER_FORMAT_DEFAULT),
        XTAG(D3DMS_PREFILTER_FORMAT_X1R5G5B5), XTAG(D3DMS_PREFILTER_FORMAT_R5G6B5),
        XTAG(D3DMS_PREFILTER_FORMAT_X8R8G8B8), XTAG(D3DMS_PREFILTER_FORMAT_A8R8G8B8),

        XTAG(D3DCLEAR_TARGET), XTAG(D3DCLEAR_ZBUFFER), XTAG(D3DCLEAR_STENCIL),

        // FVFs
        XTAG(D3DFVF_XYZ), XTAG(D3DFVF_XYZRHW), XTAG(D3DFVF_NORMAL), XTAG(D3DFVF_DIFFUSE),
        XTAG(D3DFVF_SPECULAR), XTAG(D3DFVF_TEX0), XTAG(D3DFVF_TEX1), XTAG(D3DFVF_TEX2),
        XTAG(D3DFVF_TEX3), XTAG(D3DFVF_TEX4),

        // textcoord sizes
        XTAG(T0_SIZE1), XTAG(T0_SIZE2), XTAG(T0_SIZE3), XTAG(T0_SIZE4),
        XTAG(T1_SIZE1), XTAG(T1_SIZE2), XTAG(T1_SIZE3), XTAG(T1_SIZE4),
        XTAG(T2_SIZE1), XTAG(T2_SIZE2), XTAG(T2_SIZE3), XTAG(T2_SIZE4),
        XTAG(T3_SIZE1), XTAG(T3_SIZE2), XTAG(T3_SIZE3), XTAG(T3_SIZE4),

        // D3DCMPs
        XTAG(D3DCMP_NEVER), XTAG(D3DCMP_LESS), XTAG(D3DCMP_EQUAL), XTAG(D3DCMP_LESSEQUAL),
        XTAG(D3DCMP_GREATER), XTAG(D3DCMP_NOTEQUAL), XTAG(D3DCMP_GREATEREQUAL), XTAG(D3DCMP_ALWAYS),

        XTAG(D3DTEXF_NONE), XTAG(D3DTEXF_POINT), XTAG(D3DTEXF_LINEAR), XTAG(D3DTEXF_ANISOTROPIC),
        XTAG(D3DTEXF_QUINCUNX), XTAG(D3DTEXF_GAUSSIANCUBIC),

        XTAG(TEX_None), XTAG(TEX_2d), XTAG(TEX_Cubemap), XTAG(TEX_Volume),

        XTAG(TIME_Present), XTAG(TIME_Render),

        // swaths
        XTAG(D3DSWATH_8), XTAG(D3DSWATH_16), XTAG(D3DSWATH_32),
        XTAG(D3DSWATH_64), XTAG(D3DSWATH_128), XTAG(D3DSWATH_OFF),

        XTAG(D3DCOLORWRITEENABLE_RED), XTAG(D3DCOLORWRITEENABLE_GREEN), XTAG(D3DCOLORWRITEENABLE_BLUE),
        XTAG(D3DCOLORWRITEENABLE_ALPHA), XTAG(D3DCOLORWRITEENABLE_ALL),

        // misc
        XTAG(FALSE), XTAG(TRUE)
    };

    for(int ifmt = 0; ifmt < ARRAYSIZE(rgszConsts); ifmt++)
    {
        if(!_strnicmp(rgszConsts[ifmt].szStr, szStr, cchStr))
        {
            // set val
            *pval = rgszConsts[ifmt].Val;
            return true;
        }
    }

    return false;
}
#endif 0