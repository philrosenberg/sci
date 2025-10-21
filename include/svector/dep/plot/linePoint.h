#ifndef SVECTOR_PLOTLINEPOINT_H
#define SVECTOR_PLOTLINEPOINT_H

#include "plotable.h"


namespace sci
{
	namespace plot
	{

		class LineData : public UnstructuredData
		{
		public:
			LineData(std::span<const double> xs, std::span<const double> ys, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys }, { xAxis, yAxis }, transformer), m_lineStyle(lineStyle)
			{
			}
		private:
			void plotData(plstream* pl, double scale) const override;
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

		class PointData : public UnstructuredData
		{
		public:
			PointData(std::span<const double> x, std::span<const double> y, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const Symbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ x, y }, { xAxis, yAxis }, transformer), m_symbol(symbol)
			{
			}
		private:
			Symbol m_symbol;
			void plotData(plstream* pl, double scale) const override;
			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setBrush(m_symbol.getColour());
				renderer.setPen(m_symbol.getColour(), grMillimetre(0.5));

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					m_symbol.draw(getPointFromLoggedIfNeededData(getPointer(0)[i], getPointer(1)[i]), renderer);
				}
			}
		};
	}
}


#endif
