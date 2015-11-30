#include"../include/svector/dep/wgdos.h"
#include"../include/svector/dep/ppfile.h"
#include<assert.h>

WgdosExtractor::WgdosExtractor(char *compressedData, int16_t bitsPerPoint)
:m_data((uint32_t*)compressedData)
{
	assert (bitsPerPoint<=32);
	if(bitsPerPoint>32)
		throw(PPERR_WGDOSCOMPRESSION_BPP_TOO_HIGH);

	m_stepsInCycle=0;
	int16_t bytepos=0;
	size_t step=0;
	do
	{
		m_dataMasks1[step]=0;
		for(__int32 i=0; i<std::min(bitsPerPoint,int16_t(32-bytepos)); ++i)
			m_dataMasks1[step] |= (uint32_t(0x80000000) >> (i+bytepos));
		m_dataMasks2[step]=0;
		for(__int32 i=0; i<bitsPerPoint+bytepos-32; ++i)
			m_dataMasks2[step] |= (uint32_t(0x80000000) >> i);
		m_bytePositions[step]=bytepos;

		++step;
		++m_stepsInCycle;
		bytepos+=bitsPerPoint;
		if(bytepos>31)
			bytepos-=32;
	}while(bytepos!=0);

	m_bitsPerPoint=bitsPerPoint;
	m_currentDataMask=0;
	m_currentBitmapMask=0;

	m_extractingBitmaps=false;
	m_extractingData=false;

	m_bitmapBit=0;


}

uint32_t WgdosExtractor::getNextDataPoint()
{
	//note this assumes the compressed data is unsigned.
	
	if(m_extractingBitmaps &&m_bitmapBit!=0)
		m_data+=1;

	m_extractingData=true;
	m_extractingBitmaps=false;

	int16_t rightShiftAmount=32-(m_bytePositions[m_currentDataMask]+m_bitsPerPoint);
	uint32_t result;
	if(rightShiftAmount>0)
		result=((*m_data)&(m_dataMasks1[m_currentDataMask])) >> rightShiftAmount;
	else
		result=((*m_data)&(m_dataMasks1[m_currentDataMask])) << -rightShiftAmount;

	if(m_dataMasks2[m_currentDataMask] != 0)
	{
		//in this case rightShiftAmount should always be negative
		assert(rightShiftAmount<0);
		result|=((*(m_data+1))&(m_dataMasks2[m_currentDataMask])) >> (32+rightShiftAmount);
	}

	if(m_bytePositions[m_currentDataMask]+m_bitsPerPoint > 31)
		m_data+=1;

	if(m_currentDataMask==m_stepsInCycle-1)
		m_currentDataMask=0;
	else
		++m_currentDataMask;

	return result;
}

bool WgdosExtractor::getNextBitmapMask()
{
	assert(!m_extractingData);
	
	bool result=(((*m_data) >> (31-m_bitmapBit)) << 31) == 0;
	if(m_bitmapBit==31)
	{
		m_bitmapBit=0;
		m_data+=1;
	}
	else
		++m_bitmapBit;
	return result;
}
