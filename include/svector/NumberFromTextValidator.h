#pragma once
#include<wx/validate.h>
#include<wx/textctrl.h>
#include<type_traits>

template<class T>
class NumberFromTextValidator : public wxValidator
{
public:
	NumberFromTextValidator(wxString outputFormat, T *valPtr);
	NumberFromTextValidator(wxString outputFormat, T minVal, T maxVal, T *valPtr);
	virtual wxObject *Clone() const;
	virtual bool Validate(wxWindow *parent);
	virtual bool TransferFromWindow();
	virtual bool TransferToWindow();
private:
	wxString m_outputFormat;
	T *m_valPtr;
	T textToNumber(const wxString &text) const;
	bool m_extraRangeChecks;
	T m_minVal;
	T m_maxVal;
	wxString m_lastError;
};

template<class T>
NumberFromTextValidator<T>::NumberFromTextValidator(wxString outputFormat, T *valPtr)
{
	if (!valPtr)
		throw(std::string("Error: Invalid pointer passed to NumberFromTextValidator"));

	m_valPtr = valPtr;
	m_outputFormat = outputFormat;
	m_extraRangeChecks = false;
}

template<class T>
NumberFromTextValidator<T>::NumberFromTextValidator(wxString outputFormat, T minVal, T maxVal, T *valPtr)
{
	if (!valPtr)
		throw(std::string("Error: Invalid pointer passed to NumberFromTextValidator"));

	m_valPtr = valPtr;
	m_outputFormat = outputFormat;
	m_extraRangeChecks = true;
	m_minVal = minVal;
	m_maxVal = maxVal;
}

template<class T>
wxObject *NumberFromTextValidator<T>::Clone() const
{
	if (m_extraRangeChecks)
		return new NumberFromTextValidator<T>(m_outputFormat, m_minVal, m_maxVal, m_valPtr);
	return new NumberFromTextValidator<T>(m_outputFormat, m_valPtr);
}

template<class T>
bool NumberFromTextValidator<T>::Validate(wxWindow *parent)
{
	wxTextCtrl *textCtrl = dynamic_cast<wxTextCtrl*>(GetWindow());
	wxString text = textCtrl->GetValue();
	try
	{
		textToNumber(text);
	}
	catch (...)
	{
		textCtrl->SetBackgroundColour(*wxRED);
		textCtrl->Refresh();
		return false;
	}
	textCtrl->SetBackgroundColour(wxNullColour);
	textCtrl->Refresh();
	return true;
}

template<class T>
bool NumberFromTextValidator<T>::TransferFromWindow()
{
	try
	{
		*m_valPtr = textToNumber(dynamic_cast<wxTextCtrl*>(GetWindow())->GetValue());
	}
	catch (std::string message)
	{
		m_lastError = wxString(message);
		return false;
	}
	return true;
}

template<class T>
bool NumberFromTextValidator<T>::TransferToWindow()
{
	wxString text;
	text.Printf(m_outputFormat, *m_valPtr);
	dynamic_cast<wxTextCtrl*>(GetWindow())->SetValue(text);
	return true;
}

template<class T>
T NumberFromTextValidator<T>::textToNumber(const wxString &text) const
{
	if (std::is_integral<T>())
	{
		if (std::is_signed<T>())
		{
			wxLongLong_t result;
			if (text.length() >= 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X'))
			{
				if (!text.ToLongLong(&result, 16))
					throw(std::string("Error: Invalid hex value."));
			}
			else
			{
				if (!text.ToLongLong(&result, 10))
					throw(std::string("Error: Invalid base 10 value."));
			}
			if (result > (wxLongLong_t)std::numeric_limits<T>::max() || result < (wxLongLong_t)std::numeric_limits<T>::min())
				throw(std::string("Error: out of range."));
			T finalResult = T(result);
			if (m_extraRangeChecks && (finalResult < m_minVal || finalResult > m_maxVal))
				throw(std::string("Error: out of range."));
			return finalResult;
		}
		else
		{
			wxULongLong_t result;
			if (text.length() >= 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X'))
			{
				if (!text.ToULongLong(&result, 16))
					throw(std::string("Error: Invalid hex value."));
			}
			else
			{
				if (!text.ToULongLong(&result, 10))
					throw(std::string("Error: Invalid base 10 value."));
			}
			if (result > std::numeric_limits<T>::max())
				throw(std::string("Error: out of range."));
			T finalResult = T(result);
			if (m_extraRangeChecks && (finalResult < m_minVal || finalResult > m_maxVal))
				throw(std::string("Error: out of range."));
			return finalResult;
		}
	}
	else
		throw(std::string("Error: Unexpected type for NumberFromTextValidator"));
}


template<> double NumberFromTextValidator<double>::textToNumber(const wxString &text) const;
template<> float NumberFromTextValidator<float>::textToNumber(const wxString &text) const;