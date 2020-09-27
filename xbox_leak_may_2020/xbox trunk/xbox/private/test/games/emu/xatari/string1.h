#ifndef _STRING1_H
#define _STRING1_H

class string
{
private:
	char * str;
	int len;
	static int num_strings;

public:
	string(const char * s);
	string();
	~string();
	string(const string & st);
	int length() { return len; }
	const char *c_str() const;
	const char letter(int n);
	
	//overloaded operators
	string & operator=(const string & st);
	string & operator=(const char * s);
	string & operator+=(const string & st);
	string & operator+=(const char * s);
	string & operator+=(const char s);

	//friend functions
	friend bool operator==(const string &st1, const string &st2);
	friend bool operator!=(const string &st1, const string &st2);
	friend bool operator<(const string &st1, const string &st2);
	friend bool operator>(const string &st1, const string &st2);
//	friend ostream2 & operator<<(ostream2 & os, const string & st);
};
#endif