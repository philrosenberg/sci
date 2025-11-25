#ifndef SVECTOR_PLOTPLOTABLE_H
#define SVECTOR_PLOTPLOTABLE_H

#include<vector>
#include<string>
#include<memory>


#include"../grid.h"
#include"../gridtransformview.h"
#include"elements.h"


namespace sci
{
	namespace plot
	{
		const int plotDataErrorCode = 1;
		class splotTransformer;

		//This class is the base class for all plotable items, it provides the interface for
		//the items
		class PlotableItem : public DrawableItem
		{
		public:
			PlotableItem()
				:m_transformer(nullptr), m_scaledAxes(false)
			{
				m_xAxis.push_back(nullptr);
				m_yAxis.push_back(nullptr);
			}
			PlotableItem(std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<splotTransformer> transformer)
				:m_transformer(transformer), m_scaledAxes(false), m_intersection(xAxis->getStart().getX(), yAxis->getStart().getY())
			{
				m_xAxis.push_back(xAxis);
				m_yAxis.push_back(yAxis);
				//a note about intersection
				//In theory, two axes do not need to be horizontal and vertical and their start points do not need to be the same.
				//In this case, you might follow a line from the start of each axis, parallel to the other and the intersection point
				//is the start corner of the plot area.
				//However, in general this intersection cannot be represented by a Point as it is a nonlinear combination of the
				//absolute position and the positions scaled by the height and width. Hence we don't do this.
				//Instead, we find the intersection as the point up from the start of the x axis and across from the start of the y
				//axis. This point can be represented by a Point and hence will scale properly.
			}
			virtual ~PlotableItem()
			{
			}
			constexpr size_t getNAxesSets() const
			{
				return m_xAxis.size();
			}
			void preDraw() override
			{
				for(size_t i=0; i< getNAxesSets(); ++i)
					autoscaleAxes(i);
				m_scaledAxes = true;
			}
			bool readyToDraw() const override
			{
				return m_scaledAxes;
			}
			void draw(Renderer& renderer, perMillimetre scale) override
			{
				for (size_t i = 0; i < m_xAxis.size(); ++i)
				{
					Point endCorner(m_xAxis[i]->getEnd().getX(), m_yAxis[i]->getEnd().getY());
					sci::graphics::Clipper clipper = renderer.addClippingRegion(m_intersection, endCorner);

					//plot the data
					plotData(i, renderer, scale);
				}
			}

			Point getPointFromLinearData(double x, double y, size_t axisSetIndex) const
			{
				return m_intersection + m_xAxis[axisSetIndex]->alongAxisDistanceFromLinearData(x) + m_yAxis[axisSetIndex]->alongAxisDistanceFromLinearData(y);
			}

			Point getPointFromLoggedIfNeededData(double x, double y, size_t axisSetIndex) const
			{
				return m_intersection + m_xAxis[axisSetIndex]->alongAxisDistanceFromLoggedIfNeededData(x) + m_yAxis[axisSetIndex]->alongAxisDistanceFromLoggedIfNeededData(y);
			}
			std::shared_ptr<Axis> getXAxis(size_t axisSetIndex)
			{
				return m_xAxis[axisSetIndex];
			}
			std::shared_ptr<Axis> getYAxis(size_t axisSetIndex)
			{
				return m_yAxis[axisSetIndex];
			}
			std::shared_ptr<const Axis> getXAxis(size_t axisSetIndex) const
			{
				return m_xAxis[axisSetIndex];
			}
			std::shared_ptr<const Axis> getYAxis(size_t axisSetIndex) const
			{
				return m_yAxis[axisSetIndex];
			}
		private:
			virtual void autoscaleAxes(size_t axisSetIndex) = 0;
			virtual void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const {}
			std::vector<std::shared_ptr<Axis>> m_xAxis;
			std::vector<std::shared_ptr<Axis>> m_yAxis;
			std::shared_ptr<splotTransformer> m_transformer;
			bool m_scaledAxes;
			Point m_intersection;
		};

		//This class can be used as a base class for most plotable data sets. It holds multiple 1d or 2d
		//data sets. The class types holding each data set are passed as parameter packs. Typically
		//the data would be stored as std::vector<double>, sci::GridData<double, 1> or
		//sci::GridData<double, 2>, but in theory any container class can be used which has size
		//and assign methods
		//This function stores both linear and logged versions of the data and the getData<dimension>()
		//method will select the correct one based upon the axis being plotted against.
		//This class is separated from plotable as there may be some cases where the data storage model
		//does not work and something more custom is needed, so this separates the concerns of rendering
		//and storage.
		template<class... CONTAINERS>
		class Data : public PlotableItem
		{
		public:
			using containersTuple = std::tuple<CONTAINERS...>;
			constexpr static int nDimensions = sizeof...(CONTAINERS);

			template< class... RECEIVEDCONTAINERS>
			Data(std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::array<std::shared_ptr<Scale>, nDimensions> axes, std::shared_ptr<splotTransformer> transformer, RECEIVEDCONTAINERS... data)
				:PlotableItem(xAxis, yAxis, transformer)
			{
				static_assert(sizeof...(RECEIVEDCONTAINERS) == nDimensions, "The number of containers passed in to sci::plot::Data must match the expected number");
				m_axes.push_back(axes);
				copyLinear(data...);
				calculateLog<0>();
			}
			virtual ~Data()
			{
			}
			virtual void autoscaleAxes(size_t axisSetIndex) override
			{
				autoscaleAxesRecursive<0>(axisSetIndex);
			}
			template<int dimension>
			constexpr const auto& getData(size_t setIndex) const
			{
				return m_axes[setIndex][dimension]->isLog() ? std::get<dimension>(m_dataLogged) : std::get<dimension>(m_data);
			}
			constexpr bool isLog(size_t setIndex, size_t dimension) const
			{
				return m_axes[setIndex][dimension]->isLog();
			}
			constexpr bool hasData() const
			{
				return getNPoints() > 0;
			}
			constexpr size_t getNPoints() const
			{
				if constexpr (nDimensions == 0)
					return 0;
				else
					return std::get<0>(m_data).size();
			}
			constexpr size_t getNDimensions() const
			{
				return nDimensions;
			}
			constexpr const std::array<std::shared_ptr<Scale>, nDimensions>& getScales(size_t setIndex)
			{
				return m_axes[setIndex];
			}
		private:
			template<class... REMAINING>
			void copyLinear(const auto &next, REMAINING... remaining)
			{
				constexpr int index = nDimensions - sizeof...(REMAINING) - 1;
				auto& linear = std::get<index>(m_data);
				linear.assign(next.begin(), next.end());
				if constexpr ( index != nDimensions - 1)
					copyLinear(remaining...);
			}
			template<class... REMAINING>
			void copyLinear(const sci::GridData<double, 2>& next, REMAINING... remaining)
			{
				//Need to specialize this for grids as there is no assign method for
				// multi dimensional grids
				constexpr int index = nDimensions - sizeof...(REMAINING) - 1;
				auto& linear = std::get<index>(m_data);
				linear = next;
				if constexpr (index != nDimensions - 1)
					copyLinear(remaining...);
			}
			template<int index>
			void calculateLog()
			{
				auto& linear = std::get<index>(m_data);
				auto& log = std::get<index>(m_dataLogged);
				log = linear;
				for (auto& v : log)
					v = sci::log10(v);
				if constexpr (index != nDimensions - 1)
					calculateLog<index + 1>();
			}
			template <int index>
			void autoscaleAxesRecursive(size_t setIndex)
			{
				if(m_axes[setIndex][index])
					m_axes[setIndex][index]->expand(std::get<index>(m_data));
				if constexpr (index != nDimensions - 1)
					autoscaleAxesRecursive<index + 1>(setIndex);
			}
			containersTuple m_data;
			containersTuple m_dataLogged;
			std::vector<std::array<std::shared_ptr<Scale>, nDimensions>> m_axes;
		};
	}
}


#endif
