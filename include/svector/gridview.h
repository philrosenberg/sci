#ifndef SCI_GRIDVIEW_H
#define SCI_GRIDVIEW_H

#include<ranges>
#include<array>
#include<memory>
#include<assert.h>
#include<stdexcept>
#include<type_traits>

namespace sci
{


	//concept to check if somethng is a grid at compile time
	//to be a grid a class must have:
	//   a shape method
	//   a public static ndims variable
	//   operator[]
	//   be a random access range
	template<class T>
	concept IsGrid =
		requires(std::remove_cvref_t < T > t)
	{
		t.shape();
		t.ndims;
		t.operator[](std::array<size_t, T::ndims>());
		t.getView();
		typename T::value_type;
	};
	template<class T, size_t NDIMS>
	concept IsGridDims = IsGrid<T> &&
		requires(std::remove_cvref_t < T > t)
	{
		t.ndims == NDIMS;
	};
	/*template<class T, size_t NDIMS, class VALUE_TYPE>
	concept IsGridDimsVt = IsGridDims<T, NDIMS> &&
		std::is_convertible_v< std::remove_cvref_t <T::value_type>, VALUE_TYPE>;

	template<class T, class VALUE_TYPE>
	concept IsGridVt = IsGrid<T> &&
		std::is_convertible_v< std::remove_cvref_t <T::value_type>, VALUE_TYPE>;*/

	template <class T>
	concept has_data =
		requires (T & t)
	{
		{ t.data() };
	};

	template<size_t NDIMS>
	class GridPremultipliedStridesPointer
	{
	public:
		constexpr GridPremultipliedStridesPointer(const size_t* ptrIn)
			:ptr(ptrIn)
		{}
		constexpr GridPremultipliedStridesPointer() = default;
		constexpr GridPremultipliedStridesPointer(typename GridPremultipliedStridesPointer<NDIMS> const& rhs) = default;
		constexpr GridPremultipliedStridesPointer(typename GridPremultipliedStridesPointer<NDIMS>&& rhs) = default;
		GridPremultipliedStridesPointer& operator=(typename GridPremultipliedStridesPointer<NDIMS> const& rhs) = default;
		GridPremultipliedStridesPointer& operator=(typename GridPremultipliedStridesPointer<NDIMS>&& rhs) = default;
		~GridPremultipliedStridesPointer() = default;
		const size_t* ptr;
	};
	template<>
	class GridPremultipliedStridesPointer<1>
	{
	public:
		constexpr GridPremultipliedStridesPointer() = default;
		constexpr GridPremultipliedStridesPointer(typename GridPremultipliedStridesPointer<1> const& rhs) = default;
		constexpr GridPremultipliedStridesPointer(typename GridPremultipliedStridesPointer<1>&& rhs) = default;
		constexpr GridPremultipliedStridesPointer& operator=(typename GridPremultipliedStridesPointer<1> const& rhs) = default;
		constexpr GridPremultipliedStridesPointer& operator=(typename GridPremultipliedStridesPointer<1>&& rhs) = default;
		~GridPremultipliedStridesPointer() = default;
	};
	template<>
	class GridPremultipliedStridesPointer<0>
	{
	public:
		constexpr GridPremultipliedStridesPointer() = default;
		constexpr GridPremultipliedStridesPointer(typename GridPremultipliedStridesPointer<0> const& rhs) = default;
		constexpr GridPremultipliedStridesPointer(typename GridPremultipliedStridesPointer<0>&& rhs) = default;
		constexpr GridPremultipliedStridesPointer& operator=(typename GridPremultipliedStridesPointer<0> const& rhs) = default;
		constexpr GridPremultipliedStridesPointer& operator=(typename GridPremultipliedStridesPointer<0>&& rhs) = default;
		~GridPremultipliedStridesPointer() = default;
	};

	template<size_t NDIMS>
	class GridPremultipliedStridesReference
	{
	public:
		constexpr GridPremultipliedStridesReference() = default;
		constexpr GridPremultipliedStridesReference(GridPremultipliedStridesReference<NDIMS> const& rhs) = default;
		constexpr GridPremultipliedStridesReference(GridPremultipliedStridesReference<NDIMS>&& rhs) = default;
		constexpr GridPremultipliedStridesReference& operator=(GridPremultipliedStridesReference<NDIMS> const& rhs) = default;
		constexpr GridPremultipliedStridesReference& operator=(GridPremultipliedStridesReference<NDIMS>&& rhs) = default;
		~GridPremultipliedStridesReference() = default;
		constexpr GridPremultipliedStridesReference(const size_t* stridesPtr) requires (NDIMS > 1)
			: m_ptr(stridesPtr)
		{
		}
		constexpr const size_t stride() const requires (NDIMS > 1)
		{
			return *(m_ptr.ptr);
		}
		constexpr const size_t stride() const requires (NDIMS == 1)
		{
			return 1;
		}
		constexpr const size_t stride() const requires (NDIMS == 0)
		{
			return 0;
		}
		constexpr GridPremultipliedStridesReference<NDIMS - 1> next() const requires (NDIMS > 2)
		{
			return GridPremultipliedStridesReference<NDIMS - 1>(m_ptr.ptr + 1);
		}
		constexpr GridPremultipliedStridesReference<NDIMS - 1> next() const requires (NDIMS == 1 || NDIMS == 2)
		{
			return GridPremultipliedStridesReference<NDIMS - 1>();
		}
		constexpr std::array<size_t, NDIMS> getShape(size_t size) const
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
				result.fill(0);
				if (m_ptr.ptr[0] == 0)
					return result;
				result[0] = size / m_ptr.ptr[0];
				for (size_t i = 1; i < NDIMS - 1; ++i)
				{
					if (m_ptr.ptr[i] == 0)
						return result;
					result[i] = m_ptr.ptr[i - 1] / m_ptr.ptr[i];
				}
				result[NDIMS - 1] = m_ptr.ptr[NDIMS - 2];
				return result;
			}
		}
		constexpr size_t getOffset(const std::array<size_t, NDIMS>& index) const
		{
			if constexpr (NDIMS == 0)
				return 0;
			else if constexpr (NDIMS == 1)
				return index;
			else
			{
				size_t result = 0;
				for (size_t i = 0; i < NDIMS - 1; ++i)
					result += index[i] * m_ptr.ptr[i];
				return result + index[NDIMS - 1];
			}
		}

		static constexpr std::array<size_t, NDIMS - 1> premultipliedStridesFromSize(const std::array<size_t, NDIMS>& size)
		{
			std::array<size_t, NDIMS - 1> result;
			result[NDIMS - 2] = size[NDIMS-1];
			for (size_t i = 1; i < NDIMS-1; ++i)
				result[NDIMS - 2 - i] = result[NDIMS - 1 - i] * size[NDIMS - 1 - i];
			return result;
		}
	private:
		GridPremultipliedStridesPointer<NDIMS> m_ptr;
	};

	template<class T, size_t NDIMS, class Allocator>
	class GridData;


	//begin, end, size, ranges::iterator, ranges::range_size and ranges::range_difference
	//all emulate the std versions, but for non ranges, they treat the passed type/value
	//like it is a range with a size of 1
	//This allows grid_view to treat scalars like a zero dimensional array



	/*template<class T>
	constexpr auto begin(T& t) requires std::ranges::range<T>
	{
		return std::begin(t);
	}
	template<class T>
	constexpr auto begin(T& t) requires (!std::ranges::range<T>)
	{
		return &t;
	}
	template<class T>
	constexpr auto end(T& t) requires std::ranges::range<T>
	{
		return std::end(t);
	}
	template<class T>
	constexpr auto end(T& t) requires (!std::ranges::range<T>)
	{
		return (&t)+1;
	}
	template<class T>
	constexpr auto size(T& t) requires std::ranges::range<T>
	{
		return std::size(t);
	}
	template<class T>
	constexpr auto size(T& t) requires (!std::ranges::range<T>)
	{
		return 1;
	}*/
	namespace ranges
	{
		template<class T, bool IS_RANGE>
		struct iterator
		{
		};
		template<class T>
		struct iterator<T, true>
		{
			using type = std::ranges::iterator_t<T>;
		};
		template<class T>
		struct iterator<T, false>
		{
			using type = std::remove_reference<T>::type*;
		};
		template<class T>
		using iterator_t = typename iterator<T, std::ranges::range<T>>::type;

		template<class T, bool IS_RANGE>
		struct range_size
		{
		};
		template<class T>
		struct range_size<T, true>
		{
			using type = std::ranges::range_size_t<T>;
		};
		template<class T>
		struct range_size<T, false>
		{
			using type = size_t;
		};
		template<class T>
		using range_size_t = typename range_size<T, std::ranges::range<T>>::type;

		template<class T, bool IS_RANGE>
		struct range_difference
		{
		};
		template<class T>
		struct range_difference<T, true>
		{
			using type = std::ranges::range_difference_t<T>;
		};
		template<class T>
		struct range_difference<T, false>
		{
			using type = long;
		};
		template<class T>
		using range_difference_t = typename range_difference<T, std::ranges::range<T>>::type;

	}


	template<class RANGE, size_t NDIMS>
	requires (std::ranges::random_access_range<RANGE> || (!std::ranges::range<RANGE> && NDIMS == 0) ) // either RANGE is a random access range or a scalar (i.e. Range isn't a range and NDIMS=0)
		class grid_view : public std::ranges::view_base
	{
	public:
		/*template<class R>
		class Iterator : public sci::ranges::iterator_t<R>
		{
		public:
			using base_type = sci::ranges::iterator_t<R>;
			using iterator_category = std::iterator_traits<base_type>::iterator_category;
			using value_type = std::iterator_traits<base_type>::value_type;
			using pointer = std::iterator_traits<base_type>::pointer;
			using reference = std::iterator_traits<base_type>::reference;
			using size_type = sci::ranges::range_size_t<R>;
			using difference_type = sci::ranges::range_difference_t<R>;
			//template<std::ranges::contiguous_range<R>>
			//using element_type = std::pointer_traits<pointer>::element_type;
			//using element_type = base_type::element_type;

			Iterator<R>() = default;
			Iterator<R>(base_type const& b)
				:base_type(b)
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
			Iterator<R>& operator++()
			{
				++static_cast<base_type&>(*this);
				return (*this);
			}
			// Postfix increment
			Iterator<R> operator++(int)
			{
				return static_cast<base_type&>(*this)++;
			}
			// Prefix decrement
			Iterator<R>& operator--()
			{
				--static_cast<base_type&>(*this);
				return (*this);
			}
			// Postfix decrement
			Iterator<R> operator--(int)
			{
				return static_cast<base_type&>(*this)--;
			}
			Iterator<R>& operator+=(const difference_type offset)
			{
				static_cast<base_type&>(*this) += offset;
				return *this;
			}

			Iterator<R> operator+(const difference_type offset) const noexcept
			{
				Iterator<R> tmp = *this;
				tmp += offset;
				return tmp;
			}

			friend Iterator<R> operator+(const difference_type offset, const Iterator<R>& iter) noexcept
			{
				return iter + offset;
			}

			Iterator<R>& operator-=(const difference_type offset) noexcept
			{
				return *this += -offset;
			}

			Iterator<R> operator-(const difference_type offset) const noexcept
			{
				Iterator<R> tmp = *this;
				tmp -= offset;
				return tmp;
			}

			friend Iterator<R> operator-(const difference_type offset, const Iterator<R>& iter) noexcept
			{
				return iter - offset;
			}

			difference_type operator-(const Iterator<R>& right) const noexcept
			{
				return static_cast<const base_type&>(*this) - static_cast<const base_type&>(right);
			}

			reference operator[](const difference_type offset) const noexcept
			{
				return *(*this + offset);
			}
			bool operator==(const Iterator<R>& right) const noexcept
			{
				return (static_cast<const base_type&>(*this) == static_cast<const base_type&>(right));
			}
			bool operator!=(const Iterator<R>& right) const noexcept
			{
				return !(*this == right);
			}

			bool operator<(const Iterator<R>& right) const noexcept
			{
				return static_cast<const base_type&>(*this) < static_cast<const base_type&>(right);
			}

			bool operator>(const Iterator<R>& right) const noexcept
			{
				return right < *this;
			}

			bool operator<=(const Iterator<R>& right) const noexcept
			{
				return !(right < *this);
			}

			bool operator>=(const Iterator<R>& right) const noexcept
			{
				return !(*this < right);
			}
		};

		template<bool IS_RANGE>
		struct IteratorChooser
		{};
		template<>
		struct IteratorChooser<true>
		{
			using type = Iterator<RANGE>;
			using size_type = Iterator<RANGE>::size_type;
		};
		template<>
		struct IteratorChooser<false>
		{
			typedef std::add_pointer<RANGE>::type type;
			using size_type = size_t;
			//using type = (RANGE*);
		};*/
		template<size_t NDIMS>
		struct TypeDefs
		{
			using iterator = std::ranges::iterator_t<std::remove_reference_t<RANGE>>;
			using const_iterator = iterator;
			using reference_type = std::ranges::range_reference_t<RANGE>;
			using rvalue_reference_type = std::ranges::range_rvalue_reference_t<std::remove_reference_t<RANGE>>;
			using size_type = std::ranges::range_size_t<std::remove_reference_t<RANGE>>;
			using difference_type = std::ranges::range_difference_t<std::remove_reference_t<RANGE>>;
			using sentinel = std::ranges::sentinel_t<std::remove_reference_t<RANGE>>;
			using value_type = std::iterator_traits<iterator>::value_type;
		};

		template<>
		struct TypeDefs<0>
		{
			using iterator = std::add_pointer_t<std::remove_reference_t<RANGE>>;
			using const_iterator = iterator;
			using reference_type = std::add_lvalue_reference_t<std::remove_reference_t<RANGE>>;
			using rvalue_reference_type = std::add_rvalue_reference_t<std::remove_reference_t<RANGE>>;
			using size_type = size_t;
			using difference_type = size_t;
			using sentinel = iterator;
			using value_type = RANGE;
		};

		using iterator = TypeDefs<NDIMS>::iterator;
		using const_iterator = TypeDefs<NDIMS>::const_iterator;
		using reference_type = TypeDefs<NDIMS>::reference_type;
		using rvalue_reference_type = TypeDefs<NDIMS>::rvalue_reference_type;
		using size_type = TypeDefs<NDIMS>::size_type;
		using difference_type = TypeDefs<NDIMS>::difference_type;
		using sentinel = TypeDefs<NDIMS>::sentinel;
		using value_type = TypeDefs<NDIMS>::value_type;
		//using iterator = typename IteratorChooser<std::ranges::range<typename std::remove_reference<RANGE>::type>>::type;
		//using const_iterator = iterator;
		
		//using reference_type = typename std::iterator_traits<iterator>::reference;
		//using size_type = IteratorChooser<std::ranges::range<typename std::remove_reference<RANGE>::type>>::size_type;
		//using difference_type = std::iterator_traits<iterator>::difference_type;
		//using sentinel = iterator;
		static const size_t ndims = NDIMS;

		constexpr grid_view() = default;
		constexpr grid_view(grid_view<RANGE, NDIMS> const& rhs) = default;
		constexpr grid_view(grid_view<RANGE, NDIMS>&& rhs) = default;
		constexpr grid_view operator=(grid_view<RANGE, NDIMS> const& rhs) = delete; //deleted to avoid accidentally pointing the view at a different grid, when the intention was assigning the elements of the view. Use construction or retarget instead
		constexpr grid_view operator=(grid_view<RANGE, NDIMS>&& rhs) = delete; //deleted to avoid accidentally pointing the view at a different grid, when the intention was assigning the elements of the view. Use construction or retarget instead
		template<IsGridDims<NDIMS> GRID>
		constexpr grid_view operator=(const GRID &rhs)
			requires std::convertible_to<GRID::value_type, value_type>
		{
			return assign(rhs.getView());
		}
		//set every element to a value
		template<class T>
		constexpr grid_view operator=(const T& rhs)
			requires std::convertible_to<T, value_type>
		{
			for (auto& element : (*this))
				element = value_type(rhs);
			return *this;
		}
		void retarget(grid_view<RANGE, NDIMS> const& other)
		{
			m_dataMembers = other.m_dataMembers;
			m_strides = other.m_strides;
		}
		void retarget(grid_view<RANGE, NDIMS> && other)
		{
			std::swap(m_dataMembers, other.m_dataMembers);
			std::swap(m_strides, other.m_strides);
		}
		~grid_view() = default;

		grid_view(RANGE&& range, const GridPremultipliedStridesReference<NDIMS>& strides) requires (std::ranges::random_access_range<RANGE>)
			: m_dataMembers{ new data_members_t{std::forward<RANGE>(range)} }, m_strides(strides)
		{
		}

		template<class T, class Allocator>
		grid_view(GridData<T, NDIMS, Allocator> &gridData)
			: m_dataMembers{ new data_members_t{(gridData)} }, m_strides(gridData.getStridesPointer())
		{
		}

		grid_view(RANGE&& range) requires (!std::ranges::range<RANGE> && NDIMS == 0)
			: m_dataMembers{ new data_members_t{std::forward<RANGE>(range)} }
		{
		}

		iterator begin() const
		{
			if constexpr (NDIMS > 0)
				return std::ranges::begin(m_dataMembers->m_range);
			else
				return &m_dataMembers->m_range;
		}
		iterator cbegin() const
		{
			return begin();
		}
		sentinel end() const
		{
			if constexpr (NDIMS > 0)
				return std::ranges::end(m_dataMembers->m_range);
			else
				return (&m_dataMembers->m_range) + 1;
		}
		sentinel cend() const
		{
			return end();
		}
		reference_type first() const
		{
			if constexpr (NDIMS > 0)
				return *begin();
			else
				return m_dataMembers->m_range;
		}
		reference_type last() const
		{
			if constexpr (NDIMS > 0)
				return *(end()-1);
			else
				return m_dataMembers->m_range;
		}
		const GridPremultipliedStridesReference<NDIMS> getStrides() const
		{
			return m_strides;
		}
		auto size() const
		{
			if constexpr (NDIMS > 0)
				return std::ranges::size(m_dataMembers->m_range);
			else
				return size_t(1);
		}
		reference_type operator[](const difference_type& index) requires(NDIMS == 1)
		{
			return m_dataMembers->m_range[index];
		}
		const reference_type operator[](const difference_type& index) const requires(NDIMS == 1)
		{
			return m_dataMembers->m_range[index];
		}
		reference_type operator[](const std::array<size_t, NDIMS>& index)
		{
			if constexpr (NDIMS == 0)
				return m_dataMembers->m_range;
			else if constexpr (NDIMS == 1)
				return operator[](index[0]);
			else
				return m_dataMembers->m_range[m_strides.getOffset(index)];
		}
		const reference_type operator[](const std::array<size_t, NDIMS>& index) const
		{
			if constexpr (NDIMS == 0)
				return m_dataMembers->m_range;
			else if constexpr (NDIMS == 1)
				return operator[](index[0]);
			else
				return m_dataMembers->m_range[m_strides.getOffset(index)];
		}
		auto operator[](const difference_type& index) requires(NDIMS > 1)
		{
			auto subrange = std::ranges::subrange(m_dataMembers->m_range.begin() + index * m_strides.stride(),
				m_dataMembers->m_range.begin() + (index + 1) * m_strides.stride());
			static_assert((bool)std::ranges::random_access_range<decltype(subrange)>, "subrange failed the test for being a random access range");
			return grid_view<decltype(subrange), NDIMS - 1>(std::forward<decltype(subrange)>(subrange), m_strides.next());
		}
		const auto operator[](const difference_type& index) const requires(NDIMS > 1)
		{
			auto subrange = std::ranges::subrange(m_dataMembers->m_range.begin() + index * m_strides.stride(),
				m_dataMembers->m_range.begin() + (index + 1) * m_strides.stride());
			static_assert((bool)std::ranges::random_access_range<decltype(subrange)>, "subrange failed the test for being a random access range");
			return grid_view<decltype(subrange), NDIMS - 1>(std::forward<decltype(subrange)>(subrange), m_strides.next());
		}
		std::array<size_t, NDIMS> shape() const
		{
			return m_strides.getShape(size());
		}
		reference_type at(const difference_type& index) requires(NDIMS == 1)
		{
			return m_dataMembers->m_range.at(index);
		}
		const reference_type at(const difference_type& index) const requires(NDIMS == 1)
		{
			return m_dataMembers->m_range.at(index);
		}
		reference_type at(const std::array<size_t, NDIMS>& index)
		{
			return m_dataMembers->m_range.at(m_strides.getOffset(index));
		}
		const reference_type at(const std::array<size_t, NDIMS>& index) const
		{
			return m_dataMembers->m_range.at(m_strides.getOffset(index));
		}
		auto at(const difference_type& index) requires(NDIMS > 1)
		{
			//use vector::at to do range checking on first and last element
			auto first = m_dataMembers->m_range.at(index * m_strides.stride());
			auto last = m_dataMembers->m_range.at((index + 1) * m_strides.stride()-1);

			auto subrange = std::ranges::subrange(m_dataMembers->m_range.begin() + index * m_strides.stride(),
				m_dataMembers->m_range.begin() + (index + 1) * m_strides.stride());
			static_assert((bool)std::ranges::random_access_range<decltype(subrange)>, "subrange failed the test for being a random access range");
			return grid_view<decltype(subrange), NDIMS - 1>(std::forward<decltype(subrange)>(subrange), m_strides.next());
		}
		const auto at(const difference_type& index) const requires(NDIMS > 1)
		{
			//use vector::at to do range checking on first and last element
			auto first = m_dataMembers->m_range.at(index * m_strides.stride());
			auto last = m_dataMembers->m_range.at((index + 1) * m_strides.stride() - 1);

			auto subrange = std::ranges::subrange(m_dataMembers->m_range.begin() + index * m_strides.stride(),
				m_dataMembers->m_range.begin() + (index + 1) * m_strides.stride());
			static_assert((bool)std::ranges::random_access_range<decltype(subrange)>, "subrange failed the test for being a random access range");
			return grid_view<decltype(subrange), NDIMS - 1>(std::forward<decltype(subrange)>(subrange), m_strides.next());
		}
		bool empty() const
		{
			return std::ranges::empty(m_dataMembers->m_range);
		}
		grid_view<RANGE, NDIMS>& getView()
		{
			return *this;
		}
		const grid_view<RANGE, NDIMS>& getView() const
		{
			return *this;
		}
		template<IsGridDims<NDIMS> GRID>
		grid_view<RANGE, NDIMS> assign(const GRID other)
			requires std::convertible_to<typename GRID::value_type, value_type>
		{
			if (other.shape() != shape())
				throw(std::out_of_range("Attempted to assign to a grid_view with a grid of differing shape."));
			auto iter = begin();
			auto otherIter = other.begin();
			for (; iter != end(); ++iter, ++otherIter)
				*iter = static_cast<value_type>(*otherIter);
			return *this;
		}
		auto data() const requires sci::has_data<RANGE>
		{
			return m_dataMembers->m_range.data();
		}
		auto data()
		{
			return m_dataMembers->m_range.data();
		}

	private:
		struct data_members_t
		{
			RANGE m_range;
		};
		std::shared_ptr<data_members_t> m_dataMembers;
		GridPremultipliedStridesReference<NDIMS> m_strides;
	};

	//overloads sci::assign in statistics.h
	template<class RANGE1, class RANGE2, size_t NDIMS>
	grid_view<RANGE1, NDIMS> assign(grid_view<RANGE1, NDIMS> destination, const grid_view<RANGE2, NDIMS>& source)
		requires std::convertible_to<typename grid_view<RANGE2, NDIMS>::value_type, typename grid_view<RANGE1, NDIMS>::value_type>
	{
		return destination.assign(source);
	}

	template <size_t NDIMS, class RANGE>
	requires std::ranges::random_access_range<RANGE>
		grid_view(RANGE&&, const GridPremultipliedStridesReference<NDIMS>&)->grid_view<RANGE, NDIMS>;

	template <size_t NDIMS, class RANGE>
	requires std::ranges::random_access_range<RANGE>
		grid_view(RANGE&&)->grid_view<RANGE, 1>;



	template<size_t NDIMS>
	struct grid_fn
	{
		grid_fn() = default;
		grid_fn(const GridPremultipliedStridesReference<NDIMS>& strides)
			:m_strides(strides)
		{}
		template <class RANGE>
		requires (std::ranges::random_access_range<RANGE> || (!std::ranges::range<RANGE> && NDIMS == 0) )
			auto operator()(RANGE&& range, const GridPremultipliedStridesReference<NDIMS>& strides) const
		{
			if constexpr (std::ranges::random_access_range<RANGE>)
				return grid_view<RANGE, NDIMS>{ std::forward<RANGE>(range), strides };
			else
				return grid_view<RANGE, NDIMS>{ std::forward<RANGE>(range)};
		}
		template <size_t NDIMS>
		auto operator()(const GridPremultipliedStridesReference<NDIMS>& strides) const
		{
			//This operator() returns a grid_fn which uses strides. This in turn can get piped
			return grid_fn(strides);
		}
		template <typename RANGE>
		requires (std::ranges::random_access_range<RANGE> || (!std::ranges::range<RANGE> && NDIMS == 0) )
			friend auto operator|(RANGE&& range, grid_fn const& fn)
		{
			if constexpr (std::ranges::random_access_range<RANGE>)
				return grid_view<RANGE, NDIMS>{ std::forward<RANGE>(range), fn.m_strides };
			else
				return grid_view<RANGE, NDIMS>{ std::forward<RANGE>(range)};
		}
	private:
		const GridPremultipliedStridesReference<NDIMS> m_strides;
	};

	namespace views
	{
		template<size_t NDIMS>
		grid_fn<NDIMS> grid;
	}
	
	/*template <class _Ty>
	struct pointer_traits
	{
		using pointer = _Ty;
		using element_type = typename _Get_element_type<_Ty>::type;
		using difference_type = typename _Get_ptr_difference_type<_Ty>::type;

		template <class _Other>
		using rebind = typename _Get_rebind_alias<_Ty, _Other>::type;

		using _Reftype = conditional_t<is_void_v<element_type>, char, element_type>&;

		_NODISCARD static pointer pointer_to(_Reftype _Val) noexcept(noexcept(_Ty::pointer_to(_Val))) {
			return _Ty::pointer_to(_Val);
		}
	};*/

}

/*namespace std
{
	template<class RANGE, size_t NDIMS>
	auto to_address(const typename sci::grid_view<RANGE, NDIMS>::iterator& iter) noexcept
	{
		using base_type = const typename sci::grid_view<RANGE, NDIMS>::iterator::base_type;
		return to_address(static_cast<base_type>(iter));
	}
}*/
#include"gridtupleview.h"

#endif