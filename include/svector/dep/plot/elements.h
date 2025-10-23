#ifndef SVECTOR_PLOTELEMENTS_H
#define SVECTOR_PLOTELEMENTS_H

#include<vector>
#include<string>
#include<memory>

#include<svector/sstring.h>
#include<svector/graphics.h>
#include<span>
#include"plot.h"

namespace sci
{
	namespace plot
	{

		class Symbol
		{
		public:
			Symbol(const std::span<const sci::graphics::Distance>& symbol )
				:m_symbol(symbol.begin(), symbol.end())
			{
			}

			Symbol()
			{
				m_symbol = std::vector<Distance>{
				Distance(grMillimetre(-1.0), grMillimetre(-1.0)),
					Distance(grMillimetre(1.0), grMillimetre(-1.0)),
					Distance(grMillimetre(1.0), grMillimetre(1.0)),
					Distance(grMillimetre(-1.0), grMillimetre(1.0)),
					Distance(grMillimetre(-1.0), grMillimetre(-1.0)) };
			}

			Symbol(const Symbol& symbol) = default;
			Symbol(Symbol&& symbol) = default;
			Symbol& operator=(const Symbol& symbol) = default;

			const std::vector<sci::graphics::Distance>& getSymbol() const
			{
				return m_symbol;
			}

			//draw unscaled
			void draw(const sci::graphics::Point& point, sci::graphics::Renderer& renderer) const
			{
				std::vector<Point> points(m_symbol.size());
				for (size_t i = 0; i < m_symbol.size(); ++i)
					points[i] = point + m_symbol[i];
				renderer.polygon(points);
			}

			//draw scaled
			void draw(const sci::graphics::Point& point, graphics::unitless scale, sci::graphics::Renderer& renderer) const
			{
				std::vector<Point> points(m_symbol.size());
				for (size_t i = 0; i < m_symbol.size(); ++i)
					points[i] = point + m_symbol[i] * scale;
				renderer.polygon(points);
			}

		private:
			std::vector<sci::graphics::Distance> m_symbol;
		};

		class FillStyle
		{
		public:
			FillStyle(const sci::graphics::RgbColour& colour = sci::graphics::RgbColour(0.0, 0.0, 0.0))
			{
				m_colour = colour;
			}

			sci::graphics::RgbColour getColour() const
			{
				return m_colour;
			}
			void setBrush(sci::graphics::Renderer& renderer) const
			{
				renderer.setBrush(m_colour);
			}

		private:
			sci::graphics::RgbColour m_colour;
			LineStyle m_lineStyle;
		};
	}
}


#endif
