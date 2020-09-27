//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "ZBuffer.h"

bool CZVerifyTest::VerifyFormat8(void)
{
    CSurface8* pd3ds;
    bool bError	= false;

    D3DSURFACE_DESC  Desc;
//	m_pSrcZBuffer->GetSurfaceDescription(&Desc);
    m_pDevice->GetDepthStencilSurface(&pd3ds);
    pd3ds->GetDesc(&Desc);
    pd3ds->Release();

   	// Report the format
    switch (Desc.Format)
    {
        case D3DFMT_D16:
            sprintf(msgString,"%sFormat: D3DFMT_D16, ",msgString);
            break;
        case D3DFMT_D24S8:
            sprintf(msgString,"%sFormat: D3DFMT_D24S8, ",msgString);
            break;
#ifndef UNDER_XBOX
        case D3DFMT_D16_LOCKABLE:  
            sprintf(msgString,"%sFormat: D3DFMT_D16_LOCKABLE, ",msgString);
            break;
        case D3DFMT_D32:
            sprintf(msgString,"%sFormat: D3DFMT_D32, ",msgString);
            break;
        case D3DFMT_D15S1:
            sprintf(msgString,"%sFormat: D3DFMT_D15S1, ",msgString);
            break;
        case D3DFMT_D24X4S4:
            sprintf(msgString,"%sFormat: D3DFMT_D24X4S4, ",msgString);
            break;
        case D3DFMT_D24X8:
            sprintf(msgString,"%sFormat: D3DFMT_D24X8, ",msgString);
            break;
#else
        case D3DFMT_F16:
            sprintf(msgString,"%sFormat: D3DFMT_F16, ",msgString);
            break;
        case D3DFMT_F24S8:
            sprintf(msgString,"%sFormat: D3DFMT_F24S8, ",msgString);
            break;
        case D3DFMT_LIN_D16:
            sprintf(msgString,"%sFormat: D3DFMT_LIN_D16, ",msgString);
            break;
        case D3DFMT_LIN_D24S8:
            sprintf(msgString,"%sFormat: D3DFMT_LIN_D24S8, ",msgString);
            break;
        case D3DFMT_LIN_F16:
            sprintf(msgString,"%sFormat: D3DFMT_LIN_F16, ",msgString);
            break;
        case D3DFMT_LIN_F24S8:
            sprintf(msgString,"%sFormat: D3DFMT_LIN_F24S8, ",msgString);
            break;
#endif
        default: 
            sprintf(msgString,"%sFormat: %d, ",msgString,Desc.Format);
            break;
    };

    sprintf(msgString,"%sUsage: 0x%x",msgString,Desc.Usage);

	// Check for invalid usage
    if (!(Desc.Usage & D3DUSAGE_DEPTHSTENCIL))
	{
		sprintf(errString,"%sError: D3DUSAGE_DEPTHSTENCIL not set on Z Buffer Usage!\n",errString);
		bError = true;
	}

	// Check for invalid formats
    if ((Desc.Format != D3DFMT_D16)     &&
#ifndef UNDER_XBOX
        (Desc.Format != D3DFMT_D16_LOCKABLE) &&
        (Desc.Format != D3DFMT_D32) &&
        (Desc.Format != D3DFMT_D15S1) &&
        (Desc.Format != D3DFMT_D24X4S4) &&
        (Desc.Format != D3DFMT_D24X8) &&
#else
        (Desc.Format != D3DFMT_F16) &&
        (Desc.Format != D3DFMT_F24S8) &&
        (Desc.Format != D3DFMT_LIN_D16) &&
        (Desc.Format != D3DFMT_LIN_D24S8) &&
        (Desc.Format != D3DFMT_LIN_F16) &&
        (Desc.Format != D3DFMT_LIN_F24S8) &&
#endif
        (Desc.Format != D3DFMT_D24S8))
	{
		sprintf(errString,"%sError: %d is an unknown Z Buffer Format!\n",errString,Desc.Format);
		bError = true;
	}

	return (!bError);
}

