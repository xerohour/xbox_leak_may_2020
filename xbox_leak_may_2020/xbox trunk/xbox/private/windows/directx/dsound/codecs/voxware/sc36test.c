#include <stdlib.h>

extern int STCmain(int,char**);

int main(int argc, char** argv)
{
	/**********************/
	/*** Scalable Codes ***/

	/* assign shared arguments */
	argc = 4;

	/*** SCX ***/
	argv[1] = "-iSCX";
	argv[2] = "sample_m.raw";		/* need to reassign both input and output files for scalable codecs */
	argv[3] = "sample_m_c_SCX.raw";
	STCmain(argc,argv);

	return(0);
}