#include "GraphicsFrame.h"
#include<svector/graphics.h>


/*BEGIN_EVENT_TABLE(GraphicsPanel, wxPanel)
EVT_PAINT(GraphicsPanel::OnPaint)
EVT_SIZE(GraphicsPanel::OnResize)
END_EVENT_TABLE()*/

void GraphicsPanel::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	wxRenderer renderer(&dc, GetClientSize(), grPerInch(FromDIP(96)));

	splotaxis axis1(0, 1, false, PlotScale::Direction::horizontal, 0.05, 0.45, 0.1);
	axis1.setFixedScale(0, 10);
	axis1.draw(renderer, grPerInch(96));

	splotaxis axis2(0, 1, false, PlotScale::Direction::horizontal, 0.05, 0.45, 0.2);
	axis2.setFixedScale(0, 20);
	axis2.setcolour(rgbcolour(1.0, 0.3, 0.0));
	axis2.settitlecolour(rgbcolour(0.0, 0.0, 0.5));
	axis2.setlabelcolour(rgbcolour(0.5, 0.5, 0.5));
	axis2.settitlefont(sU("Algerian"));
	axis2.setlinethickness(4);
	axis2.draw(renderer, grPerInch(96));

	splotaxis axis3(0, 1, false, PlotScale::Direction::horizontal, 0.05, 0.45, 0.3);
	axis3.setFixedScale(0, 50);
	axis3.draw(renderer, grPerInch(96));

	splotaxis axis4(0, 1, false, PlotScale::Direction::horizontal, 0.05, 0.45, 0.4);
	axis4.setFixedScale(0, 80);
	axis4.draw(renderer, grPerInch(96));


	splotaxis axis5(0, 1, false, PlotScale::Direction::vertical, 0.05, 0.45, 0.6);
	axis5.setFixedScale(0, 10);
	axis5.draw(renderer, grPerInch(96));

	splotaxis axis6(0, 1, false, PlotScale::Direction::vertical, 0.05, 0.45, 0.7);
	axis6.setFixedScale(0, 20);
	axis6.draw(renderer, grPerInch(96));

	splotaxis axis7(0, 1, false, PlotScale::Direction::vertical, 0.05, 0.45, 0.8);
	axis7.setFixedScale(0, 50);
	axis7.draw(renderer, grPerInch(96));

	splotaxis axis8(0, 1, false, PlotScale::Direction::vertical, 0.05, 0.45, 0.9);
	axis8.setFixedScale(0, 80);
	axis8.draw(renderer, grPerInch(96));


	splotaxis axis9(0, 1, true, PlotScale::Direction::horizontal, 0.05, 0.45, 0.5);
	axis9.setFixedScale(1, 10);
	axis9.draw(renderer, grPerInch(96));

	splotaxis axis10(0, 1, true, PlotScale::Direction::horizontal, 0.05, 0.45, 0.6);
	axis10.setFixedScale(1, 1e20);
	axis10.draw(renderer, grPerInch(96));

	splotaxis axis11(0, 1, true, PlotScale::Direction::horizontal, 0.05, 0.45, 0.7);
	axis11.setFixedScale(1, 200);
	axis11.draw(renderer, grPerInch(96));

	splotaxis axis12(0, 1, true, PlotScale::Direction::horizontal, 0.05, 0.45, 0.8);
	axis12.setFixedScale(1, 1e20);
	axis12.setmajorinterval(5);
	axis12.setnsubticks(4);
	axis12.draw(renderer, grPerInch(96));
}