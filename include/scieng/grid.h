#ifndef SCI_GRIDDATA_H
#define SCI_GRIDDATA_H



#include"gridview.h"
#include<vector>
#include<functional>

namespace sci
{
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
	public:
		GridDataMembers(const std::vector<typename GridDataVectorType<T>::type>& data, std::array<size_t, NDIMS> shape)
			:m_strides(calculateStrides(shape)), m_data(data), m_view(m_data, GridPremultipliedStridesReference<NDIMS>(&m_strides[0]))
		{
		}
		GridDataMembers(const GridDataMembers<T, NDIMS>& other)
			:m_strides(other.m_strides), m_data(other.m_data), m_view(m_data, GridPremultipliedStridesReference<NDIMS>(&m_strides[0]))
		{
		}
		GridDataMembers(GridDataMembers<T, NDIMS>&& other)
			:m_strides(other.strides), m_data(std::move(other.m_data())), m_view(m_data, GridPremultipliedStridesReference<NDIMS>(&m_strides[0]))
		{
		}
		constexpr size_t multiToVectorPosition(const std::array<size_t, NDIMS>& position) const
		{
			size_t pos = 0;
			for (size_t i = 0; i < NDIMS - 1; ++i)
				pos += position[i] * m_strides[i];
			return pos + position[NDIMS - 1];
		}
		constexpr std::array<size_t, NDIMS> vectorPositionToMulti(size_t index) const
		{
			std::array<size_t, NDIMS> result;
			size_t remainder = index;
			for (size_t i = 0; i < result.size() - 1; ++i)
			{
				result[i] = remainder / m_strides[i];
				remainder = remainder % m_strides;
			}
			result[NDIMS - 1] = remainder;
			return result;
		}
		void incrementPosition(std::array<size_t, NDIMS>& position, size_t amount = 1) const
		{
			position[NDIMS - 1] += amount;
			size_t extra = position[NDIMS - 1] / m_strides[NDIMS - 2];
			position[NDIMS - 1] = position[NDIMS - 1] % m_strides[NDIMS - 2];
			for (size_t i = 0; i < NDIMS - 2; ++i)
			{
				if (extra == 0)
					break;
				size_t index = NDIMS - 2 - i;
				size_t length = m_strides[index - 1] / m_strides[index];
				position[index] += extra;
				extra = position[index] / length;
				position[index] = position[index] % length;
			}
		}
		GridDataMembers<T, NDIMS>& operator=(const GridDataMembers<T, NDIMS>& other)
		{
			m_data = other.m_data;
			m_strides = other.m_strides;
			refreshView();
			return *this;
		}
		GridDataMembers<T, NDIMS>& operator=(GridDataMembers<T, NDIMS>&& other)
		{
			std::swap(m_data, other.m_data);
			std::swap(m_strides, other.m_strides);
			refreshView();
			other.refreshView();
			return *this;
		}
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
		constexpr void setStrides(const std::array<size_t, NDIMS>& shape)
		{
			//calculate and save the strides
			m_strides = calculateStrides(shape);
		}
		constexpr size_t getTopStride() const
		{
			return m_strides[0];
		}
		constexpr const size_t* getPremultipliedStridePointer() const
		{
			return &m_strides[0];
		}
		const GridPremultipliedStridesReference<NDIMS> getStrides() const
		{
			return GridPremultipliedStridesReference<NDIMS>(&m_strides[0]);
		}
		constexpr void refreshView()
		{
			m_view.retarget(m_data | views::grid<NDIMS>(GridPremultipliedStridesReference<NDIMS>(&m_strides[0])));
		}
		void swap(GridDataMembers& other)
		{
			std::swap(m_data, other.m_data);
			std::swap(m_strides, other.m_strides);
			refreshView();
			other.refreshView();
		}

	private:
		std::array<size_t, NDIMS - 1> m_strides;
		static std::array<size_t, NDIMS - 1> calculateStrides(const std::array<size_t, NDIMS> &shape)
		{
			std::array<size_t, NDIMS - 1> strides;
			for (size_t i = 0; i < strides.size(); ++i)
			{
				strides[i] = 1;
				for (size_t j = i + 1; j < shape.size(); ++j)
					strides[i] *= shape[j];
			}
			return strides;
		}
	protected:
		std::vector<typename GridDataVectorType<T>::type> m_data;
		using view_type = std::remove_cv_t<decltype(m_data | views::grid<NDIMS>(GridPremultipliedStridesReference<NDIMS>(&m_strides[0])))>; //must be after m_data and m_strides
		view_type m_view;
	};

	template<class T>
	class GridDataMembers<T, 1>
	{
	public:
		GridDataMembers(const std::vector<typename GridDataVectorType<T>::type> &data)
			:m_data(data), m_view(m_data)
		{
		}
		GridDataMembers(const GridDataMembers<T, 1>& other)
			:m_data(other.m_data), m_view(m_data)
		{
		}
		GridDataMembers(GridDataMembers<T, 1>&& other)
			:m_data(std::move(other.m_data)), m_view(m_data)
		{
		}
		constexpr size_t multiToVectorPosition(const std::array<size_t, 1>& position) const
		{
			return position[0];
		}
		constexpr std::array<size_t, 1> vectorPositionToMulti(size_t index) const
		{
			return std::array<size_t, 1>(index);
		}
		void incrementPosition(std::array<size_t, 1>& position, size_t amount = 1) const
		{
			position[0] += amount;
		}
		GridDataMembers<T, 1>& operator=(const GridDataMembers<T, 1>& other)
		{
			m_data = other.m_data;
			refreshView();
			return *this;
		}
		GridDataMembers<T, 1>& operator=(GridDataMembers<T, 1>&& other)
		{
			std::swap(m_data, other.m_data);
			refreshView();
			other.refreshView();
			return *this;
		}
	protected:
		constexpr std::array<size_t, 1> getShape() const
		{
			std::array<size_t, 1> result{ m_data.size() };
			return result;
		}
		constexpr void setStrides(const std::array<size_t, 1>&)
		{
		}
		constexpr size_t getTopStride() const
		{
			return 1;
		}
		constexpr const nullptr_t* getPremultipliedStridePointer() const
		{
			return nullptr;
		}
		constexpr void refreshView()
		{
			m_view.retarget(m_data | views::grid<1>);
		}
		void swap(GridDataMembers& other)
		{
			std::swap(m_data, other.m_data);
			refreshView();
			other.refreshView();
		}

		std::vector<typename GridDataVectorType<T>::type> m_data;
		using view_type = std::remove_cvref_t<decltype(m_data | views::grid<1>)>;
		view_type m_view;
	};

	template<class T>
	class GridDataMembers<T, 0>
	{
	public:
		GridDataMembers()
		{
		}
		GridDataMembers(const T &value)
		{
			m_data = value;
		}
		GridDataMembers(const GridDataMembers<T, 0>& other)
		{
			m_data = other.m_data;
		}
		GridDataMembers(GridDataMembers<T, 0>&& other)
		{
			std::swap(m_data, other.m_data);
		}
		constexpr size_t multiToVectorPosition(const std::array<size_t, 0>& position) const
		{
			return 0;
		}
		constexpr std::array<size_t, 0> vectorPositionToMulti(size_t index) const
		{
			return std::array<size_t, 0>();
		}
		void incrementPosition(std::array<size_t, 0>& position, size_t amount = 1) const
		{
		}
		GridDataMembers<T, 0>& operator=(const GridDataMembers<T, 0>& other)
		{
			m_data = other.m_data;
			return *this;
		}
		GridDataMembers<T, 0>& operator=(GridDataMembers<T, 0>&& other)
		{
			std::swap(m_data, other.m_data);
			return *this;
		}
	protected:
		constexpr std::array<size_t, 0> getShape() const
		{
			std::array<size_t, 0> result;
			return result;
		}
		constexpr void setStrides(const std::array<size_t, 0>&)
		{
		}
		constexpr size_t getTopStride() const
		{
			return 0;
		}
		constexpr const nullptr_t* getPremultipliedStridePointer() const
		{
			return nullptr;
		}
		constexpr void refreshView()
		{
		}
		void swap(GridDataMembers& other)
		{
			std::swap(m_data, other.m_data);
		}
		T m_data;
		using view_type = decltype(m_data | views::grid<0>);
		//view_type m_view;
	};




	template<class T, size_t NDIMS, class Allocator = std::allocator<typename GridDataVectorType<T>::type>>
	class GridData :public GridDataMembers<T, NDIMS>
	{
	public:
		using members = GridDataMembers<T, NDIMS>;

		using value_type = T;
		using data_type = typename GridDataVectorType<value_type>::type; //same as value_type except data_type = uint8_t when value_type = bool.
		using vector_type = std::vector<data_type>;
		using allocator_type = Allocator;
		using pointer = typename std::allocator_traits<Allocator>::pointer;
		using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
		using reference = data_type&;
		using const_reference = const data_type&;
		using size_type = typename vector_type::size_type;
		using difference_type = typename vector_type::difference_type;
		using iterator = members::view_type::iterator;
		using const_iterator = members::view_type::const_iterator;
		using reverse_iterator = typename std::reverse_iterator<iterator>;
		using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;
		using view_type = typename GridDataMembers<value_type, NDIMS>::view_type;

		static const size_t ndims = NDIMS;

		constexpr GridData(const std::array<size_t, NDIMS>& shape) requires (NDIMS >1)
			:members(vector_type(), std::array<size_t, NDIMS>{})
		{
			setShape(shape);
		}
		constexpr GridData(const std::array<size_t, NDIMS>& shape) requires (NDIMS == 1)
			: members(vector_type())
		{
			setShape(shape);
		}
		constexpr GridData(const std::array<size_t, NDIMS>& shape) requires (NDIMS == 0)
			: members()
		{
			setShape(shape);
		}
		constexpr GridData(const std::array<size_t, NDIMS>& shape, const T& value) requires (NDIMS > 1)
			: members(vector_type(), std::array<size_t, NDIMS>{})
		{
			setShape(shape, value);
		}
		constexpr GridData(const std::array<size_t, NDIMS>& shape, const T& value) requires (NDIMS == 1)
			: members(vector_type())
		{
			setShape(shape, value);
		}
		constexpr GridData(const std::array<size_t, NDIMS>& shape, const T& value) requires (NDIMS == 0)
			: members(value)
		{
			setShape(shape, value);
		}
		constexpr GridData(size_t size) requires (NDIMS == 1)
			: members(vector_type())
		{
			setShape(std::array<size_t, 1>{size});
		}
		constexpr GridData(size_t size, const T& value) requires (NDIMS == 1)
			:members(vector_type())
		{
			setShape(std::array<size_t, 1>{size}, value);
		}
		constexpr GridData(const T& value) requires (NDIMS == 0)
			: members(value)
		{
			setShape(std::array<size_t, 0>{}, value);
		}

		constexpr GridData() requires (NDIMS > 1)
			: members(vector_type(), std::array<size_t, NDIMS>{}) //{} guarantees 0 initialization of the elements
		{
			std::array<size_t, NDIMS> shape;
			for (size_t& size : shape)
				size = 0;
			setShape(shape);
		}

		constexpr GridData() requires (NDIMS == 1)
			: members(vector_type()) //{} guarantees 0 initialization of the elements
		{
		}

		constexpr GridData() requires (NDIMS == 0) //{} guarantees 0 initialization of the elements
		{
		}

		
		//Construct from a grid_view or other non-GridData objects that begave like a grid.
		template<IsGridDims<NDIMS> U>
		constexpr GridData(const U& other) requires (NDIMS > 1)
			: members(vector_type(other.begin(), other.end()), other.shape())
		{
		}
		//Construct from a grid_view or other non-GridData objects that begave like a grid.
		template<IsGridDims<NDIMS> U>
		constexpr GridData(const U& other) requires (NDIMS == 1)
			: members(vector_type(other.begin(), other.end()))
		{
		}
		//Construct from a grid_view or other non-GridData objects that begave like a grid.
		template<IsGridDims<NDIMS> U>
		constexpr GridData(const U& other) requires (NDIMS == 0)
			: members(*other.begin())
		{
		}

		constexpr GridData(const GridData<T, NDIMS, Allocator>& other) = default;
		constexpr GridData(GridData<T, NDIMS, Allocator>&& other) = default;
		
		constexpr GridData(std::initializer_list<T> list) requires(NDIMS==1)
			:members(vector_type(list))
		{
			setShape({ members::m_data.size() });
		}
		template<std::input_iterator ITER>
		constexpr GridData(ITER begin, ITER end, const std::array<size_t, NDIMS>& shape) requires(NDIMS == 1)
			:members(*begin)
		{
				if( begin - end != 1)
					throw(std::out_of_range("Attempted to construct a GridData object with first and last iterators which did not match the shape."));
		}
		template<std::input_iterator ITER>
		constexpr GridData(ITER begin, ITER end) requires(NDIMS == 1)
			:members(vector_type(begin, end))
		{
			setShape({ members::m_data.size() });
		}

		template<std::input_iterator ITER>
		constexpr GridData(ITER begin, ITER end, const std::array<size_t, NDIMS>& shape)
			:members(vector_type(begin, end))
		{
			size_t size = 1;
			for (size_t i = 0; i < NDIMS; ++i)
				size *= shape[i];
			if (size != end - begin)
				throw(std::out_of_range("Attempted to construct a GridData object with first and last iterators which did not match the shape."));
			setShape(shape);
		}
		
		iterator begin()
		{
			return iterator(members::m_view.begin());
		}
		iterator end()
		{
			return iterator(members::m_view.end());
		}
		const_iterator begin() const
		{
			return const_iterator(members::m_view.begin());
		}
		const_iterator end() const
		{
			return const_iterator(members::m_view.end());
		}
		reference first()
		{
			return members::m_view.first();
		}
		const_reference first() const
		{
			return members::m_view.first();
		}
		reference last()
		{
			return members::m_view.last();
		}
		const_reference last() const
		{
			return members::m_view.last();
		}
		const size_t* getStridesPointer() const
		{
			return members::getPremultipliedStridePointer();
		}
		const GridPremultipliedStridesReference<NDIMS> getStrides() const
		{
			if constexpr (NDIMS > 1)
				return GridPremultipliedStridesReference<NDIMS>(members::getPremultipliedStridePointer());
			else
				return GridPremultipliedStridesReference<NDIMS>();
		}
		template<size_t dimension>
		constexpr size_t getStride() const requires (dimension < NDIMS)
		{
			return members::getPremultipliedStridePointer()[dimension];
		}
		constexpr size_t getStride(size_t dimension) const
		{
			return members::getPremultipliedStridePointer()[dimension];
		}
		std::array<size_t, NDIMS> shape() const
		{
			return members::m_view.shape();
		}
		size_t size() const
		{
			return members::m_data.size();
		}
		void reshape(const std::array<size_t, NDIMS>& shape, const T& value = T())
		{
			if constexpr (NDIMS == 0)
			{

			}
			else if constexpr (NDIMS == 1)
			{
				members::m_data.resize(shape[0], value); //resize the data first to avoid multiple assigns
				setShape(shape);
			}
			else
			{
				std::array<size_t, NDIMS> thisShape = this->shape();
				bool simpleExpand = true;
				bool previouslyZeroSized = false;
				for (size_t i = 1; i < NDIMS; ++i)
				{
					simpleExpand &= shape[i] == thisShape[i];
					previouslyZeroSized |= thisShape[i] == 0;
				}
				if (simpleExpand || previouslyZeroSized)
				{
					size_t size = 1;
					for (size_t i = 0; i < NDIMS; ++i)
						size *= shape[i];
					members::m_data.resize(size, value); //resize the data first to avoid multiple assigns
					setShape(shape);
				}
				else
				{
					std::array<size_t, NDIMS> commonShape;
					for (size_t i = 0; i < NDIMS; ++i)
						commonShape[i] = std::min(thisShape[i], shape[i]);

					GridData<T, NDIMS> other(shape, value);
					std::array<size_t, NDIMS> pos;
					for (auto& p : pos)
						p = 0;
					size_t chunkSize = commonShape.back();
					size_t incrementIndex = NDIMS - 2;
					while (thisShape[incrementIndex + 1] == shape[incrementIndex + 1])
					{
						chunkSize *= commonShape[incrementIndex];
						--incrementIndex;
					}
					while (pos[0] < commonShape[0])
					{
						auto source = &((*this)[pos]);
						auto dest = &(other[pos]);
						for (size_t i = 0; i < chunkSize; ++i)
							*(dest + i) = *(source + i);
						++pos[incrementIndex];
						for (size_t i = 0; i < incrementIndex; ++i)
						{
							size_t index = incrementIndex - i;
							if (pos[index] == commonShape[index])
							{
								pos[index] = 0;
								++pos[index - 1];
							}
							else
								break;
						}
					}
					swap(other);
				}
			}
		}
		void resize(size_t size) requires (NDIMS == 1)
		{
			reshape(std::array<size_t, 1>{ size });
		}
		void resize(size_t size, const T & value) requires (NDIMS == 1)
		{
			reshape(std::array<size_t, 1>{ size }, value);
		}
		constexpr iterator erase(const_iterator pos) requires (NDIMS == 1)
		{
			iterator result = members::m_data.erase(pos);
			setShape(std::array<size_t, NDIMS> {members::m_data.size()});
			return result;
		}
		constexpr iterator erase(const_iterator first, const_iterator last) requires (NDIMS == 1)
		{
			iterator result = members::m_data.erase(first, last);
			setShape(std::array<size_t, NDIMS> {members::m_data.size()});
			return result;
		}
		void swap(GridData<T, NDIMS> &other)
		{
			members::swap(other);
		}
		template<IsGrid GRID>
		void insert(size_t index, const GRID &source)
		{
			static_assert(GRID::ndims == ndims || GRID::ndims == ndims - 1, "Can only insert a grid with the same number of dimensions or one dimension less than the destination grid.");
			static_assert(NDIMS != 0, "Cannot insert into a zero dimensional GridData object.");
			if constexpr (GRID::ndims == ndims)
			{

				const std::array<size_t, GRID::ndims> sourceShape = source.shape();
				std::array<size_t, GRID::ndims> thisShape = shape();

				if (size() > 0)
				{
					for (size_t i = 1; i < GRID::ndims; ++i)
						assert(sourceShape[i] == thisShape[i]);
				}

				iterator pos = begin() + index * members::getTopStride();
				members::m_data.insert(pos, source.begin(), source.begin() + sourceShape[0] * members::getTopStride());
				thisShape[0] += sourceShape[0];
				setShape(thisShape);
			}
			else
			{
				//this is inserting a grid that is one dimension smaller than this grid.
				// It should have a size equal to the top stride
				//check sizes match
				std::array<size_t, ndims> thisShape = shape();
				const std::array<size_t, GRID::ndims> sourceShape = source.shape();
				
				if (size() > 0) //need to check for this as in this case all shape elements will be zero
				{
					for (size_t i = 0; i < sourceShape.size(); ++i)
					{
						assert(sourceShape[i] == thisShape[i + 1]);
						if (sourceShape[i] != thisShape[i + 1])
							throw(std::out_of_range("Attempted to insert data into sci::GridData with an incompatible shape. Inserted shape must be the same as destination shape with the first dimension omitted"));
					}
				}
				size_t offset = index * members::getTopStride();
				iterator pos = begin() + index * members::getTopStride();
				members::m_data.insert(pos, source.begin(), source.begin() + source.size());
				++thisShape[0];
				setShape(thisShape);

				//now add this to begin to get the insert point, add 1 to the first element of shape and setshape
				/*
				//insert the data
				if (size() == 0 && index == 0)
				{
					thisShape[0] = 1;
					for (size_t i = 0; i < sourceShape.size(); ++i)
						thisShape[i + 1] = sourceShape[i];
					members::m_data.assign(source.begin(), source.begin() + source.size());
				}
				else
				{
					iterator pos = begin() + index * members::getTopStride();
					members::m_data.insert(pos, source.begin(), source.begin() + source.size());
					++thisShape[0];
				}
				setShape(thisShape);*/
			}
		}
		//insert an element at index with value source. If NDIMS > 1 then
		//all values through the lower dimensions of the inserted element
		//will have value given by source.
		void insert(size_t index, const T& source)
		{
			static_assert(NDIMS != 0, "Cannot insert into a zero dimensional GridData object.");

			assert(size() > 0 || (ndims == 1 ));
			assert(index <= shape()[0]);

			iterator pos = begin() + index * members::getTopStride();
			std::array<size_t, ndims> thisShape = shape();
			members::m_data.insert(pos, members::getTopStride(), source);
			++thisShape[0];
			setShape(thisShape);
		}
		//insert multiple elements at index with value source. If NDIMS > 1 then
		//all values through the lower dimensions of the inserted elements
		//will have value given by source.
		void insert(size_t index, size_t count, const T& source)
		{
			static_assert(NDIMS != 0, "Cannot insert into a zero dimensional GridData object.");

			assert(size() > 0 || (ndims == 1));
			assert(index <= shape()[0]);

			iterator pos = begin() + index * members::getTopStride();
			std::array<size_t, ndims> thisShape = shape();
			members::m_data.insert(pos, members::getTopStride() * count, source);
			thisShape[0]+=count;
			setShape(thisShape);
		}
		template<IsGridDims<NDIMS-1> GRID> requires(NDIMS > 1)
		void push_back(const GRID &source)
		{
			static_assert(NDIMS != 0, "Cannot push_back into a zero dimensional GridData object.");
			insert(shape()[0], source);
		}
		void push_back(const T& source) requires(NDIMS == 1)
		{
			static_assert(NDIMS != 0, "Cannot push_back into a zero dimensional GridData object.");
			insert(shape()[0], source);
		}
		constexpr void assign(size_t count, const T& value) requires(NDIMS == 1)
		{
			members::m_data.assign(count, value);
		}
		template<IsGridDims<NDIMS - 1> GRID>
		constexpr void assign(size_t count, const GRID& value)
		{
			std::array<size_t, NDIMS> shape;
			shape[0] = count;
			for (size_t i = 0; i < NDIMS - 1; ++i)
				shape[i + 1] = value.shape()[i];
			reshape(shape);
			for (size_t i = 0; i < count; ++i)
				(*this)[i].assign(value);
		}
		template<class ITER>
		constexpr void assign(ITER first, ITER last) requires(NDIMS == 1)
		{
			members::m_data.assign(first, last);
		}
		constexpr void assign(std::initializer_list<T> list) requires(NDIMS == 1)
		{
			members::m_data.assign(list);
		}
		void reserve(size_t size)
		{
			if constexpr (NDIMS == 0)
				assert(size < 2);
			else
				members::m_data.reserve(size);
			members::refreshView();
		}
		void reserve(const std::array<size_t, NDIMS>& shape)
		{
			size_t product = 1;
			for (auto s : shape)
				product *= s;
			members::m_data.reserve(product);
			members::refreshView();
		}
		auto getView()
		{
			if constexpr (ndims == 0)
				return members::m_data | views::grid<0>;
			else
				return members::m_view;
		}
		auto getView() const
		{
			if constexpr (ndims == 0)
				return members::view_type(*this);
			else
				return members::m_view;
		}
		GridData<T, NDIMS, Allocator> subGrid(size_t firstIndexBegin, size_t nSlices) const
		{
			static_assert(NDIMS > 0, "Cannot get a subgrid of a zero diminsional GridData.");
			std::array<size_t, NDIMS> subgridShape = shape();
			subgridShape[0] = nSlices;
			if constexpr (NDIMS > 1)
			{
				return GridData<T, NDIMS, Allocator>(members::m_data.begin() + firstIndexBegin * members::getTopStride(), members::m_data.begin() + (firstIndexBegin + nSlices) * members::getTopStride(), subgridShape);
			}
			else
				return GridData<T, NDIMS, Allocator>(members::m_data.begin() + firstIndexBegin, members::m_data.begin() + (firstIndexBegin + nSlices), subgridShape);
		}
		reference operator[](const std::array<size_t, NDIMS>& index)
		{
			if constexpr (NDIMS == 0)
				return members::m_data;
			else
				return members::m_view[index];
		}
		const_reference operator[](const std::array<size_t, NDIMS>& index) const
		{
			if constexpr (NDIMS == 0)
				return members::m_data;
			else
				return members::m_view[index];
		}
		reference operator[](size_t index) requires (NDIMS == 1)
		{
			return (members::m_view[index]);
		}
		const_reference operator[](size_t index) const requires (NDIMS == 1)
		{
			return (members::m_view[index]);
		}
		auto operator[](size_t index) requires (NDIMS > 1)
		{
			return (members::m_view[index]);
		}
		auto operator[](size_t index) const requires (NDIMS > 1)
		{
			return (members::m_view[index]);
		}
		reference at(const std::array<size_t, NDIMS>& index)
		{
			return members::m_view[index];
		}
		const_reference at(const std::array<size_t, NDIMS>& index) const
		{
			return members::m_view.at(index);
		}
		reference at(size_t index) requires (NDIMS == 1)
		{
			return (members::m_view.at(index));
		}
		const_reference at(size_t index) const requires (NDIMS == 1)
		{
			return (members::m_view.at(index));
		}
		auto at(size_t index) requires (NDIMS > 1)
		{
			return (members::m_view.at(index));
		}
		auto at(size_t index) const requires (NDIMS > 1)
		{
			return (members::m_view.at(index));
		}
		constexpr static size_t nDimensions()
		{
			return NDIMS;
		}
		reference front()
		{
			if constexpr (NDIMS > 0)
				return members::m_data.front();
			else
				return members::m_data;
		}
		const_reference front() const
		{
			if constexpr (NDIMS > 0)
				return members::m_data.front();
			else
				return members::m_data;
		}
		reference back()
		{
			if constexpr (NDIMS > 0)
				return members::m_data.back();
			else
				return members::m_data;
		}
		const_reference back() const
		{
			if constexpr (NDIMS > 0)
				return members::m_data.back();
			else
				return members::m_data;
		}
		pointer data()
		{
			if constexpr (NDIMS > 0)
				return members::m_data.data();
			else
				return &members::m_data;
		}
		const_pointer data() const
		{
			if constexpr (NDIMS > 0)
				return members::m_data.data();
			else
				return &members::m_data;
		}
		void clear()
		{
			std::array<size_t, NDIMS> emptyShape;
			emptyShape.fill(0);
			setShape(emptyShape);
		}
		size_t capacity() const
		{
			return members::m_data.capacity();
		}
		void shrink_to_fit()
		{
			members::m_data.shrink_to_fit();
			members::refreshView();
		}
		template<IsGridDims<NDIMS> GRID>
		GridData<value_type, NDIMS, Allocator>& operator=(const GRID& rhs)
			requires std::convertible_to<typename GRID::data_type, data_type>
		{
			reshape(rhs.shape());
			auto iter = begin();
			auto rhsIter = rhs.begin();
			for (; iter != end(); ++iter, ++rhsIter)
				*iter = data_type(*rhsIter);
			return *this;
		}
		//fill the grid with a specific value
		template<class U>
		GridData<value_type, NDIMS, Allocator>& operator=(const U& rhs)
			requires std::convertible_to<U, data_type>
		{
			auto iter = begin();
			for (; iter != end(); ++iter)
				*iter = data_type(rhs);
			return *this;
		}
		GridData<T, NDIMS, Allocator>& operator=(const GridData<T, NDIMS, Allocator>& rhs) = default;
		GridData<T, NDIMS, Allocator>& operator=(GridData<T, NDIMS, Allocator>&& rhs) = default;
	private:
		void setShape(const std::array<size_t, NDIMS>& shape)
		{
			try
			{
				members::setStrides(shape);
				if constexpr (NDIMS > 1)
				{
					//calculate full size
					size_t size = shape[0] * members::getTopStride();
					//create the data block
					members::m_data.resize(size);
				}
				else if constexpr (NDIMS == 1)
				{
					members::m_data.resize(shape[0]);
				}
			}
			catch (...)
			{
				members::refreshView();
			}
			members::refreshView();
		}
		void setShape(const std::array<size_t, NDIMS>& shape, const T& value)
		{
			try
			{
				members::setStrides(shape);
				if constexpr (NDIMS > 1)
				{
					//calculate full size
					size_t size = shape[0] * members::getTopStride();
					//create the data block
					members::m_data.resize(size, value);
				}
				else if constexpr (NDIMS == 1)
				{
					members::m_data.resize(shape[0], value);
				}
				else
				{
					members::m_data = value;
				}
			}
			catch (...)
			{
				members::refreshView();
			}
			members::refreshView();
		}

	};



	template <class T>
	concept IsGridData =
		requires(std::remove_cvref_t < T > t)
	{
		{ GridData<typename T::value_type, T::ndims>() }->std::same_as<std::remove_cvref_t < T >>;
	};


	/*struct plus_assign
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

	//template<class T, class U>
	//requires(bool(isGrid<T>() && isGrid<U>()))
	//auto operator+(const T &a, const U &b)
	//{
	//	return operate<std::plus<>>(GridView<std::add_const<T::value_type>::type, T::nDimensions()>(a), GridView<std::add_const<U::value_type>::type, U::nDimensions()>(b));
	//}
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
	}*/
}

#endif