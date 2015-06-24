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
	PPERR_COMPRESSED_DATA_HAS_WRONG_SIZE
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

class UmFile
{
public:
	struct PpHeader
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

private:
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
	class Section
	{
		friend class UmFile;
	private:
		PpHeader m_header;
		std::streamoff m_dataStart;
		size_t m_dataBytes;
		UmFile *m_parent;
		template <class T>
		T at(size_t index)
		{
			if(index<45)
				return ((__int32*)&m_header)[m_parent->m_comparator];
			else
				return ((float*)&m_header)[m_parent->m_comparator];
		}
	public:
		bool operator < (const Section & rhs)
		{
			if(m_parent->m_comparator<45)
				return ((__int32*)&m_header)[m_parent->m_comparator]<((const __int32*)&rhs.m_header)[m_parent->m_comparator];
			else
				return ((float*)&m_header)[m_parent->m_comparator]<((const float*)&rhs.m_header)[m_parent->m_comparator];
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
	std::vector<PpHeader> getFilteredHeaders();
	static void getSectionAxes(const PpHeader &header, std::vector<double> &x, std::vector<double> &y);

	//return the header for a particular section
	PpHeader getHeader(size_t sectionIndex){return m_sections[sectionIndex].m_header;}
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
	std::fstream m_fin;
	FixedHeader m_fixedHeader;
	bool m_bigEndian;
	std::vector<Section> m_sections;
	std::vector<Section> m_filteredSections;
	size_t m_comparator;

	//get data for a given section
	void getData(const Section &section, std::vector<std::vector<double>> &result);
	__int32 getNextRecordSize();
	void readRecord(__int32* record, std::basic_istream<char>::pos_type nBytes);
	void skipRecord(std::basic_istream<char>::pos_type nBytes);
	void decompressWgdos(char *dataIn, size_t nIn, double** dataOut, size_t outDim1, size_t outDim2);
	//convert an IBM float to an IEEE float. Note we pass a pointer to the float as
	//a void * to avoid acidentally doing implicit conversions e.g. if the bit are 
	//actually held in a __int32
	float fromIbmFloat(void *ibmFloat);

};


class UmFileBase
{
public:
	UmFileBase( std::string filename );
private:
	virtual void open( std::string fileName );
};

class PpFile32 : public UmFileBase
{
};

class PpFile64 : public UmFileBase
{
};

class FieldsFile32 : public UmFileBase
{
};

class FieldsFile64 : public UmFileBase
{
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