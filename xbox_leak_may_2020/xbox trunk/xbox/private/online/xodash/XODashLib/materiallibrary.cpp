//-----------------------------------------------------------------------------
// File: MaterialLibrary.cpp
//
// Desc: Classes designed to support the loading and indexing of materials.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "std.h"
#include "materiallibrary.h"
#include "utilities.h"

//-----------------------------------------------------------------------------
// Name: CMaterialLibrary()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CMaterialLibrary::CMaterialLibrary()
{
	m_nCurrentMat = 0;
	for(long i = 0; i < NUMBER_OF_MATERIALS; i++)
		m_pMaterialLib[i] = NULL;

	m_pASEtable = NULL;
	m_nASEentries = 0;

	// Setup the default materials
	Initialize();
}

//-----------------------------------------------------------------------------
// Name: ~CMaterialLibrary()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CMaterialLibrary::~CMaterialLibrary()
{
	for(long i = 0; i < NUMBER_OF_MATERIALS; i++)
	{
		if ( m_pMaterialLib[i] != NULL )
			delete m_pMaterialLib[i];
	}

	if(m_pASEtable != NULL)
		delete m_pASEtable;
}

//-----------------------------------------------------------------------------
// Name: Initalize()
// Desc: Initalizes the main material library for the Xbox Online Dashboard
//-----------------------------------------------------------------------------
HRESULT CMaterialLibrary::Initialize( void )
{
	m_nCurrentMat = 0;

	//-----------------------------------------------------------
	// Material : FlatSurfaces [0]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("FlatSurfaces", eFlatSurfaces);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 192),
								   D3DCOLOR_RGBA(20, 192, 0, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : EggGlow [1]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("EggGlow", eEggGlow);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(252, 255, 0, 0), //side
								 D3DCOLOR_RGBA(254, 255, 188, 228)); //front
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : HilightedType [2]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CBaseMaterial("HilightedType", eHilightedType);
	m_pMaterialLib[m_nCurrentMat]->SetAllColors(0.0118f, 0.1725f, 0.0f, 1.0f);
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : FlatSurfaces2sided3 [3]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("FlatSurfaces2sided3", eFlatSurfaces2sided3);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 255),
								      D3DCOLOR_RGBA(30, 255, 0, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : IconParts [4]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("IconParts", eIconParts);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(191, 255, 107, 192),
								D3DCOLOR_RGBA(0, 255, 18, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : CellEgg/Parts [5]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("CellEgg/Parts", eCellEgg_Parts);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 192),
								   D3DCOLOR_RGBA(30, 255, 0, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : GameHilite [6]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CSolidTexMaterial("GameHilite", eGameHilite);
	((CSolidTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CSolidTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetTexture("GameHilite_01.xbx");
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : CellWallStructure [7]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("CellWallStructure", eCellWallStructure);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(191, 255, 107, 51),
								     D3DCOLOR_RGBA(0, 255, 18, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : GamePod [8]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("GamePod", eGamePod);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 192),
							  D3DCOLOR_RGBA(20, 192, 0, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : InnerWall_02 [9]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffTexMaterial("InnerWall_02", eInnerWall_02);
	((CFalloffTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 255),
								 D3DCOLOR_RGBA(20, 192, 0, 20));
	((CFalloffTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetTexture("cellwall.xbx");
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : DarkenBacking [10]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CBackingTexMaterial("DarkenBacking", eDarkenBacking);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetTexture("panel4.xbx");
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : InnerWall_01 [11]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("InnerWall_01", eInnerWall_01);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 255),
								 D3DCOLOR_RGBA(40, 212, 20, 20));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : Metal_Chrome [12]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CSolidTexMaterial("Metal_Chrome", eMetal_Chrome);
	((CSolidTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CSolidTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetTexture("steel.xbx");
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : Tubes [13]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("Tubes", eTubes);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(242, 250, 153, 215),
						    D3DCOLOR_RGBA(7, 104, 0, 37));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : NavType [14]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CBaseMaterial("NavType", eNavType);
	m_pMaterialLib[m_nCurrentMat]->SetAllColors(0.8667f, 0.8157f, 0.4706f, 1.0f);
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : Shell [15]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("Shell", eShell);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 192),
								   D3DCOLOR_RGBA(20, 192, 0, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : FlatSrfc/PodParts [16]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("FlatSrfc/PodParts", eFlatSrfc_PodParts);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 192),
								   D3DCOLOR_RGBA(20, 192, 0, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : XBoxGreen [17]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CBaseMaterial("XBoxGreen", eXBoxGreen);
	m_pMaterialLib[m_nCurrentMat]->SetAllColors(0.5451f, 0.7843f, 0.0941f, 1.0f);
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : PanelBacking_01 [18]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CBackingTexMaterial("PanelBacking_01", ePanelBacking_01);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetTexture("panel6.xbx");
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : PanelBacking_02 [19]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CBackingTexMaterial("PanelBacking_02", ePanelBacking_02);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetTexture("keyboard_alpha.xbx");
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : PanelBacking_03 [20]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CBackingTexMaterial("PanelBacking_03", ePanelBacking_03);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetTexture("panel4.xbx");
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : PanelBacking_04 [21]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CBackingTexMaterial("PanelBacking_04", ePanelBacking_04);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetTexture("panel8.xbx");
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : PanelBacking_05 [22]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CBackingTexMaterial("PanelBacking_05", ePanelBacking_05);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetTexture("panel2.xbx");
	m_nCurrentMat++;

    //-----------------------------------------------------------
	// Material : PanelBacking_06 [23]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CBackingTexMaterial("PanelBacking_06", ePanelBacking_06);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CBackingTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetTexture("panel3.xbx");
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : Cell_Light [24]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("Cell_Light", eCell_Light);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 192),
								   D3DCOLOR_RGBA(20, 192, 0, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : MenuCell [25]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("MenuCell", eMenuCell);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 192),
								   D3DCOLOR_RGBA(30, 255, 0, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : wireframe [26]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffTexMaterial("wireframe", eWireframe);
	((CFalloffTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 255),
								 D3DCOLOR_RGBA(20, 192, 0, 20));
	((CFalloffTexMaterial*)m_pMaterialLib[m_nCurrentMat])->SetTexture("wireframe.xbx");
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : OuterWall01 [27]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("OuterWall01", eOuterWall01);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(243, 255, 107, 192),
								   D3DCOLOR_RGBA(30, 255, 0, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : redHighlight [28]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("red Highlight", eredHighlight);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(248, 12, 0, 192),
								   D3DCOLOR_RGBA(235, 105, 0, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : greenHighlight [29]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CFalloffMaterial("green Highlight", egreenHighlight);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	((CFalloffMaterial*)m_pMaterialLib[m_nCurrentMat])->SetFalloffColors(D3DCOLOR_RGBA(167, 236, 106, 192),
								   D3DCOLOR_RGBA(30, 255, 0, 0));
	m_nCurrentMat++;

	//-----------------------------------------------------------
	// Material : MissingMaterial [30]
	//-----------------------------------------------------------
	m_pMaterialLib[m_nCurrentMat] = new CBaseMaterial("MissingMaterial", eMissingMaterial);
	m_pMaterialLib[m_nCurrentMat]->SetAllColors(1.0f, 1.0f, 1.0f, 1.0f);
	m_nCurrentMat++;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Setup()
// Desc: Setup is called just before the engine passes the polygons up
//		 to the video card for rendering.  Setup will make all of the
//		 appropriate calls to D3D to initalize the rendering enviornment
//		 for the upcoming polys.
//-----------------------------------------------------------------------------
HRESULT CMaterialLibrary::Setup( long index, DWORD FVF )
{
	// make sure index actually exists in our array
	ASSERT((index >= 0) && (index < NUMBER_OF_MATERIALS));
	if (((index < 0) || (index >= NUMBER_OF_MATERIALS)) )
	{
		m_pMaterialLib[NUMBER_OF_MATERIALS - 1]->Setup(FVF, &m_TextureCache);
	}
    else
    {
	    // material index does not exist
	    if(m_pMaterialLib[index] == NULL)
		    return E_FAIL;

	    if(FAILED(m_pMaterialLib[index]->Setup(FVF, &m_TextureCache)))
		    return E_FAIL;
    }

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: LoadFromASE()
// Desc: Loads the base material library from the ASE file
//-----------------------------------------------------------------------------
HRESULT CMaterialLibrary::LoadFromASE( long nMtrlID, BYTE* pBuffer )
{
	char* Buffer     = (char*)pBuffer;
	char pSearch[256];

	// Pull out the number of materials in the ASE file
	sprintf( pSearch, "*MATERIAL_COUNT " );
	char* pHeader = strstr( Buffer, pSearch );

	char numberData[256];
	GetNumberData( pHeader, numberData );
	m_nASEentries = atol( numberData );

	// make sure our ASE conversion table wasn't allocated for a previous 
	// conversion
	if(NULL != m_pASEtable)
		delete m_pASEtable;

	m_pASEtable = new long[m_nASEentries];

	// Now that we have the material count, go through the list and build
	// a mapping from the ASE materials to our default library
	for(int i = 0; i < m_nASEentries; i++)
	{
		// Get to the correct material index
		sprintf( pSearch, "*MATERIAL %ld", i );
		char* pMaterial = strstr( Buffer, pSearch );

		sprintf( pSearch, "*MATERIAL_NAME" );
		char* pMatNameBuf = strstr( pMaterial, pSearch );

		// Get the material name
		char materialName[256];
		GetNameData( pMatNameBuf, materialName );
		m_pASEtable[i] = ReturnIndex( materialName );
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ReturnIndex()
// Desc: Given the name of a material in the ASE library, the function
//	     will return the index in the material library;
//-----------------------------------------------------------------------------
long CMaterialLibrary::ReturnIndex( const char *szMatName )
{
	for(long i = 0; i < NUMBER_OF_MATERIALS; i++)
	{
		if(0 == strcmp( m_pMaterialLib[i]->m_sMatName, szMatName ))
			return i;
	}

	DbgPrint("Material: %s not found in Material Library\n", szMatName);
	ASSERT(false);

	return NUMBER_OF_MATERIALS - 1;
}

//-----------------------------------------------------------------------------
// Name: ReturnIndex()
// Desc: Given the number of the material in the ASE library, the function
//		 will return the index in the material library
//-----------------------------------------------------------------------------
long CMaterialLibrary::ReturnIndex( const long tableIndex )
{
	// make sure index actually exists in our array
	ASSERT((tableIndex >= 0) && (tableIndex < NUMBER_OF_MATERIALS));
	if (((tableIndex > 1) || (tableIndex < NUMBER_OF_MATERIALS)) )
	{
		return m_pASEtable[tableIndex];
	}

	return NUMBER_OF_MATERIALS - 1;
}

//-----------------------------------------------------------------------------
// Name: GetButtonTextMaterial()
// Desc: Returns the material index for a button text object in the scene. 
//-----------------------------------------------------------------------------
long CMaterialLibrary::GetButtonTextMaterial( eButtonState curState )
{
	switch(curState)
	{
	case eButtonDefault:
		return eNavType;
		break;
	case eButtonPressed:
		return eHilightedType;
		break;
	case eButtonHighlighted:
		return eHilightedType;
		break;
	case eButtonDisabled:
		return eNavType;
		break;
    case eButtonStateMax:
		return eHilightedType;
		break;
	default:
		// state of the text in the button is not defined, assert
		ASSERT(true);
		return eNavType;
		break;
	}
}

//-----------------------------------------------------------------------------
// Name: MaterialUsesTexture()
// Desc: Returns wether the given material uses a texture map or not 
//-----------------------------------------------------------------------------
bool    CMaterialLibrary::MaterialUsesTexture( long index )
{
    	// make sure index actually exists in our array
	ASSERT((index >= 0) && (index < NUMBER_OF_MATERIALS));
	if (((index < 0) || (index >= NUMBER_OF_MATERIALS)) )
	{
		return m_pMaterialLib[NUMBER_OF_MATERIALS - 1]->HasTexture();
	}
    else
    {
	    // material index does not exist
	    if(m_pMaterialLib[index] == NULL)
		    return false;

    }

    return (m_pMaterialLib[index]->HasTexture());


}
