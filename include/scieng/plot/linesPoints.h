#ifndef SVECTOR_PLOTLINEPOINT_H
#define SVECTOR_PLOTLINEPOINT_H

#include "plotable.h"
#include "elements.h"


namespace sci
{
	namespace plot
	{
		template<class X, class Y>
		class Line : public Data<X, Y, std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>>,
			GridData<X, 1>, GridData<Y, 1>>
		{
		public:
			using data = Data<X, Y, std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>>,
				GridData<X, 1>, GridData<Y, 1>>;

			template<class XCONTAINER, class YCONTAINER>
			Line(const XCONTAINER &xs, const YCONTAINER &ys, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const LineStyle& lineStyle)
				requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis), xs, ys), m_lineStyle(lineStyle)
			{
			}
		private:
			void plotData(const data::spacialAxesSet& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				m_lineStyle.setPen(renderer);
				std::vector<Point> points(this->getNPoints<0>());

				for (size_t i = 0; i < points.size(); ++i)
					points[i] = this->getPoint<0,1>(i, axisSet);

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
		class Points : public Data<X, Y, std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>>,
			GridData<X, 1>, GridData<Y, 1>>
		{
		public:
			using data = Data<X, Y, std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>>,
				GridData<X, 1>, GridData<Y, 1>>;

			template<class XCONTAINER, class YCONTAINER>
			Points(const XCONTAINER &x, const YCONTAINER y, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const Symbol& symbol, sci::graphics::RgbColour colour)
				requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
				: data(xAxis, yAxis, std::make_tuple( xAxis, yAxis ), x, y), m_symbol(symbol), m_colour(colour)
			{
			}
		private:
			Symbol m_symbol;
			sci::graphics::RgbColour m_colour;

			void plotData(const data::spacialAxesSet& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				renderer.setBrush(m_colour);
				renderer.setPen(sci::graphics::RgbColour(), sci::plot::millimetre(0.0));

				for (size_t i = 0; i < this->getNPoints<0>(); ++i)
					m_symbol.draw(this->getPoint<0,1>(i, axisSet), renderer);
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
