// Vertex shader constants

// Avoid the special -37 and -38. This gives us 192 - 36 = 156 vertex palette
// slots to work with, which corresponds to a 78 vertex region, with xyz normal and uvs.
// If we want more, we should change to a screenspace vertex shader and use
// the slots below -36
#define VSC_WORLDVIEWPROJECTION -96
#define VSC_INDEX -92
#define VSC_DISPLACEMENT -91
#define VSC_DISPLACEMENT0 -91
#define VSC_DISPLACEMENT1 -90
#define VSC_DISPLACEMENT2 -89
#define VSC_DISPLACEMENT3 -88
#define VSC_ONES -87
#define VSC_TEXTURESCALE -86
#define VSC_CARRIER_VERTEX_PALETTE -36
#define VSC_CARRIER_NORMAL_PALETTE 28
