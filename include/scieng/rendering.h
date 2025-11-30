#ifndef SCI_RENDERING_H
#define SCI_RENDERING_H

#include<vector>
#include<array>
#include<cmath>
#include<map>
#include<span>
#include<bitset>
namespace sci
{
	struct Point
	{
		float x;
		float y;
	};

	struct Box
	{
		Point tl;
		Point br;
	};

	const std::array<Point, 4> coveragePoints{ Point { 0.3f, 0.4f },
		Point { 0.6f, 0.3f },
		Point { 0.7f, 0.6f },
		Point { 0.4f, 0.7f } };

	

	struct Colour
	{
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

	void alphaBlend(ColourOpaque& base, const Colour& top)
	{
		base.r = base.r * (1.0f - top.a) + top.r * top.a;
		base.g = base.g * (1.0f - top.a) + top.g * top.a;
		base.b = base.b * (1.0f - top.a) + top.b * top.a;
	}

	struct Coverage
	{
		Coverage() {}
		Coverage(uint8_t coverageType, float coverageFraction, Colour coverageColour)
			:type(coverageType), fraction(coverageFraction), colour(coverageColour)
		{ }
		uint8_t type;
		float fraction;
		Colour colour;
	};

	uint8_t countBits(uint8_t x)
	{
		//https://stackoverflow.com/questions/4244274/how-do-i-count-the-number-of-zero-bits-in-an-integer
		x = x - ((x >> 1) & 0x55);
		x = (x & 0x33) + ((x >> 2) & 0x33);
		x = (x + (x >> 4)) & 0x0f;
		return x;
	}

	void alphaBlend(ColourOpaque& base, Coverage coverage1, Coverage coverage2)
	{
		float bothCoverage;
		uint8_t minCoveredPoints = countBits(coverage1.type);
		uint8_t maxCoveredPoints = countBits(coverage2.type);
		if (minCoveredPoints > maxCoveredPoints)
			std::swap(minCoveredPoints, maxCoveredPoints);

		if (maxCoveredPoints == 0 || minCoveredPoints == 0)
			bothCoverage = 0.0f;

		else
		{
			uint8_t nAllCoveredPoints = countBits(coverage1.type & coverage2.type);
			bothCoverage = std::min(coverage1.fraction, coverage2.fraction) * float(nAllCoveredPoints) / float(minCoveredPoints);
		}
		coverage1.fraction -= bothCoverage;
		coverage2.fraction -= bothCoverage;
		
		float baseWeight = 1.0f - coverage1.fraction - coverage2.fraction - bothCoverage
			+ coverage1.fraction * (1.0f - coverage1.colour.a)
			+ coverage2.fraction * (1.0f - coverage2.colour.a)
			+ bothCoverage * (1.0f - coverage1.colour.a) * (1.0f - coverage2.colour.a);
		float colour1Weight = coverage1.fraction * coverage1.colour.a
			+ bothCoverage * coverage1.colour.a * (1 - coverage2.colour.a);
		float colour2Weight = coverage2.fraction * coverage2.colour.a
			+ bothCoverage * coverage2.colour.a;

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
		virtual Box getBoundingBox() const =0;
		//virtual Coverage getCoverage(Box box) const = 0;
		//virtual bool getMoreThanHalfCovered(Box box) const = 0;
	};

	class Rectangle : public Shape
	{
	public:
		Rectangle(Point corner, float width, float height, Colour colour)
			:m_corner(corner), m_width(width), m_height(height), m_colour(colour)
		{
			if (m_width < 0.0f)
			{
				m_corner.x += m_width;
				m_width *= -1.0f;
			}
			if (m_height < 0.0f)
			{
				m_corner.y += m_height;
				m_height *= -1.0f;
			}
		}
		Box getBoundingBox() const final
		{
			Box result;
			result.tl = m_corner;
			result.br = m_corner;
			result.br.x += m_width;
			result.br.y += m_height;
			return result;
		}
		
		void renderScanLine(std::span<ColourOpaque> line, float lineIndex, std::vector<bool> &partials)
		{
			float verticalFraction = 1.0f - std::min(1.0f, std::max(0.0f, m_corner.y - lineIndex)) - std::min(1.0f, std::max(0.0f, lineIndex + 1.0f - m_corner.y - m_height));
			if (verticalFraction == 0.0f)
				return;
			//get the start and end for pixels fully within the x bounds
			long startIndex = std::max(0l, long(m_corner.x)+1);
			long endIndex = std::min(long(std::size(line)), long(m_corner.x + m_width));
			if (verticalFraction == 1.0)
			{
				for (size_t i = startIndex; i < endIndex; ++i)
					alphaBlend(line[i], m_colour);
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

		Coverage getCoverage(float x, float y)
		{
			Point relativeCorner(m_corner.x - x, m_corner.y - y);
			float verticalFraction = 1.0f - std::max(std::min(1.0f, relativeCorner.y), 0.0f) - std::max(std::min(1.0f, 1.0f - relativeCorner.y - m_height), 0.0f);
			float horizontalFraction = 1.0f - std::max(std::min(1.0f, relativeCorner.x), 0.0f) - std::max(std::min(1.0f, 1.0f - relativeCorner.x - m_width), 0.0f);

			uint8_t coverageShapeX = 0;
			if (0.0f >= relativeCorner.x && 0.0f <= relativeCorner.x + m_width)
				coverageShapeX |= 0x09;
			if (1.0f >= relativeCorner.x && 1.0f <= relativeCorner.x + m_width)
				coverageShapeX |= 0x06;

			uint8_t internalPointMask = 0x10;
			for (const auto& p : coveragePoints)
			{
				if (p.x > relativeCorner.x && p.x < relativeCorner.x + m_width)
					coverageShapeX |= internalPointMask;
				internalPointMask <<= 1;
			}

			uint8_t coverageShapeY = 0;
			if (0.0f >= relativeCorner.y && 0.0f <= relativeCorner.y + m_height)
				coverageShapeY |= 0x03;
			if (1.0f >= relativeCorner.y && 1.0f <= relativeCorner.y + m_height)
				coverageShapeY |= 0x0c;

			internalPointMask = 0x10;
			for (const auto& p : coveragePoints)
			{
				if (p.y > relativeCorner.y && p.y < relativeCorner.y + m_height)
					coverageShapeY |= internalPointMask;
				internalPointMask <<= 1;
			}

			return Coverage(coverageShapeX & coverageShapeY, verticalFraction * horizontalFraction, m_colour);
		}


		
	private:
		void TransformToBasis(Point &point)
		{
			point.x -= m_corner.x;
			point.x /= m_height;
			point.y -= m_corner.y;
			point.y /= m_width;
		}

		Point m_corner;
		float m_width;
		float m_height;
		Colour m_colour;
	};
	
	template<class LENGTHTYPE, size_t SUBPIXELACCURACY>
	class BitmapCanvas
	{
	public:
		void create(size_t nColumns, size_t nRows, Colour backgroundColour, LENGTHTYPE height = LENGTHTYPE(1.0))
		{
			m_nColumns = nColumns;
			m_backgroundColour = backgroundColour;
			m_height = height;
			m_pixels.assign(nColumns * nRows, backgroundColour);
		}


		template<class SHAPEITER>
		void renderScene(SHAPEITER begin, SHAPEITER end)
		{
			size_t nRows = m_pixels.size() / m_nColumns;
			LENGTHTYPE width = m_height * double(m_nColumns) / double(nRows);

			using unitlessType = decltype(LENGTHTYPE() / LENGTHTYPE()); //deals with cases where LENGTHTYPE is some class that may or may not have dimensionality

			LENGTHTYPE pixelSize = m_height / unitlessType(nRows);

			for (SHAPEITER shape = begin; shape != end; ++shape)
				renderShape(shape, width, pixelSize);
		}
	private:
		struct PixelRenderInfo
		{
			Shape* shape;
			Coverage coverage;
			Colour colour;
		};

		template<class SHAPEITER>
		void renderShape(SHAPEITER shape, Colour colour, LENGTHTYPE width, LENGTHTYPE pixelSize)
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
		}
		template<class SHAPEITER>
		void renderShapeToPixel(PixelRenderInfo pixelRenderInfo, size_t pixelIndex, LENGTHTYPE pixelSize)
		{
			using unitlessType = decltype(LENGTHTYPE() / LENGTHTYPE()); //deals with cases where LENGTHTYPE is some class that may or may not have dimensionality

			using boundingBoxType = decltype(pixelRenderInfo.shape->getBoundingBox());

			size_t row = pixelRenderInfo.pixelIndex / m_nColumns;
			size_t column = pixelRenderInfo.pixelIndex % m_nColumns;
			//create a box the size of a pixel
			boundingBoxType pixelBox;
			pixelBox.xMin = unitlessType(column) * pixelSize;
			pixelBox.xMax = unitlessType(column + 1) * pixelSize;
			pixelBox.yMin = unitlessType(row) * pixelSize;
			pixelBox.yMax = unitlessType(row + 1) * pixelSize;

			if constexpr (SUBPIXELACCURACY == 0)
				pixelRenderInfo.coverage = pixelRenderInfo.shape->getMoreThanHalfCovered(pixelBox) ? Coverage::full : Coverage::none;
			else
				pixelRenderInfo.coverage = pixelRenderInfo.shape->getCoverage(pixelBox);

			//do nothing if the shape doesn't cover the pixel
			if (pixelRenderInfo.coverage == Coverage::none)
				return;

			//if this is a solid cover, set the pixel colour and remove any delayed rendering
			if (pixelRenderInfo.coverage == Coverage::full && pixelRenderInfo.colour.a == 0.0f)
			{
				m_pixels[pixelIndex] = pixelRenderInfo.colour;
				m_delayedPixelRenderInfo.erase(pixelIndex); //deletes if it exists
				return;
			}

			auto thisDelayedPixelRenderInfo = m_delayedPixelRenderInfo.find();

			//if this is a transparent colour and there is no lower delayed rendering, blend the colour
			if (pixelRenderInfo.coverage == Coverage::full && thisDelayedPixelRenderInfo != m_delayedPixelRenderInfo.end())
			{
				m_pixels[pixelIndex] = blend(m_pixels[pixelIndex], pixelRenderInfo.colour);
				return;
			}

			//If all the other options failed, we have to delay the rendering and render smaller boxes later
			m_delayedPixelRenderInfo[pixelIndex].push_back(pixelRenderInfo);
			

		}
		template<class SHAPEITER>
		void renderShapeToPixel()
		{

		}
		std::vector<Colour> m_pixels;
		size_t m_nColumns;
		Colour m_backgroundColour;
		LENGTHTYPE m_height;
		std::map < size_t, std::vector<PixelRenderInfo>> m_delayedPixelRenderInfo;
	};
}

#endif
