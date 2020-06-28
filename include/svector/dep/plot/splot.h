#ifndef splot_h
#define splot_h

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define SVECTOR_MUST_RESET_CRT_SECURE_NO_WARNINGS
#endif

#pragma warning(push, 0)
#include<vector>
#include<wx/colour.h>
#include<plplot/plstream.h>
#include<wx/wx.h>
#include<wx/scrolwin.h>
#include<limits>
#include<wx/print.h>
#include<wx/printdlg.h>
#pragma warning(pop)
#include"../../sstring.h"
#include"../../serr.h"
#include"../../svector.h"

#ifdef SVECTOR_MUST_RESET_CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#undef SVECTOR_MUST_RESET_CRT_SECURE_NO_WARNINGS
#endif

const int colourscaleErrorCode = 2;
const int sizescaleErrorCode = 3;
const int splot2dErrorCode = 4;

//pre declarations
class splot;
class splot2d;
class DrawableItem;

//emptyvector
static const std::vector<double> empty1d;
static const std::vector<std::vector<double> > empty2d;

template <class T>
void incrementsize(std::vector<T> &v)
{
	v.resize(v.size()+1);
}

//plot types
enum splottype
{
	scatter,
	shadecont,
	bar
};

//values for changing font with plplot
#define pl_SANSSERIF 0x80000000
#define pl_SERIF     0x80000001
#define pl_MONOSPACE 0x80000002
#define pl_SCRIPT    0x80000003
#define pl_SYMBOL    0x80000004
//
#define pl_UPRIGHT   0x80000000
#define pl_ITALIC    0x80000010
#define pl_OBLIQUE   0x80000020
//
#define pl_NOBOLD    0x80000000
#define pl_BOLD      0x80000100

//shorthand defines for transform functions
#define splot_REGULAR1DXY &lineartransform
#define splot_REGULAR2DXY &lineartransform2d

//transformation functions
void lineartransform(double xindex, double yindex, const std::vector<double> &x, const std::vector<double> &y, double &xout, double &yout);
void lineartransform2d(double xindex, double yindex, const std::vector< std::vector< double > > &x, const std::vector< std::vector< double > > &y, double &xout, double &yout);

//custom label functions
sci::string loglonghand(double axisvalue);
void customlabelinterpreter(PLINT axis, PLFLT value, char* label, PLINT length,PLPointer function);
typedef  sci::string(*customlabeler)(double);

// rgb colour. the hue colour wheel goes from 0-360, but numbers outside this range are permitted.
// lightness, saturation and alpha are in the 0-1 range and will be set to the apropriate limit if
//outside these values
class rgbcolour;

class hlscolour
{
public:
	hlscolour(double hue, double lightness, double saturation, double alpha=1.0);
	hlscolour();
	rgbcolour convertToRgb() const;
	inline double h() const {return m_h;}
	inline double l() const {return m_l;}
	inline double s() const {return m_s;}
	inline double a() const {return m_a;}
	double m_h;
	double m_l;
	double m_s;
	double m_a;
};

// rgb colour, all three colours and alpha are in the 0-1 range and will be set to 
//the apropriate limit if outside these values
class rgbcolour
{
public:
	rgbcolour(double red, double green, double blue, double alpha=1.0);
	rgbcolour();
	hlscolour convertToHls() const;
	inline double r() const {return m_r;}
	inline double g() const {return m_g;}
	inline double b() const {return m_b;}
	inline double a() const {return m_a;}
	double m_r;
	double m_g;
	double m_b;
	double m_a;
};


int sploterrorcatcher(const char *message);
class PlotData2dStructured;

class splotcolourscale
{
	friend class splot;
	friend class splot2d;
	friend class splotlegend;
public:
	splotcolourscale();
	//create a colourscale from rgb colours. Use the same number of values in the value and colour vectors to create a continuous colour scale or one more in the value vector to create a discrete colour scale
	splotcolourscale(const std::vector<double> &value, const std::vector< rgbcolour > &colour, bool logarithmic=false, bool autostretch=false);
	//create a colourscale from hls colours. Use the same number of values in the value and colour vectors to create a continuous colour scale or one more in the value vector to create a discrete colour scale
	splotcolourscale(const std::vector<double> &value, const std::vector< hlscolour > &colour, bool logarithmic=false, bool autostretch=false);
	splotcolourscale(const splotcolourscale& s);
	splotcolourscale(const splotcolourscale& predefinedscale, double newmin, double newmax);
	rgbcolour getRgbNormalisedScale( double value ) const;
	rgbcolour getRgbOriginalScale( double value ) const;
	hlscolour getHlsNormalisedScale( double value ) const;
	hlscolour getHlsOriginalScale( double value ) const;
	double getNormalisedValue( double value ) const;
	inline double getMax() const {return m_top;}
	inline double getMin() const {return m_bottom;}
	rgbcolour getRgbOffscaleBottom();
	rgbcolour getRgbOffscaleTop();
	hlscolour getHlsOffscaleBottom();
	hlscolour getHlsOffscaleTop();
	splotcolourscale& operator=(const splotcolourscale& s);
	bool isLogarithmic() const { return m_logarithmic;}
	~splotcolourscale(){};
	void setup(plstream *pl) const;
private:
	void setupdefault();
	void setup(const std::vector<double> &value, const std::vector< rgbcolour > &colour, bool logarithmic = false, bool autostretch = false);
	void setup(const std::vector<double> &value, const std::vector< hlscolour > &colour, bool logarithmic = false, bool autostretch = false);
	bool m_autovalue;
	std::vector<double> m_value;
	std::vector< double > m_colour1;
	std::vector< double > m_colour2;
	std::vector< double > m_colour3;
	std::vector< double > m_alpha;
	bool m_hls; //true for hls, false for rgb colour representation
	bool m_logarithmic; //set to true to use a log scale
	double m_bottom;
	double m_top;
	void interpolate( double value, double &c1, double &c2, double &c3, double &a) const;
};

class splotsizescale
{
	friend class splot;
	friend class splot2d;
	friend class splotlegend;
public:
	splotsizescale(const std::vector<double> &value=std::vector<double>(0), const std::vector<double> &size=std::vector<double>(0), bool logarithmic=false);
	~splotsizescale(){};
	double getsize(double value) const;
	double getSizeNormalisedScale(double value) const;
	bool isLogarithmic() const { return m_logarithmic; }
private:
	std::vector<double> m_value;
	std::vector<double> m_valueNormalised;
	std::vector<double> m_size;
	bool m_logarithmic;
};


//this is simply an interface to a 2d std::vector
class splot2dmatrix : public Contourable_Data
{
public:
	PLFLT operator()(PLINT i, PLINT j ) const {return (*m_z)[i][j];};
	splot2dmatrix(std::vector< std::vector<double> > *z):Contourable_Data((int)z->size(),(int)z->at(0).size())
	{
		sci::assertThrow(sci::rectangular(*z), sci::err(sci::SERR_PLOT, 0, sU("splot2dmatrix data is not rectangular.")));
		sci::assertThrow(z->size() <= std::numeric_limits<int>::max(), sci::err(sci::SERR_PLOT, 0, sU("data size is too large for a plplot Contourable_Data object.")));
		if( z->size() > 0)
			sci::assertThrow(z->at(0).size() <= std::numeric_limits<int>::max(), sci::err(sci::SERR_PLOT, 0, sU("data size is too large for a plplot Contourable_Data object.")));
		m_z=z;
	};
	splot2dmatrix():Contourable_Data(0,0){m_z=NULL;};
	void changepointerleavingsizethesame(std::vector< std::vector<double> > *z){m_z=z;}
private:
	std::vector< std::vector<double> > *m_z;
};

namespace sym
{
	//const sci::string filledCircle = sU("#[0x25cf]");
	const sci::string filledCircle=sU("#[0x2b24]");
	const sci::string openCircle= sU("#[0x2b55]");
	const sci::string dottedCircle= sU("#[0x25cc]");
	const sci::string openCentreFilledCircle= sU("#[0x25c9]");
	const sci::string openDoubleCircle= sU("#[0x25c9]");
	const sci::string openRoundedSquare= sU("#[0x25a2]");
	const sci::string openSquare= sU("#[0x25a1]");
	const sci::string filledSquare= sU("#[0x25a0]");
	const sci::string openFilledCentreSquare= sU("#[0x25a3]");
	const sci::string upwardFilledTriangle= sU("#[0x25b2]");
	const sci::string upwardOpenTriangle= sU("#[0x25b3]");
	const sci::string downwardFilledTriangle= sU("#[0x25bc]");
	const sci::string downwardOpenTriangle= sU("#[0x25bd]");
	const sci::string leftwardFilledTriangle= sU("#[0x25c0]");
	const sci::string leftwardOpenTriangle= sU("#[0x25c1]");
	const sci::string rightwardFilledTriangle= sU("#[0x25b6]");
	const sci::string rightwardOpenTriangle= sU("#[0x25b7]");
	const sci::string filledDiamond= sU("#[0x25c6]");
	const sci::string openDiamond= sU("#[0x25c7]");
	const sci::string openCentreFilledDiamond=sU("#[0x25c8]");

}


class splotaxis
{
	friend class splot;
	friend class splot2d;
public:
	~splotaxis(){};
	inline void setcolour(wxColour colour){m_colour=colour; m_haschanged=true;};
	inline void setlinethickness(int thickness){m_linethickness=thickness; m_haschanged=true;};
	inline void automajorintervalon(){m_automajorinterval=true; m_haschanged=true;};
	inline void autominorintervalon(){m_autonsubticks=true; m_haschanged=true;};
	inline void autointervalson();
	void setmajorinterval(double interval);
	void setnsubticks(unsigned int nsubticks);
	inline void settitle(sci::string title){m_title=title; m_haschanged=true;};
	inline void setrotatetitle(bool rotated){m_rotatetitle=rotated; m_haschanged=true;};
	inline void settitlesize(double size){m_titlesize=size; m_haschanged=true;};
	inline void settitlecolour(wxColour colour){m_titlecolour=colour; m_haschanged=true;};
	inline void settitledistance(double distance){m_titledistance=distance; m_haschanged=true;};
	inline void settitlefont(sci::string fontface){m_titlefont=fontface; m_haschanged=true;};
	inline void setmajorticklength(double length){m_majorticklength=std::abs(length); m_haschanged=true;};
	inline void setminorticklength(double length){m_minorticklength=std::abs(length); m_haschanged=true;};
	inline void setticksdirection(bool positive, bool negative){m_tickspos=positive;m_ticksneg=negative; m_haschanged=true;};
	inline void setlabelposition(bool positive){m_labelpositionpositive=positive; m_haschanged=true;};
	inline void setshowlabels(bool show){m_showlabels=show; m_haschanged=true;};
	inline void setlabelfont(sci::string fontface){m_labelfont=fontface.c_str(); m_haschanged=true;};
	inline void setlabelstyle(PLUNICODE style){m_labelfci=style; m_haschanged=true;};
	inline void setrotatedlabels(bool rotated){m_rotatelabels=rotated; m_haschanged=true;};
	inline void setlabelsize(double size){m_labelsize=size; m_haschanged=true;};
	inline void setlabelcolour(wxColour colour){m_labelcolour=colour; m_haschanged=true;};
	inline void setndecimalplaces(unsigned int ndecimalplaces){m_autodecimalplaces=false; m_ndecimalplaces=ndecimalplaces;};
	inline void setautodecimalplaces(){m_autodecimalplaces=true;};
	inline void setmaxndigits(int maxndigits){m_maxndigits=maxndigits;};
	inline void setautomaxndigits(){m_maxndigits=0;};
	inline void settimeformat(sci::string format){m_timeformat=format; m_customlabelcreator=NULL;}; //set to "" to use number format else set as per the strftime function
	inline void setcustomlabel(sci::string (*customlabelcreator)(double axisvalue)){m_customlabelcreator=customlabelcreator; m_timeformat=sU("");}; //set to NULL to use normal formatting or a pointer to a function to interpret the axis value otherwise
	//note no public function to set limits, or intersect or set the axis as logarithmic as the 
	//splot class needs to keep track of these parameters for autolimits and log plotting
	//purposes

private:
	//private constructor so only friends can create splotaxis objects
	splotaxis(double min=-1.0, double max=1.0, sci::string title=sU("Axis Title"), sci::string titlefont=sU(""), PLUNICODE titlestyle=0, double titlesize=12.0, double titledistance=3.5, const wxColour &titlecolour=wxColour(0,0,0), double intersectpoint=0.0, wxColour colour=wxColour(0,0,0), int linethickness=1, bool logarithmic=false, bool time=false, double majorticklength=0.8, double minorticklength=0.5, bool tickspositive=false, bool ticksnegative=true, bool showlabels=true, bool labelpositionpositive=false, sci::string labelfont=sU(""), PLUNICODE labelstyle=0, bool labelsrotated=false, double labelsize=9.6, const wxColour &labelcolour=wxColour(0,0,0), bool autodecimalplaces=true, unsigned int ndecimalplaces=0, bool automaxndigits=true, int maxndigits=0);
	splotaxis(double min, double max, double majorinterval, double nsubticks, sci::string title=sU("Axis Title"), sci::string titlefont=sU(""), PLUNICODE titlestyle=0, double titlesize=12.0, double titledistance=3.5, const wxColour &titlecolour=wxColour(0,0,0), double intersectpoint=0.0, wxColour colour=wxColour(0,0,0), int linethickness=1, bool logarithmic=false, bool time=false, double majorticklength=0.8, double minorticklength=0.5, bool tickspositive=false, bool ticksnegative=true, bool showlabels=true, bool labelpositionpositive=false, sci::string labelfont=sU(""), PLUNICODE labelstyle=0, bool labelsrotated=false, double labelsize=9.6, const wxColour &labelcolour=wxColour(0,0,0), bool autodecimalplaces=true, unsigned int ndecimalplaces=0, bool automaxndigits=true, int maxndigits=0);
	
	//keep track of if the axis has changed
	bool m_haschanged;

	//min/max values in plot units
	double m_min;
	double m_max;
	//location in terms of the coordinates in the other axis
	double m_intersect;

	//is the axis logaritmic
	bool m_logarithmic;

	//colour
	wxColour m_colour;

	//line thickness
	double m_linethickness;

	//intervals
	bool m_automajorinterval;
	bool m_autonsubticks;
	double m_majorinterval;
	unsigned int m_nsubticks;

	//tickmark length //scale to be defined, 0.0 is off, if either has a negative number passes
	//then the positive value is recorded but ticksinward is set true
	double m_majorticklength;
	double m_minorticklength;
	bool m_tickspos;
	bool m_ticksneg;
	bool m_showlabels;

	//label details
	bool m_labelpositionpositive;
	bool m_rotatelabels;
	double m_labelsize;
	sci::string m_labelfont;
	PLUNICODE m_labelfci;
	wxColour m_labelcolour;

	//axis title
	sci::string m_title;
	bool m_rotatetitle;
	double m_titlesize; //scale to be defined
	sci::string m_titlefont;
	PLUNICODE m_titlefci;
	wxColour m_titlecolour;
	double m_titledistance; //distance from the axis in multiples of character height
	

	//time/custom format options
	sci::string m_timeformat;
	sci::string (*m_customlabelcreator)(double axisvalue);

	bool m_autodecimalplaces;
	unsigned int m_ndecimalplaces;

	unsigned int m_maxndigits; //maximum number of digits before going to exponents, set to zero for auto
};

//have to #include this down here as classes in here need some of the declarations from
//above, but they don't get included because of the #ifdef protection
#include"plotdata.h"
class splotTransformer;

class splot
{
	friend class splotwindow;
	friend class splot2d;
	friend class splotlegend;
public:
	virtual ~splot(){}
	inline void setxautolimits(bool autolimit){m_xautolimits=autolimit; calculateautolimits(); m_haschanged=true;};
	inline void setyautolimits(bool autolimit){m_yautolimits=autolimit; calculateautolimits(); m_haschanged=true;};
	inline void setxautointersect(bool autointersect){m_xautointersect=autointersect; calculateautolimits(); m_haschanged=true;};
	inline void setyautointersect(bool autointersect){m_yautointersect=autointersect; calculateautolimits(); m_haschanged=true;};
	//inline void settransform(void (*transformfunc)(double xin, double yin, double &xout, double &yout)){m_coordtransformer.m_transformfunc=transformfunc; m_haschanged=true;};
	inline void settitle(sci::string title){m_title=title; m_haschanged=true;};
	inline void settitlesize(double size){m_titlesize=size; m_haschanged=true;};
	inline void settitledistance(double distance){m_titledistance=distance; m_haschanged=true;};
	inline void settitlefont(sci::string fontface){m_titlefont=fontface; m_haschanged=true;};
	inline void settitlestyle(uint32_t style){m_titlefci=style; m_haschanged=true;};
	inline void settitlecolour(wxColour colour){m_titlecolour=colour; m_haschanged=true;};
	inline void setmaxx(double max){m_xautolimits=false; m_xaxis.m_max=max; calculateautolimits(); m_haschanged=true;};
	inline void setminx(double min){m_xautolimits=false; m_xaxis.m_min=min; calculateautolimits(); m_haschanged=true;};
	inline void setxlimits(double min, double max){m_xautolimits=false; m_xaxis.m_min=min; m_xaxis.m_max=max; calculateautolimits(); m_haschanged=true;};
	inline void setxaxisintersect(double intersect){m_xautointersect=false; m_xaxis.m_intersect=intersect; calculateautolimits(); m_haschanged=true;};
	inline void setmaxy(double max){m_yautolimits=false; m_yaxis.m_max=max; calculateautolimits(); m_haschanged=true;};
	inline void setminy(double min){m_yautolimits=false; m_yaxis.m_min=min; calculateautolimits(); m_haschanged=true;};
	inline void setylimits(double min, double max){m_yautolimits=false; m_yaxis.m_min=min; m_yaxis.m_max=max; calculateautolimits(); m_haschanged=true;};
	inline void setyaxisintersect(double intersect){m_yautointersect=false; m_yaxis.m_intersect=intersect; calculateautolimits(); m_haschanged=true;};
	inline splotaxis *getxaxis(){return &m_xaxis; m_haschanged=true;};
	inline splotaxis *getyaxis(){return &m_yaxis; m_haschanged=true;};
	bool inline gethaschanged(){return m_haschanged;};

	virtual void removeAllData();

private:
	//keep track of if the plot has changed
	bool m_haschanged;
	
	//data
	std::vector< std::vector< double > > m_xs;
	std::vector< std::vector< double > > m_ys;
	std::vector< std::vector< double > > m_xsl;//if we are plotting on a log axis then these datasets will be filled with logged values
	std::vector< std::vector< double > > m_ysl;
	std::vector< std::vector< std::vector< double > > > m_xs2d;
	std::vector< std::vector< std::vector< double > > > m_ys2d;
	std::vector< std::vector< std::vector< double > > > m_xs2dl;//if we are plotting on a log axis then these datasets will be filled with logged values
	std::vector< std::vector< std::vector< double > > > m_ys2dl;
	std::vector< std::vector< std::vector< double > > > m_structzs; //structured z data (regular, rectilinear, curvilinear) is 2d and can be shaded/contoured, but should have size 0,0 for an x,y dataset or an unstructured dataset
	std::vector< std::vector< std::vector< double > > > m_structzsl;
	std::vector< std::vector< double > > m_colunstructzs; //unstructured z data is 1d, it currectly cannot be countoured/shaded (may change) but symbol colour may vary with this parameter. It should have size 0 for an x,y dataset or a structured dataset
	std::vector< std::vector< double > > m_colunstructzsl;
	std::vector< std::vector< double > > m_sizeunstructzs; //unstructured z data is 1d, it currectly cannot be countoured/shaded (may change) but symbol size may vary with this parameter. It should have size 0 for an x,y dataset or a structured dataset
	std::vector< std::vector< std::vector <double> > > m_us; //v and v data is for plotting vector arrows
	std::vector< std::vector< std::vector <double> > > m_vs; 
	std::vector<double> m_minstructz;//need these to fill in the colourscale past the ends if required
	std::vector<double> m_maxstructz;
	std::vector<double> m_mincolunstructz;
	std::vector<double> m_maxcolunstructz;
	std::vector<double> m_minstructzl;//same as the four previous, but for logarithmic colour scales
	std::vector<double> m_maxstructzl;
	std::vector<double> m_mincolunstructzl;
	std::vector<double> m_maxcolunstructzl;
	std::vector< std::vector< double > > m_lowerxlimits; //for bar charts
	std::vector< std::vector< double > > m_upperxlimits; //for bar charts
	std::vector< std::vector< double > > m_lowerxlimitsl; //for bar charts log versions
	std::vector< std::vector< double > > m_upperxlimitsl; //for bar charts log versions
	std::vector<bool> m_beginAtZeros; //for bar charts
	std::vector<bool> m_isGrid; //to distinguish between grid and contour style data

	//dataset properties
	std::vector<sci::string> m_pointchar; //this is the character used for plotting
	std::vector<PLUNICODE> m_pointfci; //this is the style of the char (bold, italic, font family)
	std::vector<sci::string> m_pointfont; //this is the fontface name of the font we want for the plot points. If not accessible we default to the fci family
	std::vector<double> m_linethickness; //0.0 for no line/contour
	std::vector<double> m_pointsize; //0.0 for no point, ignored for contour/shade datasets
	std::vector<wxColour> m_pointcolour;
	std::vector<wxColour> m_linecolour;
	std::vector<sci::string> m_linestyle;
	std::vector<splotcolourscale> m_colourscale; //if null shading won't be used for stuctured z data. Will be used for point colour for unstructured z data
	std::vector<splotsizescale> m_sizescale; //will be used to adjust point size for unstructured z
	std::vector< std::vector< double > > m_colourlevels; //colour levels used for colourscale plots
	std::vector<bool> m_filloffscaletop; //flag to state whether to colour offscale regions with the max colour or leave blank
	std::vector<bool> m_filloffscalebottom; //flag to state whether to colour offscale regions with the max colour or leave blank
	std::vector< std::vector< double > > m_contourlevels; //colour levels used for colourscale plots
	std::vector<double> m_contourlabelssize;
	std::vector<sci::string> m_map;
	std::vector<std::shared_ptr<splotTransformer>> m_transformers;
	std::vector<bool> m_linkcontoursandcolours;
	std::vector<std::vector<double> > m_arrowstyle;


	//axes
	splotaxis m_xaxis;
	splotaxis m_yaxis;
	//do we want to autoset limits and intersect point of axes
	bool m_xautolimits;
	bool m_yautolimits;
	bool m_xautointersect;
	bool m_yautointersect;

	//title
	sci::string m_title;
	double m_titlesize;
	double m_titledistance;//in multiples of title character height
	sci::string m_titlefont;
	PLUNICODE m_titlefci;
	wxColour m_titlecolour;

	//function pointer transformation xindex, yindex to x value, y value
	void (*m_transfunc1dxy)(double xindex, double yindex, const std::vector<double> &x, const std::vector<double> &y, double &xout, double &yout);
	void (*m_transfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&);
	virtual void calculateautolimits()=0;
	static void calculateautolimits(splotaxis &axis, const std::vector<std::vector<double>> &data1d, const std::vector<std::vector<double>> &data1dpluserrs, const std::vector<std::vector<double>> &data1dminuserrs, const std::vector<std::vector<std::vector<double>>> &data2d, bool addpadding, std::vector<double> intersectpoints, double existingMin, double existingMax);

	std::string createploptstring(const splotaxis &axis);
	void adddatasetproperties(wxColour pointcolour, double pointsize, sci::string pointsymbol, uint32_t pointstyle, sci::string pointfont, wxColour linecolour, int linewidth, sci::string linestyle, const splotcolourscale &colourscale, const splotsizescale &sizescale, unsigned int ncolourboundaries, bool filloffscaletop, bool filloffscalebottom, int ncontourlevels, double mincontour, double maxcontour, double contourlabelssize, sci::string map, const std::vector<double> &arrowstyle);
	void incrementdatasize();
	void setupcolourscale(int ncolours, const splotcolourscale &colourscale, bool filloffscaletop, bool filloffscalebottom, int ncontourlevels, double maxcontour, double mincontour, int contourlabelssize);
	//parser for generating line styles
	void parselinestyle(sci::string style, std::vector<PLINT> &marks, std::vector<PLINT> &spaces);
};


//a class which holds all the required info to generate a plot
class splot2d : public splot
{
	friend class splotwindow;
public:
	virtual ~splot2d(){}

	void setlogxaxis(bool log);
	void setlogyaxis(bool log);

	//functions to add data to the plots

	//x y scatter with constant colours and sizes
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &xposerrs=std::vector<double>(0), const std::vector<double> &xnegerrs=std::vector<double>(0), const std::vector<double> &yposerrs=std::vector<double>(0), const std::vector<double> &ynegerrs=std::vector<double>(0), wxColour pointcolour=wxColour(0,0,0), double pointsize=0.5, sci::string pointsymbol=sU("A"), wxColour linecolour=wxColour(0,0,0), double linewidth=1.0, sci::string linestyle=sU(""), wxColour xerrcolour=wxColour(0,0,0), wxColour yerrcolour=wxColour(0,0,0), double xerrwidth=1.0, double yerrwidth=1.0, std::shared_ptr<splotTransformer> transformer = NULL);
	//x y scatter with varying point colour
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, const std::vector<double> &xposerrs=std::vector<double>(0), const std::vector<double> &xnegerrs=std::vector<double>(0), const std::vector<double> &yposerrs=std::vector<double>(0), const std::vector<double> &ynegerrs=std::vector<double>(0), double pointsize=0.5, sci::string pointsymbol=sU("A"), wxColour linecolour=wxColour(0,0,0), double linewidth=1.0, sci::string linestyle=sU(""), wxColour xerrcolour=wxColour(0,0,0), wxColour yerrcolour=wxColour(0,0,0), double xerrwidth=1.0, double yerrwidth=1.0, std::shared_ptr<splotTransformer> transformer = NULL);
	//x y scatter with varying point size
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const splotsizescale &sizescale, const std::vector<double> &xposerrs=std::vector<double>(0), const std::vector<double> &xnegerrs=std::vector<double>(0), const std::vector<double> &yposerrs=std::vector<double>(0), const std::vector<double> &ynegerrs=std::vector<double>(0), wxColour pointcolour=wxColour(0,0,0), sci::string pointsymbol=sU("A"), wxColour linecolour=wxColour(0,0,0), double linewidth=1.0, sci::string linestyle=sU(""), wxColour xerrcolour=wxColour(0,0,0), wxColour yerrcolour=wxColour(0,0,0), double xerrwidth=1.0, double yerrwidth=1.0, std::shared_ptr<splotTransformer> transformer = NULL);
	//x y scatter with varying point colour and size
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &colour_zs, const std::vector<double> &size_zs, const splotsizescale &sizescale, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, const std::vector<double> &xposerrs=std::vector<double>(0), const std::vector<double> &xnegerrs=std::vector<double>(0), const std::vector<double> &yposerrs=std::vector<double>(0), const std::vector<double> &ynegerrs=std::vector<double>(0), sci::string pointsymbol=sU("A"), wxColour linecolour=wxColour(0,0,0), double linewidth=1.0, sci::string linestyle=sU(""), wxColour xerrcolour=wxColour(0,0,0), wxColour yerrcolour=wxColour(0,0,0), double xerrwidth=1.0, double yerrwidth=1.0, std::shared_ptr<splotTransformer> transformer = NULL);
	//colourscale and/or contour plot
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, bool filloffscaletop=true, bool filloffscalebottom=true, wxColour contourcolour=wxColour(0,0,0), double contourwidth=1.0, sci::string contourstyle=sU(""), int ncontourlevels=-1, double mincontour=std::numeric_limits<double>::quiet_NaN(), double maxcontour=std::numeric_limits<double>::quiet_NaN(), double contourlabelssize=0.2, std::shared_ptr<splotTransformer> transformer = NULL);
	//colourscale and/or contour plot with 2d x and y
	void adddata(const std::vector< std::vector <double> > &xs, const std::vector< std::vector <double> > &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, bool filloffscaletop=true, bool filloffscalebottom=true, wxColour contourcolour=wxColour(0,0,0), double contourwidth=1.0, sci::string contourstyle=sU(""), unsigned int ncontourlevels=-1, double mincontour=std::numeric_limits<double>::quiet_NaN(), double maxcontour=std::numeric_limits<double>::quiet_NaN(), double contourlabelssize=0.2, std::shared_ptr<splotTransformer> transformer = NULL);
	//bar chart
	void adddata(const std::vector<double> &minedges, const std::vector<double> &maxedges, const std::vector<double> heights, wxColour fillcolour=wxColour(0,0,0),wxColour linecolour=wxColour(0,0,0), double linewidth=0.0, sci::string linestyle=sU(""), bool beginatzero=false);
	//vector/arrow plot
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector< std::vector<double> > &us, const std::vector< std::vector<double> > &vs, wxColour linecolour=wxColour(0,0,0), double linewidth=1.0, const std::vector<double> &arrowstyle=std::vector<double>(0), sci::string linestyle=sU(""), std::shared_ptr<splotTransformer> transformer = NULL);
	//vector/arrow plot
	void adddata(const std::vector<std::vector<double>> &xs, const std::vector<std::vector<double>> &ys, const std::vector< std::vector<double> > &us, const std::vector< std::vector<double> > &vs, wxColour linecolour=wxColour(0,0,0), double linewidth=1.0, const std::vector<double> &arrowstyle=std::vector<double>(0), sci::string linestyle=sU(""), std::shared_ptr<splotTransformer> transformer = NULL);
	//grid plot
	void addShadedGrid(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, bool filloffscaletop=true, bool filloffscalebottom=true, std::shared_ptr<splotTransformer> transformer = NULL);
	//grid plot with 2d x and y
	void addShadedGrid(const std::vector< std::vector <double> > &xs, const std::vector< std::vector <double> > &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, bool filloffscaletop=true, bool filloffscalebottom=true, std::shared_ptr<splotTransformer> transformer = NULL);
	
	//add a DrawableItem
	void addData( std::shared_ptr<DrawableItem> drawableItem );

	void addmap(sci::string map=sU("globe"), wxColour linecolour=wxColour(0,0,0), double linewidth=1.0, sci::string linestyle=sU(""), std::shared_ptr<splotTransformer> transformer = NULL);
	void addImage(sci::string picture, double xBottomLeft, double yBottomLeft, double width, double height, int cropX0=0, int cropY0=0, int cropWidth=-1, int cropHeight=-1, double brightnessCorrection=0.0, double contrastCorrection=0.0);
	void addText(sci::string text, double x, double y, double x2, double y2, double alignment=0.0, double size=0.8, const sci::string &font=sU(""), uint32_t style=0, wxColour colour=wxColour(0,0,0));
	void setdatasetproperties(size_t setindex, wxColour pointcolour=wxColour(0,0,0), double pointsize=0.5, sci::string pointsymbol=sU("A"), wxColour linecolour=wxColour(0,0,0), int linewidth=1.0, wxColour xerrcolour=wxColour(0,0,0), wxColour yerrcolour=wxColour(0,0,0), int xerrwidth=1, int yerrwidth=1);

	void removeAllData();
	void removeData(std::shared_ptr<DrawableItem> drawableItem);

private:
	//private constructor so only friends can create a plot
	splot2d(bool logx=false, bool logy=false,sci::string title=sU(""), double titlesize=1.2, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0),void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&)=splot_REGULAR1DXY,void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&)=splot_REGULAR2DXY);
	splot2d(double minx, double maxx, double miny, double maxy, bool logx=false, bool logy=false, sci::string title=sU(""), double titlesize=1.2, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0),void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&)=splot_REGULAR1DXY,void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&)=splot_REGULAR2DXY);
	splot2d(double minx, double maxx, double miny, double maxy, double xintersect, double yintersect, bool logx=false, bool logy=false, sci::string title=sU(""), double titlesize=1.2, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0),void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&)=splot_REGULAR1DXY,void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&)=splot_REGULAR2DXY);
	//private plot function so only friends can update a plot
	void plot(plstream *pl, wxDC *dc, int width, int height, bool antialiasing, double linewidthmultiplier=1.0);

	//uncertainties
	std::vector< std::vector< double > > m_xpluserrs;
	std::vector< std::vector< double > > m_xminuserrs;
	std::vector< std::vector< double > > m_ypluserrs;
	std::vector< std::vector< double > > m_yminuserrs;
	std::vector< std::vector< double > > m_xpluserrsl;
	std::vector< std::vector< double > > m_xminuserrsl;
	std::vector< std::vector< double > > m_ypluserrsl;
	std::vector< std::vector< double > > m_yminuserrsl;
	std::vector<wxColour> m_xerrcolour;
	std::vector<wxColour> m_yerrcolour;
	std::vector<double>m_xerrthickness;
	std::vector<double>m_yerrthickness;
	//image properties
	std::vector<wxImage>m_images;
	std::vector<double> m_imageXBottomLefts;
	std::vector<double> m_imageYBottomLefts;
	std::vector<double> m_imageHeights;
	std::vector<double> m_imageWidths;
	std::vector<double> m_imageRotations;
	std::vector<double> m_imageContrasts;
	std::vector<double> m_imageBrightnesses;
	//text properties
	std::vector<sci::string> m_text;
	std::vector<sci::string> m_textFont;
	std::vector<PLUNICODE> m_textFci;
	std::vector<double> m_textSize;
	//DrawableItems
	std::vector<std::shared_ptr<DrawableItem>> m_drawableItems;


	void incrementdatasize();
	void setallparams(bool logx, bool logy,sci::string title, double titlesize, double titledistance, sci::string titlefont, int32_t titlestyle, wxColour titlecolour,void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&),void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&));
	void calculateautolimits();
};

class splotlegend : public splot
{
	friend class splotwindow;
public:
	void addentry(sci::string text, double textoffset = 0.2, double textsize = 9.6, const sci::string &textfont = sU(""), uint32_t textstyle = 0, double textspacing = 0.2, wxColour textcolour = wxColour(0, 0, 0), wxColour pointcolour = wxColour(0, 0, 0), double pointsize = 0.5, sci::string pointsymbol = sU("A"), wxColour linecolour = wxColour(0, 0, 0), int linewidth = 1, sci::string linestyle = sU(""));
	void addentry(sci::string text, double textoffset = 0.2, double textsize = 9.6, const sci::string &textfont = sU(""), uint32_t textstyle = 0, double textspacing = 0.2, rgbcolour textcolour = rgbcolour(0, 0, 0), rgbcolour pointcolour = rgbcolour(0, 0, 0), double pointsize = 0.5, sci::string pointsymbol = sU("A"), rgbcolour linecolour = rgbcolour(0, 0, 0), int linewidth = 1, sci::string linestyle = sU(""));
	void addentry(sci::string text, double textoffset = 0.2, double textsize = 9.6, const sci::string &textfont = sU(""), uint32_t textstyle = 0, double textspacing = 0.2, hlscolour textcolour = hlscolour(0, 0, 0), hlscolour pointcolour = hlscolour(0, 0, 0), double pointsize = 0.5, sci::string pointsymbol = sU("A"), hlscolour linecolour = hlscolour(0, 0, 0), int linewidth = 1, sci::string linestyle = sU(""));
	void addentry(sci::string text, const splotcolourscale &colourscale, bool filloffscaletop, bool filloffscalebottom, double headingoffset=0.05, double textoffset=0.2, double textsize=9.6, const sci::string &textfont=sU(""), uint32_t textstyle=0, double textspacing=0.2, wxColour textcolour=wxColour(0,0,0), unsigned int ncolourlevels=256, bool contours=false, size_t height=5, bool horizontal=false);
	void addentry(sci::string text, const splotsizescale &sizescale, double headingoffset=0.05, double textoffset=0.2, double textsize=9.6, const sci::string &textfont=sU(""), uint32_t textstyle=0, double textspacing=0.2, wxColour textcolour=wxColour(0,0,0), wxColour pointcolour=wxColour(0,0,0), sci::string pointsymbol=sU("A"), size_t nlines=5);
	virtual ~splotlegend(){}
private:
	//private constructor so only friends can create a legend
	splotlegend(sci::string title=sU(""), double titlesize=12.0, double titledistance=2.0, double titlespacing=0.2, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0), wxColour outlinecolour=wxColour(0,0,0), int outlinewidth=1);
	wxColour m_outlinecolour;
	int m_outlinethickness;
	double m_titlespacing;
	std::vector<double> m_headingoffset;
	std::vector<double> m_textoffset;
	std::vector<sci::string> m_text;
	std::vector<sci::string> m_textfont;
	std::vector<PLUNICODE> m_textfci;
	std::vector<double> m_textsize;
	std::vector<double> m_textspacing;
	std::vector<wxColour> m_textcolour;
	std::vector<bool> m_contours;
	std::vector<bool> m_horizontal;
	std::vector<bool> m_filloffscaletop;
	std::vector<bool> m_filloffscalebottom;

	std::vector<size_t> m_nlines;
	void adddatasetproperties(sci::string text, double textoffset, double textsize, const sci::string &textfont, uint32_t textstyle, double textspacing, wxColour textcolour, wxColour pointcolour, double pointsize, sci::string pointsymbol, uint32_t pointstyle, sci::string pointfont, wxColour linecolour, int linewidth, sci::string linestyle, const splotcolourscale &colourscale, bool filloffscaletop, bool filloffscalebottom, const splotsizescale &sizescale, unsigned int ncolourlevels, bool contours, size_t nlines, double headingoffset, bool horizontal);
	void plot(plstream *pl, double linewidthmultiplier=1.0);
	void calculateautolimits(){}; //do nothing - function only created as it is pure virtual
};

class splotwindow : public wxScrolledWindow 
{
	friend class splotPrintout;
public:
	splotwindow(wxWindow *parent, bool antialiasing, bool fixedsize=false, wxWindowID winid=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxHSCROLL | wxVSCROLL, const wxString& name = wxT("sci++ plot"));
	virtual ~splotwindow();
	void setfixedsize(int width, int height);
	void setautofitplots();
	splot2d* addplot(double xpos, double ypos, double width, double height, bool logx=false, bool logy=false,sci::string title=sU(""), double titlesize=14.4, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0));
	splot2d* addplot(double xpos, double ypos, double width, double height, double minx, double maxx, double miny, double maxy, bool logx=false, bool logy=false, sci::string title=sU(""), double titlesize=14.4, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0));
	splot2d* addplot(double xpos, double ypos, double width, double height, double minx, double maxx, double miny, double maxy, double xintersect, double yintersect, bool logx=false, bool logy=false, sci::string title=sU(""), double titlesize=14.4, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0));
	splotlegend* addlegend(double xpos, double ypos, double width, double height, sci::string title=sU(""), double titlesize=12, double titledistance=2.0, double titlespacing=0.2, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0), wxColour outlinecolour=wxColour(0,0,0), int outlinewidth=1);
	splot2d* addcanvas(double xpos, double ypos, double width, double height, bool logx=false, bool logy=false);
	bool writetofile(sci::string filename, double sizemultiplier, bool preferInkscape);
	bool writetofile(sci::string filename, int width, int height, double linewidthmultiplier, bool preferInkscape);
	bool print( bool showDialog );
	void removeplot(splot *plot);
	void moveplot(splot* plot, double xpos, double ypos, double width, double height);
private:
	bool print( bool showDialog, sci::string printerName );
	//vector of plots
	std::vector<splot2d*> m_plots;
	std::vector<splotlegend*> m_legends;
	

	//bools to indicate the plots have changed
	bool m_plotsupdated;

	//bitmap we draw on
	wxBitmap *m_bitmap;
	bool m_fixedsize; //if this is true then the bitmap/plots do not scale with the window;
	int m_bitmapwidth;
	int m_bitmapheight;

	//location and size of each plot  within window normalised to the bitmap size 0.0-1.0
	std::vector<double> m_plotxloc; 
	std::vector<double> m_plotyloc;
	std::vector<double> m_plotwidth;
	std::vector<double> m_plotheight;
	
	//location and size of each plot  within window normalised to the bitmap size 0.0-1.0
	std::vector<double> m_legendxloc; 
	std::vector<double> m_legendyloc;
	std::vector<double> m_legendwidth;
	std::vector<double> m_legendheight;

	bool m_antialiasing;

	void OnPaint(wxPaintEvent &event);
	
	
	//note the backends are 0 = DC, 1 = AGG, 2 = GC
	void DrawPlots(wxDC *dc, int width, int height, double linewidthmultiplier=1.0);
	void OnEraseBackGround(wxEraseEvent& event) {};//this is overloaded to stop flicker when resizing

	DECLARE_EVENT_TABLE();
};


class splotframe : public wxFrame
{
public:
	splotframe(wxWindow* parent, bool antialiasing, wxWindowID id=wxID_ANY, const wxString& title=wxT("Apex++ plot"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("Sci++ Plot"));
	virtual ~splotframe(){};
	inline splot2d* addplot(double xpos=0.1, double ypos=0.1, double width=0.8, double height=0.8, bool logx=false, bool logy=false, sci::string title=sU(""), double titlesize=1.2, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0)){return m_plotwind->addplot(xpos, ypos, width, height, logx, logy, title, titlesize, titledistance, titlefont, titlestyle, titlecolour);};
	inline splot2d* addplot(double minx, double maxx, double miny, double maxy, double xpos=0.1, double ypos=0.1, double width=0.8, double height=0.8, bool logx=false, bool logy=false, sci::string title=sU(""), double titlesize=14.4, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0)){return m_plotwind->addplot(xpos, ypos, width, height, minx, maxx, miny, maxy, logx, logy, title, titlesize, titledistance, titlefont, titlestyle, titlecolour);};
	inline splot2d* addplot(double minx, double maxx, double miny, double maxy, double xintersect, double yintersect, double xpos=0.1, double ypos=0.1, double width=0.8, double height=0.8, bool logx=false, bool logy=false, sci::string title=sU(""), double titlesize=14.4, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0)){return m_plotwind->addplot(xpos, ypos, width, height, minx, maxx, miny, maxy, xintersect, yintersect, logx, logy, title, titlesize, titledistance, titlefont, titlestyle, titlecolour);};
	inline splotlegend* addlegend(double xpos, double ypos, double width, double height, sci::string title, double titlesize=12, double titledistance=2.0, double titlespacing=0.2, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0), wxColour outlinecolour=wxColour(0,0,0), int outlinewidth=1){return m_plotwind->addlegend(xpos, ypos, width, height, title, titlesize, titledistance,titlespacing, titlefont, titlestyle, titlecolour, outlinecolour, outlinewidth);};
	splotlegend* addlegend(double xpos, double ypos, double width, double height, wxColour outlinecolour=wxColour(0,0,0), int outlinewidth=1){return m_plotwind->addlegend(xpos, ypos, width, height, sU(""), 0.2, 2.0, 0.2, sU(""), 0, wxColour(0,0,0), outlinecolour, outlinewidth);};
	splot2d* addcanvas(double xpos, double ypos, double width, double height, bool logx=false, bool logy=false){return m_plotwind->addcanvas(xpos,ypos,width,height,logx,logy);}
	bool writetofile(sci::string filename, int sizemultiplier=1, bool preferInkscape=false){return m_plotwind->writetofile(filename,sizemultiplier, preferInkscape);};
	bool writetofile(sci::string filename, int width, int height, double linewidthmultiplier=1.0, bool preferInkscape=false){return m_plotwind->writetofile(filename,width,height,linewidthmultiplier, preferInkscape);};
	bool print(bool showDialog){return m_plotwind->print( showDialog );}
	inline void removeplot(splot *plot){m_plotwind->removeplot(plot);}
private:
	splotwindow *m_plotwind;
};


class wxDCScaler
{
public:
	wxDCScaler(wxDC *dc, double xScale, double yScale)
	{
		m_dc=dc;
		dc->GetUserScale( &m_xScale, &m_yScale );
		dc->SetUserScale( xScale, yScale );
	}
	~wxDCScaler()
	{
		m_dc->SetUserScale( m_xScale, m_yScale );
	}
private:
	wxDC *m_dc;
	double m_xScale;
	double m_yScale;
};

class splotPrintout : public wxPrintout
{
public:
	splotPrintout(splotwindow *window, wxString printerName);
	void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);
	bool HasPage( int pageNum );
	bool OnPrintPage(int pageNum);
	bool setup(bool showDialog, wxString printerName);
	wxPrintDialogData getPrintDialogData();
private:
	wxPageSetupDialogData m_pageSetupDialogData;
	splotwindow *m_window;

};

#endif