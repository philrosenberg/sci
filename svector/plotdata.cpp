#include "../include/svector/plotdata.h"
#include "../include/svector/serr.h"

LineStyle::LineStyle( double width, const rgbcolour &colour, const std::vector<PLINT> &marks, const std::vector<PLINT> &spaces )
	: m_width( 1.0 ), m_colour(  0.0, 0.0, 0.0, 1.0 ), m_marks( marks ), m_spaces( spaces )
{
	sci::assertThrow(m_marks.size() == m_spaces.size(), sci::err() );
}
LineStyle::LineStyle( double width, const rgbcolour &colour, std::string pattern )
	: m_width( 1.0 ), m_colour(  0.0, 0.0, 0.0, 1.0 )
{
	parseLineStyle( pattern, m_marks, m_spaces);
}

double LineStyle::getWidth()
{
	return m_width;
}
void LineStyle::getPattern( std::vector<PLINT> &marks, std::vector<PLINT> &spaces )
{
	marks = m_marks;
	spaces = m_spaces;
}
rgbcolour LineStyle::getColour()
{
	return m_colour;
}
void LineStyle::setupLineStyle( plstream *pl, PLINT colourIndex, double scale )
{
	pl->scol0a( colourIndex, m_colour.r(), m_colour.g(), m_colour.b(), m_colour.a() );
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
void LineStyle::resetLineStyle( plstream *pl, PLINT colourIndex )
{
	pl->scol0a( colourIndex, 0, 0, 0, 1.0 );
	pl->styl(0,NULL, NULL);
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

DrawableItem::DrawableItem( std::shared_ptr<splotTransformer> transformer )
	:m_transformer( transformer )
{
}

void DrawableItem::draw( plstream *pl )
{
	pl->stransform( (m_transformer ? splotTransform : NULL) , (void*)( m_transformer.get() ) );
	plotData( pl );
}

void PlotData1d::getLimits(  const std::vector<double> xs, const std::vector<double> &ys, double &xMin, double &xMax, double &yMin, double &yMax, double padAmount )
{
	xMin = std::numeric_limits<double>::max();
	yMin = std::numeric_limits<double>::max();
	xMax = -xMax;
	yMax = -yMax;
	
	if( xs.size() > 0 )
	{
		const double *xi = &xs[0];
		const double *yi = &ys[0];
		const double *xEnd = xi + xs.size();
		for( ; xi != xEnd; xi++, yi++ )
		{
			if( *xi != std::numeric_limits<double>::infinity() && *xi != -std::numeric_limits<double>::infinity() )
			{
				if( *xi < xMin )
					xMin = *xi;
				if( *xi > xMax )
					xMax = *xi;
			}
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

void PlotData1d::getLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const
{
	getLimits( m_xData, m_yData, xMin, xMax, yMin, yMax, m_padLimitsAmount );
}

void PlotData1d::getLogLimits( double &xMin, double &xMax, double &yMin, double &yMax ) const
{
	getLimits( m_xDataLogged, m_yDataLogged, xMin, xMax, yMin, yMax, m_padLimitsAmount );
}

PlotData1d::PlotData1d( const std::vector<double> &xs, const std::vector<double> &ys, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount )
	:DrawableItem( transformer )
{
	sci::assertThrow( xs.size() == ys.size(), sci::err() );
	m_xData = xs;
	m_yData = ys;
	m_xDataLogged = sci::log10( xs );
	m_yDataLogged = sci::log10( ys );
	m_padLimitsAmount = autoLimitsPadAmount;
}

PlotData2dStructured::PlotData2dStructured( const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<std::vector<double>> &zs, std::shared_ptr<splotTransformer> transformer, double autoLimitsPadAmount )
	:PlotData1d( xs, ys, transformer, autoLimitsPadAmount )
{
	m_zData = zs;
	m_zDataLogged = sci:: log10( zs );
	sci::assertThrow( xs.size() == zs.size(), sci::err() );
	for( size_t i=1; i<m_zData.size(); ++i )
		sci::assertThrow( zs[i].size() == zs[0].size(), sci::err() );
	if( zs.size() > 0 )
		sci::assertThrow( zs[0].size() ==ys.size(), sci::err() );
}

LineData::LineData( const std::vector<double> &xs, const std::vector<double> &ys, const LineStyle &lineStyle, std::shared_ptr<splotTransformer> transformer )
	: PlotData1d( xs, ys, transformer ), m_lineStyle( lineStyle )
{
	m_xData = xs;
	m_yData = ys;
	m_transformer = transformer;
}
void LineData::plotData( plstream *pl )
{
	m_lineStyle.setupLineStyle( pl, 1, m_scale );
	pl->line( m_xData.size(), &m_xData[0], &m_yData[0] );
	m_lineStyle.resetLineStyle( pl, 1 );
}