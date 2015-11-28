#ifdef _WIN32
#include"../include/svector/dep/windows.h"
#endif
#ifdef _WIN32
#include"../include/svector/MemoryMap.h"
#include"../include/svector/serr.h"

sci::MemoryMap::Handle::Handle(size_t nBytes, std::string name)
{
	m_mapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		nBytes/DWORD(-1),                       // maximum object size (high-order DWORD)
		nBytes%DWORD(-1),// maximum object size (low-order DWORD)
		std::wstring(name.begin(),name.end()).c_str());                 // name of mapping object

	sci::assertThrow(m_mapFile!=NULL, sci::err());
	m_count=new size_t;
	*m_count=1;
}

sci::MemoryMap::Handle::Handle(std::string name)
{
	m_mapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		std::wstring(name.begin(),name.end()).c_str()); // name of mapping object

	sci::assertThrow(m_mapFile!=NULL, sci::err());
	m_count=new size_t;
	*m_count=1;
}

sci::MemoryMap::Handle::Handle(const MemoryMap::Handle &memoryMapHandle)
{
	m_count=memoryMapHandle.m_count;
	m_mapFile=memoryMapHandle.m_mapFile;
	(*m_count)++;
}
sci::MemoryMap::Handle & sci::MemoryMap::Handle::operator=(const MemoryMap::Handle &rhs)
{
	m_count=rhs.m_count;
	m_mapFile=rhs.m_mapFile;
	(*m_count)++;
	return *this;
}
sci::MemoryMap::Handle::~Handle()
{
	if(*m_count==1)
		CloseHandle(m_mapFile);
	(*m_count)--;
}


sci::MemoryMap::Buffer::Buffer(MemoryMap::Handle &memoryMapHandle, size_t offset, size_t nBytes)
{
	m_buffer = MapViewOfFile(memoryMapHandle.m_mapFile,   // handle to map object
						FILE_MAP_ALL_ACCESS, // read/write permission
						offset/DWORD(-1), //high order offset
						offset%DWORD(-1), //low order offset
						nBytes);//maps the whole file
	sci::assertThrow(m_buffer!=NULL, sci::err());
	m_count=new size_t;
	*m_count=1;
}
sci::MemoryMap::Buffer::Buffer(const MemoryMap::Buffer &memoryMapBuffer)
{
	m_count=memoryMapBuffer.m_count;
	m_buffer=memoryMapBuffer.m_buffer;
	(*m_count)++;
}
sci::MemoryMap::Buffer & sci::MemoryMap::Buffer::operator=(const MemoryMap::Buffer &rhs)
{
	m_count=rhs.m_count;
	m_buffer=rhs.m_buffer;
	(*m_count)++;
	return *this;
}
sci::MemoryMap::Buffer::~Buffer()
{
	if(*m_count==1)
		UnmapViewOfFile(m_buffer);
	(*m_count)--;
}

void *sci::MemoryMap::Buffer::getBuffer()
{
	return m_buffer;
}

sci::MemoryMap::MemoryMap(size_t nBytes, std::string name)
	:m_memoryMapHandle(nBytes, name)
{
}
sci::MemoryMap::MemoryMap(std::string name)
	:m_memoryMapHandle(name)
{
}
void *sci::MemoryMap::getVoidBuffer(size_t offsetBytes, size_t nBytes)
{
	m_memoryMapBuffers.push_back(MemoryMap::Buffer(m_memoryMapHandle,offsetBytes,nBytes));
	return m_memoryMapBuffers.back().getBuffer();
}
#endif
