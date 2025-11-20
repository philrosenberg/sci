#ifndef scinullstream_h
#define scinullstream_h
#include<ostream>
#include<streambuf>

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
}

#endif
	
