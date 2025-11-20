#include"../include/svector/serr.h"
#include<assert.h>
#ifdef __WXWINDOWS__
#include<wx/wx.h>
#endif
#include<sstream>
//#include<alg/ap.h>

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

sci::string sci::WindowsError::GetWindowsErrorMessageFromCode( DWORD code)
{
	//Get the error message, if any.
	if (code == 0)
		return sci::string(); //No error message has been recorded

	LPWSTR messageBuffer = nullptr;
	size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);

	std::wstring message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return sci::fromNativeUnicode<sci::string>(message);
}
#endif

sci::err::err(errcategory category, long code)
	: m_category(category), m_code(code)
{}
sci::err::err(errcategory category, long code, const std::string &message)
	: m_category(category), m_code(code), m_message(sci::fromCodepage<sci::string>(message))
{}
sci::err::err(errcategory category, long code, const sci::string &message)
	: m_category(category), m_code(code), m_message(message)
{}
/*
sci::err::err(const alglib::ap_error &err, long code)
	: m_category(SERR_ALG), m_code(code), m_message(err.msg)
{}*/
#ifdef _WIN32
sci::err::err(errcategory category, const WindowsError &windowsError)
	: m_category(category), m_code(windowsError.getCode()), m_message(windowsError.getMessage())
{}
#endif
void sci::assertThrow(bool test, const sci::err &err)
{
	assert(test);
	if(!test)
		throw err;
}

#ifdef __WXWINDOWS__
void sci::displayErrorDialogue(const sci::err &err)
{
	sci::ostringstream message;
	message << sU("The following error occurred:\n\n") << err.getErrorMessage() << sU("\n\nError type: ");
	if (err.getErrorCategory() == SERR_NC)
		message << sU("NetCdf library\n");
	if (err.getErrorCategory() == SERR_MEMMAP)
		message << sU("Memory map error\n");
	if (err.getErrorCategory() == SERR_PLOT)
		message << sU("Plot error\n");
	if (err.getErrorCategory() == SERR_MINIMISER)
		message << sU("Minimiser error\n");
	if (err.getErrorCategory() == SERR_VECTOR)
		message << sU("Vector error\n");
	if (err.getErrorCategory() == SERR_DISTRIBUTIONS)
		message << sU("Distributions error\n");
	if (err.getErrorCategory() == SERR_ANALYSIS)
		message << sU("Analysis error\n");
	if (err.getErrorCategory() == SERR_ALG)
		message << sU("ALG library error\n");

	message << sU("Code: ") << err.getErrorCode();

	wxMessageBox(nativeUnicode(message.str()), "sci::error");
}
#endif
