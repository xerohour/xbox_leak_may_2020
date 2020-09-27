
xbDebugps.dll: dlldata.obj xbDebug_p.obj xbDebug_i.obj
	link /dll /out:xbDebugps.dll /def:xbDebugps.def /entry:DllMain dlldata.obj xbDebug_p.obj xbDebug_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del xbDebugps.dll
	@del xbDebugps.lib
	@del xbDebugps.exp
	@del dlldata.obj
	@del xbDebug_p.obj
	@del xbDebug_i.obj
