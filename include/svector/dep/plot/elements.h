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

		class SymbolBase
		{
		public:
			SymbolBase(sci::string symbol, PLUNICODE fci);
			SymbolBase(const std::vector<sci::graphics::Distance>& symbol)
				:m_symbol(symbol)
			{
				m_symbolText = sU(" ");
				m_fci = 0;
			}
			sci::string getSymbol() const
			{
				return m_symbolText;
			}
			PLUNICODE getFci() const;
			void draw(const sci::graphics::Point& point, sci::graphics::Renderer& renderer) const
			{
				std::vector<Point> points(m_symbol.size());
				for (size_t i = 0; i < m_symbol.size(); ++i)
					points[i] = point + m_symbol[i];
				renderer.polygon(points);
			}
		private:
			sci::string m_symbolText;
			PLUNICODE m_fci;
			std::vector<sci::graphics::Distance> m_symbol;
		};

		class Symbol : public SymbolBase
		{
		public:
			Symbol()
				:SymbolBase(std::vector<Distance>{
				Distance(grMillimetre(-1.0), grMillimetre(-1.0)),
					Distance(grMillimetre(1.0), grMillimetre(-1.0)),
					Distance(grMillimetre(1.0), grMillimetre(1.0)),
					Distance(grMillimetre(-1.0), grMillimetre(1.0)),
					Distance(grMillimetre(-1.0), grMillimetre(-1.0)) })
			{
				m_size = 1.0;
				m_colour = rgbcolour(0, 0, 0, 1.0);
			}
			Symbol(const Symbol& symbol) = default;
			Symbol(Symbol&& symbol) = default;
			Symbol& operator=(const Symbol& symbol) = default;
			Symbol(sci::string symbol, double size = 4.0, sci::graphics::RgbColour colour = sci::graphics::RgbColour(0, 0, 0, 1.0));

			Symbol(const std::span<sci::graphics::Distance>& symbol,
				sci::graphics::RgbColour colour = sci::graphics::RgbColour(0, 0, 0, 1.0))
				: SymbolBase(std::vector<Distance>(std::begin(symbol), std::end(symbol)))
			{
				m_size = 1.0;
				m_colour = colour;
			}

			double getSize() const
			{
				return m_size;
			}

			sci::graphics::RgbColour getColour() const
			{
				return m_colour;
			}

			void setupSymbol(plstream* pl, PLINT colourIndex, double scale) const;
		private:
			double m_size;
			sci::graphics::RgbColour m_colour;
		};

		class VaryingSymbol : public SymbolBase
		{
		public:
			VaryingSymbol(sci::string symbol);
			virtual void setupSymbol(plstream* pl, PLINT colourIndex, double parameter, bool parameterPreLogged, double scale) const = 0;
			virtual bool isLogScaled() const = 0;
		private:
		};

		class ColourVaryingSymbol : public VaryingSymbol
		{
		public:
			ColourVaryingSymbol(std::shared_ptr<ColourScale> colourScale, sci::string symbol = symText::filledCircle, double size = 4.0);
			void setupSymbol(plstream* pl, PLINT colourIndex, double parameter, bool parameterPreLogged, double scale) const override;
			bool isLogScaled() const override
			{
				return m_colourScale->isLog();
			}
			std::shared_ptr<ColourScale> getColourscale() const { return m_colourScale; }
		private:
			double m_size;
			std::shared_ptr<ColourScale> m_colourScale;
		};

		class SizeVaryingSymbol : public VaryingSymbol
		{
		public:
			SizeVaryingSymbol(std::shared_ptr<SizeScale> sizeScale, sci::string symbol = symText::filledCircle, sci::graphics::RgbColour colour = sci::graphics::RgbColour(0.0, 0.0, 0.0))
				:VaryingSymbol(symbol)
			{
				m_sizeScale = sizeScale;
				m_colour = colour;
			}

			void setupSymbol(plstream* pl, PLINT colourIndex, double parameter, bool parameterPreLogged, double scale) const override;

			bool isLogScaled() const override
			{
				return m_sizeScale->isLog();
			}

			//parameterPreLogged is only utilised for log scales
			double getSize(double parameter, bool parameterPreLogged) const
			{
				return m_sizeScale->getsize(parameter, parameterPreLogged);
			}

			std::shared_ptr<SizeScale>getSizeScale() const { return m_sizeScale; }
		private:
			sci::graphics::RgbColour m_colour;
			std::shared_ptr<SizeScale> m_sizeScale;
		};

		class ColourAndSizeVaryingSymbol : public SymbolBase
		{
		public:
			ColourAndSizeVaryingSymbol(std::shared_ptr<ColourScale> colourScale, std::shared_ptr<SizeScale> sizeScale, sci::string symbol = symText::filledCircle);
			void setupSymbol(plstream* pl, PLINT colourIndex, double colourParameter, bool colourParameterPreLogged, double sizeParameter, bool sizeParameterPreLogged, double scale) const;
			bool isColourLogScaled() const
			{
				return m_colourScale->isLog();
			}

			bool isSizeLogScaled() const
			{
				return m_sizeScale->isLog();
			}

			//parameterPreLogged is only utilised for log scales
			double getSize(double parameter, bool parameterPreLogged) const
			{
				return m_sizeScale->getsize(parameter, parameterPreLogged);
			}

			std::shared_ptr<ColourScale> getColourscale() const
			{
				return m_colourScale;
			
			}

			std::shared_ptr<SizeScale>getSizeScale() const
			{
				return m_sizeScale;
			}
		private:
			std::shared_ptr<ColourScale> m_colourScale;
			std::shared_ptr<SizeScale> m_sizeScale;
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
