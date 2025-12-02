#ifndef SCI_RENDERING_H
#define SCI_RENDERING_H

#include<vector>
#include<array>
#include<cmath>
#include<map>
#include<span>
#include"string.h"
#include<fstream>
#include<ranges>
#include<memory>
namespace sci
{
	struct Point
	{
		Point(){}
		Point(float pointX, float pointY)
			:x(pointX), y(pointY)
		{ }
		float x;
		float y;
	};

	struct Box
	{
		Box(){}
		Box(Point topLeft, Point bottomRight)
			:tl(topLeft), br(bottomRight){ }
		Point tl;
		Point br;
	};

	const std::array<Point, 4> coveragePoints{ Point { 0.3f, 0.4f },
		Point { 0.6f, 0.3f },
		Point { 0.7f, 0.6f },
		Point { 0.4f, 0.7f } };

	

	struct Colour
	{
		Colour() {}
		Colour(float red, float green, float blue, float alpha = 1.0f)
			:r(red), g(green), b(blue), a(alpha)
		{ }
		float r;
		float g;
		float b;
		float a;
	};

	struct ColourOpaque
	{
		float r;
		float g;
		float b;
	};

	inline void overlay(ColourOpaque& base, const Colour& top)
	{
		base.r = base.r * (1.0f - top.a) + top.r * top.a;
		base.g = base.g * (1.0f - top.a) + top.g * top.a;
		base.b = base.b * (1.0f - top.a) + top.b * top.a;
	}

	inline Colour overlay(Colour base, Colour top)
	{
		float t2 = 1.0f - top.a;
		return Colour(top.r * top.a + base.r * base.a * t2,
			top.g * top.a + base.g * base.a * t2,
			top.b * top.a + base.b * base.a * t2,
			top.a + base.a * t2);
	}

	struct Coverage
	{
		Coverage() {}
		Coverage(uint8_t coverageShape, float coverageFraction, Colour coverageColour)
			:shape(coverageShape), fraction(coverageFraction), colour(coverageColour)
		{ }
		uint8_t shape;
		float fraction;
		Colour colour;
	};

	inline uint8_t countBits(uint8_t x)
	{
		//https://stackoverflow.com/questions/4244274/how-do-i-count-the-number-of-zero-bits-in-an-integer
		x = x - ((x >> 1) & 0x55);
		x = (x & 0x33) + ((x >> 2) & 0x33);
		x = (x + (x >> 4)) & 0x0f;
		return x;
	}

	inline Coverage splitOverlap(Coverage& coverageBottom, Coverage& coverageTop)
	{
		float t1 = 1.0f - coverageBottom.colour.a;
		float t2 = 1.0f - coverageTop.colour.a;
		Coverage overlapCoverage(uint8_t(0), 0.0f, overlay(coverageBottom.colour, coverageTop.colour));

		uint8_t minCoveredPoints = std::min(countBits(coverageBottom.shape), countBits(coverageTop.shape));

		if (minCoveredPoints == 0)
			return overlapCoverage;

		overlapCoverage.shape = countBits(coverageBottom.shape & coverageTop.shape);
		overlapCoverage.fraction = std::min(coverageBottom.fraction, coverageTop.fraction) * float(countBits(overlapCoverage.shape)) / float(minCoveredPoints);
		coverageBottom.fraction -= overlapCoverage.fraction;
		coverageTop.fraction -= overlapCoverage.fraction;

		return overlapCoverage;
	}

	inline void alphaBlend(ColourOpaque& base, Coverage coverage1, Coverage coverage2)
	{
		float overlapCoverage;
		uint8_t minCoveredPoints = countBits(coverage1.shape);
		uint8_t maxCoveredPoints = countBits(coverage2.shape);
		if (minCoveredPoints > maxCoveredPoints)
			std::swap(minCoveredPoints, maxCoveredPoints);

		if (maxCoveredPoints == 0 || minCoveredPoints == 0)
			overlapCoverage = 0.0f;

		else
		{
			uint8_t nAllCoveredPoints = countBits(coverage1.shape & coverage2.shape);
			overlapCoverage = std::min(coverage1.fraction, coverage2.fraction) * float(nAllCoveredPoints) / float(minCoveredPoints);
		}
		coverage1.fraction -= overlapCoverage;
		coverage2.fraction -= overlapCoverage;
		
		float baseWeight = 1.0f - coverage1.fraction - coverage2.fraction - overlapCoverage
			+ coverage1.fraction * (1.0f - coverage1.colour.a)
			+ coverage2.fraction * (1.0f - coverage2.colour.a)
			+ overlapCoverage * (1.0f - coverage1.colour.a) * (1.0f - coverage2.colour.a);
		float colour1Weight = coverage1.fraction * coverage1.colour.a
			+ overlapCoverage * coverage1.colour.a * (1 - coverage2.colour.a);
		float colour2Weight = coverage2.fraction * coverage2.colour.a
			+ overlapCoverage * coverage2.colour.a;

		base.r = base.r * baseWeight + coverage1.colour.r * colour1Weight + coverage2.colour.r * colour2Weight;
		base.g = base.g * baseWeight + coverage1.colour.g * colour1Weight + coverage2.colour.g * colour2Weight;
		base.b = base.b * baseWeight + coverage1.colour.b * colour1Weight + coverage2.colour.b * colour2Weight;
	}

	inline Colour blend(Colour lower, Colour upper)
	{

	}

	class Shape
	{
	public:
		virtual Box getBoundingBox() const = 0;
		virtual void renderScanLine(std::span<ColourOpaque> line, float lineIndex, std::vector<bool>& partials) = 0;
		virtual Coverage getCoverage(float row, float column) const = 0;
		//virtual bool getMoreThanHalfCovered(Box box) const = 0;
	};

	class Rectangle : public Shape
	{
	public:
		Rectangle(Point corner, float width, float height, Colour colour)
			:m_top(height < 0.0f ? corner.y + height : corner.y),
			m_bottom(height < 0.0f ? corner.y : corner.y + height),
			m_left(width < 0.0f ? corner.x + width : corner.x),
			m_right(width < 0.0f ? corner.x : corner.x + width),
			m_colour(colour)
		{
		}
		Box getBoundingBox() const final
		{
			Box result;
			result.tl = Point(m_left, m_top);
			result.br = Point(m_right, m_bottom);
			return result;
		}
		
		void renderScanLine(std::span<ColourOpaque> line, float lineIndex, std::vector<bool> &partials)
		{
			float verticalFraction = 1.0f - std::min(1.0f, std::max(0.0f, m_top - lineIndex)) - std::min(1.0f, std::max(0.0f, lineIndex + 1.0f - m_bottom));
			if (verticalFraction == 0.0f)
				return;
			//get the start and end for pixels fully within the x bounds
			long startIndex = std::max(0l, long(m_left)+1);
			long endIndex = std::min(long(std::size(line)), long(m_right));
			if (verticalFraction == 1.0)
			{
				for (size_t i = startIndex; i < endIndex; ++i)
					overlay(line[i], m_colour);
				long edge1 = startIndex - 1;
				long edge2 = endIndex;
				if (edge1 >= 0 && edge1 < long(std::size(line)))
					partials[edge1] = true;
				if (edge2 >= 0 && edge2 < long(std::size(line)))
					partials[edge2] = true;
			}
			else
			{
				startIndex = startIndex == 0 ? startIndex :startIndex - 1;
				endIndex = endIndex == long(std::size(line)) ? endIndex : endIndex + 1;
				for (size_t i = startIndex; i < endIndex; ++i)
					partials[i] = true;
			}
		}

		Coverage getCoverage(float row, float column) const
		{
			float relativeRight = m_right - column;
			float relativeLeft = m_left - column;
			float relativeBottom = m_bottom - row;
			float relativeTop = m_top - row;
			float verticalFraction = 1.0f - std::max(std::min(1.0f, relativeTop), 0.0f) - std::max(std::min(1.0f, 1.0f - relativeBottom), 0.0f);
			float horizontalFraction = 1.0f - std::max(std::min(1.0f, relativeLeft), 0.0f) - std::max(std::min(1.0f, 1.0f - relativeRight), 0.0f);

			uint8_t coverageShapeX = 0;
			if (0.0f >= relativeLeft && 0.0f <= relativeRight)
				coverageShapeX |= 0x09;
			if (1.0f >= relativeLeft && 1.0f <= relativeRight)
				coverageShapeX |= 0x06;

			uint8_t internalPointMask = 0x10;
			for (const auto& p : coveragePoints)
			{
				if (p.x > relativeLeft && p.x < relativeRight)
					coverageShapeX |= internalPointMask;
				internalPointMask <<= 1;
			}

			uint8_t coverageShapeY = 0;
			if (0.0f >= relativeTop && 0.0f <= relativeBottom)
				coverageShapeY |= 0x03;
			if (1.0f >= relativeTop && 1.0f <= relativeBottom)
				coverageShapeY |= 0x0c;

			internalPointMask = 0x10;
			for (const auto& p : coveragePoints)
			{
				if (p.y > relativeTop && p.y < relativeBottom)
					coverageShapeY |= internalPointMask;
				internalPointMask <<= 1;
			}

			return Coverage(coverageShapeX & coverageShapeY, verticalFraction * horizontalFraction, m_colour);
		}


		
	private:

		float m_top;
		float m_bottom;
		float m_left;
		float m_right;
		Colour m_colour;
	};
	
	class BitmapCanvas
	{
	public:
		void create(size_t nColumns, size_t nRows, ColourOpaque backgroundColour)
		{
			m_nColumns = nColumns;
			m_backgroundColour = backgroundColour;
			m_pixels.assign(nColumns * nRows, backgroundColour);
		}

		void renderScene()
		{
			size_t nRows = m_pixels.size() / m_nColumns;
			std::vector<bool> partials(m_nColumns);
			std::vector<Coverage> coverages; //keep this out of the loop to avoid unnecessary memory allocations
			coverages.reserve(m_nColumns);
			for (size_t i = 0; i < nRows; ++i)
			{
				//render the full pixels
				partials.assign(m_nColumns, false);
				std::span<ColourOpaque> line(m_pixels.begin() + m_nColumns * i, m_pixels.begin() + m_nColumns * (i + 1));
				const std::vector<std::unique_ptr<Shape>> &intersectingShapes = getPotentiallyIntersectingShapes(i);
				for (const std::unique_ptr<Shape>& s : intersectingShapes)
				{
					s->renderScanLine(line, float(i), partials);
				}

				//deal with the partials
				for (size_t j = 0; j < partials.size(); ++j)
				{
					coverages.clear();
					//get the coverages from each shape. The add coverage function
					//gets the coverage from the shape, but then splits it into overlapping
					//parts with each previous shape
					if (partials[j])
						for (const std::unique_ptr<Shape>& s : intersectingShapes)
							addCoverage(coverages, s, i, j);

					//if the total coverage ends up bigger than 1, reduce it
					float totalCoverage = 0.0f;
					for (const auto& c : coverages)
						totalCoverage += c.fraction;
					if (totalCoverage > 1.0f)
					{
						for (auto& c : coverages)
							c.fraction /= totalCoverage;
						totalCoverage = 1.0;
					}

					//do a weighted average of all the coverages
					const ColourOpaque originalColour = line[j];
					line[j].r *= (1.0f - totalCoverage);
					line[j].g *= (1.0f - totalCoverage);
					line[j].b *= (1.0f - totalCoverage);
					for (const auto& c : coverages)
					{
						ColourOpaque overlaidColour(originalColour);
						overlay(overlaidColour, c.colour);
						line[j].r += overlaidColour.r * c.fraction;
						line[j].g += overlaidColour.g * c.fraction;
						line[j].b += overlaidColour.b * c.fraction;
					}
				}
			}
		}
		void addCoverage(std::vector<Coverage>& coverages, const std::unique_ptr<Shape>& s, size_t row, size_t column)
		{
			Coverage coverage = s->getCoverage(float(row), float(column));
			if (coverage.fraction > 0)
			{
				size_t initialSize = coverages.size();
				for (size_t i = 0; i < initialSize; ++i)
				{
					Coverage overlap = splitOverlap(coverages[i], coverage);
					if (overlap.fraction > 0)
						coverages.push_back(overlap);
				}
				coverages.push_back(coverage);
			}
		}
		const std::vector<std::unique_ptr<Shape>> &getPotentiallyIntersectingShapes(size_t scanline)
		{
			return m_shapes;
		}
		bool writePpm(sci::string filename)
		{
			std::fstream fout(sci::toNativeUnicode(filename), std::ios::binary | std::ios::out);
			if (!fout.is_open())
				return false;
			fout << "P6 " << m_nColumns << " " << m_pixels.size()/m_nColumns << " 255 ";

			for (const auto& c : m_pixels)
			{
				std::array<uint8_t, 3> colours;
				colours[0] = colourFloatTo255(c.r);
				colours[1] = colourFloatTo255(c.g);
				colours[2] = colourFloatTo255(c.b);
				fout.write((char*)(&colours[0]), 3);
			}
			return true;
		}
		void addRectangle(const Rectangle &rectangle)
		{
			//m_shapes.push_back(std::move(std::unique_ptr<Shape>(new Rectangle(rectangle))));
			m_shapes.push_back(std::move(std::unique_ptr<Shape>(new Rectangle(rectangle))));
		}
	private:
		uint8_t colourFloatTo255(float c)
		{
			const float offset = 1.0f / 256.0f / 2.0f; //we add half an increment before flooring - this means we round rather than floor
			return uint8_t(std::floor(std::max(std::min(c, 1.0f), 0.0f) * 255.0f + offset));
		}
		/*template<class SHAPEITER>
		void renderShape(SHAPEITER shape)
		{
			using boundingBoxType = decltype(shape->getBoundingBox());

			boundingBoxType boundingBox = shape->getBoundingBox();
			boundingBox.xMin = boundingBox.xMin < LENGTHTYPE(0.0) ? LENGTHTYPE(0.0) : boundingBox.xMin;
			boundingBox.xMax = boundingBox.xMax > width ? width : boundingBox.xMax;
			boundingBox.yMin = boundingBox.yMin < LENGTHTYPE(0.0) ? LENGTHTYPE(0.0) : boundingBox.yMin;
			boundingBox.yMax = boundingBox.yMax > m_height ? m_height: boundingBox.yMax;
			size_t columnBegin = std::floor(boundingBox.xMin / pixelSize);
			size_t columnEnd = std::floor(boundingBox.xMax / pixelSize) + 1;
			size_t rowBegin = std::floor(boundingBox.yMin / pixelSize);
			size_t rowEnd = std::floor(boundingBox.yMax / pixelSize) + 1;

			//render the fully covered pixels and store the partially covered pixels for later
			for (size_t i = rowBegin; i < rowEnd; ++i)
			{
				for (size_t j = columnBegin; j < columnEnd; ++j)
				{
					size_t pixelIndex = i * m_nColumns + j;

					PixelRenderInfo pixelRenderInfo;
					pixelRenderInfo.shape = shape;
					pixelRenderInfo.colour = colour;

					renderShapeToPixel(pixelRenderInfo, pixelIndex, pixelSize);
				}
			}

			//The partially covered pixels need information about multiple shapes to
			//render - do that now
			for (auto& pixelRenderInfoVector : m_delayedPixelRenderInfo)
			{
				//renderShapeToPixelDelayed();
			}
		}*/

		std::vector<ColourOpaque> m_pixels;
		size_t m_nColumns;
		ColourOpaque m_backgroundColour;
		std::vector<std::unique_ptr<Shape>> m_shapes;
	};
}

#endif
