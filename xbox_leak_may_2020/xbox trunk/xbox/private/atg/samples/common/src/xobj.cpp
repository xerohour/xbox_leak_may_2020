//-----------------------------------------------------------------------------
// File: xobj.cpp
//
// Desc: routines for displaying objects
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "xmodel.h"
#include "xobj.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

//-----------------------------------------------------------------------------
// Constructor and destructor
//-----------------------------------------------------------------------------
CXObject::CXObject()
{
    // clear out relevant fields

    m_Model = NULL;
    m_pXBMesh = NULL;

    SetPosition(0.0f, 0.0f, 0.0f);
    SetRotation(0.0f, 0.0f, 0.0f);
    D3DXMatrixIdentity(&m_matOrientation);
    m_Material = NULL;

    m_dwFlags = 0;
    m_dwType = 0;
    m_pvInfo = NULL;
}

CXObject::~CXObject()
{
    if(m_Model)
        m_Model->Release();
#ifdef _XBOX
    if(m_pXBMesh)
        m_pXBMesh->Release();
#endif
}

//-----------------------------------------------------------------------------
// Name: SetModel
// Desc: Set the model for this object.
//-----------------------------------------------------------------------------
void CXObject::SetModel(CXModel *model)
{
    if(m_Model)                         // release existing model
        m_Model->Release();
#ifdef _XBOX
    else if(m_pXBMesh)
    {
        m_pXBMesh->Release();
        m_pXBMesh = NULL;
    }
#endif

    if(model)
        model->AddRef();                // addref to new model
    m_Model = model;                    // set new model pointer
}


#ifdef _XBOX
//-----------------------------------------------------------------------------
// Name: SetXBG
// Desc: Set the XBMesh for this object.
//-----------------------------------------------------------------------------
void CXObject::SetXBMesh(CXBMesh *xbm)
{
    if(m_pXBMesh)
        m_pXBMesh->Release();
    else if(m_Model)
    {
        m_Model->Release();
        m_Model = NULL;
    }

    if(xbm)
        xbm->AddRef();                  // addref to new xbg
    m_pXBMesh = xbm;                    // set new model pointer
}
#endif

//-----------------------------------------------------------------------------
// Name: GetRadius functions
// Desc: Since objects can be either CXModel or CXBGMesh, we return a
//       radius based on which is active.
//-----------------------------------------------------------------------------
float CXObject::GetRadius()
{
    if(m_Model)
    {
        return m_Model->m_fRadius;
    }
    else if(m_pXBMesh)
    {
        return m_pXBMesh->ComputeRadius();
    }

    return 0.0f;
}

float CXObject::GetRadius2()
{
    if(m_Model)
    {
        return m_Model->m_fRadius2;
    }
    else if(m_pXBMesh)
    {
        float fRadius = m_pXBMesh->ComputeRadius();
        return fRadius*fRadius;
    }

    return 0.0f;
}

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Calculate an objects matrix, set its relevant material, and
//       render its model.
//-----------------------------------------------------------------------------
void CXObject::Render(int flags)
{
    // set the position and orientation of the object
    if(!(flags&OBJ_NOMCALCS))
        CrunchMatrix();

    g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_matOrientation);

    // set material
    if(m_Material)
        g_pd3dDevice->SetMaterial(m_Material);
    
    // render
#ifdef _XBOX
    if(m_Model)
        m_Model->Render();
    else if(m_pXBMesh)
        m_pXBMesh->Render(g_pd3dDevice, 0);
#else
    m_Model->Render();
#endif
}

//-----------------------------------------------------------------------------
// Name: CrunchMatrix
// Desc: Calculate an objects matrix.
//-----------------------------------------------------------------------------
void CXObject::CrunchMatrix()
{
    D3DXMATRIX m;

    D3DXMatrixRotationYawPitchRoll(&m_matOrientation, m_vRotation.y, m_vRotation.x, m_vRotation.z);
    D3DXMatrixTranslation(&m, m_vPosition.x, m_vPosition.y, m_vPosition.z);
    D3DXMatrixMultiply(&m_matOrientation, &m_matOrientation, &m);
}