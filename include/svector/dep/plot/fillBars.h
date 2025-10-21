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
			VerticalBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> widths, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const LineStyle& lineStyle, const FillStyle& fillStyle, double zeroLine = 0.0, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			double m_zeroLine;
			double m_zeroLineLogged;
		};

		class FillData : public UnstructuredData
		{
		public:
			FillData(const std::vector<double>& xs, const std::vector<double>& ys, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const FillStyle& fillStyle = FillStyle(), const LineStyle& outlineStyle = noLine, std::shared_ptr<splotTransformer> transformer = nullptr);
			void plotData(plstream* pl, double scale) const override;
			void plotData(Renderer& renderer, grPerMillimetre scale) const override;
		private:
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
		};
	}
}


#endif
