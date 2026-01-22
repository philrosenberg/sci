#ifndef SCI_MULTITRANSFORM_H
#define SCI_MULTITRANSFORM_H

#include"tuple.h"
#include<assert.h>
#include"math.h"
#include"functional.h"
#include<ranges>
#include<stdexcept>

namespace sci
{
	template<auto TRANSFORM, class... CONTAINERS >
	class multitransform_view
	{
	public:
		//template<auto TRANSFORM>
		class Iterator
		{
		private:
			template<class TUPLE>
			struct IteratorTransform
			{
				using firstType = std::remove_reference_t<std::tuple_element_t<0, TUPLE>>::const_iterator;
				using remainder = IteratorTransform<sci::TailTuple_t<1, TUPLE>>;
				static const size_t size = std::tuple_size_v<TUPLE>;
			};

			template<class TUPLE>
			struct DereferenceTransform
			{
				using firstType = decltype(* std::tuple_element_t<0, TUPLE>());
				using remainder = DereferenceTransform<sci::TailTuple_t<1, TUPLE>>;
				static const size_t size = std::tuple_size_v<TUPLE>;
			};

		public:
			using containerTuple = std::tuple<const CONTAINERS&...>;
			using iterator_tuple = TransformedTuple<IteratorTransform<containerTuple>>::type;
			using reference_tuple = TransformedTuple<DereferenceTransform<iterator_tuple>>::type;

		private:
			struct Dereference
			{
			public:
				constexpr static reference_tuple values(const iterator_tuple& tuple)
				{
					return valuesInternal<0>(tuple);
				}
			private:
				template<size_t N>
				constexpr static auto valuesInternal(const iterator_tuple& tuple)
				{
					using thisType = std::tuple_element_t<N, reference_tuple>;
					if constexpr (N == std::tuple_size_v<reference_tuple> - 1)
						return std::tuple<thisType>(*(std::get<N>(tuple)));
					else if constexpr (N < std::tuple_size_v<reference_tuple>)
					{
						return std::tuple_cat(std::tuple<thisType>(*(std::get<N>(tuple))),
							valuesInternal<N+1>(tuple));
					}
				}
			};


		public:
			using base_type1 = typename std::tuple_element_t<0, iterator_tuple>;
			using transform_return_type = decltype(std::apply( TRANSFORM, std::declval<reference_tuple>()));
			//using iterator_category = std::random_access_iterator_tag;
			using value_type = transform_return_type;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using size_type = typename std::remove_reference_t<std::tuple_element_t<0, containerTuple>>::size_type;
			using difference_type = typename std::tuple_element_t<0, iterator_tuple>::difference_type;

			Iterator() = default;
			template<class... ITERS>
			Iterator(ITERS... iters)
				:m_iter{ iters... }
			{
			}
			Iterator(iterator_tuple iters)
				:m_iter(iters)
			{

			}


			value_type operator*() const
			{
				return std::apply(TRANSFORM, Dereference::values(m_iter));
				//return value_type(TRANSFORM(*(std::get<0>(m_iter)), *(std::get<1>(m_iter))));
			}
			const const_pointer operator->() const
			{
				return pointer(&(m_iter));
			}

		private:
			template<size_t N>
			void increment()
				//requires(std::input_iterator<base_type1>&& std::input_iterator<base_type2>)
			{
				if constexpr (N < std::tuple_size_v<iterator_tuple>)
				{
					++std::get<N>(m_iter);
					increment<N + 1>;
				}
			}


			template<size_t N>
			void decrement()
				//requires(std::input_iterator<base_type1>&& std::input_iterator<base_type2>)
			{
				if constexpr (N < std::tuple_size_v<iterator_tuple>)
				{
					--std::get<N>(m_iter);
					decrement<N + 1>;
				}
			}

			template<size_t N>
			void addOffset(const difference_type offset)
				//requires(std::input_iterator<base_type1>&& std::input_iterator<base_type2>)
			{
				if constexpr (N < std::tuple_size_v<iterator_tuple>)
				{
					std::get<N>(m_iter) += offset;
					addOffset<N + 1>(offset);
				}
			}
		public:

			//When we increment or move an iterator then we generally only increment the
			//elements which have dimensionality greater than zero.
			//However, if both elements have dimensionality of zero, then we do increment.
			//This is to ensure that incrementing an iterator will eventually hit end

			// Prefix increment
			Iterator& operator++()
				//requires(std::input_iterator<base_type1>&& std::input_iterator<base_type2>)
			{
				increment<0>();
				return (*this);
			}
			// Postfix increment
			Iterator operator++(int)
				//requires(std::input_iterator<base_type1>&& std::input_iterator<base_type2>)
			{
				Iterator temp = *this;
				increment<0>();
				return temp;
			}
			// Prefix decrement
			Iterator& operator--()
				//requires(std::bidirectional_iterator<base_type1> && std::bidirectional_iterator<base_type2>)
			{
				decrement<0>();
				return (*this);
			}
			// Postfix decrement
			Iterator operator--(int)
				//requires(std::bidirectional_iterator<base_type1> && std::bidirectional_iterator<base_type2>)
			{
				Iterator temp = *this;
				decrement<0>();
				return temp;
			}
			Iterator& operator+=(const difference_type offset)
				//requires(std::random_access_iterator<base_type1> && std::random_access_iterator<base_type2>)
			{
				addOffset<0>(offset);
				return *this;
			}

			Iterator operator+(const difference_type offset) const noexcept
				//requires(std::random_access_iterator<base_type1> && std::random_access_iterator<base_type2>)
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
				//requires(std::random_access_iterator<base_type1> && std::random_access_iterator<base_type2>)
			{
				return *this += -offset;
			}

			Iterator operator-(const difference_type offset) const noexcept
				//requires(std::random_access_iterator<base_type1> && std::random_access_iterator<base_type2>)
			{
				Iterator tmp = *this;
				tmp -= offset;
				return tmp;
			}

			friend Iterator operator-(const difference_type offset, const Iterator& iter) noexcept
			{
				return iter - offset;
			}

			constexpr difference_type operator-(const Iterator& right) const
			{
				return static_cast<const base_type1&>(std::get<0>(*this)) - static_cast<const base_type1&>(std::get<0>(right));
			}
			bool operator==(const Iterator& right) const noexcept
			{
				//This logic works providing we do not mix up iterators from different conainers, but as that is
				//undefined behaviour anyway, I don't think we should worry about it.
				return static_cast<const base_type1&>(std::get<0>(*this)) == static_cast<const base_type1&>(right.first);
			}
			bool operator!=(const Iterator& right) const noexcept
			{
				return !(*this == right);
			}

			bool operator<(const Iterator& right) const noexcept
			{
				//This logic works providing we do not mix up iterators from different conainers, but as that is
				//undefined behaviour anyway, I don't think we should worry about it. Also we must have these
				//operators to be reandom access iterators
				return static_cast<const base_type1&>(std::get<0>(*this)) < static_cast<const base_type1>(right.first);
			}

			bool operator>=(const Iterator& right) const noexcept
			{
				return !(*this < right);
			}

			bool operator>(const Iterator& right) const noexcept
			{
				return right <= *this;
			}

			bool operator<=(const Iterator& right) const noexcept
			{
				return !(right < *this);
			}
		private:
			iterator_tuple m_iter;
		};

		using const_iterator = Iterator;
		using iterator = const_iterator;
		using size_type = typename const_iterator::size_type;
		using difference_type = typename const_iterator::difference_type;
		using sentinel = const_iterator;
		using const_reference_type = typename const_iterator::value_type;
		using value_type = typename const_iterator::value_type;

		constexpr multitransform_view(multitransform_view<TRANSFORM, CONTAINERS...> const& rhs) = default;
		constexpr multitransform_view(multitransform_view<TRANSFORM, CONTAINERS...>&& rhs) = default;
		constexpr multitransform_view& operator=(multitransform_view<TRANSFORM, CONTAINERS...> const& rhs) = default;
		constexpr multitransform_view& operator=(multitransform_view<TRANSFORM, CONTAINERS...>&& rhs) = default;
		constexpr multitransform_view(const CONTAINERS&... containers)
			:m_containers(containers...)
		{
		}
		~multitransform_view() = default;

		private:

		private:
			struct Begin
			{
			public:
				constexpr static auto values(const iterator::containerTuple& tuple, iterator::iterator_tuple& result)
				{
					valuesInternal<0>(tuple, result);
				}
			private:
				template<size_t N>
				constexpr static void valuesInternal(const iterator::containerTuple& tuple, iterator::iterator_tuple& result)
				{
					if constexpr (N < std::tuple_size_v<typename iterator::containerTuple>)
					{
						std::get<N>(result) = std::cbegin(std::get<N>(tuple));
						valuesInternal<N + 1>(tuple, result);
					}
				}
			};
			struct End
			{
			public:
				constexpr static auto values(const iterator::containerTuple& tuple, iterator::iterator_tuple& result)
				{
					valuesInternal<0>(tuple, result);
				}
			private:
				template<size_t N>
				constexpr static void valuesInternal(const iterator::containerTuple& tuple, iterator::iterator_tuple& result)
				{
					if constexpr (N < std::tuple_size_v<typename iterator::containerTuple>)
					{
						std::get<N>(result) = std::cend(std::get<N>(tuple));
						valuesInternal<N + 1>(tuple, result);
					}
				}
			};
		public:

		const_iterator begin() const
		{
			typename iterator::iterator_tuple begins;
			Begin::values(m_containers, begins);
			return const_iterator(begins);
		}
		const_iterator cbegin() const
		{
			return begin();
		}
		sentinel end() const
		{
			typename iterator::iterator_tuple ends;
			Begin::values(m_containers, ends);
			return sentinel(ends);
		}
		sentinel cend() const
		{
			return end();
		}
		auto size() const
		{
			return std::size(std::get<0>(m_containers));
		}
		const_reference_type operator[](const difference_type& index)
		{
			return const_reference_type(*(cbegin() + index));
		}
		bool empty() const
		{
			return std::ranges::empty(std::get<0>(m_containers));
		}
	private:
		iterator::containerTuple m_containers;
	};


	template<auto TRANSFORM, class... CONTAINERS>
	auto make_multitransform_view(const CONTAINERS&... containers)
	{
		return multitransform_view<TRANSFORM, CONTAINERS...>(containers...);
	}

	/*template<size_t NDIMS>
	struct pairtransform_fn
	{
		pairtransform_fn() = default;
		{}
		template <class RANGE>
		requires std::ranges::random_access_range<RANGE>
			auto operator()(RANGE&& range) const
		{
			return grid_view<RANGE, NDIMS>{ std::forward<RANGE>(range), m_strides };
		}
		auto operator()(const GridPremultipliedStridesReference<NDIMS>& strides) const
		{
			//This operator() returns a grid_fn which uses strides. This in turn can get piped
			return grid_fn(strides);
		}
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
		pairtransforms_fn<NDIMS> gridpair;
	}*/
	//template<TRANSFORM_TYPE, TRANSFORM, class CONTAINER1>
	//using gridtransform_view = pairtransform_view<TRANSFORM_TYPE, TRANSFORM, CONTAINER1, uint8_t, decltype(&discardSecond<CONTAINER1::value_type, uint8_t>)>;

	
}

#endif