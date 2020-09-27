#include "recovpch.h"
#include "imagefile.h"

//////////////////////////////////////////////////////////////////////
// .BMP File format functions

BitmapFile::BitmapFile() {
	xpos = 0;
	ypos = 0;

	// Initialize Bitmap File Header
	Type	  = 19778;
	FileSize  = 192056;	//check this
	Reserved1 = 0;
	Reserved2 = 0;
	OffBits	  = 54;	//check this

	// Initialize Bitmap Info Header
	InfoSize	  = 40;
	Width		  = 0;
	Height		  = 0;
	Planes		  = 1;
	BitCount	  = 24;
	Compression	  = 0;
	SizeImage	  = 0;
	XPelsPerMeter = 2834;
	YPelsPerMeter = 2834;
	ClrUsed		  = 0;
	ClrImportant  = 0;
}

BitmapFile::~BitmapFile() {
    if( pixels )
    {
	    delete [] pixels;
        pixels = NULL;
    }
}

void BitmapFile::write(char *file, int imageWidth, int imageHeight, DWORD *pixels) {
	FILE *output;

	Width     = imageWidth;
	Height	  = imageHeight;
	ImageSize = imageWidth * imageHeight;

	//open the new bitmap file
	output = fopen(file, "wb");

	// Write the Bitmap File header information
	fwrite(&Type, sizeof(WORD), 1, output);
	fwrite(&FileSize, sizeof(DWORD), 1, output);
	fwrite(&Reserved1, sizeof(WORD), 1, output);
	fwrite(&Reserved2, sizeof(WORD), 1, output);
	fwrite(&OffBits, sizeof(DWORD), 1, output);

	//Write the Bitmap Info header information
	fwrite(&InfoSize, sizeof(DWORD), 1, output);
	fwrite(&Width, sizeof(DWORD), 1, output);
	fwrite(&Height, sizeof(DWORD), 1, output);
	fwrite(&Planes, sizeof(WORD), 1, output);
	fwrite(&BitCount, sizeof(WORD), 1, output);
	fwrite(&Compression, sizeof(DWORD), 1, output);
	fwrite(&SizeImage, sizeof(DWORD), 1, output);
	fwrite(&XPelsPerMeter, sizeof(DWORD), 1, output);
	fwrite(&YPelsPerMeter, sizeof(DWORD), 1, output);
	fwrite(&ClrUsed, sizeof(DWORD), 1, output);
	fwrite(&ClrImportant, sizeof(DWORD), 1, output);

	fclose(output);
}

int BitmapFile::imagesize(char *file) {
	FILE *input;
	input = fopen(file, "rb");

	// Read the Bitmap File header information
	fread(&Type, sizeof(WORD), 1, input);
	fread(&FileSize, sizeof(DWORD), 1, input);
	fread(&Reserved1, sizeof(WORD), 1, input);
	fread(&Reserved2, sizeof(WORD), 1, input);
	fread(&OffBits, sizeof(DWORD), 1, input);

	//Read the Bitmap Info header information
	fread(&InfoSize, sizeof(DWORD), 1, input);
	fread(&Width, sizeof(DWORD), 1, input);
	fread(&Height, sizeof(DWORD), 1, input);

	fclose(input);

	return (Width * Height);
}

void BitmapFile::read(int xloc, int yloc, char *file) {
	FILE *input;
	xpos = xloc;
	ypos = yloc;

	//open the new bitmap file
	input = fopen(file, "rb");
    if( !input )
    {
        XDBGWRN( APP_TITLE_NAME_A, "BitmapFile::read():fopen failed!! - '%s'\n", file );
        return;
    }

	// Read the Bitmap File header information
	fread(&Type, sizeof(WORD), 1, input);
	fread(&FileSize, sizeof(DWORD), 1, input);
	fread(&Reserved1, sizeof(WORD), 1, input);
	fread(&Reserved2, sizeof(WORD), 1, input);
	fread(&OffBits, sizeof(DWORD), 1, input);

	//Read the Bitmap Info header information
	fread(&InfoSize, sizeof(DWORD), 1, input);
	fread(&Width, sizeof(DWORD), 1, input);
	fread(&Height, sizeof(DWORD), 1, input);
	fread(&Planes, sizeof(WORD), 1, input);
	fread(&BitCount, sizeof(WORD), 1, input);
	fread(&Compression, sizeof(DWORD), 1, input);
	fread(&SizeImage, sizeof(DWORD), 1, input);
	fread(&XPelsPerMeter, sizeof(DWORD), 1, input);
	fread(&YPelsPerMeter, sizeof(DWORD), 1, input);
	fread(&ClrUsed, sizeof(DWORD), 1, input);
	fread(&ClrImportant, sizeof(DWORD), 1, input);

	ImageSize = Width * Height;
	pixels = new DWORD[ImageSize];

	float remainer = (float)Width / 4;
	float delta = remainer - (int)remainer;

	BYTE blue, green, red;
	for(int y = (int)Height - 1; y >= 0 ; y--)
	{
		for(int x = 0; x < (int)Width; x++)
		{
			fread(&blue, sizeof(BYTE), 1, input);
			fread(&green, sizeof(BYTE), 1, input);
			fread(&red, sizeof(BYTE), 1, input);
			int position = (y * (int)Width) + x;
			pixels[position] = (0xff << 24) | (red << 16) | (green << 8) | (blue);
		}
		
		//account for the extra bytes in bitmap files
		if(delta == 0.25f) fseek(input, 1, SEEK_CUR);
		else if(delta == 0.50f) fseek(input, 2, SEEK_CUR);
		else if(delta == 0.75f) fseek(input, 3, SEEK_CUR);
	}

	fclose(input);
}

bool BitmapFile::render(IDirect3DDevice8 *d3dDevice) {
	HRESULT hr;
	IDirect3DSurface8 *backBuffer;
	D3DLOCKED_RECT backBufferInfo;
	hr = d3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
	if( FAILED( hr ))
	{
		XDBGWRN( APP_TITLE_NAME_A, "BitmapFile::render():Failed to get the backbuffer\n" );
		return false;
	}

	RECT container;
	container.top = ypos;
	container.left = xpos;
	container.bottom = ypos + Height;
	container.right = xpos + Width;	

	hr = backBuffer->LockRect(&backBufferInfo, &container, D3DLOCK_TILED);
	if( FAILED( hr ))
	{
		XDBGWRN( APP_TITLE_NAME_A, "BitmapFile::render():Failed to lock the backbuffer\n" );
        backBuffer->Release();
		return false;
	}

	int destPitch = backBufferInfo.Pitch;
	int sourcePitch = Width * 4;
	BYTE *s = (BYTE*)pixels;
	BYTE *p = (BYTE*)backBufferInfo.pBits;
	for(int y = 0; y < (int)Height; y++)
	{
		memcpy(p, s, sourcePitch);
		s += sourcePitch;
		p += destPitch;
	}

	backBuffer->UnlockRect();
    backBuffer->Release();
	return true;
}