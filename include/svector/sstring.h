#ifndef sstring_h
#define sstring_h

#ifndef NOMINMAX
#define NOMINMAX
#define MUSTUNDEFNOMINMAX
#endif

//#include"svector.h"
#include<string>
#include<sstream>

#ifdef __WXWINDOWS__
#include<wx/string.h>
#endif

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
		result.push_back(char16_t(0xFEFF));//BOM

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

	sci::string fromCodepage(const std::string& string);

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

	std::string nativeCodepage(const std::wstring& str);
	std::string nativeCodepage(const std::u16string& str);
	std::string nativeCodepage(const std::u32string& str);
	std::string nativeCodepage(const std::wstring& str, char replacementCharacter);
	std::string nativeCodepage(const std::u16string& str, char replacementCharacter);
	std::string nativeCodepage(const std::u32string& str, char replacementCharacter);
	std::string nativeCodepage(const std::wstring& str, char replacementCharacter, bool& usedReplacement);
	std::string nativeCodepage(const std::u16string& str, char replacementCharacter, bool& usedReplacement);
	std::string nativeCodepage(const std::u32string& str, char replacementCharacter, bool& usedReplacement);
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

	//On linux we don't use codepage encoding, it's the same as utf8
	inline const std::string& nativeCodepage(const std::string& str)
	{
		return str;
	}
	std::string& nativeCodepage(std::string& str)
	{
		return str;
	}
	std::string nativeCodepage(const std::wstring& str)
	{
		return nativeUnicode(str);
	}
	std::string nativeCodepage(const std::u16string& str)
	{
		return nativeUnicode(str);
	}
	std::string nativeCodepage(const std::u32string& str)
	{
		return nativeUnicode(str);
	}
#endif
 
}

#ifdef _WIN32

//This class permits sci::stringstream to work when using the dynamic c runtime library.
//Without this there is an error because the ID variable is imported from the runtime and
//it does not exist for the char32_t type

// CLASS TEMPLATE numpunct
namespace std
{
	template <>
	class numpunct<char16_t> : public std::locale::facet { // facet for defining numeric punctuation text
	private:
		friend _Tidy_guard<numpunct<char16_t>>;
	public:

		__PURE_APPDOMAIN_GLOBAL  static locale::id id; // unique facet id - unlike the unspecialized version, this one is not _CRTIMP2_PURE_IMPORT

		sci::char_t decimal_point() const {
			return do_decimal_point();
		}

		sci::char_t thousands_sep() const {
			return do_thousands_sep();
		}

		std::string grouping() const {
			return do_grouping();
		}

		sci::string falsename() const {
			return do_falsename();
		}

		sci::string truename() const {
			return do_truename();
		}

		explicit numpunct(size_t _Refs = 0) : locale::facet(_Refs) { // construct from current locale
			_BEGIN_LOCINFO(_Lobj)
				_Init(_Lobj);
			if (_Kseparator == 0) {
				_Kseparator = // NB: differs from "C" locale
					_Maklocchr(',', static_cast<sci::char_t*>(nullptr), _Lobj._Getcvt());
			}
			_END_LOCINFO()
		}

		numpunct(const _Locinfo& _Lobj, size_t _Refs = 0, bool _Isdef = false) : locale::facet(_Refs) {
			_Init(_Lobj, _Isdef);
		}

		static size_t _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
			// return locale category mask and construct standard facet
			if (_Ppf && !*_Ppf) {
				*_Ppf = new numpunct<sci::char_t>(_Locinfo(_Ploc->name().c_str()), 0, false);
			}
			return _X_NUMERIC;
		}

	protected:
		virtual __CLR_OR_THIS_CALL ~numpunct() noexcept {
			_Tidy();
		}

		numpunct(const char* _Locname, size_t _Refs = 0, bool _Isdef = false) : locale::facet(_Refs) {
			_BEGIN_LOCINFO(_Lobj(_Locname))
				_Init(_Lobj, _Isdef);
			_END_LOCINFO()
		}

		template <class _Elem2>
		void _Getvals(_Elem2, const lconv* _Ptr, _Locinfo::_Cvtvec _Cvt) { // get values
			_Dp = _Maklocchr(_Ptr->decimal_point[0], static_cast<_Elem2*>(nullptr), _Cvt);
			_Kseparator = _Maklocchr(_Ptr->thousands_sep[0], static_cast<_Elem2*>(nullptr), _Cvt);
		}

		void _Getvals(wchar_t, const lconv* _Ptr, _Locinfo::_Cvtvec) { // get values
			_Dp = static_cast<sci::char_t>(_Ptr->_W_decimal_point[0]);
			_Kseparator = static_cast<sci::char_t>(_Ptr->_W_thousands_sep[0]);
		}

		void _Init(const _Locinfo& _Lobj, bool _Isdef = false) { // initialize from _Lobj
			const lconv* _Ptr = _Lobj._Getlconv();
			_Locinfo::_Cvtvec _Cvt = _Lobj._Getcvt(); // conversion information

			_Grouping = nullptr;
			_Falsename = nullptr;
			_Truename = nullptr;

			_Tidy_guard<numpunct<char16_t>> _Guard{ this };
			_Grouping = _Maklocstr(_Isdef ? "" : _Ptr->grouping, static_cast<char*>(nullptr), _Lobj._Getcvt());
			_Falsename = _Maklocstr(_Lobj._Getfalse(), static_cast<sci::char_t*>(nullptr), _Cvt);
			_Truename = _Maklocstr(_Lobj._Gettrue(), static_cast<sci::char_t*>(nullptr), _Cvt);
			_Guard._Target = nullptr;

			if (_Isdef) { // apply defaults for required facets
				// _Grouping = _Maklocstr("", static_cast<char *>(nullptr), _Cvt);
				_Dp = _Maklocchr('.', static_cast<sci::char_t*>(nullptr), _Cvt);
				_Kseparator = _Maklocchr(',', static_cast<sci::char_t*>(nullptr), _Cvt);
			}
			else {
				_Getvals(sci::char_t{}, _Ptr, _Cvt);
			}
		}

		virtual sci::char_t __CLR_OR_THIS_CALL do_decimal_point() const {
			return _Dp;
		}

		virtual sci::char_t __CLR_OR_THIS_CALL do_thousands_sep() const {
			return _Kseparator;
		}

		virtual string __CLR_OR_THIS_CALL do_grouping() const {
			return string(_Grouping);
		}

		virtual sci::string __CLR_OR_THIS_CALL do_falsename() const {
			return sci::string(_Falsename);
		}

		virtual sci::string __CLR_OR_THIS_CALL do_truename() const {
			return sci::string(_Truename);
		}

	private:
		void _Tidy() noexcept { // free all storage
			_CSTD free(const_cast<char*>(_Grouping));
			_CSTD free(const_cast<sci::char_t*>(_Falsename));
			_CSTD free(const_cast<sci::char_t*>(_Truename));
		}

		const char* _Grouping; // grouping string, "" for "C" locale
		sci::char_t _Dp; // decimal point, '.' for "C" locale
		sci::char_t _Kseparator; // thousands separator, '\0' for "C" locale
		const sci::char_t* _Falsename; // name for false, "false" for "C" locale
		const sci::char_t* _Truename; // name for true, "true" for "C" locale
	};
}

namespace sci
{



	inline std::ostream& operator << (std::ostream& stream, const sci::string& str)
	{
		return static_cast<std::ostream&>(stream << sci::nativeCodepage(str));
	}

	template<class _Elem, class _Traits>
	class BasicNullBuf : public std::basic_streambuf<_Elem, _Traits>
	{
	private:
		using int_type = typename _Traits::int_type;
	public:
		BasicNullBuf()
		{}
	protected:
		virtual int_type overflow(int_type ch = _Traits::eof())
		{
			return _Traits::not_eof(ch);// return anything except _Traits::eof() for success
		}
	};
	using NullBuf = BasicNullBuf<char_t, std::char_traits<char_t>>;

	template<class _Elem, class _Traits>
	class BasicNullStream : public std::basic_ostream<_Elem, _Traits>
	{
	public:
		BasicNullStream()
			: std::basic_ostream<_Elem, _Traits>(&m_basicNullBuf)
		{}
	private:
		BasicNullBuf<_Elem, _Traits> m_basicNullBuf;
	};
	using NullStream = BasicNullStream<char_t, std::char_traits<char_t>>;
	using NullStreamCh = BasicNullStream<char, std::char_traits<char>>;

	static NullStream nullout;
	static NullStreamCh nulloutch;

	//check out basic_stringbuf for an example of this type of class
	template<class STREAM1, class STREAM2, class _Elem, class _Traits>
	class BasicTeeStreamBuf : public std::basic_streambuf<_Elem, _Traits>
	{
	public:
		BasicTeeStreamBuf(STREAM1* stream1, STREAM2* stream2)
			:m_stream1(stream1), m_stream2(stream2)
		{}
		STREAM1* getStream1() const
		{
			return m_stream1;
		}
		STREAM2* getStream2() const
		{
			return m_stream2;
		}
		void setStream1(STREAM1 * stream)
		{
			m_stream1 = stream;
		}
		void setStream2(STREAM2* stream)
		{
			m_stream2 = stream;
		}
		using int_type = typename _Traits::int_type;
		using char_type = typename _Traits::char_type;
	protected:
		virtual int_type overflow(int_type ch = _Traits::eof())
		{
			char_type character = _Traits::to_char_type(ch);
			std::basic_string<char_type> str(1, ch);
			if (m_stream1)
				(*m_stream1) << str;
			if (m_stream2)
				(*m_stream2) << str;
			return _Traits::not_eof(ch);// return anything except _Traits::eof() for success
		}
	private:
		STREAM1* m_stream1;
		STREAM2* m_stream2;
	};
	template<class STREAM1, class STREAM2>
	using TeeStreamBuf = BasicTeeStreamBuf<STREAM1, STREAM2, char_t, std::char_traits<char_t>>;

	template<class STREAM1, class STREAM2, class _Elem, class _Traits>
	class BasicOteestream : public std::basic_ostream<_Elem, _Traits>
	{
	public:
		BasicOteestream(STREAM1* stream1, STREAM2* stream2)
			: m_basicTeeStreamBuf(stream1, stream2), std::basic_ostream<_Elem, _Traits>(&m_basicTeeStreamBuf)
		{}
		STREAM1* getStream1() const
		{
			return m_basicTeeStreamBuf.getStream1();
		}
		STREAM2* getStream2() const
		{
			return m_basicTeeStreamBuf.getStream2();
		}
		void setStream1(STREAM1* stream)
		{
			m_basicTeeStreamBuf.setStream1(stream);
		}
		void setStream2(STREAM2* stream)
		{
			m_basicTeeStreamBuf.setStream2(stream);
		}
	private:
		BasicTeeStreamBuf<STREAM1, STREAM2, _Elem, _Traits> m_basicTeeStreamBuf;
	};
	template<class STREAM1, class STREAM2>
	using Oteestream = BasicOteestream<STREAM1, STREAM2, char_t, std::char_traits<char_t>>;
}

#endif

#ifdef MUSTUNDEFNOMINMAX
#undef NOMINMAX
#endif

#endif
	
