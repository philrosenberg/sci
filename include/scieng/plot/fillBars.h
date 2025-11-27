#ifndef SVECTOR_PLOTFILLBAR_H
#define SVECTOR_PLOTFILLBAR_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		class VerticalBars : public Data<std::vector<double>, std::vector<double>, std::vector<double>>
		{
		public:
			VerticalBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> widths, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, double zeroLine = 0.0, std::shared_ptr<splotTransformer> transformer = nullptr)
				: Data<std::vector<double>, std::vector<double>, std::vector<double>>(xAxis, yAxis, std::make_tuple(xAxis, xAxis, yAxis), transformer, (xs | sci::views::grid<1>) - 0.5 * (widths | sci::views::grid<1>), (xs | sci::views::grid<1>) + 0.5 * (widths | sci::views::grid<1>), ys)
			{
				//a note on the above - the result of xs-0.5*widths and xs+0.5*widths is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address. 
				m_lineStyle = lineStyle;
				m_fillStyle = fillStyle;
				m_zeroLineLinear = zeroLine;
				m_zeroLineLogged = zeroLine > 0.0 ? std::log(zeroLine) : std::numeric_limits<double>::quiet_NaN();
			}

			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_lineStyle.getColour(), m_lineStyle.getWidth(), m_lineStyle.getPattern());
				renderer.setBrush(m_fillStyle.getColour());

				double zeroLine = getYAxis(axisSetIndex)->isLog() ? m_zeroLineLogged : m_zeroLineLinear;

				const std::vector<double>& minXs = getData<0>(axisSetIndex);
				const std::vector<double>& maxXs = getData<1>(axisSetIndex);
				const std::vector<double>& ys = getData<2>(axisSetIndex);

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
				Data<std::vector<double>, std::vector<double>, std::vector<double>>::autoscaleAxes(axisSetIndex);
			}
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			double m_zeroLineLinear;
			double m_zeroLineLogged;
		};

		class HorizontalBars : public Data<std::vector<double>, std::vector<double>, std::vector<double>>
		{
		public:
			HorizontalBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> widths, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, double zeroLine = 0.0, std::shared_ptr<splotTransformer> transformer = nullptr)
				: Data<std::vector<double>, std::vector<double>, std::vector<double>>(xAxis, yAxis, std::make_tuple(xAxis, xAxis, yAxis), transformer, xs, (ys | sci::views::grid<1>) - 0.5 * (widths | sci::views::grid<1>), (ys | sci::views::grid<1>) + 0.5 * (widths | sci::views::grid<1>))
			{
				//a note on the above - the result of xs-0.5*widths and xs+0.5*widths is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address. 
				m_lineStyle = lineStyle;
				m_fillStyle = fillStyle;
				m_zeroLineLinear = zeroLine;
				m_zeroLineLogged = zeroLine > 0.0 ? std::log(zeroLine) : std::numeric_limits<double>::quiet_NaN();
			}

			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_lineStyle.getColour(), m_lineStyle.getWidth(), m_lineStyle.getPattern());
				renderer.setBrush(m_fillStyle.getColour());

				double zeroLine = getXAxis(axisSetIndex)->isLog() ? m_zeroLineLogged : m_zeroLineLinear;

				const std::vector<double>& xs = getData<0>(axisSetIndex);
				const std::vector<double>& minYs = getData<1>(axisSetIndex);
				const std::vector<double>& maxYs = getData<2>(axisSetIndex);

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
				Data<std::vector<double>, std::vector<double>, std::vector<double>>::autoscaleAxes(axisSetIndex);
			}
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			double m_zeroLineLinear;
			double m_zeroLineLogged;
		};

		class Boxes : public Data<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>>
		{
		public:
			Boxes(std::span<const double> x1s, std::span<const double> x2s, std::span<const double> y1s, std::span<const double> y2s, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: Data<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>>(xAxis, yAxis, std::make_tuple(xAxis, xAxis, yAxis, yAxis), transformer, x1s, x2s, y1s, y2s)
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


				const std::vector<double>& x1s = getData<0>(axisSetIndex);
				const std::vector<double>& x2s = getData<1>(axisSetIndex);
				const std::vector<double>& y1s = getData<2>(axisSetIndex);
				const std::vector<double>& y2s = getData<3>(axisSetIndex);

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

		class Fill : public Data<std::vector<double>, std::vector<double>>
		{
		public:
			Fill(std::span<const double> xs, std::span<const double> ys, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const FillStyle& fillStyle = FillStyle(), const LineStyle& outlineStyle = noLine, std::shared_ptr<splotTransformer> transformer = nullptr)
				: Data<std::vector<double>, std::vector<double>>(xAxis, yAxis, std::make_tuple(xAxis, yAxis), transformer, xs, ys), m_fillStyle(fillStyle), m_lineStyle(outlineStyle)
			{
			}
			
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;
				m_lineStyle.setPen(renderer);
				m_fillStyle.setBrush(renderer);

				std::vector<Point> points(getNPoints());
				const std::vector<double>& x = getData<0>(axisSetIndex);
				const std::vector<double>& y = getData<1>(axisSetIndex);
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i] = getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex);
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
