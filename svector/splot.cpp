#include<wx/wx.h>
#include<wx/filename.h>
#include<wx/dcps.h>
#include <wx/dcsvg.h>
#include<wx/metafile.h>
#include<wx/gdicmn.h>
#include<wx/dcgraph.h> // there was some odd compile error when the wx headers were below the svector headers
                       // where wxVector<some class to do with wxGCDC>::push_back() wouldn't compile.
                       //not sure why.
#include"../include/svector/dep/plot/splot.h"
#include"../include/svector/dep/plot/transforms.h"
#include"../include/svector/serr.h"
//#include"../include/svector/svector.h"
#include"../include/svector/sstring.h"
#include"../include/svector/sreadwrite.h"
#include<cmath>
#include<algorithm>
#include<float.h>


class TempFile
{
public:
	TempFile(){m_filename=wxFileName::CreateTempFileName("");}
	~TempFile(){if(m_filename!="") wxRemoveFile(m_filename);}
	wxString getFilename(){return m_filename;}
private:
	wxString m_filename;

};

int splotexitcatcher(const char *message)
{
	wxString err(message, wxConvUTF8);
	wxMessageBox(err);
	return 0;
}

void splotabortcatcher(const char *message)
{
	wxString err(message, wxConvUTF8);
	wxMessageBox(err);
}
//transformations

void lineartransform(double xindex, double yindex, const std::vector<double> &x, const std::vector<double> &y, double &xout, double &yout)
{
	if(xindex!=xindex || yindex!=yindex)
	{
		xout=std::numeric_limits<double>::quiet_NaN();
		yout=std::numeric_limits<double>::quiet_NaN();
		return;
	}
	size_t ixindex=(int)xindex;
	size_t iyindex=(int)yindex;
	//if we have hit an index exactly then just read from the vector
	//otherwise linearly interpolate between the points
	//note that we may get xindex=x.size()-1, so checking for hitting an index
	//exactly also ensures we don't go out of bounds
	if((double)ixindex==xindex) xout=x[ixindex];
	else xout=x[ixindex]+(x[ixindex+1]-x[ixindex])*(xindex-(double)ixindex);
	if((double)iyindex==yindex) yout=y[iyindex];
	else yout=y[iyindex]+(y[iyindex+1]-y[iyindex])*(yindex-(double)iyindex);
}
void lineartransform2d(double xindex, double yindex, const std::vector< std::vector< double > > &x, const std::vector< std::vector< double > > &y, double &xout, double &yout)
{
	if(xindex!=xindex || yindex!=yindex)
	{
		xout=std::numeric_limits<double>::quiet_NaN();
		yout=std::numeric_limits<double>::quiet_NaN();
		return;
	}
	size_t ixindex=(int)xindex;
	size_t iyindex=(int)yindex;
	double fracx=xindex-double(ixindex);
	double fracy=yindex-double(iyindex);
	if(fracy==0.0 && fracx==0.0)
	{
		xout=x[ixindex][iyindex];
		yout=y[ixindex][iyindex];
	}
	else if(fracx==0.0)
	{
		xout=x[ixindex][iyindex]*(1.0-fracy)+x[ixindex][iyindex+1]*fracy;
		yout=y[ixindex][iyindex]*(1.0-fracy)+y[ixindex][iyindex+1]*fracy;
	}
	else if(fracy==0.0)
	{
		xout=x[ixindex][iyindex]*(1.0-fracx)+x[ixindex+1][iyindex]*fracx;
		yout=y[ixindex][iyindex]*(1.0-fracx)+y[ixindex+1][iyindex]*fracx;
	}
	else
	{
		double xbottom=x[ixindex][iyindex]*(1.0-fracx)+x[ixindex+1][iyindex]*fracx;
		double ybottom=y[ixindex][iyindex]*(1.0-fracx)+y[ixindex+1][iyindex]*fracx;
		double xtop=x[ixindex][iyindex+1]*(1.0-fracx)+x[ixindex+1][iyindex+1]*fracx;
		double ytop=y[ixindex][iyindex+1]*(1.0-fracx)+y[ixindex+1][iyindex+1]*fracx;
		xout=xbottom*(1.0-fracy)+xtop*fracy;
		yout=ybottom*(1.0-fracy)+ytop*fracy;
	}
}
void loglineartransform(const std::vector<double> &inputparams,std::vector<double> &outputparams)
{
	if(inputparams.size()<2) 
	{
		outputparams.resize(0);
		return;
	}
	outputparams=inputparams;
	outputparams[0]=std::log10(inputparams[0]);
}
void linearlogtransform(const std::vector<double> &inputparams,std::vector<double> &outputparams)
{
	if(inputparams.size()<2) 
	{
		outputparams.resize(0);
		return;
	}
	outputparams=inputparams;
	outputparams[0]=std::log10(inputparams[0]);
}
void loglogtransform(const std::vector<double> &inputparams,std::vector<double> &outputparams)
{
	if(inputparams.size()<2) 
	{
		outputparams.resize(0);
		return;
	}
	outputparams=inputparams;
	outputparams[1]=std::log10(inputparams[1]);
}
void polardegreestransform(const std::vector<double> &inputparams,std::vector<double> &outputparams)
{
	if(inputparams.size()<2) 
	{
		outputparams.resize(0);
		return;
	}
	outputparams=inputparams;
	outputparams[0]=std::sin(M_PI/180.0*inputparams[1])*inputparams[0];
	outputparams[1]=std::cos(M_PI/180.0*inputparams[1])*inputparams[0];
}
void polarradianstransform(const std::vector<double> &inputparams,std::vector<double> &outputparams)
{
	if(inputparams.size()<2) 
	{
		outputparams.resize(0);
		return;
	}
	outputparams=inputparams;
	outputparams[0]=std::sin(inputparams[1])*inputparams[0];
	outputparams[1]=std::cos(inputparams[1])*inputparams[0];
}


sci::string loglonghand(double axisvalue)
{
	sci::ostringstream result;
	result<<std::pow(10.0,axisvalue);
	return result.str();
}

void sci::plot::customlabelinterpreter(PLINT axis, PLFLT value, char* label, PLINT length,PLPointer function)
{
	if( length == 0 )
		return;
	sci::plot::customlabeler userfunction=( sci::plot::customlabeler )function;
	sci::string stdlabel=userfunction(value);
	if(stdlabel.length()>(unsigned int)length) stdlabel.erase(length-1);
	strcpy(label,sci::toUtf8(stdlabel).c_str());
}

void sci::plot::ColourScale::setupForImage(plstream *pl) const
{
	const double* intensity = isLog() ? &m_logValue[0] : &m_value[0];
	pl->scmap1n(256);
	pl->scmap1la(!m_hls, m_colour1.size(), intensity, &m_colour1[0], &m_colour2[0], &m_colour3[0], &m_alpha[0], NULL);
}

void sci::plot::ColourScale::setupForShade(plstream* pl) const
{
	//when doing filled contouring using plshade, plPlot takes the colourscale
	//and uses the lowest value for the first interval, the
	//highest value for the last interval and equally spaced values
	//on the colourscale for each intermediate level. This is irrespective
	//of the actual interval of the contours, which might be irregular.
	//We need to correct for this

	//Note to self - this still breaks if autoscale is off of fill off scale top/bottom are on

	sci::assertThrow(m_discrete, sci::err(sci::SERR_PLOT, colourscaleErrorCode, "sci::plot::ColourScale::setupForShade called with a not discrete colour scale."));

	size_t nBoundaries = m_value.size() / 2 + 1;
	double spacing = 1.0 / (nBoundaries - 2.0);
	std::vector<double> newValues(m_value.size());
	newValues.front() = 0.0;
	newValues.back() = 1.0;
	for (size_t i = 0; i < (newValues.size())/2-1; ++i)
	{
		newValues[i * 2 + 1] = spacing * (i + 0.5);
		newValues[i * 2 + 2] = spacing * (i + 0.5);
	}
	pl->scmap1n((newValues.size()-2) * 2);
	pl->scmap1la(!m_hls, m_colour1.size(), &newValues[0], &m_colour1[0], &m_colour2[0], &m_colour3[0], &m_alpha[0], NULL);
}

sci::GridData<double, 1> sci::plot::LevelScale::getLevels() const
{
	sci::GridData<double, 1> result;
	if (isLog())
	{
		//result = sci::pow(10.0, getLogMin() + m_logValue * (getLogMax() - getLogMin())); //this should work but doesn't so do it manually
		result.reshape(m_logValue.shape());
		auto logValueIter = m_logValue.begin();
		for (auto resultIter = result.begin(); resultIter != result.end(); ++resultIter, ++logValueIter)
			*resultIter = sci::pow(10.0, getLogMin() + *logValueIter * (getLogMax() - getLogMin()));
	}
	else
		result = getLinearMin() + m_value * (getLinearMax() - getLinearMin());
	return result;
}

std::string sci::plot::PlotAxis::createploptstring() const
{
	std::string opt = "bci";
	if (m_options.m_timeformat != sU(""))opt = opt + "d";
	if (m_options.m_customlabelcreator != NULL)opt = opt + "o";
	if (!m_options.m_labelFont.m_size.isAlwaysZero())
	{
		if (!m_options.m_labelsLeftOrDown)
			opt = opt + "m";
		else
			opt = opt + "n";
	}
	if (isLog())
		opt = opt + "l";
	if (m_options.m_majorTickLength.getLength(grMillimetre(0.0), grMillimetre(0.0)) > grMillimetre(0.0))
		opt = opt + "t";
	else
		opt = opt + "x";
	if (m_options.m_minorTickLength.getLength(grMillimetre(0.0), grMillimetre(0.0)) > grMillimetre(0.0))
		opt = opt + "s";

	return opt;
}

void sci::plot::PlotAxis::draw(plstream* pl, double scale, double pageWidth, double pageHeight)
{
	//get the tick interval and subintervals
	//double ymajint = m_yaxis.m_automajorinterval ? 0.0 : m_yaxis.m_majorinterval;
	//unsigned int ynsub = m_yaxis.m_autonsubticks ? 0.0 : m_yaxis.m_nsubticks + 1;
	double min = getLinearOrLogMin();
	double max = getLinearOrLogMax();

	if (m_direction == Direction::horizontal)
	{
		pl->vpor(m_start.getX(1.0, 1.0, grPerMillimetre(0)), m_end.getX(1.0, 1.0, grPerMillimetre(0)),
			1.0 - m_start.getY(1.0, 1.0, grPerMillimetre(0)), 1.0 - m_start.getY(1.0, 1.0, grPerMillimetre(0)) + 1.0); //reversed y direction in plplot vs Renderer
		if(isLog())
			pl->wind(std::log10(min), std::log10(max), 0.0, 1.0);
		else
			pl->wind(min, max, 0.0, 1.0);
	}
	else if (m_direction == Direction::vertical)
	{
		pl->vpor(m_start.getX(1.0, 1.0, grPerMillimetre(0)), m_start.getX(1.0, 1.0, grPerMillimetre(0)) + 1.0,
			1.0 - m_start.getY(1.0, 1.0, grPerMillimetre(0)), 1.0 - m_end.getY(1.0, 1.0, grPerMillimetre(0))); //reversed y direction in plplot vs Renderer
		if (isLog())
			pl->wind(0.0, 1.0, std::log10(min), std::log10(max));
		else
			pl->wind(0.0, 1.0, min, max);
	}


	//first the x axis
	//set the axis colour
	m_options.m_lineStyle.setupLineStyle(pl, 1, scale);
	pl->col0(1);
	//set the font for the labels
	pl->sfci(0);
	//pl->sfontf(m_xaxis.m_labelfont.mb_str());
	pl->schr(1.0, m_options.m_labelFont.m_size.getLength(grMillimetre(0.0), grMillimetre(0.0)).value<grMillimetre>());
	//set the tick lengths
	pl->smin(1.0, m_options.m_minorTickLength.getLength(grMillimetre(0.0), grMillimetre(0.0)).value<grMillimetre>() * scale);
	pl->smaj(1.0, m_options.m_majorTickLength.getLength(grMillimetre(0.0), grMillimetre(0.0)).value<grMillimetre>() * scale);
	//set the label dp and length
	pl->prec(0, 0);
	//create a plplot option string for the axis
	std::string opt = createploptstring();
	//set up axes time formats - has no impact if not using time format
	pl->timefmt(sci::toUtf8(m_options.m_timeformat).c_str());
	pl->slabelfunc(&customlabelinterpreter, (PLPointer)m_options.m_customlabelcreator);

	double majint = m_options.m_autoMajorInterval ? 0.0 : m_options.m_majorInterval;
	unsigned int nsub = m_options.m_autoNSubticks ? 0.0 : m_options.m_nSubticks + 1;
	//draw the x axis
	if (getDirection() == Direction::horizontal)
		pl->box(opt.c_str(), majint, nsub, "", 0.0, 0.0);
	else if (getDirection() == Direction::vertical)
		pl->box("", 0.0, 0, opt.c_str(), majint, nsub);



	//add axis lables and a title, to give each axis label and the title independant control over font
	//we draw them separately
	//first the x axis
	pl->sfci(0);
	//pl->sfontf(m_xaxis.m_titlefont.mb_str());
	pl->schr(1.0, m_options.m_titleFont.m_size.getLength(grMillimetre(0.0), grMillimetre(0.0)).value<grMillimetre>());
	pl->scol0a(1, m_options.m_titleFont.m_colour.r() * 255.99999, m_options.m_titleFont.m_colour.g() * 255.99999, m_options.m_titleFont.m_colour.b() * 255.99999, m_options.m_titleFont.m_colour.a());
	pl->col0(1);
	double distance = (m_options.m_titleDistance.getLength(grMillimetre(0.0), grMillimetre(0.0)) / m_options.m_titleFont.m_size.getLength(grMillimetre(0.0), grMillimetre(0.0))).value<grUnitless>();
	if (getDirection() == Direction::horizontal)
		pl->mtex("b", distance, 0.5, 0.5, sci::toUtf8(m_options.m_title).c_str());
	else if (getDirection() == Direction::vertical)
		pl->mtex("l", distance, 0.5, 0.5, sci::toUtf8(m_options.m_title).c_str());

}

void sci::plot::PlotAxis::draw(Renderer& renderer, grPerMillimetre scale)
{
	sci::graphics::StatePusher statePusher(&renderer);
	m_options.m_lineStyle.setPen(renderer);
	if (isLog())
		drawLog(renderer, scale);
	else
		drawLinear(renderer, scale);
}

void sci::plot::PlotAxis::drawLinear(Renderer & renderer, grPerMillimetre scale)
{
	//calculate the min, max and span
	double min = getLinearMin();
	double max = getLinearMax();
	if (min > max)
		std::swap(min, max);
	double span = max - min;

	//calculate major interval if needed
	double majorInterval;
	size_t nAutoSubticks = 0;
	if (m_options.m_autoMajorInterval)
	{
		double spanMagnitude = std::floor(std::log10(span));
		double spanNormalised = span / std::pow(10.0, spanMagnitude); //this will be in the range 1-9.99999;
		if (spanNormalised <= 2.0)
		{
			majorInterval = 0.2 * std::pow(10.0, spanMagnitude);
			nAutoSubticks = 3;
		}
		else if (spanNormalised <= 5.0)
		{
			majorInterval = 0.5 * std::pow(10.0, spanMagnitude);
			nAutoSubticks = 4;
		}
		else
		{
			majorInterval = 1.0 * std::pow(10.0, spanMagnitude);
			nAutoSubticks = 4;
		}
	}
	else
		majorInterval = m_options.m_majorInterval;

	//calculate the number of subticks
	size_t nSubticks;
	if (m_options.m_autoNSubticks)
	{
		if (m_options.m_autoMajorInterval)
			nSubticks = nAutoSubticks;
		else
			nSubticks = 4;
	}
	else
		nSubticks = m_options.m_nSubticks;

	//draw the ticks and labels
	renderer.setFont(m_options.m_labelFont);
	double currentMajorPosition = std::ceil(min / majorInterval) * majorInterval;
	double minorInterval = majorInterval / double(nSubticks + 1);
	grMillimetre maxLabelSize(0);
	while (currentMajorPosition <= max) //note this will be false if currentMajorPosition is a nan, so we don't need to worry about infite loops
	{
		drawTick(renderer, scale, currentMajorPosition, false);
		grMillimetre labelSize = drawLabel(renderer, scale, currentMajorPosition);
		maxLabelSize = std::max(labelSize, maxLabelSize);
		for (size_t i = 0; i < nSubticks; ++i)
		{
			double minorPosition = currentMajorPosition + double(i + 1) * minorInterval;
			if (minorPosition <= max)
				drawTick(renderer, scale, minorPosition, true);
		}
		currentMajorPosition += majorInterval;
	}

	//draw the axis
	renderer.line(m_start, m_end);

	//draw the title
	renderer.setFont(m_options.m_titleFont);
	drawTitle(renderer, scale, m_options.m_majorTickLength * grUnitless(1.4) + maxLabelSize);
}

void sci::plot::PlotAxis::drawLog(Renderer& renderer, grPerMillimetre scale)
{
	//calculate the min, max and span
	double logMin = getLogMin();
	double logMax = getLogMax();
	if (logMin > logMax)
		std::swap(logMin, logMax);
	double logSpan = logMax - logMin;

	//calculate major interval if needed
	double majorLogInterval;
	if (m_options.m_autoMajorInterval)
		majorLogInterval = std::ceil(logSpan / 10.0);
	else
		majorLogInterval = m_options.m_majorInterval;

	//calculate the number of subticks and the interval
	size_t nSubticks;
	if (m_options.m_autoNSubticks)
		nSubticks = 8;
	else
		nSubticks = m_options.m_nSubticks;
	//If major interval divided by the sub intervals is an integer, then multiply subticks by this number
	double minorLogInterval = 0;;
	double testInterval = majorLogInterval / double(nSubticks + 1);
	if (nSubticks > 0 && testInterval == ceil(testInterval))
		minorLogInterval = testInterval;
	double minorInterval = (std::pow(10.0, majorLogInterval) - 1.0) / double(nSubticks + 1);

	//draw the ticks and labels
	renderer.setFont(m_options.m_labelFont);
	double currentMajorLogPosition = std::ceil(logMin / majorLogInterval) * majorLogInterval;
	grMillimetre maxLabelSize(0);

	while (currentMajorLogPosition <= logMax) //note this will be false if currentMajorPosition is a nan, so we don't need to worry about infite loops
	{
		double currentMajorPosition = std::pow(10.0, currentMajorLogPosition);
		drawTick(renderer, scale, currentMajorPosition, false);
		grMillimetre labelSize = drawLabel(renderer, scale, currentMajorPosition);
		maxLabelSize = std::max(labelSize, maxLabelSize);
		if (minorLogInterval != 0.0)
		{
			for (size_t i = 0; i < nSubticks; ++i)
			{
				double currentMinorPosition = std::pow(10.0, currentMajorLogPosition + double(i+1) * minorLogInterval);
				if(currentMinorPosition<=getLinearMax())
					drawTick(renderer, scale, currentMinorPosition, true);
			}

		}
		else
		{
			for (size_t i = 0; i < nSubticks; ++i)
			{
				double currentMinorPosition = currentMajorPosition * double((i+1)*minorInterval  + 1.0);
				if(currentMinorPosition <=getLinearMax())
					drawTick(renderer, scale, currentMinorPosition, true);
			}
		}
		currentMajorLogPosition += majorLogInterval;
	}

	//draw the axis
	renderer.line(m_start, m_end);

	//draw the title
	renderer.setFont(m_options.m_titleFont);
	drawTitle(renderer, scale, m_options.m_majorTickLength * grUnitless(1.4) + maxLabelSize);
}

void sci::plot::PlotAxis::drawTick(Renderer& renderer, grPerMillimetre scale, double plotPosition, bool minor)
{
	Point pagePosition = m_start + alongAxisDistanceFromLinearData(plotPosition);
	Length length = minor ? m_options.m_minorTickLength : m_options.m_majorTickLength;
	Point p1 = Point(grMillimetre(0.0), grMillimetre(0.0));
	Point p2 = p1;
	Direction direction = getBestDirection(renderer);
	if (direction == Direction::horizontal)
	{
		//set the position of both ends of the tick on the axis initially
		p1 = pagePosition;
		p2 = pagePosition;
		//extend the tick out in either direction as needed
		if (m_options.m_ticksRightOrUp)
			p1+=Distance(grMillimetre(0.0), -length);
		if (m_options.m_ticksLeftOrDown)
			p2+=Distance(grMillimetre(0.0), length);
	}
	else if (direction == Direction::vertical)
	{
		//set the position of both ends of the tick on the axis initially
		p1 = pagePosition;
		p2 = pagePosition;
		//extend the tick out in either direction as needed
		if (m_options.m_ticksLeftOrDown)
			p1+=Distance(-length, grMillimetre(0.0));
		if (m_options.m_ticksRightOrUp)
			p2+=Distance(length, grMillimetre(0.0));
	}
	if(p1 != p2)
		renderer.line(p1, p2);
}

sci::graphics::millimetre sci::plot::PlotAxis::drawLabel(Renderer& renderer, grPerMillimetre scale, double plotPosition)
{
	sci::graphics::StatePusher statePusher(&renderer);
	renderer.setBrush(m_options.m_labelFont.m_colour);
	Point pagePosition = m_start + alongAxisDistanceFromLinearData(plotPosition);
	Length distanceFromAxis = m_options.m_majorTickLength * grUnitless(1.2);

	sci::string label;
	{
		//make a scope so we destroy the stream and don't access it later
		sci::stringstream strm;
		strm.precision(m_options.m_maxDigits);
		strm << plotPosition;

		//make scientific notation nicer
		size_t ePosition = strm.str().find(sU("e"));
		if (ePosition != sci::string::npos && strm.str().length() > ePosition + 2)
		{
			label = strm.str().substr(0, ePosition);
			label = label + sU("\u00D710^{");
			size_t exponentPosition = ePosition + 1;
			if (strm.str()[exponentPosition] == sU('-'))
				label = label + sU('-');
			++exponentPosition; //skip the + or -
			while (strm.str()[exponentPosition] == sU('0') && exponentPosition < strm.str().length())
				++exponentPosition;
			label = label + strm.str().substr(exponentPosition) + sU("}");
		}
		else
			label = strm.str();
	}

	grUnitless alignment(0.5);
	grMillimetre labelSize(0);
	Direction direction = getBestDirection(renderer);
	if (direction == Direction::horizontal)
	{
		//set the horizontal position and the alignment
		if (!m_options.m_labelsLeftOrDown)
		{
			pagePosition += Distance(grMillimetre(0.0), -distanceFromAxis);
			alignment = grUnitless(1.0);
		}
		else
		{
			pagePosition += Distance(grMillimetre(0.0), distanceFromAxis);
			alignment = grUnitless(0.0);
		}

		labelSize = (renderer.formattedText(label, pagePosition, grUnitless(0.5), grUnitless(alignment))).ascent;
	}
	else if (direction == Direction::vertical)
	{
		//set the horizontal position and the alignment
		if (m_options.m_labelsLeftOrDown)
		{
			pagePosition += Distance(-distanceFromAxis, grMillimetre(0.0));
			alignment = grUnitless(1.0);
		}
		else
		{
			pagePosition += Distance(distanceFromAxis, grMillimetre(0.0));
			alignment = grUnitless(0.0);
		}

		labelSize= (renderer.formattedText(label, pagePosition, grUnitless(alignment), grUnitless(0.5))).width;
	}

	return labelSize;
}

void sci::plot::PlotAxis::drawTitle(Renderer& renderer, grPerMillimetre scale, Length distanceFromAxis)
{
	sci::graphics::StatePusher statePusher(&renderer);
	renderer.setBrush(m_options.m_titleFont.m_colour);
	Point pagePosition = m_start + (m_end-m_start) *grUnitless(0.5);

	grUnitless alignment(0.5);
	Direction direction = getBestDirection(renderer);
	if (direction == Direction::horizontal)
	{
		//set the horizontal position and the alignment
		if (!m_options.m_labelsLeftOrDown)
		{
			pagePosition += Distance(grMillimetre(0.0), -distanceFromAxis);
			alignment = grUnitless(1.0);
		}
		else
		{
			pagePosition += Distance(grMillimetre(0.0), distanceFromAxis);
			alignment = grUnitless(0.0);
		}

		renderer.formattedText(m_options.m_title, pagePosition, grUnitless(0.5), grUnitless(alignment));
	}
	else if (direction == Direction::vertical)
	{
		//set the horizontal position and the alignment
		if (m_options.m_labelsLeftOrDown)
		{
			pagePosition += Distance(-distanceFromAxis, grMillimetre(0.0));
			alignment = grUnitless(1.0);
		}
		else
		{
			pagePosition += Distance(distanceFromAxis, grMillimetre(0.0));
			alignment = grUnitless(0.0);
		}

		renderer.formattedText(m_options.m_title, pagePosition, grUnitless(0.5), grUnitless(alignment), grDegree(90));
	}
}

void sci::plot::splotLabel::draw(plstream* pl, double scale, double pageWidth, double pageHeight)
{
	//pl->vpor(0.0, 1.0, 0.0, 1.0);
	//pl->wind(0.0, 1.0, 0.0, 1.0);
	//pl->sfci(m_style);
	//pl->schr(1.0, m_size * scale / 72.0 * 25.4);
	//pl->scol0a(1, m_colour.r() * 255.99999, m_colour.g() * 255.99999, m_colour.b() * 255.99999, m_colour.a());
	//pl->col0(1);
	//pl->ptex(m_xPosition, m_yPosition, 1, 0, m_alignment, sci::toUtf8(m_text).c_str());
}

void sci::plot::splotLabel::draw(Renderer& renderer, grPerMillimetre scale)
{
	sci::graphics::StatePusher statePusher(&renderer);
	renderer.setFont(m_font);
	renderer.formattedText(m_text, m_position, m_horizontalAlignment, m_verticalAlignment, m_rotation);
}

void sci::plot::HorizontalColourBar::draw(plstream* pl, double scale, double pageWidth, double pageHeight)
{
	if (m_colourscale->isDiscrete())
	{
		sci::GridData<double, 2> cb({ 2, 2 });
		cb[0][0] = m_colourscale->getLinearOrLogMin();
		cb[0][1] = cb[0][0];
		cb[1][0] = m_colourscale->getLinearOrLogMax();
		cb[1][1] = cb[1][0];
		std::vector<double> cbX{ cb[0][0], cb[1][0] };
		std::vector<double> cbY{ 0.0, 1.0 };

		ContourData data(cbX, cbY, cb, m_xAxis, m_yAxis, m_colourscale, noLine);

		data.draw(pl, scale, pageWidth, pageHeight);
		m_xAxis->draw(pl, scale, pageWidth, pageHeight);
	}
	else
	{
		sci::GridData<double, 2> cb({ 256, 1 });
		std::vector<double> cbX(cb.size()+1);


		double min = m_colourscale->getLinearOrLogMin();
		double max = m_colourscale->getLinearOrLogMax();

		if (m_colourscale->isLog())
		{
			min = std::log10(min);
			max = std::log10(max);

			double range = max-min;
			double step = range / (cb.size());

			for (size_t i = 0; i < cbX.size(); ++i)
				cbX[i] = std::pow(10, min + i * step);

			for (size_t i = 0; i < cb.size(); ++i)
				cb[i][0] = std::pow(10.0, (min + (i + 0.5) * step));
		}
		else
		{
			double range = max - min;
			double step = range / (cb.size());

			for (size_t i = 0; i < cbX.size(); ++i)
				cbX[i] = min + i * step;

			for (size_t i = 0; i < cb.size(); ++i)
				cb[i][0] = (cbX[i] + cbX[i + 1]) / 2.0;
		}
		
		std::vector<double> cbY{ 0.0, 1.0 };

		GridData data(cbX, cbY, cb, m_xAxis, m_yAxis, m_colourscale);

		data.draw(pl, scale, pageWidth, pageHeight);
		m_xAxis->draw(pl, scale, pageWidth, pageHeight);
	}
}

void sci::plot::HorizontalColourBar::draw(Renderer& renderer, grPerMillimetre scale)
{
	if (m_colourscale->isDiscrete())
	{
		sci::GridData<double, 2> cb({ 2, 2 });
		cb[0][0] = m_colourscale->getLinearMin();
		cb[0][1] = cb[0][0];
		cb[1][0] = m_colourscale->getLinearMax();
		cb[1][1] = cb[1][0];
		std::vector<double> cbX{ cb[0][0], cb[1][0] };
		std::vector<double> cbY{ 0.0, 1.0 };

		ContourData data(cbX, cbY, cb, m_xAxis, m_yAxis, m_colourscale, noLine);

		data.draw(renderer, scale);
		m_xAxis->draw(renderer, scale);
	}
	else
	{
		sci::GridData<double, 2> z({ 256, 1 });
		std::vector<double> cbX(z.shape()[0] + 1);


		double min = m_colourscale->getLinearOrLogMin();
		double max = m_colourscale->getLinearOrLogMax();

		if (m_colourscale->isLog())
		{
			double range = max - min;
			double step = range / (z.shape()[0]);

			for (size_t i = 0; i < cbX.size(); ++i)
				cbX[i] = std::pow(10, min + i * step);

			for (size_t i = 0; i < z.size(); ++i)
				z[i][0] = std::pow(10.0, (min + (i + 0.5) * step));
		}
		else
		{
			double range = max - min;
			double step = range / (z.size());

			for (size_t i = 0; i < cbX.size(); ++i)
				cbX[i] = min + i * step;

			for (size_t i = 0; i < z.size(); ++i)
				z[i][0] = (cbX[i] + cbX[i + 1]) / 2.0;
		}

		std::vector<double> cbY{ 0.0, 1.0 };

		GridData data(cbX, cbY, z, m_xAxis, m_yAxis, m_colourscale);

		data.draw(renderer, scale);
		m_xAxis->draw(renderer, scale);
	}
}

void sci::plot::PlotCanvas::render(wxDC* dc, int width, int height, double linewidthmultiplier)
{

	//no point in messing around if the size is zero
	if (height == 0 || width == 0) return;

	//create the stream
	plstream* pl = new plstream();
	pl->scol0(0, 255, 255, 255);
	pl->sexit(&splotexitcatcher);
	plsabort(&splotabortcatcher);
	//pl->sabort(&splotabortcatcher);
	//set the device to wxwidgets
	pl->sdev("wxwidgets");
	//pl->sdev("psc");
	//pl->sfnam("D:\\contourtest.ps");
	//pl->setopt("name", "d:\\contourtest.ps");

	//set up the page to the size of the bitmap
	//with 90 dpi resolution (for text sizing)
	pl->spage(90.0, 90.0, width, height, 0, 0);
	//pass in the dc
	pl->sdevdata(dc);
	//initialize the stream
	pl->init();
	//pass the dc to the stream
	pl->cmd(PLESC_DEVINIT, dc);
	//Advance to the next ( i.e. first ) page
	pl->adv(0);


	//cycle through all the Drawable Items ensuring they are ready to draw
	//not this may need calling multiple times are predraw on one item may
	//invalidate the predraw on another item
	bool ready;
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
		d->draw(pl, linewidthmultiplier, width, height);


	//draw each legend in turn
	//for (size_t i = 0; i < m_legends.size(); ++i)
	//{
	//	//set the area covered by this legend
	//	pl->vpor(m_legendxloc[i], m_legendxloc[i] + m_legendwidth[i], m_legendyloc[i], m_legendyloc[i] + m_legendheight[i]);
	//
	//	//draw the legend
	//	m_legends[i]->plot(pl, linewidthmultiplier);
	//}

	//we're done drawing so delete pl
	delete pl;
}

//Write the plot window to an image file. This can be either a raster
//or vector graphic image. The file extension defines the type of image saved. 
//Valid extensions are png, jpg, bmp, tif, pcx, xpm, xbm, pnm for bitmaps and
//ps, svg, emf for vector. If the extension does not correspond to any of these 
//strings then png is used, although the given extension will still be used 
//in the filename. the antialiasing parameter is ignored for vector graphics.

bool sci::plot::PlotCanvas::writetofile(sci::string filename, int width, int height, grPerMillimetre scale)
{
	//get the extension
	wxFileName fullfile = wxString(sci::nativeUnicode(filename));
	wxString extension = fullfile.GetExt().Lower();

	bool result = true;


	if (extension == "ps")
	{
		//here we redraw the plot like OnPaint but using a postscript DC.
		wxPrintData setupdata;
		setupdata.SetColour(true);
		setupdata.SetFilename(sci::nativeUnicode(filename));
		//setupdata.SetPaperId(wxPAPER_A4);
		setupdata.SetPaperId(wxPAPER_NONE);
		//note we set the image size in mm, but ps uses pts(1/72 inch, ~0.35 mm) and uses integer coordinates. 
		//So setting the image size to the screen resolution gives us a ps resolution of about 3 times bettr 
		//than the screen. Here we've multiplied by 10 to get 30 times better.
		int sizemultiplier = 10;
		setupdata.SetPaperSize(wxSize(width * sizemultiplier, height * sizemultiplier));
		setupdata.SetPrintMode(wxPRINT_MODE_FILE);
		//setupdata.SetQuality(wxPRINT_QUALITY_HIGH); //doesn't seem to do anything
		wxPostScriptDC psdc(setupdata);
		result = psdc.StartDoc(sci::nativeUnicode(sU("Writing ") + filename));
		if (result == false)
			return result;
		sci::graphics::wxRenderer renderer(&psdc, wxSize(width, height), scale);
		render(renderer, scale);
		psdc.EndDoc();
	}
#ifdef _WIN32
	else if (extension == "emf")
	{
		//here we redraw the plot like OnPaint but using a wxMetafile DC.
		wxMetafileDC metadc(sci::nativeUnicode(filename), width, height);
		sci::graphics::wxRenderer renderer(&metadc, wxSize(width, height), scale);
		render(renderer, scale);;//0 gives vector output
		//close the file - note this gives us a copy of the file in memory which we must delete
		wxMetafile* metafile = metadc.Close();
		result = metafile != NULL;
		delete metafile;
		//we must call this function to make the file importable into other software
		int minx = metadc.MinX();
		int maxx = metadc.MaxX();
		int miny = metadc.MinY();
		int maxy = metadc.MaxY();
		//wxMakeMetaFilePlaceable(minx,miny,maxx,maxy);
	}
#endif
	else if (extension == "svg")
	{
		wxSVGFileDC dc(sci::nativeUnicode(filename), width, height, scale.value<grPerInch>());
		sci::graphics::wxRenderer renderer(&dc, wxSize(width, height), scale);
		render(renderer, scale);
		result = true;
	}
	else
	{
		//load the image handlers
		wxInitAllImageHandlers();
		//create a wxBitmapType to define the type saved as
		//use PNG as default
		wxBitmapType type = wxBITMAP_TYPE_PNG;
		if (extension == wxT("jpg")) type = wxBITMAP_TYPE_JPEG;
		else if (extension == wxT("bmp")) type = wxBITMAP_TYPE_BMP;
		else if (extension == wxT("tif")) type = wxBITMAP_TYPE_TIF;
		else if (extension == wxT("pcx")) type = wxBITMAP_TYPE_PCX;
		else if (extension == wxT("xpm")) type = wxBITMAP_TYPE_XPM;
		else if (extension == wxT("xbm")) type = wxBITMAP_TYPE_XBM;
		else if (extension == wxT("pnm")) type = wxBITMAP_TYPE_PNM;

			
		//create a wxMemoryDC which will be linked to the bitmap. We'll use this to draw to the bitmap
		//except if using AGG then we create an image instead which we'll need to copy to the bitmap
		wxMemoryDC memdc;
		wxBitmap bitmap(width, height, 32);
		if (!bitmap.IsOk())
			return false;
		memdc.SelectObject(bitmap);
		//fill the bitmap with white giving a white background for plplot
		//or to show blank if there are no plots
		memdc.FloodFill(0, 0, *wxWHITE, wxFLOOD_BORDER);

		sci::graphics::wxRenderer renderer(&memdc, wxSize(width, height), scale);
		render(renderer, scale);


		//reselect null bitmap for the memdc
		memdc.SelectObject(wxNullBitmap);

		//write the bitmap to file
		result = bitmap.SaveFile(sci::nativeUnicode(filename), type);
			
	}

	return result;
}
