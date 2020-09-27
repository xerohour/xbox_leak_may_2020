#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "texgen.h"

bool TexGen::SetProperties()
{
    D3DMATRIX   TMatrix;
    D3DMATRIX   VMatrix = IdentityMatrix();
    D3DMATRIX   PMatrix = OrthoMatrix(1.f, 1.f, -0.5f, 0.5f);
    DWORD       num = m_dwCurrentTestNumber-1;
    DWORD       tnum = m_dwCurrentTestNumber-1;
    DWORD       tmp;
    
    
    // Draw the same picture a number of time to cover different APIs
    num /= m_dwSameTestNumber;
    
    // Cycle throw texgen types
    m_dwTestType = num % 5;
    num /= 5;
    if (0 == m_dwTestType)
    {
        // No texgen
        InitMatrix(&TMatrix,
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f
            );

        m_dwTexgenType = D3DTSS_TCI_PASSTHRU;
        m_pszTestName = "None";
    }
    else if (1 == m_dwTestType)
    {
        // Positional
        InitMatrix(&TMatrix,
                0.0f,    1.0f,  0.0f, 0.0f,
                1.0f,    0.0f,   0.0f, 0.0f,
                0.0f,    0.0f,   0.0f, 0.0f,
                0.501f,  0.501f, 0.0f, 0.0f
            );

        m_dwTexgenType = D3DTSS_TCI_CAMERASPACEPOSITION;
        m_pszTestName = "Position";
    }
    else if (2 == m_dwTestType)
    {
        // Normal
        InitMatrix(&TMatrix,
                0.0f, 0.5f, 0.0f, 0.0f,
                0.5f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.5f, 0.5f, 0.0f, 0.0f
            );

        m_dwTexgenType = D3DTSS_TCI_CAMERASPACENORMAL;
        m_pszTestName = "Normal";
    }
    else if (3 == m_dwTestType)
    {
	    // Localviewer reflection vector
        D3DVECTOR   from = cD3DVECTOR(0.0f, 0.0f, -1.0f); 
        D3DVECTOR	at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
        D3DVECTOR	up = cD3DVECTOR(0.0f, 1.0f, 0.0f);

        InitMatrix(&TMatrix, 
                0.0f, 0.5f, 0.0f, 0.0f,
                0.5f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.5f, 0.5f, 0.0f, 0.0f
            );

        VMatrix = ViewMatrix(from, at, up);
        PMatrix = OrthoMatrix(1.f, 1.f, 0.5f, 1.5f);

        m_dwTexgenType = D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
        m_pszTestName = "Reflection vector (local viewer)";
        SetRenderState(D3DRENDERSTATE_LOCALVIEWER, (DWORD)TRUE);
    }
    else
    {
        // Non-localviewer reflection vector
	    D3DVECTOR   from = cD3DVECTOR(0.0f, 0.0f, -1.0f); 
        D3DVECTOR	at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
        D3DVECTOR	up = cD3DVECTOR(0.0f, 1.0f, 0.0f);

        InitMatrix(&TMatrix,
                0.0f, 0.5f, 0.0f, 0.0f,
                0.5f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.5f, 0.5f, 0.0f, 0.0f
            );

        VMatrix = ViewMatrix(from, at, up);
        PMatrix = OrthoMatrix(1.f, 1.f, 0.5f, 1.5f);

        m_dwTexgenType = D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
        m_pszTestName = "Reflection vector (non-local viewer)";
        SetRenderState(D3DRENDERSTATE_LOCALVIEWER, (DWORD)FALSE);
    }

    // On / Off lighting
    tmp = (num & 0x01);
    num >>= 1;
    if (0 == tmp)
    {
        SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)FALSE);
    }
    else 
    {
        if (NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
        {
            SkipTests(tnum - tnum%m_dwSameTestNumber);
            return false;
        }
        SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)TRUE);
    }

    // Enable / disable vertex blending
    tmp = (num & 0x01);
    num >>= 1;
    if (0 == tmp)
    {
        SetRenderState(D3DRENDERSTATE_VERTEXBLEND, (DWORD)D3DVBLEND_DISABLE);
    }
    else
    {
        if (m_d3dcaps.MaxVertexBlendMatrices < 2)
        {
            SkipTests(tnum - tnum%m_dwSameTestNumber);
            return false;
        }
        SetRenderState(D3DRENDERSTATE_VERTEXBLEND, (DWORD)D3DVBLEND_1WEIGHT);
        if (m_dwTexgenType == D3DTSS_TCI_PASSTHRU)
            m_dwTexgenType = 1;
        else if (m_dwTexgenType == D3DTSS_TCI_CAMERASPACENORMAL)
            TMatrix._12 = 0.25f;
        else if (m_dwTexgenType == D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR)
            TMatrix._12 = 0.25f;
    }

    SetTransform(D3DTRANSFORMSTATE_TEXTURE0, &TMatrix);
    SetTransform(D3DTRANSFORMSTATE_VIEW, &VMatrix);
    SetTransform(D3DTRANSFORMSTATE_PROJECTION, &PMatrix);

    return true;
}

bool TexGen::CheckProjectedTextures(void)
{
    if (!(m_d3dcaps.TextureCaps & D3DPTEXTURECAPS_PROJECTED))
		return false;
    if (!(m_d3dcaps.TextureCaps & D3DPTEXTURECAPS_PROJECTED))
		return false;
	return true;
}
