#include "string1.h"

class istream2 {
public:

	string contents;
	int size;
	int position;

	istream2(void);
	~istream2(void);

	int peek(void);
	bool get(char &c);
	bool eof(void);
};

class ostream2 {
public:

	string contents;
	int size;
	int position;

	ostream2(void);
	~ostream2(void);

	void put(char c);

	ostream2& operator<<(const char *s);

};

class ifstream2 : public istream2 {
public:

	FILE *input;

	ifstream2(void);
	ifstream2(const char *filename);
	~ifstream2(void);

	void open(const char *filename);
	void read(BYTE *image, int sizeRead);
	int gcount(void);
	void close(void);
};
