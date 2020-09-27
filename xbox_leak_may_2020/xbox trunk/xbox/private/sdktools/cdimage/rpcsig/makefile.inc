rpcsig_c.c rpcsig_s.c rpcsig.h: rpcsig.idl
	midl /app_config /win32 /Oi /W3 rpcsig.idl