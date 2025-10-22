#ifndef SVECTOR_PLOTERRORBARS_H
#define SVECTOR_PLOTERRORBARS_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		class HorizontalErrorBars : public UnstructuredData
		{
		public:
			HorizontalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys, plusErrors, minusErrors},{ xAxis, yAxis, xAxis, xAxis }, transformer)
			{
				//a note on the above - the result of xs+plusError and xs-minusError is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address.
				m_style = style;
				m_stopLength = stopLength;
				m_useForAutoscale = useForAutoscale;
			}
			void plotData(plstream* pl, double scale) const override;
			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_style.getColour(), m_style.getWidth(), m_style.getPattern());

				const std::vector<double>& xs = getVector(0);
				const std::vector<double>& ys = getVector(1);
				const std::vector<double>& plusErrors = getVector(2);
				const std::vector<double>& minusErrors = getVector(3);
				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point pCentre = getPointFromLoggedIfNeededData(xs[i], ys[i]);
					Point p1 = getPointFromLoggedIfNeededData(xs[i]-minusErrors[i], ys[i]);
					Point p2 = getPointFromLoggedIfNeededData(xs[i] + plusErrors[i], ys[i]);
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

				const std::vector<std::shared_ptr<Scale>>& axes = getAxes();

				const std::vector<double>& xs = getVector(0);
				const std::vector<double>& ys = getVector(1);
				const std::vector<double>& plusErrors = getVector(2);
				const std::vector<double>& minusErrors = getVector(3);

				if (axes[0] && axes[0]->isAutoscale())
				{
					for (size_t i = 0; i < xs.size(); ++i)
					{
						axes[0]->expand(xs[i] + plusErrors[i]);
						axes[0]->expand(xs[i] - minusErrors[i]);
					}
				}
				if (axes[1] && axes[1]->isAutoscale())
					axes[1]->expand(ys);
			}
		private:
			LineStyle m_style;
			Length m_stopLength;
			bool m_useForAutoscale;
		};

		class VerticalErrorBars : public UnstructuredData
		{
		public:
			VerticalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, Length stopLength, const LineStyle style = sci::plot::LineStyle(), bool useForAutoscale = true, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys, plusErrors, minusErrors },{ xAxis, yAxis, yAxis, yAxis }, transformer)
			{
				//a note on the above - the result of ys+plusError and ys-minusError is an r-value, meaning we
				//can't directly take it's address. However, when we assign it to a const reference the temporary's
				//lifetime is extended to the lifetime of the reference, so then we can legally get it's address.
				m_style = style;
				m_stopLength = stopLength;
				m_useForAutoscale = useForAutoscale;
			}
			void plotData(plstream* pl, double scale) const override;

			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setPen(m_style.getColour(), m_style.getWidth(), m_style.getPattern());

				const std::vector<double>& xs = getVector(0);
				const std::vector<double>& ys = getVector(1);
				const std::vector<double>& plusErrors = getVector(2);
				const std::vector<double>& minusErrors = getVector(3);

				for (size_t i = 0; i < getNPoints(); ++i)
				{
					Point pCentre = getPointFromLoggedIfNeededData(xs[i], ys[i]);
					Point p1 = getPointFromLoggedIfNeededData(xs[i], ys[i] - minusErrors[i]);
					Point p2 = getPointFromLoggedIfNeededData(xs[i], ys[i] + plusErrors[i]);
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

				const std::vector<std::shared_ptr<Scale>>& axes = getAxes();

				const std::vector<double>& xs = getVector(0);
				const std::vector<double>& ys = getVector(1);
				const std::vector<double>& plusErrors = getVector(2);
				const std::vector<double>& minusErrors = getVector(3);

				if (axes[0] && axes[0]->isAutoscale())
					axes[0]->expand(xs);
				if (axes[1] && axes[1]->isAutoscale())
				{
					for (size_t i = 0; i < xs.size(); ++i)
					{
						axes[1]->expand(ys[i] + plusErrors[i]);
						axes[1]->expand(ys[i] - minusErrors[i]);
					}
				}
			}
		private:
			LineStyle m_style;
			Length m_stopLength;
			bool m_useForAutoscale;
		};
	}
}


#endif
