#ifndef SVECTOR_PLOTDATA_H
#define SVECTOR_PLOTDATA_H

#include<vector>
#include<string>
#include<memory>


class LineStyle
{
public:
	LineStyle( double width = 1.0, const rgbcolour &colour = rgbcolour( 0.0, 0.0, 0.0, 1.0 ), const std::vector<PLINT> &marks = std::vector<PLINT> ( 0 ), const std::vector<PLINT> &spaces = std::vector<PLINT> ( 0 ) );
	LineStyle( double width, const rgbcolour &colour, std::string pattern );
	double getWidth();
	void getPattern( std::vector<PLINT> &marks, std::vector<PLINT> &spaces );
	rgbcolour getColour() const;
	void setupLineStyle( plstream *pl, PLINT colourIndex, double scale ) const;
	void resetLineStyle( plstream *pl, PLINT colourIndex ) const;
	static void parseLineStyle( const std::string &pattern, std::vector<PLINT> &marks, std::vector<PLINT> &spaces );
private:
	double m_width;
	rgbcolour m_colour;
	std::vector<PLINT> m_marks;
	std::vector<PLINT> m_spaces;
};

const LineStyle noLine(0.0);

class SymbolBase
{
public:
	SymbolBase( std::string symbol, PLUNICODE fci );
	std::string getSymbol() const;
	PLUNICODE getFci() const;
private:
	std::string m_symbol;
	PLUNICODE m_fci;
};

class Symbol : public SymbolBase
{
public:
	Symbol ( std::string symbol = sym::filledCircle, double size = 4.0, rgbcolour colour=rgbcolour( 0, 0, 0, 1.0 ) );
	double getSize() const;
	rgbcolour getColour() const;
	void setupSymbol( plstream *pl, PLINT colourIndex, double scale ) const;
private:
	double m_size;
	rgbcolour m_colour;
};

class VaryingSymbol : public SymbolBase
{
public:
	VaryingSymbol ( std::string symbol = sym::filledCircle );
	virtual void setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool useNormalisedScale, double scale ) const = 0;
	virtual bool isLogScaled() const = 0;
private:
};

class VaryingSymbolTwoParam : public SymbolBase
{
public:
	VaryingSymbolTwoParam ( std::string symbol = sym::filledCircle );
	virtual void setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool useNormalisedScale1, bool useNormalisedScale2, double scale ) const = 0;
	virtual bool isLogScaled1() const = 0;
	virtual bool isLogScaled2() const = 0;

private:
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

class SizeVaryingSymbol : public VaryingSymbol
{
public:
	SizeVaryingSymbol ( std::string symbol = sym::filledCircle, rgbcolour colour = rgbcolour( 0.0, 0.0, 0.0 ), splotsizescale sizeScale = splotsizescale() );
	void setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool useNormalisedScale, double scale ) const;
	bool isLogScaled() const;
	double getSize(double parameter, bool useNormalisedScale) const;
private:
	rgbcolour m_colour;
	splotsizescale m_sizeScale;
};

class ColourAndSizeVaryingSymbol : public SymbolBase
{
public:
	ColourAndSizeVaryingSymbol ( std::string symbol = sym::filledCircle, splotcolourscale colourScale = splotcolourscale(), splotsizescale sizeScale = splotsizescale() );
	void setupSymbol( plstream *pl, PLINT colourIndex, double colourParameter, double sizeParameter, bool useNormalisedColourScale, bool useNormalisedSizeScale, double scale ) const;
	bool isColourLogScaled() const;
	bool isSizeLogScaled() const;
	double getSize(double parameter, bool useNormalisedScale) const;
private:
	splotcolourscale m_colourScale;
	splotsizescale m_sizeScale;
};

class FillStyle
{
public:
	FillStyle( const rgbcolour &colour = rgbcolour( 0.0, 0.0, 0.0 ) );
	FillStyle( const LineStyle &lineStyle, double lineSpacingMicrons, double angleDeg );
	FillStyle( const LineStyle &lineStyle, double lineSpacing1Microns, double angle1Deg, double lineSpacing2Microns, double angle2Deg );
	void setupFillStyle( plstream *pl, PLINT colourIndex, double scale ) const;
	void resetFillStyle(plstream *pl, PLINT colourIndex) const;
	rgbcolour getColour() const;

private:
	rgbcolour m_colour;
	LineStyle m_lineStyle;
	PLINT m_lineSpacingMicrons[2];
	PLINT m_angleDeg[2];
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

class XYAxisData : public DrawableItem
{
public:
	virtual void getLimits(double &xMin, double &xMax, double &yMin, double &yMax) const;
	virtual void getLogLimits(double &xMin, double &xMax, double &yMin, double &yMax) const;
protected:
	XYAxisData(const std::vector<double> &xs, const std::vector<double> &ys, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount);
	std::vector<double> m_xData;
	std::vector<double> m_yData;
	std::vector<double> m_xDataLogged;
	std::vector<double> m_yDataLogged;
private:
	double m_padLimitsAmount;
	static void getLimits(const std::vector<double> xs, const std::vector<double> &ys, double &xMin, double &xMax, double &yMin, double &yMax, double padAmount);
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

class PlotData1d : public XYAxisData
{
protected:
	PlotData1d( const std::vector<double> &xs, const std::vector<double> &ys, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.05 );
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

class PlotData3dLinear : public PlotData1d
{
public:
	PlotData3dLinear( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs1, const std::vector<double> &zs2, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.05 );
protected:
	std::vector<double> m_zData1;
	std::vector<double> m_zDataLogged1;
	std::vector<double> m_zDataNormalised1;
	std::vector<double> m_zDataLoggedNormalised1;
	std::vector<double> m_zData2;
	std::vector<double> m_zDataLogged2;
	std::vector<double> m_zDataNormalised2;
	std::vector<double> m_zDataLoggedNormalised2;
};

void getXYValues(double xIndex, double yIndex, double *xOutput, double *yOutput, void *data);

class PlotData2dStructured : public XYAxisData
{
public:
	PlotData2dStructured( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<std::vector<double>> &zs, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.0 );
	void getXYValues(double xIndex, double yIndex, double &xOutput, double &yOutput) const;
	void getZLimits(double &min, double &max) const;
	void getLogZLimits(double &min, double &max) const;
protected:
	std::vector<std::vector<double>> m_zData;
	std::vector<std::vector<double>> m_zDataLogged;
	std::vector<std::vector<double>> m_zDataNormalised;
	std::vector<std::vector<double>> m_zDataLoggedNormalised;
	mutable bool m_plotLogX;
	mutable bool m_plotLogY;
private:
	std::shared_ptr<splotTransformer> m_transformer;
	double m_minZ;
	double m_maxZ;
	double m_maxZLogged;
	double m_minZLogged;
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
};

class PointDataSizeVarying : public PlotData2dLinear
{
public:
	PointDataSizeVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const SizeVaryingSymbol &symbol, bool autoscaleSize = false, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.05 );
	void plotData( plstream *pl, bool xLog, bool yLog ) const;
private:
	SizeVaryingSymbol m_symbol;
	bool m_autoscaleSize;
};

class PointDataColourAndSizeVarying : public PlotData3dLinear
{
public:
	PointDataColourAndSizeVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zsColour, const std::vector<double> &zsSize, const ColourAndSizeVaryingSymbol &symbol, bool autoscaleColour = false, bool autoscaleSize = false, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.05 );
	void plotData( plstream *pl, bool xLog, bool yLog ) const;
private:
	ColourAndSizeVaryingSymbol m_symbol;
	bool m_autoscaleColour;
	bool m_autoscaleSize;
};

class HorizontalErrorBars : public PlotData3dLinear
{
public:
	HorizontalErrorBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &plusErrors, const std::vector<double> minusErrors, const LineStyle style );
	void plotData( plstream *pl, bool xLog, bool yLog ) const;
private:
	LineStyle m_style;
};

class VerticalErrorBars : public PlotData3dLinear
{
public:
	VerticalErrorBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &plusErrors, const std::vector<double> minusErrors, const LineStyle style );
	void plotData( plstream *pl, bool xLog, bool yLog ) const;
private:
	LineStyle m_style;
};

class VerticalBars : public PlotData2dLinear
{
public:
	VerticalBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &widths, const LineStyle &lineStyle, const FillStyle &fillStyle, double zeroLine = 0.0, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.05 );
	void plotData( plstream *pl, bool xLog, bool yLog ) const;
	virtual void getLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const;
	virtual void getLogLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const;
private:
	FillStyle m_fillStyle;
	LineStyle m_lineStyle;
	double m_zeroLine;
	double m_zeroLineLogged;
	double m_padLimitsAmount;
};

class GridData : public PlotData2dStructured
{
public:
	GridData(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<std::vector<double>> &zs, const splotcolourscale &colourScale, bool fillOffScaleBottom, bool fillOffScaleTop, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.0);
	void plotData(plstream *pl, bool xLog, bool yLog) const;
private:
	splotcolourscale m_colourscale;
	bool m_fillOffscaleBottom;
	bool m_fillOffscaleTop;
};

class FillData : public PlotData1d
{
public:
	FillData(const std::vector<double> &xs, const std::vector<double> &ys, const FillStyle &fillStyle = FillStyle(), const LineStyle &outlineStyle = noLine, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.0);
	void plotData(plstream *pl, bool xLog, bool yLog) const;
private:
	FillStyle m_fillStyle;
	LineStyle m_outlineStyle;
};


#endif