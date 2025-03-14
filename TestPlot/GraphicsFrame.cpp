#include "GraphicsFrame.h"
#include<svector/graphics.h>

void GraphicsPanel::OnPaint(wxPaintEvent& event)
{
	//override this function, but this is the kind of thing to do
	wxPaintDC dc(this);
	wxRenderer renderer(&dc, GetClientSize(), grPerInch(FromDIP(96)));
}