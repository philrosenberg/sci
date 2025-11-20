#ifndef SCISTRING_H
#define SCISTRING_H

#include<string>
#include<sstream>
#include"unicode.h"


//A way to set unicode strings, but change it globally if I want to swap the types
#define sU(x) u8##x

namespace sci
{
	typedef char8_t char_t;
	typedef std::basic_string<sci::char_t> string;
	typedef std::basic_stringstream<sci::char_t> stringstream;
	typedef std::basic_istringstream<sci::char_t> istringstream;
	typedef std::basic_ostringstream<sci::char_t> ostringstream;

	template<class STRING>
	inline STRING sciTo(const sci::string& string)
	{
		return utf8To<STRING>(string);
	}

	//*******************
	//toSci
	//*******************

	inline const sci::string &toSci(const std::u8string& string)
	{
		return string;
	}

	inline sci::string toSci(const std::u16string& string)
	{
		return toUtf8(string);
	}

	inline sci::string toSci(const std::u32string& string)
	{
		return toUtf8(string);
	}
#ifdef _WIN32
	inline sci::string toSci(const std::wstring& string)
	{
		return toUtf8(string);
	}
#else
	inline sci::string toSci(const std::string& string)
	{
		return toUtf8(string);
	}
#endif

	template<class STRING>
	STRING fromSci(const sci::string& string)
	{
		return fromUtf8<STRING>(string);
	}

	//*******************
	//sciFrom
	//*******************
	inline const sci::string& sciFrom(const std::u8string& string)
	{
		return string;
	}

	inline sci::string sciFrom(const std::u16string& string)
	{
		return toUtf8(string);
	}

	inline sci::string sciFrom(const std::u32string& string)
	{
		return toUtf8(string);
	}
#ifdef _WIN32
	inline sci::string sciFrom(const std::wstring& string)
	{
		return toUtf8(string);
	}
#else
	inline sci::string sciFrom(const std::string& string)
	{
		return toUtf8(string);
	}
#endif


	template<class TOSTRING, class FROMSTRING>
	TOSTRING stringConvert(const FROMSTRING& string)
	{
		sci::string intermediate = toSci(string);
		return fromSci(intermediate);
	}
}

#ifdef _WIN32
inline std::wostream& operator << (std::wostream& stream, const sci::string& str)
{
	return static_cast<std::wostream&>(stream << sci::toNativeUnicode(str));
}

inline std::wostream& operator << (std::wostream& stream, const sci::char_t *str)
{
	return static_cast<std::wostream&>(stream << sci::toNativeUnicode(sci::string(str)));
}

inline std::wistream& operator >> (std::wistream& stream, sci::string& str)
{
	std::wstring temp;
	std::wistream& result = static_cast<std::wistream&>(stream >> temp);
	str = sci::fromNativeUnicode<sci::string>(temp);
	return result;
}
#else
inline std::ostream& operator << (std::ostream& stream, const sci::string& str)
{
	return static_cast<std::ostream&>(stream << sci::toUtf8(str));
}

inline std::ostream& operator << (std::ostream& stream, const sci::char_t* str)
{
	return static_cast<std::wostream&>(stream << sci::toUtf8(sci::string(str)));
}

inline std::istream& operator >> (std::istream& stream, sci::string& str)
{
	std::string temp;
	std::istream& result = static_cast<std::istream&>(stream >> temp);
	str = sci::fromUtf8(temp);
	return result;
}
#endif

#endif
	
