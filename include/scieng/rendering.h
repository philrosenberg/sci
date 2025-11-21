#ifndef SCI_RENDERING_H
#define SCI_RENDERING_H

#include<vector>
#include<array>
#include<cmath>
#include<map>
namespace sci
{
	template<class LENGTHTYPE>
	struct Box
	{
		using lengthtype = LENGTHTYPE;
		LENGTHTYPE xMin;
		LENGTHTYPE xMax;
		LENGTHTYPE yMin;
		LENGTHTYPE yMax;
	};

	template<class LENGTHTYPE>
	struct Point
	{
		using lengthtype = LENGTHTYPE;
		LENGTHTYPE x;
		LENGTHTYPE y;
	};

	enum class Coverage
	{
		full,
		none,
		partialSingleHorizontalIntersection,
		partialSingleVerticalIntersection,
		partialComplex
	};

	template<class LENGTHTYPE>
	class Shape
	{
	public:
		using lengthtype = LENGTHTYPE;
		virtual Box<LENGTHTYPE> getBoundingBox(Point<LENGTHTYPE> point) const =0;
		virtual Coverage getCoverage(Box<LENGTHTYPE> box) const = 0;
		virtual bool getMoreThanHalfCovered(Box<LENGTHTYPE> box) const = 0;
	};

	template<class LENGTHTYPE = double>
	class Rectangle : public Shape<LENGTHTYPE>
	{
	public:
		Box<LENGTHTYPE> getBoundingBox(Point<LENGTHTYPE> point) const final
		{
			Box<LENGTHTYPE> result;
			result.xMin = point.x - m_width / 2.0;
			result.xMax = point.x + m_width / 2.0;
			result.yMin = point.x - m_height / 2.0;
			result.yMax = point.x + m_height / 2.0;
			return result;
		}
		
		Coverage getCoverage(Box<LENGTHTYPE> box) const final
		{

		}
	private:
		LENGTHTYPE m_width;
		LENGTHTYPE m_height;
	};

	struct Colour
	{
		float r;
		float g;
		float b;
		float a;
	};

	inline Colour blend(Colour lower, Colour upper)
	{

	}
	
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
			Shape<LENGTHTYPE>* shape;
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
				renderShapeToPixelDelayed();
			}
		}
		template<class SHAPEITER>
		void renderShapeToPixel(PixelRenderInfo pixelRenderInfo, size_t pixelIndex, LENGTHTYPE pixelSize)
		{
			using unitlessType = decltype(LENGTHTYPE() / LENGTHTYPE()); //deals with cases where LENGTHTYPE is some class that may or may not have dimensionality

			using boundingBoxType = decltype(pixelRenderInfo.shape->getBoundingBox());

			size_t row = pixelRenderInfo.pixelIndex / m_nColumns;
			size_t column = pixelRenderInfo.pixelIndex % m_nColumns;
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
