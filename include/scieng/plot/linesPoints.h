#ifndef SVECTOR_PLOTLINEPOINT_H
#define SVECTOR_PLOTLINEPOINT_H

#include "plotable.h"


namespace sci
{
	namespace plot
	{
		template<class X, class Y>
		class Lines : public Data<X, Y, std::vector<X>, std::vector<Y>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<Y>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;

			Lines(std::span<const double> xs, std::span<const double> ys, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: Data<X, Y, std::vector<X>, std::vector<Y>>(xAxis, yAxis, std::make_tuple(xAxis, yAxis), transformer, xs, ys), m_lineStyle(lineStyle)
			{
			}
		private:
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;
				m_lineStyle.setPen(renderer);
				std::vector<Point> points(getNPoints());
				const std::vector<double>& x = data::getData<0>(axisSetIndex);
				const std::vector<double>& y = data::getData<1>(axisSetIndex);
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i] = getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex);
				}
				renderer.polyLine(points);
			}
			LineStyle m_lineStyle;
		};

		class Points : public Data<double, double, std::vector<double>, std::vector<double>>
		{
		public:
			Points(std::span<const double> x, std::span<const double> y, std::shared_ptr<Axis<double>> xAxis, std::shared_ptr<Axis<double>> yAxis, const Symbol& symbol, sci::graphics::RgbColour colour, std::shared_ptr<splotTransformer> transformer = nullptr)
				: Data<double, double, std::vector<double>, std::vector<double>>(xAxis, yAxis, std::make_tuple( xAxis, yAxis ), transformer, x, y), m_symbol(symbol), m_colour(colour)
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

				const std::vector<double>& x = getData<0>(axisSetIndex);
				const std::vector<double>& y = getData<1>(axisSetIndex);
				for (size_t i = 0; i < getNPoints(); ++i)
				{
					m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), renderer);
				}
			}
		};
	}
}


#endif
