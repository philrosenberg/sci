#ifndef splotpanel_H
#define splotpanel_H

#include <vector>
#include <fstream>
#include <algorithm>
#include <wxplplotstream.h>
#include <wx/arrstr.h>
#include <wx/notebook.h>
#include <wx/clrpicker.h>
#include<wx/panel.h>
#include<wx/dialog.h>
#include<wx/listbox.h>
#include<wx/stattext.h>
#include<wx/textctrl.h>
#include<wx/button.h>
#include<wx/checkbox.h>
#include<wx/image.h>
#include<wx/msgdlg.h>
#include<wx/bitmap.h>
#include<wx/dcclient.h>
#include<wx/dcmemory.h>
#include<svector/svector.h>
#include<wx/frame.h>
#include<wx/filename.h>

using namespace std;

static const vector<double> none1d(0);
static const vector<vector<double> > none2d(0);
static const wxColour defaultcolour=*wxBLACK;
static const vector<wxColour> nocol1d(0);
static const double defaultlimit=std::numeric_limits<double>::quiet_NaN();

//static cptrace <double> PLOT_emptydata(wxT("<--None-->"));
//static cptrace <bool> PLOT_emptyfilter(wxT("<--None-->"));

class splotframe;




class splotpanel : public wxPanel
{
	friend class splotframe;
public:
	//function to save the plot to an image
	bool saveplot(wxString filename);

protected:
	splotpanel(wxWindow *parent,
		wxWindowID id,  
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize, 
		long style=wxTAB_TRAVERSAL,
		const wxString &name=wxT("Sci++ Plot Panel"));
	virtual ~splotpanel();
	wxBitmap *getbitmap(){return m_membitmap;};
	bool colourmapisgood(const vector< vector <double> > &map, bool shadergb);
	vector<vector <double> > getdefaultcolourmap(bool shadergb);

	//keep track of the size so we can see if the plot bitmap needs regenerating upon resize
	int m_oldwidth;
	int m_oldheight;

	//record if the plot has been updated and needs fully redrawing
	bool m_plotupdated;

	//bitmap of the current plot, saved for quick redrawing
	wxBitmap *m_membitmap;

private:
	//function called to repaint the plot. this is pure virtual and must be overloaded by the derived class
	virtual void OnPaint(wxPaintEvent &WXUNUSED(event))=0;
	void OnEraseBackGround(wxEraseEvent& event) {};//this is overloaded to stop flicker when resizing
	
	DECLARE_EVENT_TABLE();
};

class splotpanel1d : public splotpanel
{
	friend splotframe;
public:
		//functions to update the plot
	//2d x and y data for a 1d plot with muliple traces
	void updateplot(
		const vector < vector <double> > &xs,
		const vector < vector <double> > &ys,
		const vector < vector <double> > &xpluserrs=vector < vector <double> >(0),
		const vector < vector <double> > &xnegerrs=vector < vector <double> >(0),
		const vector < vector <double> > &ypluserrs=vector < vector <double> >(0),
		const vector < vector <double> > &ynegerrs=vector < vector <double> >(0),
		const vector <wxColour> &linecolours=vector<wxColour>(0),
		const vector <wxColour> &fillcolours=vector<wxColour>(0),
		const vector <wxColour> &errorcolours=vector<wxColour>(0),
		const vector <bool> &plotlines=vector<bool>(0),
		const vector <bool> &plotpoints=vector<bool>(0),
		const wxArrayString &names=wxArrayString(),
		double minx=std::numeric_limits<double>::quiet_NaN(),
		double maxx=std::numeric_limits<double>::quiet_NaN(),
		double miny=std::numeric_limits<double>::quiet_NaN(),
		double maxy=std::numeric_limits<double>::quiet_NaN(),
		bool xlog=false,
		bool ylog=false,
		wxString xlabel=wxT("x axis"),
		wxString ylabel=wxT("y axis"));
	//1d x and y data for a 1d plot with 1 trace
	void updateplot(
		const vector <double> &xs,
		const vector <double> &ys,
		const vector <double> &xpluserrs=vector<double>(0),
		const vector <double> &xnegerrs=vector<double>(0),
		const vector <double> &ypluserrs=vector<double>(0),
		const vector <double> &ynegerrs=vector<double>(0),
		const wxColour &linecolours=*wxBLACK,
		const wxColour &fillcolours=*wxBLACK,
		const wxColour &errorcolours=*wxBLACK,
		const bool &plotlines=true,
		const bool &plotpoints=true,
		const wxString &name=wxT("trace"),
		double minx=std::numeric_limits<double>::quiet_NaN(),
		double maxx=std::numeric_limits<double>::quiet_NaN(),
		double miny=std::numeric_limits<double>::quiet_NaN(),
		double maxy=std::numeric_limits<double>::quiet_NaN(),
		bool xlog=false,
		bool ylog=false,
		wxString xlabel=wxT("x axis"),
		wxString ylabel=wxT("y axis"));
	//2d x data with 1d y data for a 1d plot with muliple traces
	void updateplot(
		const vector < vector <double> > &xs,
		const vector <double>  &ys,
		const vector < vector <double> > &xpluserrs=vector < vector <double> >(0),
		const vector < vector <double> > &xnegerrs=vector < vector <double> >(0),
		const vector < vector <double> > &ypluserrs=vector < vector <double> >(0),
		const vector < vector <double> > &ynegerrs=vector < vector <double> >(0),
		const vector <wxColour> &linecolours=vector<wxColour>(0),
		const vector <wxColour> &fillcolours=vector<wxColour>(0),
		const vector <wxColour> &errorcolours=vector<wxColour>(0),
		const vector <bool> &plotlines=vector<bool>(0),
		const vector <bool> &plotpoints=vector<bool>(0),
		const wxArrayString &names=wxArrayString(),
		double minx=std::numeric_limits<double>::quiet_NaN(),
		double maxx=std::numeric_limits<double>::quiet_NaN(),
		double miny=std::numeric_limits<double>::quiet_NaN(),
		double maxy=std::numeric_limits<double>::quiet_NaN(),
		bool xlog=false,
		bool ylog=false,
		wxString xlabel=wxT("x axis"),
		wxString ylabel=wxT("y axis"));
	void addtrace(
		const vector <double> &xs,
		const vector <double> &ys,
		const vector <double> &xpluserrs=vector<double>(0),
		const vector <double> &xnegerrs=vector<double>(0),
		const vector <double> &ypluserrs=vector<double>(0),
		const vector <double> &ynegerrs=vector<double>(0),
		const wxColour &linecolours=*wxBLACK,
		const wxColour &fillcolours=*wxBLACK,
		const wxColour &errorcolours=*wxBLACK,
		const wxString &name=wxT("another trace"),
		const bool &plotlines=true,
		const bool &plotpoints=true);

protected:
	splotpanel1d(wxWindow *parent,
		wxWindowID id,  
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize, 
		long style=wxTAB_TRAVERSAL,
		const wxString &name=wxT("Sci++ Plot Panel"));

private:
	
	//variables holding data 
	wxString m_title;
	vector < vector <double> > m_xs;
	vector < vector <double> > m_ys;
	vector < vector <double> > m_xmins;
	vector < vector <double> > m_xmaxs;
	vector < vector <double> > m_ymins;
	vector < vector <double> > m_ymaxs;
	vector <wxColour> m_linecolours;
	vector <wxColour> m_fillcolours;
	vector <wxColour> m_outlinecolours;
	vector <wxColour> m_errorcolours;
	vector <bool> m_plotlines;
	vector <bool> m_plotpoints;
	wxArrayString m_names;
	wxArrayString m_symbols;
	wxArrayString m_linesymbols;
	double m_minx;
	double m_maxx;
	double m_miny;
	double m_maxy;
	bool m_xlog;
	bool m_ylog;
	wxString m_xlabel;
	wxString m_ylabel;

	//GeneratePlot to actually generate the plot either on screen or to file
	void GeneratePlot(const char *driver, void *target);
	//OnPaint function for redrawing and displaying the plots
	void OnPaint(wxPaintEvent &WXUNUSED(event));
};


class splotpanel2d : public splotpanel
{
	friend splotframe;

public:
	void updateplot(
		const vector <double> &xs,
		const vector <double> &ys,
		const vector < vector <double> > &zs,
		const vector <double> &contourlevels,
		bool showcontours,
		bool shade,
		const wxColour &contourcolour=*wxBLACK,
		const vector< vector<double> > &shademap=vector< vector<double> >(0),
		bool shadergb=true,
		double minx=std::numeric_limits<double>::quiet_NaN(),
		double maxx=std::numeric_limits<double>::quiet_NaN(),
		double miny=std::numeric_limits<double>::quiet_NaN(),
		double maxy=std::numeric_limits<double>::quiet_NaN(),
		bool xlog=false,
		bool ylog=false,
		wxString title=wxT(""),
		wxString xlabel=wxT("x axis"),
		wxString ylabel=wxT("y axis"));
	void addlayer(
		const vector <double> &xs,
		const vector <double> &ys,
		const vector < vector <double> > &zs,
		const vector <double> &contourlevels,
		bool showcontours,
		bool shade,
		const wxColour &contourcolour=*wxBLACK,
		const vector< vector<double> > &shademap=vector< vector<double> >(0),
		bool shadergb=true);
protected:
	splotpanel2d(wxWindow *parent,
		wxWindowID id,  
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize, 
		long style=wxTAB_TRAVERSAL,
		const wxString &name=wxT("Sci++ Plot Panel"));
	~splotpanel2d(){clear();};

private:
	vector<PLFLT **> m_zs;//vector of 2d plplot arrays for z values
	vector<PLcGrid2> m_xys; //vector of 2d plplot data structure which contains x and y data, note x and y are 2d! this allows transformation to arbitrary grid
	vector< vector <double> > m_contours;
	vector<wxColour> m_contourcolours;
	vector< vector<vector<double> > > m_shademaps;
	vector<bool> m_showcontours;
	vector<bool> m_shade;
	vector<bool> m_shadergb;
	vector<bool> m_rectilinear;
	wxArrayString m_linesymbols;
	double m_minx;
	double m_maxx;
	double m_miny;
	double m_maxy;
	bool m_xlog;
	bool m_ylog;
	wxString m_title;
	wxString m_xlabel;
	wxString m_ylabel;
	
	//OnPaint function for redrawing and displaying the plots
	void OnPaint(wxPaintEvent &WXUNUSED(event));
	void clear(); //frees all memory safely
};

//predefinition for the TraceSettingsDialog to allow use in PlotDialog
class TraceSettingsDialog;

class PlotDialog1d : public wxDialog
{
public:
	PlotDialog1d(wxWindow* parent, 
		wxWindowID id, 
		const wxString& title, 
		const vector < vector<double> > &datasets,
		const vector < vector<bool> > &filters,
		const wxArrayString &datanames,
		const wxArrayString &filternames,
		splotpanel1d *plotwindow=NULL, 
		const wxPoint& pos = wxDefaultPosition,
		long style = wxDEFAULT_DIALOG_STYLE, 
		const wxString& name = wxT("plotDialog"));
	splotpanel1d *getplotpanel(){return m_plotpanel;};
	void UpdateData (const vector < vector<double> > &datasets, const vector< vector<bool> > &filters, const wxArrayString &datanames, const wxArrayString &filternames);

	static const int ID_PDLGDATAADD;
	static const int ID_PDLGDATAREMOVE;
	static const int ID_PDLGDATAEDIT;
	static const int ID_PDLGDATAMOVEUP;
	static const int ID_PDLGDATAMOVEDOWN;
	static const int ID_PDLGAXISSPANX;
	static const int ID_PDLGAXISSPANY;
private:
	//dataset info
	vector < vector<double> > m_datasets; //all possible data that we might want to plot
	wxArrayString m_datanames; //names of the data 
	vector < vector<bool> > m_filters; //filters which allow us to plot only spectific data
	wxArrayString m_filternames; //names of the filters 
	vector <double> m_emptyvector; //this is just an empty vector to use in place of bad pointers

	//info about the traces being used
	vector <int> m_tracexs;
	vector <int> m_traceys;
	vector <int> m_tracefilters;
	vector <int> m_tracexposerrs;
	vector <int> m_tracexnegerrs;
	vector <int> m_traceyposerrs;
	vector <int> m_traceynegerrs;
	vector <wxColour> m_linecolours;
	vector <wxColour> m_fillcolours;
	vector <wxColour> m_errorcolours;
	vector <bool> m_plotlines;
	vector <bool> m_plotpoints;
	wxArrayString m_tracenames;

	//pointer to the plot window
	splotpanel1d *m_plotpanel;

	//Top Level GUI elements
	wxNotebook *m_tabs;
	wxPanel *m_datatab;
	wxPanel *m_axistab;
	wxSizer *m_buttonsizer; //this sizer will contain the Apply, OK and Cancel buttons
	wxBoxSizer *m_topsizer; //this sizer will arrange the top level elements

	//GUI Elements for the Data Tab
	wxBoxSizer *m_datasizer;
	wxBoxSizer *m_databuttonsizer;
	wxListBox *m_tracelist;
	wxButton *m_addbutton;
	wxButton *m_removebutton;
	wxButton *m_editbutton;
	wxButton *m_moveupbutton;
	wxButton *m_movedownbutton;

	//GUI Elements for axis tab
	wxFlexGridSizer *m_axissizer;
	wxTextCtrl *m_minxtext;
	wxTextCtrl *m_minytext;
	wxTextCtrl *m_maxxtext;
	wxTextCtrl *m_maxytext;
	wxTextCtrl *m_xtitletext;
	wxTextCtrl *m_ytitletext;
	wxTextCtrl *m_plottitletext;
	wxStaticText *m_xlabel;
	wxStaticText *m_ylabel;
	wxStaticText *m_minlabel;
	wxStaticText *m_maxlabel;
	wxStaticText *m_xtitlelabel;
	wxStaticText *m_ytitlelabel;
	wxStaticText *m_plottitlelabel;
	wxButton *m_spanxbutton;
	wxButton *m_spanybutton;
	wxCheckBox *m_xlogcheck;
	wxCheckBox *m_ylogcheck;

	//event functions
	void OnAddButton(wxCommandEvent &event);
	void OnRemoveButton(wxCommandEvent &event);
	void OnEditButton(wxCommandEvent &event);
	void OnMoveUpButton(wxCommandEvent &event);
	void OnMoveDownButton(wxCommandEvent &event);
	void OnOKButton(wxCommandEvent &event);
	void OnSpanXButton(wxCommandEvent &event);
	void OnSpanYButton(wxCommandEvent &event);

	//internal functions
	bool GetTraceSettings(TraceSettingsDialog *settingsdlg, wxString &name, int &xindex, int &yindex, int &filterindex, int &xposerrindex, int &xnegerrindex, int &yposerrindex, int &ynegerrindex, wxColour &linecol, wxColour &pointcol, wxColour &errcol, bool &plotlines, bool &plotpoints);


	DECLARE_EVENT_TABLE();
};

class TraceSettingsDialog : public wxDialog
{
	friend class PlotDialog1d; //make plotdialog a friend
protected:
	//make the constructor and other functions protected. This means only
	//PlotDialog can create and access this class
	TraceSettingsDialog(PlotDialog1d* parent, 
		wxWindowID id, 
		const wxString& title, 
		const vector< vector<double> > &datasets,
		const vector< vector<bool> > &filters,
		const wxArrayString &datanames,
		const wxArrayString &filternames,
		wxString tracename=wxEmptyString,
		int defaultx=-1,
		int defaulty=-1,
		int defaultfilter=-1,
		int defaultxposerr=-1,
		int defaultxnegerr=-1,
		int defaultyposerr=-1,
		int defaultynegerr=-1,
		wxColour linecol=*wxBLACK,
		wxColour pointcol=*wxBLACK,
		wxColour errcol=*wxBLACK,
		bool plotlines=true,
		bool plotpoints=true,
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize, 
		long style = wxDEFAULT_DIALOG_STYLE, 
		const wxString& name = wxT("traceSettingsDialog"));
	bool GetSettings(wxString *name, int *xindex, int *yindex, int *filterindex, int *xposerrindex, int *xnegerrindex, int *yposerrindex, int *ynegerrindex, wxColour *linecol, wxColour *pointcol, wxColour *errcol, bool* plotlines, bool *plotpoints);

private:

	//GUI elements

	//name textbox
	wxStaticText *m_namelabel;
	wxTextCtrl *m_nametext;

	//x and y selection lists
	wxStaticText *m_xlabel;
	wxStaticText *m_ylabel;
	wxStaticText *m_filterlabel;
	wxListBox *m_xlist;
	wxListBox *m_ylist;
	wxListBox *m_filterlist;

	//x errors selection lists
	wxStaticText *m_xposerrlabel;
	wxStaticText *m_xnegerrlabel;
	wxListBox *m_xposerrlist;
	wxListBox *m_xnegerrlist;

	//y errors selection lists
	wxStaticText *m_yposerrlabel;
	wxStaticText *m_ynegerrlabel;
	wxListBox *m_yposerrlist;
	wxListBox *m_ynegerrlist;

	//colour pickers
	wxStaticText *m_linecolourlabel;
	wxStaticText *m_pointcolourlabel;
	wxStaticText *m_errorcolourlabel;
	wxColourPickerCtrl *m_linecolourpicker;
	wxColourPickerCtrl *m_pointcolourpicker;
	wxColourPickerCtrl *m_errorcolourpicker;

	//checkboxes to select lines/point
	wxStaticText *m_lineplotlabel;
	wxStaticText *m_pointplotlabel;
	wxCheckBox *m_lineplotcheck;
	wxCheckBox *m_pointplotcheck;

	//sizers for arranging items
	wxBoxSizer *m_topsizer;
	wxBoxSizer *m_namesizer;
	wxFlexGridSizer *m_formatsizer;
	wxBoxSizer *m_tracesizer;
	wxSizer *m_buttonsizer;
	wxFlexGridSizer *m_listsizer;
};

class splotframe : public wxFrame
{
public:
	splotframe(wxWindow* parent, 
		wxWindowID id, 
		const wxString& title, 
		bool firstplot2d=false,
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize, 
		long style = wxDEFAULT_FRAME_STYLE);
	~splotframe(){};
	void splitpanellr(splotpanel *oldpanel, size_t oldpanelposition,std::vector<int>spacefractions, vector<bool> newplot2d, std::vector<splotpanel*> &plots);
	void splitpanelud(splotpanel *oldpanel, size_t oldpanelposition,std::vector<int>spacefractions, vector<bool> newplot2d, std::vector<splotpanel*> &plots);
	splotpanel *getplot(size_t index){return m_plots[index];};
	void deleteplot(splotpanel *plot);
	bool savetobitmap(wxString filename);
	bool savetobitmap(wxString filename, wxString filetype);
private:
	std::vector< splotpanel* > m_plots;

	//private backend to the lr and ud splitpanel functions above
	void splitpanel(splotpanel *oldpanel, size_t oldpanelposition,std::vector<int>spacefractions, vector<bool> newplot2d, std::vector<splotpanel*> &panels, int orientation);

};

#endif