#ifndef SVECTOR_PLOTGRIDCONTOURS_H
#define SVECTOR_PLOTGRIDCONTOURS_H

#include "plotable.h"
#include <set>


namespace sci
{
	namespace plot
	{
		class Data2d : public UnstructuredData, public StructuredData
		{
		public:
			Data2d(std::span<const double> xs, std::span<const double> ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<Scale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs, ys }, { xAxis, yAxis }, transformer), StructuredData({ &zs }, { zScale }, transformer)
			{
				sci::assertThrow(zs.shape()[0] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));
				sci::assertThrow(zs.shape()[1] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

				m_x1d = true;
				m_y1d = true;
				m_xNBoxes = zs.shape()[0];
				m_yNBoxes = zs.shape()[1];
				m_xAxis = xAxis;
				m_yAxis = yAxis;
			}

			Data2d(const sci::GridData<double, 2>& xs, const sci::GridData<double, 2>& ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<Scale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({}, {}, transformer), StructuredData({ &xs, &ys, &zs }, { xAxis, yAxis, zScale }, transformer)
			{
				sci::assertThrow(zs.shape()[0] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));
				sci::assertThrow(zs.shape()[1] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

				m_x1d = false;
				m_y1d = false;
				m_xNBoxes = zs.shape()[0];
				m_yNBoxes = zs.shape()[1];
				m_xAxis = xAxis;
				m_yAxis = yAxis;
			}
			
			Data2d(std::span<const double> xs, const sci::GridData<double, 2>& ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<Scale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ xs }, { xAxis }, transformer), StructuredData({ &ys, &zs }, { yAxis, zScale }, transformer)
			{
				sci::assertThrow(zs.shape()[0] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));
				sci::assertThrow(zs.shape()[1] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));

				m_x1d = true;
				m_y1d = false;
				m_xNBoxes = zs.shape()[0];
				m_yNBoxes = zs.shape()[1];
				m_xAxis = xAxis;
				m_yAxis = yAxis;
			}
			
			Data2d(const sci::GridData<double, 2>& xs, std::span<const double> ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<Scale> zScale, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), UnstructuredData({ ys }, { yAxis }, transformer), StructuredData({ &xs, &zs }, { xAxis, zScale }, transformer)
			{
				sci::assertThrow(zs.shape()[0] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));
				sci::assertThrow(zs.shape()[1] > 0, sci::err(sci::SERR_PLOT, plotDataErrorCode, "Data2d constructor called with zs of zero length."));


				m_x1d = false;
				m_y1d = true;
				m_xNBoxes = zs.shape()[0];
				m_yNBoxes = zs.shape()[1];
				m_xAxis = xAxis;
				m_yAxis = yAxis;
			}

			virtual void autoscaleAxes() override//because this class inherits this function from both UnstructuredData and StructuredData, it must override in this class
			{
				UnstructuredData::autoscaleAxes();
				StructuredData::autoscaleAxes();
			}

			const sci::GridData<double, 2>& getZ() const
			{
				if ((!m_x1d) && (!m_y1d))
					return getGrid(2);
				if ((!m_x1d) || (!m_y1d))
					return getGrid(1);
				return getGrid(0);
			}
		protected:
			bool m_x1d;
			bool m_y1d;
			size_t m_xNBoxes;
			size_t m_yNBoxes;
			std::shared_ptr<Axis> m_xAxis;
			std::shared_ptr<Axis> m_yAxis;
		};

		class Grid : public Data2d
		{
		public:
			Grid(std::span<const double> xs, std::span<const double> ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<ColourScale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
			{
				sci::assertThrow(xs.size() == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
				sci::assertThrow(ys.size() == zs.shape()[1] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));

				m_colourscale = colourScale;
			}
			
			Grid(const sci::GridData<double, 2>& xs, const sci::GridData<double, 2>& ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<ColourScale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr)
			: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
			{
				sci::assertThrow(xs.shape()[0] == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
				sci::assertThrow(ys.shape()[0] == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));

				sci::assertThrow(xs.shape()[1] == zs.shape()[1] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
				sci::assertThrow(ys.shape()[1] == zs.shape()[1] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));


				m_colourscale = colourScale;
			}

			Grid(std::span<const double> xs, const sci::GridData<double, 2>& ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<ColourScale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
			{
				sci::assertThrow(xs.size() == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
				sci::assertThrow(ys.shape()[0] == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));
				sci::assertThrow(ys.shape()[1] == zs.shape()[1] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));


				m_colourscale = colourScale;
			}

			Grid(const sci::GridData<double, 2>& xs, std::span<const double> ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<ColourScale> colourScale, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
			{
				sci::assertThrow(xs.shape()[0] == zs.shape()[0] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
				sci::assertThrow(xs.shape()[1] == zs.shape()[1] + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with xs with a size 1 larger than zs."));
				sci::assertThrow(ys.size() == zs[1].size() + 1, sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor must be called with ys with a size 1 larger than zs."));

				m_colourscale = colourScale;
			}
			
			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				if (!StructuredData::hasData())
					return;

				renderer.setPen(rgbcolour(), Length(grMillimetre(0.0)));

				//get the limits of the clourscale - this will get either the linear or logged min
				//as appropriate
				double colourscaleMin = m_colourscale->getLinearOrLogMin();
				double colourscaleMax = m_colourscale->getLinearOrLogMax();

				//either limit the data we plot to within the colourscale or off the limits as appropriate
				double zMin = colourscaleMin;
				double zMax = colourscaleMax;
				if (m_colourscale->fillOffscaleBottom())
					zMin = -std::numeric_limits<double>::infinity();
				if (m_colourscale->fillOffscaleTop())
					zMax = std::numeric_limits<double>::infinity();


				if (m_x1d && m_y1d)
				{
					const double* x = UnstructuredData::getPointer(0);
					const double* y = UnstructuredData::getPointer(1);
					std::vector<const double*> zs = StructuredData::getPointer(0); //returns logged data if it's a log z axis
					for (size_t i = 0; i < m_xNBoxes; ++i)
					{
						for (size_t j = 0; j < m_yNBoxes; ++j)
						{
							if (zs[i][j] < zMin)
								continue;
							if (zs[i][j] > zMax)
								continue;
							renderer.setBrush(m_colourscale->getRgbOriginalScale(zs[i][j], true));
							renderer.rectangle(getPointFromLoggedIfNeededData(x[i], y[j]), getPointFromLoggedIfNeededData(x[i + 1], y[j + 1]));
						}
					}
				}
				else if (m_x1d)
				{
					const double* x = UnstructuredData::getPointer(0);
					std::vector<const double*> y = StructuredData::getPointer(0);
					std::vector<const double*> zs = StructuredData::getPointer(1); //returns logged data if it's a log z axis
					for (size_t i = 0; i < m_xNBoxes; ++i)
					{
						for (size_t j = 0; j < m_yNBoxes; ++j)
						{
							if (zs[i][j] < zMin)
								continue;
							if (zs[i][j] > zMax)
								continue;
							renderer.setBrush(m_colourscale->getRgbOriginalScale(zs[i][j], true));
							renderer.polygon({ getPointFromLoggedIfNeededData(x[i], y[i][j]), getPointFromLoggedIfNeededData(x[i + 1], y[i + 1][j]),
								getPointFromLoggedIfNeededData(x[i + 1], y[i + 1][j + 1]), getPointFromLoggedIfNeededData(x[i], y[i][j + 1]),
								getPointFromLoggedIfNeededData(x[i], y[i][j]) });
						}
					}
				}
				else if (m_y1d)
				{
					std::vector<const double*> x = StructuredData::getPointer(0);
					const double* y = UnstructuredData::getPointer(0);
					std::vector<const double*> zs = StructuredData::getPointer(1);
					for (size_t i = 0; i < m_xNBoxes; ++i)
					{
						for (size_t j = 0; j < m_yNBoxes; ++j)
						{
							if (zs[i][j] < zMin)
								continue;
							if (zs[i][j] > zMax)
								continue;
							renderer.setBrush(m_colourscale->getRgbOriginalScale(zs[i][j], true));
							renderer.polygon({ getPointFromLoggedIfNeededData(x[i][j], y[j]), getPointFromLoggedIfNeededData(x[i + 1][j], y[j]),
								getPointFromLoggedIfNeededData(x[i + 1][j + 1], y[j + 1]), getPointFromLoggedIfNeededData(x[i][j + 1], y[j + 1]),
								getPointFromLoggedIfNeededData(x[i][j], y[j]) });
						}
					}
				}
				else
				{
					std::vector<const double*> x = StructuredData::getPointer(0);
					std::vector<const double*> y = StructuredData::getPointer(1);
					std::vector<const double*> zs = StructuredData::getPointer(2);
					for (size_t i = 0; i < m_xNBoxes; ++i)
					{
						for (size_t j = 0; j < m_yNBoxes; ++j)
						{
							if (zs[i][j] < zMin)
								continue;
							if (zs[i][j] > zMax)
								continue;
							renderer.setBrush(m_colourscale->getRgbOriginalScale(zs[i][j], true));
							renderer.polygon({ getPointFromLoggedIfNeededData(x[i][j], y[i][j]), getPointFromLoggedIfNeededData(x[i + 1][j], y[i + 1][j]),
								getPointFromLoggedIfNeededData(x[i + 1][j + 1], y[i + 1][j + 1]), getPointFromLoggedIfNeededData(x[i][j + 1], y[i][j + 1]),
								getPointFromLoggedIfNeededData(x[i][j], y[i][j]) });
						}
					}
				}
			}
		private:
			std::shared_ptr<ColourScale> m_colourscale;
		};

		class Contours : public Data2d
		{
		public:
			Contours(std::span<const double> xs, std::span<const double> ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<ColourScale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
			: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
			{
				sci::assertThrow(xs.size() == zs.shape()[0], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
				sci::assertThrow(ys.size() == zs.shape()[1], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

				m_colourscale = colourScale;
				m_levelScale = nullptr;
				m_lineStyle = lineStyle;
			}

			Contours(const sci::GridData<double, 2>& xs, const sci::GridData<double, 2>& ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<ColourScale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
			{
				sci::assertThrow(xs.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
				sci::assertThrow(ys.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

				m_colourscale = colourScale;
				m_levelScale = nullptr;
				m_lineStyle = lineStyle;
			}
			
			Contours(std::span<const double> xs, const sci::GridData<double, 2>& ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<ColourScale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
			{
				sci::assertThrow(xs.size() == zs.shape()[0], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
				sci::assertThrow(ys.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

				m_colourscale = colourScale;
				m_levelScale = nullptr;
				m_lineStyle = lineStyle;
			}

			Contours(const sci::GridData<double, 2>& xs, std::span<const double> ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<ColourScale> colourScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, colourScale, transformer)
			{
				sci::assertThrow(xs.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
				sci::assertThrow(ys.size() == zs.shape()[1], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

				m_colourscale = colourScale;
				m_levelScale = nullptr;
				m_lineStyle = lineStyle;
			}

			Contours(std::span<const double> xs, std::span<const double> ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<LevelScale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
			{
				sci::assertThrow(xs.size() == zs.shape()[0], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
				sci::assertThrow(ys.size() == zs.shape()[1], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

				m_colourscale = nullptr;
				m_levelScale = levelScale;
				m_lineStyle = lineStyle;
			}

			Contours(const sci::GridData<double, 2>& xs, const sci::GridData<double, 2>& ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<LevelScale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
			{
				sci::assertThrow(xs.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
				sci::assertThrow(ys.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

				m_colourscale = nullptr;
				m_levelScale = levelScale;
				m_lineStyle = lineStyle;
			}

			Contours(std::span<const double> xs, const sci::GridData<double, 2>& ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<LevelScale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
			{
				sci::assertThrow(xs.size() == zs.shape()[0], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
				sci::assertThrow(ys.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

				m_colourscale = nullptr;
				m_levelScale = levelScale;
				m_lineStyle = lineStyle;
			}

			Contours(const sci::GridData<double, 2>& xs, std::span<const double> ys, const sci::GridData<double, 2>& zs, std::shared_ptr<Axis> xAxis, std::shared_ptr<Axis> yAxis, std::shared_ptr<LevelScale> levelScale, const LineStyle& lineStyle, std::shared_ptr<splotTransformer> transformer = nullptr)
				: PlotableItem(xAxis, yAxis, transformer), Data2d(xs, ys, zs, xAxis, yAxis, levelScale, transformer)
			{
				sci::assertThrow(xs.shape() == zs.shape(), sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with xs and zs of different lengths."));
				sci::assertThrow(ys.size() == zs.shape()[1], sci::err(sci::SERR_PLOT, plotDataErrorCode, "GridData constructor called with ys and zs of different lengths."));

				m_colourscale = nullptr;
				m_levelScale = levelScale;
				m_lineStyle = lineStyle;
			}

			void plotData(Renderer& renderer, grPerMillimetre scale) const override
			{
				sci::GridData<double, 1> contourLevels;
				if (m_levelScale)
				{
					contourLevels = m_levelScale->getLevels();
					sci::assertThrow(contourLevels.size() > 1, sci::err(sci::SERR_PLOT, 0, "ContourData::plotData: Cannot use a level scale with fewer than 2 levels."));
				}

				//get the segments of each contour line using the marching squares algorithm.
				//note that this just gives the edges of the grid boxes that each segment touches
				const sci::GridData<double, 2>& zs = getZ();
				std::array<size_t, 2> shape = zs.shape();
				std::vector<std::set<Segment>> segmentsPerLevel = marchingSquaresSegments(zs, contourLevels);

				//render the fill

				/*std::vector<std::set<Segment>::iterator> segmentIters(segmentsPerLevel.size());
				for (size_t i = 0; i < segmentsPerLevel.size(); ++i)
					segmentIters[i] = segmentsPerLevel[i].begin();

				std::vector<std::set<Segment>::iterator> intersections;
				for (size_t i = 0; i < shape[0]-1; ++i)
				{
					for (size_t j = 0; j < shape[1]-1; ++j)
					{
						double x0;
						double x1;
						double y0;
						double y1;
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


				//render the contours
				// 
				//this is destructive - it moves segments from the segmentsPerLevel variable to the segments variable
				std::vector<std::vector<Segment>> segments;
				for (size_t i = 0; i < segmentsPerLevel.size(); ++i)
				{
					while (segmentsPerLevel[i].size() > 0)
					{
						segments.push_back(followContourBothDirections(segmentsPerLevel[i], shape[0], shape[1]));
					}
				}



				//now work out where the contours cross each grid box edge
				std::vector<double> xs;
				std::vector<double> ys;
				std::vector<Point> points;
				if (m_x1d && m_y1d)
				{
					const std::vector<double>& inputXs = getVector(0);
					const std::vector<double>& inputYs = getVector(1);
					for (size_t i = 0; i < segments.size(); ++i)
					{
						getContourLine(inputXs, inputYs, zs, segments[i], xs, ys);

						m_lineStyle.setPen(renderer);
						points.resize(xs.size());
						for (size_t j = 0; j < points.size(); ++j)
							points[j] = getPointFromLoggedIfNeededData(xs[j], ys[j]);
						renderer.polyLine(points);
					}
				}
				else if ((!m_x1d) && (!m_y1d))
				{
					const sci::GridData<double, 2>& inputXs = getGrid(0);
					const sci::GridData<double, 2>& inputYs = getGrid(1);
					for (size_t i = 0; i < segments.size(); ++i)
					{
						getContourLine(inputXs, inputYs, zs, segments[i], xs, ys);

						m_lineStyle.setPen(renderer);
						points.resize(xs.size());
						for (size_t j = 0; j < points.size(); ++j)
							points[j] = getPointFromLoggedIfNeededData(xs[j], ys[j]);
						renderer.polyLine(points);
					}
				}
				else if (!m_x1d)
				{
					const sci::GridData<double, 2>& inputXs = getGrid(0);
					const std::vector<double>& inputYs = getVector(0);
					for (size_t i = 0; i < segments.size(); ++i)
					{
						getContourLine(inputXs, inputYs, zs, segments[i], xs, ys);

						m_lineStyle.setPen(renderer);
						points.resize(xs.size());
						for (size_t j = 0; j < points.size(); ++j)
							points[j] = getPointFromLoggedIfNeededData(xs[j], ys[j]);
						renderer.polyLine(points);
					}
				}
				else
				{
					const std::vector<double>& inputXs = getVector(0);
					const sci::GridData<double, 2>& inputYs = getGrid(0);
					for (size_t i = 0; i < segments.size(); ++i)
					{
						getContourLine(inputXs, inputYs, zs, segments[i], xs, ys);

						m_lineStyle.setPen(renderer);
						points.resize(xs.size());
						for (size_t j = 0; j < points.size(); ++j)
							points[j] = getPointFromLoggedIfNeededData(xs[j], ys[j]);
						renderer.polyLine(points);
					}
				}

			}
		private:

			struct Segment
			{
				size_t index1;
				size_t index2;
				uint8_t intersectionType;
				double level;
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

			static uint8_t getIntersectionType(size_t i, size_t j, const sci::GridData<double, 2>& grid, double contourLevel)
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
			static std::vector<std::set<Segment>> marchingSquaresSegments(const sci::GridData<double, 2>& grid, std::span<const double> contourLevels)
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
				return segments;
			}

			//creates a set of contour segments for each countour level. The sets are in order from the first to last level.
			//Because we use a set, the segments are in ascending order using the < operator defined in the set class
			static std::vector<std::vector<Segment>> marchingSquaresContour(const sci::GridData<double, 2>& grid, std::span<const double> contourLevels)
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

			template <bool isDim1>
			static std::array<std::array<double, 2>, 2> getCornerValues(size_t index1, size_t index2, const std::vector<double>& v)
			{
				if constexpr (isDim1)
					return{ std::array<double, 2>{v[index1], v[index1]},
							std::array<double, 2>{v[index1 + 1], v[index1 + 1]} };
				else
					return{ std::array<double, 2>{v[index2], v[index2 + 1]},
							std::array<double, 2>{v[index2], v[index2 + 1]} };
			}

			template <bool isDim1>
			static std::array<std::array<double, 2>, 2> getCornerValues(size_t index1, size_t index2, const sci::GridData<double, 2>& g)
			{
				//isDim1 is only there to match the function above and isn't used
				return{ std::array<double, 2>{g[index1][index2], g[index1][index2 + 1]},
						std::array<double, 2>{g[index1 + 1][index2], g[index1 + 1][index2 + 1]} };

			}

			template <class DATAX, class DATAY, class DATAZ>
			static void getJoiningPoint(const DATAX& allXs, const DATAY& allYs, const DATAZ& allZs,
				const Segment& segment1, const Segment& segment2, double& x, double& y)
			{
				std::array<std::array<double, 2>, 2> xs = getCornerValues<true>(segment1.index1, segment1.index2, allXs);
				std::array<std::array<double, 2>, 2> ys = getCornerValues<false>(segment1.index1, segment1.index2, allYs);
				std::array<std::array<double, 2>, 2> zs = getCornerValues<true>(segment1.index1, segment1.index2, allZs);
				const double& contourLevel = segment1.level;
				if (segment2.index1 == segment1.index1 + 1) //right
				{
					double weight = (contourLevel - zs[1][0]) / (contourLevel - zs[1][1]);
					x = xs[1][0] + weight * (xs[1][1] - xs[1][0]);
					y = ys[1][0] + weight * (ys[1][1] - ys[1][0]);
				}
				else if (segment2.index1 == segment1.index1 - 1) //left
				{
					double weight = (contourLevel - zs[0][0]) / (contourLevel - zs[0][1]);
					x = xs[0][0] + weight * (xs[0][1] - xs[0][0]);
					y = ys[0][0] + weight * (ys[0][1] - ys[0][0]);
				}
				else if (segment2.index2 == segment1.index2 + 1) //top
				{
					double weight = (contourLevel - zs[0][1]) / (contourLevel - zs[1][1]);
					x = xs[0][1] + weight * (xs[1][1] - xs[0][1]);
					y = ys[0][1] + weight * (ys[1][1] - ys[0][1]);
				}
				else if (segment2.index2 == segment1.index2 - 1) //bottom
				{
					double weight = (contourLevel - zs[0][0]) / (contourLevel - zs[1][0]);
					x = xs[0][0] + weight * (xs[1][0] - xs[0][0]);
					y = ys[0][0] + weight * (ys[1][0] - ys[0][0]);
				}
				else
					throw("some contouring error happened that indicates a bug");
			}

			template <class DATAX, class DATAY, class DATAZ>
			static void getSegment1NonJoiningPoint(const DATAX& allXs, const DATAY& allYs, const DATAZ& allZs,
				const Segment& segment1, const Segment& segment2, double& x, double& y)
			{
				std::array<std::array<double, 2>, 2> xs = getCornerValues<true>(segment1.index1, segment1.index2, allXs);
				std::array<std::array<double, 2>, 2> ys = getCornerValues<false>(segment1.index1, segment1.index2, allYs);
				std::array<std::array<double, 2>, 2> zs = getCornerValues<true>(segment1.index1, segment1.index2, allZs);
				const double& contourLevel = segment1.level;
				if ((segment2.index1 == segment1.index1 + 1)) //right join, get left
				{
					double weight = (contourLevel - zs[0][0]) / (zs[0][1] - zs[0][0]);
					x = xs[0][0] + weight * (xs[0][1] - xs[0][0]);
					y = ys[0][0] + weight * (ys[0][1] - ys[0][0]);
				}
				else if (segment2.index1 == segment1.index1 - 1) //left join, get right
				{
					double weight = (contourLevel - zs[1][0]) / (zs[1][1] - zs[1][0]);
					x = xs[1][0] + weight * (xs[1][1] - xs[1][0]);
					y = ys[1][0] + weight * (ys[1][1] - ys[1][0]);
				}
				else if (segment2.index2 == segment1.index2 + 1) //top join, get bottom
				{
					double weight = (contourLevel - zs[0][0]) / (zs[1][0] - zs[0][0]);
					x = xs[0][0] + weight * (xs[1][0] - xs[0][0]);
					y = ys[0][0] + weight * (ys[1][0] - ys[0][0]);
				}
				else if (segment2.index2 == segment1.index2 - 1) //bottom join, get top
				{
					double weight = (contourLevel - zs[0][1]) / (zs[1][1] - zs[0][1]);
					x = xs[0][1] + weight * (xs[1][1] - xs[0][1]);
					y = ys[0][1] + weight * (ys[1][1] - ys[0][1]);
				}
			}

			template <class DATAX, class DATAY, class DATAZ>
			static void getBothPoints(const DATAX& allXs, const DATAY& allYs, const DATAZ& allZs,
				const Segment& segment1, std::vector<double>& x, std::vector<double>& y)
			{
				std::array<std::array<double, 2>, 2> xs = getCornerValues<true>(segment1.index1, segment1.index2, allXs);
				std::array<std::array<double, 2>, 2> ys = getCornerValues<false>(segment1.index1, segment1.index2, allYs);
				std::array<std::array<double, 2>, 2> zs = getCornerValues<true>(segment1.index1, segment1.index2, allZs);
				x.resize(0);
				y.resize(0);
				x.resize(2);
				y.resize(2);
				auto xIter = x.begin();
				auto yIter = y.begin();
				const double& contourLevel = segment1.level;
				if (hasRight(segment1.intersectionType)) //right
				{
					double weight = (contourLevel - zs[1][0]) / (zs[1][1] - zs[1][0]);
					*xIter = xs[1][0] + weight * (xs[1][1] - xs[1][0]);
					*yIter = ys[1][0] + weight * (ys[1][1] - ys[1][0]);
					++xIter;
					++yIter;
				}
				if (hasLeft(segment1.intersectionType)) //left
				{
					double weight = (contourLevel - zs[0][0]) / (zs[0][1] - zs[0][0]);
					*xIter = xs[0][0] + weight * (xs[0][1] - xs[0][0]);
					*yIter = ys[0][0] + weight * (ys[0][1] - ys[0][0]);
					++xIter;
					++yIter;
				}
				if (hasTop(segment1.intersectionType)) //top
				{
					double weight = (contourLevel - zs[0][1]) / (zs[1][1] - zs[0][1]);
					*xIter = xs[0][1] + weight * (xs[1][1] - xs[0][1]);
					*yIter = ys[0][1] + weight * (ys[1][1] - ys[0][1]);
					++xIter;
					++yIter;
				}
				if (hasBottom(segment1.intersectionType)) //bottom
				{
					double weight = (contourLevel - zs[0][0]) / (zs[1][0] - zs[0][0]);
					*xIter = xs[0][0] + weight * (xs[1][0] - xs[0][0]);
					*yIter = ys[0][0] + weight * (ys[1][0] - ys[0][0]);
					++xIter;
					++yIter;
				}
			}

			template <class DATAX, class DATAY, class DATAZ>
			static void getContourLine(const DATAX& allXs, const DATAY& allYs, const DATAZ& allZs,
				const std::vector<Segment>& segments, std::vector<double>& xs, std::vector<double>& ys)
			{
				if (segments.size() == 0)
				{
					xs.resize(0);
					ys.resize(0);
					return;
				}
				xs.resize(segments.size() + 1);
				ys.resize(segments.size() + 1);
				if (segments.size() == 1)
					getBothPoints(allXs, allYs, allZs, segments[0], xs, ys);
				else
				{
					for (size_t j = 0; j < segments.size() - 1; ++j)
						getJoiningPoint(allXs, allYs, allZs, segments[j], segments[j + 1], xs[j + 1], ys[j + 1]); //this omits the last and first point
					//sort the last and first point
					getSegment1NonJoiningPoint(allXs, allYs, allZs, segments[0], segments[1], xs[0], ys[0]);
					getSegment1NonJoiningPoint(allXs, allYs, allZs, segments.back(), segments[segments.size() - 2], xs.back(), ys.back());
				}
			}


			std::shared_ptr<ColourScale> m_colourscale;
			std::shared_ptr<LevelScale> m_levelScale;
			LineStyle m_lineStyle;
		};
	}
}


#endif
