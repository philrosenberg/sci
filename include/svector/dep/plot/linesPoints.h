#ifndef SVECTOR_PLOTLINEPOINT_H
#define SVECTOR_PLOTLINEPOINT_H

#include "plotable.h"


namespace sci
{
	namespace plot
	{

		class Lines : public Data<std::vector<double>, std::vector<double>>
		{
		public:
			Lines(std::span<const double> xs, std::span<const double> ys, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: Data<std::vector<double>, std::vector<double>>(xAxis, yAxis, { xAxis, yAxis }, transformer, xs, ys), m_lineStyle(lineStyle)
			{
			}
		private:
			void plotData(size_t axisSetIndex, Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;
				m_lineStyle.setPen(renderer);
				std::vector<Point> points(getNPoints());
				const std::vector<double>& x = getData<0>(axisSetIndex);
				const std::vector<double>& y = getData<1>(axisSetIndex);
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i] = getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex);
				}
				renderer.polyLine(points);
			}
			LineStyle m_lineStyle;
		};

		class Points : public Data<std::vector<double>, std::vector<double>>
		{
		public:
			Points(std::span<const double> x, std::span<const double> y, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const Symbol& symbol, sci::graphics::RgbColour colour, std::shared_ptr<splotTransformer> transformer = nullptr)
				: Data<std::vector<double>, std::vector<double>>(xAxis, yAxis, { xAxis, yAxis }, transformer, x, y), m_symbol(symbol), m_colour(colour)
			{
			}
		private:
			Symbol m_symbol;
			sci::graphics::RgbColour m_colour;

			void plotData(size_t axisSetIndex, Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setBrush(m_colour);
				renderer.setPen(sci::graphics::RgbColour(), grMillimetre(0.0));

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
