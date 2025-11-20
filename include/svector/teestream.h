#ifndef scistringstream_h
#define scistringstream_h
#include<ostream>
#include<streambuf>

namespace sci
{
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
	
