#include"../include/svector/dep/plot/transforms.h"
#include<limits>
#include<cmath>

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


