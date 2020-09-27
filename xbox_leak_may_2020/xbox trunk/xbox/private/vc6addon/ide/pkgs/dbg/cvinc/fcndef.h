typedef HWND    PWND;
typedef struct _region *PREGION;

//
//  load.c
//
void			quit (int);
//
//  process.c
//

void			set_addrs (void);
//  cv0.c
//
void    PASCAL  ESLoadExpr(int, unsigned int, LPFNEEINIT);
int 	PASCAL  ESilan (void);
int 	PASCAL  ESSetFromIndex (int);
int     PASCAL  get_initial_context (PCXT);
int 	PASCAL  line_number_address (PADDR, USHORT, char *);
void    PASCAL  go_until(PADDR, HTHD, int, BOOL);
//
//  cv1.c
//
void    PASCAL  display_pc (void);
void	PASCAL 	UpdateUserEnvir (USHORT);
void	PASCAL 	freeze_view (void);
//
//  debcom.c
//
void	PASCAL FAR compare (PADDR, long, PADDR);
//
//  utils.c
//
char *				 error_string (USHORT);
char *				 get_string (void);
int 				 eol (void);
void				 skip_white (void);
void				 skip_not_white (void);
void				 skip_to_eol (void);
void				 reset_ptxt (void);
void				 SplitPath (char *, char *, char *, char *, char *);
#ifdef OSDEBUG4
int    OSDAPI dsquit (DWORD);
#else
int    OSDAPI dsquit (unsigned);
#endif

int    LOADDS		 ds_eprintfT (const char *, char *, char *, int);
int    LOADDS		 ds_eprintf (const char  FAR *, char  FAR *, char  FAR *, int);
int    LOADDS		 ds_sprintf(char near *, const char FAR *, ...);
char * LOADDS		 ds_ultoa ( ULONG, char *, int );
char * LOADDS		 ds_itoa ( int, char *, int );
char * LOADDS		 ds_ltoa ( long, char *, int );
FLOAT10 LOADDS		 ds_strtold ( const char *, char ** );
void				 _fswap(void FAR *, void FAR *, size_t);
void				 _fqsort(void FAR *, size_t, size_t, int (*)
					 	(const void FAR *, const void FAR *));
//
//  errors.c
//
void				 fetch_error (USHORT, char  FAR *);
//
//  dispset.c
//
int 				 update_watch_window (void);
//
//	system<x>.c
//
int 				 readfar(UINT, char FAR *, UINT);

void				 dos_screen (void);
void				 island_screen (void);
void	PASCAL 	 	 DispPrompt (void);
void				 dprintf (char *, ...);
void				 dputs (char *);
