#pragma once

typedef DWORD SWIZNUM;

#pragma optimize( "gt", on )

typedef struct _CXBSwizzler
{
    DWORD m_Width, m_Height, m_Depth; // Dimensions of the texture
    DWORD m_MaskU, m_MaskV,  m_MaskW; // Internal mask for u coordinate
    DWORD m_u,     m_v,      m_w;     // Swizzled texture coordinates
} CXBSwizzler;

void InitSwizzler( CXBSwizzler* pSwiz, DWORD width, DWORD height, DWORD depth )
{
    DWORD i = 1;
    DWORD j = 1;
    DWORD k;

    pSwiz->m_Width = width; pSwiz->m_Height = height; pSwiz->m_Depth = depth;
    pSwiz->m_MaskU = 0;     pSwiz->m_MaskV  = 0;      pSwiz->m_MaskW = 0;
    pSwiz->m_u     = 0;     pSwiz->m_v      = 0;      pSwiz->m_w     = 0;


    do 
    {
        k = 0;
        if( i < width )   { pSwiz->m_MaskU |= j;   k = (j<<=1);  }
        if( i < height )  { pSwiz->m_MaskV |= j;   k = (j<<=1);  }
        if( i < depth )   { pSwiz->m_MaskW |= j;   k = (j<<=1);  }
        i <<= 1;
    } while (k);
}

__forceinline void SetU( CXBSwizzler* pSwiz, SWIZNUM num ) { pSwiz->m_u = num /* & m_MaskU */; }
__forceinline void SetV( CXBSwizzler* pSwiz, SWIZNUM num ) { pSwiz->m_v = num /* & m_MaskV */; }

__forceinline void IncU( CXBSwizzler* pSwiz )              { pSwiz->m_u = ( pSwiz->m_u - pSwiz->m_MaskU ) & pSwiz->m_MaskU; }
__forceinline void IncV( CXBSwizzler* pSwiz )              { pSwiz->m_v = ( pSwiz->m_v - pSwiz->m_MaskV ) & pSwiz->m_MaskV; }

__forceinline SWIZNUM Get2D( CXBSwizzler* pSwiz )          { return pSwiz->m_u | pSwiz->m_v; }


#pragma optimize( "", on )