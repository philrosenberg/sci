#include<wx/wx.h>
#include<wx/filename.h>
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
