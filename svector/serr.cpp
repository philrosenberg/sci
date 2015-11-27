#include"../include/svector/serr.h"

void sci::reporterr(std::string error)
{
}

void sci::reporterr(sci::errcategory category, size_t code, std::string usefulinfo)
{
}

void sci::assertThrow(bool test, const sci::err &err)
{
	assert(test);
	if(!test)
		throw err;
}