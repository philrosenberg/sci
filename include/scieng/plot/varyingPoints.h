#ifndef SVECTOR_PLOTVARYINGPOINTS_H
#define SVECTOR_PLOTVARYINGPOINTS_H

#include"plotable.h"
#include"elements.h"

namespace sci
{
	namespace plot
	{
		template<class X, class Y, class Z>
		class PointsColourVarying : public Data<X, Y, 
			std::tuple<std::shared_ptr<Axis<X>>, const std::shared_ptr<Axis<Y>>, const std::shared_ptr < ColourScale<Z>>>,
			GridData<X, 1>, GridData<Y, 1>, GridData<Z, 1>>
		{
		public:
			using data = Data<X, Y,
				std::tuple<std::shared_ptr<Axis<X>>, const std::shared_ptr<Axis<Y>>, const std::shared_ptr < ColourScale<Z>>>,
				GridData<X, 1>, GridData<Y, 1>, GridData<Z, 1>>;

			template<class XCONTAINER, class YCONTAINER, class ZCONTAINER>
			PointsColourVarying(const XCONTAINER xs, const YCONTAINER &ys, ZCONTAINER zs, const std::shared_ptr<Axis<X>> xAxis, const std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr < ColourScale<Z>> colourScale, const Symbol& symbol)
				requires XYZPlotable<1, 1, 1, XCONTAINER, YCONTAINER, ZCONTAINER, X, Y, Z>
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, colourScale), xs, ys, zs), m_symbol(symbol)
			{
			}
			void plotData(const SpacialAxesSet<X, Y>& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				renderer.setPen(rgbcolour(), millimetre(0.0));
				for (size_t i = 0; i < this->getNPoints<0>(); ++i)
				{
					renderer.setBrush(this->getTransformed<2>(i, scales));
					m_symbol.draw(this->getPoint<0, 1>(i, axisSet), renderer);
				}

			}
		private:
			Symbol m_symbol;
		};

		template<class XCONTAINER, class YCONTAINER, class ZCONTAINER, class X, class Y, class Z>
		auto makePointsColourVarying(const XCONTAINER xs, const YCONTAINER& ys, ZCONTAINER zs, const std::shared_ptr<Axis<X>> xAxis, const std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr < ColourScale<Z>> colourScale, const Symbol& symbol)
			requires XYZPlotable<1, 1, 1, XCONTAINER, YCONTAINER, ZCONTAINER, X, Y, Z>
		{
			return make_shared<PointsColourVarying<X, Y, Z>>(xs, ys, zs, xAxis, yAxis, colourScale, symbol);
		}

		template<class X, class Y, class Z>
		class PointsSizeVarying : public Data<X, Y,
			std::tuple<const std::shared_ptr<Axis<X>>, const std::shared_ptr<Axis<Y>>, const std::shared_ptr<SizeScale<Z>>>,
			GridData<X, 1>, GridData<Y, 1>, GridData<Z, 1>>
		{
		public:
			using data = Data<X, Y,
				std::tuple<const std::shared_ptr<Axis<X>>, const std::shared_ptr<Axis<Y>>, const std::shared_ptr<SizeScale<Z>>>,
				GridData<X, 1>, GridData<Y, 1>, GridData<Z, 1>>;

			template<class XCONTAINER, class YCONTAINER, class ZCONTAINER>
			PointsSizeVarying(const XCONTAINER& xs, const YCONTAINER& ys, const ZCONTAINER& zs, const std::shared_ptr<Axis<Z>> xAxis, const std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr<SizeScale<Z>> sizeScale, const Symbol& symbol, sci::graphics::RgbColour colour)
				requires XYZPlotable<1, 1, 1, XCONTAINER, YCONTAINER, ZCONTAINER, X, Y, Z>
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, sizeScale), xs, ys, zs ), m_symbol(symbol), m_colour(colour)
			{
			}
			void plotData(const SpacialAxesSet<X, Y>& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				renderer.setBrush(m_colour);
				renderer.setPen(rgbcolour(), millimetre(0.0));
				for (size_t i = 0; i < this->getNPoints<0>(); ++i)
				{
					auto size = this->getTransformed<2>(i, scales);
					m_symbol.draw(this->getPoint<0, 1>(i, axisSet), graphics::unitless(size), renderer);
				}
			}
		private:
			Symbol m_symbol;
			sci::graphics::RgbColour m_colour;
		};

		template<class XCONTAINER, class YCONTAINER, class ZCONTAINER, class X, class Y, class Z>
		auto makePointsSizeVarying(const XCONTAINER xs, const YCONTAINER& ys, ZCONTAINER zs, const std::shared_ptr<Axis<X>> xAxis, const std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr<SizeScale<Z>> sizeScale, const Symbol& symbol, sci::graphics::RgbColour colour)
			requires XYZPlotable<1, 1, 1, XCONTAINER, YCONTAINER, ZCONTAINER, X, Y, Z>
		{
			return make_shared<PointsSizeVarying<X, Y, Z>>(xs, ys, zs, xAxis, yAxis, sizeScale, symbol, colour);
		}

		template<class X, class Y, class Z1, class Z2>
		class PointsColourAndSizeVarying : public Data<X, Y,
			std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, const std::shared_ptr < ColourScale<Z1>>, const std::shared_ptr<SizeScale<Z2>>>,
			GridData<X, 1>, GridData<Y, 1>, GridData<Z1, 1>, GridData<Z2, 1>>
		{
		public:
			using data = Data<X, Y,
				std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, const std::shared_ptr < ColourScale<Z1>>, const std::shared_ptr<SizeScale<Z2>>>,
				GridData<X, 1>, GridData<Y, 1>, GridData<Z1, 1>, GridData<Z2, 1>>;

			template<class XCONTAINER, class YCONTAINER, class Z1CONTAINER, class Z2CONTAINER>
			PointsColourAndSizeVarying(const XCONTAINER& xs, const YCONTAINER ys, const Z1CONTAINER& zsColour, const Z2CONTAINER& zsSize, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, const std::shared_ptr < ColourScale<Z1>> colourScale, const std::shared_ptr<SizeScale<Z2>> sizeScale, const Symbol& symbol)
				requires XYZPlotable<1, 1, 1, XCONTAINER, YCONTAINER, Z1CONTAINER, X, Y, Z1>
				&& std::input_iterator<typename Z2CONTAINER::const_iterator>
				&& std::convertible_to<typename Z2CONTAINER::value_type, Z2>
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, colourScale, sizeScale), xs, ys, zsColour, zsSize ), m_symbol(symbol)
			{
			}
			void plotData(const SpacialAxesSet<X, Y>& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				renderer.setPen(rgbcolour(), millimetre(0.0));
				for (size_t i = 0; i < this->getNPoints<0>(); ++i)
				{
					renderer.setBrush(this->getTransformed<2>(i, scales));
					auto size = this->getTransformed<3>(i, scales);
					m_symbol.draw(this->getPoint<0, 1>(i, axisSet), graphics::unitless(size), renderer);
				}
			}
		private:
			Symbol m_symbol;
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
