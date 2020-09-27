// **************************************************************************
//
// CldToBin:		convert Motorola .cld output file to a binary file  
//       
//		Gints Klimanis, 2000
//
// **************************************************************************        

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <varargs.h>
#include <sys/types.h>

/* Headers for working with COFF files */
#ifndef BIG_ENDIAN
#define BIG_ENDIAN 0
#endif
#include "coreaddr.h"
#include "maout.h"
#include "dspext.h"

union dbl {     /* double-precision floating point/integer union */
        double dval;
        struct {
#if !BIG_ENDIAN
                unsigned long lval, hval;
#else
                unsigned long hval, lval;
#endif
        } l;
};

#define False	0
#define True	1

struct comment {	/* comment structure */
	long c_scno;
	long c_text;
	struct comment *c_next;
};

static long ReadHeaders (FILHDR *header, AOUTHDR *optionalHeader, OPTHDR *linkHeader);
static void ReadSections(FILHDR *fileHeader, int numSections, long sectionSeek, char *sectionComment);
static long DumpData(XCNHDR *sh, FILHDR *fileHeader, char *sectionComment);
static int freads (char *ptr, int size, long nitems,FILE *stream);

static void swapw (char *ptr, int size, long nitems);
static void ByteSwap32(void *ptr);
char *FileNamePtr   (char *filePath);
char *FileNameExtPtr(char *filePath);

long symentsize = SYMESZ;
/*  Global variables  */
FILHDR	fileHeader;		/* File header structure */
AOUTHDR	optionalHeader;	/* Optional header structure */
OPTHDR	linkHeader;		/* Linker header structure */
int	absolute;			/* Absolute file flag */

long numSections;		/* Number of sections */
//long sectionSeek;		/* Used to seek to first section */

long symPtr;			/* File pointer to symbol table entries */
long numSymbols;		/* Number of symbols */

int  dataWidth;        /* width of data for printing */
int  addressWidth;     /* width of address for printing */

char *strTab;			/* Pointer to start of string char. array */
long strLength;			/* Length in bytes of string array */

FILE *inFile  = NULL;	
FILE *outFile = stdout;	

char *headerString = NULL;
char *footerString = NULL;
char *nestedIncludeString = NULL;
int useCTitleHeaderFooter = False;

int verbose = False;
int write24to32 = True;
#define kMemoryMapFlag_MemoryX	0x1
#define kMemoryMapFlag_MemoryY	0x2
#define kMemoryMapFlag_MemoryP	0x4
int memoryMapFlags = kMemoryMapFlag_MemoryP | kMemoryMapFlag_MemoryY | kMemoryMapFlag_MemoryX;
int xSegments = 0;
int ySegments = 0;
int pSegments = 0;
int binary          = True;
int comma           = False;
int oneValuePerLine = False;
int byteSwap        = True;
int onlySection     = -1;
int addSectionComments = False;

#define kMaxSections	300
char sectionIncluded[kMaxSections];
int  sectionLengths [kMaxSections];
int sectionCounter = -1;

char str[2000];

/* init is to non valid memory space */
int  space = 777; /* 0=p, 1=x, 2=y, 3=l, 4=N */

struct comment *commentHead    = NULL;	/* head of comment chain */
struct comment *currentComment = NULL;	/* current comment in chain */

// **********************************************************************************
// OnInterrupt:			clean up from signal
// ********************************************************************************** 
	static void
OnInterrupt (int signum)			
{
exit(1);
}	// ---- end OnInterrupt() ---- 

// **********************************************************************************
// PrintUsage:		
// ********************************************************************************** 
	void   
PrintUsage()
{
printf("cldtobin: convert Motorola .cld file to binary or HEX test data file\n" );
printf("Usage: [Options] <infile.cld><outfile.txt>\n");
printf("Options:\n");
printf("                                    \n");
printf("WARNING:  cldtobin processes P-memory sections,\n");
printf("          but has trouble including X and Y memory sections\n");
printf("                                    \n");
printf("    -h    help     print usage statement\n");
printf("    -v    verbose  print debug info\n");
printf("                                    \n");
printf("    -noX  exclude X   Memory sections\n");
printf("    -noY  exclude Y   Memory sections\n");
printf("    -noXY exclude X&Y Memory sections\n");
printf("    -noP  exclude P   Memory sections\n");
printf("    -excludeN <n> exclude section n of N sections Range:[0..N-1]\n");
printf("              (counted in output sections, not input)\n");
printf("Options for binary output only:\n");
printf("    -no24to32  write 32-bit word in 32-bit container\n");
printf("    -nobyteSwap  no endian byte swap, after 24->32 pad\n");
printf("                                    \n");
printf("Options for text output only:\n");
printf("    -astext  output as text instead of binary\n");
printf("    -asinc   separate each word with a comma text\n");
printf("    -nestedinclude  stringused to avoid including DSP code twice\n");
printf("    -single  one text value per line\n");
printf("    -header  prepend this string to output text\n");
printf("    -footer  append  this string to output text\n");
printf("    -titleCheadfoot  C array header & footer using file name\n");
printf("    -addSectionComments  section #, word count/section, 1st word\n");
printf("\n");
printf("Written by Gints Klimanis, nVidia, Inc. 2000\n");
}	// ---- end PrintUsage() ---- 

// ************************************************************
// main:		
// ************************************************************ 
	void
main(int argc, char **argv)
{
int i;
char *s;
int argIndex = 1;
int fileIndex = 0;
char *fileNames[2];

fileNames[0] = NULL;
fileNames[1] = NULL;
for (i = 0; i < kMaxSections; i++)
	{
	sectionIncluded[i] = True;
	sectionLengths [i] = 0;
	}

if ( argc < 3 )
    {
	PrintUsage();
	exit (-1 );
    }

for (i = 1; i < argc; i++)
	{
	s = argv[i]; //printf("arg[%d]='%s'\n", i, s);

	if (s[0] == '-')
		{
		if (!stricmp(s, "-v"))
			verbose = True;
		else if (!stricmp(s, "-noX"))
			memoryMapFlags &= ~kMemoryMapFlag_MemoryX;
		else if (!stricmp(s, "-noY"))
			memoryMapFlags &= ~kMemoryMapFlag_MemoryY;
		else if (!stricmp(s, "-noXY"))
			{
			memoryMapFlags &= ~kMemoryMapFlag_MemoryX;
			memoryMapFlags &= ~kMemoryMapFlag_MemoryY;
			}
		else if (!stricmp(s, "-noP"))
			memoryMapFlags &= ~kMemoryMapFlag_MemoryP;
		else if (!stricmp(s, "-24to32"))
			write24to32 = True;
		else if (!stricmp(s, "-no24to32"))
			write24to32 = False;

		else if (!stricmp(s, "-header"))
			{
			if (++i >= argc)
				{
				fprintf(stderr, "Well, supply %s \n", argv[i-1]);
				exit(-1);
				}
			headerString = argv[i];
			}
		else if (!stricmp(s, "-nestedinclude"))
			{
			if (++i >= argc)
				{
				fprintf(stderr, "Well, supply %s \n", argv[i-1]);
				exit(-1);
				}
			nestedIncludeString = argv[i];
			}
		else if (!stricmp(s, "-footer"))
			{
			if (++i >= argc)
				{
				fprintf(stderr, "Well, supply %s \n", argv[i-1]);
				exit(-1);
				}
			footerString = argv[i];
			}
		else if (!stricmp(s, "-titleCheadfoot"))
			useCTitleHeaderFooter = True;
		else if (!stricmp(s, "-addSectionComments"))
			addSectionComments = True;
		
		else if (!stricmp(s, "-byteSwap"))
			byteSwap = True;
		else if (!stricmp(s, "-nobyteSwap"))
			byteSwap = False;
		else if (!stricmp(s, "-asText"))
			binary = False;
        else if (!stricmp(s, "-asInc"))
        { binary = False; comma = True; }
		else if (!stricmp(s, "-single"))
			oneValuePerLine = True;
		else if (!stricmp(s, "-excludeN"))
			{
			int x;
			if (++i >= argc)
				{
				fprintf(stderr, "Well, supply %s \n", argv[i-1]);
				exit(-1);
				}
			x = atoi(argv[i]);
			if (x < 0 || x > 1000)
				{
				fprintf(stderr, "Well, arg[%d]=%s out of range [0..%d]\n", i, argv[i], 0, kMaxSections-1);
				exit(-1);
				}
			sectionIncluded[x] = False;
			}
		else if (!stricmp(s, "-onlySection"))
			{
			int x;
			if (++i >= argc)
				{
				fprintf(stderr, "Well, supply %s \n", argv[i-1]);
				exit(-1);
				}
			x = atoi(argv[i]);
			if (x < 0 || x > 1000)
				{
				fprintf(stderr, "Well, arg[%d]=%s out of range [0..%d]\n", i, argv[i], 0, kMaxSections-1);
				exit(-1);
				}
			onlySection = x;
			}
		else
			{
			printf("Bogus option '%s'\n", s);
			PrintUsage();
			exit (-1 );
			}
		}
	else
		fileNames[fileIndex++] = argv[i];
	}

if (!fileNames[0])
	{
	printf("Provide input file name\n");
	PrintUsage();
	exit (-1 );
	}
if (!fileNames[1])
	{
	printf("Provide output file name\n");
	PrintUsage();
	exit (-1 );
	}
if (verbose)
	{
//	printf("memoryMapFlags=%X\n", memoryMapFlags);
	printf("Input  file: '%s'\n", fileNames[0]);
	printf("Output file: '%s'\n", fileNames[1]);
	}

  /* set up for signals, save program name, check for command line options */
    signal (SIGINT, OnInterrupt);

if ( ( inFile = fopen( fileNames[0], "rb" ) ) == NULL )
	{
	printf( "cannot open input file %s", fileNames[0] );
	exit(0);
	}
if (fileNames[1])
	{
	if (binary)
		outFile = fopen( fileNames[1], "wb" );
	else
		outFile = fopen( fileNames[1], "w" );
	 if ( outFile == NULL )
		{
		printf( "cannot open output file %s", fileNames[1] );
		fclose( inFile );
		exit(0);
		}
	}

{
char *p;
long sectionSeek;

if (!binary)
	{
// Pinpoint file name in the full file path string
	strcpy(s, FileNamePtr(fileNames[1]));
// Exclude file extension
	p = FileNameExtPtr(s);
	p[0] = '\0';
	}

// Insert some text before the DSP code data
if (!binary)
	{
	if (useCTitleHeaderFooter)
		{
	// Insert "include once" stuff
		fprintf(outFile, "#ifndef __%s_H__\n", s);
		fprintf(outFile, "#define __%s_H__\n", s);

		}
	else if (headerString)
		fprintf(outFile, "%s", headerString);
	}
sectionSeek = ReadHeaders(&fileHeader, &optionalHeader, &linkHeader);
if (-1 == sectionSeek)
	{
	printf("ReadHeaders: Bad header, Exiting ...\n");
	exit(-1);
	}

ReadSections(&fileHeader, numSections, sectionSeek, s);

if (!binary)
	{
	if (useCTitleHeaderFooter)
		fprintf(outFile, "%s", "};");
	else if (footerString)
		fprintf(outFile, "%s", footerString);

	if (nestedIncludeString)	
		fprintf(outFile, "\n#endif /* %s */\n", nestedIncludeString);

// End of "include once" stuff
	if (useCTitleHeaderFooter)
		fprintf(outFile, "\n#endif // __%s_H__\n", s);
	}
}

fclose( inFile );
if (outFile)
	{
	if (binary)
		{
		int i;
#define ZERO_PADDING	2
		for (i = 0; i < ZERO_PADDING; i++) 
			str[i] = 0;
		fwrite((void *) str, sizeof(unsigned char), ZERO_PADDING, outFile);
		}
	fclose( outFile );
	}
exit (0);
}	// ---- end main() ---- 

// ************************************************************
// ReadHeaders:		return sectionSeek, -1 on error
// ************************************************************ 
	static long
ReadHeaders(FILHDR *header, AOUTHDR *optionalHeader, OPTHDR *linkHeader)
{
if ( freads( (char *) header, sizeof(FILHDR), 1, inFile ) != 1 )
	printf ("ReadHeaders(): Cannot read file header");

/* Save global values */
    numSections = header->f_nscns;
    numSymbols  = header->f_nsyms;
    symPtr      = header->f_symptr;
    absolute    = !!(header->f_flags & F_RELFLG);

/* Check MAGIC number */
    if ( header->f_magic == M56KMAGIC )
		{
		dataWidth    = 6;
		addressWidth = 4;
		}
    else if ( header->f_magic == M96KMAGIC )
		dataWidth = addressWidth = 8;
    else if ( header->f_magic == M16KMAGIC )
		dataWidth = addressWidth = 4;
    else if ( header->f_magic == M563MAGIC )
		dataWidth = addressWidth = 6;
    else if ( header->f_magic == M566MAGIC )
		{
		dataWidth    = 6;
        addressWidth = 4;
		}
    else if ( header->f_magic == M568MAGIC )
 		dataWidth = addressWidth = 4;
    else if ( header->f_magic == SC1MAGIC )
		{
		dataWidth    = 4;
		addressWidth = 8;
		}
    else if ( header->f_magic == M567MAGIC )
		{
		dataWidth    = 4;
		addressWidth = 8;
		}
    else
		{
		printf("Header has a bad magic number\n" );
		return (-1);
		}
    /* optional header present */
    if ( header->f_opthdr )
		{
		if ( absolute )
			{
			if ( freads( (char *) optionalHeader,
				(int)header->f_opthdr, 1, inFile ) != 1 )
			printf( "cannot read optional file header" );
			}
		else
			{
			if ( freads( (char *) linkHeader,
				(int)header->f_opthdr, 1, inFile ) != 1 )
			printf( "cannot read linker file header" );
			}
		}

/* sectionSeek: File offset for first section headers */
return (sizeof(FILHDR) + header->f_opthdr);
}	// ---- end ReadHeaders() ---- 

// ************************************************************
// ReadSections:		
// ************************************************************ 
	static void
ReadSections(FILHDR *fileHeader, int numSections, long sectionSeek, char *sectionComment)
{
int i;
XCNHDR sh;		/* Section header structure */

for (i = 0; i < numSections; i++)
    {
	if (fseek (inFile, sectionSeek, 0) != 0)
	    printf ("cannot seek to section headers");
	if (freads ((char *)&sh, sizeof(XCNHDR), 1, inFile) != 1)
	    printf ("cannot read section headers");
#if !BIG_ENDIAN
	if (sh._n._s_n._s_zeroes)
	    swapw (sh._n._s_name, sizeof(long), 2);
#endif
	sectionSeek += sizeof(XCNHDR);

	DumpData (&sh, fileHeader, sectionComment);
    }


if (verbose)
	{
	// Add up section lengths
	long totalLength   = 0;
	long totalSections = 0;
printf("\n");
	for (i = 0; i <= sectionCounter; i++)
		{
if (verbose)
	printf("Section %2d: included=%d length=%d\n", i, sectionIncluded[i], sectionLengths[i]);
		if (sectionIncluded[i])
			{
			totalSections++;
			totalLength += sectionLengths[i];
			}
		}
	printf("\nTotals in output: %d words (%d bytes) in %d Sections\n", totalLength, totalLength/3, totalSections);
	}
}	// ---- end ReadSections() ----

// ************************************************************
// GetSectionName:		
// ************************************************************ 
	char *
GetSectionName (XCNHDR *sh)
{
char *secname;

if ( sh->_n._s_n._s_zeroes )
	secname = sh->_n._s_name;
else
    {
	if (sh->_n._s_n._s_offset < sizeof(strLength) ||
	    sh->_n._s_n._s_offset > strLength)
	    printf ("invalid string table offset for section header name");
	secname = &strTab[sh->_n._s_n._s_offset - sizeof(strLength)];
    }

return (secname);
}	// ---- end GetSectionName() ----

// ************************************************************
// DumpData:			Return # words in this section
// ************************************************************ 
	static long
DumpData (XCNHDR *sh, FILHDR *fileHeader, char *sectionComment)
{
    char *secname, *GetSectionName ();
    long *raw_data;
    int  j;
    char emi_name_buf[8];
	int memtype, address ;
	char *mem_field;
int isXmemory = False;
int isYmemory = False;
int isPmemory = False;

sectionCounter++;
sectionLengths [sectionCounter] = sh->_s.s_size;

if (verbose)
	printf("Section %2d:  valid=%d s_scnptr=%5d s_size=%5d s_paddr=%5d\n", 
		   sectionCounter, ( sh->_s.s_scnptr && sh->_s.s_size ), 
		   sh->_s.s_scnptr, sh->_s.s_size, 
			sh->_s.s_paddr);

if (!( sh->_s.s_scnptr && sh->_s.s_size ))
	{
	sectionIncluded[sectionCounter] = False;
	return (0);
	}

memtype = CORE_ADDR_MAP(  sh->_s.s_paddr );
address = CORE_ADDR_ADDR( sh->_s.s_paddr );
secname = GetSectionName( sh );

/* Determine memory field (optional counter ok) */
switch ( memtype )
	{
	case memory_map_p:
	    mem_field = "P";
	    break;
	    
	case memory_map_pa:
	    mem_field = "PA";
	    break;
	    
	case memory_map_pb:
	    mem_field = "PB";
	    break;
	    
	case memory_map_pe:
	    mem_field = "PE";
	    break;
	    
	case memory_map_pi:
	    mem_field = "PI";
	    break;
	    
	case memory_map_pr:
	    mem_field = "PR";
	    break;
	    
	case memory_map_y:
	    mem_field = "Y";
	    break;
	    
	case memory_map_ya:
	    mem_field = "YA";
	    break;
	    
	case memory_map_yb:
	    mem_field = "YB";
	    break;
	    
	case memory_map_ye:
	    mem_field = "YE";
	    break;
	    
	case memory_map_yi:
	    mem_field = "YI";
	    break;
	    
	case memory_map_yr:
	    mem_field = "YR";
	    break;
	    
	case memory_map_x:
	    mem_field = "X";
	    break;
	    
	case memory_map_xa:
	    mem_field = "XA";
	    break;
	    
	case memory_map_xb:
	    mem_field = "XB";
	    break;
	    
	case memory_map_xe:
	    mem_field = "XE";
	    break;
	    
	case memory_map_xi:
	    mem_field = "XI";
	    break;
	    
	case memory_map_xr:
	    mem_field = "XR";
	    break;
	    
	case memory_map_l:
	    mem_field = "L";
	    break;
	    
	case memory_map_laa:
	    mem_field = "LAA";
	    break;
	    
	case memory_map_lab:
	    mem_field = "LAB";
	    break;
	    
	case memory_map_lba:
	    mem_field = "LBA";
	    break;
	    
	case memory_map_lbb:
	    mem_field = "LBB";
	    break;
	    
	case memory_map_le:
	    mem_field = "LE";
	    break;
	    
	case memory_map_li:
	    mem_field = "LI";
	    break;
	    
	case memory_map_emi:
	    mem_field = "EM";
	    break;
	    
	case memory_map_dm:
	    mem_field = "DM";
	    break;
	    
	default:

	    if ((fileHeader->f_magic == M566MAGIC) &&
		  	  (memtype == memory_map_e1      ) && 
			  (sh->_s.s_flags & STYP_OVERLAY ) &&
			(!(sh->_s.s_flags & STYP_OVERLAYP))  )
	    {
		/* substitute when x or y overlay memory destination */
		memtype = memory_map_e0;
	    }
	    
	    if (memtype >= memory_map_e0 && memtype <= memory_map_e255)
			{
			sprintf(emi_name_buf,"E%d",memtype-memory_map_e0);
			mem_field = emi_name_buf;
			}
	    else
			mem_field = "<error>";
	break;
	}
	
sectionLengths[sectionCounter] = sh->_s.s_size;

if (!binary && useCTitleHeaderFooter)
	{
	fprintf(outFile, "\n#define k%s_Length %d\n", sectionComment, sectionLengths[sectionCounter]);

	if (nestedIncludeString)	
		fprintf(outFile, "\n#ifdef %s\n", nestedIncludeString);

	fprintf(outFile, "Int24 %sLength = k%s_Length;\n", sectionComment, sectionComment);
// Insert array preample :   Int24 fileName[] = {
	fprintf(outFile, "Int24 %s[k%s_Length] = {\n", sectionComment, sectionComment);
	}

//	if (verbose)
//		printf("Section %2d IsX=%d IsY=%d IsP=%d included=%d length=%d\n", 
//		sectionCounter, 
//		IsMemoryX(memtype), IsMemoryY(memtype), IsMemoryP(memtype), 
//		sectionIncluded[sectionCounter], sectionLengths[sectionCounter]);

if		(IsMemoryX(memtype) && !(memoryMapFlags & kMemoryMapFlag_MemoryX))
	sectionIncluded[sectionCounter] = False;
else if (IsMemoryY(memtype) && !(memoryMapFlags & kMemoryMapFlag_MemoryY))
	sectionIncluded[sectionCounter] = False;
else if (IsMemoryP(memtype) && !(memoryMapFlags & kMemoryMapFlag_MemoryP))
	sectionIncluded[sectionCounter] = False;

if (!sectionIncluded[sectionCounter])
	{
	if (verbose)
		printf("Section %2d Memory %s not included\n", sectionCounter, mem_field);

//	printf("sectionIncluded=%d IsMemoryX=%d IsMemoryY=%d IsMemoryP=%d\n",
//			sectionIncluded[sectionCounter-1], IsMemoryX(memtype), IsMemoryY(memtype), IsMemoryP(memtype));

//		if (!binary)
//			fprintf (outFile, "_DATA %s %01.*X\n", mem_field, addressWidth, address );
//	printf("_DATA %s %01.*X\n", mem_field, addressWidth, address );
	return (0);
	}

raw_data = (long *)malloc((size_t)(sh->_s.s_size * sizeof(long)));
if (!raw_data)
	{
	printf ("cannot allocate raw data for section %s", secname);
	return (0);
	}

if (fseek (inFile, sh->_s.s_scnptr, 0) != 0)
	{
	printf ("cannot seek to raw data in section %s", secname);
	return (0);
	}

if (freads ((char *)raw_data, (int)sh->_s.s_size,sizeof(long), inFile) != sizeof(long))
	{
	printf ("cannot read raw data in section %s",  secname);
	return (0);
	}


/* check for block data */
if ( sh->_s.s_flags & STYP_BLOCK )
	{
	if (!binary)
		{
	    if ( mem_field[0] == 'L' )
			{
			fprintf( outFile, "_BLOCKDATA Y %01.*X %01.*X %01.*X\n",
				addressWidth, address,
				addressWidth, CORE_ADDR_ADDR( sh->_s.s_vaddr ),
				dataWidth, *raw_data++ );

			fprintf( outFile, "_BLOCKDATA X %01.*X %01.*X %01.*X\n",
				addressWidth, address,
				addressWidth, CORE_ADDR_ADDR( sh->_s.s_vaddr ),
				dataWidth, *raw_data++ );
			}
	    else
			{
			fprintf( outFile, "_BLOCKDATA %s %01.*X %01.*X %01.*X\n",
				mem_field,
				addressWidth, address,
				addressWidth, CORE_ADDR_ADDR( sh->_s.s_vaddr ),
				dataWidth, *raw_data++ );
			}
		}
	}
	
	else
	{

	if (addSectionComments || verbose)
		{
		sprintf(str, "/* Section %2d Including Memory %s $%06X (1st word= $%06X ) length= %5d */\n", 
			sectionCounter, mem_field,  address, *raw_data,  sh->_s.s_size);

		if (!binary && addSectionComments) // && !comma)
			fprintf(outFile, "%s", str );

		if (verbose)
			printf("%s", str);
		}

		j = 0;
	    while (j < sh->_s.s_size)
	    {
		if ( mem_field[0] == 'L' )
			{
            int last = (j == sh->_s.s_size-1); 

			if (binary)
				{
				static unsigned char vv[4];
				long *p;
				vv[0] = 0;

				p     = raw_data+1;
				vv[1] = ((*p) & 0x00FF0000)>>16;
				vv[2] = ((*p) & 0x0000FF00)>>8 ;
				vv[3] = ((*p) & 0x000000FF)    ;
				if (write24to32)
					{
					if (byteSwap)
						ByteSwap32(vv);
					fwrite((void *)     vv, sizeof(unsigned char), 4, outFile);
					}
				else
					fwrite((void *) (vv+1), sizeof(unsigned char), 3, outFile);

				p     = raw_data;
				vv[1] = ((*p) & 0x00FF0000)>>16;
				vv[2] = ((*p) & 0x0000FF00)>>8 ;
				vv[3] = ((*p) & 0x000000FF)    ;
				if (write24to32)
					{
					if (byteSwap)
						ByteSwap32(vv);
					fwrite((void *)     vv, sizeof(unsigned char), 4, outFile);
					}
				else
					fwrite((void *) (vv+1), sizeof(unsigned char), 3, outFile);
				}
			else
				fprintf (outFile, "%s%01.*lX%s %s%01.*lX%s ", (comma?"0x":""), dataWidth, *(raw_data+1), (comma?",":""), comma?"0x":"", dataWidth, *raw_data, ((comma&&(!last))?",":""));
			raw_data += 2;
		    j        += 2;
			}
		else
			{
            int last = (j == sh->_s.s_size);

			if (binary)
				{
				static unsigned char vv[4];
				long *p;
				vv[0] = 0;

				p     = raw_data;
				vv[1] = ((*p) & 0x00FF0000)>>16;
				vv[2] = ((*p) & 0x0000FF00)>>8 ;
				vv[3] = ((*p) & 0x000000FF)    ;
				if (write24to32)
					{
					if (byteSwap)
						ByteSwap32(vv);
					fwrite((void *)     vv, sizeof(unsigned char), 4, outFile);
					}
				else
					fwrite((void *) (vv+1), sizeof(unsigned char), 3, outFile);
				}
			else
                fprintf (outFile, "%s%01.*lX%s ", (comma?"0x":""), dataWidth, *raw_data, ((comma&&(!last))?",":""));
			raw_data++;
		    j++;
			}
		
		if (!binary)
			{
			if (oneValuePerLine)
				fprintf (outFile, "\n");
			else if ( j % 4 == 0 && j < sh->_s.s_size )
				fprintf (outFile, "\n");
			}

	    }
		if (!binary)
			fprintf (outFile, "\n");
	}

return (sectionLengths[sectionCounter]);
}	// ---- end DumpData() ---- 


static char *map_chars[]={
"P",
"X","Y","L","N",
"LAA","LAB","LBA","LBB","LE",
"LI","PA","PB","PE","PI",
"PR","XA","XB","XE","XI",
"XR","YA","YB","YE","YI",
"YR","PT","PF","EM",
"E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9 ",
"E10", "E11", "E12", "E13", "E14", "E15", "E16", "E17", "E18", "E19", 
"E20", "E21", "E22", "E23", "E24", "E25", "E26", "E27", "E28", "E29", 
"E30", "E31", "E32", "E33", "E34", "E35", "E36", "E37", "E38", "E39", 
"E40", "E41", "E42", "E43", "E44", "E45", "E46", "E47", "E48", "E49", 
"E50", "E51", "E52", "E53", "E54", "E55", "E56", "E57", "E58", "E59", 
"E60", "E61", "E62", "E63", "E64", "E65", "E66", "E67", 
"E68", "E69", "E70", "E71", "E72", "E73", "E74", "E75", 
"E76", "E77", "E78", "E79", "E80", "E81", "E82", "E83", 
"E84", "E85", "E86", "E87", "E88", "E89", "E90", "E91", 
"E92", "E93", "E94", "E95", "E96", "E97", "E98", "E99", 
"E100", "E101", "E102", "E103", "E104", "E105", "E106", "E107", 
"E108", "E109", "E110", "E111", "E112", "E113", "E114", "E115", 
"E116", "E117", "E118", "E119", "E120", "E121", "E122", "E123", 
"E124", "E125", "E126", "E127", "E128", "E129", "E130", "E131", 
"E132", "E133", "E134", "E135", "E136", "E137", "E138", "E139", 
"E140", "E141", "E142", "E143", "E144", "E145", "E146", "E147", 
"E148", "E149", "E150", "E151", "E152", "E153", "E154", "E155", 
"E156", "E157", "E158", "E159", "E160", "E161", "E162", "E163", 
"E164", "E165", "E166", "E167", "E168", "E169", "E170", "E171", 
"E172", "E173", "E174", "E175", "E176", "E177", "E178", "E179", 
"E180", "E181", "E182", "E183", "E184", "E185", "E186", "E187", 
"E188", "E189", "E190", "E191", "E192", "E193", "E194", "E195", 
"E196", "E197", "E198", "E199", "E200", "E201", "E202", "E203", 
"E204", "E205", "E206", "E207", "E208", "E209", "E210", "E211", 
"E212", "E213", "E214", "E215", "E216", "E217", "E218", "E219", 
"E220", "E221", "E222", "E223", "E224", "E225", "E226", "E227", 
"E228", "E229", "E230", "E231", "E232", "E233", "E234", "E235", 
"E236", "E237", "E238", "E239", "E240", "E241", "E242", "E243", 
"E244", "E245", "E246", "E247", "E248", "E249", "E250", "E251", 
"E252", "E253", "E254", "E255"
    };

// **********************************************************************************
// freads:  swap bytes and read
// 
//	Treats ptr as reference to union array; if necessary,
//		swaps bytes to maintain base format byte ordering
//		(big endian).  Calls fread to do I/O..
// ********************************************************************************** 
static int
freads (char *ptr, int size, long nitems, FILE *stream)
//		char *ptr;		pointer to buffer
//		int size;		size of buffer
//		int nitems;		number of items to read
//		FILE *stream;		file pointer
{
int rc = fread (ptr, size, nitems, stream);
#if !BIG_ENDIAN
    swapw (ptr, size, nitems);
#endif
    return (rc);
}	// ---- end freads() ---- 


union wrd
{   /* word union for byte swapping */
    unsigned long l;
    unsigned char b[4];
};

// **********************************************************************************
// swapw:		swap bytes in words
// 
//	Treats ptr as reference to union array; if necessary,
//		swaps bytes to maintain base format byte ordering (big endian).
// ********************************************************************************** 
	static void
swapw (char *ptr, int size, long nitems)
//		ptr;		pointer to buffer
//		size;		size of buffer
//		nitems;		number of items to write
{
    union wrd *w;
    union wrd *end = (union wrd *)ptr +	((size * nitems) / sizeof(union wrd));
    unsigned i;

    for (w = (union wrd *)ptr; w < end; w++)
    {
	i       = w->b[0];
	w->b[0] = w->b[3];
	w->b[3] = i;
	i       = w->b[1];
	w->b[1] = w->b[2];
	w->b[2] = i;
    }
}	// ---- end swapw() ---- 

// **********************************************************************************
// ByteSwap32:		swap bytes in 32-bit word
// 
// ********************************************************************************** 
	static void
ByteSwap32(void *ptr)
{
unsigned char *b = (unsigned char *) ptr;
unsigned char x;

x    = b[0];
b[0] = b[3];
b[3] = x;
x    = b[1];
b[1] = b[2];
b[2] = x;
}	// ---- end ByteSwap32() ---- 

// **********************************************************************************
// FileNamePtr:		Return ptr to filename in file path string
// 
// ********************************************************************************** 
	char *
FileNamePtr(char *filePath)
{

char *fileName = filePath;
char *s = filePath;

while (*s != '\0')
	{
	if (*s == '\\' || *s == '/')
		{
		s++;
		if (*s != '\0')
			fileName = s;
		}
	s++;
	}

//printf("FileNamePtr: path='%s' -> fileName='%s'\n", filePath, fileName);
return (fileName);
}	// ---- end FileNamePtr() ---- 

// **********************************************************************************
// FileNameExtPtr:		Return ptr to file name extension in file path string
//						(this points to the '.' )
// ********************************************************************************** 
	char *
FileNameExtPtr(char *filePath)
{
char *fileName = filePath;
long length = strlen(filePath);
char *s;

s = filePath + length;

while (s != filePath)
	{
	if (*s == '.')
		break;
	s--;
	}

//printf("FileNameExtPtr: path='%s' -> fileExt='%s'\n", filePath, s);
return (s);
}	// ---- end FileNameExtPtr() ---- 
