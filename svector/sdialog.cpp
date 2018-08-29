#include"sdialog_internal.h"
#include <wx/textdlg.h>
#include <wx/dcclient.h>
#include <wx/dcgraph.h>
#include <wx/sizer.h>


wxArrayString wxMultipleFileSelector(const wxString &message, const wxString &default_path, const wxString &default_filename, const wxString &wildcard, int flags, wxWindow *parent, int x,int y)
{
	wxFileDialog *dlg=new wxFileDialog(parent,message,default_path,default_filename,wildcard,flags|wxFD_MULTIPLE,wxPoint(x,y));
	if(dlg->ShowModal()==wxID_CANCEL) 
	{
		dlg->Destroy();
		return wxArrayString(0,wxT(""));
	}
	wxArrayString paths;
	dlg->GetPaths(paths);
	return paths;
}

bool sci::getTextFromUser(std::string &result, const std::string &message, const std::string &title, const std::string &defaultValue)
{
	result = "";
	wxTextEntryDialog dialog(nullptr, message, title, defaultValue);
	if (dialog.ShowModal() == wxID_OK)
	{
		result = dialog.GetValue();
		return true;
	}
	else
		return false;
}
bool getRealFromUser(double &result, const std::string &message, const std::string &title, double defaultValue)
{
	result = std::numeric_limits<double>::quiet_NaN();
	wxString defaultString;
	defaultString << defaultValue;
	wxTextEntryDialog dialog(nullptr, message, title, defaultString);
	if (dialog.ShowModal() == wxID_OK)
	{
		wxString text = dialog.GetValue();
		return text.ToDouble(&result);
	}
	return false;
}
bool sci::getIntegerFromUser(long &result, const std::string &message, const std::string &title, long defaultValue)
{
	result = 0;
	wxString defaultString;
	defaultString << defaultValue;
	wxTextEntryDialog dialog(nullptr, message, title, defaultString);
	if (dialog.ShowModal() == wxID_OK)
	{
		wxString text = dialog.GetValue();
		return text.ToLong(&result);
	}
	return false;
}
	
sci::DecimalNumberTextCtrl<double>::DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, double *valPtr, const wxString &value, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	:NumberTextCtrl<double>(parent, id, "%g", valPtr, value, pos, size, style, name)
{
}

sci::DecimalNumberTextCtrl<double>::DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, double minVal, double maxVal, double *valPtr, const wxString &value, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	: NumberTextCtrl<double>(parent, id, "%g", minVal, maxVal, valPtr, value, pos, size, style, name)
{
}

sci::DecimalNumberTextCtrl<float>::DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, float *valPtr, const wxString &value, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	: NumberTextCtrl<float>(parent, id, "%g", valPtr, value, pos, size, style, name)
{
}

sci::DecimalNumberTextCtrl<float>::DecimalNumberTextCtrl(wxWindow *parent, wxWindowID id, float minVal, float maxVal, float *valPtr, const wxString &value, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	: NumberTextCtrl<float>(parent, id, "%g", minVal, maxVal, valPtr, value, pos, size, style, name)
{
}

const int sci::GraphicsFrame::PANEL_ID = wxNewId();

namespace sci
{
	class SlaveRenderPanel : public wxPanel
	{
	public:
		SlaveRenderPanel(sci::GraphicsFrame *parent, int id, bool clearBeforeRender, const wxColour &backgroundColour)
			:wxPanel(parent, id)
		{
			m_parent = parent;
			m_clearBeforeRender = clearBeforeRender;
			SetBackgroundColour(backgroundColour);
			Connect(wxEVT_PAINT, wxPaintEventHandler(SlaveRenderPanel::OnPaint));
		}
	private:
		void OnPaint(wxPaintEvent &event)
		{
			wxPaintDC dc(this);
			wxColour bg = GetBackgroundColour();
			dc.SetBackground(GetBackgroundColour());
			dc.Clear();
			m_parent->render(&dc);
		}
		sci::GraphicsFrame *m_parent;
		bool m_clearBeforeRender;
	};
}

sci::GraphicsFrame::GraphicsFrame(wxWindow *parent, wxWindowID id, const wxString& title, bool clearBeforeRender, const wxColour &backgroundColour, const wxPoint& position,
	const wxSize& size, long style, const wxString& name)
:wxFrame(parent, id, title, position, size, style, name)
{
	m_backgroundColour = backgroundColour;
	m_panel = new SlaveRenderPanel(this, PANEL_ID, clearBeforeRender, m_backgroundColour);
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_panel, 1, wxEXPAND);
	this->SetSizer(sizer);
}

void sci::GraphicsFrame::render(wxDC *dc)
{
}
