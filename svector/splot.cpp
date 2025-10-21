#include<wx/wx.h>
#include<wx/filename.h>
#include<wx/dcps.h>
#include <wx/dcsvg.h>
#include<wx/metafile.h>
#include<wx/gdicmn.h>
#include<wx/dcgraph.h> // there was some odd compile error when the wx headers were below the svector headers
                       // where wxVector<some class to do with wxGCDC>::push_back() wouldn't compile.
                       //not sure why.
#include"../include/svector/dep/plot/plot.h"
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

std::string sci::plot::Axis::createploptstring() const
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

void sci::plot::Axis::draw(plstream* pl, double scale, double pageWidth, double pageHeight)
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

void sci::plot::Label::draw(plstream* pl, double scale, double pageWidth, double pageHeight)
{
	//pl->vpor(0.0, 1.0, 0.0, 1.0);
	//pl->wind(0.0, 1.0, 0.0, 1.0);
	//pl->sfci(m_style);
	//pl->schr(1.0, m_size * scale / 72.0 * 25.4);
	//pl->scol0a(1, m_colour.r() * 255.99999, m_colour.g() * 255.99999, m_colour.b() * 255.99999, m_colour.a());
	//pl->col0(1);
	//pl->ptex(m_xPosition, m_yPosition, 1, 0, m_alignment, sci::toUtf8(m_text).c_str());
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
