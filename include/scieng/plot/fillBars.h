#ifndef SVECTOR_PLOTFILLBAR_H
#define SVECTOR_PLOTFILLBAR_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		template<class X, class Y>
		class VerticalBars : public Data<X, Y, std::vector<X>, std::vector<X>, std::vector<Y>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<X>, std::vector<Y>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;
			using data::getYAxis;
			VerticalBars(std::span<const X> xs, std::span<const Y> ys, std::span<const X> widths, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, Y zeroLine = 0.0)
				: data(xAxis, yAxis, std::make_tuple(xAxis, xAxis, yAxis), (xs | sci::views::grid<1>) - 0.5 * (widths | sci::views::grid<1>), (xs | sci::views::grid<1>) + 0.5 * (widths | sci::views::grid<1>), ys)
			{
				//a note on the above - the result of xs-0.5*widths and xs+0.5*widths is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address. 
				m_lineStyle = lineStyle;
				m_fillStyle = fillStyle;
				m_zeroLineLinear = zeroLine;
				m_zeroLineLogged = zeroLine > 0.0 ? std::log(zeroLine) : std::numeric_limits<decltype(Y()/Y())>::quiet_NaN();
			}

			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_lineStyle.getColour(), m_lineStyle.getWidth(), m_lineStyle.getPattern());
				renderer.setBrush(m_fillStyle.getColour());

				Y zeroLine = getYAxis(axisSetIndex)->isLog() ? m_zeroLineLogged : m_zeroLineLinear;

				const std::vector<X>& minXs = data::getData<0>(axisSetIndex);
				const std::vector<X>& maxXs = data::getData<1>(axisSetIndex);
				const std::vector<Y>& ys = data::getData<2>(axisSetIndex);

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point p1 = getPointFromLoggedIfNeededData(minXs[i], zeroLine, axisSetIndex);
					Point p2 = getPointFromLoggedIfNeededData(maxXs[i], ys[i], axisSetIndex);
					renderer.rectangle(p1, p2);
				}
			}
			virtual void autoscaleAxes(size_t axisSetIndex) override
			{
				getYAxis(axisSetIndex)->expand(m_zeroLineLinear);
				data::autoscaleAxes(axisSetIndex);
			}
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			Y m_zeroLineLinear;
			decltype(Y()/Y()) m_zeroLineLogged;
		};

		template<class X, class Y>
		class HorizontalBars : public Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<Y>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<Y>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;
			using data::getXAxis;
			HorizontalBars(std::span<const X> xs, std::span<const Y> ys, std::span<const Y> widths, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, X zeroLine = 0.0)
				: data(xAxis, yAxis, std::make_tuple(xAxis, xAxis, yAxis), xs, (ys | sci::views::grid<1>) - 0.5 * (widths | sci::views::grid<1>), (ys | sci::views::grid<1>) + 0.5 * (widths | sci::views::grid<1>))
			{
				//a note on the above - the result of xs-0.5*widths and xs+0.5*widths is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address. 
				m_lineStyle = lineStyle;
				m_fillStyle = fillStyle;
				m_zeroLineLinear = zeroLine;
				m_zeroLineLogged = zeroLine > 0.0 ? std::log(zeroLine) : std::numeric_limits<decltype(X()/X())>::quiet_NaN();
			}

			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_lineStyle.getColour(), m_lineStyle.getWidth(), m_lineStyle.getPattern());
				renderer.setBrush(m_fillStyle.getColour());

				X zeroLine = getXAxis(axisSetIndex)->isLog() ? m_zeroLineLogged : m_zeroLineLinear;

				const std::vector<X>& xs = data::getData<0>(axisSetIndex);
				const std::vector<Y>& minYs = data::getData<1>(axisSetIndex);
				const std::vector<Y>& maxYs = data::getData<2>(axisSetIndex);

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point p1 = getPointFromLoggedIfNeededData(zeroLine, minYs[i], axisSetIndex);
					Point p2 = getPointFromLoggedIfNeededData(xs[i], maxYs[i], axisSetIndex);
					renderer.rectangle(p1, p2);
				}
			}

			virtual void autoscaleAxes(size_t axisSetIndex) override
			{
				getXAxis(axisSetIndex)->expand(m_zeroLineLinear);
				data::autoscaleAxes(axisSetIndex);
			}
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			X m_zeroLineLinear;
			decltype(X()/X()) m_zeroLineLogged;
		};

		template<class X, class Y>
		class Boxes : public Data<X, Y, std::vector<X>, std::vector<X>, std::vector<Y>, std::vector<Y>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<X>, std::vector<Y>, std::vector<Y>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;
			Boxes(std::span<const X> x1s, std::span<const X> x2s, std::span<const Y> y1s, std::span<const Y> y2s, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<X>> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle)
				: data(xAxis, yAxis, std::make_tuple(xAxis, xAxis, yAxis, yAxis), x1s, x2s, y1s, y2s)
			{
				//a note on the above - the result of xs-0.5*widths and xs+0.5*widths is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address. 
				m_lineStyle = lineStyle;
				m_fillStyle = fillStyle;
			}

			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_lineStyle.getColour(), m_lineStyle.getWidth(), m_lineStyle.getPattern());
				renderer.setBrush(m_fillStyle.getColour());


				const std::vector<X>& x1s = data::getData<0>(axisSetIndex);
				const std::vector<X>& x2s = data::getData<1>(axisSetIndex);
				const std::vector<Y>& y1s = data::getData<2>(axisSetIndex);
				const std::vector<Y>& y2s = data::getData<3>(axisSetIndex);

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point p1 = getPointFromLoggedIfNeededData(x1s[i], y1s[i], axisSetIndex);
					Point p2 = getPointFromLoggedIfNeededData(x2s[i], y2s[i], axisSetIndex);
					renderer.rectangle(p1, p2);
				}
			}

		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
		};

		template<class X, class Y>
		class Fill : public Data<X, Y, std::vector<X>, std::vector<Y>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<Y>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;
			Fill(std::span<const X> xs, std::span<const Y> ys, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const FillStyle& fillStyle = FillStyle(), const LineStyle& outlineStyle = noLine)
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis), xs, ys), m_fillStyle(fillStyle), m_lineStyle(outlineStyle)
			{
			}
			
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;
				m_lineStyle.setPen(renderer);
				m_fillStyle.setBrush(renderer);

				const std::vector<X>& xs = data::getData<0>(axisSetIndex);
				const std::vector<X>& ys = data::getData<1>(axisSetIndex);

				std::vector<Point> points(getNPoints());
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i] = getPointFromLoggedIfNeededData(xs[i], ys[i], axisSetIndex);
				}
				renderer.polygon(points);
			}
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
		};
	}
}


#endif
