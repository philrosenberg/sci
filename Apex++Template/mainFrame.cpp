#include "mainFrame.h"

const int mainFrame::ID_FILE_EXIT = ::wxNewId();
const int mainFrame::ID_FILE_RUN = ::wxNewId();
const int mainFrame::ID_FILE_STARTTIMER = ::wxNewId();
const int mainFrame::ID_FILE_ENDTIMER = ::wxNewId();
const int mainFrame::ID_HELP_ABOUT = ::wxNewId();
const int mainFrame::ID_TIMER = ::wxNewId();

BEGIN_EVENT_TABLE(mainFrame, wxFrame)
EVT_MENU(ID_FILE_EXIT, mainFrame::OnExit)
EVT_MENU(ID_FILE_RUN, mainFrame::OnRun)
EVT_MENU(ID_FILE_STARTTIMER, mainFrame::OnStartTimer)
EVT_MENU(ID_FILE_ENDTIMER, mainFrame::OnEndTimer)
EVT_MENU(ID_HELP_ABOUT, mainFrame::OnAbout)
EVT_TIMER(ID_TIMER, mainFrame::OnTimer)
END_EVENT_TABLE()

mainFrame::mainFrame(wxFrame *frame, const wxString& title)
	: wxFrame(frame, wxID_ANY, title)
{
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu(wxT(""));
	fileMenu->Append(ID_FILE_EXIT, wxT("E&xit\tAlt+F4"), wxT("Exit the application"));
	fileMenu->Append(ID_FILE_RUN, wxT("Run\tCtrl+R"), wxT("Run Code"));
	fileMenu->Append(ID_FILE_STARTTIMER, wxT("Start Timer"), wxT("Start Timer"));
	fileMenu->Append(ID_FILE_ENDTIMER, wxT("Stop Timer"), wxT("Stop Timer"));
	mbar->Append(fileMenu, wxT("&File"));

	wxMenu* helpMenu = new wxMenu(wxT(""));
	helpMenu->Append(ID_HELP_ABOUT, wxT("&About\tF1"), wxT("Show info about this application"));
	mbar->Append(helpMenu, wxT("&Help"));

	SetMenuBar(mbar);

	wxPanel *background = new wxPanel(this);
	wxBoxSizer* topLevelSizer = new wxBoxSizer(wxVERTICAL);
	m_textOutput = new wxTextCtrl(background, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	topLevelSizer->Add(m_textOutput,1, wxEXPAND);
	background->SetSizer(topLevelSizer);

	m_actionTimer = new wxTimer(this, ID_TIMER);
}

void mainFrame::OnExit(wxCommandEvent& event)
{
	Close();
}

void mainFrame::OnRun(wxCommandEvent& event)
{
	(*m_textOutput) << "Running some code\n";
}

void mainFrame::OnStartTimer(wxCommandEvent& event)
{
	m_actionTimer->Start(10 * 1000);
	(*m_textOutput) << "Timer started\n";
}

void mainFrame::OnEndTimer(wxCommandEvent& event)
{
	m_actionTimer->Stop();
	(*m_textOutput) << "Timer stopped\n";
}

void mainFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(wxT("$projectname$ Version 1.00.0"), wxT("About $projectname$..."));
}

void mainFrame::OnTimer(wxTimerEvent& event)
{
	(*m_textOutput) << "Timer alert\n";
}

mainFrame::~mainFrame()
{
}