#ifndef SVECTOR_PLOTFILLBAR_H
#define SVECTOR_PLOTFILLBAR_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		template<class X, class Y>
		class VerticalBars : public Data<X, Y,
			std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>>,
			GridData<X, 1>, GridData<X, 1>, GridData<Y, 1>>
		{
		public:
			using data = Data<X, Y,
				std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>>,
				GridData<X, 1>, GridData<X, 1>, GridData<Y, 1>>;

			template<class XCONTAINER, class YCONTAINER>
			VerticalBars(const XCONTAINER& xs, const YCONTAINER& ys, const XCONTAINER& widths, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, Y zeroLine = Y(0.0))
				requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
				:data(xAxis, yAxis, std::make_tuple(xAxis, xAxis, yAxis), (xs | sci::views::grid<1>) - 0.5 * (widths | sci::views::grid<1>), (xs | sci::views::grid<1>) + 0.5 * (widths | sci::views::grid<1>), ys)
			{
				//a note on the above - the result of xs-0.5*widths and xs+0.5*widths is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address. 
				m_lineStyle = lineStyle;
				m_fillStyle = fillStyle;
				m_zeroLineLinear = zeroLine;
				m_zeroLineLogged = zeroLine > 0.0 ? std::log(zeroLine) : std::numeric_limits<decltype(Y()/Y())>::quiet_NaN();
			}

			void plotData(const SpacialAxesSet<X, Y>& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				renderer.setPen(m_lineStyle.getColour(), m_lineStyle.getWidth(), m_lineStyle.getPattern());
				renderer.setBrush(m_fillStyle.getColour());

				Y zeroLine = axisSet.isLog(1) ? m_zeroLineLogged : m_zeroLineLinear;

				for (size_t i = 0; i < this->getNPoints<0>(); ++i)
				{
					Point p1 = this->getPointYFixed<0>(zeroLine, i, axisSet);
					Point p2 = this->getPoint<1, 2>(i, axisSet);
					renderer.rectangle(p1, p2);
				}
			}
			virtual void autoscaleAxes() override
			{
				this->expandAxis<2>(m_zeroLineLinear);
				data::autoscaleAxes();
			}
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			Y m_zeroLineLinear;
			decltype(Y()/Y()) m_zeroLineLogged;
		};

		template<class XCONTAINER, class YCONTAINER, class X, class Y>
		auto makeVerticalBars(const XCONTAINER& xs, const YCONTAINER& ys, const XCONTAINER& widths, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, Y zeroLine = Y(0.0))
			requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
		{
			return std::make_shared<VerticalBars<X, Y>>(xs, ys, widths, xAxis, yAxis, lineStyle, fillStyle, zeroLine);
		}

		template<class X, class Y>
		class HorizontalBars : public Data<X, Y,
			std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<Axis<Y>>>,
			GridData<X, 1>, GridData<Y, 1>, GridData<Y, 1>>
		{
		public:
			using data = Data<X, Y,
				std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<Axis<Y>>>,
				GridData<X, 1>, GridData<Y, 1>, GridData<Y, 1>>;

			template<class XCONTAINER, class YCONTAINER>
			HorizontalBars(const XCONTAINER& xs, const YCONTAINER& ys, const YCONTAINER& widths, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, X zeroLine = X(0.0))
				requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
				:data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, yAxis), xs, (ys | sci::views::grid<1>) - 0.5 * (widths | sci::views::grid<1>), (ys | sci::views::grid<1>) + 0.5 * (widths | sci::views::grid<1>))
			{
				//a note on the above - the result of xs-0.5*widths and xs+0.5*widths is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address. 
				m_lineStyle = lineStyle;
				m_fillStyle = fillStyle;
				m_zeroLineLinear = zeroLine;
				m_zeroLineLogged = zeroLine > 0.0 ? std::log(zeroLine) : std::numeric_limits<decltype(X()/X())>::quiet_NaN();
			}

			void plotData(const SpacialAxesSet<X, Y>& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				renderer.setPen(m_lineStyle.getColour(), m_lineStyle.getWidth(), m_lineStyle.getPattern());
				renderer.setBrush(m_fillStyle.getColour());

				X zeroLine = axisSet.isLog(0) ? m_zeroLineLogged : m_zeroLineLinear;

				for (size_t i = 0; i < this->getNPoints<0>(); ++i)
				{
					Point p1 = this->getPointXFixed<1>(zeroLine, i, axisSet);
					Point p2 = this->getPoint<0, 2>(i, axisSet);
					renderer.rectangle(p1, p2);
				}
			}

			virtual void autoscaleAxes() override
			{
				this->expandAxis<0>(m_zeroLineLinear);
				data::autoscaleAxes();
			}
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			X m_zeroLineLinear;
			decltype(X()/X()) m_zeroLineLogged;
		};


		template<class XCONTAINER, class YCONTAINER, class X, class Y>
		auto makeHorizontalBars(const XCONTAINER& xs, const YCONTAINER& ys, const YCONTAINER& widths, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, X zeroLine = X(0.0))
			requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
		{
			return std::make_shared<HorizontalBars<X, Y>>(xs, ys, widths, xAxis, yAxis, lineStyle, fillStyle, zeroLine);
		}

		template<class X, class Y>
		class Boxes : public Data<X, Y,
			std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<Axis<Y>>>,
			GridData<X, 1>, GridData<X, 1>, GridData<Y, 1>, GridData<Y, 1>>
		{
		public:
			using data = Data<X, Y,
				std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<Axis<Y>>>,
				GridData<X, 1>, GridData<X, 1>, GridData<Y, 1>, GridData<Y, 1>>;

			template<class XCONTAINER, class YCONTAINER>
			Boxes(const XCONTAINER& x1s, const XCONTAINER& x2s, const YCONTAINER& y1s, const YCONTAINER& y2s, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<X>> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle)
				requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
				: data(xAxis, yAxis, std::make_tuple(xAxis, xAxis, yAxis, yAxis), x1s, x2s, y1s, y2s)
			{
				//a note on the above - the result of xs-0.5*widths and xs+0.5*widths is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address. 
				m_lineStyle = lineStyle;
				m_fillStyle = fillStyle;
			}

			void plotData(const SpacialAxesSet<X, Y>& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				renderer.setPen(m_lineStyle.getColour(), m_lineStyle.getWidth(), m_lineStyle.getPattern());
				renderer.setBrush(m_fillStyle.getColour());

				for (size_t i = 0; i < this->getNPoints<0>(); ++i)
				{
					Point p1 = this->getPoint<0, 2>(i, axisSet);
					Point p2 = this->getPoint<1, 3>(i, axisSet);
					renderer.rectangle(p1, p2);
				}
			}

		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
		};

		template<class XCONTAINER, class YCONTAINER, class X, class Y>
		auto makeBoxes(const XCONTAINER& x1s, const XCONTAINER& x2s, const YCONTAINER& y1s, const YCONTAINER& y2s, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<X>> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle)
			requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
		{
			return std::make_shared<Boxes<X, Y>>(x1s, x2s, y1s, y2s, xAxis, yAxis, lineStyle, fillStyle);
		}

		template<class X, class Y>
		class Fill : public Data<X, Y,
			std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>>,
			GridData<X, 1>, GridData<Y, 1>>
		{
		public:
			using data = Data<X, Y,
				std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>>,
				GridData<X, 1>, GridData<Y, 1>>;

			template<class XCONTAINER, class YCONTAINER>
			Fill(const XCONTAINER xs, const YCONTAINER ys, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const FillStyle& fillStyle = FillStyle(), const LineStyle& outlineStyle = noLine)
				requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
				:data(xAxis, yAxis, std::make_tuple(xAxis, yAxis), xs, ys), m_fillStyle(fillStyle), m_lineStyle(outlineStyle)
			{
			}
			
			void plotData(const SpacialAxesSet<X, Y>& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				m_lineStyle.setPen(renderer);
				m_fillStyle.setBrush(renderer);

				std::vector<Point> points(this->getNPoints<0>());
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i] = this->getPoint<0,1>(i, axisSet);
				}
				renderer.polygon(points);
			}
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
		};

		template<class XCONTAINER, class YCONTAINER, class X, class Y>
		auto makeFill(const XCONTAINER xs, const YCONTAINER ys, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const FillStyle& fillStyle = FillStyle(), const LineStyle& outlineStyle = noLine)
			requires XYPlotable<XCONTAINER, YCONTAINER, X, Y>
		{
			return std::make_shared<Fill<X, Y>>(xs, ys, xAxis, yAxis, fillStyle, outlineStyle);
		}
	}
}


#endif
