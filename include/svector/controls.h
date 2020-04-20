#pragma once

#include<tuple>
#include<wx/string.h>
#include<wx/dialog.h>

namespace sci
{

	template<class T>
	class InputValidityChecker
	{
	public:
		static bool checkValidValue(const wxString& value);
		static T getValue(const wxString& value);
	};

	template<>
	class InputValidityChecker<long>
	{
	public:
		static bool checkValidValue(const wxString& value)
		{
			long number;
			return value.ToLong(&number);
		}
		static long getValue(const wxString& value)
		{
			long number;
			value.ToLong(&number);
			return number;
		}
	};

	template<>
	class InputValidityChecker<long long>
	{
	public:
		static bool checkValidValue(const wxString& value)
		{
			long long number;
			return value.ToLongLong(&number);
		}
		static long long getValue(const wxString& value)
		{
			long long number;
			value.ToLongLong(&number);
			return number;
		}
	};

	template<>
	class InputValidityChecker<unsigned long>
	{
	public:
		static bool checkValidValue(const wxString& value)
		{
			unsigned long number;
			return value.ToULong(&number);
		}
		static unsigned long getValue(const wxString& value)
		{
			unsigned long number;
			value.ToULong(&number);
			return number;
		}
	};

	template<>
	class InputValidityChecker<unsigned long long>
	{
	public:
		static bool checkValidValue(const wxString& value)
		{
			unsigned long long number;
			return value.ToULongLong(&number);
		}
		static unsigned long long getValue(const wxString& value)
		{
			unsigned long long number;
			value.ToULongLong(&number);
			return number;
		}
	};

	template<>
	class InputValidityChecker<double>
	{
	public:
		static bool checkValidValue(const wxString& value)
		{
			double number;
			return value.ToDouble(&number);
		}
		static double getValue(const wxString& value)
		{
			double number;
			value.ToDouble(&number);
			return number;
		}
	};

	template<class TUPLE>
	class InputDialog : public wxDialog
	{
	public:
		InputDialog(wxWindow* parent, int id, wxString title, const std::vector<wxString>& labels, TUPLE& defaultValues)
			:wxDialog(parent, id, title)
		{
			size_t nItems = std::tuple_size<TUPLE>::value;
			wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
			wxFlexGridSizer* controlSizer = new wxFlexGridSizer(2, 5, 5);
			m_values = defaultValues;
			addControls(m_values, controlSizer, labels);
			topSizer->Add(controlSizer);
			wxSizer* buttonSizer = CreateButtonSizer(wxOK | wxCANCEL);
			if (buttonSizer)
				topSizer->Add(buttonSizer);
			SetSizer(topSizer);

			Bind(wxEVT_BUTTON, &InputDialog::OnOk, this, wxID_OK);
		}

		template<size_t I = 0, typename... Tp>
		void addControls(const std::tuple<Tp...>& defaultValues, wxFlexGridSizer* sizer, const std::vector<wxString>& labels)
		{
			sizer->Add(labels.size() > I ? new wxStaticText(this, -1, labels[I]) : new wxStaticText(this, -1, ""));
			wxString valueText;
			valueText << std::get<I>(defaultValues);
			m_inputControls.push_back(new wxTextCtrl(this, -1, valueText));
			sizer->Add(m_inputControls.back());
			if constexpr (I + 1 != sizeof...(Tp))
				addControls<I + 1>(defaultValues, sizer, labels);
		}
		TUPLE getValues()
		{
			return m_values;
		}

	private:
		template<size_t I = 0, typename... Tp>
		void parseControls(std::tuple<Tp...>& values, const std::vector<wxTextCtrl*>& controls, std::vector<bool>& success)
		{
			success.resize(controls.size());
			wxString value = controls[I]->GetValue();
			success[I] = InputValidityChecker<std::remove_reference<decltype (std::get<I>(values))>::type>::checkValidValue(controls[I]->GetValue());
			std::get<I>(values) = InputValidityChecker<std::remove_reference<decltype (std::get<I>(values))>::type>::getValue(controls[I]->GetValue());
			//value >> std::get<I>values;
			if constexpr (I + 1 != sizeof...(Tp))
				parseControls<I + 1>(values, controls, success);
		}
		void OnOk(wxCommandEvent& event)
		{
			TUPLE values;
			std::vector<bool> success;
			parseControls(values, m_inputControls, success);
			bool allOkay = true;
			for (size_t i = 0; i < success.size(); ++i)
			{
				allOkay = allOkay && success[i];
				if (!success[i])
					m_inputControls[i]->SetBackgroundColour(wxColour(255, 160, 160));
				else
					m_inputControls[i]->SetBackgroundColour(wxColour(255, 255, 255));
			}
			if (allOkay)
			{
				m_values = values;
				event.Skip();
			}
		}
		void OnCancel(wxCommandEvent& event)
		{
			for (size_t i = 0; i < m_inputControls.size(); ++i)
				m_inputControls[i]->SetBackgroundColour(wxColour(255, 255, 255));
			event.Skip();
		}
		std::vector<wxTextCtrl*> m_inputControls;
		TUPLE m_values;
	};
}