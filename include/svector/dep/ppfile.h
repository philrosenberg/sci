#ifndef PPFILE_H
#define PPFILE_H

#include<fstream>
#include<string>
#include<vector>
#include"stashcodes.h"
#include<memory>

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
	PPERR_VAX_FORMAT_NOT_SUPPORTED,
	PPERR_ZERO_SIZE,
	PPERR_PROJECTION_DATA_MEANINGLESS,
	PPERR_COMPRESSED_DATA_HAS_WRONG_SIZE,
	PPERR_32_BIT_FILE_TOO_LARGE,
	PPERR_UNKNOWN_FILE_FORMAT,
	PPERR_FILE_FORMAT_AMBIGUOUS,
	PPERR_64_BIT_FILE_TOO_LARGE,
	PPERR_UNCOMPRESSED_DATA_HAS_WRONG_SIZE
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

inline void swapEndian(int32_t &val)
{
	char* arrVal=(char*)&val;
	std::swap(arrVal[0],arrVal[3]);
	std::swap(arrVal[1],arrVal[2]);
}

inline void swapEndian(int32_t *vals, size_t nVals)
{
	for(size_t i=0; i<nVals; ++i)
		swapEndian(vals[i]);
}

inline void swapEndian(int64_t &val)
{
	char* arrVal=(char*)&val;
	std::swap(arrVal[0],arrVal[7]);
	std::swap(arrVal[1],arrVal[6]);
	std::swap(arrVal[2],arrVal[5]);
	std::swap(arrVal[3],arrVal[4]);
}

inline void swapEndian(int64_t *vals, size_t nVals)
{
	for(size_t i=0; i<nVals; ++i)
		swapEndian(vals[i]);
}

class UmFileParser;
class PpFile32;
class PpFile64;
class FieldFile32;
class FieldFile64;

class UmFile
{
public:
	struct PpHeader32
	{
		int32_t m_validYear;
		int32_t m_validMonth;
		int32_t m_validDayOfMonth;
		int32_t m_validHour;
		int32_t m_validMinute;
		int32_t m_validDayNumberOrSeconds;
		int32_t m_dataYear;
		int32_t m_dataMonth;
		int32_t m_dataDayOfMonth;
		int32_t m_dataHour;
		int32_t m_dataMinute;
		int32_t m_dataDayNumberOrSeconds;
		int32_t m_timeIndicator;
		int32_t m_forecastPeriod;
		int32_t m_dataLength;
		int32_t m_gridCode;
		int32_t m_hemisphereIndicator;
		int32_t m_nRows;
		int32_t m_nPointsPerRow;
		int32_t m_extraDataLength;
		int32_t m_packingMethod;
		int32_t m_releaseNumber;
		int32_t m_fieldCode;
		int32_t m_secondFieldCode;
		int32_t m_processingCode;
		int32_t m_verticalCoordinateType;
		int32_t m_verticalCoordinateTypeForReferenceLevel;
		int32_t m_experimentNumber;
		int32_t m_startRecord;
		int32_t m_nRecords;
		int32_t m_projectionNumber;
		int32_t m_fieldType;
		int32_t m_levelCode;
		int32_t m_reserved1;
		int32_t m_reserved2;
		int32_t m_reserved3;
		int32_t m_ensembleMemberNumber;
		int32_t m_source;
		int32_t m_dataType;
		int32_t m_startAddressInData;
		int32_t m_rimWidthHaloSizes;
		int32_t m_ItemCode;
		int32_t m_pseudoLevel;
		int32_t m_userDefined;
		int32_t m_modelCode;
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
		int64_t m_validYear;
		int64_t m_validMonth;
		int64_t m_validDayOfMonth;
		int64_t m_validHour;
		int64_t m_validMinute;
		int64_t m_validDayNumberOrSeconds;
		int64_t m_dataYear;
		int64_t m_dataMonth;
		int64_t m_dataDayOfMonth;
		int64_t m_dataHour;
		int64_t m_dataMinute;
		int64_t m_dataDayNumberOrSeconds;
		int64_t m_timeIndicator;
		int64_t m_forecastPeriod;
		int64_t m_dataLength;
		int64_t m_gridCode;
		int64_t m_hemisphereIndicator;
		int64_t m_nRows;
		int64_t m_nPointsPerRow;
		int64_t m_extraDataLength;
		int64_t m_packingMethod;
		int64_t m_releaseNumber;
		int64_t m_fieldCode;
		int64_t m_secondFieldCode;
		int64_t m_processingCode;
		int64_t m_verticalCoordinateType;
		int64_t m_verticalCoordinateTypeForReferenceLevel;
		int64_t m_experimentNumber;
		int64_t m_startRecord;
		int64_t m_nRecords;
		int64_t m_projectionNumber;
		int64_t m_fieldType;
		int64_t m_levelCode;
		int64_t m_reserved1;
		int64_t m_reserved2;
		int64_t m_reserved3;
		int64_t m_ensembleMemberNumber;
		int64_t m_source;
		int64_t m_dataType;
		int64_t m_startAddressInData;
		int64_t m_rimWidthHaloSizes;
		int64_t m_ItemCode;
		int64_t m_pseudoLevel;
		int64_t m_userDefined;
		int64_t m_modelCode;
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

	struct FixedHeader32
	{
		int32_t m_formatVersionnumber; //1
		int32_t m_subModelIndicator; //2
		int32_t m_verticalCoordinateTypeIndicator; //3
		int32_t m_horizontalCoordinateTypeIndicator; //4
		int32_t m_datasetTypeIndicator; //5
		int32_t m_runIdentifier; //6
		int32_t m_experimentNumber; //7
		int32_t m_calendarIndicator; //8
		int32_t m_gridStaggeringIndicator; //9
		int32_t m_ancillaryFileTimeIndicator; //10
		int32_t m_projectionNumber; //11
		int32_t m_modelVersionNumber; //12
		int32_t m_undocumented1; //13
		int32_t m_typeOfObsFile; //14
		int32_t m_arithmeticOperationType; //15
		int32_t m_undocumented2; //16
		int32_t m_undocumented3; //17
		int32_t m_undocumented4; //18
		int32_t m_undocumented5; //19
		int32_t m_undocumented6; //20
		int32_t m_year1; //21
		int32_t m_month1; //22
		int32_t m_dayOfMonth1; //23
		int32_t m_hour1; //24
		int32_t m_minute1; //25
		int32_t m_second1; //26
		int32_t m_dayNumber1; //27
		int32_t m_year2; //28
		int32_t m_month2; //29
		int32_t m_dayOfMonth2; //30
		int32_t m_hour2; //31
		int32_t m_minute2; //32
		int32_t m_second2; //33
		int32_t m_dayNumber2; //34
		int32_t m_year3; //35
		int32_t m_month3; //36
		int32_t m_dayOfMonth3; //37
		int32_t m_hour3; //38
		int32_t m_minute3; //39
		int32_t m_second3; //40
		int32_t m_dayNumber3; //41
		int32_t m_undocumented7To64[58]; //42-99
		int32_t m_startIntegerConstants; //100
		int32_t m_nIntegerConstants; //101
		int32_t m_undocumented65; //102
		int32_t m_undocumented66; //103
		int32_t m_undocumented67; //104
		int32_t m_startRealConstants; //105
		int32_t m_nRealConstants; //106
		int32_t m_undocumented68; //107
		int32_t m_undocumented69; //108
		int32_t m_undocumented70; //109
		int32_t m_startLevelDependantConstants; //110
		int32_t m_nLevelDependantConstantsFirstDimension; //111
		int32_t m_nLevelDependantConstantsSecondDimension; //112
		int32_t m_undocumented71; //113
		int32_t m_undocumented72; //114
		int32_t m_startRowDependantConstants; //115
		int32_t m_nRowDependantConstantsFirstDimension; //116
		int32_t m_nRowDependantConstantsSecondDimension; //117
		int32_t m_undocumented73; //118
		int32_t m_undocumented74; //119
		int32_t m_startColumnDependantConstants; //120
		int32_t m_nColumnDependantConstantsFirstDimension; //121
		int32_t m_nColumnDependantConstantsSecondDimension; //122
		int32_t m_undocumented75; //123
		int32_t m_undocumented76; //124
		int32_t m_startFieldsOfConstants; //125
		int32_t m_nFieldsOfConstantsFirstDimension; //126
		int32_t m_nFieldsOfConstantsSecondDimension; //127
		int32_t m_undocumented77; //128
		int32_t m_undocumented78; //129
		int32_t m_startExtraConstants; //130
		int32_t m_nExtraConstants; //131
		int32_t m_undocumented79; //132
		int32_t m_undocumented80; //133
		int32_t m_undocumented81; //134
		int32_t m_startTempHistoryfile; //135
		int32_t m_nTempHistoryfile; //136
		int32_t m_undocumented82; //137
		int32_t m_undocumented83; //138
		int32_t m_undocumented84; //139
		int32_t m_startCompressedFieldIndex1; //140
		int32_t m_nCompressedFieldIndex1; //141
		int32_t m_startCompressedFieldIndex2; //142
		int32_t m_nCompressedFieldIndex2; //143
		int32_t m_startCompressedFieldIndex3; //144
		int32_t m_nCompressedFieldIndex3; //145
		int32_t m_undocumented85; //146
		int32_t m_undocumented86; //147
		int32_t m_undocumented87; //148
		int32_t m_undocumented88; //149
		int32_t m_startLookupTable; //150
		int32_t m_nLookupTableFirstDimension; //151
		int32_t m_nLookupTableSecondDimentsion; //152
		int32_t m_nPrognosticFields; //153
		int32_t m_undocumented89; //154
		int32_t m_undocumented90; //155
		int32_t m_undocumented91; //156
		int32_t m_undocumented92; //157
		int32_t m_undocumented93; //158
		int32_t m_undocumented94; //159
		int32_t m_startOfData; //160
		int32_t m_dimensionOfData; //161
		int32_t m_maximumLengthOfAllFields; //162
	};

	struct FixedHeader64
	{
		int64_t m_formatVersionnumber; //1
		int64_t m_subModelIndicator; //2
		int64_t m_verticalCoordinateTypeIndicator; //3
		int64_t m_horizontalCoordinateTypeIndicator; //4
		int64_t m_datasetTypeIndicator; //5
		int64_t m_runIdentifier; //6
		int64_t m_experimentNumber; //7
		int64_t m_calendarIndicator; //8
		int64_t m_gridStaggeringIndicator; //9
		int64_t m_ancillaryFileTimeIndicator; //10
		int64_t m_projectionNumber; //11
		int64_t m_modelVersionNumber; //12
		int64_t m_undocumented1; //13
		int64_t m_typeOfObsFile; //14
		int64_t m_arithmeticOperationType; //15
		int64_t m_undocumented2; //16
		int64_t m_undocumented3; //17
		int64_t m_undocumented4; //18
		int64_t m_undocumented5; //19
		int64_t m_undocumented6; //20
		int64_t m_year1; //21
		int64_t m_month1; //22
		int64_t m_dayOfMonth1; //23
		int64_t m_hour1; //24
		int64_t m_minute1; //25
		int64_t m_second1; //26
		int64_t m_dayNumber1; //27
		int64_t m_year2; //28
		int64_t m_month2; //29
		int64_t m_dayOfMonth2; //30
		int64_t m_hour2; //31
		int64_t m_minute2; //32
		int64_t m_second2; //33
		int64_t m_dayNumber2; //34
		int64_t m_year3; //35
		int64_t m_month3; //36
		int64_t m_dayOfMonth3; //37
		int64_t m_hour3; //38
		int64_t m_minute3; //39
		int64_t m_second3; //40
		int64_t m_dayNumber3; //41
		int64_t m_undocumented7To64[58]; //42-99
		int64_t m_startIntegerConstants; //100
		int64_t m_nIntegerConstants; //101
		int64_t m_undocumented65; //102
		int64_t m_undocumented66; //103
		int64_t m_undocumented67; //104
		int64_t m_startRealConstants; //105
		int64_t m_nRealConstants; //106
		int64_t m_undocumented68; //107
		int64_t m_undocumented69; //108
		int64_t m_undocumented70; //109
		int64_t m_startLevelDependantConstants; //110
		int64_t m_nLevelDependantConstantsFirstDimension; //111
		int64_t m_nLevelDependantConstantsSecondDimension; //112
		int64_t m_undocumented71; //113
		int64_t m_undocumented72; //114
		int64_t m_startRowDependantConstants; //115
		int64_t m_nRowDependantConstantsFirstDimension; //116
		int64_t m_nRowDependantConstantsSecondDimension; //117
		int64_t m_undocumented73; //118
		int64_t m_undocumented74; //119
		int64_t m_startColumnDependantConstants; //120
		int64_t m_nColumnDependantConstantsFirstDimension; //121
		int64_t m_nColumnDependantConstantsSecondDimension; //122
		int64_t m_undocumented75; //123
		int64_t m_undocumented76; //124
		int64_t m_startFieldsOfConstants; //125
		int64_t m_nFieldsOfConstantsFirstDimension; //126
		int64_t m_nFieldsOfConstantsSecondDimension; //127
		int64_t m_undocumented77; //128
		int64_t m_undocumented78; //129
		int64_t m_startExtraConstants; //130
		int64_t m_nExtraConstants; //131
		int64_t m_undocumented79; //132
		int64_t m_undocumented80; //133
		int64_t m_undocumented81; //134
		int64_t m_startTempHistoryfile; //135
		int64_t m_nTempHistoryfile; //136
		int64_t m_undocumented82; //137
		int64_t m_undocumented83; //138
		int64_t m_undocumented84; //139
		int64_t m_startCompressedFieldIndex1; //140
		int64_t m_nCompressedFieldIndex1; //141
		int64_t m_startCompressedFieldIndex2; //142
		int64_t m_nCompressedFieldIndex2; //143
		int64_t m_startCompressedFieldIndex3; //144
		int64_t m_nCompressedFieldIndex3; //145
		int64_t m_undocumented85; //146
		int64_t m_undocumented86; //147
		int64_t m_undocumented87; //148
		int64_t m_undocumented88; //149
		int64_t m_startLookupTable; //150
		int64_t m_nLookupTableFirstDimension; //151
		int64_t m_nLookupTableSecondDimension; //152
		int64_t m_nPrognosticFields; //153
		int64_t m_undocumented89; //154
		int64_t m_undocumented90; //155
		int64_t m_undocumented91; //156
		int64_t m_undocumented92; //157
		int64_t m_undocumented93; //158
		int64_t m_undocumented94; //159
		int64_t m_startOfData; //160
		int64_t m_dimensionOfData; //161
		int64_t m_maximumLengthOfAllFields; //162
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
			return ((T*)&m_header)[index];
		}
	public:
		bool operator < (const Section32 & rhs) const
		{
			if(m_parent->m_comparator<45)
				return at<int32_t>(m_parent->m_comparator) < rhs.at<int32_t> (m_parent->m_comparator);
			else
				return at<float>(m_parent->m_comparator) < rhs.at<float> (m_parent->m_comparator);
		}
		void readHeader( std::fstream *fin, size_t nBytes);
		void setDataStart( int32_t start )
		{
			m_dataStart = start;
		}
		void setDataSize( int32_t size )
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
		Section32()
		{
			m_dataStart = 0;
			m_dataBytes = 0;
			m_parent = nullptr;
		}
		PpHeader32 getHeader() {return m_header;}

	};
	class Section64
	{
		friend class UmFile;
	private:
		PpHeader64 m_header;
		std::streamoff m_dataStart;
		size_t m_dataBytes;
		UmFile *m_parent;
		size_t m_index;
		template <class T>
		T at(size_t index) const
		{
			return ((T*)&m_header)[index];
		}
	public:
		bool operator < (const Section64 & rhs) const
		{
			if(m_parent->m_comparator<45)
				return at<int64_t>(m_parent->m_comparator) < rhs.at<int64_t> (m_parent->m_comparator);
			else
				return at<double>(m_parent->m_comparator) < rhs.at<double> (m_parent->m_comparator);
		}
		void readHeader( std::fstream *fin, size_t nBytes);
		void setDataStart( int64_t start )
		{
			m_dataStart = start;
		}
		void setDataSize( int64_t size )
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
		Section64( const Section32 &section32 )
		{
			//*this = section32;
			m_dataStart = section32.m_dataStart;
			m_dataBytes = section32.m_dataBytes;
			m_parent = section32.m_parent;
			for( size_t i=0; i<45; ++i)
				*((int64_t*)(&m_header) + i ) = section32.at<int32_t>( i );
			for( size_t i=46; i<64; ++i)
				*((double*)(&m_header) + i ) = section32.at<float>( i );
		}
		Section64()
		{
			m_dataStart = 0;
			m_dataBytes = 0;
			m_parent = nullptr;
		}
		PpHeader64 getHeader() {return m_header;}
	};


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
	std::vector<PpHeader64> getFilteredHeaders();
	static void getSectionAxesCentres(const PpHeader64 &header, std::vector<double> &x, std::vector<double> &y);
	static void getSectionAxesEdges(const PpHeader64 &header, std::vector<double> &x, std::vector<double> &y);

	//return the header for a particular section
	PpHeader64 getHeader(size_t sectionIndex){return m_sections[sectionIndex].m_header;}
	//get the number of sections in the file
	size_t getNumSections(){return m_sections.size();}
	//get a list of the stash codes in the file, sorted ascending, no entry will appear more than once
	std::vector<int64_t> getStashCodeList();
	
	//get the x and y values for a section works for regular x/y grids. throws an error for
	//spectral grids
	void getSectionAxesCentres(size_t sectionIndex, std::vector<double> &x, std::vector<double> &y);
	void getSectionAxesEdges(size_t sectionIndex, std::vector<double> &x, std::vector<double> &y);
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
	std::shared_ptr<UmFileParser> m_umFileParser;
	std::fstream m_fin;
	FixedHeader64 m_fixedHeader;
	bool m_swapEndian;
	std::vector<Section64> m_sections;
	std::vector<Section64> m_filteredSections;
	size_t m_comparator;

	//get data for a given section
	void getData(const Section64 &section, std::vector<std::vector<double>> &result);
	//int32_t getNextRecordSize();
	//void readRecord(int32_t* record, std::basic_istream<char>::pos_type nBytes);
	//void skipRecord(std::basic_istream<char>::pos_type nBytes);
	void decompressWgdos(char *dataIn, size_t nIn, double** dataOut, size_t outDim1, size_t outDim2);
	//convert an IBM float to an IEEE float. Note we pass a pointer to the float as
	//a void * to avoid acidentally doing implicit conversions e.g. if the bit are 
	//actually held in a int32_t
	float fromIbmFloat(void *ibmFloat);
	double fromIbmDouble(void *ibmFloat);

};

//UmFileParser
//This class forms a base class defining the interface for various different UM file types.
//It provides two virtual functions which child classes must overload.
//The first is parse. This must read from the fstream given starting at its current get pointer
//and parse the pp headers in the file. The pp headers are then returned as a vector. If 
//swapEndian is true then the endianness of the read words must be reversed. In addition to the 
//pp file headers, the Sections returned must have the location and size of each pp data section
//set and it must have the parent set to the parent passed in.
//The checkValidWords function must check the words passed in and if these are potentially valid for
//the file the function must return true, otherwise return false. Note that this function should not
//attempt to swap the endianness of the word passed in. Note that there is a 32 and 64 bit version of
//this function. Clearly a 32 bit reader should only return true for a 32 bit word so only needs to
//overload the 32 bit version as the default is to return false.
class UmFileParser
{
	friend class UmFile;
private:
	virtual std::vector<UmFile::Section64> parse( std::fstream *fin, UmFile *parent, bool swapEndian ) = 0;
	virtual void readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian, bool force32Bit ) = 0;
	virtual bool checkValidWords( int32_t firstWord, int32_t fifthWord )  { return false; }
	virtual bool checkValidWords( int64_t firstWord, int64_t fifthWord ) { return false; }
	template < class T >
	bool checkValidWords( T, T ) = delete; //This stops any other types resulting in callin the above versions
protected:
};

class PpFileParser32 : public UmFileParser
{
	std::vector<UmFile::Section64> parse( std::fstream *fin, UmFile *parent, bool swapEndian );
	void readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian, bool force32Bit );
	bool checkValidWords( int32_t firstWord, int32_t fifthWord );
	int32_t getNextRecordSize( std::fstream * fin, bool swapEndian );
	void skipRecord( std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian );
};

class PpFileParser64 : public UmFileParser
{
	std::vector<UmFile::Section64> parse( std::fstream *fin, UmFile *parent, bool swapEndian );
	void readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian, bool force32Bit );
	bool checkValidWords( int64_t firstWord, int64_t fifthWord );
	int64_t getNextRecordSize( std::fstream * fin, bool swapEndian );
	void skipRecord( std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian );
};

class FieldsFileParser32 : public UmFileParser
{
	std::vector<UmFile::Section64> parse( std::fstream *fin, UmFile *parent, bool swapEndian );
	void readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian, bool force32Bit );
	bool checkValidWords( int32_t firstWord, int32_t fifthWord );
};

class FieldsFileParser64 : public UmFileParser
{
	std::vector<UmFile::Section64> parse( std::fstream *fin, UmFile *parent, bool swapEndian );
	void readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian, bool force32Bit );
	bool checkValidWords( int64_t firstWord, int64_t fifthWord );
};



template <class T>
UmFile& UmFile::operator > ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if( m_comparator > 44 )
		{
			if(!(m_filteredSections[i].at<double>(m_comparator) > rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
		else
		{
			if(!(m_filteredSections[i].at<int64_t>(m_comparator) > rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
	}
	return *this;
}

template <class T>
UmFile& UmFile::operator >= ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if( m_comparator > 44 )
		{
			if(!(m_filteredSections[i].at<double>(m_comparator) >= rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
		else
		{
			if(!(m_filteredSections[i].at<int64_t>(m_comparator) >= rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
	}
	return *this;
}

template <class T>
UmFile& UmFile::operator < ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if( m_comparator > 44 )
		{
			if(!(m_filteredSections[i].at<double>(m_comparator) < rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
		else
		{
			if(!(m_filteredSections[i].at<int64_t>(m_comparator) < rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
	}
	return *this;
}

template <class T>
UmFile& UmFile::operator <= ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if( m_comparator > 44 )
		{
			if(!(m_filteredSections[i].at<double>(m_comparator) <= rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
		else
		{
			if(!(m_filteredSections[i].at<int64_t>(m_comparator) <= rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
	}
	return *this;
}

template <class T>
UmFile& UmFile::operator == ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if( m_comparator > 44 )
		{
			if(!(m_filteredSections[i].at<double>(m_comparator) == rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
		else
		{
			if(!(m_filteredSections[i].at<int64_t>(m_comparator) == rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
	}
	return *this;
}

template <class T>
UmFile& UmFile::operator != ( T rhs )
{
	for(size_t i=0; i<m_filteredSections.size(); ++i)
	{
		if( m_comparator > 44 )
		{
			if(!(m_filteredSections[i].at<double>(m_comparator) != rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
		else
		{
			if(!(m_filteredSections[i].at<int64_t>(m_comparator) != rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
	}
	return *this;
}



#endif