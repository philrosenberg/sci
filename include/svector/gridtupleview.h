#pragma once
#include"gridview.h"
#include<tuple>
#include<assert.h>
#include"array.h"

namespace sci
{


	template<class GRID1, class GRID2>
	class gridpair_view
	{
	private:
		static const size_t NDIMS1 = GRID1::ndims;
		static const size_t NDIMS2 = GRID2::ndims;
	public:
		static const size_t NDIMS = std::max(NDIMS1, NDIMS2);
		static const size_t ndims = NDIMS;
		class Iterator : public std::pair<typename GRID1::iterator, typename GRID2::iterator>
		{
		public:
			using base_type1 = typename std::ranges::iterator_t<GRID1>;
			using base_type2 = typename std::ranges::iterator_t<GRID2>;
			using iterator_category = std::random_access_iterator_tag;
			using value_type = std::pair<typename std::iterator_traits<base_type1>::value_type, typename std::iterator_traits<base_type2>::value_type>;
			//using difference_type = typename std::iterator_traits<base_type1>::difference_type;
			using pointer = std::pair<typename std::iterator_traits<base_type1>::pointer, typename std::iterator_traits<base_type2>::pointer>;
			using const_pointer = std::pair<const typename std::iterator_traits<base_type1>::pointer, const typename std::iterator_traits<base_type2>::pointer>;
			//using reference = std::pair<typename std::reference_wrapper<std::iterator_traits<base_type1>>, typename std::reference_wrapper<std::iterator_traits<base_type2>>>;
			//using const_reference = std::pair<const typename std::reference_wrapper<std::iterator_traits<base_type1>>, const typename std::reference_wrapper<std::iterator_traits<base_type2>>>;
			//using reference = std::pair<typename std::iterator_traits<base_type1>::reference, typename std::iterator_traits<base_type2>::reference>;
			//using const_reference = std::pair<const typename std::iterator_traits<base_type1>::reference, const typename std::iterator_traits<base_type2>::reference>;
			using size_type = typename std::ranges::range_size_t<GRID1>;
			using difference_type = typename std::ranges::range_difference_t<GRID1>;
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
		using value_type = iterator::value_type;

		constexpr gridpair_view() = default;
		constexpr gridpair_view(gridpair_view<GRID1, GRID2> const& rhs) = default;
		constexpr gridpair_view(gridpair_view<GRID1, GRID2> && rhs) = default;
		constexpr gridpair_view& operator=(gridpair_view<GRID1, GRID2> const& rhs) = default;
		constexpr gridpair_view& operator=(gridpair_view<GRID1, GRID2> && rhs) = default;
		constexpr gridpair_view(GRID1 grid1, GRID2 grid2)
			:m_grid1(grid1), m_grid2(grid2)
		{
			if constexpr (NDIMS1 > NDIMS2)
				m_strides = GridPremultipliedStridesReference<NDIMS1>(grid1.getStrides());
			else
				m_strides = GridPremultipliedStridesReference<NDIMS2>(grid2.getStrides());

			for (size_t i = 0; i < std::min(NDIMS1, NDIMS2); ++i)
				assert(grid1.shape()[i] == grid2.shape()[i]);
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
				return gridpair_view<decltype(sub1), decltype(sub1)>(sub1, sub2);
			}
			else if constexpr (NDIMS1 > 1)
			{
				auto sub1 = m_grid1[index];
				return gridpair_view<decltype(sub1), decltype(m_grid2)>(m_grid1[index], m_grid2);
			}
			else if constexpr (NDIMS2 > 1)
			{
				auto sub2 = m_grid2[index];
				return gridpair_view<decltype(m_grid1), decltype(sub2)>(m_grid1, m_grid2[index]);
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
				return gridpair_view<decltype(sub1), decltype(sub2)>(sub1, sub2);
			}
			else if constexpr (NDIMS1 > 1)
			{
				auto sub1 = m_grid1[index];
				return gridpair_view<decltype(sub1), decltype(m_grid2)>(m_grid1[index], m_grid2);
			}
			else if constexpr (NDIMS2 > 1)
			{
				auto sub2 = m_grid2[index];
				return gridpair_view<decltype(m_grid1), decltype(sub2)>(m_grid1, m_grid2[index]);
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
		gridpair_view<GRID1, GRID2>& getView()
		{
			return *this;
		}
		const gridpair_view<GRID1, GRID2>& getView() const
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
		GRID1 m_grid1;
		GRID2 m_grid2;
		GridPremultipliedStridesReference<std::max(NDIMS1, NDIMS2)> m_strides;
	};


	template<class GRID1, class GRID2>
	auto make_gridpair_view(GRID1 grid1, GRID2 grid2)
	{
		return gridpair_view<decltype(grid1.getView()), decltype(grid2.getView())>(grid1.getView(), grid2.getView());
	}



	template<class GRID1, class GRID2, class TRANSFORM>
	class gridpairtransform_view
	{
	private:
		static const size_t NDIMS1 = GRID1::ndims;
		static const size_t NDIMS2 = GRID2::ndims;

	public:
		static const size_t NDIMS = std::max(NDIMS1, NDIMS2);
		static const size_t ndims = NDIMS;

		class Iterator : public std::pair<typename GRID1::iterator, typename GRID2::iterator>
		{
		public:
			using base_type1 = typename std::ranges::iterator_t<GRID1>;
			using base_type2 = typename std::ranges::iterator_t<GRID2>;
			//to do - work out how to sort the return type
			using transform_return_type = decltype(TRANSFORM()(*base_type1(), *base_type2()));
			//using transform_return_type = base_type1::value_type;
			using iterator_category = std::random_access_iterator_tag;
			using value_type = transform_return_type;
			//using difference_type = typename std::iterator_traits<base_type1>::difference_type;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			//using reference = std::pair<typename std::reference_wrapper<std::iterator_traits<base_type1>>, typename std::reference_wrapper<std::iterator_traits<base_type2>>>;
			//using const_reference = std::pair<const typename std::reference_wrapper<std::iterator_traits<base_type1>>, const typename std::reference_wrapper<std::iterator_traits<base_type2>>>;
			//using reference = std::pair<typename std::iterator_traits<base_type1>::reference, typename std::iterator_traits<base_type2>::reference>;
			//using const_reference = std::pair<const typename std::iterator_traits<base_type1>::reference, const typename std::iterator_traits<base_type2>::reference>;
			using size_type = typename std::ranges::range_size_t<GRID1>;
			using difference_type = typename std::ranges::range_difference_t<GRID1>;
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
		using value_type = iterator::value_type;

		constexpr gridpairtransform_view() = default;
		constexpr gridpairtransform_view(gridpairtransform_view<GRID1, GRID2, TRANSFORM> const& rhs) = default;
		constexpr gridpairtransform_view(gridpairtransform_view<GRID1, GRID2, TRANSFORM>&& rhs) = default;
		constexpr gridpairtransform_view& operator=(gridpairtransform_view<GRID1, GRID2, TRANSFORM> const& rhs) = default;
		constexpr gridpairtransform_view& operator=(gridpairtransform_view<GRID1, GRID2, TRANSFORM>&& rhs) = default;
		constexpr gridpairtransform_view(GRID1 grid1, GRID2 grid2, TRANSFORM transform) requires(NDIMS1==NDIMS2 || NDIMS1 == 0 || NDIMS2 == 0)
			:m_grid1(grid1), m_grid2(grid2)
		{
			if constexpr (NDIMS1 > NDIMS2)
				m_strides = GridPremultipliedStridesReference<NDIMS1>(grid1.getStrides());
			else
				m_strides = GridPremultipliedStridesReference<NDIMS2>(grid2.getStrides());

			m_transform = transform;
			for(size_t i=0; i<std::min(NDIMS1, NDIMS2); ++i)
				assert(grid1.shape()[i] == grid2.shape()[i]);
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
				return gridpairtransform_view<decltype(sub1), decltype(sub2), TRANSFORM>(sub1, sub2, m_transform);
			}
			else if constexpr (NDIMS1 > 1)
			{
				auto sub1 = m_grid1[index];
				return gridpairtransform_view<decltype(sub1), decltype(m_grid2), TRANSFORM>(sub1, m_grid2, m_transform);
			}
			else if constexpr (NDIMS2 > 1)
			{
				auto sub2 = m_grid2[index];
				return gridpairtransform_view<decltype(m_grid1), decltype(sub2), TRANSFORM>(m_grid1, sub2, m_transform);
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
				return gridpairtransform_view<decltype(sub1), decltype(sub2)::range_type, TRANSFORM>(sub1, sub2, m_transform);
			}
			else if constexpr (NDIMS1 > 1)
			{
				auto sub1 = m_grid1[index];
				return gridpairtransform_view<decltype(sub1), decltype(m_grid2), TRANSFORM>(m_grid1[index], m_grid2, m_transform);
			}
			else if constexpr (NDIMS2 > 1)
			{
				auto sub2 = m_grid2[index];
				return gridpairtransform_view<decltype(m_grid1), decltype(sub2), TRANSFORM>(m_grid1, m_grid2[index], m_transform);
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
		gridpairtransform_view<GRID1, GRID2, TRANSFORM>& getView()
		{
			return *this;
		}
		const gridpairtransform_view<GRID1, GRID2, TRANSFORM>& getView() const
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
		GRID1 m_grid1;
		GRID2 m_grid2;
		GridPremultipliedStridesReference<std::max(NDIMS1, NDIMS2)> m_strides;
		TRANSFORM m_transform;
	};














	template<class GRID1, class GRID2, class TRANSFORM>
	auto make_gridpairtransform_view(const GRID1 &grid1, const GRID2 &grid2, TRANSFORM transform)
	{
		return gridpairtransform_view<decltype(getGridView(grid1)), decltype(getGridView(grid2)), TRANSFORM>(getGridView(grid1), getGridView(grid2), transform);
	}

	template<class GRID1, class GRID2, class TRANSFORM>
	auto make_ncgridpairtransform_view(GRID1& grid1, const GRID2& grid2, TRANSFORM transform)
	{
		return gridpairtransform_view<decltype(getGridView(grid1)), decltype(getGridView(grid2)), TRANSFORM>(getGridView(grid1), getGridView(grid2), transform);
	}

	template<class TRANSFORM>
	struct discardSecond
	{
		discardSecond(TRANSFORM transform)
			:m_transform(transform)
		{
		}
		discardSecond() = default;
		discardSecond(const discardSecond<TRANSFORM>& other) = default;
		discardSecond& operator=(const discardSecond<TRANSFORM>& other) = default;
		discardSecond& operator=(discardSecond<TRANSFORM>&& other) = default;
		template<class T1, class T2>
		auto operator()(T1&& t, T2) const
		{
			return m_transform(std::forward<T1>(t));
		}
		TRANSFORM m_transform;
	};

	template<size_t NDIMS>
	struct gridpair_fn
	{
		gridpair_fn() = default;
		/*grid_fn(const GridPremultipliedStridesReference<NDIMS>& strides)
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
		}*/
		template <typename RANGE>
		requires std::ranges::random_access_range<RANGE>
			friend auto operator|(RANGE&& range, gridpair_fn const& fn)
		{
			return grid_view<RANGE, NDIMS>{ std::forward<RANGE>(range), fn.m_strides };
		}
	private:
		const GridPremultipliedStridesReference<NDIMS> m_strides;
	};

	namespace views
	{
		template<size_t NDIMS>
		gridpair_fn<NDIMS> gridpair;
	}
	//template<class GRID1, class TRANSFORM>
	//using gridtransform_view = gridpairtransform_view<GRID1, uint8_t, decltype(&discardSecond<GRID1::value_type, uint8_t, TRANSFORM>)>;

	template<class GRID1, class TRANSFORM>
	auto make_gridtransform_view(const GRID1& grid1, TRANSFORM transform)
	{
		uint8_t grid2(0);
		return make_gridpairtransform_view(grid1, grid2, discardSecond<TRANSFORM>(transform));
	}

	template<class GRID1, class TRANSFORM>
	auto make_ncgridtransform_view(GRID1& grid1, TRANSFORM transform)
	{
		uint8_t grid2(0);
		return make_gridpairtransform_view(grid1, grid2, discardSecond<TRANSFORM>(transform));
	}


	template<class T, class U>
	auto plus(T a, U b)
	{
		return a + b;
	}

	template<class T, class U>
	auto minus(T a, U b)
	{
		return a - b;
	}

	template<class T, class U>
	auto multiply(T a, U b)
	{
		return a * b;
	}

	template<class T, class U>
	auto divide(T a, U b)
	{
		return a / b;
	}

	template<class T, class U>
	auto modulo(T a, U b)
	{
		return a % b;
	}

	template<class T, class U>
	auto spaceship(T a, U b)
	{
		return a <=> b;
	}

	template<class T, class U>
	auto less(T a, U b)
	{
		return a < b;
	}

	template<class T, class U>
	auto lessEqual(T a, U b)
	{
		return a <= b;
	}

	template<class T, class U>
	auto greater(T a, U b)
	{
		return a > b;
	}

	template<class T, class U>
	auto greaterEqual(T a, U b)
	{
		return a >= b;
	}

	template<class T, class U>
	auto isEqual(T a, U b)
	{
		return a == b;
	}

	template<class T, class U>
	auto notEqual(T a, U b)
	{
		return a != b;
	}

	inline uint8_t orFunc(uint8_t a, uint8_t b)
	{
		return ((a != 0) || (b != 0)) ? uint8_t(1) : uint8_t(0);
	}

	inline uint8_t andFunc(uint8_t a, uint8_t b)
	{
		return ((a != 0) && (b != 0)) ? uint8_t(1) : uint8_t(0);
	}

	inline uint8_t notFunc(uint8_t a)
	{
		return a == 0 ? uint8_t(1) : uint8_t(0);
	}

	template<class T, class U>
	auto plusEquals(T &a, U b)
	{
		return a += b;
	}

	template<class T, class U>
	auto minusEquals(T& a, U b)
	{
		return a -= b;
	}

	template<class T, class U>
	auto multiplyEquals(T& a, U b)
	{
		return a *= b;
	}

	template<class T, class U>
	auto divideEquals(T& a, U b)
	{
		return a /= b;
	}

	template<class T, class U>
	auto moduloEquals(T& a, U b)
	{
		return a %= b;
	}

	template<class T, class U>
	auto andEquals(T& a, U b)
	{
		return a &= b;
	}

	template<class T, class U>
	auto orEquals(T& a, U b)
	{
		return a |= b;
	}

	template<class T>
	auto unaryPlus(T a)
	{
		return +a;
	}

	template<class T>
	auto unaryMinus(T a)
	{
		return -a;
	}

	template<class T>
	auto prefixIncrement(T &a)
	{
		return ++a;
	}

	template<class T>
	auto postfixIncrement(T& a)
	{
		return a++;
	}

	template<class T>
	auto prefixDecrement(T& a)
	{
		return --a;
	}

	template<class T>
	auto postfixDecrement(T& a)
	{
		return a--;
	}
	


	template<class T, class U>
	auto operator+(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, plus<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator-(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, minus<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator*(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, multiply<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator/(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, divide<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator%(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, modulo<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator<=>(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, spaceship<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator<(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, less<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator<=(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, lessEqual<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator>(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, greater<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator>=(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, greaterEqual<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator==(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, isEqual<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator!=(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, notEqual<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator||(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, orFunc);
	}
	template<class T, class U>
	auto operator&&(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		return make_gridpairtransform_view(a, b, andFunc);
	}
	template<class T>
	auto operator!(const T& a) requires(bool(IsGrid<T>))
	{
		return make_gridtransform_view(a, notFunc);
	}
	template<class T, class U>
	auto &operator+=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto temp = make_ncgridpairtransform_view(a, b, plusEquals<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}
	template<class T, class U>
	auto operator-=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto temp = make_ncgridpairtransform_view(a, b, minusEquals<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}
	template<class T, class U>
	auto operator*=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto temp = make_ncgridpairtransform_view(a, b, multiplyEquals<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}
	template<class T, class U>
	auto operator/=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto temp = make_ncgridpairtransform_view(a, b, divideEquals<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}
	template<class T, class U>
	auto operator%=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto temp = make_ncgridpairtransform_view(a, b, moduloEquals<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}
	template<class T, class U>
	auto operator&=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto temp = make_ncgridpairtransform_view(a, b, andEquals<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}
	template<class T, class U>
	auto operator|=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto temp = make_ncgridpairtransform_view(a, b, orEquals<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}
	template<class T>
	auto operator+(const T& a) requires(bool(IsGrid<T>))
	{
		return make_gridtransform_view(a, unaryPlus<decltype(getGridView(a))::value_type>);
	}
	template<class T>
	auto operator-(const T& a) requires(bool(IsGrid<T>))
	{
		return make_gridtransform_view(a, unaryMinus<decltype(getGridView(a))::value_type>);
	}
	template<class T>
	auto operator++(T& a) requires(bool(IsGrid<T>))
	{
		for (auto& elem : a)
			++a;
		return a;
		//return make_ncgridtransform_view(a, prefixIncrement<decltype(getGridView(a))::value_type>);
	}
	template<class T>
	auto operator--(T& a) requires(bool(IsGrid<T>))
	{
		for (auto& elem : a)
			--a;
		return a;
		//return make_ncgridtransform_view(a, prefixDecrement<decltype(getGridView(a))::value_type>);
	}
	template<class T>
	auto operator++(T& a, int) requires(bool(IsGrid<T>))
	{
		auto temp = a;
		for (auto& elem : a)
			a++;
		return temp;
		//return make_ncgridtransform_view(a, postfixIncrement<decltype(getGridView(a))::value_type>);
	}
	template<class T>
	auto operator--(T& a, int) requires(bool(IsGrid<T>))
	{
		auto temp = a;
		for (auto& elem : a)
			a--;
		return temp;
		//return make_ncgridtransform_view(a, postfixDecrement<decltype(getGridView(a))::value_type>);
	}
}