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

	Colour toLinear(const Colour& colour)
	{
		return Colour(std::pow(colour.r, 2.2f), std::pow(colour.g, 2.2f), std::pow(colour.b, 2.2f), colour.a);
	}

	ColourOpaque toLinear(const ColourOpaque& colour)
	{
		return ColourOpaque(std::pow(colour.r, 2.2f), std::pow(colour.g, 2.2f), std::pow(colour.b, 2.2f));
	}

	ColourOpaque toPerceptual(const ColourOpaque& colour)
	{
		return ColourOpaque(std::pow(colour.r, 1.0f/2.2f), std::pow(colour.g, 1.0f / 2.2f), std::pow(colour.b, 1.0f / 2.2f));
	}

	inline void overlay(ColourOpaque& base, const Colour& top)
	{
		base.r = base.r * (1.0f - top.a) + top.r * top.a;
		base.g = base.g * (1.0f - top.a) + top.g * top.a;
		base.b = base.b * (1.0f - top.a) + top.b * top.a;
	}

	inline Colour overlay(Colour base, Colour top)
	{
		float tTop = 1.0f - top.a;
		return Colour(top.r * top.a + base.r * base.a * tTop,
			top.g * top.a + base.g * base.a * tTop,
			top.b * top.a + base.b * base.a * tTop,
			top.a + base.a * tTop);
	}


	const std::array<Point, 4> coveragePoints{ Point { 0.3f, 0.4f },
		Point { 0.6f, 0.3f },
		Point { 0.7f, 0.6f },
		Point { 0.4f, 0.7f } };

	const uint8_t coverageShapeTopLeft = 0x01;
	const uint8_t coverageShapeTopRight = 0x02;
	const uint8_t coverageShapeBottomRight = 0x04;
	const uint8_t coverageShapeBottomLeft = 0x08;
	const std::array<uint8_t, 4>coverageShapePoints{0x10, 0x20, 0x40, 0x80};
	const uint8_t coverageShapeAll = 0xff;
	const uint8_t coverageShapeNone = 0x00;

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

	inline Coverage clipPixelLeft(float xMinusColumn, const Colour& colour)
	{
		if (xMinusColumn <= 0.0f)
			return Coverage(coverageShapeAll, 1.0, colour);
		if (xMinusColumn >= 1.0f)
			return Coverage(coverageShapeNone, 0.0, colour);

		uint8_t shape = 0x06;
		for (int i = 0; i < 4; ++i)
			shape |= coverageShapePoints[i] * (uint8_t)(coveragePoints[i].x > xMinusColumn); //utilise casting bool to in gives 0 or 1
		return Coverage(shape, 1.0f - xMinusColumn, colour);
	}

	inline Coverage clipPixelLeft(float column, float x, const Colour &colour)
	{
		return clipPixelLeft(x - column, colour);
	}

	inline Coverage clipPixelRight(float xMinusColumn, const Colour& colour)
	{
		if (xMinusColumn <= 0.0f)
			return Coverage(coverageShapeNone, 0.0, colour);
		if (xMinusColumn >= 1.0f)
			return Coverage(coverageShapeAll, 1.0, colour);

		uint8_t shape = 0x09;
		for (int i = 0; i < 4; ++i)
			shape |= coverageShapePoints[i] * (uint8_t)(coveragePoints[i].x <= xMinusColumn); //utilise casting bool to in gives 0 or 1
		return Coverage(shape, xMinusColumn, colour);
	}

	inline Coverage clipPixelRight(float column, float x, const Colour& colour)
	{
		return clipPixelRight(x - column, colour);
	}

	inline Coverage clipPixelTop(float yMinusRow, const Colour& colour)
	{
		if (yMinusRow <= 0.0f)
			return Coverage(coverageShapeAll, 1.0, colour);
		if (yMinusRow >= 1.0f)
			return Coverage(coverageShapeNone, 0.0, colour);

		uint8_t shape = 0x0c;
		for (int i = 0; i < 4; ++i)
			shape |= coverageShapePoints[i] * (uint8_t)(coveragePoints[i].y > yMinusRow); //utilise casting bool to in gives 0 or 1
		return Coverage(shape, 1.0f - yMinusRow, colour);
	}

	inline Coverage clipPixelTop(float row, float y, const Colour& colour)
	{
		return clipPixelTop(y - row, colour);
	}

	inline Coverage clipPixelBottom(float yMinusRow, const Colour& colour)
	{
		if (yMinusRow <= 0.0f)
			return Coverage(coverageShapeNone, 0.0, colour);
		if (yMinusRow >= 1.0f)
			return Coverage(coverageShapeAll, 1.0, colour);

		uint8_t shape = 0x03;
		for (int i = 0; i < 4; ++i)
			shape |= coverageShapePoints[i] * (uint8_t)(coveragePoints[i].y <= yMinusRow); //utilise casting bool to in gives 0 or 1
		return Coverage(shape, yMinusRow, colour);
	}

	inline Coverage clipPixelBottom(float row, float y, const Colour& colour)
	{
		return clipPixelBottom(y - row, colour);
	}

	// Compute area fraction of unit square {0<=x<=1,0<=y<=1} on side a*x + b*y + c <= 0
	// Returns a value in [0,1].
	inline Coverage clipPixelArbitraryLine(float row, float column, float a, float b, float c, const Colour& colour)
	{
		//set xs and ys as the corners of a unit square, counter clockwise order
		const std::array<float, 4> xs = { 0.0f, 1.0f, 1.0f, 0.0f };
		const std::array<float, 4> ys = { 0.0f, 0.0f, 1.0f, 1.0f };

		//calculate mx + c - y for each point. It is zero on the line
		//negative on one side and positive on the other
		std::array<float, 4> f;
		for (int i = 0; i < 4; ++i)
		{
			f[i] = a * (xs[i] + column) + b * (ys[i] + row) + c;
		}

		// 3) For each edge i -> j = (i+1)&3 compute intersection param t (if any)
		//    We compute all t's and intersection points regardless; later we use masks
		std::array<float, 4> t;				//fraction of distance from point i to i+1 where cross occurs t = -f[i]/(f[j]-f[i]) = f[i]/(f[i]-f[j])
		std::array<float, 4> xIntersection;	// intersection x
		std::array<float, 4> yIntersection;	// intersection y
		std::array<int, 4> crosses;				// 1 if edge crosses (endpoints on different sides), else 0
		for (int i = 0; i < 4; ++i)
		{
			int j = (i + 1) & 3;
			float denominator = (f[i] - f[j]); // f0 - f1
			// To avoid 0/0, handle den == 0 in a defined numeric way:
			// If den == 0 we set t = 0 (will be ignored because cross==0).
			// (This is safe: if both endpoints are exactly on the line, we treat as non-crossing.)
			if (denominator == 0.0)
			{
				t[i] = 0.0;
				crosses[i] = 0;
				xIntersection[i] = 0.0;
				yIntersection[i] = 0.0;
			}
			else
			{
				t[i] = f[i] / denominator;
				// intersection point P = P0 + t*(P1 - P0)
				xIntersection[i] = xs[i] + t[i] * (xs[j] - xs[i]);
				yIntersection[i] = ys[i] + t[i] * (ys[j] - ys[i]);
				// cross if endpoints are on different sides (<=0 vs >0)
				crosses[i] = ((f[i] <= 0.0) != (f[j] <= 0.0)) ? 1 : 0;
			}
		}

		//merge into one array, keeping the counter clockwise order
		std::array<float, 8> xPolygon;
		std::array<float, 8> yPolyGon;
		int n = 0;
		// We must maintain order: for each square vertex i, if it's kept, push it.
		// Also if the outgoing edge i->i+1 produced an intersection that we should include in between.
		for (int i = 0; i < 4; ++i)
		{
			// vertex i
			if (f[i] >= 0.0f)
			{
				xPolygon[n] = xs[i];
				yPolyGon[n] = ys[i];
				++n;
			}
			// intersection on edge i->i+1 (stored at cx[4+i]) comes after vertex i if it exists
			if (crosses[i])
			{
				xPolygon[n] = xIntersection[i];
				yPolyGon[n] = yIntersection[i];
				++n;
			}
		}

		// Edge case: if the clipping half-plane contains all vertices (n == 4 and no intersections),
		// or none (n == 0)
		if (n == 0)
			return Coverage(0x00, 0.0, colour);   // no area on <=0 side
		if (n == 4 && f[0] >= 0.0 && f[1] >= 0.0 && f[2] >= 0.0 && f[3] >= 0.0)
			return Coverage(0xff, 1.0, colour);

		// 6) Shoelace formula for polygon area
		float sum = 0.0f;
		for (int i = 0; i < n; ++i)
		{
			int j = (i + 1) % n;
			sum += xPolygon[i] * yPolyGon[j] - xPolygon[j] * yPolyGon[i];
		}
		float area = 0.5f * std::abs(sum);
		// clamp numeric noise
		if (area <= 0.0)
			return Coverage(coverageShapeNone, 0.0, colour);
		if (area >= 1.0)
			return Coverage(coverageShapeAll, 1.0, colour);

		float shapeGradient = std::abs(a) > std::abs(b) ? a : b;
		uint8_t shape = 0x00;
		for (size_t i = 0; i < 4; ++i)
		{
			bool in = f[i] > 0.0f || (f[i] == 0.0f && shapeGradient < 0.0f);
			shape |= (uint8_t(in) << i);
		}
		for (size_t i = 0; i < 4; ++i)
		{
			float f = a * coveragePoints[i].x + b * coveragePoints[i].y + c;
			bool in = f > 0.0f || (f == 0.0 && shapeGradient < 0.0f);
			shape |= (uint8_t(in) *coverageShapePoints[i]);
		}
		return Coverage(shape, area, colour);
	}

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

		overlapCoverage.shape = (coverageBottom.shape & coverageTop.shape);
		overlapCoverage.fraction = std::min(coverageBottom.fraction, coverageTop.fraction) * (float(countBits(overlapCoverage.shape)) / float(minCoveredPoints));
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

	class Shape
	{
	public:
		virtual Box getBoundingBox() const = 0;
		virtual void renderScanLine(std::span<ColourOpaque> line, float lineIndex, std::vector<bool>& partials) = 0;
		virtual Coverage getCoverage(float row, float column) const = 0;
		//virtual bool getMoreThanHalfCovered(Box box) const = 0;
	};


	inline long constrain(long value, long min, long max)
	{
		return std::min(std::max(value, min), max);
	}

	class Rectangle : public Shape
	{
	public:
		Rectangle(Point corner, float width, float height, Colour colour)
			:m_top(height < 0.0f ? corner.y + height : corner.y),
			m_bottom(height < 0.0f ? corner.y : corner.y + height),
			m_left(width < 0.0f ? corner.x + width : corner.x),
			m_right(width < 0.0f ? corner.x : corner.x + width),
			m_colour(toLinear(colour))
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

	

	

	class HorizontalBaseTriangle : public Shape
	{
	public:
		//the triangle has points at point, (x1, y) and (x2, y)
		HorizontalBaseTriangle(Point point, float y, float x1, float x2, Colour colour)
			:m_point(point), m_y(y), m_x1(std::min(x1, x2)), m_x2(std::max(x1, x2)), m_colour(toLinear(colour))
		{
			m_mInv1 = (point.x - m_x1) / (point.y - y);
			m_mInv2 = (point.x - m_x2) / (point.y - y);
			m_m1 = 1.0f / m_mInv1;
			m_m2 = 1.0f / m_mInv2;
			m_c1 = point.y - point.x / m_mInv1;
			m_c2 = point.y - point.x / m_mInv2;
		}

		Box getBoundingBox() const final
		{
			Box result;
			result.tl = Point(std::min(m_point.x, m_x1), std::min(m_point.y, m_y));
			result.br = Point(std::max(m_point.x, m_x2), std::max(m_point.y, m_y));
			return result;
		}


		void renderScanLine(std::span<ColourOpaque> line, float lineIndex, std::vector<bool>& partials)
		{
			float top = std::min(std::max(std::min(m_point.y, m_y), lineIndex), lineIndex + 1.0f);
			float bottom = std::max(std::min(std::max(m_point.y, m_y), lineIndex + 1.0f), lineIndex);
			if (top >= bottom)
				return;
			float topLeft = (top - m_c1) * m_mInv1;
			float topRight = (top - m_c2) * m_mInv2;
			float bottomLeft = (bottom - m_c1) * m_mInv1;
			float bottomRight = (bottom - m_c2) * m_mInv2;

			long start = constrain(long(std::min(topLeft, bottomLeft)), 0l, (long)line.size());
			long end = constrain(long(std::max(topRight, bottomRight)) + 1l, 0l, (long)line.size());
			if (top != lineIndex || bottom != lineIndex + 1.0f)
			{
				for (size_t i = start; i < end; ++i)
					partials[i] = true;
				return;
			}

			long startFullyCovered = constrain(long(std::max(topLeft, bottomLeft)) + 1l, 0l, (long)line.size());
			long endFullyCovered = constrain(long(std::min(topRight, bottomRight)), 0l, (long)line.size());

			for (size_t i = startFullyCovered; i < endFullyCovered; ++i)
				overlay(line[i], m_colour);
			for (size_t i = start; i < startFullyCovered; ++i)
				partials[i] = true;
			for (size_t i = endFullyCovered; i < end; ++i)
				partials[i] = true;
		}

		Coverage getCoverage(float row, float column) const
		{
			if (row > m_y && row > m_point.y)
				return Coverage(coverageShapeNone, 0.0, m_colour);
			if (row + 1.0f < m_y && row + 1.0f < m_point.y)
				return Coverage(coverageShapeNone, 0.0, m_colour);;
			float top = std::min(std::max(std::min(m_point.y, m_y), row), row + 1.0f);
			float bottom = std::max(std::min(std::max(m_point.y, m_y), row + 1.0f), row);

			float topLeft = (top - m_c1) * m_mInv1;
			float bottomLeft = (bottom - m_c1) * m_mInv1;
			float midLeft = (topLeft + bottomLeft) * 0.5f;
			float topRight = (top - m_c2) * m_mInv2;
			float bottomRight = (bottom - m_c2) * m_mInv2;

			bool fullLeft = column > std::max(topLeft, bottomLeft);
			bool fullRight = column + 1.0f < std::min(topRight, bottomRight);
			bool fullBase = (row - m_y) * (row + 1.0f - m_y) >= 0.0f;
			if (fullLeft && fullRight &&fullBase)
				return Coverage(0xff, 1.0f, m_colour);

			Coverage leftCoverage(0xff, 1.0f, m_colour);
			Coverage rightCoverage(0xff, 1.0f, m_colour);
			Coverage baseCoverage(0xff, 1.0f, m_colour);

			if (!fullLeft)
			{
				float sign = (m_m1 < 1.0f ? -1.0f : 1.0f);
				bool shallow = std::abs(m_m1 < 1.0f);
				float a = shallow ? -1.0f * sign : m_m1 * sign;
				float b = shallow ? m_mInv1 * sign : -1.0f * sign;
				float c = shallow ? -m_c1 * m_mInv1 * sign : m_c1 * sign;
				leftCoverage = clipPixelArbitraryLine(row, column, a, b, c, m_colour);
			}
			if (!fullRight)
			{
				float sign = (m_m2 < 1.0f ? 1.0f : -1.0f); //reversed compared to left
				bool shallow = std::abs(m_m2 < 1.0f);
				float a = shallow ? -1.0f * sign : m_m2 * sign;
				float b = shallow ? m_mInv2 * sign : -1.0f * sign;
				float c = shallow ? -m_c2 * m_mInv2 : m_c2 * sign;
				rightCoverage = clipPixelArbitraryLine(row, column, a, b, c, m_colour);
			}
			if (!fullBase)
			{
				if (m_y < m_point.y)
					baseCoverage = clipPixelTop(row, m_y, m_colour);
				else
					baseCoverage = clipPixelBottom(row, m_y, m_colour);
			}

			Coverage lrCoverage = splitOverlap(leftCoverage, rightCoverage);
			Coverage coverage = splitOverlap(baseCoverage, lrCoverage);
			coverage.colour = m_colour;
			return coverage;
		}



	private:
		Point m_point;
		float m_y;
		float m_x1;
		float m_x2;
		//store line data as 0=ax+by+c
		float m_m1;
		float m_m2;
		float m_mInv1;
		float m_mInv2;
		float m_c1;
		float m_c2;
		Colour m_colour;
	};

	class Triangle : public Shape
	{
	public:
		Triangle(Point corner1, Point corner2, Point corner3, Colour colour)
			:m_triangle1(makeTriangle1({ corner1, corner2, corner3 }, colour)), m_triangle2(makeTriangle2({ corner1, corner2, corner3 }, colour))
		{
		}

		Box getBoundingBox() const final
		{
			Box box1 = m_triangle1.getBoundingBox();
			Box box2 = m_triangle2.getBoundingBox();
			Box result;
			result.tl = Point(std::min(box1.tl.x, box2.tl.x), std::min(box1.tl.y, box2.tl.y));
			result.br = Point(std::max(box1.tl.x, box2.tl.x), std::max(box1.tl.y, box2.tl.y));
			return result;
		}

		void renderScanLine(std::span<ColourOpaque> line, float lineIndex, std::vector<bool>& partials)
		{
			m_triangle1.renderScanLine(line, lineIndex, partials);
			m_triangle2.renderScanLine(line, lineIndex, partials);
		}

		Coverage getCoverage(float row, float column) const
		{
			Coverage coverage1 = m_triangle1.getCoverage(row, column);
			Coverage coverage2 = m_triangle2.getCoverage(row, column);

			return Coverage(coverage1.shape | coverage2.shape, coverage1.fraction + coverage2.fraction, coverage1.colour);
		}

	private:
		static HorizontalBaseTriangle makeTriangle1 (std::array<Point, 3> points, Colour colour)
		{
			if (points[0].y < points[1].y)
				std::swap(points[0], points[1]);
			if (points[1].y < points[2].y)
				std::swap(points[1], points[2]);
			if (points[0].y < points[1].y)
				std::swap(points[0], points[1]);

			if(points[0].y == points[2].y)
				return HorizontalBaseTriangle(points[0], points[1].y, points[1].x, points[1].x, colour);

			float xOther = points[0].x + (points[1].y - points[0].y) * (points[2].x - points[0].x) / (points[2].y - points[0].y);
			return HorizontalBaseTriangle(points[0], points[1].y, xOther, points[1].x, colour);
		}
		static HorizontalBaseTriangle makeTriangle2(std::array<Point, 3> points, Colour colour)
		{
			if (points[0].y < points[1].y)
				std::swap(points[0], points[1]);
			if (points[1].y < points[2].y)
				std::swap(points[1], points[2]);
			if (points[0].y < points[1].y)
				std::swap(points[0], points[1]);

			if (points[0].y == points[2].y)
				return HorizontalBaseTriangle(points[0], points[1].y, points[1].x, points[1].x, colour);

			float xOther = points[0].x + (points[1].y - points[0].y) * (points[2].x - points[0].x) / (points[2].y - points[0].y);
			return HorizontalBaseTriangle(points[2], points[1].y, xOther, points[1].x, colour);
		}
		HorizontalBaseTriangle m_triangle1;
		HorizontalBaseTriangle m_triangle2;
	};
	
	class BitmapCanvas
	{
	public:
		void create(size_t nColumns, size_t nRows, ColourOpaque backgroundColour)
		{
			m_nColumns = nColumns;
			m_backgroundColour = toLinear(backgroundColour);
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
				ColourOpaque perceptual = toPerceptual(c);
				std::array<uint8_t, 3> colours;
				colours[0] = colourFloatTo255(perceptual.r);
				colours[1] = colourFloatTo255(perceptual.g);
				colours[2] = colourFloatTo255(perceptual.b);
				fout.write((char*)(&colours[0]), 3);
			}
			return true;
		}
		void addRectangle(const Rectangle &rectangle)
		{
			//m_shapes.push_back(std::move(std::unique_ptr<Shape>(new Rectangle(rectangle))));
			m_shapes.push_back(std::move(std::unique_ptr<Shape>(new Rectangle(rectangle))));
		}
		void addHorizontalBaseTriangle(const HorizontalBaseTriangle& triangle)
		{
			m_shapes.emplace_back(std::unique_ptr<Shape>(new HorizontalBaseTriangle(triangle)));
		}
		void addTriangle(const Triangle& triangle)
		{
			m_shapes.emplace_back(std::unique_ptr<Shape>(new Triangle(triangle)));
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
