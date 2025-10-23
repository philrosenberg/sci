#ifndef SVECTOR_PLOTLINEPOINT_H
#define SVECTOR_PLOTLINEPOINT_H

#include "plotable.h"


namespace sci
{
	namespace plot
	{

		class Lines : public UnstructuredData
		{
		public:
			Lines(std::span<const double> xs, std::span<const double> ys, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys }, { xAxis, yAxis }, transformer), m_lineStyle(lineStyle)
			{
			}
		private:
			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;
				m_lineStyle.setPen(renderer);
				std::vector<Point> points(getNPoints());
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i] = getPointFromLoggedIfNeededData(getPointer(0)[i], getPointer(1)[i]);
				}
				renderer.polyLine(points);
			}
			LineStyle m_lineStyle;
		};

		class Points : public UnstructuredData
		{
		public:
			Points(std::span<const double> x, std::span<const double> y, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const Symbol& symbol, sci::graphics::RgbColour colour, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ x, y }, { xAxis, yAxis }, transformer), m_symbol(symbol), m_colour(colour)
			{
			}
		private:
			Symbol m_symbol;
			sci::graphics::RgbColour m_colour;

			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setBrush(m_colour);
				renderer.setPen(sci::graphics::RgbColour(), grMillimetre(0.0));

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					m_symbol.draw(getPointFromLoggedIfNeededData(getPointer(0)[i], getPointer(1)[i]), renderer);
				}
			}
		};
	}
}


#endif
