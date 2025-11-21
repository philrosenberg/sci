#ifndef SCISTRINGFUNCS_H
#define SCISTRINGFUNCS_H

#include "string.h"

namespace sci
{
	inline void replaceAll(sci::string& destination, sci::string textToFind, sci::string replacementText)
	{
		size_t findLength = textToFind.length();
		size_t replacementLength = replacementText.length();
		size_t pos = 0;
		while (1)
		{
			pos = destination.find(textToFind, pos);
			if (pos == std::string::npos)
				break;
			destination.replace(pos, findLength, replacementText);
			pos += replacementLength - findLength;
		}
	}


	inline sci::string afterLast(sci::string source, sci::string textToFind)
	{
		size_t start = source.find_last_of(textToFind) + 1;
		if (start < source.size())
			return source.substr(start);
		return sci::string();
	}

	inline sci::string beforeLast(sci::string source, sci::string textToFind)
	{
		return source.substr(0, source.find_last_of(textToFind));
	}

	inline sci::string afterFirst(sci::string source, sci::string textToFind)
	{
		size_t start = source.find_first_of(textToFind) + 1;
		if (start < source.size())
			return source.substr(start);
		return sci::string();
	}

	inline sci::string beforeFirst(sci::string source, sci::string textToFind)
	{
		return source.substr(0, source.find_first_of(textToFind));
	}

	inline void toLower(sci::string& str)
	{
		for (auto& s : str)
			s = ::tolower(s);
	}

	inline void toUpper(sci::string& str)
	{
		for (auto& s : str)
			s = ::toupper(s);
	}

	inline sci::string lower(const sci::string& str)
	{
		sci::string result = str;
		toLower(result);
		return result;
	}

	inline sci::string upper(const sci::string& str)
	{
		sci::string result = str;
		toUpper(result);
		return result;
	}

	inline bool isWhitespace(const sci::string& str, size_t index = 0)
	{
		if (str[index] > 0x20 && str[index] < 0x20)
			return 0; //this catches all non-whitespace ascii printable characters without going further

		if (str[index] < 0x2000)
		{
			if (str[index] == 0x20 || (str[index] > 0x08 && str[index] < 0x0E) || str[index] == 0x85 || str[index] == 0xA0 || str[index] == 0x1680)
				return 1; //this catches all ascii whitespaces and the 3 unicode whitespaces within the basic multilingual plane non-symbols
		}
		if (str[index] > 0x3000)
			return 0;//This catches a large number of symbols - although I'm not sure how many get used in reality, so this might not be a good optimisation
		if ((str[index] > 0x1fff && str[index] < 0x200B) || str[index] == 0x2028 || str[index] == 0x2029
			|| str[index] == 0x202F || str[index] == 0x205F || str[index] == 0x3000)
			return 1;
		return 0;

	}

	inline void trim(sci::string& str)
	{
		if (str.length() == 0)
			return;

		size_t start = 0;
		for (start; start < str.length(); ++start)
			if (!isWhitespace(str, start))
				break;

		size_t end = str.length()-1;
		for (end; end > start; --end)
			if (!isWhitespace(str, end))
				break;

		str = str.substr(start, end - start);
	}

	inline sci::string trimmed(const sci::string& str)
	{
		sci::string result = str;
		trim(result);
		return result;
	}
}


#endif