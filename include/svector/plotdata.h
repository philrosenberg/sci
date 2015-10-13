#ifndef SVECTOR_PLOTDATA_H
#define SVECTOR_PLOTDATA_H

#include<vector>
#include<string>
#include<memory>
#include"splot.h"

class LineStyle
{
public:
	LineStyle( double width = 1.0, const rgbcolour &colour = rgbcolour( 0.0, 0.0, 0.0, 1.0 ), const std::vector<PLINT> &marks = std::vector<PLINT> ( 0 ), const std::vector<PLINT> &spaces = std::vector<PLINT> ( 0 ) );
	LineStyle( double width, const rgbcolour &colour, std::string pattern );
	double getWidth();
	void getPattern( std::vector<PLINT> &marks, std::vector<PLINT> &spaces );
	rgbcolour getColour();
	void setupLineStyle( plstream *pl, PLINT colourIndex, double scale );
	void resetLineStyle( plstream *pl, PLINT colourIndex );
	static void parseLineStyle( const std::string &pattern, std::vector<PLINT> &marks, std::vector<PLINT> &spaces );
private:
	double m_width;
	rgbcolour m_colour;
	std::vector<PLINT> m_marks;
	std::vector<PLINT> m_spaces;
};


class DrawableItem
{
public:
	DrawableItem ( std::shared_ptr<splotTransformer> transformer );
	void draw( plstream *pl, bool xLog, bool yLog );
	void setScale( double scale );
	//virtual void plot( splot3d *targetPlot ) = 0;
	virtual void getLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const = 0;
	virtual void getLogLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const  = 0;
protected:
	std::shared_ptr<splotTransformer> m_transformer;
	double m_scale;
private:
	virtual void plotData( plstream *pl, bool xLog, bool yLog ) = 0;
};

class PlotData1d : public DrawableItem
{
public:
	void getLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const;
	void getLogLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const;
protected:
	PlotData1d( const std::vector<double> &xs, const std::vector<double> &ys, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.05 );
	std::vector<double> m_xData;
	std::vector<double> m_yData;
	std::vector<double> m_xDataLogged;
	std::vector<double> m_yDataLogged;
private:
	double m_padLimitsAmount;
	static void getLimits( const std::vector<double> xs, const std::vector<double> &ys, double &xMin, double &xMax, double &yMin, double &yMax, double padAmount );
	mutable double m_xMin;
	mutable double m_xMax;
	mutable double m_yMin;
	mutable double m_yMax;
	mutable double m_xMinLogged;
	mutable double m_xMaxLogged;
	mutable double m_yMinLogged;
	mutable double m_yMaxLogged;
	mutable bool m_calculatedLimits;
	mutable bool m_calculatedLogLimits;
};

class PlotData2dStructured : public PlotData1d
{
	PlotData2dStructured( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<std::vector<double>> &zs, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.0 );
	std::vector<std::vector<double>> m_zData;
	std::vector<std::vector<double>> m_zDataLogged;
};

class LineData : public PlotData1d
{
public:
	LineData( const std::vector<double> &x, const std::vector<double> &y, const LineStyle &lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr );
private:
	LineStyle m_lineStyle;
	void plotData( plstream *pl, bool xLog, bool yLog );
};



#endif