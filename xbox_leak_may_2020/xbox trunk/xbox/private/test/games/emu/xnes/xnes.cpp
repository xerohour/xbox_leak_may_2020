#define INITGUID
#include <stdio.h>
//#include <windows.h>
#include <xtl.h>
#include <c:\\xbox\\private\\inc\\xdbg.h>

#include "m6502.h"
//#include "svga.h"
#include "fce.h"
#include "config.h"
#include "general.h"
#include "version.h"
#include "swiz.h"
#include "usbmanager.h"

extern "C"
{
#include "svga.h"

	unsigned char* XBuf;
	int joy[4];
}

static LPDIRECTSOUNDBUFFER			g_pBuffer = NULL;

#define szWndClassName "FCEUCLASS"
#define APP_TITLE_NAME_A                        "XNintendo"
// Screen Dimensions
#define SCREEN_WIDTH                            640
#define SCREEN_HEIGHT                           480
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ  | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1) 
IDirect3DDevice8			*m_pD3DDevice;
DWORD dwFvf = D3DFVF_XYZ  | D3DFVF_NORMAL | D3DFVF_DIFFUSE |
              D3DFVF_TEX0 | D3DFVF_TEXCOORDSIZE2(0);
#define D3DFVF_FLATVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
typedef struct _FLATVERTEX
{
    FLOAT x, y, z, rhw; // The transformed position for the vertex.
    DWORD color;        // The vertex color.
	FLOAT tu, tv;
} FLATVERTEX;

FLATVERTEX g_Vertices[4];

static int WindowDead=0;

void PrintFatalError(char *s)
{
	DbgPrint( "*** Fatal Error : %s ***\n", s );
}

void PrintNonFatal(char *s)
{
DbgPrint( "* Message : %s *\n", s );
}



HANDLE ThreadHandle;
HWND hAppWnd=0;
HWND hParent;
HINSTANCE hin;

//LPDIRECTDRAWSURFACE4  lpDDSPrimary;  // DirectDraw primary surface
LRESULT FAR PASCAL AppWndProc(HWND,UINT,WPARAM,LPARAM);

//LPDIRECTDRAWPALETTE lpddpal=NULL;
PALETTEENTRY color_palette[256];     // Palette "buffer"

//LPDIRECTDRAW  lpDD;
//LPDIRECTDRAW4 lpDD4=0;

//DDSURFACEDESC2 ddsd;
HRESULT  ddrval;



//DDSURFACEDESC2        ddsdback;
//LPDIRECTDRAWSURFACE4  lpDDSBack;  // DirectDraw primary surface

static int vflags;
static int veflags=0;


static int fixthreadpri=0;
LRESULT FAR PASCAL AppWndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam) { 
  // this is where we receive all message concerning this window
  // we can either process a message or pass it on to the default
  // message handler of windows
 /* switch(msg) {
    case WM_KEYDOWN:
      switch( wParam ) {
        case VK_ESCAPE:
        case VK_F12:
          // Close window if Esc or F12 was pressed
          Exit=1;
          SetThreadPriority(hin,THREAD_PRIORITY_NORMAL);   
          break;
      }
      break;
    case WM_DESTROY:
      WindowDead=1;
      PostQuitMessage(0);  // Terminate Application
      break;
    case WM_ACTIVATE:
       switch(wParam&0xFFFF)
       {
        case WA_INACTIVE:
                       if(!Exit) fixthreadpri=-1;
                       else fixthreadpri=1;
                       WindowDead=1;
                       if(soundon)SetThreadPriority(ThreadHandle,THREAD_PRIORITY_IDLE);
                       break;
        case WA_ACTIVE:fixthreadpri=1;
                       WindowDead=0;if(soundon)SetThreadPriority(ThreadHandle,THREAD_PRIORITY_ABOVE_NORMAL);
                       break;
       }
    default:
      // We didn't process the message so let Windows do it
      return DefWindowProc(hWnd,msg,wParam,lParam);
   }
  // We processed the message and there
  // is no processing by Windows necessary */
  return 0L;
}

static int PaletteChanged=0;

void SetPalette(unsigned char index, unsigned char r, unsigned char g, unsigned char b)
{
color_palette[index].peRed=r;
color_palette[index].peGreen=g;
color_palette[index].peBlue=b;
PaletteChanged=1;
}

void GetPalette(unsigned char i, unsigned char *r, unsigned char *g,unsigned char *b)
{
*r=color_palette[i].peRed;
*g=color_palette[i].peGreen;
*b=color_palette[i].peBlue;
}

//LPDIRECTINPUT lpDI;
//WINDOWPLACEMENT ParentPlacement;

static char *oogv[64];
static int argco;


static char gfsdir[1024];


/*void GetBaseDirectory(char *bd, char *f)
{
 int x;
 bd[0]=0;
// GetModuleFileName(0,(LPTSTR)bd,2047);

 for(x=strlen(bd);x>=0;x--)
 {
  if(bd[x]=='\\' || bd[x]=='/')
   {bd[x]=0;break;}
 }
}
*/
byte honk;
static char fbuffer[1024];

GUID joyGUID[4];
static int joyF[4];
/*BOOL CALLBACK JoystickSearch(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
  SendDlgItemMessage(pvRef,1126,CB_ADDSTRING,0,(LPARAM)(LPSTR)lpddi->tszProductName);
  SendDlgItemMessage(pvRef,1127,CB_ADDSTRING,0,(LPARAM)(LPSTR)lpddi->tszProductName);
  SendDlgItemMessage(pvRef,1131,CB_ADDSTRING,0,(LPARAM)(LPSTR)lpddi->tszProductName);
  SendDlgItemMessage(pvRef,1132,CB_ADDSTRING,0,(LPARAM)(LPSTR)lpddi->tszProductName);

  return DIENUM_CONTINUE;
}*/


static byte temp[1024];
static int  gork;

/*UINT APIENTRY OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
 switch(uiMsg)
 {
  case WM_INITDIALOG:
         CheckDlgButton(hdlg,1121,dowait?BST_UNCHECKED:BST_CHECKED);
         CheckDlgButton(hdlg,1122,ssync?BST_CHECKED:BST_UNCHECKED);
         CheckDlgButton(hdlg,1125,soundon?BST_CHECKED:BST_UNCHECKED);
         CheckDlgButton(hdlg,1123,ntsccol?BST_CHECKED:BST_UNCHECKED);

         SendDlgItemMessage(hdlg,1120,CB_ADDSTRING,0,(LPARAM)(LPSTR)"320x240 Full Screen");
         SendDlgItemMessage(hdlg,1120,CB_ADDSTRING,0,(LPARAM)(LPSTR)"512x384 Centered");
         SendDlgItemMessage(hdlg,1120,CB_ADDSTRING,0,(LPARAM)(LPSTR)"640x480 Centered");
         SendDlgItemMessage(hdlg,1120,CB_ADDSTRING,0,(LPARAM)(LPSTR)"640x480 Scanlines");
         SendDlgItemMessage(hdlg,1120,CB_ADDSTRING,0,(LPARAM)(LPSTR)"640x480 TV Simulation");
         SendDlgItemMessage(hdlg,1120,CB_ADDSTRING,0,(LPARAM)(LPSTR)"640x480 (scaled: 2x,2y)");
         SendDlgItemMessage(hdlg,1120,CB_ADDSTRING,0,(LPARAM)(LPSTR)"1024x768 (scaled: 4x,3y)");
         SendDlgItemMessage(hdlg,1120,CB_ADDSTRING,0,(LPARAM)(LPSTR)"1280x1024 (scaled: 5x,4y)");
         SendDlgItemMessage(hdlg,1120,CB_ADDSTRING,0,(LPARAM)(LPSTR)"1600x1200 (scaled: 6x,5y)");
         SendDlgItemMessage(hdlg,1120,CB_SETCURSEL,vmode-1,(LPARAM)(LPSTR)0);

         SendDlgItemMessage(hdlg,1126,CB_ADDSTRING,0,(LPARAM)(LPSTR)"<none>");
         SendDlgItemMessage(hdlg,1127,CB_ADDSTRING,0,(LPARAM)(LPSTR)"<none>");

         SendDlgItemMessage(hdlg,1131,CB_ADDSTRING,0,(LPARAM)(LPSTR)"<none>");
         SendDlgItemMessage(hdlg,1132,CB_ADDSTRING,0,(LPARAM)(LPSTR)"<none>");

         IDirectInput_EnumDevices(lpDI, DIDEVTYPE_JOYSTICK,JoystickSearch,hdlg,DIEDFL_ATTACHEDONLY);
         SendDlgItemMessage(hdlg,1126,CB_SETCURSEL,joy[0],(LPARAM)(LPSTR)0);
         SendDlgItemMessage(hdlg,1127,CB_SETCURSEL,joy[1],(LPARAM)(LPSTR)0);
         SendDlgItemMessage(hdlg,1131,CB_SETCURSEL,joy[2],(LPARAM)(LPSTR)0);
         SendDlgItemMessage(hdlg,1132,CB_SETCURSEL,joy[3],(LPARAM)(LPSTR)0);
         break;
  case 273:
         {char bualf[256];
         GetCurrentDirectory(256,bualf);
         switch(wParam)
         {
          case 1121:dowait^=1;break;
          case 1122:ssync^=1;break; 
          case 1124:PAL^=1;break;                    
          case 1123:ntsccol^=1;     
                    break;
          case 1125:soundon^=1;      
                    break;
          case 1128:genie^=3;break;                    
         }
        }
           break;
  case WM_NOTIFY:
            if(((OFNOTIFY *)lParam)->hdr.code==CDN_FILEOK)
            {
             int zap;
             zap=SendDlgItemMessage(hdlg,1120,CB_GETCURSEL,0,(LPARAM)(LPSTR)0);
             if(zap!=CB_ERR) vmode=zap+1;

             zap=SendDlgItemMessage(hdlg,1126,CB_GETCURSEL,0,(LPARAM)(LPSTR)0);
             if(zap!=CB_ERR) joy[0]=zap;

             zap=SendDlgItemMessage(hdlg,1127,CB_GETCURSEL,0,(LPARAM)(LPSTR)0);
             if(zap!=CB_ERR) joy[1]=zap;

             zap=SendDlgItemMessage(hdlg,1131,CB_GETCURSEL,0,(LPARAM)(LPSTR)0);
             if(zap!=CB_ERR) joy[2]=zap;

             zap=SendDlgItemMessage(hdlg,1132,CB_GETCURSEL,0,(LPARAM)(LPSTR)0);
             if(zap!=CB_ERR) joy[3]=zap;


             if(GetDlgItemText(hdlg,1129,(LPTSTR)temp,1024))
              gork=-2;
             else
             {
              zap=0;
              gork=GetDlgItemInt(hdlg,1130,(BOOL *)&zap,0);
              if(!zap)
               gork=-1;
             }
            }
            break;
 }
 return 0;
}
*/

char *GetDriverConfig(void)
{
 return gfsdir;
}
char *SetDriverConfig(void)
{
 gfsdir[0]=0;
 return gfsdir;
}




IDirect3DDevice8* m_Device;
IDirect3DSurface8* m_BackBuffer;
IDirect3DVertexBuffer8* m_VertexBuffer;
int m_ScreenWidth; 
int m_ScreenHeight;
// Vertext structure
typedef struct _MYVERTEX
    {
	    D3DXVECTOR3 v;
	    float       fRHW;
	    D3DCOLOR    cDiffuse;
} MYVERTEX;

#define NUM_VB_VERTS            100

void Initialize( int width, int height)
{
    IDirect3D8* pD3D;
	D3DPRESENT_PARAMETERS d3dpp;
	D3DMATRIX mat;
	D3DMATRIX WorldMatrix;
	D3DMATRIX ViewMatrix;
	D3DXMATRIX ProjectionMatrix;
	D3DVIEWPORT8 D3DViewport;    // Direct3D Viewport

    m_ScreenWidth = width;
    m_ScreenHeight = height;
	pD3D = NULL;

	if ( m_Device == NULL )
	{
        // Create D3D 8.
		if( FAILED( pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        {
            DbgPrint( "Initialize(): Failed to create pD3D!\n" );
			return;
        }

        // Set the screen mode.
		ZeroMemory( &d3dpp, sizeof( d3dpp ) );

		d3dpp.BackBufferWidth                 = width;
		d3dpp.BackBufferHeight                = height;
		d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
		d3dpp.BackBufferCount                 = 1;

		d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;

        d3dpp.EnableAutoDepthStencil          = TRUE;
        d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
		d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

		// Create the m_Device.
		if ( FAILED ( IDirect3D8_CreateDevice( pD3D,
											   0,
                                               D3DDEVTYPE_HAL,
                                               NULL,      // Ignored on Xbox
                                               D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                               &d3dpp,
                                               &m_Device ) ) )
		{
            DbgPrint( "Initialize():Failed to CreateDevice!\n" );
			IDirect3D8_Release(pD3D);

			return;
		}

		// Now we no longer need the D3D interface so let's free it.
		IDirect3D8_Release(pD3D);
	}

    if( FAILED( IDirect3DDevice8_SetRenderState( m_Device, D3DRS_LIGHTING, FALSE ) ) )
    {
        DbgPrint( "Initialize():m_Device->SetRenderState( D3DRS_LIGHTING, FALSE ) Failed!!\n" );
    }

    if( FAILED( IDirect3DDevice8_SetRenderState( m_Device, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ) )
    {
        DbgPrint( "Initialize():m_Device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) Failed!!\n" );
    }

    if( FAILED( IDirect3DDevice8_SetRenderState( m_Device, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ) )
    {
        DbgPrint( "Initialize():m_Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) Failed!!\n" );
    }

    if( FAILED( IDirect3DDevice8_SetRenderState( m_Device, D3DRS_ALPHABLENDENABLE, TRUE ) ) )
    {
        DbgPrint( "Initialize():m_Device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) Failed!!\n" );
    }

    if( FAILED( IDirect3DDevice8_SetRenderState( m_Device, D3DRS_CULLMODE, D3DCULL_NONE ) ) )
    {
        DbgPrint( "Initialize():m_Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ) Failed!!\n" );
    }

    // Create our vertex buffer
	if( FAILED( IDirect3DDevice8_CreateVertexBuffer(m_Device, sizeof( MYVERTEX ) * NUM_VB_VERTS, 0, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &m_VertexBuffer ) ) )
    {
        DbgPrint( "Initialize():Failed to create the Vertex Buffer!!\n" );
    }

    // Set the stream source
	IDirect3DDevice8_SetStreamSource(m_Device, 0, m_VertexBuffer, sizeof( MYVERTEX ) );

    // Set our initial vertex shader that is compatible with our 2D scene
	//if( FAILED( IDirect3DDevice8_SetVertexShader(m_Device, D3DFVF_XYZRHW | D3DFVF_DIFFUSE ) ) )
	if( FAILED( IDirect3DDevice8_SetVertexShader(m_Device, D3DFVF_CUSTOMVERTEX ) ) )
    {
        DbgPrint( "Initialize():m_Device->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE ) Failed!!\n" );
    }

    // Setup matricies
    // Setup our identity matrix

    mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
    mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
    mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
    mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;

    // World Matrix
    // The world matrix controls the position and orientation 
    // of the polygons in world space. We'll use it later to 
    // spin the triangle.
    WorldMatrix = mat;

    // Set the transforms for our 3D world
    if( FAILED( IDirect3DDevice8_SetTransform( m_Device, D3DTS_WORLD, &WorldMatrix ) ) )
    {
        DbgPrint( "Initialize():Failed to set up the World Matrix!!\n" );
    }
    
    // View Matrix
    // The view matrix defines the position and orientation of 
    // the camera. Here, we are just moving it back along the z-
    // axis by 10 units.
    ViewMatrix = mat;
    ViewMatrix._43 = 10.0f;

    if( FAILED( IDirect3DDevice8_SetTransform( m_Device, D3DTS_VIEW, &ViewMatrix ) ) )
    {
        DbgPrint( "Initialize():Failed to set up the View Matrix!!\n" );
    }

    // Projection Matrix
    // The projection matrix defines how the 3-D scene is "projected" 
    // onto the 2-D render target surface. 

    // Set up a very simple projection that scales x and y 
    // by 2, and translates z by -1.0.
     //  = mat;
    D3DXMatrixPerspectiveFovLH( &ProjectionMatrix, 1.57f, 640.0f/480.0f, 0.001f, 30.0f );

    if( FAILED( IDirect3DDevice8_SetTransform( m_Device, D3DTS_PROJECTION, &ProjectionMatrix ) ) )
    {
        DbgPrint( "Initialize():Failed to set up the Projection Matrix!!\n" );
    }
    
    // Viewport

    D3DViewport.X      = 0;
    D3DViewport.Y      = 0;
    D3DViewport.Width  = width;  // rect.right - rect.left;
    D3DViewport.Height = height; // rect.bottom - rect.top;
    D3DViewport.MinZ   = 0.0f;
    D3DViewport.MaxZ   = 1.0f;

    // Set our Viewport
    if( FAILED( IDirect3DDevice8_SetViewport( m_Device, &D3DViewport ) ) )
    {
        DbgPrint( "Initialize():Failed to set the viewport!!\n" );
    }
}

void ClearScreen( DWORD color )
{
#ifdef KELVIN
    if( FAILED( IDirect3DDevice8_Clear(m_Device, 0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, color, 1.0f, 0 ) ) )
#else
    if( FAILED( IDirect3DDevice8_Clear(m_Device, 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 ) ) )
#endif
    {
        DbgPrint( "ClearScreen():Failed to clear the screen!!\n" );
    }
}

// Display the current backbuffer on to the screen
void ShowScreen( void )
{
	if( m_Device )
    {
		IDirect3DDevice8_Present(m_Device, NULL, NULL, NULL, NULL );
	}
}

D3DSURFACE_DESC    d3dsd;
D3DLOCKED_RECT     d3dlr;
DWORD			   dwDstPitch;
LPDIRECT3DTEXTURE8 texture;


void InitTexture(void)
{
	int tempwidth, tempheight;
	int NESwidth, NESheight;
	float tuValue, tvValue, tuXstart, tvYstart;
	IDirect3DVertexBuffer8* m_pD3DVertexBuffer; 
	VOID* pVertices;
	float imgWidth = 640.0f;
	float imgHeight = 480.0f;

	NESwidth = 255;
	NESheight = 220;


	if(NESwidth < 256)
		tempwidth = 256;
	else if(NESwidth >= 256)
		tempwidth = 512;

	if(NESheight < 256)
		tempheight = 256;
	else if(NESheight >= 256)
		tempheight = 512;

	tuValue = (float)NESwidth/(float)tempwidth;
	//tuValue = 1.0f;
	tvValue = (float)NESheight/(float)tempheight;
	//float tvValue = 0.878f;
	//tvValue = 1.0f;
	tuXstart = 0.0f;
	tvYstart = 0.0f;

	g_Vertices[0].x = 0.0f;
	g_Vertices[0].y = 0.0f;
	g_Vertices[0].z = 0.5f;
	g_Vertices[0].rhw = 1.0f;
	g_Vertices[0].color= 0xffffffff;
	g_Vertices[0].tu = tuXstart; //0.0;
	g_Vertices[0].tv = tvYstart; //0.0;
	
	g_Vertices[1].x = imgWidth;
	g_Vertices[1].y = 0.0f;
	g_Vertices[1].z = 0.5f;
	g_Vertices[1].rhw = 1.0f;
	g_Vertices[1].color= 0xffffffff;
	g_Vertices[1].tu = tuValue;
	g_Vertices[1].tv = tvYstart; //0.0;

	g_Vertices[2].x = 0.0f;
	g_Vertices[2].y = imgHeight;
	g_Vertices[2].z = 0.5f;
	g_Vertices[2].rhw = 1.0f;
	g_Vertices[2].color= 0xffffffff;
	g_Vertices[2].tu = tuXstart; //0.0;
	g_Vertices[2].tv = tvValue;

	g_Vertices[3].x = imgWidth;
	g_Vertices[3].y = imgHeight;
	g_Vertices[3].z = 0.5f;
	g_Vertices[3].rhw = 1.0f;
	g_Vertices[3].color= 0xffffffff;
	g_Vertices[3].tu = tuValue;
	g_Vertices[3].tv = tvValue;

	if( FAILED( D3DXCreateTexture( m_Device,
					   tempwidth,  //width
					   tempheight,  //height
					   1,
					   0,
					   D3DFMT_A8R8G8B8, //D3DFMT_R5G6B5, 
					   D3DPOOL_MANAGED,
					   &texture)))
    {
        DbgPrint( "InitTexture():Failed to create texture!!\n" );
    }

	IDirect3DTexture8_GetLevelDesc( texture, 0, &d3dsd );
    dwDstPitch = (DWORD)d3dlr.Pitch;


	//setup texture
	if( FAILED( IDirect3DDevice8_SetTexture(m_Device, 0, texture )))
    {
        DbgPrint( "InitTexture():Failed to set texture!!\n" );
    }
	if( FAILED( IDirect3DDevice8_SetTextureStageState(m_Device, 0, D3DTSS_COLOROP,   D3DTOP_MODULATE )))
    {
        DbgPrint( "InitTexture():Failed to set texture stage!!\n" );
    }
	if( FAILED( IDirect3DDevice8_SetTextureStageState(m_Device, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE )))
    {
        DbgPrint( "InitTexture():Failed to set texture stage!!\n" );
    }
	if( FAILED( IDirect3DDevice8_SetTextureStageState(m_Device, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE )))
    {
        DbgPrint( "InitTexture():Failed to set texture stage!!\n" );
    }
	if( FAILED( IDirect3DDevice8_SetTextureStageState(m_Device, 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE )))
    {
        DbgPrint( "InitTexture():Failed to set texture stage!!\n" );
    }

	//setup vertex buffer
	if( FAILED( IDirect3DDevice8_CreateVertexBuffer(m_Device,
					   4 *sizeof(FLATVERTEX),
                       0,
					   D3DFVF_FLATVERTEX,
                       D3DPOOL_DEFAULT, 
					   &m_pD3DVertexBuffer)))
    {
        DbgPrint( "InitTexture():Failed to create vertex buffer!!\n" );
    }


	if( FAILED( IDirect3DVertexBuffer8_Lock(m_pD3DVertexBuffer, 0, sizeof(g_Vertices), (BYTE**)&pVertices, 0 )))
    {
        DbgPrint( "InitTexture():Failed to lock vertex buffer!!\n" );
    }
	memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
	if( FAILED( IDirect3DVertexBuffer8_Unlock(m_pD3DVertexBuffer)))
    {
        DbgPrint( "InitTexture():Failed to unlock vertex buffer!!\n" );
    }

	if( FAILED( IDirect3DDevice8_SetStreamSource(m_Device, 0, m_pD3DVertexBuffer, sizeof(FLATVERTEX) )))
    {
        DbgPrint( "InitTexture():Failed to set stream source!!\n" );
    }
	if( FAILED( IDirect3DDevice8_SetVertexShader(m_Device, D3DFVF_FLATVERTEX )))
	{
        DbgPrint( "InitTexture():Failed to set vertex shader!!\n" );
    }
}

__inline int calcpos(int width, int x, int y)
{
	return((width * 4) * y + (x * 4));
}

__inline int calc8pos(int x, int y)
{
	return((272 * y) + x);
}

void DrawSprite(void)
{
	extern byte *XBuf;

	// Get the texture dimensions
    D3DSURFACE_DESC desc;
	int x, y, temppos, bmppos;
	CXBSwizzler s;
	int count = 0;
    DWORD dwTexWidth, dwTexHeight, dwTexDepth;

	IDirect3DTexture8_GetLevelDesc(texture, 0, &desc );
    dwTexWidth  = desc.Width;
    dwTexHeight = desc.Height;
    dwTexDepth  = 0;
//    IDirect3DTexture8_LockRect(texture, 0, &d3dlr, 0, D3DLOCK_RAWDATA );
	IDirect3DTexture8_LockRect(texture, 0, &d3dlr, 0, 0 );
	
//	CXBSwizzler s( dwTexWidth, dwTexHeight, dwTexDepth );
	InitSwizzler( &s, dwTexWidth, dwTexHeight, dwTexDepth );

	SetV( &s, 0 );
	for(y = 0; y < 256; y++)
	{
		SetU( &s, 0 );
		for(x = 0; x < 256; x++)
		{

			temppos = calc8pos(x, y);
			bmppos = calcpos(dwTexWidth, x, y);

			((DWORD*)d3dlr.pBits)[Get2D(&s)] = (color_palette[XBuf[temppos]].peRed << 16) | (color_palette[XBuf[temppos]].peGreen << 8)
											 | (color_palette[XBuf[temppos]].peBlue);
			//((BYTE*)d3dlr.pBits)[bmppos + 3] = 0x00;

//			((WORD*)d3dlr.pBits)[s.Get2D()] = (GFX.Screen[snespos + 1] << 8) | (GFX.Screen[snespos]); 

			IncU(&s);
		}
		IncV(&s);
	}

    IDirect3DTexture8_UnlockRect(texture, 0);

	IDirect3DDevice8_DrawPrimitive(m_Device, D3DPT_TRIANGLESTRIP, 0, 2 );
}


void __cdecl main( void )
{
	// Enable the first two joysticks
	joy[0] = 1;
	joy[1] = 1;



	Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );
	ClearScreen(0x00000000);
	ShowScreen();
	InitTexture();
	fceumain();
}

int PreInit(void)
{
return 1;
}
static byte vmod;

DWORD tick1 = GetTickCount();
int FPS = 0;

void BlitScreen(void)
{
	DWORD tick3 = GetTickCount();

	ClearScreen(0x00000000);
	DrawSprite();
	ShowScreen();

	Sleep( 32 );

//	while((GetTickCount() - tick3) < 33);			// Wait
/*	
	FPS++;
	if(GetTickCount() - tick1 > 1000)
	{
		DbgPrint(" FPS : %d\n", FPS);
		FPS = 0;
		tick1 = GetTickCount();
	}
*/
}

int SetVideoMode(int vmode)
{
	DbgPrint( "SetVideoMode\n" );

  return 1;
}

byte vretrace=1;
static long ZK;
static long ZL;
void WaitForVBlank(void)
{

}


void ResetVideo(void)
{
		DbgPrint( "Resetvideo\n" );

}


//LPDIRECTINPUTDEVICE lpdid=0;

int KeyboardInitialize(void)
{
		DbgPrint( "KeyboardInit\n" );

return 1;
}

void KeyboardClose(void)
{
		DbgPrint( "KeyboardClose\n" );
// if(lpdid) IDirectInputDevice_Unacquire(lpdid);
// lpdid=0;
}

int KeyboardUpdate(void){return 1;}

char buf[256];
char *KeyboardGetstate(void)
{
//DbgPrint( "keystate\n" );
/*ddrval=IDirectInputDevice_GetDeviceState(lpdid,256,buf);
switch(ddrval)
 {
  case DIERR_INPUTLOST:
  case DIERR_NOTACQUIRED:
                        IDirectInputDevice_Acquire(lpdid);
                        break;
 } */
return buf;
}

USBManager Controllers;

unsigned long GetJSOr(void)
{
    bool bButtonPressed;
    bool bFirstPress = 0;

	Controllers.ProcessInput();
	
	unsigned long ret = 0;

	// Controller 1
	int x = 0;
	if( bButtonPressed = Controllers.IsControlPressed( PORT_1, CONTROL_DPAD_LEFT, bFirstPress ) )
	{
		ret|=JOY_LEFT << (x << 3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_1, CONTROL_DPAD_RIGHT, bFirstPress ) )
	{
		ret|=JOY_RIGHT << (x << 3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_1, CONTROL_DPAD_TOP, bFirstPress ) )
	{
		ret|=JOY_UP << (x << 3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_1, CONTROL_DPAD_BOTTOM, bFirstPress ) )
	{
		ret|=JOY_DOWN << (x << 3);
	}
	if( bButtonPressed = Controllers.IsButtonPressed( PORT_1, BUTTON_A, bFirstPress ) )
	{
		ret|=1<<(x<<3);
	}
	if( bButtonPressed = Controllers.IsButtonPressed( PORT_1, BUTTON_C, bFirstPress ) )
	{
		ret|=2<<(x<<3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_1, CONTROL_SELECT, bFirstPress ) )
	{
		ret|=4<<(x<<3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_1, CONTROL_START, bFirstPress ) )
	{
		ret|=8<<(x<<3);
	}

	// Controller 2
	x = 1;
		if( bButtonPressed = Controllers.IsControlPressed( PORT_2, CONTROL_DPAD_LEFT, bFirstPress ) )
	{
		ret|=JOY_LEFT << (x << 3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_2, CONTROL_DPAD_RIGHT, bFirstPress ) )
	{
		ret|=JOY_RIGHT << (x << 3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_2, CONTROL_DPAD_TOP, bFirstPress ) )
	{
		ret|=JOY_UP << (x << 3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_2, CONTROL_DPAD_BOTTOM, bFirstPress ) )
	{
		ret|=JOY_DOWN << (x << 3);
	}
	if( bButtonPressed = Controllers.IsButtonPressed( PORT_2, BUTTON_A, bFirstPress ) )
	{
		ret|=1<<(x<<3);
	}
	if( bButtonPressed = Controllers.IsButtonPressed( PORT_2, BUTTON_C, bFirstPress ) )
	{
		ret|=2<<(x<<3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_2, CONTROL_SELECT, bFirstPress ) )
	{
		ret|=4<<(x<<3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_2, CONTROL_START, bFirstPress ) )
	{
		ret|=8<<(x<<3);
	}


	// Controller 3
	x = 2;
	if( bButtonPressed = Controllers.IsControlPressed( PORT_3, CONTROL_DPAD_LEFT, bFirstPress ) )
	{
		ret|=JOY_LEFT << (x << 3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_3, CONTROL_DPAD_RIGHT, bFirstPress ) )
	{
		ret|=JOY_RIGHT << (x << 3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_3, CONTROL_DPAD_TOP, bFirstPress ) )
	{
		ret|=JOY_UP << (x << 3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_3, CONTROL_DPAD_BOTTOM, bFirstPress ) )
	{
		ret|=JOY_DOWN << (x << 3);
	}
	if( bButtonPressed = Controllers.IsButtonPressed( PORT_3, BUTTON_A, bFirstPress ) )
	{
		ret|=1<<(x<<3);
	}
	if( bButtonPressed = Controllers.IsButtonPressed( PORT_3, BUTTON_C, bFirstPress ) )
	{
		ret|=2<<(x<<3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_3, CONTROL_SELECT, bFirstPress ) )
	{
		ret|=4<<(x<<3);
	}
	if( bButtonPressed = Controllers.IsControlPressed( PORT_3, CONTROL_START, bFirstPress ) )
	{
		ret|=8<<(x<<3);
	}

	return ret;
}

void KillJoystick(void){} 

char InitJoystick(void)
{
		DbgPrint( "InitJoystick\n" );
/*int x;
for(x=0;x<4;x++) joyF[x]=0;
joycounter=1;
puts("Enumerating joysticks...");
IDirectInput_EnumDevices(lpDI, DIDEVTYPE_JOYSTICK,JoystickFound,0,DIEDFL_ATTACHEDONLY);
for(x=0;x<4;x++) 
 if(!joyF[x]) joy[x]=0;
HideMainWindow();
for(x=0;x<4;x++)
 {
  if(!joy[x]) continue;
  
  ddrval=IDirectInput_CreateDevice2(lpDI, &joyGUID[x],&lpJoy[x],0);
  if (ddrval != DI_OK)
  {
   printNonFatal("DirectInput: Error creating DirectInput joystick device.");
   joy[x]=0;
   continue;
  }

  ddrval=IDirectInputDevice2_SetCooperativeLevel(lpJoy[x], hAppWnd, DISCL_BACKGROUND|DISCL_NONEXCLUSIVE);
  if (ddrval != DI_OK)
  {
   printNonFatal("DirectInput: Error creating DirectInput joystick cooperative level.");
   joy[x]=0;
   continue;
  }
                  
  ddrval=IDirectInputDevice2_SetDataFormat(lpJoy[x],&c_dfDIJoystick);
  if (ddrval != DI_OK)
  {
   printNonFatal("DirectInput: Error creating DirectInput joystick data format.");
   joy[x]=0;
   continue;
  }

  ddrval=IDirectInputDevice2_Acquire(lpJoy[x]);
  if (ddrval != DI_OK)
  {
   printNonFatal("DirectInput: Error acquiring DirectInput joystick.");
   joy[x]=0;
   continue;
  }

  IDirectInputDevice2_Poll(lpJoy[x]);
  ddrval=IDirectInputDevice2_GetDeviceState(lpJoy[x],sizeof(JoyStatus),&JoyStatus);
  if (ddrval != DI_OK)
  {
   printNonFatal("DirectInput: Error getting DirectInput joystick state.");
   joy[x]=0;
   continue;
  }

  JoyXCenter[x]=JoyStatus.lX;
  JoyYCenter[x]=JoyStatus.lY;
 }
*/
return 0;
}



void TrashSound()
{
	if ( g_pBuffer )
	{
		IDirectSoundBuffer_Release( g_pBuffer );
	}
}

DWORD __stdcall DSThread( void *p )
{
	static unsigned char* pBuffer;
	static DWORD dwBytes;
	DWORD i;
	HRESULT hr;

	hr = IDirectSoundBuffer_Play( g_pBuffer, 0, 0, DSBPLAY_LOOPING );

	i = 0;

	for ( ;; )
	{
		hr = IDirectSoundBuffer_Lock( g_pBuffer, i * 256, 256, (LPVOID*) &pBuffer, &dwBytes, NULL, NULL, 0 );

/*		for ( i = 0; i < 256; i++ )
		{
			pBuffer[i] = rand();
		}
*/
		if ( SUCCEEDED( hr ) )
		{
			FillSoundBuffer( pBuffer );
		}

		i = ++i % 210;
	}

	return SUCCEEDED( hr );
}

HRESULT MakeDSThread( void )
{
	HANDLE hThread = CreateThread( NULL, 4096,  DSThread, 0, 0, NULL );

	return NULL != hThread ? S_OK : E_OUTOFMEMORY;
}

int InitSound( void )
{
	WAVEFORMATEX wfx;
	DSBUFFERDESC dsbd;
	HRESULT	hr = S_OK;

	DbgPrint( "InitSound\n" );

	ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample= 8;
	wfx.nBlockAlign = 1;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	
	dsbd.dwBufferBytes = 256 * 210;
	dsbd.dwFlags = 0;
	dsbd.dwSize = sizeof( DSBUFFERDESC );
	dsbd.lpwfxFormat = &wfx;
	
	hr = DirectSoundCreateBuffer( &dsbd, &g_pBuffer );

	if ( SUCCEEDED( hr ) )
	{
		hr = MakeDSThread();
	}

	if ( SUCCEEDED( hr ) )
	{
		return 44100;
	}
	else
	{
		return 0;
	}
}

#include "netplay.c"

