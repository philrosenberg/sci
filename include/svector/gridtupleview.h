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
		static const size_t NDIMS = std::max(NDIMS1, NDIMS2);
		static const size_t ndims = NDIMS;
	public:
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
	struct isGrid< gridpair_view<GRID1, GRID2>> : std::true_type {};

	template<class GRID1, class GRID2>
	struct isGrid< gridpair_view<GRID1, GRID2>*> : std::true_type {};

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
		class Iterator : public std::pair<typename GRID1::iterator, typename GRID2::iterator>
		{
		public:
			using base_type1 = typename std::ranges::iterator_t<GRID1>;
			using base_type2 = typename std::ranges::iterator_t<GRID2>;
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

		static const size_t NDIMS = std::max(NDIMS1, NDIMS2);
		static const size_t ndims = NDIMS;

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
	struct isGrid< gridpairtransform_view<GRID1, GRID2, TRANSFORM>> : std::true_type {};













	template<class GRID1, class GRID2, class TRANSFORM>
	auto make_gridpairtransform_view(GRID1 &grid1, GRID2 &grid2, TRANSFORM transform)
	{
		auto t1 = getGridView(grid1);
		return gridpairtransform_view<decltype(getGridView(grid1)), decltype(getGridView(grid2)), TRANSFORM>(getGridView(grid1), getGridView(grid2), transform);
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
	auto operator+(const T& a, const U& b) requires(bool(isGrid<std::remove_cvref_t<T>>() || isGrid<std::remove_cvref_t<U>>()))
	{
		return make_gridpairtransform_view(a, b, plus<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator-(const T& a, const U& b) requires(bool(isGrid<std::remove_cvref_t<T>>() || isGrid<std::remove_cvref_t<U>>()))
	{
		return make_gridpairtransform_view(a, b, minus<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator*(const T& a, const U& b) requires(bool(isGrid<std::remove_cvref_t<T>>() || isGrid<std::remove_cvref_t<U>>()))
	{
		return make_gridpairtransform_view(a, b, multiply<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
	template<class T, class U>
	auto operator/(const T& a, const U& b) requires(bool(isGrid<std::remove_cvref_t<T>>() || isGrid<std::remove_cvref_t<U>>()))
	{
		return make_gridpairtransform_view(a, b, divide<decltype(getGridView(a))::value_type, decltype(getGridView(b))::value_type>);
	}
}