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
			virtual ~PlotableItem() {}
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
			void copyLinear(auto next, REMAINING... remaining)
			{
				constexpr int index = nDimensions - sizeof...(REMAINING) - 1;
				auto& linear = std::get<index>(m_data);
				linear.assign(next.begin(), next.end());
				if constexpr ( index != nDimensions - 1)
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

		class Bars : virtual public PlotableItem
		{
		public:
			Bars(std::span<const double> alongs, std::span<const double> acrosses, std::span<const double> plusErrors, std::span<const double> minusErrors, std::shared_ptr<Axis> alongAxis, std::shared_ptr<Axis> acrossAxis, bool useForAutoscale = true, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(alongAxis, acrossAxis, transformer), m_acrosses(acrosses.begin(), acrosses.end()), m_alongs(alongs.begin(), alongs.end()),
				m_alongAxis(alongAxis), m_acrossAxis(acrossAxis)
			{
				m_minAlongs.resize(alongs.size());
				m_maxAlongs.resize(alongs.size());
				for (size_t i = 0; i < alongs.size(); ++i)
				{
					m_minAlongs[i] = alongs[i] - minusErrors[i];
					m_maxAlongs[i] = alongs[i] + plusErrors[i];
				}

				m_acrossesLogged = m_acrosses;
				m_alongsLogged = m_alongs;
				m_minAlongsLogged = m_minAlongs;
				m_maxAlongsLogged = m_maxAlongs;
				for (auto& v : m_acrossesLogged)
					v = sci::log10(v);
				for (auto& v : m_alongsLogged)
					v = sci::log10(v);
				for (auto& v : m_minAlongsLogged)
					v = sci::log10(v);
				for (auto& v : m_maxAlongsLogged)
					v = sci::log10(v);

				m_useForAutoscale = useForAutoscale;
			}
			size_t getNPoints() const
			{
				return m_acrosses.size();
			}
			bool hasData() const
			{
				return getNPoints() > 0;
			}
			virtual void autoscaleAxes() override
			{
				if (!m_useForAutoscale)
					return;

				if (m_alongAxis && m_alongAxis->isAutoscale())
				{
					for (auto& x : m_minAlongs)
						m_alongAxis->expand(x);
					for (auto& x : m_maxAlongs)
						m_alongAxis->expand(x);
				}
				if (m_acrossAxis && m_acrossAxis->isAutoscale())
				{
					for (auto& y : m_acrosses)
						m_acrossAxis->expand(y);
				}
			}
			const std::vector<double>& getAlongData() const
			{
				return m_alongAxis->isLog() ? m_alongsLogged : m_alongs;
			}
			const std::vector<double>& getAcrossData() const
			{
				return m_acrossAxis->isLog() ? m_acrossesLogged : m_acrosses;
			}
			const std::vector<double>& getMinAlongData() const
			{
				return m_alongAxis->isLog() ? m_minAlongsLogged : m_minAlongs;
			}
			const std::vector<double>& getMaxAlongData() const
			{
				return m_alongAxis->isLog() ? m_maxAlongsLogged : m_maxAlongs;
			}
		private:
			std::vector<double> m_acrosses;
			std::vector<double> m_alongs;
			std::vector<double> m_minAlongs;
			std::vector<double> m_maxAlongs;
			std::vector<double> m_acrossesLogged;
			std::vector<double> m_alongsLogged;
			std::vector<double> m_minAlongsLogged;
			std::vector<double> m_maxAlongsLogged;
			std::shared_ptr<Scale> m_alongAxis;
			std::shared_ptr<Scale> m_acrossAxis;
			bool m_useForAutoscale;
		};

		//this class holds data of n dimensions where there is a 2d
		//vector in each dimension. Eg, gridded data over a plane.
		//Note this data can hold x, y and z data for a curvilinear grid
		//Note virtual inheritance so if a class derived from this class is created,
		//the PlotableItem constructor must be called from there
		class StructuredData : virtual public PlotableItem
		{
		public:
			StructuredData(const std::vector<const ::sci::GridData<double, 2>*>& data, std::vector<std::shared_ptr<Scale>> axes, std::shared_ptr<splotTransformer> transformer)
			{

				sci::assertThrow(data.size() == axes.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "StructuredData constructor called with data and axes of different lengths."));
				m_data.resize(data.size());
				m_dataLogged.resize(data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					m_data[i] = *(data[i]);
					m_dataLogged[i] = sci::log10(m_data[i]);
				}
				m_axes = axes;
			}

			virtual void autoscaleAxes() override
			{
				for (size_t i = 0; i < m_data.size(); ++i)
					for (auto d : m_data[i])
						if (m_axes[i])
							m_axes[i]->expand(d);
			}
			const ::sci::GridData<double, 2>& getGrid(size_t dimension) const
			{
				return m_axes[dimension]->isLog() ? (m_dataLogged[dimension]) : (m_data[dimension]);
			}
			bool isLog(size_t dimension) const
			{
				return m_axes[dimension]->isLog();
			}
			bool hasData() const
			{
				return m_data.size() > 0 && m_data[0].size() > 0 && m_data[0][0].size() > 0;
			}
			size_t getNDimensions() const
			{
				return m_data.size();
			}
		private:
			std::vector<::sci::GridData<double, 2>> m_data;
			std::vector<::sci::GridData<double, 2>> m_dataLogged;
			std::vector<std::shared_ptr<Scale>> m_axes;
		};
	}
}


#endif
