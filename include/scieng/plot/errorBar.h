#ifndef SVECTOR_PLOTERRORBARS_H
#define SVECTOR_PLOTERRORBARS_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		template<class X, class Y>
		class HorizontalErrorBars : public Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<X>, std::vector<X>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<X>, std::vector<X>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;
			HorizontalErrorBars(std::span<const X> xs, std::span<const Y> ys, std::span<const X> plusErrors, std::span<const X> minusErrors, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true)
				:data(xAxis, yAxis, std::make_tuple( xAxis, yAxis, xAxis, xAxis), xs, ys, (xs | sci::views::grid<1>) + (plusErrors | sci::views::grid<1>), (xs | sci::views::grid<1>) - (minusErrors | sci::views::grid<1>))
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
				const std::vector<X>& xs = data::getData<0>(axisSetIndex);
				const std::vector<Y>& ys = data::getData<1>(axisSetIndex);
				const std::vector<X>& minXs = data::getData<2>(axisSetIndex);
				const std::vector<X>& maxXs = data::getData<3>(axisSetIndex);
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
				data::autoscaleAxes(axisSetIndex);
			}
		private:
			LineStyle m_style;
			Length m_stopLength;
			bool m_useForAutoscale;
		};

		template<class X, class Y>
		class VerticalErrorBars : public Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<Y>, std::vector<Y>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<X>, std::vector<X>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;
			VerticalErrorBars(std::span<const X> xs, std::span<const Y> ys, std::span<const Y> plusErrors, std::span<const Y> minusErrors, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true)
				:data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, yAxis, yAxis), xs, ys, (ys | sci::views::grid<1>) + (plusErrors | sci::views::grid<1>), (ys | sci::views::grid<1>) - (minusErrors | sci::views::grid<1>))
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

				const std::vector<X>& xs = data::getData<0>(axisSetIndex);
				const std::vector<Y>& ys = data::getData<1>(axisSetIndex);
				const std::vector<Y>& minYs = data::getData<2>(axisSetIndex);
				const std::vector<Y>& maxYs = data::getData<3>(axisSetIndex);

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
				data::autoscaleAxes(axisSetIndex);
			}
		private:
			LineStyle m_style;
			Length m_stopLength;
			bool m_useForAutoscale;
		};
	}
}


#endif
