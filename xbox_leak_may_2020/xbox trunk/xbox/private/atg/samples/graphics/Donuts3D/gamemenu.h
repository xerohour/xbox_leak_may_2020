//-----------------------------------------------------------------------------
// File: GameMenu.h
//
// Desc: Code for in-game menus
//
// Copyright (C) Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#ifndef GAMEMENU_H
#define GAMEMENU_H




//-----------------------------------------------------------------------------
// Name: class CMenuItem
// Desc: Helper class for displaying menus. Each menu has a texture, an ID,
//       and children. An app can build menus with the constructor and the 
//       Add() function, and display the menu on top of a 3D scene with the
//       Render() function.
//-----------------------------------------------------------------------------
class CMenuItem
{
	LPDIRECT3DTEXTURE8 m_pTexture;
	DWORD              m_dwID;

	CMenuItem*         m_pParent;
	CMenuItem*         m_pChild[10];
	DWORD              m_dwNumChildren;
	DWORD              m_dwSelectedMenu;

public:
	
	// Construction / destruction
	CMenuItem( LPDIRECT3DTEXTURE8 pTexture, DWORD dwID );
	~CMenuItem();

	// Hierarchial build functions
	CMenuItem* Add( CMenuItem* );

	// Access functions
	VOID       SetID( DWORD dwID )             { m_dwID = dwID; }
	DWORD      GetID()                         { return m_dwID; }
	CMenuItem* GetParent()                     { return m_pParent; }
	CMenuItem* GetSelectedChild()              { return m_pChild[m_dwSelectedMenu]; }
	DWORD      GetNumChildren()                { return m_dwNumChildren; }
	VOID       SetSelectedMenu( DWORD dwMenu ) { m_dwSelectedMenu = dwMenu; }
	DWORD      GetSelectedMenu()               { return m_dwSelectedMenu; }

	// Render functions
	HRESULT    Render( LPDIRECT3DDEVICE8 pd3dDevice );
	HRESULT    RenderLabel( LPDIRECT3DDEVICE8 pd3dDevice );
};




#endif


