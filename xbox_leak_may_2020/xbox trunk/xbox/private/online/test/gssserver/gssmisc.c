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

#include "gss-misc.h"

FILE *display_file;

void display_status_1
PROTOTYPE( (char *m, ULONG code, int type) );

int write_all(int fildes, char *buf, unsigned int nbyte)
{
   int ret;
   char *ptr;

   for (ptr = buf; nbyte; ptr += ret, nbyte -= ret)
   {
      ret = send(fildes, ptr, nbyte,0);
      if (ret < 0)
      {
         return (ret);
      }
      else if (ret == 0)
      {
         return (ptr-buf);
      }
   }

   return (ptr-buf);
}

int read_all(int fildes, char *buf, unsigned int nbyte)
{
   int ret;
   char *ptr;

   for (ptr = buf; nbyte; ptr += ret, nbyte -= ret)
   {
      ret = recv(fildes, ptr, nbyte,0);
      if (ret < 0)
      {
         return (ret);
      }
      else if (ret == 0)
      {
         return (ptr-buf);
      }
   }

   return (ptr-buf);
}

/*
 * Function: send_token
 *
 * Purpose: Writes a token to a file descriptor.
 *
 * Arguments:
 *
 *      s               (r) an open file descriptor
 *      tok             (r) the token to write
 *
 * Returns: 0 on success, -1 on failure
 *
 * Effects:
 *
 * send_token writes the token length (as a network long) and then the
 * token data to the file descriptor s.  It returns 0 on success, and
 * -1 if an error occurs or if it could not write all the data.
 */
int send_token(int s, PSecBuffer tok)
{
   ULONG len;
   LONG ret;

   len = htonl(tok->cbBuffer);

   ret = write_all(s, (char *) &len, 4);
   if (ret < 0)
   {
      perror("sending token length");
      return -1;
   }
   else if (ret != 4)
   {
      if (display_file)
         fprintf(display_file,
                 "sending token length: %d of %d bytes written\n",
                 ret, 4);
      return -1;
   }

   ret = write_all(s, tok->pvBuffer, tok->cbBuffer);
   if (ret < 0)
   {
      perror("sending token data");
      return -1;
   }
   else if (ret != (LONG) tok->cbBuffer)
   {
      if (display_file)
         fprintf(display_file,
                 "sending token data: %d of %d bytes written\n",
                 ret, tok->cbBuffer);
      return -1;
   }

   return 0;
}

/*
 * Function: recv_token
 *
 * Purpose: Reads a token from a file descriptor.
 *
 * Arguments:
 *
 *      s               (r) an open file descriptor
 *      tok             (w) the read token
 *
 * Returns: 0 on success, -1 on failure
 *
 * Effects:
 *
 * recv_token reads the token length (as a network long), allocates
 * memory to hold the data, and then reads the token data from the
 * file descriptor s.  It blocks to read the length and data, if
 * necessary.  On a successful return, the token should be freed with
 * gss_release_buffer.  It returns 0 on success, and -1 if an error
 * occurs or if it could not read all the data.
 */
int recv_token(int s, PSecBuffer tok)
{
    LONG ret;

   ret = read_all(s, (char *) &tok->cbBuffer, 4);
   if (ret < 0)
   {
      perror("reading token length");
      return -1;
   }
   else if (ret != 4)
   {
      if (display_file)
         fprintf(display_file,
                 "reading token length: %d of %d bytes read\n",
                 ret, 4);
      return -1;
   }

   tok->cbBuffer = ntohl(tok->cbBuffer);
   tok->pvBuffer = (char *) malloc(tok->cbBuffer);
   if (tok->pvBuffer == NULL)
   {
      if (display_file)
         fprintf(display_file,
                 "Out of memory allocating token data\n");
      return -1;
   }

   ret = read_all(s, (char *) tok->pvBuffer, tok->cbBuffer);
   if (ret < 0)
   {
      perror("reading token data");
      free(tok->pvBuffer);
      return -1;
   }
   else if (ret != (LONG) tok->cbBuffer)
   {
      fprintf(stderr, "sending token data: %d of %d bytes written\n",
              ret, tok->cbBuffer);
      free(tok->pvBuffer);
      return -1;
   }

   return 0;
}

void display_status_1(
    char *m,
    ULONG code,
    int type
    )
{
   type;
   
   if (display_file)
      fprintf(display_file, "GSS-API error %s: %x\n", m,
              code);
}

/*
 * Function: display_status
 *
 * Purpose: displays GSS-API messages
 *
 * Arguments:
 *
 *      msg             a string to be displayed with the message
 *      maj_stat        the GSS-API major status code
 *      min_stat        the GSS-API minor status code
 *
 * Effects:
 *
 * The GSS-API messages associated with maj_stat and min_stat are
 * displayed on stderr, each preceeded by "GSS-API error <msg>: " and
 * followed by a newline.
 */
void display_status(
    char *msg,
    ULONG maj_stat,
    ULONG min_stat
    )
{
   display_status_1(msg, maj_stat, GSS_C_GSS_CODE);
   if ( min_stat != 0 ) display_status_1(msg, min_stat, GSS_C_MECH_CODE);
}

/*
 * Function: display_ctx_flags
 *
 * Purpose: displays the flags returned by context initation in
 *          a human-readable form
 *
 * Arguments:
 *
 *      int             ret_flags
 *
 * Effects:
 *
 * Strings corresponding to the context flags are printed on
 * stdout, preceded by "context flag: " and followed by a newline
 */

void display_ctx_flags(
    ULONG flags
    )
{
   if (flags & GSS_C_DELEG_FLAG)
      fprintf(display_file, "context flag: GSS_C_DELEG_FLAG\n");
   if (flags & GSS_C_MUTUAL_FLAG)
      fprintf(display_file, "context flag: GSS_C_MUTUAL_FLAG\n");
   if (flags & GSS_C_REPLAY_FLAG)
      fprintf(display_file, "context flag: GSS_C_REPLAY_FLAG\n");
   if (flags & GSS_C_SEQUENCE_FLAG)
      fprintf(display_file, "context flag: GSS_C_SEQUENCE_FLAG\n");
   if (flags & GSS_C_CONF_FLAG )
      fprintf(display_file, "context flag: GSS_C_CONF_FLAG \n");
   if (flags & GSS_C_INTEG_FLAG )
      fprintf(display_file, "context flag: GSS_C_INTEG_FLAG \n");
}

void print_token(
    PSecBuffer tok
    )
{
   ULONG i;
   unsigned char *p = (PUCHAR) tok->pvBuffer;

   if (!display_file)
      return;
   for (i=0; i < tok->cbBuffer; i++, p++)
   {
      fprintf(display_file, "%02x ", *p);
      if ((i % 16) == 15)
      {
         fprintf(display_file, "\n");
      }
   }
   fprintf(display_file, "\n");
   fflush(display_file);
}

/*
 * Function: connect_to_server
 *
 * Purpose: Opens a TCP connection to the name host and port.
 *
 * Arguments:
 *
 *      host            (r) the target host name
 *      port            (r) the target port, in host byte order
 *
 * Returns: the established socket file desciptor, or -1 on failure
 *
 * Effects:
 *
 * The host name is resolved with gethostbyname(), and the socket is
 * opened and connected.  If an error occurs, an error message is
 * displayed and -1 is returned.
 */

int connect_to_server(
    char *host,
    u_short port
    )
{
   struct sockaddr_in saddr;
   struct hostent *hp;
   int s;
   int err;
   WSADATA socket_data;
   USHORT version_required = 0x0101;


   err = WSAStartup(version_required, &socket_data);
   if (err)
   {
      fprintf(stderr,"Failed to initailize WSA: %d\n",err);
      return (-1);
   }


   if ((hp = gethostbyname(host)) == NULL)
   {
      fprintf(stderr, "Unknown host: %s\n", host);
      return -1;
   }

   saddr.sin_family = hp->h_addrtype;
   memcpy((char *)&saddr.sin_addr, hp->h_addr, sizeof(saddr.sin_addr));
   saddr.sin_port = htons(port);

   if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
   {
      fprintf(stderr, "creating socket - %x", GetLastError());
      return -1;
   }

   if (connect(s, (struct sockaddr *)&saddr, sizeof(saddr)) == SOCKET_ERROR)
   {
      fprintf(stderr, "connecting to server - %x", GetLastError());
      (void) closesocket(s);
      return -1;
   }
   return s;
}

/*
 * Function: client_establish_context
 *
 * Purpose: establishes a GSS-API context with a specified service and
 * returns the context handle
 *
 * Arguments:
 *
 *      s               (r) an established TCP connection to the service
 *      service_name    (r) the ASCII service name of the service
 *      context         (w) the established GSS-API context
 *      ret_flags       (w) the returned flags from init_sec_context
 *
 * Returns: 0 on success, -1 on failure
 *
 * Effects:
 *
 * service_name is imported as a GSS-API name and a GSS-API context is
 * established with the corresponding service; the service should be
 * listening on the TCP connection s.  The default GSS-API mechanism
 * is used, and mutual authentication and replay detection are
 * requested.
 *
 * If successful, the context handle is returned in context.  If
 * unsuccessful, the GSS-API error messages are displayed on stderr
 * and -1 is returned.
 */
int client_establish_context(
    int s,
    char *service_name,
    OM_uint32 deleg_flag,
    CtxtHandle *gss_context,
    OM_uint32 *ret_flags
    )
{
   SecBuffer send_tok, recv_tok;
   SecBufferDesc input_desc, output_desc;
   OM_uint32 maj_stat;
   CredHandle cred_handle;
   TimeStamp expiry;
   PCtxtHandle context_handle = NULL;


   input_desc.cBuffers = 1;
   input_desc.pBuffers = &recv_tok;
   input_desc.ulVersion = SECBUFFER_VERSION;

   recv_tok.BufferType = SECBUFFER_TOKEN;
   recv_tok.cbBuffer = 0;
   recv_tok.pvBuffer = NULL;

   output_desc.cBuffers = 1;
   output_desc.pBuffers = &send_tok;
   output_desc.ulVersion = SECBUFFER_VERSION;

   send_tok.BufferType = SECBUFFER_TOKEN;
   send_tok.cbBuffer = 0;
   send_tok.pvBuffer = NULL;

   cred_handle.dwLower = 0;
   cred_handle.dwUpper = 0;

   maj_stat = AcquireCredentialsHandle(
                                      NULL,                       // no principal name
                                      "Kerberos",                 // package name
                                      SECPKG_CRED_OUTBOUND,
                                      NULL,                       // no logon id
                                      NULL,                       // no auth data
                                      NULL,                       // no get key fn
                                      NULL,                       // noget key arg
                                      &cred_handle,
                                      &expiry
                                      );
   if (maj_stat != SEC_E_OK)
   {
      display_status("acquiring credentials",maj_stat, GetLastError());
      return (-1);
   }

   /*
    * Perform the context-establishement loop.
    */

   gss_context->dwLower = 0;
   gss_context->dwUpper = 0;

   do
   {
      maj_stat =
      InitializeSecurityContext(
                               &cred_handle,
                               context_handle,
                               service_name,
                               deleg_flag |
			       ISC_REQ_ALLOCATE_MEMORY,
                               0,          // reserved
                               SECURITY_NATIVE_DREP,
                               &input_desc,
                               0,          // reserved
                               gss_context,
                               &output_desc,
                               ret_flags,
                               &expiry
                               );

      if (recv_tok.pvBuffer)
      {
         free(recv_tok.pvBuffer);
         recv_tok.pvBuffer = NULL;
         recv_tok.cbBuffer = 0;

      }

      context_handle = gss_context;

      if (maj_stat!=SEC_E_OK && maj_stat!=SEC_I_CONTINUE_NEEDED)
      {
         display_status("initializing context", maj_stat, GetLastError());
         FreeCredentialsHandle(&cred_handle);
         return -1;
      }

      if (send_tok.cbBuffer != 0)
      {
         printf("Sending init_sec_context token (size=%d)...",
                send_tok.cbBuffer);

	 if ( getenv( "GSSCLIENT_VERBOSE" ) ) {

	   print_token( &send_tok );

	 }


         if (send_token(s, &send_tok) < 0)
         {
            FreeContextBuffer(send_tok.pvBuffer);
            FreeCredentialsHandle(&cred_handle);
            return -1;
         }
      }

      FreeContextBuffer(send_tok.pvBuffer);
      send_tok.pvBuffer = NULL;
      send_tok.cbBuffer = 0;

      if (maj_stat == SEC_I_CONTINUE_NEEDED)
      {
         printf("continue needed...");
         if (recv_token(s, &recv_tok) < 0)
         {
            FreeCredentialsHandle(&cred_handle);
            return -1;
         }
      }
      printf("\n");

   } while (maj_stat == SEC_I_CONTINUE_NEEDED);

   FreeCredentialsHandle(&cred_handle);
   return 0;
}

void read_file(
    char *file_name,
    PSecBuffer in_buf
    )
{
   int fd;
   LONG bytes_in;
   LONG  count;
   UCHAR buf[100];

   //
   // readthrough once to get the size.
   //

   if ((fd = _open(file_name, O_RDONLY, 0)) < 0)
   {
      perror("open");
      fprintf(stderr, "Couldn't open file %s\n", file_name);
      exit(1);
   }

   for (bytes_in = 0; ; bytes_in += count)
   {
      count = _read(fd, buf, sizeof(buf));
      if (count < 0)
      {
         fprintf(stderr, "read - %x", GetLastError());
         exit(1);
      }
      if (count == 0)
         break;
   }
   _close(fd);

   if ((fd = _open(file_name, O_RDONLY, 0)) < 0)
   {
      perror("open");
      fprintf(stderr, "Couldn't open file %s\n", file_name);
      exit(1);
   }

   in_buf->cbBuffer = bytes_in;
   in_buf->pvBuffer = malloc(in_buf->cbBuffer);

   if (in_buf->pvBuffer == 0)
   {
      fprintf(stderr, "Couldn't allocate %d byte buffer for reading file\n",
              in_buf->cbBuffer);
      exit(1);
   }

   memset(in_buf->pvBuffer, 0, in_buf->cbBuffer);

   for (bytes_in = 0; 
	bytes_in < (LONG) in_buf->cbBuffer; 
	bytes_in += count)
   {
      count = _read(fd, (PUCHAR) in_buf->pvBuffer+bytes_in, in_buf->cbBuffer-bytes_in);
      if (count < 0)
      {
         fprintf(stderr, "read - %x", GetLastError());
         exit(1);
      }
      if (count == 0)
         break;
   }

   if (bytes_in != count)
      fprintf(stderr, "Warning, only read in %d bytes, expected %d\n",
              bytes_in, count);
}

/*
 * Function: call_server
 *
 * Purpose: Call the "sign" service.
 *
 * Arguments:
 *
 *      host            (r) the host providing the service
 *      port            (r) the port to connect to on host
 *      service_name    (r) the GSS-API service name to authenticate to
 *      msg             (r) the message to have "signed"
 *
 * Returns: 0 on success, -1 on failure
 *
 * Effects:
 *
 * call_server opens a TCP connection to <host:port> and establishes a
 * GSS-API context with service_name over the connection.  It then
 * seals msg in a GSS-API token with gss_seal, sends it to the server,
 * reads back a GSS-API signature block for msg from the server, and
 * verifies it with gss_verify.  -1 is returned if any step fails,
 * otherwise 0 is returned.
 */
int call_server(
    char *host,
    u_short port,
    char *service_name,
    OM_uint32 deleg_flag,
    char *msg,
    int use_file,
    BOOL SignOnly
    )
{
   CtxtHandle context;
   SecBuffer in_buf, out_buf;
   SecBuffer wrap_bufs[3];
   SecBufferDesc in_buf_desc;
   SecPkgContext_Sizes sizes;
   int s;
   OM_uint32 ret_flags;
   OM_uint32 maj_stat;
   gss_qop_t qop_state;

   /* Open connection */
   if ((s = connect_to_server(host, port)) < 0)
      return -1;

   /* Establish context */
   if (client_establish_context(s, service_name, deleg_flag,  &context,
                                &ret_flags) < 0)
   {
      (void) closesocket(s);
      return -1;
   }

   maj_stat = QueryContextAttributes(
                                    &context,
                                    SECPKG_ATTR_SIZES,
                                    &sizes
                                    );
   if (maj_stat != SEC_E_OK)
   {
      display_status("querying context attributes", maj_stat, GetLastError());
      return (-1);
   }

   if (use_file)
   {
      read_file(msg, &in_buf);
   }
   else
   {
      /* Seal the message */
      in_buf.pvBuffer = msg;
      in_buf.cbBuffer = strlen(msg) + 1;
   }

   //
   // Prepare to encrypt the message
   //

   printf("Block Size is %d, inbuffer = %d\n",
          sizes.cbBlockSize, in_buf.cbBuffer);

   in_buf_desc.cBuffers = 3;
   in_buf_desc.pBuffers = wrap_bufs;
   in_buf_desc.ulVersion = SECBUFFER_VERSION;

   wrap_bufs[0].cbBuffer = sizes.cbSecurityTrailer;
   wrap_bufs[0].BufferType = SECBUFFER_TOKEN;
   wrap_bufs[0].pvBuffer = malloc(sizes.cbSecurityTrailer);

   if (wrap_bufs[0].pvBuffer == NULL)
   {
      fprintf(stderr,"Failed to allocate space for security trailer\n");
      return (-1);
   }

   wrap_bufs[1].BufferType = SECBUFFER_DATA;
   wrap_bufs[1].cbBuffer = in_buf.cbBuffer;
   wrap_bufs[1].pvBuffer = malloc(wrap_bufs[1].cbBuffer);

   if (wrap_bufs[1].pvBuffer == NULL)
   {
      fprintf(stderr,"Couldn't allocate space for wrap message\n");
      return (-1);
   }

   memcpy(
         wrap_bufs[1].pvBuffer,
         in_buf.pvBuffer,
         in_buf.cbBuffer
         );

   wrap_bufs[2].BufferType = SECBUFFER_PADDING;
   wrap_bufs[2].cbBuffer = sizes.cbBlockSize;
   wrap_bufs[2].pvBuffer = malloc(wrap_bufs[2].cbBuffer);

   if (wrap_bufs[2].pvBuffer == NULL)
   {
      fprintf(stderr,"Couldn't allocate space for wrap message\n");
      return (-1);
   }



   maj_stat = EncryptMessage(
                            &context,
                            SignOnly ? KERB_WRAP_NO_ENCRYPT : 0,
                            &in_buf_desc,
                            0);


   if (maj_stat != SEC_E_OK)
   {
      display_status("sealing message", maj_stat, GetLastError() );
      (void) closesocket(s);
      (void) DeleteSecurityContext(&context);
      return -1;
   }

   //
   // Create the mesage to send to server
   //

   out_buf.cbBuffer = wrap_bufs[0].cbBuffer + wrap_bufs[1].cbBuffer + wrap_bufs[2].cbBuffer;
   out_buf.pvBuffer = malloc(out_buf.cbBuffer);

   if (out_buf.pvBuffer == NULL)
   {
      fprintf(stderr,"Failed to allocate space for wrapepd message\n");
      return (-1);
   }

   memcpy(
         out_buf.pvBuffer,
         wrap_bufs[0].pvBuffer,
         wrap_bufs[0].cbBuffer
         );
   memcpy(
         (PUCHAR) out_buf.pvBuffer + (int) wrap_bufs[0].cbBuffer,
         wrap_bufs[1].pvBuffer,
         wrap_bufs[1].cbBuffer
         );
   memcpy(
         (PUCHAR) out_buf.pvBuffer + wrap_bufs[0].cbBuffer + wrap_bufs[1].cbBuffer,
         wrap_bufs[2].pvBuffer,
         wrap_bufs[2].cbBuffer
         );

   /* Send to server */
   if (send_token(s, &out_buf) < 0)
   {
      (void) closesocket(s);
      (void) DeleteSecurityContext(&context);
      return -1;
   }

   free(out_buf.pvBuffer);
   out_buf.pvBuffer = NULL;
   out_buf.cbBuffer = 0;
   free(wrap_bufs[0].pvBuffer);
   wrap_bufs[0].pvBuffer = NULL;
   free(wrap_bufs[1].pvBuffer);
   wrap_bufs[1].pvBuffer = NULL;

   /* Read signature block into out_buf */
   if (recv_token(s, &out_buf) < 0)
   {
      (void) closesocket(s);
      (void) DeleteSecurityContext( &context);
      return -1;
   }

   /* Verify signature block */

   in_buf_desc.cBuffers = 2;
   wrap_bufs[0] = in_buf;
   wrap_bufs[0].BufferType = SECBUFFER_DATA;
   wrap_bufs[1] = out_buf;
   wrap_bufs[1].BufferType = SECBUFFER_TOKEN;

   maj_stat = VerifySignature(&context, &in_buf_desc, 0, &qop_state);
   if (maj_stat != SEC_E_OK)
   {
      display_status("verifying signature", maj_stat, GetLastError());
      (void) closesocket(s);
      (void) DeleteSecurityContext( &context);
      return -1;
   }
   free(out_buf.pvBuffer);

   if (use_file)
      free(in_buf.pvBuffer);

   printf("Signature verified.\n");

   /* Delete context */
   maj_stat = DeleteSecurityContext(&context);
   if (maj_stat != SEC_E_OK)
   {
      display_status("deleting context", maj_stat, GetLastError() );
      (void) closesocket(s);
      return -1;
   }

   (void) closesocket(s);
   return 0;
}


