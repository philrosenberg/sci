#define _USE_MATH_DEFINES
#include<svector/splot.h>
#include<svector/svector.h>
#include<cmath>
#include<algorithm>
#include<wx/filename.h>
#include<wx/dcps.h>
#include<wx/metafile.h>
#include<wx/gdicmn.h>

BEGIN_EVENT_TABLE( splotwindow, wxPanel )
	EVT_PAINT( splotwindow::OnPaint )
    EVT_ERASE_BACKGROUND(splotwindow::OnEraseBackGround)
END_EVENT_TABLE()

int sploterrorcatcher(const char *message)
{
	return 0;
}
//transformations

void lineartransform(double xindex, double yindex, const std::vector<double> &x, const std::vector<double> &y, double &xout, double &yout)
{
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

splotcolourscale::splotcolourscale(const std::vector<double> &value, const std::vector<wxColour> &colour,bool logarithmic, bool hsl, bool rbgr)
{
	m_autovalue=false;
	//assign values as are
	m_colour1.resize(colour.size());
	m_colour2.resize(colour.size());
	m_colour3.resize(colour.size());
	m_alpha.resize(colour.size());
	for(size_t i=0; i<colour.size(); ++i)
	{
		m_colour1[i]=(double)colour[i].Red()/255.0;
		m_colour2[i]=(double)colour[i].Green()/255.0;
		m_colour3[i]=(double)colour[i].Blue()/255.0;
		m_alpha[i]=(double)colour[i].Alpha()/255.0;
	}
	m_value=value;
	m_logarithmic=logarithmic;
	m_hsl=hsl;
	m_rbgr=new bool[m_value.size()];
	for(size_t i=1; i<m_value.size(); ++i) m_rbgr[i]=rbgr;
	//check there is at least one value in m_value
	if(m_value.size()==0) m_value.resize(1,0.0);
	//check values are ascending
	for(size_t i=1; i<m_value.size(); ++i)
	{
		if(m_value[i]<=m_value[i-1])m_value.resize(i);
	}
	//check the colours and values are the same length
	m_colour1.resize(m_value.size());
	m_colour2.resize(m_value.size());
	m_colour3.resize(m_value.size());
	m_alpha.resize(m_value.size());

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
	m_hsl=s.m_hsl;
	m_rbgr=new bool[m_value.size()];
	for(size_t i=0; i<m_value.size(); ++i) m_rbgr[i]=s.m_rbgr[i];
	m_logarithmic=s.m_logarithmic;
	m_bottom=s.m_bottom;
	m_top=s.m_top;
}

splotcolourscale& splotcolourscale::operator=(const splotcolourscale& s)
{
	if(this== &s) return *this;
	m_autovalue=s.m_autovalue;
	m_value=s.m_value;
	m_colour1=s.m_colour1;
	m_colour2=s.m_colour2;
	m_colour3=s.m_colour3;
	m_alpha=s.m_alpha;
	m_hsl=s.m_hsl;
	delete[] m_rbgr;
	m_rbgr=new bool[m_value.size()];
	for(size_t i=0; i<m_value.size(); ++i) m_rbgr[i]=s.m_rbgr[i];
	m_logarithmic=s.m_logarithmic;
	return *this;
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

splot2dmatrix::splot2dmatrix(std::vector< std::vector<double> > *z, std::vector<double> *x, std::vector<double> *y, void (*transformfunc)(double xindex, double yindex, const std::vector<double> &x, const std::vector<double> &y, double &xout, double &yout))
:Contourable_Data(x->size(),y->size())
{
	m_z=z;
	m_transformer.m_x=x;
	m_transformer.m_y=y;
	m_transformer.m_transformfunc=transformfunc;
}

splotcoordtransformer::splotcoordtransformer(std::vector<double> *x,std::vector<double> *y, void (*transformfunc)(double xindex, double yindex, const std::vector<double> &x, const std::vector<double> &y, double &xout, double &yout))
{
	//if any parameters are null set them all to null
	if(x==NULL||y==NULL|| transformfunc==NULL)
	{
		m_x=NULL;
		m_y=NULL;
		m_transformfunc=NULL;
		return;
	}

	m_x=x;
	m_y=y;
	m_transformfunc=transformfunc;
}
void splotcoordtransformer::xform( PLFLT ox, PLFLT oy, PLFLT& nx, PLFLT& ny )const
{
	//if we are null then simply return the numbers back
	if(m_transformfunc==NULL)
	{
		nx=ox;
		ny=oy;
		return;
	}

	//otherwise, we set our values using m_transformfunc
	m_transformfunc(ox,oy,*m_x,*m_y,nx,ny);
}


splotaxis::splotaxis(double min, double max, wxString title, double titlesize, double titledistance, const wxColour &titlecolour, double intersectpoint, wxColour colour, int linethickness, bool logarithmic, bool time, double majorticklength, double minorticklength, bool tickspositive, bool ticksnegative, bool showlabels, bool labelpositionpositive, wxString labelfont, PLUNICODE labelstyle, bool labelsrotated, double labelsize, const wxColour &labelcolour)
{
	m_min=min;
	m_max=max;
	m_automajorinterval=true;
	m_autonsubticks=true;
	m_intersect=intersectpoint;
	m_logarithmic=logarithmic;
	m_time=time;
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
	m_rotatetitle=false;
	m_titlesize=titlesize;
	m_titledistance=titledistance;
	m_titlecolour=titlecolour;
	m_haschanged=true;
}
splotaxis::splotaxis(double min, double max, double majorinterval, double nsubticks, wxString title, double titlesize, double titledistance, const wxColour &titlecolour, double intersectpoint, wxColour colour, int linethickness, bool logarithmic, bool time, double majorticklength, double minorticklength, bool tickspositive, bool ticksnegative, bool showlabels, bool labelpositionpositive, wxString labelfont, PLUNICODE labelstyle, bool labelsrotated, double labelsize, const wxColour &labelcolour)
{
	m_min=min;
	m_max=max;
	m_majorinterval=majorinterval;
	m_nsubticks=nsubticks;
	m_automajorinterval=false;
	m_autonsubticks=false;
	m_intersect=intersectpoint;
	m_logarithmic=logarithmic;
	m_time=time;
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
	m_rotatetitle=false;
	m_titlesize=titlesize;
	m_titledistance=titledistance;
	m_titlecolour=titlecolour;
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

void splot2d::setallparams(bool logx, bool logy,std::string title, double titlesize, double titledistance, std::string titlefont, int32_t titlestyle, wxColour titlecolour, void (*transformfunc)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&))
{
	//these aren't given in the constructor parameters but must be true for this constructor
	m_xautointersect=true;
	m_yautointersect=true;
	m_xautolimits=true;
	m_yautolimits=true;

	//also set the transformfunc to NULL
	m_transfunc=transformfunc;

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

splot2d::splot2d(bool logx, bool logy,std::string title, double titlesize, double titledistance, std::string titlefont, int32_t titlestyle, wxColour titlecolour, void (*transformfunc)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&))
{
	//this is the default constructor so just set the all the params to default values
	setallparams(logx, logy, title, titlesize, titledistance, titlefont, titlestyle, titlecolour,transformfunc);
	//set y axis labels the right way round
	m_yaxis.m_rotatelabels=true;
}

splot2d::splot2d(double minx, double maxx, double miny, double maxy, bool logx, bool logy, std::string title, double titlesize, double titledistance, std::string titlefont, int32_t titlestyle, wxColour titlecolour, void (*transformfunc)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&))
{
	//set the params to default values
	setallparams(logx, logy, title, titlesize, titledistance, titlefont, titlestyle, titlecolour,transformfunc);

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
splot2d::splot2d(double minx, double maxx, double miny, double maxy, double xintersect, double yintersect, bool logx, bool logy, std::string title, double titlesize, double titledistance, std::string titlefont, int32_t titlestyle, wxColour titlecolour,void (*transformfunc)(double, double, const std::vector<double>&, const std::vector<double>&, double&, double&))
{
	//set the params to default values
	setallparams(logx, logy, title, titlesize, titledistance, titlefont, titlestyle, titlecolour,transformfunc);

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


void splot2d::adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &xposerrs, const std::vector<double> &xnegerrs, const std::vector<double> &yposerrs, const std::vector<double> &ynegerrs, wxColour pointcolour, double pointsize, std::string pointsymbol, wxColour linecolour, int linewidth, wxColour xerrcolour, wxColour yerrcolour, int xerrwidth, int yerrwidth)
{
	//checks
	if(xs.size()!=ys.size()) return;
	if(xposerrs.size()!=0 && xposerrs.size()!=xs.size())return;
	if(xposerrs.size()!=0 && xnegerrs.size()!=xs.size())return;
	if(yposerrs.size()!=0 && yposerrs.size()!=xs.size())return;
	if(ynegerrs.size()!=0 && ynegerrs.size()!=xs.size())return;

	m_xs.push_back(xs);
	m_ys.push_back(ys);
	m_structzs.resize(m_structzs.size()+1);
	m_colunstructzs.resize(m_colunstructzs.size()+1);
	m_sizeunstructzs.resize(m_sizeunstructzs.size()+1);
	if(xposerrs.size()>0 && xnegerrs.size()>0)
	{
		m_xpluserrs.push_back(xs+xposerrs);
		m_xminuserrs.push_back(xs-xnegerrs);
	}
	else if(xposerrs.size()>0)
	{
		m_xpluserrs.push_back(xs+xposerrs);
		m_xminuserrs.push_back(xs);
	}
	else if(xnegerrs.size()>0)
	{
		m_xpluserrs.push_back(xs);
		m_xminuserrs.push_back(xs-xnegerrs);
	}
	else
	{
		m_xpluserrs.push_back(std::vector<double>(0));
		m_xminuserrs.push_back(std::vector<double>(0));
	}
	if(yposerrs.size()>0 && ynegerrs.size()>0)
	{
		m_ypluserrs.push_back(ys+yposerrs);
		m_yminuserrs.push_back(ys-ynegerrs);
	}
	else if(yposerrs.size()>0)
	{
		m_ypluserrs.push_back(ys+yposerrs);
		m_yminuserrs.push_back(ys);
	}
	else if(ynegerrs.size()>0)
	{
		m_ypluserrs.push_back(ys);
		m_yminuserrs.push_back(ys-ynegerrs);
	}
	else
	{
		m_ypluserrs.push_back(std::vector<double>(0));
		m_yminuserrs.push_back(std::vector<double>(0));
	}

	adddatasetproperties(pointcolour,pointsize,pointsymbol,pl_SYMBOL,wxT("plotsymbols"),linecolour,linewidth,splotcolourscale(),splotsizescale(),xerrcolour,yerrcolour,xerrwidth,yerrwidth,0,false,false,0,std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN(),0.0);

	if(m_xaxis.m_logarithmic) 
	{
		m_xsl.push_back(sci::log10(xs));
		m_xpluserrsl.push_back(sci::log10(m_xpluserrs.back()));
		m_xminuserrsl.push_back(sci::log10(m_xminuserrs.back()));
	}
	else 
	{
		m_xsl.resize(m_xsl.size()+1);
		m_xpluserrsl.resize(m_xpluserrsl.size()+1);
		m_xminuserrsl.resize(m_xminuserrsl.size()+1);
	}
	if(m_yaxis.m_logarithmic) 
	{
		m_ysl.push_back(sci::log10(ys));
		m_ypluserrsl.push_back(sci::log10(m_ypluserrs.back()));
		m_yminuserrsl.push_back(sci::log10(m_yminuserrs.back()));
	}
	else 
	{
		m_ysl.resize(m_xsl.size()+1);
		m_ypluserrsl.resize(m_ypluserrsl.size()+1);
		m_yminuserrsl.resize(m_yminuserrsl.size()+1);
	}

	calculateautolimits();
	m_haschanged=true;
}


void splot2d::adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels, const std::vector<double> &xposerrs, const std::vector<double> &xnegerrs, const std::vector<double> &yposerrs, const std::vector<double> &ynegerrs, double pointsize, std::string pointsymbol, wxColour linecolour, int linewidth, wxColour xerrcolour, wxColour yerrcolour, int xerrwidth, int yerrwidth)
{
	//checks
	if(xs.size()!=ys.size()) return;
	if(xs.size()!=zs.size()) return;
	if(xposerrs.size()!=0 && xposerrs.size()!=xs.size())return;
	if(xposerrs.size()!=0 && xnegerrs.size()!=xs.size())return;
	if(yposerrs.size()!=0 && yposerrs.size()!=xs.size())return;
	if(ynegerrs.size()!=0 && ynegerrs.size()!=xs.size())return;

	m_xs.push_back(xs);
	m_ys.push_back(ys);
	m_colunstructzs.push_back(zs);
	m_structzs.resize(m_structzs.size()+1);
	m_sizeunstructzs.resize(m_sizeunstructzs.size()+1);
	if(xposerrs.size()>0 && xnegerrs.size()>0)
	{
		m_xpluserrs.push_back(xs+xposerrs);
		m_xminuserrs.push_back(xs-xnegerrs);
	}
	else if(xposerrs.size()>0)
	{
		m_xpluserrs.push_back(xs+xposerrs);
		m_xminuserrs.push_back(xs);
	}
	else if(xnegerrs.size()>0)
	{
		m_xpluserrs.push_back(xs);
		m_xminuserrs.push_back(xs-xnegerrs);
	}
	else
	{
		m_xpluserrs.push_back(std::vector<double>(0));
		m_xminuserrs.push_back(std::vector<double>(0));
	}
	if(yposerrs.size()>0 && ynegerrs.size()>0)
	{
		m_ypluserrs.push_back(ys+yposerrs);
		m_yminuserrs.push_back(ys-ynegerrs);
	}
	else if(yposerrs.size()>0)
	{
		m_ypluserrs.push_back(ys+yposerrs);
		m_yminuserrs.push_back(ys);
	}
	else if(ynegerrs.size()>0)
	{
		m_ypluserrs.push_back(ys);
		m_yminuserrs.push_back(ys-ynegerrs);
	}
	else
	{
		m_ypluserrs.push_back(std::vector<double>(0));
		m_yminuserrs.push_back(std::vector<double>(0));
	}

	adddatasetproperties(wxColour(0,0,0),pointsize,pointsymbol,pl_SYMBOL,wxT("plotsymbols"),linecolour,linewidth,colourscale,splotsizescale(),xerrcolour,yerrcolour,xerrwidth,yerrwidth,ncolourlevels,false,false,0,std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN(),0.0);

	if(m_xaxis.m_logarithmic) 
	{
		m_xsl.push_back(sci::log10(xs));
		m_xpluserrsl.push_back(sci::log10(m_xpluserrs.back()));
		m_xminuserrsl.push_back(sci::log10(m_xminuserrs.back()));
	}
	else 
	{
		m_xsl.resize(m_xsl.size()+1);
		m_xpluserrsl.resize(m_xpluserrsl.size()+1);
		m_xminuserrsl.resize(m_xminuserrsl.size()+1);
	}
	if(m_yaxis.m_logarithmic) 
	{
		m_ysl.push_back(sci::log10(ys));
		m_ypluserrsl.push_back(sci::log10(m_ypluserrs.back()));
		m_yminuserrsl.push_back(sci::log10(m_yminuserrs.back()));
	}
	else 
	{
		m_ysl.resize(m_xsl.size()+1);
		m_ypluserrsl.resize(m_ypluserrsl.size()+1);
		m_yminuserrsl.resize(m_yminuserrsl.size()+1);
	}

	calculateautolimits();
	m_haschanged=true;
}

void splot2d::adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const splotsizescale &sizescale, const std::vector<double> &xposerrs, const std::vector<double> &xnegerrs, const std::vector<double> &yposerrs, const std::vector<double> &ynegerrs, wxColour pointcolour, std::string pointsymbol, wxColour linecolour, int linewidth, wxColour xerrcolour, wxColour yerrcolour, int xerrwidth, int yerrwidth)
{
	//checks
	if(xs.size()!=ys.size()) return;
	if(xs.size()!=zs.size()) return;
	if(xposerrs.size()!=0 && xposerrs.size()!=xs.size())return;
	if(xposerrs.size()!=0 && xnegerrs.size()!=xs.size())return;
	if(yposerrs.size()!=0 && yposerrs.size()!=xs.size())return;
	if(ynegerrs.size()!=0 && ynegerrs.size()!=xs.size())return;

	m_xs.push_back(xs);
	m_ys.push_back(ys);
	m_colunstructzs.resize(m_colunstructzs.size()+1);
	m_structzs.resize(m_structzs.size()+1);
	m_sizeunstructzs.push_back(zs);
	if(xposerrs.size()>0 && xnegerrs.size()>0)
	{
		m_xpluserrs.push_back(xs+xposerrs);
		m_xminuserrs.push_back(xs-xnegerrs);
	}
	else if(xposerrs.size()>0)
	{
		m_xpluserrs.push_back(xs+xposerrs);
		m_xminuserrs.push_back(xs);
	}
	else if(xnegerrs.size()>0)
	{
		m_xpluserrs.push_back(xs);
		m_xminuserrs.push_back(xs-xnegerrs);
	}
	else
	{
		m_xpluserrs.push_back(std::vector<double>(0));
		m_xminuserrs.push_back(std::vector<double>(0));
	}
	if(yposerrs.size()>0 && ynegerrs.size()>0)
	{
		m_ypluserrs.push_back(ys+yposerrs);
		m_yminuserrs.push_back(ys-ynegerrs);
	}
	else if(yposerrs.size()>0)
	{
		m_ypluserrs.push_back(ys+yposerrs);
		m_yminuserrs.push_back(ys);
	}
	else if(ynegerrs.size()>0)
	{
		m_ypluserrs.push_back(ys);
		m_yminuserrs.push_back(ys-ynegerrs);
	}
	else
	{
		m_ypluserrs.push_back(std::vector<double>(0));
		m_yminuserrs.push_back(std::vector<double>(0));
	}

	adddatasetproperties(pointcolour,1.0,pointsymbol,pl_SYMBOL,wxT("plotsymbols"),linecolour,linewidth,splotcolourscale(),sizescale,xerrcolour,yerrcolour,xerrwidth,yerrwidth,0,false,false,0,std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN(),0.0);

	if(m_xaxis.m_logarithmic) 
	{
		m_xsl.push_back(sci::log10(xs));
		m_xpluserrsl.push_back(sci::log10(m_xpluserrs.back()));
		m_xminuserrsl.push_back(sci::log10(m_xminuserrs.back()));
	}
	else 
	{
		m_xsl.resize(m_xsl.size()+1);
		m_xpluserrsl.resize(m_xpluserrsl.size()+1);
		m_xminuserrsl.resize(m_xminuserrsl.size()+1);
	}
	if(m_yaxis.m_logarithmic) 
	{
		m_ysl.push_back(sci::log10(ys));
		m_ypluserrsl.push_back(sci::log10(m_ypluserrs.back()));
		m_yminuserrsl.push_back(sci::log10(m_yminuserrs.back()));
	}
	else 
	{
		m_ysl.resize(m_xsl.size()+1);
		m_ypluserrsl.resize(m_ypluserrsl.size()+1);
		m_yminuserrsl.resize(m_yminuserrsl.size()+1);
	}

	calculateautolimits();
	m_haschanged=true;
}

void splot2d::adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const splotsizescale &sizescale, const splotcolourscale &colourscale, unsigned int ncolourlevels, const std::vector<double> &xposerrs, const std::vector<double> &xnegerrs, const std::vector<double> &yposerrs, const std::vector<double> &ynegerrs, std::string pointsymbol, wxColour linecolour, int linewidth, wxColour xerrcolour, wxColour yerrcolour, int xerrwidth, int yerrwidth)
{
	//checks
	if(xs.size()!=ys.size()) return;
	if(xs.size()!=zs.size()) return;
	if(xposerrs.size()!=0 && xposerrs.size()!=xs.size())return;
	if(xposerrs.size()!=0 && xnegerrs.size()!=xs.size())return;
	if(yposerrs.size()!=0 && yposerrs.size()!=xs.size())return;
	if(ynegerrs.size()!=0 && ynegerrs.size()!=xs.size())return;

	m_xs.push_back(xs);
	m_ys.push_back(ys);
	m_colunstructzs.resize(m_colunstructzs.size()+1);
	m_colunstructzs.back().resize(1); //just add one point to colunstructszs so we know it's colourscaled, but use the sizeunstructzs to hold the data
	m_structzs.resize(m_structzs.size()+1);
	m_sizeunstructzs.push_back(zs);
	if(xposerrs.size()>0 && xnegerrs.size()>0)
	{
		m_xpluserrs.push_back(xs+xposerrs);
		m_xminuserrs.push_back(xs-xnegerrs);
	}
	else if(xposerrs.size()>0)
	{
		m_xpluserrs.push_back(xs+xposerrs);
		m_xminuserrs.push_back(xs);
	}
	else if(xnegerrs.size()>0)
	{
		m_xpluserrs.push_back(xs);
		m_xminuserrs.push_back(xs-xnegerrs);
	}
	else
	{
		m_xpluserrs.push_back(std::vector<double>(0));
		m_xminuserrs.push_back(std::vector<double>(0));
	}
	if(yposerrs.size()>0 && ynegerrs.size()>0)
	{
		m_ypluserrs.push_back(ys+yposerrs);
		m_yminuserrs.push_back(ys-ynegerrs);
	}
	else if(yposerrs.size()>0)
	{
		m_ypluserrs.push_back(ys+yposerrs);
		m_yminuserrs.push_back(ys);
	}
	else if(ynegerrs.size()>0)
	{
		m_ypluserrs.push_back(ys);
		m_yminuserrs.push_back(ys-ynegerrs);
	}
	else
	{
		m_ypluserrs.push_back(std::vector<double>(0));
		m_yminuserrs.push_back(std::vector<double>(0));
	}

	adddatasetproperties(wxColour(0,0,0),1.0,pointsymbol,pl_SYMBOL,wxT("plotsymbols"),linecolour,linewidth,colourscale,sizescale,xerrcolour,yerrcolour,xerrwidth,yerrwidth,ncolourlevels,false,false,0,std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN(),0.0);

	if(m_xaxis.m_logarithmic) 
	{
		m_xsl.push_back(sci::log10(xs));
		m_xpluserrsl.push_back(sci::log10(m_xpluserrs.back()));
		m_xminuserrsl.push_back(sci::log10(m_xminuserrs.back()));
	}
	else 
	{
		m_xsl.resize(m_xsl.size()+1);
		m_xpluserrsl.resize(m_xpluserrsl.size()+1);
		m_xminuserrsl.resize(m_xminuserrsl.size()+1);
	}
	if(m_yaxis.m_logarithmic) 
	{
		m_ysl.push_back(sci::log10(ys));
		m_ypluserrsl.push_back(sci::log10(m_ypluserrs.back()));
		m_yminuserrsl.push_back(sci::log10(m_yminuserrs.back()));
	}
	else 
	{
		m_ysl.resize(m_xsl.size()+1);
		m_ypluserrsl.resize(m_ypluserrsl.size()+1);
		m_yminuserrsl.resize(m_yminuserrsl.size()+1);
	}

	calculateautolimits();
	m_haschanged=true;
}

void splot2d::adddata(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector< std::vector <double> > &zs, const splotcolourscale &colourscale, unsigned int ncolourlevels, bool filloffscaletop, bool filloffscalebottom, wxColour contourcolour, int contourwidth, unsigned int ncontourlevels, double mincontour, double maxcontour, double contourlabelssize)
{
	//checks
	if(xs.size()!=zs.size()) return;
	for(size_t i=0; i<zs.size(); ++i) 
	{
		if(zs[i].size()!=ys.size()) return;
	}

	m_xs.push_back(xs);
	m_ys.push_back(ys);
	m_structzs.push_back(zs);
	m_colunstructzs.resize(m_colunstructzs.size()+1);
	m_sizeunstructzs.resize(m_sizeunstructzs.size()+1);
	m_xpluserrs.resize(m_xpluserrs.size()+1);
	m_xminuserrs.resize(m_xminuserrs.size()+1);
	m_ypluserrs.resize(m_ypluserrs.size()+1);
	m_yminuserrs.resize(m_yminuserrs.size()+1);
	m_xpluserrsl.resize(m_xpluserrsl.size()+1);
	m_xminuserrsl.resize(m_xminuserrsl.size()+1);
	m_ypluserrsl.resize(m_ypluserrsl.size()+1);
	m_yminuserrsl.resize(m_yminuserrsl.size()+1);
	if(m_xaxis.m_logarithmic)m_xsl.push_back(sci::log10(xs));
	else m_xsl.resize(m_xsl.size()+1);
	if(m_yaxis.m_logarithmic)m_ysl.push_back(sci::log10(ys));
	else m_ysl.resize(m_ysl.size()+1);

	adddatasetproperties(wxColour(0,0,0),1.0,"A",pl_SYMBOL,wxT("plotsymbols"),contourcolour,contourwidth,colourscale,splotsizescale(),wxColour(0,0,0),wxColour(0,0,0),1,1,ncolourlevels+1,filloffscaletop, filloffscalebottom,ncontourlevels,mincontour,maxcontour,contourlabelssize);

	calculateautolimits();
	m_haschanged=true;
}

void splot::adddatasetproperties(wxColour pointcolour, double pointsize, std::string pointsymbol, uint32_t pointstyle, wxString pointfont, wxColour linecolour, int linewidth, const splotcolourscale &colourscale, const splotsizescale &sizescale, unsigned int ncolourlevels, bool filloffscaletop, bool filloffscalebottom, unsigned int ncontourlevels, double mincontour, double maxcontour, double contourlabelssize)
{
	m_pointcolour.push_back(pointcolour);
	m_pointsize.push_back(pointsize);
	m_pointchar.push_back(wxString(pointsymbol.c_str(),wxConvUTF8));
	m_pointfont.push_back(pointfont);
	m_pointfci.push_back(pointstyle);
	m_linecolour.push_back(linecolour);
	m_linethickness.push_back(linewidth);
	m_colourscale.push_back(colourscale);
	m_sizescale.push_back(sizescale);

	m_filloffscaletop.push_back(filloffscaletop);
	m_filloffscalebottom.push_back(filloffscalebottom);
	m_colourlevels.resize(m_colourlevels.size()+1);
	if(ncolourlevels>0) 
	{
		m_colourlevels.back()=sci::indexvector<double>(std::max((int)ncolourlevels,2));
		m_colourlevels.back()*=(m_colourscale.back().m_top-m_colourscale.back().m_bottom)/m_colourlevels.back().back();
		m_colourlevels.back()+=m_colourscale.back().m_bottom;

	}
	if(filloffscalebottom)m_minstructz.push_back(sci::min<double>(m_structzs));
	else m_minstructz.push_back(std::numeric_limits<double>::quiet_NaN());
	if(filloffscaletop)m_maxstructz.push_back(sci::max<double>(m_structzs));
	else m_maxstructz.push_back(std::numeric_limits<double>::quiet_NaN());

	
	m_contourlevels.resize(m_contourlevels.size()+1);
	if(ncontourlevels>0) 
	{
		//set contours to min/max z if they are currently nans
		if(maxcontour!=maxcontour)maxcontour=m_maxstructz.back();
		if(mincontour!=mincontour)mincontour=m_minstructz.back();
		m_contourlevels.back()=sci::indexvector<double>(ncontourlevels);
		m_contourlevels.back()*=(maxcontour-mincontour)/m_colourlevels.back().back();
		m_contourlevels.back()+=mincontour;
	}
	m_contourlabelssize.push_back(contourlabelssize);
}

void splot2d::adddatasetproperties(wxColour pointcolour, double pointsize, std::string pointsymbol, uint32_t pointstyle, wxString pointfont, wxColour linecolour, int linewidth, const splotcolourscale &colourscale, const splotsizescale &sizescale, wxColour xerrcolour, wxColour yerrcolour, int xerrwidth, int yerrwidth, unsigned int ncolourlevels, bool filloffscaletop, bool filloffscalebottom, unsigned int ncontourlevels, double mincontour, double maxcontour, double contourlabelssize)
{
	m_xerrcolour.push_back(xerrcolour);
	m_yerrcolour.push_back(yerrcolour);
	m_xerrthickness.push_back(xerrwidth);
	m_yerrthickness.push_back(yerrwidth);
	splot::adddatasetproperties(pointcolour,pointsize,pointsymbol,pointstyle,pointfont,linecolour,linewidth,colourscale,sizescale,ncolourlevels,filloffscaletop,filloffscalebottom,ncontourlevels,mincontour,maxcontour,contourlabelssize);
}

void splot2d::calculateautolimits()
{
	if(m_xautolimits)
	{
		//check there is some data to search for
		if(m_xs.size()==0)
		{
			m_xaxis.m_min=std::numeric_limits<double>::quiet_NaN();
			m_xaxis.m_max=std::numeric_limits<double>::quiet_NaN();
			m_yaxis.m_min=std::numeric_limits<double>::quiet_NaN();
			m_yaxis.m_max=std::numeric_limits<double>::quiet_NaN();
			return;
		}
		if(m_xaxis.m_logarithmic)
		{
			//we find the smallest positive value
			m_xaxis.m_min=std::min(sci::mingtlimit(m_xs[0],0.0),sci::mingtlimit(m_xminuserrs,0.0));
			m_xaxis.m_max=std::max(sci::max<double>(m_xs[0]),sci::max<double>(m_xpluserrs));
			for(size_t i=0; i<m_xs.size(); ++i)
			{
				//we find the smallest positive value
				double nextmin=std::min(sci::mingtlimit(m_xs[i],0.0),sci::mingtlimit(m_xminuserrs,0.0));
				double nextmax=std::max(sci::max<double>(m_xs[i]),sci::max<double>(m_xpluserrs));
				if(nextmin<=m_xaxis.m_min)m_xaxis.m_min=nextmin/(m_structzs[i].size()>0?1.0:2.0);
				if(nextmax>=m_xaxis.m_max)m_xaxis.m_max=nextmax*(m_structzs[i].size()>0?1.0:2.0);
			}
			//check we have some positive numbers or use defaults of 1 and 10
			if(m_xaxis.m_max<=0.0)
			{
				m_xaxis.m_min=1.0;
				m_xaxis.m_max=10.0;
			}
		}
		else
		{
			m_xaxis.m_min=std::min(sci::min<double>(m_xs[0]),sci::min<double>(m_xminuserrs));
			m_xaxis.m_max=std::max(sci::max<double>(m_xs[0]),sci::max<double>(m_xpluserrs));
			for(size_t i=0; i<m_xs.size(); ++i)
			{
				double nextmin=std::min(sci::min<double>(m_xs[i]),sci::min<double>(m_xminuserrs));
				double nextmax=std::max(sci::max<double>(m_xs[i]),sci::max<double>(m_xpluserrs));
				double xrange=std::max(m_xaxis.m_max,nextmax)-std::min(m_xaxis.m_min,nextmin);
				if(nextmin<=m_xaxis.m_min)m_xaxis.m_min=nextmin-(m_structzs[i].size()>0?0.0:xrange/10.0);
				if(nextmax>=m_xaxis.m_max)m_xaxis.m_max=nextmax+(m_structzs[i].size()>0?0.0:xrange/10.0);
			}
		}
		if(!m_yautointersect)
		{
			m_xaxis.m_min=std::min(m_xaxis.m_min,m_yaxis.m_intersect);
			m_xaxis.m_max=std::max(m_xaxis.m_max,m_yaxis.m_intersect);
		}
	}
	if(m_yautolimits)
	{
		if(m_yaxis.m_logarithmic)
		{
			//we find the smallest positive value
			m_yaxis.m_min=std::min(sci::mingtlimit(m_ys[0],0.0),sci::mingtlimit(m_yminuserrs,0.0));
			m_yaxis.m_max=std::max(sci::max<double>(m_ys[0]),sci::max<double>(m_ypluserrs));
			for(size_t i=0; i<m_ys.size(); ++i)
			{
				//we find the smallest positive value
				double neytmin=std::min(sci::mingtlimit(m_ys[i],0.0),sci::mingtlimit(m_yminuserrs,0.0));
				double neytmax=std::max(sci::max<double>(m_ys[i]),sci::max<double>(m_ypluserrs));
				if(neytmin<=m_yaxis.m_min)m_yaxis.m_min=neytmin/(m_structzs[i].size()>0?1.0:2.0);
				if(neytmax>=m_yaxis.m_max)m_yaxis.m_max=neytmax*(m_structzs[i].size()>0?1.0:2.0);
			}
			//check we have some positive numbers or use defaults of 1 and 10
			if(m_yaxis.m_max<=0.0)
			{
				m_yaxis.m_min=1.0;
				m_yaxis.m_max=10.0;
			}
		}
		else
		{
			m_yaxis.m_min=std::min(sci::min<double>(m_ys[0]),sci::min<double>(m_yminuserrs));
			m_yaxis.m_max=std::max(sci::max<double>(m_ys[0]),sci::max<double>(m_ypluserrs));
			for(size_t i=0; i<m_ys.size(); ++i)
			{
				double nextmin=std::min(sci::min<double>(m_ys[i]),sci::min<double>(m_yminuserrs));
				double nextmax=std::max(sci::max<double>(m_ys[i]),sci::max<double>(m_ypluserrs));
				double yrange=std::max(m_yaxis.m_max,nextmax)-std::min(m_yaxis.m_min,nextmin);
				if(nextmin<=m_yaxis.m_min)m_yaxis.m_min=nextmin-(m_structzs[i].size()>0?0.0:yrange/10.0);
				if(nextmax>=m_yaxis.m_max)m_yaxis.m_max=nextmax+(m_structzs[i].size()>0?0.0:yrange/10.0);
			}
		}
		if(!m_xautointersect)
		{
			m_yaxis.m_min=std::min(m_yaxis.m_min,m_xaxis.m_intersect);
			m_yaxis.m_max=std::max(m_yaxis.m_max,m_xaxis.m_intersect);
		}
	}
	if(m_xautointersect) m_xaxis.m_intersect=m_yaxis.m_min;
	if(m_yautointersect) m_yaxis.m_intersect=m_xaxis.m_min;
}

wxString splot::createploptstring(const splotaxis &axis)
{
	wxString opt=wxT("bci");
	if(axis.m_time)opt=opt+wxT("d");
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


//when called with a plstream pl.init() should have already been called
//and the window and driver should have been set so the stream should 
//be ready for sending plot commands
//all plplot specific code in splot2d should be kept here
void splot2d::plot(plstream *pl)
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
	double a;
	pl->col0(0);

	
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

	
	//draw the points, lines, shadings and contours
	for(size_t i=0; i<m_xs.size(); ++i)
	{
		//set to use either logged or not logged x/y data
		double *x=&m_xs[i][0];
		double *y=&m_ys[i][0];
		if(m_xaxis.m_logarithmic) x=&m_xsl[i][0];
		if(m_yaxis.m_logarithmic) y=&m_ysl[i][0];

		//points errs and lines
		if(m_structzs[i].size()==0)
		{
			//lines
			if(m_linethickness[i]>0.0)
			{
				r=m_linecolour[i].Red();
				g=m_linecolour[i].Green();
				b=m_linecolour[i].Blue();
				pl->scol0(0,r,g,b);
				pl->col0(0);
				pl->wid(m_linethickness[i]);
				pl->line(m_xs[i].size(),x,y);
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
				pl->wid(m_xerrthickness[i]);
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
			if(m_ypluserrs[i].size()>0 && m_xminuserrs[i].size()>0)
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
				pl->wid(m_yerrthickness[i]);
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
			if(m_pointsize[i]>0.0)
			{
				pl->sfci(m_pointfci[i]);
				pl->sfontf(m_pointfont[i].mb_str());
				pl->schr(0.0,m_pointsize[i]);
				r=m_pointcolour[i].Red();
				g=m_pointcolour[i].Green();
				b=m_pointcolour[i].Blue();
				pl->scol0(0,r,g,b);
				pl->col0(0);
				if(m_sizeunstructzs[i].size()>0 && m_colunstructzs[i].size()>0)
				{
					//draw points with changing size and colour
					//set the colour scale
					pl->scmap1la(!m_colourscale[i].m_hsl,m_colourscale[i].m_value.size(),&m_colourscale[i].m_value[0],&m_colourscale[i].m_colour1[0],&m_colourscale[i].m_colour2[0],&m_colourscale[i].m_colour3[0],&m_colourscale[i].m_alpha[0],&m_colourscale[i].m_rbgr[0]);
					for(size_t j=0; j<m_xs[i].size(); ++j)
					{
						double colour=std::max(0.0,std::min(1.0,(m_sizeunstructzs[i][j]-m_colourscale[i].m_bottom)/(m_colourscale[i].m_top-m_colourscale[i].m_bottom)));
						pl->col1(colour);
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
					//set the colour scale
					pl->scmap1la(!m_colourscale[i].m_hsl,m_colourscale[i].m_value.size(),&m_colourscale[i].m_value[0],&m_colourscale[i].m_colour1[0],&m_colourscale[i].m_colour2[0],&m_colourscale[i].m_colour3[0],&m_colourscale[i].m_alpha[0],&m_colourscale[i].m_rbgr[0]);
				
					//draw points with changing colour
					for(size_t j=0; j<m_xs[i].size(); ++j)
					{
						double colour=std::max(0.0,std::min(1.0,(m_colunstructzs[i][j]-m_colourscale[i].m_bottom)/(m_colourscale[i].m_top-m_colourscale[i].m_bottom)));
						pl->col1(colour);
						pl->string(1,x+j,y+j,m_pointchar[i].mb_str());
					}

				}
				else
				{
					//draw points with static size and colour
					pl->string(m_xs[i].size(),x,y,m_pointchar[i].mb_str());
				}
			}
		}
		//shading and contours
		else
		{
			//shading first
			if(m_colourlevels[i].size()>0)
			{
				//set the colour scale
				pl->scmap1la(!m_colourscale[i].m_hsl,m_colourscale[i].m_value.size(),&m_colourscale[i].m_value[0],&m_colourscale[i].m_colour1[0],&m_colourscale[i].m_colour2[0],&m_colourscale[i].m_colour3[0],&m_colourscale[i].m_alpha[0],&m_colourscale[i].m_rbgr[0]);
				//create a contourable object for plotting
				splot2dmatrix matrix(&m_structzs[i],&m_xs[i],&m_ys[i],m_transfunc);
				bool rectilinear=matrix.isrectilinear();

				//plot the lower out of bounds if needed, note that if there is no out of bounds plplot just does nothing because minlevel>maxlevel
				if(m_filloffscalebottom[i])pl->shade(matrix,m_xs[i][0],m_xs[i].back(),m_ys[i][0],m_ys[i].back(),m_minstructz[i],m_colourlevels[i][0],1,0.0,1,0,0,0,0,rectilinear,&matrix.m_transformer);
				//plot the rest of the colour scale
				for(size_t j=0; j<m_colourlevels[i].size()-1; ++j)
				{
					//draw the shade - note we use shade not shades because shade has an interface for countourable
					// objects and shades doesn't. see the manual for plshade1 for descriptions. 
					//note sh_col is 0 to use colmap0 and 1 to use colmap1 (we want 1 for continuous mapping)
					double colour=std::max(0.0,std::min(1.0,((m_colourlevels[i][j]+m_colourlevels[i][j+1])/2.0-m_colourscale[i].m_bottom)/(m_colourscale[i].m_top-m_colourscale[i].m_bottom)));
					pl->shade(matrix,m_xs[i][0],m_xs[i].back(),m_ys[i][0],m_ys[i].back(),m_colourlevels[i][j],m_colourlevels[i][j+1],1,colour,1,0,0,0,0,rectilinear,&matrix.m_transformer);
				}
				//plot the upper out of bounds if needed, note that if there is no out of bounds plplot just does nothing because minlevel>maxlevel
				if(m_filloffscaletop[i])pl->shade(matrix,m_xs[i][0],m_xs[i].back(),m_ys[i][0],m_ys[i].back(),m_colourlevels[i].back(),m_maxstructz[i],1,1.0,1,0,0,0,0,rectilinear,&matrix.m_transformer);
			}
			//now contours
			if(m_contourlevels[i].size()>0)
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

				//create a contourable object for plotting
				splot2dmatrix matrix(&m_structzs[i],&m_xs[i],&m_ys[i],m_transfunc);
				bool rectilinear=matrix.isrectilinear();
				//plot the contours
				for(size_t j=0; j<m_contourlevels[i].size(); ++j)
				{
					//draw the contours - note we use shade not shades or cont because shade has an interface for countourable
					// objects and shades and cont don't. see the manual for plshade1 for descriptions. 
					//note sh_col is 0 to use colmap0 and 1 to use colmap1 (we want 0 for contour colour)
					//note we set fill to NULL to not fill the region between contours and for some reason we
					//CANNOT use colour 0 for the contours - it does not draw them in this case
					pl->shade(matrix,m_xs[i][0],m_xs[i].back(),m_ys[i][0],m_ys[i].back(),m_contourlevels[i][j],std::numeric_limits<double>::infinity(),0,0,0,1,m_linethickness[i],0,0,rectilinear,&matrix.m_transformer);
				}

			}
		}
	};

	//draw the axes last otherwise they get overdrawn

	//due to the crap way plplot works the outer box can get tickmarks
	//inside or outside but an axis anywhere else has to have tickmarks
	//both sides - therefore we nedd to con plplot into drawing a box
	//edge in the right location
	//this is still to do

	//the distances from the y axis and the y ticklengths scale with the plotwidth so we want to undo that
	double xp;
	double yp;
	int pagewidth;
	int pageheight;
	int pageoffx;
	int pageoffy;
	pl->gpage(xp,yp,pagewidth,pageheight,pageoffx,pageoffy);
	double scalefactor=(double)pageheight/(double)pagewidth;
	
	
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
	pl->scol0(0,r,g,b);
	pl->col0(0);
	pl->wid(m_xaxis.m_linethickness);
	//set the font for the labels
	pl->sfci(m_xaxis.m_labelfci);
	pl->sfontf(m_xaxis.m_labelfont.mb_str());
	pl->schr(0.0,m_xaxis.m_labelsize);
	//set the tick lengths
	pl->smin(0.0,m_xaxis.m_minorticklength);
	pl->smaj(0.0,m_xaxis.m_majorticklength);
	//create a plplot option string for the axis
	wxString xopt=createploptstring(m_xaxis);
	//draw the x axis
	pl->box(xopt.mb_str(),xmajint,xnsub,"",0.0,0.0);

	//now the y axis
	//set the axis colour
	r=m_yaxis.m_colour.Red();
	g=m_yaxis.m_colour.Green();
	b=m_yaxis.m_colour.Blue();
	pl->scol0(0,r,g,b);
	pl->col0(0);
	pl->wid(m_yaxis.m_linethickness);
	//set the font for the labels
	pl->sfci(m_yaxis.m_labelfci);
	pl->sfontf(m_yaxis.m_labelfont.mb_str());
	pl->schr(0.0,m_yaxis.m_labelsize);
	//set the tick lengths
	pl->smin(0.0,m_yaxis.m_minorticklength*scalefactor);
	pl->smaj(0.0,m_yaxis.m_majorticklength*scalefactor);
	//create a plplot option string for the axis
	wxString yopt=createploptstring(m_yaxis);
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
	pl->scol0(0,r,g,b);
	pl->col0(0);
	if(m_xaxis.m_rotatetitle)pl->mtex("bv",m_xaxis.m_titledistance,0.5,0.5,m_xaxis.m_title.mb_str());
	else pl->mtex("b",m_xaxis.m_titledistance,0.5,0.5,m_xaxis.m_title.mb_str());
	//then the y axis
	pl->sfci(m_yaxis.m_titlefci);
	pl->sfontf(m_yaxis.m_titlefont.mb_str());
	pl->schr(0.0,m_yaxis.m_titlesize);
	r=m_yaxis.m_titlecolour.Red();
	g=m_yaxis.m_titlecolour.Green();
	b=m_yaxis.m_titlecolour.Blue();
	pl->scol0(0,r,g,b);
	pl->col0(0);
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
	if(m_yaxis.m_rotatetitle)pl->mtex("lv",m_yaxis.m_titledistance*scalefactor,0.5,0.5,m_yaxis.m_title.mb_str());
	//if(m_yaxis.m_rotatetitle)pl->mtex("lv",m_yaxis.m_titledistance,0.5,0.5,m_yaxis.m_title.mb_str());
	else pl->mtex("l",m_yaxis.m_titledistance*scalefactor,0.5,0.5,m_yaxis.m_title.mb_str());
	


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


splotwindow::splotwindow(wxWindow *parent, bool fixedsize, wxWindowID winid, const wxPoint &pos, const wxSize &size, long style, const wxString& name)
:wxScrolledWindow(parent,winid,pos,size,style|wxFULL_REPAINT_ON_RESIZE ,name)
{
	m_fixedsize=fixedsize;
	m_plotsupdated=true;
	m_bitmapwidth=size.GetWidth();
	m_bitmapheight=size.GetHeight();
	m_bitmap=new wxBitmap(size.GetWidth(),size.GetWidth());
}

splotwindow::~splotwindow()
{
	delete m_bitmap;
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
			m_bitmap = new wxBitmap(width,height);
			memdc.SelectObject(*m_bitmap);
			//fill the bitmap with white giving a white background for plplot
			//or to show blank if there are no plots
			memdc.FloodFill(0,0,*wxWHITE,wxFLOOD_BORDER);
		}

		//remember the new size, so that on the next refresh we can check if the size has changed
		m_bitmapwidth=width;
		m_bitmapheight=height;

		//set that an update is no longer needed
		m_plotsupdated=false;
		//the plots themselve update their own status in the plot function
		DrawPlots(&memdc,2,false); //2 is the GC which gives antialiased output, 0 is any wxDC and 1 uses AGG and with or without freetype
	}
	else
	{
		//The plots haven't changed so just select the bitmap as it already exists if we haven't resized
		memdc.SelectObject(*m_bitmap);
	}

	//copy the memorydc bitmap to the panel
	pagedc.Blit(0,0,width,height,&memdc,0,0);
	//reselect null bitmap for the memdc
	memdc.SelectObject(wxNullBitmap);

	wxEndBusyCursor();
}

//Write the plot window to an image file. This can be either a bitmap
//or vector graphic image. The file extension defines the type of image saved. 
//Valid extensions are png, jpg, bmp, tif, pcx, xpm, xbm, pnm for bitmaps and
//ps for vector. If the extension does not correspond to any of these 
//strings then png is used, although the given extension will still be used 
//in the filename
bool splotwindow::writetofile(wxString filename)
{
	//get the extension
	wxFileName fullfile=filename;
	wxString extension=fullfile.GetExt().Lower();
	
	bool result;

	if(extension==wxT("ps"))
	{
		//here we redraw the plot like OnPaint but using a postscript DC.
		wxPrintData setupdata;
		setupdata.SetColour(true);
		setupdata.SetFilename(filename);
		setupdata.SetPaperId(wxPAPER_A4);
		setupdata.SetPrintMode(wxPRINT_MODE_FILE);
		setupdata.SetQuality(wxPRINT_QUALITY_MEDIUM);
		wxPostScriptDC psdc(setupdata);
		result=psdc.StartDoc(wxT("Writing ")+filename);
		if(result==false) return result;
		DrawPlots(&psdc,0,false);//0 gives vector output, I think 2 should too but it creates empty postscripts, there is no need to use freetype
		psdc.EndDoc();
	}
	else if(extension==wxT("emf"))
	{
		//here we redraw the plot like OnPaint but using a wxMetafile DC.
		int width;
		int height;
		GetClientSize(&width, &height);
		wxMetafileDC metadc(filename,width,height);
		DrawPlots(&metadc,0,false);//0 gives vector output
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
		//we are outputting as a bitmap type, basically here we want to draw the plot then write the 
		//pixels to a file

		//first refresh the screen because this function can get called before the panels have been painted
		Refresh();
		Update();
		wxSafeYield();

		/*
		//create a client dc, this is like a paintdc used in onpaint()
		//but can be used outside the OnPaint() function;
		//The client DC is linked to the screen
		wxClientDC clientdc(this);

		//create a memory dc and link a bitmap to it that has the width an height of the client area
		wxSize size=clientdc.GetSize();
		wxMemoryDC memdc;
		wxBitmap membitmap(size.GetWidth(),size.GetHeight());
		memdc.SelectObject(membitmap);

		//blit the clientDC to the memoryDC
		memdc.Blit(wxPoint(0,0),size,&clientdc,wxPoint(0,0));

		//The bitmap should now contain a copy of the screen so deselect it from
		//memdc to allow access by other functions
		memdc.SelectObject(wxNullBitmap);
		*/

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

		//write the bitmap to file
		result=this->m_bitmap->SaveFile(filename,type);
	}

	return result;
}

void splotwindow::DrawPlots(wxDC *dc, int backend, bool usefreetype)
{
	int width;
	int height;
	dc->GetSize(&width,&height);

	//no point in messing around if the size is zero
	if(height==0 || width==0) return;

	//check that the PlPlot font file exists. If it doesn't the program
	//closes itself or hangs if we construct a plplotstream so return here.
	if(wxFileExists(wxT("plxtnd5.fnt"))==false)
	{
		//the font file doesn't exist so fill with white as selected above
		dc->FloodFill(0,0,*wxWHITE,wxFLOOD_BORDER);
		wxMessageBox(wxT("Could not find the font file plxtnd5.fnt"),wxT("Error"));
		//copy the currently empty memorydc bitmap to the panel
		//dc.Blit(0,0,width,height,&memdc,0,0);
		//reselect null bitmap for the memdc
		//memdc.SelectObject(wxNullBitmap);
		//wxEndBusyCursor();
		return;
	}
	
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
	pl->sexit(&sploterrorcatcher);
	//set the device to wxwidgets
	pl->sdev("wxwidgets");

	//set which backend to use
	if(backend==0)pl->setopt( "-drvopt", "backend=0" );
	else if(backend==1)
	{
		if(usefreetype==true)pl->setopt( "-drvopt", "freetype=1,backend=1" );
		else pl->setopt( "-drvopt", "freetype=0,backend=1" );
	}
	else if(backend==2)pl->setopt( "-drvopt", "backend=2" );

	//set up the page to the size of the bitmap
	pl->spage(0.0,0.0,width,height,0,0);
	//initialize the stream
	pl->init();
	//move to the zeroth page
	pl->adv(0);

	//pass the memorydc to the stream for DC and GC or the wxImage for AGG
	if(backend==0 || backend==2)pl->cmd( PLESC_DEVINIT, dc );
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
		m_plots[i]->plot(pl);
	}
	//draw each legend in turn
	for(size_t i=0; i<m_legends.size(); ++i)
	{
		//set the area covered by this legend
		pl->vpor(m_legendxloc[i],m_legendxloc[i]+m_legendwidth[i],m_legendyloc[i],m_legendyloc[i]+m_legendheight[i]);

		//draw the legend
		m_legends[i]->plot(pl);
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

splotframe::splotframe(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
:wxFrame(parent,id,title,pos,size,style,name)
{
	m_plotwind=new splotwindow(this);
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

void splotlegend::addentry(std::string text, double textoffset, double textsize, const std::string &textfont, uint32_t textstyle, double textspacing, wxColour textcolour, wxColour pointcolour, double pointsize, std::string pointsymbol, wxColour linecolour, int linewidth)
{
	adddatasetproperties(text,textoffset,textsize,textfont,textstyle,textspacing,textcolour,pointcolour,pointsize,pointsymbol,pl_SYMBOL,wxT("plotsymbols"),linecolour,linewidth,splotcolourscale(),splotsizescale(),0,1,0.0);
}
void splotlegend::addentry(std::string text, const splotcolourscale &colourscale, double headingoffset, double textoffset, double textsize, const std::string &textfont, uint32_t textstyle, double textspacing,wxColour textcolour, unsigned int ncolourlevels, size_t nlines)
{
	adddatasetproperties(text,textoffset,textsize,textfont,textstyle,textspacing,textcolour,wxColour(0,0,0),0.0,"",pl_SYMBOL,wxT("plotsymbols"),wxColour(0,0,0),0,colourscale,splotsizescale(),ncolourlevels,nlines,headingoffset);
}
void splotlegend::addentry(std::string text, const splotsizescale &sizescale, double headingoffset, double textoffset, double textsize, const std::string &textfont, uint32_t textstyle, double textspacing,wxColour textcolour, wxColour pointcolour, std::string pointsymbol, size_t nlines)
{
	adddatasetproperties(text,textoffset,textsize,textfont,textstyle,textspacing,textcolour,pointcolour,0.0,pointsymbol,pl_SYMBOL,wxT("plotsymbols"),wxColour(0,0,0),0,splotcolourscale(),sizescale,0,nlines,headingoffset);
}

void splotlegend::adddatasetproperties(std::string text, double textoffset, double textsize, const std::string &textfont, uint32_t textstyle, double textspacing,wxColour textcolour, wxColour pointcolour, double pointsize, std::string pointsymbol, uint32_t pointstyle, wxString pointfont, wxColour linecolour, int linewidth, const splotcolourscale &colourscale, const splotsizescale &sizescale, unsigned int ncolourlevels, size_t nlines, double headingoffset)
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
	m_colourscale.push_back(colourscale);
	m_sizescale.push_back(sizescale);

	m_filloffscaletop.push_back(false);
	m_filloffscalebottom.push_back(false);
	m_colourlevels.resize(m_colourlevels.size()+1);
	if(ncolourlevels>0) 
	{
		m_colourlevels.back()=sci::indexvector<double>(std::max((int)ncolourlevels,2));
		m_colourlevels.back()*=(m_colourscale.back().m_top-m_colourscale.back().m_bottom)/m_colourlevels.back().back();
		m_colourlevels.back()+=m_colourscale.back().m_bottom;

	}
	m_minstructz.push_back(std::numeric_limits<double>::quiet_NaN());
	m_maxstructz.push_back(std::numeric_limits<double>::quiet_NaN());

	
	m_contourlevels.resize(m_contourlevels.size()+1);
	m_contourlabelssize.push_back(0.0);

	m_nlines.push_back(nlines);
}

void splotlegend::plot(plstream *pl)
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
	double a;
	pl->col0(0);

	
	//set the limits of the plot area in terms of plot units
	//note we have to log the limits for a log axis
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

		if(m_sizescale[i].m_value.size()>1 || m_colourscale[i].m_value.size()>1)
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
			pl->wid(m_linethickness[i]);
			pl->schr(0.0,m_textsize[i]);

			double x[2];
			x[0]=m_textoffset[i]*0.1;
			x[1]=m_textoffset[i]*0.9;
			double y[2];
			y[0]=1.0-position*scaledcharheightworld;
			y[1]=1.0-position*scaledcharheightworld;
			pl->line(2,x,y);
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
			pl->string(1,&x,&y,m_pointchar[i].mb_str(wxConvUTF8));
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
		if(m_colourscale[i].m_value.size()>1)
		{
			//step on past the heading
			position+=positionstep+0.5*m_textsize[i]*1.6;
			std::vector<double> x=sci::indexvector<double>(2);
			std::vector<double> y=m_colourscale[i].m_value*(m_colourscale[i].m_top-m_colourscale[i].m_bottom)+m_colourscale[i].m_bottom;
			std::vector<std::vector<double> > z(2);
			z[0]=m_colourscale[i].m_value*(m_colourscale[i].m_top-m_colourscale[i].m_bottom)+m_colourscale[i].m_bottom;
			z[1]=m_colourscale[i].m_value*(m_colourscale[i].m_top-m_colourscale[i].m_bottom)+m_colourscale[i].m_bottom;
			pl->schr(0.0,m_textsize[i]);
			//reset the viewport up where we want it
			double xmin=vporxminnorm+0.1*m_textoffset[i]*(vporxmaxnorm-vporxminnorm);
			double xmax=vporxminnorm+0.9*m_textoffset[i]*(vporxmaxnorm-vporxminnorm);
			pl->vpor(vporxminnorm+0.1*m_textoffset[i]*(vporxmaxnorm-vporxminnorm),vporxminnorm+0.9*m_textoffset[i]*(vporxmaxnorm-vporxminnorm),vporymaxnorm-(position+m_nlines[i])*scaledcharheightworld*(vporymaxnorm-vporyminnorm),vporymaxnorm-(position-0.5)*scaledcharheightworld*(vporymaxnorm-vporyminnorm));
			pl->wind(0.0,1.0,m_colourscale[i].m_bottom,m_colourscale[i].m_top);
			//set the colour scale
			pl->scmap1la(!m_colourscale[i].m_hsl,m_colourscale[i].m_value.size(),&m_colourscale[i].m_value[0],&m_colourscale[i].m_colour1[0],&m_colourscale[i].m_colour2[0],&m_colourscale[i].m_colour3[0],&m_colourscale[i].m_alpha[0],&m_colourscale[i].m_rbgr[0]);
			//create a contourable object for plotting
			splot2dmatrix matrix(&z,&x,&y,splot_REGULAR);
			bool rectilinear=matrix.isrectilinear();

			//draw the colour scale
			for(size_t j=0; j<m_colourlevels[i].size()-1; ++j)
			{
				//draw the shade - note we use shade not shades because shade has an interface for countourable
				// objects and shades doesn't. see the manual for plshade1 for descriptions. 
				//note sh_col is 0 to use colmap0 and 1 to use colmap1 (we want 1 for continuous mapping)
				double colour=std::max(0.0,std::min(1.0,((m_colourlevels[i][j]+m_colourlevels[i][j+1])/2.0-m_colourscale[i].m_bottom)/(m_colourscale[i].m_top-m_colourscale[i].m_bottom)));
				pl->shade(matrix,x[0],x.back(),y[0],y.back(),m_colourlevels[i][j],m_colourlevels[i][j+1],1,colour,1,0,0,0,0,rectilinear,&matrix.m_transformer);
			}
			
			//draw a bounding box
			pl->sfontf(m_textfont[i].mb_str(wxConvUTF8));
			pl->sfci(m_textfci[i]);
			pl->schr(0.0,m_textsize[i]);
			pl->wid(1);
			pl->scol0(1,0,0,0);
			pl->col0(1);
			pl->box("bc",0.0,0,"ctmiv",0.0,1);
			pl->box("",0.0,0,"b",0.0,0);

			//reset the viewport
			pl->vpor(vporxminnorm,vporxmaxnorm,vporyminnorm,vporymaxnorm);
			pl->wind(0.0,1.0,0.0,1.0);

			positionstep=(double)m_nlines[i]+0.5;
		}
	}
}