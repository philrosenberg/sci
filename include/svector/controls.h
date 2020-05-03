#pragma once

#include<tuple>
#include<wx/string.h>
#include<wx/dialog.h>
#include<wx/panel.h>
#include<wx/textctrl.h>
#include<wx/sizer.h>

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
	class InputPanel : public wxPanel
	{
	public:
		InputPanel(wxWindow* parent, int id, const std::vector<wxString>& labels, const TUPLE& defaultValues, bool okayButton, bool applyButton, bool cancelButton)
			:wxPanel(parent, id)
		{
			size_t nItems = std::tuple_size<TUPLE>::value;
			wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
			wxFlexGridSizer* controlSizer = new wxFlexGridSizer(2, 5, 5);
			m_values = defaultValues;
			addControls(m_values, controlSizer, labels);
			topSizer->Add(controlSizer);
			if (okayButton || applyButton || cancelButton)
			{
				wxStdDialogButtonSizer* buttonSizer = new wxStdDialogButtonSizer();
				if (okayButton)
					buttonSizer->AddButton(new wxButton(this, wxID_OK));
				if (applyButton)
					buttonSizer->AddButton(new wxButton(this, wxID_APPLY));
				if (cancelButton)
					buttonSizer->AddButton(new wxButton(this, wxID_CANCEL));
				buttonSizer->Realize();
				topSizer->Add(buttonSizer);
			}
			SetSizer(topSizer);

			Bind(wxEVT_BUTTON, &InputPanel::OnOk, this, wxID_OK);
			Bind(wxEVT_BUTTON, &InputPanel::OnCancel, this, wxID_CANCEL);
			Bind(wxEVT_BUTTON, &InputPanel::OnApply, this, wxID_APPLY);
		}
		InputPanel(wxWindow* parent, int id, const std::vector<wxString>& labels, const TUPLE& defaultValues, const std::vector<int> &buttonIds, const std::vector<wxString> &buttonLabels)
			:wxPanel(parent, id)
		{
			size_t nItems = std::tuple_size<TUPLE>::value;
			wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
			wxFlexGridSizer* controlSizer = new wxFlexGridSizer(2, 5, 5);
			m_values = defaultValues;
			addControls(m_values, controlSizer, labels);
			topSizer->Add(controlSizer);
			if (buttonLabels.size() > 0)
			{
				sci::assertThrow(buttonLabels.size() == buttonIds.size());
				wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
				for(size_t i=0; i< buttonLabels.size(); ++i)
					buttonSizer->AddButton(new wxButton(this, buttonIds[i], buttonLabels[i]));
				topSizer->Add(buttonSizer);
			}
			SetSizer(topSizer);
		}
		InputPanel(wxWindow* parent, int id, const std::vector<wxString>& labels, const TUPLE& defaultValues)
			:wxPanel(parent, id)
		{
			size_t nItems = std::tuple_size<TUPLE>::value;
			wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
			wxFlexGridSizer* controlSizer = new wxFlexGridSizer(2, 5, 5);
			m_values = defaultValues;
			addControls(m_values, controlSizer, labels);
			topSizer->Add(controlSizer);
			SetSizer(topSizer);
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

		bool validateInputs()
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
			return allOkay;
		}

		bool acceptInputs()
		{
			if (!validateInputs())
				return false;
			TUPLE values;
			std::vector<bool> success;
			parseControls(values, m_inputControls, success);
			m_values = values;
		}

		void resetInputs()
		{
			for (size_t i = 0; i < m_inputControls.size(); ++i)
				m_inputControls[i]->SetBackgroundColour(wxColour(255, 255, 255));
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
			if (acceptInputs())
				event.Skip();
		}
		void OnApply(wxCommandEvent& event)
		{
			acceptInputs();
			event.Skip();
		}
		void OnCancel(wxCommandEvent& event)
		{
			resetInputs();
			event.Skip();
		}
		std::vector<wxTextCtrl*> m_inputControls;
		TUPLE m_values;
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
			m_panel = new InputPanel<TUPLE>(this, wxID_ANY, labels, defaultValues, true, false, true);
			topSizer->Add(m_panel);
			SetSizer(topSizer);
		}
		TUPLE getValues()
		{
			return m_panel->getValues();
		}

	private:
		InputPanel<TUPLE>* m_panel;
	};
}