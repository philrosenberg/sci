#include "../include/svector/dep/plot/plotdata.h"
#include "../include/svector/dep/plot/transforms.h"
#include "../include/svector/serr.h"
#include "../include/svector/svector.h"


LineStyle::LineStyle( double width, const rgbcolour &colour, const std::vector<PLINT> &marks, const std::vector<PLINT> &spaces )
	: m_width( width ), m_colour( colour ), m_marks( marks ), m_spaces( spaces )
{
	sci::assertThrow(m_marks.size() == m_spaces.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "LineStyle constructor called with a different number of spaces to marks.") );
}
LineStyle::LineStyle( double width, const rgbcolour &colour, sci::string pattern )
	: m_width( width ), m_colour( colour )
{
	parseLineStyle( pattern, m_marks, m_spaces);
}

double LineStyle::getWidth() const
{
	return m_width;
}
void LineStyle::getPattern( std::vector<PLINT> &marks, std::vector<PLINT> &spaces ) const
{
	marks = m_marks;
	spaces = m_spaces;
}
rgbcolour LineStyle::getColour() const
{
	return m_colour;
}
void LineStyle::setupLineStyle( plstream *pl, PLINT colourIndex, double scale ) const
{
	pl->scol0a( colourIndex, m_colour.r() * 255, m_colour.g() * 255, m_colour.b() * 255, m_colour.a() );
	pl->col0( colourIndex );
	pl->width( m_width * scale );
	if(m_marks.size()==0)
		pl->styl(0,NULL, NULL);
	else
	{
		std::vector<PLINT> scaledMarks( m_marks.size() );
		std::vector<PLINT> scaledSpaces( m_spaces.size() );
		for(size_t i=0; i<scaledMarks.size(); ++i)
		{
			scaledMarks[i] = (PLINT)( m_marks[i] * scale ); 
			scaledSpaces[i] = (PLINT)( m_spaces[i] * scale );
		}
		pl->styl(m_marks.size(),&scaledMarks[0],&scaledSpaces[0]);
	}
}
void LineStyle::resetLineStyle( plstream *pl, PLINT colourIndex ) const
{
	pl->scol0a( colourIndex, 0, 0, 0, 1.0 );
	pl->styl( 0, NULL, NULL );
}

//Converts a series of characters into dots/dashes and spaces for use with Plplot. Plplot works with micrometres.
// here a space represents a gap of 200 um, a tab 800 um and for dashes a . is 200 um, a - or a _ are 800 um.
//Adjacent space or dash characters are summed, so a 1000 um gap followed by a 1000 um dash would be " \t._"
void LineStyle::parseLineStyle(const sci::string &pattern, std::vector<PLINT> &marks, std::vector<PLINT> &spaces)
{
	//set outputs to zero size
	marks.resize(0);
	spaces.resize(0);

	//return empty vectors if style is empty
	if(pattern.length()==0)
		return;


	//set up whether we are on a mark or space
	bool onmark=true;
	if(pattern[0]==' ' || pattern[0]=='\t' )
		onmark=false;

	//initialise our current lengths to zero
	int marklength=0;
	int spacelength=0;

	//work through each character of style
	for(size_t i=0; i<pattern.length(); ++i)
	{
		//if we have changed between a space and mark record the length
		if(onmark==true && (pattern[i]==' ' || pattern[i]=='\t'))
		{
			marks.push_back(marklength);
			marklength=0;
			onmark=false;
		}
		else if(onmark==false && ( pattern[i]=='_' || pattern[i]=='.' || pattern[i]=='-' ) )
		{
			spaces.push_back(spacelength);
			spacelength=0;
			onmark=true;
		}
		//add the current character to the current length
		if(pattern[i]==' ')
			spacelength+=200;
		else if(pattern[i]=='\t')
			spacelength+=800;
		else if(pattern[i]=='.')
			marklength+=200;
		else if(pattern[i]=='_')
			marklength+=800;
		else if(pattern[i]=='-')
			marklength+=800;
		else
			sci::assertThrow( false, sci::err(sci::SERR_PLOT, plotDataErrorCode, "LineStyle::parseLineStyle called with characters that cannot be converted to line marks. use only space, tab, ., -, _.") );
	}
	//add the last mark or space
	if(onmark==true)
		marks.push_back(marklength);
	else
		spaces.push_back(spacelength);

	//equalise the lengths of the vectors if needed
	if(marks.size()<spaces.size())
	{
		spaces[0] += spaces.back();
		spaces.resize( spaces.size() - 1 );
	}
	else if (spaces.size()<marks.size())
	{
		marks[0] += marks.back();
		marks.resize( marks.size() - 1 );
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

PlotFrame::PlotFrame(double bottomLeftX, double bottomLeftY, double width, double height, const FillStyle& fillStyle, const LineStyle& lineStyle, sci::string title, double titlesize, double titledistance, sci::string titlefont, int32_t titlestyle, wxColour titlecolour)
{
	m_bottomLeftX = bottomLeftX;
	m_bottomLeftY = bottomLeftY;
	m_width = width;
	m_height = height;
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
	double x[]{ m_bottomLeftX, m_bottomLeftX + m_width, m_bottomLeftX + m_width, m_bottomLeftX, m_bottomLeftX };
	double y[]{ m_bottomLeftY, m_bottomLeftY, m_bottomLeftY + m_height, m_bottomLeftY + m_height, m_bottomLeftY };

	if (m_fillStyle.getColour().a() > 0.0)
	{
		m_fillStyle.setupFillStyle(pl, 1, scale);
		pl->fill(5, x, y);
		m_fillStyle.resetFillStyle(pl, 1);
	}

	if (m_lineStyle.getWidth() > 0.0)
	{
		m_lineStyle.setupLineStyle(pl, 1, scale);
		pl->line(5, x, y);
		m_lineStyle.resetLineStyle(pl, 1);
	}


	pl->vpor(m_bottomLeftX, m_bottomLeftX + m_width, m_bottomLeftY, m_bottomLeftY + m_height);
	pl->sfci(m_titlestyle);
	//pl->sfontf(m_titlefont.mb_str());
	pl->schr(1.0, m_titlesize * scale / 72.0 * 25.4);
	pl->scol0(0, m_titlecolour.Red(), m_titlecolour.Green(), m_titlecolour.Blue());
	pl->col0(0);
	pl->mtex("t", m_titledistance, 0.5, 0.5, sci::utf16ToUtf8(m_title).c_str());
}

SymbolBase::SymbolBase( sci::string symbol, PLUNICODE fci )
{
	m_symbol = symbol;
	m_fci = fci;
}

sci::string SymbolBase::getSymbol() const
{
	return m_symbol;
}

PLUNICODE SymbolBase::getFci() const
{
	return m_fci;
}


Symbol::Symbol( sci::string symbol, double size, rgbcolour colour )
	: SymbolBase( symbol, 0 )
{
	m_size = size;
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
void ColourVaryingSymbol::setupSymbol( plstream *pl, PLINT colourIndex, double parameter, double scale ) const
{
	pl->sfci( getFci() );
	//pl->sfontf(m_pointfont[i].mb_str());
	pl->schr( m_size, scale );
	rgbcolour colour = m_colourScale->getRgbOriginalScale(parameter);
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
void SizeVaryingSymbol::setupSymbol( plstream *pl, PLINT colourIndex, double parameter, double scale ) const
{
	pl->sfci( getFci() );
	//pl->sfontf(m_pointfont[i].mb_str());
	double size = m_sizeScale->getsize(parameter);
	pl->schr( size, scale );
	pl->scol0a( colourIndex, m_colour.r() * 255, m_colour.g() * 255, m_colour.b() * 255, m_colour.a() );
	pl->col0( colourIndex );
}

bool SizeVaryingSymbol::isLogScaled() const
{
	return m_sizeScale->isLog();
}

double SizeVaryingSymbol::getSize(double parameter, bool useNormalisedScale) const
{
	return useNormalisedScale ? m_sizeScale->getSizeNormalisedScale(parameter) : m_sizeScale->getsize(parameter);
}

ColourAndSizeVaryingSymbol::ColourAndSizeVaryingSymbol ( std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotsizescale> sizeScale, sci::string symbol )
	:SymbolBase( symbol, 0 )
{
	m_colourScale = colourScale;
	m_sizeScale = sizeScale;
}

//we must call this prior to each individual symbol being plotted
void ColourAndSizeVaryingSymbol::setupSymbol( plstream *pl, PLINT colourIndex, double colourParameter, double sizeParameter, double scale ) const
{
	pl->sfci( getFci() );
	//pl->sfontf(m_pointfont[i].mb_str());
	double size = m_sizeScale->getsize( sizeParameter );
	pl->schr( size, scale );
	rgbcolour colour = m_colourScale->getRgbOriginalScale( colourParameter );
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
double ColourAndSizeVaryingSymbol::getSize(double parameter, bool useNormalisedScale) const
{
	return useNormalisedScale ? m_sizeScale->getSizeNormalisedScale(parameter) : m_sizeScale->getsize(parameter);
}

void PlotableItem::draw(plstream* pl, double scale, double pageWidth, double pageHeight)
{

	//set the position of the plot area on the page
	double xPositionStart;
	double xPositionEnd;
	double yPositionStart;
	double yPositionEnd;
	m_xAxis->getPosition(xPositionStart, xPositionEnd);
	m_yAxis->getPosition(yPositionStart, yPositionEnd);
	pl->vpor(xPositionStart, xPositionEnd, yPositionStart, yPositionEnd);

	//set the limits of the plot area in plot coordinates
	//set the limits of the plot area in terms of plot units
	//note we have to log the limits for a log axis
	double xmin = m_xAxis->getMin();
	double xmax = m_xAxis->getMax();
	double ymin = m_yAxis->getMin();
	double ymax = m_yAxis->getMax();
	if (m_xAxis->isLog())
	{
		xmin = std::log10(xmin);
		xmax = std::log10(xmax);
	}
	if (m_yAxis->isLog())
	{
		ymin = std::log10(ymin);
		ymax = std::log10(ymax);
	}
	pl->wind(xmin, xmax, ymin, ymax);

	//set the global transform
	pl->stransform((m_transformer ? splotTransform : NULL), (void*)(m_transformer.get()));

	//plot the data
	plotData(pl, scale);

	//reset the transform
	pl->stransform(NULL, NULL);
}

UnstructuredData::UnstructuredData(const std::vector<const std::vector<double>*>& data, std::vector<std::shared_ptr<PlotScale>> axes, std::shared_ptr<splotTransformer> transformer)
{
	sci::assertThrow(data.size() == axes.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "UnstructuredData constructor called with data and axes of different lengths."));
	m_data.resize(data.size());
	for (size_t i = 0; i < data.size(); ++i)
	{
		m_data[i] = *(data[i]);
		if(i>0)
			sci::assertThrow(data[0]->size() == data[i]->size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "UnstructuredData constructor called with data in different dimensions having different lengths."));
	}
	m_dataLogged = sci::log10(m_data);
	m_axes = axes;
}

void UnstructuredData::autoscaleAxes()
{
	for (size_t i = 0; i < m_data.size(); ++i)
		m_axes[i]->expand(m_data[i]);
}

void StructuredData::autoscaleAxes()
{
	for (size_t i = 0; i < m_data.size(); ++i)
		for (auto d : m_data[i])
			m_axes[i]->expand(d);
}

StructuredData::StructuredData(const std::vector<const std::vector<std::vector<double>>*>& data, std::vector<std::shared_ptr<PlotScale>> axes, std::shared_ptr<splotTransformer> transformer)
{

	sci::assertThrow(data.size() == axes.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "StructuredData constructor called with data and axes of different lengths."));
	m_data.resize(data.size());
	for (size_t i = 0; i < data.size(); ++i)
	{
		m_data[i] = *(data[i]);
		//removed the test below as it is not valid for GridData
		//if (i > 0)
		//	sci::assertThrow(data[0]->size() == data[i]->size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "StructuredData constructor called with data in different dimensions having different lengths."));
		for(size_t j=0; j<m_data[i].size(); ++j)
			if (j > 0)
				sci::assertThrow(m_data[i][0].size() == m_data[i][j].size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "StructuredData constructor called with data that is not rectanguler."));

	}
	m_dataLogged = sci::log10(m_data);
	m_axes = axes;
}

std::vector<const double*> StructuredData::getPointer(size_t dimension) const
{
	std::vector<const double*> result(m_data[dimension].size());
	if (m_axes[dimension]->isLog())
		for (size_t i = 0; i < result.size(); ++i)
			result[i] = &m_dataLogged[dimension][i][0];
	else
		for (size_t i = 0; i < result.size(); ++i)
			result[i] = &m_data[dimension][i][0];

	return result;
}

LineData::LineData( const std::vector<double> &xs, const std::vector<double> &ys, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const LineStyle &lineStyle, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &xs, &ys }, {xAxis, yAxis}, transformer), m_lineStyle(lineStyle)
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

PointData::PointData( const std::vector<double> &x, const std::vector<double> &y, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const Symbol &symbol, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &x, &y }, {xAxis, yAxis}, transformer), m_symbol(symbol)
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

PointDataColourVarying::PointDataColourVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const ColourVaryingSymbol &symbol, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &xs, &ys, &zs }, std::vector<std::shared_ptr<PlotScale>>{xAxis, yAxis, std::shared_ptr<PlotScale>(symbol.getColourscale())}, transformer), m_symbol(symbol)
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
			m_symbol.setupSymbol( pl, 1, *zi, scale);
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			pl->string(1,xi,yi,sci::toUtf8(symbol).c_str());
		}
	}
}

PointDataSizeVarying::PointDataSizeVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const SizeVaryingSymbol &symbol, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &xs, &ys, &zs }, std::vector<std::shared_ptr<PlotScale>>{xAxis, yAxis, std::shared_ptr<PlotScale>(symbol.getSizeScale())}, transformer), m_symbol(symbol)
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
			m_symbol.setupSymbol( pl, 1, *zi, scale );
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			if(m_symbol.getSize(*zi, m_autoscaleSize) > 0.0)
				pl->string(1,xi,yi,sci::toUtf8(symbol).c_str());
		}
	}
}

PointDataColourAndSizeVarying::PointDataColourAndSizeVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zsColour, const std::vector<double> &zsSize, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const ColourAndSizeVaryingSymbol &symbol, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &xs, &ys, &zsColour, &zsSize }, std::vector<std::shared_ptr<PlotScale>>{xAxis, yAxis, std::shared_ptr<PlotScale>(symbol.getColourscale()), std::shared_ptr<PlotScale>(symbol.getSizeScale())}, transformer), m_symbol(symbol)
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
			m_symbol.setupSymbol( pl, 1, *zColouri, *zSizei, scale );
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			if (m_symbol.getSize(*zSizei, m_autoscaleSize) > 0.0)
				pl->string(1,xi,yi,sci::toUtf8(symbol).c_str());
		}
	}
}

HorizontalErrorBars::HorizontalErrorBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &plusErrors, const std::vector<double> minusErrors, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const LineStyle style, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &ys, &(const std::vector<double>&)(xs + plusErrors), &(const std::vector<double>&)(xs - minusErrors) }, { yAxis, xAxis, xAxis }, transformer)
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

VerticalErrorBars::VerticalErrorBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &plusErrors, const std::vector<double> minusErrors, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const LineStyle style, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &xs, &(const std::vector<double>&)(ys + plusErrors), &(const std::vector<double>&)(ys - minusErrors) }, {xAxis, yAxis, yAxis}, transformer)
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

VerticalBars::VerticalBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &widths, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const LineStyle &lineStyle, const FillStyle &fillStyle, double zeroLine, std::shared_ptr<splotTransformer> transformer )
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &(const std::vector<double>&)(xs - 0.5 * widths), &ys, &(const std::vector<double>&)(xs + 0.5 * widths) }, { xAxis, yAxis, xAxis }, transformer)
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

FillData::FillData(const std::vector<double>& xs, const std::vector<double>& ys, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, const FillStyle& fillStyle, const LineStyle& outlineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &xs, &ys }, { xAxis, yAxis }, transformer), m_fillStyle(fillStyle), m_lineStyle(outlineStyle)
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

Data2d::Data2d(const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &xs, &ys }, { xAxis, yAxis }, transformer), StructuredData({ &zs }, { zScale }, transformer)
{
	sci::assertThrow(zs.size() > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));
	sci::assertThrow(zs[0].size() > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));
	sci::assertThrow(sci::rectangular(zs), sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with non-rectangular zs"));

	m_x1d = true;
	m_y1d = true;
	m_xSize = zs.size();
	m_ySize = zs[0].size();
}

Data2d::Data2d(const std::vector < std::vector<double>>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({}, {}, transformer), StructuredData({ &xs, &ys, &zs }, { xAxis, yAxis, zScale }, transformer)
{
	sci::assertThrow(zs.size() > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

	sci::assertThrow(zs[0].size() > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

	sci::assertThrow(sci::rectangular(zs), sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with non-rectangular zs"));
	sci::assertThrow(sci::rectangular(xs), sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with non-rectangular xs"));
	sci::assertThrow(sci::rectangular(ys), sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with non-rectangular ys"));

	m_x1d = false;
	m_y1d = false;
	m_xSize = zs.size();
	m_ySize = zs[0].size();
}

Data2d::Data2d(const std::vector<double>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &xs }, { xAxis }, transformer), StructuredData({ &ys, &zs }, { yAxis, zScale }, transformer)
{
	sci::assertThrow(zs.size() > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

	sci::assertThrow(zs[0].size() > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

	sci::assertThrow(sci::rectangular(zs), sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with non-rectangular zs"));
	sci::assertThrow(sci::rectangular(ys), sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with non-rectangular ys"));

	m_x1d = true;
	m_y1d = false;
	m_xSize = zs.size();
	m_ySize = zs[0].size();
}

Data2d::Data2d(const std::vector < std::vector<double>>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<PlotScale> zScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ &ys }, { yAxis }, transformer), StructuredData({ &xs, &zs }, { xAxis, zScale }, transformer)
{
	sci::assertThrow(zs.size() > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

	sci::assertThrow(zs[0].size() > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

	sci::assertThrow(sci::rectangular(zs), sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with non-rectangular zs"));
	sci::assertThrow(sci::rectangular(xs), sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with non-rectangular xs"));

	m_x1d = false;
	m_y1d = true;
	m_xSize = zs.size();
	m_ySize = zs[0].size();
}

void Data2d::autoscaleAxes()
{
	UnstructuredData::autoscaleAxes();
	StructuredData::autoscaleAxes();
}

GridData::GridData(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<std::vector<double>> &zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
	sci::assertThrow(ys.size() == zs[0].size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));
	
	m_colourscale = colourScale;
}

GridData::GridData(const std::vector < std::vector<double>>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
	sci::assertThrow(ys.size() == zs.size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));

	sci::assertThrow(xs[0].size() == zs[0].size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
	sci::assertThrow(ys[0].size() == zs[0].size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));
	

	m_colourscale = colourScale;
}

GridData::GridData(const std::vector<double>& xs, const std::vector < std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
	sci::assertThrow(ys.size() == zs.size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));

	sci::assertThrow(ys[0].size() == zs[0].size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));


	m_colourscale = colourScale;
}

GridData::GridData(const std::vector < std::vector<double>>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));

	sci::assertThrow(xs[0].size() == zs[0].size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
	sci::assertThrow(ys.size() == zs[0].size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));

	m_colourscale = colourScale;
}

void GridData::plotData(plstream* pl, double scale) const
{
	if (!StructuredData::hasData())
		return;


	//set up the colourscale
	m_colourscale->setupForImage(pl);
	//set up the minimum z we want to plot - this will depend if we want to plot off the bottom of the colourscale
	double zMin = m_colourscale->fillOffscaleBottom() ? -std::numeric_limits<double>::infinity() : m_colourscale->getMin();
	double zMax = m_colourscale->fillOffscaleTop() ? std::numeric_limits<double>::infinity() : m_colourscale->getMax();

	if (m_x1d && m_y1d)
	{
		const double* x = UnstructuredData::getPointer(0);
		const double* y = UnstructuredData::getPointer(1);
		std::vector<const double*> zs = StructuredData::getPointer(0);
		using GT = GridTransform<const double*, const double*, false>;

		GT transform(x, y);

		pl->imagefr(&zs[0], m_xSize, m_ySize, std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
			zMin, zMax, m_colourscale->getMin(), m_colourscale->getMax(), GT::callback, (void*)&transform);
	}
	else if (m_x1d)
	{
		const double* x = UnstructuredData::getPointer(0);
		std::vector<const double*> y = StructuredData::getPointer(0);
		std::vector<const double*> zs = StructuredData::getPointer(1);
		using GT = GridTransform<const double *, const double **, false>;

		GT transform(x, &y[0]);

		pl->imagefr(&zs[0], m_xSize, m_ySize, std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
			zMin, zMax, m_colourscale->getMin(), m_colourscale->getMax(), GT::callback, (void*)&transform);
	}
	else if (m_y1d)
	{
		std::vector<const double*> x = StructuredData::getPointer(0);
		const double* y = UnstructuredData::getPointer(0);
		std::vector<const double*> zs = StructuredData::getPointer(1);
		using GT = GridTransform<const double**, const double*, false>;

		GT transform(&x[0], y);

		pl->imagefr(&zs[0], m_xSize, m_ySize, std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
			zMin, zMax, m_colourscale->getMin(), m_colourscale->getMax(), GT::callback, (void*)&transform);
	}
	else
	{
		std::vector<const double*> x = StructuredData::getPointer(0);
		std::vector<const double*> y = StructuredData::getPointer(1);
		std::vector<const double*> zs = StructuredData::getPointer(2);
		using GT = GridTransform<const double**, const double**, false>;

		GT transform(&x[0], &y[0]);

		pl->imagefr(&zs[0], m_xSize, m_ySize, std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
			zMin, zMax, m_colourscale->getMin(), m_colourscale->getMax(), GT::callback, (void*)&transform);
	}
}

ContourData::ContourData(const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs[0].size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

	m_colourscale = colourScale;
	m_levelScale = nullptr;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(const std::vector<std::vector<double>>& xs, const std::vector<std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(xs[0].size() == zs[0].size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));
	sci::assertThrow(ys[0].size() == zs[0].size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

	m_colourscale = colourScale;
	m_levelScale = nullptr;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(const std::vector<double>& xs, const std::vector<std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));
	sci::assertThrow(ys[0].size() == zs[0].size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

	m_colourscale = colourScale;
	m_levelScale = nullptr;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(const std::vector<std::vector<double>>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotcolourscale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(xs[0].size() == zs[0].size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

	m_colourscale = colourScale;
	m_levelScale = nullptr;
	m_lineStyle = lineStyle;
}



ContourData::ContourData(const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs[0].size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

	m_colourscale = nullptr;
	m_levelScale = levelScale;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(const std::vector<std::vector<double>>& xs, const std::vector<std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(xs[0].size() == zs[0].size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));
	sci::assertThrow(ys[0].size() == zs[0].size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

	m_colourscale = nullptr;
	m_levelScale = levelScale;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(const std::vector<double>& xs, const std::vector<std::vector<double>>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));
	sci::assertThrow(ys[0].size() == zs[0].size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

	m_colourscale = nullptr;
	m_levelScale = levelScale;
	m_lineStyle = lineStyle;
}

ContourData::ContourData(const std::vector<std::vector<double>>& xs, const std::vector<double>& ys, const std::vector<std::vector<double>>& zs, std::shared_ptr<splotaxis> xAxis, std::shared_ptr<splotaxis> yAxis, std::shared_ptr<splotlevelscale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer)
	: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
{
	sci::assertThrow(xs.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(xs[0].size() == zs[0].size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
	sci::assertThrow(ys.size() == zs.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

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
		sci::assertThrow(shadeLevels.size() > 1, sci::err(sci::SERR_PLOT, 0, "ContourData::plotData: Cannot use a colourscale with fewer than 2 levels."));
		if (m_colourscale->fillOffscaleBottom())
			shadeLevels.front() = -std::numeric_limits<double>::infinity();
		if (m_colourscale->fillOffscaleTop())
			shadeLevels.back() = std::numeric_limits<double>::infinity();
	}

	//grab the contour levels from the levelScale
	std::vector<double> contourLevels;
	if (m_levelScale)
	{
		contourLevels = m_levelScale->getLevels();
		sci::assertThrow(contourLevels.size() > 1, sci::err(sci::SERR_PLOT, 0, "ContourData::plotData: Cannot use a level scale with fewer than 2 levels."));
	}

	//set up the minimum z we want to plot - this will depend if we want to plot off the bottom of the colourscale
	//double zMin = m_fillOffscaleBottom ? -std::numeric_limits<double>::infinity() : m_colourscale->getMin();
	//double zMax = m_fillOffscaleTop ? std::numeric_limits<double>::infinity() : m_colourscale->getMax();

	//set the line style for the contours
	m_lineStyle.setupLineStyle(pl, 1, scale);

	if (m_x1d && m_y1d)
	{
		const double* x = UnstructuredData::getPointer(0);
		const double* y = UnstructuredData::getPointer(1);
		std::vector<const double*> zs = StructuredData::getPointer(0);
		using GT = GridTransform<const double*, const double*, true>;

		GT transform(x, y);

		if(m_colourscale)
			pl->shades(&zs[0], m_xSize, m_ySize, NULL, std::numeric_limits<double>::quiet_NaN(),
				std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
				&shadeLevels[0], shadeLevels.size(), 1, 1, m_lineStyle.getWidth() * scale, plfill, true, GT::callback, (void*)(&transform));
		if (m_levelScale)
			pl->cont(&zs[0], m_xSize, m_ySize, 1, m_xSize, 1, m_ySize, &contourLevels[0], contourLevels.size(), GT::callback, (void*)(&transform));
	}
	else if (m_x1d)
	{
		const double* x = UnstructuredData::getPointer(0);
		std::vector<const double*> y = StructuredData::getPointer(0);
		std::vector<const double*> zs = StructuredData::getPointer(1);
		using GT = GridTransform<const double*, const double**, true>;

		GT transform(x, &y[0]);

		if (m_colourscale)
			pl->shades(&zs[0], m_xSize, m_ySize, NULL, std::numeric_limits<double>::quiet_NaN(),
				std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
				&shadeLevels[0], shadeLevels.size(), 1, 1, m_lineStyle.getWidth() * scale, plfill, true, GT::callback, (void*)(&transform));
		if (m_levelScale)
			pl->cont(&zs[0], m_xSize, m_ySize, 1, m_xSize, 1, m_ySize, &contourLevels[0], contourLevels.size(), GT::callback, (void*)(&transform));
	}
	else if (m_y1d)
	{
		std::vector<const double*> x = StructuredData::getPointer(0);
		const double* y = UnstructuredData::getPointer(0);
		std::vector<const double*> zs = StructuredData::getPointer(1);
		using GT = GridTransform<const double**, const double*, true>;

		GT transform(&x[0], y);

		if (m_colourscale)
			pl->shades(&zs[0], m_xSize, m_ySize, NULL, std::numeric_limits<double>::quiet_NaN(),
				std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
				&shadeLevels[0], shadeLevels.size(), 1, 1, m_lineStyle.getWidth() * scale, plfill, true, GT::callback, (void*)(&transform));
		if (m_levelScale)
			pl->cont(&zs[0], m_xSize, m_ySize, 1, m_xSize, 1, m_ySize, &contourLevels[0], contourLevels.size(), GT::callback, (void*)(&transform));
	}
	else
	{
		std::vector<const double*> x = StructuredData::getPointer(0);
		std::vector<const double*> y = StructuredData::getPointer(1);
		std::vector<const double*> zs = StructuredData::getPointer(2);
		using GT = GridTransform<const double**, const double**, true>;

		GT transform(&x[0], &y[0]);

		if (m_colourscale)
			pl->shades(&zs[0], m_xSize, m_ySize, NULL, std::numeric_limits<double>::quiet_NaN(),
				std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
				&shadeLevels[0], shadeLevels.size(), 1, 1, m_lineStyle.getWidth() * scale, plfill, true, GT::callback, (void*)(&transform));
		if (m_levelScale)
			pl->cont(&zs[0], m_xSize, m_ySize, 1, m_xSize, 1, m_ySize, &contourLevels[0], contourLevels.size(), GT::callback, (void*)(&transform));
	}
}
