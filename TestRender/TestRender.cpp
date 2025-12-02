#include"../include/scieng/rendering.h"

int main()
{
    //create a canvas showing tow rectangles exactly butted against each other
    //vertically and aligned on their left edge. The upper one is smaller
    //There should be no seam between then and the left edge should be continuous with no defects
    sci::BitmapCanvas canvas;
    canvas.create(500, 500, sci::ColourOpaque{ 1.0, 1.0, 1.0 });
    sci::Rectangle rectangle1(sci::Point(125.5, 125.5), 250, 250, sci::Colour(1.0f, 0.0f, 0.5f));
    sci::Rectangle rectangle2(sci::Point(125.5, 115.5), 125, 10, sci::Colour(1.0f, 0.0f, 0.5f));
    canvas.addRectangle(rectangle1);
    canvas.addRectangle(rectangle2);
    canvas.renderScene();
    canvas.writePpm(sU("test.ppm"));


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

