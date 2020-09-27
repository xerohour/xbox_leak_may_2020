#include "stdafx.h"

istream2::istream2(void)
{

}

istream2::~istream2(void)
{

}

int istream2::peek(void)
{
	return contents.letter(position);
}

bool istream2::get(char &c)
{
	c = contents.letter(position);
	position++;
	return true;
}

bool istream2::eof(void)
{
	if(position == size)
		return true;
	else
		return false;
}
	
ostream2::ostream2(void)
{
	position = 0;
	size = 0;
}

ostream2::~ostream2(void)
{

}

void ostream2::put(char c)
{
	contents += c;
	size++;
}

ostream2 & ostream2::operator << (const char *s)
{
	contents += s;
	size += strlen(s);
	return *this;
}

ifstream2::ifstream2(void)
{

}

ifstream2::ifstream2(const char *filename)
{
	input = fopen(filename, "r");

	size = 0;
	position = 0;
	char temp;
	while(!feof(input)) 
	{
		temp = getc(input);
		size++;
	}
	rewind(input);

	char *holding;
	holding = new char[size];
	for(int i = 0; i < size; i++) 
	{
		holding[i] = getc(input);
	}
	contents = holding;
	fclose(input);
}

ifstream2::~ifstream2(void)
{

}

void ifstream2::open(const char *filename)
{
	input = fopen(filename, "rb");

	size = 1;
	position = 0;
/*	char temp[1];
	while(!feof(input)) 
	{
		fread(temp, sizeof(BYTE), 1, input);
		size++;
	}
	rewind(input);
*/
}

void ifstream2::read(BYTE *image, int sizeRead)
{
	size = fread(image, sizeof(BYTE), sizeRead, input);
}

int ifstream2::gcount(void)
{
	return size;
}

void ifstream2::close(void)
{
	fclose(input);
}