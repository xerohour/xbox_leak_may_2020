
//
// keywords under the MAIN section
//

#define MAIN_SECTION_NAME       "MAIN"
#define GRAPH_KEY_NAME          "GRAPH"
#define FX_KEY_NAME             "FX"
#define IMAGE_FRIENDLY_NAME     "IMAGE_FRIENDLY_NAME"
#define NUM_TEMPBINS_NAME       "FX_NUMTEMPBINS"


#define MAX_TEMPBINS	256


//
// keywords in the state INI files under the FXSTATE section
//

#define FXSTATE_EFFECT_NAME         "FX_NAME"
#define FXSTATE_SECTION_NAME		"FXSTATE"

#define FXPARAM_DSPCODE_NAME	    "FX_DSPCODE"
#define FXPARAM_DSPSTATE_NAME	    "FX_DSPSTATE"
#define FXPARAM_NUMINPUTS_NAME		"FX_NUMINPUTS"
#define FXPARAM_NUMOUTPUTS_NAME     "FX_NUMOUTPUTS"

//
// definitions for Y memory. Found in *state.INI for each FX
//

#define FXPARAM_YMEMORY_SIZE        "FX_YMEMSIZE"

//
// definitions for X memory state. Found in *state.INI for each FX
//

#define FXPARAM_NUMINPUTS_NAME		        "FX_NUMINPUTS"
#define FXPARAM_NUMOUTPUTS_NAME		        "FX_NUMOUTPUTS"
#define FXPARAM_NUMPARAMS_NAME	        	"FX_NUMPARAMS"
#define FXPARAM_NUMPUBLICPARAMS_NAME	   	"FX_NUMPUBLICPARAMS"

#define FXPARAM_INPUT_NAME	     	"FX_INPUT"
#define FXPARAM_OUTPUT_NAME	     	"FX_OUTPUT"
#define FXPARAM_STATEPARAM_NAME   	"FX_PARAM"

#define FXPARAM_DSPCYCLES_NAME		"FX_DSPCYCLES"

#define FXPARAM_SCRATCH_LENGTH		"FX_SCRATCHLENGTH"

//
// entries that describe each parameter
//

#define FXPARAM_DESC_NAME          "FX_PARAM%d_NAME"
#define FXPARAM_DESC_DESCRIPTION   "FX_PARAM%d_DESCRIPTION"

#define FXPARAM_DESC_TYPE          "FX_PARAM%d_TYPE"
#define FXPARAM_DESC_UNITS         "FX_PARAM%d_UNITS"
#define FXPARAM_DESC_MIN           "FX_PARAM%d_MIN"
#define FXPARAM_DESC_MAX           "FX_PARAM%d_MAX"
#define FXPARAM_DESC_ATOMIC        "FX_PARAM%d_ATOMIC"

//
// the following params are defined in the dsp image INI file
//

#define FXFLAG_MIXOUTPUT           "FX_MIXOUTPUT"

// ****************************************************************************
// define the key words for the inputs of the first FX in a FX graph
// First define the VP mix bins
//

#define FX_BUFFERID_MASK			0xFFF00000			
#define FX_BUFFERID_BASE			0x00100000

#define NUM_VPMIXBINS 32
#define NUM_GPMIXBINS 32

#define NUM_FXSENDBINS 20

#define FXINPUT_VPMIXBIN_BASE		(FX_BUFFERID_BASE << 1)

//
// internal to the DSP final output GP mix bins
//

#define FXINPUT_GPMIXBIN_BASE		(FX_BUFFERID_BASE << 2)

//
// internal to the DSP fx routing temporary bins
//

#define FXINPUT_GPTEMPBIN_BASE		(FX_BUFFERID_BASE << 3)
#define FXINPUT_GPTEMPBIN_NAME      "GPTEMPBIN"


// ****************************************************************************
// definitions for output key words
// the namespace is the same with the FXINPUT_XXX definitions above, thus
// they need unique bit values
//


//
// GP internal mix bins as destination
//

#define FXOUTPUT_GPMIXBIN_BASE		(FX_BUFFERID_BASE << 4)

//
// GP temp bins
//

#define FXOUTPUT_GPTEMPBIN_BASE	    (FX_BUFFERID_BASE << 5)
#define FXOUTPUT_GPTEMPBIN_NAME		"GPTEMPBIN"

//
// friendly names for the VP and GPMIXBINS
//
/*
char* vpMixBinNames[32] ={

	"VPMIXBIN_FRONTLEFT",
	"VPMIXBIN_FRONTRIGHT",
	"VPMIXBIN_CENTER",
	"VPMIXBIN_LFE",
	"VPMIXBIN_BACKLEFT",
	"VPMIXBIN_BACKRIGHT",
	"VPMIXBIN_I3DL2SEND",
	"VPMIXBIN_XTLK_FL",
	"VPMIXBIN_XTLK_FR",
	"VPMIXBIN_XTLK_BL",
	"VPMIXBIN_XTLK_BR",
	"VPMIXBIN_FXSEND0",
	"VPMIXBIN_FXSEND1",
	"VPMIXBIN_FXSEND2",
	"VPMIXBIN_FXSEND3",
	"VPMIXBIN_FXSEND4",
	"VPMIXBIN_FXSEND5",
	"VPMIXBIN_FXSEND6",
	"VPMIXBIN_FXSEND7",
	"VPMIXBIN_FXSEND8",
	"VPMIXBIN_FXSEND9",
	"VPMIXBIN_FXSEND10",
	"VPMIXBIN_FXSEND11",
	"VPMIXBIN_FXSEND12",
	"VPMIXBIN_FXSEND13",
	"VPMIXBIN_FXSEND14",
	"VPMIXBIN_FXSEND15",
	"VPMIXBIN_FXSEND16",
	"VPMIXBIN_FXSEND17",
	"VPMIXBIN_FXSEND18",
	"VPMIXBIN_FXSEND19",
	"VPMIXBIN_RESERVED"
};


char* gpMixBinNames[32] ={

	"GPMIXBIN_FRONTLEFT",
	"GPMIXBIN_FRONTRIGHT",
	"GPMIXBIN_CENTER",
	"GPMIXBIN_LFE",
	"GPMIXBIN_BACKLEFT",
	"GPMIXBIN_BACKRIGHT",
	"GPMIXBIN_RESERVED",
	"GPMIXBIN_RESERVED",
	"GPMIXBIN_RESERVED",
	"GPMIXBIN_RESERVED",
	"GPMIXBIN_RESERVED",
	"GPMIXBIN_FXSEND0",
	"GPMIXBIN_FXSEND1",
	"GPMIXBIN_FXSEND2",
	"GPMIXBIN_FXSEND3",
	"GPMIXBIN_FXSEND4",
	"GPMIXBIN_FXSEND5",
	"GPMIXBIN_FXSEND6",
	"GPMIXBIN_FXSEND7",
	"GPMIXBIN_FXSEND8",
	"GPMIXBIN_FXSEND9",
	"GPMIXBIN_FXSEND10",
	"GPMIXBIN_FXSEND11",
	"GPMIXBIN_FXSEND12",
	"GPMIXBIN_FXSEND13",
	"GPMIXBIN_FXSEND14",
	"GPMIXBIN_FXSEND15",
	"GPMIXBIN_FXSEND16",
	"GPMIXBIN_FXSEND17",
	"GPMIXBIN_FXSEND18",
	"GPMIXBIN_FXSEND19",
	"GPMIXBIN_RESERVED"
};

*/










