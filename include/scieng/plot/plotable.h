#ifndef SVECTOR_PLOTPLOTABLE_H
#define SVECTOR_PLOTPLOTABLE_H

#include<vector>
#include<string>
#include<memory>
#include<iterator>


#include"../grid.h"
#include"../gridtransformview.h"
#include"elements.h"


namespace sci
{
	namespace plot
	{
		const int plotDataErrorCode = 1;


		template<class T>
		concept Has2DShape = requires(T t)
		{
			t.shape()[0];
			t.shape()[1];
		};

		template<class T>
		concept Has2DExtent = requires(T t)
		{
			t.extent()[0];
			t.extent()[1];
		};

		template<class T>
		concept Has2DDimensions = requires(T t)
		{
			t.dimensions()[0];
			t.dimensions()[1];
		};

		template<class CONTAINER2D, class DATATYPE>
		concept GridPlotable =
			std::input_iterator<typename CONTAINER2D::const_iterator>
			&& std::convertible_to<typename CONTAINER2D::value_type, DATATYPE>
			&& (Has2DExtent< CONTAINER2D> || Has2DShape<CONTAINER2D> || Has2DDimensions<CONTAINER2D>);

		template<class CONTAINER1D, class DATATYPE>
		concept ArrayPlotable =
			std::ranges::range<CONTAINER1D>
			&& std::input_iterator<typename CONTAINER1D::const_iterator>
			&& std::convertible_to<typename CONTAINER1D::value_type, DATATYPE>
			&& !(Has2DExtent< CONTAINER1D> || Has2DShape<CONTAINER1D> || Has2DDimensions<CONTAINER1D>);

		template<class XCONTAINER, class YCONTAINER, class X, class Y>
		concept XYPlotable =
			ArrayPlotable<XCONTAINER, X>
			&& ArrayPlotable<YCONTAINER, Y>;

		template<int XDIMS, int YDIMS, int ZDIMS, class XCONTAINER, class YCONTAINER, class ZCONTAINER, class X, class Y, class Z>
		concept XYZPlotable =
			((XDIMS == 1 && ArrayPlotable<XCONTAINER, X>) || (XDIMS == 2 && GridPlotable<XCONTAINER, X>))
			&& ((YDIMS == 1 && ArrayPlotable<YCONTAINER, Y>) || (YDIMS == 2 && GridPlotable<YCONTAINER, Y>))
			&& ((ZDIMS == 1 && ArrayPlotable<ZCONTAINER, Z>) || (ZDIMS == 2 && GridPlotable<ZCONTAINER, Z>));

		template<class CONTAINER, class DATA>
		constexpr int getPlotableNDims()
			requires(ArrayPlotable<CONTAINER, DATA> || GridPlotable<CONTAINER, DATA>)
		{
			if constexpr (ArrayPlotable<CONTAINER, DATA>)
				return 1;
			return 2;
		}

		template<class T, class...Others> requires(sizeof...(Others) == 0)
		constexpr bool AreAllGridData_v()
		{
			return IsGridData<T>;
		}

		template<class T, class...Others> requires(sizeof...(Others) > 0)
		constexpr bool AreAllGridData_v()
		{
			return IsGridData<T> && AreAllGridData_v<Others...>();
		}

		template <class T, class...Others>
		class AreAllGridData
		{
			const bool value = AreAllGridData_v<T, Others...>();
		};

		template<class T>
		inline constexpr bool testN()
		{
			return false;
		}

		template< class T >
		inline constexpr bool testN()
			requires (IsGridDims<T, 2>)
		{
			return true;
		}

		inline void test()
		{
			static_assert(testN<sci::GridData<double, 2>>(), "failed test");
		}

		//This class is the base class for all plotable items, it provides the interface for
		//the items
		template<class T, class U>
		class PlotableItem : public DrawableItem
		{
		public:
			PlotableItem()
				:m_scaledAxes(false)
			{
				m_xAxis.push_back(nullptr);
				m_yAxis.push_back(nullptr);
			}
			PlotableItem(std::shared_ptr<Axis<T>> xAxis, std::shared_ptr<Axis<U>> yAxis)
				:m_scaledAxes(false)
			{
				m_xAxis.push_back(xAxis);
				m_yAxis.push_back(yAxis);
				m_intersections.push_back(Point(xAxis->getStart().getX(), yAxis->getStart().getY()));
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
					sci::graphics::Clipper clipper = renderer.addClippingRegion(m_intersections[i], endCorner);

					//plot the data
					plotData(i, renderer, scale);
				}
			}

			Point getPointFromLinearData(T x, U y, size_t axisSetIndex) const
			{
				return m_intersections[axisSetIndex] + m_xAxis[axisSetIndex]->alongAxisDistanceFromLinearData(x) + m_yAxis[axisSetIndex]->alongAxisDistanceFromLinearData(y);
			}

			Point getPointFromLoggedIfNeededData(T x, U y, size_t axisSetIndex) const
			{
				return m_intersections[axisSetIndex] + m_xAxis[axisSetIndex]->alongAxisDistanceFromLoggedIfNeededData(x) + m_yAxis[axisSetIndex]->alongAxisDistanceFromLoggedIfNeededData(y);
			}
			std::shared_ptr<Axis<T>> getXAxis(size_t axisSetIndex)
			{
				return m_xAxis[axisSetIndex];
			}
			std::shared_ptr<Axis<U>> getYAxis(size_t axisSetIndex)
			{
				return m_yAxis[axisSetIndex];
			}
			std::shared_ptr<const Axis<T>> getXAxis(size_t axisSetIndex) const
			{
				return m_xAxis[axisSetIndex];
			}
			std::shared_ptr<const Axis<U>> getYAxis(size_t axisSetIndex) const
			{
				return m_yAxis[axisSetIndex];
			}
		private:
			virtual void autoscaleAxes(size_t axisSetIndex) = 0;
			virtual void plotData(size_t axisSetIndex, Renderer& renderer, perMillimetre scale) const {}
			std::vector<std::shared_ptr<Axis<T>>> m_xAxis;
			std::vector<std::shared_ptr<Axis<U>>> m_yAxis;
			bool m_scaledAxes;
			std::vector<Point> m_intersections;
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
		template<class X, class Y, class... CONTAINERS> requires(AreAllGridData_v<CONTAINERS...>())
		class Data : public PlotableItem<X, Y>
		{
		public:
			using containersTuple = std::tuple<CONTAINERS...>;
		private:
			//The following next two functions get a tuple of shared_ptrs to Scales, with each scale template parameter being the
			//value_type of the containers in containersTuple. The values of all the pointers is null. This function will
			//probably never get called in real code. They exists so that declytpe can be called on the result to define the type of
			//axisTuple
			template<int index>
			static auto getNullScalePointersContinued(const auto& current)
			{
				if constexpr (index == std::tuple_size<containersTuple>())
					return current;
				else
				{
					using type = typename std::tuple_element<index, containersTuple>::type::value_type;
					using scalePtrType = std::shared_ptr<Scale<type>>;
					return getNullScalePointersContinued<index + 1>(std::tuple_cat(current, std::make_tuple(scalePtrType(nullptr))));
				}
			}
			static auto getNullScalePointers()
			{
				return getNullScalePointersContinued<0>(std::tuple<>());
			}

			template <typename T>
			struct IsTuple : std::false_type {};

			template <typename... U>
			struct IsTuple<std::tuple <U...>> : std::true_type {};
		public:
			using containersTuple = std::tuple<CONTAINERS...>;
			using axisTuple = decltype(getNullScalePointers());
			constexpr static int nDimensions = sizeof...(CONTAINERS);

			template< class AXISTUPLE, class... RECEIVEDCONTAINERS>
			Data(std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, AXISTUPLE axes, RECEIVEDCONTAINERS... data)
				:PlotableItem<X, Y>(xAxis, yAxis)
			{
				static_assert(std::tuple_size<axisTuple>() == nDimensions, "Internal sci::plot::Data error - axisTuple is the wrong size");
				static_assert(sizeof...(RECEIVEDCONTAINERS) == nDimensions, "The number of containers passed in to sci::plot::Data must match the number of dimensions");
				static_assert(std::tuple_size<AXISTUPLE>() == nDimensions, "The number of axes passed in to sci::plot::Data must match the number of dimensions");
				static_assert(IsTuple<AXISTUPLE>::value, "The axes must be passed in to sci::plot as a std::tuple");
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
				return isLog<dimension>(setIndex) ? std::get<dimension>(m_dataLogged) : std::get<dimension>(m_data);
			}
			template<int dimension>
			constexpr bool isLog(size_t setIndex) const
			{
				return std::get<dimension>(m_axes[setIndex])->isLog();
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
			constexpr const auto& getScales(size_t setIndex)
			{
				return m_axes[setIndex];
			}
		private:
			template<class T, class... REMAINING>
			void copyLinear(const T& next, REMAINING... remaining)
			{
				constexpr int index = nDimensions - sizeof...(REMAINING) - 1;
				using destinationType = std::remove_cvref_t<decltype(std::get<index>(m_data))>;

				verifyCopyable<T, destinationType>();//this gives better error messages than a within function static_assert

				if constexpr (std::is_convertible_v<T, destinationType>)
					std::get<index>(m_data) = next;
				else if constexpr( IsGridDims<destinationType, 1>)
				{
					std::get<index>(m_data).assign(next.begin(), next.end());
				}
				else if constexpr(IsGridDims<destinationType, 2>)
				{
					std::array<size_t, 2> shape{ 0,0 };
					if constexpr(Has2DExtent<T>)
					{
						auto e = next.extent;
						shape = { e[0], e[1] };
					}
					else if constexpr (Has2DShape<T>)
					{
						auto s = next.shape;
						shape = { s[0], s[1] };
					}
					else if constexpr (Has2DDimensions<T>)
					{
						auto d = next.dimensions;
						shape = { d[0], d[1] };
					}
					auto& linear = std::get<index>(m_data);
					linear.reshape(shape);
					for (size_t i = 0; i < shape[0]; ++i)
						for (size_t j = 0; j < shape[1]; ++j)
							linear[i][j] = next[i][j];
				}

				if constexpr (index != nDimensions - 1)
					copyLinear(remaining...);
			}

			template<class T, class U>
			static void verifyCopyable()
			{
				//this gives better error reporting asI can see the types
				static_assert(std::is_convertible_v<T, U> || IsGridDims<U, 1> || IsGridDims < U, 1>, "The contents of type T cannot be assigned to the contents of type U.");

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
				if(std::get<index>(m_axes[setIndex]))
					std::get<index>(m_axes[setIndex])->expand(std::get<index>(m_data));
				if constexpr (index != nDimensions - 1)
					autoscaleAxesRecursive<index + 1>(setIndex);
			}

			containersTuple m_data;
			containersTuple m_dataLogged;
			std::vector<axisTuple> m_axes;
		};
	}
}


#endif
