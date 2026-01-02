#ifndef SVECTOR_PLOTELEMENTS_H
#define SVECTOR_PLOTELEMENTS_H

#include<vector>
#include<memory>
#include<span>
#include"styles.h"

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
				Distance(millimetre(-1.0), millimetre(-1.0)),
					Distance(millimetre(1.0), millimetre(-1.0)),
					Distance(millimetre(1.0), millimetre(1.0)),
					Distance(millimetre(-1.0), millimetre(1.0)),
					Distance(millimetre(-1.0), millimetre(-1.0)) };
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
	}
}


#endif
