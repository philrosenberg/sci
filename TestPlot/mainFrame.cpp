#include "mainFrame.h"
#include<wx/scrolwin.h>
#include<svector/splot.h>
//#include<svector/plotdata.h>

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

void do2dplot(wxWindow *parent, sci::string title, double scaleBegin, double scaleEnd, bool log, bool autoscale, bool fillOffscaleBottom, bool fillOffscaleTop)
{
	//create a set of plots all plotting the same z, but using either the grid or contour routines and either 1d or 2d x and y coordinates
	// we choose the function 1+1/(x^2+2y^2) as this is different in the x and y axes and outside the range 0-1, so it tests to make sure
	// we do both axes correctly and tests the weird autoscaling of plshades
	splotframe* frame = new splotframe(parent, true);
	frame->SetClientSize(800, 800);
	auto canvas = frame->getCanvas();

	std::shared_ptr<PlotFrame> box(new PlotFrame(0.02, 0.02, 0.9, 0.9, FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(2.0), title));
	std::vector<double> limits{ 0.1, 0.1 + 0.8 / 4.0, 0.1 + 2.0 * 0.8 / 4.0, 0.1 + 3.0 * 0.8 / 4.0, 0.1 + 0.8 };
	std::shared_ptr<splotaxis> xAxis1(new splotaxis(0.0, 1.0, false, PlotScale::Direction::horizontal, limits[0], limits[1], 0.1, sU("")));
	std::shared_ptr<splotaxis> xAxis2(new splotaxis(0.0, 1.0, false, PlotScale::Direction::horizontal, limits[1], limits[2], 0.1, sU("")));
	std::shared_ptr<splotaxis> xAxis3(new splotaxis(0.0, 1.0, false, PlotScale::Direction::horizontal, limits[2], limits[3], 0.1, sU("")));
	std::shared_ptr<splotaxis> xAxis4(new splotaxis(0.0, 1.0, false, PlotScale::Direction::horizontal, limits[3], limits[4], 0.1, sU("")));
	std::shared_ptr<splotaxis> yAxis1(new splotaxis(0.0, 1.0, false, PlotScale::Direction::vertical, limits[0], limits[1], 0.1, sU("")));
	std::shared_ptr<splotaxis> yAxis2(new splotaxis(0.0, 1.0, false, PlotScale::Direction::vertical, limits[1], limits[2], 0.1, sU("")));
	std::shared_ptr<splotaxis> yAxis3(new splotaxis(0.0, 1.0, false, PlotScale::Direction::vertical, limits[2], limits[3], 0.1, sU("")));

	size_t nx = 21;
	size_t ny = 21;

	std::vector<double> x1d(nx);
	std::vector<double> y1d(ny);
	for (size_t i = 0; i < nx; ++i)
		x1d[i] = i * 1.0 / (double)(nx - 1);
	for (size_t i = 0; i < ny; ++i)
		y1d[i] = i * 1.0 / (double)(ny - 1);

	std::vector<std::vector<double>> x2d(nx);
	std::vector<std::vector<double>> y2d(nx);
	for (size_t i = 0; i < nx; ++i)
	{
		x2d[i].resize(ny);
		for (size_t j = 0; j < ny; ++j)
			x2d[i][j] = i * 1.0 / (double)(nx - 1);
	}
	for (size_t i = 0; i < nx; ++i)
	{
		y2d[i].resize(ny);
		for (size_t j = 0; j < ny; ++j)
			y2d[i][j] = j * 1.0 / (double)(ny - 1);
	}



	std::vector<std::vector<double>> zGrid(nx - 1);
	for (size_t i = 0; i < nx - 1; ++i)
	{
		zGrid[i].resize(ny - 1);
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

	std::vector<std::vector<double>> zCont(nx);
	for (size_t i = 0; i < nx; ++i)
	{
		zCont[i].resize(ny);
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

	std::shared_ptr<splotcolourscale> colourScaleCont(new splotcolourscale(values, colours, log, autoscale, fillOffscaleBottom, fillOffscaleTop));
	LineStyle lineStyle(0);

	std::shared_ptr<splotlevelscale> levelScale(new splotlevelscale(values, log, autoscale));

	std::shared_ptr<splotcolourscale> colourScaleGrid(new splotcolourscale({ values.front(), values.back()}, {colours.front(), colours.back()}, log, autoscale, fillOffscaleBottom, fillOffscaleTop));
	
	std::shared_ptr<GridData> grid1(new GridData(x1d, y1d, zGrid, xAxis1, yAxis3, colourScaleGrid));
	std::shared_ptr<GridData> grid2(new GridData(x2d, y1d, zGrid, xAxis2, yAxis3, colourScaleGrid));
	std::shared_ptr<GridData> grid3(new GridData(x1d, y2d, zGrid, xAxis3, yAxis3, colourScaleGrid));
	std::shared_ptr<GridData> grid4(new GridData(x2d, y2d, zGrid, xAxis4, yAxis3, colourScaleGrid));

	std::shared_ptr<ContourData> shade1(new ContourData(x1d, y1d, zCont, xAxis1, yAxis2, colourScaleCont, lineStyle));
	std::shared_ptr<ContourData> shade2(new ContourData(x2d, y1d, zCont, xAxis2, yAxis2, colourScaleCont, lineStyle));
	std::shared_ptr<ContourData> shade3(new ContourData(x1d, y2d, zCont, xAxis3, yAxis2, colourScaleCont, lineStyle));
	std::shared_ptr<ContourData> shade4(new ContourData(x2d, y2d, zCont, xAxis4, yAxis2, colourScaleCont, lineStyle));

	std::shared_ptr<ContourData> contour1(new ContourData(x1d, y1d, zCont, xAxis1, yAxis1, levelScale, lineStyle));
	std::shared_ptr<ContourData> contour2(new ContourData(x2d, y1d, zCont, xAxis2, yAxis1, levelScale, lineStyle));
	std::shared_ptr<ContourData> contour3(new ContourData(x1d, y2d, zCont, xAxis3, yAxis1, levelScale, lineStyle));
	std::shared_ptr<ContourData> contour4(new ContourData(x2d, y2d, zCont, xAxis4, yAxis1, levelScale, lineStyle));

	std::shared_ptr< splothorizontalcolourbar> colourbarContour(new splothorizontalcolourbar(limits[0], limits[4], limits[3] + 0.06, limits[3] + 0.09, colourScaleCont, sU("Contour Colourbar")));
	std::shared_ptr< splothorizontalcolourbar> colourbarGrid(new splothorizontalcolourbar(limits[0], limits[4], limits[3] + 0.15, limits[3] + 0.18, colourScaleGrid, sU("Grid Colourbar")));

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
		splotframe* frame = new splotframe(this, true);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getCanvas();

		std::shared_ptr<PlotFrame> box(new PlotFrame(0.02, 0.02, 0.9, 0.9, FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(2.0), sU("Plot 1: This plot should have an x and y axis running from 0.0-1.0 inside\na 0.8 level grey box with a 2 pixel wide outline and no data.")));
		std::shared_ptr<splotaxis> xAxis(new splotaxis(0.0, 1.0, false, PlotScale::Direction::horizontal, 0.1, 0.9, 0.1, sU("x")));
		std::shared_ptr<splotaxis> yAxis(new splotaxis(0.0, 1.0, false, PlotScale::Direction::vertical, 0.1, 0.9, 0.1, sU("y")));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);

		frame->Show(true);
	}


	{
		//create a frame with axes running from 0-1 and 3 circular points
		//this tests that points works
		splotframe* frame = new splotframe(this, true);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getCanvas();

		std::shared_ptr<PlotFrame> box(new PlotFrame(0.02, 0.02, 0.9, 0.9, FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(2.0), sU("Plot 2: This plot should be identical to Plot 1, but have circular data points at [0.1,0.2],[0.5,0.4],[0.8,0.6]")));
		std::shared_ptr<splotaxis> xAxis(new splotaxis(0.0, 1.0, false, PlotScale::Direction::horizontal, 0.1, 0.9, 0.1, sU("x")));
		std::shared_ptr<splotaxis> yAxis(new splotaxis(0.0, 1.0, false, PlotScale::Direction::vertical, 0.1, 0.9, 0.1, sU("y")));
		std::shared_ptr<PointData> points(new PointData({ 0.1, 0.5, 0.8 }, { 0.2, 0.4, 0.6 }, xAxis, yAxis, Symbol()));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(points);

		frame->Show(true);
	}

	{
		//create a frame with auto scaled axes and 3 circular points
		//this tests that auto scaling axes works
		splotframe* frame = new splotframe(this, true);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getCanvas();

		std::shared_ptr<PlotFrame> box(new PlotFrame(0.02, 0.02, 0.9, 0.9, FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(2.0), sU("Plot 3: This plot should be identical to Plot 2, but have autoscaled axes, which\nshould range from 0.65-0.835 and 0.18-0.62")));
		std::shared_ptr<splotaxis> xAxis(new splotaxis(false, PlotScale::Direction::horizontal, 0.1, 0.9, 0.1, sU("x")));
		std::shared_ptr<splotaxis> yAxis(new splotaxis(false, PlotScale::Direction::vertical, 0.1, 0.9, 0.1, sU("y")));
		std::shared_ptr<PointData> points(new PointData({ 0.1, 0.5, 0.8 }, { 0.2, 0.4, 0.6 }, xAxis, yAxis, Symbol()));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(points);

		frame->Show(true);
	}

	{
		//create a frame showing an exponentially decreasing sin wave
		//this tests that lines and different line styles work
		splotframe* frame = new splotframe(this, true);
		frame->SetClientSize(1200, 400);
		auto canvas = frame->getCanvas();

		std::shared_ptr<PlotFrame> box(new PlotFrame(0.03, 0.05, 0.95, 0.85, FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(2.0), sU("Plot 4: This plot should show an exponentially decreasing sin and cos wave and their average on a window with a size of 1200 x 400.\n The cos wave should have dot dash markings and the sum should have identical dot dash markings and be a wine red colour")));
		std::shared_ptr<splotaxis> xAxis(new splotaxis(0.0, 2500, false, PlotScale::Direction::horizontal, 0.06, 0.98, 0.15, sU("x")));
		std::shared_ptr<splotaxis> yAxis(new splotaxis(-1.0, 1.0, false, PlotScale::Direction::vertical, 0.15, 0.85, 0.06, sU("y")));
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

		std::shared_ptr<LineData> line1(new LineData(x, y1, xAxis, yAxis, LineStyle(2.0)));
		std::shared_ptr<LineData> line2(new LineData(x, y2, xAxis, yAxis, LineStyle(2.0, rgbcolour(0, 0, 0), sU(".          -_..\t      "))));
		std::shared_ptr<LineData> line3(new LineData(x, y3, xAxis, yAxis, LineStyle(2.0, rgbcolour(0.8, 0, 0.2), { 200,1800 }, { 2000, 2000 })));

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
	{
		sci::string title = sU("Plot 10: This plot shows the same plot as plot 9, but with autoscale on for the colourscale. It\nshould look identical as plot 9 used exactly the full scale.");
		double scaleBegin = 2.0;
		double scaleEnd = 3.0;
		bool autoscale = true;
		bool fillOffscaleBottom = false;
		bool fillOffscaleTop = false;

		do2dplot(this, title, scaleBegin, scaleEnd, true, autoscale, fillOffscaleBottom, fillOffscaleTop);
	}
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

	GraphicsFrame<>* frame = new GraphicsFrame<>(this);
	frame->Show();

	GraphicsFrame<GraphicsPanel1>* frame1 = new GraphicsFrame<GraphicsPanel1>(this);
	frame1->Show();

	GraphicsFrame<GraphicsPanel2>* frame2 = new GraphicsFrame<GraphicsPanel2>(this);
	frame2->Show();
}

void mainFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(wxT("$projectname$ Version 1.00.0"), wxT("About $projectname$..."));
}

mainFrame::~mainFrame()
{
}

void GraphicsPanel1::OnPaint(wxPaintEvent& event)
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

void GraphicsPanel2::OnPaint(wxPaintEvent& event)
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
}