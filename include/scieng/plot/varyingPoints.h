#ifndef SVECTOR_PLOTVARYINGPOINTS_H
#define SVECTOR_PLOTVARYINGPOINTS_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		class PointsColourVarying : public Data<std::vector<double>, std::vector<double>, std::vector<double>>
		{
		public:
			PointsColourVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zs, const std::shared_ptr<Axis> xAxis, const std::shared_ptr<Axis> yAxis, const std::shared_ptr < ColourScale<double>> colourScale, const Symbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: Data<std::vector<double>, std::vector<double>, std::vector<double>>(xAxis, yAxis, std::make_tuple(xAxis, yAxis, colourScale), transformer, xs, ys, zs), m_symbol(symbol), m_colourScale(colourScale)
			{
			}
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				const std::vector<double>& x = getData<0>(axisSetIndex);
				const std::vector<double>& y = getData<1>(axisSetIndex);
				if (m_colourScale->isLog())
				{
					const auto &z = getData<2>(axisSetIndex);
					for (size_t i = 0; i < getNPoints(); ++i)
					{
						renderer.setBrush(m_colourScale->getRgbLog(z[i], true));
						renderer.setPen(rgbcolour(), millimetre(0.0));
						m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), renderer);
					}
				}
				else
				{
					const auto& z = getData<2>(axisSetIndex);
					for (size_t i = 0; i < getNPoints(); ++i)
					{
						renderer.setBrush(m_colourScale->getRgbLinear(z[i]));
						renderer.setPen(rgbcolour(), millimetre(0.0));
						m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), renderer);
					}
				}
			}
		private:
			Symbol m_symbol;
			const std::shared_ptr<ColourScale<double>> m_colourScale;
		};

		class PointsSizeVarying : public Data<std::vector<double>, std::vector<double>, std::vector<double>>
		{
		public:
			PointsSizeVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zs, const std::shared_ptr<Axis> xAxis, const std::shared_ptr<Axis> yAxis, const std::shared_ptr<SizeScale> sizeScale, const Symbol& symbol, sci::graphics::RgbColour colour, std::shared_ptr<splotTransformer> transformer = nullptr)
				: Data<std::vector<double>, std::vector<double>, std::vector<double>>(xAxis, yAxis, std::make_tuple(xAxis, yAxis, sizeScale), transformer, xs, ys, zs ), m_symbol(symbol), m_sizeScale(sizeScale), m_colour(colour)
			{
			}
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setBrush(m_colour);
				renderer.setPen(rgbcolour(), millimetre(0.0));
				const std::vector<double>& x = getData<0>(axisSetIndex);
				const std::vector<double>& y = getData<1>(axisSetIndex);
				if (m_sizeScale->isLog())
				{
					const auto& z = getData<2>(axisSetIndex);
					for (size_t i = 0; i < getNPoints(); ++i)
					{
						double size = m_sizeScale->getSizeLog(z[i], true);
						m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
					}
				}
				else
				{
					const auto& z = getData<2>(axisSetIndex);
					for (size_t i = 0; i < getNPoints(); ++i)
					{
						double size = m_sizeScale->getSizeLinear(z[i]);
						m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
					}
				}
			}
		private:
			Symbol m_symbol;
			sci::graphics::RgbColour m_colour;
			const std::shared_ptr<SizeScale> m_sizeScale;
		};

		class PointsColourAndSizeVarying : public Data<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>>
		{
		public:
			PointsColourAndSizeVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zsColour, std::span<const double> zsSize, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, const std::shared_ptr < ColourScale<double>> colourScale, const std::shared_ptr<SizeScale> sizeScale, const Symbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: Data<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>>(xAxis, yAxis, std::make_tuple(xAxis, yAxis, colourScale, sizeScale), transformer, xs, ys, zsColour, zsSize ), m_symbol(symbol), m_colourScale(colourScale), m_sizeScale(sizeScale)
			{
			}
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				const std::vector<double>& x = getData<0>(axisSetIndex);
				const std::vector<double>& y = getData<1>(axisSetIndex);
				if (m_colourScale->isLog())
				{
					const auto& z1 = getData<2>(axisSetIndex);
					if (m_sizeScale->isLog())
					{
						const auto& z2 = getData<3>(axisSetIndex);
						for (size_t i = 0; i < getNPoints(); ++i)
						{
							renderer.setBrush(m_colourScale->getRgbLog(z1[i], true));
							renderer.setPen(rgbcolour(), millimetre(0.0));
							double size = m_sizeScale->getSizeLog(z2[i], true);
							m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
						}
					}
					else
					{
						const auto& z2 = getData<3>(axisSetIndex);
						for (size_t i = 0; i < getNPoints(); ++i)
						{
							renderer.setBrush(m_colourScale->getRgbLog(z1[i], true));
							renderer.setPen(rgbcolour(), millimetre(0.0));
							double size = m_sizeScale->getSizeLinear(z2[i]);
							m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
						}
					}
				}
				else
				{
					const auto& z1 = getData<2>(axisSetIndex);
					if (m_sizeScale->isLog())
					{
						const auto& z2 = getData<3>(axisSetIndex);
						for (size_t i = 0; i < getNPoints(); ++i)
						{
							renderer.setBrush(m_colourScale->getRgbLinear(z1[i]));
							renderer.setPen(rgbcolour(), millimetre(0.0));
							double size = m_sizeScale->getSizeLog(z2[i], true);
							m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
						}
					}
					else
					{
						const auto& z2 = getData<3>(axisSetIndex);
						for (size_t i = 0; i < getNPoints(); ++i)
						{
							renderer.setBrush(m_colourScale->getRgbLinear(z1[i]));
							renderer.setPen(rgbcolour(), millimetre(0.0));
							double size = m_sizeScale->getSizeLinear(z2[i]);
							m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
						}
					}

				}
			}
		private:
			Symbol m_symbol;
			const std::shared_ptr<ColourScale<double>> m_colourScale;
			const std::shared_ptr<SizeScale> m_sizeScale;
		};
	}
}


#endif
