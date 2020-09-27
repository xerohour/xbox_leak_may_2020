// XBContent.h: interface for the XBContent class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "unknown.hpp"
#include "bundler.h"
#include "anim.h"

class XBContent : public _unknown2<ISAXContentHandler, ISAXErrorHandler>
{
public:
    XBContent();
    virtual ~XBContent();
	
protected:
	DWORD m_IncludeCount;	// include recursion count
	friend class CBundler;	// expose locator and error routines to bundler
	CBundler *m_pBundler;	// the bundler class writes the compiled .xbr file
	ISAXLocator *m_pLocator;	// current file name and location for error reporting
	bool m_bErrorSuppress;		// used to write error reports just once
	CHAR *m_strTemp;		// temporary string storage returned by CharString
	INT m_cchTemp;			// size of current tempory string buffer
	AnimLink *m_pAnimList;	// list of animations in the current animation block
	AnimVertexShaderParameter *m_pAnimVSPList;	// list of targets for animation
	
	//////////////////////////////////////////////////////////////////////
	// Counts for  resource name generation
	//
	struct {
		DWORD m_Texture;
		DWORD m_Surface;
		DWORD m_CubeTexture;
		DWORD m_VolumeTexture;
		DWORD m_VertexBuffer;
		DWORD m_IndexBuffer;
		DWORD m_VertexShader;
		DWORD m_PixelShader;
		DWORD m_Skeleton;
		DWORD m_Animation;
		DWORD m_Effect;
	} m_Count;
	
	//////////////////////////////////////////////////////////////////////
	// The context determines how the incoming tokens are routed.
	//
	enum Context {
		Root_Context,
		XDX_Context,
		include_Context,
		Texture_Context,
//		Palette_Context,
		Surface_Context,
		CubeTexture_Context,
		VolumeTexture_Context,
		VertexBuffer_Context,
		Vertex_Context,
		IndexBuffer_Context,
		VertexShader_Context,
		VertexShader_decl_Context,
		VertexShader_decl_stream_Context,
		VertexShader_decl_stream_vreg_Context,
		VertexShader_asm_Context,
		VertexShader_Constant_Context,
		PixelShader_Context,
		PixelShader_asm_Context,
		PixelShader_Constant_Context,
		Frame_Context,
		Matrix_Context,
		Matrix_animate_Context,
		Rotate_Context,
		Rotate_animate_Context,
		Scale_Context,
		Scale_animate_Context,
		Translate_Context,
		Translate_animate_Context,
		Effect_Context,
		Pass_Context,
		RenderState_Context,
		TextureState_Context,
		Draw_Context,
	};
#define MAX_CONTEXT 100
	int m_iContext;
	struct StackElement {
		Context m_Context;
		void *m_pData;
	} m_rContextStack[MAX_CONTEXT];

	HRESULT PushContext(Context NewContext)
	{
		if (m_iContext >= MAX_CONTEXT - 1)
			return E_FAIL;
		m_iContext++;
		m_rContextStack[m_iContext].m_Context = NewContext;
		m_rContextStack[m_iContext].m_pData = NULL;
		return S_OK;
	}
	
	HRESULT PopContext()
	{
		if (m_iContext <= 0)
			return E_FAIL;
		// TODO: free m_pData if non-NULL
		m_iContext--;
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////
	// Parsing helpers
	//
	HRESULT UnexpectedToken(WCHAR *pwchChars, int cchChars, HRESULT errCode);
	HRESULT UnexpectedAttributeToken(WCHAR *pwchLocalName, int cchLocalName,
									 WCHAR *pwchValue, int cchValue,
									 HRESULT errCode);
	HRESULT MustBeWhitespace(WCHAR *pwchChars, int cchChars);
	CHAR *CharString(WCHAR *pwch, int cch, int *pcchOut = NULL);	// converts a string of wide characters to a static string

	
	//////////////////////////////////////////////////////////////////////
	// Animate helpers
	//
	HRESULT animate(AnimCurve *pCurve, ISAXAttributes *pAttributes,
					CONST WCHAR *wstrAttribute);	// if non-NULL, this must match the attribute="name" 
	HRESULT ValidateAnimation(AnimCurve *pCurve,
							  BOOL bWrapAngle);		// do comparisons mod 2 * pi
	
	//////////////////////////////////////////////////////////////////////
	// Handlers
	//
	HRESULT Begin_include(ISAXAttributes *pAttributes);
	HRESULT End_include();
	
	HRESULT BeginCubeTexture(ISAXAttributes *pAttributes);
	HRESULT EndCubeTexture();
	
	HRESULT BeginIndexBuffer(ISAXAttributes *pAttributes);
	HRESULT IndexBufferChars(WCHAR *pwchChars, int cchChars);
	HRESULT EndIndexBuffer();

	HRESULT BeginMatrix(ISAXAttributes *pAttributes);
	HRESULT EndMatrix();
	HRESULT BeginMatrix_animate(ISAXAttributes *pAttributes);
	HRESULT EndMatrix_animate();
	
	HRESULT BeginRotate(ISAXAttributes *pAttributes);
	HRESULT EndRotate();
	HRESULT BeginRotate_animate(ISAXAttributes *pAttributes);
	HRESULT EndRotate_animate();
	
	HRESULT BeginScale(ISAXAttributes *pAttributes);
	HRESULT EndScale();
	HRESULT BeginScale_animate(ISAXAttributes *pAttributes);
	HRESULT EndScale_animate();
	
	HRESULT BeginTranslate(ISAXAttributes *pAttributes);
	HRESULT EndTranslate();
	HRESULT BeginTranslate_animate(ISAXAttributes *pAttributes);
	HRESULT EndTranslate_animate();
	
	HRESULT BeginSurface(ISAXAttributes *pAttributes);
	HRESULT EndSurface();
	
	HRESULT BeginTexture(ISAXAttributes *pAttributes);
	HRESULT TextureChars(WCHAR *pwchChars, int cchChars);
	HRESULT EndTexture();
	
//	HRESULT BeginPalette(ISAXAttributes *pAttributes);
//	HRESULT EndPalette();
	
	HRESULT BeginVertex(ISAXAttributes *pAttributes);
	HRESULT VertexChars(WCHAR *pwchChars, int cchChars);
	HRESULT EndVertex();
	
	HRESULT BeginVertexBuffer(ISAXAttributes *pAttributes);
	HRESULT EndVertexBuffer();
	
	HRESULT BeginVertexShader(ISAXAttributes *pAttributes);
	HRESULT EndVertexShader();

	HRESULT BeginVertexShader_decl(ISAXAttributes *pAttributes);
	HRESULT EndVertexShader_decl();

	HRESULT BeginVertexShader_decl_stream(ISAXAttributes *pAttributes);
	HRESULT EndVertexShader_decl_stream();

	HRESULT BeginVertexShader_decl_stream_vreg(ISAXAttributes *pAttributes);
	HRESULT EndVertexShader_decl_stream_vreg();

	HRESULT BeginVertexShader_asm(ISAXAttributes *pAttributes);
	HRESULT VertexShader_asm_Chars(WCHAR *pwchChars, int cchChars);
	HRESULT EndVertexShader_asm();

	HRESULT BeginVertexShader_Constant(ISAXAttributes *pAttributes);
	HRESULT VertexShader_Constant_Chars(WCHAR *pwchChars, int cchChars);
	HRESULT EndVertexShader_Constant();

	HRESULT BeginPixelShader(ISAXAttributes *pAttributes);
	HRESULT EndPixelShader();

	HRESULT BeginPixelShader_asm(ISAXAttributes *pAttributes);
	HRESULT PixelShader_asm_Chars(WCHAR *pwchChars, int cchChars);
	HRESULT EndPixelShader_asm();

	HRESULT BeginPixelShader_Constant(ISAXAttributes *pAttributes);
	HRESULT PixelShader_Constant_Chars(WCHAR *pwchChars, int cchChars);
	HRESULT EndPixelShader_Constant();

	HRESULT BeginFrame(ISAXAttributes *pAttributes);
	HRESULT EndFrame();
	
	HRESULT BeginEffect(ISAXAttributes *pAttributes);
	HRESULT EndEffect();
	
	HRESULT BeginPass(ISAXAttributes *pAttributes);
	HRESULT EndPass();
	
	HRESULT BeginRenderState(ISAXAttributes *pAttributes);
	HRESULT EndRenderState();
	
	HRESULT BeginTextureState(ISAXAttributes *pAttributes);
	HRESULT EndTextureState();
	
	HRESULT BeginDraw(ISAXAttributes *pAttributes);
	HRESULT EndDraw();
	
	HRESULT BeginXDX(ISAXAttributes *pAttributes);
	HRESULT EndXDX();

	//////////////////////////////////////////////////////////////////////
	// Resource writing
	//
	HRESULT WriteSkeleton(FrameExtra *pFrame);
	HRESULT WriteAnimation(AnimLink *pAnimList);

public:
 
	//////////////////////////////////////////////////////////////////////
	// ISAXContentHandler interface
	//
	virtual HRESULT STDMETHODCALLTYPE putDocumentLocator( 
        /* [in] */ ISAXLocator __RPC_FAR *pLocator);
    
    virtual HRESULT STDMETHODCALLTYPE startDocument( void);
    
    virtual HRESULT STDMETHODCALLTYPE endDocument( void);
    
    virtual HRESULT STDMETHODCALLTYPE startPrefixMapping( 
        /* [in] */ wchar_t __RPC_FAR *pwchPrefix,
        /* [in] */ int cchPrefix,
        /* [in] */ wchar_t __RPC_FAR *pwchUri,
        /* [in] */ int cchUri);
    
    virtual HRESULT STDMETHODCALLTYPE endPrefixMapping( 
        /* [in] */ wchar_t __RPC_FAR *pwchPrefix,
        /* [in] */ int cchPrefix);
    
    virtual HRESULT STDMETHODCALLTYPE startElement( 
        /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
        /* [in] */ int cchNamespaceUri,
        /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
        /* [in] */ int cchLocalName,
        /* [in] */ wchar_t __RPC_FAR *pwchRawName,
        /* [in] */ int cchRawName,
        /* [in] */ ISAXAttributes __RPC_FAR *pAttributes);
    
    virtual HRESULT STDMETHODCALLTYPE endElement( 
        /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
        /* [in] */ int cchNamespaceUri,
        /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
        /* [in] */ int cchLocalName,
        /* [in] */ wchar_t __RPC_FAR *pwchRawName,
        /* [in] */ int cchRawName);
    
    virtual HRESULT STDMETHODCALLTYPE characters( 
        /* [in] */ wchar_t __RPC_FAR *pwchChars,
        /* [in] */ int cchChars);
    
    virtual HRESULT STDMETHODCALLTYPE ignorableWhitespace( 
        /* [in] */ wchar_t __RPC_FAR *pwchChars,
        /* [in] */ int cchChars);
    
    virtual HRESULT STDMETHODCALLTYPE processingInstruction( 
        /* [in] */ wchar_t __RPC_FAR *pwchTarget,
        /* [in] */ int cchTarget,
        /* [in] */ wchar_t __RPC_FAR *pwchData,
        /* [in] */ int cchData);
    
    virtual HRESULT STDMETHODCALLTYPE skippedEntity( 
        /* [in] */ wchar_t __RPC_FAR *pwchName,
        /* [in] */ int cchName);

	//////////////////////////////////////////////////////////////////////
	// ISAXErrorHandler interface
	//
	virtual HRESULT STDMETHODCALLTYPE error( 
		/* [in] */ ISAXLocator __RPC_FAR *pLocator,
		/* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode);
	
	virtual HRESULT STDMETHODCALLTYPE fatalError( 
		/* [in] */ ISAXLocator __RPC_FAR *pLocator,
		/* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode);
	
	virtual HRESULT STDMETHODCALLTYPE ignorableWarning( 
		/* [in] */ ISAXLocator __RPC_FAR *pLocator,
		/* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode);
};
