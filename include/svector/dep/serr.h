#ifndef serr_H
#define serr_H
#include<string>
#include<Windows.h>
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
		SERR_ALG
	};
	//error codes should be selected from this list based on their errcategory

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

	class err
	{
	public:
		err(errcategory category, long code);
		err(errcategory category, long code, const std::string &message);
		err(errcategory category, const WindowsError &windowsError);
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