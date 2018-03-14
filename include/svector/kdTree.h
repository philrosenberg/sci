#pragma once
#include<svector/svector.h>
#include<memory>
#include<svector/serr.h>

//This kdtree class splits data in half through each dimension. When constructing each dimension
//will be split nSplit times.
//To create a tree with NDIMS dimensions create a kdtree<0, NDIMS> object. When you use the non-
//default constructor this will cascade down creating the different levels

namespace sci
{
	template<int LEVEL, int NDIMS>
	class kdtree
	{
	public:
		kdtree(const std::vector<std::vector<double>> &data, const std::vector<double> &minLevels, const std::vector<double> &maxLevels, size_t nSplits);
		kdtree(const std::vector<std::vector<double>> &data, const std::vector<double> &weights, double totalWeights, const std::vector<double> &minLevels, const std::vector<double> &maxLevels, size_t nSplits, bool permitPointSplitting);
		kdtree()
		{
			static_assert(NDIMS != 0, "Cannot create a kdtree with 0 dimensions.");
		}
		void split(size_t nSplits)
		{
			m_split1.split(nSplits);
			m_split2.split(nSplits);
		}
		void getBoundariesOfNodeContaining(const std::vector<double> &coordinates, std::vector<double> &minLevels, std::vector<double> &maxLevels) const
		{
			if (m_split1.getUpperLevel() > coordinates[LEVEL - 1])
				m_split1.getBoundariesOfNodeContaining(coordinates, minLevels, maxLevels);
			else
				m_split2.getBoundariesOfNodeContaining(coordinates, minLevels, maxLevels);
		}
		void getBoundariesOfNodeContaining(const std::vector<double> &coordinates, std::vector<double> &minLevels, std::vector<double> &maxLevels, double &totalWeight) const
		{
			if (m_split1.getUpperLevel() > coordinates[LEVEL - 1])
				m_split1.getBoundariesOfNodeContaining(coordinates, minLevels, maxLevels, totalWeight);
			else
				m_split2.getBoundariesOfNodeContaining(coordinates, minLevels, maxLevels, totalWeight);
		}
		void getAllBoundaries(std::vector<std::vector<double>> &minLevels, std::vector<std::vector<double>> &maxLevels) const
		{
			m_split1.getAllBoundaries(minLevels, maxLevels);
			m_split2.getAllBoundaries(minLevels, maxLevels);
		}
		void getAllBoundaries(std::vector<std::vector<double>> &minLevels, std::vector<std::vector<double>> &maxLevels, std::vector<double> &totalWeights) const
		{
			m_split1.getAllBoundaries(minLevels, maxLevels, totalWeights);
			m_split2.getAllBoundaries(minLevels, maxLevels, totalWeights);
		}
		const std::vector<std::vector<double>> &getPointsInNodeContaining(const std::vector<double> &coordinates) const
		{
			if (m_split1.getUpperLevel() > coordinates[LEVEL - 1])
				return m_split1.getPointsInNodeContaining(coordinates);
			else
				return m_split2.getPointsInNodeContaining(coordinates);
		}
		void getPointsInNodeContaining(const std::vector<double> &coordinates, std::vector<std::vector<double>> &data, std::vector<double> &weights) const
		{
			if (m_split1.getUpperLevel() > coordinates[LEVEL - 1])
				m_split1.getPointsInNodeContaining(coordinates, data, weights);
			else
				m_split2.getPointsInNodeContaining(coordinates, data, weights);
		}
		double getUpperLevel()const
		{
			static_assert(NDIMS != LEVEL, "kdtree::getUpperLevel can only be called when NDIMS!=LEVEL");
			return m_maxLevel;
		}
		double getLowerLevel()const
		{
			static_assert(NDIMS != LEVEL, "kdtree::getLowerLevel can only be called when NDIMS!=LEVEL");
			return m_minLevel;
		}
	private:
		kdtree<LEVEL - 1, NDIMS> m_split1;
		kdtree<LEVEL - 1, NDIMS> m_split2;
		double m_maxLevel;
		double m_minLevel;
	};

	template<int LEVEL, int NDIMS>
	kdtree<LEVEL, NDIMS>::kdtree(const std::vector<std::vector<double>> &data, const std::vector<double> &minLevels, const std::vector<double> &maxLevels, size_t nSplits)
	{
		static_assert(NDIMS != 0, "Cannot create a kdtree with 0 dimensions.");

		//deal with edge cases first where we have only 1 or zero points passed in
		if (data[LEVEL - 1].size() == 1)
		{
			std::vector<double> split1maxLevels = maxLevels;
			std::vector<double> split2minLevels = minLevels;
			split2minLevels[LEVEL - 1] = maxLevels[LEVEL - 1];
			m_split1 = kdtree<LEVEL - 1, NDIMS>(data, minLevels, split1maxLevels, nSplits);
			m_split2 = kdtree<LEVEL - 1, NDIMS>(std::vector<std::vector<double>> (NDIMS), split2minLevels, maxLevels, nSplits);
			return;
		}
		if (data[LEVEL - 1].size() == 0)
		{
			std::vector<double> split1maxLevels = maxLevels;
			std::vector<double> split2minLevels = minLevels;
			split1maxLevels[LEVEL - 1] = split2minLevels[LEVEL - 1] = (maxLevels[LEVEL - 1] + minLevels[LEVEL-1] ) /2.0;
			m_split1 = kdtree<LEVEL - 1, NDIMS>(data, minLevels, split1maxLevels, nSplits);
			m_split2 = kdtree<LEVEL - 1, NDIMS>(data, split2minLevels, maxLevels, nSplits);
			return;
		}

		std::vector<std::vector<double>> sorted1(NDIMS);
		std::vector<std::vector<double>> sorted2(NDIMS);

		//sort the data based on column LEVEL and split it in half
		std::vector<size_t> newLocations;
		std::vector<std::vector<double>> sortedData(NDIMS);
		sci::sort(data[LEVEL - 1], sortedData[LEVEL-1], newLocations);
		for(size_t i=0; i<NDIMS; ++i)
			if(i!=LEVEL-1)
				sortedData[i]=sci::reorder(data[i], newLocations);
		for (size_t i = 0; i < NDIMS; ++i)
		{
			sorted1[i] = std::vector<double>(sortedData[i].begin(), sortedData[i].begin() + sortedData[i].size() / 2);
			sorted2[i] = std::vector<double>(sortedData[i].begin() + sortedData[i].size() / 2, sortedData[i].end());
		}
		if (LEVEL == NDIMS) //tree node with LEVEL==NDIMS never need to know its own range - because there was no split into this node it doesn't make sense
		{
			m_maxLevel = std::numeric_limits<double>::infinity();
			m_minLevel = -std::numeric_limits<double>::infinity();
		}
		else
		{
			m_maxLevel = maxLevels[LEVEL];
			m_minLevel = minLevels[LEVEL];
		}
		std::vector<double> split1maxLevels = maxLevels;
		std::vector<double> split2minLevels = minLevels;
		//we don't need to worry about a case where one side has no points, we dealt with that earlier
		split1maxLevels[LEVEL - 1] = split2minLevels[LEVEL - 1] = (sorted1[LEVEL - 1].back() + sorted2[LEVEL - 1].front()) / 2.0;
		

		//pass the split data to the next level down;
		m_split1 = kdtree<LEVEL - 1, NDIMS>(sorted1, minLevels, split1maxLevels, nSplits);
		m_split2 = kdtree<LEVEL - 1, NDIMS>(sorted2, split2minLevels, maxLevels, nSplits);
	}

	template<int LEVEL, int NDIMS>
	kdtree<LEVEL, NDIMS>::kdtree(const std::vector<std::vector<double>> &data, const std::vector<double> &weights, double totalWeight, const std::vector<double> &minLevels, const std::vector<double> &maxLevels, size_t nSplits, bool permitPointSplitting)
	{
		static_assert(NDIMS != 0, "Cannot create a kdtree with 0 dimensions.");

		//deal with edge cases first where we have only 1 or zero points passed in
		if (!permitPointSplitting)
		{
			if (data[LEVEL - 1].size() == 1)
			{
				std::vector<double> split1maxLevels = maxLevels;
				std::vector<double> split2minLevels = minLevels;
				split2minLevels[LEVEL - 1] = maxLevels[LEVEL - 1];
				m_split1 = kdtree<LEVEL - 1, NDIMS>(data, minLevels, split1maxLevels, nSplits);
				m_split2 = kdtree<LEVEL - 1, NDIMS>(std::vector<std::vector<double>>(NDIMS), split2minLevels, maxLevels, nSplits);
				return;
			}
			if (data[LEVEL - 1].size() == 0)
			{
				std::vector<double> split1maxLevels = maxLevels;
				std::vector<double> split2minLevels = minLevels;
				split1maxLevels[LEVEL - 1] = split2minLevels[LEVEL - 1] = (maxLevels[LEVEL - 1] + minLevels[LEVEL - 1]) / 2.0;
				m_split1 = kdtree<LEVEL - 1, NDIMS>(data, minLevels, split1maxLevels, nSplits);
				m_split2 = kdtree<LEVEL - 1, NDIMS>(data, split2minLevels, maxLevels, nSplits);
				return;
			}
		}

		std::vector<std::vector<double>> sorted1(NDIMS);
		std::vector<std::vector<double>> sorted2(NDIMS);

		//sort the data based on column LEVEL and split it in half
		std::vector<size_t> newLocations;
		std::vector<std::vector<double>> sortedData(NDIMS);
		sci::sort(data[LEVEL - 1], sortedData[LEVEL-1], newLocations);
		for (size_t i = 0; i < sortedData.size(); ++i)
			if (i != LEVEL-1)
				sortedData[i] = sci::reorder(data[i], newLocations);
		std::vector<double> sortedWeights = sci::reorder(weights, newLocations);

		//find the point which splits the weights in half
		size_t splitIndex = 0;
		double sumWeight = 0.0;
		double lowerWeight = 0.0;
		double upperWeight = 0.0;
		for (size_t i = 0; i < data[LEVEL - 1].size(); ++i)
		{
			sumWeight += sortedWeights[i];
			if (sumWeight > totalWeight / 2.0)
			{
				splitIndex = i;
				break;
			}
		}
		//if we are not permitting point splitting then ensure we have at least
		//one point on either side of the split.
		if (!permitPointSplitting)
		{
			splitIndex = std::max(splitIndex, (size_t)1);
		}
		upperWeight = sumWeight - totalWeight / 2.0;
		lowerWeight = sortedWeights[splitIndex] - upperWeight;

		//split the arrays in two, note if point splittting is permitted then we duplicate the point
		//at the split and divide the weights between the dublicated points
		for (size_t i = 0; i < NDIMS; ++i)
		{
			sorted1[i] = std::vector<double>(sortedData[i].begin(), sortedData[i].begin() + splitIndex + (permitPointSplitting ? 1 : 0));
			sorted2[i] = std::vector<double>(sortedData[i].begin() + splitIndex, sortedData[i].end());
		}
		//split the weights, note that there will be one overlapping point if we permit point splitting
		std::vector<double> weights1 = std::vector<double>(sortedWeights.begin(), sortedWeights.begin() + splitIndex + (permitPointSplitting ? 1 : 0));
		std::vector<double> weights2 = std::vector<double>(sortedWeights.begin() + splitIndex, sortedWeights.end());
		//split the weight of the overlapping point
		if (permitPointSplitting)
		{
			weights1.back() = lowerWeight;
			weights2.front() = upperWeight;
		}

		//record the max/min level
		if (LEVEL == NDIMS) //tree node with LEVEL==NDIMS never need to know its own range - because there was no split into this node it doesn't make sense
		{
			m_maxLevel = std::numeric_limits<double>::infinity();
			m_minLevel = -std::numeric_limits<double>::infinity();
		}
		else
		{
			m_maxLevel = maxLevels[LEVEL];
			m_minLevel = minLevels[LEVEL];
		}

		//if the user passed in all integers, then we can end up with the last weight
		//of weights1 being zero. In this case, just remove this point.
		//Note it should not be possible to end up with zero points if we do this, unless
		//the user passed in weights of all zeros, in which case this would have been
		//caught previously;
		if ( permitPointSplitting && weights1.back() == 0.0)
		{
			weights1.resize(weights1.size() - 1);
			for (size_t i = 0; i < sorted1.size(); ++i)
				sorted1[i].resize(sorted1[i].size() - 1);
		}

		std::vector<double> split1maxLevels = maxLevels;
		std::vector<double> split2minLevels = minLevels;
		//calculating the split level this way works whether or not re removed the last point of sorted1
		split1maxLevels[LEVEL - 1] = split2minLevels[LEVEL - 1] = (sorted1[LEVEL-1].back()+sorted2[LEVEL-1].front())/2.0;

		double totalWeight1;
		double totalWeight2;
		if(permitPointSplitting)
			totalWeight1 = totalWeight2 = totalWeight / 2.0;
		else
		{
			totalWeight1 = totalWeight/2.0 - lowerWeight;
			totalWeight2 = totalWeight / 2.0 + lowerWeight;
		}

		//pass the split data to the next level down;
		m_split1 = kdtree<LEVEL - 1, NDIMS>(sorted1, weights1, totalWeight1, minLevels, split1maxLevels, nSplits, permitPointSplitting);
		m_split2 = kdtree<LEVEL - 1, NDIMS>(sorted2, weights2, totalWeight2, split2minLevels, maxLevels, nSplits, permitPointSplitting);
	}

	template<int NDIMS>
	class kdtree<0, NDIMS>
	{
	public:
		kdtree(const std::vector<std::vector<double>> &data, const std::vector<double> &minLevels, const std::vector<double> &maxLevels, size_t nSplits)
		{
			static_assert(NDIMS != 0, "Cannot create a kdtree with 0 dimensions.");
			sci::assertThrow(data.size() == NDIMS, sci::err(SERR_ANALYSIS, -9999, "A kdtree must be created with a number of arrays equal to the NDIMS parameter."));
			for (size_t i = 1; i < data.size(); ++i)
				sci::assertThrow(data[i].size() == data[0].size(), sci::err(SERR_ANALYSIS, -9999, "All data arrays passed into a kdtree must have the same length."));
			m_split = nullptr;
			m_data = data;
			m_maxLevels = maxLevels;
			m_minLevels = minLevels;
			m_minLevel = minLevels[0];
			m_maxLevel = maxLevels[0];
			m_nSplits = 0;
			m_permitPointSplitting = false;
			split(nSplits);
		}
		kdtree(const std::vector<std::vector<double>> &data, const std::vector<double> &weights, double totalWeight, const std::vector<double> &minLevels, const std::vector<double> &maxLevels, size_t nSplits, bool permitPointSplitting)
		{
			static_assert(NDIMS != 0, "Cannot create a kdtree with 0 dimensions.");
			sci::assertThrow(data.size() == NDIMS, sci::err(SERR_ANALYSIS, -9999, "A kdtree must be created with a number of arrays equal to the NDIMS parameter."));
			for (size_t i = 1; i < data.size(); ++i)
				sci::assertThrow(data[i].size() == data[0].size(), sci::err(SERR_ANALYSIS, -9999, "All data arrays passed into a kdtree must have the same length."));
			m_split = nullptr;
			m_data = data;
			m_maxLevels = maxLevels;
			m_minLevels = minLevels;
			m_minLevel = minLevels[0];
			m_maxLevel = maxLevels[0];
			m_nSplits = 0;
			m_weights = weights;
			m_totalWeight = totalWeight;
			m_permitPointSplitting = permitPointSplitting;
			split(nSplits);
		}
		kdtree(const std::vector<std::vector<double>> &data, const std::vector<double> &weights, const std::vector<double> &minLevels, const std::vector<double> &maxLevels, size_t nSplits, bool permitPointSplitting)
		{
			static_assert(NDIMS != 0, "Cannot create a kdtree with 0 dimensions.");
			sci::assertThrow(data.size() == NDIMS, sci::err(SERR_ANALYSIS, -9999, "A kdtree must be created with a number of arrays equal to the NDIMS parameter."));
			for (size_t i = 1; i < data.size(); ++i)
				sci::assertThrow(data[i].size() == data[0].size(), sci::err(SERR_ANALYSIS, -9999, "All data arrays passed into a kdtree must have the same length."));
			m_split = nullptr;
			m_data = data;
			m_maxLevels = maxLevels;
			m_minLevels = minLevels;
			m_minLevel = minLevels[0];
			m_maxLevel = maxLevels[0];
			m_nSplits = 0;
			m_weights = weights;
			m_totalWeight = sci::sum(weights);
			sci::assertThrow(sci::allTrue(weights >= 0.0), sci::err(SERR_ANALYSIS, -9999, "Negative weights or nan weights in kdtree data are not permited"));
			sci::assertThrow(m_totalWeight > 0.0 && m_totalWeight < std::numeric_limits<double>::infinity(), sci::err(SERR_ANALYSIS, -9999, "The total weight of all data passed into a kdtree must be non-zero and not infinite."));
			m_permitPointSplitting = permitPointSplitting;
			split(nSplits);
		}
		kdtree()
		{
			static_assert(NDIMS != 0, "Cannot create a kdtree with 0 dimensions.");
			m_split = nullptr;
			m_data = std::vector<std::vector<double>>(NDIMS);
			m_maxLevels = std::vector<double>(NDIMS, std::numeric_limits<double>::infinity());
			m_minLevels = std::vector<double>(NDIMS, -std::numeric_limits<double>::infinity());
			m_maxLevel = std::numeric_limits<double>::infinity();
			m_minLevel = -std::numeric_limits<double>::infinity();
			m_nSplits = 0;
			m_totalWeight = 0;
			m_permitPointSplitting = false;
		}
		void split(size_t nSplits)
		{
			//we can stop!
			if (nSplits == 0)
				return;

			//record how many splits there are from here on
			m_nSplits += nSplits;

			//create more levels below this one - the levels will iterate through until nSplits reaches 0;
			if (!m_split)
			{
				if( m_weights.size()>0)
					m_split.reset(new kdtree<NDIMS, NDIMS>(m_data, m_weights, m_totalWeight, m_minLevels, m_maxLevels, nSplits - 1, m_permitPointSplitting));
				else
					m_split.reset(new kdtree<NDIMS, NDIMS>(m_data, m_minLevels, m_maxLevels, nSplits - 1));
			}
			//if we have already split then just pass it down
			else
			{
				m_split->split(nSplits);
				m_data.resize(0);
				m_data.shrink_to_fit();
			}
		}
		void getBoundariesOfNodeContaining(const std::vector<double> &coordinates, std::vector<double> &minLevels, std::vector<double> &maxLevels) const
		{
			if (m_split)
				m_split->getBoundariesOfNodeContaining(coordinates, minLevels, maxLevels);
			else
			{
				minLevels = m_minLevels;
				maxLevels = m_maxLevels;
			}
		}
		void getBoundariesOfNodeContaining(const std::vector<double> &coordinates, std::vector<double> &minLevels, std::vector<double> &maxLevels, double &totalWeight) const
		{
			if (m_split)
				m_split->getBoundariesOfNodeContaining(coordinates, minLevels, maxLevels, totalWeight);
			else
			{
				minLevels = m_minLevels;
				maxLevels = m_maxLevels;
				totalWeight = sci::sum<double>(m_weights);
			}
		}
		void getAllBoundaries(std::vector<std::vector<double>> &minLevels, std::vector<std::vector<double>> &maxLevels) const
		{
			minLevels.reserve(std::pow(2, NDIMS*m_nSplits));
			maxLevels.reserve(std::pow(2, NDIMS*m_nSplits));
			if (m_split)
				m_split->getAllBoundaries(minLevels, maxLevels);
			else
			{
				minLevels.push_back(m_minLevels);
				maxLevels.push_back(m_maxLevels);
			}
		}
		void getAllBoundaries(std::vector<std::vector<double>> &minLevels, std::vector<std::vector<double>> &maxLevels, std::vector<double> &totalWeights) const
		{
			minLevels.reserve(std::pow(2, NDIMS*m_nSplits));
			maxLevels.reserve(std::pow(2, NDIMS*m_nSplits));
			totalWeights.reserve(std::pow(2, NDIMS*m_nSplits));
			if (m_split)
				m_split->getAllBoundaries(minLevels, maxLevels, totalWeights);
			else
			{
				minLevels.push_back(m_minLevels);
				maxLevels.push_back(m_maxLevels);
				totalWeights.push_back(sci::sum<double>(m_weights));
			}
		}
		const std::vector<std::vector<double>> &getPointsInNodeContaining(const std::vector<double> &coordinates) const
		{
			if (m_split)
				return m_split->getPointsInNodeContaining(coordinates);
			else
			{
				return m_data;
			}
		}
		void getPointsInNodeContaining(const std::vector<double> &coordinates, std::vector<std::vector<double>> &data, std::vector<double> &weights) const
		{
			if (m_split)
				return m_split->getPointsInNodeContaining(coordinates);
			else
			{
				data = m_data;
				weights = m_weights;
			}
		}
		double getUpperLevel()const
		{
			return m_maxLevel;
		}
		double getLowerLevel()const
		{
			return m_minLevel;
		}
	private:
		std::vector<std::vector<double>> m_data;
		std::unique_ptr<kdtree<NDIMS, NDIMS>> m_split;
		std::vector<double> m_maxLevels;
		std::vector<double> m_minLevels;
		double m_maxLevel;
		double m_minLevel;
		size_t m_nSplits;
		std::vector<double> m_weights;
		double m_totalWeight;
		bool m_permitPointSplitting;
	};
}
