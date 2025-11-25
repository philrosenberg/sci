#ifndef SVECTOR_PLOTERRORBARS_H
#define SVECTOR_PLOTERRORBARS_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		class HorizontalErrorBars : public Data<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>>
		{
		public:
			HorizontalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true, std::shared_ptr<splotTransformer> transformer = nullptr)
				:Data<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>>(xAxis, yAxis, { xAxis, yAxis, xAxis, xAxis }, transformer, xs, ys, (xs | sci::views::grid<1>) + (plusErrors | sci::views::grid<1>), (xs | sci::views::grid<1>) - (minusErrors | sci::views::grid<1>))
			{
				m_style = style;
				m_stopLength = stopLength;
				m_useForAutoscale = useForAutoscale;
			}
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_style.getColour(), m_style.getWidth(), m_style.getPattern());

				//note that we render in two halves so that if any of the error bars are negative we stil hit the x, y point
				const std::vector<double>& xs = getData<0>(axisSetIndex);
				const std::vector<double>& ys = getData<1>(axisSetIndex);
				const std::vector<double>& minXs = getData<2>(axisSetIndex);
				const std::vector<double>& maxXs = getData<3>(axisSetIndex);
				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point pCentre = getPointFromLoggedIfNeededData(xs[i], ys[i], axisSetIndex);
					Point p1 = getPointFromLoggedIfNeededData(minXs[i], ys[i], axisSetIndex);
					Point p2 = getPointFromLoggedIfNeededData(maxXs[i], ys[i], axisSetIndex);
					renderer.line(p1, pCentre);
					renderer.line(p2, pCentre);
					Distance halfStop(Length(sci::graphics::zeroLength), m_stopLength / sci::graphics::unitless(2));
					renderer.line(p1 - halfStop, p1 + halfStop);
					renderer.line(p2 - halfStop, p2 + halfStop);
				}
			}
			virtual void autoscaleAxes(size_t axisSetIndex) override
			{
				if (!m_useForAutoscale)
					return;
				Data<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>>::autoscaleAxes(axisSetIndex);
			}
		private:
			LineStyle m_style;
			Length m_stopLength;
			bool m_useForAutoscale;
		};

		class VerticalErrorBars : public Data<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>>
		{
		public:
			VerticalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true, std::shared_ptr<splotTransformer> transformer = nullptr)
				:Data<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>>(xAxis, yAxis, { xAxis, yAxis, yAxis, yAxis }, transformer, xs, ys, (ys | sci::views::grid<1>) + (plusErrors | sci::views::grid<1>), (ys | sci::views::grid<1>) - (minusErrors | sci::views::grid<1>))
			{
				m_style = style;
				m_stopLength = stopLength;
				m_useForAutoscale = useForAutoscale;
			}

			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_style.getColour(), m_style.getWidth(), m_style.getPattern());

				const std::vector<double>& xs = getData<0>(axisSetIndex);
				const std::vector<double>& ys = getData<1>(axisSetIndex);
				const std::vector<double>& minYs = getData<2>(axisSetIndex);
				const std::vector<double>& maxYs = getData<3>(axisSetIndex);

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point pCentre = getPointFromLoggedIfNeededData(xs[i], ys[i], axisSetIndex);
					Point p1 = getPointFromLoggedIfNeededData(xs[i], minYs[i], axisSetIndex);
					Point p2 = getPointFromLoggedIfNeededData(xs[i], maxYs[i], axisSetIndex);
					renderer.line(p1, pCentre);
					renderer.line(p2, pCentre);
					Distance halfStop(Length(m_stopLength / sci::graphics::unitless(2)), sci::graphics::zeroLength);
					renderer.line(p1 - halfStop, p1 + halfStop);
					renderer.line(p2 - halfStop, p2 + halfStop);
				}
			}
			virtual void autoscaleAxes(size_t axisSetIndex) override
			{
				if (!m_useForAutoscale)
					return;
				Data<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>>::autoscaleAxes(axisSetIndex);
			}
		private:
			LineStyle m_style;
			Length m_stopLength;
			bool m_useForAutoscale;
		};
	}
}


#endif
