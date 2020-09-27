//////////////////////////////////////////////////////////////////////////////
//
// File: mview.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MVIEW_H
#define MVIEW_H


enum SKINNING_METHOD
{
    SKIN_NONINDEXED,
    SKIN_INDEXED,
};


struct SMeshContainer
{
    ID3DXMesh*          m_pMesh;
    SMeshContainer*     m_pNext;
    char*               m_strName;

    D3DMATERIAL8*       m_pMaterials;
    LPDIRECT3DTEXTURE8* m_pTextures;
    DWORD               m_cpattr;
    DWORD               m_dwNumMaterials;
    DWORD               m_iAttrSplit;

    // Skin info
    LPD3DXSKINMESH      m_pSkinMesh;
    D3DXATTRIBUTERANGE* m_pAttrTable;
    D3DXMATRIX**        m_pBoneMatrix;
    LPD3DXBUFFER        m_pBoneNamesBuf;
    LPD3DXBUFFER        m_pBoneOffsetBuf;
    D3DXMATRIX*         m_pBoneOffsetMat;
    DWORD*              m_rgiAdjacency;
    DWORD               m_numBoneComb;
    DWORD               m_maxFaceInfl;
    LPD3DXBUFFER        m_pBoneCombinationBuf;
    SKINNING_METHOD     m_Method;

	SMeshContainer()
	{
		m_pMesh          = NULL;
		m_pMaterials     = NULL;
		m_pTextures      = NULL;
		m_cpattr         = 0;
		m_iAttrSplit     = 0;
		m_dwNumMaterials = 0;
		m_pNext          = NULL;
		m_strName        = NULL;
		m_pSkinMesh      = NULL;
		m_pAttrTable     = NULL;
		m_pBoneMatrix    = NULL;
		m_pBoneNamesBuf  = NULL;
		m_pBoneOffsetBuf = NULL;
		m_pBoneOffsetMat = NULL;
		m_rgiAdjacency   = NULL;
		m_numBoneComb    = 0;
		m_maxFaceInfl    = 0;
		m_pBoneCombinationBuf = NULL;
		m_Method         = SKIN_NONINDEXED;
	}

	~SMeshContainer()
	{
        SAFE_DELETE_ARRAY( m_pMaterials );
        SAFE_DELETE_ARRAY( m_pTextures );

		SAFE_RELEASE( m_pMesh );
        SAFE_RELEASE( m_pSkinMesh );
        SAFE_RELEASE( m_pBoneNamesBuf );
        SAFE_RELEASE( m_pBoneOffsetBuf );
        SAFE_RELEASE( m_pBoneCombinationBuf );

        SAFE_DELETE_ARRAY( m_pBoneMatrix );
        SAFE_DELETE_ARRAY( m_pAttrTable );
        SAFE_DELETE_ARRAY( m_strName );
        SAFE_DELETE_ARRAY( m_rgiAdjacency );
        SAFE_DELETE( m_pNext );
	}
};


// X File formation rotate key
struct SRotateKeyXFile
{
	DWORD		dwTime;
	DWORD		dwFloats;	
	float		w;
	float		x;
	float		y;
	float		z;
};


struct SScaleKeyXFile
{
	DWORD	    dwTime;
	DWORD	    dwFloats;	
	D3DXVECTOR3	vScale;	
};


struct SPositionKeyXFile
{
	DWORD	    dwTime;
	DWORD	    dwFloats;	
	D3DXVECTOR3	vPos;	
};


struct SMatrixKeyXFile
{
	DWORD	    dwTime;
	DWORD	    dwFloats;	
//	D3DXMATRIX	mat;	// Note: we cannot use D3DXMATRIX (which is typedef'ed
	                    //       as a 16-byte aligned XGMATRIX) since matrices
	                    //       read from the file are not 16-byte aligned.
	struct
	{
		FLOAT _11, _12, _13, _14;
		FLOAT _21, _22, _23, _24;
		FLOAT _31, _32, _33, _34;
		FLOAT _41, _42, _43, _44;
	} mat;
};


// in memory versions
struct SRotateKey
{
	DWORD			dwTime;
	D3DXQUATERNION	quatRotate;	
};


struct SPositionKey
{
	DWORD	    dwTime;
	D3DXVECTOR3	vPos;	
};


struct SScaleKey
{
	DWORD	    dwTime;
	D3DXVECTOR3	vScale;	
};


struct SMatrixKey
{
	DWORD	    dwTime;
	D3DXMATRIX	mat;	
};


struct SFrame
{
    SMeshContainer* m_pMeshContainer;
    D3DXMATRIX      m_matRot;
    D3DXMATRIX      m_matTrans;
    D3DXMATRIX      m_matRotOrig;
    D3DXMATRIX      m_matCombined;

    // animation information
    SPositionKey*   m_pPositionKeys;
    UINT            m_dwNumPositionKeys;
    SRotateKey*     m_pRotateKeys;
    UINT            m_dwNumRotateKeys;
    SScaleKey*      m_pScaleKeys;
    UINT            m_dwNumScaleKeys;
    SMatrixKey*     m_pMatrixKeys;
    UINT            m_dwNumMatrixKeys;

    SFrame*         m_pframeAnimNext;
    SFrame*         m_pframeToAnimate;

    SFrame*         m_pSibling;
    SFrame*         m_pChild;

    bool            m_bAnimationFrame;
    char*           m_strName;

    SFrame()
	{
		m_pMeshContainer   = NULL;
		m_pPositionKeys    = NULL;
		m_dwNumPositionKeys = 0;
		m_pScaleKeys       = NULL;
		m_dwNumScaleKeys   = 0;
		m_pRotateKeys      = NULL;
		m_dwNumRotateKeys  = 0;
		m_pMatrixKeys      = NULL;
		m_dwNumMatrixKeys  = 0;
		m_pframeAnimNext   = NULL;
		m_pframeToAnimate  = NULL;
		m_pSibling         = NULL;
		m_pChild           = NULL;
		m_bAnimationFrame  = false;
		m_strName          = NULL;
        D3DXMatrixIdentity( &m_matRot );
        D3DXMatrixIdentity( &m_matRotOrig );
        D3DXMatrixIdentity( &m_matTrans );
    }

    ~SFrame()
    {
        delete[] m_strName;
        delete   m_pMeshContainer;        
        delete   m_pChild;
        delete   m_pSibling;

        delete[] m_pPositionKeys;
        delete[] m_pRotateKeys;
        delete[] m_pScaleKeys;
        delete[] m_pMatrixKeys;

        // Do NOT delete m_pframeAnimNext
        // Do NOT delete m_pframeToAnimate
    }

    void SetTime( float fTime );

    SFrame* FindFrame( char* strFrame )
    {
        SFrame* pFrame;

        if( m_strName && (strcmp(m_strName, strFrame)==0) )
            return this;

        if( m_pChild )
        {
            pFrame = m_pChild->FindFrame( strFrame );
            if( pFrame )
                return pFrame;
        }

        if( m_pSibling )
        {
            pFrame = m_pSibling->FindFrame( strFrame );
            if( pFrame )
                return pFrame;
        }

        return NULL;
    }

    void ResetMatrix()
    {
        m_matRot = m_matRotOrig;
        D3DXMatrixIdentity( &m_matTrans );

        if( m_pChild )
            m_pChild->ResetMatrix();

        if( m_pSibling )
            m_pSibling->ResetMatrix();
    }

    void AddFrame( SFrame* pFrame )
    {
        if( m_pChild == NULL )
        {
            m_pChild = pFrame;
        }
        else
        {
            pFrame->m_pSibling = m_pChild->m_pSibling;
            m_pChild->m_pSibling = pFrame;
        }
    }

    void AddMesh( SMeshContainer* pmc )
    {
        pmc->m_pNext = m_pMeshContainer;
        m_pMeshContainer = pmc;
    }
};


struct SDrawElement
{
    SFrame*       m_pRootFrame;

    D3DXVECTOR3   m_vCenter;
    float         m_fRadius;

	// Name of element for selection purposes
	char*         m_strName;

    // Animation list
    SFrame*       m_pframeAnimHead;

	// Next element in list
	SDrawElement* m_pNext;

    float         m_fCurrentTime;
    float         m_fMaxTime;

	SDrawElement()
	{
		m_vCenter        = D3DXVECTOR3(0.0f,0.0f,0.0f);
		m_fRadius        = 1.0f;
		m_strName        = NULL;
        m_pRootFrame     = NULL;
        m_pframeAnimHead = NULL;
		m_pNext          = NULL;
	}

	~SDrawElement()
	{
        delete   m_pRootFrame;
		delete   m_pNext;
        delete[] m_strName;

        // Do NOT delete m_pframeAnimHead;
	}

    void AddAnimationFrame( SFrame* pframeAnim )
    {
        pframeAnim->m_pframeAnimNext = m_pframeAnimHead;
        m_pframeAnimHead = pframeAnim;
    }

    SFrame* FindFrame( char* strName )
    {
        if( m_pRootFrame == NULL )
            return NULL;
        else
            return m_pRootFrame->FindFrame( strName );
    }
};




HRESULT CalculateBoundingSphere( SDrawElement* pDrawElement );



class CSkinnedMesh
{
public:
	LPDIRECT3DDEVICE8 m_pd3dDevice;

    SKINNING_METHOD m_method;
    DWORD           m_dwFVF;
	SMeshContainer* m_pmcSelectedMesh;
    SFrame*         m_pframeSelected;
    SDrawElement*   m_pdeSelected;
	SDrawElement*   m_pdeHead;

    HRESULT FindBones( SFrame* pCurrentFrame, SDrawElement* pDrawElement );
    HRESULT LoadMeshHierarchy( CHAR* strMesh, CXBPackedResource* pResource );
    HRESULT LoadAnimationSet( LPDIRECTXFILEDATA pxofobjCur, SDrawElement* pDrawElement,
                              DWORD dwOptions, DWORD dwFVF, SFrame* pParentFrame );
    HRESULT LoadAnimation( LPDIRECTXFILEDATA pxofobjCur, SDrawElement* pDrawElement,
                           DWORD dwOptions, DWORD dwFVF, SFrame* pParentFrame );
    HRESULT LoadFrames( LPDIRECTXFILEDATA pxofobjCur, SDrawElement* pDrawElement,
                        DWORD dwOptions, DWORD dwFVF, SFrame* pParentFrame );
    HRESULT LoadMesh( LPDIRECTXFILEDATA pxofobjCur,
                      DWORD dwOptions, DWORD dwFVF, SFrame* pParentFrame );
    HRESULT DeleteSelectedMesh();
    HRESULT DrawMeshContainer( SMeshContainer* pMeshContainer );
    HRESULT UpdateFrames( SFrame* pCurrentFrame, D3DXMATRIX &matCur );
    HRESULT GenerateMesh( SMeshContainer* pMeshContainer );
    HRESULT DrawFrames( SFrame* pCurrentFrame, UINT &dwNumTriangles );


	HRESULT Render();
	
	CSkinnedMesh();
};




#endif

