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
#include"../../graphics.h"

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



int sploterrorcatcher(const char *message);
class PlotData2dStructured;



class PlotScale
{
	//only allow construction by these (derived) types
	//then we can control the m_direction variable and
	//ensure splotaxis is horizontal and vertical and
	//splotcolourscale and splotsizescale are none.
	friend class splotaxis;
	friend class splotcolourscale;
	friend class splotsizescale;
	friend class splotlevelscale;
	friend class splotinterpolatedscale;
public:
	virtual ~PlotScale() {}
	enum class Direction
	{
		horizontal,
		vertical,
		none
	};
	//void setMin(double min) { m_min = min; }
	//void setMax(double max) { m_max = max; }
	double getMin() const { return m_log ? std::pow(10, m_logMin) : m_min; }
	double getMax() const { return m_log ? std::pow(10, m_logMax) : m_max; }
	double getLogMin() const { return m_logMin; }
	double getLogMax() const { return m_logMax; }
	bool isAutoscale() const { return m_autoscale; }
	bool isLog() const { return m_log; }
	void expand(const std::vector<double>& data);
	void expand(double value)
	{
		if (!m_autoscale)
			return;
		if (value != value)
			return;
		if (value == std::numeric_limits<double>::infinity())
			return;
		if (value == -std::numeric_limits<double>::infinity())
			return;
		if (m_minPoint != m_minPoint || value < m_minPoint)
		{
			if (!m_log || value > 0.0)
			{
				m_minPoint = value;
				m_min = m_minPoint - m_autoscaleEndSpace * (m_maxPoint - m_minPoint);
				m_max = m_maxPoint + m_autoscaleEndSpace * (m_maxPoint - m_minPoint);
				m_logMinPoint = std::log10(value);
				m_logMin = m_logMinPoint - m_autoscaleEndSpace * (m_logMaxPoint - m_logMinPoint);
				m_logMax = m_logMaxPoint + m_autoscaleEndSpace * (m_logMaxPoint - m_logMinPoint);
			}
		}
		if (m_maxPoint != m_maxPoint || value > m_maxPoint)
		{
			m_maxPoint = value;
			m_min = m_minPoint - m_autoscaleEndSpace * (m_maxPoint - m_minPoint);
			m_max = m_maxPoint + m_autoscaleEndSpace * (m_maxPoint - m_minPoint);
			m_logMaxPoint = std::log10(value);
			m_logMin = m_logMinPoint - m_autoscaleEndSpace * (m_logMaxPoint - m_logMinPoint);
			m_logMax = m_logMaxPoint + m_autoscaleEndSpace * (m_logMaxPoint - m_logMinPoint);
		}
	}
	void contract()
	{
		if (!m_autoscale)
			return;
		m_min = std::numeric_limits<double>::quiet_NaN();
		m_max = std::numeric_limits<double>::quiet_NaN();
		m_logMin = std::numeric_limits<double>::quiet_NaN();
		m_logMax = std::numeric_limits<double>::quiet_NaN();
		m_minPoint = std::numeric_limits<double>::quiet_NaN();
		m_maxPoint = std::numeric_limits<double>::quiet_NaN();
		m_logMinPoint = std::numeric_limits<double>::quiet_NaN();
		m_logMaxPoint = std::numeric_limits<double>::quiet_NaN();
	}
	void setAutoscale(double autoscaleEndSpace)
	{
		m_autoscale = true;
		m_autoscaleEndSpace = autoscaleEndSpace;
		contract();
	}
	void setFixedScale(double min, double max)
	{
		m_autoscale = false;
		m_min = min;
		m_max = max;
		m_logMin = std::log10(min);
		m_logMax = std::log10(max);
		m_minPoint = std::numeric_limits<double>::quiet_NaN();
		m_maxPoint = std::numeric_limits<double>::quiet_NaN();
		m_logMinPoint = std::numeric_limits<double>::quiet_NaN();
		m_logMaxPoint = std::numeric_limits<double>::quiet_NaN();
		m_autoscaleEndSpace = std::numeric_limits<double>::quiet_NaN();
	}
	void setLog()
	{
		m_log = true;
		if (m_autoscale)
			contract();
	}
	void setLinear()
	{
		m_log = false;
		if (m_autoscale)
			contract();
	}
	Direction getDirection()
	{
		return m_direction;
	}
	template<class T>
	void setupInterpolatingScale(std::vector<double>& value, std::vector<double>& logValue, std::vector<T>& output, bool autostretch)
	{
		sci::assertThrow(value.size() > 1 && value.size() == output.size(), sci::err(sci::SERR_PLOT, colourscaleErrorCode, "PlotScale::setupInterpolatingScale called with invalid sizes for the values or colours array."));
		//check values are ascending or descending, catch Nans at the same time
		bool ascending = true;
		bool descending = true;
		for (size_t i = 1; i < value.size(); ++i)
		{
			ascending &= value[i] >= value[i - 1];
			descending &= value[i] <= value[i - 1];
		}
		bool monotonic = ascending || descending;
		sci::assertThrow(monotonic, sci::err(sci::SERR_PLOT, colourscaleErrorCode, "splotcolourscale::setup called with values which are neither monotonically ascending nor monotonically descending."));

		//assign values
		if (descending)
		{
			value = sci::reverse(value);
			output = sci::reverse(output);
		}

		//remove any nans and infinities - in particular they can be created by the logging
		std::vector<SBOOL> filter = sci::isEq(value, value) && sci::notEq(value, std::numeric_limits<double>::infinity()) && sci::notEq(value, -std::numeric_limits<double>::infinity());
		value = sci::subvector(value, filter);
		output = sci::subvector(output, filter);
		sci::assertThrow(value.size() > 1, sci::err(sci::SERR_PLOT, colourscaleErrorCode, "splotcolourscale::setup called with a values array containing only NaNs and +/- infnity."));


		if (!autostretch)
		{
			setFixedScale(value.front(), value.back());
		}

		logValue.resize(value.size());
		for (size_t i = 0; i < value.size(); ++i)
			logValue[i] = std::log10(value[i]);

		//scale to 0.0-1.0 range
		double offset = value.front();
		double range = value.back() - value.front();
		value -= offset;
		value /= range;

		double logOffset = logValue.front();
		double logRange = logValue.back() - logValue.front();
		logValue -= logOffset;
		logValue /= logRange;
	}
private:
	PlotScale(double min, double max, bool log, Direction direction)
	{
		m_direction = direction;
		setFixedScale(min, max);
		m_log = log;
	}
	PlotScale(bool log, Direction direction, double autoscaleEndSpace)
	{
		m_direction = direction;
		m_autoscale = true;
		m_log = log;
		m_autoscaleEndSpace = autoscaleEndSpace;
		contract();
	}
	PlotScale(Direction direction, double autoscaleEndSpace)
	{
		m_direction = direction;
		m_autoscale = true;
		m_log = false;
		m_autoscaleEndSpace = autoscaleEndSpace;
		contract();
	}
	double m_min;
	double m_max;
	double m_logMin;
	double m_logMax;
	double m_minPoint;
	double m_maxPoint;
	double m_logMinPoint;
	double m_logMaxPoint;
	bool m_log;
	bool m_autoscale;
	double m_autoscaleEndSpace;
	Direction m_direction;
};

class splotcolourscale : public PlotScale
{
	friend class splot;
	friend class splot2d;
	friend class splotlegend;
public:
	splotcolourscale();
	//create a colourscale from rgb colours. Use the same number of values in the value and colour vectors to create a continuous colour scale or one more in the value vector to create a discrete colour scale
	splotcolourscale(const std::vector<double> &value, const std::vector< rgbcolour > &colour, bool logarithmic=false, bool autostretch=false, bool fillOffscaleBottom = false, bool fillOffscaleTop = false);
	//create a colourscale from hls colours. Use the same number of values in the value and colour vectors to create a continuous colour scale or one more in the value vector to create a discrete colour scale
	splotcolourscale(const std::vector<double> &value, const std::vector< hlscolour > &colour, bool logarithmic=false, bool autostretch=false, bool fillOffscaleBottom = false, bool fillOffscaleTop = false);
	//note that valuePrelogged is only utilised if this is a log scale
	rgbcolour getRgbOriginalScale( double value, bool valuePrelogged) const;
	//note that valuePrelogged is only utilised if this is a log scale
	hlscolour getHlsOriginalScale( double value, bool valuePrelogged) const;
	rgbcolour getRgbOffscaleBottom() const;
	rgbcolour getRgbOffscaleTop() const;
	hlscolour getHlsOffscaleBottom() const;
	hlscolour getHlsOffscaleTop() const;
	virtual ~splotcolourscale(){};
	void setupForImage(plstream* pl) const;
	void setupForShade(plstream *pl) const;
	bool isDiscrete() const { return m_discrete; }
	std::vector<double> getDiscreteValues() const;
	bool fillOffscaleBottom() const { return m_fillOffscaleBottom; }
	bool fillOffscaleTop() const { return m_fillOffscaleTop; }
	bool setFilOffscaleBottom(bool fill) {m_fillOffscaleBottom = fill; }
	bool setFilOffscaleTop(bool fill) {m_fillOffscaleTop = fill; }
private:
	void setupdefault();
	void setup(const std::vector<double> &value, const std::vector< rgbcolour > &colour, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop);
	void setup(const std::vector<double> &value, const std::vector< hlscolour > &colour, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop);
	std::vector<double> m_value;
	std::vector<double> m_logValue;
	std::vector< double > m_colour1;
	std::vector< double > m_colour2;
	std::vector< double > m_colour3;
	std::vector< double > m_alpha;
	bool m_hls; //true for hls, false for rgb colour representation
	bool m_discrete;
	bool m_fillOffscaleBottom;
	bool m_fillOffscaleTop;
	//note that valuePrelogged is only utilised if this is a log scale
	void interpolate( double value, double &c1, double &c2, double &c3, double &a, bool valuePrelogged) const;
};


class splotsizescale : public PlotScale
{
	friend class splot;
	friend class splot2d;
	friend class splotlegend;
public:
	splotsizescale(const std::vector<double> &value=std::vector<double>(0), const std::vector<double> &size=std::vector<double>(0), bool logarithmic=false, bool autostretch = false, bool fillOffscaleBottom = false, bool fillOffscaleTop = false);
	~splotsizescale(){};
	//note that valuePrelogged is only utilised if this is a log scale
	double getsize(double value, bool valuePreLogged) const;
	bool fillOffscaleBottom() const { return m_fillOffscaleBottom; }
	bool fillOffscaleTop() const { return m_fillOffscaleTop; }
	bool setFilOffscaleBottom(bool fill) { m_fillOffscaleBottom = fill; }
	bool setFilOffscaleTop(bool fill) { m_fillOffscaleTop = fill; }
private:
	std::vector<double> m_value;
	std::vector<double> m_logValue;
	std::vector<double> m_size;
	bool m_fillOffscaleBottom;
	bool m_fillOffscaleTop;
};


class splotlevelscale : public PlotScale
{
	friend class splot;
	friend class splot2d;
	friend class splotlegend;
public:
	splotlevelscale(const std::vector<double>& value = std::vector<double>(0), bool logarithmic = false, bool autostretch = false);
	~splotlevelscale() {};
	std::vector<double> getLevels() const;
private:
	std::vector<double> m_value;
	std::vector<double> m_logValue;
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

class DrawableItem
{
public:
	virtual ~DrawableItem() {}
	virtual void preDraw() = 0;
	virtual void draw(plstream* pl, double scale, double pageWidth, double pageHeight) = 0;
	virtual void draw(Renderer& renderer, grPerMillimetre scale)
	{
	}
	virtual bool readyToDraw() const = 0;
};

class splotaxis: public PlotScale, public DrawableItem
{
	friend class splot;
	friend class splot2d;
public:
	splotaxis(double min, double max, bool log, Direction direction, double positionStart, double positionEnd, double perpendicularPosition, sci::string title = sU("Axis Title"), sci::string titlefont = sU(""), PLUNICODE titlestyle = 0, double titlesize = 12.0, double titledistance = 3.5, const rgbcolour& titlecolour = rgbcolour(0, 0, 0), double intersectpoint = 0.0, rgbcolour colour = rgbcolour(0, 0, 0), int linethickness = 1, double majorticklength = 4.0, double minorticklength = 2.0, bool tickspositive = false, bool ticksnegative = true, bool showlabels = true, bool labelpositionpositive = false, sci::string labelfont = sU(""), PLUNICODE labelstyle = 0, bool labelsrotated = false, double labelsize = 9.6, const rgbcolour& labelcolour = rgbcolour(0, 0, 0), bool autodecimalplaces = true, unsigned int ndecimalplaces = 0, bool automaxndigits = true, int maxndigits = 0);
	splotaxis(bool log, Direction direction, double positionStart, double positionEnd, double perpendicularPosition, sci::string title = sU("Axis Title"), sci::string titlefont = sU(""), PLUNICODE titlestyle = 0, double titlesize = 12.0, double titledistance = 3.5, const rgbcolour& titlecolour = rgbcolour(0, 0, 0), double intersectpoint = 0.0, rgbcolour colour = rgbcolour(0, 0, 0), int linethickness = 1, double majorticklength = 4.0, double minorticklength = 2.0, bool tickspositive = false, bool ticksnegative = true, bool showlabels = true, bool labelpositionpositive = false, sci::string labelfont = sU(""), PLUNICODE labelstyle = 0, bool labelsrotated = false, double labelsize = 9.6, const rgbcolour& labelcolour = rgbcolour(0, 0, 0), bool autodecimalplaces = true, unsigned int ndecimalplaces = 0, bool automaxndigits = true, int maxndigits = 0);
	splotaxis(double min, double max, bool log, Direction direction, double positionStart, double positionEnd, double perpendicularPosition, double majorinterval, double nsubticks, sci::string title = sU("Axis Title"), sci::string titlefont = sU(""), PLUNICODE titlestyle = 0, double titlesize = 12.0, double titledistance = 3.5, const rgbcolour& titlecolour = rgbcolour(0, 0, 0), double intersectpoint = 0.0, rgbcolour colour = rgbcolour(0, 0, 0), int linethickness = 1, double majorticklength = 4.0, double minorticklength = 2.0, bool tickspositive = false, bool ticksnegative = true, bool showlabels = true, bool labelpositionpositive = false, sci::string labelfont = sU(""), PLUNICODE labelstyle = 0, bool labelsrotated = false, double labelsize = 9.6, const rgbcolour& labelcolour = rgbcolour(0, 0, 0), bool autodecimalplaces = true, unsigned int ndecimalplaces = 0, bool automaxndigits = true, int maxndigits = 0);
	splotaxis(bool log, Direction direction, double positionStart, double positionEnd, double perpendicularPosition, double majorinterval, double nsubticks, sci::string title = sU("Axis Title"), sci::string titlefont = sU(""), PLUNICODE titlestyle = 0, double titlesize = 12.0, double titledistance = 3.5, const rgbcolour& titlecolour = rgbcolour(0, 0, 0), double intersectpoint = 0.0, rgbcolour colour = rgbcolour(0, 0, 0), int linethickness = 1, double majorticklength = 4.0, double minorticklength = 2.0, bool tickspositive = false, bool ticksnegative = true, bool showlabels = true, bool labelpositionpositive = false, sci::string labelfont = sU(""), PLUNICODE labelstyle = 0, bool labelsrotated = false, double labelsize = 9.6, const rgbcolour& labelcolour = rgbcolour(0, 0, 0), bool autodecimalplaces = true, unsigned int ndecimalplaces = 0, bool automaxndigits = true, int maxndigits = 0);
	~splotaxis(){};
	inline void setcolour(rgbcolour colour){m_colour=colour; m_haschanged=true;};
	inline void setlinethickness(int thickness){m_linethickness=thickness; m_haschanged=true;};
	inline void automajorintervalon(){m_automajorinterval=true; m_haschanged=true;};
	inline void autominorintervalon(){m_autonsubticks=true; m_haschanged=true;};
	inline void autointervalson();
	void setmajorinterval(double interval);
	void setnsubticks(unsigned int nsubticks);
	inline void settitle(sci::string title){m_title=title; m_haschanged=true;};
	inline void setrotatetitle(bool rotated){m_rotatetitle=rotated; m_haschanged=true;};
	inline void settitlesize(double size){m_titlesize=size; m_haschanged=true;};
	inline void settitlecolour(rgbcolour colour){m_titlecolour=colour; m_haschanged=true;};
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
	inline void setlabelcolour(rgbcolour colour){m_labelcolour=colour; m_haschanged=true;};
	inline void setndecimalplaces(unsigned int ndecimalplaces){m_autodecimalplaces=false; m_ndecimalplaces=ndecimalplaces;};
	inline void setautodecimalplaces(){m_autodecimalplaces=true;};
	inline void setmaxndigits(int maxndigits){m_maxndigits=maxndigits;};
	inline void setautomaxndigits(){m_maxndigits=0;};
	inline void settimeformat(sci::string format){m_timeformat=format; m_customlabelcreator=NULL;}; //set to "" to use number format else set as per the strftime function
	inline void setcustomlabel(sci::string (*customlabelcreator)(double axisvalue)){m_customlabelcreator=customlabelcreator; m_timeformat=sU("");}; //set to NULL to use normal formatting or a pointer to a function to interpret the axis value otherwise
	inline void setPosition(double start, double end) { m_positionStart = start; m_positionEnd = end; }
	inline void getPosition(double& start, double& end) { start = m_positionStart; end = m_positionEnd; }
	inline const sci::string& getTitle() const { return m_title; }
																																					//note no public function to set limits, or intersect or set the axis as logarithmic as the 
	//splot class needs to keep track of these parameters for autolimits and log plotting
	//purposes

	void preDraw() override
	{
	}
	void draw(plstream* pl, double scale, double pageWidth, double pageHeight) override;
	void draw(Renderer& renderer, grPerMillimetre scale) override;
	bool readyToDraw() const override
	{
		return true;
	}
	double plotToPage(double value)
	{
		double fraction;
		if(isLog())
			fraction = (std::log10(value) - getLogMin()) / (getLogMax() - getLogMin());
		else
			fraction = (value - getMin()) / (getMax() - getMin());
		if (m_reversed)
			fraction = 1.0 - fraction;
		return m_positionStart + (m_positionEnd - m_positionStart) * fraction;
	}
	double pageToPlot(double value)
	{
		double fraction = (value - m_positionStart) / (m_positionEnd - m_positionStart);
		if (m_reversed)
			fraction = 1.0 - fraction;
		if (isLog())
			return std::pow(10, fraction * (getLogMax() - getLogMin()) + getLogMin());
		return fraction * (getMax() - getMin()) + getMin();
	}
private:
	
	void setup(double positionStart, double positionEnd, double perpendicularPosition, sci::string title, sci::string titlefont, PLUNICODE titlestyle, double titlesize, double titledistance, const rgbcolour& titlecolour, double intersectpoint, rgbcolour colour, int linethickness, double majorticklength, double minorticklength, bool tickspositive, bool ticksnegative, bool showlabels, bool labelpositionpositive, sci::string labelfont, PLUNICODE labelstyle, bool labelsrotated, double labelsize, const rgbcolour& labelcolour, bool autodecimalplaces, unsigned int ndecimalplaces, bool automaxndigits, int maxndigits);

	void drawLinear(Renderer& renderer, grPerMillimetre scale);
	void drawLog(Renderer& renderer, grPerMillimetre scale);
	void drawTick(Renderer& renderer, grPerMillimetre scale, double plotPosition, bool minor);
	grMillimetre drawLabel(Renderer& renderer, grPerMillimetre scale, double plotPosition); //returns the sise of the label perpedicular to the axis
	void drawTitle(Renderer& renderer, grPerMillimetre scale, grMillimetre distanceFromAxis);

	std::string createploptstring() const;
	//keep track of if the axis has changed
	bool m_haschanged;

	//location in terms of the coordinates in the other axis
	double m_intersect;

	//colour
	rgbcolour m_colour;

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
	rgbcolour m_labelcolour;

	//axis title
	sci::string m_title;
	bool m_rotatetitle;
	double m_titlesize; //scale to be defined
	sci::string m_titlefont;
	PLUNICODE m_titlefci;
	rgbcolour m_titlecolour;
	double m_titledistance; //distance from the axis in multiples of character height
	

	//time/custom format options
	sci::string m_timeformat;
	sci::string (*m_customlabelcreator)(double axisvalue);

	bool m_autodecimalplaces;
	unsigned int m_ndecimalplaces;

	unsigned int m_maxndigits; //maximum number of digits before going to exponents, set to zero for auto

	double m_positionStart;
	double m_positionEnd;
	double m_perpendicularPosition;
	bool m_reversed;
};

class CubehelixColourscale : public splotcolourscale
{
public:
	CubehelixColourscale(const std::vector<double>& values, double startHue, double hueRotation, double startBrightness, double endBrightness, double saturation, double gamma, bool logarithmic, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop, bool discrete)
		: splotcolourscale(values, CubehelixColourscale::getCubehelixColours(values, logarithmic, startHue, hueRotation, startBrightness, endBrightness, saturation, gamma, discrete),
			logarithmic, autostretch, fillOffscaleBottom, fillOffscaleTop)
	{

	}
	CubehelixColourscale(double minValue, double maxValue, size_t nPoints, double startHue, double hueRotation, double startBrightness, double endBrightness, double saturation, double gamma, bool logarithmic, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop, bool discrete)
		: splotcolourscale(getEvenlyDistributedValues(minValue, maxValue, nPoints, logarithmic),
			getCubehelixColours(minValue, maxValue, nPoints, logarithmic, startHue, hueRotation, startBrightness, endBrightness, saturation, gamma, discrete),
			logarithmic, autostretch, fillOffscaleBottom, fillOffscaleTop)
	{

	}
	CubehelixColourscale(const std::vector<double>& values, bool logarithmic, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop, bool discrete)
		: splotcolourscale(values, CubehelixColourscale::getCubehelixColours(values, logarithmic, 180, 540, 0.8, 0.2, 1.0, 1.0, discrete),
			logarithmic, autostretch, fillOffscaleBottom, fillOffscaleTop)
	{

	}
	CubehelixColourscale(double minValue, double maxValue, size_t nPoints, bool logarithmic, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop, bool discrete)
		: splotcolourscale(getEvenlyDistributedValues(minValue, maxValue, nPoints, logarithmic),
			getCubehelixColours(minValue, maxValue, nPoints, logarithmic, 180, 540, 0.8, 0.2, 1.0, 1.0, discrete),
			logarithmic, autostretch, fillOffscaleBottom, fillOffscaleTop)
	{

	}
	static std::vector<rgbcolour> getCubehelixColours(double minValue, double maxValue, size_t nPoints, bool logarithmic, double startHue, double hueRotation, double startBrightness, double endBrightness, double saturation, double gamma, bool discrete)
	{
		if (logarithmic)
		{
			minValue = std::log10(minValue);
			maxValue = std::log10(maxValue);
		}
		std::vector<double> values(nPoints);
		for (size_t i = 0; i < nPoints; ++i)
		{
			double f = (double)i / double(nPoints - 1);
			values[i] = minValue * (1 - f) + maxValue * f;
		}
		//call the other get colours funtion with the derived values. Note we always set logaritmic to false in this call as we have already logged the values.
		return getCubehelixColours(values, false, startHue, hueRotation, startBrightness, endBrightness, saturation, gamma, discrete);
	}
	static std::vector<rgbcolour> getCubehelixColours(std::vector<double> values, bool logarithmic, double startHue, double hueRotation, double startBrightness, double endBrightness, double saturation, double gamma, bool discrete)
	{
		if (logarithmic)
			for (double& v : values)
				v = std::log10(v);
		if (discrete)
		{
			std::vector<double> newValues(values.size() - 1);

			for (size_t i = 0; i < newValues.size(); ++i)
				newValues[i] = (values[i] + values[i + 1]) / 2.0;
			return getCubehelixColours(newValues, false, startHue, hueRotation, startBrightness, endBrightness, saturation, gamma, false);
		}
		//make the hue consistent with the start parameter used in D. A. Green 2011
		startHue -= std::floor(startHue / 360.0) * 360.0;
		startHue /= 120;
		startHue += 1;

		//create a vector to hold the colours
		std::vector<rgbcolour> colours(values.size());

		//calculate each colour
		double valuesRange = values.back() - values.front();
		double brightnessRange = endBrightness - startBrightness;
		for (size_t i = 0; i < colours.size(); ++i)
		{
			double brightness = startBrightness + (values[i] - values[0]) / valuesRange * brightnessRange;
			double angle = M_2PI * (startHue / 3.0 + 1.0 + hueRotation / 360.0 * brightness);
			double gammaBrightness = std::pow(brightness, gamma);
			double amplitude = saturation * gammaBrightness * (1 - gammaBrightness) / 2.0;
			double red = gammaBrightness + amplitude * (-0.14861 * std::cos(angle) + 1.78277 * std::sin(angle));
			double green = gammaBrightness + amplitude * (-0.29227 * std::cos(angle) - 0.90649 * std::sin(angle));
			double blue = gammaBrightness + amplitude * (1.97294 * std::cos(angle));
			colours[i] = rgbcolour(red, green, blue);
		}
		//std::fstream fout;
		//fout.open("colourscale", std::ios::out);
		//for (size_t i = 0; i < colours.size(); ++i)
		//	fout << values[i] << "," << colours[i].r() << "," << colours[i].g() << "," << colours[i].b() << "\n";
		//fout.close();
		return colours;
	}
private:
	static std::vector<double> getEvenlyDistributedValues(double minValue, double maxValue, size_t nPoints, bool logarithmic)
	{
		if (logarithmic)
		{
			minValue = std::log10(minValue);
			maxValue = std::log10(maxValue);
			std::vector<double> values(nPoints);
			for (size_t i = 0; i < nPoints; ++i)
			{
				double f = (double)i / double(nPoints - 1);
				values[i] = std::pow(10,minValue * (1 - f) + maxValue * f);
			}
			return values;
		}
		std::vector<double> values(nPoints);
		for (size_t i = 0; i < nPoints; ++i)
		{
			double f = (double)i / double(nPoints - 1);
			values[i] = minValue * (1 - f) + maxValue * f;
		}
		return values;
	}
};

class splothorizontalcolourbar : public DrawableItem
{
public:
	splothorizontalcolourbar(double horizontalStart, double horizontalEnd, double verticalStart, double verticalEnd,
		std::shared_ptr<splotcolourscale> colourscale, sci::string title, sci::string titlefont = sU(""), PLUNICODE titlestyle = 0, double titlesize = 12.0,
		double titledistance = 2.5, const rgbcolour& titlecolour = rgbcolour(0, 0, 0), double intersectpoint = 0.0, rgbcolour colour = rgbcolour(0, 0, 0),
		int linethickness = 1, double majorticklength = 0.8, double minorticklength = 0.5, bool tickspositive = false,
		bool ticksnegative = true, bool showlabels = true, bool labelpositionpositive = false, sci::string labelfont = sU(""), PLUNICODE labelstyle = 0,
		bool labelsrotated = false, double labelsize = 9.6, const rgbcolour& labelcolour = rgbcolour(0, 0, 0), bool autodecimalplaces = true,
		unsigned int ndecimalplaces = 0, bool automaxndigits = true, int maxndigits = 0)
	{
		m_horizontalStart = horizontalStart;
		m_horizontalEnd = horizontalEnd;
		m_verticalStart = verticalStart;
		m_verticalEnd = verticalEnd;
		m_title = title;
		m_colourscale = colourscale;

		m_timeformat = sU("");
		m_customlabelcreator = NULL;
		m_colour = colour;
		m_linethickness = linethickness;
		m_tickspos = tickspositive;
		m_ticksneg = ticksnegative;
		m_majorticklength = std::abs(majorticklength);
		m_minorticklength = std::abs(minorticklength);
		m_showlabels = showlabels;
		m_labelfont = labelfont;
		m_labelfci = labelstyle;
		m_rotatelabels = labelsrotated;
		m_labelsize = labelsize;
		m_labelcolour = labelcolour;
		m_labelpositionpositive = labelpositionpositive;
		m_title = title;
		m_titlefont = titlefont;
		m_titlefci = titlestyle;
		m_rotatetitle = false;
		m_titlesize = titlesize;
		m_titledistance = titledistance;
		m_titlecolour = titlecolour;
		m_autodecimalplaces = autodecimalplaces;
		m_ndecimalplaces = ndecimalplaces;
		if (automaxndigits == true)
			m_maxndigits = 0;
		else
			m_maxndigits = maxndigits;
	}
	void preDraw() override
	{
	}
	void draw(plstream* pl, double scale, double pageWidth, double pageHeight) override;
	bool readyToDraw() const override
	{
		return true;
	}
	void setTitle(sci::string title)
	{
		m_title = title;
	}
private:
	std::shared_ptr<splotcolourscale> m_colourscale;
	double m_horizontalStart;
	double m_horizontalEnd;
	double m_verticalStart;
	double m_verticalEnd;

	rgbcolour m_colour;

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
	rgbcolour m_labelcolour;

	//axis title
	sci::string m_title;
	bool m_rotatetitle;
	double m_titlesize; //scale to be defined
	sci::string m_titlefont;
	PLUNICODE m_titlefci;
	rgbcolour m_titlecolour;
	double m_titledistance; //distance from the axis in multiples of character height


	//time/custom format options
	sci::string m_timeformat;
	sci::string(*m_customlabelcreator)(double axisvalue);

	bool m_autodecimalplaces;
	unsigned int m_ndecimalplaces;

	unsigned int m_maxndigits; //maximum number of digits before going to exponents, set to zero for auto

	double m_positionStart;
	double m_positionEnd;
	double m_perpendicularPosition;
};

class splotLabel : public DrawableItem
{
public:
	splotLabel(sci::string text, double xPosition, double yPosition, double size = 12.0, double alignment = 0.0, sci::string font = sU(""), PLUNICODE style = 0, rgbcolour colour = rgbcolour(0.0, 0.0, 0.0))
		:m_text(text), m_xPosition(xPosition), m_yPosition(yPosition), m_size(size), m_alignment(alignment), m_font(font), m_style(style), m_colour(colour)
	{

	}
	virtual void preDraw() override
	{};
	virtual void draw(plstream* pl, double scale, double pageWidth, double pageHeight);
	virtual bool readyToDraw() const override
	{
		return true;
	}
private:
	sci::string m_text;
	double m_xPosition;
	double m_yPosition;
	double m_size;
	double m_alignment;
	sci::string m_font;
	PLUNICODE m_style;
	rgbcolour m_colour;
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
	splot()
	{
		m_haschanged = true;
	}
	virtual ~splot(){}
	//inline void settransform(void (*transformfunc)(double xin, double yin, double &xout, double &yout)){m_coordtransformer.m_transformfunc=transformfunc; m_haschanged=true;};
	inline void settitle(sci::string title){m_title=title; m_haschanged=true;};
	inline void settitlesize(double size){m_titlesize=size; m_haschanged=true;};
	inline void settitledistance(double distance){m_titledistance=distance; m_haschanged=true;};
	inline void settitlefont(sci::string fontface){m_titlefont=fontface; m_haschanged=true;};
	inline void settitlestyle(uint32_t style){m_titlefci=style; m_haschanged=true;};
	inline void settitlecolour(rgbcolour colour){m_titlecolour=colour; m_haschanged=true;};
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
	std::vector<rgbcolour> m_pointcolour;
	std::vector<rgbcolour> m_linecolour;
	std::vector<sci::string> m_linestyle;
	std::vector<std::shared_ptr<splotcolourscale>> m_colourscale; //if null shading won't be used for stuctured z data. Will be used for point colour for unstructured z data
	std::vector<std::shared_ptr<splotsizescale>> m_sizescale; //will be used to adjust point size for unstructured z
	std::vector< std::vector< double > > m_colourlevels; //colour levels used for colourscale plots
	std::vector<bool> m_filloffscaletop; //flag to state whether to colour offscale regions with the max colour or leave blank
	std::vector<bool> m_filloffscalebottom; //flag to state whether to colour offscale regions with the max colour or leave blank
	std::vector< std::vector< double > > m_contourlevels; //colour levels used for colourscale plots
	std::vector<double> m_contourlabelssize;
	std::vector<sci::string> m_map;
	std::vector<std::shared_ptr<splotTransformer>> m_transformers;
	std::vector<bool> m_linkcontoursandcolours;
	std::vector<std::vector<double> > m_arrowstyle;


	//title
	sci::string m_title;
	double m_titlesize;
	double m_titledistance;//in multiples of title character height
	sci::string m_titlefont;
	PLUNICODE m_titlefci;
	rgbcolour m_titlecolour;

	//function pointer transformation xindex, yindex to x value, y value
	void (*m_transfunc1dxy)(double xindex, double yindex, const std::vector<double> &x, const std::vector<double> &y, double &xout, double &yout);
	void (*m_transfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&);
	virtual void calculateautolimits()=0;
	static void calculateautolimits(splotaxis &axis, const std::vector<std::vector<double>> &data1d, const std::vector<std::vector<double>> &data1dpluserrs, const std::vector<std::vector<double>> &data1dminuserrs, const std::vector<std::vector<std::vector<double>>> &data2d, bool addpadding, std::vector<double> intersectpoints, double existingMin, double existingMax);

	void adddatasetproperties(rgbcolour pointcolour, double pointsize, sci::string pointsymbol, uint32_t pointstyle, sci::string pointfont, rgbcolour linecolour, int linewidth, sci::string linestyle, const splotcolourscale &colourscale, const splotsizescale &sizescale, unsigned int ncolourboundaries, bool filloffscaletop, bool filloffscalebottom, int ncontourlevels, double mincontour, double maxcontour, double contourlabelssize, sci::string map, const std::vector<double> &arrowstyle);
	void incrementdatasize();
	void setupcolourscale(int ncolours, const splotcolourscale& colourscale, bool filloffscaletop, bool filloffscalebottom, int ncontourlevels, double maxcontour, double mincontour, int contourlabelssize)
	{

	}
	//parser for generating line styles
	void parselinestyle(sci::string style, std::vector<PLINT> &marks, std::vector<PLINT> &spaces);
};


//a class which holds all the required info to generate a plot
class splot2d : public splot
{
	friend class splotwindow;
public:
	virtual ~splot2d(){}

	//functions to add data to the plots

	//x y scatter with constant colours and sizes
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &xposerrs=std::vector<double>(0), const std::vector<double> &xnegerrs=std::vector<double>(0), const std::vector<double> &yposerrs=std::vector<double>(0), const std::vector<double> &ynegerrs=std::vector<double>(0), rgbcolour pointcolour=rgbcolour(0,0,0), double pointsize=0.5, sci::string pointsymbol=sU("A"), rgbcolour linecolour=rgbcolour(0,0,0), double linewidth=1.0, sci::string linestyle=sU(""), rgbcolour xerrcolour=rgbcolour(0,0,0), rgbcolour yerrcolour=rgbcolour(0,0,0), double xerrwidth=1.0, double yerrwidth=1.0, std::shared_ptr<splotTransformer> transformer = NULL);
	//x y scatter with varying point colour
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, const std::vector<double> &xposerrs=std::vector<double>(0), const std::vector<double> &xnegerrs=std::vector<double>(0), const std::vector<double> &yposerrs=std::vector<double>(0), const std::vector<double> &ynegerrs=std::vector<double>(0), double pointsize=0.5, sci::string pointsymbol=sU("A"), rgbcolour linecolour=rgbcolour(0,0,0), double linewidth=1.0, sci::string linestyle=sU(""), rgbcolour xerrcolour=rgbcolour(0,0,0), rgbcolour yerrcolour=rgbcolour(0,0,0), double xerrwidth=1.0, double yerrwidth=1.0, std::shared_ptr<splotTransformer> transformer = NULL);
	//x y scatter with varying point size
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, std::shared_ptr<splotsizescale> sizescale, const std::vector<double> &xposerrs=std::vector<double>(0), const std::vector<double> &xnegerrs=std::vector<double>(0), const std::vector<double> &yposerrs=std::vector<double>(0), const std::vector<double> &ynegerrs=std::vector<double>(0), rgbcolour pointcolour=rgbcolour(0,0,0), sci::string pointsymbol=sU("A"), rgbcolour linecolour=rgbcolour(0,0,0), double linewidth=1.0, sci::string linestyle=sU(""), rgbcolour xerrcolour=rgbcolour(0,0,0), rgbcolour yerrcolour=rgbcolour(0,0,0), double xerrwidth=1.0, double yerrwidth=1.0, std::shared_ptr<splotTransformer> transformer = NULL);
	//x y scatter with varying point colour and size
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &colour_zs, const std::vector<double> &size_zs, std::shared_ptr<splotsizescale> sizescale, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, const std::vector<double> &xposerrs=std::vector<double>(0), const std::vector<double> &xnegerrs=std::vector<double>(0), const std::vector<double> &yposerrs=std::vector<double>(0), const std::vector<double> &ynegerrs=std::vector<double>(0), sci::string pointsymbol=sU("A"), rgbcolour linecolour=rgbcolour(0,0,0), double linewidth=1.0, sci::string linestyle=sU(""), rgbcolour xerrcolour=rgbcolour(0,0,0), rgbcolour yerrcolour=rgbcolour(0,0,0), double xerrwidth=1.0, double yerrwidth=1.0, std::shared_ptr<splotTransformer> transformer = NULL);
	//colourscale and/or contour plot
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, bool filloffscaletop=true, bool filloffscalebottom=true, rgbcolour contourcolour=rgbcolour(0,0,0), double contourwidth=1.0, sci::string contourstyle=sU(""), int ncontourlevels=-1, double mincontour=std::numeric_limits<double>::quiet_NaN(), double maxcontour=std::numeric_limits<double>::quiet_NaN(), double contourlabelssize=0.2, std::shared_ptr<splotTransformer> transformer = NULL);
	//colourscale and/or contour plot with 2d x and y
	void adddata(const std::vector< std::vector <double> > &xs, const std::vector< std::vector <double> > &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, bool filloffscaletop=true, bool filloffscalebottom=true, rgbcolour contourcolour=rgbcolour(0,0,0), double contourwidth=1.0, sci::string contourstyle=sU(""), unsigned int ncontourlevels=-1, double mincontour=std::numeric_limits<double>::quiet_NaN(), double maxcontour=std::numeric_limits<double>::quiet_NaN(), double contourlabelssize=0.2, std::shared_ptr<splotTransformer> transformer = NULL);
	//bar chart
	void adddata(const std::vector<double> &minedges, const std::vector<double> &maxedges, const std::vector<double> heights, rgbcolour fillcolour=rgbcolour(0,0,0),rgbcolour linecolour=rgbcolour(0,0,0), double linewidth=0.0, sci::string linestyle=sU(""), bool beginatzero=false);
	//vector/arrow plot
	void adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector< std::vector<double> > &us, const std::vector< std::vector<double> > &vs, rgbcolour linecolour=rgbcolour(0,0,0), double linewidth=1.0, const std::vector<double> &arrowstyle=std::vector<double>(0), sci::string linestyle=sU(""), std::shared_ptr<splotTransformer> transformer = NULL);
	//vector/arrow plot
	void adddata(const std::vector<std::vector<double>> &xs, const std::vector<std::vector<double>> &ys, const std::vector< std::vector<double> > &us, const std::vector< std::vector<double> > &vs, rgbcolour linecolour=rgbcolour(0,0,0), double linewidth=1.0, const std::vector<double> &arrowstyle=std::vector<double>(0), sci::string linestyle=sU(""), std::shared_ptr<splotTransformer> transformer = NULL);
	//grid plot
	void addShadedGrid(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, bool filloffscaletop=true, bool filloffscalebottom=true, std::shared_ptr<splotTransformer> transformer = NULL);
	//grid plot with 2d x and y
	void addShadedGrid(const std::vector< std::vector <double> > &xs, const std::vector< std::vector <double> > &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, bool filloffscaletop=true, bool filloffscalebottom=true, std::shared_ptr<splotTransformer> transformer = NULL);
	
	//add a DrawableItem
	void addData( std::shared_ptr<DrawableItem> drawableItem );

	void addmap(sci::string map=sU("globe"), rgbcolour linecolour=rgbcolour(0,0,0), double linewidth=1.0, sci::string linestyle=sU(""), std::shared_ptr<splotTransformer> transformer = NULL);
	void addImage(sci::string picture, double xBottomLeft, double yBottomLeft, double width, double height, int cropX0=0, int cropY0=0, int cropWidth=-1, int cropHeight=-1, double brightnessCorrection=0.0, double contrastCorrection=0.0);
	void addText(sci::string text, double x, double y, double x2, double y2, double alignment=0.0, double size=0.8, const sci::string &font=sU(""), uint32_t style=0, rgbcolour colour=rgbcolour(0,0,0));
	void setdatasetproperties(size_t setindex, rgbcolour pointcolour=rgbcolour(0,0,0), double pointsize=0.5, sci::string pointsymbol=sU("A"), rgbcolour linecolour=rgbcolour(0,0,0), int linewidth=1.0, rgbcolour xerrcolour=rgbcolour(0,0,0), rgbcolour yerrcolour=rgbcolour(0,0,0), int xerrwidth=1, int yerrwidth=1);

	void removeAllData();
	void removeData(std::shared_ptr<DrawableItem> drawableItem);

	inline void setxautointersect(bool autointersect) { m_xautointersect = autointersect; calculateautolimits(); m_haschanged = true; };
	inline void setyautointersect(bool autointersect) { m_yautointersect = autointersect; calculateautolimits(); m_haschanged = true; };
	inline void setxaxisintersect(double intersect) { m_xautointersect = false; m_xaxis.m_intersect = intersect; calculateautolimits(); m_haschanged = true; };
	inline void setyaxisintersect(double intersect) { m_yautointersect = false; m_yaxis.m_intersect = intersect; calculateautolimits(); m_haschanged = true; };
	inline splotaxis* getxaxis() { m_haschanged = true; return &m_xaxis; };
	inline splotaxis* getyaxis() { m_haschanged = true; return &m_yaxis; };
	void calculateautolimits() {}

private:
	//private constructor so only friends can create a plot
	splot2d(bool logx=false, bool logy=false,sci::string title=sU(""), double titlesize=1.2, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, rgbcolour titlecolour=rgbcolour(0,0,0),void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&)=splot_REGULAR1DXY,void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&)=splot_REGULAR2DXY);
	splot2d(double minx, double maxx, double miny, double maxy, bool logx=false, bool logy=false, sci::string title=sU(""), double titlesize=1.2, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, rgbcolour titlecolour=rgbcolour(0,0,0),void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&)=splot_REGULAR1DXY,void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&)=splot_REGULAR2DXY);
	splot2d(double minx, double maxx, double miny, double maxy, double xintersect, double yintersect, bool logx=false, bool logy=false, sci::string title=sU(""), double titlesize=1.2, double titledistance=2.0, sci::string titlefont=sU(""), int32_t titlestyle=0, rgbcolour titlecolour=rgbcolour(0,0,0),void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&)=splot_REGULAR1DXY,void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&)=splot_REGULAR2DXY);
	//private plot function so only friends can update a plot
	void plot(plstream *pl, wxDC *dc, int width, int height, bool antialiasing, double linewidthmultiplier=1.0);


	//axes
	splotaxis m_xaxis;
	splotaxis m_yaxis;
	//do we want to autoset limits and intersect point of axes
	bool m_xautointersect;
	bool m_yautointersect;

	//uncertainties
	std::vector< std::vector< double > > m_xpluserrs;
	std::vector< std::vector< double > > m_xminuserrs;
	std::vector< std::vector< double > > m_ypluserrs;
	std::vector< std::vector< double > > m_yminuserrs;
	std::vector< std::vector< double > > m_xpluserrsl;
	std::vector< std::vector< double > > m_xminuserrsl;
	std::vector< std::vector< double > > m_ypluserrsl;
	std::vector< std::vector< double > > m_yminuserrsl;
	std::vector<rgbcolour> m_xerrcolour;
	std::vector<rgbcolour> m_yerrcolour;
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
	void setallparams(sci::string title, double titlesize, double titledistance, sci::string titlefont, int32_t titlestyle, rgbcolour titlecolour,void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&),void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&));
};

class splotlegend : public splot
{
	friend class splotwindow;
public:
	void addentry(sci::string text, double textoffset = 0.2, double textsize = 9.6, const sci::string &textfont = sU(""), uint32_t textstyle = 0, double textspacing = 0.2, rgbcolour textcolour = rgbcolour(0, 0, 0), rgbcolour pointcolour = rgbcolour(0, 0, 0), double pointsize = 0.5, sci::string pointsymbol = sU("A"), rgbcolour linecolour = rgbcolour(0, 0, 0), int linewidth = 1, sci::string linestyle = sU(""));
	void addentry(sci::string text, double textoffset = 0.2, double textsize = 9.6, const sci::string &textfont = sU(""), uint32_t textstyle = 0, double textspacing = 0.2, hlscolour textcolour = hlscolour(0, 0, 0), hlscolour pointcolour = hlscolour(0, 0, 0), double pointsize = 0.5, sci::string pointsymbol = sU("A"), hlscolour linecolour = hlscolour(0, 0, 0), int linewidth = 1, sci::string linestyle = sU(""));
	void addentry(sci::string text, std::shared_ptr<splotcolourscale> colourscale, bool filloffscaletop, bool filloffscalebottom, double headingoffset=0.05, double textoffset=0.2, double textsize=9.6, const sci::string &textfont=sU(""), uint32_t textstyle=0, double textspacing=0.2, rgbcolour textcolour=rgbcolour(0,0,0), unsigned int ncolourlevels=256, bool contours=false, size_t height=5, bool horizontal=false);
	void addentry(sci::string text, std::shared_ptr<splotsizescale> sizescale, double headingoffset=0.05, double textoffset=0.2, double textsize=9.6, const sci::string &textfont=sU(""), uint32_t textstyle=0, double textspacing=0.2, rgbcolour textcolour=rgbcolour(0,0,0), rgbcolour pointcolour=rgbcolour(0,0,0), sci::string pointsymbol=sU("A"), size_t nlines=5);
	virtual ~splotlegend(){}
private:
	//private constructor so only friends can create a legend
	splotlegend(sci::string title=sU(""), double titlesize=12.0, double titledistance=2.0, double titlespacing=0.2, sci::string titlefont=sU(""), int32_t titlestyle=0, rgbcolour titlecolour=rgbcolour(0,0,0), rgbcolour outlinecolour=rgbcolour(0,0,0), int outlinewidth=1);
	rgbcolour m_outlinecolour;
	int m_outlinethickness;
	double m_titlespacing;
	std::vector<double> m_headingoffset;
	std::vector<double> m_textoffset;
	std::vector<sci::string> m_text;
	std::vector<sci::string> m_textfont;
	std::vector<PLUNICODE> m_textfci;
	std::vector<double> m_textsize;
	std::vector<double> m_textspacing;
	std::vector<rgbcolour> m_textcolour;
	std::vector<bool> m_contours;
	std::vector<bool> m_horizontal;
	std::vector<bool> m_filloffscaletop;
	std::vector<bool> m_filloffscalebottom;

	std::vector<size_t> m_nlines;
	void adddatasetproperties(sci::string text, double textoffset, double textsize, const sci::string &textfont, uint32_t textstyle, double textspacing, rgbcolour textcolour, rgbcolour pointcolour, double pointsize, sci::string pointsymbol, uint32_t pointstyle, sci::string pointfont, rgbcolour linecolour, int linewidth, sci::string linestyle, std::shared_ptr<splotcolourscale> colourscale, bool filloffscaletop, bool filloffscalebottom, std::shared_ptr<splotsizescale> sizescale, unsigned int ncolourlevels, bool contours, size_t nlines, double headingoffset, bool horizontal);
	void plot(plstream *pl, double linewidthmultiplier=1.0);
	void calculateautolimits(){}; //do nothing - function only created as it is pure virtual
};

class PlotCanvas
{
public:
	void addItem(std::shared_ptr<DrawableItem> item)
	{
		m_drawableItems.push_back(item);
	}
	void removeItem(std::shared_ptr<DrawableItem> item)
	{
		for (auto i = m_drawableItems.begin(); i != m_drawableItems.end(); ++i)
		{
			if (*i == item)
			{
				m_drawableItems.erase(i);
				--i;
			}
		}
	}
	void render(wxDC *dc, int width, int height, double linewidthmultiplier);
	void render(Renderer& renderer, grPerMillimetre scale);
	bool writetofile(sci::string filename, int width, int height, bool antialiasing, double linewidthmultiplier = 1.0, bool preferInkscape = false);
private:
	std::vector<std::shared_ptr<DrawableItem>> m_drawableItems;

};

class splotwindow : public wxScrolledWindow 
{
	friend class splotPrintout;
public:
	splotwindow(wxWindow *parent, bool antialiasing, bool fixedsize=false, wxWindowID winid=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxHSCROLL | wxVSCROLL, const wxString& name = wxT("sci++ plot"));
	virtual ~splotwindow();
	void setfixedsize(int width, int height);
	void setautofitplots();
	//plotlegend* addlegend(double xpos, double ypos, double width, double height, sci::string title=sU(""), double titlesize=12, double titledistance=2.0, double titlespacing=0.2, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0), wxColour outlinecolour=wxColour(0,0,0), int outlinewidth=1);
	bool writetofile(sci::string filename, double sizemultiplier, bool preferInkscape);
	bool writetofile(sci::string filename, int width, int height, double linewidthmultiplier, bool preferInkscape);
	bool print( bool showDialog );
	void forceRerender()
	{
		m_needRerender = true;
		Refresh();
	}
	std::shared_ptr<PlotCanvas> getCanvas()
	{
		return m_plotCanvas;
	}
private:
	std::shared_ptr<PlotCanvas> m_plotCanvas;
	bool print( bool showDialog, sci::string printerName );
	//vector of plots
	std::vector<splotlegend*> m_legends;
	static void collectErrors(void* log, char* message)
	{
		(*((wxString*)(log))) << message;
	}
	

	//bitmap we draw on
	wxBitmap* m_bitmap;
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

	wxString m_log;
	bool m_antialiasing;
	bool m_needRerender;

	void OnPaint(wxPaintEvent &event);
	
	
	//note the backends are 0 = DC, 1 = AGG, 2 = GC
	void OnEraseBackGround(wxEraseEvent& event) {};//this is overloaded to stop flicker when resizing

	DECLARE_EVENT_TABLE();
};


class splotframe : public wxFrame
{
public:
	splotframe(wxWindow* parent, bool antialiasing, wxWindowID id=wxID_ANY, const wxString& title=wxT("Apex++ plot"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("Sci++ Plot"));
	virtual ~splotframe(){};
	//inline splotlegend* addlegend(double xpos, double ypos, double width, double height, sci::string title, double titlesize=12, double titledistance=2.0, double titlespacing=0.2, sci::string titlefont=sU(""), int32_t titlestyle=0, wxColour titlecolour=wxColour(0,0,0), wxColour outlinecolour=wxColour(0,0,0), int outlinewidth=1){return m_plotwind->addlegend(xpos, ypos, width, height, title, titlesize, titledistance,titlespacing, titlefont, titlestyle, titlecolour, outlinecolour, outlinewidth);};
	//splotlegend* addlegend(double xpos, double ypos, double width, double height, wxColour outlinecolour=wxColour(0,0,0), int outlinewidth=1){return m_plotwind->addlegend(xpos, ypos, width, height, sU(""), 0.2, 2.0, 0.2, sU(""), 0, wxColour(0,0,0), outlinecolour, outlinewidth);};
	bool writetofile(sci::string filename, int sizemultiplier=1, bool preferInkscape=false){return m_plotwind->writetofile(filename,sizemultiplier, preferInkscape);};
	bool writetofile(sci::string filename, int width, int height, double linewidthmultiplier=1.0, bool preferInkscape=false){return m_plotwind->writetofile(filename,width,height,linewidthmultiplier, preferInkscape);};
	bool print(bool showDialog){return m_plotwind->print( showDialog );}
	std::shared_ptr<PlotCanvas> getCanvas()
	{
		return m_plotwind->getCanvas();
	}
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