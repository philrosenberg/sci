#ifndef SVECTOR_PLOTERRORBARS_H
#define SVECTOR_PLOTERRORBARS_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		template<class X, class Y>
		class HorizontalErrorBars : public Data<X, Y,
			std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<X>>>,
			GridData<X, 1>, GridData<Y, 1>, GridData<X, 1>, GridData<X, 1>>
		{
		public:
			using data = Data<X, Y,
				std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<X>>>,
				GridData<X, 1>, GridData<Y, 1>, GridData<X, 1>, GridData<X, 1>>;

			template<class XCONTAINER, class YCONTAINER>
			HorizontalErrorBars(const XCONTAINER& xs, const YCONTAINER& ys, const XCONTAINER& plusErrors, const XCONTAINER& minusErrors, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true)
				requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
				:data(xAxis, yAxis, std::make_tuple( xAxis, yAxis, xAxis, xAxis), xs, ys, (xs | sci::views::grid<1>) + (plusErrors | sci::views::grid<1>), (xs | sci::views::grid<1>) - (minusErrors | sci::views::grid<1>))
			{
				m_style = style;
				m_stopLength = stopLength;
				m_useForAutoscale = useForAutoscale;
			}

			void plotData(const SpacialAxesSet<X, Y>& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				renderer.setPen(m_style.getColour(), m_style.getWidth(), m_style.getPattern());

				//note that we render in two halves so that if any of the error bars are negative we stil hit the x, y point
				for (size_t i = 0; i < this->getNPoints<0>(); ++i)
				{
					Point pCentre = this->getPoint<0, 1>(i, axisSet);
					Point p1 = this->getPoint<2, 1>(i, axisSet);
					Point p2 = this->getPoint<3, 1>(i, axisSet);
					renderer.line(p1, pCentre);
					renderer.line(p2, pCentre);
					Distance halfStop(Length(sci::graphics::zeroLength), m_stopLength / sci::graphics::unitless(2));
					renderer.line(p1 - halfStop, p1 + halfStop);
					renderer.line(p2 - halfStop, p2 + halfStop);
				}
			}
			virtual void autoscaleAxes() override
			{
				if (!m_useForAutoscale)
					return;
				data::autoscaleAxes();
			}
		private:
			LineStyle m_style;
			Length m_stopLength;
			bool m_useForAutoscale;
		};


		template<class XCONTAINER, class YCONTAINER, class X, class Y>
		auto makeHorizontalErrorBars(const XCONTAINER& xs, const YCONTAINER& ys, const XCONTAINER& plusErrors, const XCONTAINER& minusErrors, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true)
			requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
		{
			return std::make_shared<HorizontalErrorBars<X, Y>>(xs, ys, plusErrors, minusErrors, xAxis, yAxis, stopLength, style, useForAutoscale);
		}

		template<class X, class Y>
		class VerticalErrorBars : public Data<X, Y,
			std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<Axis<Y>>>,
			GridData<X, 1>, GridData<Y, 1>, GridData<Y, 1>, GridData<Y, 1>>
		{
		public:
			using data = Data<X, Y,
				std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<Axis<Y>>>,
				GridData<X, 1>, GridData<Y, 1>, GridData<Y, 1>, GridData<Y, 1>>;

			template<class XCONTAINER, class YCONTAINER>
			VerticalErrorBars(XCONTAINER xs, YCONTAINER ys, YCONTAINER plusErrors, YCONTAINER minusErrors, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true)
				:data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, yAxis, yAxis), xs, ys, (ys | sci::views::grid<1>) + (plusErrors | sci::views::grid<1>), (ys | sci::views::grid<1>) - (minusErrors | sci::views::grid<1>))
			{
				m_style = style;
				m_stopLength = stopLength;
				m_useForAutoscale = useForAutoscale;
			}

			void plotData(const SpacialAxesSet<X, Y>& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				renderer.setPen(m_style.getColour(), m_style.getWidth(), m_style.getPattern());

				for (size_t i = 0; i < this->getNPoints<0>(); ++i)
				{
					Point pCentre = this->getPoint<0, 1>(i, axisSet);
					Point p1 = this->getPoint<0, 2>(i, axisSet);
					Point p2 = this->getPoint<0, 3>(i, axisSet);
					renderer.line(p1, pCentre);
					renderer.line(p2, pCentre);
					Distance halfStop(Length(m_stopLength / sci::graphics::unitless(2)), sci::graphics::zeroLength);
					renderer.line(p1 - halfStop, p1 + halfStop);
					renderer.line(p2 - halfStop, p2 + halfStop);
				}
			}
			virtual void autoscaleAxes() override
			{
				if (!m_useForAutoscale)
					return;
				data::autoscaleAxes();
			}
		private:
			LineStyle m_style;
			Length m_stopLength;
			bool m_useForAutoscale;
		};


		template<class XCONTAINER, class YCONTAINER, class X, class Y>
		auto makeVerticalErrorBars(const XCONTAINER& xs, const YCONTAINER& ys, const XCONTAINER& plusErrors, const XCONTAINER& minusErrors, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true)
			requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
		{
			return std::make_shared<VerticalErrorBars<X, Y>>(xs, ys, plusErrors, minusErrors, xAxis, yAxis, stopLength, style, useForAutoscale);
		}
	}
}


#endif
