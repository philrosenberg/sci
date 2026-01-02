#ifndef sgraphics_h
#define sgraphics_h

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define SVECTOR_MUST_RESET_CRT_SECURE_NO_WARNINGS
#endif

#include<vector>
#include"string.h"
#include"stringfuncs.h"
#include"Units.h"
#include<utility>
#include<array>
#include<map>
#include<optional>
#include<span>
#include<memory>

namespace sci
{
	namespace graphics
	{

		using unitless = sci::Physical<sci::Unitless, double>;
		using millimetre = sci::Physical<sci::Metre<1, -3>, double>;
		using perMillimetre = decltype(unitless() / millimetre());
		using inch = sci::Physical<sci::Inch<>, double>;
		using perInch = decltype(unitless() / inch());
		using degree = sci::Physical<sci::Degree<>, double>;
		using textPoint = sci::Physical<sci::TextPoint<>, double>;


		#ifdef SVECTOR_MUST_RESET_CRT_SECURE_NO_WARNINGS
		#undef _CRT_SECURE_NO_WARNINGS
		#undef SVECTOR_MUST_RESET_CRT_SECURE_NO_WARNINGS
		#endif

		// rgb colour. the hue colour wheel goes from 0-360, but numbers outside this range are permitted.
		// lightness, saturation and alpha are in the 0-1 range and will be set to the apropriate limit if
		//outside these values
		class RgbColour;

		class HlsColour
		{
		public:
			
			HlsColour(degree hue, double lightness, double saturation, double alpha = 1.0)
			{
				m_h = hue;
				m_l = std::max(std::min(lightness, 1.0), 0.0);
				m_s = std::max(std::min(saturation, 1.0), 0.0);
				m_a = std::max(std::min(alpha, 1.0), 0.0);
			}
			HlsColour()
			{
				m_h = degree(0.0);
				m_l = 0.0;
				m_s = 0.0;
				m_a = 1.0;
			}
			RgbColour convertToRgb() const;
			inline degree h() const { return m_h; }
			inline double l() const { return m_l; }
			inline double s() const { return m_s; }
			inline double a() const { return m_a; }
			degree m_h;
			double m_l;
			double m_s;
			double m_a;
		};

		// rgb colour, all three colours and alpha are in the 0-1 range and will be set to 
		//the apropriate limit if outside these values
		class RgbColour
		{
		public:
			RgbColour(double red, double green, double blue, double alpha = 1.0)
			{
				m_r = std::max(std::min(red, 1.0), 0.0);;
				m_g = std::max(std::min(green, 1.0), 0.0);
				m_b = std::max(std::min(blue, 1.0), 0.0);
				m_a = std::max(std::min(alpha, 1.0), 0.0);
			}
			RgbColour()
			{
				m_r = 0.0;
				m_g = 0.0;
				m_b = 0.0;
				m_a = 1.0;
			}
			HlsColour convertToHls() const
			{
				const char redMax = 1;
				const char greenMax = 2;
				const char blueMax = 4;
				const char redMin = 8;
				const char greenMin = 16;
				const char blueMin = 32;

				char minMaxChannel = 0;
				if (m_r >= m_g && m_r >= m_b)
				{
					minMaxChannel += redMax;
					minMaxChannel += m_g <= m_b ? greenMin : blueMin;
				}
				else if (m_g >= m_b)
				{
					minMaxChannel += greenMax;
					minMaxChannel += m_r <= m_b ? redMin : blueMin;
				}
				else
				{
					minMaxChannel += blueMax;
					minMaxChannel += m_r <= m_g ? redMin : greenMin;
				}

				double min = minMaxChannel & redMin ? m_r : (minMaxChannel & greenMin ? m_g : m_b);
				double max = minMaxChannel & redMax ? m_r : (minMaxChannel & greenMax ? m_g : m_b);
				
				double chroma = max - min;

				double hDash;
				if (minMaxChannel & greenMax)
					hDash = (m_b - m_r) / chroma + 2.0;
				else if (minMaxChannel & blueMax)
					hDash = (m_r - m_g) / chroma + 4.0;
				else
				{
					double segment = (m_g - m_b) / chroma;
					hDash = segment < 0.0 ? segment + 6 : segment;
				}

				degree hue = unitless(hDash) * degree(60);
				//this will give an indefinite (0/0) when r==g==b


				double lightness = 0.5 * (min + max);

				double saturation = chroma / (1 - std::abs(2 * lightness - 1));
				//again this gives an indefinite if r==g==b
				if (saturation != saturation)
					saturation = 0.0;

				return HlsColour(hue, lightness, saturation, m_a);
			}
			inline double r() const { return m_r; }
			inline double g() const { return m_g; }
			inline double b() const { return m_b; }
			inline double a() const { return m_a; }
			double m_r;
			double m_g;
			double m_b;
			double m_a;
		};

		inline RgbColour HlsColour::convertToRgb() const
		{
			if (m_s == 0 || m_s != m_s || m_h != m_h)
				return RgbColour(m_l, m_l, m_l);



			double chroma = m_s * (1.0 - std::abs(2 * m_l - 1));
			double max = m_l + 0.5 * chroma;
			double min = 2 * m_l - max;

			const char redMax = 1;
			const char greenMax = 2;
			const char blueMax = 4;
			const char redMin = 8;
			const char greenMin = 16;
			const char blueMin = 32;

			degree hue = m_h - sci::floor<unitless>(m_h / degree(360)) * degree(360);

			double hDash = (hue / degree(60)).value<unitless>();
			char minMaxChannel = 0;
			if (hDash >= 5.0)
				minMaxChannel += redMax;
			else if (hDash >= 3.0)
				minMaxChannel += blueMax;
			else if (hDash >= 1.0)
				minMaxChannel += greenMax;
			else
				minMaxChannel += redMax;

			if (hDash >= 4.0)
				minMaxChannel += greenMin;
			else if (hDash >= 2.0)
				minMaxChannel += redMin;
			else
				minMaxChannel += blueMin;

			double red;
			double green;
			double blue;
			if (minMaxChannel & blueMax)
			{
				blue = max;
				if (minMaxChannel & redMin)
				{
					red = min;
					green = red - (hDash - 4.0) * chroma;
				}
				else
				{
					green = min;
					red = green + (hDash - 4.0) * chroma;
				}
			}
			else if (minMaxChannel & greenMax)
			{
				green = max;
				if (minMaxChannel & redMin)
				{
					red = min;
					blue = red + (hDash - 2.0) * chroma;
				}
				else
				{
					blue = min;
					red = blue - (hDash - 2.0) * chroma;
				}
			}
			else if (minMaxChannel & blueMin) // red is max
			{
				red = max;
				blue = min;
				green = hDash * chroma + blue;
			}
			else //red is max green is min
			{
				red = max;
				green = min;
				blue = green - (hDash - 6.0) * chroma;
			}

			return RgbColour(red, green, blue, m_a);
		}

		//a length is a scalar, it's primary use is as the x and y components of GraphicsVector or where an item needs a size with no direction
		//It is a combination of an absolute distance represented internally as a sci::Physical with a length dimension
		//plus a value that is a fraction of the page width, plus a value that is a fraction of the page length.
		//In this way you can create a length that has a fixed size no matter how the page scales, or that scales
		//with the page width or the page height
		class Length
		{
		public:
			enum ScaleDirection
			{
				xDirection,
				yDirection
			};
			constexpr Length(unitless length, ScaleDirection scaleDirection)
			{
				set(length, scaleDirection);
			}
			constexpr Length(millimetre length)
			{
				set(length);
			}
			constexpr Length(textPoint length)
			{
				set(millimetre(length));
			}
			constexpr Length()
			{
				m_absolute = millimetre(0);
				m_lengthOverWidth = unitless(0);
				m_lengthOverHeight = unitless(0);
			}
			constexpr Length(const Length&) = default;
			constexpr Length& operator=(const Length&) = default;
			constexpr Length(Length&& other) = default;
			constexpr void set(unitless x, ScaleDirection scaleDirection)
			{
				m_absolute = millimetre(0);
				m_lengthOverWidth = unitless(0);
				m_lengthOverHeight = unitless(0);
				if (ScaleDirection::xDirection == scaleDirection)
					m_lengthOverWidth = x;
				if (ScaleDirection::yDirection == scaleDirection)
					m_lengthOverHeight = x;
			}
			constexpr void set(millimetre length)
			{
				m_absolute = length;
				m_lengthOverWidth = unitless(0);
				m_lengthOverHeight = unitless(0);
			}
			//returns the length in device coordinates
			constexpr double getLength(double width, double height, perMillimetre scale) const
			{
				return (m_absolute * scale + m_lengthOverWidth * unitless(width) + m_lengthOverHeight * unitless(height)).value<unitless>();
			}
			//returns the length in abslute coordinates
			constexpr millimetre getLength(millimetre width, millimetre height) const
			{
				return m_absolute + m_lengthOverWidth * width + m_lengthOverHeight * height;
			}
			constexpr bool operator==(const Length& other) const
			{
				return m_absolute == other.m_absolute &&
					m_lengthOverWidth == other.m_lengthOverWidth &&
					m_lengthOverHeight == other.m_lengthOverHeight;
			}
			constexpr bool operator!=(const Length& other) const
			{
				return !((*this) == other);
			}
			constexpr Length& operator+=(const Length& other)
			{
				m_absolute += other.m_absolute;
				m_lengthOverWidth += other.m_lengthOverWidth;
				m_lengthOverHeight += other.m_lengthOverHeight;
				return*this;
			}
			constexpr Length operator+(const Length& other) const
			{
				Length newLength = *this;
				newLength += other;
				return newLength;
			}
			constexpr Length& operator-=(const Length& other)
			{
				m_absolute -= other.m_absolute;
				m_lengthOverWidth -= other.m_lengthOverWidth;
				m_lengthOverHeight -= other.m_lengthOverHeight;
				return*this;
			}
			constexpr Length operator-(const Length& other) const
			{
				Length newLength = *this;
				newLength -= other;
				return newLength;
			}
			constexpr Length& operator*=(const unitless& other)
			{
				m_absolute *= other;
				m_lengthOverWidth *= other;
				m_lengthOverHeight *= other;
				return*this;
			}
			constexpr Length operator*(const unitless& other) const
			{
				Length newLength = *this;
				newLength *= other;
				return newLength;
			}
			constexpr Length& operator/=(const unitless& other)
			{
				m_absolute /= other;
				m_lengthOverWidth /= other;
				m_lengthOverHeight /= other;
				return*this;
			}
			constexpr Length operator/(const unitless& other) const
			{
				Length newLength = *this;
				newLength /= other;
				return newLength;
			}
			constexpr Length operator-()
			{
				Length newLength = *this;
				newLength.m_absolute = -newLength.m_absolute;
				newLength.m_lengthOverWidth = -newLength.m_lengthOverWidth;
				newLength.m_lengthOverHeight = -newLength.m_lengthOverHeight;
				return newLength;
			}
			constexpr bool isAlwaysZero() const
			{
				return m_absolute == millimetre(0.0) && m_lengthOverWidth == unitless(0.0) && m_lengthOverHeight == unitless(0.0);
			}

		private:
			millimetre m_absolute;
			unitless m_lengthOverWidth;
			unitless m_lengthOverHeight;
		};

		//a useful constant for zero length
		const Length zeroLength(millimetre(0));

		//This class is a 2D vector with a Length in the x and y direction
		//It provides functionality for doing vector maths, but is not used
		//directly. Instead use the Distance or Point classes which
		//inherit from this
		class GraphicsVector
		{
			friend class Distance;
			friend class Point;

		public:
			enum ScaleDirection
			{
				parallelDirection,
				xDirection,
				yDirection
			};
		private:
			constexpr GraphicsVector(unitless x, unitless y, ScaleDirection scale = GraphicsVector::ScaleDirection::parallelDirection)
			{
				set(x, y, scale);
			}
			constexpr GraphicsVector(millimetre x, millimetre y)
			{
				set(x, y);
			}
			constexpr GraphicsVector(Length x, Length y)
			{
				set(x, y);
			}
			constexpr GraphicsVector() = default;
			constexpr GraphicsVector(const GraphicsVector&) = default;
			constexpr GraphicsVector(GraphicsVector&&) = default;

		public:
			constexpr GraphicsVector& operator=(const GraphicsVector&) = default;
			virtual ~GraphicsVector() = default;
			constexpr void set(unitless x, unitless y, ScaleDirection scaleDirection = GraphicsVector::ScaleDirection::parallelDirection)
			{
				Length::ScaleDirection xScaleDirection;
				Length::ScaleDirection yScaleDirection;
				if (ScaleDirection::parallelDirection == scaleDirection)
				{
					xScaleDirection = Length::ScaleDirection::xDirection;
					yScaleDirection = Length::ScaleDirection::yDirection;
				}
				if (ScaleDirection::xDirection == scaleDirection)
				{
					xScaleDirection = Length::ScaleDirection::xDirection;
					yScaleDirection = Length::ScaleDirection::xDirection;
				}
				if (ScaleDirection::yDirection == scaleDirection)
				{
					xScaleDirection = Length::ScaleDirection::yDirection;
					yScaleDirection = Length::ScaleDirection::yDirection;
				}
		
				m_x.set(x, xScaleDirection);
				m_y.set(y, yScaleDirection);
			}
			constexpr void set(millimetre x, millimetre y)
			{
				m_x.set(x);
				m_y.set(y);
			}
			constexpr void set(Length x, Length y)
			{
				m_x = x;
				m_y = y;
			}
			constexpr double getX(double width, double height, perMillimetre scale) const
			{
				return m_x.getLength(width, height, scale);
			}
			constexpr double getY(double width, double height, perMillimetre scale) const
			{
				return m_y.getLength(width, height, scale);
			}
			constexpr millimetre getX(millimetre width, millimetre height) const
			{
				return m_x.getLength(width, height);
			}
			constexpr millimetre getY(millimetre width, millimetre height) const
			{
				return m_y.getLength(width, height);
			}
			constexpr const Length &getX() const
			{
				return m_x;
			}
			constexpr const Length &getY() const
			{
				return m_y;
			}
			constexpr bool operator==(const GraphicsVector& other) const
			{
				return m_x == other.m_x && m_y == other.m_y;
			}
			constexpr bool operator!=(const GraphicsVector& other) const
			{
				return !((*this) == other);
			}
			constexpr GraphicsVector& operator+=(const GraphicsVector& other)
			{
				m_x += other.m_x;
				m_y += other.m_y;
				return* this;
			}
			constexpr GraphicsVector operator+(const GraphicsVector& other) const
			{
				GraphicsVector newVector = *this;
				newVector += other;
				return newVector;
			}
			constexpr GraphicsVector& operator-=(const GraphicsVector& other)
			{
				m_x -= other.m_x;
				m_y -= other.m_y;
				return*this;
			}
			constexpr GraphicsVector operator-(const GraphicsVector& other) const
			{
				GraphicsVector newVector = *this;
				newVector -= other;
				return newVector;
			}
			constexpr GraphicsVector& operator*=(const unitless& other)
			{
				m_x *= other;
				m_y *= other;
				return*this;
			}
			constexpr GraphicsVector operator*(const unitless& other) const
			{
				GraphicsVector newVector = *this;
				newVector *= other;
				return newVector;
			}
			constexpr GraphicsVector& operator/=(const unitless& other)
			{
				m_x /= other;
				m_y /= other;
				return*this;
			}
			constexpr GraphicsVector operator/(const unitless& other) const
			{
				GraphicsVector newVector = *this;
				newVector /= other;
				return newVector;
			}
			constexpr GraphicsVector& operator*=(const std::array<unitless, 2>& other)
			{
				m_x *= other[0];
				m_y *= other[1];
				return*this;
			}
			constexpr GraphicsVector operator*(const std::array<unitless, 2>& other) const
			{
				GraphicsVector newVector = *this;
				newVector *= other;
				return newVector;
			}
			constexpr GraphicsVector& operator/=(const std::array<unitless, 2>& other)
			{
				m_x /= other[0];
				m_y /= other[1];
				return*this;
			}
			constexpr GraphicsVector operator/(const std::array<unitless, 2>& other) const
			{
				GraphicsVector newVector = *this;
				newVector /= other;
				return newVector;
			}
			constexpr GraphicsVector& operator-()
			{
				GraphicsVector newVector = *this;
				newVector.m_x = -newVector.m_x;
				newVector.m_y = -newVector.m_y;
				return*this;
			}
	
		private:
			Length m_x;
			Length m_y;
		};

		//A distance is a 2D vector which represents the difference in position of two Points
		//It can be used to represent the size of something, but should not be used to
		//represent the position, unless it is specifically representing the difference in
		//two positions
		class Distance : public GraphicsVector
		{
		public:
			constexpr Distance(unitless x, unitless y, ScaleDirection scaleDirection = GraphicsVector::ScaleDirection::parallelDirection)
				:GraphicsVector(x, y, scaleDirection)
			{
			}
			constexpr Distance(millimetre x, millimetre y)
				: GraphicsVector(x, y)
			{
			}
			constexpr Distance(Length x, Length y)
				: GraphicsVector(x, y)
			{
			}
			constexpr Distance() = default;
			constexpr Distance(const Distance&) = default;
			constexpr Distance& operator=(const Distance&) = default;
			constexpr Distance& operator+=(const Distance& distance)
			{
				GraphicsVector::operator+=(distance);
				return *this;
			}
			constexpr Distance operator+(const Distance& distance) const
			{
				Distance newDistance = *this;
				newDistance += distance;
				return newDistance;
			}
			constexpr Distance& operator-=(const Distance& distance)
			{
				GraphicsVector::operator-=(distance);
				return *this;
			}
			constexpr Distance operator-(const Distance& distance) const
			{
				Distance newDistance = *this;
				newDistance -= distance;
				return newDistance;
			}
			constexpr Distance& operator*=(const unitless& scale)
			{
				GraphicsVector::operator*=(scale);
				return *this;
			}
			constexpr Distance operator*(const unitless& scale) const
			{
				Distance newDistance = *this;
				newDistance *= scale;
				return newDistance;
			}
			constexpr Distance& operator/=(const unitless& scale)
			{
				GraphicsVector::operator/=(scale);
				return *this;
			}
			constexpr Distance operator/(const unitless& scale) const
			{
				Distance newDistance = *this;
				newDistance /= scale;
				return newDistance;
			}
			constexpr GraphicsVector& operator*=(const std::array<unitless, 2>& scale)
			{
				GraphicsVector::operator*=(scale);
				return *this;
			}
			constexpr Distance operator*(const std::array<unitless, 2>& scale) const
			{
				Distance newDistance = *this;
				newDistance *= scale;
				return newDistance;
			}
			constexpr GraphicsVector& operator/=(const std::array<unitless, 2>& scale)
			{
				GraphicsVector::operator/=(scale);
				return *this;
			}
			constexpr Distance operator/(const std::array<unitless, 2>& scale) const
			{
				Distance newDistance = *this;
				newDistance /= scale;
				return newDistance;
			}
			constexpr Distance operator-() const
			{
				Distance newDistance = *this;
				return newDistance.operator-();
			}
		};

		constexpr Distance operator*(const unitless& scale, const Distance &distance)
		{
			return distance * scale;
		}

		constexpr Distance operator*(const std::array<unitless, 2>& scale, const Distance& distance)
		{
			return distance * scale;
		}

		//A Point is a 2D vector representing a position.
		//It should not be used to represent a size.
		class Point : public GraphicsVector
		{
		public:
			constexpr Point(unitless x, unitless y, ScaleDirection scaleDirection = GraphicsVector::ScaleDirection::parallelDirection)
				:GraphicsVector(x, y, scaleDirection)
			{
			}
			constexpr Point(millimetre x, millimetre y)
				:GraphicsVector(x, y)
			{
			}
			constexpr Point(Length x, Length y)
				: GraphicsVector(x, y)
			{
			}
			constexpr Point() = default;
			constexpr Point(const Point&) = default;
			constexpr Point& operator=(const Point&) = default;
			constexpr Point& operator+=(const Distance& distance)
			{
				GraphicsVector::operator+=(distance);
				return *this;
			}
			constexpr Point operator+(const Distance& distance) const
			{
				Point newPoint = *this;
				newPoint += distance;
				return newPoint;
			}
			constexpr Point& operator-=(const Distance& distance)
			{
				GraphicsVector::operator-=(distance);
				return *this;
			}
			constexpr Point operator-(const Distance& distance) const
			{
				Point newPoint = *this;
				newPoint -= distance;
				return newPoint;
			}
			constexpr Distance operator-(const Point& other) const
			{
				return Distance(getX() - other.getX(), getY() - other.getY());
			}
		};

		struct TextMetric
		{
			millimetre width;
			millimetre ascent;
			millimetre descent;
			millimetre topAscent;
			millimetre bottomDescent;
		};

		class Renderer;

		class FontSetter
		{
		public:
			FontSetter(Renderer& renderer)
				:m_renderer(&renderer)
			{}
		private:
			Renderer* m_renderer;
		};

		class Clipper
		{
		public:
			virtual ~Clipper();
			Clipper()
				:m_renderer(nullptr)
			{ }
			Clipper& operator=(Clipper&& other)
			{
				if (&other == this)
					return *this;
				m_renderer = other.m_renderer;
				other.m_renderer = nullptr;
				return *this;
			}
			Clipper(Clipper&& other)
			{
				if (&other == this)
					return;
				m_renderer = other.m_renderer;
				other.m_renderer = nullptr;
			}
		private:
			friend class Renderer;
			Clipper(Renderer* renderer)
			{
				m_renderer = renderer;
			}
			Clipper(const Clipper&) = delete;
			Clipper& operator=(const Clipper&) = delete;
			Renderer* m_renderer;
		};


		template<class DATA_TYPE>
		class TrieNode
		{
		public:
			TrieNode()
				:m_isEnd(false)
			{
			}
			TrieNode(std::initializer_list<sci::string> strings)
				:m_isEnd(false)
			{
				for (auto& s : strings)
					add(s);
			}
			TrieNode(std::initializer_list<std::pair<sci::string, DATA_TYPE>> strings)
				:m_isEnd(false)
			{
				for (auto& s : strings)
					add(s.first, s.second);
			}
			void add(const sci::string &string, size_t start = 0, size_t length = std::string::npos)
			{
				if (length == 0 || start == string.length())
				{
					m_isEnd = true;
					return;
				}
				auto next = m_nodes.find(string[start]);
				if (next == m_nodes.end())
				{
					TrieNode newNode;
					newNode.add(string, start + 1, length - 1);
					m_nodes.insert(std::pair<char16_t, TrieNode>(string[start], newNode));
				}
			}
			void add(const sci::string& string, const DATA_TYPE &data, size_t start = 0, size_t length = std::string::npos)
			{
				if (length == 0 || start == string.length())
				{
					m_isEnd = true;
					m_data = data;
					return;
				}
				auto next = m_nodes.find(string[start]);
				if (next == m_nodes.end())
				{
					TrieNode newNode;
					newNode.add(string, data, start + 1, length - 1);
					m_nodes.insert(std::pair<char16_t, TrieNode>(string[start], newNode));
				}
			}
			std::optional<size_t> startsWithContainedWord(const sci::string& string, size_t start = 0) const
			{
				if (m_isEnd)
					return start;

				auto next = m_nodes.find(string[start]);
				if (next == m_nodes.end())
					return {};

				return next->second.startsWithContainedWord(string, start + 1);
			}
			std::optional<DATA_TYPE> isContainedWord(const sci::string& string, size_t start = 0, size_t length = std::string::npos) const
			{
				if (length == 0 || start == string.length())
					return m_isEnd ? std::optional<DATA_TYPE>(m_data) : std::nullopt;

				auto next = m_nodes.find(string[start]);
				if (next == m_nodes.end())
					return {};


				return next->second.isContainedWord(string, start + 1, length - 1);
			}
		private:
			void merge(const TrieNode& other)
			{
				for (const auto& n : other.m_nodes)
				{
					auto matching = m_nodes.find(n.first);
					if (matching == m_nodes.end())
						m_nodes.insert(n);
					else
						matching->second.merge(n.second);
				}
			}
			std::map<char16_t, TrieNode> m_nodes;
			bool m_isEnd;
			DATA_TYPE m_data;
		};

		using trie_t = std::pair<sci::string, sci::string>;
		const TrieNode<sci::string> g_mathCommands
		{
			trie_t{sU("hat"), sU("^")},
			trie_t{sU("_"), sU("_")},
			trie_t{sU("backslash"), sU("\\")},
			trie_t{sU("alpha"), sU("α")},
			trie_t{sU("beta"), sU("β")},
			trie_t{sU("gamma"), sU("γ")},
			trie_t{sU("Gamma"), sU("Γ")}
		};

		class Renderer
		{
		public:
			enum FontFamily //currently the intersection of svg and wxWidgets - see https://developer.mozilla.org/en-US/docs/Web/CSS/font-family#generic-name and https://docs.wxwidgets.org/3.2/interface_2wx_2font_8h.html#a0cd7bfd21a4f901245d3c86d8ea0c080
			{
				defaultFont,
				sansSerif, //swiss
				serif, //roman
				mono, //teletype
				cursive, //script
				fantasy //decorative
			};
			class Font
			{
			public:
				Font(std::vector<sci::string> facenames = std::vector<sci::string>(), Length size = textPoint(12.0), RgbColour colour = RgbColour(), FontFamily backupFamily = FontFamily::defaultFont, bool bold = false, bool italic = false, bool underline = false, RgbColour backgroundColour = RgbColour(0, 0, 0, 1))
					:m_facenames(facenames), m_size(size), m_colour(colour), m_backupFamily(backupFamily), m_bold(bold), m_italic(italic), m_underline(underline), m_backgroundColour(backgroundColour)
				{}
		
				Font(sci::string facename, Length size = textPoint(12.0), RgbColour colour = RgbColour(), FontFamily backupFamily = FontFamily::defaultFont, bool bold = false, bool italic = false, bool underline = false, RgbColour backgroundColour = RgbColour(0, 0, 0, 1))
					:m_facenames(std::vector<sci::string>(1,facename)), m_size(size), m_colour(colour), m_backupFamily(backupFamily), m_bold(bold), m_italic(italic), m_underline(underline), m_backgroundColour(backgroundColour)
				{}
		
				std::vector<sci::string> m_facenames;
				Length m_size;
				RgbColour m_colour;
				FontFamily m_backupFamily;
				bool m_bold;
				bool m_italic;
				bool m_underline;
				RgbColour m_backgroundColour;
			};

			virtual void pushState() = 0;
			virtual void popState() = 0;
			virtual void setBrush(const HlsColour& colour) = 0;
			virtual void setBrush(const RgbColour& colour) = 0;
			virtual void setPen(const HlsColour& colour, const Length &thickness, const std::vector<Length> &dashes = std::vector<Length>(0)) = 0;
			virtual void setPen(const RgbColour& colour, const Length &thickness, const std::vector<Length>& dashes = std::vector<Length>(0)) = 0;
			virtual void line(const Point& p1, const Point& p2) = 0;
			virtual void line(const Point& point, const Distance& distance) = 0;
			virtual TextMetric text(const sci::string &str, const Point& position, unitless horizontalAlignment, unitless verticalAlignment) = 0;
			TextMetric formattedText(const sci::string& str, const Point& position, unitless horizontalAlignment = unitless(0.0), unitless verticalAlignment = unitless(0.0), degree rotation = degree(0.0), textPoint minTextSize = textPoint(5))
			{
				TextMetric result;
				unitless spacing(1.25);

				//break the text into lines
				size_t newLine = -1;
				millimetre verticalOffset(0.0);
				bool firstLine = true;
				do
				{
					size_t lineStart = newLine + 1;
					newLine = str.find(sU('\n'), lineStart);
					textChunk text(str.substr(lineStart, newLine - lineStart));

					TextMetric extent = text.getExtent(*this, minTextSize);
					if (firstLine)
					{
						verticalOffset += extent.ascent * verticalAlignment;
						result = extent;
						firstLine = false;
					}
					else
					{
						result.width = std::max(result.width, extent.width);
						result.bottomDescent += (extent.ascent + extent.descent) * spacing;
					}

					Distance alignmentOffset(millimetre((extent.width * horizontalAlignment) * sci::cos(rotation) + (verticalOffset)*sci::sin(rotation)),
						-millimetre(extent.width * horizontalAlignment) * sci::sin(rotation) + (verticalOffset)*sci::cos(rotation));
					text.render(*this, minTextSize, position - alignmentOffset, rotation);
					verticalOffset -= (extent.ascent + extent.descent) * spacing;


				} while (newLine != sci::string::npos);

				return result;
			}
			virtual TextMetric rotatedText(const sci::string& str, const Point& position, unitless horizontalAlignment, unitless verticalAlignment, degree rotation) = 0;
			virtual TextMetric getUnformattedTextExtent(const sci::string& str) = 0;
			virtual void setFont(const Font &font)
			{
				setFont(font.m_facenames, font.m_size, font.m_colour, font.m_backupFamily, font.m_bold, font.m_italic, font.m_underline, font.m_backgroundColour);
			}
			virtual void setFont(sci::string facename, Length size, RgbColour colour, FontFamily backupFamily = FontFamily::defaultFont, bool bold = false, bool italic = false, bool underline = false, RgbColour backgroundColour = RgbColour(0, 0, 0, 1)) = 0;
			virtual void setFont(std::vector<sci::string> facenames, Length size, RgbColour colour, FontFamily backupFamily = FontFamily::defaultFont, bool bold = false, bool italic = false, bool underline = false, RgbColour backgroundColour = RgbColour(0, 0, 0, 1)) = 0;
			virtual void scaleFontSize(unitless scale) = 0;
			virtual millimetre getFontSize() const = 0;
			virtual void elipse(const Point& position, const Distance& radius, unitless xAlignemnt = unitless(0.5), unitless yAlignment = unitless(0.5)) = 0;
			virtual void rectangle(const Point& position, const Distance& size, unitless xAlignemnt = unitless(0.0), unitless yAlignment = unitless(0.0)) = 0;
			virtual void rectangle(const Point& corner1, const Point& corner2)
			{
				rectangle(corner1, corner2 - corner1, unitless(0.0), unitless(0.0));
			}
			virtual void polyLine(const std::vector<Point>& points) = 0;
			virtual void polygon(const std::vector<Point>& points) = 0;
			virtual degree getAngle(const Distance& distance) const = 0;
			//this project might be worth a look at some time https://github.com/bkaradzic/bgfx?tab=readme-ov-file

			class textChunk
			{
			public:
				textChunk(const sci::string& str)
				{
					m_text = str;
					m_baselineOffset = unitless(0.0);
					m_size = unitless(1.0);
					parse();
				}
				TextMetric getExtent(Renderer& renderer, millimetre minSize) const
				{
					TextMetric basicExtent = renderer.getUnformattedTextExtent(sU("M"));
					return getExtent(renderer, renderer.getFontSize(), minSize, basicExtent.ascent, false, Point(), millimetre(0.0), millimetre(0.0), degree(0));
				}
				TextMetric render(Renderer& renderer, millimetre minSize, Point point, degree rotation) const
				{
					TextMetric basicExtent = renderer.getUnformattedTextExtent(sU("M"));
					point += Distance(millimetre(basicExtent.ascent * sci::sin(rotation)), millimetre(basicExtent.ascent * sci::cos(rotation)));
					return getExtent(renderer, renderer.getFontSize(), minSize, basicExtent.ascent, true, point, millimetre(0.0), millimetre(0.0), rotation);
				}
				sci::string m_text;
				std::vector<std::unique_ptr<textChunk>> m_chunks;
				unitless m_baselineOffset;
				unitless m_size;
				/*std::optional<sci::string> m_facename;
				std::optional<double> m_size;
				std::optional <RgbColour> m_colour;
				std::optional <Renderer::FontFamily> m_backupFamily;
				std::optional<bool> m_bold;
				std::optional<bool> m_italic;
				std::optional<bool> m_underline;
				std::optional<bool> m_overline;
				std::optional <RgbColour> m_backgroundColour;
				TextMetric m_extent;*/

				//returns the index of the closing bracket which matches with the bracket at openingBracketPosition
				//Only looks at the substring from openingBracketPosition with length maxLength. Set to npos to
				//check the whole string if needed
				//returns npos if no matching close bracket was found or the character indicated at openingBracketPosition
				//is none of ( { [
				static size_t findClosingBracket(const sci::string &str, size_t openingBracketPosition, size_t maxLength = sci::string::npos)
				{
					const sci::string::basic_string::value_type openingBracket = str[openingBracketPosition];
					sci::string::basic_string::value_type closingBracket;
					if (openingBracket == sU('('))
						closingBracket = sU(')');
					else if (openingBracket == sU('['))
						closingBracket = sU(']');
					else if (openingBracket == sU('{'))
						closingBracket = sU('}');
					else
						return sci::string::npos;
					size_t position = openingBracketPosition + 1;
					size_t openCount = 1;
					while (position - openingBracketPosition < maxLength && position < str.length())
					{
						if (str[position] == closingBracket)
							--openCount;
						else if (str[position] == openingBracket)
							++openCount;

						if (openCount == 0)
							return position;
						++position;
					}

					return std::string::npos;
				}

				static bool isCatcode11(const sci::string::basic_string::value_type& character)
				{
					return (character >= sU('a') && character <= sU('z')) || (character >= sU('A') && character <= sU('Z'));
				}
				static size_t getCommandNameEnd(const sci::string& string, size_t start)
				{
					//assume start points to the \ character

					//check we have at least one character in the string
					if (start+1 == string.length())
						return start+1;

					//We copy the latex command structure. A command is either multiple characters
					//with what's called catcode 11 or a single character with any catcode other than 11.
					//The characters with catcode 11 are a,...,z and A,...,Z. This can in theory be changed
					//using the \catcode command, but I think life is too short and I'm not writing an
					//entire latex parser here.

					//check the single character non catcode 11 case
					if (!isCatcode11(string[start + 1]))
						return start + 2;

					//find the first character that is not catcode 11
					size_t end = start + 2;
					for (; end < string.length(); ++end)
					{
						if (!isCatcode11(string[end]))
							break;
					}
					//gobble any spaces for multi character commands
					for (; end < string.length(); ++end)
						if (!sci::isWhitespace(string, end))
							break;
			
					return end;
				}

				size_t getChunkEnd(const sci::string& string, size_t start)
				{

					//we have the following options
					// _3
					// ^3
					// _{more than one character}
					// ^{more than one character}
					// \command
					// \command{argument}
					// \command{argument1}{argument2}
					// \command[optionalArgument]{argument} #can have multiple optional and not optional arguments

					//right now we are not supporting commands with arguments

					//if we only have the start character, the end is the string end
					if (string.length() - start == 1)
						return sci::string::npos;

					//for _ and ^ find if we have the single character or bracket version and return appropriately
					if (string[start] == sU('^') || string[0] == sU('^'))
					{
						if (string[start + 1] == sU('{'))
							return findClosingBracket(string, start + 1) + 1;
						else
							return start + 2;
					}

					//if we have a \, check if this is a command, if so return appropriately
					if (string[start] == sU('\\'))
					{
						size_t commandEnd = getCommandNameEnd(string, start);
						std::optional<size_t> end = g_mathCommands.startsWithContainedWord(string, start + 1);
						return end.value_or(sci::string::npos);
					}
			
					//if we get here then we don't start with a special character, the end is either the
					//next special character or the string end.
					return string.find_first_of(sU("\\^_"), start + 1);

				}

				void parse()
				{

					if (m_text.length() == 0)
						return;

					//if this is a chunk where the whole string is a sub or super script then sort that out
					if ((m_text[0] == sU('^') || m_text[0] == sU('_'))
						&& (m_text.length() <=2 ||(m_text[1]==sU('{') && m_text.back() == sU('}'))))
					{
						m_size = unitless(0.66);
						if (m_text[0] == sU('^'))
							m_baselineOffset = unitless(0.66);
						else
							m_baselineOffset = unitless(-0.22);

						//trim the command characters
						if (m_text.length() < 2)
						{
							m_text = sU("");
							return;
						}
						else if (m_text[1] == sU('{') && m_text.back() == sU('}'))
							m_text = m_text.substr(2, m_text.length() - 3);
						else
							m_text = m_text.substr(1);
					}

					//if this is a chunk that is just a command then sort that out
					//note we don't yet support commands with parameters
					if (m_text[0] == sU('\\'))
					{
						//check for a simple escape
						if (m_text.length() == 2 && isCatcode11(m_text[1]))
							m_text = m_text.substr(1);
						else
						{
							//trim blanks from the end of a command
							while (sci::isWhitespace(m_text, m_text.length() - 1))
								m_text.pop_back();
							std::optional<sci::string> replacement = g_mathCommands.isContainedWord(m_text, 1);
							m_text = replacement.value_or(m_text);
						}
					}

					//split into sub chunks
					size_t currentStart = 0;
					size_t currentEnd = getChunkEnd(m_text, currentStart);
					while (currentEnd != sci::string::npos && currentEnd != m_text.length())
					{
						m_chunks.push_back(std::unique_ptr<textChunk>(new textChunk(m_text.substr(currentStart, currentEnd - currentStart))));

						currentStart = currentEnd;
						currentEnd = getChunkEnd(m_text, currentStart);
					}

					//once we leave the loop above currentStart and currentEnd will delimit the last chunk in the string
					//if we have other chunks in m_chunks then add this and blank out the string
					//if we have no other chunks then m_text holds the only chunk so just leave it as it is
					if (m_chunks.size() > 0)
					{
						m_chunks.push_back(std::unique_ptr<textChunk>(new textChunk(m_text.substr(currentStart, currentEnd - currentStart))));
						m_text = sU("");
					}
				}

				TextMetric getExtent(Renderer &renderer, millimetre currentSize, millimetre minSize, millimetre currentAscendor, bool render, Point baselineStartPoint, millimetre baselineX, millimetre baselineY, degree rotation) const
				{
					baselineY += currentAscendor * m_baselineOffset;
					TextMetric extent;
					extent.width = millimetre(0.0);
					extent.ascent = millimetre(0.0);
					extent.descent = millimetre(0.0);
					extent.topAscent = millimetre(0.0);
					extent.bottomDescent = millimetre(0.0);
					unitless textScale = std::max(m_size, unitless(minSize / currentSize));
					if (textScale != unitless(1.0))
						renderer.scaleFontSize(textScale);
					try
					{
						if (m_text.length() > 0)
						{
							//get the extent so we know how much to lift from the baseline
							extent = renderer.getUnformattedTextExtent(m_text);
							if (render)
							{
								Distance alignmentOffset(millimetre(baselineX * sci::cos(rotation) + (-(baselineY + extent.ascent)) * sci::sin(rotation)),
									millimetre(-baselineX * sci::sin(rotation) + (-(baselineY + extent.ascent)) * sci::cos(rotation)));
								extent = renderer.rotatedText(m_text, baselineStartPoint + alignmentOffset, unitless(0.0), unitless(0.0), rotation);
							}
							baselineX += extent.width;

						}
						else
						{
							extent = renderer.getUnformattedTextExtent(sU("")); //this will set the ascent and descent
							millimetre standardAscent = extent.ascent;
							for (auto& c : m_chunks)
							{
								TextMetric thisExtent = c->getExtent(renderer, currentSize * textScale, minSize, standardAscent, render, baselineStartPoint, baselineX, baselineY, rotation);
								extent.width += thisExtent.width;
								extent.topAscent = std::max(thisExtent.ascent, extent.topAscent);
								extent.bottomDescent = std::max(thisExtent.descent, extent.bottomDescent);
								baselineX += thisExtent.width;
							}
						}
					}
					catch (...)
					{
						if (textScale != unitless(1.0))
							renderer.scaleFontSize(unitless(1.0) / textScale);
						throw;
					}
					if (textScale != unitless(1.0))
						renderer.scaleFontSize(unitless(1.0) / textScale);

					//shift the ascent and descent based upon the baseline offset
					extent.ascent += currentAscendor * m_baselineOffset;
					extent.descent -= currentAscendor * m_baselineOffset;
					extent.topAscent += currentAscendor * m_baselineOffset;
					extent.bottomDescent -= currentAscendor * m_baselineOffset;

					//return the result
					return extent;
				}
			};
			friend class Clipper;
			Clipper addClippingRegion(Point point, Distance distance)
			{
				applyClippingRegion(point, distance);
				return std::move(Clipper(this));
			}
			Clipper addClippingRegion(Point corner1, Point corner2)
			{
				applyClippingRegion(corner1, corner2);
				return std::move(Clipper(this));
			}
			Clipper addClippingRegion(std::span<const Point> points)
			{
				applyClippingRegion(points);
				return std::move(Clipper(this));
			}
		private:
			virtual void applyClippingRegion(Point point, Distance distance) = 0;
			virtual void applyClippingRegion(Point corner1, Point corner2) = 0;
			virtual void applyClippingRegion(std::span<const Point> points) = 0;
			virtual void popClippingRegion() = 0;
		};


		inline Clipper::~Clipper()
		{
			if (m_renderer)
				m_renderer->popClippingRegion();
		}

		class StatePusher
		{
		public:
			StatePusher(Renderer* renderer)
			: m_renderer(renderer)
			{
				m_renderer->pushState();
			}
			~StatePusher()
			{
				m_renderer->popState();
			}
		private:
			Renderer* m_renderer;

		};

		class NullRenderer : public Renderer
		{
		public:

			virtual void pushState() override
			{
			}
			virtual void popState() override
			{
			}
			virtual void setBrush(const HlsColour& colour) override
			{
			}
			virtual void setBrush(const RgbColour& colour) override
			{
			}
			virtual void setPen(const HlsColour& colour, const Length& thickness, const std::vector<Length>& dashes = std::vector<Length>(0)) override
			{
			}
			virtual void setPen(const RgbColour& colour, const Length& thickness, const std::vector<Length>& dashes = std::vector<Length>(0)) override
			{
			}
			virtual void setFont(sci::string facename, Length size, RgbColour colour, Renderer::FontFamily backupFamily = Renderer::FontFamily::defaultFont, bool bold = false, bool italic = false, bool underline = false, RgbColour backgroundColour = RgbColour(0, 0, 0, 1)) override
			{
			}
			virtual void setFont(std::vector<sci::string> facenames, Length size, RgbColour colour, Renderer::FontFamily backupFamily = Renderer::FontFamily::defaultFont, bool bold = false, bool italic = false, bool underline = false, RgbColour backgroundColour = RgbColour(0, 0, 0, 1)) override
			{
			}
			millimetre getFontSize() const override
			{
				return millimetre(0);
			}
			virtual void line(const Point& p1, const Point& p2) override
			{
			}

			virtual void line(const Point& point, const Distance& distance) override
			{
			}

			virtual TextMetric text(const sci::string& str, const Point& position, unitless horizontalAlignment, unitless verticalAlignment) override
			{
				return TextMetric{ millimetre(0), millimetre(0), millimetre(0), millimetre(0), millimetre(0) };
			}

			virtual TextMetric rotatedText(const sci::string& str, const Point& position, unitless horizontalAlignment, unitless verticalAlignment, degree rotation) override
			{
				return TextMetric{ millimetre(0), millimetre(0), millimetre(0), millimetre(0), millimetre(0) };
			}

			virtual TextMetric getUnformattedTextExtent(const sci::string& str) override
			{
				return TextMetric{ millimetre(0), millimetre(0), millimetre(0), millimetre(0), millimetre(0) };
			}
			virtual void elipse(const Point& position, const Distance& radius, unitless xAlignemnt = unitless(0.5), unitless yAlignment = unitless(0.5)) override
			{
			}
			virtual void rectangle(const Point& position, const Distance& size, unitless xAlignemnt = unitless(0.0), unitless yAlignment = unitless(0.0)) override
			{
			}
			virtual void rectangle(const Point& corner1, const Point& corner2) override
			{
			}
			void scaleFontSize(unitless scale) override
			{
			}
			virtual void polyLine(const std::vector<Point>& points)
			{
			}

			virtual void polygon(const std::vector<Point>& points)
			{
			}

			virtual degree getAngle(const Distance& distance) const
			{
				return degree(0);
			}

			virtual void applyClippingRegion(Point point, Distance distance) override
			{
			}

			virtual void applyClippingRegion(Point corner1, Point corner2) override
			{
			}

			virtual void applyClippingRegion(std::span<const Point> points) override
			{
			}

			virtual void popClippingRegion() override
			{
			}
		};

	}
}

#endif