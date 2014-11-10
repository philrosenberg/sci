#include "../include/svector/sstring.h"

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