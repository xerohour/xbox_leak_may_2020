#ifndef __ImageFile_H
#define __ImageFile_H

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

class BitmapFile {
private:
	int ImageSize;
	DWORD *pixels;
	int xpos, ypos;

	// Bitmap File Header
	WORD Type;
	DWORD FileSize;
	WORD Reserved1;
	WORD Reserved2;
	DWORD OffBits;
	
	// Bitmap Info Header
	DWORD InfoSize;
	DWORD Width;
	DWORD Height;
	WORD Planes;
	WORD BitCount;
	DWORD Compression;
	DWORD SizeImage;
	DWORD XPelsPerMeter;
	DWORD YPelsPerMeter;
	DWORD ClrUsed;
	DWORD ClrImportant;

public:
	BitmapFile(void);
	~BitmapFile(void);
	
	void write(char *file, int imageWidth, int imageHeight, DWORD *pixels);
	int imagesize(char *file);
	void read(int xloc, int yloc, char *file);
	bool render(IDirect3DDevice8 *d3dDevice);

    DWORD GetWidth() { return Width; };         // Get the Width of the bitmap file
    DWORD GetHeight() { return Height; };       // Get the Height of the bitmap file
    int   GetXPos() { return xpos; };           // Get the X Position of the bitmap file
    int   GetYPos() { return ypos; };           // Get the Y Position of the bitmap file
    void  SetXPos( int x ) { xpos = x; };       // Set the X Position of the bitmap file
    void  SetYPos( int y ) { ypos = y; };       // Set the Y Position of the bitmap file
};

#endif