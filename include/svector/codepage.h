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
	inline UINT getWindowsCodePage()
	{
		char* localeTemp = std::setlocale(LC_ALL, nullptr);
		if (localeTemp == 0)
			return CP_ACP;
		std::wstring locale = sci::utf8To<std::wstring>(std::string(localeTemp));
		if (locale == L"C") //this will be the situation almost every time
			return CP_ACP;

		wchar_t buffer[10];
		GetLocaleInfoEx(locale.c_str(), LOCALE_IDEFAULTANSICODEPAGE, buffer, 10);
		
		UINT result = _wtoi(buffer);
		if (result == 0)
			return CP_ACP;
		return result;
	}

	inline sci::string fromCodepage(const std::string& string)
	{
#ifdef _WIN32
		if (string.length() == 0)
			return sci::string();
		UINT codePage = getWindowsCodePage();
		size_t nCharsNeeded = MultiByteToWideChar(codePage, 0, string.c_str(), -1, nullptr, 0);

		std::vector<WCHAR> buffer(nCharsNeeded);
		MultiByteToWideChar(codePage, 0, string.c_str(), -1, &buffer[0], nCharsNeeded);
		std::wstring wideString(&buffer[0]);
		return fromNativeUnicode(wideString);
#else
		return utf8ToUtf16(string);
#endif
	}

	inline std::string nativeCodepage(const std::wstring& str)
	{
#ifdef _WIN32
		UINT codePage = getWindowsCodePage();
		int nBytesNeeded = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
		std::vector<char>buffer(nBytesNeeded);
		WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &buffer[0], (int)buffer.size(), NULL, NULL);
		return std::string(&buffer[0]);
#else
		return utf16ToUtf8(string);
#endif
	}

	inline std::string nativeCodepage(const std::u16string& str)
	{
#ifdef _WIN32
		UINT codePage = getWindowsCodePage();
		int nBytesNeeded = WideCharToMultiByte(codePage, 0, reinterpret_cast<const wchar_t*>(str.c_str()), -1, NULL, 0, NULL, NULL);
		std::vector<char>buffer(nBytesNeeded);
		WideCharToMultiByte(codePage, 0, reinterpret_cast<const wchar_t*>(str.c_str()), -1, &buffer[0], (int)buffer.size(), NULL, NULL);
		return std::string(&buffer[0]);
#else
		return utf16ToUtf8(string);
#endif
	}

	inline std::string nativeCodepage(const std::u32string& str)
	{
#ifdef _WIN32
		UINT codePage = getWindowsCodePage();
		std::wstring wideString = sci::nativeUnicode(str);
		int nBytesNeeded = WideCharToMultiByte(codePage, 0, wideString.c_str(), -1, NULL, 0, NULL, NULL);
		std::vector<char>buffer(nBytesNeeded);
		WideCharToMultiByte(codePage, 0, wideString.c_str(), -1, &buffer[0], (int)buffer.size(), NULL, NULL);
		return std::string(&buffer[0]);
#else
		return utf32ToUtf8(string);
#endif
	}

	inline std::string nativeCodepage(const std::wstring& str, char replacementCharacter)
	{
#ifdef _WIN32
		UINT codePage = getWindowsCodePage();
		int nBytesNeeded = WideCharToMultiByte(codePage, 0, str.c_str(), -1, NULL, 0, &replacementCharacter, NULL);
		std::vector<char>buffer(nBytesNeeded);
		WideCharToMultiByte(codePage, 0, str.c_str(), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, NULL);
		return std::string(&buffer[0]);
#else
		return utf16ToUtf8(string);
#endif
	}

	inline std::string nativeCodepage(const std::u16string& str, char replacementCharacter)
	{
#ifdef _WIN32
		UINT codePage = getWindowsCodePage();
		int nBytesNeeded = WideCharToMultiByte(codePage, 0, reinterpret_cast<const wchar_t*>(str.c_str()), -1, NULL, 0, &replacementCharacter, NULL);
		std::vector<char>buffer(nBytesNeeded);
		WideCharToMultiByte(codePage, 0, reinterpret_cast<const wchar_t*>(str.c_str()), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, NULL);
		return std::string(&buffer[0]);
#else
		return utf16ToUtf8(string);
#endif
	}

	inline std::string nativeCodepage(const std::u32string& str, char replacementCharacter)
	{
#ifdef _WIN32
		UINT codePage = getWindowsCodePage();
		std::wstring wideString = sci::nativeUnicode(str);
		int nBytesNeeded = WideCharToMultiByte(codePage, 0, wideString.c_str(), -1, NULL, 0, &replacementCharacter, NULL);
		std::vector<char>buffer(nBytesNeeded);
		WideCharToMultiByte(codePage, 0, wideString.c_str(), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, NULL);
		return std::string(&buffer[0]);
#else
		return utf32ToUtf8(string);
#endif
	}

	inline std::string nativeCodepage(const std::wstring& str, char replacementCharacter, bool& usedReplacement)
	{
#ifdef _WIN32
		UINT codePage = getWindowsCodePage();
		BOOL replaced;
		int nBytesNeeded = WideCharToMultiByte(codePage, 0, str.c_str(), -1, NULL, 0, &replacementCharacter, &replaced);
		std::vector<char>buffer(nBytesNeeded);
		WideCharToMultiByte(codePage, 0, str.c_str(), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, &replaced);
		usedReplacement = replaced > 0;
		return std::string(&buffer[0]);
#else
		usedReplacement = false;
		return utf32ToUtf8(string);
#endif
	}

	inline std::string nativeCodepage(const std::u16string& str, char replacementCharacter, bool& usedReplacement)
	{
#ifdef _WIN32
		UINT codePage = getWindowsCodePage();
		BOOL replaced;
		int nBytesNeeded = WideCharToMultiByte(codePage, 0, reinterpret_cast<const wchar_t*>(str.c_str()), -1, NULL, 0, &replacementCharacter, &replaced);
		std::vector<char>buffer(nBytesNeeded);
		WideCharToMultiByte(codePage, 0, reinterpret_cast<const wchar_t*>(str.c_str()), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, &replaced);
		usedReplacement = replaced > 0;
		return std::string(&buffer[0]);
#else
		usedReplacement = false;
		return utf16ToUtf8(string);
#endif
	}

	inline std::string nativeCodepage(const std::u32string& str, char replacementCharacter, bool& usedReplacement)
	{
#ifdef _WIN32
		UINT codePage = getWindowsCodePage();
		BOOL replaced;
		std::wstring wideString = sci::nativeUnicode(str);
		int nBytesNeeded = WideCharToMultiByte(codePage, 0, wideString.c_str(), -1, NULL, 0, &replacementCharacter, &replaced);
		std::vector<char>buffer(nBytesNeeded);
		WideCharToMultiByte(codePage, 0, wideString.c_str(), -1, &buffer[0], (int)buffer.size(), &replacementCharacter, &replaced);
		usedReplacement = replaced > 0;
		return std::string(&buffer[0]);
#else
		usedReplacement = false;
		return utf32ToUtf8(string);
#endif
	}
}

inline std::ostream& operator << (std::ostream& stream, const sci::string& str)
{
	return static_cast<std::ostream&>(stream << sci::nativeCodepage(str));
}

inline std::istream& operator >> (std::istream& stream, sci::string& str)
{
	std::string temp;
	std::istream& result = static_cast<std::istream&>(stream >> temp);
	str = sci::fromCodepage(temp);
	return result;
}

/*

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

*/

#endif