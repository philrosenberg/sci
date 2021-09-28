#pragma once
#include"gridview.h"
#include<tuple>
#include<assert.h>

namespace sci
{


	template<class RANGE1, size_t NDIMS1, class RANGE2, size_t NDIMS2>
	class gridpair_view
	{
	private:
		const size_t NDIMS;
	public:
		class Iterator : public std::pair<typename grid_view<RANGE1, NDIMS1>::iterator, typename grid_view<RANGE2, NDIMS2>::iterator>
		{
		public:
			using base_type1 = typename std::ranges::iterator_t<grid_view<RANGE1, NDIMS1>>;
			using base_type2 = typename std::ranges::iterator_t<grid_view<RANGE2, NDIMS2>>;
			using iterator_category = std::random_access_iterator_tag;
			using value_type = std::pair<typename std::iterator_traits<base_type1>::value_type, typename std::iterator_traits<base_type2>::value_type>;
			//using difference_type = typename std::iterator_traits<base_type1>::difference_type;
			using pointer = std::pair<typename std::iterator_traits<base_type1>::pointer, typename std::iterator_traits<base_type2>::pointer>;
			using const_pointer = std::pair<const typename std::iterator_traits<base_type1>::pointer, const typename std::iterator_traits<base_type2>::pointer>;
			using reference = std::pair<typename std::iterator_traits<base_type1>::reference, typename std::iterator_traits<base_type2>::pointer>;
			using const_reference = std::pair<const typename std::iterator_traits<base_type1>::reference, const typename std::iterator_traits<base_type2>::reference>;
			using size_type = typename std::ranges::range_size_t<grid_view<RANGE1, NDIMS1>>;
			using difference_type = typename std::ranges::range_difference_t<grid_view<RANGE1, NDIMS1>>;
			//template<std::ranges::contiguous_range<RANGE>>
			using element_type = value_type;

			Iterator() = default;
			Iterator(base_type1 const& b1, base_type2 const& b2)
				:std::pair<base_type1, base_type2>(b1, b2)
			{

			}


			reference operator*() const
			{
				return reference(*static_cast<base_type1>(this->first), *static_cast<base_type2>(this->second));
			}
			pointer operator->()
			{
				return pointer(static_cast<base_type1>(this->first).operator->(), static_cast<base_type2>(this->second).operator->());
			}
			const pointer operator->() const
			{
				return pointer(static_cast<const base_type1>(this->first).operator->(), static_cast<const base_type2>(this->second).operator->());
			}
			// Prefix increment
			Iterator& operator++()
			{
				if constexpr(NDIMS1 > 0)
					++static_cast<base_type1&>(this->first);
				if constexpr(NDIMS2 > 0)
					++static_cast<base_type2&>(this->second);
				return (*this);
			}
			// Postfix increment
			Iterator operator++(int)
			{
				Iterator temp = *this;
				if constexpr (NDIMS1 > 0)
					static_cast<base_type1&>(this->first)++;
				if constexpr (NDIMS2 > 0)
					static_cast<base_type2&>(this->second)++;
				return temp;
			}
			// Prefix decrement
			Iterator& operator--()
			{
				if constexpr (NDIMS1 > 0)
					--static_cast<base_type1&>(this->first);
				if constexpr (NDIMS2 > 0)
					--static_cast<base_type2&>(this->second);
				return (*this);
			}
			// Postfix decrement
			Iterator operator--(int)
			{
				Iterator temp = *this;
				if constexpr (NDIMS1 > 0)
					static_cast<base_type1&>(this->first)--;
				if constexpr (NDIMS2 > 0)
					static_cast<base_type2&>(this->second)--;
				return temp;
			}
			Iterator& operator+=(const difference_type offset)
			{
				if constexpr (NDIMS1 > 0)
					static_cast<base_type1&>(this->first)+=offset;
				if constexpr (NDIMS2 > 0)
					static_cast<base_type2&>(this->second)+=offset;
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
				return static_cast<const base_type1&>(this->first) - static_cast<const base_type1&>(right.first);
			}

			reference operator[](const difference_type offset) const noexcept
			{
				return *(*this + offset);
			}
			bool operator==(const Iterator& right) const noexcept
			{
				return (static_cast<const base_type1&>(this->first) == static_cast<const base_type1&>(right->first));
			}
			bool operator!=(const Iterator& right) const noexcept
			{
				return !(*this == right);
			}

			bool operator<(const Iterator& right) const noexcept
			{
				return static_cast<const base_type1&>(this->first) < static_cast<const base_type1>(right->second);
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
		using const_reference_type = iterator::const_reference;
		using size_type = iterator::size_type;
		using difference_type = iterator::difference_type;
		using sentinel = iterator;

		constexpr gridpair_view() = default;
		constexpr gridpair_view(gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2> const& rhs) = default;
		constexpr gridpair_view(gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2> && rhs) = default;
		constexpr gridpair_view& operator=(gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2> const& rhs) = default;
		constexpr gridpair_view& operator=(gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2> && rhs) = default;
		constexpr gridpair_view(grid_view<RANGE1, NDIMS2> grid1, grid_view<RANGE2, NDIMS2> grid2)
			:m_grid1(grid1), m_grid2(grid2), NDIMS(NDIMS1>NDIMS2 ? NDIMS1 : NDIMS2)
		{
			assert(grid1.shape() == grid2.shape());
		}
		~gridpair_view() = default;

		iterator begin() const
		{
			return iterator(std::begin(m_grid1), std::begin(m_grid2));
		}
		iterator cbegin() const
		{
			return begin();
		}
		sentinel end() const
		{
			return sentinel(std::end(m_grid1), std::end(m_grid2));
		}
		sentinel cend() const
		{
			return end();
		}
		auto size() const
		{
			return std::size(m_grid1);
		}
		reference_type operator[](const difference_type& index) requires(NDIMS ==1)
		{
			return reference_type(*(begin()+index));
		}
		const_reference_type operator[](const difference_type& index) const requires(NDIMS == 1)
		{
			return const_reference_type(*(begin() + index));
		}
		reference_type operator[](const std::array<size_t, std::max(NDIMS1,NDIMS2)>& index)
		{
			return reference_type(*(begin() + m_grid1.getStrides().getOffset(index)));
		}
		const_reference_type operator[](const std::array<size_t, std::max(NDIMS1, NDIMS2)>& index) const
		{
			return const_reference_type(*(begin() + m_grid1.getStrides().getOffset(index)));
		}
		auto operator[](const difference_type& index) requires(std::max(NDIMS1, NDIMS2) != 1)
		{
			if constexpr (NDIMS1 > 0 && NDIMS2 > 0)
			{
				return gridpair_view<RANGE1, NDIMS1 - 1, RANGE2, NDIMS2 - 1>(m_grid1[index], m_grid2[index]);
			}
			else if constexpr (NDIMS1 > 1)
			{
				return gridpair_view<RANGE1, NDIMS1 - 1, RANGE2, NDIMS2>(m_grid1[index], m_grid2);
			}
			else if constexpr (NDIMS2 > 1)
			{
				return gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2 - 1>(m_grid1, m_grid2[index]);
			}
			else
			{
				return gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2>(m_grid1, m_grid2);
			}
		}
		const auto operator[](const difference_type& index) const requires(NDIMS > 1)
		{
			if constexpr (NDIMS1 > 0 && NDIMS2 > 0)
			{
				return gridpair_view<RANGE1 const, NDIMS1 - 1, RANGE2 const, NDIMS2 - 1>(m_grid1[index], m_grid2[index]);
			}
			else if constexpr (NDIMS1 > 1)
			{
				return gridpair_view<RANGE1 const, NDIMS1 - 1, RANGE2 const, NDIMS2>(m_grid1[index], m_grid2);
			}
			else if constexpr (NDIMS2 > 1)
			{
				return gridpair_view<RANGE1 const, NDIMS1, RANGE2 const, NDIMS2 - 1>(m_grid1, m_grid2[index]);
			}
			else
			{
				return gridpair_view<RANGE1 const, NDIMS1, RANGE2 const, NDIMS2>(m_grid1, m_grid2);
			}
		}
		std::array<size_t, std::max(NDIMS1, NDIMS2)> shape() const
		{
			return m_grid1.shape();
		}
		/*reference_type at(const difference_type& index) requires(NDIMS == 1)
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
			auto last = m_dataMembers->m_range.at((index + 1) * m_strides.stride() - 1);

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
		}*/
		bool empty() const
		{
			return std::ranges::empty(m_grid1);
		}



	private:
		grid_view<RANGE1, NDIMS1> m_grid1;
		grid_view<RANGE2, NDIMS2> m_grid2;
	};


	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>() && std::ranges::random_access_range<T>&& std::ranges::random_access_range<U>))
		auto operator+(T& a, U& b)
	{
		using pair_type = gridpair_view<T, T::ndims, U, U::ndims>;
		pair_type pair(a, b);
		return pair | std::ranges::views::transform([](pair_type::const_reference_type p){ return (p.first + p.second); });
	}
}