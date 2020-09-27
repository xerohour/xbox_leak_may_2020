//Do not include this file. Include <cppsvc.h> instead
// lexclass.h
// These are the token types that the lexer can give back to us
// They're separated out here so that you don't have to bring all the baggage
// of the lexer classes along with you if you want to analyze tokens

#pragma once

enum TOKCLS { // token classes
	tokclsError = 0,

	// all standard language keywords //1-0x17f
	tokclsKeyWordMin = 1,

	// for block start/end that are keywords instead of operators...like
	// Pascal or BASIC for instance.
	tokclsKeyWordOpenBlock = 0x17e,
	tokclsKeyWordCloseBlock = tokclsKeyWordOpenBlock+1,

	tokclsKeyWordMax = tokclsKeyWordCloseBlock+1, //0x180,

	// all language operators  //0x180-0x200
	tokclsOpMin = tokclsKeyWordMax,
	tokclsOpSpecOpenBlock = 0x1fe,
	tokclsOpSpecCloseBlock = tokclsOpSpecOpenBlock+1,
	tokclsOpMax = tokclsOpSpecCloseBlock+1, //0x200,

	// special, hard coded operators that editor keys off of
	tokclsOpSpecMin = 0x200,
	tokclsOpSpecEOL = 0x200,
	tokclsOpSpecLineCmt = 0x201, // automatic skip to eol on this one
	tokclsOpSpecEOS = 0x202,
	tokclsOpSpecMax = 0x210,

	// all identifiers, give ~500 possibilities
	tokclsIdentMin = 0x210,
	tokclsIdentUser = 0x211,	// special idents (user keywords)
	tokclsIdentMax = 0x400,

	// all constants (numeric and string)
	tokclsConstMin = 0x400,
	tokclsConstInteger = 0x400,
	tokclsConstReal = 0x401,
	tokclsConstString = 0x402,
	tokclsStringPart = 0x402,		// partial string ("....)
	tokclsConstMax = 0x410,

	// comments
	tokclsCommentMin = 0x500,
	tokclsCommentPart = 0x500,	// partial comment (/* ...)
	tokclsCommentMax = 0x510,

	// language dependent token class(es) start at 0x800 to 0xfff
	tokclsUserMin = 0x800,
	tokclsUserLast = 0xfff,
	tokclsUserMax = 0x1000,

	// mask to ignore all the bits in a token[class] that the lexer can use
	// for private status.	they will be masked off and ignored by clients
	// of the lexer.  A good use of this feature is to encode the real token
	// type in the lexer private portion (area is ~tokclsMask) when including
	// meta-token types (such as MFC/Wizard user token types) so that other
	// clients of the lexer can keep that information.
	tokclsUserMask = ~(tokclsUserMin - 1),
	tokclsMask = tokclsUserLast,

};

typedef int 	TOKEN;

// Alternate way of looking at a token, editor will only look at tokUser.
// Other clients of the lexer (like the parser or the EE) may want to look
// at the actual token in tokAct.  If any of tokAct is set, then it is expected
// that the actual token is different than the meta token it passed back.
// The status bits are only used by the lexer for whatever it wants.

union TOK_ALT  {
	TOKEN 	tok;
	struct {
        unsigned        tokUser : 12;
        unsigned        tokUserStatus : 4;
		unsigned		tokAct : 12;
		unsigned		tokActStatus : 4;
    };
};

// A text token block indicates the token, and its starting and ending 
// indexes in the line of source just lexed.
// Note that for any N > 0, rgtxtb[N].ibTokMin >= rgtxtb[N-1].ibTokMac.
// if it is such that rgtxtb[N].ibTokMin > rgtxtb[N-1].ibTokMac, then 
// the intervening unclassified characters are treated as white space tokens.
struct TXTB { // Text token class block
	TOKEN 	tok;
	UINT	ibTokMin;		// token length given by ibTokMac - ibTokMin
	UINT	ibTokMac;		// given in bytes
};
