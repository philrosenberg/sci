#ifndef serr_H
#define serr_H

#include<string>
#include<assert.h>
namespace sci
{
	enum errcategory
	{
		SERR_NC
	};
	//error codes should be selected from this list based on their errcategory
	class err
	{
	};
		

	void reporterr(std::string error);
	void reporterr(errcategory category, size_t code, std::string usefulinfo);
	void assertThrow(bool test, const sci::err &err);
}


#endif