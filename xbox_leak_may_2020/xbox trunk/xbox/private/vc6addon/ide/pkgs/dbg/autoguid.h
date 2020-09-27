// GUIDs used in our automation object model that are not exposed to the user
//  in include\objmodel\dbgguid.h.  These are the dispinterface IDs and the
//  CLSIDs which are declared in the type library and used internally to 
//  implement the objects, but are not exposed to C++ clients through our
//  headers.

// implementation class GUID
// {34C63003-AE64-11cf-AB59-00AA00C091A1}
DEFINE_GUID(CLSID_Debugger,0x34C63003L,0xAE64,0x11CF,0xAB,0x59,0x00,0xAA,0x00,0xC0,0x91,0xA1);

// implementation class GUID
// {34C63006-AE64-11cf-AB59-00AA00C091A1}
DEFINE_GUID(CLSID_Breakpoint,0x34C63006L,0xAE64,0x11CF,0xAB,0x59,0x00,0xAA,0x00,0xC0,0x91,0xA1);

// implementation class GUID
// {34C63009-AE64-11cf-AB59-00AA00C091A1}
DEFINE_GUID(CLSID_Breakpoints,0x34C63009L,0xAE64,0x11CF,0xAB,0x59,0x00,0xAA,0x00,0xC0,0x91,0xA1);

