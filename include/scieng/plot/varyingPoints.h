#ifndef SVECTOR_PLOTVARYINGPOINTS_H
#define SVECTOR_PLOTVARYINGPOINTS_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		template<class X, class Y, class Z>
		class PointsColourVarying : public Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<Z>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<Z>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;
			PointsColourVarying(std::span<const X> xs, std::span<const Y> ys, std::span<const Z> zs, const std::shared_ptr<Axis<X>> xAxis, const std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr < ColourScale<Z>> colourScale, const Symbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, colourScale), transformer, xs, ys, zs), m_symbol(symbol), m_colourScale(colourScale)
			{
			}
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				const std::vector<X>& x = data::getData<0>(axisSetIndex);
				const std::vector<Y>& y = data::getData<1>(axisSetIndex);
				if (m_colourScale->isLog())
				{
					const auto &z = data::getData<2>(axisSetIndex);
					for (size_t i = 0; i < getNPoints(); ++i)
					{
						renderer.setBrush(m_colourScale->getRgbLog(z[i]));
						renderer.setPen(rgbcolour(), millimetre(0.0));
						m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), renderer);
					}
				}
				else
				{
					const auto& z = data::getData<2>(axisSetIndex);
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
			const std::shared_ptr<ColourScale<Z>> m_colourScale;
		};

		template<class X, class Y, class Z>
		class PointsSizeVarying : public Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<Z>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<Z>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;
			PointsSizeVarying(std::span<const X> xs, std::span<const Y> ys, std::span<const Z> zs, const std::shared_ptr<Axis<Z>> xAxis, const std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr<SizeScale<Z>> sizeScale, const Symbol& symbol, sci::graphics::RgbColour colour, std::shared_ptr<splotTransformer> transformer = nullptr)
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, sizeScale), transformer, xs, ys, zs ), m_symbol(symbol), m_sizeScale(sizeScale), m_colour(colour)
			{
			}
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setBrush(m_colour);
				renderer.setPen(rgbcolour(), millimetre(0.0));
				const std::vector<X>& x = data::getData<0>(axisSetIndex);
				const std::vector<Y>& y = data::getData<1>(axisSetIndex);
				if (m_sizeScale->isLog())
				{
					const auto& z = data::getData<2>(axisSetIndex);
					for (size_t i = 0; i < getNPoints(); ++i)
					{
						auto size = m_sizeScale->getSizeLog(z[i]);
						m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
					}
				}
				else
				{
					const auto& z = data::getData<2>(axisSetIndex);
					for (size_t i = 0; i < getNPoints(); ++i)
					{
						Z size = m_sizeScale->getSizeLinear(z[i]);
						m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
					}
				}
			}
		private:
			Symbol m_symbol;
			sci::graphics::RgbColour m_colour;
			const std::shared_ptr<SizeScale<Z>> m_sizeScale;
		};

		template<class X, class Y, class Z1, class Z2>
		class PointsColourAndSizeVarying : public Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<Z1>, std::vector<Z2>>
		{
		public:
			using data = Data<X, Y, std::vector<X>, std::vector<Y>, std::vector<Z1>, std::vector<Z2>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;
			PointsColourAndSizeVarying(std::span<const X> xs, std::span<const Y> ys, std::span<const Z1> zsColour, std::span<const Z2> zsSize, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr < ColourScale<Z1>> colourScale, const std::shared_ptr<SizeScale<Z2>> sizeScale, const Symbol& symbol, std::shared_ptr<splotTransformer> transformer = nullptr)
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, colourScale, sizeScale), transformer, xs, ys, zsColour, zsSize ), m_symbol(symbol), m_colourScale(colourScale), m_sizeScale(sizeScale)
			{
			}
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				const std::vector<X>& x = data::getData<0>(axisSetIndex);
				const std::vector<Y>& y = data::getData<1>(axisSetIndex);
				if (m_colourScale->isLog())
				{
					const auto& z1 = data::getData<2>(axisSetIndex);
					if (m_sizeScale->isLog())
					{
						const auto& z2 = data::getData<3>(axisSetIndex);
						for (size_t i = 0; i < getNPoints(); ++i)
						{
							renderer.setBrush(m_colourScale->getRgbLog(z1[i]));
							renderer.setPen(rgbcolour(), millimetre(0.0));
							auto size = m_sizeScale->getSizeLog(z2[i]);
							m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
						}
					}
					else
					{
						const auto& z2 = data::getData<3>(axisSetIndex);
						for (size_t i = 0; i < getNPoints(); ++i)
						{
							renderer.setBrush(m_colourScale->getRgbLog(z1[i]));
							renderer.setPen(rgbcolour(), millimetre(0.0));
							Z2 size = m_sizeScale->getSizeLinear(z2[i]);
							m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
						}
					}
				}
				else
				{
					const auto& z1 = data::getData<2>(axisSetIndex);
					if (m_sizeScale->isLog())
					{
						const auto& z2 = data::getData<3>(axisSetIndex);
						for (size_t i = 0; i < getNPoints(); ++i)
						{
							renderer.setBrush(m_colourScale->getRgbLinear(z1[i]));
							renderer.setPen(rgbcolour(), millimetre(0.0));
							auto size = m_sizeScale->getSizeLog(z2[i]);
							m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
						}
					}
					else
					{
						const auto& z2 = data::getData<3>(axisSetIndex);
						for (size_t i = 0; i < getNPoints(); ++i)
						{
							renderer.setBrush(m_colourScale->getRgbLinear(z1[i]));
							renderer.setPen(rgbcolour(), millimetre(0.0));
							Z2 size = m_sizeScale->getSizeLinear(z2[i]);
							m_symbol.draw(getPointFromLoggedIfNeededData(x[i], y[i], axisSetIndex), graphics::unitless(size), renderer);
						}
					}

				}
			}
		private:
			Symbol m_symbol;
			const std::shared_ptr<ColourScale<Z1>> m_colourScale;
			const std::shared_ptr<SizeScale<Z2>> m_sizeScale;
		};
	}
}


#endif
