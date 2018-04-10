#ifndef sdialog_h
#define sdialog_h

#include<wx/filedlg.h>

wxArrayString wxMultipleFileSelector(const wxString &message, const wxString &default_path=wxT(""), const wxString &default_filename=wxT(""), const wxString &wildcard=wxT("*.*"), int flags=0, wxWindow *parent=NULL, int x=-1,int y=-1);

namespace sci
{
	bool getTextFromUser(std::string &result, const std::string &message, const std::string &title, const std::string &defaultValue);
	bool getRealFromUser(double &result, const std::string &message, const std::string &title, double defaultValue);
	bool getIntegerFromUser(long &result, const std::string &message, const std::string &title, long defaultValue);
}

#endif