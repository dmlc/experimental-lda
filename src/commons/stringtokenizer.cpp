#include "stringtokenizer.h"
 
StringTokenizer::StringTokenizer(std::string str, std::string delim, bool returnDelims) : str(str), delim(delim), returnDelims(returnDelims), idx(0), count(UINT_MAX)
{	}

unsigned StringTokenizer::count_tokens()
{
	if (count == UINT_MAX)
	{
		count = 0;
		unsigned delimCount = 0;
		bool prevIsDelim = true;

		for (unsigned i = 0; i < str.length(); ++i)
		{
			if (delim.find_first_of(str[i]) != std::string::npos)
			{
				if (!prevIsDelim)	count++;
				delimCount++;
				prevIsDelim = true;
			}
			else
			{
				prevIsDelim = false;
			}
		}
		if (!prevIsDelim)	count++;
		
		// if counting delmiters add them into the token count
		count = returnDelims ? count + delimCount : count;
	}
	
	return count;
}

bool StringTokenizer::hasMoreElements()
{
	return hasMoreTokens();
}

bool StringTokenizer::hasMoreTokens()
{
	if (!returnDelims)
	{
		//get rid of all delims
		while (delim.find_first_of(str[idx]) != std::string::npos && idx < str.length()){ ++idx; }
	}
	return idx < str.length();
}

std::string StringTokenizer::nextToken()
{
	std::string result;
	//delim becomes a token on its own
	if (returnDelims && idx < str.length())
	{
		result = str.substr(idx, ++idx);
	}
	else
	{
		//get rid of all delims
		while (delim.find_first_of(str[idx]) != std::string::npos && idx < str.length()){
			++idx;
		}

		//if something left in the string
		if (idx < str.length())
		{
			unsigned start = idx;
			while (idx < str.length() && delim.find_first_of(str[idx]) == std::string::npos){
				++idx;
			}
			result = str.substr(start, idx-start);
		}
		else
		{
			throw std::out_of_range("No further tokens!");
		}
	}
	return result;
}

const char* StringTokenizer::nextElement()
{
	std::string a = nextToken();
	return a.c_str();
}

