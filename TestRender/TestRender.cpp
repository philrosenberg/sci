#include"../include/scieng/rendering.h"
#include<assert.h>

int main()
{
    // Simple sanity checks
    // Horizontal line y = 0.5  ->  half below has area 0.5
    {
        float a = 0.0f, b = 1.0f, c = -0.5f;
        auto f = sci::clipPixelArbitraryLine(0.0f, 0.0f, a, b, c, sci::Colour());
        assert(std::abs(f.fraction - 0.5) < 1e-6);
        assert(f.shape == (sci::coverageShapeBottomLeft | sci::coverageShapeBottomRight | sci::coverageShapePoints[2] | sci::coverageShapePoints[3]));
    }
    // Diagonal line x + y - 1 = 0 -> area of bottom right triangle with x+y >= 1 is 1/2
    {
        float a = 1.0f, b = 1.0f, c = -1.0f;
        auto f = sci::clipPixelArbitraryLine(0.0f, 0.0f, a, b, c, sci::Colour());
        assert(std::abs(f.fraction - 0.5) < 1e-6);
        //top right and bottom left are on the line and should be omitted as we are clipping left/top
        assert(f.shape == (sci::coverageShapeBottomRight | sci::coverageShapePoints[2] | sci::coverageShapePoints[3]));
    }
    // right half x >= 0.3  -> area 0.7
    {
        float a = 1.0f, b = 0.0f, c = -0.3f;
        auto f = sci::clipPixelArbitraryLine(0.0f, 0.0f, a, b, c, sci::Colour());
        assert(std::abs(f.fraction - 0.7) < 1e-6);
        assert(f.shape == (sci::coverageShapeTopRight | sci::coverageShapeBottomRight | sci::coverageShapePoints[1] | sci::coverageShapePoints[2] | sci::coverageShapePoints[3]));
    }
    // Entire plane on negative side
    {
        float a = 0.0f, b = 0.0f, c = -1.0f;
        auto f = sci::clipPixelArbitraryLine(0.0f, 0.0f, a, b, c, sci::Colour());
        assert(std::abs(f.fraction -0.0) < 1e-6);
        assert(f.shape == sci::coverageShapeNone);
    }
    // Entire plane on positive side?
    {
        float a = 0.0f, b = 0.0f, c = 1.0f;
        auto f = sci::clipPixelArbitraryLine(0.0f, 0.0f, a, b, c, sci::Colour());
        assert(std::abs(f.fraction - 1.0) < 1e-6);
        assert(f.shape == sci::coverageShapeAll);
    }

    //create a canvas showing two rectangles exactly butted against each other
    //vertically and aligned on their left edge. The upper one is smaller
    //There should be no seam between then and the left edge should be continuous with no defects
    sci::BitmapCanvas canvas;
    canvas.create(500, 500, sci::ColourOpaque{ 1.0, 1.0, 1.0 });
    sci::Rectangle rectangle1(sci::Point(125.5, 125.5), 250.0f, 250.0f, sci::Colour(1.0f, 0.0f, 0.5f));
    sci::Rectangle rectangle2(sci::Point(125.5, 115.5), 125.0f, 10.0f, sci::Colour(1.0f, 0.0f, 0.5f));
    sci::HorizontalBaseTriangle triangle(sci::Point(313.0f, 115.5f), 125.5f, 250.5f, 375.5f, sci::Colour(1.0f, 0.0f, 0.5f));
    canvas.addRectangle(rectangle1);
    canvas.addRectangle(rectangle2);
    canvas.addHorizontalBaseTriangle(triangle);
    canvas.renderScene();
    canvas.writePpm(sU("test.ppm"));

    float baseLength = 50.0f;
    float shallowHeight = 10.0f;
    float steepHeight = 40.0f;
    float lateralOffset = 30.0f;
    sci::BitmapCanvas triangleCanvas;
    triangleCanvas.create(200, 300, sci::ColourOpaque{ 1.0, 1.0, 1.0 });

    float direction = -1.0f;
    float y = 50.0f;
    float centre = 35.0;
    for (size_t i = 0; i < 2; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            float thisCentre = centre + float(j) * 65.0f;
            float left = thisCentre - baseLength / 2.0f;
            float right = left + baseLength;
            sci::Point point((float(j) - 1.0f) * lateralOffset + thisCentre, y + direction * shallowHeight);
            sci::HorizontalBaseTriangle triangle(point, y, left, right, sci::Colour(1.0f, 0.0f, 0.5f));
            triangleCanvas.addHorizontalBaseTriangle(triangle);
        }
        direction *= -1.0f;
        y += 10.0f;
    }

    direction = -1.0f;
    y = 130.0f;
    centre = 35.0;
    for (size_t i = 0; i < 2; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            float thisCentre = centre + float(j) * 65.0f;
            float left = thisCentre - baseLength / 2.0f;
            float right = left + baseLength;
            sci::Point point((float(j) - 1.0f) * lateralOffset + thisCentre, y + direction * steepHeight);
            sci::HorizontalBaseTriangle triangle(point, y, left, right, sci::Colour(1.0f, 0.0f, 0.5f));
            triangleCanvas.addHorizontalBaseTriangle(triangle);
        }
        direction *= -1.0f;
        y += 10.0f;
    }

    triangleCanvas.addTriangle(sci::Triangle(sci::Point(20.5f, 200.0f),
        sci::Point(90.0f, 280.0f),
        sci::Point(170.1f, 250.0f),
        sci::Colour(1.0f, 0.0f, 0.5f)));

    triangleCanvas.renderScene();
    triangleCanvas.writePpm(sU("triangle.ppm"));


    for (size_t i = 0; i < 4; ++i)
    {
        std::vector<sci::ColourOpaque> line(5, sci::ColourOpaque{ 1.0f, 1.0f, 1.0f });
        std::vector<bool> partials(5, false);
        sci::Rectangle rect1(sci::Point{ 0.5f, 0.7f }, 2.0f, 2.0f, sci::Colour(0.0, 0.0, 0.0, 1.0));
        sci::Rectangle rect2(sci::Point{ 0.5f, 2.7f }, 2.0f, 2.0f, sci::Colour(0.0, 0.0, 0.0, 1.0));
        rect1.renderScanLine(line, float(i), partials);
        rect2.renderScanLine(line, float(i), partials);
        float xf = 0.0f;
        size_t xi = 0;
        for (const auto& p : partials)
        {
            sci::Coverage coverage;
            if (p)
            {
                sci::Coverage coverage1 = rect1.getCoverage(xf, float(i));
                sci::Coverage coverage2 = rect2.getCoverage(xf, float(i));
                sci::alphaBlend(line[xi], coverage1, coverage2);
            }
            xf += 1.0f;
            xi++;
        }
    }
}

