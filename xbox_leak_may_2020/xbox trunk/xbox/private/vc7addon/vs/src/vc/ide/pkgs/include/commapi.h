/////////////////////////////////////////////////////////////////////////////
//	COMMAPI.H
//		Common package interfaces
//
//  Interfaces that may be implemented by more than one package
//   should be declared here.  For example, ICommandProvider is
//   an interface that any package who provides dynamic (transient) commands
//   determined at run-time should implement.

#ifndef __COMMAPI_H__
#define __COMMAPI_H__


/////////////////////////////////////////////////////////////////////////////
// ICommandProvider interface
// A package who provides dynamic (transient) commands (commands not determined
//  until run-time) must implement this interface and return it
//  in response to a call to CPackage::GetCommandProvider.

// DevAut1 is an example of a package which implements this, since it
//  provides commands from macros & Add-Ins, which are unknown until run-time

#undef  INTERFACE
#define INTERFACE ICommandProvider

DECLARE_INTERFACE_(ICommandProvider, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// ICommandProvider methods

	// This is called when the shell is persisting the command bars into the
	//  registry, and gives the package a chance to save custom information
	//  alongside the command bar button.
	// This function returns the size of the extra information that needs
	//  to be saved to uniquely identify this command id. If pData is
	//  non-NULL, then the information should be placed into pData,
	//  and pData can be assumed to be large enough to accommodate all the data.
	// The shell calls this function once to ascertain the size of memory block
	//  required, and then again with a memory block of the correct (or larger) size.
	// If the package does not want to save special information for this command
	//  (because it is not transient, for example), then it can return 0, and
	//  the shell will do the default saving of the toolbar.
	STDMETHOD_(WORD, SaveCommand)(WORD nId, LPVOID pData) PURE;

	// Given a block of data previously returned by SaveCommand, this function
	//  converts the block of data to a command id which is valid for this
	//  session of Developer Studio.
	// The shell will be smart enough to know not to call this function for
	//  those commands which the ICommandProvider-implementation did not provide
	//  extra info via SaveCommand.
	STDMETHOD_(WORD, LoadCommand)(LPVOID pData, DWORD dwSize) PURE;

	// This function returns the glyph index and bitmap for a given id and
	//  large/small state.  The shell may call this for commands which the
	//  ICommandProvider-implementation did not provide extra info via
	//  SaveCommand.  For example, if the user right clicks on a command bar,
	//  the shell needs to know if it should enable the menu item to reset
	//  the glyph to the default--thus, it will call this to see if the package
	//  has a default glyph to provide.
	// If the package does not have a default glyph to provide, it may return
	//  NULL, and the shell will deal with it in a mature fashion.
	STDMETHOD_(HBITMAP, GetBitmap)(WORD nId, BOOL bLarge, LPDWORD pnGlyph) PURE;
};

#endif //__COMMAPI_H__
