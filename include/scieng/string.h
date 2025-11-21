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

	//*******************
	//sciTo
	//*******************

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

	//*******************
	//fromSci
	//*******************

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
//This class permits sci::stringstream to work when using the dynamic c runtime library.
//Without this there is an error because the numpunct<char_type>::id member variable is
//defined within the runtime, but there is no specialization for our sci::char_t.
//We create that specialisation and make the id member inline, so we can remain header
//only.
namespace std
{
	template <>
	class numpunct<sci::char_t> : public std::locale::facet { // facet for defining numeric punctuation text
	private:
		friend _Tidy_guard<numpunct<sci::char_t>>;
	public:

		__PURE_APPDOMAIN_GLOBAL  static inline locale::id id; // unique facet id - unlike the unspecialized version, this one is not _CRTIMP2_PURE_IMPORT

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

			_Tidy_guard<numpunct<sci::char_t>> _Guard{ this };
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
	
