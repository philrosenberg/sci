#ifndef SVECTOR_PLOTFILLBAR_H
#define SVECTOR_PLOTFILLBAR_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		class VerticalBars : public Bars
		{
		public:
			VerticalBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> widths, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, double zeroLine = 0.0, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), m_yAxis(yAxis),
				Bars(xs, ys, half(widths), half(widths), xAxis, yAxis, true, transformer)
			{
				//a note on the above - the result of xs-0.5*widths and xs+0.5*widths is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address. 
				m_lineStyle = lineStyle;
				m_fillStyle = fillStyle;
				m_zeroLineLinear = zeroLine;
				m_zeroLineLogged = zeroLine > 0.0 ? std::log(zeroLine) : std::numeric_limits<double>::quiet_NaN();
			}

			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_lineStyle.getColour(), m_lineStyle.getWidth(), m_lineStyle.getPattern());
				renderer.setBrush(m_fillStyle.getColour());

				double zeroLine = m_yAxis->isLog() ? m_zeroLineLogged : m_zeroLineLinear;

				const std::vector<double>& minXs = getMinAlongData();
				const std::vector<double>& maxXs = getMaxAlongData();
				const std::vector<double>& ys = getAcrossData();

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point p1 = getPointFromLoggedIfNeededData(minXs[i], zeroLine);
					Point p2 = getPointFromLoggedIfNeededData(maxXs[i], ys[i]);
					renderer.rectangle(p1, p2);
				}
			}
			virtual void autoscaleAxes() override
			{
				m_yAxis->expand(m_zeroLineLinear);
				Bars::autoscaleAxes();
			}
		private:
			static std::vector<double> half(const std::span<const double> values)
			{
				std::vector<double> result(values.begin(), values.end());
				for (auto& r : result)
					r = 0.5 * r;
				return result;
			}
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			double m_zeroLineLinear;
			double m_zeroLineLogged;
			std::shared_ptr<Scale> m_yAxis;
		};

		class HorizontalBars : public Bars
		{
		public:
			HorizontalBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> widths, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, double zeroLine = 0.0, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), m_xAxis(xAxis),
				Bars(ys, xs, half(widths), half(widths), yAxis, xAxis, true, transformer)
			{
				//a note on the above - the result of xs-0.5*widths and xs+0.5*widths is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address. 
				m_lineStyle = lineStyle;
				m_fillStyle = fillStyle;
				m_zeroLineLinear = zeroLine;
				m_zeroLineLogged = zeroLine > 0.0 ? std::log(zeroLine) : std::numeric_limits<double>::quiet_NaN();
			}

			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_lineStyle.getColour(), m_lineStyle.getWidth(), m_lineStyle.getPattern());
				renderer.setBrush(m_fillStyle.getColour());

				double zeroLine = m_xAxis->isLog() ? m_zeroLineLogged : m_zeroLineLinear;

				const std::vector<double>& minYs = getMinAlongData();
				const std::vector<double>& maxYs = getMaxAlongData();
				const std::vector<double>& xs = getAcrossData();

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point p1 = getPointFromLoggedIfNeededData(zeroLine, minYs[i]);
					Point p2 = getPointFromLoggedIfNeededData(xs[i], maxYs[i]);
					renderer.rectangle(p1, p2);
				}
			}

			virtual void autoscaleAxes() override
			{
				m_xAxis->expand(m_zeroLineLinear);
				Bars::autoscaleAxes();
			}
		private:
			static std::vector<double> half(const std::span<const double> values)
			{
				std::vector<double> result(values.begin(), values.end());
				for (auto& r : result)
					r = 0.5 * r;
				return result;
			}
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			double m_zeroLineLinear;
			double m_zeroLineLogged;
			std::shared_ptr<Scale> m_xAxis;
		};

		class Fill : public Data<std::vector<double>, std::vector<double>>
		{
		public:
			Fill(std::span<const double> xs, std::span<const double> ys, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const FillStyle& fillStyle = FillStyle(), const LineStyle& outlineStyle = noLine, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer),
				Data<std::vector<double>, std::vector<double>>({ xAxis, yAxis }, transformer, xs, ys), m_fillStyle(fillStyle), m_lineStyle(outlineStyle)
			{
			}
			
			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;
				m_lineStyle.setPen(renderer);
				m_fillStyle.setBrush(renderer);

				std::vector<Point> points(getNPoints());
				const std::vector<double>& x = getData<0>();
				const std::vector<double>& y = getData<1>();
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i] = getPointFromLoggedIfNeededData(x[i], y[i]);
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
