#pragma once

#include<tuple>
#include<wx/string.h>
#include<wx/dialog.h>
#include<wx/panel.h>
#include<wx/textctrl.h>
#include<wx/sizer.h>
#include<wx/checkbox.h>
#include<wx/combobox.h>
#include<wx/stattext.h>
#include"sstring.h"
#include<vector>
#include"serr.h"

namespace sci
{
	template<class T>
	inline bool textToValue(const wxString& source, T& destination);

	template<>
	inline bool textToValue<long>(const wxString& source, long& destination)
	{
		return source.ToLong(&destination);
	}
	template<>
	inline bool textToValue<long long>(const wxString& source, long long& destination)
	{
		return source.ToLongLong(&destination);
	}
	template<>
	inline bool textToValue<unsigned long>(const wxString& source, unsigned long& destination)
	{
		return source.ToULong(&destination);
	}
	template<>
	inline bool textToValue<unsigned long long>(const wxString& source, unsigned long long& destination)
	{
		return source.ToULongLong(&destination);
	}
	template<>
	inline bool textToValue<float>(const wxString& source, float& destination)
	{
		double temp;
		if (!source.ToDouble(&temp))
			return false;
		destination = float(temp);
			return true;
	}
	template<>
	inline bool textToValue<double>(const wxString& source, double& destination)
	{
		return source.ToDouble(&destination);
	}
	template<>
	inline bool textToValue<wxString>(const wxString& source, wxString& destination)
	{
		destination = source;
		return true;
	}
	template<>
	inline bool textToValue<sci::string>(const wxString& source, sci::string& destination)
	{
		destination = sci::fromUtf8(std::string(source.ToUTF8()));
		return true;
	}

	template<class T>
	struct Control_Type_Traits
	{
	};

	template<>
	struct Control_Type_Traits<long>
	{
		typedef wxTextCtrl ControlType;
		typedef ControlType* ControlPointerType;
	};

	template<>
	struct Control_Type_Traits<long long>
	{
		typedef wxTextCtrl ControlType;
		typedef ControlType* ControlPointerType;
	};

	template<>
	struct Control_Type_Traits<unsigned long>
	{
		typedef wxTextCtrl ControlType;
		typedef ControlType* ControlPointerType;
	};

	template<>
	struct Control_Type_Traits<unsigned long long>
	{
		typedef wxTextCtrl ControlType;
		typedef ControlType* ControlPointerType;
	};

	template<>
	struct Control_Type_Traits<float>
	{
		typedef wxTextCtrl ControlType;
		typedef ControlType* ControlPointerType;
	};

	template<>
	struct Control_Type_Traits<double>
	{
		typedef wxTextCtrl ControlType;
		typedef ControlType* ControlPointerType;
	};

	template<>
	struct Control_Type_Traits<sci::string>
	{
		typedef wxTextCtrl ControlType;
		typedef ControlType* ControlPointerType;
	};

	template<>
	struct Control_Type_Traits<wxString>
	{
		typedef wxTextCtrl ControlType;
		typedef ControlType* ControlPointerType;
	};

	template<>
	struct Control_Type_Traits<bool>
	{
		typedef wxCheckBox ControlType;
		typedef ControlType* ControlPointerType;
	};

	template<>
	struct Control_Type_Traits<std::pair<size_t,std::vector<sci::string>>>
	{
		typedef wxComboBox ControlType;
		typedef ControlType* ControlPointerType;
	};

	template<>
	struct Control_Type_Traits<std::pair<size_t, std::vector<wxString>>>
	{
		typedef wxComboBox ControlType;
		typedef ControlType* ControlPointerType;
	};

	template<class T>
	class GenericControl : public Control_Type_Traits<T>::ControlType
	{
	public:
		typedef T ValueType;
		GenericControl(wxWindow* parent, int id, T defaultValue)
		{
			if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxCheckBox>::value)
				Control_Type_Traits<T>::ControlType::Create(parent, id, "");
			//else if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxComboBox>::value)
			//	Create(parent, id, "", wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
			else
				Control_Type_Traits<T>::ControlType::Create(parent, id);
			setValue(defaultValue);
		}
		bool hasValidValue()
		{
			if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxTextCtrl>::value)
			{
				T number;
				return textToValue(Control_Type_Traits<T>::ControlType::GetValue(), number);
			}
			else if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxComboBox>::value)
				return Control_Type_Traits<T>::ControlType::GetSelection() != wxNOT_FOUND;
			else
				return true;
		}
		T getValue()
		{
			if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxTextCtrl>::value)
			{
				T number;
				textToValue(Control_Type_Traits<T>::ControlType::GetValue(), number);
				return number;
			}
			else if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxComboBox>::value)
			{
				T result;
				result.first = Control_Type_Traits<T>::ControlType::GetSelection();
				wxArrayString controlElements = Control_Type_Traits<T>::ControlType::GetStrings();
				result.second.resize(controlElements.size());
				if constexpr(std::is_same < T::second_type::value_type, wxString>::value)
				{
					for (size_t i = 0; i < result.second.size(); ++i)
					{
						result.second[i] = controlElements[i];
					}
				}
				else
				{
					for (size_t i = 0; i < result.second.size(); ++i)
					{
						result.second[i] = sci::fromUtf8(controlElements[i].ToUTF8());
					}
				}
				return result;
			}
			else
				return Control_Type_Traits<T>::ControlType::GetValue();
		}
		void setValue(T value)
		{
			if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxTextCtrl>::value)
			{
				wxString valueText;
				valueText << value;
				Control_Type_Traits<T>::ControlType::SetValue(valueText);
			}
			else if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxComboBox>::value)
			{
				//check if the options have changes first
				wxArrayString controlStrings = Control_Type_Traits<T>::ControlType::GetStrings();
				bool same = controlStrings.size() == value.second.size();
				if constexpr (std::is_same<T::second_type::value_type, wxString>::value)
				{
					//first if we have wxStrings
					if (same)
					{
						for (size_t i = 0; i < controlStrings.size(); ++i)
						{
							same = same && controlStrings[i] == value.second[i];
						}
					}
					if (!same)
					{
						Control_Type_Traits<T>::ControlType::Set(value.second.size(), &value.second[0]);
					}

				}
				else
				{
					if (same)
					{
						for (size_t i = 0; i < controlStrings.size(); ++i)
						{
							wxString newString;
							newString << sci::nativeUnicode(value.second[i]);
							same = same && controlStrings[i] == newString;
						}
					}
					if (!same)
					{
						wxArrayString newStrings;
						newStrings.resize(value.second.size());
						for (size_t i = 0; i < newStrings.size(); ++i)
							newStrings[i] << sci::nativeUnicode(value.second[i]);
						Control_Type_Traits<T>::ControlType::Set(newStrings);
					}
				}

				//now set the correct option
				SetSelection(value.first);
			}
			else
				SetValue(value);
		}

		static wxEventTypeTag<wxCommandEvent> getControlModifiedEventTag()
		{
			if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxTextCtrl>::value)
				return wxEVT_TEXT;
			else if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxCheckBox>::value)
				return wxEVT_CHECKBOX;
			else if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxComboBox>::value)
				return wxEVT_COMBOBOX;
			else 
				static_assert(std::is_same< Control_Type_Traits<T>::ControlType, wxTextCtrl>::value, "Error with types in GenericControl");//got to have some comparison in static assert even though it's in a constexpr if
		}

		void setColour(wxColour colour)
		{
			if constexpr (std::is_same< Control_Type_Traits<T>::ControlType, wxComboBox>::value)
			{
				Control_Type_Traits<T>::ControlType::SetBackgroundColour(colour);
			}
			else
				Control_Type_Traits<T>::ControlType::SetBackgroundColour(colour);
		}
	};


	template<class TUPLE, int BEGIN, int LENGTH>
	struct Control_Tuple
	{
		typedef typename std::tuple_element<BEGIN, TUPLE>::type BeginType;
		typedef typename GenericControl<BeginType> *BeginControlPointerType;
		typedef typename Control_Tuple<TUPLE, BEGIN + 1, LENGTH - 1>::type RemainderControlPointerType;
		typedef decltype(std::tuple_cat(std::make_tuple(BeginControlPointerType()), RemainderControlPointerType())) type;
	};
	template<class TUPLE, int BEGIN>
	struct Control_Tuple<TUPLE, BEGIN, 1>
	{
		typedef typename std::tuple_element<BEGIN, TUPLE>::type BeginType;
		typedef typename GenericControl<BeginType> *BeginControlPointerType;
		typedef typename std::tuple<BeginControlPointerType> type;
	};
	/*template<class TUPLE>
	struct Control_Tuple
	{
		typdef Control_Tuple<TUPLE, 0, std::size<TUPLE>()>::type type;
	};*/

	template<class TUPLE>
	class InputPanel : public wxPanel
	{
	public:
		InputPanel(wxWindow* parent, int id, const std::vector<wxString>& labels, const TUPLE& defaultValues, bool okayButton, bool applyButton, bool cancelButton)
			:wxPanel(parent, id)
		{
			size_t nItems = std::tuple_size<TUPLE>::value;
			m_hasFlushedChanges.assign(m_nControls, true);
			wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
			wxFlexGridSizer* controlSizer = new wxFlexGridSizer(2, 5, 5);
			m_values = defaultValues;
			m_tempValues = defaultValues;
			makeControls(m_values, controlSizer, labels);
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
			m_hasFlushedChanges.assign(m_nControls, true);
			wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
			wxFlexGridSizer* controlSizer = new wxFlexGridSizer(2, 5, 5);
			m_values = defaultValues;
			makeControls(m_values, controlSizer, labels);
			topSizer->Add(controlSizer);
			if (buttonLabels.size() > 0)
			{
				sci::assertThrow(buttonLabels.size() == buttonIds.size(), sci::err(sci::SERR_CONTROLS, 0, sU("Attempted to create an Input panel, but the number of button IDs did not match the number of buttons")));
				wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
				for(size_t i=0; i< buttonLabels.size(); ++i)
					buttonSizer->Add(new wxButton(this, buttonIds[i], buttonLabels[i]));
				topSizer->Add(buttonSizer);
			}
			SetSizer(topSizer);
		}
		InputPanel(wxWindow* parent, int id, const std::vector<wxString>& labels, const TUPLE& defaultValues)
			:wxPanel(parent, id)
		{
			size_t nItems = std::tuple_size<TUPLE>::value;
			m_hasFlushedChanges.assign(m_nControls, true);
			wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
			wxFlexGridSizer* controlSizer = new wxFlexGridSizer(2, 5, 5);
			m_values = defaultValues;
			makeControls(m_values, controlSizer, labels);
			Refresh(); //This shouldn't be needed, but sometimes the controls aren't getting rendered and this fixes it
			topSizer->Add(controlSizer);
			SetSizer(topSizer);
		}

		TUPLE getValues()
		{
			return m_values;
		}

		template<size_t I>
		typename std::tuple_element<I,TUPLE>::type getValue()
		{
			return std::get<I>(m_values);
		}

		TUPLE getTempValues()
		{
			return m_tempValues;
		}

		template<size_t I>
		typename std::tuple_element<I, TUPLE>::type getTempValue()
		{
			return std::get<I>(m_tempValues);
		}

		void setValues(const TUPLE& values, bool markModified)
		{
			if (markModified)
			{
				m_tempValues = values;
				displayValues(m_tempValues);
			}
			else
			{
				m_values = values;
				m_tempValues = values;
				displayValues(m_values);
			}
			processModified(nullptr);
		}

		template<int I, class T>
		void setValue(const T &value, bool markModified)
		{
			if (markModified)
			{
				std::get<I>(m_tempValues) = value;
				displayValue<I>(value);
			}
			else
			{
				std::get<I>(m_values) = value;
				std::get<I>(m_tempValues) = value;
				displayValue<I>(value);
			}
			processModified(std::get<I>(m_inputControls));
		}

		bool validateInputs()
		{
			TUPLE values;
			std::vector<bool> success;
			parseControls(values, success);
			bool allOkay = true;
			for (size_t i = 0; i < success.size(); ++i)
			{
				allOkay = allOkay && success[i];
			}
			return allOkay;
		}

		bool acceptInputs()
		{
			if (!validateInputs())
				return false;
			TUPLE values;
			std::vector<bool> success;
			parseControls(values, success);
			m_values = values;
			m_hasFlushedChanges.assign(m_nControls, true);
			return true;
		}

		void resetInputs()
		{
			displayValues(m_values);
			m_hasFlushedChanges.assign(m_nControls, true);
		}

		bool hasFlushedChanged() const
		{
			for (auto i = m_hasFlushedChanges.begin(); i != m_hasFlushedChanges.end(); ++i)
				if (!(*i))
					return false;
			return true;
		}

		size_t getControlIndexFromPointer(wxObject* control)
		{
			return getControlIndexFromPointerInternal(control);
		}

		static constexpr int getControlId()
		{
			return ID_CTRL;
		}

	private:

		template<size_t I=0>
		void makeControls(const TUPLE& defaultValues, wxFlexGridSizer* sizer, const std::vector<wxString>& labels)
		{
			sizer->Add(labels.size() > I ? new wxStaticText(this, -1, labels[I]) : new wxStaticText(this, -1, ""));
			auto value = std::get<I>(defaultValues);
			std::get<I>(m_inputControls)=(new GenericControl<decltype(value)>(this, ID_CTRL, value));
			sizer->Add(std::get<I>(m_inputControls));
			if constexpr (I + 1 != std::tuple_size<TUPLE>())
				makeControls<I + 1>(defaultValues, sizer, labels);

			//Bind function to events. Only need one Bind per event type
			if (I == 0)
			{
				Bind(wxEVT_TEXT, &InputPanel::OnControlChanged, this, ID_CTRL);
				Bind(wxEVT_CHECKBOX, &InputPanel::OnControlChanged, this, ID_CTRL);
				Bind(wxEVT_COMBOBOX, &InputPanel::OnControlChanged, this, ID_CTRL);
			}
		}

		template<size_t I = 0>
		void displayValues(const TUPLE& values)
		{
			auto value = std::get<I>(values);
			std::get<I>(m_inputControls)->setValue(value);
			std::get<I>(m_inputControls)->setColour(wxColour(255, 255, 255));
			std::get<I>(m_inputControls)->Refresh();

			if constexpr (I + 1 != std::tuple_size<TUPLE>())
				displayValues<I + 1>(values);
		}

		template<size_t I, class T>
		void displayValue(const T &value)
		{
			std::get<I>(m_inputControls)->setValue(value);
			std::get<I>(m_inputControls)->setColour(wxColour(255, 255, 255));
			std::get<I>(m_inputControls)->Refresh();
		}

		template<size_t I = 0>
		void parseControls(TUPLE& inputValues, std::vector<bool>& success)
		{
			success.resize(m_nControls);
			success[I] = std::get<I>(m_inputControls)->hasValidValue();
			if (success[I])
			{
				std::get<I>(inputValues) = std::get<I>(m_inputControls)->getValue();
				std::get<I>(m_inputControls)->setColour(wxColour(255, 255, 255));
				std::get<I>(m_inputControls)->Refresh();
			}
			else
			{
				std::get<I>(m_inputControls)->setColour(wxColour(255, 160, 160));
				std::get<I>(m_inputControls)->Refresh();
			}

			if constexpr (I + 1 != std::tuple_size<TUPLE>())
				parseControls<I + 1>(inputValues, success);
		}

		//returns true if a valid change was made to the control compared to the temp (not accepted) value, returns false, otherwise.
		template<size_t I=0>
		bool processModified(wxObject* modifiedControl)
		{
			if (modifiedControl == std::get<I>(m_inputControls) || modifiedControl == nullptr)
			{
				//we have found the control requested
				bool result = true;

				//check if the new value is valid
				if (!std::get<I>(m_inputControls)->hasValidValue())
				{
					result = false;
					std::get<I>(m_inputControls)->setColour(wxColour(255, 160, 160));
				}
				//check if the value is different to the previous known value and also the accepted value and format appropriately
				else
				{
					auto value = std::get<I>(m_inputControls)->getValue();

					if (value == std::get<I>(m_tempValues))
						result = false;
					else
						std::get<I>(m_tempValues) = value;

					if (value == std::get <I>(m_values))
					{
						m_hasFlushedChanges[I] = true;
					}
					else
					{
						m_hasFlushedChanges[I] = false;
						std::get<I>(m_inputControls)->setColour(wxColour(160, 180, 255));
					}
				}

				//if we were asked to process a specific control, return whether it has changed
				if(modifiedControl)
					return result;
			}
			if constexpr (I + 1 != m_nControls)
				return processModified<I+1>(modifiedControl);
			return false;
		}

		template<size_t I = 0>
		size_t getControlIndexFromPointerInternal(wxObject *control)
		{
			if (control == std::get<I>(m_inputControls))
			{
				return I;
			}

			if constexpr (I + 1 != m_nControls)
				return getControlIndexFromPointerInternal<I + 1>(control);
			return -1;
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
		void OnControlChanged(wxCommandEvent& event)
		{
			bool newChange = processModified(event.GetEventObject());
			if(newChange)
				event.Skip();
		}

		static const int ID_CTRL;
		static const size_t m_nControls = std::tuple_size<TUPLE>::value;
		typename Control_Tuple<TUPLE, 0, m_nControls>::type m_inputControls;
		TUPLE m_values;
		TUPLE m_tempValues;
		std::vector<bool> m_hasFlushedChanges;
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

	template< class TUPLE >
	const int InputPanel<TUPLE>::ID_CTRL = ::wxNewId();
}