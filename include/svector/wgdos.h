#ifndef WGDOS_H
#define WGDOS_H
#include<vector>

class WgdosExtractor
{
public:
	//pass a pointer to the data you wish to decompress and number of bits per point
	//in theory the data must have an extra 3bytes padding at the end to avoid possible
	//illegal reads.
	WgdosExtractor(char *compressedData, __int16 bitsPerPoint);
	unsigned __int32 getNextDataPoint();
	bool getNextBitmapMask();
private:
	const unsigned __int32 *m_data;
	__int16 m_bitsPerPoint;
	 __int16 m_stepsInCycle;
	unsigned __int32 m_dataMasks1[1024];
	unsigned __int32 m_dataMasks2[1024];
	__int16 m_bytePositions[1024];
	char m_bitmapMasks[8];
	size_t m_currentDataMask;
	size_t m_currentBitmapMask;
	bool m_extractingBitmaps;
	bool m_extractingData;
	__int16 m_bitmapBit;
};

#endif