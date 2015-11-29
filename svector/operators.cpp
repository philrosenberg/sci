#include"operators_internal.h"
#include<limits>
	
std::vector<SBOOL> operator||(const std::vector<SBOOL> &a, const std::vector<SBOOL> &b)
{
	std::vector <SBOOL> result(std::max(a.size(),b.size()),std::numeric_limits<SBOOL>::quiet_NaN());
	size_t len=std::min(a.size(),b.size());
	for(size_t i=0; i<len; i++) result[i]=a[i]|b[i];
	return result;
}

std::vector< std::vector<SBOOL> > operator||(const std::vector< std::vector<SBOOL> > &a, const std::vector< std::vector<SBOOL> > &b)
{
	std::vector< std::vector <SBOOL> > result(std::max(a.size(),b.size()));
	size_t len=std::min(a.size(),b.size());
	for(size_t i=0; i<len; i++) result[i]=a[i]||b[i];
	return result;
}

std::vector<SBOOL> operator&&(const std::vector<SBOOL> &a, const std::vector<SBOOL> &b)
{
	std::vector <SBOOL> result(std::max(a.size(),b.size()),std::numeric_limits<SBOOL>::quiet_NaN());
	size_t len=std::min(a.size(),b.size());
	for(size_t i=0; i<len; i++) result[i]=(a[i]&b[i])==0?0:1;
	return result;
}

std::vector< std::vector<SBOOL> > operator&&(const std::vector< std::vector<SBOOL> > &a, const std::vector< std::vector<SBOOL> > &b)
{
	std::vector< std::vector <SBOOL> > result(std::max(a.size(),b.size()));
	size_t len=std::min(a.size(),b.size());
	for(size_t i=0; i<len; i++) result[i]=a[i]&&b[i];
	return result;
}

std::vector<SBOOL> operator||(const std::vector<SBOOL> &a, SBOOL b)
{
	size_t len=a.size();
	std::vector <SBOOL> result(len);
	for(size_t i=0; i<len; i++) result[i]=a[i]|b;
	return result;
}

std::vector<SBOOL> operator&&(const std::vector<SBOOL> &a, SBOOL b)
{
	size_t len=a.size();
	std::vector <SBOOL> result(len);
	for(size_t i=0; i<len; i++) result[i]=(a[i]&b)==0?0:1;
	return result;
}

std::vector<SBOOL> operator||(SBOOL a, const std::vector<SBOOL> &b)
{
	size_t len=b.size();
	std::vector <SBOOL> result(len);
	for(size_t i=0; i<len; i++) result[i]=a|b[i];
	return result;
}

std::vector<SBOOL> operator&&(SBOOL a, const std::vector<SBOOL> &b)
{
	size_t len=b.size();
	std::vector <SBOOL> result(len);
	for(size_t i=0; i<len; i++) result[i]=(a&b[i])==0?0:1;
	return result;
}

std::vector<SBOOL> operator!(const std::vector<SBOOL> &a)
{
	size_t len=a.size();
	std::vector<SBOOL> result(len);
	for(size_t i=0; i<len; i++) result[i]=a[i]==0?1:0;
	return result;
}
