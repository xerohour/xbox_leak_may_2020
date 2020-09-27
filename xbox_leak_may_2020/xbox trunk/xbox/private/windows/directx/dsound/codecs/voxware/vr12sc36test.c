#include <stdlib.h>

#define VR12	1
#define SCX		1

extern int RTmain(int,char**);
extern int STCmain(int,char**);

int main(int argc, char** argv)
{
	/************************/
	/*** Real Time Codecs ***/

	/* assign shared arguments */
	argc = 5;
	argv[2] = "-isample_m.raw";
	argv[3] = "-vsample_m.vox";

	/*** VR12 ***/
	#if (VR12 == 1)
		argv[1] = "-cVR12";
		argv[4] = "-osample_m_c_VR12.raw";
		RTmain(argc,argv);
	#endif

	
	/**********************/
	/*** Scalable Codes ***/

	/* assign shared arguments */
	argc = 4;

	/*** SCX ***/
	#if (SCX == 1)
		argv[1] = "-iSCX";
		argv[2] = "sample_m.raw";		/* need to reassign both input and output files for scalable codecs */
		argv[3] = "sample_m_c_SCX.raw";
		STCmain(argc,argv);
	#endif

	return(0);
}