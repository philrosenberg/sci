// testString.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../include/svector/sstring.h"
#include<iostream>

int main()
{
    const sci::string test = sU("\uFEFFa\u007F \u0080 \u07ff \u0800 \uFFFF \U00010000 \U0010FFFF");
    const std::string utf8Test = sci::toUtf8(test);
    const std::string utf8Result = "a\x7F \xC2\x80 \xDF\xBF \xE0\xA0\x80 \xEF\xBF\xBF \xF0\x90\x80\x80 \xF4\x8F\xBF\xBF";
    const sci::string utf16Test = sci::fromUtf8(utf8Test);
    if (utf8Test == utf8Result)
    {
        std::cout << "To UTF-8 passed" << std::endl;
        if (test == utf16Test)
            std::cout << "UTF-16 round trip test passed" << std::endl;
        else
            std::cout << "UTF-16 round trip test failed" << std::endl;
    }
    else
        std::cout << "To UTF-8 failed" << std::endl;

}
