#include "../include/svector/ppfile.h"
#include<assert.h>
#include<vector>
#include<cmath>
#include<limits>
#include"../include/svector/wgdos.h"
#include<algorithm>

UmFile::UmFile(std::string name)
{
	std::vector<std::shared_ptr<UmFileParser>> umFileTypes;
	m_comparator=compForecastPeriod;
	umFileTypes.push_back(std::shared_ptr<UmFileParser>(new PpFileParser32));
	umFileTypes.push_back(std::shared_ptr<UmFileParser>(new PpFileParser64));
	umFileTypes.push_back(std::shared_ptr<UmFileParser>(new FieldsFileParser32));
	umFileTypes.push_back(std::shared_ptr<UmFileParser>(new FieldsFileParser64));

	m_fin.open(name.c_str(), std::ios::in|std::ios::binary);
	if(!m_fin.is_open())
		throw(PPERR_CANNOT_OPEN_FILE);

	//decide if we have a pp file or fields file, 64 bit or 32 bit, need to swap endianness or not
	m_umFileParser = nullptr;
	m_bigEndian=false;

	__int32 first32;
	__int32 first32Swapped;
	__int64 first64;
	__int64 first64Swapped;
	m_fin.read((char*)(&first32), sizeof(first32) );
	m_fin.seekg( 0 );
	m_fin.read((char*)(&first64), sizeof(first64) );
	m_fin.seekg( 0 );
	first32Swapped = first32;
	swapEndian( first32Swapped );
	first64Swapped = first64;
	swapEndian( first64Swapped );
	__int32 fifth32;
	__int32 fifth32Swapped;
	__int64 fifth64;
	__int64 fifth64Swapped;
	m_fin.seekg( 4 * sizeof( __int32) );
	m_fin.read((char*)(&fifth32), sizeof(fifth32) );
	m_fin.seekg( 4 * sizeof( __int64) );
	m_fin.read((char*)(&fifth64), sizeof(fifth64) );
	m_fin.seekg( 0 );
	fifth32Swapped = fifth32;
	swapEndian( fifth32Swapped );
	fifth64Swapped = fifth64;
	swapEndian( fifth64Swapped );
	for( size_t i=0; i< umFileTypes.size(); ++i)
	{
		if( umFileTypes[i]->checkValidWords( first32, fifth32 ) )
		{
			if( m_umFileParser )
				throw( PPERR_FILE_FORMAT_AMBIGUOUS );
			m_bigEndian = false;
			m_umFileParser = umFileTypes[i];
		}
		if( umFileTypes[i]->checkValidWords( first32Swapped, fifth32Swapped ) )
		{
			if( m_umFileParser )
				throw( PPERR_FILE_FORMAT_AMBIGUOUS );
			m_bigEndian = true;
			m_umFileParser = umFileTypes[i];
		}
		if( umFileTypes[i]->checkValidWords( first64, fifth64 ) )
		{
			if( m_umFileParser )
				throw( PPERR_FILE_FORMAT_AMBIGUOUS );
			m_bigEndian = false;
			m_umFileParser = umFileTypes[i];
		}
		if( umFileTypes[i]->checkValidWords( first64Swapped, fifth64Swapped ) )
		{
			if( m_umFileParser )
				throw( PPERR_FILE_FORMAT_AMBIGUOUS );
			m_bigEndian = true;
			m_umFileParser = umFileTypes[i];
		}
	}

	if( m_umFileParser == nullptr )
		throw( PPERR_UNKNOWN_FILE_FORMAT );

	m_sections = m_umFileParser->parse( &m_fin, this, m_bigEndian );

	m_filteredSections=m_sections;
}

/*void UmFile::readRecord(__int32* record, std::basic_istream<char>::pos_type nBytes)
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
}*/

/*__int32 UmFile::getNextRecordSize()
{
	__int32 size;
	m_fin.read((char*)&size,sizeof(size));
	if(m_bigEndian)
		swapEndian(size);
	return size;
}*/

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


void UmFile::getData(const Section64 &section, std::vector<std::vector<double>> &result)
{
	m_fin.clear();
	m_fin.seekg(section.m_dataStart);
	//create space for the data, we add 4 bytes padding to ensure we don't
	//try to read data that we don't have permission for if decoding wgdos.
	std::vector<char> rawData(section.m_dataBytes+4);
	m_umFileParser->readRecord(&rawData[0], &m_fin, section.m_dataBytes, m_bigEndian);
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

std::vector<UmFile::PpHeader64> UmFile::getFilteredHeaders()
{
	std::vector<PpHeader64> result(m_filteredSections.size());
	for(size_t i=0; i<m_filteredSections.size(); ++i)
		result[i]=m_filteredSections[i].m_header;
	return result;
}


std::vector<__int64> UmFile::getStashCodeList()
{
	std::vector<__int64> list;
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

void UmFile::getSectionAxes(const PpHeader64 &header, std::vector<double> &x, std::vector<double> &y)
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

void UmFile::Section32::readHeader( std::fstream *fin, size_t nBytes )
{
	fin->read((char*)&m_header,nBytes);
	if(fin->gcount()!=nBytes)
		throw(PPERR_REACHED_FILE_END_UNEXPECTEDLY);
	if(m_parent->m_bigEndian)
			swapEndian( (_int32*)&m_header,nBytes/4 );
}

void UmFile::Section64::readHeader( std::fstream *fin, size_t nBytes )
{
	fin->read((char*)&m_header,nBytes);
	if(fin->gcount()!=nBytes)
		throw(PPERR_REACHED_FILE_END_UNEXPECTEDLY);
	if(m_parent->m_bigEndian)
			swapEndian( (_int64*)&m_header,nBytes/4 );
}

std::vector<UmFile::Section64> PpFileParser32::parse( std::fstream *fin, UmFile *parent, bool bigEndian  )
{
	std::vector<UmFile::Section64> result;

	if(!fin->is_open())
		throw(PPERR_CANNOT_OPEN_FILE);

	while (1)
	{
		__int32 headerSize=getNextRecordSize( fin, bigEndian );
		if( fin->eof() )
			break; //no more records

		if( headerSize != 64 * sizeof(_int32) )
			throw( PPERR_RECORD_WRONG_LENGTH );

		//read the header
		UmFile::Section32 section; //note we read in using a section 32, then convert it to a Section64 to return it
		section.setParent( parent );
		section.readHeader( fin, headerSize );

		//check the size of the header which should follow
		__int32 size = getNextRecordSize( fin, bigEndian );
		assert( size == headerSize );
		if( size != headerSize )
			throw(PPERR_RECORD_WRONG_LENGTH);

		//get the size of the data that follows and set the size in section
		section.setDataSize( getNextRecordSize( fin, bigEndian ) );
		//set the data location 
		if( fin->tellg() > (std::streampos)std::numeric_limits<__int32>::max() )
			throw PPERR_32_BIT_FILE_TOO_LARGE;
		section.setDataStart( (__int32) fin->tellg() );

		//skip the data
		skipRecord( fin, section.getDataSize(), bigEndian );

		//if we have reached the file end then it meant we didn't have a full record. Throw Error
		if(fin->eof())
			throw( PPERR_REACHED_FILE_END_UNEXPECTEDLY );

		//store the section
		result.push_back( section );

	}
	return result;
}

void PpFileParser32::readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool bigEndian )
{
	fin->read((char*)record,nBytes);
	if(fin->gcount()!=nBytes)
		throw(PPERR_REACHED_FILE_END_UNEXPECTEDLY);
	if(bigEndian)
		swapEndian((__int32*)record,nBytes/4);
	__int32 size;
	fin->read((char*)&size,sizeof(size));
	if(bigEndian)
		swapEndian(size);
	assert(size==nBytes);
	if(size!=nBytes)
		throw(PPERR_RECORD_WRONG_LENGTH);
}

__int32 PpFileParser32::getNextRecordSize( std::fstream * fin, bool bigEndian )
{
	__int32 size;
	fin->read((char*)&size,sizeof(size));
	if(bigEndian)
		swapEndian(size);
	return size;
}

void PpFileParser32::skipRecord( std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool bigEndian )
{
	//Skip past the data then read the post data size and check it
	fin->seekg(fin->tellg()+nBytes);
	__int32 size = getNextRecordSize( fin, bigEndian );
	assert(size==nBytes);
	if(size!=nBytes)
		throw(PPERR_RECORD_WRONG_LENGTH);
}

bool PpFileParser32::checkValidWords( __int32 firstWord, __int32 fifthWord )
{
	//for a ppFile the first word should be the size in bytes of the
	//first ppHeader
	return firstWord == 256;
}

std::vector<UmFile::Section64> PpFileParser64::parse( std::fstream *fin, UmFile *parent, bool bigEndian  )
{
	std::vector<UmFile::Section64> result;
	if(!fin->is_open())
		throw(PPERR_CANNOT_OPEN_FILE);

	while (1)
	{
		__int64 headerSize=getNextRecordSize( fin, bigEndian );
		if( fin->eof() )
			break; //no more records

		if( headerSize != 64 * sizeof(__int64) )
			throw( PPERR_RECORD_WRONG_LENGTH );

		//read the header
		UmFile::Section64 section;
		section.setParent( parent );
		section.readHeader( fin, headerSize );

		//check the size of the header which should follow
		__int64 size = getNextRecordSize( fin, bigEndian );
		assert( size == headerSize );
		if( size != headerSize )
			throw(PPERR_RECORD_WRONG_LENGTH);

		//get the size of the data that follows and set the size in section
		section.setDataSize( getNextRecordSize( fin, bigEndian ) );
		//set the data location 
		if( fin->tellg() > (std::streampos)std::numeric_limits<__int64>::max() )
			throw PPERR_64_BIT_FILE_TOO_LARGE;
		section.setDataStart( (__int64) fin->tellg() );

		//skip the data
		skipRecord( fin, section.getDataSize(), bigEndian );

		//if we have reached the file end then it meant we didn't have a full record. Throw Error
		if(fin->eof())
			throw( PPERR_REACHED_FILE_END_UNEXPECTEDLY );

		//store the section
		result.push_back(section);
	}
	return result;
}

void PpFileParser64::readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool bigEndian )
{
	fin->read((char*)record,nBytes);
	if(fin->gcount()!=nBytes)
		throw(PPERR_REACHED_FILE_END_UNEXPECTEDLY);
	if(bigEndian)
		swapEndian((__int32*)record,nBytes/4); //even 64 bit files seem to use 32 bit data
	__int64 size;
	fin->read((char*)&size,sizeof(size));
	if(bigEndian)
		swapEndian(size);
	assert(size==nBytes);
	if(size!=nBytes)
		throw(PPERR_RECORD_WRONG_LENGTH);
}

__int64 PpFileParser64::getNextRecordSize( std::fstream * fin, bool bigEndian )
{
	__int64 size;
	fin->read((char*)&size,sizeof(size));
	if(bigEndian)
		swapEndian(size);
	return size;
}

void PpFileParser64::skipRecord( std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool bigEndian )
{
	//Skip past the data then read the post data size and check it
	fin->seekg(fin->tellg()+nBytes);
	__int64 size = getNextRecordSize( fin, bigEndian );
	assert(size==nBytes);
	if(size!=nBytes)
		throw(PPERR_RECORD_WRONG_LENGTH);
}

bool PpFileParser64::checkValidWords( __int64 firstWord, __int64 fifthWord )
{
	//for a ppFile the first word should be the size in bytes of the
	//first ppHeader
	return firstWord == 512;
}

std::vector<UmFile::Section64> FieldsFileParser32::parse( std::fstream *fin, UmFile *parent, bool bigEndian  )
{
	std::vector<UmFile::Section64> result;
	if(!fin->is_open())
		throw(PPERR_CANNOT_OPEN_FILE);

	//read in the fixed length header
	UmFile::FixedHeader32 fixedHeader;
	fin->read( (char*)(&fixedHeader), sizeof(fixedHeader) );
	if( bigEndian )
		swapEndian( (__int32*)(&fixedHeader), sizeof( fixedHeader )/sizeof(__int32) );

	//check that the ppHeaders are definitely 64 words - in Obs files it is 128
	assert( fixedHeader.m_nLookupTableFirstDimension == 64 );
	if( fixedHeader.m_nLookupTableFirstDimension != 64 )
		throw( PPERR_RECORD_WRONG_LENGTH );

	//go to the beginning of the ppHeaders
	fin->seekg( fixedHeader.m_startLookupTable * sizeof(__int32) ); //I thought there would be a 1 to 0 index correction here, but it seems not

	//note the length of the header in bytes;
	__int32 headerSize=fixedHeader.m_nLookupTableFirstDimension * sizeof(__int32);

	for ( __int32 i=0; i< fixedHeader.m_nLookupTableSecondDimentsion; ++i)
	{
		//read the header
		UmFile::Section32 section;
		section.setParent( parent );
		section.readHeader( fin, headerSize );

		//if we have reached the file end then it meant we didn't have a full record. Throw Error
		if(fin->eof())
			throw( PPERR_REACHED_FILE_END_UNEXPECTEDLY );

		//get the size and location of the data from the header and set it int the section.
		//Note that often (always?) the UM produces files that have the max (4096) number of
		//ppHeaders in the lookup table but then leave unused headers filled with -99. This
		//makes sense as it is generally not possible to instert in a file - just overwrite.
		UmFile::PpHeader32 ppHeader = section.getHeader();
		if( ppHeader.m_nRecords > 0 && ppHeader.m_startRecord > 0 )
		{
			section.setDataSize( section.getHeader().m_nRecords * sizeof(__int32) );
			section.setDataStart( ( section.getHeader().m_startRecord - 1 ) * sizeof(__int32) );
		}
		else
		{
			section.setDataSize( 0 );
			section.setDataStart( 0 );
		}

		//store the section
		result.push_back(section);
	}
	return result;
}

void FieldsFileParser32::readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool bigEndian )
{
	fin->read((char*)record,nBytes);
	if(fin->gcount()!=nBytes)
		throw(PPERR_REACHED_FILE_END_UNEXPECTEDLY);
	if(bigEndian)
		swapEndian((__int32*)record,nBytes/4);
}

bool FieldsFileParser32::checkValidWords( __int32 firstWord, __int32 fifthWord )
{
	//For a FIELDSFile the first word should be the first word of
	//the fixed length header, which currently can be one of the
	//following values
	return (firstWord == 15 || firstWord == -32768 || firstWord == 20) && fifthWord == 3;
}

std::vector<UmFile::Section64> FieldsFileParser64::parse( std::fstream *fin, UmFile *parent, bool bigEndian  )
{
	std::vector<UmFile::Section64> result;
	if(!fin->is_open())
		throw(PPERR_CANNOT_OPEN_FILE);

	//read in the fixed length header
	UmFile::FixedHeader64 fixedHeader;
	fin->read( (char*)(&fixedHeader), sizeof(fixedHeader) );
	if( bigEndian )
		swapEndian( (__int64*)(&fixedHeader), sizeof( fixedHeader )/sizeof(__int64) );

	//check that the ppHeaders are definitely 64 words - in Obs files it is 128
	assert( fixedHeader.m_nLookupTableFirstDimension == 64 );
	if( fixedHeader.m_nLookupTableFirstDimension != 64 )
		throw( PPERR_RECORD_WRONG_LENGTH );

	//go to the beginning of the ppHeaders
	fin->seekg( ( fixedHeader.m_startLookupTable - 1 ) * sizeof(__int64) );

	//note the length of the header in bytes;
	__int64 headerSize=fixedHeader.m_nLookupTableFirstDimension * sizeof(__int64);

	for ( __int64 i=0; i< fixedHeader.m_nLookupTableSecondDimension; ++i)
	{
		//read the header
		UmFile::Section64 section;
		section.setParent( parent );
		section.readHeader( fin, headerSize );

		//if we have reached the file end then it meant we didn't have a full record. Throw Error
		if(fin->eof())
			throw( PPERR_REACHED_FILE_END_UNEXPECTEDLY );

		//get the size and location of the data from the header and set it int the section.
		//Note that often (always?) the UM produces files that have the max (4096) number of
		//ppHeaders in the lookup table but then leave unused headers filled with -99. This
		//makes sense as it is generally not possible to instert in a file - just overwrite.
		UmFile::PpHeader64 ppHeader = section.getHeader();
		if( ppHeader.m_nRecords > 0 && ppHeader.m_startRecord > 0 )
		{
			section.setDataSize( section.getHeader().m_nRecords * sizeof(__int64) );
			section.setDataStart( section.getHeader().m_startRecord * sizeof(__int64) ); //I thought there would be a 1 to 0 index correction here, but it seems not
		}
		else
		{
			section.setDataSize( 0 );
			section.setDataStart( 0 );
		}

		//store the section
		result.push_back(section);
	}
	return result;
}

void FieldsFileParser64::readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool bigEndian )
{
	fin->read((char*)record,nBytes);
	if(fin->gcount()!=nBytes)
		throw(PPERR_REACHED_FILE_END_UNEXPECTEDLY);
	if(bigEndian)
		swapEndian((__int32*)record,nBytes/4); //even 64 bit files seem to use 32 bit data
}

bool FieldsFileParser64::checkValidWords( __int64 firstWord, __int64 fifthWord )
{
	//For a FIELDSFile the first word should be the first word of
	//the fixed length header, which currently can be one of the
	//following values
	return (firstWord == 15 || firstWord == -32768 || firstWord == 20) && fifthWord == 3;
}