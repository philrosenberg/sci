#pragma once

#include<ranges>
#include<array>
#include<memory>

namespace sci
{
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
				result[0] = size / m_ptr.ptr[0];
				for (size_t i = 1; i < NDIMS - 1; ++i)
					result[i] = m_ptr.ptr[i - 1] / m_ptr.ptr[i];
				result[NDIMS - 1] = m_ptr.ptr[NDIMS - 2];
				return result;
			}
		}
		constexpr size_t getOffset(const std::array<size_t, NDIMS>& index)
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
	private:
		GridPremultipliedStridesPointer<NDIMS> m_ptr;
	};


	template<class RANGE, size_t NDIMS>
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
				static_cast<base_type&>(*this) += offset;
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

		constexpr grid_view() = default;
		constexpr grid_view(grid_view<RANGE, NDIMS> const& rhs) = default;
		constexpr grid_view(grid_view<RANGE, NDIMS>&& rhs) = default;
		constexpr grid_view& operator=(grid_view<RANGE, NDIMS> const& rhs) = default;
		constexpr grid_view& operator=(grid_view<RANGE, NDIMS>&& rhs) = default;
		~grid_view() = default;

		grid_view(RANGE&& range, const GridPremultipliedStridesReference<NDIMS>& strides)
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
			return grid_view<decltype(subrange), NDIMS - 1>(std::forward<decltype(subrange)>(subrange), m_strides.next());
		}
		const auto operator[](const difference_type& index) const requires(NDIMS > 1)
		{
			auto subrange = std::ranges::subrange(m_dataMembers->m_range.begin() + index * m_strides.stride(),
				m_dataMembers->m_range.begin() + (index + 1) * m_strides.stride());
			static_assert((bool)std::ranges::random_access_range<decltype(subrange)>, "subrange failed the test for being a random access range");
			return grid_view<decltype(subrange), NDIMS - 1>(std::forward<decltype(subrange)>(subrange), m_strides.next());
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
		GridPremultipliedStridesReference<NDIMS> m_strides;
	};


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
		template <class RANGE, size_t NDIMS>
		requires std::ranges::random_access_range<RANGE>
			auto operator()(RANGE&& range, const GridPremultipliedStridesReference<NDIMS>& strides) const
		{
			return grid_view<RANGE, NDIMS>{ std::forward<RANGE>(range), m_strides };
		}
		template <size_t NDIMS>
		auto operator()(const GridPremultipliedStridesReference<NDIMS>& strides) const
		{
			//This operator() returns a grid_fn which uses strides. This in turn can get piped
			return grid_fn(strides);
		}
		template <typename RANGE>
		requires std::ranges::random_access_range<RANGE>
			friend auto operator|(RANGE&& range, grid_fn const& fn)
		{
			return grid_view<RANGE, NDIMS>{ std::forward<RANGE>(range), fn.m_strides };
		}
	private:
		const GridPremultipliedStridesReference<NDIMS> m_strides;
	};

	namespace views
	{
		template<size_t NDIMS>
		grid_fn<NDIMS> grid;
	}
}