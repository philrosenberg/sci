#ifndef SCI_PLOT_STYLES
#define SCI_PLOT_STYLES

#include"../graphics.h"
namespace sci
{
	namespace plot
	{

		using rgbcolour = sci::graphics::RgbColour;
		using hlscolour = sci::graphics::HlsColour;
		using Length = sci::graphics::Length;
		using Point = sci::graphics::Point;
		using Renderer = sci::graphics::Renderer;
		using Distance = sci::graphics::Distance;
		using millimetre = sci::graphics::millimetre;
		using perMillimetre = sci::graphics::perMillimetre;
		using textPoint = sci::graphics::textPoint;
		using degree = sci::graphics::degree;
		using unitless = sci::graphics::unitless;
		using perInch = sci::graphics::perInch;

		class LineStyle
		{
		public:
			LineStyle(Length width = millimetre(0.5), const rgbcolour& colour = rgbcolour(0.0, 0.0, 0.0, 1.0), const std::vector<Length>& dashes = std::vector<Length>(0))
				: m_width(width), m_colour(colour), m_dashes(dashes)
			{
			}

			LineStyle(Length width, const rgbcolour& colour, const sci::string& pattern)
				: m_width(width), m_colour(colour)
			{
				parseLineStyle(pattern, width, m_dashes);
			}

			constexpr Length getWidth() const
			{
				return m_width;
			}

			std::vector<Length> getPattern() const
			{
				return m_dashes;
			}

			rgbcolour getColour() const
			{
				return m_colour;
			}

			//Converts a series of characters into dots/dashes and spaces.
			//Here a space represents a gap of 0.5m, a tab 1.5 mm and for dashes a . is 0.5 mm, a - or a _ are 1.5 mm.
			//Adjacent space or dash characters are summed, so a 2 mm gap followed by a 2 mm dash could be "    ...."
			//or " \t._"
			static void parseLineStyle(const sci::string& pattern, Length lineWidth, std::vector<Length>& dashes)
			{
				//set outputs to zero size
				dashes.resize(0);

				//return empty vectors if style is empty
				if (pattern.length() == 0)
					return;


				//set up to start on a mark
				bool onmark = true;
				if (pattern[0] == ' ' || pattern[0] == '\t')
					dashes.push_back(millimetre(0.0));

				//initialise our current lengths to zero
				Length marklength = millimetre(0.0);
				Length spacelength = millimetre(0.0);

				//work through each character of style
				for (size_t i = 0; i < pattern.length(); ++i)
				{
					//if we have changed between a space and mark record the length
					if (onmark == true && (pattern[i] == ' ' || pattern[i] == '\t'))
					{
						dashes.push_back(marklength);
						marklength = millimetre(0.0);
						onmark = false;
					}
					else if (onmark == false && (pattern[i] == '_' || pattern[i] == '.' || pattern[i] == '-'))
					{
						dashes.push_back(spacelength);
						spacelength = millimetre(0.0);
						onmark = true;
					}
					//add the current character to the current length
					if (pattern[i] == ' ')
						spacelength += lineWidth * unitless(0.5);
					else if (pattern[i] == '\t')
						spacelength += lineWidth * unitless(2.0);
					else if (pattern[i] == '.')
						marklength += lineWidth * unitless(0.5);
					else if (pattern[i] == '_')
						marklength += lineWidth * unitless(2.0);
					else if (pattern[i] == '-')
						marklength += lineWidth * unitless(2.0);
				}
				//add the last mark or space
				if (onmark == true)
					dashes.push_back(marklength);
				else
					dashes.push_back(spacelength);

				if (dashes.size() == 0)
					return;

				//if we ended on a mark merge it with the first mark and remove it
				if (onmark)
				{
					dashes[0] += dashes.back();
					dashes.pop_back();
				}
			}
			void setPen(Renderer& renderer) const
			{
				renderer.setPen(m_colour, m_width, m_dashes);
			}
		private:
			Length m_width;
			rgbcolour m_colour;
			std::vector<Length> m_dashes;
		};

		const LineStyle noLine(millimetre(0.0));

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
