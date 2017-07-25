#include "plotdata_internal.h"
#include "serr_internal.h"


LineStyle::LineStyle( double width, const rgbcolour &colour, const std::vector<PLINT> &marks, const std::vector<PLINT> &spaces )
	: m_width( width ), m_colour( colour ), m_marks( marks ), m_spaces( spaces )
{
	sci::assertThrow(m_marks.size() == m_spaces.size(), sci::err() );
}
LineStyle::LineStyle( double width, const rgbcolour &colour, std::string pattern )
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
		pl->styl(m_marks.size(),&m_marks[0],&m_spaces[0]);
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
void LineStyle::parseLineStyle(const std::string &pattern, std::vector<PLINT> &marks, std::vector<PLINT> &spaces)
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
			sci::assertThrow( false, sci::err() );
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

SymbolBase::SymbolBase( std::string symbol, PLUNICODE fci )
{
	m_symbol = symbol;
	m_fci = fci;
}

std::string SymbolBase::getSymbol() const
{
	return m_symbol;
}

PLUNICODE SymbolBase::getFci() const
{
	return m_fci;
}


Symbol::Symbol( std::string symbol, double size, rgbcolour colour )
	: SymbolBase( symbol, 0 )
{
	m_size = size;
	m_colour = colour;
}

void Symbol::setupSymbol( plstream *pl, PLINT colourIndex, double scale ) const
{
	pl->sfci( getFci() );
	//pl->sfontf(m_pointfont[i].mb_str());
	pl->schr( m_size, 1.0 );
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

VaryingSymbol::VaryingSymbol ( std::string symbol )
	: SymbolBase( symbol, 0 )
{
}

ColourVaryingSymbol::ColourVaryingSymbol ( std::string symbol, double size, splotcolourscale colourScale )
	:VaryingSymbol( symbol )
{
	m_size = size;
	m_colourScale = colourScale;
}

void ColourVaryingSymbol::setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool useNormalisedScale, double scale ) const
{
	pl->sfci( getFci() );
	//pl->sfontf(m_pointfont[i].mb_str());
	pl->schr( m_size, 1.0 );
	rgbcolour colour = useNormalisedScale ? m_colourScale.getRgbNormalisedScale( parameter ) : m_colourScale.getRgbOriginalScale( parameter );
	pl->scol0a( colourIndex, colour.r() * 255, colour.g() * 255, colour.b() * 255, colour.a() );
	pl->col0( colourIndex );
}

bool ColourVaryingSymbol::isLogScaled() const
{
	return m_colourScale.isLogarithmic();
}

SizeVaryingSymbol::SizeVaryingSymbol ( std::string symbol, rgbcolour colour, splotsizescale sizeScale )
	:VaryingSymbol( symbol )
{
	m_sizeScale = sizeScale;
	m_colour = colour;
}

void SizeVaryingSymbol::setupSymbol( plstream *pl, PLINT colourIndex, double parameter, bool useNormalisedScale, double scale ) const
{
	pl->sfci( getFci() );
	//pl->sfontf(m_pointfont[i].mb_str());
	double size = getSize(parameter, useNormalisedScale);
	pl->schr( size, 1.0 );
	pl->scol0a( colourIndex, m_colour.r() * 255, m_colour.g() * 255, m_colour.b() * 255, m_colour.a() );
	pl->col0( colourIndex );
}

bool SizeVaryingSymbol::isLogScaled() const
{
	return m_sizeScale.isLogarithmic();
}

double SizeVaryingSymbol::getSize(double parameter, bool useNormalisedScale) const
{
	return useNormalisedScale ? m_sizeScale.getSizeNormalisedScale(parameter) : m_sizeScale.getsize(parameter);
}

ColourAndSizeVaryingSymbol::ColourAndSizeVaryingSymbol ( std::string symbol, splotcolourscale colourScale, splotsizescale sizeScale )
	:SymbolBase( symbol, 0 )
{
	m_colourScale = colourScale;
	m_sizeScale = sizeScale;
}

void ColourAndSizeVaryingSymbol::setupSymbol( plstream *pl, PLINT colourIndex, double colourParameter, double sizeParameter, bool useNormalisedColourScale, bool useNormalisedSizeScale, double scale ) const
{
	pl->sfci( getFci() );
	//pl->sfontf(m_pointfont[i].mb_str());
	double size = useNormalisedSizeScale ? m_sizeScale.getSizeNormalisedScale( sizeParameter ) : m_sizeScale.getsize( sizeParameter );
	pl->schr( size, 1.0 );
	rgbcolour colour = useNormalisedColourScale ? m_colourScale.getRgbNormalisedScale( colourParameter ) : m_colourScale.getRgbOriginalScale( colourParameter );
	pl->scol0a( colourIndex, colour.r() * 255, colour.g() * 255, colour.b() * 255, colour.a() );
	pl->col0( colourIndex );
}
bool ColourAndSizeVaryingSymbol::isColourLogScaled() const
{
	return m_colourScale.isLogarithmic();
}
bool ColourAndSizeVaryingSymbol::isSizeLogScaled() const
{
	return m_sizeScale.isLogarithmic();
}
double ColourAndSizeVaryingSymbol::getSize(double parameter, bool useNormalisedScale) const
{
	return useNormalisedScale ? m_sizeScale.getSizeNormalisedScale(parameter) : m_sizeScale.getsize(parameter);
}

DrawableItem::DrawableItem( std::shared_ptr<splotTransformer> transformer )
	:m_transformer( transformer )
{
	m_scale = 1.0;
}

void DrawableItem::draw( plstream *pl, bool xLog, bool yLog )
{
	pl->stransform( (m_transformer ? splotTransform : NULL) , (void*)( m_transformer.get() ) );
	plotData( pl, xLog, yLog );
}

void XYAxisData::getLimits(  const std::vector<double> xs, const std::vector<double> &ys, double &xMin, double &xMax, double &yMin, double &yMax, double padAmount )
{
	xMin = std::numeric_limits<double>::max();
	yMin = std::numeric_limits<double>::max();
	xMax = -xMin;
	yMax = -yMin;
	
	if (xs.size() > 0)
	{
		const double *xi = &xs[0];
		const double *xEnd = xi + xs.size();
		for (; xi != xEnd; xi++)
		{
			if (*xi != std::numeric_limits<double>::infinity() && *xi != -std::numeric_limits<double>::infinity())
			{
				if (*xi < xMin)
					xMin = *xi;
				if (*xi > xMax)
					xMax = *xi;
			}
		}
	}
	if (ys.size() > 0)
	{
		const double *yi = &ys[0];
		const double *yEnd = yi + ys.size();
		for (; yi != yEnd; yi++)
		{
			if( *yi != std::numeric_limits<double>::infinity() && *yi != -std::numeric_limits<double>::infinity() )
			{
				if( *yi < yMin )
					yMin = *yi;
				if( *yi > yMax )
					yMax = *yi;
			}
		}
	}

	if( xMax < xMin )
		xMax = xMin = std::numeric_limits<double>::quiet_NaN();
	if( yMax < yMin )
		yMax = yMin = std::numeric_limits<double>::quiet_NaN();

	double xDiff = xMax - xMin;
	xMax += xDiff * padAmount;
	xMin -= xDiff * padAmount;
	double yDiff = yMax - yMin;
	yMax += yDiff * padAmount;
	yMin -= yDiff * padAmount;
}

void XYAxisData::getLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const
{
	if( ! m_calculatedLimits )
	{
		getLimits( m_xData, m_yData, m_xMin, m_xMax, m_yMin, m_yMax, m_padLimitsAmount );
		m_calculatedLimits = true;
	}
	xMin = m_xMin;
	xMax = m_xMax;
	yMin = m_yMin;
	yMax = m_yMax;
}

void XYAxisData::getLogLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const
{
	if( !m_calculatedLogLimits )
	{
		getLimits( m_xDataLogged, m_yDataLogged, m_xMinLogged, m_xMaxLogged, m_yMinLogged, m_yMaxLogged, m_padLimitsAmount );
		m_calculatedLogLimits = true;
	}
	xMin = m_xMinLogged;
	xMax = m_xMaxLogged;
	yMin = m_yMinLogged;
	yMax = m_yMaxLogged;
}

XYAxisData::XYAxisData( const std::vector<double> &xs, const std::vector<double> &ys, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount )
	:DrawableItem( transformer )
{
	m_xData = xs;
	m_yData = ys;
	m_xDataLogged = sci::log10( xs );
	m_yDataLogged = sci::log10( ys );
	m_padLimitsAmount = autoLimitsPadAmount;
	m_calculatedLimits = false;
	m_calculatedLogLimits = false;
}

PlotData1d::PlotData1d(const std::vector<double> &xs, const std::vector<double> &ys, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount)
	:XYAxisData(xs, ys, transformer, autoLimitsPadAmount)
{
	sci::assertThrow(xs.size() == ys.size(), sci::err());
}

PlotData2dLinear::PlotData2dLinear( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount )
	:PlotData1d( xs, ys, transformer, autoLimitsPadAmount )
{
	sci::assertThrow( zs.size() == xs.size(), sci::err() );
	m_zData = zs;
	m_zDataLogged = sci::log10( m_zData );
	double zMin = sci::min<double>( m_zData );
	double zRange = sci::max<double>( m_zData ) - zMin;
	m_zDataNormalised = ( m_zData - zMin ) / zRange;
	double zMinLogged = sci::min<double>( m_zDataLogged );
	double zRangeLogged = sci::max<double>( m_zDataLogged ) - zMinLogged;
	m_zDataLoggedNormalised = ( m_zDataLogged - zMinLogged ) / zRangeLogged;
}

PlotData3dLinear::PlotData3dLinear( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs1, const std::vector<double> &zs2, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount )
	:PlotData1d( xs, ys, transformer, autoLimitsPadAmount )
{
	sci::assertThrow( zs1.size() == xs.size(), sci::err() );
	sci::assertThrow( zs2.size() == xs.size(), sci::err() );
	m_zData1 = zs1;
	m_zDataLogged1 = sci::log10( m_zData1 );
	double zMin1 = sci::min<double>( m_zData1 );
	double zRange1 = sci::max<double>( m_zData1 ) - zMin1;
	m_zDataNormalised1 = ( m_zData1 - zMin1 ) / zRange1;
	double zMinLogged1 = sci::min<double>( m_zDataLogged1 );
	double zRangeLogged1 = sci::max<double>( m_zDataLogged1 ) - zMinLogged1;
	m_zDataLoggedNormalised1 = ( m_zDataLogged1 - zMinLogged1 ) / zRangeLogged1;
	
	m_zData2 = zs2;
	m_zDataLogged2 = sci::log10( m_zData2 );
	double zMin2 = sci::min<double>( m_zData2 );
	double zRange2 = sci::max<double>( m_zData2 ) - zMin2;
	m_zDataNormalised2 = ( m_zData2 - zMin2 ) / zRange2;
	double zMinLogged2 = sci::min<double>( m_zDataLogged2 );
	double zRangeLogged2 = sci::max<double>( m_zDataLogged2 ) - zMinLogged2;
	m_zDataLoggedNormalised2 = ( m_zDataLogged2 - zMinLogged2 ) / zRangeLogged2;

}

PlotData2dStructured::PlotData2dStructured( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<std::vector<double>> &zs, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount )
	:XYAxisData( xs, ys, transformer, autoLimitsPadAmount )
{

	m_zData = zs;
	m_zDataLogged = sci:: log10( zs );
	m_transformer = transformer;

	
	m_minZ = std::numeric_limits<double>::infinity();
	m_maxZ = -std::numeric_limits<double>::infinity();
	m_minZLogged = std::numeric_limits<double>::infinity();
	m_maxZLogged = -std::numeric_limits<double>::infinity();
	double *z;
	double *zEnd;
	for (size_t i = 0; i < m_zData.size(); ++i)
	{
		z = &m_zData[i][0];
		zEnd = z + m_zData[i].size();
		for (;z < zEnd;++z)
		{
			if (*z > m_maxZ && *z != std::numeric_limits<double>::infinity())
				m_maxZ = *z;
			if (*z < m_minZ && *z != -std::numeric_limits<double>::infinity())
				m_minZ = *z;
		}
	}
	for (size_t i = 0; i < m_zDataLogged.size(); ++i)
	{
		z = &m_zDataLogged[i][0];
		zEnd = z + m_zData[i].size();
		for (;z < zEnd;++z)
		{
			if (*z > m_maxZ && *z != std::numeric_limits<double>::infinity())
				m_maxZLogged = *z;
			if (*z < m_minZ && *z != -std::numeric_limits<double>::infinity())
				m_minZLogged = *z;
		}
	}
	if(m_minZ == std::numeric_limits<double>::infinity())
		m_minZ = std::numeric_limits<double>::quiet_NaN();
	if (m_maxZ == -std::numeric_limits<double>::infinity())
		m_maxZ = std::numeric_limits<double>::quiet_NaN();
	if (m_minZLogged == std::numeric_limits<double>::infinity())
		m_minZLogged = std::numeric_limits<double>::quiet_NaN();
	if (m_maxZLogged == -std::numeric_limits<double>::infinity())
		m_maxZLogged = std::numeric_limits<double>::quiet_NaN();
}
void PlotData2dStructured::getZLimits(double &min, double &max) const
{
	min = m_minZ;
	max = m_maxZ;
}
void PlotData2dStructured::getLogZLimits(double &min, double &max) const
{
	min = m_minZLogged;
	max = m_maxZLogged;
}

void getXYValues(double xIndex, double yIndex, double *xOutput, double *yOutput, void *data)
{
	PlotData2dStructured *plotData2DStructured = (PlotData2dStructured*)data;
	plotData2DStructured->getXYValues(xIndex, yIndex, *xOutput, *yOutput);
}

void PlotData2dStructured::getXYValues(double xIndex, double yIndex, double &xOutput, double &yOutput) const
{
	size_t xI = size_t(xIndex);
	size_t yI = size_t(yIndex);
	double xRemainder = xIndex - xI;
	double yRemainder = yIndex - yI;

	if (xRemainder == 0.0)
		xOutput = m_xData[xI];
	else
		xOutput = (m_xData[xI + 1] - m_xData[xI])*xRemainder + m_xData[xI];

	if (yRemainder == 0.0)
		yOutput = m_yData[yI];
	else
		yOutput = (m_yData[yI + 1] - m_yData[yI])*yRemainder + m_yData[yI];
}

LineData::LineData( const std::vector<double> &xs, const std::vector<double> &ys, const LineStyle &lineStyle, std::shared_ptr<splotTransformer> transformer )
	: PlotData1d( xs, ys, transformer ), m_lineStyle( lineStyle )
{
}
void LineData::plotData( plstream *pl, bool xLog, bool yLog ) const
{
	m_lineStyle.setupLineStyle( pl, 1, m_scale );
	const double *x = xLog ? &m_xDataLogged[0] : &m_xData[0];
	const double *y = yLog ? &m_yDataLogged[0] : &m_yData[0];
	
	pl->line( m_xData.size(), x, y );
	m_lineStyle.resetLineStyle( pl, 1 );
}

PointData::PointData( const std::vector<double> &x, const std::vector<double> &y, const Symbol &symbol, std::shared_ptr<splotTransformer> transformer )
	: PlotData1d( x, y, transformer ), m_symbol( symbol )
{
}
void PointData::plotData( plstream *pl, bool xLog, bool yLog ) const
{
	if (m_xData.size() == 0)
		return;
	m_symbol.setupSymbol( pl, 1, 1.0 );
	std::string symbol = m_symbol.getSymbol();
	const double *x = xLog ? &m_xDataLogged[0] : &m_xData[0];
	const double *y = yLog ? &m_yDataLogged[0] : &m_yData[0];
	if( symbol.length() > 0)
		//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
		pl->string(m_xData.size(),x,y,symbol.c_str());
}

PointDataColourVarying::PointDataColourVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const ColourVaryingSymbol &symbol, bool autoscaleColour, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount )
	:PlotData2dLinear( xs, ys, zs, transformer, autoLimitsPadAmount )
{
	m_symbol = symbol;
	m_autoscaleColour = autoscaleColour;
}

void PointDataColourVarying::plotData( plstream *pl, bool xLog, bool yLog ) const
{
	if (m_xData.size() == 0)
		return;
	std::string symbol = m_symbol.getSymbol();
	const double *x = xLog ? &m_xDataLogged[0] : &m_xData[0];
	const double *y = yLog ? &m_yDataLogged[0] : &m_yData[0];
	const double *z = m_symbol.isLogScaled() ? 
		( m_autoscaleColour ? &m_zDataLoggedNormalised[0] : &m_zDataLogged[0] )
		: ( m_autoscaleColour ? &m_zDataNormalised[0] : &m_zData[0] );
	if( symbol.length() > 0)
	{
		const double *xi = x;
		const double *yi = y;
		const double *zi = z;
		const double *xEnd = x + m_xData.size();
		for( ; xi != xEnd; ++xi, ++yi, ++zi )
		{
			m_symbol.setupSymbol( pl, 1, *zi, m_autoscaleColour, 1.0 );
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			pl->string(1,xi,yi,symbol.c_str());
		}
	}
}

PointDataSizeVarying::PointDataSizeVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zs, const SizeVaryingSymbol &symbol, bool autoscaleSize, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount )
	:PlotData2dLinear( xs, ys, zs, transformer, autoLimitsPadAmount )
{
	m_symbol = symbol;
	m_autoscaleSize = autoscaleSize;
}

void PointDataSizeVarying::plotData( plstream *pl, bool xLog, bool yLog ) const
{
	if (m_xData.size() == 0)
		return;
	std::string symbol = m_symbol.getSymbol();
	const double *x = xLog ? &m_xDataLogged[0] : &m_xData[0];
	const double *y = yLog ? &m_yDataLogged[0] : &m_yData[0];
	const double *z = m_symbol.isLogScaled() ? 
		( m_autoscaleSize ? &m_zDataLoggedNormalised[0] : &m_zDataLogged[0] )
		: ( m_autoscaleSize ? &m_zDataNormalised[0] : &m_zData[0] );
	if( symbol.length() > 0)
	{
		const double *xi = x;
		const double *yi = y;
		const double *zi = z;
		const double *xEnd = x + m_xData.size();
		for( ; xi != xEnd; ++xi, ++yi, ++zi )
		{
			m_symbol.setupSymbol( pl, 1, *zi, m_autoscaleSize, 1.0 );
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			if(m_symbol.getSize(*zi, m_autoscaleSize) > 0.0)
				pl->string(1,xi,yi,symbol.c_str());
		}
	}
}

PointDataColourAndSizeVarying::PointDataColourAndSizeVarying( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &zsColour, const std::vector<double> &zsSize, const ColourAndSizeVaryingSymbol &symbol, bool autoscaleColour, bool autoscaleSize, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount )
	:PlotData3dLinear( xs, ys, zsColour, zsSize, transformer, autoLimitsPadAmount )
{
	m_symbol = symbol;
	m_autoscaleColour = autoscaleColour;
	m_autoscaleSize = autoscaleSize;
}
void PointDataColourAndSizeVarying::plotData( plstream *pl, bool xLog, bool yLog ) const
{
	if (m_xData.size() == 0)
		return;
	std::string symbol = m_symbol.getSymbol();
	const double *x = xLog ? &m_xDataLogged[0] : &m_xData[0];
	const double *y = yLog ? &m_yDataLogged[0] : &m_yData[0];
	const double *zColour = m_symbol.isColourLogScaled() ? 
		( m_autoscaleSize ? &m_zDataLoggedNormalised1[0] : &m_zDataLogged1[0] )
		: ( m_autoscaleSize ? &m_zDataNormalised1[0] : &m_zData1[0] );
	const double *zSize = m_symbol.isSizeLogScaled() ? 
		( m_autoscaleSize ? &m_zDataLoggedNormalised2[0] : &m_zDataLogged2[0] )
		: ( m_autoscaleSize ? &m_zDataNormalised2[0] : &m_zData2[0] );
	if( symbol.length() > 0)
	{
		const double *xi = x;
		const double *yi = y;
		const double *zColouri = zColour;
		const double *zSizei = zSize;
		const double *xEnd = x + m_xData.size();
		for( ; xi != xEnd; ++xi, ++yi, ++zColouri, ++zSizei )
		{
			m_symbol.setupSymbol( pl, 1, *zColouri, *zSizei, m_autoscaleColour, m_autoscaleSize, 1.0 );
			//pl->poin(m_xs[i].size(),x,y,m_pointchar[i][0]);
			if (m_symbol.getSize(*zSizei, m_autoscaleSize) > 0.0)
				pl->string(1,xi,yi,symbol.c_str());
		}
	}
}

HorizontalErrorBars::HorizontalErrorBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &plusErrors, const std::vector<double> minusErrors, const LineStyle style )
	:PlotData3dLinear(xs, ys, xs+plusErrors, xs-minusErrors)
{
	m_style = style;
}
void HorizontalErrorBars::plotData( plstream *pl, bool xLog, bool yLog ) const
{
	if (m_yData.size() == 0)
		return;
	const double *y = yLog ? &m_yDataLogged[0] : &m_yData[0];
	const double *xMinusErrors = xLog ? &m_zDataLogged1[0] : &m_zData1[0];
	const double *xPlusErrors = xLog ? &m_zDataLogged2[0] : &m_zData2[0];

	m_style.setupLineStyle( pl, 1, m_scale );	
	pl->errx( m_xData.size(),xMinusErrors, xPlusErrors, y );
	m_style.resetLineStyle( pl, 1 );
}

VerticalErrorBars::VerticalErrorBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &plusErrors, const std::vector<double> minusErrors, const LineStyle style )
	:PlotData3dLinear(xs, ys, ys+plusErrors, ys-minusErrors)
{
	m_style = style;
}
void VerticalErrorBars::plotData( plstream *pl, bool xLog, bool yLog ) const
{
	if (m_xData.size() == 0)
		return;
	const double *x = xLog ? &m_xDataLogged[0] : &m_xData[0];
	const double *yMinusErrors = yLog ? &m_zDataLogged1[0] : &m_zData1[0];
	const double *yPlusErrors = yLog ? &m_zDataLogged2[0] : &m_zData2[0];

	m_style.setupLineStyle( pl, 1, m_scale );	
	pl->erry( m_xData.size(), x, yMinusErrors, yPlusErrors );
	m_style.resetLineStyle( pl, 1 );
}

VerticalBars::VerticalBars( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &widths, const LineStyle &lineStyle, const FillStyle &fillStyle, double zeroLine, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount )
	:PlotData2dLinear( xs - 0.5 * widths, ys, xs  + 0.5 * widths, transformer, autoLimitsPadAmount )
{
	m_lineStyle = lineStyle;
	m_fillStyle = fillStyle;
	m_zeroLine = zeroLine;
	m_zeroLineLogged = zeroLine > 0.0 ? std::log( m_zeroLine ) : std::numeric_limits<double>::quiet_NaN();
	m_padLimitsAmount = m_padLimitsAmount;
}
void VerticalBars::plotData( plstream *pl, bool xLog, bool yLog ) const
{
	if (m_xData.size() == 0)
		return;
	double x[4];
	double y[4];
	for(size_t i=0; i<m_xData.size(); ++i)
	{
		if( xLog )
		{
			x[0] = x[1] = m_xDataLogged[i];
			x[2] = x[3] = m_zDataLogged[i];
		}
		else
		{
			x[0] = x[1] = m_xData[i];
			x[2] = x[3] = m_zData[i];
		}
		if( yLog )
		{
			y[0] = y[3] = m_zeroLineLogged;
			y[1] = y[2] = m_yDataLogged[i];
		}
		else
		{
			y[0] = y[3] = m_zeroLine;
			y[1] = y[2] = m_yData[i];
		}
		

		//do fill first
		m_fillStyle.setupFillStyle( pl, 1, 1.0 );
		pl->fill(4,&x[0],&y[0]);
		
		m_lineStyle.setupLineStyle( pl, 1, 1.0 );
		pl->line( 4, x, y );
	}
}

void VerticalBars::getLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const
{
	xMin = sci::min<double>( m_xData );
	xMax = sci::max<double>( m_zData );
	yMin = sci::min<double>( m_yData );
	yMax = sci::max<double>( m_yData );

	double xRange = xMax - xMin;
	xMin -= xRange * m_padLimitsAmount;
	xMax += xRange * m_padLimitsAmount;

	if( yMin >= m_zeroLine )
	{
		yMin = m_zeroLine;
		double yRange = yMax - yMin;
		yMax += yRange * m_padLimitsAmount;
	}
	else
	{
		double yRange = yMax - yMin;
		yMin -= yRange * m_padLimitsAmount;
		yMax += yRange * m_padLimitsAmount;
	}
}
void VerticalBars::getLogLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const
{
	xMin = sci::min<double>( m_xDataLogged );
	xMax = sci::max<double>( m_zDataLogged );
	yMin = sci::min<double>( m_yDataLogged );
	yMax = sci::max<double>( m_yDataLogged );

	double xRange = xMax - xMin;
	xMin -= xRange * m_padLimitsAmount;
	xMax += xRange * m_padLimitsAmount;

	if( yMin >= m_zeroLineLogged )
	{
		yMin = m_zeroLineLogged;
		double yRange = yMax - yMin;
		yMax += yRange * m_padLimitsAmount;
	}
	else
	{
		double yRange = yMax - yMin;
		yMin -= yRange * m_padLimitsAmount;
		yMax += yRange * m_padLimitsAmount;
	}
}

GridData::GridData(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<std::vector<double>> &zs, const splotcolourscale &colourScale, bool fillOffScaleBottom, bool fillOffScaleTop, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount)
	:PlotData2dStructured(xs, ys, zs, transformer, autoLimitsPadAmount)
{
	sci::assertThrow(xs.size() == zs.size() + 1, sci::err());
	if (zs.size() == 0)
		sci::assertThrow(ys.size() == 0, sci::err());
	else
	{
		sci::assertThrow(ys.size() == zs[0].size() + 1, sci::err());
		for (size_t i = 1; i < zs.size(); ++i)
			sci::assertThrow(zs[i].size() == zs[0].size(), sci::err());
	}

	m_colourscale = colourScale;
	m_fillOffscaleBottom = fillOffScaleBottom;
	m_fillOffscaleTop = fillOffScaleTop;
}

void GridData::plotData(plstream *pl, bool xLog, bool yLog) const
{
	if (m_zData.size() == 0 || m_zData[0].size() == 0)
		return;

	//set up a 2d double ** style array for the z data
	std::vector<const double*> zs(m_zData.size());
	if (m_colourscale.isLogarithmic())
	{
		for (size_t i = 0; i < zs.size(); ++i)
			zs[i] = &(m_zDataLogged[i][0]);
	}
	else
	{
		for (size_t i = 0; i < zs.size(); ++i)
			zs[i] = &(m_zData[i][0]);
	}

	//set up the colourscale
	m_colourscale.setup(pl);

	m_plotLogX = xLog;
	m_plotLogY = yLog;
	double zMin = m_fillOffscaleBottom ? -std::numeric_limits<double>::infinity() : m_colourscale.getMin();
	double zMax = m_fillOffscaleTop ? std::numeric_limits<double>::infinity() : m_colourscale.getMax();
	pl->imagefr(&zs[0], m_zData.size(), m_zData[0].size(), 0, m_xData.size() - 1, 0, m_yData.size() - 1,
		zMin, zMax, m_colourscale.getMin(), m_colourscale.getMax(), &(::getXYValues), (void*)this);
}

FillData::FillData(const std::vector<double> &xs, const std::vector<double> &ys, const FillStyle &fillStyle, const LineStyle &outlineStyle, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount)
	:PlotData1d(xs, ys, transformer, autoLimitsPadAmount), m_fillStyle(fillStyle), m_outlineStyle(outlineStyle)
{

}

void FillData::plotData(plstream *pl, bool xLog, bool yLog) const
{
	const double *x = xLog ? &m_xDataLogged[0] : &m_xData[0];
	const double *y = yLog ? &m_yDataLogged[0] : &m_yData[0];
	m_outlineStyle.setupLineStyle(pl, 1, m_scale);
	pl->line(m_xData.size(), x, y);
	m_outlineStyle.resetLineStyle(pl, 1);

	m_fillStyle.setupFillStyle(pl, 1, m_scale);
	pl->fill(m_xData.size(), x, y);

	m_fillStyle.resetFillStyle(pl, 1); //just so we don't have residually selected hatching patterns
}