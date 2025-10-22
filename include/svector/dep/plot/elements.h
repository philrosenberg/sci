#ifndef SVECTOR_PLOTELEMENTS_H
#define SVECTOR_PLOTELEMENTS_H

#include<vector>
#include<string>
#include<memory>

#include<svector/sstring.h>
#include<svector/graphics.h>
#include<plplot/plstream.h>
#include<span>
#include"plot.h"

namespace sci
{
	namespace plot
	{

		class Symbol
		{
		public:
			Symbol(sci::string symbol, PLUNICODE fci);
			Symbol(const std::span<const sci::graphics::Distance>& symbol )
				:m_symbol(symbol.begin(), symbol.end())
			{
				m_symbolText = sU(" ");
				m_fci = 0;
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
			sci::string getSymbol() const
			{
				return m_symbolText;
			}
			PLUNICODE getFci() const;
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
			sci::string m_symbolText;
			PLUNICODE m_fci;
			std::vector<sci::graphics::Distance> m_symbol;
		};

		class FillStyle
		{
		public:
			FillStyle(const sci::graphics::RgbColour& colour = sci::graphics::RgbColour(0.0, 0.0, 0.0))
			{
				m_colour = colour;
				m_lineSpacingMicrons[0] = 0;
				m_angleDeg[0] = 0;
				m_lineSpacingMicrons[1] = 0;
				m_angleDeg[1] = 0;
			}
			FillStyle(const LineStyle& lineStyle, double lineSpacingMicrons, double angleDeg)
			{
				m_lineStyle = lineStyle;
				m_lineSpacingMicrons[0] = (PLINT)sci::round(lineSpacingMicrons);
				m_angleDeg[0] = (PLINT)sci::round(angleDeg * 10.0);
				m_lineSpacingMicrons[1] = 0;
				m_angleDeg[1] = 0;
			}
			FillStyle(const LineStyle& lineStyle, double lineSpacing1Microns, double angle1Deg, double lineSpacing2Microns, double angle2Deg)
			{
				m_lineStyle = lineStyle;
				m_lineSpacingMicrons[0] = (PLINT)sci::round(lineSpacing1Microns);
				m_angleDeg[0] = (PLINT)sci::round(angle1Deg * 10.0);
				m_lineSpacingMicrons[1] = (PLINT)sci::round(lineSpacing2Microns);
				m_angleDeg[1] = (PLINT)sci::round(angle2Deg * 10.0);
			}
			void setupFillStyle(plstream* pl, PLINT colourIndex, double scale) const;
			void resetFillStyle(plstream* pl, PLINT colourIndex) const;
			sci::graphics::RgbColour getColour() const
			{
				if (m_lineSpacingMicrons[0] > 0)
					return m_lineStyle.getColour();
				return m_colour;
			}
			void setBrush(sci::graphics::Renderer& renderer) const
			{
				renderer.setBrush(m_colour);
			}

		private:
			sci::graphics::RgbColour m_colour;
			LineStyle m_lineStyle;
			PLINT m_lineSpacingMicrons[2];
			PLINT m_angleDeg[2];
		};
	}
}


#endif
