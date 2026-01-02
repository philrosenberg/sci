#ifndef SCI_PLOT_COLOURBAR_H
#define SCI_PLOT_COLOURBAR_H

#include"gridContour.h"

namespace sci
{
	namespace plot
	{

		template<class T>
		class HorizontalColourBar : public DrawableItem
		{
		public:
			HorizontalColourBar(Point bottomLeft, Point topRight, std::shared_ptr<ColourScale<T>> colourscale, typename Axis<T>::Options axisOptions = Axis<T>::Options())
				:m_colourscale(colourscale),
				m_xAxis(new Axis<T>(m_colourscale->getLinearMin(), m_colourscale->getLinearMax(), m_colourscale->isLog(), bottomLeft, Point(topRight.getX(), bottomLeft.getY()), axisOptions, sci::plot::Scale<double>::Direction::horizontal)),
				m_yAxis(new Axis<double>(0.0, 1.0, false, bottomLeft, Point(bottomLeft.getX(), topRight.getY()), Axis<double>::Options::getBlankAxis(), sci::plot::Scale<double>::Direction::horizontal))
			{
			}
			void preDraw() override
			{
			}
			void draw(Renderer& renderer, perMillimetre scale) override
			{
				using unitless_type = decltype(T() / T());

				if (m_colourscale->isDiscrete())
				{
					/*sci::GridData<T, 2> cb({2, 2});
					cb[0][0] = m_colourscale->getLinearMin();
					cb[0][1] = cb[0][0];
					cb[1][0] = m_colourscale->getLinearMax();
					cb[1][1] = cb[1][0];
					std::vector<T> cbX{ cb[0][0], cb[1][0] };
					std::vector<double> cbY{ 0.0, 1.0 };

					Contours<1, 1, double, double, T> data(cbX, cbY, cb, m_xAxis, m_yAxis, m_colourscale, noLine);

					data.draw(renderer, scale);
					m_xAxis->draw(renderer, scale);*/
				}
				else
				{
					sci::GridData<T, 2> z({ 256, 1 });
					std::vector<T> cbX(z.shape()[0] + 1);



					if (m_colourscale->isLog())
					{
						auto min = m_colourscale->getLogMin();
						auto max = m_colourscale->getLogMax();
						auto range = max - min;
						auto step = range / unitless_type(z.shape()[0]);

						for (size_t i = 0; i < cbX.size(); ++i)
							cbX[i] = T(1.0) * sci::pow(unitless_type(10), min + unitless_type(i) * step);

						for (size_t i = 0; i < z.size(); ++i)
							z[i][0] = T(1.0) * sci::pow(unitless_type(10.0), (min + unitless_type(i + 0.5) * step));
					}
					else
					{
						auto min = m_colourscale->getLinearMin();
						auto max = m_colourscale->getLinearMax();
						auto range = max - min;
						auto step = range / unitless_type(z.size());

						for (size_t i = 0; i < cbX.size(); ++i)
							cbX[i] = min + unitless_type(i) * step;

						for (size_t i = 0; i < z.size(); ++i)
							z[i][0] = (cbX[i] + cbX[i + 1]) / unitless_type(2.0);
					}

					std::vector<double> cbY{ 0.0, 1.0 };

					Grid<1, 1, double, double, T> data(cbX, cbY, z, m_xAxis, m_yAxis, m_colourscale);

					data.draw(renderer, scale);
					m_xAxis->draw(renderer, scale);
				}
			}
			bool readyToDraw() const override
			{
				return true;
			}
		private:
			std::shared_ptr<ColourScale<T>> m_colourscale;
			std::shared_ptr<Axis<T>> m_xAxis;
			std::shared_ptr<Axis<double>> m_yAxis;

		};

		template<class T>
		auto makeHorizontalColourBar(Point bottomLeft, Point topRight, std::shared_ptr<ColourScale<T>> colourscale, typename Axis<T>::Options axisOptions = Axis<T>::Options())
		{
			return std::make_shared< HorizontalColourBar<T>>(bottomLeft, topRight, colourscale, axisOptions);
		}
	}
}

#endif