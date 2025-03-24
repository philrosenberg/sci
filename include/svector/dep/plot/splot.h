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
	friend class PlotAxis;
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
	Direction getDirection() const
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
		m_log = log;
		setFixedScale(min, max);
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

namespace symText
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

class LineStyle
{
public:
	LineStyle(Length width = grMillimetre(0.5), const rgbcolour& colour = rgbcolour(0.0, 0.0, 0.0, 1.0), const std::vector<Length>& dashes = std::vector<Length>(0));
	LineStyle(Length width, const rgbcolour& colour, sci::string pattern);
	Length getWidth() const;
	void getPattern(std::vector<Length>& dashes) const;
	rgbcolour getColour() const;
	void setupLineStyle(plstream* pl, PLINT colourIndex, double scale) const;
	void resetLineStyle(plstream* pl, PLINT colourIndex) const;
	static void parseLineStyle(const sci::string& pattern, Length lineWidth, std::vector<Length>& dashes);
	void setPen(Renderer& renderer) const
	{
		renderer.setPen(m_colour, m_width, m_dashes);
	}
private:
	Length m_width;
	rgbcolour m_colour;
	std::vector<Length> m_dashes;
};

const LineStyle noLine(grMillimetre(0.0));

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

class PlotAxis: public PlotScale, public DrawableItem
{
	friend class splot;
	friend class splot2d;
public:
	class Options
	{
	public:
		Options(sci::string title = sU(""))
			:m_title(title), m_titleDistance(grTextPoint(42)), m_majorTickLength(grMillimetre(4.0)), m_minorTickLength(grMillimetre(2.0)),
			m_ticksLeftOrDown(true), m_ticksRightOrUp(false), m_labelsLeftOrDown(true), m_labelDirection(grMillimetre(1.0), grMillimetre(0.0)), m_majorInterval(0.0),
			m_nSubticks(0), m_autoMajorInterval(true), m_autoNSubticks(true), m_customlabelcreator(nullptr), m_maxDigits(4)
		{}
		static Options getDefaultAxis()
		{
			return Options();
		}
		static Options getBlankAxis()
		{
			Options result;
			result.m_lineStyle = noLine;
			result.m_labelFont.m_size = grMillimetre(0.0);
			return result;
		}
		Options& setTitle(const sci::string& title)
		{
			m_title = title;
			return *this;
		}
		//to do create functions like above for all properties
		//so we can chain setting parameters
		sci::string m_title;
		Renderer::Font m_titleFont;
		Length m_titleDistance;
		LineStyle m_lineStyle;
		Length m_majorTickLength;
		Length m_minorTickLength;
		bool m_ticksLeftOrDown;
		bool m_ticksRightOrUp;
		Renderer::Font m_labelFont;
		bool m_labelsLeftOrDown;
		Distance m_labelDirection;
		double m_majorInterval;
		size_t m_nSubticks;
		bool m_autoMajorInterval;
		bool m_autoNSubticks;
		sci::string m_timeformat;
		sci::string(*m_customlabelcreator)(double axisvalue);
		size_t m_maxDigits;
	};
	PlotAxis(double min, double max, bool log, Point start, Point end, Options options =  Options(), Direction direction = Direction::none)
		:PlotScale(min, max, log, direction), m_options(options), m_start(start), m_end(end), m_hasChanged(true)
	{}
	PlotAxis(bool log, Point start, Point end, Options options = Options(), Direction direction = Direction::none)
		:PlotScale(log, direction, 0.05), m_options(options), m_start(start), m_end(end), m_hasChanged(true)
	{

	}
	PlotAxis(Point start, Point end, Options options = Options(), Direction direction = Direction::none)
		:PlotScale(direction, 0.05), m_options(options), m_start(start), m_end(end), m_hasChanged(true)
	{

	}
	
	~PlotAxis(){};
	const Options& getOptions() const
	{
		return m_options;
	}
	void setOptions(const Options& options)
	{
		m_hasChanged = true;
		m_options = options;
	}
	Direction getBestDirection(Renderer &renderer) const
	{
		Direction direction = PlotScale::getDirection();
		if (Direction::none == direction)
		{
			grDegree angle = renderer.getAngle(m_end - m_start);
			if (angle < grDegree(0.0))
				angle += grDegree(360.0);
			if (angle < grDegree(45.0))
				direction = Direction::horizontal;
			else if (angle < grDegree(135.0))
				direction = Direction::vertical;
			else if (angle < grDegree(225.0))
				direction = Direction::horizontal;
			else if (angle < grDegree(315.0))
				direction = Direction::vertical;
			else
				direction = Direction::horizontal;
		}
		return direction;
	}
	
	void setPosition(Point start, Point end)
	{
		m_hasChanged = true;
		m_start = start;
		m_end = end;
	}
	void getPosition(Point& start, Point& end) const
	{
		start = m_start;
		end = m_end;
	}
	inline Point getStart() const
	{
		return m_start;
	}
	inline Point getEnd() const
	{
		return m_end;
	}

	void preDraw() override
	{
	}
	void draw(plstream* pl, double scale, double pageWidth, double pageHeight) override;
	void draw(Renderer& renderer, grPerMillimetre scale) override;
	bool readyToDraw() const override
	{
		return true;
	}
	Distance alongAxisDistance(double value) const
	{
		double fraction;
		if (isLog())
			fraction = (std::log10(value) - getLogMin()) / (getLogMax() - getLogMin());
		else
			fraction = (value - getMin()) / (getMax() - getMin());
		return (m_end - m_start) * grUnitless(fraction);
	}
	
private:
	void drawLinear(Renderer& renderer, grPerMillimetre scale);
	void drawLog(Renderer& renderer, grPerMillimetre scale);
	void drawTick(Renderer& renderer, grPerMillimetre scale, double plotPosition, bool minor);
	grMillimetre drawLabel(Renderer& renderer, grPerMillimetre scale, double plotPosition); //returns the sise of the label perpedicular to the axis
	void drawTitle(Renderer& renderer, grPerMillimetre scale, Length distanceFromAxis);

	std::string createploptstring() const;

	bool m_hasChanged;
	Options m_options;
	Point m_start;
	Point m_end;
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
	splothorizontalcolourbar(Point bottomLeft, Length height, Length width, std::shared_ptr<splotcolourscale> colourscale, PlotAxis::Options axisOptions = PlotAxis::Options())
		:m_colourscale(colourscale),
		m_xAxis(new PlotAxis(m_colourscale->getMin(), m_colourscale->getMax(), m_colourscale->isLog(), bottomLeft, Point(bottomLeft.getX(), bottomLeft.getY() + width), axisOptions, PlotScale::Direction::horizontal)),
		m_yAxis(new PlotAxis(m_colourscale->getMin(), m_colourscale->getMax(), false, bottomLeft, Point(bottomLeft.getX() + height, bottomLeft.getY()), PlotAxis::Options::getBlankAxis(), PlotScale::Direction::horizontal))
	{
	}
	void preDraw() override
	{
	}
	void draw(plstream* pl, double scale, double pageWidth, double pageHeight) override;
	bool readyToDraw() const override
	{
		return true;
	}
private:
	std::shared_ptr<splotcolourscale> m_colourscale;
	std::shared_ptr<PlotAxis> m_xAxis;
	std::shared_ptr<PlotAxis> m_yAxis;

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
	void render(Renderer& renderer, grPerMillimetre scale)
	{
		//cycle through all the Drawable Items ensuring they are ready to draw
		//note this may need calling multiple times are predraw on one item may
		//invalidate the predraw on another item
		bool ready = false;
		do
		{
			for (auto& d : m_drawableItems)
				if (!d->readyToDraw())
					d->preDraw();
			ready = true;
			for (auto& d : m_drawableItems)
				ready = ready && d->readyToDraw();
		} while (!ready);

		//draw the Drawable Items
		for (auto& d : m_drawableItems)
			d->draw(renderer, scale);
	}
	bool writetofile(sci::string filename, int width, int height, bool antialiasing, double linewidthmultiplier = 1.0, bool preferInkscape = false);
private:
	std::vector<std::shared_ptr<DrawableItem>> m_drawableItems;

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

#endif