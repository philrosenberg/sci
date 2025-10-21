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
			HorizontalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle style, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
		private:
			LineStyle m_style;
		};

		class VerticalErrorBars : public UnstructuredData
		{
		public:
			VerticalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle style, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
		private:
			LineStyle m_style;
		};
	}
}


#endif
