#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include <svector/math.h>
#include<limits>

void splotmapform(void* userp, int npoints, double* x, double* y);


//these are for applying arbitrary x and y transforms
//currently they probably have problems for log axes

//this is the function interface for PlPlot
void splotTransform(double oldX, double oldY, double* newX, double* newY, void* userP);

class splotTransformer
{
public:
	virtual ~splotTransformer() {}
	//receives the x/y index as a float - may be between indices
	//outputs the x and y values at the given indices
	virtual void transform(double oldx, double oldy, double& newx, double& newy)const = 0;
	virtual bool isrectilinear()const { return false; };
	virtual bool isNull() const { return false; }
};

class NullTransformer : public splotTransformer
{
public:
	void transform(double oldx, double oldy, double& newx, double& newy)const
	{
		newx = oldx;
		newy = oldy;
	}
	bool isNull() const override
	{
		return true;
	}
};

class GeosTransformer : public splotTransformer
{
public:
	GeosTransformer(double viewingLon, bool transparent = false, double polarRadius = 6356583.8, double equatorialRadius = 6378169.0, double orbitRadius = 42164000)
	{
		m_viewingLon = viewingLon * sci::m_pi / 180.0;
		m_transparent = transparent;
		m_polarRadiusSq = polarRadius * polarRadius;
		m_equatorialRadiusSq = equatorialRadius * equatorialRadius;
		m_orbitRadius = orbitRadius;
	}
	void transform(double lon, double lat, double& xAng, double& yAng) const
	{
		//RadialRadius is the radial distance outward of the point from the Earth's axis
		//x is the distance of the point along the axis from the earth centre to the satellite
		//z is the distance of the point along the axis aligned with the earths axis
		//y is the distance along the axis perp the the other two
		lon *= sci::m_pi / 180.0;
		lat *= sci::m_pi / 180.0;
		double theta = lon - m_viewingLon;
		if (!m_transparent && (theta > sci::m_pi / 2.0 || theta < -sci::m_pi / 2.0))
		{
			xAng = std::numeric_limits<double>::quiet_NaN();
			yAng = std::numeric_limits<double>::quiet_NaN();
			return;
		}

		double RadialRadiusSq = m_polarRadiusSq * m_equatorialRadiusSq / (m_polarRadiusSq + m_equatorialRadiusSq * std::pow(std::tan(lat), 2));
		double RadialRadius = std::sqrt(RadialRadiusSq);

		double z = sqrt(m_polarRadiusSq * (1.0 - RadialRadiusSq / m_equatorialRadiusSq));
		if (lat < 0.0)
			z = -z;
		double x = RadialRadius * std::cos(theta);
		double y = RadialRadius * std::sin(theta);

		yAng = std::atan(z / (m_orbitRadius - x)) * 180.0 / sci::m_pi;
		xAng = std::atan(y / (m_orbitRadius - x)) * 180.0 / sci::m_pi;
	}
private:
	double m_viewingLon;
	double m_polarRadiusSq;
	double m_equatorialRadiusSq;
	double m_orbitRadius;
	bool m_transparent;
};


#endif