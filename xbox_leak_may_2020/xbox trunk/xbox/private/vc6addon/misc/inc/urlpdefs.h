//	File:			URLPDEFS.H
//
//	Description:	Has flag definitions for URLPicker
//

#ifndef	__URLPDEFS_H
#define	__URLPDEFS_H

// Flags for the dwFlags parameter when calling the Execute method
#define	URLP_SHOWTARGETFRAME			0x0001	// show target frame edit control

		// the following 3 are mutually exclusive
#define	URLP_CREATEURLTITLE				0x0002	// use "Create URL" title for dialog
#define	URLP_EDITURLTITLE				0x0004	// use "Edit URL" title for dialog
#define	URLP_CUSTOMTITLE				0x0008	// use custom title specified by user

		// the following 3 are mutually exclusive
#define	URLP_DOCRELATIVEURLTYPE			0x0010	// return doc relative URL
#define	URLP_ROOTRELATIVEURLTYPE		0x0020	// return root relative URL
#define	URLP_ABSOLUTEURLTYPE			0x0040	// return absolute URL

		// make sure that these do not clash with the flag above
		// any of the following two can be specified with a logical 'OR' 
#define	URLP_DISALLOWDOCRELATIVEURLTYPE	0x0080	// do not show "Doc Relative" in URL Type combo box
#define	URLP_DISALLOWROOTRELATIVEURLTYPE	0x0100	// do not show "Root Relative" in URL Type combo box
#define	URLP_DISALLOWABSOLUTEURLTYPE	0x0200	// do not show "Absolute" in URL Type combo box

#endif	//__URLPDEFS_H
