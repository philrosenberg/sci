#ifndef scistringstream_h
#define scistringstream_h


#include"string.h"
#include<sstream>

#ifdef __WXWINDOWS__
#include<wx/string.h>
#endif

//A way to set unicode strings, but change it globally if I want to swap the types
#define sU(x) u##x

namespace sci
{
	typedef std::basic_stringstream<sci::char_t> stringstream;
	typedef std::basic_istringstream<sci::char_t> istringstream;
	typedef std::basic_ostringstream<sci::char_t> ostringstream;
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

		__PURE_APPDOMAIN_GLOBAL static locale::id id; // unique facet id - unlike the unspecialized version, this one is not _CRTIMP2_PURE_IMPORT

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

__PURE_APPDOMAIN_GLOBAL inline std::locale::id std::numpunct<char16_t>::id;


namespace sci
{

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
	
