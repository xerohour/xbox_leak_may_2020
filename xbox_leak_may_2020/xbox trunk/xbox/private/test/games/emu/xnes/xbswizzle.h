//-----------------------------------------------------------------------------
// Name: XBSwizzle.h
//
// Desc: Allows simple manipulations of a swizzled texture, without the 
//       hassle or overhead of unswizzling the whole thing in order to tweak
//       a few points on the texture. This works with texture maps, cube
//       maps, and volume maps.

//       Depending on your purpose in messing with a swizzled texture, 
//       certain optimizations can be performed by doing things a little 
//       differently than are done here.
//
// Notes: 
//       Most of the time when messing with a texture map, you will be 
//       incrementing through it by a constant value in each dimension. In 
//       order to speed up this process considerably, those deltas are 
//       converted to an intermediate value (via the Swizzle() methodd), 
//       which can be used to quickly increment or change a particular 
//       dimension.
//
//       The type SWIZNUM is used to represent numbers returned by Swizzle(), 
//       also known as "intermediate values" in this documentation.
//
//       Code in comments, like /* & m_MaskU */ may be uncommented in 
//       order to provide some sort of parameter sanity. (It assures that
//       any number passed to num will only alter the dimension specified.)
//
// Methods:
//       SWIZNUM Swizzle() = converts num to an intermediate value that can
//                           be used to alter a coordinate
//       void Unswizzle()  = takes an index to the swizzled texture, and 
//                           returns the unswizzled coordinate
//       void Set(num)     = sets the coordinate to num, where num is an 
//                           intermediate value returned by Swizzle()
//       void Add(num)     = adds num to the coordinate, where num is an 
//                           intermediate value returned by Swizzle()
//       void Sub(num)     = subtracts num from the coordinate, where num is
//                           an intermediate value returned by Swizzle()
//       void Inc()        = increments the coordinate of dimension dim by 1
//       void Dec()        = decrements the coordinate of dimension dim by 1
//       SWIZNUM Get2D()   = returns the index to the swizzled standard or 
//                           cube texture, based on the current texture 
//                           coordinates specified by the previous 5 methods
//       SWIZNUM Get3D()   = returns the index to the swizzled volume texture,
//                           based on the current texture coordinates 
//                           specified by the previous 5 methods
//       void Swizzle2D()  = Swizzles/unswizzles a 2d texture from Src to 
//                           Dest.
//       void Swizzle3D()  = Swizzles/unswizzles a volume texture from Src 
//                           to Dest.
//
// Optimizations:
//       Subtraction is used so that only ANDing is necessary (no ORing, as in
//       other methods). For this reason, the Sub method is faster than the 
//       Add method. Inc and Dec are roughly the same speed, because Dec 
//       requires a shift, but Inc requires accessing an array.
//  
//       It may be better to have Swizzle() convert the negative of the value 
//       specified, and then to reverse the roles of Add and Sub. That would 
//       make the more logical manuvering routine (Add) the faster of the two.
//
// Hist: 10.18.00 - jgould  - Created/last modified
//       11.15.00 - mwetzel - Port for Xbox samples
//
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBSWIZZLE_H
#define XBSWIZZLE_H

typedef DWORD SWIZNUM; 


// Turn on compiler optimizations
#pragma optimize( "gt", on )




//-----------------------------------------------------------------------------
// Name: class CXBSwizzler()
// Desc: Class to aid in addressing data from swizzled textures
//-----------------------------------------------------------------------------
class CXBSwizzler 
{
private:
    DWORD m_Width, m_Height, m_Depth; // Dimensions of the texture
    DWORD m_MaskU, m_MaskV,  m_MaskW; // Internal mask for u coordinate
    DWORD m_u,     m_v,      m_w;     // Swizzled texture coordinates

public:
    // Initializes the swizzler
    CXBSwizzler( DWORD width, DWORD height, DWORD depth )
    { 
        m_Width = width; m_Height = height; m_Depth = depth;
        m_MaskU = 0;     m_MaskV  = 0;      m_MaskW = 0;
        m_u     = 0;     m_v      = 0;      m_w     = 0;

        DWORD i = 1;
        DWORD j = 1;
        DWORD k;
        do 
        {
            k = 0;
            if( i < width )   { m_MaskU |= j;   k = (j<<=1);  }
            if( i < height )  { m_MaskV |= j;   k = (j<<=1);  }
            if( i < depth )   { m_MaskW |= j;   k = (j<<=1);  }
            i <<= 1;
        } while (k);
    }

    // Swizzles a texture coordinate
    __forceinline SWIZNUM SwizzleU( DWORD num )
    {
        SWIZNUM r = 0;
        for( DWORD i=1; i<=m_MaskU; i<<=1 ) 
        {
            if( m_MaskU & i )  r |= (num & i);
            else               num <<= 1;
        }
        return r;
    }
    __forceinline SWIZNUM SwizzleV( DWORD num )
    {
        SWIZNUM r = 0;
        for( DWORD i=1; i<=m_MaskV; i<<=1 ) 
        {
            if( m_MaskV & i )  r |= (num & i);
            else               num <<= 1;
        }
        return r;
    }
    __forceinline SWIZNUM SwizzleW( DWORD num )
    {
        SWIZNUM r = 0;
        for( DWORD i=1; i<=m_MaskW; i<<=1 ) 
        {
            if( m_MaskW & i )  r |= (num & i);
            else               num <<= 1;
        }
        return r;
    }
    __forceinline SWIZNUM Swizzle( DWORD u, DWORD v, DWORD w )
    {
        return SwizzleU(u) | SwizzleV(v) | SwizzleW(w);
    }
    
    // Unswizzles a texture coordinate
    __forceinline DWORD UnswizzleU( SWIZNUM num )
    {
        DWORD r = 0; //result
        for( DWORD i = 1, j = 1; i; i <<= 1 ) 
        {
            if( m_MaskU & i )  {   r |= (num & j);   j <<= 1; } 
            else               {   num >>= 1; }
        }
        return r;
    }
    __forceinline DWORD UnswizzleV( SWIZNUM num )
    {
        DWORD r = 0; //result
        for( DWORD i = 1, j = 1; i; i <<= 1 ) 
        {
            if( m_MaskV & i )  {   r |= (num & j);   j <<= 1; } 
            else               {   num >>= 1; }
        }
        return r;
    }
    __forceinline DWORD UnswizzleW( SWIZNUM num )
    {
        DWORD r = 0; //result
        for( DWORD i = 1, j = 1; i; i <<= 1 ) 
        {
            if( m_MaskW & i )  {   r |= (num & j);   j <<= 1; } 
            else               {   num >>= 1; }
        }
        return r;
    }

    // Sets a texture coordinate
    __forceinline void SetU( SWIZNUM num ) { m_u = num /* & m_MaskU */; }
    __forceinline void SetV( SWIZNUM num ) { m_v = num /* & m_MaskV */; }
    __forceinline void SetW( SWIZNUM num ) { m_w = num /* & m_MaskW */; }
    
    // Adds a value to a texture coordinate
    __forceinline void AddU( SWIZNUM num ) { m_u = ( m_u - ( (0-num) & m_MaskU ) ) & m_MaskU; }
    __forceinline void AddV( SWIZNUM num ) { m_v = ( m_v - ( (0-num) & m_MaskV ) ) & m_MaskV; }
    __forceinline void AddW( SWIZNUM num ) { m_w = ( m_w - ( (0-num) & m_MaskW ) ) & m_MaskW; }

    // Subtracts a value from a texture coordinate
    __forceinline void SubU( SWIZNUM num ) { m_u = ( m_u - num /* & m_MaskU */ ) & m_MaskU; }
    __forceinline void SubV( SWIZNUM num ) { m_v = ( m_v - num /* & m_MaskV */ ) & m_MaskV; }
    __forceinline void SubW( SWIZNUM num ) { m_w = ( m_w - num /* & m_MaskW */ ) & m_MaskW; }

    // Increments a texture coordinate
    __forceinline void IncU()              { m_u = ( m_u - m_MaskU ) & m_MaskU; }
    __forceinline void IncV()              { m_v = ( m_v - m_MaskV ) & m_MaskV; }
    __forceinline void IncW()              { m_w = ( m_w - m_MaskW ) & m_MaskW; }

    // Decrements a texture coordinate
    __forceinline void DecU()              { m_u = ( m_u - (1<<0)) & m_MaskU; }
    __forceinline void DecV()              { m_v = ( m_v - (1<<1)) & m_MaskV; }
    __forceinline void DecW()              { m_w = ( m_w - (1<<2)) & m_MaskW; }

    // Gets the current swizzled address for a 2D or 3D texture
    __forceinline SWIZNUM Get2D()          { return m_u | m_v; }
    __forceinline SWIZNUM Get3D()          { return m_u | m_v | m_w; }
    
    // Swizzles an entire 2D texture
    template <class TYPE>
    void Swizzle2D( TYPE *Src, TYPE* Dest )
    {       
        unsigned int u, v, i = 0;
        SetU( 0 );
        SetV( 0 );
        for( v = m_Height; v--; ) 
        {
            for( u = m_Width; u--; ) 
            {
                Dest[Get2()] = Src[i++];
                IncU();
            }
            IncV();
        }
    }

    // Unswizzles an entire 2D texture
    template <class TYPE>
    void Unswizzle2D( TYPE *Src, TYPE* Dest )
    {
        unsigned int u, v, i = 0;
        SetU( 0 );
        SetV( 0 );
        for( v = m_Height; v--; ) 
        {
            for( u = m_Width; u--; ) 
            {
                Dest[i++] = Src[Get2()];
                IncU();
            }
            IncV();
        }
    }

    // Swizzles an entire 3D texture
    template <class TYPE>
    void Swizzle3D( TYPE *Src, TYPE* Dest )
    {
        unsigned int u, v, w, i = 0;
        SetU( 0 );
        SetV( 0 );
        SetW( 0 );
        for( w = m_Depth; w--; ) 
        {
            for( v = m_Height; v--; ) 
            {
                for( u = m_Width; u--; ) 
                {
                    Dest[Get3()] = Src[i++];
                    IncU();
                }
                IncV();
            }
            IncW();
        }
    }

    // Unswizzles an entire 3D texture
    template <class TYPE>
    void Unswizzle3D( TYPE *Src, TYPE* Dest )
    {
        unsigned int u, v, w, i = 0;
        SetU( 0 );
        SetV( 0 );
        SetW( 0 );
        for( w = m_Depth; w--; ) 
        {
            for( v = m_Height; v--; ) 
            {
                for( u = m_Width; u--; ) 
                {
                    Dest[i++] = Src[Get3()];
                    IncU();
                }
                IncV();
            }
            IncW();
        }
    }
};




// Restore the compiler optimization flags
#pragma optimize( "", on )




#endif




