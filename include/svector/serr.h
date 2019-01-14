#ifndef serr_H
#define serr_H
#include<string>
#ifdef _WIN32
#include<Windows.h>
#endif

//namespace alglib
//{
//	class ap_error;
//}

namespace sci
{
	enum errcategory
	{
		SERR_NC,
		SERR_MEMMAP,
		SERR_PLOT,
		SERR_MINIMISER,
		SERR_VECTOR,
		SERR_DISTRIBUTIONS,
		SERR_ANALYSIS,
		SERR_ALG,
		SERR_TIME,
		SERR_USER,
		SERR_KISSFFT
	};
	//error codes should be selected from this list based on their errcategory

#ifdef _WIN32
	class WindowsError
	{
	public:
		WindowsError();
		WindowsError(DWORD code);
		DWORD getCode() const { return m_code; }
		std::string getMessage() const { return m_message; }
		static std::string GetWindowsErrorMessageFromCode(DWORD code);
	private:
		DWORD m_code;
		std::string m_message;
	};
#endif

	class err
	{
	public:
		err(errcategory category, long code);
		err(errcategory category, long code, const std::string &message);
		//err(const alglib::ap_error &err, long code);
#ifdef _WIN32
		err(errcategory category, const WindowsError &windowsError);
#endif
		errcategory getErrorCategory() const { return m_category; }
		long getErrorCode() const { return m_code; }
		std::string getErrorMessage() const { return m_message; }
		
	private:
		const errcategory m_category;
		const long m_code;
		const std::string m_message;
	};
		

	//void reporterr(std::string error);
	//void reporterr(errcategory category, size_t code, std::string usefulinfo);
	void assertThrow(bool test, const sci::err &err);
	void displayErrorDialogue(const sci::err &err);
}


#endif
