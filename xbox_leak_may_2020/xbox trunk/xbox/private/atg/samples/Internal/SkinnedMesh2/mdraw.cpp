//-----------------------------------------------------------------------------
// File: mdraw.cpp
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <XBApp.h>
#include <XBFont.h>
#include <XBResource.h>
#include <XBUtil.h>
#include "SkinnedMesh.h"


HRESULT CSkinnedMesh::DrawMeshContainer(SMeshContainer* pMeshContainer)
{
    UINT ipattr;
    HRESULT hr = S_OK;
    LPD3DXBONECOMBINATION pBoneComb;

    if( pMeshContainer->m_pSkinMesh )
    {
        D3DCAPS8    caps;

        m_pd3dDevice->GetDeviceCaps(&caps);

        if( m_method != pMeshContainer->m_Method )
        {
            GenerateMesh(pMeshContainer);
        }

        if( m_method == SKIN_NONINDEXED )
        {
            m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, pMeshContainer->m_maxFaceInfl - 1);
            pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_pBoneCombinationBuf->GetBufferPointer());
            for( ipattr = 0; ipattr < pMeshContainer->m_cpattr; ipattr++ )
            {
                for( DWORD i = 0; i < pMeshContainer->m_maxFaceInfl; i++ )
                {
                    DWORD matid = pBoneComb[ipattr].BoneId[i];
                    if( matid != UINT_MAX && (ipattr == 0 || matid != pBoneComb[ipattr - 1].BoneId[i]) )
                    {
                        m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(i), pMeshContainer->m_pBoneMatrix[matid]);
                        m_pd3dDevice->MultiplyTransform(D3DTS_WORLDMATRIX(i), &pMeshContainer->m_pBoneOffsetMat[matid]);
                    }
                }

                if( ipattr == 0 || (pBoneComb[ipattr].AttribId != pBoneComb[ipattr - 1].AttribId) )
                {
                    m_pd3dDevice->SetMaterial(&(pMeshContainer->m_pMaterials[pBoneComb[ipattr].AttribId]));
                    m_pd3dDevice->SetTexture(0, pMeshContainer->m_pTextures[pBoneComb[ipattr].AttribId]);
                }

		        hr = pMeshContainer->m_pMesh->DrawSubset( ipattr );
                if(FAILED(hr))
                    return hr;
            }
            m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);
		}
        else if( m_method == SKIN_INDEXED )
        {
            if (caps.MaxVertexBlendMatrixIndex < pMeshContainer->m_pSkinMesh->GetNumBones())
            {
/*
NOT ON XBOX
                m_pd3dDevice->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, TRUE);
*/
            }

            m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, pMeshContainer->m_maxFaceInfl - 1);
            if (pMeshContainer->m_maxFaceInfl - 1)
			{
/*
NOT ON XBOX
                m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
*/
			}
            pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_pBoneCombinationBuf->GetBufferPointer());
            for( ipattr = 0; ipattr < pMeshContainer->m_cpattr; ipattr++ )
            {
                for( DWORD i = 0; i < 255/*paletteSize*/; i++ )
                {
                    DWORD matid = pBoneComb[ipattr].BoneId[i];
                    if( matid != UINT_MAX )
                    {
                        m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(i), pMeshContainer->m_pBoneMatrix[matid]);
                        m_pd3dDevice->MultiplyTransform(D3DTS_WORLDMATRIX(i), &pMeshContainer->m_pBoneOffsetMat[matid]);
                    }
                }
                
                m_pd3dDevice->SetMaterial(&(pMeshContainer->m_pMaterials[pBoneComb[ipattr].AttribId]));
                m_pd3dDevice->SetTexture(0, pMeshContainer->m_pTextures[pBoneComb[ipattr].AttribId]);

                hr = pMeshContainer->m_pMesh->DrawSubset( ipattr );
                if(FAILED(hr))
                    return hr;
            }
/*
NOT ON XBOX
            m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
*/
            m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);

            if (caps.MaxVertexBlendMatrixIndex < pMeshContainer->m_pSkinMesh->GetNumBones())
            {
/*
NOT ON XBOX
                m_pd3dDevice->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, FALSE);
*/
            }
        }
    }
    else
    {
        for( ipattr = 0; ipattr < pMeshContainer->m_cpattr; ipattr++ )
        {
            m_pd3dDevice->SetMaterial(&(pMeshContainer->m_pMaterials[ipattr]));
            m_pd3dDevice->SetTexture(0, pMeshContainer->m_pTextures[ipattr]);
		    hr = pMeshContainer->m_pMesh->DrawSubset( ipattr );
            if(FAILED(hr))
                return hr;
        }
    }

    return S_OK;
}




HRESULT CSkinnedMesh::UpdateFrames( SFrame* pCurrentFrame, D3DXMATRIX &matCur )
{
    HRESULT hr;

    pCurrentFrame->m_matCombined = matCur;
    D3DXMatrixMultiply( &pCurrentFrame->m_matCombined, &pCurrentFrame->m_matRot, &matCur );
    D3DXMatrixMultiply( &pCurrentFrame->m_matCombined, &pCurrentFrame->m_matCombined, &pCurrentFrame->m_matTrans );
    
	for( SFrame* pChildFrame = pCurrentFrame->m_pChild; pChildFrame; pChildFrame = pChildFrame->m_pSibling )
    {
        hr = UpdateFrames( pChildFrame, pCurrentFrame->m_matCombined );
        if( FAILED(hr) )
            return hr;
    }
    return S_OK;
}




HRESULT CSkinnedMesh::DrawFrames( SFrame* pCurrentFrame, UINT& dwNumTriangles )
{
    HRESULT hr;

    if( pCurrentFrame->m_pMeshContainer )
    {
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &pCurrentFrame->m_matCombined );
    }

    SMeshContainer* pMeshContainer = pCurrentFrame->m_pMeshContainer;
    while( pMeshContainer )
    {
        hr = DrawMeshContainer( pMeshContainer );
        if( FAILED(hr) )
            return hr;

        dwNumTriangles += pMeshContainer->m_pMesh->GetNumFaces();

        pMeshContainer = pMeshContainer->m_pNext;
    }

    SFrame* pChildFrame = pCurrentFrame->m_pChild;
    while( pChildFrame )
    {
        hr = DrawFrames( pChildFrame, dwNumTriangles );
        if( FAILED(hr) )
            return hr;

        pChildFrame = pChildFrame->m_pSibling;
    }

    return S_OK;
}




void SFrame::SetTime( float fGlobalTime )
{
    UINT        iKey;
    UINT        dwp2;
    UINT        dwp3;
    D3DXMATRIX  matResult;
    D3DXMATRIX  matTemp;
    float       fTime1;
    float       fTime2;
    float       fLerpValue;
    D3DXVECTOR3 vScale;
    D3DXVECTOR3 vPos;
    D3DXQUATERNION quat;
    BOOL  bAnimate = false;
    float fTime;

    if( m_pMatrixKeys )
    {
        fTime = fmodf( fGlobalTime, (float)m_pMatrixKeys[m_dwNumMatrixKeys-1].dwTime );

        for( iKey = 0; iKey < m_dwNumMatrixKeys; iKey++ )
        {
            if( (float)m_pMatrixKeys[iKey].dwTime > fTime )
            {
                dwp3 = iKey;

                if( iKey > 0 )
                    dwp2= iKey - 1;
                else  // when iKey == 0, then dwp2 == 0
                    dwp2 = iKey;
                break;
            }
        }
        fTime1 = (float)m_pMatrixKeys[dwp2].dwTime;
        fTime2 = (float)m_pMatrixKeys[dwp3].dwTime;

        if( (fTime2 - fTime1) ==0 )
            fLerpValue = 0;
        else
            fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

        if( fLerpValue > 0.5f )
            iKey = dwp3;
        else
            iKey = dwp2;

        m_pframeToAnimate->m_matRot = m_pMatrixKeys[iKey].mat;
    }
    else
    {
        D3DXMatrixIdentity( &matResult );

        if( m_pScaleKeys )
        {
            dwp2 = dwp3 = 0;

            fTime = (float)fmod(fGlobalTime, m_pScaleKeys[m_dwNumScaleKeys-1].dwTime);

            for( iKey = 0 ;iKey < m_dwNumScaleKeys; iKey++ )
            {
                if( (float)m_pScaleKeys[iKey].dwTime > fTime )
                {
                    dwp3 = iKey;

                    if( iKey > 0 )
                        dwp2= iKey - 1;
                    else  // when iKey == 0, then dwp2 == 0
                        dwp2 = iKey;

                    break;
                }
            }
            fTime1 = (float)m_pScaleKeys[dwp2].dwTime;
            fTime2 = (float)m_pScaleKeys[dwp3].dwTime;

            if( (fTime2 - fTime1) ==0 )
                fLerpValue = 0;
            else
                fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

            D3DXVec3Lerp( &vScale, &m_pScaleKeys[dwp2].vScale,
                                   &m_pScaleKeys[dwp3].vScale, fLerpValue );

            D3DXMatrixScaling(&matTemp, vScale.x, vScale.y, vScale.z);

            D3DXMatrixMultiply(&matResult, &matResult, &matTemp);

            bAnimate = true;
        }

        // Check rotate keys
        if( m_pRotateKeys )
        {
            dwp2 = dwp3 = 0;

            fTime = (float)fmod(fGlobalTime, m_pRotateKeys[m_dwNumRotateKeys-1].dwTime);

            for( iKey = 0 ;iKey < m_dwNumRotateKeys ; iKey++ )
            {
                if( (float)m_pRotateKeys[iKey].dwTime > fTime )
                {
                    dwp3 = iKey;

                    if( iKey > 0 )
                        dwp2 = iKey - 1;
                    else  // when iKey == 0, then dwp2 == 0
                        dwp2 = iKey;
                    break;
                }
            }
            fTime1 = (float)m_pRotateKeys[dwp2].dwTime;
            fTime2 = (float)m_pRotateKeys[dwp3].dwTime;

            if( (fTime2 - fTime1) ==0 )
                fLerpValue = 0;
            else
                fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

            //s=0;
            D3DXQUATERNION q1,q2;
            q1.x = -m_pRotateKeys[dwp2].quatRotate.x;
            q1.y = -m_pRotateKeys[dwp2].quatRotate.y;
            q1.z = -m_pRotateKeys[dwp2].quatRotate.z;
            q1.w =  m_pRotateKeys[dwp2].quatRotate.w;

            q2.x = -m_pRotateKeys[dwp3].quatRotate.x;
            q2.y = -m_pRotateKeys[dwp3].quatRotate.y;
            q2.z = -m_pRotateKeys[dwp3].quatRotate.z;
            q2.w =  m_pRotateKeys[dwp3].quatRotate.w;

            D3DXQuaternionSlerp( &quat, &q1, &q2, fLerpValue );

            D3DXMatrixRotationQuaternion(&matTemp, &quat);

            D3DXMatrixMultiply(&matResult, &matResult, &matTemp);
            bAnimate = true;
        }

        if( m_pPositionKeys )
        {
            dwp2=dwp3=0;

            fTime = (float)fmod(fGlobalTime, m_pPositionKeys[m_dwNumRotateKeys-1].dwTime);

            for( iKey = 0 ;iKey < m_dwNumPositionKeys ; iKey++ )
            {
                if( (float)m_pPositionKeys[iKey].dwTime > fTime )
                {
                    dwp3 = iKey;

                    if( iKey > 0 )
                        dwp2 = iKey - 1;
                    else  // when iKey == 0, then dwp2 == 0
                        dwp2 = iKey;

                    break;
                }
            }
            fTime1 = (float)m_pPositionKeys[dwp2].dwTime;
            fTime2 = (float)m_pPositionKeys[dwp3].dwTime;

            if ((fTime2 - fTime1) ==0)
                fLerpValue = 0;
            else
                fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

            D3DXVec3Lerp( (D3DXVECTOR3*)&vPos, &m_pPositionKeys[dwp2].vPos,
                                               &m_pPositionKeys[dwp3].vPos, fLerpValue );

            D3DXMatrixTranslation( &matTemp, vPos.x, vPos.y, vPos.z );

            D3DXMatrixMultiply( &matResult, &matResult, &matTemp );
            bAnimate = true;
        }
        else
        {
            D3DXMatrixTranslation( &matTemp, m_pframeToAnimate->m_matRotOrig._41, 
				                             m_pframeToAnimate->m_matRotOrig._42, 
											 m_pframeToAnimate->m_matRotOrig._43 );

            D3DXMatrixMultiply( &matResult, &matResult, &matTemp );
        }

        if( bAnimate )
        {
            m_pframeToAnimate->m_matRot = matResult;
        }
    }
}




HRESULT CSkinnedMesh::Render()
{
	UINT   dwNumTriangles = 0;
	HRESULT hr;

	for( SDrawElement* pDrawElement = m_pdeHead; pDrawElement; pDrawElement = pDrawElement->m_pNext )
    {
        hr = DrawFrames( pDrawElement->m_pRootFrame, dwNumTriangles );
        if( FAILED(hr) )
            return hr;
    }

	return S_OK;
}


