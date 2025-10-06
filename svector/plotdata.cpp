#include "../include/svector/dep/plot/plotdata.h"
#include "../include/svector/dep/plot/transforms.h"
#include "../include/svector/serr.h"
#include "../include/svector/svector.h"
#include<set>


LineStyle::LineStyle( Length width, const rgbcolour &colour, const std::vector<Length> &dashes)
	: m_width( width ), m_colour( colour ), m_dashes( dashes )
{
}
LineStyle::LineStyle( Length width, const rgbcolour &colour, sci::string pattern )
	: m_width( width ), m_colour( colour )
{
	parseLineStyle( pattern, width, m_dashes);
}

Length LineStyle::getWidth() const
{
	return m_width;
}
void LineStyle::getPattern( std::vector<Length> &dashes ) const
{
	dashes = m_dashes;
}
rgbcolour LineStyle::getColour() const
{
	return m_colour;
}
void LineStyle::setupLineStyle( plstream *pl, PLINT colourIndex, double scale ) const
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
void LineStyle::resetLineStyle( plstream *pl, PLINT colourIndex ) const
{
	pl->scol0a( colourIndex, 0, 0, 0, 1.0 );
	pl->styl( 0, NULL, NULL );
}

//Converts a series of characters into dots/dashes and spaces.
//Here a space represents a gap of 0.5m, a tab 1.5 mm and for dashes a . is 0.5 mm, a - or a _ are 1.5 mm.
//Adjacent space or dash characters are summed, so a 2 mm gap followed by a 2 mm dash could be "    ...."
//or " \t._"
void LineStyle::parseLineStyle(const sci::string &pattern, Length lineWidth, std::vector<Length> &dashes)
{
	//set outputs to zero size
	dashes.resize(0);

	//return empty vectors if style is empty
	if(pattern.length()==0)
		return;


	//set up to start on a mark
	bool onmark=true;
	if (pattern[0] == ' ' || pattern[0] == '\t')
		dashes.push_back(grMillimetre(0.0));

	//initialise our current lengths to zero
	Length marklength = grMillimetre(0.0);
	Length spacelength = grMillimetre(0.0);

	//work through each character of style
	for(size_t i=0; i<pattern.length(); ++i)
	{
		//if we have changed between a space and mark record the length
		if(onmark==true && (pattern[i]==' ' || pattern[i]=='\t'))
		{
			dashes.push_back(marklength);
			marklength=grMillimetre(0.0);
			onmark=false;
		}
		else if(onmark==false && ( pattern[i]=='_' || pattern[i]=='.' || pattern[i]=='-' ) )
		{
			dashes.push_back(spacelength);
			spacelength=grMillimetre(0.0);
			onmark=true;
		}
		//add the current character to the current length
		if(pattern[i]==' ')
			spacelength+= lineWidth * grUnitless(0.5);
		else if(pattern[i]=='\t')
			spacelength+= lineWidth * grUnitless(2.0);
		else if(pattern[i]=='.')
			marklength+= lineWidth * grUnitless(0.5);
		else if(pattern[i]=='_')
			marklength+= lineWidth * grUnitless(2.0);
		else if(pattern[i]=='-')
			marklength+= lineWidth * grUnitless(2.0);
	}
	//add the last mark or space
	if(onmark==true)
		dashes.push_back(marklength);
	else
		dashes.push_back(spacelength);

	if (dashes.size() == 0)
		return;

	//if we ended on a mark merge it with the first mark and remove it
	if(onmark)
	{
		dashes[0] += dashes.back();
		dashes.pop_back();
	}
}

FillStyle::FillStyle( const rgbcolour &colour )
{
	m_colour = colour;
	m_lineSpacingMicrons[0] = 0;
	m_angleDeg[0] = 0;
	m_lineSpacingMicrons[1] = 0;
	m_angleDeg[1] = 0;
}

FillStyle::FillStyle( const LineStyle &lineStyle, double lineSpacingMicrons, double angleDeg )
{
	m_lineStyle = lineStyle;
	m_lineSpacingMicrons[0] = (PLINT)sci::round( lineSpacingMicrons );
	m_angleDeg[0] = (PLINT)sci::round( angleDeg * 10.0 );
	m_lineSpacingMicrons[1] = 0;
	m_angleDeg[1] = 0;
}

FillStyle::FillStyle( const LineStyle &lineStyle, double lineSpacing1Microns, double angle1Deg, double lineSpacing2Microns, double angle2Deg )
{
	m_lineStyle = lineStyle;
	m_lineSpacingMicrons[0] = (PLINT)sci::round( lineSpacing1Microns );
	m_angleDeg[0] = (PLINT)sci::round( angle1Deg * 10.0 );
	m_lineSpacingMicrons[1] = (PLINT)sci::round( lineSpacing2Microns );
	m_angleDeg[1] = (PLINT)sci::round( angle2Deg * 10.0 );
}

void FillStyle::setupFillStyle( plstream *pl, PLINT colourIndex, double scale ) const
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

void FillStyle::resetFillStyle(plstream *pl, PLINT colourIndex) const
{
	pl->scol0a(colourIndex, 0, 0, 0, 1.0);
	pl->psty(0);
}

rgbcolour FillStyle::getColour() const
{
	if( m_lineSpacingMicrons[0] > 0 )
		return m_lineStyle.getColour();
	return m_colour;
}

PlotFrame::PlotFrame(const Point topLeft, const Point bottomRight, const FillStyle& fillStyle, const LineStyle& lineStyle, sci::string title,
	Length titlesize, Length titledistance, sci::string titlefont, int32_t titlestyle, rgbcolour titlecolour)
{
	m_topLeft = topLeft;
	m_bottomRight = bottomRight;
	m_fillStyle = fillStyle;
	m_lineStyle = lineStyle;
	m_title = title;
	m_titlesize = titlesize;
	m_titledistance = titledistance;
	m_titlefont = titlefont;
	m_titlestyle = titlestyle;
	m_titlecolour = titlecolour;
}

void PlotFrame::draw(plstream* pl, double scale, double pageWidth, double pageHeight)
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

void PlotFrame::draw(Renderer& renderer, grPerMillimetre scale)
{
	sci::graphics::StatePusher state(&renderer);
	m_fillStyle.setBrush(renderer);
	m_lineStyle.setPen(renderer);
	renderer.rectangle(m_topLeft, m_bottomRight - m_topLeft);

	Point titlePosition(m_topLeft.getX() + (m_bottomRight.getX() - m_topLeft.getX()) / grUnitless(2), m_topLeft.getY() - m_titledistance);

	renderer.setFont(m_titlefont, m_titlesize, m_titlecolour);
	renderer.formattedText(m_title, titlePosition, grUnitless(0.5), grUnitless(0.0));
}

SymbolBase::SymbolBase( sci::string symbol, PLUNICODE fci )
{
	m_symbolText = symbol;
	m_fci = fci;
}

SymbolBase::SymbolBase(const std::vector<Distance>& symbol)
	:m_symbol(symbol)
{
	m_symbolText = sU(" ");
	m_fci = 0;
}

sci::string SymbolBase::getSymbol() const
{
	return m_symbolText;
}

PLUNICODE SymbolBase::getFci() const
{
	return m_fci;
}

void SymbolBase::draw(const Point& point, Renderer& renderer) const
{
	std::vector<Point> points(m_symbol.size());
	for (size_t i = 0; i < m_symbol.size(); ++i)
		points[i] = point + m_symbol[i];
	renderer.polygon(points);
}

Symbol::Symbol()
	:SymbolBase(std::vector<Distance>{
		Distance(grMillimetre(-1.0), grMillimetre(-1.0)),
		Distance(grMillimetre(1.0), grMillimetre(-1.0)),
		Distance(grMillimetre(1.0), grMillimetre(1.0)),
		Distance(grMillimetre(-1.0), grMillimetre(1.0)),
		Distance(grMillimetre(-1.0), grMillimetre(-1.0)) })
{
	m_size = 1.0;
	m_colour = rgbcolour(0, 0, 0, 1.0);
}


Symbol::Symbol(sci::string symbol, double size, rgbcolour colour)
	: SymbolBase(symbol, 0)
{
	m_size = size;
	m_colour = colour;
}

Symbol::Symbol(const std::span<Distance> &symbol, rgbcolour colour)
	: SymbolBase( std::vector<Distance>(std::begin(symbol), std::end(symbol)))
{
	m_size = 1.0;
	m_colour = colour;
}

void Symbol::setupSymbol( plstream *pl, PLINT colourIndex, double scale ) const
{
	pl->sfci( getFci() );
	//pl->sfontf(m_pointfont[i].mb_str());
	pl->schr( m_size, scale );
	pl->scol0a( colourIndex, m_colour.r() * 255, m_colour.g() * 255, m_colour.b() * 255, m_colour.a() );
	pl->col0( colourIndex );
}

double Symbol::getSize() const
{
	return m_size;
}

rgbcolour Symbol::getColour() const
{
	return m_colour;
}

VaryingSymbol::VaryingSymbol ( sci::string symbol )
	: SymbolBase( symbol, 0 )
{
}

ColourVaryingSymbol::ColourVaryingSymbol ( std::shared_ptr<splotcolourscale> colourScale, sci::string symbol, double size )
	:VaryingSymbol( symbol )
{
	m_size = size;
	m_colourScale = colourScale;
}

//we must call this prior to each individual symbol being plotted
void ColourVaryingSymbol::setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool parameterPreLogged, double scale ) const
{
	pl->sfci( getFci() );
	//pl->sfontf(m_pointfont[i].mb_str());
	pl->schr( m_size, scale );
	rgbcolour colour = m_colourScale->getRgbOriginalScale(parameter, parameterPreLogged);
	pl->scol0a( colourIndex, colour.r() * 255, colour.g() * 255, colour.b() * 255, colour.a() );
	pl->col0( colourIndex );
}

bool ColourVaryingSymbol::isLogScaled() const
{
	return m_colourScale->isLog();
}

SizeVaryingSymbol::SizeVaryingSymbol (std::shared_ptr<splotsizescale> sizeScale, sci::string symbol, rgbcolour colour)
	:VaryingSymbol( symbol )
{
	m_sizeScale = sizeScale;
	m_colour = colour;
}

//we must call this prior to each individual symbol being plotted
void SizeVaryingSymbol::setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool parameterPreLogged, double scale ) const
{
	pl->sfci( getFci() );
	//pl->sfontf(m_pointfont[i].mb_str());
	double size = m_sizeScale->getsize(parameter, parameterPreLogged);
	pl->schr( size, scale );
	pl->scol0a( colourIndex, m_colour.r() * 255, m_colour.g() * 255, m_colour.b() * 255, m_colour.a() );
	pl->col0( colourIndex );
}

bool SizeVaryingSymbol::isLogScaled() const
{
	return m_sizeScale->isLog();
}

double SizeVaryingSymbol::getSize(double parameter, bool parameterPreLogged) const
{
	return m_sizeScale->getsize(parameter, parameterPreLogged);
}

ColourAndSizeVaryingSymbol::ColourAndSizeVaryingSymbol ( std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotsizescale> sizeScale, sci::string symbol )
	:SymbolBase( symbol, 0 )
{
	m_colourScale = colourScale;
	m_sizeScale = sizeScale;
}

//we must call this prior to each individual symbol being plotted
void ColourAndSizeVaryingSymbol::setupSymbol( plstream *pl, PLINT colourIndex, double colourParameter, bool colourParameterPreLogged, double sizeParameter, bool sizeParameterPreLogged, double scale ) const
{
	pl->sfci( getFci() );
	//pl->sfontf(m_pointfont[i].mb_str());
	double size = m_sizeScale->getsize( sizeParameter, sizeParameterPreLogged );
	pl->schr( size, scale );
	rgbcolour colour = m_colourScale->getRgbOriginalScale( colourParameter, colourParameterPreLogged );
	pl->scol0a( colourIndex, colour.r() * 255, colour.g() * 255, colour.b() * 255, colour.a() );
	pl->col0( colourIndex );
}
bool ColourAndSizeVaryingSymbol::isColourLogScaled() const
{
	return m_colourScale->isLog();
}
bool ColourAndSizeVaryingSymbol::isSizeLogScaled() const
{
	return m_sizeScale->isLog();
}
double ColourAndSizeVaryingSymbol::getSize(double parameter, bool parameterPreLogged) const
{
	return m_sizeScale->getsize(parameter, parameterPreLogged);
}

void PlotableItem::draw(plstream* pl, double scale, double pageWidth, double pageHeight)
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

void PlotableItem::draw(Renderer& renderer, grPerMillimetre scale)
{
	Point endCorner(m_xAxis->getEnd().getX(), m_yAxis->getEnd().getY());
	renderer.setClippingRegion(m_intersection, endCorner);

	//plot the data
	plotData(renderer, scale);
}

UnstructuredData::UnstructuredData(const std::vector<std::span<const double>>& data, std::vector<std::shared_ptr<PlotScale>> axes, std::shared_ptr<splotTransformer> transformer)
{
	sci::assertThrow(data.size() == axes.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "UnstructuredData constructor called with data and axes of different lengths."));
	m_data.resize(data.size());
	for (size_t i = 0; i < data.size(); ++i)
	{
		m_data[i] = std::vector<double>(data[i].begin(), data[i].end());
		//if(i>0)
		//	sci::assertThrow(data[0]->size() == data[i]->size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "UnstructuredData constructor called with data in different dimensions having different lengths."));
	}
	m_dataLogged.resize(m_data.size());
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_dataLogged[i].resize(m_data[i].size());
			for(size_t j=0; j< m_dataLogged[i].size(); ++j)
				m_dataLogged[i][j] = sci::log10(m_data[i][j]);
	}
	//m_dataLogged = std::vector<std::vector<double>>(m_data.size());
	//for (size_t i = 0; i < m_dataLogged.size(); ++i)
	//{
	//	m_dataLogged[i] = std::vector<double>(m_data[i].size());
	//	for (size_t j = 0; j < m_dataLogged[i].size(); ++j)
	//		m_dataLogged[i][j] = std::max(std::log10(m_data[i][j]), std::numeric_limits<double>::min()); //don't let logged data get to -infinity, it causes problems in plplot
	//}

	m_axes = axes;
}

void UnstructuredData::autoscaleAxes()
{
	for (size_t i = 0; i < m_data.size(); ++i)
		if(m_axes[i])
			m_axes[i]->expand(m_data[i]);
}

void StructuredData::autoscaleAxes()
{
	for (size_t i = 0; i < m_data.size(); ++i)
		for (auto d : m_data[i])
			if(m_axes[i])
				m_axes[i]->expand(d);
}

StructuredData::StructuredData(const std::vector<const sci::GridData<double, 2>*>& data, std::vector<std::shared_ptr<PlotScale>> axes, std::shared_ptr<splotTransformer> transformer)
{

	sci::assertThrow(data.size() == axes.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "StructuredData constructor called with data and axes of different lengths."));
	m_data.resize(data.size());
	m_dataLogged.resize(data.size());
	for (size_t i = 0; i < data.size(); ++i)
	{
		m_data[i] = *(data[i]);
		m_dataLogged[i] = sci::log10(m_data[i]);	
	}
	m_axes = axes;
}

std::vector<const double*> StructuredData::getPointer(size_t dimension) const
{
	std::vector<const double*> result(m_data[dimension].shape()[0]);
	if (m_axes[dimension]->isLog())
		for (size_t i = 0; i < result.size(); ++i)
			result[i] = &m_dataLogged[dimension][i][0];
	else
		for (size_t i = 0; i < result.size(); ++i)
			result[i] = &m_data[dimension][i][0];

	return result;
}

LineData::LineData(std::span<const double> xs, std::span<const double> ys, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const LineStyle &lineStyle, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys }, {xAxis, yAxis}, transformer), m_lineStyle(lineStyle)
{
}

void LineData::plotData( plstream *pl, double scale) const
{
	if (!hasData())
		return;
	m_lineStyle.setupLineStyle( pl, 1, scale );
	const double* x = getPointer(0);
	const double* y = getPointer(1);
	
	pl->line( getNPoints(), x, y);
	m_lineStyle.resetLineStyle( pl, 1 );
}

void LineData::plotData(Renderer& renderer, grPerMillimetre scale) const
{
	if (!hasData())
		return;
	m_lineStyle.setPen(renderer);
	std::vector<Point> points(getNPoints());
	for (size_t i = 0; i < points.size(); ++i)
	{
		points[i] = getPoint(getPointer(0)[i], getPointer(1)[i]);
	}
	renderer.polyLine(points);
}

PointData::PointData(std::span<const double> x, std::span<const double> y, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const Symbol &symbol, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ x, y }, {xAxis, yAxis}, transformer), m_symbol(symbol)
{
}
void PointData::plotData( plstream *pl, double scale) const
{
	if (!hasData())
		return;
	m_symbol.setupSymbol( pl, 1, scale );
	sci::string symbol = m_symbol.getSymbol();
	const double* x = getPointer(0);
	const double* y = getPointer(1);
	if( symbol.length() > 0)
		//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
		pl->string(getNPoints(), x, y, sci::toUtf8(symbol).c_str());
}

void PointData::plotData(Renderer& renderer, grPerMillimetre scale) const
{
	if (!hasData())
		return;

	renderer.setBrush(m_symbol.getColour());
	renderer.setPen(m_symbol.getColour(), grMillimetre(0.5));

	for (size_t i = 0; i < getNPoints(); ++i)
	{
		m_symbol.draw(getPoint(getPointer(0)[i], getPointer(1)[i]), renderer);
	}
}

PointDataColourVarying::PointDataColourVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const ColourVaryingSymbol &symbol, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys, zs }, std::vector<std::shared_ptr<PlotScale>>{xAxis, yAxis, std::shared_ptr<PlotScale>(symbol.getColourscale())}, transformer), m_symbol(symbol)
{
}

void PointDataColourVarying::plotData( plstream *pl, double scale) const
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
			m_symbol.setupSymbol( pl, 1, *zi, scale, true);
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			pl->string(1,xi,yi,sci::toUtf8(symbol).c_str());
		}
	}
}

PointDataSizeVarying::PointDataSizeVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const SizeVaryingSymbol &symbol, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys, zs }, std::vector<std::shared_ptr<PlotScale>>{xAxis, yAxis, std::shared_ptr<PlotScale>(symbol.getSizeScale())}, transformer), m_symbol(symbol)
{
}

void PointDataSizeVarying::plotData( plstream *pl, double scale) const
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
			m_symbol.setupSymbol( pl, 1, *zi, scale, true );
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			if(m_symbol.getSize(*zi, true) > 0.0)
				pl->string(1,xi,yi,sci::toUtf8(symbol).c_str());
		}
	}
}

PointDataColourAndSizeVarying::PointDataColourAndSizeVarying(std::span<const double> xs, std::span<const double> ys, std::span<const double> zsColour, std::span<const double> zsSize, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const ColourAndSizeVaryingSymbol &symbol, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys, zsColour, zsSize }, std::vector<std::shared_ptr<PlotScale>>{xAxis, yAxis, std::shared_ptr<PlotScale>(symbol.getColourscale()), std::shared_ptr<PlotScale>(symbol.getSizeScale())}, transformer), m_symbol(symbol)
{
}
void PointDataColourAndSizeVarying::plotData( plstream *pl, double scale) const
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
			m_symbol.setupSymbol( pl, 1, *zColouri, true, *zSizei, true, scale );
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			if (m_symbol.getSize(*zSizei, true) > 0.0)
				pl->string(1,xi,yi,sci::toUtf8(symbol).c_str());
		}
	}
}

HorizontalErrorBars::HorizontalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const LineStyle style, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ ys,
		sci::GridData<double, 1>(sci::GridData<double, 1>(xs.begin(), xs.end()) + sci::GridData<double, 1>(plusErrors.begin(), plusErrors.end())),
		sci::GridData<double, 1>(sci::GridData<double, 1>(xs.begin(), xs.end()) - sci::GridData<double, 1>(minusErrors.begin(), minusErrors.end())) },
		{yAxis, xAxis, xAxis}, transformer)
{
	//a note on the above - the result of xs+plusError and xs-minusError is an r-value, meaning we
	//can't directly take it's address. However, when we assign it to a const reference the temporary's
	//lifetime is extended to the lifetime of the reference, so then we can legally get it's address.
	m_style = style;
}
void HorizontalErrorBars::plotData( plstream *pl, double scale) const
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

VerticalErrorBars::VerticalErrorBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const LineStyle style, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs,
		sci::GridData<double, 1>(sci::GridData<double, 1>(ys.begin(), ys.end()) + sci::GridData<double, 1>(plusErrors.begin(), plusErrors.end())),
		sci::GridData<double, 1>(sci::GridData<double, 1>(ys.begin(), ys.end()) - sci::GridData<double, 1>(minusErrors.begin(), minusErrors.end())) },
		{xAxis, yAxis, yAxis}, transformer)
{
	//a note on the above - the result of ys+plusError and ys-minusError is an r-value, meaning we
	//can't directly take it's address. However, when we assign it to a const reference the temporary's
	//lifetime is extended to the lifetime of the reference, so then we can legally get it's address.
	m_style = style;
}
void VerticalErrorBars::plotData( plstream *pl, double scale ) const
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

VerticalBars::VerticalBars(std::span<const double> xs, std::span<const double> ys, std::span<const double> widths, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const LineStyle &lineStyle, const FillStyle &fillStyle, double zeroLine, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer),
	UnstructuredData({ sci::GridData<double,1>(sci::GridData<double,1>(xs.begin(), xs.end()) - 0.5 * sci::GridData<double,1>(widths.begin(), widths.end())),
		ys,
		sci::GridData<double,1>(sci::GridData<double,1>(xs.begin(), xs.end()) + 0.5 * sci::GridData<double,1>(widths.begin(), widths.end())) },
		{xAxis, yAxis, xAxis}, transformer)
{
	//a note on the above - the result of xs-0.5*widths and xs+0.5*widths is an r-value, meaning we
	//can't directly take it's address. However, when we assign it to a const reference the temporary's
	//lifetime is extended to the lifetime of the reference, so then we can legally get it's address. 
	m_lineStyle = lineStyle;
	m_fillStyle = fillStyle;
	m_zeroLine = zeroLine;
	m_zeroLineLogged = zeroLine > 0.0 ? std::log( m_zeroLine ) : std::numeric_limits<double>::quiet_NaN();
}
void VerticalBars::plotData( plstream *pl, double scale) const
{
	if (!hasData())
		return;
	double x[4];
	double y[4];
	const double* xMinusWidth = getPointer(0);
	const double* yData = getPointer(1);
	const double* xPlusWidth = getPointer(1);

	double zero = isLog(1) ? std::log10(m_zeroLine) : m_zeroLine;
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

FillData::FillData(const std::vector<double>& xs, const std::vector<double>& ys, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, const FillStyle& fillStyle, const LineStyle& outlineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys }, { xAxis, yAxis }, transformer), m_fillStyle(fillStyle), m_lineStyle(outlineStyle)
{

}

void FillData::plotData(plstream* pl, double scale) const
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

void FillData::plotData(Renderer& renderer, grPerMillimetre scale) const
{
	if (!hasData())
		return;
	m_lineStyle.setPen(renderer);
	m_fillStyle.setBrush(renderer);

	std::vector<Point> points(getNPoints());
	for (size_t i = 0; i < points.size(); ++i)
	{
		points[i] = getPoint(getPointer(0)[i], getPointer(1)[i]);
	}

	renderer.polygon(points);
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

Data2d::Data2d(std::span<const double> xs, std::span<const double> ys, const sci::GridData<double, 2> &zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys }, { xAxis, yAxis }, transformer), StructuredData({ &zs }, { zScale }, transformer)
{
	sci::assertThrow(zs.shape()[0] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));
	sci::assertThrow(zs.shape()[1] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

	m_x1d = true;
	m_y1d = true;
	m_xNBoxes = zs.shape()[0];
	m_yNBoxes = zs.shape()[1];
	m_xAxis = xAxis;
	m_yAxis = yAxis;
}

Data2d::Data2d(const sci::GridData<double, 2> &xs, const sci::GridData<double, 2> &ys, const sci::GridData<double, 2> &zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({}, {}, transformer), StructuredData({ &xs, &ys, &zs }, { xAxis, yAxis, zScale }, transformer)
{
	sci::assertThrow(zs.shape()[0] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));
	sci::assertThrow(zs.shape()[1] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

	m_x1d = false;
	m_y1d = false;
	m_xNBoxes = zs.shape()[0];
	m_yNBoxes = zs.shape()[1];
	m_xAxis = xAxis;
	m_yAxis = yAxis;
}

Data2d::Data2d(std::span<const double> xs, const sci::GridData<double, 2> &ys, const sci::GridData<double, 2> &zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs }, { xAxis }, transformer), StructuredData({ &ys, &zs }, { yAxis, zScale }, transformer)
{
	sci::assertThrow(zs.shape()[0] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));
	sci::assertThrow(zs.shape()[1] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

	m_x1d = true;
	m_y1d = false;
	m_xNBoxes = zs.shape()[0];
	m_yNBoxes = zs.shape()[1];
	m_xAxis = xAxis;
	m_yAxis = yAxis;
}

Data2d::Data2d(const sci::GridData<double, 2> &xs, std::span<const double> ys, const sci::GridData<double, 2> &zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ ys }, { yAxis }, transformer), StructuredData({ &xs, &zs }, { xAxis, zScale }, transformer)
{
	sci::assertThrow(zs.shape()[0] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));
	sci::assertThrow(zs.shape()[1] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));


	m_x1d = false;
	m_y1d = true;
	m_xNBoxes = zs.shape()[0];
	m_yNBoxes = zs.shape()[1];
	m_xAxis = xAxis;
	m_yAxis = yAxis;
}

void Data2d::autoscaleAxes()
{
	UnstructuredData::autoscaleAxes();
	StructuredData::autoscaleAxes();
}

GridData::GridData(std::span<const double> xs, std::span<const double> ys, const sci::GridData<double, 2> &zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
	sci::assertThrow(ys.size() == zs.shape()[1] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));
	
	m_colourscale = colourScale;
}

GridData::GridData(const sci::GridData<double, 2> &xs, const sci::GridData<double, 2> &ys, const sci::GridData<double, 2> &zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.shape()[0] == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
	sci::assertThrow(ys.shape()[0] == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));

	sci::assertThrow(xs.shape()[1] == zs.shape()[1] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
	sci::assertThrow(ys.shape()[1] == zs.shape()[1] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));
	

	m_colourscale = colourScale;
}

GridData::GridData(std::span<const double> xs, const sci::GridData<double, 2> &ys, const sci::GridData<double, 2> &zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
	sci::assertThrow(ys.shape()[0] == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));
	sci::assertThrow(ys.shape()[1] == zs.shape()[1] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));


	m_colourscale = colourScale;
}

GridData::GridData(const sci::GridData<double, 2> &xs, std::span<const double> ys, const sci::GridData<double, 2> &zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.shape()[0] == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
	sci::assertThrow(xs.shape()[1] == zs.shape()[1] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
	sci::assertThrow(ys.size() == zs[1].size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));

	m_colourscale = colourScale;
}

void GridData::plotData(plstream* pl, double scale) const
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

void GridData::plotData(Renderer& renderer, grPerMillimetre scale) const
{
	if (!StructuredData::hasData())
		return;

	renderer.setPen(rgbcolour(), Length(grMillimetre(0.0)));

	//get the limits of the clourscale - this will get either the linear or logged min
	//as appropriate
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
		std::vector<const double*> zs = StructuredData::getPointer(0); //returns logged data if it's a log z axis
		for (size_t i = 0; i < m_xNBoxes; ++i)
		{
			for (size_t j = 0; j < m_yNBoxes; ++j)
			{
				if (zs[i][j] < zMin)
					continue;
				if (zs[i][j] > zMax)
					continue;
				renderer.setBrush(m_colourscale->getRgbOriginalScale(zs[i][j], true));
				renderer.rectangle(getPoint(x[i], y[j]), getPoint(x[i + 1], y[j + 1]));
			}
		}
	}
	else if (m_x1d)
	{
		const double* x = UnstructuredData::getPointer(0);
		std::vector<const double*> y = StructuredData::getPointer(0);
		std::vector<const double*> zs = StructuredData::getPointer(1); //returns logged data if it's a log z axis
		for (size_t i = 0; i < m_xNBoxes; ++i)
		{
			for (size_t j = 0; j < m_yNBoxes; ++j)
			{
				if (zs[i][j] < zMin)
					continue;
				if (zs[i][j] > zMax)
					continue;
				renderer.setBrush(m_colourscale->getRgbOriginalScale(zs[i][j], true));
				renderer.polygon({ getPoint(x[i], y[i][j]), getPoint(x[i + 1], y[i + 1][j]),
					getPoint(x[i + 1], y[i + 1][j + 1]), getPoint(x[i], y[i][j + 1]),
					getPoint(x[i], y[i][j]) });
			}
		}
	}
	else if (m_y1d)
	{
		std::vector<const double*> x = StructuredData::getPointer(0);
		const double* y = UnstructuredData::getPointer(0);
		std::vector<const double*> zs = StructuredData::getPointer(1);
		for (size_t i = 0; i < m_xNBoxes; ++i)
		{
			for (size_t j = 0; j < m_yNBoxes; ++j)
			{
				if (zs[i][j] < zMin)
					continue;
				if (zs[i][j] > zMax)
					continue;
				renderer.setBrush(m_colourscale->getRgbOriginalScale(zs[i][j], true));
				renderer.polygon({ getPoint(x[i][j], y[j]), getPoint(x[i + 1][j], y[j]),
					getPoint(x[i + 1][j + 1], y[j + 1]), getPoint(x[i][j + 1], y[j + 1]),
					getPoint(x[i][j], y[j]) });
			}
		}
	}
	else
	{
		std::vector<const double*> x = StructuredData::getPointer(0);
		std::vector<const double*> y = StructuredData::getPointer(1);
		std::vector<const double*> zs = StructuredData::getPointer(2);
		for (size_t i = 0; i < m_xNBoxes; ++i)
		{
			for (size_t j = 0; j < m_yNBoxes; ++j)
			{
				if (zs[i][j] < zMin)
					continue;
				if (zs[i][j] > zMax)
					continue;
				renderer.setBrush(m_colourscale->getRgbOriginalScale(zs[i][j], true));
				renderer.polygon({ getPoint(x[i][j], y[i][j]), getPoint(x[i + 1][j], y[i + 1][j]),
					getPoint(x[i + 1][j + 1], y[i + 1][j + 1]), getPoint(x[i][j + 1], y[i][j + 1]),
					getPoint(x[i][j], y[i][j]) });
			}
		}
	}
}

ContourData::ContourData(std::span<const double> xs, std::span<const double> ys, const sci::GridData<double, 2>& zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.shape()[0], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs.shape()[1], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

	m_colourscale = colourScale;
	m_levelScale = nullptr;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(const sci::GridData<double, 2> & xs, const sci::GridData<double, 2> & ys, const sci::GridData<double, 2> & zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));
	
	m_colourscale = colourScale;
	m_levelScale = nullptr;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(std::span<const double> xs, const sci::GridData<double, 2> & ys, const sci::GridData<double, 2> & zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.shape()[0], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));
	
	m_colourscale = colourScale;
	m_levelScale = nullptr;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(const sci::GridData<double, 2> & xs, std::span<const double> ys, const sci::GridData<double, 2> & zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs.shape()[1], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

	m_colourscale = colourScale;
	m_levelScale = nullptr;
	m_lineStyle = lineStyle;
}



ContourData::ContourData(std::span<const double> xs, std::span<const double> ys, const sci::GridData<double, 2> & zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
{
	sci::assertThrow(xs.size() == zs.shape()[0], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs.shape()[1], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

	m_colourscale = nullptr;
	m_levelScale = levelScale;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(const sci::GridData<double, 2> & xs, const sci::GridData<double, 2> & ys, const sci::GridData<double, 2> & zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
{
	sci::assertThrow(xs.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));
	
	m_colourscale = nullptr;
	m_levelScale = levelScale;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(std::span<const double> xs, const sci::GridData<double, 2> & ys, const sci::GridData<double, 2> & zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
{
	sci::assertThrow(xs.size() == zs.shape()[0], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));
	
	m_colourscale = nullptr;
	m_levelScale = levelScale;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(const sci::GridData<double, 2> & xs, std::span<const double> ys, const sci::GridData<double, 2> & zs, std::shared_ptr<PlotAxis> xAxis, std::shared_ptr<PlotAxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
{
	sci::assertThrow(xs.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs.shape()[1], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

	m_colourscale = nullptr;
	m_levelScale = levelScale;
	m_lineStyle = lineStyle;
}

void ContourData::plotData(plstream* pl, double scale) const
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

struct Segment
{
	size_t index1;
	size_t index2;
	uint8_t intersectionType;
	double level;
	bool operator==(const Segment &other) const
	{
		return index1 == other.index1 && index2 == other.index2 && intersectionType == other.intersectionType;
	}
	bool operator!=(const Segment& other) const
	{
		return !(*this == other);
	}
	bool operator<(const Segment& other) const
	{
		return index1 == other.index1 ? 
			(index2 == other.index2 ? intersectionType < other.intersectionType : index2 < other.index2)
			: index1 < other.index1;
	}
};

uint8_t getIntersectionType(size_t i, size_t j, const sci::GridData<double, 2>& grid, double contourLevel)
{
	uint8_t intersectionType = 0;
	if (grid[i][j] > contourLevel)
		intersectionType = intersectionType | 0x01;
	if (grid[i + 1][j] > contourLevel)
		intersectionType = intersectionType | 0x02;
	if (grid[i + 1][j + 1] > contourLevel)
		intersectionType = intersectionType | 0x04;
	if (grid[i][j + 1] > contourLevel)
		intersectionType = intersectionType | 0x08;


	//disambiguate saddle points
	if (5 == intersectionType && (grid[i + 1][j + 1] + grid[i + 1][j] + grid[i][j] + grid[i][j + 1]) / 4.0 <= contourLevel)
		intersectionType = 16;
	else if (10 == intersectionType && (grid[i + 1][j + 1] + grid[i + 1][j] + grid[i][j] + grid[i][j + 1]) / 4.0 <= contourLevel)
		intersectionType = 17;
	return intersectionType;
}

uint8_t disambiguateContours(uint8_t intersectionType)
{
	//for contours we don't care which corners are higher or lower, so we can save some
	//search time by skipping the duplicate countour paths
	if (intersectionType == 16)
		return 5;
	if (intersectionType == 17)
		return 10;
	if (intersectionType > 7 && intersectionType != 10)
		return 15 - intersectionType;
	return intersectionType;
}

constexpr std::array<uint8_t, 3> getLeftIntersectionTypes()
{
	return { 1, 3, 7 };
}

constexpr std::array<uint8_t, 3> getRightIntersectionTypes()
{
	return { 2, 3, 4};
}

constexpr std::array<uint8_t, 3> getTopIntersectionTypes()
{
	return { 4, 6, 7 };
}

constexpr std::array<uint8_t, 3> getBottomIntersectionTypes()
{
	return { 1, 2, 6 };
}

constexpr bool hasLeft(uint8_t intersectionType)
{
	return intersectionType == getLeftIntersectionTypes()[0] ||
		intersectionType == getLeftIntersectionTypes()[1] ||
		intersectionType == getLeftIntersectionTypes()[2];
}

constexpr bool hasRight(uint8_t intersectionType)
{
	return intersectionType == getRightIntersectionTypes()[0] ||
		intersectionType == getRightIntersectionTypes()[1] ||
		intersectionType == getRightIntersectionTypes()[2];
}

constexpr bool hasTop(uint8_t intersectionType)
{
	return intersectionType == getTopIntersectionTypes()[0] ||
		intersectionType == getTopIntersectionTypes()[1] ||
		intersectionType == getTopIntersectionTypes()[2];
}

constexpr bool hasBottom(uint8_t intersectionType)
{
	return intersectionType == getBottomIntersectionTypes()[0] ||
		intersectionType == getBottomIntersectionTypes()[1] ||
		intersectionType == getBottomIntersectionTypes()[2];
}

//retuens all possible link options. Where there are fewer than 6 options
//the unused elements of the result will be filled with the given segment
std::array<Segment, 6> getPossibleLinkOptions(Segment segment, size_t nX, size_t nY)
{
	//we are assuming we have disambiguated contours and split cases 5 and 10
	std::array<Segment, 6> result{ segment, segment, segment, segment, segment, segment };
	auto insertPoint = result.begin();
	if (hasLeft(segment.intersectionType) && segment.index1 > 1)
	{
		*insertPoint = { segment.index1 - 1, segment.index2, getLeftIntersectionTypes()[0] };
		*(insertPoint + 1) = { segment.index1 - 1, segment.index2, getLeftIntersectionTypes()[1] };
		*(insertPoint + 2) = { segment.index1 - 1, segment.index2, getLeftIntersectionTypes()[2] };
		insertPoint += 3;
	}
	if (hasRight(segment.intersectionType) && segment.index1 < nX)
	{
		*insertPoint = { segment.index1 + 1, segment.index2, getRightIntersectionTypes()[0] };
		*(insertPoint + 1) = { segment.index1 + 1, segment.index2, getRightIntersectionTypes()[1] };
		*(insertPoint + 2) = { segment.index1 + 1, segment.index2, getRightIntersectionTypes()[2] };
		insertPoint += 3;
	}
	if (hasTop(segment.intersectionType) && segment.index2 < nY)
	{
		*insertPoint = { segment.index1, segment.index2 + 1, getTopIntersectionTypes()[0] };
		*(insertPoint + 1) = { segment.index1, segment.index2 + 1, getTopIntersectionTypes()[1] };
		*(insertPoint + 2) = { segment.index1, segment.index2 + 1, getTopIntersectionTypes()[2] };
		insertPoint += 3;
	}
	if (hasBottom(segment.intersectionType) && segment.index2 > 1)
	{
		*insertPoint = { segment.index1, segment.index2 - 1, getBottomIntersectionTypes()[0] };
		*(insertPoint + 1) = { segment.index1, segment.index2 - 1, getBottomIntersectionTypes()[1] };
		*(insertPoint + 2) = { segment.index1, segment.index2 - 1, getBottomIntersectionTypes()[2] };
		insertPoint += 3;
	}
	return result;
}

//follows a contour in either direction removing the elements from segments as it goes.
//the return value is a vector of ordered segments starting at begin
//the begin segment will be included in the retuned result
std::vector<Segment> followContourEitherDirection(Segment begin, std::set<Segment>& segments, size_t nDim1, size_t nDim2)
{
	std::vector<Segment> line;

	//check the begin segment actually exists in the segments set
	auto found = segments.find(begin);
	if (found == segments.end())
		return line;

	line.push_back(*found);
	segments.erase(found);

	Segment previousSegment{ -1, -1, 0 };
	while (1)
	{
		std::array<Segment, 6> linkOptions = getPossibleLinkOptions(line.back(), nDim1, nDim2); // any elements equal to line.back() are a flag for blank
		Segment nextSegment = line.back();
		for (auto& s : linkOptions)
		{
			if (s != line.back() && !(s.index1 == previousSegment.index1 && s.index2 == previousSegment.index2))
			{
				auto found = segments.find(s);
				if (found != segments.end())
				{
					nextSegment = *found;
					segments.erase(found);
					break;
				}
			}
		}
		if (nextSegment != line.back())
		{
			previousSegment = line.back();
			line.push_back(nextSegment);
		}
		else
			break;
	}
	return line;
}

std::vector<Segment> followContourBothDirections(std::set<Segment>& segments, size_t nDim1, size_t nDim2)
{
	if (segments.size() == 0)
		return std::vector<Segment>(0);

	//create 2 line halves running in opposite directions from the first point
	Segment begin = *segments.begin();
	//remove the 

	std::vector<Segment> line1 = followContourEitherDirection(begin, segments, nDim1, nDim2);
	std::vector<Segment> line2 = followContourEitherDirection(begin, segments, nDim1, nDim2); // this will follow in the other direction as there is nowhere else to go
	segments.erase(begin); // erase the begin point here so that the line will automatically close if it is a loop

	//if line2 has any elements, reverse it and prepend it to line 1, otherwise line 1 is our complete contour
	if (line2.size() > 0)
	{
		line2.reserve(line2.size() + line1.size() - 1);
		std::reverse(line2.begin(), line2.end());
		line2.insert(line2.end(), line1.begin() + 1, line1.end());
		return line2;
	}
	else
		return line1;
}

//creates a set of contour segments for each countour level. The sets are in order from the first to last level.
//Because we use a set, the segments are in ascending order using the < operator defined in the set class
std::vector<std::set<Segment>> marchingSquaresSegments(const sci::GridData<double, 2>& grid, std::span<const double> contourLevels)
{
	std::array<size_t, 2> shape = grid.shape();

	//get all the intesections of grids with contours using the marching squares algorithm
	std::vector<std::set<Segment>> segments(contourLevels.size());
	for (size_t i = 0; i < shape[0] - 1; ++i)
	{
		for (size_t j = 0; j < shape[1] - 1; ++j)
		{
			for (size_t k = 0; k < contourLevels.size(); ++k)
			{
				uint8_t intersectionType = disambiguateContours(getIntersectionType(i, j, grid, contourLevels[k]));
				if (intersectionType != 0 && intersectionType != 15 && intersectionType != 5 && intersectionType != 10)
					segments[k].insert(Segment{ i, j, intersectionType, contourLevels[k] });
				else if (5 == intersectionType || 16 == intersectionType)
				{
					segments[k].insert(Segment{ i, j, 2, contourLevels[k] });
					segments[k].insert(Segment{ i, j, 7, contourLevels[k] });
				}
				else if (10 == intersectionType || 17 == intersectionType)
				{
					segments[k].insert(Segment{ i, j, 1, contourLevels[k] });
					segments[k].insert(Segment{ i, j, 4, contourLevels[k] });
				}
			}
		}
	}
	return segments;
}

//creates a set of contour segments for each countour level. The sets are in order from the first to last level.
//Because we use a set, the segments are in ascending order using the < operator defined in the set class
std::vector<std::vector<Segment>> marchingSquaresContour(const sci::GridData<double, 2>  &grid, std::span<const double> contourLevels)
{
	std::array<size_t, 2> shape = grid.shape();

	//get all the intesections of grids with contours using the marching squares algorithm
	std::vector<std::set<Segment>> segments(contourLevels.size());
	for (size_t i = 0; i < shape[0] - 1; ++i)
	{
		for (size_t j = 0; j < shape[1] - 1; ++j)
		{
			for (size_t k = 0; k < contourLevels.size(); ++k)
			{
				uint8_t intersectionType = disambiguateContours( getIntersectionType(i, j, grid, contourLevels[k]) );
				if (intersectionType != 0 && intersectionType != 15 && intersectionType != 5 && intersectionType != 10)
					segments[k].insert(Segment{i, j, intersectionType, contourLevels[k]});
				else if (5 == intersectionType || 16 == intersectionType)
				{
					segments[k].insert(Segment{i, j, 2, contourLevels[k] });
					segments[k].insert(Segment{i, j, 7, contourLevels[k] });
				}
				else if (10 == intersectionType || 17 == intersectionType)
				{
					segments[k].insert(Segment{i, j, 1, contourLevels[k] });
					segments[k].insert(Segment{i, j, 4, contourLevels[k] });
				}
			}
		}
	}


	//link all those segments into lines
	std::vector<std::vector<Segment>> result;
	for (size_t i = 0; i < segments.size(); ++i)
	{
		while (segments[i].size() > 0)
		{
			result.push_back(followContourBothDirections(segments[i], shape[0], shape[1]));
		}
	}
	return result;
}

template <bool isDim1>
std::array<std::array<double, 2>, 2> getCornerValues(size_t index1, size_t index2, const std::vector<double>& v)
{
	if constexpr (isDim1)
		return{ std::array<double, 2>{v[index1], v[index1]},
				std::array<double, 2>{v[index1 + 1], v[index1 + 1]} };
	else
		return{ std::array<double, 2>{v[index2], v[index2 + 1]},
				std::array<double, 2>{v[index2], v[index2 + 1]} };
}

template <bool isDim1>
std::array<std::array<double, 2>, 2> getCornerValues(size_t index1, size_t index2, const sci::GridData<double, 2>& g)
{
	//isDim1 is only there to match the function above and isn't used
	return{ std::array<double, 2>{g[index1][index2], g[index1][index2 + 1]},
			std::array<double, 2>{g[index1 + 1][index2], g[index1 + 1][index2 + 1]} };

}

template <class DATAX, class DATAY, class DATAZ>
void getJoiningPoint(const DATAX &allXs, const DATAY& allYs, const DATAZ& allZs,
	const Segment &segment1, const Segment& segment2, double& x, double& y)
{
	std::array<std::array<double, 2>, 2> xs = getCornerValues<true>(segment1.index1, segment1.index2, allXs);
	std::array<std::array<double, 2>, 2> ys = getCornerValues<false>(segment1.index1, segment1.index2, allYs);
	std::array<std::array<double, 2>, 2> zs = getCornerValues<true>(segment1.index1, segment1.index2, allZs);
	const double& contourLevel = segment1.level;
	if (segment2.index1 == segment1.index1 + 1) //right
	{
		double weight = (contourLevel - zs[1][0]) / (contourLevel - zs[1][1]);
		x = xs[1][0] + weight * (xs[1][1] - xs[1][0]);
		y = ys[1][0] + weight * (ys[1][1] - ys[1][0]);
	}
	else if (segment2.index1 == segment1.index1 - 1) //left
	{
		double weight = (contourLevel - zs[0][0]) / (contourLevel - zs[0][1]);
		x = xs[0][0] + weight * (xs[0][1] - xs[0][0]);
		y = ys[0][0] + weight * (ys[0][1] - ys[0][0]);
	}
	else if (segment2.index2 == segment1.index2 + 1) //top
	{
		double weight = (contourLevel - zs[0][1]) / (contourLevel - zs[1][1]);
		x = xs[0][1] + weight * (xs[1][1] - xs[0][1]);
		y = ys[0][1] + weight * (ys[1][1] - ys[0][1]);
	}
	else if (segment2.index2 == segment1.index2 - 1) //bottom
	{
		double weight = (contourLevel - zs[0][0]) / (contourLevel - zs[1][0]);
		x = xs[0][0] + weight * (xs[1][0] - xs[0][0]);
		y = ys[0][0] + weight * (ys[1][0] - ys[0][0]);
	}
	else
		throw("some contouring error happened that indicates a bug");
}

template <class DATAX, class DATAY, class DATAZ>
void getSegment1NonJoiningPoint(const DATAX& allXs, const DATAY& allYs, const DATAZ& allZs,
	const Segment& segment1, const Segment& segment2, double& x, double& y)
{
	std::array<std::array<double, 2>, 2> xs = getCornerValues<true>(segment1.index1, segment1.index2, allXs);
	std::array<std::array<double, 2>, 2> ys = getCornerValues<false>(segment1.index1, segment1.index2, allYs);
	std::array<std::array<double, 2>, 2> zs = getCornerValues<true>(segment1.index1, segment1.index2, allZs);
	const double& contourLevel = segment1.level;
	if ((segment2.index1 == segment1.index1 + 1)) //right join, get left
	{
		double weight = (contourLevel - zs[0][0]) / (zs[0][1] - zs[0][0]);
		x = xs[0][0] + weight * (xs[0][1] - xs[0][0]);
		y = ys[0][0] + weight * (ys[0][1] - ys[0][0]);
	}
	else if (segment2.index1 == segment1.index1 - 1) //left join, get right
	{
		double weight = (contourLevel - zs[1][0]) / (zs[1][1] - zs[1][0]);
		x = xs[1][0] + weight * (xs[1][1] - xs[1][0]);
		y = ys[1][0] + weight * (ys[1][1] - ys[1][0]);
	}
	else if (segment2.index2 == segment1.index2 + 1) //top join, get bottom
	{
		double weight = (contourLevel - zs[0][0]) / (zs[1][0] - zs[0][0]);
		x = xs[0][0] + weight * (xs[1][0] - xs[0][0]);
		y = ys[0][0] + weight * (ys[1][0] - ys[0][0]);
	}
	else if (segment2.index2 == segment1.index2 - 1) //bottom join, get top
	{
		double weight = (contourLevel - zs[0][1]) / (zs[1][1] - zs[0][1]);
		x = xs[0][1] + weight * (xs[1][1] - xs[0][1]);
		y = ys[0][1] + weight * (ys[1][1] - ys[0][1]);
	}
}

template <class DATAX, class DATAY, class DATAZ>
void getBothPoints(const DATAX& allXs, const DATAY& allYs, const DATAZ& allZs,
	const Segment& segment1, std::vector<double>& x, std::vector<double>& y)
{
	std::array<std::array<double, 2>, 2> xs = getCornerValues<true>(segment1.index1, segment1.index2, allXs);
	std::array<std::array<double, 2>, 2> ys = getCornerValues<false>(segment1.index1, segment1.index2, allYs);
	std::array<std::array<double, 2>, 2> zs = getCornerValues<true>(segment1.index1, segment1.index2, allZs);
	x.resize(0);
	y.resize(0);
	x.resize(2);
	y.resize(2);
	auto xIter = x.begin();
	auto yIter = y.begin();
	const double& contourLevel = segment1.level;
	if (hasRight(segment1.intersectionType)) //right
	{
		double weight = (contourLevel - zs[1][0]) / (zs[1][1] - zs[1][0]);
		*xIter = xs[1][0] + weight * (xs[1][1] - xs[1][0]);
		*yIter = ys[1][0] + weight * (ys[1][1] - ys[1][0]);
		++xIter;
		++yIter;
	}
	if (hasLeft(segment1.intersectionType)) //left
	{
		double weight = (contourLevel - zs[0][0]) / (zs[0][1] - zs[0][0]);
		*xIter = xs[0][0] + weight * (xs[0][1] - xs[0][0]);
		*yIter = ys[0][0] + weight * (ys[0][1] - ys[0][0]);
		++xIter;
		++yIter;
	}
	if (hasTop(segment1.intersectionType)) //top
	{
		double weight = (contourLevel - zs[0][1]) / (zs[1][1] - zs[0][1]);
		*xIter = xs[0][1] + weight * (xs[1][1] - xs[0][1]);
		*yIter = ys[0][1] + weight * (ys[1][1] - ys[0][1]);
		++xIter;
		++yIter;
	}
	if (hasBottom(segment1.intersectionType)) //bottom
	{
		double weight = (contourLevel - zs[0][0]) / (zs[1][0] - zs[0][0]);
		*xIter = xs[0][0] + weight * (xs[1][0] - xs[0][0]);
		*yIter = ys[0][0] + weight * (ys[1][0] - ys[0][0]);
		++xIter;
		++yIter;
	}
}

template <class DATAX, class DATAY, class DATAZ>
void getContourLine(const DATAX& allXs, const DATAY& allYs, const DATAZ& allZs,
	const std::vector<Segment>& segments, std::vector<double>& xs, std::vector<double>& ys)
{
	if (segments.size() == 0)
	{
		xs.resize(0);
		ys.resize(0);
		return;
	}
	xs.resize(segments.size() + 1);
	ys.resize(segments.size() + 1);
	if (segments.size() == 1)
		getBothPoints(allXs, allYs, allZs, segments[0], xs, ys);
	else
	{
		for (size_t j = 0; j < segments.size() - 1; ++j)
			getJoiningPoint(allXs, allYs, allZs, segments[j], segments[j + 1], xs[j + 1], ys[j + 1]); //this omits the last and first point
		//sort the last and first point
		getSegment1NonJoiningPoint(allXs, allYs, allZs, segments[0], segments[1], xs[0], ys[0]);
		getSegment1NonJoiningPoint(allXs, allYs, allZs, segments.back(), segments[segments.size() - 2], xs.back(), ys.back());
	}
}

void ContourData::plotData(Renderer& renderer, grPerMillimetre scale) const
{
	sci::GridData<double, 1> contourLevels;
	if (m_levelScale)
	{
		contourLevels = m_levelScale->getLevels();
		sci::assertThrow(contourLevels.size() > 1, sci::err(sci::SERR_PLOT, 0, "ContourData::plotData: Cannot use a level scale with fewer than 2 levels."));
	}

	//get the segments of each contour line using the marching squares algorithm.
	//note that this just gives the edges of the grid boxes that each segment touches
	const sci::GridData<double, 2>& zs = getZ();
	std::array<size_t, 2> shape = zs.shape();
	std::vector<std::set<Segment>> segmentsPerLevel = marchingSquaresSegments(zs, contourLevels);

	//render the fill

	std::vector<std::set<Segment>::iterator> segmentIters(segmentsPerLevel.size());
	for (size_t i = 0; i < segmentsPerLevel.size(); ++i)
		segmentIters[i] = segmentsPerLevel[i].begin();

	std::vector<std::set<Segment>::iterator> intersections;
	for (size_t i = 0; i < shape[0]-1; ++i)
	{
		for (size_t j = 0; j < shape[1]-1; ++j)
		{
			double x0;
			double x1;
			double y0;
			double y1;
			//if both x and y are the same dimensionality then the will be stored in the same
			//class with x as index 0 and y as index 1. Otherwise they will be stored in different
			//classes, both as index 0;
			size_t yIndex = (m_x1d == m_y1d) ? 1 : 0;
			if (m_x1d)
			{
				x0 = getVector(0)[i];
				x1 = getVector(0)[i + 1];
			}
			else
			{
				x0 = getGrid(0)[i][j];
				x1 = getGrid(0)[i + 1][j + 1];
			}
			if (m_y1d)
			{
				y0 = getVector(yIndex)[j];
				y1 = getVector(yIndex)[j + 1];
			}
			else
			{
				y0 = getGrid(yIndex)[i][j];
				y1 = getGrid(yIndex)[i + 1][j + 1];
			}
			std::pair<size_t, size_t> ijPair(i, j);
			for (size_t k = 0; k < segmentIters.size(); ++k)
			{
				if (segmentIters[k] != segmentsPerLevel[k].end() && i== segmentIters[k]->index1 && j== segmentIters[k]->index2)
				{
					intersections.push_back(segmentIters[k]);
				}
			}
			if (intersections.size() == 0)
			{
				//full rectangle with no intersection - just fill
				renderer.rectangle(getPoint(x0, y0), getPoint(x1, y1));
			}
			else if (intersections.size() == 1)
			{
				//we can just use the intersectionType from the single intersection to sort this
				
			}
			else
			{
				//we're getting more complicated here
				//we can work through each band and if the countour level either side only has one 
				//intersection, then we can work out the nodes (corners, plus points where segments
				//touch the edge) and these will make a convex polygon, so we need to arrange them
				//clockwise or anticlockwise

				//if there is a contour level with 2 segments (this is the maximum possible0 then we
				//have a sadle point and multiple polygons the same shade 
			}


			intersections.resize(0);
		}
	}
	

	//render the contours
	// 
	//this is destructive - it moves segments from the segmentsPerLevel variable to the segments variable
	std::vector<std::vector<Segment>> segments;
	for (size_t i = 0; i < segmentsPerLevel.size(); ++i)
	{
		while (segmentsPerLevel[i].size() > 0)
		{
			segments.push_back(followContourBothDirections(segmentsPerLevel[i], shape[0], shape[1]));
		}
	}



	//now work out where the contours cross each grid box edge
	std::vector<double> xs;
	std::vector<double> ys;
	std::vector<Point> points;
	if (m_x1d && m_y1d)
	{
		const std::vector<double>& inputXs = getVector(0);
		const std::vector<double>& inputYs = getVector(1);
		for (size_t i = 0; i < segments.size(); ++i)
		{
			getContourLine(inputXs, inputYs, zs, segments[i], xs, ys);

			m_lineStyle.setPen(renderer);
			points.resize(xs.size());
			for (size_t j = 0; j < points.size(); ++j)
				points[j] = getPoint(xs[j], ys[j]);
			renderer.polyLine(points);
		}
	}
	else if ((!m_x1d) && (!m_y1d))
	{
		const sci::GridData<double, 2>& inputXs = getGrid(0);
		const sci::GridData<double, 2>& inputYs = getGrid(1);
		for (size_t i = 0; i < segments.size(); ++i)
		{
			getContourLine(inputXs, inputYs, zs, segments[i], xs, ys);

			m_lineStyle.setPen(renderer);
			points.resize(xs.size());
			for (size_t j = 0; j < points.size(); ++j)
				points[j] = getPoint(xs[j], ys[j]);
			renderer.polyLine(points);
		}
	}
	else if (!m_x1d)
	{
		const sci::GridData<double, 2>& inputXs = getGrid(0);
		const std::vector<double>& inputYs = getVector(0);
		for (size_t i = 0; i < segments.size(); ++i)
		{
			getContourLine(inputXs, inputYs, zs, segments[i], xs, ys);

			m_lineStyle.setPen(renderer);
			points.resize(xs.size());
			for (size_t j = 0; j < points.size(); ++j)
				points[j] = getPoint(xs[j], ys[j]);
			renderer.polyLine(points);
		}
	}
	else
	{
		const std::vector<double>& inputXs = getVector(0);
		const sci::GridData<double, 2>& inputYs = getGrid(0);
		for (size_t i = 0; i < segments.size(); ++i)
		{
			getContourLine(inputXs, inputYs, zs, segments[i], xs, ys);

			m_lineStyle.setPen(renderer);
			points.resize(xs.size());
			for (size_t j = 0; j < points.size(); ++j)
				points[j] = getPoint(xs[j], ys[j]);
			renderer.polyLine(points);
		}
	}

}
