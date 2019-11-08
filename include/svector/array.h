#pragma once

#include<vector>

namespace sci
{
	template <class TYPE, size_t NDIMS>
	class ArrayTransferer
	{
	public:
		template<class ITERATOR>
		static void transfer(ITERATOR destination, ITERATOR source, const size_t* destinationSize, const size_t* sourceSize, const size_t* destinationStride, const size_t* sourceStride)
		{
			size_t n = *destinationSize < *sourceSize ? *destinationSize : *sourceSize;
			ITERATOR destinationEnd = destination + (*destinationSize) * (*destinationStride);
			while (destination != destinationEnd)
			{
				ArrayTransferer<TYPE, NDIMS - 1>::transfer(destination, source, destinationSize + 1, sourceSize + 1, destinationStride + 1, sourceStride + 1);
			}
				
		}
	};
	template <class TYPE>
	class ArrayTransferer<TYPE, 1>
	{
	public:
		template<class ITERATOR>
		static void transfer(ITERATOR destination, ITERATOR source, const size_t *destinationSize, const size_t *sourceSize, const size_t* destinationStride, const size_t* sourceStride)
		{
			size_t n = *destinationSize < * sourceSize ? *destinationSize : *sourceSize;
			ITERATOR destinationEnd = destination + *destinationSize;
			for (; destination != destinationEnd; ++destination, ++source)
				*source = *destination;
		}
	};

	template <class ITERATOR, size_t NDIMS>
	class ArrayIndexer
	{
	public:
		ArrayIndexer(ITERATOR memory, size_t *dimensions, size_t* strides)
			:m_begin(memory), m_dimensions(dimensions), m_strides(strides)
		{
		}
		ArrayIndexer<ITERATOR, NDIMS - 1> operator[](size_t index)
		{
			return ArrayIndexer<ITERATOR, NDIMS - 1>(m_begin + index * (*(m_strides + 1)), m_dimensions + 1, m_strides +1);
		}
		const ArrayIndexer<ITERATOR, NDIMS - 1> operator[](size_t index) const
		{
			return ArrayIndexer<ITERATOR, NDIMS - 1>(m_begin + index * (*(m_strides + 1)), m_dimensions + 1, m_strides + 1);
		}
	private:
		ITERATOR m_begin;
		size_t *m_dimensions;
		size_t* m_strides;
	};

	template <class ITERATOR>
	class ArrayIndexer< ITERATOR, 1>
	{
	public:
		ArrayIndexer(ITERATOR memory, size_t* dimensions, size_t *strides)
			:m_begin(memory), m_dimensions(dimensions), m_strides(strides)
		{
		}
		typename ITERATOR::reference operator[](size_t index)
		{
			return ITERATOR::reference(*(m_begin + index));
		}
		const typename ITERATOR::reference operator[](size_t index) const
		{
			return ITERATOR::reference(*(m_begin + index));
		}
	private:
		ITERATOR m_begin;
		size_t* m_dimensions;
		size_t* m_strides;
	};

	template < class TYPE, size_t NDIMS>
	class Array
	{
	public:
		Array()
		{
			for (size_t i = 0; i < NDIMS; ++i)
				m_dimensions[i] = 0;
			for (size_t i = 0; i < NDIMS; ++i)
				m_strides[i] = 0;
			m_memory.resize(0);
		}
		Array(const Array<size_t, 1> &dimensions)
		{
			for (size_t i = 0; i < NDIMS; ++i)
				m_strides[i] = 0;
			setSize(dimensions);
		}
		void setSize(const Array<size_t, 1>& dimensions);
		/*{
			//work out the needed strides for the new size
			bool stridesUnchanged = true;
			size_t neededStrides[NDIMS];
			for (size_t i = 0; i < NDIMS; ++i)
				neededStrides[i] = 1;
			for (size_t i = 0; i < NDIMS; ++i)
				for (size_t j = i+1; j < NDIMS; ++j)
					neededStrides[i] *= dimensions[j];
			//check if the strides have changed
			for (size_t i = 0; i < NDIMS; ++i)
				if (neededStrides[i] > m_strides[i])
					stridesUnchanged = false;

			if (stridesUnchanged)
			{
				for (size_t i = 0; i < NDIMS; ++i)
					m_dimensions[i] = dimensions[i];
				size_t size = dimensions[0];
				for (size_t i = 0; i < NDIMS-1; ++i)
					size *= m_strides[i];
				m_memory.resize(size);
			}
			else if (m_memory.size() == 0)
			{
				for (size_t i = 0; i < NDIMS; ++i)
					m_dimensions[i] = dimensions[i];
				for (size_t i = 0; i < NDIMS; ++i)
					m_strides[i] = neededStrides[i];
				size_t size = dimensions[0];
				for (size_t i = 0; i < NDIMS - 1; ++i)
					size *= m_strides[i];
				m_memory.resize(size);
			}
			else
			{
				size_t newStrides[NDIMS];
				for (size_t i = 0; i < NDIMS; ++i)
					newStrides[i] = neededStrides[i] > m_strides[i] ? neededStrides[i] : newStrides[i];
				std::vector<TYPE> newMemory(neededStrides[0] * dimensions[0]);
				ArrayTransferer<TYPE, NDIMS>::transfer(newMemory.begin(), m_memory.begin(), &dimensions[0], m_dimensions, newStrides, m_strides);
				std::swap(m_memory, newMemory);
			}
		}*/
		auto operator[](size_t index) -> decltype (ArrayIndexer<std::vector<TYPE>::iterator, NDIMS>(std::vector<TYPE>::iterator(), 0, 0)[0])
		{
			return ArrayIndexer<std::vector<TYPE>::iterator, NDIMS>(m_memory.begin(), m_dimensions, m_strides)[index];
		}
		auto operator[](size_t index) const -> decltype (ArrayIndexer<const std::vector<TYPE>::iterator, NDIMS>(std::vector<TYPE>::iterator(), nullptr, nullptr)[0])
		{
			return ArrayIndexer<const std::vector<TYPE>::iterator, NDIMS> (std::vector<TYPE>::iterator(), nullptr, nullptr)[index];
		}
	private:
		std::vector<TYPE> m_memory;
		size_t m_dimensions[NDIMS];
		size_t m_strides[NDIMS];
	};
	template < class TYPE>
	class Array<TYPE, 1>
	{
	public:
		Array()
		{
			m_memory.resize(0);
		}
		Array(const Array<size_t, 1>& dimensions)
		{
			setSize(dimensions);
		}
		Array(size_t size)
		{
			setSize(size);
		}
		void setSize(const Array<size_t, 1>& dimensions)
		{
			m_memory.resize(dimensions[0]);
		}
		void setSize(size_t size)
		{
			m_memory.resize(size);
		}
		auto operator[](size_t index) -> decltype (ArrayIndexer<typename std::vector<TYPE>::iterator, 1>(std::vector<TYPE>::iterator(), 0, 0)[0])
		{
			return m_memory[index];
		}
		const TYPE & operator[](size_t index) const
		{
			return m_memory[index];
		}
	private:
		std::vector<TYPE> m_memory;
	};
}

template < class TYPE, size_t NDIMS>
void sci::Array<TYPE, NDIMS>::setSize(const Array<size_t, 1>& dimensions)
{
	//work out the needed strides for the new size
	bool stridesUnchanged = true;
	size_t neededStrides[NDIMS];
	for (size_t i = 0; i < NDIMS; ++i)
		neededStrides[i] = 1;
	for (size_t i = 0; i < NDIMS; ++i)
		for (size_t j = i + 1; j < NDIMS; ++j)
			neededStrides[i] *= dimensions[j];
	//check if the strides have changed
	for (size_t i = 0; i < NDIMS; ++i)
		if (neededStrides[i] > m_strides[i])
			stridesUnchanged = false;

	if (stridesUnchanged)
	{
		for (size_t i = 0; i < NDIMS; ++i)
			m_dimensions[i] = dimensions[i];
		//first dimension may have grown so we must resize
		m_memory.resize(m_dimensions[0] * m_strides[0]);
	}
	else if (m_memory.size() == 0)
	{
		for (size_t i = 0; i < NDIMS; ++i)
			m_dimensions[i] = dimensions[i];
		for (size_t i = 0; i < NDIMS; ++i)
			m_strides[i] = neededStrides[i];
		m_memory.resize(m_dimensions[0]*m_strides[0]);
	}
	else
	{
		size_t newStrides[NDIMS];
		for (size_t i = 0; i < NDIMS; ++i)
			newStrides[i] = neededStrides[i] > m_strides[i] ? neededStrides[i] : newStrides[i];
		std::vector<TYPE> newMemory(neededStrides[0] * dimensions[0]);
		ArrayTransferer<TYPE, NDIMS>::transfer(newMemory.begin(), m_memory.begin(), &dimensions[0], m_dimensions, newStrides, m_strides);
		std::swap(m_memory, newMemory);
	}
}