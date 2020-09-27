//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Cubemap.h"

bool CCubemap::BltCubicTexture(void)
{
/*
	// Blt from source sysmem to vidmem method
    POINT Pt = {0,0};

	for (int i=0;i<6;i++) 
    {
    	for (int j=0;j<m_paSysTexture->m_pSrcSurface->GetNumAttachedSurfaces();j++) 
        {
            CDirectDrawSurface * pSurface = m_paTexture->m_pSrcSurface->GetCubicSurface(i)->GetAttachedSurface(j);
            CDirectDrawSurface * pSysSurface = m_paSysTexture->m_pSrcSurface->GetCubicSurface(i)->GetAttachedSurface(j);

            // Blt from source sysmem to source target method
            if (!pSurface->Blt(pSysSurface,Pt))
	        {
		        WriteToLog("Source Blt(%d) failed with HResult = %s.",i,GetHResultString(GetLastError()).c_str());
                RELEASE(pSurface);
                RELEASE(pSysSurface);
		        return false;
	        }

            RELEASE(pSurface);
            RELEASE(pSysSurface);

	        if (NULL != m_pRefTarget)
	        {
                CDirectDrawSurface * pSurface = m_paTexture->m_pRefSurface->GetCubicSurface(i)->GetAttachedSurface(j);
                CDirectDrawSurface * pSysSurface = m_paSysTexture->m_pRefSurface->GetCubicSurface(i)->GetAttachedSurface(j);

	            // Blt from ref sysmem to ref target method
		        if (!pSurface->Blt(pSysSurface,Pt))
		        {
			        WriteToLog("Reference Blt(%d) failed with HResult = %s.",i,GetHResultString(GetLastError()).c_str());
                    RELEASE(pSurface);
                    RELEASE(pSysSurface);
			        return false;
		        }

                RELEASE(pSurface);
                RELEASE(pSysSurface);
	        }
        }
    }
*/
    return true;
}

