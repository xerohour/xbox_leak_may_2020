/* 
 * vars.h
 */

#ifdef MAIN_FILE
#	define EXT
#else
#	define EXT extern
#endif


EXT char				g_dest_dir[MAX_PATHNAME_LENGTH];
EXT long				g_set_id;
EXT cab_file_list_header	g_strip_path_list;
EXT cab_file_list_header    g_list_of_files_to_add;
EXT bool				g_confirm_files;
EXT bool				g_overwrite_files;
EXT bool				g_recurse_subdirs;
EXT bool                g_offline;
EXT bool				g_preserve_path_names;
EXT t_compression_type	g_compression_type;
EXT long				g_compression_memory;
EXT long				g_reserved_cfheader_space;
EXT long				g_reserved_cffolder_space;
EXT long                g_disk_size;
EXT char                g_cabinet_template[MAX_PATHNAME_LENGTH];
EXT char                g_metadata_file_path[MAX_PATHNAME_LENGTH];
EXT bool                g_debug_mode;

