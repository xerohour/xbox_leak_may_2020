#include "stdafx.h"

int string::num_strings = 0;

string::string(const char * s)
{
	len = strlen(s);
	str = new char[len + 1];
	strcpy(str, s);
	num_strings++;
}

string::string()
{
	static const char * s = "";
	len = strlen(s);
	str = new char[len + 1];
	strcpy(str, s);
	num_strings++;
}

string::~string()
{
	--num_strings;
	delete [] str;
}

string::string(const string & st)
{
	num_strings++;
	len = st.len;
	str = new char [len + 1];
	strcpy(str, st.str);
}

const char* string::c_str(void) const
{
	return (const char *) str;
}

const char string::letter(int n)
{
	return (const char) str[n];
}

string & string::operator=(const string & st)
{
	if (this == &st)
		return * this;
	delete [] str;
	len = st.len;
	str = new char[len + 1];
	strcpy(str, st.str);
	return *this;
}

string & string::operator=(const char * s)
{
	delete [] str;
	len = strlen(s);
	str = new char[len + 1];
	strcpy(str, s);
	return *this;
}

string & string::operator+=(const string & st)
{
	string tempstr = str;
	int templen = len;

	delete [] str;
	len = st.len + templen;
	str = new char[len + 1];
	for(int i = 0; i < len; i++)
		str[i] = '\0';
	strcat(str, tempstr.str);
	strcat(str, st.str);
	return *this;
}

string & string::operator+=(const char * s)
{
	string tempstr = str;
	int templen = len;

	delete [] str;
	len = strlen(s) + templen;
	str = new char[len + 1];
	for(int i = 0; i < len; i++)
		str[i] = '\0';
	strcat(str, tempstr.str);
	strcat(str, s);
	return *this;
}

string & string::operator+=(const char s)
{
	string tempstr = str;
	int templen = len;

	delete [] str;
	len = 1 + templen;
	str = new char[len + 1];
	for(int i = 0; i < len; i++)
		str[i] = '\0';
	for(i = 0; i < templen; i++)
		str[i] = tempstr.str[i];
	str[templen] = s;
	str[templen + 1] = '\0';
	return *this;
}

bool operator==(const string &st1, const string &st2)
{
	if (strcmp(st1.str, st2.str) == 0)
		return true;
	else
		return false;
}

bool operator!=(const string &st1, const string &st2)
{
	if (strcmp(st1.str, st2.str) == 0)
		return false;
	else
		return true;
}

bool operator>(const string &st1, const string &st2)
{
	if (strcmp(st1.str, st2.str) > 0)
		return true;
	else
		return false;
}

bool operator<(const string &st1, const string &st2)
{
	if (strcmp(st1.str, st2.str) < 0)
		return true;
	else
		return false;
}

/*
ostream2 & operator<<(ostream2 & os, const string & st)
{
	os << st.str;
	return os;
}
*/
