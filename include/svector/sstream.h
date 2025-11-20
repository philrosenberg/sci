#ifndef scistringstream_h
#define scistringstream_h


#include"string.h"
#include<sstream>

namespace sci
{
	typedef std::basic_stringstream<sci::char_t> stringstream;
	typedef std::basic_istringstream<sci::char_t> istringstream;
	typedef std::basic_ostringstream<sci::char_t> ostringstream;
}
#endif
	
