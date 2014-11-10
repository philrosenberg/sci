#include"../include/svector/sdialog.h"


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

	

