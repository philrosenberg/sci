#include "sstring_internal.h"
#include <algorithm>
#include <Windows.h>
#include <vector>

void sci::replaceAll(std::string &destination, std::string textToFind, std::string replacementText)
{
	size_t findLength=textToFind.length();
	size_t replacementLength=replacementText.length();
	size_t pos=0;
	while(1)
	{
		pos=destination.find(textToFind,pos);
		if(pos==std::string::npos)
			break;
		destination.replace(pos,findLength,replacementText);
		pos+=replacementLength-findLength;
	}
}


std::string sci::afterLast(std::string source, std::string textToFind)
{
	size_t start=source.find_last_of(textToFind)+1;
	if(start<source.size())
		return source.substr(start);
	return std::string();
}

std::string sci::beforeLast(std::string source, std::string textToFind)
{
	return source.substr(0,source.find_last_of(textToFind));
}
std::string sci::afterFirst(std::string source, std::string textToFind)
{
	size_t start=source.find_first_of(textToFind)+1;
	if(start<source.size())
		return source.substr(start);
	return std::string();
}
std::string sci::beforeFirst(std::string source, std::string textToFind)
{
	return source.substr(0,source.find_first_of(textToFind));
}


std::string sci::lower( const std::string &str )
{
	std::string result=str;
	sci::toLower( result );
	return result;
}
std::string sci::upper( const std::string &str )
{
	std::string result=str;
	sci::toUpper( result );
	return result;
}
void sci::toLower( std::string &str )
{
	if( str.length() > 0 )
		std::transform(&str[0], &str[str.length()-1], &str[0], ::tolower);
}
void sci::toUpper( std::string &str )
{
	if( str.length() > 0 )
		std::transform(&str[0], &str[str.length()-1], &str[0], ::toupper);
}

std::string sci::trimmed( const std::string &str )
{
	std::string result = str;
	sci::trim( result );
	return result;
}

void sci::trim( std::string &str )
{
	std::string whitespaces=" \t\n\r";
	size_t start=str.find_first_not_of(whitespaces);
	if( start > str.length() )
		str = "";
	else
	{
		size_t end=str.find_last_not_of(whitespaces);
		str=str.substr( start, end - start + 1 );
	}
}

//*************wstring versions************************

void sci::replaceAll(std::wstring &destination, std::wstring textToFind, std::wstring replacementText)
{
	size_t findLength = textToFind.length();
	size_t replacementLength = replacementText.length();
	size_t pos = 0;
	while (1)
	{
		pos = destination.find(textToFind, pos);
		if (pos == std::wstring::npos)
			break;
		destination.replace(pos, findLength, replacementText);
		pos += replacementLength - findLength;
	}
}


std::wstring sci::afterLast(std::wstring source, std::wstring textToFind)
{
	size_t start = source.find_last_of(textToFind) + 1;
	if (start<source.size())
		return source.substr(start);
	return std::wstring();
}

std::wstring sci::beforeLast(std::wstring source, std::wstring textToFind)
{
	return source.substr(0, source.find_last_of(textToFind));
}
std::wstring sci::afterFirst(std::wstring source, std::wstring textToFind)
{
	size_t start = source.find_first_of(textToFind) + 1;
	if (start<source.size())
		return source.substr(start);
	return std::wstring();
}
std::wstring sci::beforeFirst(std::wstring source, std::wstring textToFind)
{
	return source.substr(0, source.find_first_of(textToFind));
}


std::wstring sci::lower(const std::wstring &str)
{
	std::wstring result = str;
	sci::toLower(result);
	return result;
}
std::wstring sci::upper(const std::wstring &str)
{
	std::wstring result = str;
	sci::toUpper(result);
	return result;
}
void sci::toLower(std::wstring &str)
{
	if (str.length() > 0)
		std::transform(&str[0], &str[str.length() - 1], &str[0], ::tolower);
}
void sci::toUpper(std::wstring &str)
{
	if (str.length() > 0)
		std::transform(&str[0], &str[str.length() - 1], &str[0], ::toupper);
}

std::wstring sci::trimmed(const std::wstring &str)
{
	std::wstring result = str;
	sci::trim(result);
	return result;
}

void sci::trim(std::wstring &str)
{
	std::wstring whitespaces = L" \t\n\r";
	size_t start = str.find_first_not_of(whitespaces);
	if (start > str.length())
		str = L"";
	else
	{
		size_t end = str.find_last_not_of(whitespaces);
		str = str.substr(start, end - start + 1);
	}
}

std::string sci::utf16To8(const std::wstring &string)
{
	//I'm never sure if it is safe to do
	//std::string str; str.resize(somelength); memcpy(str[0], source, somelength);
	//but I know I can do that with a vector, so we'll use a vector as our buffer
	//then copy that buffer to our string using its constructor

	//use -1 for the length of the wide string in this call - this means the null terminating
	//character will be checked for and included.
	int nBytesNeeded = WideCharToMultiByte(CP_UTF8, 0, string.c_str(), -1, NULL, 0, NULL, NULL);
	std::vector<char>buffer(nBytesNeeded);
	WideCharToMultiByte(CP_UTF8, 0, string.c_str(), -1, &buffer[0], (int)buffer.size(), NULL, NULL);
	return std::string(&buffer[0]);
}

std::wstring sci::utf8To16(const std::string &string)
{
	int nCharsNeeded = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, NULL, 0);
	std::vector<wchar_t> buffer(nCharsNeeded);
	MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, &buffer[0], (int)buffer.size());
	return std::wstring(&buffer[0]);
}