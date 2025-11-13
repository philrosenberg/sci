#ifndef SVECTOR_PLOTPLOTABLE_H
#define SVECTOR_PLOTPLOTABLE_H

#include<vector>
#include<string>
#include<memory>


#include<svector/grid.h>
#include<svector/gridtransformview.h>
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
				:m_xAxis(nullptr), m_yAxis(nullptr), m_transformer(nullptr), m_scaledAxes(false)
			{
			}
			PlotableItem(std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<splotTransformer> transformer)
				:m_xAxis(xAxis), m_yAxis(yAxis), m_transformer(transformer), m_scaledAxes(false), m_intersection(xAxis->getStart().getX(), yAxis->getStart().getY())
			{
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
			void preDraw() override
			{
				autoscaleAxes();
				m_scaledAxes = true;
			}
			bool readyToDraw() const override
			{
				return m_scaledAxes;
			}
			void draw(Renderer& renderer, grPerMillimetre scale) override
			{
				Point endCorner(m_xAxis->getEnd().getX(), m_yAxis->getEnd().getY());
				sci::graphics::Clipper clipper = renderer.addClippingRegion(m_intersection, endCorner);

				//plot the data
				plotData(renderer, scale);
			}

			Point getPointFromLinearData(double x, double y) const
			{
				return m_intersection + m_xAxis->alongAxisDistanceFromLinearData(x) + m_yAxis->alongAxisDistanceFromLinearData(y);
			}

			Point getPointFromLoggedIfNeededData(double x, double y) const
			{
				return m_intersection + m_xAxis->alongAxisDistanceFromLoggedIfNeededData(x) + m_yAxis->alongAxisDistanceFromLoggedIfNeededData(y);
			}
		private:
			virtual void autoscaleAxes() = 0;
			virtual void plotData(Renderer& renderer, grPerMillimetre scale) const {};
			std::shared_ptr<Axis> m_xAxis;
			std::shared_ptr<Axis> m_yAxis;
			std::shared_ptr<splotTransformer> m_transformer;
			bool m_scaledAxes;
			Point m_intersection;
		};

		//This class can be used as a base class for plotable data sets. It holds multiple 1d or 2d
		//data sets. The class types holding each data set are passed as parameter packs. Typically
		//the data would be stored as std::vector<double>, sci::GridData<double, 1> or
		//sci::GridData<double, 2>, but in theory any container class can be used which has size
		//and assign methods
		//This function stores both linear and logged versions of the data and the getData<dimension>()
		//method will select the correct one based upon the axis being plotted against.
		template<class... CONTAINERS>
		class Data : virtual public PlotableItem
		{
		public:
			using containersTuple = std::tuple<CONTAINERS...>;
			constexpr static int nDimensions = sizeof...(CONTAINERS);

			template< class... RECEIVEDCONTAINERS>
			Data(std::array<std::shared_ptr<Scale>, nDimensions> axes, std::shared_ptr<splotTransformer> transformer, RECEIVEDCONTAINERS... data)
				:m_axes(axes)
			{
				static_assert(sizeof...(RECEIVEDCONTAINERS) == nDimensions, "The number of containers passed in to sci::plot::Data must match the expected number");
				copyLinear(data...);
				calculateLog<0>();
			}
			virtual ~Data()
			{
			}
			virtual void autoscaleAxes() override
			{
				autoscaleAxesRecursive<0>();
			}
			template<int dimension>
			constexpr const auto& getData() const
			{
				return m_axes[dimension]->isLog() ? std::get<dimension>(m_dataLogged) : std::get<dimension>(m_data);
			}
			constexpr bool isLog(size_t dimension) const
			{
				return m_axes[dimension]->isLog();
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
			constexpr const std::array<std::shared_ptr<Scale>, nDimensions>& getAxes()
			{
				return m_axes;
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
			void autoscaleAxesRecursive()
			{
				if(m_axes[index])
					m_axes[index]->expand(std::get<index>(m_data));
				if constexpr (index != nDimensions - 1)
					autoscaleAxesRecursive<index + 1>();
			}
			containersTuple m_data;
			containersTuple m_dataLogged;
			std::array<std::shared_ptr<Scale>, nDimensions> m_axes;
		};
	}
}


#endif
