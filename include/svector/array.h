#pragma once

#include<vector>
#include<assert.h>

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

#include<array>
#include<span>
#include<functional>
#include<ranges>
#include<memory>
#include<deque>

namespace sci
{
	template<class T, size_t NDIMS>
	class GridView;

















	template<class U>
	struct GridDataVectorType
	{
		using type = U;
	};
	template<>
	struct GridDataVectorType<bool>
	{
		using type = uint8_t;
	};

	template<class T, size_t NDIMS>
	class GridDataMembers
	{
	protected:
		constexpr std::array<size_t, NDIMS> getShape() const
		{
			std::array<size_t, NDIMS> result;
			result[0] = m_data.size() / m_strides[0];
			for (size_t i = 1; i < NDIMS-1; ++i)
				result[i] = m_strides[i - 1] / m_strides[i];
			result[NDIMS - 1] = m_strides[NDIMS - 2];
			return result;
		}
		constexpr void setStride(const std::array<size_t, NDIMS>& shape)
		{
			//calculate and save the strides
			for (size_t i = 0; i < m_strides.size(); ++i)
			{
				m_strides[i] = 1;
				for (size_t j = i + 1; j < shape.size(); ++j)
					m_strides[i] *= shape[j];
			}
		}
		constexpr size_t getTopStride() const
		{
			return m_strides[0];
		}
		constexpr size_t multiToVectorPosition(const std::array<size_t, NDIMS>& position) const
		{
			size_t pos = 0;
			for (size_t i = 0; i < NDIMS - 1; ++i)
				pos += position[i] * m_strides[i];
			return pos + position[NDIMS - 1];
		}
		constexpr const size_t* getPremultipliedStridePointer() const
		{
			return &m_strides[0];
		}

		std::vector<GridDataVectorType<T>> m_data;

	private:
		std::array<size_t, NDIMS - 1> m_strides;
	};

	template<class T>
	class GridDataMembers<T, 1>
	{
	protected:
		constexpr std::array<size_t, 1> getShape() const
		{
			std::array<size_t, 1> result{ m_data.size() };
			return result;
		}
		constexpr void setStride(const std::array<size_t, 1>&)
		{
		}
		constexpr size_t getTopStride() const
		{
			return 1;
		}
		constexpr size_t multiToVectorPosition(const std::array<size_t, 1>& position) const
		{
			return position[0];
		}
		constexpr const nullptr_t* getPremultipliedStridePointer() const
		{
			return nullptr;
		}

		std::vector<GridDataVectorType<T>> m_data;
	};

	template<class T>
	class GridDataMembers<T, 0>
	{
	protected:
		constexpr std::array<size_t, 0> getShape() const
		{
			std::array<size_t, 0> result;
			return result;
		}
		constexpr void setStride(const std::array<size_t, 0>&)
		{
		}
		constexpr size_t getTopStride() const
		{
			return 0;
		}
		constexpr size_t multiToVectorPosition(const std::array<size_t, 0>& position) const
		{
			return 0;
		}
		constexpr const nullptr_t* getPremultipliedStridePointer() const
		{
			return nullptr;
		}
		T m_data;
	};



	template<class T, size_t NDIMS, class Allocator = std::allocator<typename GridDataVectorType<T>::type>>
	class GridData :public GridDataMembers<T, NDIMS>
	{
	public:
		class ConstIterator
		{
		public:
			using iterator_category = std::contiguous_iterator_tag;
			using value_type = T;
			using difference_type = std::allocator_traits<Allocator>::difference_type;
			using pointer = const T*;
			using reference = const T&;
			using element_type = T;

			ConstIterator() = default;

			ConstIterator(pointer ptr)
				: m_ptr(ptr) 
			{}

			reference operator*() const
			{
				return *m_ptr;
			}
			pointer operator->() const
			{
				return m_ptr;
			}
			// Prefix increment
			ConstIterator& operator++()
			{
				m_ptr++;
				return *this;
			}
			// Postfix increment
			ConstIterator operator++(int)
			{
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}
			// Prefix decrement
			ConstIterator& operator--()
			{
				m_ptr--;
				return *this;
			}
			// Postfix decrement
			ConstIterator operator--(int)
			{
				Iterator tmp = *this;
				--(*this);
				return tmp;
			}
			ConstIterator& operator+=(const difference_type offset) noexcept
			{
				_Verify_offset(offset);
				m_ptr += offset;
				return *this;
			}

			ConstIterator operator+(const difference_type offset) const noexcept
			{
				ConstIterator tmp = *this;
				tmp += offset;
				return tmp;
			}

			friend ConstIterator operator+(const difference_type offset, const ConstIterator& iter) noexcept
			{
				return iter + offset;
			}

			ConstIterator& operator-=(const difference_type offset) noexcept
			{
				return *this += -offset;
			}

			ConstIterator operator-(const difference_type offset) const noexcept
			{
				ConstIterator tmp = *this;
				tmp -= offset; 
				return tmp;
			}

			difference_type operator-(const ConstIterator& right) const noexcept
			{
				compatible(right);
				return m_ptr - right.m_ptr;
			}

			friend ConstIterator operator-(const difference_type offset, const ConstIterator& iter) noexcept
			{
				return iter - offset;
			}

			reference operator[](const difference_type offset) const noexcept
			{
				return *(*this + offset);
			}
			bool compatible(const ConstIterator& right)
			{
				return true;
			}
			const GridData<T, NDIMS, Allocator>* getContainer() const noexcept
			{
			}
			void _Verify_offset(const difference_type offset) const noexcept
			{
#if _ITERATOR_DEBUG_LEVEL == 0
				(void)offset;
#else
				const auto container = getContainer();
				_STL_VERIFY(offset == 0 || m_ptr, "cannot seek value-initialized GridData iterator");
				if (offset < 0) {
					_STL_VERIFY(offset >= container->begin().m_ptr - m_ptr, "cannot seek GridData iterator before begin");
				}

				if (offset > 0) {
					_STL_VERIFY(offset < container->end().m_ptr - m_ptr, "cannot seek GridData iterator after end");
				}
#endif // _ITERATOR_DEBUG_LEVEL == 0
			}
			bool operator==(const ConstIterator& right) const noexcept
			{
				return (*this == right);
			}
			bool operator!=(const ConstIterator& right) const noexcept
			{
				return !(*this == right);
			}

			bool operator<(const ConstIterator& right) const noexcept
			{
				compatible(right);
				return m_ptr < right.m_ptr;
			}

			bool operator>(const ConstIterator& right) const noexcept
			{
				return right < *this;
			}

			bool operator<=(const ConstIterator& right) const noexcept
			{
				return !(right < *this);
			}

			bool operator>=(const ConstIterator& right) const noexcept
			{
				return !(*this < right);
			}
		protected:
			T* m_ptr;
		};
		class Iterator : public ConstIterator
		{
		public:
			using iterator_category = std::contiguous_iterator_tag;
			using value_type = T;
			using difference_type = std::allocator_traits<Allocator>::difference_type;
			using pointer = T*;
			using reference = T&;
			using element_type = T;

			Iterator() = default;

			Iterator(pointer ptr)
				: ConstIterator(ptr)
			{}

			reference operator*() const
			{
				return *ConstIterator::m_ptr;
			}
			pointer operator->()
			{
				return ConstIterator::m_ptr;
			}
			const pointer operator->() const
			{
				return ConstIterator::m_ptr;
			}
			// Prefix increment
			Iterator& operator++()
			{
				ConstIterator::m_ptr++;
				return *this;
			}
			// Postfix increment
			Iterator operator++(int)
			{
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}
			// Prefix decrement
			Iterator& operator--()
			{
				ConstIterator::m_ptr--;
				return *this;
			}
			// Postfix decrement
			Iterator operator--(int)
			{
				Iterator tmp = *this;
				--(*this);
				return tmp;
			}
			Iterator& operator+=(const difference_type offset) noexcept
			{
				ConstIterator::_Verify_offset(offset);
				ConstIterator::m_ptr += offset;
				return *this;
			}

			Iterator operator+(const difference_type offset) const noexcept
			{
				Iterator tmp = *this;
				tmp += offset;
				return tmp;
			}

			friend Iterator operator+(const difference_type offset, const Iterator &iter) noexcept
			{
				return iter + offset;
			}

			Iterator& operator-=(const difference_type offset) noexcept
			{
				return *this += -offset;
			}

			Iterator operator-(const difference_type offset) const noexcept
			{
				Iterator tmp = *this;
				tmp -= offset;
				return tmp;
			}

			friend Iterator operator-(const difference_type offset, const Iterator& iter) noexcept
			{
				return iter - offset;
			}

			difference_type operator-(const ConstIterator& right) const noexcept
			{
				compatible(right);
				return ConstIterator::m_ptr - right.m_ptr;
			}

			reference operator[](const difference_type offset) const noexcept
			{
				return *(*this + offset);
			}
			/*bool compatible(const ConstIterator& right)
			{
				return true;
			}*/
			/*const GridData<T, NDIMS, Allocator>* getContainer() const noexcept
			{
			}*/
			/*bool operator==(const ConstIterator& right) const noexcept
			{
				return (*this == right);
			}
			bool operator!=(const ConstIterator& right) const noexcept
			{
				return !(*this == right);
			}

			bool operator<(const ConstIterator& right) const noexcept
			{
				compatible(right);
				return m_ptr < right.m_ptr;
			}

			bool operator>(const ConstIterator& right) const noexcept
			{
				return right < *this;
			}

			bool operator<=(const ConstIterator& right) const noexcept
			{
				return !(right < *this);
			}

			bool operator>=(const ConstIterator& right) const noexcept
			{
				return !(*this < right);
			}*/
		};
		//using _Alty = _Rebind_alloc_t<_Alloc, _Ty>;
		//using allocator_traits = std::allocator_traits<typename std::allocator_traits<Allocator>::template rebind_alloc<T>>;
	public:
		using value_type = T;
		using allocator_type = Allocator;
		using pointer = typename std::allocator_traits<Allocator>::pointer;
		using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
		using reference = value_type &;
		using const_reference = const value_type &;
		using size_type = typename std::vector<T>::size_type;
		using difference_type = typename std::vector<T>::difference_type;
		using iterator = Iterator;
		using const_iterator = ConstIterator;
		using reverse_iterator = typename std::reverse_iterator<iterator>;
		using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;
		using data_type = typename GridDataVectorType<T>::type;
		using members = GridDataMembers<T, NDIMS>;

		constexpr GridData(const std::array<size_t, NDIMS>& sizes)
		{
			setSize(sizes);
		}
		constexpr GridData(const std::array<size_t, NDIMS>& sizes, const T&value)
		{
			setSize(sizes);
		}
		constexpr GridData()
		{
			std::array<size_t, NDIMS> shape;
			for (size_t &size : shape)
				size = 0;
			setSize(shape);
		}
		template<class U>
		requires(std::is_convertible_v < U(*)[], value_type(*)[] >)
		constexpr GridData(GridView<U, NDIMS> view)
		{
			std::array<size_t, NDIMS> shape = view.getShape();
			size_t size = 1;
			for (size_t i = 0; i < NDIMS; ++i)
				size *= shape[i];
			members::m_data.assign(view.begin(), view.begin() + size);
			setSize(shape);
		}
		template<class U>
		requires(std::is_convertible_v < U(*)[], value_type(*)[] >)
		constexpr GridData(const GridData<U, NDIMS> &other)
		{
			std::array<size_t, NDIMS> shape = other.getShape();
			size_t size = 1;
			for (size_t i = 0; i < NDIMS; ++i)
				size *= shape[i];
			members::m_data.assign(other.begin(), other.begin() + size);
			setSize(shape);
		}
		constexpr GridData(std::initializer_list<T> list)
		{
			static_assert(NDIMS == 1, "Cannot use an initializer list constructor on GridData with more than 1 dimension.");
			members::m_data = std::vector<T>(list);
			setSize({ members::m_data.size() });
		}
		iterator begin()
		{
			return iterator(members::m_data.begin()._Ptr);
		}
		iterator end()
		{
			return iterator(members::m_data.end()._Ptr);
		}
		const_iterator begin() const
		{
			return const_iterator(members::m_data.begin()._Ptr);
		}
		const_iterator end() const
		{
			return const_iterator(members::m_data.end()._Ptr);
		}
		/*T* begin()
		{
			return &m_data[0];
		}
		T* end()
		{
			return &m_data[0]+m_data.size();
		}
		T* const begin() const
		{
			return &m_data[0];
		}
		T* const end() const
		{
			return &m_data[0]+m_data.size();
		}*/
		std::span<T> getSpan()
		{
			return std::span<T>(*this);
		}
		std::span<T const> getSpan() const
		{
			return std::span<T const>(*this);
		}
		const size_t* getStridesPointer() const
		{
			return members::getStridesPointer();
		}
		std::array<size_t, NDIMS> getShape() const
		{
			return members::getShape();
		}
		size_t size() const
		{
			return members::m_data.size();
		}
		void resize(const std::array<size_t, NDIMS> &shape)
		{
			bool simpleExpand = true;
			for (size_t i = 1; i < NDIMS; ++i)
				simpleExpand &= shape[i] == members::m_sizes[i];
			if (simpleExpand)
				setSize(shape);
			else
				throw("I have not yet coded beyond simple multi-d resizes.");
		}
		void resize(const std::array<size_t, NDIMS>& shape, const T& value)
		{
			bool simpleExpand = true;
			for (size_t i = 1; i < NDIMS; ++i)
				simpleExpand &= shape[i] == members::m_sizes[i];
			if (simpleExpand)
			{
				size_t size = 1;
				for (size_t i = 0; i < NDIMS; ++i)
					size *= shape[i];
				members::m_data.resize(size, value); //resize the data first to avoid multiple assigns
				setSize(shape);
			}
			else
				throw("I have not yet coded beyond simple multi-d resizes.");
		}
		void insert(size_t index, GridView<T const, NDIMS> source)
		{
			std::array<size_t, NDIMS> sourceShape = source.getShape();
#ifdef _DEBUG
			for (size_t i = 1; i < NDIMS; ++i)
				assert(sourceShape[i] == members::m_sizes[i]);
#endif
			iterator pos = begin() + index * members::getTopStride();
			members::m_data.insert(pos, source.begin(), source.begin() + sourceShape[0] * members::getTopStride());
		}
		/*void insert(size_t index, GridView<T, NDIMS - 1> source)
		{
			//To Do
		}*/
		void insert(size_t index, GridView<T const, NDIMS - 1> source)
		{
			static_assert(NDIMS != 0, "Cannot insert into a zero dimensional GridData object.");

			//check sizes match
			assert(members::getTopStride() == source.size());
			if constexpr (NDIMS > 1)
			{
				for (size_t i = 0; i < NDIMS - 2; ++i)
					assert(source.getPremultipliedStridesPointer()[i] == members::getPremultipliedStridesPointer()[i + 1]);
			}

			//insert the data
			iterator pos = begin() + index * members::getTopStride();
			members::m_data.insert(pos, source.begin(), source.begin() + members::getTopStride());
		}
		void insert(size_t index, size_t count, GridView<T const, NDIMS - 1> source)
		{
			static_assert(NDIMS != 0, "Cannot insert into a zero dimensional GridData object.");

			//check sizes match
			assert(members::getTopStride() == source.size());
			if constexpr (NDIMS > 1)
			{
				for (size_t i = 0; i < NDIMS - 2; ++i)
					assert(source.getPremultipliedStridesPointer()[i] == members::getPremultipliedStridesPointer()[i + 1]);
			}

			iterator pos = begin() + index * members::getTopStride();
			//expand with default construction
			insert(index, count * members::getTopStride(), T());
			//perform repeated assigns to put the data in
			for(size_t i=0; i<count; ++i)
				members::m_data.insert(pos + (i * members::getTopStride()), source.begin(), source.begin() + members::getTopStride());
		}
		void insert(size_t index, const T &source)
		{
			static_assert(NDIMS != 0, "Cannot insert into a zero dimensional GridData object.");

			iterator pos = begin() + index * members::getTopStride();
			members::m_data.insert(pos, members::getTopStride(), source);
		}
		void insert(size_t index, size_t count, const T& source)
		{
			static_assert(NDIMS != 0, "Cannot insert into a zero dimensional GridData object.");

			iterator pos = begin() + index * members::getTopStride();
			members::m_data.insert(pos, members::getTopStride() * count, source);
		}
		template<class U>
		void push_back(GridView<U, NDIMS - 1> source)
		{
			static_assert(NDIMS != 0, "Cannot push_back into a zero dimensional GridData object.");

			insert(members::m_data.size()/members::getTopStride(), source);
		}
		template<class U>
		void push_back(GridData<U, NDIMS - 1> source)
		{
			static_assert(NDIMS != 0, "Cannot push_back into a zero dimensional GridData object.");

			insert(members::m_data.size() / members::getTopStride(), source);
		}
		void push_back(const T &source)
		{
			static_assert(NDIMS != 0, "Cannot push_back into a zero dimensional GridData object.");

			members::m_data.resize(members::m_data.size() + members::getTopStride(), source);
		}
		void reserve(size_t size)
		{
			if constexpr (NDIMS == 0)
				assert(size < 2);
			else
				members::m_data.reserve(size);
		}
		void reserve(const std::array<size_t, NDIMS>& shape)
		{
			size_t product = 1;
			for (auto iter = size.begin(); iter != size.end(); ++iter)
				product *= size;
			members::m_data.reserve(product);
		}
		GridView<T, NDIMS> getGridView()
		{
			return GridView<T, NDIMS>(*this);
		}
		GridView<T, NDIMS> getGridView() const
		{
			return GridView<T const, NDIMS>(*this);
		}
		GridView<T, NDIMS> getGridView(size_t offset, size_t elements)
		{
			return GridView<T, NDIMS>(std::span<T>(members::m_data.begin() + offset * members::getTopStride(), elements * members::getTopStride()), members::getPremultipliedStridePointer());
		}
		T& operator[](const std::array<size_t, NDIMS>& index)
		{
			return *(begin + members::multToVectorPosition());
		}
		const T& operator[](const std::array<size_t, NDIMS>& index) const
		{
			return *(begin + members::multToVectorPosition());
		}
		decltype(auto) operator[](size_t index)
		{
			//decltype(auto) allows it to return a reference vs just auto which will never return a reference
			if constexpr (NDIMS == 0)
			{
				assert(index == 0);
				T& result = members::m_data;
				return (result); //needs to have the brackets to return a reference
			}
			else if constexpr (NDIMS == 1)
			{
				T& result = *(begin() + index);
				return (result); //needs to have the brackets to return a reference
			}
			else
			{
				GridView<T, NDIMS - 1> result = getGridView()[index];
				return result;
			}
		}
		constexpr static size_t nDimensions()
		{
			return NDIMS;
		}
	private:
		void setShape(const std::array<size_t, NDIMS>& shape)
		{
			members::setStrides(shape);
			if constexpr (NDIMS > 1)
			{
				//calculate full size
				size_t size = shape[0]*members::getTopStride();
				//create the data block
				members::m_data.resize(size);
			}
		}
		void setShape(const std::array<size_t, NDIMS>& shape, const T& value)
		{
			members::setStrides(shape);
			if constexpr (NDIMS > 1)
			{
				//calculate full size
				size_t size = shape[0] * members::getTopStride();
				//create the data block
				members::m_data.resize(size, value);
			}
		}
	};

	/*template<class T, size_t NDIMS, class Allocator>
	bool GridData<T, NDIMS, Allocator>::ConstIterator::operator==(const GridData<T, NDIMS, Allocator>::ConstIterator& a, const GridData<T, NDIMS, Allocator>::ConstIterator& b)
	{
		return a.m_ptr == b.m_ptr;
	};
	template<class T, size_t NDIMS, class Allocator>
	bool GridData<T, NDIMS, Allocator>::ConstIterator::operator!= (const GridData<T, NDIMS, Allocator>::ConstIterator& a, const GridData<T, NDIMS, Allocator>::ConstIterator& b)
	{
		return a.m_ptr != b.m_ptr;
	};*/


	template<class T, size_t NDIMS>
	class GridView
	{
	public:
		using value_type = typename std::span<T>::value_type;
		using pointer = typename std::span<T>::pointer;
		using const_pointer = typename std::span<T>::const_pointer;
		using reference = typename std::span<T>::reference;
		using const_reference = typename std::span<T>::const_reference;
		using size_type = typename std::span<T>::size_type;
		using difference_type = typename std::span<T>::difference_type;
		using iterator = typename std::span<T>::iterator;
		using reverse_iterator = typename std::span<T>::reverse_iterator;

		template<class OtherT, class Allocator>
		requires(std::is_convertible_v < OtherT(*)[], value_type(*)[] >)
			GridView(GridData<OtherT, NDIMS, Allocator>& gridData)
			: m_strides(gridData.getStridesPointer())
		{
			m_span = gridData.getSpan();
		}
		template<class OtherT, class Allocator>
		requires(std::is_convertible_v < OtherT(*)[], value_type(*)[] >)
			GridView(const GridData<OtherT, NDIMS, Allocator>& gridData)
			: m_strides(gridData.getStridesPointer())
		{
			m_span = gridData.getSpan();
		}
		template<class OtherT>
		requires(std::is_convertible_v < OtherT(*)[], value_type(*)[] >)
			GridView(const GridView<OtherT, NDIMS>& other)
		{
			m_span = other.m_span;
			m_strides = other.m_strides;
		}
		template<class OtherT>
		requires(std::is_convertible_v < OtherT(*)[], value_type(*)[] >)
			GridView(const GridView<OtherT, NDIMS>&& other)
		{
			std::swap(m_span, other.m_span);
			std::swap(m_strides, other.m_strides);
		}
		GridView(std::span<T> span, const size_t* const premultipliedStrides)
			:m_span(span), m_strides(premultipliedStrides)
		{
		}
		template<class R>
		requires(std::is_convertible_v < R(*)[], value_type(*)[] >&&
			std::contiguous_iterator<R::iterator>)
		GridView(R&& range, size_t *premultipliedStrides)
		{
			m_span(range);
			m_strides = premultipliedStrides;
		}
		GridView<T, NDIMS - 1> operator[](size_t index)
		{
			return GridView<T, NDIMS - 1>(m_span.subspan(index * (*m_strides), *m_strides), m_strides + 1);
		}
		const GridView<T const, NDIMS - 1> operator[](size_t index) const
		{
			return GridView<T const, NDIMS - 1>(m_span.subspan(index * (*m_strides), *m_strides), m_strides + 1);
		}
		iterator begin()
		{
			return m_span.begin();
		}
		iterator end()
		{
			return m_span.end();
		}
		std::array<size_t, NDIMS> getShape() const
		{
			std::array<size_t, NDIMS> result;
			result[0] = m_span.size() / m_strides[0];
			for (size_t i = 1; i < NDIMS; ++i)
				result[i] = result[i - 1] / m_strides[i - 1];
		}
		size_t size() const
		{
			return m_span.size();
		}
		constexpr static size_t nDimensions()
		{
			return NDIMS;
		}
		const size_t* getPremultipliedStridesPointer() const
		{
			return m_strides;
		}
	private:
		std::span<T> m_span;
		const size_t *m_strides;
	};


	template<class T>
	class GridView<T, 1>
	{
	public:

		//using range_type = std::ranges::random_access_range<value_type>;
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using size_type = size_t;
		using difference_type = long;
		//using iterator = std::ranges::iterator_t( typename range_type::iterator;
		//using const_iterator = typename std::span<T>::const_iterator;
		//using reverse_iterator = typename range_type::reverse_iterator;
		//using const_reverse_iterator = std::span<T>::const_iterator;

		template<class OtherT, class Allocator>
		requires(std::is_convertible_v < OtherT(*)[], value_type(*)[] >)
		GridView(GridData<OtherT, 1, Allocator>& gridData)
		{
			m_span = gridData;
		}
		template<class OtherT, class Allocator>
		requires(std::is_convertible_v < OtherT(*)[], value_type(*)[] >)
		GridView(const GridData<OtherT, 1, Allocator>& gridData)
		{
			m_span = gridData;
		}
		template<class OtherT>
		requires(std::is_convertible_v < OtherT(*)[], value_type(*)[] >)
		GridView(const GridView<OtherT, 1> &other)
		{
			m_span = other.m_span;
		}
		template<class OtherT>
		requires(std::is_convertible_v < OtherT(*)[], value_type(*)[] >)
		GridView(const GridView<OtherT, 1>&& other)
		{
			std::swap(m_span, other.m_span);
		}
		GridView(std::span<T> span, const size_t* const strides) //for compatibility with multidimensional GridViews. the strides pointer will not be used in 1d
			:m_span(span)
		{
		}
		reference operator[](size_t index)
		{
			return m_span[index];
		}
		const_reference operator[](size_t index) const
		{
			return m_span[index];
		}
		auto begin()
		{
			return m_span.begin();
		}
		auto end()
		{
			return m_span.end();
		}
		std::array<size_t, 1> getShape() const
		{
			std::array<size_t, 1> result;
			result[0] = m_span.size();
		}
		size_t size() const
		{
			return m_span.size();
		}
		constexpr static size_t nDimensions()
		{
			return 1;
		}
		nullptr_t getPremultipliedStridesPointer() const
		{
			return nullptr;
		}
	private:
		std::span<T> m_span;
	};

	template<size_t NDIMS>
	class GridPremultipliedStridesPointer
	{
	public:
		GridPremultipliedStridesPointer(const size_t* ptrIn)
			:ptr(ptrIn)
		{}
		GridPremultipliedStridesPointer() = default;
		const size_t* ptr;
	};
	template<>
	class GridPremultipliedStridesPointer<1>
	{
	public:
		GridPremultipliedStridesPointer() = default;
	};
	template<>
	class GridPremultipliedStridesPointer<0>
	{
	public:
		GridPremultipliedStridesPointer() = default;
	};

	template<size_t NDIMS>
	class GridPremultipliedStridesReference
	{
	public:
		GridPremultipliedStridesReference() = default;
		GridPremultipliedStridesReference(const size_t* stridesPtr) requires (NDIMS > 1)
			:m_ptr(stridesPtr)
		{
		}
		const size_t stride() const requires (NDIMS > 1)
		{
			return *(m_ptr.ptr);
		}
		const size_t stride() const requires (NDIMS == 1)
		{
			return 1;
		}
		const size_t stride() const requires (NDIMS == 0)
		{
			return 0;
		}
		GridPremultipliedStridesReference<NDIMS - 1> next() const requires (NDIMS > 2)
		{
			return GridPremultipliedStridesReference<NDIMS - 1>(m_ptr.ptr + 1);
		}
		GridPremultipliedStridesReference<NDIMS - 1> next() const requires (NDIMS == 1 || NDIMS == 2)
		{
			return GridPremultipliedStridesReference<NDIMS - 1>();
		}
		std::array<size_t, NDIMS> getShape(size_t size) const
		{
			if constexpr (NDIMS == 0)
			{
				return std::array<size_t, 0>{};
			}
			else if constexpr (NDIMS == 1)
			{
			return std::array<size_t, 1>{size};
			}
			else
			{
				std::array<size_t, NDIMS> result;
				result[0] = size / m_ptr.ptr[0];
				for (size_t i = 1; i < NDIMS - 1; ++i)
					result[i] = m_ptr.ptr[i - 1] / m_ptr.ptr[i];
				result[NDIMS - 1] = m_ptr.ptr[NDIMS - 2];
				return result;
			}
		}
		size_t getOffset(const std::array<size_t, NDIMS>& index)
		{
			if constexpr (NDIMS == 0)
				return 0;
			else if constexpr (NDIMS == 1)
				return index;
			else
			{
				size_t result = 0;
				for (size_t i = 0; i < NDIMS - 1; ++i)
					result += index * m_ptr.ptr[i];
				return result + index[NDIMS - 1];
			}
		}
	private:
		GridPremultipliedStridesPointer<NDIMS> m_ptr;
	};


	template<size_t NDIMS, class RANGE>
	requires std::ranges::random_access_range<RANGE>
		class grid_view : public std::ranges::view_base
	{
	public:
		class Iterator : public std::ranges::iterator_t<RANGE>
		{
		public:
			using base_type = std::ranges::iterator_t<RANGE>;
			using iterator_category = std::iterator_traits<base_type>::iterator_category;
			using value_type = std::iterator_traits<base_type>::value_type;
			using difference_type = std::iterator_traits<base_type>::difference_type;
			using pointer = std::iterator_traits<base_type>::pointer;
			using reference = std::iterator_traits<base_type>::reference;
			using size_type = std::ranges::range_size_t<RANGE>;
			using difference_type = std::ranges::range_difference_t<RANGE>;
			//template<std::ranges::contiguous_range<RANGE>>
			using element_type = std::pointer_traits<pointer>::element_type;

			Iterator() = default;
			Iterator(base_type const& b)
				:base_type( b )
			{

			}


			reference operator*() const
			{
				return *static_cast<base_type>(*this);
			}
			pointer operator->()
			{
				return static_cast<pointer>(static_cast<base_type>(*this).operator->());
			}
			const pointer operator->() const
			{
				return static_cast<const pointer>(static_cast<const base_type>(*this).operator->());
			}
			// Prefix increment
			Iterator& operator++()
			{
				++static_cast<base_type&>(*this);
				return (*this);
			}
			// Postfix increment
			Iterator operator++(int)
			{
				return static_cast<base_type&>(*this)++;
			}
			// Prefix decrement
			Iterator& operator--()
			{
				--static_cast<base_type&>(*this);
				return (*this);
			}
			// Postfix decrement
			Iterator operator--(int)
			{
				return static_cast<base_type&>(*this)--;
			}
			Iterator& operator+=(const difference_type offset)
			{
				static_cast<base_type&>(*this)+=offset;
				return *this;
			}

			Iterator operator+(const difference_type offset) const noexcept
			{
				Iterator tmp = *this;
				tmp += offset;
				return tmp;
			}

			friend Iterator operator+(const difference_type offset, const Iterator& iter) noexcept
			{
				return iter + offset;
			}

			Iterator& operator-=(const difference_type offset) noexcept
			{
				return *this += -offset;
			}

			Iterator operator-(const difference_type offset) const noexcept
			{
				Iterator tmp = *this;
				tmp -= offset;
				return tmp;
			}

			friend Iterator operator-(const difference_type offset, const Iterator& iter) noexcept
			{
				return iter - offset;
			}

			difference_type operator-(const Iterator& right) const noexcept
			{
				return static_cast<const base_type&>(*this) - static_cast<const base_type&>(right);
			}

			reference operator[](const difference_type offset) const noexcept
			{
				return *(*this + offset);
			}
			bool operator==(const Iterator& right) const noexcept
			{
				return (static_cast<const base_type&>(*this) == static_cast<const base_type&>(right));
			}
			bool operator!=(const Iterator& right) const noexcept
			{
				return !(*this == right);
			}

			bool operator<(const Iterator& right) const noexcept
			{
				return static_cast<const base_type&>(*this) < static_cast<const base_type&>(right);
			}

			bool operator>(const Iterator& right) const noexcept
			{
				return right < *this;
			}

			bool operator<=(const Iterator& right) const noexcept
			{
				return !(right < *this);
			}

			bool operator>=(const Iterator& right) const noexcept
			{
				return !(*this < right);
			}
		};

		using iterator = Iterator;
		using const_iterator = iterator;
		using reference_type = iterator::reference;
		using size_type = iterator::size_type;
		using difference_type = iterator::difference_type;
		using sentinel = iterator;

		grid_view() = default;
		constexpr grid_view(grid_view const& rhs) = default;
		constexpr grid_view(grid_view&& rhs) = default;
		constexpr grid_view& operator=(grid_view const& rhs) = default;
		constexpr grid_view& operator=(grid_view&& rhs) = default;
		~grid_view() = default;

		grid_view(RANGE&& range, const GridPremultipliedStridesReference<NDIMS> &strides)
			: m_dataMembers{ new data_members_t{std::forward<RANGE>(range)} }, m_strides(strides)
		{
		}

		iterator begin() const
		{
			return std::begin(m_dataMembers->m_range);
		}
		iterator cbegin() const
		{
			return begin();
		}
		sentinel end() const
		{
			return std::end(m_dataMembers->m_range);
		}
		sentinel cend() const
		{
			return end();
		}
		auto size() const
		{
			return std::size(m_dataMembers->m_range);
		}
		reference_type operator[](const difference_type &index) requires(NDIMS==1)
		{
			return m_dataMembers->m_range[index];
		}
		const reference_type operator[](const difference_type& index) const requires(NDIMS == 1)
		{
			return m_dataMembers->m_range[index];
		}
		reference_type operator[](const std::array<size_t, NDIMS> &index)
		{
			return m_dataMembers->m_range[m_strides.getOffset(index)];
		}
		const reference_type operator[](const std::array<size_t, NDIMS>& index) const
		{
			return m_dataMembers->m_range[m_strides.getOffset(index)];
		}
		auto operator[](const difference_type& index) requires(NDIMS > 1)
		{
			auto subrange = std::ranges::subrange(m_dataMembers->m_range.begin() + index * m_strides.stride(),
				m_dataMembers->m_range.begin() + (index + 1) * m_strides.stride());
			static_assert((bool)std::ranges::random_access_range<decltype(subrange)>, "subrange failed the test for being a random access range");
			return grid_view<NDIMS - 1, decltype(subrange)>(std::forward<decltype(subrange)>(subrange), m_strides.next());
		}
		const auto operator[](const difference_type& index) const requires(NDIMS > 1)
		{
			auto subrange = std::ranges::subrange(m_dataMembers->m_range.begin() + index * m_strides.stride(),
				m_dataMembers->m_range.begin() + (index + 1) * m_strides.stride());
			static_assert((bool)std::ranges::random_access_range<decltype(subrange)>, "subrange failed the test for being a random access range");
			return grid_view<NDIMS - 1, decltype(subrange)>(std::forward<decltype(subrange)>(subrange), m_strides.next());
		}
		std::array<size_t, NDIMS> shape()
		{
			return m_strides.getShape(size());
		}

	private:
		struct data_members_t
		{
			RANGE m_range;
		};
		std::shared_ptr<data_members_t> m_dataMembers;
		const GridPremultipliedStridesReference<NDIMS> m_strides;
	};


	template <size_t NDIMS, class RANGE>
	requires std::ranges::random_access_range<RANGE>
		grid_view(RANGE&&, const GridPremultipliedStridesReference<NDIMS>&)->grid_view<NDIMS, RANGE>;

	template <size_t NDIMS, class RANGE>
	requires std::ranges::random_access_range<RANGE>
		grid_view(RANGE&&)->grid_view<1, RANGE>;

	

	static_assert((bool)std::ranges::random_access_range<grid_view<1, std::deque<double>>>, "sci::grid_view failed the test for being a random access range");
	static_assert((bool)std::ranges::range<grid_view<1, std::deque<double>>>, "sci::grid_view failed the test for being a range");
	static_assert((bool)std::random_access_iterator<std::ranges::iterator_t<grid_view<1, std::deque<double>>>>, "sci::grid_view failed the test for having a random access iterator");
	static_assert((bool)std::bidirectional_iterator<std::ranges::iterator_t<grid_view<1, std::deque<double>>>>, "sci::grid_view failed the test for having a bidirectional iterator");
	static_assert(std::input_iterator<std::ranges::iterator_t<grid_view<1, std::deque<double>>>>, "sci::grid_view failed the test for having a input iterator");
	static_assert(std::forward_iterator<std::ranges::iterator_t<grid_view<1, std::deque<double>>>>, "sci::grid_view failed the test for having a forward iterator");
	static_assert(std::output_iterator<std::ranges::iterator_t<grid_view<1, std::deque<double>>>, int>, "sci::grid_view failed the test for having a output iterator");
	static_assert(std::indirectly_readable<std::ranges::iterator_t<grid_view<1, std::deque<double>>>>, "sci::grid_view failed the test for having a indirectly readable iterator");
	static_assert((bool)!std::ranges::contiguous_range< grid_view<1, std::deque<double>>>, "grid_view<std::deque<>> should not be contiguous");

	static_assert((bool)std::ranges::contiguous_range<grid_view<1, std::vector<double>>>, "sci::grid_view<std::vector<>> failed the test for being a contiguous range");

	template<size_t NDIMS>
	struct grid_fn
	{
		grid_fn() = default;
		grid_fn(const GridPremultipliedStridesReference<NDIMS>& strides)
			:m_strides(strides)
		{}
		template <size_t NDIMS, class RANGE>
		requires std::ranges::random_access_range<RANGE>
		auto operator()(RANGE&& range, const GridPremultipliedStridesReference<NDIMS>& strides) const
		{
			return grid_view<NDIMS, RANGE>{ std::forward<RANGE>(range), m_strides };
		}
		template <size_t NDIMS>
			auto operator()(const GridPremultipliedStridesReference<NDIMS>& strides) const
		{
			//This operator() returns a grid_fn which uses strides. This in turn can get piped
			return grid_fn( strides );
		}
		template <typename RANGE>
		requires std::ranges::random_access_range<RANGE>
			friend auto operator|(RANGE&& range, grid_fn const& fn)
		{
			return grid_view<NDIMS, RANGE>{ std::forward<RANGE>(range), fn.m_strides };
		}
	private:
		const GridPremultipliedStridesReference<NDIMS> m_strides;
	};

	namespace views
	{
		template<size_t NDIMS>
		grid_fn<NDIMS> grid;
	}











	struct plus_assign
	{
		template <class T, class U>
		constexpr auto operator()(T&& a, U&& b) const
			noexcept(noexcept(static_cast<T&&>(a) += static_cast<U&&>(b))) // strengthened
			-> decltype(static_cast<T&&>(a) += static_cast<U&&>(b))
		{
			return static_cast<T&&>(a) += static_cast<U&&>(b);
		}
	};

	struct minus_assign
	{
		template <class T, class U>
		constexpr auto operator()(T&& a, U&& b) const
			noexcept(noexcept(static_cast<T&&>(a) -= static_cast<U&&>(b))) // strengthened
			-> decltype(static_cast<T&&>(a) -= static_cast<U&&>(b))
		{
			return static_cast<T&&>(a) -= static_cast<U&&>(b);
		}
	};

	struct multiplies_assign
	{
		template <class T, class U>
		constexpr auto operator()(T&& a, U&& b) const
			noexcept(noexcept(static_cast<T&&>(a) *= static_cast<U&&>(b))) // strengthened
			-> decltype(static_cast<T&&>(a) *= static_cast<U&&>(b))
		{
			return static_cast<T&&>(a) *= static_cast<U&&>(b);
		}
	};

	struct divides_assign
	{
		template <class T, class U>
		constexpr auto operator()(T&& a, U&& b) const
			noexcept(noexcept(static_cast<T&&>(a) /= static_cast<U&&>(b))) // strengthened
			-> decltype(static_cast<T&&>(a) /= static_cast<U&&>(b))
		{
			return static_cast<T&&>(a) /= static_cast<U&&>(b);
		}
	};


	template<class OP>
	struct op
	{
	};
	template<>
	struct op<std::plus<>>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t + u;
		}
	};
	template<>
	struct op<std::minus<>>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t - u;
		}
	};
	template<>
	struct op<std::divides<>>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t / u;
		}
	};
	template<>
	struct op<std::multiplies<>>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t * u;
		}
	};
	template<>
	struct op<plus_assign>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t += u;
		}
	};
	template<>
	struct op<minus_assign>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t -= u;
		}
	};
	template<>
	struct op<divides_assign>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t /= u;
		}
	};
	template<>
	struct op<multiplies_assign>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t *= u;
		}
	};
	template<>
	struct op<std::greater<>>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t > u;
		}
	};

	template<>
	struct op<std::less<>>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t < u;
		}
	};

	template<>
	struct op<std::greater_equal<>>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t >= u;
		}
	};

	template<>
	struct op<std::less_equal<>>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t <= u;
		}
	};

	template<>
	struct op<std::equal_to<>>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t == u;
		}
	};

	template<>
	struct op<std::not_equal_to<>>
	{
		template<class T, class U>
		auto static doOp(const T& t, const U& u)
		{
			return t != u;
		}
	};

	template<class T>
	struct isGrid : std::false_type {};

	template<class T, size_t NDIMS>
	struct isGrid< GridView<T, NDIMS>> : std::true_type {};

	template<class T, size_t NDIMS>
	struct isGrid< GridData<T, NDIMS>> : std::true_type {};

	template<class OP, class T, class U, size_t NDIMS>
	requires(!isGrid<U>())
	auto operate(GridView<T, NDIMS> a, const U& b)
	{
		using elementType = decltype(op<OP>::doOp(GridView<T, NDIMS>::value_type(), b));
		GridData<elementType, NDIMS> result(a.getShape());
		auto iterResult = result.begin();
		for (auto itera = a.begin(); itera != a.end(); ++itera, ++iterResult)
			*iterResult = op<OP>::doOp(*itera, b);
		return result;
	}
	template<class OP, class T, class U, size_t NDIMS>
	requires(!isGrid<T>())
	auto operate(const T& a, GridView<U const, NDIMS> b)
	{
		using elementType = decltype(op<OP>::doOp(a, GridView<U const, NDIMS>::value_type()));
		GridData<elementType, NDIMS> result(b.getShape());
		auto iterResult = result.begin();
		for (auto iterb = b.begin(); iterb != b.end(); ++iterb, ++iterResult)
			*iterResult = op<OP>(a, *iterb);
		return result;
	}

	template<class OP, class T, class U, size_t NDIMS1, size_t NDIMS2>
	auto operate(GridView<T, NDIMS1> a, GridView<U, NDIMS2> b)
	{
		static_assert(NDIMS1 == NDIMS2, "Cannot perform arithmetic operations on GridData with different dimensions.");
		using elementType = decltype(op<OP>::doOp(GridView<T, NDIMS1>::value_type(), GridView<U, NDIMS2>::value_type()));
		GridData<elementType, NDIMS1> result(a.getShape());
		auto iterb = b.begin();
		auto iterResult = result.begin();
		for (auto itera = a.begin(); itera != a.end(); ++itera, ++iterb, ++iterResult)
			*iterResult = op<OP>::doOp(*itera, *iterb);
		return result;
	}

	template<class OP, class T, class U, size_t NDIMS>
	requires(!isGrid<U>())
	GridView<T, NDIMS>& operateAssign(GridView<T, NDIMS> a, const U& b)
	{
		using elementType = decltype(op<OP>::doOp(GridView<T const, NDIMS>::value_type(), b));
		for (auto itera = a.begin(); itera != a.end(); ++itera)
			*itera = op<OP>::doOp(*itera, b);
		return a;
	}

	template<class OP, class T, class U, size_t NDIMS1, size_t NDIMS2>
	GridView<T, NDIMS1> & operateAssign(GridView<T, NDIMS1> a, GridView<U const, NDIMS2> b)
	{
		static_assert(NDIMS1 == NDIMS2, "Cannot perform arithmetic operations on GridData with different dimensions.");
		using elementType = decltype(op<OP>::doOp(GridView<T const, NDIMS1>::value_type(), GridView<U const, NDIMS2>::value_type()));
		auto iterb = b.begin();
		for (auto itera = a.begin(); itera != a.end(); ++itera, ++iterb)
			*itera = op<OP>::doOp(*itera, *iterb);
		return a;
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
	auto operator+(const T &a, const U &b)
	{
		return operate<std::plus<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
	auto operator+(const T &a, const U& b)
	{
		return operate<std::plus<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), b);
	}
	template<class T, class U>
	requires(bool(!isGrid<T>() && isGrid<U>()))
	auto operator+(const T& a, const U &b)
	{
		return operate<std::plus<>>(a, GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
	auto operator-(const T &a, const U &b)
	{
		return operate<std::minus<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
	auto operator-(const T &a, const U& b)
	{
		return operate<std::minus<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), b);
	}
	template<class T, class U>
	requires(bool(!isGrid<T>() && isGrid<U>()))
	auto operator-(const T& a, const U &b)
	{
		return operate<std::minus<>>(a, GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
	auto operator*(const T &a, const U &b)
	{
		return operate<std::multiplies<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
	auto operator*(const T &a, const U& b)
	{
		return operate<std::multiplies<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), b);
	}
	template<class T, class U>
	requires(bool(!isGrid<T>() && isGrid<U>()))
	auto operator*(const T& a, const U &b)
	{
		return operate<std::multiplies<>>(a, GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
	auto operator/(const T &a, const U &b)
	{
		return operate<std::divides<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), GridView< std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
	auto operator/(const T &a, const U& b)
	{
		return operate<std::divides<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), b);
	}
	template<class T, class U>
	requires(bool(!isGrid<T>() && isGrid<U>()))
	auto operator/(const T& a, const U &b)
	{
		return operate<std::divides<>>(a, GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}








	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
		auto operator>(const T& a, const U& b)
	{
		return operate<std::greater<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
		auto operator>(const T& a, const U& b)
	{
		return operate<std::greater<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), b);
	}
	template<class T, class U>
	requires(bool(!isGrid<T>() && isGrid<U>()))
		auto operator>(const T& a, const U& b)
	{
		return operate<std::greater<>>(a, GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
		auto operator<(const T& a, const U& b)
	{
		return operate<std::less<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
		auto operator<(const T& a, const U& b)
	{
		return operate<std::less<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), b);
	}
	template<class T, class U>
	requires(bool(!isGrid<T>() && isGrid<U>()))
		auto operator<(const T& a, const U& b)
	{
		return operate<std::less<>>(a, GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
		auto operator>=(const T& a, const U& b)
	{
		return operate<std::greater_equal<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
		auto operator>=(const T& a, const U& b)
	{
		return operate<std::greater_equal<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), b);
	}
	template<class T, class U>
	requires(bool(!isGrid<T>() && isGrid<U>()))
		auto operator>=(const T& a, const U& b)
	{
		return operate<std::greater_equal<>>(a, GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
		auto operator<=(const T& a, const U& b)
	{
		return operate<std::less_equal<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
		auto operator<=(const T& a, const U& b)
	{
		return operate<std::less_equal<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), b);
	}
	template<class T, class U>
	requires(bool(!isGrid<T>() && isGrid<U>()))
		auto operator<=(const T& a, const U& b)
	{
		return operate<std::less_equal<>>(a, GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
		auto operator==(const T& a, const U& b)
	{
		return operate<std::equal_to<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
		auto operator==(const T& a, const U& b)
	{
		return operate<std::equal_to<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), b);
	}
	template<class T, class U>
	requires(bool(!isGrid<T>() && isGrid<U>()))
		auto operator==(const T& a, const U& b)
	{
		return operate<std::equal_to<>>(a, GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
		auto operator!=(const T& a, const U& b)
	{
		return operate<std::not_equal_to<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
		auto operator!=(const T& a, const U& b)
	{
		return operate<std::not_equal_to<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), b);
	}
	template<class T, class U>
	requires(bool(!isGrid<T>() && isGrid<U>()))
		auto operator!=(const T& a, const U& b)
	{
		return operate<std::not_equal_to<>>(a, GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
		auto operator+=(T& a, const U& b)
	{
		return operateAssign<std::plus<>>(GridView<T::value_type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
		auto operator+=(T& a, const U& b)
	{
		return operateAssign<std::plus<>>(GridView<T::value_type, T::nDimensions()>(a), b);
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
		auto operator-=(T& a, const U& b)
	{
		return operateAssign<std::minus<>>(GridView<T::value_type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
		auto operator-=(T& a, const U& b)
	{
		return operateAssign<std::minus<>>(GridView<T::value_type, T::nDimensions()>(a), b);
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
		auto operator*=(T& a, const U& b)
	{
		return operateAssign<std::multiplies<>>(GridView<T::value_type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
		auto operator*=(T& a, const U& b)
	{
		return operateAssign<std::multiplies<>>(GridView<T::value_type, T::nDimensions()>(a), b);
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>()))
		auto operator/=(T& a, const U& b)
	{
		return operateAssign<std::divides<>>(GridView<T::value_type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	}
	template<class T, class U>
	requires(bool(isGrid<T>() && !isGrid<U>()))
		auto operator/=(T &a, const U& b)
	{
		return operateAssign<std::divides<>>(GridView<T::value_type, T::nDimensions()>(a), b);
	}
}