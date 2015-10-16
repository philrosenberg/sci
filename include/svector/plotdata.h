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
	void setupLineStyle( plstream *pl, PLINT colourIndex, double scale ) const;
	void resetLineStyle( plstream *pl, PLINT colourIndex ) const;
	static void parseLineStyle( const std::string &pattern, std::vector<PLINT> &marks, std::vector<PLINT> &spaces );
private:
	double m_width;
	rgbcolour m_colour;
	std::vector<PLINT> m_marks;
	std::vector<PLINT> m_spaces;
};

class Symbol
{
public:
	Symbol ( std::string symbol = sym::filledCircle, double size = 4.0, rgbcolour colour=rgbcolour( 0, 0, 0, 1.0 ) );
	std::string getSymbol() const;
	double getSize() const;
	rgbcolour getColour() const;
	PLUNICODE getFci() const;
	void setupSymbol( plstream *pl, PLINT colourIndex, double scale ) const;
private:
	std::string m_symbol;
	double m_size;
	rgbcolour m_colour;
	PLUNICODE m_fci;
};

class VaryingSymbol
{
public:
	VaryingSymbol ( std::string symbol = sym::filledCircle );
	std::string getSymbol() const;
	PLUNICODE getFci() const;
	virtual void setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool useNormalisedScale, double scale ) const = 0;
	virtual bool isLogScaled() const = 0;
private:
	std::string m_symbol;
	PLUNICODE m_fci;
};

class ColourVaryingSymbol : public VaryingSymbol
{
public:
	ColourVaryingSymbol ( std::string symbol = sym::filledCircle, double size = 4.0, splotcolourscale colourScale = splotcolourscale() );
	void setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool useNormalisedScale, double scale ) const;
	bool isLogScaled() const;
private:
	double m_size;
	splotcolourscale m_colourScale;
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
	virtual void plotData( plstream *pl, bool xLog, bool yLog ) const = 0;
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

class PlotData2dLinear : public PlotData1d
{
public:
	PlotData2dLinear( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.05 );
protected:
	std::vector<double> m_zData;
	std::vector<double> m_zDataLogged;
	std::vector<double> m_zDataNormalised;
	std::vector<double> m_zDataLoggedNormalised;
};

class PlotData2dStructured : public PlotData1d
{
	PlotData2dStructured( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<std::vector<double>> &zs, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.0 );
	std::vector<std::vector<double>> m_zData;
	std::vector<std::vector<double>> m_zDataLogged;
	std::vector<std::vector<double>> m_zDataNormalised;
	std::vector<std::vector<double>> m_zDataLoggedNormalised;
};

class LineData : public PlotData1d
{
public:
	LineData( const std::vector<double> &x, const std::vector<double> &y, const LineStyle &lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr );
private:
	LineStyle m_lineStyle;
	void plotData( plstream *pl, bool xLog, bool yLog ) const;
};

class PointData : public PlotData1d
{
public:
	PointData( const std::vector<double> &x, const std::vector<double> &y, const Symbol &symbol, std::shared_ptr<splotTransformer> transformer = nullptr );
private:
	Symbol m_symbol;
	void plotData( plstream *pl, bool xLog, bool yLog ) const;
};

class PointDataColourVarying : public PlotData2dLinear
{
public:
	PointDataColourVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const ColourVaryingSymbol &symbol, bool autoscaleColour = false, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.05 );
	void plotData( plstream *pl, bool xLog, bool yLog ) const;
private:
	ColourVaryingSymbol m_symbol;
	bool m_autoscaleColour;
	double m_zMin;
	double m_zRange;
};



#endif