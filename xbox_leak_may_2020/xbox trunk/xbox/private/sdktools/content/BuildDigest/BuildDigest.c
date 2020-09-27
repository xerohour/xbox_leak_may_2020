/*
 * main.c
 */

#define MAIN_FILE
#include <windows.h>
#include <winsockx.h>
#include <xbox.h>
#include <xonlinep.h>
#include <stdio.h>

#include "cryptcab.h"
#include "dgstfile.h"

#include "BuildDigest.rc"

static bool parse_options(int argc, char **argv);
static void init_options(void);
static void cleanup(void);


void _cdecl main(int argc, char **argv)
{
	bool	result;

	init_options();

	result = parse_options(argc, argv);
	
	cleanup();

	if (result == false)
		exit(1);
	else
		exit(0);
}


static void title(void)
{
	printf(
		"\n"
        "Microsoft (R) Xbox (R) Digest File Builder - Version " 
        VER_PRODUCTVERSION_STRING ".%u\n"
		VER_LEGALCOPYRIGHT_STR ".\n"
		"\n",
		VER_PRODUCTBUILD
	);

}

static void help(void)
{
	printf(
        "Usage:\n\n"
        "BuildDigest [-r] <path> <key> <titleid> <maj_ver> <min_ver> [@list] [files]\n"
		"\n"
		"<path> is a required parameter that specifies the path of the\n"
		"        new digest. This path must specify an existing directory\n"
		"        without a file name. The name of a digest file must be\n"
		"        %s\n"
		"\n"
		"<key> is a required parameter that specifies key to use to sign the digest\n"
		"        file. This must be expressed in the hexadecimal string format.\n"
		"\n"
		"<titleid> is a required parameter that specifies the title ID whose contents\n"
		"        for which this digest is created. This parameter must be specified in\n"
		"        hexadecimal form.\n"
		"\n"
		"<maj_ver> is a required parameter that specifies the decimal major version of\n"
		"        the title whose contents for which this digest is created\n"
		"\n"
		"<min_ver> is a required parameter that specifies the decimal minor version of\n"
		"        the title whose contents for which this digest is created\n"
		"\n"
		"The list of files to be included is specified through a combination of\n"
		"filenames (which may contain wildcards), and/or list files. Filenames\n"
		"are relative to the current directory from which this application is run.\n"
		"List files may contain file names (one name per line) that are relative\n"
		"to the directory from which this program is run.\n"
		"\n"
		"By default, wildcard matches are non-recursive. Recursive matches can be\n"
		"enabled by specifying the -r option. The -r option is global and thus\n"
		"affects all wildcard matches. For example, to create a digest that\n"
		"contains all files under the current directory hierarchy, one would do:\n"
		"\n"
		"BuildDigest -r .\\ 000102030405060708090a0b0c0d0e0f fffe0000 2 0 *.*\n"
		"\n"
		"The directory hierarchy will be preserved when the digest is created\n"
		"This allows the publisher to layout the files exactly as it should appear\n"
		"on the Xbox, and then build the corresponding digest file.\n"
		"\n"
		"This application can be used to dump an existing digest file:\n"
		"\n"
		"BuildDigest -d [-v] <path> <key>\n"
		"\n"
		"The <path> parameter specifies the directory which contains the\n"
		"%s file (fixed filename). <key> specifies the key that\n"
		"is used to sign the digest. This application will not dump the\n"
		"digest file unless the correct key is furnished.\n"
		"\n"
		"An optional -v (verify) can be specified to verify each checksum\n"
		"against the actual file. If this flag is specified, however, then\n"
		"the application must be run from the root directory from which the\n"
		"original digest was created.\n"
		"\n",
		XONLINEAUTOUPD_TARGET_MANIFEST_FILE_NAME,
		XONLINEAUTOUPD_TARGET_MANIFEST_FILE_NAME
	);
}

static void init_options(void)
{
	g_confirm_files		= false;
	g_overwrite_files	= false;
	g_set_id			= 0;
    g_disk_size         = LONG_MAX;

	g_debug_mode = false;
	g_recurse_subdirs = false;
	g_preserve_path_names = true;
	g_reserved_cfheader_space = 344;
	g_reserved_cffolder_space = 28;
	g_compression_type = COMPRESSION_TYPE_LZX;
	g_compression_memory = 15;

	init_list(&g_strip_path_list);
    init_list(&g_list_of_files_to_add);
}

static void cleanup(void)
{
	free_list(&g_strip_path_list);
    free_list(&g_list_of_files_to_add);
}


static bool parse_options(int argc, char **argv)
{
	HRESULT	hr = S_OK;
	int		argnum;
	int		cleft;
	int		cblength;
	int		cbsymmkey;
	bool	result;
	bool	fverify = FALSE;
	bool	fdump = FALSE;
	char	szdigest[MAX_PATH];
	byte	rgbsymmkey[XONLINE_KEY_LENGTH];

	DGSTFILE_HEADER	dfh;

	title();

	if (argc < 2)
	{
		help();
		return false;
	}

	argnum = 1;

	while (argnum < argc)
	{
		if ((argv[argnum][0] == '-') || (argv[argnum][0] == '/')) /* option? */
		{
			switch (argv[argnum][1])
			{
	            case '?':
	            case 'h':
	            case 'H':
	                help();
	                return false;
	                break;

				case 'r':
				case 'R':
					g_recurse_subdirs = TRUE;
					break;

				case 'd':
				case 'D':
					fdump = TRUE;
					break;

				case 'v':
				case 'V':
					fverify = TRUE;
					break;

				default:
					printf("Unknown option -%c\n", argv[argnum][1]);
					return false;
			}

			argnum++;
		}
		else
			break;
	}

	// See if we have the right number of parameters
	cleft = argc - argnum;
	if (cleft < 2)
	{
		puts("Too few arguments for BuildDigest\n");
		help();
		return false;
	}

	// Build the digest path
	cblength = strlen(argv[argnum]);
	if (cblength >= MAX_PATH)
	{
		puts("Digest path too long");
		return false;
	}
	strcpy(szdigest, argv[argnum++]);
	if (szdigest[cblength - 1] != '\\')
	szdigest[cblength++] = '\\';
	szdigest[cblength] = '\0';
	cblength += strlen(XONLINEAUTOUPD_TARGET_MANIFEST_FILE_NAME);
	if (cblength >= MAX_PATH)
	{
		puts("Digest path too long");
		return false;
	}
	strcat(szdigest, XONLINEAUTOUPD_TARGET_MANIFEST_FILE_NAME);

	// Convert the symmetric key to a byte array
	cbsymmkey = sizeof(rgbsymmkey);
	hr = ConvertSymmetricKeyToBinary(
				argv[argnum], strlen(argv[argnum]),
				rgbsymmkey, &cbsymmkey);
	if (hr != S_OK)
	{
		printf("Invalid symmetric key value %s\n", argv[argnum]);
		return false;
	}
	argnum++;
	cleft = argc - argnum;
	
	if (fdump)
	{
		// We are going to dump a manifest file
		if (cleft)
		{
			printf("Incorrect number of arguments for -d\n");
			help();
			return false;
		}

		// Call the dump API
		hr = DumpDigest(szdigest, fverify, rgbsymmkey, cbsymmkey);
		if (FAILED(hr))
		{
			printf("Failed to dump digest %s: %u (%08x)\n", 
						szdigest, hr, hr);
			return false;
		}
	}
	else
	{
		// We are creating a manifest file
		if (cleft < 4)
		{
			printf("Too few arguments to build new manifest\n");
			help();
			return false;
		}

		// OK, set up the digest file header
		dfh.wReservedSize = 0;
		dfh.wFlags = 0;
		dfh.wRating = 0;
		hr = ConvertToTitleId(argv[argnum++], &(dfh.dfi.dwTitleId));
		if (FAILED(hr))
		{
			puts("Invalid Title ID");
			return false;
		}
		if ((dfh.dfi.wTitleVersionHi = (WORD)atoi(argv[argnum++])) == 0)
		{
			puts("Invalid Version number");
			return false;
		}
		dfh.dfi.wTitleVersionLo = (WORD)atoi(argv[argnum++]);
		dfh.dfi.OfferingId = 0;

		// Delete the digest if it already exists
		DeleteFile(szdigest);

		// Get the list of files to process
		for (; argnum < argc; argnum++)
		{
            /* an @file? */
            if (argv[argnum][0] == '@')
            {
                if (input_file_list_from_file(&argv[argnum][1], &g_list_of_files_to_add) == false)
                {
                    printf("Error reading list of files from '%s'", &argv[argnum][1]);
                    return false;
                }

                continue;
            }

			result = expand_wildcard(
                &g_list_of_files_to_add,
				argv[argnum],
				g_recurse_subdirs,
				NULL
			);

			if (result == false)
				return false;
		}

		if (!g_list_of_files_to_add.flh_head)
		{
			printf("No input files specified\n");
			return false;
		}

		printf("Building digest %s with the following parameters:\n"\
				"  Title ID: %08x\n"\
				"  Title Version: %u.%u\n\n",
				szdigest, dfh.dfi.dwTitleId, 
				dfh.dfi.wTitleVersionHi,
				dfh.dfi.wTitleVersionLo);

		// Call the API to actually build the digest
		hr = CreateDigestFromFileList(
					&g_list_of_files_to_add,
					&dfh,
					szdigest,
					rgbsymmkey,
					cbsymmkey
					);
		if (FAILED(hr))
		{
			printf("Failed to create digest %s: %u (%08x)\n", 
						szdigest, hr, hr);
			return false;
		}
		else
		{
			printf("Digest %s successfully created\n\n", szdigest);
		}
	}

	return true;
}

