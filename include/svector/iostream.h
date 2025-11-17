#ifndef SCI_IOSTREAM
#define SCI_IOSTREAM
#include<iostream>
namespace sci
{
#ifdef _WIN32
	std::wostream& cout(std::wcout);
	std::wistream &cin(std::wcin);
	std::wostream& cerr(std::wcerr);
	std::wostream& clog(std::wclog);
#else
	std::ostream& cout(std::cout);
	std::istream& cin(std::cin);
	std::ostream& cerr(std::cerr);
	std::ostream& clog(std::clog);
#endif

}
#endif