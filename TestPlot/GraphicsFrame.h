#pragma once
#ifndef GRAPHICS_TEST_FRAME_H
#define GRAPHICS_TEST_FRAME_H

#include "app.h"
#include<svector/splot.h>

class GraphicsPanel : public wxPanel
{
public:
	GraphicsPanel(wxWindow *parent, int id = wxID_ANY)
		:wxPanel(parent, id)
	{
		Connect(wxEVT_PAINT, wxPaintEventHandler(GraphicsPanel::OnPaint));
		Connect(wxEVT_SIZE, wxSizeEventHandler(GraphicsPanel::OnResize));
	}
private:
	virtual void OnPaint(wxPaintEvent & event);
	void OnResize(wxSizeEvent& event)
	{
		//refresh the whole panel so it is all redrawn, not just the invalidated area
		Refresh();
	}
	//DECLARE_EVENT_TABLE();
};

template<class PANEL=GraphicsPanel>
class GraphicsFrame : public wxFrame
{
public:
	GraphicsFrame(wxFrame* parent)
		: wxFrame(parent, wxID_ANY, "Graphics frame", wxDefaultPosition, FromDIP(wxSize(800, 600)))
	{
		m_panel = new PANEL(this);
	}
	~GraphicsFrame()
	{}
private:
	PANEL* m_panel;

};
#endif // APEX_MAINFRAME_H