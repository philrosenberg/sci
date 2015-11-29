#include"../include/svector/transforms.h"
#include "../include/svector/dep/math.h"

const double PI=3.14159265359;

/*void splotmapform(void *userp, int npoints, double *x, double *y)
{
	splotcoordtransformer *transformer=(splotcoordtransformer*)userp;
	double newx;
	double newy;
	for(int i=0; i<npoints; ++i)
	{
		transformer->xform(x[i], y[i], newx, newy);
		x[i]=newx;
		y[i]=newy;
	}
}*/

//constructor when using 1d x and y data
splotindextransformer1d::splotindextransformer1d(std::vector<double> *x,std::vector<double> *y, void (*transformfunc)(double xindex, double yindex, const std::vector<double> &x, const std::vector<double> &y, double &xout, double &yout))
{
	if(x==NULL || y==NULL || transformfunc==NULL)
	{
		m_x=NULL;
		m_y=NULL;
		m_transformfunc=NULL;
	}
	else
	{
		m_x=x;
		m_y=y;
		m_transformfunc=transformfunc;
	}
}

//constructor when using 2d x and y data
splotindextransformer2d::splotindextransformer2d(std::vector< std::vector< double > > *x, std::vector< std::vector< double > > *y, void (*transformfunc)(double xindex, double yindex, const std::vector< std::vector< double > > &x, const std::vector< std::vector< double > > &y, double &xout, double &yout))
{
	if(x==NULL || y==NULL || transformfunc==NULL)
	{
		m_x=NULL;
		m_y=NULL;
		m_transformfunc=NULL;
	}
	else
	{
		m_x=x;
		m_y=y;
		m_transformfunc=transformfunc;
	}
}
	
void splotindextransformer1d::xform( PLFLT ox, PLFLT oy, PLFLT& nx, PLFLT& ny )const
{
	//ox is oldx, i.e the index
	//nx is newx i.e. the actual x value
	//same for y

	//we set our values using m_transformfunc or if we are uninitialised just return the indices
	if(m_transformfunc!=NULL)m_transformfunc(ox,oy,*m_x,*m_y,nx,ny);
	else
	{
		nx=ox;
		ny=oy;
	}
}
	
void splotindextransformer2d::xform( PLFLT ox, PLFLT oy, PLFLT& nx, PLFLT& ny )const
{
	//ox is oldx, i.e the index
	//nx is newx i.e. the actual x value
	//same for y

	//we set our values using m_transformfunc or if we are uninitialised just return the indices
	if(m_transformfunc!=NULL)m_transformfunc(ox,oy,*m_x,*m_y,nx,ny);
	else
	{
		nx=ox;
		ny=oy;
	}
}


void splotTransform(double oldX, double oldY, double *newX, double *newY, void* userP)
{
	splotTransformer *transformer=(splotTransformer*) userP;
	transformer->transform(oldX, oldY, *newX, *newY);
}


GeosTransformer::GeosTransformer(double viewingLon, bool transparent, double polarRadius, double equatorialRadius, double orbitRadius)
{
	m_viewingLon=viewingLon*PI/180.0;
	m_transparent=transparent;
	m_polarRadiusSq=polarRadius*polarRadius;
	m_equatorialRadiusSq=equatorialRadius*equatorialRadius;
	m_orbitRadius=orbitRadius;
}

void::GeosTransformer::transform(double lon, double lat, double &xAng, double &yAng) const
{
	//RadialRadius is the radial distance outward of the point from the Earth's axis
	//x is the distance of the point along the axis from the earth centre to the satellite
	//z is the distance of the point along the axis aligned with the earths axis
	//y is the distance along the axis perp the the other two
	lon*=PI/180.0;
	lat*=PI/180.0;
	double theta=lon-m_viewingLon;
	if( !m_transparent && (theta>PI/2.0 || theta<-PI/2.0))
	{
		xAng=std::numeric_limits<double>::quiet_NaN();
		yAng=std::numeric_limits<double>::quiet_NaN();
		return;
	}

	double RadialRadiusSq=m_polarRadiusSq*m_equatorialRadiusSq/(m_polarRadiusSq+m_equatorialRadiusSq*std::pow(std::tan(lat),2));
	double RadialRadius=std::sqrt(RadialRadiusSq);

	double z=sqrt(m_polarRadiusSq*(1.0-RadialRadiusSq/m_equatorialRadiusSq));
	if(lat<0.0)
		z=-z;
	double x=RadialRadius*std::cos(theta);
	double y=RadialRadius*std::sin(theta);

	yAng=std::atan(z/(m_orbitRadius-x))*180.0/PI;
	xAng=std::atan(y/(m_orbitRadius-x))*180.0/PI;
}


