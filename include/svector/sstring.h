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

//A way to set unicode strings, but change it globally if I want to swap the types
#define sU(x) u##x

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

	typedef char16_t char_t;
	typedef std::basic_string<sci::char_t> string;
	typedef std::basic_stringstream<sci::char_t> stringstream;
	typedef std::basic_istringstream<sci::char_t> istringstream;
	typedef std::basic_ostringstream<sci::char_t> ostringstream;
#ifdef _WIN32
	//On Windows the native unicode version is UTS-16 represented by std:::wstring
	const std::wstring &nativeUnicode(const std::wstring &str);
	std::wstring &nativeUnicode(std::wstring &str);
	std::wstring nativeUnicode(const std::u16string &str);
	std::wstring nativeUnicode(const std::u32string &str);
	std::string ucs16ToUtf8(const std::wstring &string);
	std::wstring utf8ToUcs16(const std::string &string);

	std::string nativeCodepage(const std::wstring &str);
	std::string nativeCodepage(const std::u16string &str);
	std::string nativeCodepage(const std::u32string &str);
	std::string nativeCodepage(const std::wstring &str, char replacementCharacter);
	std::string nativeCodepage(const std::u16string &str, char replacementCharacter);
	std::string nativeCodepage(const std::u32string &str, char replacementCharacter);
	std::string nativeCodepage(const std::wstring &str, char replacementCharacter, bool &usedReplacement);
	std::string nativeCodepage(const std::u16string &str, char replacementCharacter, bool &usedReplacement);
	std::string nativeCodepage(const std::u32string &str, char replacementCharacter, bool &usedReplacement);
#else
	//On Linux the native unicode version is UTS-8 represented by std:::string
	const std::string &nativeUnicode(const std::string &str);
	std::string &nativeUnicode(std::string &str);
	std::string nativeUnicode(const std::wstring &str);
	std::string nativeUnicode(const std::u16string &str);
	std::string nativeUnicode(const std::u32string &str);

	//On linux we don't use codepage encoding, it's the same as utf8
	const std::string &nativeCodepage(const std::string &str) { return str; }
	std::string &nativeCodepage(std::string &str) { return str; }
	std::string nativeCodepage(const std::wstring &str) { return nativeUnicode(str); }
	std::string nativeCodepage(const std::u16string &str) { return nativeUnicode(str); }
	std::string nativeCodepage(const std::u32string &str) { return nativeUnicode(str); }

	const std::string &nativeCodepage(const std::string &str, char replacementCharacter) { return str; }
	std::string &nativeCodepage(std::string &str, char replacementCharacter) { return str; }
	std::string nativeCodepage(const std::wstring &str, char replacementCharacter) { return nativeUnicode(str); }
	std::string nativeCodepage(const std::u16string &str, char replacementCharacter) { return nativeUnicode(str); }
	std::string nativeCodepage(const std::u32string &str, char replacementCharacter) { return nativeUnicode(str); }

	const std::string &nativeCodepage(const std::string &str, char replacementCharacter, bool &usedReplacement) { usedReplacement = false; return str; }
	std::string &nativeCodepage(std::string &str, char replacementCharacter, bool &usedReplacement) { usedReplacement = false; return str; }
	std::string nativeCodepage(const std::wstring &str, char replacementCharacter, bool &usedReplacement) { usedReplacement = false; return nativeUnicode(str); }
	std::string nativeCodepage(const std::u16string &str, char replacementCharacter, bool &usedReplacement) { usedReplacement = false; return nativeUnicode(str); }
	std::string nativeCodepage(const std::u32string &str, char replacementCharacter, bool &usedReplacement) { usedReplacement = false; return nativeUnicode(str); }
#endif

	std::string utf16ToUtf8(const std::u16string &string);
	std::string utf32ToUtf8(const std::u32string &string);

	std::u16string utf8ToUtf16(const std::string &string);
	std::u16string utf32ToUtf16(const std::u32string &string);

	std::u32string utf8ToUtf32(const std::string &string);
	std::u32string utf16ToUtf32(const std::u16string &string);

	std::string toUtf8(const sci::string &string);
	std::u16string toUtf16(const sci::string &string);
	std::u32string toUtf32(const sci::string &string);

	sci::string fromUtf8(const std::string &string);
	sci::string fromUtf16(const std::u16string &string);
	sci::string fromUtf32(const std::u32string &string);
}


#endif
	
