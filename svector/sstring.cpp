#include "../include/svector/sstring.h"
#include <algorithm>

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