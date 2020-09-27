// propguid.h


#pragma once
// for property browser.
// define these custom guids on a property for special effects


// GUID_PROPERTYRECURSE -- the property is an IDispatch and will become a parent node. It is expected that the interface have
// a DISPID_ENABLED property, which will be displayed as a bool.
// {BF7971D2-28BF-11d2-8314-0000F8076314}
extern "C" const __declspec(selectany) GUID GUID_PROPERTYRECURSE = {0xBF7971D2L,0x28BF,0x11D2, { 0x83,0x14,0x00,0x00,0xF8,0x07,0x63,0x14 } };

// GUID_PROPERTYPOPULARITY -- the property is considered the rank in popularity of a property. This is for quick filtering in the property grid.
//                            highest rank is 0. Common have a value < 256. Esoteric have value > 256. Set this in idl similar to:
//                            [custom(GUID_PROPERTYPOPULARITY, 32)]
// {08315133-3C6C-11D2-8CED-00C04F8EEA30}
extern "C" const __declspec(selectany) GUID GUID_PROPERTYPOPULARITY = {0x08315133L,0x3C6C,0x11D2, { 0x8C,0xED,0x00,0xC0,0x4F,0x8E,0xEA,0x30 }};

// GUID_PROPERTYITALICS -- the property's tag is to be shown in italics
// {08315134-3C6C-11D2-8CED-00C04F8EEA30}
extern "C" const __declspec(selectany) GUID GUID_PROPERTYITALICS = {0x08315134L,0x3C6C,0x11D2, { 0x8C,0xED,0x00,0xC0,0x4F,0x8E,0xEA,0x30 }};

// GUID_PROPERTYBOLD -- the property's tag is to be shown in bold
// {08315135-3C6C-11D2-8CED-00C04F8EEA30}
extern "C" const __declspec(selectany) GUID GUID_PROPERTYBOLD = {0x08315135L,0x3C6C,0x11D2, { 0x8C,0xED,0x00,0xC0,0x4F,0x8E,0xEA,0x30 }};

// GUID_PROPERTYINDENTSIZE -- the property's tag is to be shown indented by the specified amount. 
//							[custom(GUID_PROPERTYINDENTSIZE, 4)]  // indent property tag by 4. Default is 0.
// {08315136-3C6C-11D2-8CED-00C04F8EEA30}
extern "C" const __declspec(selectany) GUID GUID_PROPERTYINDENTSIZE = {0x08315136L,0x3C6C,0x11D2, { 0x8C,0xED,0x00,0xC0,0x4F,0x8E,0xEA,0x30 }};
