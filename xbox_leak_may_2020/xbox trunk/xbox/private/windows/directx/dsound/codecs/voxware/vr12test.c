#include <stdlib.h>

extern int RTmain(int,char**);

int main(int argc, char** argv)
{
	/************************/
	/*** Real Time Codecs ***/

	/* assign shared arguments */
	argc = 5;
	argv[2] = "-isample_m.raw";
	argv[3] = "-vsample_m.vox";

	/*** VR12 ***/
	argv[1] = "-cVR12";
	argv[4] = "-osample_m_c_VR12.raw";
	RTmain(argc,argv);


	return(0);
}