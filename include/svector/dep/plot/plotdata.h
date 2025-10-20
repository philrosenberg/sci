#ifndef SVECTOR_PLOTDATA_H
#define SVECTOR_PLOTDATA_H

#include<vector>
#include<string>
#include<memory>

#include"splot.h"
#include"../../Units.h"
#include<svector/grid.h>
#include<svector/gridtransformview.h>

const int plotDataErrorCode = 1;

namespace sci
{
	namespace plot
	{

		class SymbolBase
		{
		public:
			SymbolBase(::sci::string symbol, PLUNICODE fci);
			SymbolBase(const std::vector<::sci::graphics::Distance>& symbol);
			::sci::string getSymbol() const;
			PLUNICODE getFci() const;
			void draw(const ::sci::graphics::Point& point, ::sci::graphics::Renderer& renderer) const;
		private:
			::sci::string m_symbolText;
			PLUNICODE m_fci;
			std::vector<::sci::graphics::Distance> m_symbol;
		};

		class Symbol : public SymbolBase
		{
		public:
			Symbol();
			Symbol(const Symbol& symbol) = default;
			Symbol(Symbol&& symbol) = default;
			Symbol& operator=(const Symbol& symbol) = default;
			Symbol(::sci::string symbol, double size = 4.0, ::sci::graphics::RgbColour colour = ::sci::graphics::RgbColour(0, 0, 0, 1.0));
			Symbol(const std::span<::sci::graphics::Distance>& symbol,
				::sci::graphics::RgbColour colour = ::sci::graphics::RgbColour(0, 0, 0, 1.0));
			double getSize() const;
			::sci::graphics::RgbColour getColour() const;
			void setupSymbol(plstream* pl, PLINT colourIndex, double scale) const;
		private:
			double m_size;
			::sci::graphics::RgbColour m_colour;
		};

		class VaryingSymbol : public SymbolBase
		{
		public:
			VaryingSymbol(::sci::string symbol);
			virtual void setupSymbol(plstream* pl, PLINT colourIndex, double parameter, bool parameterPreLogged, double scale) const = 0;
			virtual bool isLogScaled() const = 0;
		private:
		};

		class ColourVaryingSymbol : public VaryingSymbol
		{
		public:
			ColourVaryingSymbol(std::shared_ptr<ColourScale> colourScale, ::sci::string symbol = symText::filledCircle, double size = 4.0);
			void setupSymbol(plstream* pl, PLINT colourIndex, double parameter, bool parameterPreLogged, double scale) const override;
			bool isLogScaled() const override;
			std::shared_ptr<ColourScale> getColourscale() const { return m_colourScale; }
		private:
			double m_size;
			std::shared_ptr<ColourScale> m_colourScale;
		};

		class SizeVaryingSymbol : public VaryingSymbol
		{
		public:
			SizeVaryingSymbol(std::shared_ptr<SizeScale> sizeScale, ::sci::string symbol = symText::filledCircle, ::sci::graphics::RgbColour colour = ::sci::graphics::RgbColour(0.0, 0.0, 0.0));
			void setupSymbol(plstream* pl, PLINT colourIndex, double parameter, bool parameterPreLogged, double scale) const override;
			bool isLogScaled() const override;
			//parameterPreLogged is only utilised for log scales
			double getSize(double parameter, bool parameterPreLogged) const;
			std::shared_ptr<SizeScale>getSizeScale() const { return m_sizeScale; }
		private:
			::sci::graphics::RgbColour m_colour;
			std::shared_ptr<SizeScale> m_sizeScale;
		};

		class ColourAndSizeVaryingSymbol : public SymbolBase
		{
		public:
			ColourAndSizeVaryingSymbol(std::shared_ptr<ColourScale> colourScale, std::shared_ptr<SizeScale> sizeScale, ::sci::string symbol = symText::filledCircle);
			void setupSymbol(plstream* pl, PLINT colourIndex, double colourParameter, bool colourParameterPreLogged, double sizeParameter, bool sizeParameterPreLogged, double scale) const;
			bool isColourLogScaled() const;
			bool isSizeLogScaled() const;
			//parameterPreLogged is only utilised for log scales
			double getSize(double parameter, bool parameterPreLogged) const;
			std::shared_ptr<ColourScale> getColourscale() const { return m_colourScale; }
			std::shared_ptr<SizeScale>getSizeScale() const { return m_sizeScale; }
		private:
			std::shared_ptr<ColourScale> m_colourScale;
			std::shared_ptr<SizeScale> m_sizeScale;
		};

		class FillStyle
		{
		public:
			FillStyle(const ::sci::graphics::RgbColour& colour = ::sci::graphics::RgbColour(0.0, 0.0, 0.0))
			{
				m_colour = colour;
				m_lineSpacingMicrons[0] = 0;
				m_angleDeg[0] = 0;
				m_lineSpacingMicrons[1] = 0;
				m_angleDeg[1] = 0;
			}
			FillStyle(const LineStyle& lineStyle, double lineSpacingMicrons, double angleDeg)
			{
				m_lineStyle = lineStyle;
				m_lineSpacingMicrons[0] = (PLINT)::sci::round(lineSpacingMicrons);
				m_angleDeg[0] = (PLINT)::sci::round(angleDeg * 10.0);
				m_lineSpacingMicrons[1] = 0;
				m_angleDeg[1] = 0;
			}
			FillStyle(const LineStyle& lineStyle, double lineSpacing1Microns, double angle1Deg, double lineSpacing2Microns, double angle2Deg)
			{
				m_lineStyle = lineStyle;
				m_lineSpacingMicrons[0] = (PLINT)::sci::round(lineSpacing1Microns);
				m_angleDeg[0] = (PLINT)::sci::round(angle1Deg * 10.0);
				m_lineSpacingMicrons[1] = (PLINT)::sci::round(lineSpacing2Microns);
				m_angleDeg[1] = (PLINT)::sci::round(angle2Deg * 10.0);
			}
			void setupFillStyle(plstream* pl, PLINT colourIndex, double scale) const;
			void resetFillStyle(plstream* pl, PLINT colourIndex) const;
			::sci::graphics::RgbColour getColour() const
			{
				if (m_lineSpacingMicrons[0] > 0)
					return m_lineStyle.getColour();
				return m_colour;
			}
			void setBrush(::sci::graphics::Renderer& renderer) const
			{
				renderer.setBrush(m_colour);
			}

		private:
			::sci::graphics::RgbColour m_colour;
			LineStyle m_lineStyle;
			PLINT m_lineSpacingMicrons[2];
			PLINT m_angleDeg[2];
		};

		class PlotFrame : public DrawableItem
		{
		public:
			PlotFrame(const Point topLeft, const Point bottomRight, const FillStyle& fillStyle = FillStyle(), const LineStyle& lineStyle = noLine,
				::sci::string title = sU(""), Length titlesize = Length(grTextPoint(12.0)), Length titledistance = Length(grTextPoint(12.0)),
				::sci::string titlefont = sU(""), int32_t titlestyle = 0, rgbcolour titlecolour = rgbcolour(0, 0, 0));
			void preDraw() override
			{
			}
			void draw(plstream* pl, double scale, double pageWidth, double pageHeight) override;
			void draw(Renderer& renderer, grPerMillimetre scale) override;
			bool readyToDraw() const override
			{
				return true;
			}
		private:
			Point m_topLeft;
			Point m_bottomRight;
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			::sci::string m_title;
			Length m_titlesize;
			Length m_titledistance;
			::sci::string m_titlefont;
			int32_t m_titlestyle;
			rgbcolour m_titlecolour;
		};

		class splotTransformer;


		class PlotableItem : public DrawableItem
		{
		public:
			PlotableItem(std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotTransformer> transformer)
				:m_xAxis(xAxis), m_yAxis(yAxis), m_transformer(transformer), m_scaledAxes(false), m_intersection(xAxis->getStart().getX(), yAxis->getStart().getY())
			{
				//a note about intersection
				//In theory, two axes do not need to be horizontal and vertical and their start points do not need to be the same.
				//In this case, you might follow a line from the start of each axis, parallel to the other and the intersection point
				//is the start corner of the plot area.
				//However, in general this intersection cannot be represented by a Point as it is a nonlinear combination of the
				//absolute position and the positions scaled by the height and width. Hence we don't do this.
				//Instead, we find the intersection as the point up from the start of the x axis and across from the start of the y
				//axis. This point can be represented by a Point and hence will scale properly.
			}
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
			void draw(Renderer& renderer, grPerMillimetre scale) override;
			Point getPointFromLinearData(double x, double y) const
			{
				return m_intersection + m_xAxis->alongAxisDistanceFromLinearData(x) + m_yAxis->alongAxisDistanceFromLinearData(y);
			}
			Point getPointFromLoggedIfNeededData(double x, double y) const
			{
				return m_intersection + m_xAxis->alongAxisDistanceFromLoggedIfNeededData(x) + m_yAxis->alongAxisDistanceFromLoggedIfNeededData(y);
			}
		private:
			virtual void autoscaleAxes() = 0;
			virtual void plotData(plstream* pl, double scale) const = 0;
			virtual void plotData(Renderer& renderer, grPerMillimetre scale) const {};
			std::shared_ptr<PlotAxis> m_xAxis;
			std::shared_ptr<PlotAxis> m_yAxis;
			std::shared_ptr<splotTransformer> m_transformer;
			bool m_scaledAxes;
			Point m_intersection;
		};

		//this class holds data of n dimensions where there is just a 1d
		//vector in each dimension. Eg, x-y scatter data or line data
		//or x y z data where the z data is one value for each x, y point
		class UnstructuredData : virtual public PlotableItem
		{
		public:
			virtual void autoscaleAxes() override;
			UnstructuredData(const std::vector<std::span<const double>>& data, std::vector<std::shared_ptr<Scale>> axes, std::shared_ptr<splotTransformer> transformer);
			const double* getPointer(size_t dimension) const
			{
				return m_axes[dimension]->isLog() ? (&(m_dataLogged[dimension][0])) : (&(m_data[dimension][0]));
			}
			const std::vector<double>& getVector(size_t dimension) const
			{
				return m_axes[dimension]->isLog() ? (m_dataLogged[dimension]) : (m_data[dimension]);
			}
			bool isLog(size_t dimension) const
			{
				return m_axes[dimension]->isLog();
			}
			bool hasData() const
			{
				return m_data.size() > 0 && m_data[0].size() > 0;
			}
			size_t getNPoints() const
			{
				return m_data[0].size();
			}
			size_t getNDimensions() const
			{
				return m_data.size();
			}
		protected:
		private:
			std::vector<std::vector<double>> m_data;
			std::vector<std::vector<double>> m_dataLogged;
			std::vector<std::shared_ptr<Scale>> m_axes;
		};

		//this class holds data of n dimensions where there is a 2d
		//vector in each dimension. Eg, gridded data over a plane.
		//Note this data can hold x, y and z data for a curvilinear grid
		class StructuredData : virtual public PlotableItem
		{
		public:
			virtual void autoscaleAxes() override;
			StructuredData(const std::vector<const ::sci::GridData<double, 2>*>& data, std::vector<std::shared_ptr<Scale>> axes, std::shared_ptr<splotTransformer> transformer);
			std::vector<const double*> getPointer(size_t dimension) const;
			const ::sci::GridData<double, 2>& getGrid(size_t dimension) const
			{
				return m_data[dimension];
			}
			bool isLog(size_t dimension) const
			{
				return m_axes[dimension]->isLog();
			}
			bool hasData() const
			{
				return m_data.size() > 0 && m_data[0].size() > 0 && m_data[0][0].size() > 0;
			}
			size_t getNDimensions() const
			{
				return m_data.size();
			}
		private:
			std::vector<::sci::GridData<double, 2>> m_data;
			std::vector<::sci::GridData<double, 2>> m_dataLogged;
			std::vector<std::shared_ptr<Scale>> m_axes;
		};

		template<class X_UNIT, class Y_UNIT>
		class PhysicalPlotData
		{
		public:
			::sci::string getXAxisUnits(::sci::string brace = sU("(")) const
			{
				::sci::string closeBrace = brace;
				if (closeBrace == sU("("))
					closeBrace = sU(")");
				else if (closeBrace == sU("["))
					closeBrace = sU("]");
				else if (closeBrace == sU("{"))
					closeBrace = sU("}");
				else if (closeBrace == sU("<"))
					closeBrace = sU(">");
				return brace + X_UNIT::getShortRepresentation(::sci::string(sU("#u")), ::sci::string(sU("#d"))) + closeBrace;
			}
			::sci::string getYAxisUnits(::sci::string brace = sU("(")) const
			{
				::sci::string closeBrace = brace;
				if (closeBrace == sU("("))
					closeBrace = sU(")");
				else if (closeBrace == sU("["))
					closeBrace = sU("]");
				else if (closeBrace == sU("{"))
					closeBrace = sU("}");
				else if (closeBrace == sU("<"))
					closeBrace = sU(">");
				return brace + Y_UNIT::getShortRepresentation(::sci::string(sU("#u")), ::sci::string(sU("#d"))) + closeBrace;
			}
			typedef X_UNIT xUnitType;
			typedef Y_UNIT yUnitType;
		};

		class LineData : public UnstructuredData
		{
		public:
			LineData(std::span<const double> x, std::span<const double> y, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
		private:
			void plotData(plstream* pl, double scale) const override;
			void plotData(Renderer& renderer, grPerMillimetre scale) const override;
			LineStyle m_lineStyle;
		};

		template <class IN_X_UNIT, class IN_Y_UNIT, class TR_X_UNIT = IN_X_UNIT, class TR_Y_UNIT = IN_Y_UNIT>
		class PhysicalLineData : public PhysicalPlotData<TR_X_UNIT, TR_Y_UNIT>, public LineData
		{
		public:
			template<class X, class Y>
			PhysicalLineData(const std::vector<::sci::Physical<X, double>>& xs, const std::vector<::sci::Physical<Y, double>>& ys, std::shared_ptr<Scale> xAxis, std::shared_ptr<Scale> yAxis, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: LineData(::sci::physicalsToValues<::sci::Physical<IN_X_UNIT, double>>(xs), ::sci::physicalsToValues<::sci::Physical<IN_Y_UNIT, double>>(ys), lineStyle, transformer)
			{
			}
		};

		class PointData : public UnstructuredData
		{
		public:
			PointData(std::span<const double> x, std::span<const double> y, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const Symbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr);
		private:
			Symbol m_symbol;
			void plotData(plstream* pl, double scale) const override;
			void plotData(Renderer& renderer, grPerMillimetre scale) const override;
		};

		template <class IN_X_UNIT, class IN_Y_UNIT, class TR_X_UNIT = IN_X_UNIT, class TR_Y_UNIT = IN_Y_UNIT>
		class PhysicalPointData : public PhysicalPlotData<TR_X_UNIT, TR_Y_UNIT>, public PointData
		{
		public:
			template<class X, class Y>
			PhysicalPointData(const std::vector<::sci::Physical<X, double>>& xs, const std::vector<::sci::Physical<Y, double>>& ys, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const Symbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PointData(::sci::physicalsToValues<::sci::Physical<IN_X_UNIT, double>>(xs), ::sci::physicalsToValues<::sci::Physical<IN_Y_UNIT, double>>(ys), symbol, transformer)
			{
			}
		};

		class PointDataColourVarying : public UnstructuredData
		{
		public:
			PointDataColourVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const ColourVaryingSymbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
		private:
			ColourVaryingSymbol m_symbol;
		};

		class PointDataSizeVarying : public UnstructuredData
		{
		public:
			PointDataSizeVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const SizeVaryingSymbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
		private:
			SizeVaryingSymbol m_symbol;
		};

		class PointDataColourAndSizeVarying : public UnstructuredData
		{
		public:
			PointDataColourAndSizeVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zsColour, std::span<const double> zsSize, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const ColourAndSizeVaryingSymbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
		private:
			ColourAndSizeVaryingSymbol m_symbol;
		};

		class HorizontalErrorBars : public UnstructuredData
		{
		public:
			HorizontalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const LineStyle style, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
		private:
			LineStyle m_style;
		};

		class VerticalErrorBars : public UnstructuredData
		{
		public:
			VerticalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const LineStyle style, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
		private:
			LineStyle m_style;
		};

		class VerticalBars : public UnstructuredData
		{
		public:
			VerticalBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> widths, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, double zeroLine = 0.0, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			double m_zeroLine;
			double m_zeroLineLogged;
		};

		class FillData : public UnstructuredData
		{
		public:
			FillData(const std::vector<double>& xs, const std::vector<double>& ys, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const FillStyle& fillStyle = FillStyle(), const LineStyle& outlineStyle = noLine, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
			void plotData(Renderer& renderer, grPerMillimetre scale) const override;
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
		};

		class Data2d : public UnstructuredData, public StructuredData
		{
		public:
			Data2d(std::span<const double> xs, std::span<const double> ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<Scale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr);
			Data2d(const ::sci::GridData<double, 2>& xs, const ::sci::GridData<double, 2>& ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<Scale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr);
			Data2d(std::span<const double> xs, const ::sci::GridData<double, 2>& ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<Scale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr);
			Data2d(const ::sci::GridData<double, 2>& xs, std::span<const double> ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<Scale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr);
			virtual void autoscaleAxes() override;//because this class inherits this function from both UnstructuredData and StructuredData, it must override in this class
			const ::sci::GridData<double, 2>& getZ() const
			{
				if ((!m_x1d) && (!m_y1d))
					return getGrid(2);
				if ((!m_x1d) || (!m_y1d))
					return getGrid(1);
				return getGrid(0);
			}
		protected:
			bool m_x1d;
			bool m_y1d;
			size_t m_xNBoxes;
			size_t m_yNBoxes;
			std::shared_ptr<PlotAxis> m_xAxis;
			std::shared_ptr<PlotAxis> m_yAxis;
		};

		class GridData : public Data2d
		{
		public:
			GridData(std::span<const double> xs, std::span<const double> ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<ColourScale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr);
			GridData(const ::sci::GridData<double, 2>& xs, const ::sci::GridData<double, 2>& ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<ColourScale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr);
			GridData(std::span<const double> xs, const ::sci::GridData<double, 2>& ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<ColourScale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr);
			GridData(const ::sci::GridData<double, 2>& xs, std::span<const double> ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<ColourScale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
			void plotData(Renderer& renderer, grPerMillimetre scale) const override;
		private:
			std::shared_ptr<ColourScale> m_colourscale;
		};

		template <class IN_X_UNIT, class IN_Y_UNIT, class IN_Z_UNIT, class TR_X_UNIT = IN_X_UNIT, class TR_Y_UNIT = IN_Y_UNIT>
		class PhysicalGridData : public PhysicalPlotData<TR_X_UNIT, TR_Y_UNIT>, public GridData
		{
		public:
			template<class X_VALUE_TYPE, class Y_VALUE_TYPE, class Z_VALUE_TYPE>
			PhysicalGridData(const std::vector<::sci::Physical<IN_X_UNIT, X_VALUE_TYPE>>& xs, const std::vector<::sci::Physical<IN_Y_UNIT, Y_VALUE_TYPE>>& ys, const std::vector<std::vector<::sci::Physical<IN_Z_UNIT, Z_VALUE_TYPE>>>& zs, const ColourScale& colourScale, bool fillOffScaleBottom, bool fillOffScaleTop, std::shared_ptr<splotTransformer> transformer = nullptr, double autoLimitsPadAmount = 0.0)
				:GridData(::sci::physicalsToValues<IN_X_UNIT, ::sci::Physical<IN_X_UNIT, X_VALUE_TYPE>, double >(xs), ::sci::physicalsToValues<IN_Y_UNIT, ::sci::Physical<IN_Y_UNIT, Y_VALUE_TYPE>, double>(ys), ::sci::physicalsToValues<IN_Z_UNIT, std::vector<::sci::Physical<IN_Z_UNIT, Z_VALUE_TYPE>>, double >(zs), colourScale, fillOffScaleBottom, fillOffScaleTop, transformer, autoLimitsPadAmount)
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
			ContourData(std::span<const double> xs, std::span<const double> ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<ColourScale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
			ContourData(const ::sci::GridData<double, 2>& xs, const ::sci::GridData<double, 2>& ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<ColourScale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
			ContourData(std::span<const double> xs, const ::sci::GridData<double, 2>& ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<ColourScale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
			ContourData(const ::sci::GridData<double, 2>& xs, std::span<const double> ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<ColourScale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);

			ContourData(std::span<const double> xs, std::span<const double> ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<LevelScale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
			ContourData(const ::sci::GridData<double, 2>& xs, const ::sci::GridData<double, 2>& ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<LevelScale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
			ContourData(std::span<const double> xs, const ::sci::GridData<double, 2>& ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<LevelScale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
			ContourData(const ::sci::GridData<double, 2>& xs, std::span<const double> ys, const ::sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<LevelScale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
			void plotData(Renderer& renderer, grPerMillimetre scale) const override;
		private:

			std::shared_ptr<ColourScale> m_colourscale;
			std::shared_ptr<LevelScale> m_levelScale;
			LineStyle m_lineStyle;
		};
	}
}


#endif
