#ifndef SVECTOR_PLOTERRORBARS_H
#define SVECTOR_PLOTERRORBARS_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		class HorizontalErrorBars : public Bars
		{
		public:
			HorizontalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer),
				Bars(xs, ys, plusErrors, minusErrors, xAxis, yAxis, useForAutoscale, transformer)
			{
				m_style = style;
				m_stopLength = stopLength;
			}
			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_style.getColour(), m_style.getWidth(), m_style.getPattern());

				//note that we render in two halves so that if any of the error bars are negative we stil hit the x, y point
				const std::vector<double>& xs = getAlongData();
				const std::vector<double>& ys = getAcrossData();
				const std::vector<double>& minXs = getMinAlongData();
				const std::vector<double>& maxXs = getMaxAlongData();
				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point pCentre = getPointFromLoggedIfNeededData(xs[i], ys[i]);
					Point p1 = getPointFromLoggedIfNeededData(minXs[i], ys[i]);
					Point p2 = getPointFromLoggedIfNeededData(maxXs[i], ys[i]);
					renderer.line(p1, pCentre);
					renderer.line(p2, pCentre);
					Distance halfStop(Length(sci::graphics::zeroLength), m_stopLength / sci::graphics::unitless(2));
					renderer.line(p1 - halfStop, p1 + halfStop);
					renderer.line(p2 - halfStop, p2 + halfStop);
				}
			}
		private:
			LineStyle m_style;
			Length m_stopLength;
			bool m_useForAutoscale;
		};

		class VerticalErrorBars : public Bars
		{
		public:
			VerticalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer),
				Bars(ys, xs, plusErrors, minusErrors, yAxis, xAxis, useForAutoscale, transformer)
			{
				m_style = style;
				m_stopLength = stopLength;
			}

			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_style.getColour(), m_style.getWidth(), m_style.getPattern());

				const std::vector<double>& xs = getAcrossData();
				const std::vector<double>& ys = getAlongData();
				const std::vector<double>& minYs = getMinAlongData();
				const std::vector<double>& maxYs = getMaxAlongData();

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point pCentre = getPointFromLoggedIfNeededData(xs[i], ys[i]);
					Point p1 = getPointFromLoggedIfNeededData(xs[i], minYs[i]);
					Point p2 = getPointFromLoggedIfNeededData(xs[i], maxYs[i]);
					renderer.line(p1, pCentre);
					renderer.line(p2, pCentre);
					Distance halfStop(Length(m_stopLength / sci::graphics::unitless(2)), sci::graphics::zeroLength);
					renderer.line(p1 - halfStop, p1 + halfStop);
					renderer.line(p2 - halfStop, p2 + halfStop);
				}
			}
		private:
			LineStyle m_style;
			Length m_stopLength;
		};
	}
}


#endif
