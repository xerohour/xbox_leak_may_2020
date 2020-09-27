/*
 * Copyright 1994 by OpenVision Technologies, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and
 * that both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OpenVision not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. OpenVision makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * OPENVISION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL OPENVISION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* Our build process sucks.  Compensate by disabling warnings that 
   are more or less extraneous. */

#pragma warning( disable:4242 )
#pragma warning( disable:4311 )
#pragma warning( disable:4244 )

#ifndef _GSSMISC_H_
   #define _GSSMISC_H_

   #ifdef UNICODE
      #undef UNICODE
   #endif
   #include <windows.h>
   #include <rpc.h>
   #include <stdio.h>
   #include <stdlib.h>
   #include <io.h>
   #include <fcntl.h>
   #include <winsock2.h>
   #define SECURITY_WIN32
   #include <security.h>
   #include <ntsecapi.h>
   #include "gssapi.h"

   #define PROTOTYPE(_x_) _x_
extern FILE *display_file;

int send_token
PROTOTYPE( (int s, PSecBuffer tok) );
int recv_token
PROTOTYPE( (int s, PSecBuffer tok) );
void display_status
PROTOTYPE( (char *msg, ULONG maj_stat, ULONG min_stat) );
void display_ctx_flags
PROTOTYPE( (ULONG flags) );
void print_token
PROTOTYPE( (PSecBuffer tok) );

#endif

int 
connect_to_server( IN char *host, u_short port);

int call_server( char *host, 
		 u_short port, 
		 char *service_name, 
		 OM_uint32 deleg_flag, 
		 char *msg, 
		 int use_file,
		 BOOL SignOnly );


typedef struct
{

   PCHAR     name;
   OM_uint32 value;
   PCHAR     realname;

} FLAGMAPPING, *PFLAGMAPPING;


