#ifndef SCISTRING_H
#define SCISTRING_H


//#include"svector.h"
#include<string>
#include<sstream>


//A way to set unicode strings, but change it globally if I want to swap the types
#define sU(x) u##x

namespace sci
{
	typedef char16_t char_t;
	typedef std::basic_string<sci::char_t> string;
	typedef std::basic_stringstream<sci::char_t> stringstream;
	typedef std::basic_istringstream<sci::char_t> istringstream;
	typedef std::basic_ostringstream<sci::char_t> ostringstream;
	const string bom = sU("\uFEFF");

	

	void inline constexpr appendCodepoint(std::string& utf8String, uint32_t codePoint)
	{
		if (codePoint > uint32_t(0x10FFFF))
		{
			appendCodepoint(utf8String, uint32_t(0xFFFD));
			return;
		}
		if (codePoint < uint32_t(0x0080))
		{
			utf8String.push_back(char(codePoint));
			return;
		}
		if (codePoint < 0x0800)
		{
			utf8String.push_back(char((codePoint >> 6) | uint32_t(0x00C0)));
			utf8String.push_back(char((codePoint & uint32_t(0x003F)) | uint32_t(0x0080)));
			return;
		}
		if (codePoint < 0x010000)
		{
			utf8String.push_back(char((codePoint >> 12) | uint32_t(0x00E0)));
			utf8String.push_back(char(((codePoint >> 6) & uint32_t(0x003F)) | uint32_t(0x0080)));
			utf8String.push_back(char((codePoint & uint32_t(0x003F)) | uint32_t(0x0080)));
			return;
		}

		utf8String.push_back(char((codePoint >> 18) | uint32_t(0x00F0)));
		utf8String.push_back(char(((codePoint >> 12) & uint32_t(0x003F)) | uint32_t(0x0080)));
		utf8String.push_back(char(((codePoint >> 6) & uint32_t(0x003F)) | uint32_t(0x0080)));
		utf8String.push_back(char((codePoint & uint32_t(0x003F)) | uint32_t(0x0080)));
	}

	void inline constexpr appendCodepoint(std::u16string& utf16String, uint32_t codePoint)
	{
		if (codePoint > uint32_t(0x10FFFF))
		{
			utf16String.push_back(char16_t(0xFFFD));
			return;
		}

		if (codePoint < uint32_t(0x10000))
		{
			utf16String.push_back(char16_t(codePoint));
			return;
		}

		codePoint -= uint32_t(0x10000); //now in range 0x00000 0xFFFFF
		utf16String.push_back(char16_t(codePoint >> 10) + char16_t(0xD800));
		utf16String.push_back(char16_t(codePoint & 0x003FF) + char16_t(0xDC00));
	}

#ifdef _WIN32
	void inline constexpr appendCodepoint(std::wstring& utf16String, uint32_t codePoint)
	{
		if (codePoint > uint32_t(0x10FFFF))
		{
			utf16String.push_back(char16_t(0xFFFD));
			return;
		}

		if (codePoint < uint32_t(0x10000))
		{
			utf16String.push_back(char16_t(codePoint));
			return;
		}

		codePoint -= uint32_t(0x10000); //now in range 0x00000 0xFFFFF
		utf16String.push_back(char16_t(codePoint >> 10) + char16_t(0xD800));
		utf16String.push_back(char16_t(codePoint & 0x003FF) + char16_t(0xDC00));
	}
#else
	void inline constexpr appendCodepoint(std::wstring& utf32String, uint32_t codePoint)
	{
		if (codePoint > uint32_t(0x10FFFF))
		{
			utf32String.push_back(char32_t(0xFFFD));
			return;
		}
		utf32String.push_back(char16_t(codePoint));
	}
#endif

	void inline constexpr appendCodepoint(std::u32string& utf32String, uint32_t codePoint)
	{
		if (codePoint > uint32_t(0x10FFFF))
		{
			utf32String.push_back(char32_t(0xFFFD));
			return;
		}
		utf32String.push_back(char16_t(codePoint));
	}


	template<class STRING>
	inline STRING utf16NativeTo(const std::u16string& string)
	{
		STRING result;
		result.reserve(string.length() * 2);
		uint32_t codePoint;

		size_t i = string[0] == 0xfeff ? 1 : 0; //skip any BOM
		for (; i < string.length() - 1; ++i) //the -1 is in case the last codepoint is a surrogate pair
		{
			if (uint16_t(string[i]) < uint16_t(0xD800) || uint16_t(string[i]) > uint16_t(0xDFFF))
				codePoint = uint32_t(string[i]);
			else if (uint16_t(string[i+1]) < uint16_t(0xD800) || uint16_t(string[i+1]) > uint16_t(0xDFFF))
				codePoint = uint32_t(string[i]); //this is not valid UTC-16, as code points in the range 0xD800 to 0xE000 should come in pairs, but some platforms allow it so work with it
			else
			{
				//2 unit codepoint
				codePoint = ((uint32_t(string[i] - uint32_t(0xD800)) << 10) + uint32_t(string[i + 1] - uint32_t(0xDC00))) + uint32_t(0x10000);
				++i;
			}
			appendCodepoint(result, codePoint);
		}

		if (i < string.length()) //deal with the last codepoint if it wasn't a surrogate pair
			appendCodepoint(result, uint32_t(string[i]));
		return result;
	}

	template<class STRING>
	inline STRING utf16To(const std::u16string& string)
	{
		if (string.length() == 0)
			return STRING();

		bool nativeEndian = true; //assume native unless there is a BOM to say otherwise
		//in theory if we have long strings we can check the higer order 8 bits and most of them
		//will be 0 if this is native. However, this won't work if we are reencoding short strings
		//so I don't do it.
		if (string[0] == 0xfffe)
			nativeEndian = false; //a reversed BOM guarentees non-native endian

		if (nativeEndian)
			return utf16NativeTo<STRING>(string);

		//non native case
		std::u16string copy = string;
		for (auto& c : copy)
		{
			char* p = (char*)(&c);
			std::swap(*p, *(p + 1));
		}
		return utf16NativeTo<STRING>(copy);
	}

	inline std::string utf16ToUtf8(const std::u16string& string)
	{
		return utf16To<std::string>(string);
	}

	inline std::u32string utf16ToUtf32(const std::u16string& string)
	{
		return utf16To<std::u32string>(string);
	}

	template<class STRING>
	STRING utf8To(const std::string& string)
	{
		STRING result;
		//result.push_back(char16_t(0xFEFF));//BOM

		result.reserve(string.length() / 2);
		uint32_t codePoint;

		for (size_t i = 0; i < string.length(); ++i)
		{
			if (uint8_t(string[i]) > 0xF4)
				codePoint = 0xFFFD; //error, outside unicode range

			else if (uint8_t(string[i]) < 0x80)
				codePoint = uint32_t(string[i]); //standard ascii character

			else if (uint8_t(string[i]) > uint8_t(0x7F0) && uint8_t(string[i]) < uint8_t(0xC0))
				codePoint = 0xFFFD; //error, continuation byte at the start of a character

			else if (uint8_t(string[i]) > uint8_t(0xEF)) //4 byte sequence
			{
				if (i > string.length() - 4)
					codePoint = 0xFFFD; //error missing bytes
				else if (uint8_t(string[i]) == uint8_t(0xF0) && uint8_t(string[i + 1]) < uint8_t(0x90))
					codePoint = 0xFFFD; //error overlong encoding
				else if (uint8_t(string[i]) == uint8_t(0xF4) && uint8_t(string[i + 1]) > uint8_t(0x8f))
					codePoint = 0xFFFD; //error, outside unicode range
				else if ((uint8_t(string[i + 1]) & 0xC0) != 0x80)
					codePoint = 0xFFFD; //error, non continuation character
				else if ((uint8_t(string[i + 2]) & 0xC0) != 0x80)
				{
					codePoint = 0xFFFD; //error, non continuation character
					++i;
				}
				else if ((uint8_t(string[i + 3]) & 0xC0) != 0x80)
				{
					codePoint = 0xFFFD; //error, non continuation character
					i += 2;
				}
				else
				{
					codePoint = ((uint32_t(string[i]) & uint32_t(0x07)) << 18)
						+ ((uint32_t(string[i + 1]) & uint32_t(0x3F)) << 12)
						+ ((uint32_t(string[i + 2]) & uint32_t(0x3F)) << 6)
						+ (uint32_t(string[i + 3]) & uint32_t(0x3F));
					i += 3;
				}
			}
			else if (uint8_t(string[i]) > uint8_t(0xDF)) //3 byte sequence
			{
				if (i > string.length() - 3)
					codePoint = 0xFFFD; //error missing bytes
				else if (uint8_t(string[i]) == uint8_t(0xE0) && uint8_t(string[i + 1]) < uint8_t(0xA0))
					codePoint = 0xFFFD; //error overlong encoding
				else if ((uint8_t(string[i + 1]) & 0xC0) != 0x80)
					codePoint = 0xFFFD; //error, non continuation character
				else if ((uint8_t(string[i + 2]) & 0xC0) != 0x80)
				{
					codePoint = 0xFFFD; //error, non continuation character
					++i;
				}
				else
				{
					codePoint = ((uint32_t(string[i]) & uint32_t(0x0F)) << 12)
						+ ((uint32_t(string[i + 1]) & uint32_t(0x3F)) << 6)
						+ (uint32_t(string[i + 2]) & uint32_t(0x3F));
					i += 2;
				}
			}
			else if (uint8_t(string[i]) > uint8_t(0xC1)) //2 byte sequence
			{
				if (i > string.length() - 2)
					codePoint = 0xFFFD; //error missing bytes
				else if ((uint8_t(string[i + 1]) & 0xC0) != 0x80)
					codePoint = 0xFFFD; //error, non continuation character
				else
				{
					codePoint = ((uint32_t(string[i]) & uint32_t(0x1F)) << 6)
						+ (uint32_t(string[i + 1]) & uint32_t(0x3F));
					++i;
				}
			}
			else
				codePoint = 0xFFFD; //error overlong encoding

			appendCodepoint(result, codePoint);
		}

		return result;
	}

	inline std::u16string utf8ToUtf16(const std::string& string)
	{
		return utf8To<std::u16string>(string);
	}

	inline std::u32string utf8ToUtf32(const std::string& string)
	{
		return utf8To<std::u32string>(string);
	}

	inline std::string utf32ToUtf8(const std::u32string& string)
	{
		std::string result;
		for (auto& s : string)
			appendCodepoint(result, uint32_t(s));
		return result;
	}

	inline std::u16string utf32ToUtf16(const std::u32string& string)
	{
		std::u16string result;
		for (auto& s : string)
			appendCodepoint(result, uint32_t(s));
		return result;
	}

	inline std::string toUtf8(const sci::string& string)
	{
		return utf16ToUtf8(string);
	}
	inline std::u16string toUtf16(const sci::string& string)
	{
		return string;
	}
	inline std::u32string toUtf32(const sci::string& string)
	{
		return utf16ToUtf32(string);;
	}

	inline sci::string fromUtf8(const std::string& string)
	{
		return utf8ToUtf16(string);
	}

	inline sci::string fromUtf16(const std::u16string& string)
	{
		return string;
	}

	inline sci::string fromUtf32(const std::u32string& string)
	{
		return utf32ToUtf16(string);
	}

	template<class STRING>
	sci::string fromNativeUnicode(const STRING &string)
	{
		if constexpr (sizeof(STRING::basic_string::value_type) == sizeof(sci::string::basic_string::value_type))
			return sci::string(string.begin(), string.end());
		else if (sizeof(STRING::basic_string::value_type) == 1)
			return fromUtf8(string);
		else
			return fromUtf32(string);
	}



#ifdef _WIN32
	//On Windows the native unicode version is UCS-16 represented by std:::wstring
	const inline std::wstring& nativeUnicode(const std::wstring& str)
	{
		return str;
	}
	inline std::wstring& nativeUnicode(std::wstring& str)
	{
		return str;
	}
	inline std::wstring nativeUnicode(const std::u16string& str)
	{
		return std::wstring(str.begin(), str.end());
	}
	inline std::wstring nativeUnicode(const std::u32string& str)
	{
		std::wstring result;
		result.reserve(str.length());
		for (auto& s : str)
			appendCodepoint(result, s);
		return result;
	}
#else
	//On Linux the native unicode version is UTS-8 represented by std:::string
	inline const std::string& nativeUnicode(const std::string& str)
	{
		return str;
	}
	inline std::string& nativeUnicode(std::string& str)
	{
		return str;
	}
	inline std::string nativeUnicode(const std::wstring& str)
	{
		std::string result;
		result.reserve(str.length());
		for (auto& s : str)
			appendCodepoint(result, s);
		return result;
	}
	inline std::string nativeUnicode(const std::u16string& str)
	{
		return utf16ToUtf8(str);
	}
	inline std::string nativeUnicode(const std::u32string& str)
	{
		return utf32ToUtf8(str);
	}
#endif

 
}

#ifdef _WIN32
inline std::wostream& operator << (std::wostream& stream, const sci::string& str)
{
	return static_cast<std::wostream&>(stream << sci::nativeUnicode(str));
}

inline std::wostream& operator << (std::wostream& stream, const sci::char_t *str)
{
	return static_cast<std::wostream&>(stream << sci::nativeUnicode(str));
}

inline std::wistream& operator >> (std::wistream& stream, sci::string& str)
{
	std::wstring temp;
	std::wistream& result = static_cast<std::wistream&>(stream >> temp);
	str = sci::fromNativeUnicode(temp);
	return result;
}
#else
inline std::ostream& operator << (std::ostream& stream, const sci::string& str)
{
	return static_cast<std::ostream&>(stream << sci::nativeUnicode(str));
}

inline std::istream& operator >> (std::istream& stream, sci::string& str)
{
	std::string temp;
	std::istream& result = static_cast<std::istream&>(stream >> temp);
	str = sci::fromNativeUnicode(temp);
	return result;
}
#endif

#endif
	
