#ifndef SVECTOR_PLOTPLOTABLE_H
#define SVECTOR_PLOTPLOTABLE_H

#include<vector>
#include<string>
#include<memory>
#include<iterator>


#include"../grid.h"
#include"../gridtransformview.h"
#include"plot.h"
#include"../tuple.h"


namespace sci
{
	namespace plot
	{
		template<class T>
		concept HasShape = requires(T &t)
		{
			t.shape();//has a shape method
			t.shape().size();//the result of shape has a size method
			t.shape()[0];//the result of shape can be indexed
		};

		template<class T>
		concept HasExtent = requires(T &t)
		{
			t.extent();
			t.extent().size();
			t.extent()[0];
		};

		template<class T>
		concept HasDimensions = requires(T t)
		{
			t.dimensions();
			t.dimensions().size();
			t.dimensions()[0];
		};

		template<class T>
		struct NDims
		{
			const static size_t value = 0;
		};

		template<class T> requires (HasShape<T>)
		struct NDims<T>
		{
			const static size_t value = decltype(T().shape()){}.size();
		};

		template<class T> requires (HasExtent<T>)
		struct NDims<T>
		{
			const static size_t value = decltype(T().extent()){}.size();
		};

		template<class T> requires (HasDimensions<T>)
		struct NDims<T>
		{
			const static size_t value = decltype(T().dimensions()){}.size();
		};

		template<class T>
		concept Is2D = NDims<T>::value == 2;


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
			&& Is2D<CONTAINER2D>;

		template<class CONTAINER1D, class DATATYPE>
		concept ArrayPlotable =
			std::ranges::range<CONTAINER1D>
			&& std::input_iterator<typename CONTAINER1D::const_iterator>
			&& std::convertible_to<typename CONTAINER1D::value_type, DATATYPE>
			&& !Is2D<CONTAINER1D>;

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

		template<class... DATATYPES>
		class SpacialAxesSet
		{
		public:
			virtual constexpr Point getPoint(const DATATYPES&... data) const = 0;
			virtual constexpr void setClippingRegion(Renderer& renderer, perMillimetre scale, sci::graphics::Clipper& clipper) const = 0;
			virtual constexpr ~SpacialAxesSet() {}
			virtual constexpr bool isLog(size_t n) const = 0;
			
		};

		template <class X, class Y>
		class XYAxisSet : public SpacialAxesSet<X, Y, decltype(sci::log10(X())), decltype(sci::log10(Y()))>
		{
		public:
			using LOGX = decltype(sci::log10(X()));
			using LOGY = decltype(sci::log10(Y()));
			XYAxisSet(std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis)
				:m_xAxis(xAxis), m_yAxis(yAxis), m_intersection(xAxis->getStart().getX(), yAxis->getStart().getY())
			{

				//a note about intersection
				//In theory, two axes do not need to be horizontal and vertical and their start points do not need to be the same.
				//In this case, you might follow a line from the start of each axis, parallel to the other and the intersection point
				//is the start corner of the plot area.
				//However, in general this intersection cannot be represented by a Point as it is a nonlinear combination of the
				//absolute position and the positions scaled by the height and width. Hence we don't do this.
				//Instead, we find the intersection as the point up from the start of the x axis and across from the start of the y
				//axis. This point can be represented by a Point and hence will scale properly
			}
			constexpr Point getPoint(const X& x, const Y& y, const LOGX& logX, const LOGY& logY) const override
			{
				return m_intersection + m_xAxis->alongAxisDistanceFromLoggedIfNeededData(x, logX) + m_yAxis->alongAxisDistanceFromLoggedIfNeededData(y, logY);
			}
			constexpr void setClippingRegion(Renderer& renderer, perMillimetre scale, sci::graphics::Clipper &clipper) const override
			{
				Point endCorner(m_xAxis->getEnd().getX(), m_yAxis->getEnd().getY());
				clipper = renderer.addClippingRegion(m_intersection, endCorner);
			}

			constexpr bool isLog(size_t n) const override
			{
				return n == 0 ? m_xAxis->isLog() : m_yAxis->isLog();
			}

			template<int index>
			constexpr auto &getAxis()
			{
				if constexpr (index == 0)
					return m_xAxis;
				return m_yAxis;
			}
			constexpr std::shared_ptr<Axis<X>> getXAxis()
			{
				return m_xAxis;
			}
			constexpr std::shared_ptr<Axis<Y>> getYAxis()
			{
				return m_yAxis;
			}
		private:
			Point m_intersection;
			std::shared_ptr<Axis<X>> m_xAxis;
			std::shared_ptr<Axis<Y>> m_yAxis;
		};

		//This class is the base class for all plotable items, it provides the interface for
		//the items
		template<class SPACIALAXESSET, class SCALESTUPLE>
		class PlotableItem : public DrawableItem
		{
		public:
			using scalesTuple = SCALESTUPLE;
			PlotableItem()
				:m_axesReadyToDraw(false)
			{
			}
			PlotableItem(SPACIALAXESSET spacialAxes, scalesTuple scales)
				:m_axesReadyToDraw(false)
			{
				m_axesSets.emplace_back(std::make_unique<SPACIALAXESSET>(spacialAxes));
				m_scales.emplace_back(scales);
			}
			template<class X, class Y>
			PlotableItem(std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, scalesTuple scales)
				:m_axesReadyToDraw(false)
			{
				m_axesSets.emplace_back(std::make_unique<XYAxisSet<X, Y>>(xAxis, yAxis));
				m_scales.emplace_back(scales);
			}
			virtual ~PlotableItem()
			{
			}
			constexpr size_t getNAxesSets() const
			{
				return m_axesSets.size();
			}
			void preDraw() override
			{
				autoscaleAxes();
				m_axesReadyToDraw = true;
			}
			bool readyToDraw() const override
			{
				return m_axesReadyToDraw;
			}
			void draw(Renderer& renderer, perMillimetre scale) override
			{
				for (size_t i = 0; i < m_axesSets.size(); ++i)
				{
					sci::graphics::Clipper clipper;
					m_axesSets[i]->setClippingRegion(renderer, scale, clipper);

					//plot the data
					plotData(*m_axesSets[i], m_scales[i], renderer, scale);
				}
			}

			template<int SET, class T>
			constexpr void expandAxis(const T& value)
			{
				//this function will deal with both individual types and containers
				for (size_t i = 0; i < m_scales.size(); ++i)
				{
					if (std::get<SET>(m_scales[i]))
						std::get<SET>(m_scales[i])->expand(value);
				}
			}
		private:

			virtual void autoscaleAxes() = 0;
			virtual void plotData(const SPACIALAXESSET& axisSet, const scalesTuple& scales, Renderer& renderer, perMillimetre scale) const {}
			std::vector<std::unique_ptr<SPACIALAXESSET>> m_axesSets; //this is a set of pointers because we could plot on different axis sets which inherit from the base type
			std::vector<scalesTuple> m_scales; //this doesn't need to be pointers as scalesTuple is always the same type 
			bool m_axesReadyToDraw;
		};

		

		template<class containers>
		struct SpScale
		{
			using value_type = typename std::tuple_element_t<0, containers>::value_type;
			using type = std::shared_ptr<Scale<value_type>>;
			using next = SpScale<TailTuple_t<1, containers>>;
			static const size_t size = std::tuple_size_v<containers>;
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
		template<class X, class Y, class SCALESTUPLE, class... CONTAINERS> requires(AreAllGridData_v<CONTAINERS...>())
		class Data : public PlotableItem<SpacialAxesSet<X, Y, decltype(sci::log10(X())), decltype(sci::log10(Y()))>, SCALESTUPLE>
		{
		private:
			template<class SCALESTUPLETOEXTRACTFROM, class CONTAINERSTUPLETOGETDIMSFROM> requires(std::tuple_size_v<SCALESTUPLETOEXTRACTFROM> == std::tuple_size_v<CONTAINERSTUPLETOGETDIMSFROM>)
			struct UnitlessExtract
			{
				using firstType = sci::GridData<typename std::tuple_element_t<0, SCALESTUPLETOEXTRACTFROM>::element_type::unitless_type,
					std::tuple_element_t<0, CONTAINERSTUPLETOGETDIMSFROM>::ndims>;
				using remainder = UnitlessExtract<TailTuple_t<1, SCALESTUPLETOEXTRACTFROM>, TailTuple_t<1, CONTAINERSTUPLETOGETDIMSFROM>>;
				static const size_t size = std::tuple_size_v<SCALESTUPLETOEXTRACTFROM>;
			};
		public:
			using containersTuple = std::tuple<CONTAINERS...>;
			using logTuple = TransformedTuple_t<UnitlessExtract<SCALESTUPLE, containersTuple>>;
			constexpr static int nDimensions = sizeof...(CONTAINERS);
			using spacialAxesSet = SpacialAxesSet < X, Y, decltype(sci::log10(X())), decltype(sci::log10(Y()))>;
		private:
			using parent = PlotableItem<spacialAxesSet, SCALESTUPLE>;
			//The following next two functions get a tuple of shared_ptrs to Scales, with each scale template parameter being the
			//value_type of the containers in containersTuple. The values of all the pointers is null. This function will
			//probably never get called in real code. They exists so that declytpe can be called on the result to define the type of
			//scalesTuple

			template <typename T>
			struct IsTuple : std::false_type {};

			template <typename... U>
			struct IsTuple<std::tuple <U...>> : std::true_type {};
		public:

			using scalesTuple = parent::scalesTuple;
			template<int SET>
			using ContainerType = typename std::tuple_element_t<SET, containersTuple>;

		private:
			//a generic 1d/2d get data function
			template<int SET, bool PREFERFIRST>
			constexpr auto getLoggedData(size_t index1, size_t index2) const
			{
				if constexpr (NDims<ContainerType<SET>>::value == 2)
					return std::get<SET>(m_dataLogged)[index1][index2];
				else
				{
					size_t index = PREFERFIRST ? index1 : index2;
					return std::get<SET>(m_dataLogged)[index];
				}
			}
			//a generic 1d/2d get data function
			template<int SET, bool PREFERFIRST>
			constexpr auto getLinearData(size_t index1, size_t index2) const
			{
				if constexpr (NDims<ContainerType<SET>>::value == 2)
					return std::get<SET>(m_data)[index1][index2];
				else
				{
					size_t index = PREFERFIRST ? index1 : index2;
					return std::get<SET>(m_data)[index];
				}
			}

			//get data when it's 1d
			template<int SET>
			constexpr auto getLoggedData(size_t index) const
				requires (SET >= 0 && NDims<ContainerType<SET>>::value == 1)
			{
				return getLoggedData<SET, true>(index, 0);
			}

			//get data when it's 1d
			template<int SET>
			constexpr auto getLinearData(size_t index) const
				requires (SET >= 0 && NDims<ContainerType<SET>>::value == 1)
			{
				return getLinearData<SET, true>(index, 0);
			}
		public:

			template<class... RECEIVEDCONTAINERS>
			Data(std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, SCALESTUPLE scales, RECEIVEDCONTAINERS... data)
				:PlotableItem<spacialAxesSet, SCALESTUPLE>(xAxis, yAxis, scales)
			{
				static_assert(std::tuple_size<scalesTuple>() == nDimensions, "Internal sci::plot::Data error - scalesTuple is the wrong size");
				static_assert(sizeof...(RECEIVEDCONTAINERS) == nDimensions, "The number of containers passed in to sci::plot::Data must match the number of dimensions");
				static_assert(std::tuple_size<SCALESTUPLE>() == nDimensions, "The number of axes passed in to sci::plot::Data must match the number of dimensions");
				static_assert(IsTuple<SCALESTUPLE>::value, "The axes must be passed in to sci::plot as a std::tuple");
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
			template<int SET>
			constexpr size_t getNPoints() const
				requires(SET>=0 && NDims<ContainerType<SET>>::value==1)
			{
				return std::get<SET>(m_data).size();
			}
			template<int SET>
			auto getShape() const
				requires(SET >= 0 && NDims<ContainerType<SET>>::value == 2)
			{
				return getShapeOf(std::get<SET>(m_data));
			}
			constexpr size_t getNDimensions() const
			{
				return nDimensions;
			}

			//get the point as plotted on axesSet from the index element of the datasets at indices XSET and YSET
			template<int XSET, int YSET>
			constexpr Point getPoint(size_t index, const spacialAxesSet& axesSet) const
				requires(XSET >= 0 && YSET >= 0
				&& NDims<ContainerType<XSET>>::value == 1
				&& NDims<ContainerType<YSET>>::value == 1)
			{
				return axesSet.getPoint(getLinearData<XSET>(index), getLinearData<YSET>(index),
					getLoggedData<XSET>(index), getLoggedData<YSET>(index));
			}

			//get the point as plotted on axesSet from the ith/jth element of the datasets at indices XSET and YSET
			//if the dataset at XSET is 1d, the index1 element is used for this dataset
			//if the dataset at YSET is 1d, the index2 element is used for this dataset
			//if either dataset is 2d then the index1,index2 element is used
			template<int XSET, int YSET>
			constexpr Point getPoint(size_t index1, size_t index2, const spacialAxesSet& axesSet) const
				requires(XSET >= 0 && YSET >= 0)
			{
				return axesSet.getPoint(getLinearData<XSET, true>(index1, index2), getLinearData<YSET, false>(index1, index2),
					getLoggedData<XSET, true>(index1, index2), getLoggedData<YSET, false>(index1, index2));
			}

			template<int YSET, class XTYPE>
			constexpr Point getPointXFixed(XTYPE x, decltype(sci::log10(XTYPE())) loggedX, size_t index, const spacialAxesSet& axesSet) const
				requires(YSET >= 0)
			{
				return axesSet.getPoint(x, getLinearData<YSET>(index),
					loggedX, getLoggedData<YSET>(index));
			}

			template<int XSET, class YTYPE>
			constexpr Point getPointYFixed(YTYPE y, decltype(sci::log10(YTYPE())) loggedY, size_t index, const spacialAxesSet& axesSet) const
				requires (XSET >= 0)
			{
				return axesSet.getPoint(getLinearData<XSET>(index), y, 
					 getLoggedData<XSET>(index), loggedY);
			}

			template<class XTYPE, class YTYPE>
			constexpr Point getPointFixed(XTYPE x, YTYPE y, decltype(sci::log10(XTYPE())) loggedX, decltype(sci::log10(YTYPE())) loggedY, const spacialAxesSet& axesSet) const
			{
				return axesSet.getPoint(x, y, loggedX, loggedY);
			}

			template<int SET>
			constexpr auto getTransformed(size_t index, const scalesTuple& scales) const
				requires(SET >= 0 && NDims<ContainerType<SET>>::value == 1)
			{
				const auto& scale = std::get<SET>(scales);
				if (scale->isLog())
					return scale->getTransformed(getLoggedData<SET>(index));
				else
					return scale->getTransformed(getLinearData<SET>(index));
			}
			template<int SET>
			constexpr auto getTransformed(size_t index1, size_t index2, const scalesTuple& scales) const
				requires(SET >= 0 && NDims<ContainerType<SET>>::value == 2)
			{
				const auto& scale = std::get<SET>(scales);
				if (scale->isLog())
					return scale->getTransformed(getLoggedData<SET, true>(index1, index2));
				else
					return scale->getTransformed(getLinearData<SET, true>(index1, index2));
			}

			//This function must only be called when the data us plotted against a level scale.
			//gets the 0-1 normalised distance of inputData of a level between two points
			//if scale->Levels[level] == data<SET>[point1Index] the result is 0;
			//if scale->Levels[level] == data<SET>[point2Index] the result is 1;
			template<int SET>
			constexpr auto getWeight(size_t level, size_t point1Index, size_t point2Index, const scalesTuple& scales) const
				requires(SET >= 0 && NDims<ContainerType<SET>>::value == 1)
			{
				const auto& scale = std::get<SET>(scales);
				if (scale->isLog())
				{
					auto v1 = getLoggedData<SET>(point1Index);
					auto v2 = getLoggedData<SET>(point2Index);
					return (scale->getLevelLogged(level) - v1) / (v2 - v1);
				}
				else
				{
					auto v1 = getLinearData<SET>(point1Index);
					auto v2 = getLinearData<SET>(point2Index);
					return (scale->getLevelLinear(level) - v1) / (v2 - v1);
				}
			}

			//if scale->Levels[level] == data<SET>[point1Index1][point1Index2] the result is 0;
			//if scale->Levels[level] == data<SET>[point2Index1][point2Index2] the result is 1; 
			//if data<set> is 1d, then point1Index1 is used.
			//if data<set> is 2d, then point2Index2 is used.
			template<int SET>
			constexpr auto getWeight(size_t level, size_t point1Index1, size_t point1Index2, size_t point2Index1, size_t point2Index2, const scalesTuple& scales) const
				requires(SET >= 0)
			{
				const auto& scale = std::get<SET>(scales);
				if (scale->isLog())
				{
					auto v1 = getLoggedData<SET, true>(point1Index1, point1Index2);
					auto v2 = getLoggedData<SET, false>(point2Index1, point2Index2);
					return (scale->getLevelLogged(level) - v1) / (v2 - v1);
				}
				else
				{
					auto v1 = getLinearData<SET, true>(point1Index1, point1Index2);
					auto v2 = getLinearData<SET, false>(point2Index1, point2Index2);
					return (scale->getLevelLinear(level) - v1) / (v2 - v1);
				}
			}

			template<int SET>
			constexpr auto isOffScale(size_t index, const scalesTuple& scales) const
				requires(SET >= 0 && NDims<ContainerType<SET>>::value == 1)
			{
				const auto& scale = std::get<SET>(scales);
				if (scale->isLog())
				{
					if (getLoggedData<SET>(index) < scale->getLogMin())
						return true;
					if (getLoggedData<SET>(index) > scale->getLogMax())
						return true;
					return false;
				}
				else
				{
					if (getLinearData<SET>(index) < scale->getMin())
						return true;
					if (getLinearData<SET>(index) > scale->getMax())
						return true;
					return false;
				}
			}

			template<int SET>
			constexpr auto isOffScale(size_t index1, size_t index2, const scalesTuple& scales) const
				requires(SET >= 0 && NDims<ContainerType<SET>>::value == 2)
			{
				const auto& scale = std::get<SET>(scales);
				if (scale->isLog())
				{
					if (getLoggedData<SET, true>(index1, index2) < scale->getLogMin())
						return true;
					if (getLoggedData<SET, true>(index1, index2) > scale->getLogMax())
						return true;
					return false;
				}
				else
				{
					if (getLinearData<SET, true>(index1, index2) < scale->getLinearMin())
						return true;
					if (getLinearData<SET, true>(index1, index2) > scale->getLinearMax())
						return true;
					return false;
				}
			}
		private:

			template<class T>
			static constexpr auto getShapeOf(const T& t)
				requires(HasShape<T> || HasExtent<T> || HasDimensions<T>)
			{
				if constexpr (HasShape<T>)
				{
					auto temp = t.shape();
					std::array<size_t, std::tuple_size_v<decltype(temp)>> result;
					for (size_t i = 0; i < result.size(); ++i)
						result[i] = temp[i];
					return result;
				}
				if constexpr (HasExtent<T>)
				{
					auto temp = t.extent();
					std::array<size_t, std::tuple_size_v<decltype(temp)>> result;
					for (size_t i = 0; i < result.size(); ++i)
						result[i] = temp[i];
					return result;
				}
				if constexpr (HasDimensions<T>)
				{
					auto temp = t.dimension();
					std::array<size_t, std::tuple_size_v<decltype(temp)>> result;
					for (size_t i = 0; i < result.size(); ++i)
						result[i] = temp[i];
					return result;
				}
			}

			template<class T, class... REMAINING>
			constexpr void copyLinear(const T& next, REMAINING... remaining)
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
					std::array<size_t, 2> shape = getShapeof(next);
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
			constexpr void calculateLog()
			{
				using scaleType = typename std::tuple_element_t<index, SCALESTUPLE>::element_type::value_type;
				auto& linear = std::get<index>(m_data);
				auto& log = std::get<index>(m_dataLogged);
				log.reshape(linear.shape());
				auto linearIter = linear.begin();
				auto linearEnd = linear.end();
				auto logIter = log.begin();
				for (; linearIter != linearEnd; ++linearIter, ++logIter)
					*logIter = sci::log10(*linearIter/scaleType(1));
				if constexpr (index != nDimensions - 1)
					calculateLog<index + 1>();
			}
			
			template <int SET>
			constexpr void autoscaleAxesRecursive()
				requires(SET>=0)
			{
				this->expandAxis<SET>(std::get<SET>(m_data));
				if constexpr (SET != nDimensions - 1)
					autoscaleAxesRecursive<SET + 1>();
			}

			containersTuple m_data;
			logTuple m_dataLogged;
		};
	}
}


#endif
