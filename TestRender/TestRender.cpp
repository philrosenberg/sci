#include"../include/scieng/rendering.h"

int main()
{
    sci::BitmapCanvas<double, 0> canvas;
    canvas.create(500, 500, sci::Colour{ 1.0, 1.0, 1.0, 0.0 }, 1.0);

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

