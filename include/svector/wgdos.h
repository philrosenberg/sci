#ifndef WGDOS_H
#define WGDOS_H
#include<vector>
#include<stdint.h>

class WgdosExtractor
{
public:
	//pass a pointer to the data you wish to decompress and number of bits per point
	//in theory the data must have an extra 3bytes padding at the end to avoid possible
	//illegal reads.
	WgdosExtractor(char *compressedData, __int16 bitsPerPoint);
	uint32_t getNextDataPoint();
	bool getNextBitmapMask();
private:
	const uint32_t *m_data;
	int16_t m_bitsPerPoint;
	int16_t m_stepsInCycle;
	uint32_t m_dataMasks1[1024];
	uint32_t m_dataMasks2[1024];
	int16_t m_bytePositions[1024];
	char m_bitmapMasks[8];
	size_t m_currentDataMask;
	size_t m_currentBitmapMask;
	bool m_extractingBitmaps;
	bool m_extractingData;
	int16_t m_bitmapBit;
};

#endif