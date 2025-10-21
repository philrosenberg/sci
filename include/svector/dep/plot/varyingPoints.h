#ifndef SVECTOR_PLOTVARYINGPOINTS_H
#define SVECTOR_PLOTVARYINGPOINTS_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		class PointDataColourVarying : public UnstructuredData
		{
		public:
			PointDataColourVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const ColourVaryingSymbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys, zs }, std::vector<std::shared_ptr<sci::plot::Scale>>{xAxis, yAxis, std::shared_ptr<sci::plot::Scale>(symbol.getColourscale())}, transformer), m_symbol(symbol)
			{
			}
			void plotData(plstream* pl, double scale) const override;
		private:
			ColourVaryingSymbol m_symbol;
		};

		class PointDataSizeVarying : public UnstructuredData
		{
		public:
			PointDataSizeVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const SizeVaryingSymbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys, zs }, std::vector<std::shared_ptr<sci::plot::Scale>>{xAxis, yAxis, std::shared_ptr<sci::plot::Scale>(symbol.getSizeScale())}, transformer), m_symbol(symbol)
			{
			}
			void plotData(plstream* pl, double scale) const override;
		private:
			SizeVaryingSymbol m_symbol;
		};

		class PointDataColourAndSizeVarying : public UnstructuredData
		{
		public:
			PointDataColourAndSizeVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zsColour, std::span<const double> zsSize, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const ColourAndSizeVaryingSymbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys, zsColour, zsSize }, std::vector<std::shared_ptr<sci::plot::Scale>>{xAxis, yAxis, std::shared_ptr<sci::plot::Scale>(symbol.getColourscale()), std::shared_ptr<sci::plot::Scale>(symbol.getSizeScale())}, transformer), m_symbol(symbol)
			{
			}
			void plotData(plstream* pl, double scale) const override;
		private:
			ColourAndSizeVaryingSymbol m_symbol;
		};
	}
}


#endif
