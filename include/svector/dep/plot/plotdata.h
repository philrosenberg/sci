#ifndef SVECTOR_PLOTDATA_H
#define SVECTOR_PLOTDATA_H

#include<vector>
#include<string>
#include<memory>

#include"splot.h"
#include"../../Units.h"

const int plotDataErrorCode = 1;

class LineStyle
{
public:
	LineStyle( double width = 1.0, const rgbcolour &colour = rgbcolour( 0.0, 0.0, 0.0, 1.0 ), const std::vector<PLINT> &marks = std::vector<PLINT> ( 0 ), const std::vector<PLINT> &spaces = std::vector<PLINT> ( 0 ) );
	LineStyle( double width, const rgbcolour &colour, sci::string pattern );
	double getWidth() const;
	void getPattern( std::vector<PLINT> &marks, std::vector<PLINT> &spaces ) const;
	rgbcolour getColour() const;
	void setupLineStyle( plstream *pl, PLINT colourIndex, double scale ) const;
	void resetLineStyle( plstream *pl, PLINT colourIndex ) const;
	static void parseLineStyle( const sci::string &pattern, std::vector<PLINT> &marks, std::vector<PLINT> &spaces );
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
	SymbolBase( sci::string symbol, PLUNICODE fci );
	sci::string getSymbol() const;
	PLUNICODE getFci() const;
private:
	sci::string m_symbol;
	PLUNICODE m_fci;
};

class Symbol : public SymbolBase
{
public:
	Symbol ( sci::string symbol = sym::filledCircle, double size = 4.0, rgbcolour colour=rgbcolour( 0, 0, 0, 1.0 ) );
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
	VaryingSymbol ( sci::string symbol = sym::filledCircle );
	virtual void setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool useNormalisedScale, double scale ) const = 0;
	virtual bool isLogScaled() const = 0;
private:
};

class VaryingSymbolTwoParam : public SymbolBase
{
public:
	VaryingSymbolTwoParam ( sci::string symbol = sym::filledCircle );
	virtual void setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool useNormalisedScale1, bool useNormalisedScale2, double scale ) const = 0;
	virtual bool isLogScaled1() const = 0;
	virtual bool isLogScaled2() const = 0;

private:
};

class ColourVaryingSymbol : public VaryingSymbol
{
public:
	ColourVaryingSymbol ( sci::string symbol = sym::filledCircle, double size = 4.0, splotcolourscale colourScale = splotcolourscale() );
	void setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool useNormalisedScale, double scale ) const;
	bool isLogScaled() const;
private:
	double m_size;
	splotcolourscale m_colourScale;
};

class SizeVaryingSymbol : public VaryingSymbol
{
public:
	SizeVaryingSymbol ( sci::string symbol = sym::filledCircle, rgbcolour colour = rgbcolour( 0.0, 0.0, 0.0 ), splotsizescale sizeScale = splotsizescale() );
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
	ColourAndSizeVaryingSymbol ( sci::string symbol = sym::filledCircle, splotcolourscale colourScale = splotcolourscale(), splotsizescale sizeScale = splotsizescale() );
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

class splotTransformer;

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
	static void getLimits(const std::vector<double> &xs, const std::vector<double> &ys, double &xMin, double &xMax, double &yMin, double &yMax, double padAmount);
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


template<class X_UNIT, class Y_UNIT>
class PhysicalPlotData
{
public:
	sci::string getXAxisUnits(sci::string brace=sU("(")) const
	{
		sci::string closeBrace = brace;
		if (closeBrace == sU("("))
			closeBrace = sU(")");
		else if (closeBrace == sU("["))
			closeBrace = sU("]");
		else if (closeBrace == sU("{"))
			closeBrace = sU("}");
		else if (closeBrace == sU("<"))
			closeBrace = sU(">");
		return brace + X_UNIT::getShortRepresentation(sU("#u"), sU("#d")) + closeBrace;
	}
	sci::string getYAxisUnits(sci::string brace = sU("(")) const
	{
		sci::string closeBrace = brace;
		if (closeBrace == sU("("))
			closeBrace = sU(")");
		else if (closeBrace == sU("["))
			closeBrace = sU("]");
		else if (closeBrace == sU("{"))
			closeBrace = sU("}");
		else if (closeBrace == sU("<"))
			closeBrace = sU(">");
		return brace + Y_UNIT::getShortRepresentation(sU("#u"), sU("#d")) + closeBrace;
	}
	typedef X_UNIT xUnitType;
	typedef Y_UNIT yUnitType;
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

template <class IN_X_UNIT, class IN_Y_UNIT, class TR_X_UNIT = IN_X_UNIT, class TR_Y_UNIT = IN_Y_UNIT>
class PhysicalLineData : public PhysicalPlotData<TR_X_UNIT, TR_Y_UNIT>, public LineData
{
public:
	PhysicalLineData(const std::vector<sci::Physical<IN_X_UNIT, double>> &xs, const std::vector<sci::Physical<IN_Y_UNIT, double>> &ys, const LineStyle &lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
		: LineData(sci::physicalsToValues<sci::Physical<IN_X_UNIT,double>>(xs), sci::physicalsToValues<sci::Physical<IN_Y_UNIT, double>>(ys), lineStyle, transformer)
	{
	}
};

class PointData : public PlotData1d
{
public:
	PointData( const std::vector<double> &x, const std::vector<double> &y, const Symbol &symbol, std::shared_ptr<splotTransformer> transformer = nullptr );
private:
	Symbol m_symbol;
	void plotData(plstream *pl, bool xLog, bool yLog) const;
};

template <class IN_X_UNIT, class IN_Y_UNIT, class TR_X_UNIT = IN_X_UNIT, class TR_Y_UNIT = IN_Y_UNIT>
class PhysicalPointData : public PhysicalPlotData<TR_X_UNIT, TR_Y_UNIT>, public PointData
{
public:
	template<class X_VALUE_TYPE, class Y_VALUE_TYPE>
	PhysicalPointData(const std::vector<sci::Physical<IN_X_UNIT, X_VALUE_TYPE>> &xs, const std::vector<sci::Physical<IN_Y_UNIT, Y_VALUE_TYPE>> &ys, const Symbol &symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
		: PointData(sci::physicalsToValues<IN_X_UNIT, sci::Physical<IN_X_UNIT, X_VALUE_TYPE>, double>(xs), sci::physicalsToValues<IN_Y_UNIT, sci::Physical<IN_Y_UNIT, Y_VALUE_TYPE>, double>(ys), symbol, transformer)
	{
	}
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

template <class IN_X_UNIT, class IN_Y_UNIT, class IN_Z_UNIT, class TR_X_UNIT = IN_X_UNIT, class TR_Y_UNIT = IN_Y_UNIT>
class PhysicalGridData : public PhysicalPlotData<TR_X_UNIT,TR_Y_UNIT>, public GridData
{
public:
	template<class X_VALUE_TYPE, class Y_VALUE_TYPE, class Z_VALUE_TYPE>
	PhysicalGridData(const std::vector<sci::Physical<IN_X_UNIT, X_VALUE_TYPE>> &xs, const std::vector<sci::Physical<IN_Y_UNIT, Y_VALUE_TYPE>> &ys, const std::vector<std::vector<sci::Physical<IN_Z_UNIT, Z_VALUE_TYPE>>> &zs, const splotcolourscale &colourScale, bool fillOffScaleBottom, bool fillOffScaleTop, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.0)
		:GridData(sci::physicalsToValues<IN_X_UNIT, sci::Physical<IN_X_UNIT, X_VALUE_TYPE>, double >(xs), sci::physicalsToValues<IN_Y_UNIT, sci::Physical<IN_Y_UNIT, Y_VALUE_TYPE>, double>(ys), sci::physicalsToValues<IN_Z_UNIT, std::vector<sci::Physical<IN_Z_UNIT, Z_VALUE_TYPE>>, double >(zs), colourScale, fillOffScaleBottom, fillOffScaleTop, transformer, autoLimitsPadAmount)
	{
	}
	/*template<>
	PhysicalGridData<double, double, double>(const std::vector<sci::Physical<IN_X_UNIT, double>> &xs, const std::vector<sci::Physical<IN_Y_UNIT, double>> &ys, const std::vector<std::vector<sci::Physical<IN_Z_UNIT, double>>> &zs, const splotcolourscale &colourScale, bool fillOffScaleBottom, bool fillOffScaleTop, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.0)
		:GridData(sci::physicalsToValues<IN_X_UNIT>(xs), sci::physicalsToValues<IN_Y_UNIT>(ys), sci::physicalsToValues<IN_Z_UNIT>(zs), colourScale, fillOffScaleBottom, fillOffScaleTop, transformer, autoLimitsPadAmount)
	{
	}*/
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
