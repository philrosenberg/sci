#define _CRT_SECURE_NO_WARNINGS
#include"serr_internal.h"
#include<assert.h>
#include<wx/wx.h>
#include<sstream>
#include<alg\ap.h>

#ifdef _WIN32
sci::WindowsError::WindowsError()
{
	m_code = ::GetLastError();
	m_message = GetWindowsErrorMessageFromCode(m_code);
}
sci::WindowsError::WindowsError(DWORD code)
{
	m_code = code;
	m_message = GetWindowsErrorMessageFromCode(m_code);
}

std::string sci::WindowsError::GetWindowsErrorMessageFromCode( DWORD code)
{
	//Get the error message, if any.
	if (code == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}
#endif

sci::err::err(errcategory category, long code)
	: m_category(category), m_code(code)
{}
sci::err::err(errcategory category, long code, const std::string &message)
	: m_category(category), m_code(code), m_message(message)
{}
sci::err::err(const alglib::ap_error &err, long code)
	: m_category(SERR_ALG), m_code(code), m_message(err.msg)
{}

sci::err::err(errcategory category, const WindowsError &windowsError)
	: m_category(category), m_code(windowsError.getCode()), m_message(windowsError.getMessage())
{}

void sci::assertThrow(bool test, const sci::err &err)
{
	assert(test);
	if(!test)
		throw err;
}

void sci::displayErrorDialogue(const sci::err &err)
{
	std::stringstream message;
	message << "The following error occurred:\n\n" << err.getErrorMessage() << "\n\nError type: ";
	if (err.getErrorCategory() == SERR_NC)
		message << "NetCdf library\n";
	if (err.getErrorCategory() == SERR_MEMMAP)
		message << "Memory map error\n";
	if (err.getErrorCategory() == SERR_PLOT)
		message << "Plot error\n";
	if (err.getErrorCategory() == SERR_MINIMISER)
		message << "Minimiser error\n";
	if (err.getErrorCategory() == SERR_VECTOR)
		message << "Vector error\n";
	if (err.getErrorCategory() == SERR_DISTRIBUTIONS)
		message << "Distributions error\n";
	if (err.getErrorCategory() == SERR_ANALYSIS)
		message << "Analysis error\n";
	if (err.getErrorCategory() == SERR_ALG)
		message << "ALG library error\n";

	message << "Code: " << err.getErrorCode();

	wxMessageBox(message.str(), "sci::error");
}