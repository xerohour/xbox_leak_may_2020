#include <windows.h>
#include <d3dx8.h>
#include <xgraphics.h>
#include <stdio.h>
#include "Bundler.h"
#include "LoadImage.h"
#include "BaseTexture.h"

#ifdef _DEBUG
#pragma comment(lib, "../../../Public/wsdk/lib/i386/xgraphicsd.lib")
#else
#pragma comment(lib, "../../../Public/wsdk/lib/i386/xgraphics.lib")
#endif

#pragma comment(lib, "d3d8.lib")
#pragma comment(lib, "d3dx8.lib")

CBundler::CBundler()
{
	m_pd3d = NULL;
	m_pd3ddev = NULL;
	m_hFile = INVALID_HANDLE_VALUE;
}

bool CBundler::Init()
{
    HRESULT hr;
    D3DDISPLAYMODE dispMode;
    D3DPRESENT_PARAMETERS presentParams;

    m_pd3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (m_pd3d == NULL)
    {
        ErrorMsg("Couldn't create Direct3d - is it installed?\n");
        return false;
    }

    m_pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispMode);

    ZeroMemory(&presentParams, sizeof(presentParams));
    presentParams.Windowed = TRUE;
    presentParams.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
    presentParams.BackBufferWidth = 8;
    presentParams.BackBufferHeight = 8;
    presentParams.BackBufferFormat = dispMode.Format;

    hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, GetDesktopWindow(),
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &presentParams, &m_pd3ddev);
    if (FAILED(hr))
    {
        ErrorMsg("Couldn't create reference device\n");
        return false;
    }

    return true;
}

void CBundler::ErrorMsg( const char * szMsg, ... )
{
	va_list args;
	va_start(args, szMsg);

	char szBuffer [512];
	_vsnprintf(szBuffer, sizeof(szBuffer), szMsg, args);
	puts(szBuffer);

	va_end(args);
}

HRESULT CBundler::WriteData( void * pbBuff, DWORD cb )
{
	DWORD dwWrite;
	WriteFile(m_hFile, pbBuff, cb, &dwWrite, NULL);
	return S_OK;
}

HRESULT CBundler::PadToAlignment( DWORD dwAlign )
{
	return S_OK;
}


char* szInputFile = NULL;
char* szOutputFile = NULL;
char* szOutputFormat = NULL;
int nOutputWidth = 0;
int nOutputHeight = 0;
int nOutputLevels = 1;

void ProcessFile()
{
	CBundler bundler;
	if (!bundler.Init())
		exit(1);

	CBaseTexture texture(&bundler);
    LPDIRECT3DSURFACE8 pSurface;

	printf("Converting %s to %s...\n", szInputFile, szOutputFile);
	if (texture.LoadSurface(szInputFile, "", &pSurface))
	{
		fprintf(stderr, "Error loading %s\n", szInputFile);
		exit(2);
	}

	if (szOutputFormat == NULL)
		szOutputFormat = "D3DFMT_A8R8G8B8";

	int nFormat = texture.FormatFromString(szOutputFormat);
	if (nFormat < 0)
	{
		printf("Unknown format %s\n", szOutputFormat);
		exit(1);
	}

	texture.m_nFormat = nFormat;

	if (nOutputWidth == 0 || nOutputHeight == 0)
	{
		D3DSURFACE_DESC desc;
		pSurface->GetDesc(&desc);

		for (nOutputWidth = 1;  nOutputWidth  < (int)desc.Width; nOutputWidth <<= 1)
			;
		for (nOutputHeight = 1; nOutputHeight < (int)desc.Height; nOutputHeight <<= 1)
			;

		printf("\t%dx%d pixels\n", nOutputWidth, nOutputHeight);
	}

	if (FAILED(texture.ResizeSurface(nOutputWidth, nOutputHeight, &pSurface)))
	{
		fprintf(stderr, "Failed to resize!\n");
		exit(2);
	}

	bundler.m_hFile = CreateFile(szOutputFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (bundler.m_hFile == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Cannot create %s\n", szOutputFile);
		exit(2);
	}

	D3DTexture d3dtex;
	XGSetTextureHeader(nOutputWidth, nOutputHeight, nOutputLevels, 0, (D3DFORMAT)g_TextureFormats[nFormat].dwXboxFormat, D3DPOOL_DEFAULT, (IDirect3DTexture8*)&d3dtex, 0, 0);
	bundler.WriteData(&d3dtex, sizeof(d3dtex));

	DWORD cbData = 0;
	texture.SaveSurface(&cbData, nOutputLevels, pSurface);

	CloseHandle(bundler.m_hFile);
}

void UsageExit()
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\txt [options] input output\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "\t-f d3dformat\n");
	fprintf(stderr, "\t-w width\n");
	fprintf(stderr, "\t-h height\n");
	fprintf(stderr, "\t-m mipmap-levels\n");
	exit(1);
}

int __cdecl main(int argc, char* argv[])
{
	for (int i = 1; i < argc; i += 1)
	{
		char* sz = argv[i];

		if (sz[0] == '-' || sz[0] == '/')
		{
			switch (sz[1])
			{
			default:
				UsageExit();
				break;

			case 'f':
				if (sz[2] == 0)
				{
					i += 1;
					sz = argv[i];
				}
				else
				{
					sz += 2;
				}

				szOutputFormat = sz;
				break;

			case 'w':
				if (sz[2] == 0)
				{
					i += 1;
					sz = argv[i];
				}
				else
				{
					sz += 2;
				}

				nOutputWidth = atoi(sz);
				break;

			case 'h':
				if (sz[2] == 0)
				{
					i += 1;
					sz = argv[i];
				}
				else
				{
					sz += 2;
				}

				nOutputHeight = atoi(sz);
				break;

			case 'm':
				if (sz[2] == 0)
				{
					i += 1;
					sz = argv[i];
				}
				else
				{
					sz += 2;
				}

				nOutputLevels = atoi(sz);
				break;
			}
		}
		else
		{
			if (szInputFile == NULL)
				szInputFile = sz;
			else if (szOutputFile == NULL)
				szOutputFile = sz;
			else
				UsageExit();
		}
	}

	if (szInputFile == NULL || szOutputFile == NULL)
		UsageExit();

	ProcessFile();

	return 0;
}
