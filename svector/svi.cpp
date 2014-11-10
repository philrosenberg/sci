#include "../include/svector/svi.h"

void svi::vectortoalg(const std::vector<double> &src, alglib::real_1d_array &dst)
{
	dst.setcontent(src.size(),&src[0]);
}
void svi::algtovector(const alglib::real_1d_array &src, std::vector<double> &dst)
{
	dst.resize(src.length());
	memcpy(&dst[0],src.getcontent(),src.length()*sizeof(double));
}
void svi::vectorstoalgc(const std::vector<double> &re_src, const std::vector<double> &im_src, alglib::complex_1d_array &dst)
{
	if(re_src.size()!=im_src.size()) return;
	dst.setlength(re_src.size());
	for(size_t i=0; i<re_src.size(); ++i)
	{
		dst[i].x=*(re_src.begin()+i);
		dst[i].y=*(im_src.begin()+i);
	}
}
void svi::algctovectors(const alglib::complex_1d_array &src, std::vector<double> &re_dst, std::vector<double> &im_dst)
{
	re_dst.resize(src.length());
	im_dst.resize(src.length());
	double *re_dst_begin=&re_dst[0];
	double *im_dst_begin=&im_dst[0];
	for(size_t i=0; i<re_dst.size(); ++i)
	{
		*(re_dst_begin+i)=src[i].x;
		*(im_dst_begin+i)=src[i].y;
	}
}
void svi::vectorctoalgc(const std::vector<std::complex<double>> &src, alglib::complex_1d_array &dst)
{
	dst.setlength(src.size());
	const std::complex<double> *src_begin=&src[0];
	for(size_t i=0; i<src.size(); ++i)
	{
		const std::complex<double> *srci=src_begin+i;
		dst[i].x=srci->real();
		dst[i].y=srci->imag();
	}
}
void svi::algctovectorc(const alglib::complex_1d_array &src, std::vector<std::complex<double>> &dst)
{
	dst.resize(src.length());
	std::complex<double> *dst_begin=&dst[0];
	for(size_t i=0; i<dst.size(); ++i)
	{
		std::complex<double> *dsti=dst_begin+i;
		*dsti=std::complex<double>(src[i].x,src[i].y);
	}
}


void svi::transposedvectortoalg(const std::vector< std::vector<double> > &src, alglib::real_2d_array &dst)
{
	dst.setlength(src[0].size(),src.size());
	for(size_t i=0; i<src[0].size(); ++i)
	{
		for(size_t j=0; j<src.size(); ++j) dst[i][j]=src[j][i];
	}
}


void svi::transposedalgtovector(const alglib::real_2d_array &src, std::vector< std::vector<double> > &dst)
{
	dst.resize(src.rows());
	for(alglib::ae_int_t i=0; i<src.rows(); ++i)
	{
		dst[i].resize(src.cols());
		for(alglib::ae_int_t j=0; j<src.cols(); ++j) dst[i][j]=src[j][i];
	}
}


void svi::vectortoalg(const std::vector< std::vector<double> > &src, alglib::real_2d_array &dst)
{
	dst.setlength(src[0].size(),src.size());
	for(size_t i=0; i<src[0].size(); ++i)
	{
		for(size_t j=0; j<src.size(); ++j) dst[i][j]=src[i][j];
	}
}


void svi::algtovector(const alglib::real_2d_array &src, std::vector< std::vector<double> > &dst)
{
	dst.resize(src.rows());
	for(alglib::ae_int_t i=0; i<src.rows(); ++i)
	{
		dst[i].resize(src.cols());
		for(alglib::ae_int_t j=0; j<src.cols(); ++j) dst[i][j]=src[i][j];
	}
}

