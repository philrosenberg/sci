#ifndef SVECTOR_PLOTVARYINGPOINTS_H
#define SVECTOR_PLOTVARYINGPOINTS_H

#include"plotable.h"


namespace sci
{
	namespace plot
	{
		template<class X, class Y, class Z>
		class PointsColourVarying : public Data<X, Y, GridData<X, 1>, GridData<Y, 1>, GridData<Z, 1>>
		{
		public:
			using data = Data<X, Y, GridData<X, 1>, GridData<Y, 1>, GridData<Z, 1>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;
			
			template<class XCONTAINER, class YCONTAINER, class ZCONTAINER>
			PointsColourVarying(const XCONTAINER xs, const YCONTAINER &ys, ZCONTAINER zs, const std::shared_ptr<Axis<X>> xAxis, const std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr < ColourScale<Z>> colourScale, const Symbol& symbol)
				requires XYZPlotable<1, 1, 1, XCONTAINER, YCONTAINER, ZCONTAINER, X, Y, Z>
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, colourScale), xs, ys, zs), m_symbol(symbol), m_colourScale(colourScale)
			{
			}
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				const GridData<X, 1>& x = data::getData<0>(axisSetIndex);
				const GridData<Y, 1>& y = data::getData<1>(axisSetIndex);
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

		template<class XCONTAINER, class YCONTAINER, class ZCONTAINER, class X, class Y, class Z>
		auto makePointsColourVarying(const XCONTAINER xs, const YCONTAINER& ys, ZCONTAINER zs, const std::shared_ptr<Axis<X>> xAxis, const std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr < ColourScale<Z>> colourScale, const Symbol& symbol)
			requires XYZPlotable<1, 1, 1, XCONTAINER, YCONTAINER, ZCONTAINER, X, Y, Z>
		{
			return make_shared<PointsColourVarying<X, Y, Z>>(xs, ys, zs, xAxis, yAxis, colourScale, symbol);
		}

		template<class X, class Y, class Z>
		class PointsSizeVarying : public Data<X, Y, GridData<X, 1>, GridData<Y, 1>, GridData<Z, 1>>
		{
		public:
			using data = Data<X, Y, GridData<X, 1>, GridData<Y, 1>, GridData<Z, 1>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;

			template<class XCONTAINER, class YCONTAINER, class ZCONTAINER>
			PointsSizeVarying(const XCONTAINER& xs, const YCONTAINER& ys, const ZCONTAINER& zs, const std::shared_ptr<Axis<Z>> xAxis, const std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr<SizeScale<Z>> sizeScale, const Symbol& symbol, sci::graphics::RgbColour colour)
				requires XYZPlotable<1, 1, 1, XCONTAINER, YCONTAINER, ZCONTAINER, X, Y, Z>
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, sizeScale), xs, ys, zs ), m_symbol(symbol), m_sizeScale(sizeScale), m_colour(colour)
			{
			}
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				renderer.setBrush(m_colour);
				renderer.setPen(rgbcolour(), millimetre(0.0));
				const GridData<X, 1>& x = data::getData<0>(axisSetIndex);
				const GridData<Y, 1>& y = data::getData<1>(axisSetIndex);
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

		template<class XCONTAINER, class YCONTAINER, class ZCONTAINER, class X, class Y, class Z>
		auto makePointsSizeVarying(const XCONTAINER xs, const YCONTAINER& ys, ZCONTAINER zs, const std::shared_ptr<Axis<X>> xAxis, const std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr<SizeScale<Z>> sizeScale, const Symbol& symbol, sci::graphics::RgbColour colour)
			requires XYZPlotable<1, 1, 1, XCONTAINER, YCONTAINER, ZCONTAINER, X, Y, Z>
		{
			return make_shared<PointsSizeVarying<X, Y, Z>>(xs, ys, zs, xAxis, yAxis, sizeScale, symbol, colour);
		}

		template<class X, class Y, class Z1, class Z2>
		class PointsColourAndSizeVarying : public Data<X, Y, GridData<X, 1>, GridData<Y, 1>, GridData<Z1, 1>, GridData<Z2, 1>>
		{
		public:
			using data = Data<X, Y, GridData<X, 1>, GridData<Y, 1>, GridData<Z1, 1>, GridData<Z2, 1>>;
			using data::hasData;
			using data::getNPoints;
			using data::getPointFromLoggedIfNeededData;

			template<class XCONTAINER, class YCONTAINER, class Z1CONTAINER, class Z2CONTAINER>
			PointsColourAndSizeVarying(const XCONTAINER& xs, const YCONTAINER ys, const Z1CONTAINER& zsColour, const Z2CONTAINER& zsSize, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr < ColourScale<Z1>> colourScale, const std::shared_ptr<SizeScale<Z2>> sizeScale, const Symbol& symbol)
				requires XYZPlotable<1, 1, 1, XCONTAINER, YCONTAINER, Z1CONTAINER, X, Y, Z1>
				&& std::input_iterator<typename Z2CONTAINER::const_iterator>
				&& std::convertible_to<typename Z2CONTAINER::value_type, Z2>
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, colourScale, sizeScale), xs, ys, zsColour, zsSize ), m_symbol(symbol), m_colourScale(colourScale), m_sizeScale(sizeScale)
			{
			}
			void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const override
			{
				if (!hasData())
					return;

				const GridData<X, 1>& x = data::getData<0>(axisSetIndex);
				const GridData<Y, 1>& y = data::getData<1>(axisSetIndex);
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


		template<class XCONTAINER, class YCONTAINER, class Z1CONTAINER, class Z2CONTAINER, class X, class Y, class Z1, class Z2>
		auto makePointsColourAndSizeVarying(const XCONTAINER& xs, const YCONTAINER ys, const Z1CONTAINER& zsColour, const Z2CONTAINER& zsSize, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr < ColourScale<Z1>> colourScale, const std::shared_ptr<SizeScale<Z2>> sizeScale, const Symbol& symbol)
			requires XYZPlotable<1, 1, 1, XCONTAINER, YCONTAINER, Z1CONTAINER, X, Y, Z1>
			&& std::input_iterator<typename Z2CONTAINER::const_iterator>
			&& std::convertible_to<typename Z2CONTAINER::value_type, Z2>
		{
			return make_shared<PointsColourAndSizeVarying<X, Y, Z1, Z2>>(xs, ys, zsColour, zsSize, xAxis, yAxis, colourScale, sizeScale, symbol);
		}
	}
}


#endif
