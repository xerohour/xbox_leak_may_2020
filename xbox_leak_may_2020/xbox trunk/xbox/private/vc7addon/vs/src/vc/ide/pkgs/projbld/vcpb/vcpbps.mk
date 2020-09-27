
vcpbps.dll: dlldata.obj vcpb_p.obj vcpb_i.obj
	link /dll /out:vcpbps.dll /def:vcpbps.def /entry:DllMain dlldata.obj vcpb_p.obj vcpb_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del vcpbps.dll
	@del vcpbps.lib
	@del vcpbps.exp
	@del dlldata.obj
	@del vcpb_p.obj
	@del vcpb_i.obj
