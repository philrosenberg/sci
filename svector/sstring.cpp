#include "../include/svector/sstring.h"
#include "../include/svector/serr.h"
#include <algorithm>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <vector>
#include <locale>
#ifdef __WXWINDOWS__
#include <wx/string.h>
#endif

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



//This is a simple class that can be used to inherit from code_cvt
//we do this because on gcc code_cvt has a protected destructor
//so we must inherit from it and provide a public destructor
template<class CODE_CVT>
struct sci_code_cvt : CODE_CVT
{
	template<class ...Args>
	sci_code_cvt(Args&& ...args) : CODE_CVT(std::forward<Args>(args)...) {}
	~sci_code_cvt() {}
};

#ifdef _WIN32
//On Windows the native unicode version is UTF-16 represented by std:::wstring

//We cannot know if a std::string is utf-8 or codepage, so don't attempt
//this conversion.
//std::wstring nativeUnicode(const std::string &str)
//{
//
//}


std::string sci::nativeCodepage(const std::wstring &str)
{
	int nBytesNeeded = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
	std::vector<char>buffer(nBytesNeeded);
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &buffer[0], (int)buffer.size(), NULL, NULL);
	return std::string(&buffer[0]);
}

std::string sci::nativeCodepage(const std::u16string &str)
{
	int nBytesNeeded = WideCharToMultiByte(CP_ACP, 0, reinterpret_cast<const wchar_t *>(str.c_str()), -1, NULL, 0, NULL, NULL);
	std::vector<char>buffer(nBytesNeeded);
	WideCharToMultiByte(CP_ACP, 0, reinterpret_cast<const wchar_t *>(str.c_str()), -1, &buffer[0], (int)buffer.size(), NULL, NULL);
	return std::string(&buffer[0]);
}

std::string sci::nativeCodepage(const std::u32string &str)
{
	std::wstring wideString = sci::nativeUnicode(str);
	int nBytesNeeded = WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, NULL, 0, NULL, NULL);
	std::vector<char>buffer(nBytesNeeded);
	WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, &buffer[0], (int)buffer.size(), NULL, NULL);
	return std::string(&buffer[0]);
}

std::string sci::nativeCodepage(const std::wstring &str, char replacementCharacter)
{
	int nBytesNeeded = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, &replacementCharacter, NULL);
	std::vector<char>buffer(nBytesNeeded);
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, NULL);
	return std::string(&buffer[0]);
}
std::string sci::nativeCodepage(const std::u16string &str, char replacementCharacter)
{
	int nBytesNeeded = WideCharToMultiByte(CP_ACP, 0, reinterpret_cast<const wchar_t *>(str.c_str()), -1, NULL, 0, &replacementCharacter, NULL);
	std::vector<char>buffer(nBytesNeeded);
	WideCharToMultiByte(CP_ACP, 0, reinterpret_cast<const wchar_t *>(str.c_str()), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, NULL);
	return std::string(&buffer[0]);
}
std::string sci::nativeCodepage(const std::u32string &str, char replacementCharacter)
{
	std::wstring wideString = sci::nativeUnicode(str);
	int nBytesNeeded = WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, NULL, 0, &replacementCharacter, NULL);
	std::vector<char>buffer(nBytesNeeded);
	WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, NULL);
	return std::string(&buffer[0]);
}
std::string sci::nativeCodepage(const std::wstring &str, char replacementCharacter, bool &usedReplacement)
{
	BOOL replaced;
	int nBytesNeeded = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, &replacementCharacter, &replaced);
	std::vector<char>buffer(nBytesNeeded);
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, &replaced);
	usedReplacement = replaced > 0;
	return std::string(&buffer[0]);
}
std::string sci::nativeCodepage(const std::u16string &str, char replacementCharacter, bool &usedReplacement)
{
	BOOL replaced;
	int nBytesNeeded = WideCharToMultiByte(CP_ACP, 0, reinterpret_cast<const wchar_t *>(str.c_str()), -1, NULL, 0, &replacementCharacter, &replaced);
	std::vector<char>buffer(nBytesNeeded);
	WideCharToMultiByte(CP_ACP, 0, reinterpret_cast<const wchar_t *>(str.c_str()), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, &replaced);
	usedReplacement = replaced > 0;
	return std::string(&buffer[0]);
}
std::string sci::nativeCodepage(const std::u32string &str, char replacementCharacter, bool &usedReplacement)
{
	BOOL replaced;
	std::wstring wideString = sci::nativeUnicode(str);
	int nBytesNeeded = WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, NULL, 0, &replacementCharacter, &replaced);
	std::vector<char>buffer(nBytesNeeded);
	WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, &replaced);
	usedReplacement = replaced > 0;
	return std::string(&buffer[0]);
}
#else
//On Linux the native unicode version is UTF-8 represented by std:::string

//avoid copying by using references
const std::string &sci::nativeUnicode(const std::string &str)
{
	//no conversion to do, just return the string
	return str;
}
std::string &sci::nativeUnicode(std::string &str)
{
	//no conversion to do, just return the string
	return str;
}
std::string sci::nativeUnicode(const std::wstring &str)
{
	typedef sci_code_cvt< std::codecvt<wchar_t, char, std::mbstate_t>> deletableCvt;
	std::wstring_convert<deletableCvt, wchar_t>converter; // Converter between UTF-8 and UTF-16 wide characters. Windows uses wchar_t for UTF-16.
	return converter.to_bytes(str);
}
std::string sci::nativeUnicode(const std::u16string &str)
{
	typedef sci_code_cvt<std::codecvt<char16_t, char, std::mbstate_t>> deletableCvt;
	std::wstring_convert<deletableCvt, char16_t> converter; // Converter between UTF-8 and UTF-16 wide characters. Windows uses wchar_t for UTF-16.
	return converter.to_bytes(str);
}
std::string sci::nativeUnicode(const std::u32string &str)
{
	typedef sci_code_cvt<std::codecvt<char32_t, char, std::mbstate_t>> deletableCvt;
	std::wstring_convert<deletableCvt, char32_t> converter; // Converter between UTF-8 and UTF-16 wide characters. Windows uses wchar_t for UTF-16.
	return converter.to_bytes(str);
}
#endif

sci::string sci::fromCodepage(const std::string& string)
{
#ifdef _WIN32
	if (string.length() == 0)
		return sci::string();
	size_t nCharsNeeded = MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, nullptr, 0);

	sci::assertThrow(nCharsNeeded > 0, sci::err(sci::SERR_STRING, sci::WindowsError(GetLastError())));

	std::vector<WCHAR> buffer(nCharsNeeded);
	sci::assertThrow(MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, &buffer[0], nCharsNeeded) != 0,
		sci::err(sci::SERR_STRING, sci::WindowsError(GetLastError())));
	std::wstring wideString(&buffer[0]);
	return fromNativeUnicode(wideString);

#else
	return utf8ToUtf16(string);
#endif
}

#ifdef _WIN32
//needed for the definition of the numpunct<char16_t>::id variable
__PURE_APPDOMAIN_GLOBAL std::locale::id std::numpunct<char16_t>::id;
#endif
