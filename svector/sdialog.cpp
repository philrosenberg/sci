#include"sdialog_internal.h"
#include <wx/textdlg.h>


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
	

