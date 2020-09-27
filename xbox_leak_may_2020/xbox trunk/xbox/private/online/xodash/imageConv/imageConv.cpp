//-----------------------------------------------------------------------------
// File: imageConv.cpp
//
// Desc: Utility for converting a directory of images to the Xbox Online
//		 Dashboard bundled format.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "imageConv.h"
#include "bundler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Name: CImageConv()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CImageConv::CImageConv()
{
    m_strInputDir[0] = m_strOutputDir[0] = 0;
    m_bExplicitOutput = m_bExplicitInput = m_bForceOutput = FALSE;
}

//-----------------------------------------------------------------------------
// Name: ~CImageConv()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CImageConv::~CImageConv()
{
	// Clean up the linked list of image files
    ImageList::iterator i;
	for(i = m_ImageFiles.begin(); i != m_ImageFiles.end(); i++)
	{
		delete *i;
	}

	m_ImageFiles.erase(m_ImageFiles.begin(), m_ImageFiles.end());

}

//-----------------------------------------------------------------------------
// Name: PrintUsage()
// Desc: Prints out the usage string for help
//-----------------------------------------------------------------------------
void CImageConv::PrintUsage()
{
    printf( "USAGE: imageConv [-i <input image directory (required)>]\n" );
    printf( "                 [-o <output bundled image directory (required)>]\n" );
	printf( "                 [-f] force overwrite of destination files\n" );
    printf( "                 [-?]\n");
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Parses and validates the command line arguments
//-----------------------------------------------------------------------------
HRESULT CImageConv::Initialize ( int argc, char * argv[] )
{
    for(int n = 1; n < argc; n++)
    {
        // step through each command line parameter
        if((argv[n][0] == '/') || (argv[n][0] == '-'))
        {				
			if(((argv[n][1] == 'i') || (argv[n][1] == 'o')) && ((n+1) == argc))
				return E_INVALIDARG;

            // found an option
            if(lstrcmpi("i", &(argv[n][1])) == 0)
            {
                // Input Directory
                lstrcpyA( m_strInputDir, argv[n+1] );
                m_bExplicitInput = TRUE;
				n++;    // skip two args
            }
            
			else if(lstrcmpi("o", &(argv[n][1])) == 0)
            {
                // Output Directory
                lstrcpyA( m_strOutputDir, argv[n+1] );
                m_bExplicitOutput = TRUE;
				n++;    // skip two args
            }

			else if(lstrcmpi("f", &(argv[n][1])) == 0)
			{
				m_bForceOutput = TRUE;
			}

            else if(lstrcmpi("?", &(argv[n][1])) == 0)
                return E_INVALIDARG;    // causes usage to be displayed
            else
                return E_INVALIDARG;
        }
	}

	if(!m_bExplicitInput || !m_bExplicitOutput)
		return E_INVALIDARG;

	if(ValidateDir( m_strInputDir ) == E_FAIL)
	{
		printf( "ERROR: Input image directory does not exist.\n" );
		return E_FAIL;
	}
	if(ValidateDir( m_strOutputDir ) == E_FAIL)
	{
		if(!CreateDirectory( m_strOutputDir, NULL))
		{
			printf( "ERROR: Unable to create output directory.\n" );
			return E_FAIL;
		}
		printf( "WARNING: Output image directory does not exist, creating...\n" );
	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ValidateDir()
// Desc: Append a trailing slash if necessary, and ensure the 
//       directory exists
//-----------------------------------------------------------------------------
HRESULT CImageConv::ValidateDir ( char* dir )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	// add a trailing back slash if not present
	if( dir[strlen(dir)] != '\\')
		dir = strcat(dir, "\\");

	// check to see if directory is present
	char searchDir[MAX_PATH];
	strcpy(searchDir, dir);
	strcat(searchDir, "*");
	hFind = FindFirstFile(searchDir, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return E_FAIL;
	} 
	else 
	{
		FindClose(hFind);
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FindImages()
// Desc: Traverses the image files from the input directory
//	     and stores them in a linked list
//-----------------------------------------------------------------------------
HRESULT CImageConv::FindImages ( char* extension )
{
	HRESULT			hr;
	WIN32_FIND_DATA FindFileData;  
	HANDLE			hFoundFile;
	char			searchDir[MAX_PATH];
	BOOL			fileResult;

	strcpy(searchDir, m_strInputDir);
	strcat(searchDir, extension);
	hFoundFile = FindFirstFile(searchDir, &FindFileData);

	fileResult = hFoundFile != INVALID_HANDLE_VALUE;
	while (fileResult)
	{
		// Add the first image file to the file list
		CImageItem *newImage = new CImageItem();

		char fullDir[MAX_PATH];
		strcpy(fullDir, m_strInputDir);
		strcat(fullDir, FindFileData.cFileName);
		strcpy(newImage->fullpath, fullDir);
		strcpy(newImage->filename, FindFileData.cFileName);

		// Find out the width, height, and depth of the image
		hr = FillImageInfo(newImage);

		// Is the image valid?
		if(hr == S_OK)
		{
			hr = CheckImageDest(newImage);
			
			// Does the destination image already exist?
			if(hr == S_OK || m_bForceOutput)
			{			
				// Insert the image data for the file into the list
				m_ImageFiles.insert( m_ImageFiles.end(), newImage );
				m_nImageFiles++;
			}
		}

		fileResult = FindNextFile(hFoundFile, &FindFileData);
	}

	FindClose(hFoundFile);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CheckImageDest()
// Desc: Checks to see if the destination image already exists
//-----------------------------------------------------------------------------
HRESULT CImageConv::CheckImageDest ( CImageItem *imageFile )
{
	WIN32_FIND_DATA FindFileData;  
	HANDLE			hFoundFile;
	char			destDir[MAX_PATH];
	char			drive[MAX_PATH];
	char			dir[MAX_PATH];
	char			fname[MAX_PATH];
	char			ext[MAX_PATH];

	_splitpath (imageFile->fullpath, drive, dir, fname, ext);

	// Generate output location, and store the value in the CImageItem
	sprintf(destDir, "%s%s.xbx", m_strOutputDir, fname);
	strcpy(imageFile->outputpath, destDir);

	// Does the destination file already exist?
	hFoundFile = FindFirstFile(imageFile->outputpath, &FindFileData);
	if (hFoundFile == INVALID_HANDLE_VALUE)
	{
		// Destination image file not found
		return S_OK;
	}
	
	return E_FAIL;
}

//-----------------------------------------------------------------------------
// Name: FillImageInfo()
// Desc: Takes in an imageFile data structure, reads out the image file,
//	     and populates the remaining fields of the data structure with
//		 the correct image information (width, height, depth).
//
// Current Support: BMP, TGA
//-----------------------------------------------------------------------------
HRESULT CImageConv::FillImageInfo ( CImageItem *imageFile )
{
	char drive[MAX_PATH];
	char dir[MAX_PATH];
	char fname[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath (imageFile->fullpath, drive, dir, fname, ext);

	FILE * fImage;								// Image file pointer
	fImage = fopen(imageFile->fullpath, "rb");	// Open image For reading

	if(fImage == NULL)					
	{
		fclose(fImage);
		return E_FAIL;	
	}

	if(strcmp(ext, ".tga") == 0)
	{
		// dealing with a Targa file
		TGAHeader tgaInfo;

		if(fread(&tgaInfo, sizeof(TGAHeader), 1, fImage) == 0)
		{
			fclose(fImage);
			return E_FAIL;
		}	

		imageFile->width  = tgaInfo.width;				// Set Height
		imageFile->height = tgaInfo.height;				// Set The Width
		imageFile->depth  = tgaInfo.pixel_depth;		// Set Bits Per Pixel
	}
	else if(strcmp(ext, ".bmp") == 0)
	{
		// dealing with a Bitmap file
		BMPHeader bmpInfo;

		if(fread(&bmpInfo, sizeof(BMPHeader), 1, fImage) == 0)
		{
			fclose(fImage);
			return E_FAIL;
		}

		imageFile->width  = bmpInfo.Width;				// Set Height
		imageFile->height = bmpInfo.Height;				// Set The Width
		imageFile->depth  = bmpInfo.BitCount;			// Set Bits Per Pixel
	}

	// Validate image information
	if((imageFile->width <= 0) || (imageFile->height <= 0) || (imageFile->depth <= 0))
	{
		printf( "ERROR: Image file format is invalid" );
		fclose(fImage);
		return E_FAIL;
	}

	fclose(fImage);

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program
//-----------------------------------------------------------------------------
int _cdecl main( int argc, char * argv[] )
{
    HRESULT hr;
    CImageConv imgConverter;

    hr = imgConverter.Initialize( argc, argv );
    if(hr == E_INVALIDARG)
    {
        imgConverter.PrintUsage();
        return 0;
    }
    else if(FAILED(hr))
	{
        return 0;
	}

	imgConverter.FindImages( "*.tga" );
	imgConverter.FindImages( "*.bmp" );


	// Bundle Images
	CBundler bundler;
	hr = bundler.Initialize( argc, argv );
    if(FAILED(hr))
    {
        bundler.ErrorMsg( "ERROR: D3D Initialization failed.\n" );
        return 0;
    }

	ImageList::iterator i;
	for(i = imgConverter.m_ImageFiles.begin(); i != imgConverter.m_ImageFiles.end(); i++)
	{
		bundler.HandleTexture( *i );
	}

    return 0;
}