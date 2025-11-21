#ifndef SCI_GRIDTRANSFORM_H
#define SCI_GRIDTRANSFORM_H

#include"gridview.h"
#include<tuple>
#include<assert.h>
#include"math.h"
#include"functional.h"

namespace sci
{
	template<auto TRANSFORM, class GRID1, class GRID2 >
	class gridpairtransform_view
	{
	private:
		static const size_t NDIMS1 = GRID1::ndims;
		static const size_t NDIMS2 = GRID2::ndims;

	public:
		static const size_t NDIMS = std::max(NDIMS1, NDIMS2);
		static const size_t ndims = NDIMS;

		template<auto TRANSFORM>
		class Iterator : public std::pair<typename GRID1::iterator, typename GRID2::iterator>
		{
		public:
			using base_type1 = typename std::ranges::iterator_t<GRID1>;
			using base_type2 = typename std::ranges::iterator_t<GRID2>;
			//to do - work out how to sort the return type
			using transform_return_type = decltype(TRANSFORM(*(base_type1()), *(base_type2())));
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
			Iterator(base_type1 const& b1, base_type2 const& b2)
				:std::pair<base_type1, base_type2>(b1, b2)
			{

			}


			value_type operator*() const
			{
				return value_type(TRANSFORM(*static_cast<base_type1>(this->first), *static_cast<base_type2>(this->second)));
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
				if constexpr (NDIMS1 > 0 && NDIMS2 > 0)
				{
					difference_type firstDifference = this->first - right.first;
					difference_type secondDifference = this->second - right.second;
					assert(firstDifference == secondDifference);
					return firstDifference;
				}
				else if constexpr (NDIMS1 >0)
					return this->first - right.first;
				else 
					return this->second - right.second;
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

		using iterator = Iterator<TRANSFORM>;
		using const_iterator = iterator;
		//using reference_type = iterator::reference;
		//using const_reference_type = iterator::const_reference;
		using size_type = typename iterator::size_type;
		using difference_type = typename iterator::difference_type;
		using sentinel = iterator;
		//using  reference_type = typename iterator::value_type;
		using  const_reference_type = typename const_iterator::value_type;
		using value_type = typename iterator::value_type;

		constexpr gridpairtransform_view() = default;
		constexpr gridpairtransform_view(gridpairtransform_view<TRANSFORM, GRID1, GRID2> const& rhs) = default;
		constexpr gridpairtransform_view(gridpairtransform_view<TRANSFORM, GRID1, GRID2>&& rhs) = default;
		constexpr gridpairtransform_view& operator=(gridpairtransform_view<TRANSFORM, GRID1, GRID2> const& rhs) = default;
		constexpr gridpairtransform_view& operator=(gridpairtransform_view<TRANSFORM, GRID1, GRID2>&& rhs) = default;
		constexpr gridpairtransform_view(GRID1 grid1, GRID2 grid2) requires(NDIMS1==NDIMS2 || NDIMS1 == 0 || NDIMS2 == 0)
			:m_grid1(grid1), m_grid2(grid2)
		{
			if constexpr (NDIMS1 > NDIMS2)
				m_strides = GridPremultipliedStridesReference<NDIMS1>(grid1.getStrides());
			else
				m_strides = GridPremultipliedStridesReference<NDIMS2>(grid2.getStrides());

			for(size_t i=0; i<std::min(NDIMS1, NDIMS2); ++i)
				assert(grid1.shape()[i] == grid2.shape()[i]);
		}
		~gridpairtransform_view() = default;

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
		/*reference_type operator[](const difference_type& index) requires(NDIMS == 1)
		{
			return reference_type(*(begin() + index));
		}*/
		const_reference_type operator[](const difference_type& index) const requires(NDIMS == 1)
		{
			return const_reference_type(*(begin() + index));
		}
		/*reference_type operator[](const std::array<size_t, NDIMS>& index)
		{
			return reference_type(*(begin() + m_strides.getOffset(index)));
		}*/
		const_reference_type operator[](const std::array<size_t, NDIMS>& index) const
		{
			return const_reference_type(*(begin() + m_grid1.getStrides().getOffset(index)));
		}
		/* auto operator[](const difference_type& index) requires(NDIMS != 1);
		{
			if constexpr (NDIMS1 > 0 && NDIMS2 > 0)
			{
				auto sub1 = m_grid1[index];
				auto sub2 = m_grid2[index];
				return make_gridpairtransform_view < TRANSFORM, decltype(sub1)>(sub1, sub2);
			}
			else if constexpr (NDIMS1 > 0)
			{
				auto sub1 = m_grid1[index];
				return gridpairtransform_view<TRANSFORM, decltype(sub1), decltype(m_grid2)>(sub1, m_grid2);
			}
			else if constexpr (NDIMS2 > 0)
			{
				auto sub2 = m_grid2[index];
				return gridpairtransform_view<TRANSFORM, decltype(m_grid1), decltype(sub2)>(m_grid1, sub2);
			}
			//else //operator[] for two scalars probably is nonsensical
			//{
			//	return gridpair_view<decltype(m_grid1), 0, decltype(m_grid2), 0>(m_grid1, m_grid2);
			//}
		}*/
		
		const auto operator[](const difference_type& index) const requires(NDIMS1 > 1 && NDIMS2 > 1)
		{
			auto sub1 = m_grid1[index];
			auto sub2 = m_grid2[index];
			return gridpairtransform_view<TRANSFORM, decltype(sub1), decltype(sub2)>(sub1, sub2);
		}

		const auto operator[](const difference_type& index) const requires(NDIMS1 > 1 && NDIMS2 < 2)
		{
			auto sub1 = m_grid1[index];
			return gridpairtransform_view<TRANSFORM, decltype(sub1), decltype(m_grid2)>(m_grid1[index], m_grid2);
		}

		const auto operator[](const difference_type& index) const requires(NDIMS2 > 1 && NDIMS1 < 2)
		{
			auto sub2 = m_grid2[index];
			return gridpairtransform_view<TRANSFORM, decltype(m_grid1), decltype(sub2)>(m_grid1, m_grid2[index]);
		}
		const auto operator[](const difference_type& index) const requires(NDIMS == 0)
		{

		}
		std::array<size_t, NDIMS> shape() const
		{
			if constexpr (NDIMS1 > NDIMS2)
				return m_grid1.shape();
			else
				return m_grid2.shape();
		}
		gridpairtransform_view<TRANSFORM, GRID1, GRID2>& getView()
		{
			return *this;
		}
		const gridpairtransform_view<TRANSFORM, GRID1, GRID2>& getView() const
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



	template<class T>
	auto getGridView(T& grid) requires(IsGrid<T>)
	{
		return grid.getView();
	}

	template<class T>
	auto getGridView(T& scalar) requires(!IsGrid<T>)
	{
		return scalar | views::grid<0>;
	}


	template<auto TRANSFORM, class GRID1, class GRID2>
	auto make_gridpairtransform_view(const GRID1 &grid1, const GRID2 &grid2)
	{
		return gridpairtransform_view<TRANSFORM, decltype(getGridView(grid1)), decltype(getGridView(grid2))>(getGridView(grid1), getGridView(grid2));
	}

	template<auto TRANSFORM, class GRID1, class GRID2>
	auto make_ncgridpairtransform_view(GRID1& grid1, const GRID2& grid2)
	{
		return gridpairtransform_view<TRANSFORM, decltype(getGridView(grid1)), decltype(getGridView(grid2))>(getGridView(grid1), getGridView(grid2));
	}

	/*template<auto TRANSFORM, class GRID1, class GRID2 >
	auto gridpairtransform_view<TRANSFORM, GRID1, GRID2>::operator[](const difference_type& index) requires(NDIMS != 1)
	{
		if constexpr (NDIMS1 > 0 && NDIMS2 > 0)
		{
			auto sub1 = m_grid1[index];
			auto sub2 = m_grid2[index];
			return sci::GridData<double, NDIMS-1>();
			//return make_gridpairtransform_view<TRANSFORM>(sub1, sub2);
		}
		else if constexpr (NDIMS1 > 0)
		{
			auto sub1 = m_grid1[index];
			return gridpairtransform_view<TRANSFORM, decltype(sub1), decltype(m_grid2)>(sub1, m_grid2);
		}
		else if constexpr (NDIMS2 > 0)
		{
			auto sub2 = m_grid2[index];
			return gridpairtransform_view<TRANSFORM, decltype(m_grid1), decltype(sub2)>(m_grid1, sub2);
		}
		//else //operator[] for two scalars probably is nonsensical
		//{
		//	return gridpair_view<decltype(m_grid1), 0, decltype(m_grid2), 0>(m_grid1, m_grid2);
		//}
	}*/

	template<auto TRANSFORM, class T1, class T2 >
	auto discardSecond(const T1& t, T2)
	{
		return TRANSFORM(t);
	}

	template<auto TRANSFORM, class T1, class T2 >
	auto ncDiscardSecond(T1& t, T2)
	{
		return TRANSFORM(t);
	}

	template<auto TRANSFORM, IsGrid GRID1>
	auto make_gridtransform_view(const GRID1& grid1)
	{
		uint8_t grid2(0);
		return make_gridpairtransform_view<discardSecond<TRANSFORM, typename GRID1::value_type, uint8_t>, GRID1, uint8_t>(grid1, grid2);
	}

	template<auto TRANSFORM, IsGrid GRID1>
	auto make_ncgridtransform_view(GRID1& grid1)
	{
		uint8_t grid2(0);
		return make_ncgridpairtransform_view<ncDiscardSecond<TRANSFORM, typename GRID1::value_type, uint8_t>, GRID1, uint8_t>(grid1, grid2);
	}

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
	//template<TRANSFORM_TYPE, TRANSFORM, class GRID1>
	//using gridtransform_view = gridpairtransform_view<TRANSFORM_TYPE, TRANSFORM, GRID1, uint8_t, decltype(&discardSecond<GRID1::value_type, uint8_t>)>;

	template<class T, class U>
	auto operator+(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return plus(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator-(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return minus(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator*(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return multiplies(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator/(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return divides(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator%(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return modulus(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator<=>(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return spaceship(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator<(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return less(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator<=(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return less_equal(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator>(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return greater(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator>=(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return greater_equal(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator==(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return equal_to(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator!=(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return not_equal_to(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator||(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return logical_or(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator&&(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return logical_and(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T>
	auto operator!(const T& a) requires(bool(IsGrid<T>))
	{
		auto transform = [](const typename T::value_type& x) { return logical_not(x); };
		return make_gridpairtransform_view<transform>(a);
	}

	template<class T, class U>
	auto &operator+=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto bv = getGridView(b);
		using UV = decltype(bv);
		auto transform = [](typename T::value_type& x, const typename UV::value_type& y) { return plus_assign(x, y); };
		auto temp = make_ncgridpairtransform_view<transform>(a, bv);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}

	template<class T, class U>
	auto operator-=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto bv = getGridView(b);
		using UV = decltype(bv);
		auto transform = [](typename T::value_type& x, const typename UV::value_type& y) { return minus_assign(x, y); };
		auto temp = make_ncgridpairtransform_view<transform>(a, bv);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}

	template<class T, class U>
	auto operator*=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto bv = getGridView(b);
		using UV = decltype(bv);
		auto transform = [](typename T::value_type& x, const typename UV::value_type& y) { return multiplies_assign(x, y); };
		auto temp = make_ncgridpairtransform_view<transform>(a, bv);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}

	template<class T, class U>
	auto operator/=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto bv = getGridView(b);
		using UV = decltype(bv);
		auto transform = [](typename T::value_type& x, const typename UV::value_type& y) { return divides_assign(x, y); };
		auto temp = make_ncgridpairtransform_view<transform>(a, bv);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}

	template<class T, class U>
	auto operator%=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto bv = getGridView(b);
		using UV = decltype(bv);
		auto transform = [](typename T::value_type& x, const typename UV::value_type& y) { return modulus_assign(x, y); };
		auto temp = make_ncgridpairtransform_view<transform>(a, bv);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}

	template<class T, class U>
	auto operator&=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto bv = getGridView(b);
		using UV = decltype(bv);
		auto transform = [](typename T::value_type& x, const typename UV::value_type& y) { return and_assign(x, y); };
		auto temp = make_ncgridpairtransform_view<transform>(a, bv);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}

	template<class T, class U>
	auto operator|=(T& a, const U& b) requires(bool(IsGrid<T>))
	{
		auto bv = getGridView(b);
		using UV = decltype(bv);
		auto transform = [](typename T::value_type& x, const typename UV::value_type& y) { return or_assign(x, y); };
		auto temp = make_ncgridpairtransform_view<transform>(a, bv);
		for (auto elem : temp) //this dereferences each element of a causing the assignment to happen
			;
		return a;
	}
	
	template<class T>
	auto operator+(const T& a) requires(bool(IsGrid<T>))
	{
		auto transform = [](const typename T::value_type& x) { return identity(x); };
		return make_gridpairtransform_view<transform>(a);
	}

	template<class T>
	auto operator-(const T& a) requires(bool(IsGrid<T>))
	{
		auto transform = [](const typename T::value_type& x) { return negate(x); };
		return make_gridpairtransform_view<transform>(a);
	}

	template<class T>
	auto operator++(T& a) requires(bool(IsGrid<T>))
	{
		for (auto& elem : a)
			prefix_increment(elem);
		return a;
	}

	template<class T>
	auto operator--(T& a) requires(bool(IsGrid<T>))
	{
		for (auto& elem : a)
			prefix_decrement(elem);
		return a;
	}
	
	template<class T>
	auto operator++(T& a, int) requires(bool(IsGrid<T>))
	{
		auto temp = a;
		auto aIter = a.begin();
		auto aEnd = a.end();
		auto tempIter = temp.begin();
		for (; aIter != aEnd; ++aIter, ++tempIter)
			*tempIter = postfix_increment(*aIter);
		return temp;
	}

	template<class T>
	auto operator--(T& a, int) requires(bool(IsGrid<T>))
	{
		auto temp = a;
		auto aIter = a.begin();
		auto aEnd = a.end();
		auto tempIter = temp.begin();
		for (; aIter != aEnd; ++aIter, ++tempIter)
			*tempIter = postfix_decrement(*aIter);
		return temp;
	}
	
	template<class T, class U>
	auto operator|(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return bit_or(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T, class U>
	auto operator&(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return bit_and(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	template<class T>
	auto operator~(const T& a) requires(bool(IsGrid<T>))
	{
		auto transform = [](const typename T::value_type& x) { return bit_not(x); };
		return make_gridpairtransform_view<transform>(a);
	}
	
	template<class T, class U>
	auto operator^(const T& a, const U& b) requires(bool(IsGrid<T> || IsGrid<U>))
	{
		auto av = getGridView(a);
		auto bv = getGridView(b);
		using TV = decltype(av);
		using UV = decltype(bv);
		auto transform = [](const typename TV::value_type& x, const typename UV::value_type& y) { return bit_xor(x, y); };
		return make_gridpairtransform_view<transform>(av, bv);
	}

	namespace internalGridTransform
	{
		//these functions exist because, when I pass a templated function as a template parameter to
		//make_gridtransform_view, it cannot utilise the concept correctly to determine which
		//specialization to call. I can instead pass these unambiguous functions and they in turn
		//will call the correct function.
		//For the sake of privacy I tried making these functions lamdas internal to the functions
		//referencing them, but this cause a heap space error in the compiler

		template<class T>
		auto log10(const T& t)
		{
			return sci::log10(t);
		}

		template<class T>
		auto sqrt(const T& t)
		{
			return sci::sqrt(t);
		}

		template<class T>
		auto abs(const T& t)
		{
			return sci::abs(t);
		}

		template<int POW, class T>
		auto pow(const T& t)
		{
			return sci::pow<POW>(t);
		}

		template<int ROOT, class T>
		auto root(const T& base)
		{
			return sci::root<ROOT>(base);
		}

		template<class T, class U>
		auto pow(const T& t, const U& u)
		{
			return sci::pow(t, u);
		}

		template<class T>
		auto erf(const T& value)
		{
			return sci::erf(value);
		}
	}
	
	template<IsGrid T>
	auto log10(const T& a)
	{
		return make_gridtransform_view<sci::internalGridTransform::log10<typename T::value_type>>(a);
	}

	template<IsGrid T>
	auto sqrt(const T& a)
	{
		return make_gridtransform_view<sci::internalGridTransform::sqrt<typename T::value_type>>(a);
	}

	template<IsGrid T>
	auto abs(const T& a)
	{
		return make_gridtransform_view<internalGridTransform::abs<typename T::value_type>>(a);
	}

	template<int POW, IsGrid T>
	auto pow(const T& a)
	{
		return make_gridtransform_view<sci::internalGridTransform::pow<POW, typename T::value_type>>(a);
	}

	template<int ROOT, IsGrid T>
	auto root(const T& a)
	{
		return make_gridtransform_view<sci::internalGridTransform::root<ROOT, typename T::value_type>>(a);
	}

	template<IsGrid T, IsGrid U>
	auto pow(const T& a, const U &s)
	{
		auto av = getGridView(a);
		auto sv = getGridView(s);
		using TV = decltype(av);
		using UV = decltype(sv);
		return make_gridpairtransform_view<internalGridTransform::pow<typename TV::value_type, typename UV::value_type>>(av, sv);
	}

	template<IsGrid T>
	auto erf(const T& value)
	{
		return make_gridtransform_view<internalGridTransform::erf<typename T::value_type>>(value);
	}
}

#endif