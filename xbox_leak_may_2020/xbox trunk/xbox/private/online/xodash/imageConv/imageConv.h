//-----------------------------------------------------------------------------
// File: imageConv.h
//
// Desc: Header file for the Image Converter
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef IMAGECONV_H
#define IMAGECONV_H

#include <assert.h>
#include <windows.h>
#include <d3dx8.h>
#include <list>

class CImageItem
{
public:
	CHAR		 fullpath[MAX_PATH];	// Holds the full path and filename of the image
	CHAR		 filename[MAX_PATH]; 	// Holds the actual name of the file
	CHAR		 outputpath[MAX_PATH];  // Holds the destination location for the image
	unsigned int width;					// Width of Image
	unsigned int height;				// Height of Image
	unsigned int depth;					// Depth of Image
};

using namespace std;
typedef list<CImageItem*>  ImageList;

#pragma pack(1)

// TGA file header data
typedef struct
{
	BYTE		   image_id_len;
	BYTE		   color_map;
	BYTE		   image_type;
	unsigned short cm_first_entry;
	unsigned short cm_length;
	BYTE		   cm_entry_size;
	unsigned short x_origin;
	unsigned short y_origin;
	unsigned short width;
	unsigned short height;
	BYTE		   pixel_depth;
	BYTE		   image_descriptor;
} TGAHeader;

// BMP file header data
typedef struct
{ 
	unsigned short Type;          // signature - 'BM'
	unsigned long  Size;          // file size in bytes
	unsigned short Reserved1;     // 0
	unsigned short Reserved2;     // 0
	unsigned long  OffBits;       // offset to bitmap
	unsigned long  StructSize;    // size of this struct (40)
	unsigned long  Width;         // bmap width in pixels
	unsigned long  Height;        // bmap height in pixels
	unsigned short Planes;        // num planes - always 1
	unsigned short BitCount;      // bits per pixel
	unsigned long  Compression;   // compression flag
	unsigned long  SizeImage;     // image size in bytes
	long           XPelsPerMeter; // horz resolution
	long           YPelsPerMeter; // vert resolution
	unsigned long  ClrUsed;       // 0 -> color table size
	unsigned long  ClrImportant;  // important color count
} BMPHeader;

#pragma pack()



//-----------------------------------------------------------------------------
// Name: class CImageConv
// Desc: Main application class for driving the Image Converter.  Handles 
//		 directory polling, error checking, and the convertion.
//-----------------------------------------------------------------------------
class CImageConv
{
public:
    CImageConv();
    ~CImageConv();
	
	void    PrintUsage();

	HRESULT Initialize ( int argc, char * argv[] );
	HRESULT ValidateDir ( char* dir );
	HRESULT FindImages ( char* extension );
	HRESULT FillImageInfo ( CImageItem *imageFile );
	HRESULT CheckImageDest ( CImageItem *imageFile );

	// Linked list of original image filenames
	int				m_nImageFiles;	
	ImageList		m_ImageFiles;
   
public:

	// Path Strings
	CHAR   m_strInputDir[MAX_PATH];
    CHAR   m_strOutputDir[MAX_PATH];

    // For specifying output directories on cmd line vs. in code
    BOOL   m_bExplicitInput;
    BOOL   m_bExplicitOutput;  
	BOOL   m_bForceOutput;  
};


#endif // IMAGECONV_H