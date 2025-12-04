#ifndef SVECTOR_PLOTLINEPOINT_H
#define SVECTOR_PLOTLINEPOINT_H

#include "plotable.h"


namespace sci
{
	namespace plot
	{
		template<class X, class Y>
		class Line : public Data<X, Y, std::vector<X>, std::vector<Y>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<Y>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;

			template<class XCONTAINER, class YCONTAINER>
			Line(const XCONTAINER &xs, const YCONTAINER &ys, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const LineStyle& lineStyle)
				requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis), xs, ys), m_lineStyle(lineStyle)
			{
			}
		private:
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;
				m_lineStyle.setPen(renderer);
				std::vector<Point> points(getNPoints());
				const std::vector<X>& xs = data::getData<0>(axisSetIndex);
				const std::vector<Y>& ys = data::getData<1>(axisSetIndex);
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i] = getPointFromLoggedIfNeededData(xs[i], ys[i], axisSetIndex);
				}
				renderer.polyLine(points);
			}
			LineStyle m_lineStyle;
		};

		template<class XCONTAINER, class YCONTAINER, class X, class Y>
		auto makeLine(const XCONTAINER &xs, const YCONTAINER &ys, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const LineStyle& lineStyle)
			requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
		{
			return std::make_shared<Line<X, Y>>(xs, ys, xAxis, yAxis, lineStyle);
		}

		template<class X, class Y>
		class Points : public Data<X, Y, std::vector<X>, std::vector<Y>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<Y>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;
			template<class XCONTAINER, class YCONTAINER>
			Points(const XCONTAINER &x, const YCONTAINER y, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const Symbol& symbol, sci::graphics::RgbColour colour)
				requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
				: data(xAxis, yAxis, std::make_tuple( xAxis, yAxis ), x, y), m_symbol(symbol), m_colour(colour)
			{
			}
		private:
			Symbol m_symbol;
			sci::graphics::RgbColour m_colour;

			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setBrush(m_colour);
				renderer.setPen(sci::graphics::RgbColour(), millimetre(0.0));

				const std::vector<X>& xs = data::getData<0>(axisSetIndex);
				const std::vector<Y>& ys = data::getData<1>(axisSetIndex);
				for (size_t i = 0; i < getNPoints(); ++i)
				{
					m_symbol.draw(getPointFromLoggedIfNeededData(xs[i], ys[i], axisSetIndex), renderer);
				}
			}
		};

		template<class XCONTAINER, class YCONTAINER, class X, class Y>
		auto makePoints(const XCONTAINER& xs, const YCONTAINER& ys, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const Symbol& symbol, sci::graphics::RgbColour colour)
			requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
		{
			return std::make_shared<Points<X, Y>>(xs, ys, xAxis, yAxis, symbol, colour);
		}
	}
}


#endif
