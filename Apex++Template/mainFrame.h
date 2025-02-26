#ifndef APEX_MAINFRAME_H
#define APEX_MAINFRAME_H

#include "app.h"
#include<svector/svector.h>
#include<svector/splot.h>
#include<svector/sreadwrite.h>
#include<svector/sstring.h>
#include<wx/timer.h>

class mainFrame : public wxFrame
{
public:
	static const int ID_FILE_EXIT;
	static const int ID_FILE_RUN;
	static const int ID_FILE_STARTTIMER;
	static const int ID_FILE_ENDTIMER;
	static const int ID_FILE_RUN;
	static const int ID_HELP_ABOUT;
	static const int ID_TIMER;

	mainFrame(wxFrame *frame, const wxString& title);
	~mainFrame();
private:
	wxTextCtrl *m_textOutput;
	wxTimer* m_actionTimer;
	void OnExit(wxCommandEvent& event);
	void OnRun(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnStartTimer(wxCommandEvent& event);
	void OnEndTimer(wxCommandEvent& event);
	void OnTimer(wxTimerEvent& event);
	DECLARE_EVENT_TABLE();
};
#endif // APEX_MAINFRAME_H
