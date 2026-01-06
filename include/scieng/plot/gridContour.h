#ifndef SVECTOR_PLOTGRIDCONTOURS_H
#define SVECTOR_PLOTGRIDCONTOURS_H

#include "plotable.h"
#include <set>


namespace sci
{
	namespace plot
	{
		template<int Dimensions1, int Dimensions2, class X, class Y, class Z> requires((Dimensions1 == 1 || Dimensions1 == 2) && (Dimensions2 == 1 || Dimensions2 == 2))
		class Grid : public Data<X, Y,
			std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<ColourScale<Z>>>,
			sci::GridData<X, Dimensions1>, sci::GridData<Y, Dimensions2>, sci::GridData<Z, 2>>
		{
		public:
			using data = Data<X, Y,
				std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<ColourScale<Z>>>, sci::GridData<X, Dimensions1>, sci::GridData<Y, Dimensions2>, sci::GridData<Z, 2>>;

			//x and y should be 1d or 2d to match the dimensions passed in
			template<class XCONTAINER, class YCONTAINER, class ZGRID>
			Grid(const XCONTAINER& xs, const YCONTAINER& ys, const ZGRID& zs, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, std::shared_ptr<ColourScale<Z>> colourScale)
				requires(XYZPlotable<Dimensions1, Dimensions2, 2, XCONTAINER, YCONTAINER, ZGRID, X, Y, Z>)
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, colourScale), xs, ys, zs)
			{
				//sci::assertThrow(xs.size() == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
				//sci::assertThrow(ys.size() == zs.shape()[1] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));

				m_colourscale = colourScale;
			}

			
			void plotData(const data::spacialAxesSet& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				renderer.setPen(rgbcolour(), Length(sci::plot::millimetre(0.0)));

				std::array<size_t, 2> shape = this->getShape<2>();
				for (size_t i = 0; i < shape[0]; ++i)
				{
					for (size_t j = 0; j < shape[1]; ++j)
					{
						if (this->isOffScale<2>(i, j, scales))
							continue;
						renderer.setBrush(this->getTransformed<2>(i, j, scales));
						renderer.polygon({ this->getPoint<0,1>(i, j, axisSet),
							this->getPoint<0,1>(i + 1, j, axisSet),
							this->getPoint<0,1>(i + 1, j + 1, axisSet),
							this->getPoint<0,1>(i, j + 1, axisSet),
							this->getPoint<0,1>(i, j, axisSet) });
					}
				}

				
			}
		private:
			std::shared_ptr<ColourScale<Z>> m_colourscale;
		};

		template<class X, class Y, class Z, class XCONTAINER, class YCONTAINER, class ZGRID>
		auto makeGrid(const XCONTAINER& xs, const YCONTAINER& ys, const ZGRID& zs, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, std::shared_ptr<ColourScale<Z>> colourScale)
		{
			constexpr int Dimensions1 = getPlotableNDims<XCONTAINER, X>();
			constexpr int Dimensions2 = getPlotableNDims<YCONTAINER, Y>();
			static_assert (XYZPlotable<Dimensions1, Dimensions2, 2, XCONTAINER, YCONTAINER, ZGRID, X, Y, Z>, "Attempted to create a sci::plot::Grid with data that is not plotable.");
			return std::make_shared<Grid<Dimensions1, Dimensions2, X, Y, Z>>(xs, ys, zs, xAxis, yAxis, colourScale);
		}

		template<int Dimensions1, int Dimensions2, class X, class Y, class Z> requires((Dimensions1 == 1 || Dimensions1 == 2) && (Dimensions2 == 1 || Dimensions2 == 2))
		class Contours : public Data<X, Y,
			std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<LevelScale<Z, float>>>,
			sci::GridData<X, Dimensions1>, sci::GridData<Y, Dimensions2>, sci::GridData<Z, 2>>
		{
		public:
			using data = Data<X, Y,
				std::tuple<std::shared_ptr<Axis<X>>, std::shared_ptr<Axis<Y>>, std::shared_ptr<LevelScale<Z, float>>>,
				sci::GridData<X, Dimensions1>, sci::GridData<Y, Dimensions2>, sci::GridData<Z, 2>>;

			/*template<class XCONTAINER, class YCONTAINER, class ZGRID>
			Contours(const XCONTAINER& xs, const YCONTAINER& ys, const ZGRID& zs, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, std::shared_ptr<ColourScale<Z>> colourScale, const LineStyle& lineStyle)
				requires(XYZPlotable<Dimensions1, Dimensions2, 2, XCONTAINER, YCONTAINER, ZGRID, X, Y, Z>)
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, colourScale), xs, ys, zs)
			{
				//sci::assertThrow(xs.size() == zs.shape()[0], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
				//sci::assertThrow(ys.size() == zs.shape()[1], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

				m_colourscale = colourScale;
				m_levelScale = nullptr;
				m_lineStyle = lineStyle;
			}*/

			template<class XCONTAINER, class YCONTAINER, class ZGRID>
			Contours(const XCONTAINER& xs, const YCONTAINER& ys, const ZGRID& zs, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, std::shared_ptr<LevelScale<Z, float>> levelScale, const LineStyle& lineStyle)
				requires(XYZPlotable<Dimensions1, Dimensions2, 2, XCONTAINER, YCONTAINER, ZGRID, X, Y, Z>)
				: data(xAxis, yAxis, std::make_tuple(xAxis, yAxis, levelScale), xs, ys, zs)
			{
				//sci::assertThrow(xs.size() == zs.shape()[0], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
				//sci::assertThrow(ys.size() == zs.shape()[1], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

				m_colourscale = nullptr;
				m_levelScale = levelScale;
				m_lineStyle = lineStyle;
			}

			void plotData(const data::spacialAxesSet& axisSet, const data::scalesTuple& scales, Renderer& renderer, sci::plot::perMillimetre scale) const override
			{
				sci::GridData<Z, 1> contourLevels;
				if (m_levelScale)
				{
					if (m_levelScale->isLog())
						contourLevels = m_levelScale->getLevelsLogged();
					else
						contourLevels = m_levelScale->getLevelsLinear();
					//sci::assertThrow(contourLevels.size() > 1, sci::err(sci::SERR_PLOT, 0, "ContourData::plotData: Cannot use a level scale with fewer than 2 levels."));
				}

				//get the segments of each contour line using the marching squares algorithm.
				//note that this just gives the edges of the grid boxes that each segment touches
				std::vector<std::set<Segment>> segmentsPerLevel = marchingSquaresSegments(scales);

				//render the fill

				/*std::vector<std::set<Segment>::iterator> segmentIters(segmentsPerLevel.size());
				for (size_t i = 0; i < segmentsPerLevel.size(); ++i)
					segmentIters[i] = segmentsPerLevel[i].begin();

				std::vector<std::set<Segment>::iterator> intersections;
				for (size_t i = 0; i < shape[0]-1; ++i)
				{
					for (size_t j = 0; j < shape[1]-1; ++j)
					{
						X x0;
						X x1;
						Y y0;
						Y y1;
						//if both x and y are the same dimensionality then they will be stored in the same
						//class with x as index 0 and y as index 1. Otherwise they will be stored in different
						//classes, both as index 0;
						size_t yIndex = (m_x1d == m_y1d) ? 1 : 0;
						if (m_x1d)
						{
							x0 = getVector(0)[i];
							x1 = getVector(0)[i + 1];
						}
						else
						{
							x0 = getGrid(0)[i][j];
							x1 = getGrid(0)[i + 1][j + 1];
						}
						if (m_y1d)
						{
							y0 = getVector(yIndex)[j];
							y1 = getVector(yIndex)[j + 1];
						}
						else
						{
							y0 = getGrid(yIndex)[i][j];
							y1 = getGrid(yIndex)[i + 1][j + 1];
						}
						std::pair<size_t, size_t> ijPair(i, j);
						for (size_t k = 0; k < segmentIters.size(); ++k)
						{
							if (segmentIters[k] != segmentsPerLevel[k].end() && i== segmentIters[k]->index1 && j== segmentIters[k]->index2)
							{
								intersections.push_back(segmentIters[k]);
							}
						}
						if (intersections.size() == 0)
						{
							//full rectangle with no intersection - just fill
							renderer.rectangle(getPoint(x0, y0), getPoint(x1, y1));
						}
						else if (intersections.size() == 1)
						{
							//we can just use the intersectionType from the single intersection to sort this

						}
						else
						{
							//we're getting more complicated here
							//we can work through each band and if the countour level either side only has one
							//intersection, then we can work out the nodes (corners, plus points where segments
							//touch the edge) and these will make a convex polygon, so we need to arrange them
							//clockwise or anticlockwise

							//if there is a contour level with 2 segments (this is the maximum possible0 then we
							//have a sadle point and multiple polygons the same shade
						}


						intersections.resize(0);
					}
				}*/

				//***************************************
				//render the contours
				//*************************************
				
				//concatenate the segments into lines
				//this is destructive - it moves segments from the segmentsPerLevel variable to the segments variable
				std::array<size_t, 2> shape = this->getShape<2>();
				std::vector<std::vector<Segment>> segments;
				for (size_t i = 0; i < segmentsPerLevel.size(); ++i)
				{
					while (segmentsPerLevel[i].size() > 0)
					{
						segments.push_back(followContourBothDirections(segmentsPerLevel[i], shape[0], shape[1]));
					}
				}

				//now work out where the contours cross each grid box edge and plot
				
				m_lineStyle.setPen(renderer);
				for (size_t i = 0; i < segments.size(); ++i)
				{
					std::vector<Point> points = getContourLine(segments[i], axisSet, scales);
					renderer.polyLine(points);
				}

			}
		private:

			struct Segment
			{
				size_t index1;
				size_t index2;
				uint8_t intersectionType;
				size_t level;
				bool operator==(const Segment& other) const
				{
					return index1 == other.index1 && index2 == other.index2 && intersectionType == other.intersectionType;
				}
				bool operator!=(const Segment& other) const
				{
					return !(*this == other);
				}
				bool operator<(const Segment& other) const
				{
					return index1 == other.index1 ?
						(index2 == other.index2 ? intersectionType < other.intersectionType : index2 < other.index2)
						: index1 < other.index1;
				}
			};

			template<class T>
			static constexpr uint8_t getIntersectionType(size_t i, size_t j, const sci::GridData<T, 2>& grid, T contourLevel)
			{
				uint8_t intersectionType = 0;
				if (grid[i][j] > contourLevel)
					intersectionType = intersectionType | 0x01;
				if (grid[i + 1][j] > contourLevel)
					intersectionType = intersectionType | 0x02;
				if (grid[i + 1][j + 1] > contourLevel)
					intersectionType = intersectionType | 0x04;
				if (grid[i][j + 1] > contourLevel)
					intersectionType = intersectionType | 0x08;


				//disambiguate saddle points
				if (5 == intersectionType && (grid[i + 1][j + 1] + grid[i + 1][j] + grid[i][j] + grid[i][j + 1]) / 4.0 <= contourLevel)
					intersectionType = 16;
				else if (10 == intersectionType && (grid[i + 1][j + 1] + grid[i + 1][j] + grid[i][j] + grid[i][j + 1]) / 4.0 <= contourLevel)
					intersectionType = 17;
				return intersectionType;
			}

			constexpr auto sumCorners(size_t i, size_t j, const data::scalesTuple& scales) const
			{
				return this->getTransformed<2>(i + 1, j + 1, scales)
					+ this->getTransformed<2>(i + 1, j, scales)
					+ this->getTransformed<2>(i, j, scales)
					+ this->getTransformed<2>(i, j + 1, scales);
			}

			constexpr uint8_t getIntersectionType(size_t i, size_t j, size_t contourLevel, const data::scalesTuple&scales) const
			{
				uint8_t intersectionType = 0;
				if (this->getTransformed<2>(i, j, scales) > float(contourLevel))
					intersectionType = intersectionType | 0x01;
				if (this->getTransformed<2>(i + 1, j, scales) > float(contourLevel))
					intersectionType = intersectionType | 0x02;
				if (this->getTransformed<2>(i + 1, j + 1, scales) > float(contourLevel))
					intersectionType = intersectionType | 0x04;
				if (this->getTransformed<2>(i, j + 1, scales) > float(contourLevel))
					intersectionType = intersectionType | 0x08;


				//disambiguate saddle points
				if (5 == intersectionType && sumCorners(i, j, scales) * 0.25 <= float(contourLevel))
					intersectionType = 16;
				else if (10 == intersectionType && sumCorners(i, j, scales) * 0.25 <= float(contourLevel))
					intersectionType = 17;
				return intersectionType;
			}

			static uint8_t disambiguateContours(uint8_t intersectionType)
			{
				//for contours we don't care which corners are higher or lower, so we can save some
				//search time by skipping the duplicate countour paths
				if (intersectionType == 16)
					return 5;
				if (intersectionType == 17)
					return 10;
				if (intersectionType > 7 && intersectionType != 10)
					return 15 - intersectionType;
				return intersectionType;
			}

			static constexpr std::array<uint8_t, 3> getLeftIntersectionTypes()
			{
				return { 1, 3, 7 };
			}

			static constexpr std::array<uint8_t, 3> getRightIntersectionTypes()
			{
				return { 2, 3, 4 };
			}

			static constexpr std::array<uint8_t, 3> getTopIntersectionTypes()
			{
				return { 4, 6, 7 };
			}

			static constexpr std::array<uint8_t, 3> getBottomIntersectionTypes()
			{
				return { 1, 2, 6 };
			}

			static constexpr bool hasLeft(uint8_t intersectionType)
			{
				return intersectionType == getLeftIntersectionTypes()[0] ||
					intersectionType == getLeftIntersectionTypes()[1] ||
					intersectionType == getLeftIntersectionTypes()[2];
			}

			static constexpr bool hasRight(uint8_t intersectionType)
			{
				return intersectionType == getRightIntersectionTypes()[0] ||
					intersectionType == getRightIntersectionTypes()[1] ||
					intersectionType == getRightIntersectionTypes()[2];
			}

			static constexpr bool hasTop(uint8_t intersectionType)
			{
				return intersectionType == getTopIntersectionTypes()[0] ||
					intersectionType == getTopIntersectionTypes()[1] ||
					intersectionType == getTopIntersectionTypes()[2];
			}

			static constexpr bool hasBottom(uint8_t intersectionType)
			{
				return intersectionType == getBottomIntersectionTypes()[0] ||
					intersectionType == getBottomIntersectionTypes()[1] ||
					intersectionType == getBottomIntersectionTypes()[2];
			}

			//retuens all possible link options. Where there are fewer than 6 options
			//the unused elements of the result will be filled with the given segment
			static std::array<Segment, 6> getPossibleLinkOptions(Segment segment, size_t nX, size_t nY)
			{
				//we are assuming we have disambiguated contours and split cases 5 and 10
				std::array<Segment, 6> result{ segment, segment, segment, segment, segment, segment };
				auto insertPoint = result.begin();
				if (hasLeft(segment.intersectionType) && segment.index1 > 1)
				{
					*insertPoint = { segment.index1 - 1, segment.index2, getLeftIntersectionTypes()[0] };
					*(insertPoint + 1) = { segment.index1 - 1, segment.index2, getLeftIntersectionTypes()[1] };
					*(insertPoint + 2) = { segment.index1 - 1, segment.index2, getLeftIntersectionTypes()[2] };
					insertPoint += 3;
				}
				if (hasRight(segment.intersectionType) && segment.index1 < nX)
				{
					*insertPoint = { segment.index1 + 1, segment.index2, getRightIntersectionTypes()[0] };
					*(insertPoint + 1) = { segment.index1 + 1, segment.index2, getRightIntersectionTypes()[1] };
					*(insertPoint + 2) = { segment.index1 + 1, segment.index2, getRightIntersectionTypes()[2] };
					insertPoint += 3;
				}
				if (hasTop(segment.intersectionType) && segment.index2 < nY)
				{
					*insertPoint = { segment.index1, segment.index2 + 1, getTopIntersectionTypes()[0] };
					*(insertPoint + 1) = { segment.index1, segment.index2 + 1, getTopIntersectionTypes()[1] };
					*(insertPoint + 2) = { segment.index1, segment.index2 + 1, getTopIntersectionTypes()[2] };
					insertPoint += 3;
				}
				if (hasBottom(segment.intersectionType) && segment.index2 > 1)
				{
					*insertPoint = { segment.index1, segment.index2 - 1, getBottomIntersectionTypes()[0] };
					*(insertPoint + 1) = { segment.index1, segment.index2 - 1, getBottomIntersectionTypes()[1] };
					*(insertPoint + 2) = { segment.index1, segment.index2 - 1, getBottomIntersectionTypes()[2] };
					insertPoint += 3;
				}
				return result;
			}

			//follows a contour in either direction removing the elements from segments as it goes.
			//the return value is a vector of ordered segments starting at begin
			//the begin segment will be included in the retuned result
			static std::vector<Segment> followContourEitherDirection(Segment begin, std::set<Segment>& segments, size_t nDim1, size_t nDim2)
			{
				std::vector<Segment> line;

				//check the begin segment actually exists in the segments set
				auto found = segments.find(begin);
				if (found == segments.end())
					return line;

				line.push_back(*found);
				segments.erase(found);

				Segment previousSegment{ size_t(-1), size_t(-1), 0};
				while (1)
				{
					std::array<Segment, 6> linkOptions = getPossibleLinkOptions(line.back(), nDim1, nDim2); // any elements equal to line.back() are a flag for blank
					Segment nextSegment = line.back();
					for (auto& s : linkOptions)
					{
						if (s != line.back() && !(s.index1 == previousSegment.index1 && s.index2 == previousSegment.index2))
						{
							auto found = segments.find(s);
							if (found != segments.end())
							{
								nextSegment = *found;
								segments.erase(found);
								break;
							}
						}
					}
					if (nextSegment != line.back())
					{
						previousSegment = line.back();
						line.push_back(nextSegment);
					}
					else
						break;
				}
				return line;
			}

			static std::vector<Segment> followContourBothDirections(std::set<Segment>& segments, size_t nDim1, size_t nDim2)
			{
				if (segments.size() == 0)
					return std::vector<Segment>(0);

				//create 2 line halves running in opposite directions from the first point
				Segment begin = *segments.begin();
				//remove the 

				std::vector<Segment> line1 = followContourEitherDirection(begin, segments, nDim1, nDim2);
				std::vector<Segment> line2 = followContourEitherDirection(begin, segments, nDim1, nDim2); // this will follow in the other direction as there is nowhere else to go
				segments.erase(begin); // erase the begin point here so that the line will automatically close if it is a loop

				//if line2 has any elements, reverse it and prepend it to line 1, otherwise line 1 is our complete contour
				if (line2.size() > 0)
				{
					line2.reserve(line2.size() + line1.size() - 1);
					std::reverse(line2.begin(), line2.end());
					line2.insert(line2.end(), line1.begin() + 1, line1.end());
					return line2;
				}
				else
					return line1;
			}

			//creates a set of contour segments for each countour level. The sets are in order from the first to last level.
			//Because we use a set, the segments are in ascending order using the < operator defined in the set class
			static std::vector<std::set<Segment>> marchingSquaresSegments(const sci::GridData<Z, 2>& grid, std::span<const Z> contourLevels)
			{
				std::array<size_t, 2> shape = grid.shape();

				//get all the intesections of grids with contours using the marching squares algorithm
				std::vector<std::set<Segment>> segments(contourLevels.size());
				for (size_t i = 0; i < shape[0] - 1; ++i)
				{
					for (size_t j = 0; j < shape[1] - 1; ++j)
					{
						for (size_t k = 0; k < contourLevels.size(); ++k)
						{
							uint8_t intersectionType = disambiguateContours(getIntersectionType(i, j, grid, contourLevels[k]));
							if (intersectionType != 0 && intersectionType != 15 && intersectionType != 5 && intersectionType != 10)
								segments[k].insert(Segment{ i, j, intersectionType, k });
							else if (5 == intersectionType || 16 == intersectionType)
							{
								segments[k].insert(Segment{ i, j, 2, k });
								segments[k].insert(Segment{ i, j, 7, k });
							}
							else if (10 == intersectionType || 17 == intersectionType)
							{
								segments[k].insert(Segment{ i, j, 1, k });
								segments[k].insert(Segment{ i, j, 4, k });
							}
						}
					}
				}
				return segments;
			}

			//creates a set of contour segments for each countour level. The sets are in order from the first to last level.
			//Because we use a set, the segments are in ascending order using the < operator defined in the set class
			std::vector<std::set<Segment>> marchingSquaresSegments(const data::scalesTuple& scales) const
			{
				size_t nLevels = m_levelScale->getNLevels();

				std::array<size_t, 2> shape = this->getShape<2>();

				//get all the intesections of grids with contours using the marching squares algorithm
				std::vector<std::set<Segment>> segments(nLevels);
				for (size_t i = 0; i < shape[0] - 1; ++i)
				{
					for (size_t j = 0; j < shape[1] - 1; ++j)
					{
						for (size_t k = 0; k < nLevels; ++k)
						{
							uint8_t intersectionType = disambiguateContours(getIntersectionType(i, j, k, scales));
							if (intersectionType != 0 && intersectionType != 15 && intersectionType != 5 && intersectionType != 10)
								segments[k].insert(Segment{ i, j, intersectionType, k});
							else if (5 == intersectionType || 16 == intersectionType)
							{
								segments[k].insert(Segment{ i, j, 2, k });
								segments[k].insert(Segment{ i, j, 7, k });
							}
							else if (10 == intersectionType || 17 == intersectionType)
							{
								segments[k].insert(Segment{ i, j, 1, k });
								segments[k].insert(Segment{ i, j, 4, k });
							}
						}
					}
				}
				return segments;
			}

			//creates a set of contour segments for each countour level. The sets are in order from the first to last level.
			//Because we use a set, the segments are in ascending order using the < operator defined in the set class
			static std::vector<std::vector<Segment>> marchingSquaresContour(const sci::GridData<Z, 2>& grid, std::span<const Z> contourLevels)
			{
				std::array<size_t, 2> shape = grid.shape();

				//get all the intesections of grids with contours using the marching squares algorithm
				std::vector<std::set<Segment>> segments(contourLevels.size());
				for (size_t i = 0; i < shape[0] - 1; ++i)
				{
					for (size_t j = 0; j < shape[1] - 1; ++j)
					{
						for (size_t k = 0; k < contourLevels.size(); ++k)
						{
							uint8_t intersectionType = disambiguateContours(getIntersectionType(i, j, grid, contourLevels[k]));
							if (intersectionType != 0 && intersectionType != 15 && intersectionType != 5 && intersectionType != 10)
								segments[k].insert(Segment{ i, j, intersectionType, contourLevels[k] });
							else if (5 == intersectionType || 16 == intersectionType)
							{
								segments[k].insert(Segment{ i, j, 2, contourLevels[k] });
								segments[k].insert(Segment{ i, j, 7, contourLevels[k] });
							}
							else if (10 == intersectionType || 17 == intersectionType)
							{
								segments[k].insert(Segment{ i, j, 1, contourLevels[k] });
								segments[k].insert(Segment{ i, j, 4, contourLevels[k] });
							}
						}
					}
				}


				//link all those segments into lines
				std::vector<std::vector<Segment>> result;
				for (size_t i = 0; i < segments.size(); ++i)
				{
					while (segments[i].size() > 0)
					{
						result.push_back(followContourBothDirections(segments[i], shape[0], shape[1]));
					}
				}
				return result;
			}

			Point getLeftIntersection(size_t index1, size_t index2, size_t contourLevel, const data::spacialAxesSet& axes, const data::scalesTuple& scales) const
			{
				Point p0 = this->getPoint<0, 1>(index1, index2, axes);
				Point p1 = this->getPoint<0, 1>(index1, index2 + 1, axes);
				float weight = this->getWeight<2>(contourLevel, index1, index2, index1, index2 + 1, scales);
				return p0 + (p1 - p0) * unitless(weight);
			}

			Point getTopIntersection(size_t index1, size_t index2, float contourLevel, const data::spacialAxesSet& axes, const data::scalesTuple& scales) const
			{
				Point p0 = this->getPoint<0, 1>(index1, index2 + 1, axes);
				Point p1 = this->getPoint<0, 1>(index1 + 1, index2 + 1, axes);
				float weight = this->getWeight<2>(contourLevel, index1, index2 + 1, index1 + 1, index2 + 1, scales);
				return p0 + (p1 - p0) * unitless(weight);
			}

			Point getRightIntersection(size_t index1, size_t index2, float contourLevel, const data::spacialAxesSet& axes, const data::scalesTuple& scales)const
			{
				Point p0 = this->getPoint<0, 1>(index1 + 1, index2 + 1, axes);
				Point p1 = this->getPoint<0, 1>(index1 + 1, index2, axes);
				float weight = this->getWeight<2>(contourLevel, index1 + 1, index2 + 1, index1 + 1, index2, scales);
				return p0 + (p1 - p0) * unitless(weight);
			}

			Point getBottomIntersection(size_t index1, size_t index2, float contourLevel, const data::spacialAxesSet& axes, const data::scalesTuple &scales) const
			{
				Point p0 = this->getPoint<0, 1>(index1 + 1, index2, axes);
				Point p1 = this->getPoint<0, 1>(index1, index2, axes);
				float weight = this->getWeight<2>(contourLevel, index1 + 1, index2, index1, index2, scales);
				return p0 + (p1 - p0) * unitless(weight);
			}

			Point getJoiningPoint(const Segment& segment1, const Segment& segment2, const data::spacialAxesSet& axes, const data::scalesTuple& scales) const
			{
				size_t contourLevel = segment1.level;
				if ((segment2.index1 == segment1.index1 + 1)) //right join
					return getRightIntersection(segment1.index1, segment1.index2, contourLevel, axes, scales);
				else if (segment2.index1 == segment1.index1 - 1) //left join
					return getLeftIntersection(segment1.index1, segment1.index2, contourLevel, axes, scales);
				else if (segment2.index2 == segment1.index2 + 1) //top join
					return getTopIntersection(segment1.index1, segment1.index2, contourLevel, axes, scales);
				else if (segment2.index2 == segment1.index2 - 1) //bottom join
					return getBottomIntersection(segment1.index1, segment1.index2, contourLevel, axes, scales);
				throw("Attempted to get the non-joining point of two contour segments that are not joined.");
			}

			Point getSegment1NonJoiningPoint(const Segment& segment1, const Segment& segment2, const data::spacialAxesSet& axes, const data::scalesTuple& scales) const
			{
				size_t contourLevel = segment1.level;
				if ((segment2.index1 == segment1.index1 + 1)) //right join, get left
					return getLeftIntersection(segment1.index1, segment1.index2, contourLevel, axes, scales);
				else if (segment2.index1 == segment1.index1 - 1) //left join, get right
					return getRightIntersection(segment1.index1, segment1.index2, contourLevel, axes, scales);
				else if (segment2.index2 == segment1.index2 + 1) //top join, get bottom
					return getBottomIntersection(segment1.index1, segment1.index2, contourLevel, axes, scales);
				else if (segment2.index2 == segment1.index2 - 1) //bottom join, get top
					return getTopIntersection(segment1.index1, segment1.index2, contourLevel, axes, scales);
				throw("Attempted to get a joining point of two contour segments that are not joined.");
			}

			void getBothPoints(const Segment& segment, std::vector<Point>& points, const data::spacialAxesSet& axes, const data::scalesTuple& scales) const
			{
				points.resize(2);
				auto iter = points.begin();
				size_t contourLevel = segment.level;
				if (hasLeft(segment.intersectionType)) //left
				{
					*iter = getLeftIntersection(segment.index1, segment.index2, contourLevel, axes, scales);
					++iter;
				}
				if (hasTop(segment.intersectionType)) //top
				{
					*iter = getTopIntersection(segment.index1, segment.index2, contourLevel, axes, scales);
					++iter;
				}
				if (hasRight(segment.intersectionType)) //right
				{
					*iter = getRightIntersection(segment.index1, segment.index2, contourLevel, axes, scales);
					++iter;
				}
				if (hasBottom(segment.intersectionType)) //bottom
				{
					*iter = getBottomIntersection(segment.index1, segment.index2, contourLevel, axes, scales);
					++iter;
				}
			}

			std::vector<Point> getContourLine(const std::vector<Segment>& segments, const data::spacialAxesSet& axes, const data::scalesTuple& scales) const
			{
				if (segments.size() == 0)
					return std::vector<Point>(0);
				
				std::vector<Point> result(segments.size() + 1);

				if (segments.size() == 1)
					getBothPoints(segments[0], result, axes, scales);
				else
				{
					for (size_t j = 0; j < segments.size() - 1; ++j)
						result[j+1] =  getJoiningPoint(segments[j], segments[j + 1], axes, scales); //this omits the last and first point
					//sort the last and first point
					result[0] = getSegment1NonJoiningPoint(segments[0], segments[1], axes, scales);
					result.back() = getSegment1NonJoiningPoint(segments.back(), segments[segments.size() - 2], axes, scales);
				}
				return result;
			}

			std::shared_ptr<ColourScale<Z>> m_colourscale;
			std::shared_ptr<LevelScale<Z, float>> m_levelScale;
			LineStyle m_lineStyle;
		};



		/*template<class X, class Y, class Z, class XCONTAINER, class YCONTAINER, class ZGRID>
		auto makeContours(const XCONTAINER& xs, const YCONTAINER& ys, const ZGRID& zs, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, std::shared_ptr<ColourScale<Z>> colourScale, const LineStyle& lineStyle)
		{
			constexpr int Dimensions1 = getPlotableNDims<XCONTAINER, X>();
			constexpr int Dimensions2 = getPlotableNDims<YCONTAINER, Y>();
			static_assert (XYZPlotable<Dimensions1, Dimensions2, 2, XCONTAINER, YCONTAINER, ZGRID, X, Y, Z>, "Attempted to create a sci::plot::Contours with data that is not plotable.");
			return std::make_shared<Contours<Dimensions1, Dimensions2, X, Y, Z>>(xs, ys, zs, xAxis, yAxis, colourScale, lineStyle);
		}*/

		template<class X, class Y, class Z, class XCONTAINER, class YCONTAINER, class ZGRID>
		auto makeContours(const XCONTAINER& xs, const YCONTAINER& ys, const ZGRID& zs, std::shared_ptr<Axis<X>> xAxis, std::shared_ptr<Axis<Y>> yAxis, std::shared_ptr<LevelScale<Z, float>> levelScale, const LineStyle& lineStyle)
		{
			constexpr int Dimensions1 = getPlotableNDims<XCONTAINER, X>();
			constexpr int Dimensions2 = getPlotableNDims<YCONTAINER, Y>();
			static_assert (XYZPlotable<Dimensions1, Dimensions2, 2, XCONTAINER, YCONTAINER, ZGRID, X, Y, Z>, "Attempted to create a sci::plot::Contours with data that is not plotable.");
			return std::make_shared<Contours<Dimensions1, Dimensions2, X, Y, Z>>(xs, ys, zs, xAxis, yAxis, levelScale, lineStyle);
		}
	}
}


#endif
