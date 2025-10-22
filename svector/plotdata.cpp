#include "../include/svector/dep/plot/plot.h"
#include "../include/svector/dep/plot/transforms.h"
#include "../include/svector/serr.h"
#include "../include/svector/svector.h"
#include "../include/svector/dep/plot/linesPoints.h"
#include "../include/svector/dep/plot/varyingPoints.h"
#include "../include/svector/dep/plot/fillBars.h"
#include "../include/svector/dep/plot/errorBar.h"
#include "../include/svector/dep/plot/gridContour.h"
#include<set>

void sci::plot::LineStyle::setupLineStyle( plstream *pl, PLINT colourIndex, double scale ) const
{
	pl->scol0a( colourIndex, m_colour.r() * 255, m_colour.g() * 255, m_colour.b() * 255, m_colour.a() );
	pl->col0( colourIndex );
	pl->width( m_width.getLength(grMillimetre(0.0), grMillimetre(0.0)).value<grMillimetre>() * scale );
	if(m_dashes.size()==0)
		pl->styl(0,NULL, NULL);
	else
	{
		std::vector<PLINT> scaledMarks( m_dashes.size() / 2 );
		std::vector<PLINT> scaledSpaces(m_dashes.size() / 2 );
		for(size_t i=0; i<scaledMarks.size(); ++i)
		{
			scaledMarks[i] = (PLINT)( m_dashes[i * 2].getLength(grMillimetre(0.0), grMillimetre(0.0)).value<grMillimetre>() * 1000.0 * scale );
			scaledSpaces[i] = (PLINT)( m_dashes[i * 2 + 1].getLength(grMillimetre(0.0), grMillimetre(0.0)).value<grMillimetre>() * 1000.0 * scale );
		}
		pl->styl(scaledMarks.size(),&scaledMarks[0],&scaledSpaces[0]);
	}
}
void sci::plot::LineStyle::resetLineStyle( plstream *pl, PLINT colourIndex ) const
{
	pl->scol0a( colourIndex, 0, 0, 0, 1.0 );
	pl->styl( 0, NULL, NULL );
}

void sci::plot::FillStyle::setupFillStyle( plstream *pl, PLINT colourIndex, double scale ) const
{
	if( m_lineSpacingMicrons[0] > 0 )
	{
		PLINT nSets = m_lineSpacingMicrons[1] > 0 ? 2 : 1;
		m_lineStyle.setupLineStyle( pl, colourIndex, scale );
		pl->pat( nSets, m_angleDeg, m_lineSpacingMicrons );
	}
	else
	{
		pl->scol0a( colourIndex, m_colour.r() * 255, m_colour.g() * 255, m_colour.b() * 255, m_colour.a() );
		pl->col0( colourIndex );
		pl->psty( 0 );
	}
}

void sci::plot::FillStyle::resetFillStyle(plstream *pl, PLINT colourIndex) const
{
	pl->scol0a(colourIndex, 0, 0, 0, 1.0);
	pl->psty(0);
}

void sci::plot::PlotFrame::draw(plstream* pl, double scale, double pageWidth, double pageHeight)
{
	pl->vpor(- 1e-5, 1.00005, -1e-5, 1.00005);
	pl->wind(-1e-5, 1.00005, -1e-5, 1.00005);
	double left = m_topLeft.getX(1.0, 1.0, grPerMillimetre(0.0));
	double top = 1.0 - m_topLeft.getY(1.0, 1.0, grPerMillimetre(0.0)); //plplot has positive y up, Renderer has positive y down
	double bottom = 1.0 - m_bottomRight.getY(1.0, 1.0, grPerMillimetre(0.0)); //plplot has positive y up, Renderer has positive y down
	double right = m_bottomRight.getX(1.0, 1.0, grPerMillimetre(0.0));
	double x[]{ left, right, right, left, left };
	double y[]{ bottom, bottom, top, top, bottom };

	if (m_fillStyle.getColour().a() > 0.0)
	{
		m_fillStyle.setupFillStyle(pl, 1, scale);
		pl->fill(5, x, y);
		m_fillStyle.resetFillStyle(pl, 1);
	}

	if (m_lineStyle.getWidth().getLength(grMillimetre(0.0), grMillimetre(0.0)) > grMillimetre(0.0))
	{
		m_lineStyle.setupLineStyle(pl, 1, scale);
		pl->line(5, x, y);
		m_lineStyle.resetLineStyle(pl, 1);
	}


	pl->vpor(left, right, bottom, top);
	pl->sfci(m_titlestyle);
	//pl->sfontf(m_titlefont.mb_str());
	pl->schr(1.0, m_titlesize.getLength(grMillimetre(0.0), grMillimetre(0.0)).value<grMillimetre>() * scale); //set text height in mm
	pl->scol0(0, m_titlecolour.r()*255, m_titlecolour.g()*255, m_titlecolour.b()*255);
	pl->col0(0);
	double distance = m_titledistance.getLength(grMillimetre(0), grMillimetre(0)).value<grMillimetre>() / m_titlesize.getLength(grMillimetre(0), grMillimetre(0)).value<grMillimetre>();
	pl->mtex("t", distance, 0.5, 0.5, sci::utf16ToUtf8(m_title).c_str());
}

void sci::plot::PlotableItem::draw(plstream* pl, double scale, double pageWidth, double pageHeight)
{

	//set the position of the plot area on the page
	Point xPositionStart;
	Point xPositionEnd;
	Point yPositionStart;
	Point yPositionEnd;
	m_xAxis->getPosition(xPositionStart, xPositionEnd);
	m_yAxis->getPosition(yPositionStart, yPositionEnd);
	pl->vpor(xPositionStart.getX(1.0, 1.0, grPerMillimetre(0.0)), xPositionEnd.getX(1.0, 1.0, grPerMillimetre(0.0)),
		1.0 - yPositionStart.getY(1.0, 1.0, grPerMillimetre(0.0)), 1.0 - yPositionEnd.getY(1.0, 1.0, grPerMillimetre(0.0))); //reversed y coordinate for plplot and Point

	//set the limits of the plot area in plot coordinates
	//set the limits of the plot area in terms of plot units
	//note we have to log the limits for a log axis
	double xmin = m_xAxis->getLinearOrLogMin();
	double xmax = m_xAxis->getLinearOrLogMax();
	double ymin = m_yAxis->getLinearOrLogMin();
	double ymax = m_yAxis->getLinearOrLogMax();
	
	pl->wind(xmin, xmax, ymin, ymax);

	//set the global transform
	pl->stransform((m_transformer ? splotTransform : NULL), (void*)(m_transformer.get()));

	//plot the data
	plotData(pl, scale);

	//reset the transform
	pl->stransform(NULL, NULL);
}

void sci::plot::Lines::plotData( plstream *pl, double scale) const
{
	if (!hasData())
		return;
	m_lineStyle.setupLineStyle( pl, 1, scale );
	const double* x = getPointer(0);
	const double* y = getPointer(1);
	
	pl->line( getNPoints(), x, y);
	m_lineStyle.resetLineStyle( pl, 1 );
}

void sci::plot::Points::plotData( plstream *pl, double scale) const
{
	if (!hasData())
		return;
	//m_symbol.setupSymbol( pl, 1, scale );
	sci::string symbol = m_symbol.getSymbol();
	const double* x = getPointer(0);
	const double* y = getPointer(1);
	if( symbol.length() > 0)
		//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
		pl->string(getNPoints(), x, y, sci::toUtf8(symbol).c_str());
}

void sci::plot::PointsColourVarying::plotData( plstream *pl, double scale) const
{
	if (!hasData())
		return;
	sci::string symbol = m_symbol.getSymbol();
	const double* x = getPointer(0);
	const double* y = getPointer(1);
	const double* z = getPointer(2);
	if( symbol.length() > 0)
	{
		const double *xi = x;
		const double *yi = y;
		const double *zi = z;
		const double *xEnd = x + getNPoints();
		for( ; xi != xEnd; ++xi, ++yi, ++zi )
		{
			//m_symbol.setupSymbol( pl, 1, *zi, scale, true);
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			pl->string(1,xi,yi,sci::toUtf8(symbol).c_str());
		}
	}
}

void sci::plot::PointsSizeVarying::plotData( plstream *pl, double scale) const
{
	if (!hasData())
		return;
	sci::string symbol = m_symbol.getSymbol();
	const double *x = getPointer(0);
	const double* y = getPointer(1);
	const double *z = getPointer(2);
	if( symbol.length() > 0)
	{
		const double *xi = x;
		const double *yi = y;
		const double *zi = z;
		const double *xEnd = x + getNPoints();
		for( ; xi != xEnd; ++xi, ++yi, ++zi )
		{
			//m_symbol.setupSymbol( pl, 1, *zi, scale, true );
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			//if(m_symbol.getSize(*zi, true) > 0.0)
				//pl->string(1,xi,yi,sci::toUtf8(symbol).c_str());
		}
	}
}

void sci::plot::PointsColourAndSizeVarying::plotData( plstream *pl, double scale) const
{
	if (!hasData())
		return;
	sci::string symbol = m_symbol.getSymbol();
	const double* x = getPointer(0);
	const double* y = getPointer(1);
	const double* zColour = getPointer(2);
	const double* zSize = getPointer(3);
	if( symbol.length() > 0)
	{
		const double *xi = x;
		const double *yi = y;
		const double *zColouri = zColour;
		const double *zSizei = zSize;
		const double *xEnd = x + getNPoints();
		for( ; xi != xEnd; ++xi, ++yi, ++zColouri, ++zSizei )
		{
			//m_symbol.setupSymbol( pl, 1, *zColouri, true, *zSizei, true, scale );
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			//if (m_symbol.getSize(*zSizei, true) > 0.0)
				//pl->string(1,xi,yi,sci::toUtf8(symbol).c_str());
		}
	}
}

void sci::plot::HorizontalErrorBars::plotData( plstream *pl, double scale) const
{
	if (!hasData())
		return;
	const double* y = getPointer(0);
	const double* xPlusErrors = getPointer(1);
	const double* xMinusErrors = getPointer(2);


	m_style.setupLineStyle( pl, 1, scale );	
	pl->errx( getNPoints(), xMinusErrors, xPlusErrors, y);
	m_style.resetLineStyle( pl, 1 );
}

void sci::plot::VerticalErrorBars::plotData( plstream *pl, double scale ) const
{
	if (!hasData())
		return;
	const double* x = getPointer(0);
	const double* yPlusErrors = getPointer(1);
	const double* yMinusErrors = getPointer(2);

	m_style.setupLineStyle( pl, 1, scale );	
	pl->erry( getNPoints(), x, yMinusErrors, yPlusErrors);
	m_style.resetLineStyle( pl, 1 );
}

void sci::plot::VerticalBars::plotData( plstream *pl, double scale) const
{
	if (!hasData())
		return;
	double x[4];
	double y[4];
	const double* xMinusWidth = getPointer(0);
	const double* yData = getPointer(1);
	const double* xPlusWidth = getPointer(1);

	double zero = isLog(1) ? m_zeroLineLogged : m_zeroLineLinear;
	for(size_t i=0; i<getNPoints(); ++i)
	{
		
		x[0] = x[1] = xMinusWidth[i];
		x[2] = x[3] = xPlusWidth[i];
		
		y[0] = y[3] = zero;
		y[1] = y[2] = yData[i];
		
		//do fill first
		m_fillStyle.setupFillStyle( pl, 1, scale );
		pl->fill(4,&x[0],&y[0]);
		
		//then the line
		m_lineStyle.setupLineStyle( pl, 1, scale );
		pl->line( 4, x, y );
	}
}

void sci::plot::Fill::plotData(plstream* pl, double scale) const
{
	if (!hasData())
		return;

	const double* x = getPointer(0);
	const double* y = getPointer(1);

	m_lineStyle.setupLineStyle(pl, 1, scale);
	pl->line(getNPoints(), x, y);
	m_lineStyle.resetLineStyle(pl, 1);

	m_fillStyle.setupFillStyle(pl, 1, scale);
	pl->fill(getNPoints(), x, y);

	m_fillStyle.resetFillStyle(pl, 1); //just so we don't have residually selected hatching patterns
}







template<class T>
struct is2d
{
	static const bool value = false;
};

template<class T>
struct is2d<T**>
{
	static const bool value = true;
};

template<class X, class Y, bool INTERPOLATE>
class GridTransform
{
public:
	GridTransform(X x, Y y)
	{
		m_x = x;
		m_y = y;
	}
	template<int N> //N isn't used in this version - included to enable function overloadining
	static inline constexpr double interpolate(double x, double y, const double **v)
	{
		size_t xi = size_t(x);
		size_t yi = size_t(y);
		if constexpr (INTERPOLATE)
		{
			x = x - double(xi);
			y = y - double(yi);
			//we check if we have exactly hit integers. This isn't to optimise
			//as it's probably slower, but to check we don't read past the end of the array
			if (x == 0 && y == 0)
				return (v[xi][yi]);
			else if (x == 0)
			{
				double v0 = v[xi][yi];
				double v1 = v[xi][yi + 1];
				return v0 * (1.0 - y) + v1 * y;
			}
			else if (y == 0)
			{
				double v0 = v[xi][yi];
				double v1 = v[xi + 1][yi];
				return v0 * (1.0 - x) + v1 * x;
			}
			else
			{
				double vx0y0 = v[size_t(x)][size_t(y)];
				double vx1y0 = v[size_t(x) + 1][size_t(y)];
				double vx0y1 = v[size_t(x)][size_t(y) + 1];
				double vx1y1 = v[size_t(x) + 1][size_t(y) + 1];
				return vx0y0 * (1.0 - x) * (1.0 - y) + vx1y0 * x * (1.0 - y) + vx0y1 * (1.0 - x) * y + vx1y1 * x * y;
			}
		}
		else
			return(v[xi][yi]);
	}
	template<int N> //0 for using x, 1 for using y. We do this so we don't have to check dimensionality in callback
	static inline constexpr double interpolate(double x, double y, const double *v)
	{
		double a;
		if constexpr (N == 0)
			a = x;
		else
			a = y;
		if constexpr (INTERPOLATE)
		{
			double v0 = v[size_t(a)];
			double v1 = v[size_t(a) + 1];
			a = a - std::floor(a);
			return v1 * a + v0*(1.0 - a);
		}
		else
			return v[size_t(a)];
	}
	static void callback(PLFLT x, PLFLT y, PLFLT_NC_SCALAR xp, PLFLT_NC_SCALAR yp, PLPointer data)
	{
		GridTransform<X, Y, INTERPOLATE>* gt = (GridTransform*)(data);
		*xp = GridTransform<X, Y, INTERPOLATE>::interpolate<0>(x, y, gt->m_x);
		*yp = GridTransform<X, Y, INTERPOLATE>::interpolate<1>(x, y, gt->m_y);
	}

private:
	X m_x;
	Y m_y;
};

void sci::plot::Grid::plotData(plstream* pl, double scale) const
{
	if (!StructuredData::hasData())
		return;


	//set up the colourscale
	m_colourscale->setupForImage(pl);

	//get the limits of the clourscale
	double colourscaleMin = m_colourscale->getLinearOrLogMin();
	double colourscaleMax = m_colourscale->getLinearOrLogMax();

	//either limit the data we plot to within the colourscale or off the limits as appropriate
	double zMin = colourscaleMin;
	double zMax = colourscaleMax;
	if (m_colourscale->fillOffscaleBottom())
		zMin = -std::numeric_limits<double>::infinity();
	if (m_colourscale->fillOffscaleTop())
		zMax = std::numeric_limits<double>::infinity();


	if (m_x1d && m_y1d)
	{
		const double* x = UnstructuredData::getPointer(0);
		const double* y = UnstructuredData::getPointer(1);
		std::vector<const double*> zs = StructuredData::getPointer(0);
		using GT = GridTransform<const double*, const double*, false>;

		GT transform(x, y);

		pl->imagefr(&zs[0], m_xNBoxes, m_yNBoxes, std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
			zMin, zMax, colourscaleMin, colourscaleMax, GT::callback, (void*)&transform);
	}
	else if (m_x1d)
	{
		const double* x = UnstructuredData::getPointer(0);
		std::vector<const double*> y = StructuredData::getPointer(0);
		std::vector<const double*> zs = StructuredData::getPointer(1);
		using GT = GridTransform<const double *, const double **, false>;

		GT transform(x, &y[0]);

		pl->imagefr(&zs[0], m_xNBoxes, m_yNBoxes, std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
			zMin, zMax, colourscaleMin, colourscaleMax, GT::callback, (void*)&transform);
	}
	else if (m_y1d)
	{
		std::vector<const double*> x = StructuredData::getPointer(0);
		const double* y = UnstructuredData::getPointer(0);
		std::vector<const double*> zs = StructuredData::getPointer(1);
		using GT = GridTransform<const double**, const double*, false>;

		GT transform(&x[0], y);

		pl->imagefr(&zs[0], m_xNBoxes, m_yNBoxes, std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
			zMin, zMax, colourscaleMin, colourscaleMax, GT::callback, (void*)&transform);
	}
	else
	{
		std::vector<const double*> x = StructuredData::getPointer(0);
		std::vector<const double*> y = StructuredData::getPointer(1);
		std::vector<const double*> zs = StructuredData::getPointer(2);
		using GT = GridTransform<const double**, const double**, false>;

		GT transform(&x[0], &y[0]);

		pl->imagefr(&zs[0], m_xNBoxes, m_yNBoxes, std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
			zMin, zMax, colourscaleMin, colourscaleMax, GT::callback, (void*)&transform);
	}
}

void sci::plot::Contours::plotData(plstream* pl, double scale) const
{
	if (!StructuredData::hasData())
		return;


	//grab the levels from the colourscale
	std::vector<double> shadeLevels;
	if (m_colourscale)
	{
		//set up the colourscale
		m_colourscale->setupForShade(pl);
		shadeLevels = m_colourscale->getDiscreteValues();
		if (m_colourscale->isLog())
			for (auto& s : shadeLevels)
				s = log10(s);
		sci::assertThrow(shadeLevels.size() > 1, sci::err(sci::SERR_PLOT, 0, "ContourData::plotData: Cannot use a colourscale with fewer than 2 levels."));
		//if (m_colourscale->fillOffscaleBottom())
		//	shadeLevels.front() = -std::numeric_limits<double>::infinity();
		//if (m_colourscale->fillOffscaleTop())
		//	shadeLevels.back() = std::numeric_limits<double>::infinity();
	}

	//grab the contour levels from the levelScale
	sci::GridData<double, 1> contourLevels;
	if (m_levelScale)
	{
		contourLevels = m_levelScale->getLevels();
		if (m_levelScale->isLog())
			for (auto& c : contourLevels)
				c = log10(c);
		sci::assertThrow(contourLevels.size() > 1, sci::err(sci::SERR_PLOT, 0, "ContourData::plotData: Cannot use a level scale with fewer than 2 levels."));
	}

	//set up the minimum z we want to plot - this will depend if we want to plot off the bottom of the colourscale
	//double zMin = m_fillOffscaleBottom ? -std::numeric_limits<double>::infinity() : m_colourscale->getMin();
	//double zMax = m_fillOffscaleTop ? std::numeric_limits<double>::infinity() : m_colourscale->getMax();

	//set the line style for the contours
	m_lineStyle.setupLineStyle(pl, 1, scale);
	double lineWidth = m_lineStyle.getWidth().getLength(grMillimetre(0.0), grMillimetre(0.0)).value<grMillimetre>();

	if (m_x1d && m_y1d)
	{
		const double* x = UnstructuredData::getPointer(0);
		const double* y = UnstructuredData::getPointer(1);
		std::vector<const double*> zs = StructuredData::getPointer(0);
		using GT = GridTransform<const double*, const double*, true>;

		GT transform(x, y);

		if(m_colourscale)
			pl->shades(&zs[0], m_xNBoxes, m_yNBoxes, NULL, std::numeric_limits<double>::quiet_NaN(),
				std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
				&shadeLevels[0], shadeLevels.size(), 1, 1, lineWidth * scale, plfill, true, GT::callback, (void*)(&transform));
		if (m_levelScale)
			pl->cont(&zs[0], m_xNBoxes, m_yNBoxes, 1, m_xNBoxes, 1, m_yNBoxes, &contourLevels[0], contourLevels.size(), GT::callback, (void*)(&transform));
	}
	else if (m_x1d)
	{
		const double* x = UnstructuredData::getPointer(0);
		std::vector<const double*> y = StructuredData::getPointer(0);
		std::vector<const double*> zs = StructuredData::getPointer(1);
		using GT = GridTransform<const double*, const double**, true>;

		GT transform(x, &y[0]);

		if (m_colourscale)
			pl->shades(&zs[0], m_xNBoxes, m_yNBoxes, NULL, std::numeric_limits<double>::quiet_NaN(),
				std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
				&shadeLevels[0], shadeLevels.size(), 1, 1, lineWidth * scale, plfill, true, GT::callback, (void*)(&transform));
		if (m_levelScale)
			pl->cont(&zs[0], m_xNBoxes, m_yNBoxes, 1, m_xNBoxes, 1, m_yNBoxes, &contourLevels[0], contourLevels.size(), GT::callback, (void*)(&transform));
	}
	else if (m_y1d)
	{
		std::vector<const double*> x = StructuredData::getPointer(0);
		const double* y = UnstructuredData::getPointer(0);
		std::vector<const double*> zs = StructuredData::getPointer(1);
		using GT = GridTransform<const double**, const double*, true>;

		GT transform(&x[0], y);

		if (m_colourscale)
			pl->shades(&zs[0], m_xNBoxes, m_yNBoxes, NULL, std::numeric_limits<double>::quiet_NaN(),
				std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
				&shadeLevels[0], shadeLevels.size(), 1, 1, lineWidth * scale, plfill, true, GT::callback, (void*)(&transform));
		if (m_levelScale)
			pl->cont(&zs[0], m_xNBoxes, m_yNBoxes, 1, m_xNBoxes, 1, m_yNBoxes, &contourLevels[0], contourLevels.size(), GT::callback, (void*)(&transform));
	}
	else
	{
		std::vector<const double*> x = StructuredData::getPointer(0);
		std::vector<const double*> y = StructuredData::getPointer(1);
		std::vector<const double*> zs = StructuredData::getPointer(2);
		using GT = GridTransform<const double**, const double**, true>;

		GT transform(&x[0], &y[0]);

		if (m_colourscale)
			pl->shades(&zs[0], m_xNBoxes, m_yNBoxes, NULL, std::numeric_limits<double>::quiet_NaN(),
				std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
				&shadeLevels[0], shadeLevels.size(), 1, 1, lineWidth * scale, plfill, true, GT::callback, (void*)(&transform));
		if (m_levelScale)
			pl->cont(&zs[0], m_xNBoxes, m_yNBoxes, 1, m_xNBoxes, 1, m_yNBoxes, &contourLevels[0], contourLevels.size(), GT::callback, (void*)(&transform));
	}
}

