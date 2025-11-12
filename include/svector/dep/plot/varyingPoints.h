#ifndef SVECTOR_PLOTVARYINGPOINTS_H
#define SVECTOR_PLOTVARYINGPOINTS_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		class PointsColourVarying : public UnstructuredData
		{
		public:
			PointsColourVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zs, const std::shared_ptr<Axis> xAxis, const std::shared_ptr<Axis> yAxis, const std::shared_ptr < ColourScale> colourScale, const Symbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys, zs }, std::vector<std::shared_ptr<sci::plot::Scale>>{xAxis, yAxis, colourScale}, transformer), m_symbol(symbol), m_colourScale(colourScale)
			{
			}
			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				const std::vector<double>& x = getVector(0);
				const std::vector<double>& y = getVector(1);
				const std::vector<double>& z = getVector(2);
				for (size_t i = 0; i < getNPoints(); ++i)
				{
					renderer.setBrush(m_colourScale->getRgbOriginalScale(z[i], true));
					renderer.setPen(rgbcolour(), grMillimetre(0.0));
					m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i]), renderer);
				}
			}
		private:
			Symbol m_symbol;
			const std::shared_ptr<ColourScale> m_colourScale;
		};

		class PointsSizeVarying : public UnstructuredData
		{
		public:
			PointsSizeVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zs, const std::shared_ptr<Axis> xAxis, const std::shared_ptr<Axis> yAxis, const std::shared_ptr<SizeScale> sizeScale, const Symbol& symbol, sci::graphics::RgbColour colour, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys, zs }, std::vector<std::shared_ptr<sci::plot::Scale>>{xAxis, yAxis, sizeScale}, transformer), m_symbol(symbol), m_sizeScale(sizeScale), m_colour(colour)
			{
			}
			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setBrush(m_colour);
				renderer.setPen(rgbcolour(), grMillimetre(0.0));
				const std::vector<double>& x = getVector(0);
				const std::vector<double>& y = getVector(1);
				const std::vector<double>& z = getVector(2);
				for (size_t i = 0; i < getNPoints(); ++i)
				{
					double size = m_sizeScale->getsize(z[i], true);
					m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i]), graphics::unitless(size), renderer);
				}
			}
		private:
			Symbol m_symbol;
			sci::graphics::RgbColour m_colour;
			const std::shared_ptr<SizeScale> m_sizeScale;
		};

		class PointsColourAndSizeVarying : public UnstructuredData
		{
		public:
			PointsColourAndSizeVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zsColour, std::span<const double> zsSize, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const std::shared_ptr < ColourScale> colourScale, const std::shared_ptr<SizeScale> sizeScale, const Symbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys, zsColour, zsSize }, std::vector<std::shared_ptr<sci::plot::Scale>>{xAxis, yAxis, colourScale, sizeScale}, transformer), m_symbol(symbol), m_colourScale(colourScale), m_sizeScale(sizeScale)
			{
			}
			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!hasData())
					return;

				const std::vector<double>& x = getVector(0);
				const std::vector<double>& y = getVector(1);
				const std::vector<double>& z1 = getVector(2);
				const std::vector<double>& z2 = getVector(3);
				for (size_t i = 0; i < getNPoints(); ++i)
				{
					renderer.setBrush(m_colourScale->getRgbOriginalScale(z1[i], true));
					renderer.setPen(rgbcolour(), grMillimetre(0.0));
					double size = m_sizeScale->getsize(z2[i], true);
					m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i]), graphics::unitless(size), renderer);
				}
			}
		private:
			Symbol m_symbol;
			const std::shared_ptr<ColourScale> m_colourScale;
			const std::shared_ptr<SizeScale> m_sizeScale;
		};
	}
}


#endif
