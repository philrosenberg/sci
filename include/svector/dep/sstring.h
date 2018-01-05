#ifndef sstring_h
#define sstring_h

//#include"svector.h"
#include<string>
#include<sstream>

template <class T>
std::string& operator << (std::string &lhs, const T &rhs)
{
	std::ostringstream stream;
	stream << rhs;
	lhs=lhs+stream.str();
	return lhs;
}

template <class T>
std::string operator >> (std::string &lhs, T &rhs)
{
	std::istringstream stream(lhs);
	stream >> rhs;
	return lhs.substr((std::string::size_type)stream.gcount());
}

namespace sci
{
	void replaceAll(std::string &destination, std::string textToFind, std::string replacementText);
	std::string afterLast(std::string source, std::string textToFind);
	std::string beforeLast(std::string source, std::string textToFind);
	std::string afterFirst(std::string source, std::string textToFind);
	std::string beforeFirst(std::string source, std::string textToFind);
	std::string lower( const std::string &str );
	std::string upper( const std::string &str );
	void toLower( std::string &str );
	void toUpper( std::string &str );
	std::string trimmed( const std::string &str );
	void trim( std::string &str );

	void replaceAll(std::wstring &destination, std::wstring textToFind, std::wstring replacementText);
	std::wstring afterLast(std::wstring source, std::wstring textToFind);
	std::wstring beforeLast(std::wstring source, std::wstring textToFind);
	std::wstring afterFirst(std::wstring source, std::wstring textToFind);
	std::wstring beforeFirst(std::wstring source, std::wstring textToFind);
	std::wstring lower(const std::wstring &str);
	std::wstring upper(const std::wstring &str);
	void toLower(std::wstring &str);
	void toUpper(std::wstring &str);
	std::wstring trimmed(const std::wstring &str);
	void trim(std::wstring &str);

	std::string utf16To8(const std::wstring &string);
	std::wstring utf8To16(const std::string &string);
}


#endif
	