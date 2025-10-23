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

		//this class holds data of n dimensions where there is just a 1d
		//vector in each dimension. Eg, x-y scatter data or line data
		//or x y z data where the z data is one value for each x, y point
		//Note virtual inheritance so if a class derived from this class is created,
		//the PlotableItem constructor must be called from there
		class UnstructuredData : virtual public PlotableItem
		{
		public:
			UnstructuredData(const std::vector<std::span<const double>>& data, std::vector<std::shared_ptr<Scale>> axes, std::shared_ptr<splotTransformer> transformer)
			{
				sci::assertThrow(data.size() == axes.size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "UnstructuredData constructor called with data and axes of different lengths."));
				m_data.resize(data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					m_data[i] = std::vector<double>(data[i].begin(), data[i].end());
					//if(i>0)
					//	sci::assertThrow(data[0]->size() == data[i]->size(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "UnstructuredData constructor called with data in different dimensions having different lengths."));
				}
				m_dataLogged.resize(m_data.size());
				for (size_t i = 0; i < m_data.size(); ++i)
				{
					m_dataLogged[i].resize(m_data[i].size());
					for (size_t j = 0; j < m_dataLogged[i].size(); ++j)
						m_dataLogged[i][j] = sci::log10(m_data[i][j]);
				}
				//m_dataLogged = std::vector<std::vector<double>>(m_data.size());
				//for (size_t i = 0; i < m_dataLogged.size(); ++i)
				//{
				//	m_dataLogged[i] = std::vector<double>(m_data[i].size());
				//	for (size_t j = 0; j < m_dataLogged[i].size(); ++j)
				//		m_dataLogged[i][j] = std::max(std::log10(m_data[i][j]), std::numeric_limits<double>::min()); //don't let logged data get to -infinity, it causes problems in plplot
				//}

				m_axes = axes;
			}

			virtual void autoscaleAxes() override
			{
				for (size_t i = 0; i < m_data.size(); ++i)
					if (m_axes[i])
						m_axes[i]->expand(m_data[i]);
			}

			const double* getPointer(size_t dimension) const
			{
				return m_axes[dimension]->isLog() ? (&(m_dataLogged[dimension][0])) : (&(m_data[dimension][0]));
			}
			const std::vector<double>& getVector(size_t dimension) const
			{
				return m_axes[dimension]->isLog() ? (m_dataLogged[dimension]) : (m_data[dimension]);
			}
			bool isLog(size_t dimension) const
			{
				return m_axes[dimension]->isLog();
			}
			bool hasData() const
			{
				return m_data.size() > 0 && m_data[0].size() > 0;
			}
			size_t getNPoints() const
			{
				return m_data[0].size();
			}
			size_t getNDimensions() const
			{
				return m_data.size();
			}
			const std::vector<std::shared_ptr<Scale>>& getAxes()
			{
				return m_axes;
			}
		protected:
		private:
			std::vector<std::vector<double>> m_data;
			std::vector<std::vector<double>> m_dataLogged;
			std::vector<std::shared_ptr<Scale>> m_axes;
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

			std::vector<const double*> getPointer(size_t dimension) const
			{
				std::vector<const double*> result(m_data[dimension].shape()[0]);
				if (m_axes[dimension]->isLog())
					for (size_t i = 0; i < result.size(); ++i)
						result[i] = &m_dataLogged[dimension][i][0];
				else
					for (size_t i = 0; i < result.size(); ++i)
						result[i] = &m_data[dimension][i][0];

				return result;
			}

			const ::sci::GridData<double, 2>& getGrid(size_t dimension) const
			{
				return m_data[dimension];
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
