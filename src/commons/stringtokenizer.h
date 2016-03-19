/*
* Made to match very closely the StringTokenizer class of Java
* Details at http://docs.oracle.com/javase/7/docs/api/java/util/StringTokenizer.html
*/

#ifndef _STRINGTOKENIZER_H
#define _STRINGTOKENIZER_H

#include <string>
#include <vector>
#include <stdexcept>

class StringTokenizer
{
private:
	std::string str;
	std::string delim;
	bool returnDelims;
	int count;
    unsigned idx;

public:
	StringTokenizer(std::string str, std::string delim = " \t\n\r\f", bool returnDelims = false);
       
    int count_tokens();
	bool hasMoreElements();
	bool hasMoreTokens();
	const char* nextElement();
	std::string nextToken();
};

#endif

