#include "mainFrame.h"
#include<svector/svector.h>
#include<svector/splot.h>
#include<svector/sreadwrite.h>
#include<svector/sstring.h>
#include<wx/scrolwin.h>

const int mainFrame::ID_FILE_EXIT = ::wxNewId();
const int mainFrame::ID_FILE_RUN_PLOT_TESTS = ::wxNewId();
const int mainFrame::ID_FILE_RUN_GRAPHICS_TESTS = ::wxNewId();
const int mainFrame::ID_HELP_ABOUT = ::wxNewId();

BEGIN_EVENT_TABLE(mainFrame, wxFrame)
EVT_MENU(ID_FILE_EXIT, mainFrame::OnExit)
EVT_MENU(ID_FILE_RUN_PLOT_TESTS, mainFrame::OnRunPlotTests)
EVT_MENU(ID_FILE_RUN_GRAPHICS_TESTS, mainFrame::OnRunGraphicsTests)
EVT_MENU(ID_HELP_ABOUT, mainFrame::OnAbout)
END_EVENT_TABLE()

mainFrame::mainFrame(wxFrame *frame, const wxString& title)
	: wxFrame(frame, -1, title)
{
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu(wxT(""));
	fileMenu->Append(ID_FILE_EXIT, wxT("E&xit\tAlt+F4"), wxT("Exit the application"));
	fileMenu->Append(ID_FILE_RUN_PLOT_TESTS, wxT("Run Plot Tests\tCtrl+P"), wxT("Run Code"));
	fileMenu->Append(ID_FILE_RUN_GRAPHICS_TESTS, wxT("Run Graphics Tests\tCtrl+G"), wxT("Run Code"));
	mbar->Append(fileMenu, wxT("&File"));

	wxMenu* helpMenu = new wxMenu(wxT(""));
	helpMenu->Append(ID_HELP_ABOUT, wxT("&About\tF1"), wxT("Show info about this application"));
	mbar->Append(helpMenu, wxT("&Help"));

	SetMenuBar(mbar);
}

void mainFrame::OnExit(wxCommandEvent& event)
{
	Close();
}

class PlotPanel : public wxPanel
{

};

void do2dplot(wxFrame *parent, sci::string title, double scaleBegin, double scaleEnd, bool log, bool autoscale, bool fillOffscaleBottom, bool fillOffscaleTop)
{
	//create a set of plots all plotting the same z, but using either the grid or contour routines and either 1d or 2d x and y coordinates
	// we choose the function 1+1/(x^2+2y^2) as this is different in the x and y axes and outside the range 0-1, so it tests to make sure
	// we do both axes correctly and tests the weird autoscaling of plshades
	GraphicsFrame<PlotCanvasPanel> *frame = new GraphicsFrame<PlotCanvasPanel>(parent);
	frame->SetClientSize(800, 800);
	auto canvas = frame->getPanel()->getCanvas();

	std::shared_ptr<PlotFrame> box(new PlotFrame(Point(grUnitless(0.02), grUnitless(0.08)), Point(grUnitless(0.92), grUnitless(0.98)), FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(grMillimetre(1.0)), title, Length(grTextPoint(12.0)), Length(grTextPoint(30.0))));
	std::vector<grUnitless> limits{ grUnitless(0.1), grUnitless(0.1 + 0.8 / 4.0), grUnitless(0.1 + 2.0 * 0.8 / 4.0), grUnitless(0.1 + 3.0 * 0.8 / 4.0), grUnitless(0.1 + 0.8) };
	PlotAxis::Options options;
	std::shared_ptr<PlotAxis> xAxis1(new PlotAxis(0.0, 1.0, false, Point(limits[0],grUnitless(0.9)), Point(limits[1], grUnitless(0.9)),  options.setTitle(sU("x-1d y-1d"))));
	std::shared_ptr<PlotAxis> xAxis2(new PlotAxis(0.0, 1.0, false, Point(limits[1], grUnitless(0.9)), Point(limits[2], grUnitless(0.9)), options.setTitle(sU("x-2d y-1d"))));
	std::shared_ptr<PlotAxis> xAxis3(new PlotAxis(0.0, 1.0, false, Point(limits[2], grUnitless(0.9)), Point(limits[3], grUnitless(0.9)), options.setTitle(sU("x-1d y-2d"))));
	std::shared_ptr<PlotAxis> xAxis4(new PlotAxis(0.0, 1.0, false, Point(limits[3], grUnitless(0.9)), Point(limits[4], grUnitless(0.9)), options.setTitle(sU("x-2d y-2d"))));
	std::shared_ptr<PlotAxis> yAxis1(new PlotAxis(0.0, 1.0, false, Point(limits[0], grUnitless(0.9)), Point(limits[0], grUnitless(0.7)), options.setTitle(sU("Contour"))));
	std::shared_ptr<PlotAxis> yAxis2(new PlotAxis(0.0, 1.0, false, Point(limits[0], grUnitless(0.7)), Point(limits[0], grUnitless(0.5)), options.setTitle(sU("Shade"))));
	std::shared_ptr<PlotAxis> yAxis3(new PlotAxis(0.0, 1.0, false, Point(limits[0], grUnitless(0.5)), Point(limits[0], grUnitless(0.3)), options.setTitle(sU("Grid"))));
	
	size_t nx = 21;
	size_t ny = 21;

	std::vector<double> x1d(nx);
	std::vector<double> y1d(ny);
	for (size_t i = 0; i < nx; ++i)
		x1d[i] = i * 1.0 / (double)(nx - 1);
	for (size_t i = 0; i < ny; ++i)
		y1d[i] = i * 1.0 / (double)(ny - 1);

	sci::GridData<double, 2> x2d({ nx, ny });
	sci::GridData<double, 2> y2d({ nx, ny });
	for (size_t i = 0; i < nx; ++i)
	{
		for (size_t j = 0; j < ny; ++j)
			x2d[i][j] = i * 1.0 / (double)(nx - 1);
	}
	for (size_t i = 0; i < nx; ++i)
	{
		for (size_t j = 0; j < ny; ++j)
			y2d[i][j] = j * 1.0 / (double)(ny - 1);
	}



	sci::GridData<double, 2> zGrid({ nx - 1, ny - 1 });
	for (size_t i = 0; i < nx - 1; ++i)
	{
		for (size_t j = 0; j < ny - 1; ++j)
		{
			double x = (x1d[i] + x1d[i + 1]) / 2.0;
			double y = (y1d[j] + y1d[j + 1]) / 2.0;
			if (log)
				zGrid[i][j] = std::pow(10.0, y);
			else
				zGrid[i][j] = 1.0 + 1.0 / (1.0 + x * x + 2.0 * y * y);
		}
	}

	sci::GridData<double, 2> zCont({ nx, ny });
	for (size_t i = 0; i < nx; ++i)
	{
		for (size_t j = 0; j < ny; ++j)
		{
			if (log)
				zCont[i][j] = std::pow(10.0, y1d[j]);
			else
				zCont[i][j] = 1.0 + 1.0 / (1.0 + x1d[i] * x1d[i] + 2.0 * y1d[j] * y1d[j]);
		}
	}

	if (log)
	{
		scaleBegin = std::log10(scaleBegin);
		scaleEnd = std::log10(scaleEnd);
	}

	double scaleRange = scaleEnd - scaleBegin;

	size_t nColours = 10;
	std::vector<hlscolour> colours(nColours);
	std::vector<double> values(nColours + 1);
	double hueInterval = 240.0 / (nColours - 1);
	double valueInterval = scaleRange / nColours;
	for (size_t i = 0; i < nColours; ++i)
		colours[i] = hlscolour(240 - i * hueInterval, 0.5, 1.0);
	for (size_t i = 0; i < nColours + 1; ++i)
		values[i] = scaleBegin + i * valueInterval;

	if (log)
		for (auto& v : values)
			v = std::pow(10.0, v);

	std::shared_ptr<splotcolourscale> colourScaleDiscrete(new splotcolourscale(values, colours, log, autoscale, fillOffscaleBottom, fillOffscaleTop));
	LineStyle contourStyle(grMillimetre(0.5));

	std::shared_ptr<splotlevelscale> levelScale(new splotlevelscale(values, log, autoscale));

	std::shared_ptr<splotcolourscale> colourScaleContinuous(new splotcolourscale(values, colours, log, autoscale, fillOffscaleBottom, fillOffscaleTop));
	
	std::shared_ptr<GridData> grid1(new GridData(x1d, y1d, zGrid, xAxis1, yAxis3, colourScaleContinuous));
	std::shared_ptr<GridData> grid2(new GridData(x2d, y1d, zGrid, xAxis2, yAxis3, colourScaleContinuous));
	std::shared_ptr<GridData> grid3(new GridData(x1d, y2d, zGrid, xAxis3, yAxis3, colourScaleContinuous));
	std::shared_ptr<GridData> grid4(new GridData(x2d, y2d, zGrid, xAxis4, yAxis3, colourScaleContinuous));

	std::shared_ptr<ContourData> shade1(new ContourData(x1d, y1d, zCont, xAxis1, yAxis2, colourScaleDiscrete, noLine));
	std::shared_ptr<ContourData> shade2(new ContourData(x2d, y1d, zCont, xAxis2, yAxis2, colourScaleDiscrete, noLine));
	std::shared_ptr<ContourData> shade3(new ContourData(x1d, y2d, zCont, xAxis3, yAxis2, colourScaleDiscrete, noLine));
	std::shared_ptr<ContourData> shade4(new ContourData(x2d, y2d, zCont, xAxis4, yAxis2, colourScaleDiscrete, noLine));

	std::shared_ptr<ContourData> contour1(new ContourData(x1d, y1d, zCont, xAxis1, yAxis1, levelScale, contourStyle));
	std::shared_ptr<ContourData> contour2(new ContourData(x2d, y1d, zCont, xAxis2, yAxis1, levelScale, contourStyle));
	std::shared_ptr<ContourData> contour3(new ContourData(x1d, y2d, zCont, xAxis3, yAxis1, levelScale, contourStyle));
	std::shared_ptr<ContourData> contour4(new ContourData(x2d, y2d, zCont, xAxis4, yAxis1, levelScale, contourStyle));

	std::shared_ptr< splothorizontalcolourbar> colourbarContour(new splothorizontalcolourbar(Point(limits[0], grUnitless(0.22)), Length(grUnitless(0.03), Length::Scale::yDirection), Length(limits[4]-limits[0], Length::Scale::xDirection), colourScaleDiscrete, PlotAxis::Options(sU("Discrete Colourbar used by Shade"))));
	std::shared_ptr< splothorizontalcolourbar> colourbarGrid(new splothorizontalcolourbar(Point(limits[0], grUnitless(0.22 - 0.09)), Length(grUnitless(0.03), Length::Scale::yDirection), Length(limits[4] - limits[0], Length::Scale::xDirection), colourScaleContinuous, PlotAxis::Options(sU("Continuous Colourbar used by Grid"))));
	
	canvas->addItem(box);
	canvas->addItem(grid1);
	canvas->addItem(grid2);
	canvas->addItem(grid3);
	canvas->addItem(grid4);
	canvas->addItem(shade1);
	canvas->addItem(shade2);
	canvas->addItem(shade3);
	canvas->addItem(shade4);
	canvas->addItem(contour1);
	canvas->addItem(contour2);
	canvas->addItem(contour3);
	canvas->addItem(contour4);
	canvas->addItem(xAxis1);
	canvas->addItem(xAxis2);
	canvas->addItem(xAxis3);
	canvas->addItem(xAxis4);
	canvas->addItem(yAxis1);
	canvas->addItem(yAxis2);
	canvas->addItem(yAxis3);
	canvas->addItem(colourbarContour);
	canvas->addItem(colourbarGrid);

	frame->Show(true);
}

void mainFrame::OnRunPlotTests(wxCommandEvent& event)
{
	{
		//create a frame with empty axes running from 0-1
		//this just tests that drawing axes works
		GraphicsFrame<PlotCanvasPanel> *frame = new GraphicsFrame<PlotCanvasPanel>(this);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getPanel()->getCanvas();

		std::shared_ptr<PlotFrame> box(new PlotFrame(Point(grUnitless(0.02), grUnitless(0.08)), Point(grUnitless(0.92), grUnitless(0.98)), FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(grMillimetre(1.0)), sU("Plot 1: This plot should have an x and y axis running from 0.0-1.0 inside\na 0.8 level grey box with a 1 mm wide outline and no data."), Length(grTextPoint(12.0)), Length(grTextPoint(30.0))));
		PlotAxis::Options options;
		std::shared_ptr<PlotAxis> xAxis(new PlotAxis(0.0, 1.0, false, Point(grUnitless(0.1), grUnitless(0.9)), Point(grUnitless(0.9), grUnitless(0.9)), options.setTitle(sU("x"))));
		std::shared_ptr<PlotAxis> yAxis(new PlotAxis(0.0, 1.0, false, Point(grUnitless(0.1), grUnitless(0.9)), Point(grUnitless(0.1), grUnitless(0.1)), options.setTitle(sU("y"))));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);

		frame->Show(true);
	}

	{
		//create a frame with axes running from 0-1 and 3 circular points
		//this tests that points works
		GraphicsFrame<PlotCanvasPanel> *frame = new GraphicsFrame<PlotCanvasPanel>(this);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getPanel()->getCanvas();

		std::shared_ptr<PlotFrame> box(new PlotFrame(Point(grUnitless(0.02), grUnitless(0.08)), Point(grUnitless(0.92), grUnitless(0.98)), FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(grMillimetre(1.0)), sU("Plot 2: This plot should be identical to Plot 1, but have square data points at [0.1,0.2],[0.5,0.4],[0.8,0.6]"), Length(grTextPoint(12.0)), Length(grTextPoint(30.0))));
		PlotAxis::Options options;
		std::shared_ptr<PlotAxis> xAxis(new PlotAxis(0.0, 1.0, false, Point(grUnitless(0.1), grUnitless(0.9)), Point(grUnitless(0.9), grUnitless(0.9)), options.setTitle(sU("x"))));
		std::shared_ptr<PlotAxis> yAxis(new PlotAxis(0.0, 1.0, false, Point(grUnitless(0.1), grUnitless(0.9)), Point(grUnitless(0.1), grUnitless(0.1)), options.setTitle(sU("y"))));
		std::shared_ptr<PointData> points(new PointData(std::array<double, 3>{ 0.1, 0.5, 0.8 }, std::array<double, 3>{ 0.2, 0.4, 0.6 }, xAxis, yAxis, Symbol()));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(points);

		frame->Show(true);
	}

	{
		//create a frame with auto scaled axes and 3 circular points
		//this tests that auto scaling axes works
		GraphicsFrame<PlotCanvasPanel> *frame = new GraphicsFrame<PlotCanvasPanel>(this);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getPanel()->getCanvas();

		std::shared_ptr<PlotFrame> box(new PlotFrame(Point(grUnitless(0.02), grUnitless(0.08)), Point(grUnitless(0.92), grUnitless(0.98)), FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(grMillimetre(1.0)), sU("Plot 3: This plot should be identical to Plot 2, but have autoscaled axes, which\nshould range from 0.065-0.835 and 0.18-0.62"), Length(grTextPoint(12.0)), Length(grTextPoint(30.0))));
		PlotAxis::Options options;
		std::shared_ptr<PlotAxis> xAxis(new PlotAxis(false, Point(grUnitless(0.1), grUnitless(0.9)), Point(grUnitless(0.9), grUnitless(0.9)), options.setTitle(sU("x"))));
		std::shared_ptr<PlotAxis> yAxis(new PlotAxis(false, Point(grUnitless(0.1), grUnitless(0.9)), Point(grUnitless(0.1), grUnitless(0.1)), options.setTitle(sU("y"))));
		std::shared_ptr<PointData> points(new PointData(std::array<double, 3>{ 0.1, 0.5, 0.8 }, std::array<double, 3>{ 0.2, 0.4, 0.6 }, xAxis, yAxis, Symbol()));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(points);

		frame->Show(true);
	}

	{
		//create a frame showing an exponentially decreasing sin wave
		//this tests that lines and different line styles work
		GraphicsFrame<PlotCanvasPanel> *frame = new GraphicsFrame<PlotCanvasPanel>(this);
		frame->SetClientSize(1200, 400);
		auto canvas = frame->getPanel()->getCanvas();

		std::shared_ptr<PlotFrame> box(new PlotFrame(Point(grUnitless(0.03), grUnitless(0.15)), Point(grUnitless(0.98), grUnitless(0.9)), FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(grMillimetre(1.0)), sU("Plot 4: This plot should show an exponentially decreasing sin and cos wave and their average on a window with a size of 1200 x 400.\n The cos wave should have dot dash markings and the sum should have identical dot dash markings and be a wine red colour"), Length(grTextPoint(12.0)), Length(grTextPoint(30.0))));
		PlotAxis::Options options;
		std::shared_ptr<PlotAxis> xAxis(new PlotAxis(0.0, 2500, false, Point(grUnitless(0.06), grUnitless(0.85)), Point(grUnitless(0.98), grUnitless(0.85)), options.setTitle(sU("x"))));
		std::shared_ptr<PlotAxis> yAxis(new PlotAxis(-1.0, 1.0, false, Point(grUnitless(0.06), grUnitless(0.85)), Point(grUnitless(0.06), grUnitless(0.15)), options.setTitle(sU("y"))));
		std::vector<double> x(501);
		std::vector<double> y1(501);
		std::vector<double> y2(501);
		std::vector<double> y3(501);
		for (size_t i = 0; i < x.size(); ++i)
			x[i] = 5.0 * i;
		for (size_t i = 0; i < y1.size(); ++i)
			y1[i] = std::sin(x[i] * M_2PI / 360.0) * std::exp(-x[i]/720.0);
		for (size_t i = 0; i < y1.size(); ++i)
			y2[i] = std::cos(x[i] * M_2PI / 360.0) * std::exp(-x[i] / 720.0);
		for (size_t i = 0; i < y1.size(); ++i)
			y3[i] = (y1[i]+y2[i])/2.0;

		std::shared_ptr<LineData> line1(new LineData(x, y1, xAxis, yAxis, LineStyle(grMillimetre(1.0))));
		std::shared_ptr<LineData> line2(new LineData(x, y2, xAxis, yAxis, LineStyle(grMillimetre(1.0), rgbcolour(0, 0, 0), sU(".       -_\t   "))));
		std::shared_ptr<LineData> line3(new LineData(x, y3, xAxis, yAxis, LineStyle(grMillimetre(1.0), rgbcolour(0.8, 0, 0.2), { grMillimetre(0.5), grMillimetre(3.5), grMillimetre(3.0), grMillimetre(3.0) })));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(line1);
		canvas->addItem(line2);
		canvas->addItem(line3);

		frame->Show(true);
	}
	
	/* {
		//create a set of plots all plotting the same z, but using either the grid or contour routines and either 1d or 2d x and y coordinates
		// we choose the function 1+1/(x^2+2y^2) as this is different in the x and y axes and outside the range 0-1, so it tests to make sure
		// we do both axes correctly and tests the weird autoscaling of plshades

		sci::string title = sU("Plot 5: This plot should show a set of grid and contour plots of 1+1/(1+x#u2#d+2y#u2#d) with colour scale autoscaled");
		double scaleBegin = 0.0;
		double scaleEnd = 1.0;
		bool autoscale = true;
		bool fillOffscaleBottom = false;
		bool fillOffscaleTop = false;

		do2dplot(this, title, scaleBegin, scaleEnd, false, autoscale, fillOffscaleBottom, fillOffscaleTop);
	}

	{
		sci::string title = sU("Plot 6: This plot shows the same function as plot 5, but the colour scale is fixed and goes from 1.5 to 1.9");
		double scaleBegin = 1.5;
		double scaleEnd = 1.9;
		bool autoscale = false;
		bool fillOffscaleBottom = false;
		bool fillOffscaleTop = false;

		do2dplot(this, title, scaleBegin, scaleEnd, false, autoscale, fillOffscaleBottom, fillOffscaleTop);
	}

	{
		sci::string title = sU("Plot 7: This plot shows the same function as plot 5 and 6 with the same fixed range\ncolour scale as plot 6, but set to fill offscale at the bottom");
		double scaleBegin = 1.5;
		double scaleEnd = 1.9;
		bool autoscale = false;
		bool fillOffscaleBottom = true;
		bool fillOffscaleTop = false;

		do2dplot(this, title, scaleBegin, scaleEnd, false, autoscale, fillOffscaleBottom, fillOffscaleTop);
	}
	{
		sci::string title = sU("Plot 8: This plot shows the same function as plot 5 and 6 with the same fixed range\ncolour scale as plot 6, but set to fill offscale at the top");
		double scaleBegin = 1.5;
		double scaleEnd = 1.9;
		bool autoscale = false;
		bool fillOffscaleBottom = false;
		bool fillOffscaleTop = true;

		do2dplot(this, title, scaleBegin, scaleEnd, false, autoscale, fillOffscaleBottom, fillOffscaleTop);
	}*/

	{
		sci::string title = sU("Plot 9: This plot shows a set of grid and contour plots for the function z=10#uy#d, with\na log colourscale. It should show even horizontal stripes.");
		double scaleBegin = 1.0;
		double scaleEnd = 10.0;
		bool autoscale = false;
		bool fillOffscaleBottom = false;
		bool fillOffscaleTop = false;

		do2dplot(this, title, scaleBegin, scaleEnd, true, autoscale, fillOffscaleBottom, fillOffscaleTop);
	}
	/* {
		sci::string title = sU("Plot 10: This plot shows the same plot as plot 9, but with autoscale on for the colourscale. It\nshould look identical as plot 9 used exactly the full scale.");
		double scaleBegin = 2.0;
		double scaleEnd = 3.0;
		bool autoscale = true;
		bool fillOffscaleBottom = false;
		bool fillOffscaleTop = false;

		do2dplot(this, title, scaleBegin, scaleEnd, true, autoscale, fillOffscaleBottom, fillOffscaleTop);
	}*/
	/* {
		sci::string title = sU("Plot 11: This plot shows the same data as Plot 9, but with a scale from 2-5, rather than 1-10.");
		double scaleBegin = 2.0;
		double scaleEnd = 5.0;
		bool autoscale = false;
		bool fillOffscaleBottom = false;
		bool fillOffscaleTop = false;

		do2dplot(this, title, scaleBegin, scaleEnd, true, autoscale, fillOffscaleBottom, fillOffscaleTop);
	}
	{
		sci::string title = sU("Plot 12: This plot shows the same data as Plot 11, but with filloffscalebottom.");
		double scaleBegin = 2.0;
		double scaleEnd = 5.0;
		bool autoscale = false;
		bool fillOffscaleBottom = true;
		bool fillOffscaleTop = false;

		do2dplot(this, title, scaleBegin, scaleEnd, true, autoscale, fillOffscaleBottom, fillOffscaleTop);
	}
	{
		sci::string title = sU("Plot 13: This plot shows the same data as Plot 11, but with filloffscaletop.");
		double scaleBegin = 2.0;
		double scaleEnd = 5.0;
		bool autoscale = false;
		bool fillOffscaleBottom = false;
		bool fillOffscaleTop = true;

		do2dplot(this, title, scaleBegin, scaleEnd, true, autoscale, fillOffscaleBottom, fillOffscaleTop);
	}*/
}

void mainFrame::OnRunGraphicsTests(wxCommandEvent& event)
{
	{
		//unit test for Point
		grMillimetre x(0.5);
		grMillimetre y(0.6);
		const Point p1(x, y);
		Point p2;
		p2 = p1;
		assert(x == p1.getX(grMillimetre(10), grMillimetre(10)));
		assert(y == p1.getY(grMillimetre(10), grMillimetre(10)));
	}

	GraphicsFrame<PlotAxisTestPanel>* frame = new GraphicsFrame<PlotAxisTestPanel>(this);
	frame->Show();

	GraphicsFrame<RectangleTestPanel>* frame1 = new GraphicsFrame<RectangleTestPanel>(this);
	frame1->Show();

	GraphicsFrame<TextTestPanel>* frame2 = new GraphicsFrame<TextTestPanel>(this);
	frame2->Show();

	GraphicsFrame<PlotLineTestPanel>* frame3 = new GraphicsFrame<PlotLineTestPanel>(this);
	frame3->Show();
}

void mainFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(wxT("$projectname$ Version 1.00.0"), wxT("About $projectname$..."));
}

mainFrame::~mainFrame()
{
}

void RectangleTestPanel::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	wxRenderer renderer(&dc, GetClientSize(), grPerInch(FromDIP(96)));
	Point p1 = Point(grUnitless(0.1), grUnitless(0.1));
	Point p2 = Point(grUnitless(0.1), grUnitless(0.7));
	Point p3 = Point(grUnitless(0.3), grUnitless(0.7));
	Point p4 = Point(grUnitless(0.3), grUnitless(0.1));

	renderer.line(p1, p2);
	renderer.line(p2, p3);
	renderer.line(p3, p4);
	renderer.line(p4, p1);


	Point p5 = Point(grUnitless(0.4), grUnitless(0.1));
	Point p6 = Point(grUnitless(0.4), grUnitless(0.1)) + Distance(grMillimetre(0), grMillimetre(70));
	Point p7 = Point(grUnitless(0.4), grUnitless(0.1)) + Distance(grMillimetre(20), grMillimetre(70));
	Point p8 = Point(grUnitless(0.4), grUnitless(0.1)) + Distance(grMillimetre(20), grMillimetre(0));

	renderer.line(p5, p6);
	renderer.line(p6, p7);
	renderer.line(p7, p8);
	renderer.line(p8, p5);

	Point p9 = Point(grUnitless(0.7), grUnitless(0.1));
	Point p10 = Point(grUnitless(0.7), grUnitless(0.1)) + Distance(grUnitless(0), grUnitless(0.5), GraphicsVector::Scale::xDirection);
	Point p11 = Point(grUnitless(0.7), grUnitless(0.1)) + Distance(grUnitless(0.2), grUnitless(0.5), GraphicsVector::Scale::xDirection);
	Point p12 = Point(grUnitless(0.7), grUnitless(0.1)) + Distance(grUnitless(0.2), grUnitless(0), GraphicsVector::Scale::xDirection);

	renderer.line(p9, p10);
	renderer.line(p10, p11);
	renderer.line(p11, p12);
	renderer.line(p12, p9);

	renderer.text(sU("The left box will scale with resize, middle is fixed size and right will scale only with the window x size"),
		Point(grUnitless(0.5), grUnitless(0.8)), grUnitless(0.5), grUnitless(0.0));
}

Distance boundingBox(const TextMetric& textMetric)
{
	return (Distance(textMetric.width, textMetric.ascent + textMetric.descent));
}

void TextTestPanel::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	wxRenderer renderer(&dc, GetClientSize(), grPerInch(FromDIP(96)));

	Distance size(grMillimetre(2.0), grMillimetre(2.0));

	Point p1(grUnitless(0.02), grUnitless(0.1));
	renderer.rectangle(p1, size * grUnitless(2.), grUnitless(0.5), grUnitless(0.5));
	renderer.elipse(p1, size);
	TextMetric size1 = renderer.text(sU("text aligned 0.0,0.0 with the circle centre"), p1, grUnitless(0.0), grUnitless(0.0));
	renderer.rectangle(p1, boundingBox(size1), grUnitless(0.0), grUnitless(0.0));
	renderer.text(sU("text aligned 0.0,0.0 with the circle centre"), p1, grUnitless(0.0), grUnitless(0.0));

	Point p2(grUnitless(0.5), grUnitless(0.1));
	renderer.rectangle(p2, size * grUnitless(2.), grUnitless(0.5), grUnitless(0.5));
	renderer.elipse(p2, size);
	TextMetric size2 = renderer.text(sU("text aligned 0.5,0.0 with the circle centre"), p2, grUnitless(0.5), grUnitless(0.0));
	renderer.rectangle(p2, boundingBox(size2), grUnitless(0.5), grUnitless(0.0));
	renderer.text(sU("text aligned 0.5,0.0 with the circle centre"), p2, grUnitless(0.5), grUnitless(0.0));

	Point p3(grUnitless(0.98), grUnitless(0.1));
	renderer.rectangle(p3, size * grUnitless(2.), grUnitless(0.5), grUnitless(0.5));
	renderer.elipse(p3, size);
	TextMetric size3 = renderer.text(sU("text aligned 1.0,0.0 with the circle centre"), p3, grUnitless(1.0), grUnitless(0.0));
	renderer.rectangle(p3, boundingBox(size3), grUnitless(1.0), grUnitless(0.0));
	renderer.text(sU("text aligned 1.0,0.0 with the circle centre"), p3, grUnitless(1.0), grUnitless(0.0));

	Point p4 = p1+Distance(grMillimetre(0.0), grMillimetre(10.0));
	renderer.rectangle(p4, size * grUnitless(2.), grUnitless(0.5), grUnitless(0.5));
	renderer.elipse(p4, size);
	renderer.text(sU("text aligned 0.0,0.5 with the circle centre"), p4, grUnitless(0.0), grUnitless(0.5));

	Point p5 = p2 + Distance(grMillimetre(0.0), grMillimetre(10.0));
	renderer.rectangle(p5, size * grUnitless(2.), grUnitless(0.5), grUnitless(0.5));
	renderer.elipse(p5, size);
	renderer.text(sU("text aligned 0.5,0.5 with the circle centre"), p5, grUnitless(0.5), grUnitless(0.5));

	Point p6 = p3 + Distance(grMillimetre(0.0), grMillimetre(10.0));
	renderer.rectangle(p6, size * grUnitless(2.), grUnitless(0.5), grUnitless(0.5));
	renderer.elipse(p6, size);
	renderer.text(sU("text aligned 1.0,0.5 with the circle centre"), p6, grUnitless(1.0), grUnitless(0.5));

	Point p7 = p4 + Distance(grMillimetre(0.0), grMillimetre(10.0));
	renderer.rectangle(p7, size * grUnitless(2.), grUnitless(0.5), grUnitless(0.5));
	renderer.elipse(p7, size);
	renderer.text(sU("text aligned 0.0,1.0 with the circle centre"), p7, grUnitless(0.0), grUnitless(1.0));

	Point p8 = p5 + Distance(grMillimetre(0.0), grMillimetre(10.0));
	renderer.rectangle(p8, size * grUnitless(2.), grUnitless(0.5), grUnitless(0.5));
	renderer.elipse(p8, size);
	renderer.text(sU("text aligned 0.5,1.0 with the circle centre"), p8, grUnitless(0.5), grUnitless(1.0));

	Point p9 = p6 + Distance(grMillimetre(0.0), grMillimetre(10.0));
	renderer.rectangle(p9, size * grUnitless(2.), grUnitless(0.5), grUnitless(0.5));
	renderer.elipse(p9, size);
	renderer.text(sU("text aligned 1.0,1.0 with the circle centre"), p9, grUnitless(1.0), grUnitless(1.0));

	Point pr1 = p8 + Distance(grMillimetre(0.0), grMillimetre(10.0));
	renderer.rotatedText(sU("Rotated with 0.0,0.0 alignment"), pr1, grUnitless(0.0), grUnitless(0.0), grDegree(0.0));
	renderer.rotatedText(sU("Rotated with 0.0,0.0 alignment"), pr1, grUnitless(0.0), grUnitless(0.0), grDegree(180.0));

	Point pr2 = pr1 + Distance(grMillimetre(0.0), grMillimetre(10.0));
	renderer.rotatedText(sU("Rotated with 0.5,0.0 alignment"), pr2, grUnitless(0.5), grUnitless(0.0), grDegree(0.0));
	renderer.rotatedText(sU("Rotated with 0.5,0.0 alignment"), pr2, grUnitless(0.5), grUnitless(0.0), grDegree(180.0));

	Point pr3 = pr2 + Distance(grMillimetre(0.0), grMillimetre(10.0));
	renderer.rotatedText(sU("Rotated with 0.0,0.0 alignment"), pr3, grUnitless(1.0), grUnitless(0.0), grDegree(0.0));
	renderer.rotatedText(sU("Rotated with 1.0,0.0 alignment"), pr3, grUnitless(1.0), grUnitless(0.0), grDegree(180.0));

	Point pr4 = pr3 + Distance(grMillimetre(0.0), grMillimetre(10.0));
	renderer.rotatedText(sU("Rotated with 0.0,0.5 alignment"), pr4, grUnitless(0.0), grUnitless(0.5), grDegree(0.0));
	renderer.rotatedText(sU("Rotated with 0.0,0.5 alignment"), pr4, grUnitless(0.0), grUnitless(0.5), grDegree(180.0));
	
	Point pr5 = pr4 + Distance(grMillimetre(0.0), grMillimetre(10.0));
	renderer.rotatedText(sU("Rotated with 0.0,1.0 alignment"), pr5, grUnitless(0.0), grUnitless(1.0), grDegree(0.0));
	renderer.rotatedText(sU("Rotated with 0.0,1.0 alignment"), pr5, grUnitless(0.0), grUnitless(1.0), grDegree(180.0));


	Point pr6 = pr3 + Distance(grMillimetre(0.0), grMillimetre(90.0));
	renderer.rectangle(pr6, size * grUnitless(2.), grUnitless(0.5), grUnitless(0.5));
	renderer.elipse(pr6, size);
	renderer.rotatedText(sU("Rotated with 0.5,0.5 alignment 000 deg       Rotated with 0.5,0.5 alignment 000 deg"), pr6, grUnitless(0.5), grUnitless(0.5), grDegree(0.0));
	renderer.rotatedText(sU("Rotated with 0.5,0.5 alignment 045 deg       Rotated with 0.5,0.5 alignment 045 deg"), pr6, grUnitless(0.5), grUnitless(0.5), grDegree(45.0));
	renderer.rotatedText(sU("Rotated with 0.5,0.5 alignment 090 deg       Rotated with 0.5,0.5 alignment 090 deg"), pr6, grUnitless(0.5), grUnitless(0.5), grDegree(90.0));
	renderer.rotatedText(sU("Rotated with 0.5,0.5 alignment 135 deg       Rotated with 0.5,0.5 alignment 135 deg"), pr6, grUnitless(0.5), grUnitless(0.5), grDegree(135.0));
	
	renderer.line(Point(grUnitless(0.5), grUnitless(0.0)), Point(grUnitless(0.5), grUnitless(1.0)));


	renderer.setFont(sci::string(sU("Arial") ), Length(grTextPoint(40)), rgbcolour(0, 0, 0));
	TextMetric AlphabetSize = renderer.text(sU("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"),
		Point(grUnitless(0.05), grUnitless(0.7)), grUnitless(0.0), grUnitless(0.0));
	renderer.rectangle(Point(grUnitless(0.05), grUnitless(0.7)), boundingBox(AlphabetSize));
	renderer.setFont(sci::string(sU("Arial")), Length(grTextPoint(40)), rgbcolour(0, 0, 0));
	renderer.text(sU("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"),
		Point(grUnitless(0.05), grUnitless(0.7)), grUnitless(0.0), grUnitless(0.0));

	TextMetric xSize = renderer.formattedText(sU("M^2N^{3\\alpha}"), Point(grUnitless(0.05), grUnitless(0.8)));
	renderer.rectangle(Point(grUnitless(0.05), grUnitless(0.8)), boundingBox(xSize));
	renderer.formattedText(sU("M^2N^{3\\alpha}"), Point(grUnitless(0.05), grUnitless(0.8)));

	renderer.formattedText(sU("M^2N^{3\\alpha}"), Point(grUnitless(0.9), grUnitless(0.5)), grUnitless(0.0), grUnitless(0.0), grDegree(90.0));
	renderer.formattedText(sU("M^2N^{3\\alpha}"), Point(grUnitless(0.9), grUnitless(0.5)), grUnitless(0.0), grUnitless(0.0), grDegree(225.0));
	renderer.elipse(Point(grUnitless(0.9), grUnitless(0.5)), size);
}

void PlotAxisTestPanel::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	wxRenderer renderer(&dc, GetClientSize(), grPerInch(FromDIP(96)));

	sci::string axisTitle = sU("Area (m^2)");


	Point left(grUnitless(0.05), grUnitless(0.01));
	Point right(grUnitless(0.45), grUnitless(0.01));
	Point top(grUnitless(0.55), grUnitless(0.05));
	Point bottom(grUnitless(0.55), grUnitless(0.45));
	Distance voffset(grUnitless(0.0), grUnitless(0.125));
	Distance hoffset(grUnitless(0.1), grUnitless(0.0));

	//linear horizontal axes

	PlotAxis axis1(0.0, 10.0, false, left, right, PlotAxis::Options(sU("0-10 with auto major interval of 2 and 3 subticks")));
	axis1.draw(renderer, grPerInch(96));

	PlotAxis axis2(0.0, 20.0, false, left + voffset, right + voffset, PlotAxis::Options(sU("0-20 with auto major interval of 2 and 3 subticks")));
	axis2.draw(renderer, grPerInch(96));

	PlotAxis axis3(0.0, 50.0, false, left + voffset * grUnitless(2.0), right + voffset * grUnitless(2.0), PlotAxis::Options(sU("0-50 with auto major interval of 10 and 4 subticks")));
	axis3.draw(renderer, grPerInch(96));

	PlotAxis axis4(0.0, 80.0, false, left + voffset * grUnitless(3.0), right + voffset * grUnitless(3.0), PlotAxis::Options(sU("0-80 with auto major interval of 10 and 4 subticks")));
	axis4.draw(renderer, grPerInch(96));

	//linear vertical axes

	PlotAxis axis5(0.0, 10.0, false, bottom, top, PlotAxis::Options(sU("1-10 with auto major interval of 2 and 3 subticks")));
	axis5.draw(renderer, grPerInch(96));

	PlotAxis axis6(0.0, 20.0, false, bottom + hoffset * grUnitless(1.0), top + hoffset * grUnitless(1.0), PlotAxis::Options(sU("0-20 with auto major interval of 2 and 3 subticks")));
	axis6.draw(renderer, grPerInch(96));

	PlotAxis axis7(0.0, 50.0, false, bottom + hoffset * grUnitless(2.0), top + hoffset * grUnitless(2.0), PlotAxis::Options(sU("0-50 with auto major interval of 10 and 4 subticks")));
	axis7.draw(renderer, grPerInch(96));

	PlotAxis axis8(0.0, 80.0, false, bottom + hoffset * grUnitless(3.0), top + hoffset * grUnitless(3.0), PlotAxis::Options(sU("0-80 with auto major interval of 10 and 4 subticks")));
	axis8.draw(renderer, grPerInch(96));

	//log axes

	PlotAxis axis9(1.0, 10.0, true, left + voffset * grUnitless(4.0), right + voffset * grUnitless(4.0), PlotAxis::Options(sU("1-10 log increments")));
	axis9.draw(renderer, grPerInch(96));

	PlotAxis axis10(1.0, 100000.0, true, left + voffset * grUnitless(5.0), right + voffset * grUnitless(5.0), PlotAxis::Options(sU("1-1\u00D710^{5} log increments (scientific from 10,000)")));
	axis10.draw(renderer, grPerInch(96));

	PlotAxis axis11(1.0, 200.0, true, left + voffset * grUnitless(6.0), right + voffset * grUnitless(6.0), PlotAxis::Options(sU("1-200 log increments")));
	axis11.draw(renderer, grPerInch(96));

	PlotAxis::Options options12(sU("1-1\u00D710^{20} ticks on decades"));
	options12.m_majorInterval = 5;
	options12.m_nSubticks = 4;
	options12.m_autoMajorInterval = false;
	options12.m_autoNSubticks = false;
	PlotAxis axis12(1.0, 1e20, true, left + voffset * grUnitless(7.0), right + voffset * grUnitless(7.0), options12);
	axis12.draw(renderer, grPerInch(96));



	PlotAxis::Options options13;
	options13.m_lineStyle = LineStyle(grMillimetre(1), rgbcolour(1.0, 0.3, 0.0));
	options13.m_title = sU("Diferent colours with Algerian font");
	options13.m_titleFont.m_facenames = { sci::string(sU("Algerian")) };
	options13.m_titleFont.m_colour = rgbcolour(0.0, 0.0, 0.5);
	options13.m_labelFont.m_colour = rgbcolour(0.5, 0.5, 0.5);
	PlotAxis axis13(0.0, 20.0, false, bottom + voffset, bottom + (right - left) + voffset, options13);
	axis13.draw(renderer, grPerInch(96));
}

void PlotLineTestPanel::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	wxRenderer renderer(&dc, GetClientSize(), grPerInch(FromDIP(96)));

	//bottom left plot regular axis directions
	std::shared_ptr<PlotAxis> xAxis1(new PlotAxis(0.0, 1.0, false, Point(grUnitless(0.1), grUnitless(0.9)),
		Point(grUnitless(0.45), grUnitless(0.9)), PlotAxis::Options(sU("Normal x axis"))));
	std::shared_ptr<PlotAxis> yAxis1(new PlotAxis(0.0, 1.0, false, Point(grUnitless(0.1), grUnitless(0.9)),
		Point(grUnitless(0.1), grUnitless(0.55)), PlotAxis::Options(sU("Normal y axis"))));
	
	//top left plot
	//reversed both x values and end points to give regular direction
	//reversed y end points to give reversed axis
	std::shared_ptr<PlotAxis> xAxis2(new PlotAxis(1.0, 0.0, false, Point(grUnitless(0.45), grUnitless(0.1)),
		Point(grUnitless(0.1), grUnitless(0.1)), PlotAxis::Options(sU("Double reversed x axis"))));
	std::shared_ptr<PlotAxis> yAxis2(new PlotAxis(0.0, 1.0, false, Point(grUnitless(0.1), grUnitless(0.1)),
		Point(grUnitless(0.1), grUnitless(0.45)), PlotAxis::Options(sU("Reversed position y axis"))));

	//bottom right plot
	//reversed x end points to give reversed direction
	//reversed both y values and end points to give regular direction
	std::shared_ptr<PlotAxis> xAxis3(new PlotAxis(0.0, 1.0, false, Point(grUnitless(0.9), grUnitless(0.9)),
		Point(grUnitless(0.55), grUnitless(0.9)), PlotAxis::Options(sU("Reversed position x axis"))));
	std::shared_ptr<PlotAxis> yAxis3(new PlotAxis(1.0, 0.0, false, Point(grUnitless(0.9), grUnitless(0.55)),
		Point(grUnitless(0.9), grUnitless(0.9)), PlotAxis::Options(sU("Double reversed y axis"))));

	//top right plot
	//reversed x values to give reversed direction
	//reversed y values to give reversed direction
	std::shared_ptr<PlotAxis> xAxis4(new PlotAxis(1.0, 0.0, false, Point(grUnitless(0.55), grUnitless(0.1)),
		Point(grUnitless(0.9), grUnitless(0.1)), PlotAxis::Options(sU("Reversed values x axis"))));
	std::shared_ptr<PlotAxis> yAxis4(new PlotAxis(1.0, 0.0, false, Point(grUnitless(0.9), grUnitless(0.45)),
		Point(grUnitless(0.9), grUnitless(0.1)), PlotAxis::Options(sU("Reversed values y axis"))));
	//yAxis->setticksdirection(true, false);

	xAxis1->draw(renderer, grPerInch(96));
	yAxis1->draw(renderer, grPerInch(96));

	xAxis2->draw(renderer, grPerInch(96));
	yAxis2->draw(renderer, grPerInch(96));

	xAxis3->draw(renderer, grPerInch(96));
	yAxis3->draw(renderer, grPerInch(96));

	xAxis4->draw(renderer, grPerInch(96));
	yAxis4->draw(renderer, grPerInch(96));

	std::vector<double> x{ 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
	std::vector<double> y{ 0.05, 0.23, 0.64, 0.73, 0.82, 0.68, 0.52, 0.18, 0.07 };
	std::vector<Distance> squareSymbol{ Distance(grMillimetre(-1.0), grMillimetre(-1.0)), Distance(grMillimetre(1.0), grMillimetre(-1.0)),
	Distance(grMillimetre(1.0), grMillimetre(1.0)), Distance(grMillimetre(-1.0), grMillimetre(1.0)), Distance(grMillimetre(-1.0), grMillimetre(-1.0)) };

	std::shared_ptr<LineData> lineData1(new LineData(x, y, xAxis1, yAxis1, LineStyle()));
	std::shared_ptr<PointData> pointData1(new PointData(x, y, xAxis1, yAxis1, Symbol(squareSymbol, rgbcolour(0.5, 0.0, 0.8))));
	lineData1->draw(renderer, grPerInch(96));
	pointData1->draw(renderer, grPerInch(96));

	std::shared_ptr<LineData> lineData2(new LineData(x, y, xAxis2, yAxis2, LineStyle()));
	std::shared_ptr<PointData> pointData2(new PointData(x, y, xAxis2, yAxis2, Symbol(squareSymbol, rgbcolour(0.5, 0.0, 0.8))));
	lineData2->draw(renderer, grPerInch(96));
	pointData2->draw(renderer, grPerInch(96));

	std::shared_ptr<LineData> lineData3(new LineData(x, y, xAxis3, yAxis3, LineStyle()));
	std::shared_ptr<PointData> pointData3(new PointData(x, y, xAxis3, yAxis3, Symbol(squareSymbol, rgbcolour(0.5, 0.0, 0.8))));
	lineData3->draw(renderer, grPerInch(96));
	pointData3->draw(renderer, grPerInch(96));

	std::shared_ptr<LineData> lineData4(new LineData(x, y, xAxis4, yAxis4, LineStyle()));
	std::shared_ptr<PointData> pointData4(new PointData(x, y, xAxis4, yAxis4, Symbol(squareSymbol, rgbcolour(0.5, 0.0, 0.8))));
	lineData4->draw(renderer, grPerInch(96));
	pointData4->draw(renderer, grPerInch(96));
}
