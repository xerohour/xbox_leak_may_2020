
vcprojcnvtps.dll: dlldata.obj vcprojcnvt_p.obj vcprojcnvt_i.obj
	link /dll /out:vcprojcnvtps.dll /def:vcprojcnvtps.def /entry:DllMain dlldata.obj vcprojcnvt_p.obj vcprojcnvt_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del vcprojcnvtps.dll
	@del vcprojcnvtps.lib
	@del vcprojcnvtps.exp
	@del dlldata.obj
	@del vcprojcnvt_p.obj
	@del vcprojcnvt_i.obj
