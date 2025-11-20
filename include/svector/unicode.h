#ifndef SCIUNICODE_H
#define SCIUNICODE_H

#include<string>


//A way to set unicode strings, but change it globally if I want to swap the types
#define sU(x) u8##x

namespace sci
{
	inline void swapEndian(char16_t& character)
	{
		char8_t* c = (char8_t*)&character;
		std::swap(c[0], c[1]);
	}

	inline void swapEndian(char32_t& character)
	{
		char8_t* c = (char8_t*)&character;
		std::swap(c[0], c[3]);
		std::swap(c[1], c[2]);
	}

	typedef char8_t char_t;
	typedef std::basic_string<sci::char_t> string;
	typedef std::basic_stringstream<sci::char_t> stringstream;
	typedef std::basic_istringstream<sci::char_t> istringstream;
	typedef std::basic_ostringstream<sci::char_t> ostringstream;
	const std::u16string bom = u"\uFEFF";
	const std::u32string bom32 = U"\U0000FEFF";

	inline void ensureUtf8()
	{
#ifdef _WIN32
		std::string currentLocale = std::setlocale(LC_ALL, nullptr);
		if (currentLocale.length() < 5 || currentLocale.substr(currentLocale.length() - 5) != ".utf8")
			std::setlocale(LC_ALL, (currentLocale + ".utf8").c_str());
#endif
		//do nothing on linux
	}


	void inline constexpr appendCodepoint(std::u8string& utf8String, uint32_t codePoint)
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

	//*******************
	//utf32To
	//*******************
	template<class STRING>
	inline STRING utf32NativeTo(const std::u32string& string)
	{
		STRING result;
		for (auto& c : string)
			appendCodepoint(result, (uint32_t)c);
		return result;
	}
	template<class STRING>
	inline STRING utf32To(const std::u32string& string)
	{
		STRING result;
		std::u32string bomSwapped = bom32;
		swapEndian(bomSwapped[0]);
		bool nativeEndian = (string[0] != bomSwapped[0]);

		if (nativeEndian)
			return utf32NativeTo<STRING>(string);

		//non native case
		std::u32string copy = string;
		for (auto& c : copy)
			swapEndian(c);
		return utf32NativeTo<STRING>(copy);
	}


	//*******************
	//utf16To
	//*******************
	template<class STRING>
	inline STRING utf16NativeTo(const std::u16string& string)
	{
		STRING result;
		result.reserve(string.length() * 2);
		uint32_t codePoint;

		size_t i = string[0] == bom[0] ? 1 : 0; //skip any BOM
		for (; i < string.length() - 1; ++i) //the -1 is in case the last codepoint is a surrogate pair
		{
			if (uint16_t(string[i]) < uint16_t(0xD800) || uint16_t(string[i]) > uint16_t(0xDFFF))
				codePoint = uint32_t(string[i]);
			else if (uint16_t(string[i + 1]) < uint16_t(0xD800) || uint16_t(string[i + 1]) > uint16_t(0xDFFF))
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

		//assume native unless there is a BOM to say otherwise
		//in theory if we have long strings we can check the higer order 8 bits and most of them
		//will be 0 if this is native. However, this won't work if we are reencoding short strings
		//so I don't do it.
		std::u16string bomSwapped = bom;
		swapEndian(bomSwapped[0]);
		bool nativeEndian = string[0] != bomSwapped[0];

		if (nativeEndian)
			return utf16NativeTo<STRING>(string);

		//non native case
		std::u16string copy = string;
		for (auto& c : copy)
			swapEndian(c);
		return utf16NativeTo<STRING>(copy);
	}


	//*******************
	//utf8To
	//*******************
	template<class STRING>
	STRING utf8To(const std::u8string& string)
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


	//*******************
	//nativeUnicodeTo
	//*******************
#ifdef _WIN32
	template<class STRING>
	STRING nativeUnicodeTo(const std::wstring& string)
	{
		return utf16To<STRING>(std::u16string(string.begin(), string.end()));
	}
#else
	template<class STRING>
	STRING nativeUnicodeTo(const std::string& string)
	{
		return utf8To<STRING>(std::u8string(string.begin(), string.end()));
	}
#endif



	//*******************
	//utf32From
	//*******************
	inline std::u32string utf32From(const std::u8string& string)
	{
		return utf8To<std::u32string>(string);
	}

	inline std::u32string utf32From(const std::u16string& string)
	{
		return utf16To<std::u32string>(string);
	}

	inline const std::u32string &utf32From(const std::u32string& string)
	{
		return string;
	}

#ifdef _WIN32
	inline std::u32string utf32From(const std::wstring& string)
	{
		std::u16string temp(string.begin(), string.end());
		return utf32From(temp);
	}
#else
	inline std::u32string& utf32From(const std::string& string)
	{
		std::u8string temp(string.begin(), string.end());
		return utf32From(temp);
	}
#endif

	//*******************
	//utf16From
	//*******************
	inline std::u16string utf16From(const std::u8string& string)
	{
		return utf8To<std::u16string>(string);
	}

	inline const std::u16string &utf16From(const std::u16string& string)
	{
		return string;
	}

	inline std::u16string utf16From(const std::u32string& string)
	{
		return utf32To<std::u16string>(string);
	}

#ifdef _WIN32
	inline std::u16string utf16From(const std::wstring& string)
	{
		return std::u16string(string.begin(), string.end());
	}
#else
	inline std::u16string& utf16From(const std::string& string)
	{
		std::u8string temp(string.begin(), string.end());
		return utf16From(temp);
	}
#endif

	//*******************
	//utf8From
	//*******************
	inline const std::u8string &utf8From(const std::u8string& string)
	{
		return string;
	}

	inline std::u8string utf8From(const std::u16string& string)
	{
		return utf16To<std::u8string>(string);
	}

	inline std::u8string utf8From(const std::u32string& string)
	{
		return utf32To<std::u8string>(string);
	}

#ifdef _WIN32
	inline std::u8string utf8From(const std::wstring& string)
	{
		std::u16string temp(string.begin(), string.end());
		return utf8From(temp);
	}
#else
	inline std::u8string& utf8From(const std::string& string)
	{
		return std::u8string(string.begin(), string.end());
	}
#endif

	//*******************
	//nativeUnicodeFrom
	//*******************
#ifdef _WIN32
	inline std::wstring nativeUnicodeFrom(const std::u8string& string)
	{
		std::u16string temp = utf16From(string);
		return std::wstring(temp.begin(), temp.end());
	}
	inline std::wstring nativeUnicodeFrom(const std::u16string& string)
	{
		std::u16string temp = utf16From(string);
		return std::wstring(temp.begin(), temp.end());
	}
	inline std::wstring nativeUnicodeFrom(const std::u32string& string)
	{
		std::u16string temp = utf16From(string);
		return std::wstring(temp.begin(), temp.end());
	}
	inline const std::wstring &nativeUnicodeFrom(const std::wstring& string)
	{
		return string;
	}
#else
	inline std::wstring nativeUnicodeFrom(const std::u8string& string)
	{
		std::u8string temp = utf8From(string);
		return std::wstring(temp.begin(), temp.end());
	}
	inline std::wstring nativeUnicodeFrom(const std::u16string& string)
	{
		std::u8string temp = utf8From(string);
		return std::wstring(temp.begin(), temp.end());
	}
	inline std::wstring nativeUnicodeFrom(const std::u32string& string)
	{
		std::u8string temp = utf8From(string);
		return std::wstring(temp.begin(), temp.end());
	}
	inline const std::string& nativeUnicodeFrom(const std::string& string)
	{
		return string;
	}
#endif

	
	//*******************
	//toUtf8
	//*******************

	inline std::u8string toUtf8(const std::u8string& string)
	{
		return utf8From(string);
	}

	inline std::u8string toUtf8(const std::u16string& string)
	{
		return utf8From(string);
	}

	inline std::u8string toUtf8(const std::u32string& string)
	{
		return utf8From(string);
	}
#ifdef _WIN32
	inline std::u8string toUtf8(const std::wstring& string)
	{
		return utf8From(string);
	}
#else
	inline std::u8string toUtf8(const std::string& string)
	{
		return utf8From(string);
	}
#endif


	//*******************
	//toUtf16
	//*******************
	inline std::u16string toUtf16(const std::u8string& string)
	{
		return utf16From(string);
	}

	inline std::u16string toUtf16(const std::u16string& string)
	{
		return utf16From(string);
	}

	inline std::u16string toUtf16(const std::u32string& string)
	{
		return utf16From(string);
	}
#ifdef _WIN32
	inline std::u16string toUtf16(const std::wstring& string)
	{
		return utf16From(string);
	}
#else
	inline std::u16string toUtf16(const std::string& string)
	{
		return utf16From(string);
	}
#endif


	//*******************
	//toUtf32
	//*******************
	inline std::u32string toUtf32(const std::u8string& string)
	{
		return utf32From(string);
	}

	inline std::u32string toUtf32(const std::u16string& string)
	{
		return utf32From(string);
	}

	inline std::u32string toUtf32(const std::u32string& string)
	{
		return utf32From(string);
	}
#ifdef _WIN32
	inline std::u32string toUtf32(const std::wstring& string)
	{
		return utf32From(string);
	}
#else
	inline std::u32string toUtf32(const std::string& string)
	{
		return utf32From(string);
	}
#endif

	//*******************
	//toNativeUnicode
	//*******************
#ifdef _WIN32
	inline std::wstring toNativeUnicode(const std::u8string& string)
	{
		return nativeUnicodeFrom(string);
	}

	inline std::wstring toNativeUnicode(const std::u16string& string)
	{
		return nativeUnicodeFrom(string);
	}

	inline std::wstring toNativeUnicode(const std::u32string& string)
	{
		return nativeUnicodeFrom(string);
	}
	inline std::wstring toNativeUnicode(const std::wstring& string)
	{
		return nativeUnicodeFrom(string);
	}
#else
	inline std::string toNativeUnicode(const std::u8string& string)
	{
		return nativeUnicodeFrom(string);
	}

	inline std::string toNativeUnicode(const std::u16string& string)
	{
		return nativeUnicodeFrom(string);
	}

	inline std::string toNativeUnicode(const std::u32string& string)
	{
		return nativeUnicodeFrom(string);
	}
	inline std::string toNativeUnicode(const std::wstring& string)
	{
		return nativeUnicodeFrom(string);
	}
#endif

	//*******************
	//fromUtf8
	//*******************
	template<class STRING>
	STRING fromUtf8(const std::u8string& string)
	{
		return utf8To<STRING>(string);
	}

	//*******************
	//fromUtf16
	//*******************
	template<class STRING>
	STRING fromUtf16(const std::u16string& string)
	{
		return utf16To<STRING>(string);
	}

	//*******************
	//fromUtf32
	//*******************
	template<class STRING>
	STRING fromUtf32(const std::u32string& string)
	{
		return utf32To<STRING>(string);
	}

	//*******************
	//fromNativeUnicode
	//*******************
#ifdef _WIN32
	template<class STRING>
	STRING fromNativeUnicode(const std::wstring& string)
	{
		return nativeUnicodeTo<STRING>(string);
	}
#else
	template<class STRING>
	STRING fromNativeUnicode(const std::string& string)
	{
		return nativeUnicodeTo<STRING>(string);
	}
#endif
}

#endif
	
