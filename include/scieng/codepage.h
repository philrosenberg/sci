#ifndef SCICODEPAGE_H
#define SCICODEPAGE_H

#include "string.h"
#include<vector>

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define UNDEFWIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#define UNDEFNOMINMAX
#endif
//#ifndef NONLS
//#define NONLS
//#define UNDEFNONLS
//#endif
#ifndef NOAPISET
#define NOAPISET
#define UNDEFNOAPISET
#endif
#include <Windows.h>
#include<stringapiset.h>
#ifdef UNDEFNOAPISET
#undef NOAPISET
#endif
//#ifdef UNDEFNONLS
//#undef NONLS
//#endif
#ifdef UNDEFNOMINMAX
#undef NOMINMAX
#endif
#ifdef UNDEFWIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

#endif


namespace sci
{
#ifdef _WIN32
	inline UINT getWindowsCodePage()
	{
		char* localeTemp = std::setlocale(LC_ALL, nullptr);
		if (localeTemp == 0)
			return CP_ACP;
		std::wstring locale = toNativeUnicode(std::u8string(localeTemp, localeTemp + std::strlen(localeTemp)));
		if (locale == L"C") //this will be the situation almost every time
			return CP_ACP;

		wchar_t buffer[10];
		GetLocaleInfoEx(locale.c_str(), LOCALE_IDEFAULTANSICODEPAGE, buffer, 10);
		
		UINT result = _wtoi(buffer);
		if (result == 0)
			return CP_ACP;
		return result;
	}
#endif

	//**************************************************
	//codepageTo functions
	//**************************************************
	template <class STRING>
	inline STRING codepageTo(const std::string& string)
	{
#ifdef _WIN32
		if (string.length() == 0)
			return STRING();
		UINT codePage = getWindowsCodePage();
		int nCharsNeeded = MultiByteToWideChar(codePage, 0, string.c_str(), -1, nullptr, 0);

		std::wstring buffer((size_t)nCharsNeeded, 0);
		MultiByteToWideChar(codePage, 0, string.c_str(), -1, &buffer[0], nCharsNeeded);
		return nativeUnicodeTo<STRING>(buffer);
#else
		return utf8To<STRING>(string);
#endif
	}

	//**************************************************
	//codepageFrom functions
	//**************************************************

#ifdef _WIN32
	inline std::string codepageFrom(const std::wstring& str, char replacementCharacter = '?')
	{
		UINT codePage = getWindowsCodePage();
		int nBytesNeeded = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
		std::string buffer((size_t)nBytesNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, NULL);
		return buffer;
	}
#else
	inline std::string codepageFrom(const std::wstring& str, char replacementCharacter = '?')
	{
		std::u8string temp = utf8From(str);
		return std::string(temp.begin(), temp.end());
	}
#endif

	inline std::string codepageFrom(const std::u8string& str, char replacementCharacter = '?')
	{
#ifdef _WIN32
		return codepageFrom(nativeUnicodeFrom(str), replacementCharacter);
#else
		std::u8string temp = utf8From(str);
		return std::string(temp.begin(), temp.end());
#endif
	}

	inline std::string codepageFrom(const std::u16string& str, char replacementCharacter = '?')
	{
#ifdef _WIN32
		return codepageFrom(nativeUnicodeFrom(str), replacementCharacter);
#else
		std::u8string temp = utf8From(str);
		return std::string(temp.begin(), temp.end());
#endif
	}

	inline std::string codepageFrom(const std::u32string& str, char replacementCharacter = '?')
	{
#ifdef _WIN32
		return codepageFrom(nativeUnicodeFrom(str), replacementCharacter);
#else
		std::u8string temp = utf8From(str);
		return std::string(temp.begin(), temp.end());
#endif
	}

	inline const std::string &codepageFrom(const std::string& str, char replacementCharacter = '?')
	{
		return str;
	}

	//*************************************
	//toCodepage functions
	//*************************************
#
	inline std::string toCodepage(const std::wstring& str, char replacementCharacter = '?')
	{
		return codepageFrom(str, replacementCharacter);
	}

	inline std::string toCodepage(const std::u8string& str, char replacementCharacter = '?')
	{
		return codepageFrom(str, replacementCharacter);
	}

	inline std::string toCodepage(const std::u16string& str, char replacementCharacter = '?')
	{
		return codepageFrom(str, replacementCharacter);
	}

	inline std::string toCodepage(const std::u32string& str, char replacementCharacter = '?')
	{
		return codepageFrom(str, replacementCharacter);
	}

	inline const std::string &toCodepage(const std::string& str, char replacementCharacter = '?')
	{
		return str;
	}

	//*************************************
	//fromCodepage functions
	//*************************************
	template<class STRING>
	inline STRING fromCodepage(const std::string& str)
	{
		return codepageTo<STRING>(str);
	}

	//*************************************
	//to_______ functions
	//*************************************

#ifdef _WIN32 //These functions all already exist on non-Windoows for the unicode version
	inline std::u8string toUtf8(const std::string& string)
	{
		return fromCodepage<std::u8string>(string);
	}

	inline std::u16string toUtf16(const std::string& string)
	{
		return fromCodepage<std::u16string>(string);
	}

	inline std::u32string toUtf32(const std::string& string)
	{
		return fromCodepage<std::u32string>(string);
	}

	inline sci::string toSci(const std::string& string)
	{
		return fromCodepage<sci::string>(string);
	}

	inline std::wstring toNativeUnicode(const std::string& string)
	{
		return fromCodepage<std::wstring>(string);
	}
#endif 

	//***************************************
	//from_______ functions
	//***************************************
#ifdef _WIN32 //These functions use the template in unicode.h on non-Windoows
	template<>
	inline std::string fromUtf8<std::string>(const std::u8string& string)
	{
		return toCodepage(string);
	}
	template<>
	inline std::string fromUtf16<std::string>(const std::u16string& string)
	{
		return toCodepage(string);
	}
	template<>
	inline std::string fromUtf32<std::string>(const std::u32string& string)
	{
		return toCodepage(string);
	}
	template<>
	inline std::string fromSci<std::string>(const sci::string& string)
	{
		return toCodepage(string);
	}
	template<>
	inline std::string fromNativeUnicode<std::string>(const std::wstring& string)
	{
		return toCodepage(string);
	}
#endif

}

#endif