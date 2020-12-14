#ifndef sstring_h
#define sstring_h

#ifndef NOMINMAX
#define NOMINMAX
#define MUSTUNDEFNOMINMAX
#endif

//#include"svector.h"
#include<string>
#include<sstream>

class wxString;

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
	//On Windows the native unicode version is UCS-16 represented by std:::wstring
	const std::wstring &nativeUnicode(const std::wstring &str);
	std::wstring &nativeUnicode(std::wstring &str);
	std::wstring nativeUnicode(const std::u16string &str);
	std::wstring nativeUnicode(const std::u32string &str);
	std::wstring nativeUnicode(const wxString& str);
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

	wxString& operator<<(wxString& str1, const sci::string& str2);
#else
	//On Linux the native unicode version is UTS-8 represented by std:::string
	const std::string &nativeUnicode(const std::string &str);
	std::string &nativeUnicode(std::string &str);
	std::string nativeUnicode(const std::wstring &str);
	std::string nativeUnicode(const std::u16string &str);
	std::string nativeUnicode(const std::u32string &str);
	std::string nativeUnicode(const wxString& str);

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
	sci::string fromCodepage(const std::string &string);
#ifdef _WIN32
	sci::string fromNativeUnicode(const std::wstring &string);
#else
	sci::string fromNativeUnicode(const std::string &string);
#endif
	sci::string fromUtf16(const std::u16string &string);
	sci::string fromUtf32(const std::u32string &string);
	sci::string fromWxString(const wxString &string);
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
				*_Ppf = new numpunct<sci::char_t>(_Locinfo(_Ploc->c_str()), 0, false);
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

#endif

#ifdef MUSTUNDEFNOMINMAX
#undef NOMINMAX
#endif

#endif
	
