//-----------------------------------------------------------------------------
// File: GearModel.h
//
// Desc: Model of a gear
//
// Hist: 02.19.00 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef GEARMODEL_H
#define GEARMODEL_H


    

//-----------------------------------------------------------------------------
// Name: class CGearMesh
// Desc: 
//-----------------------------------------------------------------------------
class CGearMesh
{
protected:
    LPDIRECT3DVERTEXBUFFER8 m_pBodyVB;
    LPDIRECT3DINDEXBUFFER8  m_pBodyIB;
    DWORD      m_dwNumBodyVertices;
    DWORD      m_dwNumBodyIndices;

    LPDIRECT3DVERTEXBUFFER8 m_pTeethVB;
    DWORD      m_dwNumTeethVertices;

public:
    FLOAT      m_fRotationSpeed;
    FLOAT      m_fRotationAngle;
    D3DXMATRIX m_matPosition;
    D3DXMATRIX m_matRotation;

public:
    virtual HRESULT Create( LPDIRECT3DDEVICE8 pd3dDevice, FLOAT fRadius, 
                            FLOAT fRotationSpeed, DWORD dwNumTeeth,
                            FLOAT fRotationOffset );
    virtual HRESULT FrameMove( LPDIRECT3DDEVICE8 pd3dDevice, FLOAT fTime );
    virtual HRESULT Render( LPDIRECT3DDEVICE8 pd3dDevice );
};




//-----------------------------------------------------------------------------
// Name: class CBumpyGearMesh
// Desc: 
//-----------------------------------------------------------------------------
class CBumpyGearMesh : public CGearMesh
{
    LPDIRECT3DVERTEXBUFFER8 m_pBodyBasisVB;
    LPDIRECT3DVERTEXBUFFER8 m_pTeethBasisVB;

public:
    HRESULT Create( LPDIRECT3DDEVICE8 pd3dDevice, FLOAT fRadius, 
                    FLOAT fRotationSpeed, DWORD dwNumTeeth,
                    FLOAT fRotationOffset );
    HRESULT Render( LPDIRECT3DDEVICE8 pd3dDevice );
};




#endif // GEARMODEL_H



