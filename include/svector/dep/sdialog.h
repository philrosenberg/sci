#ifndef sdialog_h
#define sdialog_h

#include<wx/filedlg.h>
#include<wx/textctrl.h>
#include<svector/NumberFromTextValidator.h>

wxArrayString wxMultipleFileSelector(const wxString &message, const wxString &default_path=wxT(""), const wxString &default_filename=wxT(""), const wxString &wildcard=wxT("*.*"), int flags=0, wxWindow *parent=NULL, int x=-1,int y=-1);

namespace sci
{
	bool getTextFromUser(std::string &result, const std::string &message, const std::string &title, const std::string &defaultValue);
	bool getRealFromUser(double &result, const std::string &message, const std::string &title, double defaultValue);
	bool getIntegerFromUser(long &result, const std::string &message, const std::string &title, long defaultValue);

	template<class T>
	class NumberTextCtrl : public wxTextCtrl
	{
	public:
		NumberTextCtrl(wxWindow *parent, wxWindowID id, wxString outputFormat, T *valPtr, const wxString &value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxTextCtrlNameStr);
		NumberTextCtrl(wxWindow *parent, wxWindowID id, wxString outputFormat, T minVal, T maxVal, T *valPtr, const wxString &value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxTextCtrlNameStr);
		virtual ~NumberTextCtrl() {}
	};

	template<class T>
	NumberTextCtrl<T>::NumberTextCtrl(wxWindow *parent, wxWindowID id, wxString outputFormat, T *valPtr, const wxString &value, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
		:wxTextCtrl(parent, id, value, pos, size, style, NumberFromTextValidator<T>(outputFormat, valPtr), name)
	{
	}

	template<class T>
	NumberTextCtrl<T>::NumberTextCtrl(wxWindow *parent, wxWindowID id, wxString outputFormat, T minVal, T maxVal, T *valPtr, const wxString &value, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
		: wxTextCtrl(parent, id, value, pos, size, style, NumberFromTextValidator<T>(outputFormat, minVal, maxVal, valPtr), name)
	{
	}

	template<class T>
	class DecimalNumberTextCtrl : public NumberTextCtrl<T>
	{
	public:
		DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, T *valPtr, const wxString &value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxTextCtrlNameStr);
		DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, T minVal, T maxVal, T *valPtr, const wxString &value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxTextCtrlNameStr);
		virtual ~DecimalNumberTextCtrl() {}
	};

	template<class T>
	DecimalNumberTextCtrl<T>::DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, T *valPtr, const wxString &value, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
		:NumberTextCtrl<T>(parent, id, "%d", valPtr, value, pos, size, style, name)
	{
	}

	template<class T>
	DecimalNumberTextCtrl<T>::DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, T minVal, T maxVal, T *valPtr, const wxString &value, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
		: NumberTextCtrl<T>(parent, id, "%d", minVal, maxVal, valPtr, value, pos, size, style, name)
	{
	}

	template<class T>
	class HexNumberTextCtrl : public NumberTextCtrl<T>
	{
	public:
		HexNumberTextCtrl(wxWindow *parent, wxWindowID id, T *valPtr, const wxString &value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxTextCtrlNameStr);
		HexNumberTextCtrl(wxWindow *parent, wxWindowID id, T minVal, T maxVal, T *valPtr, const wxString &value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxTextCtrlNameStr);
		virtual ~HexNumberTextCtrl() {}
	};

	template<class T>
	HexNumberTextCtrl<T>::HexNumberTextCtrl(wxWindow *parent, wxWindowID id, T *valPtr, const wxString &value, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
		:NumberTextCtrl<T>(parent, id, wxString("0x%0") + wxString::Format("%d", sizeof(T) * 2) + wxString("x"), valPtr, value, pos, size, style, name)
	{
		//note that the slightly odd format string above constructs something like 0x%0nx, where n is the minimum number of
		//digits to output, in this case sizeof(T)*2.
		//If you can't remember how printf parsing works (I never can) then %0nx outputs in lower case hex (x) with at least
		//n digits (n) and pads with zeros rather than spaces (0)
		assert(std::is_unsigned<T>());
	}

	template<class T>
	HexNumberTextCtrl<T>::HexNumberTextCtrl(wxWindow *parent, wxWindowID id, T minVal, T maxVal, T *valPtr, const wxString &value, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
		: NumberTextCtrl<T>(parent, id, wxString("0x%0") + wxString::Format("%d", sizeof(T) * 2) + wxString("x"), minVal, maxVal, valPtr, value, pos, size, style, name)
	{
		//note that the slightly odd format string above constructs something like 0x%0nx, where n is the minimum number of
		//digits to output, in this case sizeof(T)*2.
		//If you can't remember how printf parsing works (I never can) then %0nx outputs in lower case hex (x) with at least
		//n digits (n) and pads with zeros rather than spaces (0)
		assert(std::is_unsigned<T>());
	}


	template<>
	class DecimalNumberTextCtrl<double> : public NumberTextCtrl<double>
	{
	public:
		DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, double *valPtr, const wxString &value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxTextCtrlNameStr);
		DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, double minVal, double maxVal, double *valPtr, const wxString &value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxTextCtrlNameStr);
		virtual ~DecimalNumberTextCtrl() {}
	};
	template<>
	class DecimalNumberTextCtrl<float> : public NumberTextCtrl<float>
	{
	public:
		DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, float *valPtr, const wxString &value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxTextCtrlNameStr);
		DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, float minVal, float maxVal, float *valPtr, const wxString &value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxTextCtrlNameStr);
		virtual ~DecimalNumberTextCtrl() {}
	};
}

#endif