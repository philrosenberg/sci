#pragma once
#include"gridview.h"
#include<tuple>
#include<assert.h>
#include"array.h"

namespace sci
{


	template<class RANGE1, size_t NDIMS1, class RANGE2, size_t NDIMS2>
	class gridpair_view
	{
	private:
		static const size_t NDIMS = std::max(NDIMS1, NDIMS2);
		static const size_t ndims = NDIMS;
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
			//using reference = std::pair<typename std::reference_wrapper<std::iterator_traits<base_type1>>, typename std::reference_wrapper<std::iterator_traits<base_type2>>>;
			//using const_reference = std::pair<const typename std::reference_wrapper<std::iterator_traits<base_type1>>, const typename std::reference_wrapper<std::iterator_traits<base_type2>>>;
			//using reference = std::pair<typename std::iterator_traits<base_type1>::reference, typename std::iterator_traits<base_type2>::reference>;
			//using const_reference = std::pair<const typename std::iterator_traits<base_type1>::reference, const typename std::iterator_traits<base_type2>::reference>;
			using size_type = typename std::ranges::range_size_t<grid_view<RANGE1, NDIMS1>>;
			using difference_type = typename std::ranges::range_difference_t<grid_view<RANGE1, NDIMS1>>;
			//template<std::ranges::contiguous_range<RANGE>>
			//using element_type = std::pair<typename base_type1::element_type, typename base_type2::element_type>;
			//using element_type = std::pointer_traits<pointer>::element_type;

			Iterator() = default;
			Iterator(base_type1 const& b1, base_type2 const& b2)
				:std::pair<base_type1, base_type2>(b1, b2)
			{

			}


			value_type operator*() const
			{
				return value_type(*static_cast<base_type1>(this->first), *static_cast<base_type2>(this->second));
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

			value_type operator[](const difference_type offset) const noexcept
			{
				return *(*this + offset);
			}
			bool operator==(const Iterator& right) const noexcept
			{
				return (static_cast<const base_type1&>(this->first) == static_cast<const base_type1&>(right.first));
			}
			bool operator!=(const Iterator& right) const noexcept
			{
				return !(*this == right);
			}

			bool operator<(const Iterator& right) const noexcept
			{
				return static_cast<const base_type1&>(this->first) < static_cast<const base_type1>(right.second);
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
		//using reference_type = iterator::reference;
		//using const_reference_type = iterator::const_reference;
		using size_type = iterator::size_type;
		using difference_type = iterator::difference_type;
		using sentinel = iterator;
		using  reference_type = iterator::value_type;
		using  const_reference_type = iterator::value_type;

		constexpr gridpair_view() = default;
		constexpr gridpair_view(gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2> const& rhs) = default;
		constexpr gridpair_view(gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2> && rhs) = default;
		constexpr gridpair_view& operator=(gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2> const& rhs) = default;
		constexpr gridpair_view& operator=(gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2> && rhs) = default;
		constexpr gridpair_view(grid_view<RANGE1, NDIMS1> grid1, grid_view<RANGE2, NDIMS2> grid2)
			:m_grid1(grid1), m_grid2(grid2), m_strides(NDIMS1 > NDIMS2 ? grid1.getStrides() : grid2.getStrides())
		{
			assert(grid1.ndims == 0 || grid2.ndims == 0 || grid1.shape() == grid2.shape());
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
		const GridPremultipliedStridesReference<NDIMS> getStrides() const
		{
			return m_strides;
		}
		auto size() const
		{
			return std::size(m_grid1);
		}
		reference_type operator[](const difference_type& index) requires(NDIMS ==1)
		{
			return reference_type(*(begin()+index));
		}
		const_reference_type operator[](const difference_type& index) const requires(std::max(NDIMS1, NDIMS2) == 1)
		{
			return const_reference_type(*(begin() + index));
		}
		reference_type operator[](const std::array<size_t, NDIMS>& index)
		{
			return reference_type(*(begin() + m_strides.getOffset(index)));
		}
		const_reference_type operator[](const std::array<size_t, NDIMS>& index) const
		{
			return const_reference_type(*(begin() + m_grid1.getStrides().getOffset(index)));
		}
		auto operator[](const difference_type& index) requires(NDIMS != 1)
		{
			if constexpr (NDIMS1 > 0 && NDIMS2 > 0)
			{
				auto sub1 = m_grid1[index];
				auto sub2 = m_grid2[index];
				return gridpair_view<decltype(sub1)::range_type, NDIMS1 - 1, decltype(sub2)::range_type, NDIMS2 - 1>(sub1, sub2);
			}
			else if constexpr (NDIMS1 > 1)
			{
				auto sub1 = m_grid1[index];
				return gridpair_view<decltype(sub1)::range_type, NDIMS1 - 1, decltype(m_grid2), 0>(m_grid1[index], m_grid2);
			}
			else if constexpr (NDIMS2 > 1)
			{
				auto sub2 = m_grid2[index];
				return gridpair_view<decltype(m_grid1), 0, decltype(sub2)::range_type, NDIMS2 - 1>(m_grid1, m_grid2[index]);
			}
			//else //operator[] for two scalars probably is nonsensical
			//{
			//	return gridpair_view<decltype(m_grid1), 0, decltype(m_grid2), 0>(m_grid1, m_grid2);
			//}
		}
		const auto operator[](const difference_type& index) const requires(NDIMS > 1)
		{
			if constexpr (NDIMS1 > 0 && NDIMS2 > 0)
			{
				auto sub1 = m_grid1[index];
				auto sub2 = m_grid2[index];
				return gridpair_view<decltype(sub1)::range_type, NDIMS1 - 1, decltype(sub2)::range_type, NDIMS2 - 1>(sub1, sub2);
			}
			else if constexpr (NDIMS1 > 1)
			{
				auto sub1 = m_grid1[index];
				return gridpair_view<decltype(sub1)::range_type, NDIMS1 - 1, decltype(m_grid2), 0>(m_grid1[index], m_grid2);
			}
			else if constexpr (NDIMS2 > 1)
			{
				auto sub2 = m_grid2[index];
				return gridpair_view<decltype(m_grid1), 0, decltype(sub2)::range_type, NDIMS2 - 1>(m_grid1, m_grid2[index]);
			}
			//else //operator[] for two scalars probably is nonsensical
			//{
			//	return gridpair_view<decltype(m_grid1), 0, decltype(m_grid2), 0>(m_grid1, m_grid2);
			//}
		}
		std::array<size_t, NDIMS> shape() const
		{
			if constexpr (NDIMS1 > NDIMS2)
				return m_grid1.shape();
			else
				return m_grid2.shape();
		}
		gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2>& getView()
		{
			return *this;
		}
		const gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2>& getView() const
		{
			return *this;
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
		GridPremultipliedStridesReference<std::max(NDIMS1, NDIMS2)> m_strides;
	};


	template<class RANGE1, size_t NDIMS1, class RANGE2, size_t NDIMS2>
	auto make_gridpair_view(grid_view<RANGE1, NDIMS1> grid1, grid_view<RANGE2, NDIMS2> grid2)
	{
		return gridpair_view<RANGE1, NDIMS1, RANGE2, NDIMS2>(grid1, grid2);
	}



	template<class RANGE1, size_t NDIMS1, class RANGE2, size_t NDIMS2, class TRANSFORM>
	class gridpairtransform_view
	{
	private:
		static const size_t NDIMS = std::max(NDIMS1, NDIMS2);
		static const size_t ndims = NDIMS;


	public:
		class Iterator : public std::pair<typename grid_view<RANGE1, NDIMS1>::iterator, typename grid_view<RANGE2, NDIMS2>::iterator>
		{
		public:
			using base_type1 = typename std::ranges::iterator_t<grid_view<RANGE1, NDIMS1>>;
			using base_type2 = typename std::ranges::iterator_t<grid_view<RANGE2, NDIMS2>>;
			//to do - work out how to sort the return type
			//using transform_return_type = decltype(TRANSFORM t(*base_type1(), *base_type2()));
			using transform_return_type = base_type1::value_type;
			using iterator_category = std::random_access_iterator_tag;
			using value_type = transform_return_type;
			//using difference_type = typename std::iterator_traits<base_type1>::difference_type;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			//using reference = std::pair<typename std::reference_wrapper<std::iterator_traits<base_type1>>, typename std::reference_wrapper<std::iterator_traits<base_type2>>>;
			//using const_reference = std::pair<const typename std::reference_wrapper<std::iterator_traits<base_type1>>, const typename std::reference_wrapper<std::iterator_traits<base_type2>>>;
			//using reference = std::pair<typename std::iterator_traits<base_type1>::reference, typename std::iterator_traits<base_type2>::reference>;
			//using const_reference = std::pair<const typename std::iterator_traits<base_type1>::reference, const typename std::iterator_traits<base_type2>::reference>;
			using size_type = typename std::ranges::range_size_t<grid_view<RANGE1, NDIMS1>>;
			using difference_type = typename std::ranges::range_difference_t<grid_view<RANGE1, NDIMS1>>;
			//template<std::ranges::contiguous_range<RANGE>>
			//using element_type = std::pair<typename base_type1::element_type, typename base_type2::element_type>;
			//using element_type = std::pointer_traits<pointer>::element_type;

			Iterator() = default;
			Iterator(base_type1 const& b1, base_type2 const& b2, TRANSFORM transform)
				:std::pair<base_type1, base_type2>(b1, b2), m_transform(transform)
			{

			}


			value_type operator*() const
			{
				return value_type(m_transform(*static_cast<base_type1>(this->first), *static_cast<base_type2>(this->second)));
			}
			pointer operator->()
			{
				return pointer(&(*this));
			}
			const pointer operator->() const
			{
				return pointer(&(*this));
			}
			// Prefix increment
			Iterator& operator++()
			{
				if constexpr (NDIMS1 > 0)
					++static_cast<base_type1&>(this->first);
				if constexpr (NDIMS2 > 0)
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
					static_cast<base_type1&>(this->first) += offset;
				if constexpr (NDIMS2 > 0)
					static_cast<base_type2&>(this->second) += offset;
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

			value_type operator[](const difference_type offset) const noexcept
			{
				return *(*this + offset);
			}
			bool operator==(const Iterator& right) const noexcept
			{
				return (static_cast<const base_type1&>(this->first) == static_cast<const base_type1&>(right.first));
			}
			bool operator!=(const Iterator& right) const noexcept
			{
				return !(*this == right);
			}

			bool operator<(const Iterator& right) const noexcept
			{
				return static_cast<const base_type1&>(this->first) < static_cast<const base_type1>(right.second);
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
			private:
				TRANSFORM m_transform;
		};


		using iterator = Iterator;
		using const_iterator = iterator;
		//using reference_type = iterator::reference;
		//using const_reference_type = iterator::const_reference;
		using size_type = iterator::size_type;
		using difference_type = iterator::difference_type;
		using sentinel = iterator;
		using  reference_type = iterator::value_type;
		using  const_reference_type = iterator::value_type;

		constexpr gridpairtransform_view() = default;
		constexpr gridpairtransform_view(gridpairtransform_view<RANGE1, NDIMS1, RANGE2, NDIMS2, TRANSFORM> const& rhs) = default;
		constexpr gridpairtransform_view(gridpairtransform_view<RANGE1, NDIMS1, RANGE2, NDIMS2, TRANSFORM>&& rhs) = default;
		constexpr gridpairtransform_view& operator=(gridpairtransform_view<RANGE1, NDIMS1, RANGE2, NDIMS2, TRANSFORM> const& rhs) = default;
		constexpr gridpairtransform_view& operator=(gridpairtransform_view<RANGE1, NDIMS1, RANGE2, NDIMS2, TRANSFORM>&& rhs) = default;
		constexpr gridpairtransform_view(grid_view<RANGE1, NDIMS1> grid1, grid_view<RANGE2, NDIMS2> grid2, TRANSFORM transform)
			:m_grid1(grid1), m_grid2(grid2), m_strides(NDIMS1 > NDIMS2 ? grid1.getStrides() : grid2.getStrides())
		{
			m_transform = transform;
			assert(grid1.ndims==0 || grid2.ndims == 0 ||grid1.shape() == grid2.shape());
		}
		~gridpairtransform_view() = default;

		iterator begin() const
		{
			return iterator(std::begin(m_grid1), std::begin(m_grid2), m_transform);
		}
		iterator cbegin() const
		{
			return begin();
		}
		sentinel end() const
		{
			return sentinel(std::end(m_grid1), std::end(m_grid2), m_transform);
		}
		sentinel cend() const
		{
			return end();
		}
		const GridPremultipliedStridesReference<NDIMS> getStrides() const
		{
			return m_strides;
		}
		auto size() const
		{
			return std::size(m_grid1);
		}
		reference_type operator[](const difference_type& index) requires(NDIMS == 1)
		{
			return reference_type(*(begin() + index));
		}
		const_reference_type operator[](const difference_type& index) const requires(std::max(NDIMS1, NDIMS2) == 1)
		{
			return const_reference_type(*(begin() + index));
		}
		reference_type operator[](const std::array<size_t, NDIMS>& index)
		{
			return reference_type(*(begin() + m_strides.getOffset(index)));
		}
		const_reference_type operator[](const std::array<size_t, NDIMS>& index) const
		{
			return const_reference_type(*(begin() + m_grid1.getStrides().getOffset(index)));
		}
		auto operator[](const difference_type& index) requires(NDIMS != 1)
		{
			if constexpr (NDIMS1 > 0 && NDIMS2 > 0)
			{
				auto sub1 = m_grid1[index];
				auto sub2 = m_grid2[index];
				return gridpairtransform_view<decltype(sub1)::range_type, NDIMS1 - 1, decltype(sub2)::range_type, NDIMS2 - 1, TRANSFORM>(sub1, sub2, m_transform);
			}
			else if constexpr (NDIMS1 > 1)
			{
				auto sub1 = m_grid1[index];
				return gridpair_view<decltype(sub1)::range_type, NDIMS1 - 1, decltype(m_grid2), 0, TRANSFORM>(m_grid1[index], m_grid2, m_transform);
			}
			else if constexpr (NDIMS2 > 1)
			{
				auto sub2 = m_grid2[index];
				return gridpair_view<decltype(m_grid1), 0, decltype(sub2)::range_type, NDIMS2 - 1, TRANSFORM>(m_grid1, m_grid2[index], m_transform);
			}
			//else //operator[] for two scalars probably is nonsensical
			//{
			//	return gridpair_view<decltype(m_grid1), 0, decltype(m_grid2), 0>(m_grid1, m_grid2);
			//}
		}
		const auto operator[](const difference_type& index) const requires(NDIMS > 1)
		{
			if constexpr (NDIMS1 > 0 && NDIMS2 > 0)
			{
				auto sub1 = m_grid1[index];
				auto sub2 = m_grid2[index];
				return gridpair_view<decltype(sub1)::range_type, NDIMS1 - 1, decltype(sub2)::range_type, NDIMS2 - 1, TRANSFORM>(sub1, sub2, m_transform);
			}
			else if constexpr (NDIMS1 > 1)
			{
				auto sub1 = m_grid1[index];
				return gridpair_view<decltype(sub1)::range_type, NDIMS1 - 1, decltype(m_grid2), 0, TRANSFORM>(m_grid1[index], m_grid2, m_transform);
			}
			else if constexpr (NDIMS2 > 1)
			{
				auto sub2 = m_grid2[index];
				return gridpair_view<decltype(m_grid1), 0, decltype(sub2)::range_type, NDIMS2 - 1, TRANSFORM>(m_grid1, m_grid2[index], m_transform);
			}
			//else //operator[] for two scalars probably is nonsensical
			//{
			//	return gridpair_view<decltype(m_grid1), 0, decltype(m_grid2), 0>(m_grid1, m_grid2);
			//}
		}
		std::array<size_t, NDIMS> shape() const
		{
			if constexpr (NDIMS1 > NDIMS2)
				return m_grid1.shape();
			else
				return m_grid2.shape();
		}
		gridpairtransform_view<RANGE1, NDIMS1, RANGE2, NDIMS2, TRANSFORM>& getView()
		{
			return *this;
		}
		const gridpairtransform_view<RANGE1, NDIMS1, RANGE2, NDIMS2, TRANSFORM>& getView() const
		{
			return *this;
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
		GridPremultipliedStridesReference<std::max(NDIMS1, NDIMS2)> m_strides;
		TRANSFORM m_transform;
	};













	template<class GRID1, class GRID2, class TRANSFORM>
	auto make_gridpairtransform_view(GRID1 &grid1, GRID2 &grid2, TRANSFORM transform)// requires(bool(isGrid<GRID1>() && idGrid<GRID2>()))
	{
		using range1 = decltype(grid1.getView())::range_type;
		using range2 = decltype(grid2.getView())::range_type;
		return gridpairtransform_view<range1, GRID1::ndims, range2, GRID2::ndims, TRANSFORM>(grid1.getView(), grid2.getView(), transform);
	}
	/*template<class T, size_t NDIMS1, class RANGE2, size_t NDIMS2, class TRANSFORM>
	auto make_gridpairtransform_view(GridData<T, NDIMS1> &grid1, GridData<U, NDIMS2> &grid2, TRANSFORM transform)
	{
		return make_gridpairtransform_view(grid1.getView(), grid2.getView(), transform);
	}
	template<class RANGE1, size_t NDIMS1, class U, size_t NDIMS2, class TRANSFORM>
	auto make_gridpairtransform_view(GridData<T, NDIMS1>& grid1, GridData<U, NDIMS2>& grid2, TRANSFORM transform)
	{
		return make_gridpairtransform_view(grid1.getView(), grid2.getView(), transform);
	}
	template<class T, size_t NDIMS1, class U, size_t NDIMS2, class TRANSFORM>
	auto make_gridpairtransform_view(GridData<T, NDIMS1>& grid1, GridData<U, NDIMS2>& grid2, TRANSFORM transform)
	{
		return make_gridpairtransform_view(grid1.getView(), grid2.getView(), transform);
	}*/



	auto add(double a, double b)
	{
		return a + b;
	}

	template<class T, class U>
	requires(bool(isGrid<T>() && isGrid<U>() && std::ranges::random_access_range<T>&& std::ranges::random_access_range<U>))
		auto operator+(T& a, U& b)
	{
		return make_gridpairtransform_view(a, b, add);
		//using pair_type = gridpair_view<T, T::ndims, U, U::ndims>;
		//pair_type pair(a, b);
		//auto operated = std::forward<pair_type>(pair) | std::ranges::views::transform([](pair_type::const_reference_type p){ return (p.first + p.second); });
		//return operated | views::grid<std::max(T::ndims, U::ndims)>(a.getStrides());
		//using pair_type = gridpair_view<T, T::ndims, U, U::ndims>;
	}
}