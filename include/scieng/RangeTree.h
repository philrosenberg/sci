#ifndef SCI_RANGETREE_H
#define SCI_RANGETREE_H

#include<algorithm>
#include<vector>
#include<set>
#include<map>
#include<memory>
namespace sci
{
	template<class T, class U>
	struct Boundary
	{
		T value;
		U* object;
		bool operator<(const Boundary<T, U>& other) const
		{
			return value < other.value;
		}
	};
	template<class T, class U>
	class RangeTree
	{
	public:
		template< class ITER>
		void build(ITER begin, ITER end)
		{
			//sort the ends
			std::sort(begin, end);

			//build a vector of intervals omitting duplicates and
			//a vector of sets indicating which ranges are valid between the intervals
			//the front and back elements of boundaries will never get accessed and represent -/+ infinity
			//the front and back elements of contains is an empty set
			//This ensures an out of range query finds no ranges
			std::vector<T> boundaries(std::distance(begin, end) + 1);
			boundaries[0] = 2 * begin->value - (end - 1)->value; //avoid accidentally having the -infinity element equal to the first real element by subtracting the range
			std::vector<std::set<U*>> contains(boundaries.size()-1);
			size_t index = 1; 
			for (ITER iter = begin; iter != end; ++iter)
			{
				if (iter->value == boundaries[index-1])
				{
					//duplicate boundary, apply the change to the previous contains element
					
					//try to insert the object to the contains set
					auto inserted = contains[index-1].insert(iter->object);
					//if the insert failed, it's because it was already there and this was the right edge
					if (!inserted.second)
						contains[index - 1].erase(inserted.first);
				}
				else
				{
					//new boundary, add a new contains element the same as the previous, but with the
					//next object added or removed
					contains[index] = contains[index - 1];
					//try to insert the object to the contains set
					auto inserted = contains[index].insert(iter->object);
					//if the insert failed, it's because it was already there and this was the right edge
					if (!inserted.second)
						contains[index].erase(inserted.first);
					boundaries[index] = iter->value;
					++index;
				}
			}

			boundaries[index] = 2 * (end - 1)->value - begin->value; //add the range to the last value to give the +infinity value. Not really needed, but might help debugging
			
			buildPreSorted(boundaries.begin(), boundaries.begin()+ index + 1, contains.begin()); //don't use boundaries.end() as duplicate boundaries mean we may not use the whole vector
		}
		template< class BOUNDARY_ITER, class SET_ITER>
		void buildPreSorted(const BOUNDARY_ITER begin, const BOUNDARY_ITER end, SET_ITER setBegin)
		{
			size_t distance = std::distance(begin, end);
			if (distance > 2)
			{
				BOUNDARY_ITER breakPoint = begin + distance / 2;
				SET_ITER setBreakPoint = setBegin + distance / 2;
				m_mid = *breakPoint;
				m_left.reset(new RangeTree<T, U>());
				m_right.reset(new RangeTree<T, U>());
				m_left->buildPreSorted(begin, breakPoint+1, setBegin);
				m_right->buildPreSorted(breakPoint, end, setBreakPoint);
			}
			else //distance must equal 2, it can only be 1 if the user passed in only one boundary
			{
				m_contains = *setBegin;
			}
		}
		std::set<U*> getRangesContaining(T point)
		{
			if (!m_left)
				return m_contains;
			if (point < m_mid)
				return m_left->getRangesContaining(point);
			return m_right->getRangesContaining(point);
		}
		std::set<U*> getAllRanges()
		{
			if (!m_left)
				return m_contains;
			std::set<U*> result = m_left->getAllRanges();
			result.merge(m_right->getAllRanges());
			return result;
		}
		std::set<U*> getRangesContainingLimitAndAbove(T point)
		{
			if (!m_left)
				return m_contains;

			if (point < m_mid)
			{
				std::set<U*> result = m_left->getRangesContainingLimitAndAbove( point );
				result.merge(m_right->getAllRanges());
				return result;
			}

			return m_right->getRangesContainingLimitAndAbove(point);
		}
		std::set<U*> getRangesContainingLimitAndBelow(T point)
		{
			if (!m_left)
				return m_contains;

			if (point < m_mid)
				return m_left->getRangesContainingLimitAndBelow(point);

			std::set<U*> result = m_left->getAllRanges();
			result.merge(m_right->getRangesContainingLimitAndBelow(point));
			return result;
		}
		std::set<U*> getRangesIntersectingRange(T min, T max)
		{
			if (!m_left)
				return m_contains;
			if (max < m_mid)
				return m_left->getRangesIntersectingRange(min, max);
			if (! (min < m_mid)) //avoids needing a <= or > operator
				return m_right->getRangesIntersectingRange(min, max);

			std::set<U*> result = m_left->getRangesContainingLimitAndAbove(min);
			result.merge(m_right->getRangesContainingLimitAndBelow(max));
			return result;
		}
	private:
		std::unique_ptr<RangeTree<T, U>> m_left;
		std::unique_ptr<RangeTree<T, U>> m_right;
		T m_mid;
		std::set<U*> m_contains;
	};



	
}

#endif
