#include "mainFrame.h"

const int mainFrame::ID_FILE_EXIT = ::wxNewId();
const int mainFrame::ID_FILE_RUN = ::wxNewId();
const int mainFrame::ID_HELP_ABOUT = ::wxNewId();

BEGIN_EVENT_TABLE( mainFrame, wxFrame )
	EVT_MENU( ID_FILE_EXIT, mainFrame::OnExit )
	EVT_MENU( ID_FILE_RUN, mainFrame::OnRun )
	EVT_MENU( ID_HELP_ABOUT, mainFrame::OnAbout )
END_EVENT_TABLE()


void getWeightsFlat( const std::vector<double> &diameters, std::vector<double> &weights )
{
	weights.resize(0);
	weights.resize(diameters.size(), 1.0);
}

void getWeightsNormal( const std::vector<double> &diameters, std::vector<double> &weights, double mean, double standardDeviation )
{
	//weights.resize(diameters.size(), 1.0);
	//for(size_t i=0; i<weights.size(); ++i)
		//weights[i]=sci::
}


mainFrame::mainFrame( wxFrame *frame, const wxString& title )
	: wxFrame( frame, -1, title )
{
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu( wxT("") );
	fileMenu->Append( ID_FILE_EXIT, wxT("E&xit\tAlt+F4"), wxT("Exit the application") );
	fileMenu->Append( ID_FILE_RUN, wxT("Run\tCtrl+R"), wxT("Run Code") );
	mbar->Append( fileMenu, wxT("&File") );

	wxMenu* helpMenu = new wxMenu( wxT("") );
	helpMenu->Append( ID_HELP_ABOUT, wxT("&About\tF1"), wxT("Show info about this application") );
	mbar->Append( helpMenu, wxT("&Help") );
	
	SetMenuBar( mbar );
}

void mainFrame::OnExit( wxCommandEvent& event )
{
	Close();
}

void mainFrame::OnRun( wxCommandEvent& event )
{
	
}

void mainFrame::OnAbout( wxCommandEvent& event )
{
	wxMessageBox( wxT("$projectname$ Version 1.00.0"), wxT("About cdpfog...") );
}

mainFrame::~mainFrame()
{
}