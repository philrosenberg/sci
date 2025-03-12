#ifndef APEX_MAINFRAME_H
#define APEX_MAINFRAME_H

#include "app.h"
#include<svector/svector.h>
#include<svector/splot.h>
#include<svector/sreadwrite.h>
#include<svector/sstring.h>
#include"GraphicsFrame.h"

class mainFrame : public wxFrame
{
public:
	static const int ID_FILE_EXIT;
	static const int ID_FILE_RUN_PLOT_TESTS;
	static const int ID_FILE_RUN_GRAPHICS_TESTS;
	static const int ID_HELP_ABOUT;

	mainFrame(wxFrame *frame, const wxString& title);
	~mainFrame();
private:
	void OnExit(wxCommandEvent& event);
	void OnRunPlotTests(wxCommandEvent& event);
	void OnRunGraphicsTests(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();
};

class GraphicsPanel1 : public GraphicsPanel
{
public:
	GraphicsPanel1(wxWindow* parent, int id = wxID_ANY)
		:GraphicsPanel(parent, id)
	{}
private:
	virtual void OnPaint(wxPaintEvent& event) override;
};

class GraphicsPanel2 : public GraphicsPanel
{
public:
	GraphicsPanel2(wxWindow* parent, int id = wxID_ANY)
		:GraphicsPanel(parent, id)
	{}
private:
	virtual void OnPaint(wxPaintEvent& event) override;
};
#endif // APEX_MAINFRAME_H
