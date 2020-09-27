/*
 * main.c
 */

#define MAIN_FILE
#include "cabarc.h"
#include "BuildContent.rc"
#include "oldnames.h"

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
        "Microsoft (R) Xbox (R) Content Builder - Version " 
        VER_PRODUCTVERSION_STRING ".%u\n"
		VER_LEGALCOPYRIGHT_STR ".\n"
		"\n",
		VER_PRODUCTBUILD
	);

}

static void help(void)
{
	printf(
        "Usage: BuildContent [-r] [-o] [-D] N <package_path> <metadata_path> [@list] [files]\n"
		"\n"
		"<package_path> is a required parameter that specifies the path of the\n"
		"        resulting package. If only a file name is specified, the default\n"
		"        is the current directory.\n"
		"\n"
		"<metadata_path> is a required parameter that specifies the name and\n"
		"        location of a file that contains the catalog-referral metadata \n"
		"        about this package. This program does not validate the metadata\n"
		"        contained in the file, and it is the content publisher's\n"
		"        responsibility to make sure the metadata is valid.\n"
		"\n"
		"The list of files to be packaged is specified through a combination of\n"
		"filenames (which may contain wildcards), and/or list files. Filenames\n"
		"are relative to the current directory from which this application is run.\n"
		"List files may contain file names (one name per line) that are relative\n"
		"to the directory from which this program is run.\n"
		"\n"
		"By default, wildcard matches are non-recursive. Recursive matches can be\n"
		"enabled by specifying the -r option. The -r option is global and thus\n"
		"affects all wildcard matches. For example, to create a package that\n"
		"contains all files under the current directory hierarchy, one would do:\n"
		"\n"
		"The -o option can be specified to indicate the package is to be for offline\n"
		"distribution. The resultant package will be an installable package signed\n"
		"with the DevKit key and can be verified using the DevKit public key. The\n"
		"symmteric key used for offline packages will be TESTTESTTESTTEST.\n"
		"\n"
		"BuildContent -r N test.xcp ..\\metadata.dat *.*\n"
		"\n"
		"The directory hierarchy will be preserved when the package is installed\n"
		"on the Xbox hard disk. This allows the content publisher to layout the\n"
		"content exactly as it should appear on the Xbox, and then build a\n"
		"package the preserves the whole layout.\n"
		"\n"
		"A debug mode can be specified using the -D option, which will output\n"
		"the raw cabinet file without the secure packaging. This allows the\n"
		"content publisher to list and extract the files to verify that the\n"
		"cabinet layout is correct. The raw cabinet file will be named\n"
		"<package_path>.cab. The raw cabinet can be accessed using the\n"
		"standard cabarc utility, or one of the following commands:\n"
		"\n"
		"List:       BuildContent L <raw_cabinet>\n"
        "Extract:    BuildContent X <raw_cabinet> [files] [output_dir]\n"
		"\n"
		"Notes\n"
		"-----\n"
		"When creating a cabinet, the plus sign (+) may be used as a filename\n"
		"to force a folder boundary;\n"
		"\n"
		"e.g. BuildContent N test.xcp ..\\info.dat *.c test.h + *.bmp + @..\\filelist\n"
        "\n"
        "The example above causes test.xcp to be created, using ..\\info.dat\n"
        "as the metadata file. test.xcp will contain 3 folders, one with *.c\n"
        "and test.h, another with *.bmp, and a third one containing all the files\n"
        "specified ina file named filelist in the parent directory.\n"
        "\n"
        "The Xbox content package format allows up to 100 folders per package.\n"
        "Do not exceed this limit or your package will not work.\n"
        "\n"
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
	g_offline = false;

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
	int		argnum;
	bool	result;

	title();

	if (argc < 2)
	{
		help();
		return false;
	}

	argnum = 1;

    while ((argv[argnum][0] == '-') || (argv[argnum][0] == '/')) /* option? */
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
				g_recurse_subdirs = true;
				break;

			case 'o':
				g_offline = true;
				break;

			case 'D':
				g_debug_mode = true;
				break;

			default:
				printf("Unknown option -%c\n", argv[argnum][1]);
				return false;
		}

		argnum++;

		if (argnum >= argc)
		{
			printf("You need to specify a command\n");
			return false;
		}
	}

	if (argnum+1 >= argc)
	{
		printf("Insufficient parameters\n");
		return false;
	}

	switch (toupper(argv[argnum][0]))
	{
		case 'L': /* list files in cabinet */
		{
			char *cab_name;

			cab_name = argv[++argnum];

			list_cab(cab_name);
			break;
		}

		case 'N': /* add files to cabinet */
		{
			char *cab_name;

			argnum++;
			cab_name = argv[argnum++];

			if (argnum >= argc)
			{
				printf("Insufficient parameters\n");
				return false;
			}
			strcpy(g_metadata_file_path, argv[argnum++]);

			for (; argnum < argc; argnum++)
			{
                if (!strcmp(argv[argnum], FOLDER_SEPARATOR_STRING))
				{
                    (void) add_to_list(&g_list_of_files_to_add, FOLDER_SEPARATOR_STRING, NULL);
					continue;
				}

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
				printf("No files specified\n");
				return false;
			}
            create_new_cab(cab_name, &g_list_of_files_to_add);
			break;
		}

		case 'X': /* extract files from cabinet */
		{
			char *cab_name;
			cab_file_list_header list_of_files_to_extract;

			init_list(&list_of_files_to_extract);

			cab_name = argv[++argnum];
			argnum++;

			for (; argnum < argc; argnum++)
			{
				/*
				 * Destination dir?
				 */
				if (strlen(argv[argnum]) > 0)
				{
					if (argv[argnum][ strlen(argv[argnum])-1 ] == '\\')
					{
						strcpy(g_dest_dir, argv[argnum]);
						continue;
					}
				}

                if (argv[argnum][0] == '@')
                {
                    printf("Error, @files not supported for extract\n");
                    return false;
                }

				result = add_to_list(
					&list_of_files_to_extract,
                    argv[argnum],
                    NULL
				);

				if (result == false)
					return false;
			}
			
			extract_from_cab(cab_name, &list_of_files_to_extract);
			free_list(&list_of_files_to_extract);
			break;
		}

		default:
		{
			printf("Unknown command %c\n", argv[argnum][0]);
			return false;
		}
	}
	
	return true;
}

