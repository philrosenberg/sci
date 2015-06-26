#ifndef PPFILE_H
#define PPFILE_H

#include<fstream>
#include<string>
#include<vector>
#include"stashcodes.h"

enum PPErr
{
	PPERR_CANNOT_OPEN_FILE,
	PPERR_CANNOT_DETERMINE_ENDIANNESS,
	PPERR_REACHED_FILE_END_UNEXPECTEDLY,
	PPERR_RECORD_WRONG_LENGTH,
	PPERR_WGDOSCOMPRESSION_BPP_TOO_HIGH,
	PPERR_COMPRESSION_NOT_SUPPORTED,
	PPERR_CRAY32_PACKING_NOT_SUPPORTED,
	PPERR_GRIB_PACKING_NOT_SUPPORTED,
	PPERR_RLE_PACKING_NOT_SUPPORTED,
	PPERR_IBM_FORMAT_NOT_SUPPORTED,
	PPERR_CRAY_FORMAT_NOT_SUPPORTED,
	PPERR_GRIB_FORMAT_NOT_SUPPORTED,
	PPERR_ZERO_SIZE,
	PPERR_PROJECTION_DATA_MEANINGLESS,
	PPERR_COMPRESSED_DATA_HAS_WRONG_SIZE,
	PPERR_32_BIT_FILE_TOO_LARGE,
	PPERR_UNKNOWN_FILE_FORMAT,
	PPERR_FILE_FORMAT_AMBIGUOUS,
	PPERR_64_BIT_FILE_TOO_LARGE
};

enum
{
	projLatLon=1,
	projLatLonCentred, //lat lon but with points at grid centres
	projPolarStereographic,
	projSpectral,
	projMercator,
	projPlanePolar,
	projCartesian,
	projCrossSection
};

inline void swapEndian(__int32 &val)
{
	char* arrVal=(char*)&val;
	std::swap(arrVal[0],arrVal[3]);
	std::swap(arrVal[1],arrVal[2]);
}

inline void swapEndian(__int32 *vals, size_t nVals)
{
	for(size_t i=0; i<nVals; ++i)
		swapEndian(vals[i]);
}

inline void swapEndian(__int64 &val)
{
	char* arrVal=(char*)&val;
	std::swap(arrVal[0],arrVal[7]);
	std::swap(arrVal[1],arrVal[6]);
	std::swap(arrVal[2],arrVal[5]);
	std::swap(arrVal[3],arrVal[4]);
}

inline void swapEndian(__int64 *vals, size_t nVals)
{
	for(size_t i=0; i<nVals; ++i)
		swapEndian(vals[i]);
}

class UmFileBase;
class PpFile32;
class PpFile64;
class FieldFile32;
class FieldFile64;

class UmFile
{
public:
	struct PpHeader32
	{
		__int32 m_validYear;
		__int32 m_validMonth;
		__int32 m_validDayOfMonth;
		__int32 m_validHour;
		__int32 m_validMinute;
		__int32 m_validDayNumberOrSeconds;
		__int32 m_dataYear;
		__int32 m_dataMonth;
		__int32 m_dataDayOfMonth;
		__int32 m_dataHour;
		__int32 m_dataMinute;
		__int32 m_dataDayNumberOrSeconds;
		__int32 m_timeIndicator;
		__int32 m_forecastPeriod;
		__int32 m_dataLength;
		__int32 m_gridCode;
		__int32 m_hemisphereIndicator;
		__int32 m_nRows;
		__int32 m_nPointsPerRow;
		__int32 m_extraDataLength;
		__int32 m_packingMethod;
		__int32 m_releaseNumber;
		__int32 m_fieldCode;
		__int32 m_secondFieldCode;
		__int32 m_processingCode;
		__int32 m_verticalCoordinateType;
		__int32 m_verticalCoordinateTypeForReferenceLevel;
		__int32 m_experimentNumber;
		__int32 m_startRecord;
		__int32 m_nRecords;
		__int32 m_projectionNumber;
		__int32 m_fieldType;
		__int32 m_levelCode;
		__int32 m_reserved1;
		__int32 m_reserved2;
		__int32 m_reserved3;
		__int32 m_ensembleMemberNumber;
		__int32 m_source;
		__int32 m_dataType;
		__int32 m_startAddressInData;
		__int32 m_rimWidthHaloSizes;
		__int32 m_ItemCode;
		__int32 m_pseudoLevel;
		__int32 m_userDefined;
		__int32 m_modelCode;
		float m_upperLayerBoundary1;
		float m_upperLayerBoundary2;
		float m_reserved4;
		float m_reserved5;
		float m_datum;
		float m_packingAccuracy;
		float m_level1;
		float m_lowerLayerBoundary1;
		float m_level2;
		float m_lowerLayerBoundary2;
		float m_pseusoNorthPoleRealLatitude;
		float m_pseudoNorthPoleRealLongitude;
		float m_gor;
		float m_zy;
		float m_dy;
		float m_zx;
		float m_dx;
		float m_missingValue;
		float m_mksScalingFactor;
	};
	struct PpHeader64
	{
		__int64 m_validYear;
		__int64 m_validMonth;
		__int64 m_validDayOfMonth;
		__int64 m_validHour;
		__int64 m_validMinute;
		__int64 m_validDayNumberOrSeconds;
		__int64 m_dataYear;
		__int64 m_dataMonth;
		__int64 m_dataDayOfMonth;
		__int64 m_dataHour;
		__int64 m_dataMinute;
		__int64 m_dataDayNumberOrSeconds;
		__int64 m_timeIndicator;
		__int64 m_forecastPeriod;
		__int64 m_dataLength;
		__int64 m_gridCode;
		__int64 m_hemisphereIndicator;
		__int64 m_nRows;
		__int64 m_nPointsPerRow;
		__int64 m_extraDataLength;
		__int64 m_packingMethod;
		__int64 m_releaseNumber;
		__int64 m_fieldCode;
		__int64 m_secondFieldCode;
		__int64 m_processingCode;
		__int64 m_verticalCoordinateType;
		__int64 m_verticalCoordinateTypeForReferenceLevel;
		__int64 m_experimentNumber;
		__int64 m_startRecord;
		__int64 m_nRecords;
		__int64 m_projectionNumber;
		__int64 m_fieldType;
		__int64 m_levelCode;
		__int64 m_reserved1;
		__int64 m_reserved2;
		__int64 m_reserved3;
		__int64 m_ensembleMemberNumber;
		__int64 m_source;
		__int64 m_dataType;
		__int64 m_startAddressInData;
		__int64 m_rimWidthHaloSizes;
		__int64 m_ItemCode;
		__int64 m_pseudoLevel;
		__int64 m_userDefined;
		__int64 m_modelCode;
		double m_upperLayerBoundary1;
		double m_upperLayerBoundary2;
		double m_reserved4;
		double m_reserved5;
		double m_datum;
		double m_packingAccuracy;
		double m_level1;
		double m_lowerLayerBoundary1;
		double m_level2;
		double m_lowerLayerBoundary2;
		double m_pseusoNorthPoleRealLatitude;
		double m_pseudoNorthPoleRealLongitude;
		double m_gor;
		double m_zy;
		double m_dy;
		double m_zx;
		double m_dx;
		double m_missingValue;
		double m_mksScalingFactor;
	};

	struct FixedHeader
	{
		__int32 m_formatVersionnumber;
		__int32 m_subModelIndicator;
		__int32 m_verticalCoordinateTypeIndicator;
		__int32 m_horizontalCoordinateTypeIndicator;
		__int32 m_datasetTypeIndicator;
		__int32 m_runIdentifier;
		__int32 m_experimentNumber;
		__int32 m_calendarIndicator;
		__int32 m_gridStaggeringIndicator;
		__int32 m_ancillaryFileTimeIndicator;
		__int32 m_projectionNumber;
	};
	class Section32
	{
		friend class UmFile;
		friend class Section64;
	private:
		PpHeader32 m_header;
		std::streamoff m_dataStart;
		size_t m_dataBytes;
		UmFile *m_parent;
		template <class T>
		T at(size_t index) const
		{
			if(index<45)
				return ((__int32*)&m_header)[m_parent->m_comparator];
			else
				return ((float*)&m_header)[m_parent->m_comparator];
		}
	public:
		bool operator < (const Section32 & rhs) const
		{
			if(m_parent->m_comparator<45)
				return ((__int32*)&m_header)[m_parent->m_comparator]<((const __int32*)&rhs.m_header)[m_parent->m_comparator];
			else
				return ((float*)&m_header)[m_parent->m_comparator]<((const float*)&rhs.m_header)[m_parent->m_comparator];
		}
		void readHeader( std::fstream *fin, size_t nBytes);
		void setDataStart( _int32 start )
		{
			m_dataStart = start;
		}
		void setDataSize( _int32 size )
		{
			m_dataBytes = size;
		}
		void setParent( UmFile *parent )
		{
			m_parent = parent;
		}
		size_t getDataSize()
		{
			return m_dataBytes;
		}

	};
	class Section64
	{
		friend class UmFile;
	private:
		PpHeader64 m_header;
		std::streamoff m_dataStart;
		size_t m_dataBytes;
		UmFile *m_parent;
		template <class T>
		T at(size_t index) const
		{
			if(index<45)
				return ((__int32*)&m_header)[m_parent->m_comparator];
			else
				return ((float*)&m_header)[m_parent->m_comparator];
		}
	public:
		bool operator < (const Section64 & rhs) const
		{
			if(m_parent->m_comparator<45)
				return ((__int32*)&m_header)[m_parent->m_comparator]<((const __int32*)&rhs.m_header)[m_parent->m_comparator];
			else
				return ((float*)&m_header)[m_parent->m_comparator]<((const float*)&rhs.m_header)[m_parent->m_comparator];
		}
		void readHeader( std::fstream *fin, size_t nBytes);
		void setDataStart( __int64 start )
		{
			m_dataStart = start;
		}
		void setDataSize( __int64 size )
		{
			m_dataBytes = size;
		}
		void setParent( UmFile *parent )
		{
			m_parent = parent;
		}
		size_t getDataSize()
		{
			return m_dataBytes;
		}
		Section64 &operator=( const Section32 &rhs )
		{
			m_dataStart = rhs.m_dataStart;
			m_dataBytes = rhs.m_dataBytes;
			m_parent = m_parent;
			for( size_t i=0; i<45; ++i)
				*((__int64*)(&m_header) + i ) = rhs.at<__int32>( i );
			for( size_t i=46; i<64; ++i)
				*((double*)(&m_header) + i ) = rhs.at<float>( i );
		}

	};
	friend class Section;


public:
	UmFile(std::string name);

	//read and return the data from a particular section
	std::vector<std::vector<double>> getData(size_t sectionIndex);
	void getData(size_t sectionIndex, std::vector<std::vector<double>> &result);
	//read and return data from multiple sections giving a data cube
	std::vector<std::vector<std::vector<double>>> getData(const std::vector<size_t> &sectionIndices);
	void getData(const std::vector<size_t> &sectionIndices, std::vector<std::vector<std::vector<double>>> &result);
	//read and return data from multiple sections giving a 4D data hypercube
	std::vector<std::vector<std::vector<std::vector<double>>>> getData(const std::vector<std::vector<size_t>> &sectionIndices);
	void getData(const std::vector<std::vector<size_t>> &sectionIndices, std::vector<std::vector<std::vector<std::vector<double>>>> &result);
	
	//read and return filtered data
	std::vector<std::vector<std::vector<double>>> getFilteredData(const std::vector<size_t> &sectionIndices);
	void getFilteredData(std::vector<std::vector<std::vector<double>>> &result);
	std::vector<std::vector<std::vector<double>>> getFilteredData();

	//return an array of filtered headers
	std::vector<PpHeader32> getFilteredHeaders();
	static void getSectionAxes(const PpHeader32 &header, std::vector<double> &x, std::vector<double> &y);

	//return the header for a particular section
	PpHeader32 getHeader(size_t sectionIndex){return m_sections[sectionIndex].m_header;}
	//get the number of sections in the file
	size_t getNumSections(){return m_sections.size();}
	//get a list of the stash codes in the file, sorted ascending, no entry will appear more than once
	std::vector<__int32> getStashCodeList();
	
	//get the x and y values for a section works for regular x/y grids. throws an error for
	//spectral grids
	void getSectionAxes(size_t sectionIndex, std::vector<double> &x, std::vector<double> &y);
	void getPolarStereographicParams(size_t sectionIndex, double &orientation, double &referenceLatitude, double &gridLengthAtRef, double &poleX, double &poleY);
	double getSpectralReferenceLongitude(size_t sectionIndex);
	void getCrossSectionStashCodes(size_t sectionIndex, int &xStashCode, int &yStashCode);

	//Functions to allow filtering of the data based on header properties
	UmFile& setComparator(int comparator){ m_comparator=comparator; return *this; }
	void sortHeaders();
	void resetFilteredHeaders(){m_filteredSections=m_sections;}
	std::vector<size_t> getFilteredSectionIndices();

	template <class T>
	UmFile& operator > ( T rhs );
	template <class T>
	UmFile& operator >= ( T rhs );
	template <class T>
	UmFile& operator < ( T rhs );
	template <class T>
	UmFile& operator <= ( T rhs );
	template <class T>
	UmFile& operator == ( T rhs );
	template <class T>
	UmFile& operator != ( T rhs );

	enum
	{
		compValidYear,
		compValidMonth,
		compValidDayOfMonth,
		compValidHour,
		compValidMinute,
		compValidDayNumberOrSeconds,
		compDataYear,
		compDataMonth,
		compDataDayOfMonth,
		compDataHour,
		compDataMinute,
		compDataDayNumberOrSeconds,
		compTimeIndicator,
		compForecastPeriod,
		compDataLength,
		compGridCode,
		compHemisphereIndicator,
		compNRows,
		compNPointsPerRow,
		compExtraDataLength,
		compPackingMethod,
		compReleaseNumber,
		compFieldCode,
		compSecondFieldCode,
		compProcessingCode,
		compVerticalCoordinateType,
		compVerticalCoordinateTypeForReferenceLevel,
		compExperimentNumber,
		compStartRecord,
		compNRecords,
		compProjectionNumber,
		compFieldType,
		compLevelCode,
		compReserved1,
		compReserved2,
		compReserved3,
		compEnsembleMemberNumber,
		compSource,
		compDataType,
		compStartAddressInData,
		compRimWidthHaloSizes,
		compItemCode,
		compPseudoLevel,
		compUserDefined,
		compModelCode,
		compUpperLayerBoundary1,
		compUpperLayerBoundary2,
		compReserved4,
		compReserved5,
		compDatum,
		compPackingAccuracy,
		compLevel1,
		compLowerLayerBoundary1,
		compLevel2,
		compLowerLayerBoundary2,
		compPseusoNorthPoleRealLatitude,
		compPseudoNorthPoleRealLongitude,
		compGor,
		compZy,
		compDy,
		compZx,
		compDx,
		compMissingValue,
		compMksScalingFactor
	};
	

private:
	std::vector<UmFileBase*> m_umFileTypes;
	UmFileBase *m_umFileBase;
	std::fstream m_fin;
	FixedHeader m_fixedHeader;
	bool m_bigEndian;
	std::vector<Section32> m_sections;
	std::vector<Section32> m_filteredSections;
	size_t m_comparator;

	//get data for a given section
	void getData(const Section32 &section, std::vector<std::vector<double>> &result);
	__int32 getNextRecordSize();
	void readRecord(__int32* record, std::basic_istream<char>::pos_type nBytes);
	void skipRecord(std::basic_istream<char>::pos_type nBytes);
	void decompressWgdos(char *dataIn, size_t nIn, double** dataOut, size_t outDim1, size_t outDim2);
	//convert an IBM float to an IEEE float. Note we pass a pointer to the float as
	//a void * to avoid acidentally doing implicit conversions e.g. if the bit are 
	//actually held in a __int32
	float fromIbmFloat(void *ibmFloat);

};

//UmFileBase
//This class forms a base class defining the interface for various different UM file types.
//It provides two pure virtual functions which child classes must overload.
//The first is open. This must read from the fstream given starting at its current get pointer
//and parse the pp headers in the file. The pp headers are then returned as a vector. If 
//bigEndian is true then the endianness of the read words must be reversed. In addition to the 
//pp file headers, the Sections returned must have the location and size of each pp data section
//set and it must have the parent set to the parent passed in.
//The checkValidFirstWord function must check the word passed in and if this is a potentially valid
//first word the function must return true, otherwise return false. Note that this function should not
//attempt to swap the endianness of the word passed in. Note that there is a 32 and 64 bit version of
//this function. Clearly a 32 bit reader should only return true for a 32 bit word so only needs to
//overload the 32 bit version as the default is to return false.
class UmFileBase
{
	friend class UmFile;
private:
	virtual std::vector<UmFile::Section32> open( std::fstream *fin, UmFile *parent, bool bigEndian ) = 0;
	virtual bool checkValidFirstWord( __int32 firstWord )  { return false; }
	virtual bool checkValidFirstWord( __int64 firstWord ) { return false; }
	template < class T >
	bool checkValidFirstWord( T ) = delete; //This stops any other types resulting in callin the above versions
protected:
};

class PpFile32 : public UmFileBase
{
	std::vector<UmFile::Section32> open( std::fstream *fin, UmFile *parent, bool bigEndian );
	bool checkValidFirstWord( __int32 firstWord );
	__int32 getNextRecordSize( std::fstream * fin, bool bigEndian );
	void skipRecord( std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool bigEndian );
};

class PpFile64 : public UmFileBase
{
	std::vector<UmFile::Section32> open( std::fstream *fin, UmFile *parent, bool bigEndian );
	bool checkValidFirstWord( __int64 firstWord );
	__int64 getNextRecordSize( std::fstream * fin, bool bigEndian );
	void skipRecord( std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool bigEndian );
};

class FieldsFile32 : public UmFileBase
{
	std::vector<UmFile::Section32> open( std::fstream *fin, UmFile *parent, bool bigEndian );
	bool checkValidFirstWord( __int32 firstWord );
};

class FieldsFile64 : public UmFileBase
{
	std::vector<UmFile::Section32> open( std::fstream *fin, UmFile *parent, bool bigEndian );
	bool checkValidFirstWord( __int64 firstWord );
};



template <class T>
UmFile& UmFile::operator > ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if(!(m_filteredSections[i].at<T>(m_comparator) > rhs))
		{
			m_filteredSections.erase(m_filteredSections.begin()+i);
			--i;
		}
	}
	return *this;
}

template <class T>
UmFile& UmFile::operator >= ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if(!(m_filteredSections[i].at<T>(m_comparator) >= rhs))
		{
			m_filteredSections.erase(m_filteredSections.begin()+i);
			--i;
		}
	}
	return *this;
}

template <class T>
UmFile& UmFile::operator < ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if(!(m_filteredSections[i].at<T>(m_comparator) < rhs))
		{
			m_filteredSections.erase(m_filteredSections.begin()+i);
			--i;
		}
	}
	return *this;
}

template <class T>
UmFile& UmFile::operator <= ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if(!(m_filteredSections[i].at<T>(m_comparator) <= rhs))
		{
			m_filteredSections.erase(m_filteredSections.begin()+i);
			--i;
		}
	}
	return *this;
}

template <class T>
UmFile& UmFile::operator == ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if(!(m_filteredSections[i].at<T>(m_comparator) == rhs))
		{
			m_filteredSections.erase(m_filteredSections.begin()+i);
			--i;
		}
	}
	return *this;
}

template <class T>
UmFile& UmFile::operator != ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if(!(m_filteredSections[i].at<T>(m_comparator) != rhs))
		{
			m_filteredSections.erase(m_filteredSections.begin()+i);
			--i;
		}
	}
	return *this;
}



#endif