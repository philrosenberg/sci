#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include <vector>

void splotmapform(void *userp, int npoints, double *x, double *y);


//these are for applying arbitrary x and y transforms
//currently they probably have problems for log axes

//this is the function interface for PlPlot
void splotTransform(double oldX, double oldY, double *newX, double *newY, void* userP);

class splotTransformer
{
public:
	virtual ~splotTransformer(){}
	//receives the x/y index as a float - may be between indices
	//outputs the x and y values at the given indices
	virtual void transform( double oldx, double oldy, double& newx, double& newy )const = 0;
	virtual bool isrectilinear()const {return false;};
	virtual bool isNull() const {return false;}
};

class NullTransformer : public splotTransformer
{
public:
	void transform( double oldx, double oldy, double& newx, double& newy )const {newx=oldx; newy=oldy;}
	bool isNull() const {return true;}
};

class GeosTransformer : public splotTransformer
{
public:
	GeosTransformer(double viewingLon, bool transparent=false, double polarRadius=6356583.8, double equatorialRadius=6378169.0, double orbitRadius=42164000);
	void transform(double lon, double lat, double &xAng, double &yAng) const;
private:
	double m_viewingLon;
	double m_polarRadiusSq;
	double m_equatorialRadiusSq;
	double m_orbitRadius;
	bool m_transparent;
};


#endif