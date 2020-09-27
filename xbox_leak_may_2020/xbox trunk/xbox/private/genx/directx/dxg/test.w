sinclude(`d3dmkhdr.m4')dnl This file must be preprocessed by the m4 preprocessor.

begin_interface(IDirect3D8)
begin_methods()

declare_method(RegisterSoftwareDevice, void* pInitializeFunction);

declare_method2(UINT, GetAdapterCount);
declare_method(GetAdapterIdentifier, UINT, DWORD *Flags, D3DADAPTER_IDENTIFIER8* pIdentifier);
declare_method2(VOID, GetAdapterModeCount, UINT Adapter);
declare_method(EnumAdapterModes, UINT Adapter, UINT Mode, D3DDISPLAYMODE* pMode);
declare_method(GetAdapterDisplayMode, UINT Adapter, D3DDISPLAYMODE* pMode);

end_methods()
end_interface()

