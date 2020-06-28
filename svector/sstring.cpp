#include "../include/svector/sstring.h"
#include "../include/svector/serr.h"
#include <algorithm>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <vector>
#include <locale>
#include <wx/string.h>

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
//avoid copying by using references
const std::wstring &sci::nativeUnicode(const std::wstring &str)
{
	//no conversion needed
	return str;
}
std::wstring &sci::nativeUnicode(std::wstring &str)
{
	//no conversion needed
	return str;
}

std::wstring sci::nativeUnicode(const std::u16string &str)
{
	//no conversion needed, just copying
	return std::wstring(str.begin(), str.end());
}

std::wstring sci::nativeUnicode(const std::u32string& str)
{
	return sci::nativeUnicode(utf32ToUtf16(str));
}

std::wstring sci::nativeUnicode(const wxString& str)
{
	return str.wc_str(wxMBConvUTF16());
}

wxString& sci::operator<<(wxString& str1, const sci::string& str2)
{
	return str1 << nativeUnicode(str2);
}

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

#ifdef _WIN32
std::string sci::ucs16ToUtf8(const std::wstring &string)
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

std::wstring sci::utf8ToUcs16(const std::string &string)
{
	int nCharsNeeded = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, NULL, 0);
	std::vector<wchar_t> buffer(nCharsNeeded);
	MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, &buffer[0], (int)buffer.size());
	return std::wstring(&buffer[0]);
}
#endif

std::string sci::utf16ToUtf8(const std::u16string &string)
{
#ifdef _WIN32
	//there is a bug in vs 2015 and 17, that means we must use int16_t, not char16_t
	typedef sci_code_cvt<std::codecvt<int16_t, char, std::mbstate_t>> deleteableCvt;
	std::wstring_convert<deleteableCvt, int16_t> converter;
	auto p = reinterpret_cast<const int16_t *>(string.data());
	return converter.to_bytes(p, p + string.length());
#else
	typedef sci_code_cvt<std::codecvt<char16_t, char, std::mbstate_t>> deleteableCvt;
	std::wstring_convert<deleteableCvt, char16_t> converter; // Converter between UTF-8 and UTF-16 wide characters. Windows uses wchar_t for UTF-16.
	return converter.to_bytes(string);
#endif
}

std::string sci::utf32ToUtf8(const std::u32string &string)
{
#ifdef _WIN32
	//there is a bug in vs 2015 and 17, that means we must use int16_t, not char16_t
	typedef sci_code_cvt<std::codecvt<int32_t, char, std::mbstate_t>> deleteableCvt;
	std::wstring_convert<deleteableCvt, int32_t> converter;
	auto p = reinterpret_cast<const int32_t *>(string.data());
	return converter.to_bytes(p, p + string.length());
#else
	typedef sci_code_cvt< std::codecvt<char32_t, char, std::mbstate_t>> deleteableCvt;
	std::wstring_convert<deleteableCvt, char32_t> converter; // Converter between UTF-8 and UTF-16 wide characters. Windows uses wchar_t for UTF-16.
	return converter.to_bytes(string);
#endif
}

std::u16string sci::utf8ToUtf16(const std::string &string)
{
#ifdef _WIN32
	//there is a bug in vs 2015 and 17, that means we must use int16_t, not char16_t
	typedef sci_code_cvt<std::codecvt<int16_t, char, std::mbstate_t>> deleteableCvt;
	std::wstring_convert<deleteableCvt, int16_t> converter;
	std::basic_string<int16_t> resultInt = converter.from_bytes(string);
	auto p = reinterpret_cast<const char16_t *>(resultInt.data());
	return std::u16string(p, p + resultInt.length());
#else
	typedef sci_code_cvt<std::codecvt<char16_t, char, std::mbstate_t>> deleteableCvt;
	std::wstring_convert<deleteableCvt, char16_t> converter; // Converter between UTF-8 and UTF-16 wide characters. Windows uses wchar_t for UTF-16.
	return converter.from_bytes(string);
#endif
}




std::u16string sci::utf32ToUtf16(const std::u32string &string)
{
	std::u16string result;
	result.reserve(string.length()+string.length()/10);//most of the time this will be sufficient as few characters use surrogate pairs, could be a max of 2 times
	for (auto strIt = string.begin(); strIt != string.end(); ++strIt)
	{

		if (*strIt < 0xd800 || (*strIt > 0xdfff && *strIt < 0x10000))
		{
			//basic multilingual plane - just write the value out
			result.push_back(wchar_t(*strIt));
		}
		else if (*strIt > 0xFFFF && *strIt < 0x20000)
		{
			char32_t val = *strIt - 0x10000;
			char32_t highTen = val >> 10;
			char32_t lowTen = val & 0x3ff;
			result.push_back(wchar_t(highTen + 0xd800));
			result.push_back(wchar_t(lowTen + 0xdc00));
		}
		else
		{
			result.push_back(0xfffd); //The character was invalid utf32 so use the unicode replacement character
		}
	}
	return result;
}

std::u32string sci::utf8ToUtf32(const std::string &string)
{
#ifdef _WIN32
	typedef sci_code_cvt<std::codecvt<int32_t, char, std::mbstate_t>> deleteableCvt;
	//there is a bug in vs 2015 and 17, that means we must use int16_t, not char16_t
	std::wstring_convert<deleteableCvt, int32_t> converter;
	std::basic_string<int32_t> resultInt = converter.from_bytes(string);
	auto p = reinterpret_cast<const char32_t *>(resultInt.data());
	return std::u32string(p, p + resultInt.length());
#else
	typedef sci_code_cvt<std::codecvt<char32_t, char, std::mbstate_t>> deleteableCvt;
	std::wstring_convert<deleteableCvt, char32_t> converter; // Converter between UTF-8 and UTF-16 wide characters. Windows uses wchar_t for UTF-16.
	return converter.from_bytes(string);
#endif
}

std::u32string sci::utf16ToUtf32(const std::u16string &string)
{
	std::u32string result;
	result.reserve(string.length());
	for (auto strIt = string.begin(); strIt != string.end(); ++strIt)
	{

		if (*strIt < 0xd800 || *strIt > 0xdfff)
		{
			//basic multilingual plane - just write the value out
			result.push_back(*strIt);
		}
		else if (*strIt < 0xd800)
		{
			//this is the highest ten bits of a surrogate pair
			if (strIt + 1 == string.end() || *(strIt+1) < 0xdc00 || *(strIt+1) > 0xdfff)
			{
				//we only have half a character, or the next character is not a valid low ten bits
				//use the unicode replacement character
				result.push_back(0xfffd);
			}
			else
			{
				result.push_back(((*strIt) - 0xd800) << 10 );
				result.back() |= *(strIt + 1) - 0xdc00;
			}
		}
		else
		{
			//this is the lowest ten bits of a surrogate pair
			if (strIt + 1 == string.end() || *(strIt + 1) < 0xd800 || *(strIt + 1) > 0xdbff)
			{
				//we only have half a character, or the next character is not a valid high ten bits
				//use the unicode replacement character
				result.push_back(0xfffd);
			}
			else
			{
				result.push_back((*(strIt + 1) - 0xd800) << 10);
				result.back() |= (*strIt) - 0xdc00;
			}
		}
	}
	return result;
}

std::string sci::toUtf8(const sci::string &string)
{
	return utf16ToUtf8(string);
}
std::u16string sci::toUtf16(const sci::string &string)
{
	return string;
}
std::u32string sci::toUtf32(const sci::string &string)
{
	return utf16ToUtf32(string);;
}

sci::string sci::fromUtf8(const std::string &string)
{
	return utf8ToUtf16(string);
}
sci::string sci::fromCodepage(const std::string &string)
{
#ifdef _WIN32
	if (string.length() == 0)
		return sci::string();
	size_t nCharsNeeded = MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, nullptr, 0);

	sci::assertThrow(nCharsNeeded > 0, sci::err(sci::SERR_STRING, sci::WindowsError(GetLastError())));

	std::vector<WCHAR> buffer(nCharsNeeded);
	sci::assertThrow(MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, &buffer[0], nCharsNeeded) !=0,
		sci::err(sci::SERR_STRING, sci::WindowsError(GetLastError())));
	std::wstring wideString(&buffer[0]);
	return fromNativeUnicode(wideString);
	
#else
	return utf8ToUtf16(string);
#endif
}
#ifdef _WIN32
sci::string sci::fromNativeUnicode(const std::wstring &string)
{
	static_assert (sizeof(wchar_t) == sizeof(char16_t), "Wide string characters must be 2 bytes for the unicode conversion to work here.");
	std::u16string u16String((char16_t*)string.c_str());
	return fromUtf16(u16String);
}
#else
sci::string sci::fromNativeUnicode(const std::string &string)
{
	return sci::fromUtf8(string);
}
#endif
sci::string sci::fromUtf16(const std::u16string &string)
{
	return string;
}
sci::string sci::fromUtf32(const std::u32string &string)
{
	return utf32ToUtf16(string);
}
sci::string sci::fromWxString(const wxString &string)
{
	return sci::fromUtf8(std::string(string.utf8_str()));
}

