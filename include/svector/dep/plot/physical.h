#ifndef SVECTOR_PLOTPHYSICAL_H
#define SVECTOR_PLOTPHYSICAL_H

#include "linePoint.h"
#include "gridContour.h"


namespace sci
{
	namespace plot
	{
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

		template <class IN_X_UNIT, class IN_Y_UNIT, class TR_X_UNIT = IN_X_UNIT, class TR_Y_UNIT = IN_Y_UNIT>
		class PhysicalPointData : public PhysicalPlotData<TR_X_UNIT, TR_Y_UNIT>, public PointData
		{
		public:
			template<class X, class Y>
			PhysicalPointData(const std::vector<::sci::Physical<X, double>>& xs, const std::vector<::sci::Physical<Y, double>>& ys, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const Symbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PointData(::sci::physicalsToValues<::sci::Physical<IN_X_UNIT, double>>(xs), ::sci::physicalsToValues<::sci::Physical<IN_Y_UNIT, double>>(ys), symbol, transformer)
			{
			}
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
	}
}


#endif
