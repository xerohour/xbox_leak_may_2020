/*
 * proto.h
 */

/* lists.c */
void init_list(cab_file_list_header *header);

bool concatenate_and_free_list(
   cab_file_list_header *dest_list,
   cab_file_list_header *source_list
);

bool add_to_list(
   cab_file_list_header *header,
   const byte *filename,
   const byte *logical_filename
);

void free_list(cab_file_list_header *header);

struct cab_file_link *filename_in_list(
   const byte *filename,
   const cab_file_list_header *header,
   bool require_exact_match
);

bool file_in_wildcard_list(
   const byte *filename,
   const cab_file_list_header *header
);

bool input_file_list_from_file(
    const byte *filename,
    cab_file_list_header *header
);


/* memory.c */
byte *mem_malloc(long size);
void mem_free(byte *mem);


/* wildcard.c */
bool wildcards_in_string(const byte *wildcard);

bool wildcard_match(const byte *string, const byte *wildcard);

bool expand_wildcard(
   cab_file_list_header *list_header,
   const byte           *wildcard,
   bool                 recurse_subdirs,
   bool                 (*qualifier_function)(const byte *filename)
);


/* fdiutil.c */
char *return_fdi_error_string(FDIERROR err);


/* fciutil.c */
void strip_path(char *filename, char *stripped_name);
int get_percentage(unsigned long a, unsigned long b);
char *return_fci_error_string(FCIERROR err);


/* cmdlist.c */
bool list_cab(char *cabname);


/* cmdext.c */
bool create_directories_for_file(const byte *filename);
bool extract_from_cab(char *cabname, cab_file_list_header *list);


/* cmdnew.c */
bool create_new_cab(char *cabname, cab_file_list_header *files_to_add);


/* callback.c */
FNFCIALLOC(callback_fci_alloc);
FNFCIFREE(callback_fci_free);
FNFCIOPEN(callback_fci_file_open);
FNFCICLOSE(callback_fci_file_close);
FNFCIREAD(callback_fci_file_read);
FNFCIWRITE(callback_fci_file_write);
FNFCISEEK(callback_fci_file_seek);
FNFCIDELETE(callback_fci_file_delete);

FNALLOC(callback_alloc);
FNFREE(callback_free);
FNOPEN(callback_file_open);
FNCLOSE(callback_file_close);
FNREAD(callback_file_read);
FNWRITE(callback_file_write);
FNSEEK(callback_file_seek);

