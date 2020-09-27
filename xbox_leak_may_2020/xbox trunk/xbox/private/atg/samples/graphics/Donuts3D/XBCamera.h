//-----------------------------------------------------------------------------
// File: XBCamera.h
//
// Desc: Camera class.
//
// Copyright (C) Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>




//-----------------------------------------------------------------------------
// Name: class CXBCamera
// Desc:
//-----------------------------------------------------------------------------
class CXBCamera
{
    D3DXVECTOR3 m_vEyePt;       // Attributes for view matrix
    D3DXVECTOR3 m_vLookatPt;
    D3DXVECTOR3 m_vUpVec;
    D3DXVECTOR3 m_vView;
    D3DXVECTOR3 m_vCross;

    FLOAT       m_fFOV;         // Attributes for projection matrix
    FLOAT       m_fAspect;
    FLOAT       m_fNearPlane;
    FLOAT       m_fFarPlane;

    D3DXMATRIX  m_matView;      // Resulting view matrix
    D3DXMATRIX  m_matBillboard; // Special view matrix for billboards
    D3DXMATRIX  m_matProj;      // Resulting projection matrix

public:
    //-------------------------------------------------------------------------
    // Access functions
    //-------------------------------------------------------------------------
    D3DXVECTOR3 GetEyePt()           { return m_vEyePt; }
    D3DXVECTOR3 GetLookatPt()        { return m_vLookatPt; }
    D3DXVECTOR3 GetUpVec()           { return m_vUpVec; }
    D3DXVECTOR3 GetViewDir()         { return m_vView; }
    D3DXVECTOR3 GetCross()           { return m_vCross; }

    D3DXMATRIX  GetViewMatrix()      { return m_matView; }
    D3DXMATRIX  GetBillboardMatrix() { return m_matBillboard; }
    D3DXMATRIX  GetProjMatrix()      { return m_matProj; }

    //-------------------------------------------------------------------------
    // Set attributes for the view matrix
    //-------------------------------------------------------------------------
    VOID SetViewParams( D3DXVECTOR3 &vEyePt, D3DXVECTOR3& vLookatPt,
                        D3DXVECTOR3& vUpVec )
    {
        m_vEyePt    = vEyePt;
        m_vLookatPt = vLookatPt;
        m_vUpVec    = vUpVec;
        D3DXVec3Normalize( &m_vView, &(m_vLookatPt - m_vEyePt) );
        D3DXVec3Cross( &m_vCross, &m_vView, &m_vUpVec );
        D3DXMatrixLookAtLH( &m_matView, &m_vEyePt, &m_vLookatPt, &m_vUpVec );
        D3DXMatrixInverse( &m_matBillboard, NULL, &m_matView );
        m_matBillboard._41 = 0.0f;
        m_matBillboard._42 = 0.0f;
        m_matBillboard._43 = 0.0f;
    }

    //-------------------------------------------------------------------------
    // Set attributes for the projection matrix
    //-------------------------------------------------------------------------
    VOID SetProjParams( FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane,
                        FLOAT fFarPlane )
    {
        m_fFOV        = fFOV;
        m_fAspect     = fAspect;
        m_fNearPlane  = fNearPlane;
        m_fFarPlane   = fFarPlane;
        D3DXMatrixPerspectiveFovLH( &m_matProj, fFOV, fAspect, fNearPlane, fFarPlane );
    }

    //-------------------------------------------------------------------------
    // Constructor
    //-------------------------------------------------------------------------
    CXBCamera()
    {
        SetViewParams( D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3(0.0f,0.0f,1.0f),
                       D3DXVECTOR3(0.0f,1.0f,0.0f) );
        SetProjParams( D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
    }
};




