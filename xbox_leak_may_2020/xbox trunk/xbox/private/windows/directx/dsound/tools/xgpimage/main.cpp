/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	main.cpp

Abstract:

	Entry point for xgpimage

Author:

	George Chrysanthakopoulos (georgic) 15-Mar-2001

Revision History:

	15-Mar-2001 georgic
		Initial Version
	04-Jun-2001	robheit
		Pulled code out of fxdspimg.cpp

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "fxdspimg.h"

//------------------------------------------------------------------------------
//	main
//------------------------------------------------------------------------------
int __cdecl main(int argc, char* argv[])
{
    int err;

    CDspImageBuilder Builder(APP_NAME);

    err = Builder.ParseCommandLine(argc,argv);
    if (err != ERROR_SUCCESS) {

        return err;

    }
    
    //
    // parse the actual file and assemble the dsp image
    //

    err = Builder.ParseInputFile();
    if (err != ERROR_SUCCESS) {
		Builder.DebugPrint(DBGLVL_ERROR,"Failed parsing input file");
        return err;
    }

    //
	// parsing complete.
	// at this point we should have Graphs created with an array of FX in each
	// the next step is to validate the FX chain and claculate the resource reqs
	//

	err = Builder.ValidateFxGraphs();
    if (err != ERROR_SUCCESS) {
		Builder.DebugPrint(DBGLVL_ERROR,"Validation of FX chains returned error");
        return err;
    }


    err = Builder.BuildDspImage();
    if (err != ERROR_SUCCESS) {
		Builder.DebugPrint(DBGLVL_ERROR,"Failed to created DSP image");
        return err;
    }

    err = Builder.CreateEnumHeader();
    if (err != ERROR_SUCCESS) {
		Builder.DebugPrint(DBGLVL_ERROR,"Failed to created C- Language enumeration header");
        return err;
    }

	if (!err) {
		Builder.DebugPrint(DBGLVL_ERROR, "%s run succesfully, scratch image generated.",APP_NAME);
	}

	return err;
}
