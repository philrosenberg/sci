#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#pragma warning(push, 0)
#include <plplot/plstream.h>
#pragma warning(pop)
#include <vector>

void splotmapform(void *userp, int npoints, double *x, double *y);

//these are for transforming from index to x/y values
class splotindextransformer : public Coord_Xformer
{
public:
	//receives the x/y index as a float - may be between indices
	//outputs the x and y values at the given indices
	virtual void xform( PLFLT ox, PLFLT oy, PLFLT& nx, PLFLT& ny )const {nx=ox; ny=oy;}
	virtual bool isrectilinear()const {return false;};
};

class splotindextransformer1d : public splotindextransformer
{
	friend class splot2d;
	friend class splotlegend;
public:
	//receives the x/y index as a float - may be between indices
	//outputs the x and y values at the given indices
	void xform( PLFLT ox, PLFLT oy, PLFLT& nx, PLFLT& ny )const;
	bool isrectilinear()const {return true;}
private:
	splotindextransformer1d(std::vector< double > *x,std::vector< double > *y, void (*transformfunc)(double xindex, double yindex, const std::vector<double> &x, const std::vector<double> &y, double &xout, double &yout)=NULL);
	std::vector< double > *m_x;
	std::vector< double > *m_y;
	//function called by xform. receives the x/y index as a float - may be between indices
	//the m_x and m_y values from this object are used for x and y
	//outputs the x and y values at the given indices
	void (*m_transformfunc)(double xindex, double yindex, const std::vector< double > &x, const std::vector< double > &y, double &xout, double &yout);
};

class splotindextransformer2d : public splotindextransformer
{
	friend class splot2d;
	friend class splotlegend;
public:
	//receives the x/y index as a float - may be between indices
	//outputs the x and y values at the given indices
	void xform( PLFLT ox, PLFLT oy, PLFLT& nx, PLFLT& ny )const;
	bool isrectilinear()const {return false;}
private:
	splotindextransformer2d(std::vector< std::vector< double > > *x, std::vector< std::vector< double > > *y, void (*transformfunc)(double xindex, double yindex, const std::vector< std::vector< double > > &x, const std::vector< std::vector< double > > &y, double &xout, double &yout)=NULL);
	std::vector< std::vector< double > > *m_x;
	std::vector< std::vector< double > > *m_y;
	//function called by xform. receives the x/y index as a float - may be between indices
	//the m_x and m_y values from this object are used for x and y
	//outputs the x and y values at the given indices
	void (*m_transformfunc)(double xindex, double yindex, const std::vector< std::vector< double > > &x, const std::vector< std::vector< double > > &y, double &xout, double &yout);
};

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