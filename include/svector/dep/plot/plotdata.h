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
	VaryingSymbol ( sci::string symbol );
	virtual void setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool parameterPreLogged, double scale ) const = 0;
	virtual bool isLogScaled() const = 0;
private:
};

class ColourVaryingSymbol : public VaryingSymbol
{
public:
	ColourVaryingSymbol ( std::shared_ptr<splotcolourscale> colourScale, sci::string symbol = sym::filledCircle, double size = 4.0 );
	void setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool parameterPreLogged, double scale ) const override;
	bool isLogScaled() const override;
	std::shared_ptr<splotcolourscale> getColourscale() const { return m_colourScale; }
private:
	double m_size;
	std::shared_ptr<splotcolourscale> m_colourScale;
};

class SizeVaryingSymbol : public VaryingSymbol
{
public:
	SizeVaryingSymbol ( std::shared_ptr<splotsizescale> sizeScale, sci::string symbol = sym::filledCircle, rgbcolour colour = rgbcolour( 0.0, 0.0, 0.0 ) );
	void setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool parameterPreLogged, double scale ) const override;
	bool isLogScaled() const override;
	//parameterPreLogged is only utilised for log scales
	double getSize(double parameter, bool parameterPreLogged) const;
	std::shared_ptr<splotsizescale>getSizeScale() const { return m_sizeScale; }
private:
	rgbcolour m_colour;
	std::shared_ptr<splotsizescale> m_sizeScale;
};

class ColourAndSizeVaryingSymbol : public SymbolBase
{
public:
	ColourAndSizeVaryingSymbol ( std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotsizescale> sizeScale, sci::string symbol = sym::filledCircle);
	void setupSymbol( plstream *pl, PLINT colourIndex, double colourParameter, bool colourParameterPreLogged, double sizeParameter, bool sizeParameterPreLogged, double scale ) const;
	bool isColourLogScaled() const;
	bool isSizeLogScaled() const;
	//parameterPreLogged is only utilised for log scales
	double getSize(double parameter, bool parameterPreLogged) const;
	std::shared_ptr<splotcolourscale> getColourscale() const { return m_colourScale; }
	std::shared_ptr<splotsizescale>getSizeScale() const { return m_sizeScale; }
private:
	std::shared_ptr<splotcolourscale> m_colourScale;
	std::shared_ptr<splotsizescale> m_sizeScale;
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

class PlotFrame : public DrawableItem
{
public:
	PlotFrame(double bottomLeftX, double bottomLeftY, double width, double height, const FillStyle& fillStyle = FillStyle(), const LineStyle& lineStyle = noLine, sci::string title = sU(""), double titlesize = 12, double titledistance = 2.0, sci::string titlefont = sU(""), int32_t titlestyle = 0, wxColour titlecolour = wxColour(0, 0, 0));
	void preDraw() override
	{
	}
	void draw(plstream* pl, double scale, double pageWidth, double pageHeight) override;
	bool readyToDraw() const override
	{
		return true;
	}
private:
	double m_bottomLeftX;
	double m_bottomLeftY;
	double m_width;
	double m_height;
	FillStyle m_fillStyle;
	LineStyle m_lineStyle;
	sci::string m_title;
	double m_titlesize;
	double m_titledistance;
	sci::string m_titlefont;
	int32_t m_titlestyle;
	wxColour m_titlecolour;
};

class splotTransformer;


class PlotableItem : public DrawableItem
{
public:
	PlotableItem(std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotTransformer> transformer)
		:m_xAxis(xAxis), m_yAxis(yAxis), m_transformer(transformer), m_scaledAxes(false)
	{}
	virtual ~PlotableItem() {}
	void preDraw() override
	{
		autoscaleAxes();
		m_scaledAxes = true;
	}
	bool readyToDraw() const override
	{
		return m_scaledAxes;
	}
	void draw(plstream* pl, double scale, double pageWidth, double pageHeight) override;
private:
	virtual void autoscaleAxes() = 0;
	virtual void plotData(plstream* pl, double scale) const = 0;
	std::shared_ptr<splotaxis> m_xAxis;
	std::shared_ptr<splotaxis> m_yAxis;
	std::shared_ptr<splotTransformer> m_transformer;
	bool m_scaledAxes;
};

//this class holds data of n dimensions where there is just a 1d
//vector in each dimension. Eg, x-y scatter data or line data
//or x y z data where the z data is one value for each x, y point
class UnstructuredData : virtual public PlotableItem
{
public:
	virtual void autoscaleAxes() override;
	UnstructuredData(const std::vector<const std::vector<double>*> &data, std::vector<std::shared_ptr<PlotScale>> axes, std::shared_ptr<splotTransformer> transformer);
	const double* getPointer(size_t dimension) const { return m_axes[dimension]->isLog() ? (&(m_dataLogged[dimension][0])) : (&(m_data[dimension][0])); }
	bool isLog(size_t dimension) const { return m_axes[dimension]->isLog(); }
	bool hasData() const { return m_data.size() > 0 && m_data[0].size() > 0; }
	size_t getNPoints() const { return m_data[0].size(); }
	size_t getNDimensions() const { return m_data.size(); }
protected:
private:
	std::vector<std::vector<double>> m_data;
	std::vector<std::vector<double>> m_dataLogged;
	std::vector<std::shared_ptr<PlotScale>> m_axes;
};

//this class holds data of n dimensions where there is a 2d
//vector in each dimension. Eg, gridded data over a plane.
//Note this data can hold x, y and z data for a curvilinear grid
class StructuredData : virtual public PlotableItem
{
public:
	virtual void autoscaleAxes() override;
	StructuredData(const std::vector<const std::vector<std::vector<double>>*>& data, std::vector<std::shared_ptr<PlotScale>> axes, std::shared_ptr<splotTransformer> transformer);
	std::vector<const double*> getPointer(size_t dimension) const;
	bool isLog(size_t dimension) const { return m_axes[dimension]->isLog(); }
	bool hasData() const { return m_data.size() > 0 && m_data[0].size() > 0 && m_data[0][0].size() > 0; }
	size_t getNDimensions() const { return m_data.size(); }
private:
	std::vector<std::vector<std::vector<double>>> m_data;
	std::vector<std::vector<std::vector<double>>> m_dataLogged;
	std::vector<std::shared_ptr<PlotScale>> m_axes;
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
		return brace + X_UNIT::getShortRepresentation(sci::string(sU("#u")), sci::string(sU("#d"))) + closeBrace;
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
		return brace + Y_UNIT::getShortRepresentation(sci::string(sU("#u")), sci::string(sU("#d"))) + closeBrace;
	}
	typedef X_UNIT xUnitType;
	typedef Y_UNIT yUnitType;
};

class LineData : public UnstructuredData
{
public:
	LineData( const std::vector<double> &x, const std::vector<double> &y, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const LineStyle &lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr );
private:
	LineStyle m_lineStyle;
	void plotData( plstream *pl, double scale) const override;
};

template <class IN_X_UNIT, class IN_Y_UNIT, class TR_X_UNIT = IN_X_UNIT, class TR_Y_UNIT = IN_Y_UNIT>
class PhysicalLineData : public PhysicalPlotData<TR_X_UNIT, TR_Y_UNIT>, public LineData
{
public:
	template<class X, class Y>
	PhysicalLineData(const std::vector<sci::Physical<X, double>> &xs, const std::vector<sci::Physical<Y, double>> &ys, std::shared_ptr<PlotScale> xAxis, std::shared_ptr<PlotScale> yAxis, const LineStyle &lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
		: LineData(sci::physicalsToValues<sci::Physical<IN_X_UNIT,double>>(xs), sci::physicalsToValues<sci::Physical<IN_Y_UNIT, double>>(ys), lineStyle, transformer)
	{
	}
};

class PointData : public UnstructuredData
{
public:
	PointData( const std::vector<double> &x, const std::vector<double> &y, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const Symbol &symbol, std::shared_ptr<splotTransformer> transformer = nullptr );
private:
	Symbol m_symbol;
	void plotData(plstream *pl, double scale) const override;
};

template <class IN_X_UNIT, class IN_Y_UNIT, class TR_X_UNIT = IN_X_UNIT, class TR_Y_UNIT = IN_Y_UNIT>
class PhysicalPointData : public PhysicalPlotData<TR_X_UNIT, TR_Y_UNIT>, public PointData
{
public:
	template<class X, class Y>
	PhysicalPointData(const std::vector<sci::Physical<X, double>> &xs, const std::vector<sci::Physical<Y, double>> &ys, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const Symbol &symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
		: PointData(sci::physicalsToValues<sci::Physical<IN_X_UNIT, double>>(xs), sci::physicalsToValues<sci::Physical<IN_Y_UNIT, double>>(ys), symbol, transformer)
	{
	}
};

class PointDataColourVarying : public UnstructuredData
{
public:
	PointDataColourVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const ColourVaryingSymbol &symbol, std::shared_ptr<splotTransformer> transformer = nullptr);
	void plotData( plstream *pl, double scale) const override;
private:
	ColourVaryingSymbol m_symbol;
	bool m_autoscaleColour;
};

class PointDataSizeVarying : public UnstructuredData
{
public:
	PointDataSizeVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const SizeVaryingSymbol &symbol, std::shared_ptr<splotTransformer> transformer = nullptr);
	void plotData( plstream *pl, double scale) const override;
private:
	SizeVaryingSymbol m_symbol;
	bool m_autoscaleSize;
};

class PointDataColourAndSizeVarying : public UnstructuredData
{
public:
	PointDataColourAndSizeVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zsColour, const std::vector<double> &zsSize, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const ColourAndSizeVaryingSymbol &symbol, std::shared_ptr<splotTransformer> transformer = nullptr);
	void plotData( plstream *pl, double scale) const override;
private:
	ColourAndSizeVaryingSymbol m_symbol;
	bool m_autoscaleColour;
	bool m_autoscaleSize;
};

class HorizontalErrorBars : public UnstructuredData
{
public:
	HorizontalErrorBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &plusErrors, const std::vector<double> minusErrors, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const LineStyle style, std::shared_ptr<splotTransformer> transformer = nullptr);
	void plotData( plstream *pl, double scale) const override;
private:
	LineStyle m_style;
};

class VerticalErrorBars : public UnstructuredData
{
public:
	VerticalErrorBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &plusErrors, const std::vector<double> minusErrors, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const LineStyle style, std::shared_ptr<splotTransformer> transformer = nullptr);
	void plotData( plstream *pl, double scale) const override;
private:
	LineStyle m_style;
};

class VerticalBars : public UnstructuredData
{
public:
	VerticalBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &widths, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const LineStyle &lineStyle, const FillStyle &fillStyle, double zeroLine = 0.0, std::shared_ptr<splotTransformer> transformer = nullptr );
	void plotData( plstream *pl, double scale) const override;
private:
	FillStyle m_fillStyle;
	LineStyle m_lineStyle;
	double m_zeroLine;
	double m_zeroLineLogged;
	double m_padLimitsAmount;
};

class FillData : public UnstructuredData
{
public:
	FillData(const std::vector<double>& xs, const std::vector<double>& ys, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const FillStyle& fillStyle = FillStyle(), const LineStyle& outlineStyle = noLine, std::shared_ptr<splotTransformer> transformer = nullptr);
	void plotData(plstream* pl, double scale) const override;
private:
	FillStyle m_fillStyle;
	LineStyle m_lineStyle;
};

class Data2d : public UnstructuredData, public StructuredData
{
public:
	Data2d(const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr);
	Data2d(const std::vector < std::vector<double>>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr);
	Data2d(const std::vector<double>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr);
	Data2d(const std::vector < std::vector<double>>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr);
	virtual void autoscaleAxes() override;//because this class inherits this function from both UnstructuredData and StructuredData, it must override in this class
protected:
	bool m_x1d;
	bool m_y1d;
	size_t m_xSize;
	size_t m_ySize;
};

class GridData : public Data2d
{
public:
	GridData(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<std::vector<double>> &zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr);
	GridData(const std::vector < std::vector<double>>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr);
	GridData(const std::vector<double>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr);
	GridData(const std::vector < std::vector<double>>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr);
	void plotData(plstream *pl, double scale) const override;
private:
	std::shared_ptr<splotcolourscale> m_colourscale;
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

class ContourData : public Data2d
{
public:
	ContourData(const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
	ContourData(const std::vector < std::vector<double>>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
	ContourData(const std::vector<double>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
	ContourData(const std::vector < std::vector<double>>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);

	ContourData(const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
	ContourData(const std::vector < std::vector<double>>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
	ContourData(const std::vector<double>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
	ContourData(const std::vector < std::vector<double>>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
	void plotData(plstream* pl, double scale) const override;
private:
	
	std::shared_ptr<splotcolourscale> m_colourscale;
	std::shared_ptr<splotlevelscale> m_levelScale;
	LineStyle m_lineStyle;
};


#endif
