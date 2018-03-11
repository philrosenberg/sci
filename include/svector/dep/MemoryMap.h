#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include<string>
#include<vector>

namespace sci
{
	class MemoryMap
	{
		//pre declaration of internal classes
		class Buffer;
		class Handle;

		class Handle
		{
			friend class MemoryMap::Buffer;
		public:
			Handle(size_t nBytes, std::string name);
			Handle(std::string name);
			Handle(const MemoryMap::Handle &memoryMapHandle);
			Handle & operator=(const MemoryMap::Handle &rhs);
			~Handle();
		private:
			HANDLE m_mapFile;
			size_t *m_count;
		};

		class Buffer
		{
		public:
			Buffer(MemoryMap::Handle &memoryMapHandle, size_t offset, size_t nBytes);
			Buffer(const MemoryMap::Buffer &memoryMapBuffer);
			Buffer & operator=(const MemoryMap::Buffer &rhs);
			~Buffer();
			void *getBuffer();
		private:
			void *m_buffer;
			size_t *m_count;
		};
		public:
		MemoryMap(size_t nBytes, std::string name);
		MemoryMap(std::string name);
		void *getVoidBuffer(size_t offsetBytes=0, size_t nBytes=0);
		//This version gets a pointer to the buffer cast as a T*, the offset and size are in terms
		//of the type, i.e. with T=int and size=5 you would get a buffer of 5 ints, not 5 bytes
		template<class T>
		T *getBuffer(size_t offsetIndex=0, size_t size=0);
		//this frees up a buffer you have requested, note that there is no requirement to do this
		//as all buffers will be freed when the Memory Map is destroyed. However, you may wish to free
		//a buffer sto save memory.
		void freeBuffer(void *buffer);

		private:
		MemoryMap::Handle m_memoryMapHandle;
		std::vector<MemoryMap::Buffer> m_memoryMapBuffers;

	};
}

#endif
