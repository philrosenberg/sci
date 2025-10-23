#ifndef SVECTOR_PLOTFILLBAR_H
#define SVECTOR_PLOTFILLBAR_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		class VerticalBars : public UnstructuredData
		{
		public:
			VerticalBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> widths, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, double zeroLine = 0.0, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer),
				UnstructuredData({ xs, ys, widths},	{ xAxis, yAxis, xAxis }, transformer)
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

				double zeroLine = isLog(1) ? m_zeroLineLogged : m_zeroLineLinear;

				const std::vector<double>& xs = getVector(0);
				const std::vector<double>& ys = getVector(1);
				const std::vector<double>& widths = getVector(2);

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point p1 = getPointFromLoggedIfNeededData(xs[i] - 0.5 * widths[i], zeroLine);
					Point p2 = getPointFromLoggedIfNeededData(xs[i] + 0.5 * widths[i], ys[i]);
					renderer.rectangle(p1, p2);
				}
			}

			virtual void autoscaleAxes() override
			{
				const std::vector<std::shared_ptr<Scale>>& axes = getAxes();

				const std::vector<double>& xs = getVector(0);
				const std::vector<double>& ys = getVector(1);
				const std::vector<double>& widths = getVector(2);

				if (axes[0] && axes[0]->isAutoscale())
				{
					for (size_t i = 0; i < xs.size(); ++i)
					{
						axes[0]->expand(xs[i] + 0.5 * widths[i]);
						axes[0]->expand(xs[i] - 0.5 * widths[i]);
					}
				}
				if (axes[1] && axes[1]->isAutoscale())
				{
					axes[1]->expand(ys);
					axes[1]->expand(axes[1]->isLog() ? m_zeroLineLogged : m_zeroLineLinear);
				}

			}
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			double m_zeroLineLinear;
			double m_zeroLineLogged;
		};

		class HorizontalBars : public UnstructuredData
		{
		public:
			HorizontalBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> widths, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, double zeroLine = 0.0, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer),
				UnstructuredData({ xs, ys, widths }, { xAxis, yAxis, xAxis }, transformer)
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

				double zeroLine = isLog(0) ? m_zeroLineLogged : m_zeroLineLinear;

				const std::vector<double>& xs = getVector(0);
				const std::vector<double>& ys = getVector(1);
				const std::vector<double>& widths = getVector(2);

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point p1 = getPointFromLoggedIfNeededData(zeroLine, ys[i] - 0.5 * widths[i]);
					Point p2 = getPointFromLoggedIfNeededData(xs[i], ys[i] + 0.5 * widths[i]);
					renderer.rectangle(p1, p2);
				}
			}

			virtual void autoscaleAxes() override
			{
				const std::vector<std::shared_ptr<Scale>>& axes = getAxes();

				const std::vector<double>& xs = getVector(0);
				const std::vector<double>& ys = getVector(1);
				const std::vector<double>& widths = getVector(2);

				if (axes[0] && axes[0]->isAutoscale())
				{
					axes[0]->expand(xs);
					axes[0]->expand(axes[0]->isLog() ? m_zeroLineLogged : m_zeroLineLinear);
				}

				if (axes[1] && axes[1]->isAutoscale())
				{
					for (size_t i = 0; i < xs.size(); ++i)
					{
						axes[1]->expand(ys[i] + 0.5 * widths[i]);
						axes[1]->expand(ys[i] - 0.5 * widths[i]);
					}
				}

			}
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			double m_zeroLineLinear;
			double m_zeroLineLogged;
		};

		class Fill : public UnstructuredData
		{
		public:
			Fill(std::span<const double> xs, std::span<const double> ys, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const FillStyle& fillStyle = FillStyle(), const LineStyle& outlineStyle = noLine, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys }, { xAxis, yAxis }, transformer), m_fillStyle(fillStyle), m_lineStyle(outlineStyle)
			{
			}
			
			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;
				m_lineStyle.setPen(renderer);
				m_fillStyle.setBrush(renderer);

				std::vector<Point> points(getNPoints());
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i] = getPointFromLoggedIfNeededData(getPointer(0)[i], getPointer(1)[i]);
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
