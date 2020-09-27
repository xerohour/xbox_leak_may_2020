/*
 *
 * optn.hxx
 *
 * Option handling
 *
 */

struct OPTH {
	char *m_szExe;

	OPTH(int &cArg, char **&rgszArg, const char *szOptionChars="-/");

	BOOL FParseOption(char *&sz, char);
	void UsageFailure(void);
};
