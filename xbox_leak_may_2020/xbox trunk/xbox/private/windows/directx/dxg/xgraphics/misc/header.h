namespace XGRAPHICS
{

#define FMT_RENDERTARGET 0x80
#define FMT_DEPTHBUFFER 0x40
#define FMT_BITSPERPIXEL 0x3c
#define FMT_LINEAR 0x02
#define FMT_SWIZZLED 0x01

#define FMT_32BPP 0x20
#define FMT_16BPP 0x10
#define FMT_8BPP 0x08
#define FMT_4BPP 0x04

D3DCONST BYTE g_TextureFormat[] =
{
    FMT_8BPP  |                    FMT_SWIZZLED,  // 0x00 D3DFMT_L8                   
    FMT_8BPP  |                    FMT_SWIZZLED,  // 0x01 D3DFMT_AL8                  
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x02 D3DFMT_A1R5G5B5             
    FMT_16BPP | FMT_RENDERTARGET | FMT_SWIZZLED,  // 0x03 D3DFMT_X1R5G5B5             
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x04 D3DFMT_A4R4G4B4             
    FMT_16BPP | FMT_RENDERTARGET | FMT_SWIZZLED,  // 0x05 D3DFMT_R5G6B5               
    FMT_32BPP | FMT_RENDERTARGET | FMT_SWIZZLED,  // 0x06 D3DFMT_A8R8G8B8             
    FMT_32BPP | FMT_RENDERTARGET | FMT_SWIZZLED,  // 0x07 D3DFMT_X8R8G8B8/D3DFMT_X8L8V8U8
    0,                                            // 0x08
    0,                                            // 0x09
    0,                                            // 0x0A
    FMT_8BPP  |                    FMT_SWIZZLED,  // 0x0B D3DFMT_P8                   
    FMT_4BPP,                                     // 0x0C D3DFMT_DXT1                 
    0,                                            // 0x0D
    FMT_8BPP,                                     // 0x0E D3DFMT_DXT2/D3DFMT_DXT3
    FMT_8BPP,                                     // 0x0F D3DFMT_DXT4/D3DFMT_DXT5
    FMT_16BPP |                    FMT_LINEAR,    // 0x10 D3DFMT_LIN_A1R5G5B5         
    FMT_16BPP | FMT_RENDERTARGET | FMT_LINEAR,    // 0x11 D3DFMT_LIN_R5G6B5           
    FMT_32BPP | FMT_RENDERTARGET | FMT_LINEAR,    // 0x12 D3DFMT_LIN_A8R8G8B8         
    FMT_8BPP  |                    FMT_LINEAR,    // 0x13 D3DFMT_LIN_L8               
    0,                                            // 0x14
    0,                                            // 0x15
    FMT_16BPP |                    FMT_LINEAR,    // 0x16 D3DFMT_LIN_R8B8             
    FMT_16BPP |                    FMT_LINEAR,    // 0x17 D3DFMT_LIN_G8B8             
    0,                                            // 0x18
    FMT_8BPP  |                    FMT_SWIZZLED,  // 0x19 D3DFMT_A8                   
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x1A D3DFMT_A8L8                 
    FMT_8BPP  |                    FMT_LINEAR,    // 0x1B D3DFMT_LIN_AL8              
    FMT_16BPP | FMT_RENDERTARGET | FMT_LINEAR,    // 0x1C D3DFMT_LIN_X1R5G5B5         
    FMT_16BPP |                    FMT_LINEAR,    // 0x1D D3DFMT_LIN_A4R4G4B4         
    FMT_32BPP | FMT_RENDERTARGET | FMT_LINEAR,    // 0x1E D3DFMT_LIN_X8R8G8B8         
    FMT_8BPP  |                    FMT_LINEAR,    // 0x1F D3DFMT_LIN_A8               
    FMT_16BPP |                    FMT_LINEAR,    // 0x20 D3DFMT_LIN_A8L8             
    0,                                            // 0x21
    0,                                            // 0x22
    0,                                            // 0x23
    FMT_32BPP,                                    // 0x24 D3DFMT_UYVY                 
    FMT_32BPP,                                    // 0x25 D3DFMT_YUY2                 
    0,                                            // 0x26
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x27 D3DFMT_R6G5B5/D3DFMT_L6V5U5
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x28 D3DFMT_G8B8/D3DFMT_V8U8
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x29 D3DFMT_R8B8                 
    FMT_32BPP | FMT_DEPTHBUFFER |  FMT_SWIZZLED,  // 0x2A D3DFMT_D24S8                
    FMT_32BPP | FMT_DEPTHBUFFER |  FMT_SWIZZLED,  // 0x2B D3DFMT_F24S8                
    FMT_16BPP | FMT_DEPTHBUFFER |  FMT_SWIZZLED,  // 0x2C D3DFMT_D16_LOCKABLE/D3DFMT_D16
    FMT_16BPP | FMT_DEPTHBUFFER |  FMT_SWIZZLED,  // 0x2D D3DFMT_F16                  
    FMT_32BPP | FMT_DEPTHBUFFER |  FMT_LINEAR,    // 0x2E D3DFMT_LIN_D24S8            
    FMT_32BPP | FMT_DEPTHBUFFER |  FMT_LINEAR,    // 0x2F D3DFMT_LIN_F24S8            
    FMT_16BPP | FMT_DEPTHBUFFER |  FMT_LINEAR,    // 0x30 D3DFMT_LIN_D16              
    FMT_16BPP | FMT_DEPTHBUFFER |  FMT_LINEAR,    // 0x31 D3DFMT_LIN_F16              
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x32 D3DFMT_L16                  
    FMT_32BPP |                    FMT_SWIZZLED,  // 0x33 D3DFMT_V16U16               
    0,                                            // 0x34
    FMT_16BPP |                    FMT_LINEAR,    // 0x35 D3DFMT_LIN_L16              
    0,                                            // 0x36
    FMT_16BPP |                    FMT_LINEAR,    // 0x37 D3DFMT_LIN_R6G5B5           
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x38 D3DFMT_R5G5B5A1             
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x39 D3DFMT_R4G4B4A4             
    FMT_32BPP |                    FMT_SWIZZLED,  // 0x3A D3DFMT_A8B8G8R8/D3DFMT_Q8W8V8U8             
    FMT_32BPP |                    FMT_SWIZZLED,  // 0x3B D3DFMT_B8G8R8A8             
    FMT_32BPP |                    FMT_SWIZZLED,  // 0x3C D3DFMT_R8G8B8A8             
    FMT_16BPP |                    FMT_LINEAR,    // 0x3D D3DFMT_LIN_R5G5B5A1         
    FMT_16BPP |                    FMT_LINEAR,    // 0x3E D3DFMT_LIN_R4G4B4A4         
    FMT_32BPP |                    FMT_LINEAR,    // 0x3F D3DFMT_LIN_A8B8G8R8         
    FMT_32BPP |                    FMT_LINEAR,    // 0x40 D3DFMT_LIN_B8G8R8A8         
    FMT_32BPP |                    FMT_LINEAR,    // 0x41 D3DFMT_LIN_R8G8B8A8         
};

inline bool IsSwizzledD3DFORMAT(
    D3DFORMAT Format
    )
{
    return g_TextureFormat[Format] & FMT_SWIZZLED;
}

inline bool IsCompressedD3DFORMAT(
    DWORD Format
    )
{
    switch(Format)
    {
    case D3DFMT_DXT1:
    case D3DFMT_DXT2:  // DXT3
    case D3DFMT_DXT4:  // DXT4
        return true;
    }

    return false;
}

inline BOOL IsValidDepthBufferD3DFORMAT(
    DWORD Format
    )
{
    return g_TextureFormat[Format] & FMT_DEPTHBUFFER;
}

inline DWORD BitsPerPixelOfD3DFORMAT(
    DWORD Format
    )
{
    return g_TextureFormat[Format] & FMT_BITSPERPIXEL;
}

#pragma warning (disable: 4035)
inline DWORD __fastcall Log2(
    DWORD Value
    )
{
    __asm { bsf eax,[Value] }
}
#pragma warning (default: 4035)

inline DWORD MinimumTextureSizeOfD3DFORMAT(
    DWORD Format
    )
{
    return IsCompressedD3DFORMAT(Format) ? 2 : 0;
}

inline DWORD CalcPitch(
    DWORD Width,
    DWORD TexelSize
    )
{
    return (Width * TexelSize / 8 + D3DTEXTURE_PITCH_ALIGNMENT - 1) &
            ~(D3DTEXTURE_PITCH_ALIGNMENT - 1);
}

inline DWORD PitchFromSize(
    DWORD Size
    )
{
    return (((Size & D3DSIZE_PITCH_MASK) >> D3DSIZE_PITCH_SHIFT) + 1) * D3DTEXTURE_PITCH_ALIGNMENT;
}

//------------------------------------------------------------------------------
// Encodes the Format and Size parameters for a surface and returns the number
// of bytes required to the data for the surface.  
//
DWORD EncodeFormat(
    DWORD Width,
    DWORD Height,
    DWORD Depth,
    DWORD Levels,
    D3DFORMAT D3DFormat,
    DWORD Pitch,
    bool isCubeMap,  
    bool isVolume,
    DWORD *pFormat,
    DWORD *pSize
    );

} // namespace XGRAPHICS

