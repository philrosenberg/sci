#include "../include/svector/ppfile.h"
#include<assert.h>
#include<vector>
#include<cmath>
#include<limits>
#include"../include/svector/wgdos.h"
#include<algorithm>

UmFile::UmFile(std::string name)
{
	m_comparator=compForecastPeriod;
	m_bigEndian=false;

	m_fin.open(name.c_str(), std::ios::in|std::ios::binary);
	if(!m_fin.is_open())
		throw(PPERR_CANNOT_OPEN_FILE);

	bool isPp; //Decide whether this is a pp (sequential) or UM (indexed tabled) file

	//read the size of the first record, this should be the 256 word fixed length header for a pp file
	__int32 headerSize=getNextRecordSize();
	if(headerSize!=256)
	{
		m_bigEndian=true;
		swapEndian(headerSize);
		if(headerSize!=256)
			isPp=true;
			//throw(PPERR_CANNOT_DETERMINE_ENDIANNESS);
		else
			isPp=false;
	}
	else
		isPp = true;
	while (1)
	{
		Section section;
		readRecord((__int32*)&section.m_header,sizeof(section.m_header));
		section.m_dataBytes=getNextRecordSize();
		section.m_dataStart=m_fin.tellg();
		section.m_parent=this;
		skipRecord(section.m_dataBytes);
		if(m_fin.eof())
			break;
		m_sections.push_back(section);

		__int32 headerSize=getNextRecordSize();

		if(m_fin.eof())
			break;
	}
	m_filteredSections=m_sections;
}

void UmFile::readRecord(__int32* record, std::basic_istream<char>::pos_type nBytes)
{
	m_fin.read((char*)record,nBytes);
	if(m_fin.gcount()!=nBytes)
		throw(PPERR_REACHED_FILE_END_UNEXPECTEDLY);
	if(m_bigEndian)
		swapEndian(record,nBytes/4);
	__int32 size;
	m_fin.read((char*)&size,sizeof(size));
	if(m_bigEndian)
		swapEndian(size);
	assert(size==nBytes);
	if(size!=nBytes)
		throw(PPERR_RECORD_WRONG_LENGTH);
}

void UmFile::skipRecord(std::basic_istream<char>::pos_type nBytes)
{
	m_fin.seekg(m_fin.tellg()+nBytes);
	__int32 size;
	m_fin.read((char*)&size,sizeof(size));
	if(m_bigEndian)
		swapEndian(size);
	assert(size==nBytes);
	if(size!=nBytes)
		throw(PPERR_RECORD_WRONG_LENGTH);
}

__int32 UmFile::getNextRecordSize()
{
	__int32 size;
	m_fin.read((char*)&size,sizeof(size));
	if(m_bigEndian)
		swapEndian(size);
	return size;
}

void UmFile::decompressWgdos(char *dataIn, size_t nIn, double** dataOut, size_t outDim1, size_t outDim2)
{
	size_t nread=0;
	char *pos=dataIn;

	__int32 packedLength=*((__int32*)pos);
	pos+=sizeof(__int32);
	nread+=sizeof(__int32);

	__int32 precisionPower=*((__int32*)pos);
	double precision=std::pow(2.0,precisionPower);
	pos+=sizeof(__int32);
	nread+=sizeof(__int32);
	
	__int16 nRows=*((__int16*)pos);
	pos+=sizeof(__int16);
	nread+=sizeof(__int16);
	
	__int16 nPointsInRow=*((__int16*)pos);
	pos+=sizeof(__int16);
	nread+=sizeof(__int16);

	if(nRows!=outDim1 || nPointsInRow!=outDim2)
		throw(PPERR_COMPRESSED_DATA_HAS_WRONG_SIZE);
	std::vector<char> missingMask(nPointsInRow,0);
	std::vector<char> minimumMask(nPointsInRow,0);
	std::vector<char> zeroMask(nPointsInRow,0);

	for(__int16 i=0; i<nRows; ++i)
	{
		float baseValue=fromIbmFloat(pos);
		pos+=sizeof(float);
		nread+=sizeof(float);

		__int16 nWords=*((__int16*)pos);
		pos+=sizeof(__int16);
		nread+=sizeof(__int16);

		__int16 nBitsWithFlags=*((__int16*)pos);
		pos+=sizeof(__int16);
		nread+=sizeof(__int16);

		//split the nbits int
		bool hasMissingMask=(nBitsWithFlags & 0x0020)!=0;
		bool hasMinimumMask=(nBitsWithFlags & 0x0040)!=0;
		bool hasZeroMask=(nBitsWithFlags & 0x0080)!=0;
		__int16 nBits=nBitsWithFlags & 0x001f;



		//check for empty rows, and fill with nans
		if(nWords==0 && !hasMissingMask && !hasMinimumMask && !hasZeroMask)
		{
			for(__int16 j=0; j<nPointsInRow; ++j)
				dataOut[i][j]=std::numeric_limits<double>::quiet_NaN();
		}
		else
		{
			WgdosExtractor wgdosExtractor(pos,nBits);

			if(hasMissingMask)
				for(__int16 j=0; j<nPointsInRow; ++j)
					missingMask[j]=wgdosExtractor.getNextBitmapMask();
			else
				missingMask.assign(nPointsInRow,0);

			if(hasMinimumMask)
				for(__int16 j=0; j<nPointsInRow; ++j)
					minimumMask[j]=wgdosExtractor.getNextBitmapMask();
			else
				minimumMask.assign(nPointsInRow,0);

			if(hasZeroMask)
				for(__int16 j=0; j<nPointsInRow; ++j)
					zeroMask[j]=wgdosExtractor.getNextBitmapMask();
			else
				zeroMask.assign(nPointsInRow,0);

			if(hasMissingMask || hasMinimumMask || hasZeroMask)
			{
				for(__int16 j=0; j<nPointsInRow; ++j)
				{
					if(missingMask[j]!=0)
						dataOut[i][j]=std::numeric_limits<double>::quiet_NaN();
					else if(minimumMask[j]!=0)
						dataOut[i][j]=baseValue;
					else if(zeroMask[j]!=0)
						dataOut[i][j]=0.0;
					else
						dataOut[i][j]=(double)wgdosExtractor.getNextDataPoint()*precision+baseValue;
				}
			}
			else
				for(__int16 j=0; j<nPointsInRow; ++j)
					dataOut[i][j]=(double)wgdosExtractor.getNextDataPoint()*precision+baseValue;
		}
		
		pos+=nWords*4;
		nread+=nWords*4;
	}

}

std::vector<std::vector<double>> UmFile::getData(size_t sectionIndex)
{
	std::vector<std::vector<double>> result;
	getData(sectionIndex,result);
	return result;

}


void UmFile::getData(const Section &section, std::vector<std::vector<double>> &result)
{
	m_fin.clear();
	m_fin.seekg(section.m_dataStart);
	//create space for the data, we add 4 bytes padding to ensure we don't
	//try to read data that we don't have permission for if decoding wgdos.
	std::vector<char> rawData(section.m_dataBytes+4);
	readRecord((__int32*)&rawData[0],section.m_dataBytes);
	//create a vector for the result
	result.resize(section.m_header.m_nRows);
	for(size_t i=0; i<result.size(); ++i)
		result[i].resize(section.m_header.m_nPointsPerRow);
	if(result.size()==0)
		return;
	if(result[0].size()==0)
		return;

	int packing=section.m_header.m_packingMethod%10;
	int compressionFlag=(section.m_header.m_packingMethod%100)/10;
	int compressionInfo=(section.m_header.m_packingMethod%1000)/100;
	int numberFormat=(section.m_header.m_packingMethod%10000)/1000;

	if(compressionFlag!=0)
		throw(PPERR_COMPRESSION_NOT_SUPPORTED);

	if(numberFormat==1)
		throw(PPERR_IBM_FORMAT_NOT_SUPPORTED);
	if(numberFormat==1)
		throw(PPERR_CRAY_FORMAT_NOT_SUPPORTED);
	if(numberFormat==1)
		throw(PPERR_GRIB_FORMAT_NOT_SUPPORTED);

	if(packing==0)
	{
		//The data isn't compressed, but it is a float rather than a double, convert it
		assert(result.size()*result[0].size()<=rawData.size()/4);
		float *floatArray=(float*)(&rawData[0]);
		size_t rowLength=result[0].size();
		for(size_t i=0; i<result.size(); ++i)
			result[i/rowLength][i%rowLength]=floatArray[i]-section.m_header.m_datum;
	}
	else if(packing==1)
	{
		std::vector<double*> pointerArray(result.size());
		for(size_t i=0; i<result.size(); ++i)
			pointerArray[i]=&result[i][0];
		decompressWgdos(&rawData[0],rawData.size(),&pointerArray[0],result.size(),result[0].size());
		if(section.m_header.m_datum!=0.0)
		{
			for(size_t i=0; i<pointerArray.size(); ++i)
			{
				double *end=pointerArray[i]+result[i].size();
				for(double* iter=pointerArray[i]; iter<end; ++iter)
					*iter-=section.m_header.m_datum;
			}
		}
	}
	else if(packing==2)
		throw(PPERR_CRAY32_PACKING_NOT_SUPPORTED);
	else if(packing==3)
		throw(PPERR_GRIB_PACKING_NOT_SUPPORTED);
	else if(packing==4)
		throw(PPERR_RLE_PACKING_NOT_SUPPORTED);

}

void UmFile::getData(size_t sectionIndex, std::vector<std::vector<double>> &result)
{
	getData(m_sections[sectionIndex],result);
}
void UmFile::getData(const std::vector<size_t> &sectionIndices, std::vector<std::vector<std::vector<double>>> &result)
{
	result.resize(sectionIndices.size());
	for(size_t i=0; i<result.size(); ++i)
	{
		getData(sectionIndices[i],result[i]);
	}
}

std::vector<std::vector<std::vector<double>>> UmFile::getData(const std::vector<size_t> &sectionIndices)
{
	std::vector<std::vector<std::vector<double>>> result;
	getData(sectionIndices, result);
	return result;
}

void UmFile::getData(const std::vector<std::vector<size_t>> &sectionIndices, std::vector<std::vector<std::vector<std::vector<double>>>> &result)
{
	result.resize(sectionIndices.size());
	for(size_t i=0; i<result.size(); ++i)
	{
		getData(sectionIndices[i],result[i]);
	}
}

std::vector<std::vector<std::vector<std::vector<double>>>> UmFile::getData(const std::vector<std::vector<size_t>> &sectionIndices)
{
	std::vector<std::vector<std::vector<std::vector<double>>>> result;
	getData(sectionIndices, result);
	return result;
}

void UmFile::getFilteredData(std::vector<std::vector<std::vector<double>>> &result)
{
	result.resize(m_filteredSections.size());
	for(size_t i=0; i<m_filteredSections.size(); ++i)
		getData(m_filteredSections[i],result[i]);
}

std::vector<std::vector<std::vector<double>>> UmFile::getFilteredData()
{
	std::vector<std::vector<std::vector<double>>> result;
	getFilteredData(result);
	return result;
}

std::vector<UmFile::PpHeader32> UmFile::getFilteredHeaders()
{
	std::vector<PpHeader32> result(m_filteredSections.size());
	for(size_t i=0; i<m_filteredSections.size(); ++i)
		result[i]=m_filteredSections[i].m_header;
	return result;
}


std::vector<__int32> UmFile::getStashCodeList()
{
	std::vector<__int32> list;
	for(size_t i=0; i<m_sections.size(); ++i)
	{
		bool gotAlready=false;
		for(size_t j=0; j<list.size(); ++j)
			if(m_sections[i].m_header.m_ItemCode==list[j])
				gotAlready=true;
		if(!gotAlready)
			list.push_back(m_sections[i].m_header.m_ItemCode);
	}
	std::sort(list.begin(),list.end());

	return list;
}

void UmFile::getSectionAxes(const PpHeader32 &header, std::vector<double> &x, std::vector<double> &y)
{
	if(header.m_gridCode==projSpectral)
		throw(PPERR_PROJECTION_DATA_MEANINGLESS);
		
	x.resize(header.m_nPointsPerRow);
	y.resize(header.m_nRows);
	if(x.size()==0 || y.size()==0)
		throw (PPERR_ZERO_SIZE);
	
	double dx=header.m_dx;
	double dy=header.m_dy;

	if(header.m_gridCode==2)
	{
		x[0]=header.m_zx + 0.5*dx;
		y[0]=header.m_zy + 0.5*dy;
	}
	else if(header.m_gridCode==projPolarStereographic)
	{
		x[0]=0.0;
		y[0]=0.0;
		dx=dy;
	}
	else
	{
		x[0]=header.m_zx + dx;
		y[0]=header.m_zy + dy;
	}


	for(size_t i=1; i<x.size(); ++i)
		x[i]=x[i-1]+dx;
	for(size_t i=1; i<y.size(); ++i)
		y[i]=y[i-1]+dy;

}

void UmFile::getSectionAxes(size_t sectionIndex, std::vector<double> &x, std::vector<double> &y)
{
	getSectionAxes(m_sections[sectionIndex].m_header,x,y);
}


void UmFile::getPolarStereographicParams(size_t sectionIndex, double &orientation, double &referenceLatitude, double &gridLengthAtRef, double &poleX, double &poleY)
{
	if(m_sections[sectionIndex].m_header.m_gridCode==projPolarStereographic)
	{
		orientation=m_sections[sectionIndex].m_header.m_gor;
		referenceLatitude=m_sections[sectionIndex].m_header.m_zy;
		gridLengthAtRef=m_sections[sectionIndex].m_header.m_dy;
		poleX=m_sections[sectionIndex].m_header.m_zx;
		poleY=m_sections[sectionIndex].m_header.m_dx;
	}
	else
		throw(PPERR_PROJECTION_DATA_MEANINGLESS);
}

double UmFile::getSpectralReferenceLongitude(size_t sectionIndex)
{
	if(m_sections[sectionIndex].m_header.m_gridCode==projSpectral)
		return m_sections[sectionIndex].m_header.m_zx;
	else
		throw(PPERR_PROJECTION_DATA_MEANINGLESS);
}

void UmFile::getCrossSectionStashCodes(size_t sectionIndex, int &xStashCode, int &yStashCode)
{
	if(m_sections[sectionIndex].m_header.m_gridCode<10000)
		throw(PPERR_PROJECTION_DATA_MEANINGLESS);

	yStashCode=m_sections[sectionIndex].m_header.m_gridCode%100;
	xStashCode=(m_sections[sectionIndex].m_header.m_gridCode%10000)/100;
}

float UmFile::fromIbmFloat(void *ibmFloat)
{
	unsigned __int32 fraction;
	unsigned __int32 exponent;
	unsigned __int32 sign;

	//copy the data into an int32, ensuring no implicit conversion
	//by using pointers
	unsigned __int32 from=*((unsigned __int32*)ibmFloat);

	//grab the sign exponent and fraction, all right aligned
	sign = (from >> 31);
	exponent = (from << 1) >> 25;
	fraction = (from << 8) >> 8;

	//convert exponent from base 16 with offset of 64 to base 2 with offset 127
	//exp=(exp-64)*4+127; The following is expanded and faster
	exponent = (exponent<<2)-129;

	//for ieee we need to trim off the leading 1 of the fraction
	//so shift it up to 3 bits (there shouldn't be more than three
	//leading zeros in an IBM float) decrementing the exponent by one each time
	//note we need the zero check to avoid infinite loop.
	if(fraction!=0)
	{
		while(! (0x01000000 & fraction) )
		{
			fraction <<= 1;
			--exponent;
		}
	}
	fraction &= 0x00ffffff;

	//check the exponent hasn't gone crazy and check we can actually fit
	//this value into an ieee float
	//cast it to signed to check easier
	__int32 signedExponent=exponent;
	if(signedExponent>255)
	{
		if(sign==0)
			return std::numeric_limits<float>::infinity();
		else
			return -std::numeric_limits<float>::infinity();
	}
	else if (signedExponent < -24)
		return 0.0;
	else if (signedExponent < 0)
	{
		//this is a denormal number
		//so it lies somewhere between 0.0 and the smallest number
		// that can be represented by val=1.nnn*exp(pow), where 
		//1.nnn is a binary number
		//we need to stick out leading 1 back on and turn it into
		//just a fraction
		fraction = (fraction |=0x01000000) >> (-signedExponent);
		exponent=0;
	}

	//stick it together
	float result;
	unsigned __int32 *pResult=(unsigned __int32*)&result;
	*pResult=(sign << 31) | ( exponent << 23) | (fraction >> 1);
	return result;

}

void UmFile::sortHeaders()
{
	std::sort(m_filteredSections.begin(),m_filteredSections.end());
}