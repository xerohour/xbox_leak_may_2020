//-----------------------------------------------------------------------------
// File: Shader.h
//
// Desc: Shader classes for advanced lighting
//
// Hist: 03.14.01 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_SHADER_H
#define TECH_CERT_GAME_SHADER_H

#include "Common.h"
#include "File.h"
//#include "Texture.h" not currently used
#include "Light.h"




//-----------------------------------------------------------------------------
// Name: class Shader
// Desc: Abstract base shader object
//-----------------------------------------------------------------------------
class Shader
{

protected:

    // Shader types (used when shader object is saved/read from file)
    enum ShaderTypes
    { 
        TypeShaderFlat = 1, 
        TypeShaderLit, 
        TypeShaderTexturedLit, 
        TypeShaderTexturedBump
    };

    static LightList m_LightList;

public:

    Shader()
    {
    }

    virtual ~Shader() = 0
    {
    }

    virtual DWORD Type()
    { 
        return 0;
    }

    virtual HRESULT Save( const File& )
    {
        return S_OK;
    }

    virtual HRESULT Load( const File& )
    { 
        return S_OK;
    }

    virtual INT GetNumPasses()
    { 
        return 1;
    }

    virtual VOID Output( INT iPass )
    { 
        (VOID)iPass;
    }

    // Create a shader based on the passed in type
    static Shader* CreateShaderOfType( DWORD dwShaderType );

};




#if (0)
//-----------------------------------------------------------------------------
// Name: class ShaderFlat
// Desc: Flat shader object
//-----------------------------------------------------------------------------
class ShaderFlat : public Shader
{

    D3DMATERIAL8 m_Material;

public:

    ShaderFlat();
    ShaderFlat( FLOAT fRed, FLOAT fGreen, FLOAT fBlue );

    virtual DWORD Type()
    { 
        return TypeShaderFlat;
    }

    virtual HRESULT Save( const File& );
    virtual HRESULT Load( const File& );
    virtual VOID    Output( INT iPass );

};
#endif




//-----------------------------------------------------------------------------
// Name: class ShaderLit
// Desc: Lit shader object
//-----------------------------------------------------------------------------
class ShaderLit : public Shader
{

    D3DMATERIAL8 m_Material;

public:

    ShaderLit();
    ShaderLit( const D3DMATERIAL8& );

    virtual DWORD Type()
    { 
        return TypeShaderLit;
    }

    virtual HRESULT Save( const File& );
    virtual HRESULT Load( const File& );
    virtual VOID    Output( INT iPass );

};




#if (0)
//-----------------------------------------------------------------------------
// Name: class ShaderTexturedLit
// Desc: Lit and textured shader object
//-----------------------------------------------------------------------------
class ShaderTexturedLit : public Shader
{

    Texture*     m_pTexture;
    D3DMATERIAL8 m_Material;

public:

    ShaderTexturedLit();
    ShaderTexturedLit( Texture*, const D3DMATERIAL8& );
    ~ShaderTexturedLit();

    virtual DWORD Type()
    { 
        return TypeShaderTexturedLit;
    }

    virtual HRESULT Save( const File& );
    virtual HRESULT Load( const File& );
    virtual VOID    Output( INT iPass );

};




//-----------------------------------------------------------------------------
// Name: class ShaderTexturedBump
// Desc: Bumpmapped texture shader object
//-----------------------------------------------------------------------------
class ShaderTexturedBump : public Shader
{
    Texture* m_pImageTexture;
    Texture* m_pBumpTexture;

    DWORD    m_dwBumpVertexShaderHandle;
    DWORD    m_dwImageVertexShaderHandle;
    DWORD    m_dwBumpPixelShaderHandle;
    DWORD    m_dwImagePixelShaderHandle;

    LPDIRECT3DCUBETEXTURE8 m_pNormalizationCubemap;

public:

    ShaderTexturedBump();
    ShaderTexturedBump( Texture* pImageTexture, Texture* pBumpTexture );
    ~ShaderTexturedBump();

    virtual DWORD Type()
    { 
        return TypeShaderTexturedBump;
    }

    virtual HRESULT Save( const File& );
    virtual HRESULT Load( const File& );
    virtual INT     GetNumPasses();
    virtual VOID    Output( INT iPass );

private:

    VOID InitShaders();

};
#endif // 0




#endif // TECH_CERT_GAME_SHADER_H
