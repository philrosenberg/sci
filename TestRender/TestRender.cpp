#include"../include/svector/rendering.h"

int main()
{
    sci::BitmapCanvas<double, 0> canvas;
    canvas.create(500, 500, sci::Colour{ 1.0, 1.0, 1.0, 0.0 }, 1.0);
}

