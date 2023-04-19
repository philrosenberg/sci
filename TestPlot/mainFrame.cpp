#include "mainFrame.h"
#include<wx/scrolwin.h>
#include<svector/splot.h>
//#include<svector/plotdata.h>

const int mainFrame::ID_FILE_EXIT = ::wxNewId();
const int mainFrame::ID_FILE_RUN = ::wxNewId();
const int mainFrame::ID_HELP_ABOUT = ::wxNewId();

BEGIN_EVENT_TABLE(mainFrame, wxFrame)
EVT_MENU(ID_FILE_EXIT, mainFrame::OnExit)
EVT_MENU(ID_FILE_RUN, mainFrame::OnRun)
EVT_MENU(ID_HELP_ABOUT, mainFrame::OnAbout)
END_EVENT_TABLE()

mainFrame::mainFrame(wxFrame *frame, const wxString& title)
	: wxFrame(frame, -1, title)
{
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu(wxT(""));
	fileMenu->Append(ID_FILE_EXIT, wxT("E&xit\tAlt+F4"), wxT("Exit the application"));
	fileMenu->Append(ID_FILE_RUN, wxT("Run\tCtrl+R"), wxT("Run Code"));
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

void mainFrame::OnRun(wxCommandEvent& event)
{
	/* {
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
	}*/

	{
		//create a set of plots all plotting the same z, but using either the grid or contour routines and either 1d or 2d x and y coordinates
		// we choose the function 1+1/(x^2+2y^2) as this is different in the x and y axes and outside the range 0-1, so it tests to make sure
		// we do both axes correctly and tests the weird autoscaling of plshades
		splotframe* frame = new splotframe(this, true);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getCanvas();

		std::shared_ptr<PlotFrame> box(new PlotFrame(0.02, 0.02, 0.9, 0.9, FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(2.0), sU("Plot 5: This plot should show a set of grid and contour plots of 1+1/(1+x#u2#d+2y#u2#d)")));
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
			x1d[i] = i * 1.0 / (double)(nx-1);
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



		std::vector<std::vector<double>> zGrid(nx-1);
		for (size_t i = 0; i < nx - 1; ++i)
		{
			zGrid[i].resize(ny - 1);
			for (size_t j = 0; j < ny - 1; ++j)
			{
				double x = (x1d[i] + x1d[i + 1]) / 2.0;
				double y = (y1d[j] + y1d[j + 1]) / 2.0;
				zGrid[i][j] = 1.0 + 1.0 / (1.0 + x * x + 2.0 * y * y);
			}
		}

		std::vector<std::vector<double>> zCont(nx);
		for (size_t i = 0; i < nx; ++i)
		{
			zCont[i].resize(ny);
			for (size_t j = 0; j < ny; ++j)
			{
				zCont[i][j] = 1.0 + 1.0 / (1.0 + x1d[i] * x1d[i] + 2.0 * y1d[j] * y1d[j]);
			}
		}

		size_t nColours = 10;
		std::vector<hlscolour> colours(nColours);
		std::vector<double> values(nColours + 1);
		double hueInterval = 240.0 / (nColours - 1);
		double valueInterval = 1.0 / nColours;
		for (size_t i = 0; i < nColours; ++i)
			colours[i] = hlscolour(240 - i * hueInterval, 0.5, 1.0);
		for (size_t i = 0; i < nColours + 1; ++i)
			values[i] = i * valueInterval;
		std::shared_ptr<splotcolourscale> colourScaleCont(new splotcolourscale(values, colours, false, true));
		LineStyle lineStyle(0);

		std::shared_ptr<splotlevelscale> levelScale(new splotlevelscale(values, values, false, true));

		std::shared_ptr<splotcolourscale> colourScaleGrid(new splotcolourscale());
		std::shared_ptr<GridData> grid1(new GridData(x1d, y1d, zGrid, xAxis1, yAxis3, colourScaleGrid, false, false));
		std::shared_ptr<GridData> grid2(new GridData(x2d, y1d, zGrid, xAxis2, yAxis3, colourScaleGrid, false, false));
		std::shared_ptr<GridData> grid3(new GridData(x1d, y2d, zGrid, xAxis3, yAxis3, colourScaleGrid, false, false));
		std::shared_ptr<GridData> grid4(new GridData(x2d, y2d, zGrid, xAxis4, yAxis3, colourScaleGrid, false, false));

		std::shared_ptr<ContourData> shade1(new ContourData(x1d, y1d, zCont, xAxis1, yAxis2, colourScaleCont, false, false, lineStyle));
		std::shared_ptr<ContourData> shade2(new ContourData(x2d, y1d, zCont, xAxis2, yAxis2, colourScaleCont, false, false, lineStyle));
		std::shared_ptr<ContourData> shade3(new ContourData(x1d, y2d, zCont, xAxis3, yAxis2, colourScaleCont, false, false, lineStyle));
		std::shared_ptr<ContourData> shade4(new ContourData(x2d, y2d, zCont, xAxis4, yAxis2, colourScaleCont, false, false, lineStyle));

		std::shared_ptr<ContourData> contour1(new ContourData(x1d, y1d, zCont, xAxis1, yAxis1, levelScale, false, false, lineStyle));
		std::shared_ptr<ContourData> contour2(new ContourData(x2d, y1d, zCont, xAxis2, yAxis1, levelScale, false, false, lineStyle));
		std::shared_ptr<ContourData> contour3(new ContourData(x1d, y2d, zCont, xAxis3, yAxis1, levelScale, false, false, lineStyle));
		std::shared_ptr<ContourData> contour4(new ContourData(x2d, y2d, zCont, xAxis4, yAxis1, levelScale, false, false, lineStyle));

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

		frame->Show(true);
	}
	/*{
		//create a frame with axes running from 0-1 and 3 circular points
		splotframe* frame = new splotframe(this, true);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getCanvas();

		std::shared_ptr<PlotFrame> box(new PlotFrame(0.02, 0.02, 0.9, 0.9, FillStyle(rgbcolour(0.8, 0.8, 0.8)), LineStyle(2.0), sU("Plot 5: This plot should show a contour plot of 1/(1+x#u2#d+y#u2#d)")));
		std::shared_ptr<splotaxis> xAxis(new splotaxis(0.0, 1.0, false, PlotScale::Direction::horizontal, 0.1, 0.9, 0.1, sU("x")));
		std::shared_ptr<splotaxis> yAxis(new splotaxis(0.0, 1.0, false, PlotScale::Direction::vertical, 0.1, 0.9, 0.1, sU("y")));

		std::vector<double> x(21);
		std::vector<double> y(21);
		for (size_t i = 0; i < x.size(); ++i)
			x[i] = i * 1.0 / (double)(x.size() - 1);
		for (size_t i = 0; i < y.size(); ++i)
			y[i] = i * 1.0 / (double)(y.size() - 1);
		std::vector<std::vector<double>> z(x.size());
		for (size_t i = 0; i < z.size(); ++i)
		{
			z[i].resize(y.size());
			for (size_t j = 0; j < z[i].size(); ++j)
				z[i][j] = 1.0 / (1.0 + x[i] * x[i] + y[j] * y[j]);
		}

		//GridData g(x, y, z, xAxis, yAxis, new colourscale(), false, false);
		size_t nColours = 10;
		std::vector<hlscolour> colours(nColours);
		std::vector<double> values(nColours+1);
		double hueInterval = 240.0 / (nColours - 1);
		double valueInterval = 1.0 / nColours;
		for (size_t i = 0; i < nColours; ++i)
			colours[i] = hlscolour(240 - i * hueInterval, 0.5, 1.0);
		for (size_t i = 0; i < nColours + 1; ++i)
			values[i] = i * valueInterval;
		std::shared_ptr<splotcolourscale> colourScale(new splotcolourscale(values, colours, false, false));
		LineStyle lineStyle(0);
		std::shared_ptr<ContourData> contour(new ContourData(x, y, z, xAxis, yAxis, colourScale, false, false, lineStyle));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(contour);

		frame->Show(true);
	}*/
}

void mainFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(wxT("$projectname$ Version 1.00.0"), wxT("About $projectname$..."));
}

mainFrame::~mainFrame()
{
}