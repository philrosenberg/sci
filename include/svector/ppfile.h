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

	struct FixedHeader32
	{
		__int32 m_formatVersionnumber; //1
		__int32 m_subModelIndicator; //2
		__int32 m_verticalCoordinateTypeIndicator; //3
		__int32 m_horizontalCoordinateTypeIndicator; //4
		__int32 m_datasetTypeIndicator; //5
		__int32 m_runIdentifier; //6
		__int32 m_experimentNumber; //7
		__int32 m_calendarIndicator; //8
		__int32 m_gridStaggeringIndicator; //9
		__int32 m_ancillaryFileTimeIndicator; //10
		__int32 m_projectionNumber; //11
		__int32 m_modelVersionNumber; //12
		__int32 m_undocumented1; //13
		__int32 m_typeOfObsFile; //14
		__int32 m_arithmeticOperationType; //15
		__int32 m_undocumented2; //16
		__int32 m_undocumented3; //17
		__int32 m_undocumented4; //18
		__int32 m_undocumented5; //19
		__int32 m_undocumented6; //20
		__int32 m_year1; //21
		__int32 m_month1; //22
		__int32 m_dayOfMonth1; //23
		__int32 m_hour1; //24
		__int32 m_minute1; //25
		__int32 m_second1; //26
		__int32 m_dayNumber1; //27
		__int32 m_year2; //28
		__int32 m_month2; //29
		__int32 m_dayOfMonth2; //30
		__int32 m_hour2; //31
		__int32 m_minute2; //32
		__int32 m_second2; //33
		__int32 m_dayNumber2; //34
		__int32 m_year3; //35
		__int32 m_month3; //36
		__int32 m_dayOfMonth3; //37
		__int32 m_hour3; //38
		__int32 m_minute3; //39
		__int32 m_second3; //40
		__int32 m_dayNumber3; //41
		__int32 m_undocumented7To64[58]; //42-99
		__int32 m_startIntegerConstants; //100
		__int32 m_nIntegerConstants; //101
		__int32 m_undocumented65; //102
		__int32 m_undocumented66; //103
		__int32 m_undocumented67; //104
		__int32 m_startRealConstants; //105
		__int32 m_nRealConstants; //106
		__int32 m_undocumented68; //107
		__int32 m_undocumented69; //108
		__int32 m_undocumented70; //109
		__int32 m_startLevelDependantConstants; //110
		__int32 m_nLevelDependantConstantsFirstDimension; //111
		__int32 m_nLevelDependantConstantsSecondDimension; //112
		__int32 m_undocumented71; //113
		__int32 m_undocumented72; //114
		__int32 m_startRowDependantConstants; //115
		__int32 m_nRowDependantConstantsFirstDimension; //116
		__int32 m_nRowDependantConstantsSecondDimension; //117
		__int32 m_undocumented73; //118
		__int32 m_undocumented74; //119
		__int32 m_startColumnDependantConstants; //120
		__int32 m_nColumnDependantConstantsFirstDimension; //121
		__int32 m_nColumnDependantConstantsSecondDimension; //122
		__int32 m_undocumented75; //123
		__int32 m_undocumented76; //124
		__int32 m_startFieldsOfConstants; //125
		__int32 m_nFieldsOfConstantsFirstDimension; //126
		__int32 m_nFieldsOfConstantsSecondDimension; //127
		__int32 m_undocumented77; //128
		__int32 m_undocumented78; //129
		__int32 m_startExtraConstants; //130
		__int32 m_nExtraConstants; //131
		__int32 m_undocumented79; //132
		__int32 m_undocumented80; //133
		__int32 m_undocumented81; //134
		__int32 m_startTempHistoryfile; //135
		__int32 m_nTempHistoryfile; //136
		__int32 m_undocumented82; //137
		__int32 m_undocumented83; //138
		__int32 m_undocumented84; //139
		__int32 m_startCompressedFieldIndex1; //140
		__int32 m_nCompressedFieldIndex1; //141
		__int32 m_startCompressedFieldIndex2; //142
		__int32 m_nCompressedFieldIndex2; //143
		__int32 m_startCompressedFieldIndex3; //144
		__int32 m_nCompressedFieldIndex3; //145
		__int32 m_undocumented85; //146
		__int32 m_undocumented86; //147
		__int32 m_undocumented87; //148
		__int32 m_undocumented88; //149
		__int32 m_startLookupTable; //150
		__int32 m_nLookupTableFirstDimension; //151
		__int32 m_nLookupTableSecondDimentsion; //152
		__int32 m_nPrognosticFields; //153
		__int32 m_undocumented89; //154
		__int32 m_undocumented90; //155
		__int32 m_undocumented91; //156
		__int32 m_undocumented92; //157
		__int32 m_undocumented93; //158
		__int32 m_undocumented94; //159
		__int32 m_startOfData; //160
		__int32 m_dimensionOfData; //161
		__int32 m_maximumLengthOfAllFields; //162
	};

	struct FixedHeader64
	{
		__int64 m_formatVersionnumber; //1
		__int64 m_subModelIndicator; //2
		__int64 m_verticalCoordinateTypeIndicator; //3
		__int64 m_horizontalCoordinateTypeIndicator; //4
		__int64 m_datasetTypeIndicator; //5
		__int64 m_runIdentifier; //6
		__int64 m_experimentNumber; //7
		__int64 m_calendarIndicator; //8
		__int64 m_gridStaggeringIndicator; //9
		__int64 m_ancillaryFileTimeIndicator; //10
		__int64 m_projectionNumber; //11
		__int64 m_modelVersionNumber; //12
		__int64 m_undocumented1; //13
		__int64 m_typeOfObsFile; //14
		__int64 m_arithmeticOperationType; //15
		__int64 m_undocumented2; //16
		__int64 m_undocumented3; //17
		__int64 m_undocumented4; //18
		__int64 m_undocumented5; //19
		__int64 m_undocumented6; //20
		__int64 m_year1; //21
		__int64 m_month1; //22
		__int64 m_dayOfMonth1; //23
		__int64 m_hour1; //24
		__int64 m_minute1; //25
		__int64 m_second1; //26
		__int64 m_dayNumber1; //27
		__int64 m_year2; //28
		__int64 m_month2; //29
		__int64 m_dayOfMonth2; //30
		__int64 m_hour2; //31
		__int64 m_minute2; //32
		__int64 m_second2; //33
		__int64 m_dayNumber2; //34
		__int64 m_year3; //35
		__int64 m_month3; //36
		__int64 m_dayOfMonth3; //37
		__int64 m_hour3; //38
		__int64 m_minute3; //39
		__int64 m_second3; //40
		__int64 m_dayNumber3; //41
		__int64 m_undocumented7To64[58]; //42-99
		__int64 m_startIntegerConstants; //100
		__int64 m_nIntegerConstants; //101
		__int64 m_undocumented65; //102
		__int64 m_undocumented66; //103
		__int64 m_undocumented67; //104
		__int64 m_startRealConstants; //105
		__int64 m_nRealConstants; //106
		__int64 m_undocumented68; //107
		__int64 m_undocumented69; //108
		__int64 m_undocumented70; //109
		__int64 m_startLevelDependantConstants; //110
		__int64 m_nLevelDependantConstantsFirstDimension; //111
		__int64 m_nLevelDependantConstantsSecondDimension; //112
		__int64 m_undocumented71; //113
		__int64 m_undocumented72; //114
		__int64 m_startRowDependantConstants; //115
		__int64 m_nRowDependantConstantsFirstDimension; //116
		__int64 m_nRowDependantConstantsSecondDimension; //117
		__int64 m_undocumented73; //118
		__int64 m_undocumented74; //119
		__int64 m_startColumnDependantConstants; //120
		__int64 m_nColumnDependantConstantsFirstDimension; //121
		__int64 m_nColumnDependantConstantsSecondDimension; //122
		__int64 m_undocumented75; //123
		__int64 m_undocumented76; //124
		__int64 m_startFieldsOfConstants; //125
		__int64 m_nFieldsOfConstantsFirstDimension; //126
		__int64 m_nFieldsOfConstantsSecondDimension; //127
		__int64 m_undocumented77; //128
		__int64 m_undocumented78; //129
		__int64 m_startExtraConstants; //130
		__int64 m_nExtraConstants; //131
		__int64 m_undocumented79; //132
		__int64 m_undocumented80; //133
		__int64 m_undocumented81; //134
		__int64 m_startTempHistoryfile; //135
		__int64 m_nTempHistoryfile; //136
		__int64 m_undocumented82; //137
		__int64 m_undocumented83; //138
		__int64 m_undocumented84; //139
		__int64 m_startCompressedFieldIndex1; //140
		__int64 m_nCompressedFieldIndex1; //141
		__int64 m_startCompressedFieldIndex2; //142
		__int64 m_nCompressedFieldIndex2; //143
		__int64 m_startCompressedFieldIndex3; //144
		__int64 m_nCompressedFieldIndex3; //145
		__int64 m_undocumented85; //146
		__int64 m_undocumented86; //147
		__int64 m_undocumented87; //148
		__int64 m_undocumented88; //149
		__int64 m_startLookupTable; //150
		__int64 m_nLookupTableFirstDimension; //151
		__int64 m_nLookupTableSecondDimension; //152
		__int64 m_nPrognosticFields; //153
		__int64 m_undocumented89; //154
		__int64 m_undocumented90; //155
		__int64 m_undocumented91; //156
		__int64 m_undocumented92; //157
		__int64 m_undocumented93; //158
		__int64 m_undocumented94; //159
		__int64 m_startOfData; //160
		__int64 m_dimensionOfData; //161
		__int64 m_maximumLengthOfAllFields; //162
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
				return at<__int32>(m_parent->m_comparator) < rhs.at<__int32> (m_parent->m_comparator);
			else
				return at<float>(m_parent->m_comparator) < rhs.at<float> (m_parent->m_comparator);
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
				return at<__int64>(m_parent->m_comparator) < rhs.at<__int64> (m_parent->m_comparator);
			else
				return at<double>(m_parent->m_comparator) < rhs.at<double> (m_parent->m_comparator);
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
		Section64( const Section32 &section32 )
		{
			//*this = section32;
			m_dataStart = section32.m_dataStart;
			m_dataBytes = section32.m_dataBytes;
			m_parent = section32.m_parent;
			for( size_t i=0; i<45; ++i)
				*((__int64*)(&m_header) + i ) = section32.at<__int32>( i );
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
	static void getSectionAxes(const PpHeader64 &header, std::vector<double> &x, std::vector<double> &y);

	//return the header for a particular section
	PpHeader64 getHeader(size_t sectionIndex){return m_sections[sectionIndex].m_header;}
	//get the number of sections in the file
	size_t getNumSections(){return m_sections.size();}
	//get a list of the stash codes in the file, sorted ascending, no entry will appear more than once
	std::vector<__int64> getStashCodeList();
	
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
	std::shared_ptr<UmFileParser> m_umFileParser;
	std::fstream m_fin;
	FixedHeader64 m_fixedHeader;
	bool m_swapEndian;
	std::vector<Section64> m_sections;
	std::vector<Section64> m_filteredSections;
	size_t m_comparator;

	//get data for a given section
	void getData(const Section64 &section, std::vector<std::vector<double>> &result);
	//__int32 getNextRecordSize();
	//void readRecord(__int32* record, std::basic_istream<char>::pos_type nBytes);
	//void skipRecord(std::basic_istream<char>::pos_type nBytes);
	void decompressWgdos(char *dataIn, size_t nIn, double** dataOut, size_t outDim1, size_t outDim2);
	//convert an IBM float to an IEEE float. Note we pass a pointer to the float as
	//a void * to avoid acidentally doing implicit conversions e.g. if the bit are 
	//actually held in a __int32
	float fromIbmFloat(void *ibmFloat);
	double UmFile::fromIbmDouble(void *ibmFloat);

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
	virtual bool checkValidWords( __int32 firstWord, __int32 fifthWord )  { return false; }
	virtual bool checkValidWords( __int64 firstWord, __int64 fifthWord ) { return false; }
	template < class T >
	bool checkValidWords( T, T ) = delete; //This stops any other types resulting in callin the above versions
protected:
};

class PpFileParser32 : public UmFileParser
{
	std::vector<UmFile::Section64> parse( std::fstream *fin, UmFile *parent, bool swapEndian );
	void readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian, bool force32Bit );
	bool checkValidWords( __int32 firstWord, __int32 fifthWord );
	__int32 getNextRecordSize( std::fstream * fin, bool swapEndian );
	void skipRecord( std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian );
};

class PpFileParser64 : public UmFileParser
{
	std::vector<UmFile::Section64> parse( std::fstream *fin, UmFile *parent, bool swapEndian );
	void readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian, bool force32Bit );
	bool checkValidWords( __int64 firstWord, __int64 fifthWord );
	__int64 getNextRecordSize( std::fstream * fin, bool swapEndian );
	void skipRecord( std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian );
};

class FieldsFileParser32 : public UmFileParser
{
	std::vector<UmFile::Section64> parse( std::fstream *fin, UmFile *parent, bool swapEndian );
	void readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian, bool force32Bit );
	bool checkValidWords( __int32 firstWord, __int32 fifthWord );
};

class FieldsFileParser64 : public UmFileParser
{
	std::vector<UmFile::Section64> parse( std::fstream *fin, UmFile *parent, bool swapEndian );
	void readRecord( void * record, std::fstream *fin, std::basic_istream<char>::pos_type nBytes, bool swapEndian, bool force32Bit );
	bool checkValidWords( __int64 firstWord, __int64 fifthWord );
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
			if(!(m_filteredSections[i].at<__int64>(m_comparator) > rhs))
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
			if(!(m_filteredSections[i].at<__int64>(m_comparator) >= rhs))
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
			if(!(m_filteredSections[i].at<__int64>(m_comparator) < rhs))
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
			if(!(m_filteredSections[i].at<_int64>(m_comparator) <= rhs))
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
			if(!(m_filteredSections[i].at<__int64>(m_comparator) == rhs))
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
			if(!(m_filteredSections[i].at<__int64>(m_comparator) != rhs))
			{
				m_filteredSections.erase(m_filteredSections.begin()+i);
				--i;
			}
		}
	}
	return *this;
}



#endif