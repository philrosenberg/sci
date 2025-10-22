#include "mainFrame.h"
#include<svector/svector.h>
#include<svector/plot.h>
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

using unitless = sci::graphics::unitless;
using millimetre = sci::graphics::millimetre;
using textPoint = sci::graphics::textPoint;
using degree = sci::graphics::degree;
using perMillimetre = sci::graphics::perMillimetre;
using perInch = sci::graphics::perInch;
using HlsColour = sci::graphics::HlsColour;
using RgbColour = sci::graphics::RgbColour;

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
	sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel> *frame = new sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>(parent);
	frame->SetClientSize(800, 800);
	auto canvas = frame->getPanel()->getCanvas();

	std::shared_ptr<sci::plot::PlotFrame> box(new sci::plot::PlotFrame(sci::plot::Point(unitless(0.02), unitless(0.08)), sci::plot::Point(unitless(0.92), unitless(0.98)), sci::plot::FillStyle(sci::graphics::RgbColour(0.8, 0.8, 0.8)), sci::plot::LineStyle(sci::graphics::millimetre(1.0)), title, sci::graphics::Length(sci::graphics::textPoint(12.0)), sci::plot::Length(sci::graphics::textPoint(30.0))));
	std::vector<unitless> limits{ unitless(0.1), unitless(0.1 + 0.8 / 4.0), unitless(0.1 + 2.0 * 0.8 / 4.0), unitless(0.1 + 3.0 * 0.8 / 4.0), unitless(0.1 + 0.8) };
	sci::plot::Axis::Options options;
	std::shared_ptr<sci::plot::Axis> xAxis1(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(limits[0],unitless(0.9)), sci::graphics::Point(limits[1], unitless(0.9)),  options.setTitle(sU("x-1d y-1d"))));
	std::shared_ptr<sci::plot::Axis> xAxis2(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(limits[1], unitless(0.9)), sci::graphics::Point(limits[2], unitless(0.9)), options.setTitle(sU("x-2d y-1d"))));
	std::shared_ptr<sci::plot::Axis> xAxis3(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(limits[2], unitless(0.9)), sci::graphics::Point(limits[3], unitless(0.9)), options.setTitle(sU("x-1d y-2d"))));
	std::shared_ptr<sci::plot::Axis> xAxis4(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(limits[3], unitless(0.9)), sci::graphics::Point(limits[4], unitless(0.9)), options.setTitle(sU("x-2d y-2d"))));
	std::shared_ptr<sci::plot::Axis> yAxis1(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(limits[0], unitless(0.9)), sci::graphics::Point(limits[0], unitless(0.7)), options.setTitle(sU("Contour"))));
	std::shared_ptr<sci::plot::Axis> yAxis2(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(limits[0], unitless(0.7)), sci::graphics::Point(limits[0], unitless(0.5)), options.setTitle(sU("Shade"))));
	std::shared_ptr<sci::plot::Axis> yAxis3(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(limits[0], unitless(0.5)), sci::graphics::Point(limits[0], unitless(0.3)), options.setTitle(sU("Grid"))));
	
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
	std::vector<HlsColour> colours(nColours);
	std::vector<double> valuesDiscrete(nColours + 1);
	std::vector<double> valuesContinuous(nColours);
	degree hueInterval = degree(240.0) / unitless(nColours - 1);
	double valueIntervalDiscrete = scaleRange / (nColours);
	double valueIntervalContinuous = scaleRange / (nColours - 1);
	for (size_t i = 0; i < nColours; ++i)
	{
		colours[i] = HlsColour(degree(240) - unitless(i) * hueInterval, 0.5, 1.0);
	}
	for (size_t i = 0; i < valuesDiscrete.size(); ++i)
		valuesDiscrete[i] = scaleBegin + i * valueIntervalDiscrete;
	for (size_t i = 0; i < valuesContinuous.size(); ++i)
		valuesContinuous[i] = scaleBegin + i * valueIntervalContinuous;

	if (log)
	{
		for (auto& v : valuesDiscrete)
			v = std::pow(10.0, v);
		for (auto& v : valuesContinuous)
			v = std::pow(10.0, v);
	}

	std::shared_ptr<sci::plot::ColourScale> colourScaleDiscrete(new sci::plot::ColourScale(valuesDiscrete, colours, log, autoscale, fillOffscaleBottom, fillOffscaleTop));
	sci::plot::LineStyle contourStyle(millimetre(0.5));

	std::shared_ptr<sci::plot::LevelScale> levelScale(new sci::plot::LevelScale(valuesDiscrete, log, autoscale));

	std::shared_ptr<sci::plot::ColourScale> colourScaleContinuous(new sci::plot::ColourScale(valuesContinuous, colours, log, autoscale, fillOffscaleBottom, fillOffscaleTop));
	
	std::shared_ptr<sci::plot::Grid> grid1(new sci::plot::Grid(x1d, y1d, zGrid, xAxis1, yAxis3, colourScaleContinuous));
	std::shared_ptr<sci::plot::Grid> grid2(new sci::plot::Grid(x2d, y1d, zGrid, xAxis2, yAxis3, colourScaleContinuous));
	std::shared_ptr<sci::plot::Grid> grid3(new sci::plot::Grid(x1d, y2d, zGrid, xAxis3, yAxis3, colourScaleContinuous));
	std::shared_ptr<sci::plot::Grid> grid4(new sci::plot::Grid(x2d, y2d, zGrid, xAxis4, yAxis3, colourScaleContinuous));

	std::shared_ptr<sci::plot::Contours> shade1(new sci::plot::Contours(x1d, y1d, zCont, xAxis1, yAxis2, colourScaleDiscrete, sci::plot::noLine));
	std::shared_ptr<sci::plot::Contours> shade2(new sci::plot::Contours(x2d, y1d, zCont, xAxis2, yAxis2, colourScaleDiscrete, sci::plot::noLine));
	std::shared_ptr<sci::plot::Contours> shade3(new sci::plot::Contours(x1d, y2d, zCont, xAxis3, yAxis2, colourScaleDiscrete, sci::plot::noLine));
	std::shared_ptr<sci::plot::Contours> shade4(new sci::plot::Contours(x2d, y2d, zCont, xAxis4, yAxis2, colourScaleDiscrete, sci::plot::noLine));

	std::shared_ptr<sci::plot::Contours> contour1(new sci::plot::Contours(x1d, y1d, zCont, xAxis1, yAxis1, levelScale, contourStyle));
	std::shared_ptr<sci::plot::Contours> contour2(new sci::plot::Contours(x2d, y1d, zCont, xAxis2, yAxis1, levelScale, contourStyle));
	std::shared_ptr<sci::plot::Contours> contour3(new sci::plot::Contours(x1d, y2d, zCont, xAxis3, yAxis1, levelScale, contourStyle));
	std::shared_ptr<sci::plot::Contours> contour4(new sci::plot::Contours(x2d, y2d, zCont, xAxis4, yAxis1, levelScale, contourStyle));

	std::shared_ptr< sci::plot::HorizontalColourBar> colourbarContour(new sci::plot::HorizontalColourBar(sci::graphics::Point(limits[0], unitless(0.22)), sci::graphics::Point(limits[4], unitless(0.19)), colourScaleDiscrete, sci::plot::Axis::Options(sU("Discrete Colourbar used by Shade"))));
	std::shared_ptr< sci::plot::HorizontalColourBar> colourbarGrid(new sci::plot::HorizontalColourBar(sci::graphics::Point(limits[0], unitless(0.22 - 0.09)), sci::graphics::Point(limits[4], unitless(0.19-0.09)), colourScaleContinuous, sci::plot::Axis::Options(sU("Continuous Colourbar used by Grid"))));
	
	//do a test with a NullRenderer - this makes debugging much easier
	sci::graphics::NullRenderer nullRenderer;
	bool preDrawDone;
	do
	{
		box->preDraw();
		grid1->preDraw();
		grid2->preDraw();
		grid3->preDraw();
		grid4->preDraw();
		shade1->preDraw();
		shade2->preDraw();
		shade3->preDraw();
		shade4->preDraw();
		contour1->preDraw();
		contour2->preDraw();
		contour3->preDraw();
		contour4->preDraw();
		xAxis1->preDraw();
		xAxis2->preDraw();
		xAxis3->preDraw();
		xAxis4->preDraw();
		yAxis1->preDraw();
		yAxis2->preDraw();
		yAxis3->preDraw();
		colourbarContour->preDraw();
		colourbarGrid->preDraw();

		preDrawDone = box->readyToDraw() &&
		grid1->readyToDraw() &&
		grid2->readyToDraw() &&
		grid3->readyToDraw() &&
		grid4->readyToDraw() &&
		shade1->readyToDraw() &&
		shade2->readyToDraw() &&
		shade3->readyToDraw() &&
		shade4->readyToDraw() &&
		contour1->readyToDraw() &&
		contour2->readyToDraw() &&
		contour3->readyToDraw() &&
		contour4->readyToDraw() &&
		xAxis1->readyToDraw() &&
		xAxis2->readyToDraw() &&
		xAxis3->readyToDraw() &&
		xAxis4->readyToDraw() &&
		yAxis1->readyToDraw() &&
		yAxis2->readyToDraw() &&
		yAxis3->readyToDraw() &&
		colourbarContour->readyToDraw() &&
		colourbarGrid->readyToDraw();
		
	} while (!preDrawDone);


	box->draw(nullRenderer, perMillimetre(1));
	grid1->draw(nullRenderer, perMillimetre(1));
	grid2->draw(nullRenderer, perMillimetre(1));
	grid3->draw(nullRenderer, perMillimetre(1));
	grid4->draw(nullRenderer, perMillimetre(1));
	shade1->draw(nullRenderer, perMillimetre(1));
	shade2->draw(nullRenderer, perMillimetre(1));
	shade3->draw(nullRenderer, perMillimetre(1));
	shade4->draw(nullRenderer, perMillimetre(1));
	contour1->draw(nullRenderer, perMillimetre(1));
	contour2->draw(nullRenderer, perMillimetre(1));
	contour3->draw(nullRenderer, perMillimetre(1));
	contour4->draw(nullRenderer, perMillimetre(1));
	xAxis1->draw(nullRenderer, perMillimetre(1));
	xAxis2->draw(nullRenderer, perMillimetre(1));
	xAxis3->draw(nullRenderer, perMillimetre(1));
	xAxis4->draw(nullRenderer, perMillimetre(1));
	yAxis1->draw(nullRenderer, perMillimetre(1));
	yAxis2->draw(nullRenderer, perMillimetre(1));
	yAxis3->draw(nullRenderer, perMillimetre(1));
	colourbarContour->draw(nullRenderer, perMillimetre(1));
	colourbarGrid->draw(nullRenderer, perMillimetre(1));

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
		sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel> *frame = new sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>(this);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getPanel()->getCanvas();

		std::shared_ptr<sci::plot::PlotFrame> box(new sci::plot::PlotFrame(sci::graphics::Point(unitless(0.02), unitless(0.08)), sci::graphics::Point(unitless(0.92), unitless(0.98)), sci::plot::FillStyle(RgbColour(0.8, 0.8, 0.8)), sci::plot::LineStyle(millimetre(1.0)), sU("Plot 1: This plot should have an x and y axis running from 0.0-1.0 inside\na 0.8 level grey box with a 1 mm wide outline and no data."), sci::graphics::Length(textPoint(12.0)), sci::graphics::Length(textPoint(30.0))));
		sci::plot::Axis::Options options;
		std::shared_ptr<sci::plot::Axis> xAxis(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.9), unitless(0.9)), options.setTitle(sU("x"))));
		std::shared_ptr<sci::plot::Axis> yAxis(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.1), unitless(0.1)), options.setTitle(sU("y"))));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);

		frame->Show(true);
	}

	{
		//create a frame with axes running from 0-1 and 3 circular sci::graphics::Points
		//this tests that sci::graphics::Points works
		sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel> *frame = new sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>(this);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getPanel()->getCanvas();

		std::shared_ptr<sci::plot::PlotFrame> box(new sci::plot::PlotFrame(sci::graphics::Point(unitless(0.02), unitless(0.08)), sci::graphics::Point(unitless(0.92), unitless(0.98)), sci::plot::FillStyle(RgbColour(0.8, 0.8, 0.8)), sci::plot::LineStyle(millimetre(1.0)), sU("Plot 2: This plot should be identical to Plot 1, but have square data sci::graphics::Points at [0.1,0.2],[0.5,0.4],[0.8,0.6]"), sci::graphics::Length(textPoint(12.0)), sci::graphics::Length(textPoint(30.0))));
		sci::plot::Axis::Options options;
		std::array<double, 3> xs{ 0.1, 0.5, 0.8 };
		std::array<double, 3> ys{ 0.2, 0.4, 0.6 };
		std::array<double, 3> xErrors{ 0.2, 0.1, 0.12 };
		std::array<double, 3> yErrors{ 0.05, 0.05, 0.05 };
		std::shared_ptr<sci::plot::Axis> xAxis(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.9), unitless(0.9)), options.setTitle(sU("x"))));
		std::shared_ptr<sci::plot::Axis> yAxis(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.1), unitless(0.1)), options.setTitle(sU("y"))));
		std::shared_ptr<sci::plot::Points> points(new sci::plot::Points(xs, ys, xAxis, yAxis, sci::plot::Symbol(), sci::graphics::RgbColour()));
		std::shared_ptr<sci::plot::HorizontalErrorBars> xBars(new sci::plot::HorizontalErrorBars(xs, ys, xErrors, xErrors, xAxis, yAxis, sci::graphics::millimetre(5)));
		std::shared_ptr<sci::plot::VerticalErrorBars> yBars(new sci::plot::VerticalErrorBars(xs, ys, yErrors, yErrors, xAxis, yAxis, sci::graphics::millimetre(5)));


		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(xBars);
		canvas->addItem(yBars);
		canvas->addItem(points);

		frame->Show(true);
	}

	{
		//create a frame with auto scaled axes and 3 circular sci::graphics::Points
		//this tests that auto scaling axes works
		sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel> *frame = new sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>(this);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getPanel()->getCanvas();

		std::shared_ptr<sci::plot::PlotFrame> box(new sci::plot::PlotFrame(sci::graphics::Point(unitless(0.02), unitless(0.08)), sci::graphics::Point(unitless(0.92), unitless(0.98)), sci::plot::FillStyle(RgbColour(0.8, 0.8, 0.8)), sci::plot::LineStyle(millimetre(1.0)), sU("Plot 3: This plot should be identical to Plot 2, but have autoscaled axes, which\nshould range from 0.065-0.835 and 0.18-0.62"), sci::graphics::Length(textPoint(12.0)), sci::graphics::Length(textPoint(30.0))));
		sci::plot::Axis::Options options;
		std::array<double, 3> xs{ 0.1, 0.5, 0.8 };
		std::array<double, 3> ys{ 0.2, 0.4, 0.6 };
		std::array<double, 3> xErrors{ 0.2, 0.1, 0.12 };
		std::array<double, 3> yErrors{ 0.05, 0.05, 0.05 };
		std::shared_ptr<sci::plot::Axis> xAxis(new sci::plot::Axis(false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.9), unitless(0.9)), options.setTitle(sU("x"))));
		std::shared_ptr<sci::plot::Axis> yAxis(new sci::plot::Axis(false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.1), unitless(0.1)), options.setTitle(sU("y"))));
		std::shared_ptr<sci::plot::Points> points(new sci::plot::Points(xs, ys, xAxis, yAxis, sci::plot::Symbol(), sci::graphics::RgbColour()));
		std::shared_ptr<sci::plot::HorizontalErrorBars> xBars(new sci::plot::HorizontalErrorBars(xs, ys, xErrors, xErrors, xAxis, yAxis, sci::graphics::millimetre(5), sci::plot::LineStyle(), false));
		std::shared_ptr<sci::plot::VerticalErrorBars> yBars(new sci::plot::VerticalErrorBars(xs, ys, yErrors, yErrors, xAxis, yAxis, sci::graphics::millimetre(5), sci::plot::LineStyle(), false));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(xBars);
		canvas->addItem(yBars);
		canvas->addItem(points);

		frame->Show(true);
	}

	{
		//create a frame with auto scaled axes and 3 circular sci::graphics::Points
		//this tests that auto scaling axes works
		sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>* frame = new sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>(this);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getPanel()->getCanvas();

		std::shared_ptr<sci::plot::PlotFrame> box(new sci::plot::PlotFrame(sci::graphics::Point(unitless(0.02), unitless(0.08)), sci::graphics::Point(unitless(0.92), unitless(0.98)), sci::plot::FillStyle(RgbColour(0.8, 0.8, 0.8)), sci::plot::LineStyle(millimetre(1.0)), sU("Plot 4: This plot should be identical to Plot 3, but have autoscaled axes, where\nthe errors bars contribute to the autoscaling"), sci::graphics::Length(textPoint(12.0)), sci::graphics::Length(textPoint(30.0))));
		sci::plot::Axis::Options options;
		std::array<double, 3> xs{ 0.1, 0.5, 0.8 };
		std::array<double, 3> ys{ 0.2, 0.4, 0.6 };
		std::array<double, 3> xErrors{ 0.2, 0.1, 0.12 };
		std::array<double, 3> yErrors{ 0.05, 0.05, 0.05 };
		std::shared_ptr<sci::plot::Axis> xAxis(new sci::plot::Axis(false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.9), unitless(0.9)), options.setTitle(sU("x"))));
		std::shared_ptr<sci::plot::Axis> yAxis(new sci::plot::Axis(false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.1), unitless(0.1)), options.setTitle(sU("y"))));
		std::shared_ptr<sci::plot::Points> points(new sci::plot::Points(xs, ys, xAxis, yAxis, sci::plot::Symbol(), sci::graphics::RgbColour()));
		std::shared_ptr<sci::plot::HorizontalErrorBars> xBars(new sci::plot::HorizontalErrorBars(xs, ys, xErrors, xErrors, xAxis, yAxis, sci::graphics::millimetre(5), sci::plot::LineStyle(), true));
		std::shared_ptr<sci::plot::VerticalErrorBars> yBars(new sci::plot::VerticalErrorBars(xs, ys, yErrors, yErrors, xAxis, yAxis, sci::graphics::millimetre(5), sci::plot::LineStyle(), true));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(xBars);
		canvas->addItem(yBars);
		canvas->addItem(points);

		frame->Show(true);
	}

	{
		//create a frame with auto scaled axes, 3 vertical and 3 horizontal bars
		//this tests that auto scaling axes works
		sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>* frame = new sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>(this);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getPanel()->getCanvas();

		std::shared_ptr<sci::plot::PlotFrame> box(new sci::plot::PlotFrame(sci::graphics::Point(unitless(0.02), unitless(0.08)), sci::graphics::Point(unitless(0.92), unitless(0.98)), sci::plot::FillStyle(RgbColour(0.8, 0.8, 0.8)), sci::plot::LineStyle(millimetre(1.0)), sU("Plot 3: This plot should be identical to Plot 2, but have autoscaled axes, which\nshould range from 0.065-0.835 and 0.18-0.62"), sci::graphics::Length(textPoint(12.0)), sci::graphics::Length(textPoint(30.0))));
		sci::plot::Axis::Options options;
		std::array<double, 3> xs1{ 0.2, 1.0, 1.6 };
		std::array<double, 3> ys1{ 0.2, 0.4, 0.6 };
		std::array<double, 3> xs2{ 0.1, 0.5, 0.8 };
		std::array<double, 3> ys2{ 0.4, 0.8, 1.2 };
		std::array<double, 3> widths{ 0.2, 0.2, 0.2 };
		std::array<double, 3> xErrors{ 0.2, 0.1, 0.12 };
		std::array<double, 3> yErrors{ 0.05, 0.05, 0.05 };
		std::shared_ptr<sci::plot::Axis> xAxis(new sci::plot::Axis(false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.9), unitless(0.9)), options.setTitle(sU("x"))));
		std::shared_ptr<sci::plot::Axis> yAxis(new sci::plot::Axis(false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.1), unitless(0.1)), options.setTitle(sU("y"))));
		std::shared_ptr<sci::plot::VerticalBars> verticalBars(new sci::plot::VerticalBars(xs1, ys1, widths, xAxis, yAxis, sci::plot::LineStyle(), sci::graphics::RgbColour(0.6, 0.0, 0.8)));
		std::shared_ptr<sci::plot::HorizontalBars> horizontalBars(new sci::plot::HorizontalBars(xs2, ys2, widths, xAxis, yAxis, sci::plot::LineStyle(), sci::graphics::RgbColour(0.6, 0.0, 0.8)));
		std::shared_ptr<sci::plot::VerticalErrorBars> yErrorBars(new sci::plot::VerticalErrorBars(xs1, ys1, yErrors, yErrors, xAxis, yAxis, sci::graphics::millimetre(5), sci::plot::LineStyle(), false));
		std::shared_ptr<sci::plot::HorizontalErrorBars> xErrorBars(new sci::plot::HorizontalErrorBars(xs2, ys2, xErrors, xErrors, xAxis, yAxis, sci::graphics::millimetre(5), sci::plot::LineStyle(), false));
		
		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(verticalBars);
		canvas->addItem(horizontalBars);
		canvas->addItem(xErrorBars);
		canvas->addItem(yErrorBars);

		frame->Show(true);
	}

	{
		//create a frame with auto scaled axes and a filled area
		//this tests that auto scaling axes works
		sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>* frame = new sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>(this);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getPanel()->getCanvas();

		std::shared_ptr<sci::plot::PlotFrame> box(new sci::plot::PlotFrame(sci::graphics::Point(unitless(0.02), unitless(0.08)), sci::graphics::Point(unitless(0.92), unitless(0.98)), sci::plot::FillStyle(RgbColour(0.8, 0.8, 0.8)), sci::plot::LineStyle(millimetre(1.0)), sU("Plot 4: This plot should be identical to Plot 3, but have autoscaled axes, where\nthe errors bars contribute to the autoscaling"), sci::graphics::Length(textPoint(12.0)), sci::graphics::Length(textPoint(30.0))));
		sci::plot::Axis::Options options;
		std::array<double, 5> xs1{ 0.1, 0.5, 0.4, 0.2, 0.1 };
		std::array<double, 5> ys1{ 0.2, 0.4, 0.6, 0.4, 0.5 };
		std::array<double, 6> xs2{ 0.6, 1.0, 0.9, 0.7, 0.6, 0.6 };
		std::array<double, 6> ys2{ 0.7, 0.9, 1.1, 0.9, 1.0, 0.7 };
		std::shared_ptr<sci::plot::Axis> xAxis(new sci::plot::Axis(false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.9), unitless(0.9)), options.setTitle(sU("x"))));
		std::shared_ptr<sci::plot::Axis> yAxis(new sci::plot::Axis(false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.1), unitless(0.1)), options.setTitle(sU("y"))));
		std::shared_ptr<sci::plot::Fill> fill1(new sci::plot::Fill(xs1, ys1, xAxis, yAxis, sci::graphics::RgbColour(0.2, 0.2, 0.8), sci::plot::LineStyle(sci::graphics::millimetre(2))));
		std::shared_ptr<sci::plot::Fill> fill2(new sci::plot::Fill(xs2, ys2, xAxis, yAxis, sci::graphics::RgbColour(0.2, 0.2, 0.8), sci::plot::LineStyle(sci::graphics::millimetre(2))));
		
		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(fill1);
		canvas->addItem(fill2);

		frame->Show(true);
	}

	{
		//create a plot showing points with varying size and colour
		sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>* frame = new sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>(this);
		frame->SetClientSize(800, 800);
		auto canvas = frame->getPanel()->getCanvas();

		std::vector<double> y1(10, 1.0);
		std::vector<double> y2(10, 2.0);
		std::vector<double> y3(10, 3.0);
		std::vector<double> x{ 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };
		std::vector<double> z = x;

		std::shared_ptr <sci::plot::ColourScale> colourScale( new sci::plot::ColourScale(std::vector<double>{1.0, 10.0}, std::vector<sci::graphics::RgbColour>{sci::graphics::RgbColour(1.0, 0.0, 0.0), sci::graphics::RgbColour(0.0, 0.0, 1.0)}));
		std::shared_ptr <sci::plot::SizeScale > sizeScale(new sci::plot::SizeScale(std::vector<double>{1.0, 10.0}, std::vector<double>{1.0, 10.0}));

		std::shared_ptr<sci::plot::PlotFrame> box(new sci::plot::PlotFrame(sci::graphics::Point(unitless(0.02), unitless(0.08)), sci::graphics::Point(unitless(0.92), unitless(0.98)), sci::plot::FillStyle(RgbColour(0.8, 0.8, 0.8)), sci::plot::LineStyle(millimetre(1.0)), sU("Varying Symbol Plot. Going left to right, points should transition from small to big and red to blue."), sci::graphics::Length(textPoint(12.0)), sci::graphics::Length(textPoint(30.0))));
		sci::plot::Axis::Options options;
		std::shared_ptr<sci::plot::Axis> xAxis(new sci::plot::Axis(0.0, 11.0, false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.9), unitless(0.9)), options.setTitle(sU("x"))));
		std::shared_ptr<sci::plot::Axis> yAxis(new sci::plot::Axis(0.0, 4.0, false, sci::graphics::Point(unitless(0.1), unitless(0.9)), sci::graphics::Point(unitless(0.1), unitless(0.1)), options.setTitle(sU("y"))));
		std::shared_ptr<sci::plot::PointsSizeVarying> pointsSizeVarying(new sci::plot::PointsSizeVarying(x, y1, z, xAxis, yAxis, sizeScale, sci::plot::Symbol(), sci::graphics::RgbColour()));
		std::shared_ptr<sci::plot::PointsColourVarying> pointsColourVarying(new sci::plot::PointsColourVarying(x, y2, z, xAxis, yAxis, colourScale, sci::plot::Symbol()));
		std::shared_ptr<sci::plot::PointsColourAndSizeVarying> pointsColourAndSizeVarying(new sci::plot::PointsColourAndSizeVarying(x, y3, z, z, xAxis, yAxis, colourScale, sizeScale, sci::plot::Symbol()));

		canvas->addItem(box);
		canvas->addItem(xAxis);
		canvas->addItem(yAxis);
		canvas->addItem(pointsSizeVarying);
		canvas->addItem(pointsColourVarying);
		canvas->addItem(pointsColourAndSizeVarying);

		frame->Show(true);
	}

	{
		//create a frame showing an exponentially decreasing sin wave
		//this tests that lines and different line styles work
		sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel> *frame = new sci::graphics::GraphicsFrame<sci::plot::PlotCanvasPanel>(this);
		frame->SetClientSize(1200, 400);
		auto canvas = frame->getPanel()->getCanvas();

		std::shared_ptr<sci::plot::PlotFrame> box(new sci::plot::PlotFrame(sci::plot::Point(unitless(0.03), unitless(0.15)), sci::graphics::Point(unitless(0.98), unitless(0.9)), sci::plot::FillStyle(RgbColour(0.8, 0.8, 0.8)), sci::plot::LineStyle(millimetre(1.0)), sU("Plot 4: This plot should show an exponentially decreasing sin and cos wave and their average on a window with a size of 1200 x 400.\n The cos wave should have dot dash markings and the sum should have identical dot dash markings and be a wine red colour"), sci::graphics::Length(textPoint(12.0)), sci::graphics::Length(textPoint(30.0))));
		sci::plot::Axis::Options options;
		std::shared_ptr<sci::plot::Axis> xAxis(new sci::plot::Axis(0.0, 2500, false, sci::graphics::Point(unitless(0.06), unitless(0.85)), sci::graphics::Point(unitless(0.98), unitless(0.85)), options.setTitle(sU("x"))));
		std::shared_ptr<sci::plot::Axis> yAxis(new sci::plot::Axis(-1.0, 1.0, false, sci::graphics::Point(unitless(0.06), unitless(0.85)), sci::graphics::Point(unitless(0.06), unitless(0.15)), options.setTitle(sU("y"))));
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

		std::shared_ptr<sci::plot::Lines> line1(new sci::plot::Lines(x, y1, xAxis, yAxis, sci::plot::LineStyle(millimetre(1.0))));
		std::shared_ptr<sci::plot::Lines> line2(new sci::plot::Lines(x, y2, xAxis, yAxis, sci::plot::LineStyle(millimetre(1.0), RgbColour(0, 0, 0), sU(".       -_\t   "))));
		std::shared_ptr<sci::plot::Lines> line3(new sci::plot::Lines(x, y3, xAxis, yAxis, sci::plot::LineStyle(millimetre(1.0), RgbColour(0.8, 0, 0.2), { millimetre(0.5), millimetre(3.5), millimetre(3.0), millimetre(3.0) })));

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
		//unit test for sci::graphics::Point
		millimetre x(0.5);
		millimetre y(0.6);
		const sci::graphics::Point p1(x, y);
		sci::graphics::Point p2;
		p2 = p1;
		assert(x == p1.getX(millimetre(10), millimetre(10)));
		assert(y == p1.getY(millimetre(10), millimetre(10)));
	}

	sci::graphics::GraphicsFrame<PlotAxisTestPanel>* frame = new sci::graphics::GraphicsFrame<PlotAxisTestPanel>(this);
	frame->Show();

	sci::graphics::GraphicsFrame<RectangleTestPanel>* frame1 = new sci::graphics::GraphicsFrame<RectangleTestPanel>(this);
	frame1->Show();

	sci::graphics::GraphicsFrame<TextTestPanel>* frame2 = new sci::graphics::GraphicsFrame<TextTestPanel>(this);
	frame2->Show();

	sci::graphics::GraphicsFrame<PlotLineTestPanel>* frame3 = new sci::graphics::GraphicsFrame<PlotLineTestPanel>(this);
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
	sci::graphics::wxRenderer renderer(&dc, GetClientSize(), perInch(FromDIP(96)));
	sci::graphics::Point p1 = sci::graphics::Point(unitless(0.1), unitless(0.1));
	sci::graphics::Point p2 = sci::graphics::Point(unitless(0.1), unitless(0.7));
	sci::graphics::Point p3 = sci::graphics::Point(unitless(0.3), unitless(0.7));
	sci::graphics::Point p4 = sci::graphics::Point(unitless(0.3), unitless(0.1));

	renderer.line(p1, p2);
	renderer.line(p2, p3);
	renderer.line(p3, p4);
	renderer.line(p4, p1);


	sci::graphics::Point p5 = sci::graphics::Point(unitless(0.4), unitless(0.1));
	sci::graphics::Point p6 = sci::graphics::Point(unitless(0.4), unitless(0.1)) + sci::graphics::Distance(millimetre(0), millimetre(70));
	sci::graphics::Point p7 = sci::graphics::Point(unitless(0.4), unitless(0.1)) + sci::graphics::Distance(millimetre(20), millimetre(70));
	sci::graphics::Point p8 = sci::graphics::Point(unitless(0.4), unitless(0.1)) + sci::graphics::Distance(millimetre(20), millimetre(0));

	renderer.line(p5, p6);
	renderer.line(p6, p7);
	renderer.line(p7, p8);
	renderer.line(p8, p5);

	sci::graphics::Point p9 = sci::graphics::Point(unitless(0.7), unitless(0.1));
	sci::graphics::Point p10 = sci::graphics::Point(unitless(0.7), unitless(0.1)) + sci::graphics::Distance(unitless(0), unitless(0.5), sci::graphics::Point::ScaleDirection::xDirection);
	sci::graphics::Point p11 = sci::graphics::Point(unitless(0.7), unitless(0.1)) + sci::graphics::Distance(unitless(0.2), unitless(0.5), sci::graphics::Point::ScaleDirection::xDirection);
	sci::graphics::Point p12 = sci::graphics::Point(unitless(0.7), unitless(0.1)) + sci::graphics::Distance(unitless(0.2), unitless(0), sci::graphics::Point::ScaleDirection::xDirection);

	renderer.line(p9, p10);
	renderer.line(p10, p11);
	renderer.line(p11, p12);
	renderer.line(p12, p9);

	renderer.text(sU("The left box will scale with resize, middle is fixed size and right will scale only with the window x size"),
		sci::graphics::Point(unitless(0.5), unitless(0.8)), unitless(0.5), unitless(0.0));
}

sci::graphics::Distance boundingBox(const sci::graphics::TextMetric& textMetric)
{
	return (sci::graphics::Distance(textMetric.width, textMetric.ascent + textMetric.descent));
}

void TextTestPanel::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	sci::graphics::wxRenderer renderer(&dc, GetClientSize(), perInch(FromDIP(96)));

	sci::graphics::Distance size(millimetre(2.0), millimetre(2.0));

	sci::graphics::Point p1(unitless(0.02), unitless(0.1));
	renderer.rectangle(p1, size * unitless(2.), unitless(0.5), unitless(0.5));
	renderer.elipse(p1, size);
	sci::graphics::TextMetric size1 = renderer.text(sU("text aligned 0.0,0.0 with the circle centre"), p1, unitless(0.0), unitless(0.0));
	renderer.rectangle(p1, boundingBox(size1), unitless(0.0), unitless(0.0));
	renderer.text(sU("text aligned 0.0,0.0 with the circle centre"), p1, unitless(0.0), unitless(0.0));

	sci::graphics::Point p2(unitless(0.5), unitless(0.1));
	renderer.rectangle(p2, size * unitless(2.), unitless(0.5), unitless(0.5));
	renderer.elipse(p2, size);
	sci::graphics::TextMetric size2 = renderer.text(sU("text aligned 0.5,0.0 with the circle centre"), p2, unitless(0.5), unitless(0.0));
	renderer.rectangle(p2, boundingBox(size2), unitless(0.5), unitless(0.0));
	renderer.text(sU("text aligned 0.5,0.0 with the circle centre"), p2, unitless(0.5), unitless(0.0));

	sci::graphics::Point p3(unitless(0.98), unitless(0.1));
	renderer.rectangle(p3, size * unitless(2.), unitless(0.5), unitless(0.5));
	renderer.elipse(p3, size);
	sci::graphics::TextMetric size3 = renderer.text(sU("text aligned 1.0,0.0 with the circle centre"), p3, unitless(1.0), unitless(0.0));
	renderer.rectangle(p3, boundingBox(size3), unitless(1.0), unitless(0.0));
	renderer.text(sU("text aligned 1.0,0.0 with the circle centre"), p3, unitless(1.0), unitless(0.0));

	sci::graphics::Point p4 = p1+ sci::graphics::Distance(millimetre(0.0), millimetre(10.0));
	renderer.rectangle(p4, size * unitless(2.), unitless(0.5), unitless(0.5));
	renderer.elipse(p4, size);
	renderer.text(sU("text aligned 0.0,0.5 with the circle centre"), p4, unitless(0.0), unitless(0.5));

	sci::graphics::Point p5 = p2 + sci::graphics::Distance(millimetre(0.0), millimetre(10.0));
	renderer.rectangle(p5, size * unitless(2.), unitless(0.5), unitless(0.5));
	renderer.elipse(p5, size);
	renderer.text(sU("text aligned 0.5,0.5 with the circle centre"), p5, unitless(0.5), unitless(0.5));

	sci::graphics::Point p6 = p3 + sci::graphics::Distance(millimetre(0.0), millimetre(10.0));
	renderer.rectangle(p6, size * unitless(2.), unitless(0.5), unitless(0.5));
	renderer.elipse(p6, size);
	renderer.text(sU("text aligned 1.0,0.5 with the circle centre"), p6, unitless(1.0), unitless(0.5));

	sci::graphics::Point p7 = p4 + sci::graphics::Distance(millimetre(0.0), millimetre(10.0));
	renderer.rectangle(p7, size * unitless(2.), unitless(0.5), unitless(0.5));
	renderer.elipse(p7, size);
	renderer.text(sU("text aligned 0.0,1.0 with the circle centre"), p7, unitless(0.0), unitless(1.0));

	sci::graphics::Point p8 = p5 + sci::graphics::Distance(millimetre(0.0), millimetre(10.0));
	renderer.rectangle(p8, size * unitless(2.), unitless(0.5), unitless(0.5));
	renderer.elipse(p8, size);
	renderer.text(sU("text aligned 0.5,1.0 with the circle centre"), p8, unitless(0.5), unitless(1.0));

	sci::graphics::Point p9 = p6 + sci::graphics::Distance(millimetre(0.0), millimetre(10.0));
	renderer.rectangle(p9, size * unitless(2.), unitless(0.5), unitless(0.5));
	renderer.elipse(p9, size);
	renderer.text(sU("text aligned 1.0,1.0 with the circle centre"), p9, unitless(1.0), unitless(1.0));

	sci::graphics::Point pr1 = p8 + sci::graphics::Distance(millimetre(0.0), millimetre(10.0));
	renderer.rotatedText(sU("Rotated with 0.0,0.0 alignment"), pr1, unitless(0.0), unitless(0.0), degree(0.0));
	renderer.rotatedText(sU("Rotated with 0.0,0.0 alignment"), pr1, unitless(0.0), unitless(0.0), degree(180.0));

	sci::graphics::Point pr2 = pr1 + sci::graphics::Distance(millimetre(0.0), millimetre(10.0));
	renderer.rotatedText(sU("Rotated with 0.5,0.0 alignment"), pr2, unitless(0.5), unitless(0.0), degree(0.0));
	renderer.rotatedText(sU("Rotated with 0.5,0.0 alignment"), pr2, unitless(0.5), unitless(0.0), degree(180.0));

	sci::graphics::Point pr3 = pr2 + sci::graphics::Distance(millimetre(0.0), millimetre(10.0));
	renderer.rotatedText(sU("Rotated with 0.0,0.0 alignment"), pr3, unitless(1.0), unitless(0.0), degree(0.0));
	renderer.rotatedText(sU("Rotated with 1.0,0.0 alignment"), pr3, unitless(1.0), unitless(0.0), degree(180.0));

	sci::graphics::Point pr4 = pr3 + sci::graphics::Distance(millimetre(0.0), millimetre(10.0));
	renderer.rotatedText(sU("Rotated with 0.0,0.5 alignment"), pr4, unitless(0.0), unitless(0.5), degree(0.0));
	renderer.rotatedText(sU("Rotated with 0.0,0.5 alignment"), pr4, unitless(0.0), unitless(0.5), degree(180.0));
	
	sci::graphics::Point pr5 = pr4 + sci::graphics::Distance(millimetre(0.0), millimetre(10.0));
	renderer.rotatedText(sU("Rotated with 0.0,1.0 alignment"), pr5, unitless(0.0), unitless(1.0), degree(0.0));
	renderer.rotatedText(sU("Rotated with 0.0,1.0 alignment"), pr5, unitless(0.0), unitless(1.0), degree(180.0));


	sci::graphics::Point pr6 = pr3 + sci::graphics::Distance(millimetre(0.0), millimetre(90.0));
	renderer.rectangle(pr6, size * unitless(2.), unitless(0.5), unitless(0.5));
	renderer.elipse(pr6, size);
	renderer.rotatedText(sU("Rotated with 0.5,0.5 alignment 000 deg       Rotated with 0.5,0.5 alignment 000 deg"), pr6, unitless(0.5), unitless(0.5), degree(0.0));
	renderer.rotatedText(sU("Rotated with 0.5,0.5 alignment 045 deg       Rotated with 0.5,0.5 alignment 045 deg"), pr6, unitless(0.5), unitless(0.5), degree(45.0));
	renderer.rotatedText(sU("Rotated with 0.5,0.5 alignment 090 deg       Rotated with 0.5,0.5 alignment 090 deg"), pr6, unitless(0.5), unitless(0.5), degree(90.0));
	renderer.rotatedText(sU("Rotated with 0.5,0.5 alignment 135 deg       Rotated with 0.5,0.5 alignment 135 deg"), pr6, unitless(0.5), unitless(0.5), degree(135.0));
	
	renderer.line(sci::graphics::Point(unitless(0.5), unitless(0.0)), sci::graphics::Point(unitless(0.5), unitless(1.0)));


	renderer.setFont(sci::string(sU("Arial") ), sci::graphics::Length(textPoint(40)), RgbColour(0, 0, 0));
	sci::graphics::TextMetric AlphabetSize = renderer.text(sU("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"),
		sci::graphics::Point(unitless(0.05), unitless(0.7)), unitless(0.0), unitless(0.0));
	renderer.rectangle(sci::graphics::Point(unitless(0.05), unitless(0.7)), boundingBox(AlphabetSize));
	renderer.setFont(sci::string(sU("Arial")), sci::graphics::Length(textPoint(40)), RgbColour(0, 0, 0));
	renderer.text(sU("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"),
		sci::graphics::Point(unitless(0.05), unitless(0.7)), unitless(0.0), unitless(0.0));

	sci::graphics::TextMetric xSize = renderer.formattedText(sU("M^2N^{3\\alpha}"), sci::graphics::Point(unitless(0.05), unitless(0.8)));
	renderer.rectangle(sci::graphics::Point(unitless(0.05), unitless(0.8)), boundingBox(xSize));
	renderer.formattedText(sU("M^2N^{3\\alpha}"), sci::graphics::Point(unitless(0.05), unitless(0.8)));

	renderer.formattedText(sU("M^2N^{3\\alpha}"), sci::graphics::Point(unitless(0.9), unitless(0.5)), unitless(0.0), unitless(0.0), degree(90.0));
	renderer.formattedText(sU("M^2N^{3\\alpha}"), sci::graphics::Point(unitless(0.9), unitless(0.5)), unitless(0.0), unitless(0.0), degree(225.0));
	renderer.elipse(sci::graphics::Point(unitless(0.9), unitless(0.5)), size);
}

void PlotAxisTestPanel::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	sci::graphics::wxRenderer renderer(&dc, GetClientSize(), perInch(FromDIP(96)));

	sci::string axisTitle = sU("Area (m^2)");


	sci::graphics::Point left(unitless(0.05), unitless(0.01));
	sci::graphics::Point right(unitless(0.45), unitless(0.01));
	sci::graphics::Point top(unitless(0.55), unitless(0.05));
	sci::graphics::Point bottom(unitless(0.55), unitless(0.45));
	sci::graphics::Distance voffset(unitless(0.0), unitless(0.125));
	sci::graphics::Distance hoffset(unitless(0.1), unitless(0.0));

	//linear horizontal axes

	sci::plot::Axis axis1(0.0, 10.0, false, left, right, sci::plot::Axis::Options(sU("0-10 with auto major interval of 2 and 3 subticks")));
	axis1.draw(renderer, perInch(96));

	sci::plot::Axis axis2(0.0, 20.0, false, left + voffset, right + voffset, sci::plot::Axis::Options(sU("0-20 with auto major interval of 2 and 3 subticks")));
	axis2.draw(renderer, perInch(96));

	sci::plot::Axis axis3(0.0, 50.0, false, left + voffset * unitless(2.0), right + voffset * unitless(2.0), sci::plot::Axis::Options(sU("0-50 with auto major interval of 5 and 4 subticks")));
	axis3.draw(renderer, perInch(96));

	sci::plot::Axis axis4(0.0, 80.0, false, left + voffset * unitless(3.0), right + voffset * unitless(3.0), sci::plot::Axis::Options(sU("0-80 with auto major interval of 10 and 4 subticks")));
	axis4.draw(renderer, perInch(96));

	//linear vertical axes

	sci::plot::Axis axis5(0.0, 10.0, false, bottom, top, sci::plot::Axis::Options(sU("1-10 with auto major interval of 2 and 3 subticks")));
	axis5.draw(renderer, perInch(96));

	sci::plot::Axis axis6(0.0, 20.0, false, bottom + hoffset * unitless(1.0), top + hoffset * unitless(1.0), sci::plot::Axis::Options(sU("0-20 with auto major interval of 2 and 3 subticks")));
	axis6.draw(renderer, perInch(96));

	sci::plot::Axis axis7(0.0, 50.0, false, bottom + hoffset * unitless(2.0), top + hoffset * unitless(2.0), sci::plot::Axis::Options(sU("0-50 with auto major interval of 5 and 4 subticks")));
	axis7.draw(renderer, perInch(96));

	sci::plot::Axis axis8(0.0, 80.0, false, bottom + hoffset * unitless(3.0), top + hoffset * unitless(3.0), sci::plot::Axis::Options(sU("0-80 with auto major interval of 10 and 4 subticks")));
	axis8.draw(renderer, perInch(96));

	//log axes

	sci::plot::Axis axis9(1.0, 10.0, true, left + voffset * unitless(4.0), right + voffset * unitless(4.0), sci::plot::Axis::Options(sU("1-10 log increments")));
	axis9.draw(renderer, perInch(96));

	sci::plot::Axis axis10(1.0, 100000.0, true, left + voffset * unitless(5.0), right + voffset * unitless(5.0), sci::plot::Axis::Options(sU("1-1\u00D710^{5} log increments (scientific from 10,000)")));
	axis10.draw(renderer, perInch(96));

	sci::plot::Axis axis11(1.0, 200.0, true, left + voffset * unitless(6.0), right + voffset * unitless(6.0), sci::plot::Axis::Options(sU("1-200 log increments")));
	axis11.draw(renderer, perInch(96));

	sci::plot::Axis::Options options12(sU("1-1\u00D710^{20} ticks on decades"));
	options12.m_majorInterval = 5;
	options12.m_nSubticks = 4;
	options12.m_autoMajorInterval = false;
	options12.m_autoNSubticks = false;
	sci::plot::Axis axis12(1.0, 1e20, true, left + voffset * unitless(7.0), right + voffset * unitless(7.0), options12);
	axis12.draw(renderer, perInch(96));



	sci::plot::Axis::Options options13;
	options13.m_lineStyle = sci::plot::LineStyle(millimetre(1), RgbColour(1.0, 0.3, 0.0));
	options13.m_title = sU("Diferent colours with Algerian font");
	options13.m_titleFont.m_facenames = { sci::string(sU("Algerian")) };
	options13.m_titleFont.m_colour = RgbColour(0.0, 0.0, 0.5);
	options13.m_labelFont.m_colour = RgbColour(0.5, 0.5, 0.5);
	sci::plot::Axis axis13(0.0, 20.0, false, bottom + voffset, bottom + (right - left) + voffset, options13);
	axis13.draw(renderer, perInch(96));
}

void PlotLineTestPanel::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	sci::graphics::wxRenderer renderer(&dc, GetClientSize(), perInch(FromDIP(96)));

	//bottom left plot regular axis directions
	std::shared_ptr<sci::plot::Axis> xAxis1(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(unitless(0.1), unitless(0.9)),
		sci::graphics::Point(unitless(0.45), unitless(0.9)), sci::plot::Axis::Options(sU("Normal x axis"))));
	std::shared_ptr<sci::plot::Axis> yAxis1(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(unitless(0.1), unitless(0.9)),
		sci::graphics::Point(unitless(0.1), unitless(0.55)), sci::plot::Axis::Options(sU("Normal y axis"))));
	
	//top left plot
	//reversed both x values and end sci::graphics::Points to give regular direction
	//reversed y end sci::graphics::Points to give reversed axis
	std::shared_ptr<sci::plot::Axis> xAxis2(new sci::plot::Axis(1.0, 0.0, false, sci::graphics::Point(unitless(0.45), unitless(0.1)),
		sci::graphics::Point(unitless(0.1), unitless(0.1)), sci::plot::Axis::Options(sU("Double reversed x axis"))));
	std::shared_ptr<sci::plot::Axis> yAxis2(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(unitless(0.1), unitless(0.1)),
		sci::graphics::Point(unitless(0.1), unitless(0.45)), sci::plot::Axis::Options(sU("Reversed position y axis"))));

	//bottom right plot
	//reversed x end sci::graphics::Points to give reversed direction
	//reversed both y values and end sci::graphics::Points to give regular direction
	std::shared_ptr<sci::plot::Axis> xAxis3(new sci::plot::Axis(0.0, 1.0, false, sci::graphics::Point(unitless(0.9), unitless(0.9)),
		sci::graphics::Point(unitless(0.55), unitless(0.9)), sci::plot::Axis::Options(sU("Reversed position x axis"))));
	std::shared_ptr<sci::plot::Axis> yAxis3(new sci::plot::Axis(1.0, 0.0, false, sci::graphics::Point(unitless(0.9), unitless(0.55)),
		sci::graphics::Point(unitless(0.9), unitless(0.9)), sci::plot::Axis::Options(sU("Double reversed y axis"))));

	//top right plot
	//reversed x values to give reversed direction
	//reversed y values to give reversed direction
	std::shared_ptr<sci::plot::Axis> xAxis4(new sci::plot::Axis(1.0, 0.0, false, sci::graphics::Point(unitless(0.55), unitless(0.1)),
		sci::graphics::Point(unitless(0.9), unitless(0.1)), sci::plot::Axis::Options(sU("Reversed values x axis"))));
	std::shared_ptr<sci::plot::Axis> yAxis4(new sci::plot::Axis(1.0, 0.0, false, sci::graphics::Point(unitless(0.9), unitless(0.45)),
		sci::graphics::Point(unitless(0.9), unitless(0.1)), sci::plot::Axis::Options(sU("Reversed values y axis"))));
	//yAxis->setticksdirection(true, false);

	xAxis1->draw(renderer, perInch(96));
	yAxis1->draw(renderer, perInch(96));

	xAxis2->draw(renderer, perInch(96));
	yAxis2->draw(renderer, perInch(96));

	xAxis3->draw(renderer, perInch(96));
	yAxis3->draw(renderer, perInch(96));

	xAxis4->draw(renderer, perInch(96));
	yAxis4->draw(renderer, perInch(96));

	std::vector<double> x{ 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
	std::vector<double> y{ 0.05, 0.23, 0.64, 0.73, 0.82, 0.68, 0.52, 0.18, 0.07 };
	std::vector<sci::graphics::Distance> squareSymbol{ sci::graphics::Distance(millimetre(-1.0), millimetre(-1.0)), sci::graphics::Distance(millimetre(1.0), millimetre(-1.0)),
	sci::graphics::Distance(millimetre(1.0), millimetre(1.0)), sci::graphics::Distance(millimetre(-1.0), millimetre(1.0)), sci::graphics::Distance(millimetre(-1.0), millimetre(-1.0)) };

	std::shared_ptr<sci::plot::Lines> lineData1(new sci::plot::Lines(x, y, xAxis1, yAxis1, sci::plot::LineStyle()));
	std::shared_ptr<sci::plot::Points> pointData1(new sci::plot::Points(x, y, xAxis1, yAxis1, sci::plot::Symbol(squareSymbol), RgbColour(0.5, 0.0, 0.8)));
	lineData1->draw(renderer, perInch(96));
	pointData1->draw(renderer, perInch(96));

	std::shared_ptr<sci::plot::Lines> lineData2(new sci::plot::Lines(x, y, xAxis2, yAxis2, sci::plot::LineStyle()));
	std::shared_ptr<sci::plot::Points> pointData2(new sci::plot::Points(x, y, xAxis2, yAxis2, sci::plot::Symbol(squareSymbol), RgbColour(0.5, 0.0, 0.8)));
	lineData2->draw(renderer, perInch(96));
	pointData2->draw(renderer, perInch(96));

	std::shared_ptr<sci::plot::Lines> lineData3(new sci::plot::Lines(x, y, xAxis3, yAxis3, sci::plot::LineStyle()));
	std::shared_ptr<sci::plot::Points> pointData3(new sci::plot::Points(x, y, xAxis3, yAxis3, sci::plot::Symbol(squareSymbol), RgbColour(0.5, 0.0, 0.8)));
	lineData3->draw(renderer, perInch(96));
	pointData3->draw(renderer, perInch(96));

	std::shared_ptr<sci::plot::Lines> lineData4(new sci::plot::Lines(x, y, xAxis4, yAxis4, sci::plot::LineStyle()));
	std::shared_ptr<sci::plot::Points> pointData4(new sci::plot::Points(x, y, xAxis4, yAxis4, sci::plot::Symbol(squareSymbol), RgbColour(0.5, 0.0, 0.8)));
	lineData4->draw(renderer, perInch(96));
	pointData4->draw(renderer, perInch(96));
}
