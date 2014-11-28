#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include"../include/svector/splot.h"
#include"../include/svector/svector.h"
#include"../include/svector/sstring.h"
#include"../include/svector/sreadwrite.h"
#include<cmath>
#include<algorithm>
#include<wx/wx.h>
#include<wx/filename.h>
#include<wx/dcps.h>
#include<wx/metafile.h>
#include<wx/gdicmn.h>
#include"../include/svector/serr.h"

BEGIN_EVENT_TABLE( splotwindow, wxPanel )
	EVT_PAINT( splotwindow::OnPaint )
    EVT_ERASE_BACKGROUND(splotwindow::OnEraseBackGround)
END_EVENT_TABLE()

int splotexitcatcher(const char *message)
{
	wxString err(message, wxConvUTF8);
	wxMessageBox(err);
	return 0;
}

void splotabortcatcher(const char *message)
{
	wxString err(message, wxConvUTF8);
	//wxMessageBox(err);
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
	if(yindex>1000.0)
	{
		int myint=5;
	}
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


std::string loglonghand(double axisvalue)
{
	std::string result;
	result<<std::pow(10.0,axisvalue);
	return result;
}

void customlabelinterpreter(PLINT axis, PLFLT value, char* label, PLINT length,PLPointer function)
{
	customlabeler userfunction=( customlabeler )function;
	std::string stdlabel=userfunction(value);
	if(stdlabel.length()>(unsigned int)length) stdlabel.erase(length);
	strcpy_s(label,length,stdlabel.c_str());
}

hlscolour::hlscolour(double hue, double lightness, double saturation, double alpha)
{
	m_h=hue;
	m_l=std::max(std::min(lightness,1.0),0.0);
	m_s=std::max(std::min(saturation,1.0),0.0);
	m_a=std::max(std::min(alpha,1.0),0.0);
}
hlscolour::hlscolour()
{
	m_h=0.0;
	m_l=0.0;
	m_s=0.0;
	m_a=1.0;
}
rgbcolour hlscolour::convertToRgb()
{
	double r, g, b;
	plhlsrgb( m_h, m_l, m_s, &r, &g, &b );
	return rgbcolour( r, g, b, m_a );
}
rgbcolour::rgbcolour(double red, double green, double blue, double alpha)
{
	m_r=std::max(std::min(red,1.0),0.0);;
	m_g=std::max(std::min(green,1.0),0.0);
	m_b=std::max(std::min(blue,1.0),0.0);
	m_a=std::max(std::min(alpha,1.0),0.0);
}
rgbcolour::rgbcolour()
{
	m_r=0.0;
	m_g=0.0;
	m_b=0.0;
	m_a=1.0;
}
hlscolour rgbcolour::convertToHls()
{
	double h, l, s;
	plrgbhls(m_r, m_g, m_b, &h, &l, &s );
	return hlscolour( h, l, s, m_a );
}

splotcolourscale::splotcolourscale()
{
	setupdefault();
}

splotcolourscale::splotcolourscale(const std::vector<double> &value, const std::vector<rgbcolour> &colour,bool logarithmic, bool autostretch)
{
	sci::assertThrow( value.size()>1 && value.size()==colour.size(), sci::err() );
	//check values are ascending or descending, catch Nans at the same time
	bool ascending=true;
	bool descending=true;
	for(size_t i=1; i<value.size(); ++i)
	{
		ascending &= value[i]>=value[i-1];
		descending &= value[i]<=value[i-1];
	}
	bool monotonic=ascending || descending;
	sci::assertThrow(monotonic, sci::err());

	//assign values
	if(descending)
		m_value=sci::reverse(value);
	else
		m_value=value;

	//assign colours
	m_colour1.resize(colour.size());
	m_colour2.resize(colour.size());
	m_colour3.resize(colour.size());
	m_alpha.resize(colour.size());
	for(size_t i=0; i<colour.size(); ++i)
	{
		m_colour1[i]=colour[i].r();
		m_colour2[i]=colour[i].g();
		m_colour3[i]=(double)colour[i].b();
		m_alpha[i]=(double)colour[i].a();
	}
	if(descending)
	{
		sci::reverseinplace(m_colour1);
		sci::reverseinplace(m_colour2);
		sci::reverseinplace(m_colour3);
		sci::reverseinplace(m_alpha);
	}
	if(logarithmic) m_value=sci::log10(m_value);
	m_logarithmic=logarithmic;
	m_hls=false;

	//remove any nans and infinities - in particular they can be created by the logging
	std::vector<SBOOL> filter=m_value==m_value && m_value!=std::numeric_limits<double>::infinity() && m_value!=-std::numeric_limits<double>::infinity();
	m_value=sci::subvector(m_value, filter);
	m_colour1=sci::subvector(m_colour1, filter);
	m_colour2=sci::subvector(m_colour2, filter);
	m_colour3=sci::subvector(m_colour3, filter);
	m_alpha=sci::subvector(m_alpha, filter);
	sci::assertThrow(m_value.size()>1, sci::err());


	//remember the top and bottom and normalise scale to 0.0-1.0 range
	m_bottom=m_value[0];
	m_top=m_value.back();
	m_value-=m_bottom;
	m_value/=m_top-m_bottom;
	m_autovalue=autostretch;
}	

splotcolourscale::splotcolourscale(const std::vector<double> &value, const std::vector<hlscolour> &colour,bool logarithmic, bool autostretch)
{
	sci::assertThrow( value.size()>1 && value.size()==colour.size(), sci::err() );
	//check values are ascending or descending, catch Nans at the same time
	bool ascending=true;
	bool descending=true;
	for(size_t i=1; i<value.size(); ++i)
	{
		ascending &= value[i]>=value[i-1];
		descending &= value[i]<=value[i-1];
	}
	bool monotonic=ascending || descending;
	sci::assertThrow(monotonic, sci::err());

	//assign values
	if(descending)
		m_value=sci::reverse(value);
	else
		m_value=value;

	m_autovalue=autostretch;
	//assign values as are
	m_colour1.resize(colour.size());
	m_colour2.resize(colour.size());
	m_colour3.resize(colour.size());
	m_alpha.resize(colour.size());
	for(size_t i=0; i<colour.size(); ++i)
	{
		m_colour1[i]=colour[i].h();
		m_colour2[i]=colour[i].l();
		m_colour3[i]=colour[i].s();
		m_alpha[i]=colour[i].a();
	}

	//deal with nans for hues: hue should be a nan for greys which will help with blending
	//if they are all nans then set them all to 0.0;
	bool allnans=true;
	bool anynans=false;
	for(size_t i=0; i<m_colour1.size();++i)
	{
		if(m_colour1[i]==m_colour1[i]) allnans=false;
		else anynans=true;
	}
	if(allnans) m_colour1=std::vector<double>(m_colour1.size(),0.0);
	else if(anynans && m_colour1.size()==2)
	{
		if(m_colour1[0]!=m_colour1[0]) m_colour1[0]=m_colour1[1];
		else m_colour1[1]=m_colour1[0];
	}
	else if (anynans)
	{
		while(anynans)
		{
			anynans=false;
			//loop through getting any singles
			for(size_t i=1; i<m_colour1.size()-1; ++i)
			{
				if(m_colour1[i]!=m_colour1[i])
				{
					if(m_colour1[i-1]==m_colour1[i-1] && m_colour1[i+1]==m_colour1[i+1])
					{
						m_colour1[i]=sci::linearinterpolate(m_value[i],m_value[i-1],m_value[i+1],m_colour1[i-1],m_colour1[i+1]);
					}
				}
			}
			//then catch any with just a previous neighboor
			for(size_t i=1; i<m_colour1.size(); ++i)
			{
				if(m_colour1[i]!=m_colour1[i])
				{
					if(m_colour1[i-1]==m_colour1[i-1]) 
					{
						m_colour1[i]=m_colour1[i-1];
						++i;
					}
				}
			}
			//finally a neighbour after
			for(size_t i=m_colour1.size()-2; i!=0; --i)//note this works due to unsigned size_t 
			{
				if(m_colour1[i]!=m_colour1[i])
				{
					if(m_colour1[i+1]==m_colour1[i+1]) 
					{
						m_colour1[i]=m_colour1[i+1];
						--i;
						if(i==0) break;
					}
				}
			}
			if(m_colour1[0]!=m_colour1[0])
			{
				if(m_colour1[1]==m_colour1[1]) m_colour1[0]=m_colour1[1];
			}

			//check if any nans remain
			for(size_t i=0; i<m_colour1.size();++i)
			{
				if(m_colour1[i]!=m_colour1[i]) anynans=true;
			}
		}
	}

	if(logarithmic) 
		m_value=sci::log10(m_value);
	m_logarithmic=logarithmic;
	m_hls=true;
	
	//put the colours in the correct order too
	if(descending) 
	{
		sci::reverseinplace(m_colour1);
		sci::reverseinplace(m_colour2);
		sci::reverseinplace(m_colour3);
		sci::reverseinplace(m_alpha);
	}

	//remember the top and bottom and normalise scale to 0.0-1.0 range
	m_bottom=m_value[0];
	m_top=m_value.back();
	m_value-=m_bottom;
	m_value/=m_top-m_bottom;
}

splotcolourscale::splotcolourscale(const splotcolourscale& s)
{
	m_autovalue=s.m_autovalue;
	m_value=s.m_value;
	m_colour1=s.m_colour1;
	m_colour2=s.m_colour2;
	m_colour3=s.m_colour3;
	m_alpha=s.m_alpha;
	m_hls=s.m_hls;
	m_logarithmic=s.m_logarithmic;
	m_bottom=s.m_bottom;
	m_top=s.m_top;
}

splotcolourscale::splotcolourscale(const splotcolourscale& predefinedscale, double newbottom, double newtop)
{
	m_autovalue=false;
	m_value=predefinedscale.m_value;
	m_colour1=predefinedscale.m_colour1;
	m_colour2=predefinedscale.m_colour2;
	m_colour3=predefinedscale.m_colour3;
	m_alpha=predefinedscale.m_alpha;
	m_hls=predefinedscale.m_hls;
	m_logarithmic=predefinedscale.m_logarithmic;
	if(m_logarithmic)
	{
		m_bottom=std::log10(newbottom);
		m_top=std::log10(newtop);
	}
	else
	{
		m_bottom=newbottom;
		m_top=newtop;
	}
}

void splotcolourscale::interpolate( double value, double &c1, double &c2, double &c3, double &a) const
{
	if( value<= 0.0 )
	{
		c1=m_colour1[0];
		c2=m_colour2[0];
		c3=m_colour3[0];
		a=m_alpha[0];
		return;
	}
	else if( value>=1.0 )
	{
		c1=m_colour1.back();
		c2=m_colour2.back();
		c3=m_colour3.back();
		a=m_alpha.back();
		return;
	}
	
	size_t maxIndex=1;
	while(value>m_value[maxIndex])
		maxIndex++;
	double highWeight=(value-m_value[maxIndex-1])/(m_value[maxIndex]-m_value[maxIndex-1]);
	c1=m_colour1[maxIndex]*highWeight+m_colour1[maxIndex-1]*(1.0-highWeight);
	c2=m_colour2[maxIndex]*highWeight+m_colour2[maxIndex-1]*(1.0-highWeight);
	c3=m_colour3[maxIndex]*highWeight+m_colour3[maxIndex-1]*(1.0-highWeight);
	a=m_alpha[maxIndex]*highWeight+m_alpha[maxIndex-1]*(1.0-highWeight);
}

rgbcolour splotcolourscale::getRgbNormalisedScale( double value ) const
{
	if( m_hls )
	{
		hlscolour hls=getHlsNormalisedScale( value );
		double r, g, b;
		plhlsrgb( hls.h(), hls.l(), hls.s(), &r, &g, &b );
		return rgbcolour( r, g, b, hls.a() );
	}
	double r, g, b, a;
	interpolate( value, r, g, b, a );
	return rgbcolour(r, g, b, a);
}

rgbcolour splotcolourscale::getRgbOriginalScale( double value ) const
{
	return getRgbNormalisedScale(getNormalisedValue( value ) );
}

hlscolour splotcolourscale::getHlsNormalisedScale( double value ) const
{
	if( !m_hls )
	{
		rgbcolour rgb=getRgbNormalisedScale( value );
		double h, l, s;
		plrgbhls( rgb.r(), rgb.g(), rgb.b(), &h, &l, &s );
		return hlscolour( h, l, s, rgb.a() );
	}
	double h, l, s, a;
	interpolate( value, h, l, s, a );
	return hlscolour(h, l, s, a);
}

hlscolour splotcolourscale::getHlsOriginalScale( double value ) const
{
	return getHlsNormalisedScale(getNormalisedValue( value ) );
}
	
double splotcolourscale::getNormalisedValue( double value ) const
{
	return (value-m_bottom)/(m_top-m_bottom);
}

splotcolourscale& splotcolourscale::operator=(const splotcolourscale& s)
{
	m_autovalue=s.m_autovalue;
	m_value=s.m_value;
	m_colour1=s.m_colour1;
	m_colour2=s.m_colour2;
	m_colour3=s.m_colour3;
	m_alpha=s.m_alpha;
	m_hls=s.m_hls;
	m_logarithmic=s.m_logarithmic;
	m_bottom=s.m_bottom;
	m_top=s.m_top;
	return *this;
}

void splotcolourscale::setupdefault()
{
	m_autovalue=true;
	//assign values as are
	m_colour1.resize(0);
	m_colour2.resize(0);
	m_colour3.resize(0);
	m_alpha.resize(0);
	m_colour1.push_back(240.0);
	m_colour1.push_back(0.0);
	m_colour2.push_back(0.5);
	m_colour2.push_back(0.5);
	m_colour3.push_back(1.0);
	m_colour3.push_back(1.0);
	m_alpha.push_back(1.0);
	m_alpha.push_back(1.0);
	m_value.resize(0);
	m_value.push_back(0.0);
	m_value.push_back(1.0);
	m_logarithmic=false;
	m_hls=true;
}	
rgbcolour splotcolourscale::getRgbOffscaleBottom()
{
	if(!m_hls)
		return rgbcolour (m_colour1[0], m_colour2[0], m_colour3[0], m_alpha[0]);
	else
		return hlscolour (m_colour1[0], m_colour2[0], m_colour3[0], m_alpha[0]).convertToRgb();
}
rgbcolour splotcolourscale::getRgbOffscaleTop()
{
	if(!m_hls)
		return rgbcolour (m_colour1.back(), m_colour2.back(), m_colour3.back(), m_alpha.back());
	else
		return hlscolour (m_colour1.back(), m_colour2.back(), m_colour3.back(), m_alpha.back()).convertToRgb();
}
hlscolour splotcolourscale::getHlsOffscaleBottom()
{
	if(!m_hls)
		return rgbcolour (m_colour1[0], m_colour2[0], m_colour3[0], m_alpha[0]).convertToHls();
	else
		return hlscolour (m_colour1[0], m_colour2[0], m_colour3[0], m_alpha[0]);
}
hlscolour splotcolourscale::getHlsOffscaleTop()
{
	if(!m_hls)
		return rgbcolour (m_colour1.back(), m_colour2.back(), m_colour3.back(), m_alpha.back()).convertToHls();
	else
		return hlscolour (m_colour1.back(), m_colour2.back(), m_colour3.back(), m_alpha.back());
}

splotsizescale::splotsizescale(const std::vector<double> &value, const std::vector<double> &size, bool logarithmic)
{
	//assign values as are
	m_value=value;
	m_size=size;
	m_logarithmic=logarithmic;
	//check there is at least one value in m_value
	if(m_value.size()==0) m_value.resize(1,0.0);
	//check values are ascending
	for(size_t i=1; i<m_value.size(); ++i)
	{
		if(m_value[i]<=m_value[i-1])m_value.resize(i);
	}
	//check the sizes and values are the same length
	m_size.resize(m_value.size(),0.0);
}

double splotsizescale::getsize(double value)
{
	if(value<=m_value[0]) return m_size[0];
	if(value>=m_value.back()) return m_size.back();
	size_t lowerindex=0;
	while(value<m_value[lowerindex])++lowerindex;
	return (m_size[lowerindex]-m_size[lowerindex+1])/(m_value[lowerindex]-m_value[lowerindex+1])*(value-m_value[lowerindex])+m_size[lowerindex];
}


splotaxis::splotaxis(double min, double max, wxString title, wxString titlefont, PLUNICODE titlestyle, double titlesize, double titledistance, const wxColour &titlecolour, double intersectpoint, wxColour colour, int linethickness, bool logarithmic, bool time, double majorticklength, double minorticklength, bool tickspositive, bool ticksnegative, bool showlabels, bool labelpositionpositive, wxString labelfont, PLUNICODE labelstyle, bool labelsrotated, double labelsize, const wxColour &labelcolour, bool autodecimalplaces, unsigned int ndecimalplaces, bool automaxndigits, int maxndigits)
{
	m_min=min;
	m_max=max;
	m_automajorinterval=true;
	m_autonsubticks=true;
	m_intersect=intersectpoint;
	m_logarithmic=logarithmic;
	m_timeformat=wxEmptyString;
	m_customlabelcreator=NULL;
	m_colour=colour;
	m_linethickness=linethickness;
	m_tickspos=tickspositive;
	m_ticksneg=ticksnegative;
	m_majorticklength=std::abs(majorticklength);
	m_minorticklength=std::abs(minorticklength);
	m_showlabels=showlabels;
	m_labelfont=labelfont;
	m_labelfci=labelstyle;
	m_rotatelabels=labelsrotated;
	m_labelsize=labelsize;
	m_labelcolour=labelcolour;
	m_labelpositionpositive=labelpositionpositive;
	m_title=title;
	m_titlefont=titlefont;
	m_titlefci=titlestyle;
	m_rotatetitle=false;
	m_titlesize=titlesize;
	m_titledistance=titledistance;
	m_titlecolour=titlecolour;
	m_autodecimalplaces=autodecimalplaces;
	m_ndecimalplaces=ndecimalplaces;
	if(automaxndigits==true)m_maxndigits=0;
	else m_maxndigits=maxndigits;

	m_haschanged=true;
}
splotaxis::splotaxis(double min, double max, double majorinterval, double nsubticks, wxString title, wxString titlefont, PLUNICODE titlestyle, double titlesize, double titledistance, const wxColour &titlecolour, double intersectpoint, wxColour colour, int linethickness, bool logarithmic, bool time, double majorticklength, double minorticklength, bool tickspositive, bool ticksnegative, bool showlabels, bool labelpositionpositive, wxString labelfont, PLUNICODE labelstyle, bool labelsrotated, double labelsize, const wxColour &labelcolour, bool autodecimalplaces, unsigned int ndecimalplaces, bool automaxndigits, int maxndigits)
{
	m_min=min;
	m_max=max;
	m_majorinterval=majorinterval;
	m_nsubticks=nsubticks;
	m_automajorinterval=false;
	m_autonsubticks=false;
	m_intersect=intersectpoint;
	m_logarithmic=logarithmic;
	m_timeformat=wxEmptyString;
	m_customlabelcreator=NULL;
	m_colour=colour;
	m_linethickness=linethickness;
	m_tickspos=tickspositive;
	m_ticksneg=ticksnegative;
	m_majorticklength=std::abs(majorticklength);
	m_minorticklength=std::abs(minorticklength);
	m_showlabels=showlabels;
	m_labelfont=labelfont;
	m_labelfci=labelstyle;
	m_rotatelabels=labelsrotated;
	m_labelsize=labelsize;
	m_labelcolour=labelcolour;
	m_labelpositionpositive=labelpositionpositive;
	m_title=title;
	m_titlefont=titlefont;
	m_titlefci=titlestyle;
	m_rotatetitle=false;
	m_titlesize=titlesize;
	m_titledistance=titledistance;
	m_titlecolour=titlecolour;
	m_autodecimalplaces=autodecimalplaces;
	m_ndecimalplaces=ndecimalplaces;
	if(automaxndigits==true)m_maxndigits=0;
	else m_maxndigits=maxndigits;

	m_haschanged=true;
}

void splotaxis::autointervalson()
{
	m_automajorinterval=true;
	m_autonsubticks=true;
	m_haschanged=true;
}

void splotaxis::setmajorinterval(double interval)
{
	m_majorinterval=interval;
	m_automajorinterval=false;
	m_haschanged=true;
}

void splotaxis::setnsubticks(unsigned int nsubticks)
{
	m_nsubticks=nsubticks;
	m_autonsubticks=false;
	m_haschanged=true;
}

void splot2d::setallparams(bool logx, bool logy,std::string title, double titlesize, double titledistance, std::string titlefont, int32_t titlestyle, wxColour titlecolour,void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&),void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&))
{
	//these aren't given in the constructor parameters but must be true for this constructor
	m_xautointersect=true;
	m_yautointersect=true;
	m_xautolimits=true;
	m_yautolimits=true;

	//also set the transformfunc to an appropriate value
	m_transfunc1dxy=transformfunc1dxy;
	m_transfunc2dxy=transformfunc2dxy;

	//now for the given parameters
	m_xaxis.m_logarithmic=logx;
	m_yaxis.m_logarithmic=logy;
	m_title=wxString(title.c_str(),wxConvUTF8);
	m_titlesize=titlesize;
	m_titledistance=titledistance;
	m_titlefont=wxString(titlefont.c_str(),wxConvUTF8);
	m_titlefci=titlestyle;
	m_titlecolour=titlecolour;
	m_haschanged=true;
	
	calculateautolimits();
}

splot2d::splot2d(bool logx, bool logy,std::string title, double titlesize, double titledistance, std::string titlefont, int32_t titlestyle, wxColour titlecolour,void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&),void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&))
{
	//this is the default constructor so just set the all the params to default values
	setallparams(logx, logy, title, titlesize, titledistance, titlefont, titlestyle, titlecolour,transformfunc1dxy,transformfunc2dxy);
	//set y axis labels the right way round and make the title further away
	m_yaxis.m_rotatelabels=true;
}

splot2d::splot2d(double minx, double maxx, double miny, double maxy, bool logx, bool logy, std::string title, double titlesize, double titledistance, std::string titlefont, int32_t titlestyle, wxColour titlecolour,void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&),void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&))
{
	//set the params to default values
	setallparams(logx, logy, title, titlesize, titledistance, titlefont, titlestyle, titlecolour,transformfunc1dxy,transformfunc2dxy);

	//modify these to give manual axis limits
	m_xautolimits=false;
	m_yautolimits=false;
	m_xaxis.m_min=minx;
	m_xaxis.m_max=maxx;
	m_yaxis.m_min=miny;
	m_yaxis.m_max=maxy;
	m_yaxis.m_rotatelabels=true;

	//use calculateautolimits to set new intersects
	calculateautolimits();
}
splot2d::splot2d(double minx, double maxx, double miny, double maxy, double xintersect, double yintersect, bool logx, bool logy, std::string title, double titlesize, double titledistance, std::string titlefont, int32_t titlestyle, wxColour titlecolour,void (*transformfunc1dxy)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&),void (*transformfunc2dxy)(double, double, const std::vector< std::vector< double > >&, const std::vector< std::vector< double > >&, double&, double&))
{
	//set the params to default values
	setallparams(logx, logy, title, titlesize, titledistance, titlefont, titlestyle, titlecolour,transformfunc1dxy,transformfunc2dxy);

	//modify these to give manual axis limits and intersects
	m_xautolimits=false;
	m_yautolimits=false;
	m_xaxis.m_min=minx;
	m_xaxis.m_max=maxx;
	m_xaxis.m_intersect=xintersect;
	m_yaxis.m_min=miny;
	m_yaxis.m_max=maxy;
	m_yaxis.m_intersect=yintersect;
	m_yaxis.m_rotatelabels=true;
}

//scatter
void splot2d::adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &xposerrs, const std::vector<double> &xnegerrs, const std::vector<double> &yposerrs, const std::vector<double> &ynegerrs, wxColour pointcolour, double pointsize, std::string pointsymbol, wxColour linecolour, double linewidth, std::string linestyle, wxColour xerrcolour, wxColour yerrcolour, double xerrwidth, double yerrwidth, splotTransformer *transformer)
{
	//checks
	if(xs.size()!=ys.size()) return;
	if(xposerrs.size()!=0 && xposerrs.size()!=xs.size())return;
	if(xposerrs.size()!=0 && xnegerrs.size()!=xs.size())return;
	if(yposerrs.size()!=0 && yposerrs.size()!=xs.size())return;
	if(ynegerrs.size()!=0 && ynegerrs.size()!=xs.size())return;

	//increase the size of all data
	incrementdatasize();

	//assign the new data sets needed
	m_xs.back()=xs;
	m_ys.back()=ys;
	if(xposerrs.size()>0 && xnegerrs.size()>0)
	{
		m_xpluserrs.back()=xs+xposerrs;
		m_xminuserrs.back()=xs-xnegerrs;
	}
	else if(xposerrs.size()>0)
	{
		m_xpluserrs.back()=xs+xposerrs;
		m_xminuserrs.back()=xs;
	}
	else if(xnegerrs.size()>0)
	{
		m_xpluserrs.back()=xs;
		m_xminuserrs.back()=xs-xnegerrs;
	}
	if(yposerrs.size()>0 && ynegerrs.size()>0)
	{
		m_ypluserrs.back()=ys+yposerrs;
		m_yminuserrs.back()=ys-ynegerrs;
	}
	else if(yposerrs.size()>0)
	{
		m_ypluserrs.back()=ys+yposerrs;
		m_yminuserrs.back()=ys;
	}
	else if(ynegerrs.size()>0)
	{
		m_ypluserrs.back()=ys;
		m_yminuserrs.back()=ys-ynegerrs;
	}

	//set the data properties as needed
	m_pointcolour.back()=pointcolour;
	m_pointsize.back()=pointsize;
	m_pointchar.back()=wxString(pointsymbol.c_str(),wxConvUTF8);
	m_pointfont.back()=wxT("plotsymbols");
	m_linecolour.back()=linecolour;
	m_linethickness.back()=linewidth;
	m_linestyle.back()=wxString(linestyle.c_str(),wxConvUTF8);
	m_xerrcolour.back()=xerrcolour;
	m_yerrcolour.back()=yerrcolour;
	m_xerrthickness.back()=xerrwidth;
	m_yerrthickness.back()=yerrwidth;
	
	if(m_xaxis.m_logarithmic) 
	{
		m_xsl.back()=sci::log10(xs);
		m_xpluserrsl.back()=sci::log10(m_xpluserrs.back());
		m_xminuserrsl.back()=sci::log10(m_xminuserrs.back());
		for(size_t i=0; i<m_xminuserrsl.back().size(); ++i)
		{
			if(m_xminuserrsl.back()[i]!=m_xminuserrsl.back()[i])
				m_xminuserrsl.back()[i]=DBL_MIN_10_EXP-1;
		}
	}
	if(m_yaxis.m_logarithmic) 
	{
		m_ysl.back()=sci::log10(ys);
		m_ypluserrsl.back()=sci::log10(m_ypluserrs.back());
		m_yminuserrsl.back()=sci::log10(m_yminuserrs.back());
		for(size_t i=0; i<m_yminuserrsl.back().size(); ++i)
		{
			if(m_yminuserrsl.back()[i]!=m_yminuserrsl.back()[i])
				m_yminuserrsl.back()[i]=DBL_MIN_10_EXP-1;
		}
	}
	m_transformers.back()=transformer;

	calculateautolimits();
	m_haschanged=true;
}

//scatter varying colour
void splot2d::adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels, const std::vector<double> &xposerrs, const std::vector<double> &xnegerrs, const std::vector<double> &yposerrs, const std::vector<double> &ynegerrs, double pointsize, std::string pointsymbol, wxColour linecolour, double linewidth, std::string linestyle, wxColour xerrcolour, wxColour yerrcolour, double xerrwidth, double yerrwidth, splotTransformer *transformer)
{
	//checks
	if(xs.size()!=ys.size()) return;
	if(xs.size()!=zs.size()) return;
	if(xposerrs.size()!=0 && xposerrs.size()!=xs.size())return;
	if(xposerrs.size()!=0 && xnegerrs.size()!=xs.size())return;
	if(yposerrs.size()!=0 && yposerrs.size()!=xs.size())return;
	if(ynegerrs.size()!=0 && ynegerrs.size()!=xs.size())return;

	//increase the size of all data
	incrementdatasize();

	//assign the new data sets needed
	m_xs.back()=xs;
	m_ys.back()=ys;
	m_colunstructzs.back()=zs;
	if(xposerrs.size()>0 && xnegerrs.size()>0)
	{
		m_xpluserrs.back()=xs+xposerrs;
		m_xminuserrs.back()=xs-xnegerrs;
	}
	else if(xposerrs.size()>0)
	{
		m_xpluserrs.back()=xs+xposerrs;
		m_xminuserrs.back()=xs;
	}
	else if(xnegerrs.size()>0)
	{
		m_xpluserrs.back()=xs;
		m_xminuserrs.back()=xs-xnegerrs;
	}
	if(yposerrs.size()>0 && ynegerrs.size()>0)
	{
		m_ypluserrs.back()=ys+yposerrs;
		m_yminuserrs.back()=ys-ynegerrs;
	}
	else if(yposerrs.size()>0)
	{
		m_ypluserrs.back()=ys+yposerrs;
		m_yminuserrs.back()=ys;
	}
	else if(ynegerrs.size()>0)
	{
		m_ypluserrs.back()=ys;
		m_yminuserrs.back()=ys-ynegerrs;
	}

	if(m_xaxis.m_logarithmic) 
	{
		m_xsl.back()=sci::log10(xs);
		m_xpluserrsl.back()=sci::log10(m_xpluserrs.back());
		m_xminuserrsl.back()=sci::log10(m_xminuserrs.back());
	}
	if(m_yaxis.m_logarithmic) 
	{
		m_ysl.back()=sci::log10(ys);
		m_ypluserrsl.back()=sci::log10(m_ypluserrs.back());
		m_yminuserrsl.back()=sci::log10(m_yminuserrs.back());
	}

	if(colourscale.m_logarithmic) 
	{
		m_colunstructzsl.back()=sci::log10(m_colunstructzs.back());
		sci::replaceNegativeInfs(m_colunstructzsl, std::numeric_limits<double>::lowest());
		sci::replacePositiveInfs(m_colunstructzsl, std::numeric_limits<double>::max());
	}
	else
	{
		sci::replaceNegativeInfs(m_colunstructzs, std::numeric_limits<double>::lowest());
		sci::replacePositiveInfs(m_colunstructzs, std::numeric_limits<double>::max());
	}

	m_pointsize.back()=pointsize;
	m_pointchar.back()=wxString(pointsymbol.c_str(),wxConvUTF8);
	m_pointfont.back()=wxT("plotsymbols");
	m_linecolour.back()=linecolour;
	m_linethickness.back()=linewidth;
	m_linestyle.back()=wxString(linestyle.c_str(),wxConvUTF8);
	m_xerrcolour.back()=xerrcolour;
	m_yerrcolour.back()=yerrcolour;
	m_xerrthickness.back()=xerrwidth;
	m_yerrthickness.back()=yerrwidth;
	setupcolourscale(ncolourlevels,colourscale,true,true,0,0.0,0.0,0);
	m_transformers.back()=transformer;
	
	calculateautolimits();
	m_haschanged=true;
}

//scatter varying size
void splot2d::adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const splotsizescale &sizescale, const std::vector<double> &xposerrs, const std::vector<double> &xnegerrs, const std::vector<double> &yposerrs, const std::vector<double> &ynegerrs, wxColour pointcolour, std::string pointsymbol, wxColour linecolour, double linewidth, std::string linestyle, wxColour xerrcolour, wxColour yerrcolour, double xerrwidth, double yerrwidth, splotTransformer *transformer)
{
	//checks
	if(xs.size()!=ys.size()) return;
	if(xs.size()!=zs.size()) return;
	m_sizeunstructzs.back()=zs;
	if(xposerrs.size()!=0 && xposerrs.size()!=xs.size())return;
	if(xposerrs.size()!=0 && xnegerrs.size()!=xs.size())return;
	if(yposerrs.size()!=0 && yposerrs.size()!=xs.size())return;
	if(ynegerrs.size()!=0 && ynegerrs.size()!=xs.size())return;

	//increase the size of all data
	incrementdatasize();

	//assign the new data sets needed
	m_xs.back()=xs;
	m_ys.back()=ys;
	if(xposerrs.size()>0 && xnegerrs.size()>0)
	{
		m_xpluserrs.back()=xs+xposerrs;
		m_xminuserrs.back()=xs-xnegerrs;
	}
	else if(xposerrs.size()>0)
	{
		m_xpluserrs.back()=xs+xposerrs;
		m_xminuserrs.back()=xs;
	}
	else if(xnegerrs.size()>0)
	{
		m_xpluserrs.back()=xs;
		m_xminuserrs.back()=xs-xnegerrs;
	}
	if(yposerrs.size()>0 && ynegerrs.size()>0)
	{
		m_ypluserrs.back()=ys+yposerrs;
		m_yminuserrs.back()=ys-ynegerrs;
	}
	else if(yposerrs.size()>0)
	{
		m_ypluserrs.back()=ys+yposerrs;
		m_yminuserrs.back()=ys;
	}
	else if(ynegerrs.size()>0)
	{
		m_ypluserrs.back()=ys;
		m_yminuserrs.back()=ys-ynegerrs;
	}

	//set the data properties as needed
	m_pointcolour.back()=pointcolour;
	m_pointchar.back()=wxString(pointsymbol.c_str(),wxConvUTF8);
	m_pointfont.back()=wxT("plotsymbols");
	m_linecolour.back()=linecolour;
	m_linethickness.back()=linewidth;
	m_linestyle.back()=wxString(linestyle.c_str(),wxConvUTF8);
	m_xerrcolour.back()=xerrcolour;
	m_yerrcolour.back()=yerrcolour;
	m_xerrthickness.back()=xerrwidth;
	m_yerrthickness.back()=yerrwidth;
	m_sizescale.back()=sizescale;
	
	if(m_xaxis.m_logarithmic) 
	{
		m_xsl.back()=sci::log10(xs);
		m_xpluserrsl.back()=sci::log10(m_xpluserrs.back());
		m_xminuserrsl.back()=sci::log10(m_xminuserrs.back());
	}
	if(m_yaxis.m_logarithmic) 
	{
		m_ysl.back()=sci::log10(ys);
		m_ypluserrsl.back()=sci::log10(m_ypluserrs.back());
		m_yminuserrsl.back()=sci::log10(m_yminuserrs.back());
	}
	m_transformers.back()=transformer;

	calculateautolimits();
	m_haschanged=true;
}

//scatter with varying point colour and size
void splot2d::adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &colour_zs, const std::vector<double> &size_zs, const splotsizescale &sizescale, const splotcolourscale &colourscale, unsigned int ncolourlevels, const std::vector<double> &xposerrs, const std::vector<double> &xnegerrs, const std::vector<double> &yposerrs, const std::vector<double> &ynegerrs, std::string pointsymbol, wxColour linecolour, double linewidth, std::string linestyle, wxColour xerrcolour, wxColour yerrcolour, double xerrwidth, double yerrwidth, splotTransformer *transformer)
{
	//checks
	if(xs.size()!=ys.size()) return;
	if(xs.size()!=colour_zs.size()) return;
	if(xs.size()!=size_zs.size()) return;
	if(xposerrs.size()!=0 && xposerrs.size()!=xs.size())return;
	if(xposerrs.size()!=0 && xnegerrs.size()!=xs.size())return;
	if(yposerrs.size()!=0 && yposerrs.size()!=xs.size())return;
	if(ynegerrs.size()!=0 && ynegerrs.size()!=xs.size())return;

	//increase the size of all data
	incrementdatasize();

	//assign the new data sets needed
	m_xs.back()=xs;
	m_ys.back()=ys;
	m_colunstructzs.back()=colour_zs;
	m_sizeunstructzs.back()=size_zs;
	if(xposerrs.size()>0 && xnegerrs.size()>0)
	{
		m_xpluserrs.back()=xs+xposerrs;
		m_xminuserrs.back()=xs-xnegerrs;
	}
	else if(xposerrs.size()>0)
	{
		m_xpluserrs.back()=xs+xposerrs;
		m_xminuserrs.back()=xs;
	}
	else if(xnegerrs.size()>0)
	{
		m_xpluserrs.back()=xs;
		m_xminuserrs.back()=xs-xnegerrs;
	}
	if(yposerrs.size()>0 && ynegerrs.size()>0)
	{
		m_ypluserrs.back()=ys+yposerrs;
		m_yminuserrs.back()=ys-ynegerrs;
	}
	else if(yposerrs.size()>0)
	{
		m_ypluserrs.back()=ys+yposerrs;
		m_yminuserrs.back()=ys;
	}
	else if(ynegerrs.size()>0)
	{
		m_ypluserrs.back()=ys;
		m_yminuserrs.back()=ys-ynegerrs;
	}

	if(m_xaxis.m_logarithmic) 
	{
		m_xsl.back()=sci::log10(xs);
		m_xpluserrsl.back()=sci::log10(m_xpluserrs.back());
		m_xminuserrsl.back()=sci::log10(m_xminuserrs.back());
	}
	if(m_yaxis.m_logarithmic) 
	{
		m_ysl.back()=sci::log10(ys);
		m_ypluserrsl.back()=sci::log10(m_ypluserrs.back());
		m_yminuserrsl.back()=sci::log10(m_yminuserrs.back());
	}
	if(colourscale.m_logarithmic) 
	{
		m_colunstructzsl.back()=sci::log10(m_colunstructzs.back());
		sci::replaceNegativeInfs(m_colunstructzsl, std::numeric_limits<double>::lowest());
		sci::replacePositiveInfs(m_colunstructzsl, std::numeric_limits<double>::max());
	}
	else
	{
		sci::replaceNegativeInfs(m_colunstructzs, std::numeric_limits<double>::lowest());
		sci::replacePositiveInfs(m_colunstructzs, std::numeric_limits<double>::max());
	}

	//set the data properties as needed
	m_pointchar.back()=wxString(pointsymbol.c_str(),wxConvUTF8);
	m_pointfont.back()=wxT("plotsymbols");
	m_linecolour.back()=linecolour;
	m_linethickness.back()=linewidth;
	m_linestyle.back()=wxString(linestyle.c_str(),wxConvUTF8);
	m_xerrcolour.back()=xerrcolour;
	m_yerrcolour.back()=yerrcolour;
	m_xerrthickness.back()=xerrwidth;
	m_yerrthickness.back()=yerrwidth;
	m_sizescale.back()=sizescale;
	setupcolourscale(ncolourlevels,colourscale,true,true,0,0.0,0.0,0);
	m_transformers.back()=transformer;

	calculateautolimits();
	m_haschanged=true;
}

//contour/colour
void splot2d::adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels, bool filloffscaletop, bool filloffscalebottom, wxColour contourcolour, double contourwidth, std::string contourstyle, int ncontourlevels, double mincontour, double maxcontour, double contourlabelssize, splotTransformer *transformer)
{
	//checks
	if(xs.size()!=zs.size()) return;
	for(size_t i=0; i<zs.size(); ++i) 
	{
		if(zs[i].size()!=ys.size()) return;
	}

	//increase the size of all data
	incrementdatasize();

	m_xs.back()=xs;
	m_ys.back()=ys;
	m_structzs.back()=zs;
	m_transformers.back()=transformer;
	m_isGrid.back()=false;

	//check the x values for nans
	double *px0=&m_xs.back()[0];
	for (size_t i=0; i<m_xs.back().size(); ++i) 
	{
		if(*(px0+i) != *(px0+i))
		{
			m_xs.back().erase(m_xs.back().begin()+i);
			m_structzs.back().erase(m_structzs.back().begin()+i);
			--i;
		}
	}
	if(m_xs.back().size()==0)
	{
		//if we are left with no x values then set all data to zero size so they won't be plotted
		m_xs.back()=std::vector<double>(0);
		m_ys.back()=std::vector<double>(0);
		m_structzs.back()=std::vector<std::vector<double>>(0);
		return;
	}
	//check the y values for nans
	std::vector<double> *pz0=&m_structzs.back()[0];
	double *py0=&m_ys.back()[0];
	for (size_t i=0; i<m_ys.back().size(); ++i) 
	{
		if(*(py0+i) != *(py0+i))
		{
			m_ys.back().erase(m_ys.back().begin()+i);
			for(size_t j=0; j<m_structzs.size(); ++j)(pz0+j)->erase((pz0+j)->begin()+i);
			--i;
		}
	}
	if(m_ys.back().size()==0)
	{
		//if we are left with no x values then set all data to zero size so they won't be plotted
		m_xs.back()=std::vector<double>(0);
		m_ys.back()=std::vector<double>(0);
		m_structzs.back()=std::vector<std::vector<double>>(0);
		return;
	}

	//generate logged values if needed and replace infinities with max/lowest doubles
	if(colourscale.m_logarithmic) 
	{
		m_structzsl.back()=sci::log10(m_structzs.back());
		sci::replaceNegativeInfs(m_structzsl, std::numeric_limits<double>::lowest());
		sci::replacePositiveInfs(m_structzsl, std::numeric_limits<double>::max());
	}
	else
	{
		sci::replaceNegativeInfs(m_structzs, std::numeric_limits<double>::lowest());
		sci::replacePositiveInfs(m_structzs, std::numeric_limits<double>::max());
	}
	if(m_xaxis.m_logarithmic)m_xsl.back()=sci::log10(xs);
	if(m_yaxis.m_logarithmic)m_ysl.back()=sci::log10(ys);

	//replace any nans in the z data with max double
	//sci::replacenans(m_structzs.back(),std::numeric_limits<double>::max());
	//sci::replacenans(m_structzsl.back(),std::numeric_limits<double>::max());

	m_linecolour.back()=contourcolour;
	m_linethickness.back()=contourwidth;
	m_linestyle.back()=wxString(contourstyle.c_str(),wxConvUTF8);
	setupcolourscale(ncolourlevels,colourscale,filloffscaletop,filloffscalebottom,ncontourlevels,maxcontour,mincontour,contourlabelssize);

	calculateautolimits();
	m_haschanged=true;
}

//contour/colour with 2d x and y
void splot2d::adddata(const std::vector< std::vector <double> > &xs, const std::vector< std::vector <double> > &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels, bool filloffscaletop, bool filloffscalebottom, wxColour contourcolour, double contourwidth, std::string contourstyle, unsigned int ncontourlevels, double mincontour, double maxcontour, double contourlabelssize, splotTransformer *transformer)
{
	//checks
	if(xs.size()!=zs.size()) return;
	if(ys.size()!=zs.size()) return;
	for(size_t i=0; i<zs.size(); ++i) 
	{
		if(zs[i].size()!=xs[i].size()) return;
		if(zs[i].size()!=ys[i].size()) return;
	}

	//increase the size of all data
	incrementdatasize();

	m_xs2d.back()=xs;
	m_ys2d.back()=ys;
	m_structzs.back()=zs;
	m_transformers.back()=transformer;
	m_isGrid.back()=false;


	//generate logged values if needed
	if(m_xaxis.m_logarithmic)m_xs2dl.back()=sci::log10(xs);
	if(m_yaxis.m_logarithmic)m_ys2dl.back()=sci::log10(ys);
	if(colourscale.m_logarithmic) m_structzsl.back()=sci::log10(m_structzs.back());

	//replace any nans in the z data with max double
	sci::replacenans(m_structzs.back(),std::numeric_limits<double>::max());
	sci::replacenans(m_structzsl.back(),std::numeric_limits<double>::max());

	m_linecolour.back()=contourcolour;
	m_linethickness.back()=contourwidth;
	m_linestyle.back()=wxString(contourstyle.c_str(),wxConvUTF8);
	setupcolourscale(ncolourlevels,colourscale,filloffscaletop,filloffscalebottom,ncontourlevels,maxcontour,mincontour,contourlabelssize);

	calculateautolimits();
	m_haschanged=true;
}

//bar chart
void splot2d::adddata(const std::vector<double> &minedges, const std::vector<double> &maxedges, const std::vector<double> heights, wxColour fillcolour,wxColour linecolour, double linewidth, std::string linestyle, bool beginatzero)
{

	//checks
	if(minedges.size()!=maxedges.size()) return;
	if(minedges.size()!=heights.size()) return;

	//increase the size of all data
	incrementdatasize();

	m_lowerxlimits.back()=minedges;
	m_upperxlimits.back()=maxedges;
	m_ys.back()=heights;
	m_beginAtZeros.back()=beginatzero;

	m_pointcolour.back()=fillcolour;
	m_linecolour.back()=linecolour;
	m_linethickness.back()=linewidth;
	m_linestyle.back()=wxString(linestyle.c_str(),wxConvUTF8);
	
	if(m_xaxis.m_logarithmic) 
	{
		m_lowerxlimitsl.back()=sci::log10(minedges);
		m_upperxlimitsl.back()=sci::log10(maxedges);
	}
	if(m_yaxis.m_logarithmic) 
	{
		m_ysl.back()=sci::log10(heights);
	}

	calculateautolimits();
	m_haschanged=true;
}

//colourplot - like a 3d histogram with colours representing bar height viewed from above 
//void splot2d::adddata(const std::vector< std::vector <double> > &xs, const std::vector< std::vector <double> > &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels, bool filloffscaletop, bool filloffscalebottom, wxColour contourcolour, int contourwidth, std::string contourstyle, unsigned int ncontourlevels, double mincontour, double maxcontour, double contourlabelssize)
//{
//}

//vector/arrow plot
void splot2d::adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector< std::vector<double> > &us, const std::vector< std::vector<double> > &vs, wxColour linecolour, double linewidth, const std::vector<double> &arrowstyle, std::string linestyle, splotTransformer *transformer)
{
	//checks
	if(xs.size()==0) return;
	if(ys.size()==0) return;
	if(us.size()!=xs.size()) return;
	if(vs.size()!=xs.size()) return;
	if(!sci::rectangular(us)) return;
	if(!sci::rectangular(vs)) return;
	if(us[0].size()!=ys.size()) return;
	if(vs[0].size()!=ys.size()) return;

	//increase the size of all data
	incrementdatasize();

	//set data as needed
	m_xs.back()=xs;
	m_ys.back()=ys;
	m_us.back()=us;
	m_vs.back()=vs;
	if(m_xaxis.m_logarithmic)m_xsl.back()=sci::log10(xs);
	if(m_yaxis.m_logarithmic)m_ysl.back()=sci::log10(ys);

	//set properties as needed
	m_linecolour.back()=linecolour;
	m_linethickness.back()=linewidth;
	m_arrowstyle.back()=arrowstyle;
	m_transformers.back()=transformer;
	
	calculateautolimits();
	m_haschanged=true;
}
//vector/arrow plot with 2d x/y
void splot2d::adddata(const std::vector<std::vector<double>> &xs, const std::vector<std::vector<double>> &ys, const std::vector< std::vector<double> > &us, const std::vector< std::vector<double> > &vs, wxColour linecolour, double linewidth, const std::vector<double> &arrowstyle, std::string linestyle, splotTransformer *transformer)
{
	//checks
	if(xs.size()==0) return;
	if(ys.size()==0) return;
	if(xs[0].size()==0) return;
	if(ys[0].size()!=xs[0].size()) return;
	if(!sci::rectangular(xs)) return;
	if(!sci::rectangular(ys)) return;
	if(us.size()!=xs.size()) return;
	if(vs.size()!=xs.size()) return;
	if(!sci::rectangular(us)) return;
	if(!sci::rectangular(vs)) return;
	if(us[0].size()!=xs[0].size()) return;
	if(vs[0].size()!=xs[0].size()) return;

	//increase the size of all data
	incrementdatasize();

	//set data as needed
	m_xs2d.back()=xs;
	m_ys2d.back()=ys;
	m_us.back()=us;
	m_vs.back()=vs;
	if(m_xaxis.m_logarithmic)m_xs2dl.back()=sci::log10(xs);
	if(m_yaxis.m_logarithmic)m_ys2dl.back()=sci::log10(ys);
	m_transformers.back()=transformer;

	//set properties as needed
	m_linecolour.back()=linecolour;
	m_linethickness.back()=linewidth;
	m_arrowstyle.back()=arrowstyle;
	
	calculateautolimits();
	m_haschanged=true;
}

//grid plot
void splot2d::addShadedGrid(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels, bool filloffscaletop, bool filloffscalebottom, splotTransformer *transformer)
{
	//checks
	sci::assertThrow(xs.size()==zs.size()+1,sci::err());
	for(size_t i=0; i<zs.size(); ++i) 
	{
		sci::assertThrow(ys.size()==zs[i].size()+1,sci::err());
	}

	//increase the size of all data
	incrementdatasize();

	m_xs.back()=xs;
	m_ys.back()=ys;
	m_structzs.back()=zs;
	m_transformers.back()=transformer;
	m_isGrid.back()=true;

	//check the x values for nans
	double *px0=&m_xs.back()[0];
	for (size_t i=0; i<m_xs.back().size(); ++i) 
	{
		if(*(px0+i) != *(px0+i))
		{
			m_xs.back().erase(m_xs.back().begin()+i);
			m_structzs.back().erase(m_structzs.back().begin()+i);
			--i;
		}
	}

	//check the y values for nans
	std::vector<double> *pz0=&m_structzs.back()[0];
	double *py0=&m_ys.back()[0];
	for (size_t i=0; i<m_ys.back().size(); ++i) 
	{
		if(*(py0+i) != *(py0+i))
		{
			m_ys.back().erase(m_ys.back().begin()+i);
			for(size_t j=0; j<m_structzs.size(); ++j)(pz0+j)->erase((pz0+j)->begin()+i);
			--i;
		}
	}
	sci::assertThrow( m_xs.back().size()>0, sci::err() );
	sci::assertThrow( m_ys.back().size()>0, sci::err() );

	//generate logged values if needed and replace infinities with max/lowest doubles
	if(colourscale.m_logarithmic) 
	{
		m_structzsl.back()=sci::log10(m_structzs.back());
		sci::replaceNegativeInfs(m_structzsl, std::numeric_limits<double>::lowest());
		sci::replacePositiveInfs(m_structzsl, std::numeric_limits<double>::max());
	}
	else
	{
		sci::replaceNegativeInfs(m_structzs, std::numeric_limits<double>::lowest());
		sci::replacePositiveInfs(m_structzs, std::numeric_limits<double>::max());
	}
	if(m_xaxis.m_logarithmic)m_xsl.back()=sci::log10(xs);
	if(m_yaxis.m_logarithmic)m_ysl.back()=sci::log10(ys);

	//replace any nans in the z data with max double
	//sci::replacenans(m_structzs.back(),std::numeric_limits<double>::max());
	//sci::replacenans(m_structzsl.back(),std::numeric_limits<double>::max());

	setupcolourscale(ncolourlevels,colourscale,filloffscaletop,filloffscalebottom,0,std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN(),0);

	calculateautolimits();
	m_haschanged=true;
}
//grid plot with 2d x and y
void addShadedGrid(const std::vector< std::vector <double> > &xs, const std::vector< std::vector <double> > &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels=256, bool filloffscaletop=true, bool filloffscalebottom=true, splotTransformer *transformer=NULL)
{
}
	


void splot2d::addmap(std::string map, wxColour linecolour, double linewidth, std::string linestyle, splotTransformer *transformer)
{
	//increase the size of all data
	incrementdatasize();

	m_linecolour.back()=linecolour;
	m_linethickness.back()=linewidth;
	m_linestyle.back()=wxString(linestyle.c_str(),wxConvUTF8);
	m_map.back()=map;
	m_transformers.back()=transformer;

	calculateautolimits();
	m_haschanged=true;
}

void splot2d::addImage(std::string image, double xBottomLeft, double yBottomLeft, double width, double height, int cropX0, int cropY0, int cropWidth, int cropHeight, double brightnessCorrection, double contrastCorrection)
{
	incrementdatasize();
	wxInitAllImageHandlers();
	m_images.back().LoadFile(wxString(image.c_str(),wxConvUTF8));
	m_imageXBottomLefts.back()=xBottomLeft;
	m_imageYBottomLefts.back()=yBottomLeft;
	m_imageWidths.back()=width;
	m_imageHeights.back()=height;
	m_imageRotations.back()=0.0;
	if(cropX0!=0 ||cropY0!=0 || (cropWidth>0 && cropWidth!=m_images.back().GetWidth()) || (cropHeight>0 && cropHeight!=m_images.back().GetHeight()))
		m_images.back()=m_images.back().Resize(wxSize(cropWidth,cropHeight),wxPoint(cropX0,cropY0));
	m_imageBrightnesses.back()=brightnessCorrection;
	if(contrastCorrection==0.0)
		m_imageContrasts.back()=0.0;
	else if(contrastCorrection<=-1.0)
		m_imageContrasts.back()=-std::numeric_limits<double>::infinity();
	else if(contrastCorrection>=1.0)
		m_imageContrasts.back()=std::numeric_limits<double>::infinity();
	else
		m_imageContrasts.back()=std::tan((contrastCorrection+1.0)*M_PI/4.0);

}

void splot2d::addText(std::string text, double x1, double y1, double x2, double y2, double alignment, double size, const std::string &font, uint32_t style, wxColour colour)
{
	incrementdatasize();
	m_text.back()=text;
	m_xs.back()=std::vector<double>(2,x1*(1.0-alignment)+x2*alignment);
	m_ys.back()=std::vector<double>(2,y1*(1.0-alignment)+y2*alignment);
	m_xs.back()[1]=x2;
	m_ys.back()[1]=y2;
	if(m_xs.back()[0]==m_xs.back()[1] && m_ys.back()[0]==m_ys.back()[1])
		m_xs.back()[1]=m_xs.back()[0]+1.0;
	m_textSize.back()=size;
	m_textFont.back()=font;
	m_textFci.back()=style;
	m_pointcolour.back()=colour;
}


void splot::adddatasetproperties(wxColour pointcolour, double pointsize, std::string pointsymbol, uint32_t pointstyle, wxString pointfont, wxColour linecolour, int linewidth, wxString linestyle, const splotcolourscale &colourscale, const splotsizescale &sizescale, unsigned int ncolourboundaries, bool filloffscaletop, bool filloffscalebottom, int ncontourlevels, double mincontour, double maxcontour, double contourlabelssize, std::string map, const std::vector<double> &arrowstyle)
{
	m_pointcolour.push_back(pointcolour);
	m_pointsize.push_back(pointsize);
	m_pointchar.push_back(wxString(pointsymbol.c_str(),wxConvUTF8));
	m_pointfont.push_back(pointfont);
	m_pointfci.push_back(pointstyle);
	m_linecolour.push_back(linecolour);
	m_linethickness.push_back(linewidth);
	m_linestyle.push_back(linestyle);
	m_colourscale.push_back(colourscale);
	m_sizescale.push_back(sizescale);
	m_arrowstyle.push_back(arrowstyle);

	m_filloffscaletop.push_back(filloffscaletop);
	m_filloffscalebottom.push_back(filloffscalebottom);
	m_colourlevels.resize(m_colourlevels.size()+1);
	if(ncolourboundaries>1 && !m_colourscale.back().m_autovalue) 
	{
		m_colourlevels.back()=sci::indexvector<double>(std::max((int)ncolourboundaries,2));
		m_colourlevels.back()*=(m_colourscale.back().m_top-m_colourscale.back().m_bottom)/m_colourlevels.back().back();
		m_colourlevels.back()+=m_colourscale.back().m_bottom;

	}
	else if(ncolourboundaries>1) 
	{
		//in this case we fill the boundaries with nans to be filled in by calculateautolimits()
		m_colourlevels.back().resize(std::max((int)ncolourboundaries,2),std::numeric_limits<double>::quiet_NaN());
	}
	if(filloffscalebottom || (m_colourlevels.back().size()>0 && m_colourscale.back().m_autovalue))
	{
		m_minstructz.push_back(sci::mingtlimit(m_structzs.back(),-std::numeric_limits<double>::max()));
		m_mincolunstructz.push_back(sci::mingtlimit(m_colunstructzs.back(),-std::numeric_limits<double>::max()));
		if (m_colourscale.back().m_logarithmic)
		{
			if(m_minstructz.back()>0) m_minstructzl.push_back(std::log10(m_minstructz.back()));
			else m_minstructzl.push_back(std::log10(sci::mingtlimit(m_structzsl.back(),0.0)));
			if(m_mincolunstructz.back()>0) m_mincolunstructzl.push_back(std::log10(m_mincolunstructz.back()));
			else m_mincolunstructzl.push_back(std::log10(sci::mingtlimit(m_colunstructzsl.back(),0.0)));
		}
		else 
		{
			m_minstructzl.push_back(std::numeric_limits<double>::quiet_NaN());
			m_mincolunstructzl.push_back(std::numeric_limits<double>::quiet_NaN());
		}
	}
	else 
	{
		m_minstructz.push_back(std::numeric_limits<double>::quiet_NaN());
		m_minstructzl.push_back(std::numeric_limits<double>::quiet_NaN());
		m_mincolunstructz.push_back(std::numeric_limits<double>::quiet_NaN());
		m_mincolunstructzl.push_back(std::numeric_limits<double>::quiet_NaN());
	}
	if(filloffscaletop || (m_colourlevels.back().size()>0 && m_colourscale.back().m_autovalue))
	{
		m_maxstructz.push_back(sci::maxltlimit<double>(m_structzs.back(),std::numeric_limits<double>::max()));
		m_maxcolunstructz.push_back(sci::maxltlimit<double>(m_colunstructzs.back(),std::numeric_limits<double>::max()));
		if (m_colourscale.back().m_logarithmic)
		{
			m_maxstructzl.push_back(std::log10(m_maxstructz.back()));
			m_maxcolunstructzl.push_back(std::log10(m_maxcolunstructz.back()));
		}
		else
		{
			m_maxstructzl.push_back(std::numeric_limits<double>::quiet_NaN());
			m_maxcolunstructzl.push_back(std::numeric_limits<double>::quiet_NaN());
		}
	}
	else 
	{
		m_maxstructz.push_back(std::numeric_limits<double>::quiet_NaN());
		m_maxstructzl.push_back(std::numeric_limits<double>::quiet_NaN());
		m_maxcolunstructz.push_back(std::numeric_limits<double>::quiet_NaN());
		m_maxcolunstructzl.push_back(std::numeric_limits<double>::quiet_NaN());
	}

	
	m_contourlevels.resize(m_contourlevels.size()+1);
	if(ncontourlevels>0) 
	{
		//set contours to min/max z if they are currently nans
		//note we might have already calculated this so check and recalculate if needed
		if(maxcontour!=maxcontour)maxcontour=(m_maxstructz.back()==m_maxstructz.back()?m_maxstructz.back():sci::max<double>(m_structzs));
		if(mincontour!=mincontour)mincontour=(m_minstructz.back()==m_minstructz.back()?m_minstructz.back():sci::min<double>(m_structzs));
		m_contourlevels.back()=sci::indexvector<double>(ncontourlevels+2); //we use plus 2 because we won't plot a contour at the max/min
		m_contourlevels.back()*=(maxcontour-mincontour)/(double)(m_contourlevels.back().size()-1);
		m_contourlevels.back()+=mincontour;
		//now remove those extra two contours at min/max
		m_contourlevels.back().erase(m_contourlevels.back().begin());
		m_contourlevels.back().erase(m_contourlevels.back().end()-1);
		m_linkcontoursandcolours.push_back(false);
	}
	else if(ncontourlevels<0)
	{
		m_linkcontoursandcolours.push_back(true);
	}
	else m_linkcontoursandcolours.push_back(false);
	m_contourlabelssize.push_back(contourlabelssize);
	m_map.push_back(map);
}

void splot::setupcolourscale(int ncolours, const splotcolourscale &colourscale, bool filloffscaletop, bool filloffscalebottom, int ncontourlevels, double maxcontour, double mincontour, int contourlabelssize)
{
	int ncolourboundaries=ncolours+1;
	m_colourscale.back()=colourscale;
	m_filloffscaletop.back()=filloffscaletop;
	m_filloffscalebottom.back()=filloffscalebottom;
	if(ncolourboundaries>1 && !m_colourscale.back().m_autovalue) 
	{
		m_colourlevels.back()=sci::indexvector<double>(std::max((int)ncolourboundaries,2));
		m_colourlevels.back()*=(m_colourscale.back().m_top-m_colourscale.back().m_bottom)/m_colourlevels.back().back();
		m_colourlevels.back()+=m_colourscale.back().m_bottom;

	}
	else if(ncolourboundaries>1) 
	{
		//in this case we fill the boundaries with nans to be filled in by calculateautolimits()
		m_colourlevels.back().resize(std::max((int)ncolourboundaries,2),std::numeric_limits<double>::quiet_NaN());
	}
	if(filloffscalebottom || (m_colourlevels.back().size()>0 && m_colourscale.back().m_autovalue))
	{
		m_minstructz.back()=sci::mingtlimit(m_structzs.back(),-std::numeric_limits<double>::max());
		m_mincolunstructz.back()=sci::mingtlimit(m_colunstructzs.back(),-std::numeric_limits<double>::max());
		if (m_colourscale.back().m_logarithmic)
		{
			if(m_minstructz.back()>0) m_minstructzl.back()=std::log10(m_minstructz.back());
			else m_minstructzl.back()=std::log10(sci::mingtlimit(m_structzsl.back(),0.0));
			if(m_mincolunstructz.back()>0) m_mincolunstructzl.back()=std::log10(m_mincolunstructz.back());
			else m_mincolunstructzl.back()=std::log10(sci::mingtlimit(m_colunstructzsl.back(),0.0));
		}
		else 
		{
			m_minstructzl.back()=std::numeric_limits<double>::quiet_NaN();
			m_mincolunstructzl.back()=std::numeric_limits<double>::quiet_NaN();
		}
	}
	else 
	{
		m_minstructz.back()=std::numeric_limits<double>::quiet_NaN();
		m_minstructzl.back()=std::numeric_limits<double>::quiet_NaN();
		m_mincolunstructz.back()=std::numeric_limits<double>::quiet_NaN();
		m_mincolunstructzl.back()=std::numeric_limits<double>::quiet_NaN();
	}
	if(filloffscaletop || (m_colourlevels.back().size()>0 && m_colourscale.back().m_autovalue))
	{
		m_maxstructz.back()=sci::maxltlimit<double>(m_structzs.back(),std::numeric_limits<double>::max());
		m_maxcolunstructz.back()=sci::maxltlimit<double>(m_colunstructzs.back(),std::numeric_limits<double>::max());
		if (m_colourscale.back().m_logarithmic)
		{
			m_maxstructzl.back()=std::log10(m_maxstructz.back());
			m_maxcolunstructzl.back()=std::log10(m_maxcolunstructz.back());
		}
		else
		{
			m_maxstructzl.back()=std::numeric_limits<double>::quiet_NaN();
			m_maxcolunstructzl.back()=std::numeric_limits<double>::quiet_NaN();
		}
	}
	else 
	{
		m_maxstructz.back()=std::numeric_limits<double>::quiet_NaN();
		m_maxstructzl.back()=std::numeric_limits<double>::quiet_NaN();
		m_maxcolunstructz.back()=std::numeric_limits<double>::quiet_NaN();
		m_maxcolunstructzl.back()=std::numeric_limits<double>::quiet_NaN();
	}

	if(ncontourlevels>0) 
	{
		//set contours to min/max z if they are currently nans
		//note we might have already calculated this so check and recalculate if needed
		if(maxcontour!=maxcontour)maxcontour=(m_maxstructz.back()==m_maxstructz.back()?m_maxstructz.back():sci::max<double>(m_structzs));
		if(mincontour!=mincontour)mincontour=(m_minstructz.back()==m_minstructz.back()?m_minstructz.back():sci::min<double>(m_structzs));
		m_contourlevels.back()=sci::indexvector<double>(ncontourlevels+2); //we use plus 2 because we won't plot a contour at the max/min
		m_contourlevels.back()*=(maxcontour-mincontour)/(double)(m_contourlevels.back().size()-1);
		m_contourlevels.back()+=mincontour;
		//now remove those extra two contours at min/max
		m_contourlevels.back().erase(m_contourlevels.back().begin());
		m_contourlevels.back().erase(m_contourlevels.back().end()-1);
		m_linkcontoursandcolours.back()=false;
	}
	else if(ncontourlevels<0)
	{
		m_linkcontoursandcolours.back()=true;
	}
	else m_linkcontoursandcolours.back()=false;
	m_contourlabelssize.back()=contourlabelssize;
}

void splot::incrementdatasize()
{
	incrementsize(m_xs);
	incrementsize(m_ys);
	incrementsize(m_xsl);
	incrementsize(m_ysl);
	incrementsize(m_xs2d);
	incrementsize(m_ys2d);
	incrementsize(m_xs2dl);
	incrementsize(m_ys2dl);
	incrementsize(m_structzs);
	incrementsize(m_structzsl);
	incrementsize(m_colunstructzs);
	incrementsize(m_colunstructzsl);
	incrementsize(m_sizeunstructzs);
	incrementsize(m_lowerxlimits);
	incrementsize(m_upperxlimits);
	incrementsize(m_lowerxlimitsl);
	incrementsize(m_upperxlimitsl);
	incrementsize(m_beginAtZeros);
	incrementsize(m_isGrid);
	incrementsize(m_minstructz);
	incrementsize(m_maxstructz);
	incrementsize(m_minstructzl);
	incrementsize(m_maxstructzl);
	incrementsize(m_mincolunstructz);
	incrementsize(m_maxcolunstructz);
	incrementsize(m_mincolunstructzl);
	incrementsize(m_maxcolunstructzl);
	incrementsize(m_us);
	incrementsize(m_vs);

	incrementsize(m_pointchar);
	incrementsize(m_pointfci);
	incrementsize(m_pointfont); 
	incrementsize(m_linethickness);
	incrementsize(m_pointsize);
	incrementsize(m_pointcolour);
	incrementsize(m_linecolour);
	incrementsize(m_linestyle);
	incrementsize(m_colourscale);
	incrementsize(m_sizescale);
	incrementsize(m_colourlevels);
	incrementsize(m_filloffscaletop);
	incrementsize(m_filloffscalebottom);
	incrementsize(m_contourlevels);
	incrementsize(m_contourlabelssize);
	incrementsize(m_map);
	incrementsize(m_transformers);
	incrementsize(m_linkcontoursandcolours);
	incrementsize(m_arrowstyle);
}

void splot::parselinestyle(wxString style, std::vector<PLINT> &marks, std::vector<PLINT> &spaces)
{
	//set outputs to zero size
	marks.resize(0);
	spaces.resize(0);

	//remove any bad characters from the beginning of style
	while(style.Len()>0 && !(style[0]==wxT(' ') || style[0]==wxT('\t') || style[0]==wxT('.') || style[0]==wxT('_'))) style=style.substr(1);

	//return empty vectors if style is empty
	if(style.Len()==0) return;

	//set up whether we are on a mark or space
	bool onmark=true;
	if(style[0]==wxT(' ') || style[0]==wxT('\t')) onmark=false;

	//initialise our current lengths to zero
	int marklength=0;
	int spacelength=0;

	//work through each character of style
	for(size_t i=0; i<style.Len(); ++i)
	{
		//if we have changed between a space and mark record the length
		if(onmark==true && (style[i]==wxT(' ') || style[i]==wxT('\t')))
		{
			marks.push_back(marklength);
			marklength=0;
			onmark=false;
		}
		else if(onmark==false && (style[i]==wxT('_') || style[i]==wxT('.')))
		{
			spaces.push_back(spacelength);
			spacelength=0;
			onmark=true;
		}
		//add the current character to the current length
		if(style[i]==wxT(' ')) spacelength+=200;
		else if(style[i]==wxT('\t')) spacelength+=800;
		else if(style[i]==wxT('.')) marklength+=200;
		else if(style[i]==wxT('_')) marklength+=800;
	}
	//add the last mark or space
	if(onmark==true) marks.push_back(marklength);
	else spaces.push_back(spacelength);

	//equalise the lengths of the vectors if needed
	if(marks.size()<spaces.size()) marks.push_back(0);
	else if (spaces.size()<marks.size()) spaces.push_back(0);
}

void splot2d::incrementdatasize()
{
	//the data
	incrementsize(m_xpluserrs);
	incrementsize(m_xminuserrs);
	incrementsize(m_ypluserrs);
	incrementsize(m_yminuserrs);
	incrementsize(m_xpluserrsl);
	incrementsize(m_xminuserrsl);
	incrementsize(m_ypluserrsl);
	incrementsize(m_yminuserrsl);
	incrementsize(m_images);
	incrementsize(m_imageXBottomLefts);
	incrementsize(m_imageYBottomLefts);
	incrementsize(m_imageWidths);
	incrementsize(m_imageHeights);
	incrementsize(m_imageRotations);
	incrementsize(m_imageBrightnesses);
	incrementsize(m_imageContrasts);
	incrementsize(m_text);
	incrementsize(m_textFont);
	incrementsize(m_textFci);
	incrementsize(m_textSize);

	//the data properties
	m_xerrcolour.push_back(wxColour(0,0,0));
	m_yerrcolour.push_back(wxColour(0,0,0));
	m_xerrthickness.push_back(1);
	m_yerrthickness.push_back(1);

	//now do the splot variables
	splot::incrementdatasize();
}

void splot2d::calculateautolimits()
{
	if(m_xautolimits)
	{
		std::vector<double> intersectpoints;
		if(!m_yautointersect) intersectpoints.push_back(m_yaxis.m_intersect);
		bool addpadding=true;
		for(size_t i=0; i<m_structzs.size(); ++i) addpadding=addpadding&&(m_structzs[i].size()==0);
		splot::calculateautolimits(m_xaxis,m_xs,m_xpluserrs,m_xminuserrs,m_xs2d,addpadding,intersectpoints);
	}
	if(m_yautolimits)
	{
		std::vector<double> intersectpoints;
		if(!m_xautointersect) intersectpoints.push_back(m_xaxis.m_intersect);
		bool addpadding=true;
		for(size_t i=0; i<m_structzs.size(); ++i) addpadding=addpadding&&(m_structzs[i].size()==0);
		splot::calculateautolimits(m_yaxis,m_ys,m_ypluserrs,m_yminuserrs,m_ys2d,addpadding,intersectpoints);
	}
	if(m_xautointersect) m_xaxis.m_intersect=m_yaxis.m_min;
	if(m_yautointersect) m_yaxis.m_intersect=m_xaxis.m_min;

	for(size_t i=0; i<m_colunstructzs.size(); ++i)
	{
		if(m_colunstructzs[i].size()>0)
		{
			if(m_colourlevels[i].size()>0 && m_colourscale[i].m_autovalue)
			{
				if(m_colourscale[i].m_logarithmic)
				{
					if(m_minstructzl[i]==m_minstructzl[i])
					{
						m_colourscale[i].m_bottom=m_minstructzl[i];
						m_colourscale[i].m_top=m_maxstructzl[i];
					}
					else
					{
						m_colourscale[i].m_bottom=m_mincolunstructzl[i];
						m_colourscale[i].m_top=m_maxcolunstructzl[i];
					}
					m_colourlevels[i]=sci::indexvector<double>(m_colourlevels[i].size())/((double)m_colourlevels[i].size()-1.0)*(m_colourscale[i].m_top-m_colourscale[i].m_bottom)+m_colourscale[i].m_bottom;
				}
				else
				{
					if(m_minstructz[i]==m_minstructz[i])
					{
						m_colourscale[i].m_bottom=m_minstructz[i];
						m_colourscale[i].m_top=m_maxstructz[i];
					}
					else
					{
						m_colourscale[i].m_bottom=m_mincolunstructz[i];
						m_colourscale[i].m_top=m_maxcolunstructz[i];
					}
					m_colourlevels[i]=sci::indexvector<double>(m_colourlevels[i].size())/((double)m_colourlevels[i].size()-1.0)*(m_colourscale[i].m_top-m_colourscale[i].m_bottom)+m_colourscale[i].m_bottom;
				}
			}
			//to do: same for contours
		}
	}
	for(size_t i=0; i<m_structzs.size(); ++i)
	{
		if(m_structzs[i].size()>0)
		{
			if(m_colourlevels[i].size()>0 && m_colourscale[i].m_autovalue)
			{
				if(m_colourscale[i].m_logarithmic)
				{
					m_colourscale[i].m_bottom=m_minstructzl[i];
					m_colourscale[i].m_top=m_maxstructzl[i];
					m_colourlevels[i]=sci::indexvector<double>(m_colourlevels[i].size())/((double)m_colourlevels[i].size()-1.0)*(m_colourscale[i].m_top-m_colourscale[i].m_bottom)+m_colourscale[i].m_bottom;
				}
				else
				{
					m_colourscale[i].m_bottom=m_minstructz[i];
					m_colourscale[i].m_top=m_maxstructz[i];
					m_colourlevels[i]=sci::indexvector<double>(m_colourlevels[i].size())/((double)m_colourlevels[i].size()-1.0)*(m_colourscale[i].m_top-m_colourscale[i].m_bottom)+m_colourscale[i].m_bottom;
				}
			}
			//to do: same for contours
		}
	}
}

void splot::calculateautolimits(splotaxis &axis, const std::vector<std::vector<double>> &data1d, const std::vector<std::vector<double>> &data1dpluserrs, const std::vector<std::vector<double>> &data1dminuserrs, const std::vector<std::vector<std::vector<double>>> &data2d, bool addpadding, std::vector<double> intersectpoints)
{
	//check there is some data to search for
	if(data1d.size()==0 &&data2d.size()==0)
	{
		axis.m_min=std::numeric_limits<double>::quiet_NaN();
		axis.m_max=std::numeric_limits<double>::quiet_NaN();
	}

	double lowerlimit=-std::numeric_limits<double>::infinity();
	if(axis.m_logarithmic)
		lowerlimit=0.0;

	//we find the smallest positive value for the min and the max value for the max
	axis.m_min=sci::mingtlimit(data1d,lowerlimit);
	double min1dminuserrs=sci::mingtlimit(data1dminuserrs,lowerlimit);
	if((axis.m_min!=axis.m_min) || min1dminuserrs < axis.m_min) axis.m_min=min1dminuserrs;
	double min2d=sci::mingtlimit(data2d,lowerlimit);
	if((axis.m_min!=axis.m_min) || min2d < axis.m_min) axis.m_min=min2d;

	axis.m_max=sci::maxltlimit(data1d,std::numeric_limits<double>::infinity());
	double max1dpluserrs=sci::maxltlimit(data1dpluserrs,std::numeric_limits<double>::infinity());
	if((axis.m_max!=axis.m_max) || max1dpluserrs > axis.m_max) axis.m_max=max1dpluserrs;
	double max2d=sci::maxltlimit(data2d,std::numeric_limits<double>::infinity());
	if((axis.m_max!=axis.m_max) || min2d < axis.m_max) axis.m_max=max2d;

	//add padding if needed
	if(addpadding)
	{
		if(axis.m_logarithmic)
		{
			axis.m_min/=2.0;
			axis.m_max*=2.0;
		}
		else
		{
			double extra=0.1*(axis.m_max-axis.m_min);
			axis.m_max+=extra;
			axis.m_min-=extra;
		}
	}
	
	for(size_t i=0; i<intersectpoints.size(); ++i)
	{
		if(intersectpoints[i]>lowerlimit && intersectpoints[i]==intersectpoints[i])
			axis.m_min=std::min(axis.m_min,intersectpoints[i]);
		if(intersectpoints[i]<std::numeric_limits<double>::infinity() && intersectpoints[i]==intersectpoints[i])
			axis.m_max=std::max(axis.m_max,intersectpoints[i]);
	}
}

wxString splot::createploptstring(const splotaxis &axis)
{
	wxString opt=wxT("bci");
	if(axis.m_timeformat!=wxEmptyString)opt=opt+wxT("d");
	if(axis.m_customlabelcreator!=NULL)opt=opt+wxT("o");
	if(axis.m_showlabels)
	{
		if(axis.m_labelpositionpositive)opt=opt+wxT("m");
		else opt=opt+wxT("n");
	}
	if(axis.m_logarithmic)opt=opt+wxT("l");
	if(axis.m_majorticklength>0.0)opt=opt+wxT("t");
	else opt=opt+wxT("x");
	if(axis.m_minorticklength>0.0)opt=opt+wxT("s");
	if(axis.m_rotatelabels)opt=opt+wxT("v");

	return opt;
}

void splot2d::setlogxaxis(bool log)
{
	if(log==true)
	{
		m_xaxis.m_logarithmic=true;
		for(size_t i=0; i<m_xs.size(); ++i)
		{
			m_xsl[i]=sci::log10(m_xs[i]);
			m_xpluserrsl[i]=sci::log10(m_xpluserrs[i]);
			m_xminuserrsl[i]=sci::log10(m_xminuserrs[i]);
		}
	}
	else
	{
		m_xaxis.m_logarithmic=false;
		for(size_t i=0; i<m_xs.size(); ++i)
		{
			m_xsl[i].resize(0);
			m_xpluserrsl[i].resize(0);
			m_xminuserrsl[i].resize(0);
		}
	}
	calculateautolimits();
}

void splot2d::setlogyaxis(bool log)
{
	if(log==true)
	{
		m_yaxis.m_logarithmic=true;
		for(size_t i=0; i<m_ys.size(); ++i)
		{
			m_ysl[i]=sci::log10(m_ys[i]);
			m_ypluserrsl[i]=sci::log10(m_ypluserrs[i]);
			m_yminuserrsl[i]=sci::log10(m_yminuserrs[i]);
		}
	}
	else
	{
		m_yaxis.m_logarithmic=false;
		for(size_t i=0; i<m_ys.size(); ++i)
		{
			m_ysl[i].resize(0);
			m_ypluserrsl[i].resize(0);
			m_yminuserrsl[i].resize(0);
		}
	}
	calculateautolimits();
}

class PlTempTransformer
{
public:
	PlTempTransformer( plstream *pl, void (*transform)(PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer) , PLPointer input)
		:m_pl(pl)
	{
		m_pl->stransform(transform, input);
	}
	~PlTempTransformer()
	{
		m_pl->stransform(NULL, NULL);
	}
private:
	plstream *m_pl;
};

//when called with a plstream pl.init() should have already been called
//and the window and driver should have been set so the stream should 
//be ready for sending plot commands
//all plplot specific code in splot2d should be kept here
void splot2d::plot(plstream *pl, wxDC *dc, int width, int height, bool antialiasing, double linewidthmultiplier)
{

	unsigned int r;
	unsigned int g;
	unsigned int b;
	double a;
	pl->col0(0);

	//wxMessageBox(wxT("starting plot"));
	
	//set the limits of the plot area in terms of plot units
	//note we have to log the limits for a log axis
	double xmin=m_xaxis.m_min;
	double xmax=m_xaxis.m_max;
	double ymin=m_yaxis.m_min;
	double ymax=m_yaxis.m_max;
	if(m_xaxis.m_logarithmic)
	{
		xmin=std::log10(xmin);
		xmax=std::log10(xmax);
	}
	if(m_yaxis.m_logarithmic)
	{
		ymin=std::log10(ymin);
		ymax=std::log10(ymax);
	}
	pl->wind(xmin,xmax,ymin,ymax);

	
	//check if any of the x limits are nans. If so then there is no point in doing anything else
	bool nanlimit= xmin!=xmin || xmax!=xmax ||ymin!=ymin || ymax!=ymax;
	bool inflimit= xmax==std::numeric_limits<double>::infinity() || xmin==std::numeric_limits<double>::infinity() || ymax==std::numeric_limits<double>::infinity() || ymin==std::numeric_limits<double>::infinity();
	bool neginflimit= -xmax==std::numeric_limits<double>::infinity() || -xmin==std::numeric_limits<double>::infinity() || -ymax==std::numeric_limits<double>::infinity() || -ymin==std::numeric_limits<double>::infinity();
	if( nanlimit || inflimit || neginflimit)
	{
		//just draw an empty set of axes
		r=m_xaxis.m_colour.Red();
		g=m_xaxis.m_colour.Green();
		b=m_xaxis.m_colour.Blue();
		pl->scol0(1,r,g,b);
		pl->col0(1);
		pl->width(m_xaxis.m_linethickness*linewidthmultiplier);
		pl->box("bc",0.0,0,"",0.0,0);
		r=m_yaxis.m_colour.Red();
		g=m_yaxis.m_colour.Green();
		b=m_yaxis.m_colour.Blue();
		pl->scol0(1,r,g,b);
		pl->col0(1);
		pl->width(m_yaxis.m_linethickness*linewidthmultiplier);
		pl->box("",0.0,0,"bc",0.0,0);

		// to do draw axis titles too, but given the probs with y axis title don't do it yet
		return;
	}

	
	//wxMessageBox(wxT("drawing data"));
	//draw the points, lines, shadings and contours
	for(size_t i=0; i<m_xs.size(); ++i)
	{
		//set to use either logged or not logged x/y data
		//and create a raw double pointer to the 1D data if
		//needed or if we have 2d data a pointer to an appropriate
		//index transformer
		double *x=NULL;
		double *y=NULL;

		std::vector<double> *x1d=&m_xs[i];
		std::vector<double> *y1d=&m_ys[i];
		std::vector<std::vector<double>> *x2d=&m_xs2d[i];
		std::vector<std::vector<double>> *y2d=&m_ys2d[i];


		if(m_xs[i].size()>0) //this is not the case when drawing a map
		{
			
			x=&m_xs[i][0];
			y=&m_ys[i][0];
			if(m_xaxis.m_logarithmic) 
			{
				x=&m_xsl[i][0];
				x1d=&m_xsl[i];
			}
			if(m_yaxis.m_logarithmic) 
			{
				y=&m_ysl[i][0];
				y1d=&m_ysl[i];
			}
		}
		else if(m_xs2d[i].size()>0)
		{
			if(m_xaxis.m_logarithmic) x2d=&m_xs2dl[i];
			if(m_yaxis.m_logarithmic) y2d=&m_ys2dl[i];
		}
		
		splotindextransformer1d transformer1d(x1d,y1d,m_transfunc1dxy);
		splotindextransformer2d transformer2d(x2d,y2d,m_transfunc2dxy);
		splotindextransformer *transformer=NULL;
		if(x1d->size()>0)
			transformer=&transformer1d;
		else if(x2d->size()>0)
			transformer=&transformer2d;
			

		//add the user provided transform
		PlTempTransformer tempTransformer( pl, (m_transformers[i] ? splotTransform : NULL) , (void*)(m_transformers[i].get()) );

		//vectors/arrows
		if(m_us[i].size()>0 && m_vs[i].size()>0)
		{
			pl->scol0(1,m_linecolour[i].Red(),m_linecolour[i].Green(),m_linecolour[i].Blue());
			pl->col0(1);
			pl->width(m_linethickness[i]);
			//plvect takes two double**s as inputs, we can also use the same transform
			//as for contours/shades for x/y values
			size_t dim1len=m_us[i].size();
			double **uptr=new double* [dim1len];
			double **vptr=new double* [dim1len];
			for(size_t j=0; j<dim1len; ++j)
			{
				uptr[j]=&(m_us[i][j][0]);
				vptr[j]=&(m_vs[i][j][0]);
			}
			pl->vect(uptr,vptr,dim1len,m_us[i][0].size(),0,Coord_Xform_evaluator,transformer);
			delete vptr;
			delete uptr;
		}
		//images
		else if(m_images[i].IsOk())
		{
			//copy the image
			wxImage tempImage=m_images[i];
			//deal with negative widths and backwards axes
			//after this minx/miny will be smaller than maxx/maxy
			//and width and imageWidth/imageHeight will be positive
			//ac imageX0/imageY0 will be the bottom left corner
			double maxx=m_xaxis.m_max;
			double maxy=m_yaxis.m_max;
			double minx=m_xaxis.m_min;
			double miny=m_yaxis.m_min;
			double imageX0=m_imageXBottomLefts[i];
			double imageY0=m_imageYBottomLefts[i];
			double imageWidth=m_imageWidths[i];
			double imageHeight=m_imageHeights[i];
			if((maxx-minx)*imageWidth<0.0)
			{
				tempImage=tempImage.Mirror(true);
				imageX0+=imageWidth;
			}
			if((maxy-miny)*imageHeight<0.0)
			{
				tempImage=tempImage.Mirror(false);
				imageY0+=imageHeight;
			}
			if(maxx<minx)
			{
				minx*=-1.0;
				maxx*=-1.0;
				imageX0*=-1.0;
			}
			if(maxy<miny)
			{
				miny*=-1.0;
				maxy*=-1.0;
				imageY0*=-1.0;
			}
			imageWidth=std::abs(imageWidth);
			imageHeight=std::abs(imageHeight);


			//calculate some scalings values
			double vpXMin;
			double vpXMax;
			double vpYMin;
			double vpYMax;
			pl->gvpd(vpXMin,vpXMax,vpYMin,vpYMax); //get them on the 0.0-1.0 scale
			double origPixelsPerX=(double)tempImage.GetWidth()/(imageWidth);
			double origPixelsPerY=(double)tempImage.GetHeight()/(imageHeight);
			double newPixelsPerX=(sci::round((double)width*vpXMax)-sci::round(width*vpXMin))/(maxx-minx);
			double newPixelsPerY=(sci::round((double)height*vpYMax)-sci::round((double)height*vpYMin))/(maxy-miny);
			double origPixelsPerNewPixelX=origPixelsPerX/newPixelsPerX;
			double origPixelsPerNewPixelY=origPixelsPerY/newPixelsPerY;

			//work out how much to chop from the sides
			double chopleft=sci::round(std::max(0.0,minx-imageX0)*origPixelsPerX);
			double chopright=sci::round(std::max(0.0,imageX0+imageWidth-maxx)*origPixelsPerX);
			double chopbottom=sci::round(std::max(0.0,miny-imageY0)*origPixelsPerY);
			double choptop=sci::round(std::max(0.0,imageY0+imageHeight-maxy)*origPixelsPerY);

			//check if we will have any image left and draw it if needed
			if(int(chopleft+chopright)<tempImage.GetWidth() && int(choptop+chopbottom)<tempImage.GetHeight())
			{
				//do the cropping
				tempImage=tempImage.GetSubImage(wxRect(chopleft,choptop,sci::round(tempImage.GetWidth()-chopleft-chopright),sci::round(tempImage.GetHeight()-chopbottom-choptop)));

				//if the picture is going to get bigger do bightness/contrast correction now
				bool upscaling=origPixelsPerNewPixelX*tempImage.GetWidth()*origPixelsPerNewPixelY*tempImage.GetHeight() < tempImage.GetWidth()*tempImage.GetHeight();
				if(upscaling)
				{
					unsigned char* data=tempImage.GetData();
					size_t length=tempImage.GetWidth()*tempImage.GetHeight()*3;
					unsigned char* dataEnd=data+length;
					if(m_imageBrightnesses[i]!=0.0)
					{
						for(unsigned char* datai=data; datai!=dataEnd; ++datai)
						{
							if(m_imageBrightnesses[i]<0.0)
								*datai=(unsigned char)sci::round(std::max(0.0,*datai*(1.0+m_imageBrightnesses[i])));
							else
								*datai=(unsigned char)sci::round(std::min(255.0,*datai+((255-*datai)*m_imageBrightnesses[i])));
						}
					}
					if(m_imageContrasts[i] != 0.0)
					{
						for(unsigned char* datai=data; datai!=dataEnd; ++datai)
						{
							*datai=(char)sci::round(std::min(255.0,std::max(0.0,(*datai-127.5)*m_imageContrasts[i]+127.5)));
						}
					}
				}
						

				//now scale it 
				wxImageResizeQuality quality = antialiasing ? wxIMAGE_QUALITY_HIGH : wxIMAGE_QUALITY_NORMAL;
				tempImage.Rescale((int)sci::round(tempImage.GetWidth()/origPixelsPerNewPixelX),(int)sci::round(tempImage.GetHeight()/origPixelsPerNewPixelY),quality);
				
				//if we are downscaling then do the brightness/contrast correction now
				if(!upscaling)
				{
					unsigned char* data=tempImage.GetData();
					size_t length=tempImage.GetWidth()*tempImage.GetHeight()*3;
					unsigned char* dataEnd=data+length;
					if(m_imageBrightnesses[i]!=0.0)
					{
						for(unsigned char* datai=data; datai!=dataEnd; ++datai)
						{
							if(m_imageBrightnesses[i]<0.0)
								*datai=(unsigned char)sci::round(std::max(0.0,*datai*(1.0+m_imageBrightnesses[i])));
							else
								*datai=(unsigned char)sci::round(std::min(255.0,*datai+((255-*datai)*m_imageBrightnesses[i])));
						}
					}
					if(m_imageContrasts[i] != 0.0)
					{
						for(unsigned char* datai=data; datai!=dataEnd; ++datai)
						{
							*datai=(char)sci::round(std::min(255.0,std::max(0.0,(*datai-127.5)*m_imageContrasts[i]+127.5)));
						}
					}
				}

				//convert to a bitmap
				wxBitmap bmp(tempImage);

				//draw it to the dc
				//note that we try to ensure that it meets at the 
				double imageXPos=vpXMin*width;
				if(chopleft==0.0)
					imageXPos+=(imageX0-minx)*newPixelsPerX;
				double imageYPos=height*(1.0-vpYMin)-tempImage.GetHeight();
				if(chopbottom==0.0)
					imageYPos-=(imageY0-miny)*newPixelsPerY;
				wxDCScaler scaler(dc, 1.0, 1.0 ); //this resets the scale until the scaler goes out of scope
				dc->DrawBitmap(bmp,sci::round(imageXPos),sci::round(imageYPos));
			}
		}
		//text
		else if(m_text[i]!="")
		{
			pl->sfontf(m_textFont[i].c_str());
			pl->sfci(m_textFci[i]);
			pl->schr(0.0,m_textSize[i]);
			a=m_pointcolour[i].Alpha()/255.0;
			r=m_pointcolour[i].Red();
			g=m_pointcolour[i].Green();
			b=m_pointcolour[i].Blue();
			pl->scol0a(1,r,g,b,a);
			pl->col0(1);
			pl->ptex(x[0],y[0],x[1]-x[0],y[1]-y[0],0.0,m_text[i].c_str());
		}
		//points, errs, lines and bar
		else if(m_structzs[i].size()==0)
		{
			//bar
			if(m_lowerxlimits[i].size()>0 &&m_upperxlimits[i].size())
			{
				//check where the axis is
				double miny=m_xaxis.m_intersect;

				std::vector<double> x(4);
				std::vector<double> y(4);
				for(size_t j=0; j<m_ys[i].size(); ++j)
				{
					x[0]=m_lowerxlimits[i][j];
					x[1]=m_lowerxlimits[i][j];
					x[2]=m_upperxlimits[i][j];
					x[3]=m_upperxlimits[i][j];
					y[0]=m_beginAtZeros[i] && !m_yaxis.m_logarithmic ? 0 : miny;
					y[1]=m_ys[i][j];
					y[2]=m_ys[i][j];
					y[3]=m_beginAtZeros[i] && !m_yaxis.m_logarithmic ? 0 : miny;

					//do fill first
					a=m_pointcolour[i].Alpha()/255.0;
					r=m_pointcolour[i].Red();
					g=m_pointcolour[i].Green();
					b=m_pointcolour[i].Blue();
					if(a>0)
					{
						pl->width(0.0);
						pl->scol0a(1,r,g,b,a);
						pl->col0(1);
						pl->fill(4,&x[0],&y[0]);
					}

					//then the outline
					a=m_linecolour[i].Alpha()/255.0;
					r=m_linecolour[i].Red();
					g=m_linecolour[i].Green();
					b=m_linecolour[i].Blue();
					pl->width(m_linethickness[i]*linewidthmultiplier);
					pl->scol0a(1,r,g,b,a);
					pl->col0(1);
					pl->line(4,&x[0],&y[0]);
				}
			}

			//lines
			if(m_linethickness[i]>0.0)
			{
				r=m_linecolour[i].Red();
				g=m_linecolour[i].Green();
				b=m_linecolour[i].Blue();
				pl->scol0(1,r,g,b);
				pl->col0(1);
				pl->width(m_linethickness[i]*linewidthmultiplier);
				//set the line style
				std::vector<PLINT>marks;
				std::vector<PLINT>spaces;
				parselinestyle(m_linestyle[i],marks,spaces);
				if(marks.size()==0)pl->styl(0,NULL, NULL);
				else pl->styl(marks.size(),&marks[0],&spaces[0]);
				//if the size of x is greater than 0, this is a line plot
				//otherwise this is a map
				if(m_xs[i].size()>0)pl->line(m_xs[i].size(),x,y);
				else 
				{
					if(m_transformers[i])
						pl->map(NULL,m_map[i].c_str(),-360.0,360.0,-90.0,90.0);
					else
						pl->map(NULL,m_map[i].c_str(),m_xaxis.m_min,m_xaxis.m_max,m_yaxis.m_min,m_yaxis.m_max);
				}
				//set line style back to normal
				pl->styl(0,NULL, NULL);
			}
			//errors
			if(m_xpluserrs[i].size()>0 && m_xminuserrs[i].size()>0)
			{
				double *xpluserr=&m_xpluserrs[i][0];
				double *xminuserr=&m_xminuserrs[i][0];
				if(m_xaxis.m_logarithmic) 
				{
					xpluserr=&m_xpluserrsl[i][0];
					xminuserr=&m_xminuserrsl[i][0];
				}
				r=m_xerrcolour[i].Red();
				g=m_xerrcolour[i].Green();
				b=m_xerrcolour[i].Blue();
				pl->scol0(0,r,g,b);
				pl->col0(0);
				pl->width(m_xerrthickness[i]*linewidthmultiplier);
				//unfortunately if we have infinities or nans in our error bars then we get crazy lines
				//on the plot. Therefore check and plot each error bar individually
				//pl->errx(m_ys[i].size(),xminuserr,xpluserr,y);
				for(size_t j=0; j<m_ys[i].size(); ++j)
				{
					bool minusokay=xminuserr[j]==xminuserr[j] && -xminuserr[j]!=std::numeric_limits<double>::infinity() && xminuserr[j]!=-std::numeric_limits<double>::infinity();
					bool plusokay=xpluserr[j]==xpluserr[j] && -xpluserr[j]!=std::numeric_limits<double>::infinity() && xpluserr[j]!=-std::numeric_limits<double>::infinity();
					bool yokay=y[j]==y[j] && -y[j]!=std::numeric_limits<double>::infinity() && y[j]!=-std::numeric_limits<double>::infinity();
					if(minusokay&&plusokay&&yokay) pl->errx(1,xminuserr+j,xpluserr+j,y+j);
				}

			}
			if(m_ypluserrs[i].size()>0 && m_yminuserrs[i].size()>0)
			{
				double *ypluserr=&m_ypluserrs[i][0];
				double *yminuserr=&m_yminuserrs[i][0];
				if(m_yaxis.m_logarithmic) 
				{
					ypluserr=&m_ypluserrsl[i][0];
					yminuserr=&m_yminuserrsl[i][0];
				}
				r=m_yerrcolour[i].Red();
				g=m_yerrcolour[i].Green();
				b=m_yerrcolour[i].Blue();
				pl->scol0(0,r,g,b);
				pl->col0(0);
				pl->width(m_yerrthickness[i]*linewidthmultiplier);
				//pl->erry(m_xs[i].size(),x,yminuserr,ypluserr);
				for(size_t j=0; j<m_xs[i].size(); ++j)
				{
					bool minusokay=yminuserr[j]==yminuserr[j] && -yminuserr[j]!=std::numeric_limits<double>::infinity() && yminuserr[j]!=-std::numeric_limits<double>::infinity();
					bool plusokay=ypluserr[j]==ypluserr[j] && -ypluserr[j]!=std::numeric_limits<double>::infinity() && ypluserr[j]!=-std::numeric_limits<double>::infinity();
					bool xokay=x[j]==x[j] && -x[j]!=std::numeric_limits<double>::infinity() && x[j]!=-std::numeric_limits<double>::infinity();
					if(minusokay&&plusokay&&xokay) pl->erry(1,x+j,yminuserr+j,ypluserr+j);
				}
			}
			//points
			if(m_pointsize[i]>0.0 && m_pointchar[i].Len()>0)
			{
				pl->sfci(m_pointfci[i]);
				pl->sfci(0);
				pl->sfontf(m_pointfont[i].mb_str());
				pl->schr(0.0,m_pointsize[i]);
				r=m_pointcolour[i].Red();
				g=m_pointcolour[i].Green();
				b=m_pointcolour[i].Blue();
				pl->scol0(1,r,g,b);
				pl->col0(1);
				if(m_sizeunstructzs[i].size()>0 && m_colunstructzs[i].size()>0)
				{
					//draw points with changing size and colour
					//set the colour scale
					for(size_t j=0; j<m_xs[i].size(); ++j)
					{
						rgbcolour rgb;
						double val;
						if(m_colourscale[i].m_logarithmic)
							val=m_colunstructzsl[i][j];
						else
							val=m_colunstructzs[i][j];
							
						if(m_colourscale[i].getNormalisedValue( val ) < 0.0 )
							rgb=m_colourscale[i].getRgbOffscaleBottom();
						else if(m_colourscale[i].getNormalisedValue( val ) > 1.0 )
							rgb=m_colourscale[i].getRgbOffscaleTop();
						else 
							rgb=m_colourscale[i].getRgbOriginalScale( val );
						pl->scol0a( 1, sci::round( rgb.r() * 255.0), sci::round( rgb.g() * 255.0), sci::round( rgb.b() * 255.0), rgb.a() );
						pl->col0(1);
						plschr(0.0,m_sizescale[i].getsize(m_sizeunstructzs[i][j]));
						pl->string(1,x+j,y+j,m_pointchar[i].mb_str());
					}
				}
				else if(m_sizeunstructzs[i].size()>0)
				{
					//draw points with changing size
					for(size_t j=0; j<m_xs[i].size(); ++j)
					{
						plschr(0.0,m_sizescale[i].getsize(m_sizeunstructzs[i][j]));
						pl->string(1,x+j,y+j,m_pointchar[i].mb_str());
					}
				}
				else if(m_colunstructzs[i].size()>0)
				{
					//draw points with changing colour
					for(size_t j=0; j<m_xs[i].size(); ++j)
					{
						rgbcolour rgb;
						double val;
						if(m_colourscale[i].m_logarithmic)
							val=m_colunstructzsl[i][j];
						else
							val=m_colunstructzs[i][j];
							
						if( val==val)
						{
							if(m_colourscale[i].getNormalisedValue( val ) < 0.0 )
								rgb=m_colourscale[i].getRgbOffscaleBottom();
							else if(m_colourscale[i].getNormalisedValue( val ) > 1.0 )
								rgb=m_colourscale[i].getRgbOffscaleTop();
							else 
								rgb=m_colourscale[i].getRgbOriginalScale( val );
							pl->scol0a( 1, sci::round( rgb.r() * 255.0), sci::round( rgb.g() * 255.0), sci::round( rgb.b() * 255.0), rgb.a() );
							pl->col0(1);
							pl->string(1,x+j,y+j,m_pointchar[i].mb_str());
						}
					}
				}
				else
				{
					//draw points with static size and colour
					//pl->schr(0.0,m_pointsize[i]);
					//pl->ssym(0.0,m_pointsize[i]);
					if(m_pointchar[i].Len()>0)
						//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
						pl->string(m_xs[i].size(),x,y,m_pointchar[i].mb_str());
				}
			}
		}
		//shading and contours
		else
		{
			//set up a plotting matrix and check if the transform is rectilinear
			splot2dmatrix matrix(&m_structzs[i]);
			if(m_colourscale[i].m_logarithmic)matrix.changepointerleavingsizethesame(&m_structzsl[i]);
			bool rectilinear;
			double minx;
			double miny;
			double maxx;
			double maxy;
			if(m_xs[i].size()>0 && m_ys[i].size()>0)
			{
				rectilinear=transformer->isrectilinear() && (!m_transformers[i] || m_transformers[i]->isrectilinear());
				minx=sci::min<double>(m_xs[i]);
				maxx=sci::max<double>(m_xs[i]);
				miny=sci::min<double>(m_ys[i]);
				maxy=sci::max<double>(m_ys[i]);
			}
			else 
			{
				rectilinear=false;
				minx=sci::min<double>(m_xs2d[i]);
				maxx=sci::max<double>(m_xs2d[i]);
				miny=sci::min<double>(m_ys2d[i]);
				maxy=sci::max<double>(m_ys2d[i]);
			}
			double minz;
			double maxz;
			if(m_colourscale[i].m_logarithmic)
			{
				minz=m_minstructzl[i];
				maxz=m_maxstructzl[i];
			}
			else
			{
				minz=m_minstructz[i];
				maxz=m_maxstructz[i];
			}
			//shading first
			if(m_colourlevels[i].size()>0)
			{
				rgbcolour rgb;
				//fill off scale bottom first
				//if( m_filloffscalebottom[i] && minz<m_colourscale[i].m_bottom )
				if( m_filloffscalebottom[i] )
				{
					rgb = m_colourscale[i].getRgbOffscaleBottom( );
					pl->scol0a( 1, sci::round( rgb.r() * 255.0), sci::round( rgb.g() * 255.0), sci::round( rgb.b() * 255.0), rgb.a() );
					pl->shade( matrix, minx, maxx, miny, maxy, -std::numeric_limits<double>::infinity(), 
						m_colourscale[i].m_bottom, 0, 1, 1, 0, 0, 0, 0, rectilinear, transformer);
				}
				//fill the different levels
				for( size_t j=1; j<m_colourlevels[i].size(); ++j )
				{
					//check if this level is in bounds
					if( m_colourscale[i].getNormalisedValue( m_colourlevels[i][j] ) < 0.0 )
						continue;
					if( m_colourscale[i].getNormalisedValue( m_colourlevels[i][j-1] ) > 1.0 )
						break;
					//get the colour
					rgb=m_colourscale[i].getRgbOriginalScale( (m_colourlevels[i][j-1] + m_colourlevels[i][j])/2.0 );
					pl->scol0a( 1, sci::round( rgb.r() * 255.0), sci::round( rgb.g() * 255.0), sci::round( rgb.b() * 255.0), rgb.a() );
					//get the edges - being careful for crossing off the ends of the scale
					double lowLimit=m_colourlevels[i][j-1];
					double highLimit=m_colourlevels[i][j];
					if(m_colourscale[i].getNormalisedValue( m_colourlevels[i][j-1] ) < 0.0  )
						lowLimit=m_colourscale[i].m_bottom;
					if(m_colourscale[i].getNormalisedValue( m_colourlevels[i][j-1] ) > 1.0  )
						highLimit=m_colourscale[i].m_top;
					//draw the band
					pl->shade( matrix, minx, maxx, miny, maxy, lowLimit, 
						highLimit, 0, 1, 1, 0, 0, 0, 0, rectilinear, transformer);
				}
				//fill off scale top
				//if( m_filloffscaletop[i] && maxz>m_colourscale[i].m_top )
				if( m_filloffscaletop[i] )
				{
					rgb = m_colourscale[i].getRgbOffscaleTop( );
					pl->scol0a( 1, sci::round( rgb.r() * 255.0), sci::round( rgb.g() * 255.0), sci::round( rgb.b() * 255.0), rgb.a() );
					pl->shade( matrix, minx, maxx, miny, maxy, m_colourscale[i].m_top, 
						std::numeric_limits<double>::infinity(), 0, 1, 1, 0, 0, 0, 0, rectilinear, transformer);
				}

				/*
				//set the colour scale
				pl->scmap1la(!m_colourscale[i].m_hls,m_colourscale[i].m_value.size(),&m_colourscale[i].m_value[0],&m_colourscale[i].m_colour1[0],&m_colourscale[i].m_colour2[0],&m_colourscale[i].m_colour3[0],&m_colourscale[i].m_alpha[0],&m_colourscale[i].m_rbgr[0]);

				//plot the lower out of bounds if needed, note that if there is no out of bounds plplot just does nothing because minlevel>maxlevel
				if(m_filloffscalebottom[i] && m_minstructz[i]<m_colourscale[i].m_bottom)pl->shade(matrix,minx,maxx,miny,maxy,m_minstructz[i],m_colourscale[i].m_bottom,1,0.0,1,0,0,0,0,rectilinear,transformer);
				//plot the rest of the colour scale
				for(size_t j=0; j<m_colourlevels[i].size()-1; ++j)
				{
					//draw the shade - note we use shade not shades because shade has an interface for countourable
					// objects and shades doesn't. see the manual for plshade1 for descriptions. 
					//note sh_col is 0 to use colmap0 and 1 to use colmap1 (we want 1 for continuous mapping)
					double colour=std::max(0.0,std::min(1.0,((m_colourlevels[i][j]+m_colourlevels[i][j+1])/2.0-m_colourscale[i].m_bottom)/(m_colourscale[i].m_top-m_colourscale[i].m_bottom)));
					//double colour=m_colourscale[i].m
					//draw each segment, including lower contour if needed, except for last segment which has both contours
					pl->shade(matrix,minx,maxx,miny,maxy,m_colourlevels[i][j],m_colourlevels[i][j+1],1,colour,1,0,0,0,0,rectilinear,transformer);
					//pl->shade(matrix,m_xs[i][0],m_xs[i].back(),m_ys[i][0],m_ys[i].back(),m_colourlevels[i][j],m_colourlevels[i][j+1],0,1,1,0,0,0,0,rectilinear,&matrix.m_transformer);
					//pl->shade(matrix,m_xs[i][0],m_xs[i].back(),m_ys[i][0],m_ys[i].back(),m_contourlevels[i][j],100000000.0,1,colour,1,0,0,0,0,rectilinear,&matrix.m_transformer);
				}
				//plot the upper out of bounds if needed, note that if there is no out of bounds plplot just does nothing because minlevel>maxlevel
				if(m_filloffscaletop[i] && m_maxstructz[i]>m_colourscale[i].m_top)pl->shade(matrix,minx,maxx,miny,maxy,m_colourscale[i].m_top,m_maxstructz[i],1,1.0,1,0,0,0,0,rectilinear,transformer);
				*/
			}
			//now contours
			if(m_contourlevels[i].size()>0 || (m_colourlevels[i].size()>0 && m_linkcontoursandcolours[i]))
			{
				//set colour for contours
				r=m_linecolour[i].Red();
				g=m_linecolour[i].Green();
				b=m_linecolour[i].Blue();
				a=(double)m_linecolour[i].Alpha()/255.0;
				pl->scol0a(1,r,g,b,a);
				pl->col0(1);
				//set fill to transparent
				pl->scol0a(2,0,0,0,0);
				//set the line style
				std::vector<PLINT>marks;
				std::vector<PLINT>spaces;
				parselinestyle(m_linestyle[i],marks,spaces);
				if(marks.size()==0)pl->styl(0,NULL, NULL);
				else pl->styl(marks.size(),&marks[0],&spaces[0]);

				//plot the contours
				std::vector<double> *pcontourlevels=&m_contourlevels[i];
				if(m_colourlevels[i].size()>0 && m_linkcontoursandcolours[i])
					pcontourlevels=&m_colourlevels[i];
				if(m_linethickness[i]>0.0)
				{
					//draw the contours - note we use shade not shades or cont because shade has an interface for countourable
					// objects and shades and cont don't. see the manual for plshade1 for descriptions. 
					//note sh_col is 0 to use colmap0 and 1 to use colmap1 (we want 0 for contour colour)
					//note we set fill to NULL to not fill the region between contours and for some reason we
					//CANNOT use colour 0 for the contours - it does not draw them in this case
					for(size_t j=0; j<pcontourlevels->size(); ++j)
						pl->shade(matrix,minx,maxx,miny,maxy,(*pcontourlevels)[j],std::numeric_limits<double>::infinity(),0,2,0,1,m_linethickness[i],0,0,rectilinear,transformer);
				}

				//set line style back to normal
				pl->styl(0,NULL, NULL);

			}
		}
	};

	//draw the axes last otherwise they get overdrawn

	//due to the crap way plplot works the outer box can get tickmarks
	//inside or outside but an axis anywhere else has to have tickmarks
	//both sides - therefore we nedd to con plplot into drawing a box
	//edge in the right location
	//this is still to do
	
	//get the tick interval and subintervals
	double xmajint=m_xaxis.m_automajorinterval?0.0:m_xaxis.m_majorinterval;
	unsigned int xnsub=m_xaxis.m_autonsubticks?0.0:m_xaxis.m_nsubticks+1;
	double ymajint=m_yaxis.m_automajorinterval?0.0:m_yaxis.m_majorinterval;
	unsigned int ynsub=m_yaxis.m_autonsubticks?0.0:m_yaxis.m_nsubticks+1;

	//first the x axis
	//set the axis colour
	r=m_xaxis.m_colour.Red();
	g=m_xaxis.m_colour.Green();
	b=m_xaxis.m_colour.Blue();
	pl->scol0(1,r,g,b);
	pl->col0(1);
	pl->width(m_xaxis.m_linethickness*linewidthmultiplier);
	//set the font for the labels
	pl->sfci(m_xaxis.m_labelfci);
	pl->sfontf(m_xaxis.m_labelfont.mb_str());
	pl->schr(0.0,m_xaxis.m_labelsize);
	//set the tick lengths
	pl->smin(0.0,m_xaxis.m_minorticklength);
	pl->smaj(0.0,m_xaxis.m_majorticklength);
	//set the label dp and length
	pl->sxax(m_xaxis.m_maxndigits,0);
	if(m_xaxis.m_autodecimalplaces)pl->prec(0,0);
	else pl->prec(1,m_xaxis.m_ndecimalplaces);
	//create a plplot option string for the axis
	wxString xopt=createploptstring(m_xaxis);
	//set up axes time formats - has no impact if not using time format
	pl->timefmt(m_xaxis.m_timeformat.mb_str());
	pl->slabelfunc(&customlabelinterpreter,m_xaxis.m_customlabelcreator);
	//draw the x axis
	pl->box(xopt.mb_str(),xmajint,xnsub,"",0.0,0.0);

	//now the y axis
	//set the axis colour
	r=m_yaxis.m_colour.Red();
	g=m_yaxis.m_colour.Green();
	b=m_yaxis.m_colour.Blue();
	pl->scol0(1,r,g,b);
	pl->col0(1);
	pl->width(m_yaxis.m_linethickness*linewidthmultiplier);
	//set the font for the labels
	pl->sfci(m_yaxis.m_labelfci);
	pl->sfontf(m_yaxis.m_labelfont.mb_str());
	pl->schr(0.0,m_yaxis.m_labelsize);
	//set the tick lengths
	pl->smin(0.0,m_yaxis.m_minorticklength);
	pl->smaj(0.0,m_yaxis.m_majorticklength);
	//set the label dp and length
	pl->syax(m_yaxis.m_maxndigits,0);
	if(m_yaxis.m_autodecimalplaces)pl->prec(0,0);
	else pl->prec(1,m_yaxis.m_ndecimalplaces);
	//create a plplot option string for the axis
	wxString yopt=createploptstring(m_yaxis);
	//set up axes time formats - has no impact if not using time format
	pl->timefmt(m_yaxis.m_timeformat.mb_str());
	pl->slabelfunc(&customlabelinterpreter,m_yaxis.m_customlabelcreator);
	//draw the y axis
	//to do: allow the labels to have different colour to the axis
	pl->box("",0.0,0,yopt.mb_str(),ymajint,ynsub);
	//pl->box("",0.0,0,yopt.mb_str(),100,1);
	
	
	
	//add axis lables and a title, to give each axis label and the title independant control over font
	//we draw them separately
	//first the x axis
	pl->sfci(m_xaxis.m_titlefci);
	pl->sfontf(m_xaxis.m_titlefont.mb_str());
	pl->schr(0.0,m_xaxis.m_titlesize);
	r=m_xaxis.m_titlecolour.Red();
	g=m_xaxis.m_titlecolour.Green();
	b=m_xaxis.m_titlecolour.Blue();
	pl->scol0(1,r,g,b);
	pl->col0(1);
	if(m_xaxis.m_rotatetitle)pl->mtex("bv",m_xaxis.m_titledistance,0.5,0.5,m_xaxis.m_title.mb_str());
	else pl->mtex("b",m_xaxis.m_titledistance,0.5,0.5,m_xaxis.m_title.mb_str());
	//then the y axis
	pl->sfci(m_yaxis.m_titlefci);
	pl->sfontf(m_yaxis.m_titlefont.mb_str());
	pl->schr(0.0,m_yaxis.m_titlesize);
	r=m_yaxis.m_titlecolour.Red();
	g=m_yaxis.m_titlecolour.Green();
	b=m_yaxis.m_titlecolour.Blue();
	pl->scol0(1,r,g,b);
	pl->col0(1);
	/*double pagexmin;
	double pagexmax;
	double pageymin;
	double pageymax;
	double vpxmin;
	double vpxmax;
	double vpymin;
	double vpymax;
	double charheightdep;
	double charheight;
	pl->gspa(pagexmin,pagexmax,pageymin,pageymax);
	pl->gvpw(vpxmin,vpxmax,vpymin,vpymax);
	pl->gchr(charheightdep,charheight);
	double scalefactor=(xmax-xmin)/(pagewidth)*(pageheight)/(ymax-ymin);
	if(m_yaxis.m_rotatelabels)pl->mtex("lv",-m_yaxis.m_titledistance+(pagexmax-pagexmin)*vpxmin/charheight,0.5,0.5,m_yaxis.m_title.mb_str());
	else pl->mtex("l",m_yaxis.m_titledistance*scalefactor,0.5,0.5,m_yaxis.m_title.mb_str());*/
	if(m_yaxis.m_rotatetitle)pl->mtex("lv",m_yaxis.m_titledistance,0.5,0.5,m_yaxis.m_title.mb_str());
	//if(m_yaxis.m_rotatetitle)pl->mtex("lv",m_yaxis.m_titledistance,0.5,0.5,m_yaxis.m_title.mb_str());
	else pl->mtex("l",m_yaxis.m_titledistance,0.5,0.5,m_yaxis.m_title.mb_str());
	


	//draw the title
	pl->sfci(m_titlefci);
	pl->sfontf(m_titlefont.mb_str());
	pl->schr(0.0,m_titlesize);
	r=m_titlecolour.Red();
	g=m_titlecolour.Green();
	b=m_titlecolour.Blue();
	pl->scol0(0,r,g,b);
	pl->col0(0);
	pl->mtex("t",m_titledistance,0.5,0.5,m_title.mb_str());
	
	//legend
	/*double legendwidth;
	double legendheight;
	pl->scol0(0,255,255,255);//background colour
	pl->scol0(1,0,0,0);//box colour
	pl->scol0(2,255,0,0);
	std::vector<PLINT>legendopts(2);
	legendopts[0]=PL_LEGEND_LINE;
	legendopts[1]=PL_LEGEND_COLOR_BOX;
	std::vector<PLINT>textcolours(2,0);
	std::vector<const char*>legendtext(2);
	const char text0[]="y";
	const char text1[]="z";
	legendtext[0]=text0;
	legendtext[1]=text1;
	std::vector<PLINT>boxcolours(2,0);
	std::vector<PLINT>boxpatterns(2,0);
	std::vector<PLINT>boxlinewidths(2,0);
	std::vector<double>boxscales(2,1.0);
	std::vector<PLINT>legendlinecolours(2,2);
	std::vector<PLINT>legendlinewidths(2,1);
	std::vector<PLINT>legendlinestyles(2,0);
	pl->legend(&legendwidth,&legendheight,PL_POSITION_TOP|PL_POSITION_RIGHT,PL_LEGEND_BOUNDING_BOX,0,0.5,0.4,0,1,0,2,1,2,&legendopts[0],1,0.8,0.2,0,&textcolours[0],&legendtext[0],&boxcolours[0],&boxpatterns[0],&boxscales[0],&boxlinewidths[0],&legendlinecolours[0],&legendlinestyles[0],&legendlinewidths[0],NULL,NULL,NULL,NULL);
	*/
	//set the status to not changed since last redraw

	m_haschanged=false;
	m_yaxis.m_haschanged=false;
	m_xaxis.m_haschanged=false;
	
}


splotwindow::splotwindow(wxWindow *parent, bool antialiasing, bool fixedsize, wxWindowID winid, const wxPoint &pos, const wxSize &size, long style, const wxString& name)
:wxScrolledWindow(parent,winid,pos,size,style|wxFULL_REPAINT_ON_RESIZE ,name)
{
	m_fixedsize=fixedsize;
	m_plotsupdated=true;
	m_bitmapwidth=size.GetWidth();
	m_bitmapheight=size.GetHeight();
	m_bitmap=new wxBitmap(size.GetWidth(),size.GetWidth());
	m_antialiasing=antialiasing;
}

splotwindow::~splotwindow()
{
	delete m_bitmap;
	for(size_t i=0; i<m_plots.size(); ++i) delete m_plots[i];
	for(size_t i=0; i<m_legends.size(); ++i) delete m_legends[i];
}

void splotwindow::setfixedsize(int width, int height)
{
	m_fixedsize=false;
	m_bitmapwidth=width;
	m_bitmapheight=height;
	this->SetScrollbars(1,1,width,height);
}

void splotwindow::setautofitplots()
{
	m_fixedsize=true;
	m_bitmapwidth=this->GetClientSize().x;
	m_bitmapheight=this->GetClientSize().y;
	Refresh();
}


splot2d* splotwindow::addplot(double xpos, double ypos, double width, double height, bool logx, bool logy, std::string title, double titlesize, double titledistance, std::string titlefont, int32_t titlestyle, wxColour titlecolour)
{
	splot2d *newplot=new splot2d(logx,logy,title,titlesize,titledistance,titlefont,titlestyle,titlecolour);
	m_plots.push_back(newplot);
	m_plotxloc.push_back(xpos);
	m_plotyloc.push_back(ypos);
	m_plotwidth.push_back(width);
	m_plotheight.push_back(height);
	m_plotsupdated=true;
	Refresh();
	return newplot;
}

splot2d* splotwindow::addplot(double xpos, double ypos, double width, double height, double minx, double maxx, double miny, double maxy, bool logx, bool logy, std::string title, double titlesize, double titledistance, std::string titlefont, int32_t titlestyle, wxColour titlecolour)
{
	splot2d *newplot=new splot2d(minx,maxx,miny,maxy,logx,logy,title,titlesize,titledistance,titlefont,titlestyle,titlecolour);
	m_plots.push_back(newplot);
	m_plotxloc.push_back(xpos);
	m_plotyloc.push_back(ypos);
	m_plotwidth.push_back(width);
	m_plotheight.push_back(height);
	m_plotsupdated=true;
	Refresh();
	return newplot;
}
splot2d* splotwindow::addplot(double xpos, double ypos, double width, double height, double minx, double maxx, double miny, double maxy, double xintersect, double yintersect, bool logx, bool logy, std::string title, double titlesize, double titledistance, std::string titlefont, int32_t titlestyle, wxColour titlecolour)
{
	splot2d *newplot=new splot2d(minx,maxx,miny,maxy,xintersect,yintersect,logx,logy,title,titlesize,titledistance,titlefont,titlestyle,titlecolour);
	m_plots.push_back(newplot);
	m_plotxloc.push_back(xpos);
	m_plotyloc.push_back(ypos);
	m_plotwidth.push_back(width);
	m_plotheight.push_back(height);
	m_plotsupdated=true;
	Refresh();
	return newplot;
}

splotlegend* splotwindow::addlegend(double xpos, double ypos, double width, double height, std::string title, double titlesize, double titledistance, double titlespacing, std::string titlefont, int32_t titlestyle, wxColour titlecolour, wxColour outlinecolour, int outlinewidth)
{
	splotlegend *newlegend=new splotlegend(title,titlesize,titledistance,titlespacing,titlefont,titlestyle,titlecolour,outlinecolour,outlinewidth);
	m_legends.push_back(newlegend);
	m_legendxloc.push_back(xpos);
	m_legendyloc.push_back(ypos);
	m_legendheight.push_back(height);
	m_legendwidth.push_back(width);
	m_plotsupdated=true;
	Refresh();
	return newlegend;
}

splot2d* splotwindow::addcanvas(double xpos, double ypos, double width, double height, bool logx, bool logy)
{
	splot2d *canvas=addplot(xpos,ypos,width,height,logx,logy);
	if(logx)
		canvas->setxlimits(1.0,10.0);
	else
		canvas->setxlimits(0.0,1.0);
	if(logy)
		canvas->setylimits(1.0,10.0);
	else
		canvas->setylimits(0.0,1.0);
	canvas->getxaxis()->settitle("");
	canvas->getxaxis()->setlabelsize(0);
	canvas->getxaxis()->setmajorticklength(0.0);
	canvas->getxaxis()->setminorticklength(0.0);
	canvas->getxaxis()->setlinethickness(0.0);
	canvas->getyaxis()->settitle("");
	canvas->getyaxis()->setlabelsize(0);
	canvas->getyaxis()->setmajorticklength(0.0);
	canvas->getyaxis()->setminorticklength(0.0);
	canvas->getyaxis()->setlinethickness(0.0);
	
	return canvas;
}

void splotwindow::OnPaint(wxPaintEvent &event)
{
	wxBeginBusyCursor();

	int width;
	int height;
	if(m_fixedsize)
	{
		width=m_bitmapwidth;
		height=m_bitmapheight;
	}
	else
	{
		width=GetClientSize().GetX();
		height=GetClientSize().GetY();
	}

	//create the device context for drawing with. Note that OnPaint must 
	//ALWAY create a paintdc even if it isn't used. the wxPaintDC is linked to the
	//screen output 
	wxPaintDC pagedc(this);
	PrepareDC(pagedc); //this should set all coordinates if we have a scrollable window


	//create a wxMemoryDC which will be linked to the bitmap. We'll use this to draw to the bitmap
	//except if using AGG then we create an image instead which we'll need to copy to the bitmap
	wxMemoryDC memdc;

	//check if any of the plots have been updated
	for(size_t i=0; i<m_plots.size(); ++i) m_plotsupdated||m_plots[i]->gethaschanged();
	//if the plots have been resized or if any plots have been updated since the last redraw then
	//we need to regenerate the plot bitmap. Otherwise we can just use the old bitmap to redraw the plot
	if(width!=m_bitmapwidth || height!=m_bitmapheight || m_plotsupdated==true)
	{

		//If it has then we need to redraw the plot from scratch

		//create a new bitmap that is the correct size and link it to the memory dc
		if(width!=m_bitmapwidth||height!=m_bitmapheight)
		{
			delete m_bitmap;
			m_bitmap = new wxBitmap(width,height,32);
			memdc.SelectObject(*m_bitmap);
			//fill the bitmap with white giving a white background for plplot
			//or to show blank if there are no plots
			memdc.FloodFill(0,0,*wxWHITE,wxFLOOD_BORDER);
		}
		else
		{
			memdc.SelectObject(*m_bitmap);
		}

		//remember the new size, so that on the next refresh we can check if the size has changed
		m_bitmapwidth=width;
		m_bitmapheight=height;

		//set that an update is no longer needed
		m_plotsupdated=false;
		//the plots themselve update their own status in the plot function
		//DrawPlots(&memdc,0,false); //testing dc
		//DrawPlots(&memdc,1,false); //testing agg no freetype
		//DrawPlots(&memdc,1,true); //testing agg with freetype

		//select the backend
		//2 is the GC which gives antialiased output, 0 is any wxDC and 1 uses AGG and with or without freetype
		//if(m_antialiasing) DrawPlots(&memdc,2,false);
		if(m_antialiasing) 
		{
			//wxGCDC gcdc(memdc);
			//DrawPlots(&gcdc,width,height,3,false);
			DrawPlots(&memdc,width,height,2,false);
			//DrawPlots(&memdc,width,height,1,true); //testing agg with freetype
		}
		else 
			DrawPlots(&memdc,width,height,0,false);
	}
	else
	{
		//The plots haven't changed so just select the bitmap as it already exists if we haven't resized
		memdc.SelectObject(*m_bitmap);
	}

	//copy the memorydc bitmap to the panel
	//pagedc.Blit(0,0,width,height,&memdc,0,0);
	//reselect null bitmap for the memdc
	pagedc.Blit(0,0,width,height,&memdc,0,0);
	memdc.SelectObject(wxNullBitmap);

	wxEndBusyCursor();
}

//Write the plot window to an image file. This can be either a bitmap
//or vector graphic image. The file extension defines the type of image saved. 
//Valid extensions are png, jpg, bmp, tif, pcx, xpm, xbm, pnm for bitmaps and
//ps for vector. If the extension does not correspond to any of these 
//strings then png is used, although the given extension will still be used 
//in the filename

bool splotwindow::writetofile(wxString filename, int width, int height, double linewidthmultiplier)
{
	//get the extension
	wxFileName fullfile=filename;
	wxString extension=fullfile.GetExt().Lower();
	
	bool result;

	if(extension=="ps" || extension =="eps" || extension =="pdf")
	{
		//check we have access to the output
		std::fstream fout;
		fout.open(filename.mb_str(), std::ios::out);
		if(fout.is_open())
		{
			fout.close();
			wxString printer="Apex++ "+extension.Upper();
			wxString intermediateFile="C:\\ProgramData\\Apex++\\print."+extension;
			if(wxFile::Exists(intermediateFile))
				wxRemoveFile(intermediateFile);
			result=print(false, printer);
			if(result)
			{
				//wait for the file to appear
				while(!wxFileExists(intermediateFile))
				{
				}
				//repeatedly try to rename the file until it succeeds
				while(!MoveFile(intermediateFile, filename))
				{
					DWORD err=GetLastError();
					if(err != 32 //sharing violation - i.e. the file is still in use/being written
						&& err != 80 ) //destination file already exists
						result=false;
					if(err == 80 )
					{
						wxRemoveFile(filename);
						//check if the delete was successful
						if(wxFile::Exists(filename))
							result=false;
					}
					if( !result )
						break;
				}
			}
		}
		else
			result=false;
	}

	/*{
		//here we redraw the plot like OnPaint but using a postscript DC.
		wxPrintData setupdata;
		setupdata.SetColour(true);
		setupdata.SetFilename(filename);
		//setupdata.SetPaperId(wxPAPER_A4);
		setupdata.SetPaperId(wxPAPER_NONE);
		//note we set the image size in mm, but ps uses pts(1/72 inch, ~0.35 mm) and uses integer coordinates. 
		//So setting the image size to the screen resolution gives us a ps resolution of about 3 times bettr 
		//than the screen. Here we've multiplied by 10 to get 30 times better.
		int sizemultiplier=10;
		setupdata.SetPaperSize(wxSize(width*sizemultiplier,height*sizemultiplier));
		setupdata.SetPrintMode(wxPRINT_MODE_FILE);
		//setupdata.SetQuality(wxPRINT_QUALITY_HIGH); //doesn't seem to do anything
		wxPostScriptDC psdc(setupdata);
		result=psdc.StartDoc(wxT("Writing ")+filename);
		if(result==false) return result;
		DrawPlots(&psdc,width*sizemultiplier,height*sizemultiplier,0,false,linewidthmultiplier*sizemultiplier);//0 gives vector output, I think 2 should too but it creates empty postscripts, there is no need to use freetype
		psdc.EndDoc();
	}*/
	else if(extension=="pdf")
		print( false, "PDFCreator" );
	else if(extension==wxT("emf"))
	{
		//here we redraw the plot like OnPaint but using a wxMetafile DC.
		GetClientSize(&width, &height);
		wxMetafileDC metadc(filename,width,height);
		DrawPlots(&metadc,width,height,0,false,linewidthmultiplier);//0 gives vector output
		//close the file - note this gives us a copy of the file in memory which we must delete
		wxMetafile *metafile=metadc.Close();
		result=metafile!=NULL;
		delete metafile;
		//we must call this function to make the file importable into other software
		int minx=metadc.MinX();
		int maxx=metadc.MaxX();
		int miny=metadc.MinY();
		int maxy=metadc.MaxY();
		//wxMakeMetaFilePlaceable(minx,miny,maxx,maxy);
	}
	else
	{
		//load the image handlers
		wxInitAllImageHandlers();
		//create a wxBitmapType to define the type saved as
		//again use PNG as default
		wxBitmapType type=wxBITMAP_TYPE_PNG;
		if(extension==wxT("jpg")) type=wxBITMAP_TYPE_JPEG;
		else if(extension==wxT("bmp")) type=wxBITMAP_TYPE_BMP;
		else if(extension==wxT("tif")) type=wxBITMAP_TYPE_TIF;
		else if(extension==wxT("pcx")) type=wxBITMAP_TYPE_PCX;
		else if(extension==wxT("xpm")) type=wxBITMAP_TYPE_XPM;
		else if(extension==wxT("xbm")) type=wxBITMAP_TYPE_XBM;
		else if(extension==wxT("pnm")) type=wxBITMAP_TYPE_PNM;

		//we are outputting as a bitmap type, basically here we want to draw the plot then write the 
		//pixels to a file
		if(width==m_bitmapwidth && height==m_bitmapheight)
		{
			//first refresh the screen because this function can get called before the panels have been painted
			Refresh();
			Update();
			wxSafeYield();
			//write the bitmap to file
			result=m_bitmap->SaveFile(filename,type);
		}
		else
		{
			//create a wxMemoryDC which will be linked to the bitmap. We'll use this to draw to the bitmap
			//except if using AGG then we create an image instead which we'll need to copy to the bitmap
			wxMemoryDC memdc;
			wxBitmap bitmap(width,height,32);
			memdc.SelectObject(bitmap);
			//fill the bitmap with white giving a white background for plplot
			//or to show blank if there are no plots
			memdc.FloodFill(0,0,*wxWHITE,wxFLOOD_BORDER);

			//the plots themselve update their own status in the plot function
			//DrawPlots(&memdc,0,false); //testing dc
			//DrawPlots(&memdc,1,false); //testing agg no freetype
			//DrawPlots(&memdc,1,true); //testing agg with freetype
			//DrawPlots(&memdc,0,false);
		
			//select the backend
			//2 is the GC which gives antialiased output, 0 is any wxDC and 1 uses AGG and with or without freetype
			if(m_antialiasing)
			{
				//wxGCDC gcdc(memdc);
				//DrawPlots(&gcdc,width,height,3,false);
				DrawPlots(&memdc,width,height,2,false,linewidthmultiplier);
			}
			else DrawPlots(&memdc,width,height,0,false,linewidthmultiplier);

			//reselect null bitmap for the memdc
			memdc.SelectObject(wxNullBitmap);

			//write the bitmap to file
			result=bitmap.SaveFile(filename,type);
		}
	}

	return result;
}

//asabove, but the size will be a multiple of the window size
bool splotwindow::writetofile(wxString filename, double sizemultiplier)
{
	Refresh();
	Update();
	wxSafeYield();
	int width=sci::round(GetClientSize().GetX()*sizemultiplier);
	int height=sci::round(GetClientSize().GetY()*sizemultiplier);
	return writetofile(filename,width,height,sizemultiplier);
}

bool splotwindow::print( bool showDialog )
{
	return print (showDialog, "" );
}

bool splotwindow::print( bool showDialog, wxString printerName )
{
	splotPrintout printout(this, printerName);
	//show page setup dialog
	//if(!printout.setup(true, printerName))
		//return false;
	wxPrintDialogData printDialogData(printout.getPrintDialogData());
    wxPrinter printer(&printDialogData);
    return printer.Print(NULL, &printout, showDialog);
}

void splotwindow::DrawPlots(wxDC *dc, int width, int height, int backend, bool usefreetype, double linewidthmultiplier)
{

	//no point in messing around if the size is zero
	if(height==0 || width==0) return;
	
	//create an image pointer, we'll initialise it to non-NULL later if needed for AGG
	wxImage *image=NULL;

	//Check we have some plots to plot
	if(m_plots.size()==0 &&m_legends.size()==0)
	{
		dc->FloodFill(0,0,*wxWHITE,wxFLOOD_BORDER);
		return;
	}

	//create the stream
	plstream *pl=new plstream();
	pl->scol0(0,255,255,255);
	pl->sexit(&splotexitcatcher);
	//pl->sabort(&splotabortcatcher);
	//set the device to wxwidgets
	pl->sdev("wxwidgets");
	//pl->sdev("psc");
	//pl->sfnam("D:\\contourtest.ps");
	//pl->setopt("name", "d:\\contourtest.ps");

	//set which backend to use
	if(backend==0)pl->setopt( "-drvopt", "backend=0" );
	else if(backend==1)
	{
		if(usefreetype==true)pl->setopt( "-drvopt", "freetype=1,backend=1" );
		else pl->setopt( "-drvopt", "freetype=0,backend=1" );
	}
	else
	{
		std::string backendString="backend=";
		backendString << backend;
		pl->setopt( "-drvopt", backendString.c_str() );
	}

	//set up the page to the size of the bitmap
	pl->spage(0.0,0.0,width,height,0,0);
	//initialize the stream
	pl->init();
	//move to the zeroth page
	pl->adv(0);
	//pass the memorydc to the stream for DC and GC or the wxImage for AGG
	if(backend==0 || backend==2 || backend==3 )pl->cmd( PLESC_DEVINIT, dc );
	else if(backend==1)
	{
		image=new wxImage(width,height);
		pl->cmd( PLESC_DEVINIT, image );
	}


	//draw each plot in turn
	for(size_t i=0; i<m_plots.size(); ++i)
	{
		//set the area covered by this plot
		pl->vpor(m_plotxloc[i],m_plotxloc[i]+m_plotwidth[i],m_plotyloc[i],m_plotyloc[i]+m_plotheight[i]);

		//draw the plot
		m_plots[i]->plot(pl,dc,width,height,m_antialiasing,linewidthmultiplier);
	}
	//draw each legend in turn
	for(size_t i=0; i<m_legends.size(); ++i)
	{
		//set the area covered by this legend
		pl->vpor(m_legendxloc[i],m_legendxloc[i]+m_legendwidth[i],m_legendyloc[i],m_legendyloc[i]+m_legendheight[i]);

		//draw the legend
		m_legends[i]->plot(pl,linewidthmultiplier);
	}

	//we're done drawing so delete pl
	delete pl;

	//for AGG, we have generated a wximage, blt this to the dc
	if(backend==1)
	{
		wxBitmap newbitmap(*image);
		wxMemoryDC memdc(newbitmap);
		dc->Blit(0,0,width,height,&memdc,0,0);
	}

}

void splotwindow::removeplot(splot *plot)
{
	for (size_t i=0; i<m_plots.size(); ++i)
	{
		if(m_plots[i]==plot)
		{
			delete m_plots[i];
			m_plots.erase(m_plots.begin()+i);
			m_plotxloc.erase(m_plotxloc.begin()+i);
			m_plotyloc.erase(m_plotyloc.begin()+i);
			m_plotwidth.erase(m_plotwidth.begin()+i);
			m_plotheight.erase(m_plotheight.begin()+i);
		}
	}
	for (size_t i=0; i<m_legends.size(); ++i)
	{
		if(m_legends[i]==plot)
		{
			delete m_legends[i];
			m_legends.erase(m_legends.begin()+i);
			m_legendxloc.erase(m_legendxloc.begin()+i);
			m_legendyloc.erase(m_legendyloc.begin()+i);
			m_legendwidth.erase(m_legendwidth.begin()+i);
			m_legendheight.erase(m_legendheight.begin()+i);
		}
	}
}

splotframe::splotframe(wxWindow* parent, bool antialiasing, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
:wxFrame(parent,id,title,pos,size,style,name)
{
	m_plotwind=new splotwindow(this, antialiasing,false);
	wxBoxSizer *sizer=new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(m_plotwind,1,wxEXPAND);
	SetSizer(sizer);
}

splotlegend::splotlegend(std::string title, double titlesize, double titledistance, double titlespacing, std::string titlefont, int32_t titlestyle, wxColour titlecolour, wxColour outlinecolour, int outlinewidth)
{
	m_title=wxString(title.c_str(),wxConvUTF8);
	m_titlesize=titlesize;
	m_titledistance=-titledistance;
	m_titlespacing=titlespacing;
	m_titlefont=wxString(titlefont.c_str(),wxConvUTF8);
	m_titlefci=titlestyle;
	m_titlecolour=titlecolour;
	m_outlinecolour=outlinecolour;
	m_outlinethickness=outlinewidth;
}

void splotlegend::addentry(std::string text, double textoffset, double textsize, const std::string &textfont, uint32_t textstyle, double textspacing, wxColour textcolour, wxColour pointcolour, double pointsize, std::string pointsymbol, wxColour linecolour, int linewidth, std::string linestyle)
{
	adddatasetproperties(text,textoffset,textsize,textfont,textstyle,textspacing,textcolour,pointcolour,pointsize,pointsymbol,pl_SYMBOL,wxT("plotsymbols"),linecolour,linewidth,linestyle,splotcolourscale(),false,false,splotsizescale(),0,false,1,0.0, false);
}
void splotlegend::addentry(std::string text, const splotcolourscale &colourscale, bool filloffscaletop, bool filloffscalebottom, double headingoffset, double textoffset, double textsize, const std::string &textfont, uint32_t textstyle, double textspacing,wxColour textcolour, unsigned int ncolourlevels, bool contours, size_t height, bool horizontal)
{
	adddatasetproperties(text,textoffset,textsize,textfont,textstyle,textspacing,textcolour,wxColour(0,0,0),0.0,"",pl_SYMBOL,wxT("plotsymbols"),wxColour(0,0,0),0,"",colourscale,filloffscaletop,filloffscalebottom,splotsizescale(),ncolourlevels,contours,height,headingoffset, horizontal);
}
void splotlegend::addentry(std::string text, const splotsizescale &sizescale, double headingoffset, double textoffset, double textsize, const std::string &textfont, uint32_t textstyle, double textspacing,wxColour textcolour, wxColour pointcolour, std::string pointsymbol, size_t nlines)
{
	adddatasetproperties(text,textoffset,textsize,textfont,textstyle,textspacing,textcolour,pointcolour,0.0,pointsymbol,pl_SYMBOL,wxT("plotsymbols"),wxColour(0,0,0),0,"",splotcolourscale(),false,false,sizescale,0,false,nlines,headingoffset, false);
}

void splotlegend::adddatasetproperties(std::string text, double textoffset, double textsize, const std::string &textfont, uint32_t textstyle, double textspacing,wxColour textcolour, wxColour pointcolour, double pointsize, std::string pointsymbol, uint32_t pointstyle, wxString pointfont, wxColour linecolour, int linewidth, std::string linestyle, const splotcolourscale &colourscale, bool filloffscaletop, bool filloffscalebottom, const splotsizescale &sizescale, unsigned int ncolourlevels, bool contours, size_t nlines, double headingoffset, bool horizontal)
{
	m_headingoffset.push_back(headingoffset);
	m_text.push_back(wxString(text.c_str(),wxConvUTF8));
	m_textoffset.push_back(textoffset);
	m_textsize.push_back(textsize);
	m_textfont.push_back(wxString(textfont.c_str(),wxConvUTF8));
	m_textfci.push_back(textstyle);
	m_textspacing.push_back(textspacing);
	m_textcolour.push_back(textcolour);
	m_pointcolour.push_back(pointcolour);
	m_pointsize.push_back(pointsize);
	m_pointchar.push_back(wxString(pointsymbol.c_str(),wxConvUTF8));
	m_pointfont.push_back(pointfont);
	m_pointfci.push_back(pointstyle);
	m_linecolour.push_back(linecolour);
	m_linethickness.push_back(linewidth);
	m_linestyle.push_back(wxString(linestyle.c_str(),wxConvUTF8));
	m_colourscale.push_back(colourscale);
	m_sizescale.push_back(sizescale);

	m_filloffscaletop.push_back(filloffscaletop);
	m_filloffscalebottom.push_back(filloffscalebottom);
	m_colourlevels.resize(m_colourlevels.size()+1);
	if(ncolourlevels>0) 
	{
		m_colourlevels.back()=sci::indexvector<double>(std::max((int)ncolourlevels+1,2));
		m_colourlevels.back()*=(m_colourscale.back().m_top-m_colourscale.back().m_bottom)/m_colourlevels.back().back();
		m_colourlevels.back()+=m_colourscale.back().m_bottom;
		double interval=m_colourlevels.back().size()>1 ? m_colourlevels.back()[1]-m_colourlevels.back()[0] : 1.0;
		if(filloffscalebottom)
			m_colourlevels.back().insert(m_colourlevels.back().begin(),(m_colourlevels.back())[0]-interval);
		if(filloffscaletop)
			m_colourlevels.back().push_back(m_colourlevels.back().back()+interval);
	}
	m_minstructz.push_back(std::numeric_limits<double>::quiet_NaN());
	m_maxstructz.push_back(std::numeric_limits<double>::quiet_NaN());

	
	m_contourlevels.resize(m_contourlevels.size()+1);
	m_contourlabelssize.push_back(0.0);

	m_contours.push_back(contours);
	m_nlines.push_back(nlines);
	m_horizontal.push_back(horizontal);
}

void splotlegend::plot(plstream *pl, double linewidthmultiplier)
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
	//double a;
	pl->col0(0);

	
	//set the limits of the plot area in terms of plot units
	double xmin=0.0;
	double xmax=1.0;
	double ymin=0.0;
	double ymax=1.0;
	pl->wind(xmin,xmax,ymin,ymax);

	//get a conversion factor for character height scale to height in world coordinates
	double pagexminmm;
	double pagexmaxmm;
	double pageyminmm;
	double pageymaxmm;
	pl->gspa(pagexminmm,pagexmaxmm,pageyminmm,pageymaxmm); //get page size in mm
	double pageheightmm=pageymaxmm-pageyminmm;
	double vporxminnorm;
	double vporxmaxnorm;
	double vporyminnorm;
	double vporymaxnorm;
	pl->gvpd(vporxminnorm,vporxmaxnorm,vporyminnorm,vporymaxnorm); //get viewport size in fraction of page size
	//calculate viewportheight in mm
	double vporheightmm=pageheightmm*(vporymaxnorm-vporyminnorm);
	//we know viewport height in world coordinates - it's ymax-ymin which when writing this was 1.0;
	//get character with scaled height 1.0 size in mm
	double defchrmm;
	double scaledcharheightmm;
	pl->schr(0.0,1.0);
	pl->gchr(defchrmm,scaledcharheightmm);
	double scaledcharheightworld=scaledcharheightmm/vporheightmm*(ymax-ymin);

	//draw a box round the legend
	if(m_outlinethickness>0)
	{
		pl->scol0(0,255,255,255);
		pl->col0(0);
		double xs[5]={0.0,0.0,1.0,1.0,0.0};
		double ys[5]={0.0,1.0,1.0,0.0,0.0};
		pl->fill(4,xs,ys);
		pl->scol0(1,m_outlinecolour.Red(),m_outlinecolour.Green(),m_outlinecolour.Blue());
		pl->col0(1);
		pl->line(5,xs,ys);
	}

	//draw the title
	pl->sfontf(m_titlefont.mb_str(wxConvUTF8));
	pl->sfci(m_titlefci);
	pl->schr(0.0,m_titlesize);
	r=m_titlecolour.Red();
	g=m_titlecolour.Green();
	b=m_titlecolour.Blue();
	pl->scol0(1,r,g,b);
	pl->col0(1);
	pl->mtex("t",m_titledistance+0.5,0.5,0.5,m_title.mb_str(wxConvUTF8));
	//pl->ptex(0.5,0.5,1.0,0.5,0.5,m_title.mb_str(wxConvUTF8)); //testing rotated text
	//pl->ptex(0.5,0.5,1.0,0.4,0.5,m_title.mb_str(wxConvUTF8)); //testing rotated text

	//calulate the position in unity scaled lines of the next line of the legend
	double position=(-m_titledistance+1.0+m_titlespacing)*m_titlesize;
	double positionstep=0.0;
	//draw the legend entries
	for(size_t i=0; i<m_text.size(); ++i)
	{
		position+=positionstep+std::max(m_textsize[i],m_pointsize[i])*0.5*1.6; //the factor of 1.6 is because a spacing of 1 character height gives overlap of tall/caoital letters
		//draw the text
		//offset is the y offset in units of char height scaled to 1
		r=m_textcolour[i].Red();
		g=m_textcolour[i].Green();
		b=m_textcolour[i].Blue();
		pl->scol0(1,r,g,b);
		pl->col0(1);
		pl->sfontf(m_textfont[i].mb_str(wxConvUTF8));
		pl->sfci(m_textfci[i]);
		pl->schr(0.0,m_textsize[i]);

		if(m_sizescale[i].m_value.size()>1 || m_colourlevels[i].size()>0)
		{
			pl->ptex(m_headingoffset[i],1.0-position*scaledcharheightworld,0.0,0.0,0.0,m_text[i].mb_str(wxConvUTF8));
		}
		else pl->ptex(m_textoffset[i],1.0-position*scaledcharheightworld,0.0,0.0,0.0,m_text[i].mb_str(wxConvUTF8));

		//keep track of how high the line is
		positionstep=std::max(m_textsize[i],m_pointsize[i])*(0.5*1.6+m_textspacing[i]);//The factor of 1.6 is because ys and gs overlap

		//then the lines
		if(m_linethickness[i]>0.0)
		{
			r=m_linecolour[i].Red();
			g=m_linecolour[i].Green();
			b=m_linecolour[i].Blue();
			pl->scol0(1,r,g,b);
			pl->col0(1);
			pl->width(m_linethickness[i]*linewidthmultiplier);
			pl->schr(0.0,m_textsize[i]);
			//set the line style
			std::vector<PLINT>marks;
			std::vector<PLINT>spaces;
			parselinestyle(m_linestyle[i],marks,spaces);
			if(marks.size()==0)pl->styl(0,NULL, NULL);
			else pl->styl(marks.size(),&marks[0],&spaces[0]);

			double x[2];
			x[0]=m_textoffset[i]*0.1;
			x[1]=m_textoffset[i]*0.9;
			double y[2];
			y[0]=1.0-position*scaledcharheightworld;
			y[1]=1.0-position*scaledcharheightworld;
			pl->line(2,x,y);
			//set line style back to normal
			pl->styl(0,NULL, NULL);
		}
		
		//then points
		if(m_pointsize[i]>0.0)
		{
			r=m_pointcolour[i].Red();
			g=m_pointcolour[i].Green();
			b=m_pointcolour[i].Blue();
			pl->scol0(1,r,g,b);
			pl->col0(1);
			pl->sfontf(m_pointfont[i].mb_str(wxConvUTF8));
			pl->sfci(m_pointfci[i]);
			pl->schr(0.0,m_pointsize[i]);
			double x=m_textoffset[i]*0.5;
			double y=1.0-position*scaledcharheightworld;
			//pl->string(1,&x,&y,m_pointchar[i].mb_str(wxConvUTF8))
			pl->string(1,&x,&y,m_pointchar[i].mb_str());
			//set text params back as they were
			r=m_textcolour[i].Red();
			g=m_textcolour[i].Green();
			b=m_textcolour[i].Blue();
			pl->scol0(1,r,g,b);
			pl->col0(1);
			pl->sfontf(m_textfont[i].mb_str(wxConvUTF8));
			pl->sfci(m_textfci[i]);
			pl->schr(0.0,m_textsize[i]);

			double positionstep=std::max(positionstep,m_pointsize[i]*(0.5*1.6+m_textspacing[i]));
		}

		//then size scale
		if(m_sizescale[i].m_value.size()>1)
		{
			std::vector<double> sizes(m_nlines[i]);
			if(m_nlines[i]==1) sizes[0]=(m_sizescale[i].m_size[0]+m_sizescale[i].m_size.back())/2.0;
			else
			{
				for(size_t j=0; j<m_nlines[i]; ++j) sizes[j]=m_sizescale[i].m_size[0]+(m_sizescale[i].m_size.back()-m_sizescale[i].m_size[0])*(double)j/((double)m_nlines[i]-1.0);
			}

			//draw the first point
			r=m_pointcolour[i].Red();
			g=m_pointcolour[i].Green();
			b=m_pointcolour[i].Blue();
			pl->scol0(1,r,g,b);
			pl->col0(1);
			pl->sfontf(m_pointfont[i].mb_str(wxConvUTF8));
			pl->sfci(m_pointfci[i]);
			position+=positionstep+0.5*std::max(sizes[0],m_textsize[i]*1.6);//here the 1.6 is only applied to text as symbols don't have tall or dangly bits;
			pl->schr(0.0,sizes[0]);
			double x=m_textoffset[i]*0.5;
			double y=1.0-position*scaledcharheightworld;
			pl->string(1,&x,&y,m_pointchar[i].mb_str(wxConvUTF8));
			
			//then the first label
			r=m_textcolour[i].Red();
			g=m_textcolour[i].Green();
			b=m_textcolour[i].Blue();
			pl->scol0(1,r,g,b);
			pl->col0(1);
			pl->sfontf(m_textfont[i].mb_str(wxConvUTF8));
			pl->sfci(m_textfci[i]);
			pl->schr(0.0,m_textsize[i]);
			wxString value;
			value << m_sizescale[i].m_value[0];
			pl->ptex(m_textoffset[i],1.0-position*scaledcharheightworld,0.0,0.0,0.0,value.mb_str(wxConvUTF8));
			//now the remaining points
			for(size_t j=1; j<m_nlines[i]; ++j)
			{
				//points
				r=m_pointcolour[i].Red();
				g=m_pointcolour[i].Green();
				b=m_pointcolour[i].Blue();
				pl->scol0(1,r,g,b);
				pl->col0(1);
				pl->sfontf(m_pointfont[i].mb_str(wxConvUTF8));
				pl->sfci(m_pointfci[i]);
				//position+=std::max(sizes[j-1]*0.5+m_textspacing[i]*m_textsize[i],m_textsize[i]*(0.5*1.6+m_textspacing[i]));
				position+=std::max(sizes[j-1]+m_textspacing[i]*m_textsize[i],m_textsize[i]*(1.6+m_textspacing[i]));
				position+=0.5*std::max(sizes[j],m_textsize[i]*1.6)*2.0;
				pl->schr(0.0,sizes[j]);
				double x=m_textoffset[i]*0.5;
				double y=1.0-position*scaledcharheightworld;
				pl->string(1,&x,&y,m_pointchar[i].mb_str(wxConvUTF8));
				//labels
				r=m_textcolour[i].Red();
				g=m_textcolour[i].Green();
				b=m_textcolour[i].Blue();
				pl->scol0(1,r,g,b);
				pl->col0(1);
				pl->sfontf(m_textfont[i].mb_str(wxConvUTF8));
				pl->sfci(m_textfci[i]);
				pl->schr(0.0,m_textsize[i]);
				value.clear();
				value << m_sizescale[i].m_value[0]+(m_sizescale[i].m_value.back()-m_sizescale[i].m_value[0])*(double)j/(double)(m_nlines[i]+1);
				pl->ptex(m_textoffset[i],1.0-position*scaledcharheightworld,0.0,0.0,0.0,value.mb_str(wxConvUTF8));
			}
			positionstep=m_textspacing[i]*m_textsize[i]+std::max(sizes.back(),m_textsize[i]);
			//set text params back as they were
			r=m_textcolour[i].Red();
			g=m_textcolour[i].Green();
			b=m_textcolour[i].Blue();
			pl->scol0(1,r,g,b);
			pl->col0(1);
			pl->sfontf(m_textfont[i].mb_str(wxConvUTF8));
			pl->sfci(m_textfci[i]);
			pl->schr(0.0,m_textsize[i]);
		}

		//and finally a colour scale
		if(m_colourlevels[i].size()>0)
		{
			//bool horizontal=false;

			//step on past the heading
			position+=positionstep+0.5*m_textsize[i]*1.6;
			std::vector<double> x=sci::indexvector<double>(2);
			//std::vector<double> y=m_colourscale[i].m_value*(m_colourscale[i].m_top-m_colourscale[i].m_bottom)+m_colourscale[i].m_bottom;
			std::vector<double> y=m_colourlevels[i];

			std::vector<std::vector<double> > z(2);
			z[0]=y;
			z[1]=y;
			if( m_horizontal[i])
			{
				std::swap( x, y );
				z=sci::transpose( z );
			}
			pl->schr(0.0,m_textsize[i]);
			//reset the viewport up where we want it
			double xmin=vporxminnorm+0.1*m_textoffset[i]*(vporxmaxnorm-vporxminnorm);
			double xmax=vporxminnorm+0.9*m_textoffset[i]*(vporxmaxnorm-vporxminnorm);
			double ymin=vporymaxnorm-(position+m_nlines[i])*scaledcharheightworld*(vporymaxnorm-vporyminnorm);
			double ymax=vporymaxnorm-(position-0.5)*scaledcharheightworld*(vporymaxnorm-vporyminnorm);
			pl->vpor( xmin, xmax, ymin, ymax );
			//set up the min/max x/y values within the viewport
			pl->wind( x[0], x.back(), y[0], y.back() );
			//create a contourable object for plotting
			splot2dmatrix matrix(&z);
			bool rectilinear=true;
			splotindextransformer1d transformer(&x,&y,splot_REGULAR1DXY);


			//draw the colour scale
				
			rgbcolour rgb;
			//fill the different levels
			for( size_t j=1; j<m_colourlevels[i].size(); ++j )
			{
				//check if this level is in bounds
				if( m_colourscale[i].getNormalisedValue( m_colourlevels[i][j] ) < 0.0 )
					continue;
				if( m_colourscale[i].getNormalisedValue( m_colourlevels[i][j-1] ) > 1.0 )
					break;
				//get the colour
				rgb=m_colourscale[i].getRgbOriginalScale( (m_colourlevels[i][j-1] + m_colourlevels[i][j])/2.0 );
				pl->scol0a( 1, sci::round( rgb.r() * 255.0 ), sci::round( rgb.g() * 255.0 ), sci::round( rgb.b() * 255.0 ), rgb.a() );
				//get the edges - being careful for crossing off the ends of the scale
				double lowLimit=m_colourlevels[i][j-1];
				double highLimit=m_colourlevels[i][j];
				//I'm pretty sure these limit checks are not needed
				//if(m_colourscale[i].getNormalisedValue( m_colourlevels[i][j-1] ) < 0.0 && !m_filloffscalebottom[i])
				//	lowLimit=m_colourscale[i].m_bottom;
				//if(m_colourscale[i].getNormalisedValue( m_colourlevels[i][j] ) > 1.0 && !m_filloffscaletop[i])
				//	highLimit=m_colourscale[i].m_top;
				//draw the band
				pl->shade( matrix, x[0], x.back(), y[0], y.back(), lowLimit, 
					highLimit, 0, 1, 1, 0, 0, 0, 0, true, nullptr);
			}
			if(m_contours[i])
			{
				pl->scol0(1,0,0,0);
				pl->col0(1);
				pl->width(1.0*linewidthmultiplier);
				for(size_t j=0; j<m_colourlevels[i].size()-2; ++j)
				{
					std::vector<double> x=sci::indexvector<double>(2);
					std::vector<double> y(2,m_colourlevels[i][j+1]);
					if( m_horizontal[i] )
						std::swap( x, y );
					pl->line(2,&x[0],&y[0]);
				}
			}
			
			//draw a bounding box
			pl->sfontf(m_textfont[i].mb_str(wxConvUTF8));
			pl->sfci(m_textfci[i]);
			pl->schr(0.0,m_textsize[i]);
			pl->width(1.0*linewidthmultiplier);
			pl->scol0(1,0,0,0);
			pl->col0(1);
			//options to draw two ends and axis with ticks
			std::string xOpts="bc";
			std::string yOpts="ctmiv";
			//options to draw the side oposite axis without ticks
			std::string xOpts2="";
			std::string yOpts2="b";
			if( m_colourscale[i].m_logarithmic )
				yOpts.append( "l" );
			if( m_horizontal[i] )
			{
				yOpts.replace(yOpts.find("c"),1,"b");
				yOpts.replace(yOpts.find("m"),1,"n");
				yOpts2.replace(yOpts2.find("b"),1,"c");
				std::swap( xOpts, yOpts );
				std::swap( xOpts2, yOpts2 );
			}
			pl->box(xOpts.c_str(),0.0,0,yOpts.c_str(),0.0,1);
			pl->box(xOpts2.c_str(),0.0,0,yOpts2.c_str(),0.0,0);

			//reset the viewport
			pl->vpor(vporxminnorm,vporxmaxnorm,vporyminnorm,vporymaxnorm);
			pl->wind(0.0,1.0,0.0,1.0);

			positionstep=(double)m_nlines[i]+0.5;
		}
	}
}

splotPrintout::splotPrintout(splotwindow *window, wxString printerName)
	:wxPrintout(window->GetName())
{
	m_window=window;
	setup(false, printerName);
}
void splotPrintout::GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo)
{
	*minPage=1;
	*maxPage=1;
	*pageFrom=1;
	*pageTo=1;
}

bool splotPrintout::HasPage( int pageNum )
{
	return pageNum==1;
}

//setup the print parameters. If showDialog is true a page setup dialog
//will be shown. PrinterName can be set to the printer name to be used
//or to an empty string to use the default printer
bool splotPrintout::setup(bool showDialog, wxString printerName)
    {
        // don't show page setup dialog, use default values
        wxPrintData printData;
        printData.SetPrintMode( wxPRINT_MODE_PRINTER );
        printData.SetOrientation( wxPORTRAIT );
        printData.SetNoCopies(1);
		printData.SetPaperId( wxPAPER_A4 ); 
		printData.SetPrinterName( printerName );
 
        m_pageSetupDialogData=wxPageSetupDialogData(printData);
        //pageSetupDialogData.SetMarginTopLeft    (wxPoint(m_margin_left,  m_margin_top));
        //pageSetupDialogData.SetMarginBottomRight(wxPoint(m_margin_right, m_margin_bottom)); 
 
        if (showDialog)
        {
            wxPageSetupDialog dialog( NULL, &m_pageSetupDialogData );
            if (dialog.ShowModal() == wxID_OK)
            {
 
                m_pageSetupDialogData = dialog.GetPageSetupData();
                //m_orient = m_page_setup.GetPrintData().GetOrientation();
                //m_paper_type = m_page_setup.GetPrintData().GetPaperId();
 
                //wxPoint marginTopLeft = m_page_setup.GetMarginTopLeft();
                //wxPoint marginBottomRight = m_page_setup.GetMarginBottomRight();
                //m_margin_left   = marginTopLeft.x;
                //m_margin_right  = marginBottomRight.x;
                //m_margin_top    = marginTopLeft.y;
                //m_margin_bottom = marginBottomRight.y;
            }
            else
            {
                std::cout << "user canceled at first dialog" << std::endl;
                return false;
            }
        }
        return true;
    }


bool splotPrintout::OnPrintPage(int pageNum)
{
    std::cout << "printing page " << pageNum << std::endl;
 
	int m_coord_system_width=m_window->GetClientSize().x;
	int m_coord_system_height=m_window->GetClientSize().y;

    // ---- setup DC with coordinate system ----
    FitThisSizeToPageMargins(wxSize(m_coord_system_width, m_coord_system_height), m_pageSetupDialogData);
 
    wxRect fitRect = GetLogicalPageMarginsRect(m_pageSetupDialogData);
 
    wxCoord xoff = (fitRect.width - m_coord_system_width) / 2;
    wxCoord yoff = (fitRect.height - m_coord_system_height) / 2;
    OffsetLogicalOrigin(xoff, yoff);
 
    wxDC* ptr = GetDC();
    if (ptr==NULL || !ptr->IsOk())
    {
        std::cout << "DC is not Ok, interrupting printing" << std::endl;
        return false;
    }
    wxDC& dc = *ptr;

	m_window->DrawPlots( ptr,m_coord_system_width, m_coord_system_height, 0, false, 1.0);
 
    return true;
}

wxPrintDialogData splotPrintout::getPrintDialogData()
{
	return m_pageSetupDialogData.GetPrintData();
}