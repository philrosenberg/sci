#ifndef splot_h
#define splot_h


#include"../string.h"
#include"../graphics.h"
#include"../grid.h"
#include"../gridtransformview.h"
#include<algorithm>


namespace sci
{
	namespace plot
	{

		const int colourscaleErrorCode = 2;
		const int sizescaleErrorCode = 3;
		const int splot2dErrorCode = 4;


		template<class T>
		class Scale
		{
		public:
			using value_type = T;
			using unitless_type = decltype(T() / T());
			using log_type = decltype(sci::log10(unitless_type()));
			using value_type_squared = decltype(T()* T());

			enum class Direction
			{
				horizontal,
				vertical,
				none
			};

			Scale(value_type min, value_type max, bool log, Direction direction)
			{
				m_direction = direction;
				m_log = log;
				setFixedScale(min, max);
			}
			Scale(bool log, Direction direction, unitless_type autoscaleEndSpace)
			{
				m_direction = direction;
				m_autoscale = true;
				m_log = log;
				m_autoscaleEndSpace = autoscaleEndSpace;
				contract();
			}
			Scale(Direction direction, unitless_type autoscaleEndSpace)
			{
				m_direction = direction;
				m_autoscale = true;
				m_log = false;
				m_autoscaleEndSpace = autoscaleEndSpace;
				contract();
			}
			virtual ~Scale() {}
			/*value_type getLinearOrLogMin() const
			{
				return m_log ? getLogMin() : getLinearMin();
			}
			double getLinearOrLogMax() const
			{
				return m_log ? getLogMax() : getLinearMax();
			}*/
			value_type getLinearMin() const
			{
				if (!m_autoscale)
					return m_minFixed;
				value_type min = m_minPoint - m_autoscaleEndSpace * (m_maxPoint - m_minPoint);
				//if all the points are one side of zero, but min is the other side of zero or closer to zero than to m_minPoint then set min to zero
				if (m_minPoint * m_maxPoint >= value_type_squared(0.0) && (min * m_maxPoint < value_type_squared(0.0) || m_minPoint / min > unitless_type(2.0)))
					min = value_type(0.0);
				return min;
			}
			value_type getLinearMax() const
			{
				if (!m_autoscale)
					return m_maxFixed;
				value_type max = m_maxPoint + m_autoscaleEndSpace * (m_maxPoint - m_minPoint);
				//as per getLinearMin
				if (m_minPoint * m_maxPoint >= value_type_squared(0.0) && (max * m_minPoint < value_type_squared(0.0) || m_maxPoint / max > unitless_type(2.0)))
					max = value_type(0.0);
				return max;
			}
			unitless_type getLogMin() const
			{
				if (!m_autoscale)
					return m_logMinFixed;
				return m_logMinPoint - m_autoscaleEndSpace * (m_logMaxPoint - m_logMinPoint);;
			}
			unitless_type getLogMax() const
			{
				if (!m_autoscale)
					return m_logMaxFixed;
				return m_logMaxPoint + m_autoscaleEndSpace * (m_logMaxPoint - m_logMinPoint);
			}
			bool isAutoscale() const
			{
				return m_autoscale;
			}
			bool isLog() const
			{
				return m_log;
			}
			template<class T>
			void expand(const T& data)
			{
				if (!isAutoscale())
					return;
				
				value_type min = std::numeric_limits<value_type>::quiet_NaN();
				value_type max = std::numeric_limits<value_type>::quiet_NaN();
				if (isLog())
				{
					for (auto& x : data)
					{
						if (x > 0 && x < std::numeric_limits<value_type>::infinity())
						{
							min = min < x ? min : x;
							max = max > x ? max : x;
						}
					}
				}
				else
				{
					for (auto& x : data)
					{
						if (std::isfinite(x))
						{
							min = min < x ? min : x;
							max = max > x ? max : x;
						}
					}
				}
				expand(min);
				expand(max);
			}
			void expand(value_type value)
			{
				if (!m_autoscale)
					return;
				if (!std::isfinite(value))
					return;
				if (m_minPoint != m_minPoint || value < m_minPoint)
				{
					if (!m_log || value > value_type(0.0))
						m_minPoint = value;
				}
				if (m_maxPoint != m_maxPoint || value > m_maxPoint)
					m_maxPoint = value;
			}
			void contract()
			{
				if (!m_autoscale)
					return;
				m_minPoint = std::numeric_limits<value_type>::quiet_NaN();
				m_maxPoint = std::numeric_limits<value_type>::quiet_NaN();
				m_logMinPoint = std::numeric_limits<value_type>::quiet_NaN();
				m_logMaxPoint = std::numeric_limits<value_type>::quiet_NaN();
			}
			void setAutoscale(double autoscaleEndSpace)
			{
				m_autoscale = true;
				m_autoscaleEndSpace = autoscaleEndSpace;
				contract();
			}
			void setFixedScale(value_type min, value_type max)
			{
				m_autoscale = false;
				m_minFixed = min;
				m_maxFixed = max;
				m_logMinFixed = sci::log10(min);
				m_logMaxFixed = sci::log10(max);
				m_minPoint = std::numeric_limits<double>::quiet_NaN();
				m_maxPoint = std::numeric_limits<double>::quiet_NaN();
				m_logMinPoint = std::numeric_limits<double>::quiet_NaN();
				m_logMaxPoint = std::numeric_limits<double>::quiet_NaN();
				m_autoscaleEndSpace = std::numeric_limits<double>::quiet_NaN();
			}
			void setLog()
			{
				m_log = true;
				if (m_autoscale)
					contract();
			}
			void setLinear()
			{
				m_log = false;
				if (m_autoscale)
					contract();
			}
			Direction getDirection() const
			{
				return m_direction;
			}

			//this function is provided to help inheriting classes with different output types
			//It puts value in ascending order, calculates the log equivalents and removes nans and infinities
			//from the linear values.
			//It then normalises the scales from 0-1 and puts the limits in the in/max variable
			//the logMin will be the smallest non -infinity value
			template<class U>
			static void setupInterpolatingScale(const std::span<const value_type> values, sci::GridData<unitless_type, 1>& normalisedValues, sci::GridData<unitless_type, 1>& normalisedLogValues, sci::GridData<U, 1>& outputs, value_type& linearMin, value_type& linearMax, unitless_type& logMin, unitless_type& logMax)
			{
				assert(values.size() > 1);
				assert(values.size() == outputs.size());
				//sci::assertThrow(value.size() > 1 && value.size() == output.size(), sci::err(sci::SERR_PLOT, colourscaleErrorCode, "sci::plot::Scale::setupInterpolatingScale called with invalid sizes for the values or colours array."));
				//check values are ascending or descending, and get the max/min at the same time
				bool ascending = true;
				bool descending = true;
				linearMax = -std::numeric_limits<value_type>::infinity();
				linearMin = std::numeric_limits<value_type>::infinity();
				value_type linearMinGtZero = std::numeric_limits<value_type>::infinity();

				if (values[0] > linearMax && values[0] != std::numeric_limits<value_type>::infinity())
					linearMax = values[0];
				if (values[0] < linearMin && values[0] != -std::numeric_limits<value_type>::infinity())
					linearMin = values[0];
				if (values[0] < linearMinGtZero && values[0] > value_type(0))
					linearMinGtZero = values[0];

				for (size_t i = 1; i < values.size(); ++i)
				{
					ascending &= values[i] >= values[i - 1];
					descending &= values[i] <= values[i - 1];
					if (values[i] > linearMax && values[i] != std::numeric_limits<value_type>::infinity())
						linearMax = values[i];
					if (values[i] < linearMin && values[i] != -std::numeric_limits<value_type>::infinity())
						linearMin = values[i];
					if (values[i] < linearMinGtZero && values[i] > value_type(0))
						linearMinGtZero = values[i];
				}
				logMin = sci::log10(linearMinGtZero);
				logMax = sci::log10(linearMax);
				bool monotonic = ascending || descending;
				assert(monotonic);


				//sci::assertThrow(monotonic, sci::err(sci::SERR_PLOT, colourscaleErrorCode, "sci::plot::ColourScale::setup called with values which are neither monotonically ascending nor monotonically descending."));

				//assign the normalised data fitlering nans and infinities from the linear data
				//we accept that there could be -infinities in the log data
				normalisedValues.resize(values.size());
				normalisedLogValues.resize(values.size());
				outputs.resize(values.size()); //this should already be the case, but make sure until we decide on an effective error reporting mechanism
				size_t insertPos = 0;
				for (size_t i = 0; i < values.size(); ++i)
				{
					if (normalisedValues[i] == normalisedValues[i] &&
						normalisedValues[i] != std::numeric_limits<unitless_type>::infinity() &&
						normalisedValues[i] != -std::numeric_limits<unitless_type>::infinity())
					{
						normalisedValues[insertPos] = (values[i] - linearMin) / (linearMax - linearMin);
						normalisedLogValues[insertPos] = (sci::log10(values[i]) - logMin) / (logMax - logMin);
						outputs[insertPos] = outputs[i];
						++insertPos;
					}
				}
				normalisedValues.resize(insertPos);
				normalisedLogValues.resize(insertPos);
				outputs.resize(insertPos);

				assert(normalisedValues.size() > 0);

				//reverse if descending
				if (descending)
				{
					std::reverse(normalisedValues.begin(), normalisedValues.end());
					std::reverse(normalisedLogValues.begin(), normalisedLogValues.end());
					std::reverse(outputs.begin(), outputs.end());
				}
			}
		private:
			value_type m_minFixed;
			value_type m_maxFixed;
			unitless_type m_logMinFixed;
			unitless_type m_logMaxFixed;
			value_type m_minPoint;
			value_type m_maxPoint;
			unitless_type m_logMinPoint;
			unitless_type m_logMaxPoint;
			bool m_log;
			bool m_autoscale;
			unitless_type m_autoscaleEndSpace;
			Direction m_direction;
		};

		using rgbcolour = sci::graphics::RgbColour;
		using hlscolour = sci::graphics::HlsColour;
		using Length = sci::graphics::Length;
		using Point = sci::graphics::Point;
		using Renderer = sci::graphics::Renderer;
		using Distance = sci::graphics::Distance;
		using millimetre = sci::graphics::millimetre;
		using perMillimetre = sci::graphics::perMillimetre;
		using textPoint = sci::graphics::textPoint;
		using degree = sci::graphics::degree;
		using unitless = sci::graphics::unitless;
		using perInch = sci::graphics::perInch;

		template<class T>
		class ColourScale : public Scale<T>
		{
			friend class splot;
			friend class splot2d;
			friend class splotlegend;
		public:
			using value_type = typename Scale<T>::value_type;
			using unitless_type = typename Scale<T>::unitless_type;

			ColourScale()
				:sci::plot::Scale<T>(false, Scale<T>::Direction::none, unitless_type(0.0))
			{
				setupdefault();
			}

			//create a colourscale from rgb colours. Use the same number of values in the value and colour vectors to create a continuous colour scale or one more in the value vector to create a discrete colour scale
			ColourScale(std::span<const value_type> values, std::span<const rgbcolour> colours, bool logarithmic = false, bool autostretch = false, bool fillOffscaleBottom = false, bool fillOffscaleTop = false)
				:sci::plot::Scale<T>(logarithmic, Scale<T>::Direction::none, Scale<T>::unitless_type(0.0))//assume autoscaling to start, but change this later if needed
			{
				if (values.size() == colours.size())
				{
					//setup continuous colour scale
					m_discrete = false;
					setup(values, colours, autostretch, fillOffscaleBottom, fillOffscaleTop);
				}
				else
				{
					//setup discrete colour scale
					m_discrete = true;
					sci::GridData<value_type, 1> newValues(colours.size() * 2);
					std::vector<rgbcolour> newColours(colours.size() * 2);
					for (size_t i = 0; i < colours.size(); ++i)
					{
						newColours[i * 2] = colours[i];
						newColours[i * 2 + 1] = colours[i];
						newValues[i * 2] = values[i];
						newValues[i * 2 + 1] = values[i + 1];
					}
					setup(newValues, newColours, autostretch, fillOffscaleBottom, fillOffscaleTop);
				}
			}

			//create a colourscale from hls colours. Use the same number of values in the value and colour vectors to create a continuous colour scale or one more in the value vector to create a discrete colour scale
			ColourScale(std::span<const value_type> values, std::span<const hlscolour> colours, bool logarithmic = false, bool autostretch = false, bool fillOffscaleBottom = false, bool fillOffscaleTop = false)
				:sci::plot::Scale<T>(logarithmic, Scale<T>::Direction::none, unitless_type(0.0))
			{
				if (values.size() == colours.size())
				{
					//setup continuous colour scale
					m_discrete = false;
					setup(values, colours, autostretch, fillOffscaleBottom, fillOffscaleTop);
				}
				else
				{
					//setup discrete colour scale
					m_discrete = true;
					sci::GridData<value_type, 1> newValues(colours.size() * 2);
					std::vector<hlscolour> newColours(colours.size() * 2);
					for (size_t i = 0; i < colours.size(); ++i)
					{
						newColours[i * 2] = colours[i];
						newColours[i * 2 + 1] = colours[i];
						newValues[i * 2] = values[i];
						newValues[i * 2 + 1] = values[i + 1];
					}
					setup(newValues, newColours, autostretch, fillOffscaleBottom, fillOffscaleTop);
				}
			}

			//note that valuePrelogged is only utilised if this is a log scale
			rgbcolour getRgbLinear(value_type value) const
			{
				if (m_hls)
					return getHlsLinear(value).convertToRgb();

				double r, g, b, a;
				interpolateLinear(value, r, g, b, a);
				return rgbcolour(r, g, b, a);
			}

			//note that valuePrelogged is only utilised if this is a log scale
			hlscolour getHlsLinear(value_type value) const
			{
				if (!m_hls)
					return getRgbLinear(value).convertToHls();

				double h, l, s, a;
				interpolateLinear(value, h, l, s, a);
				return hlscolour(degree(h), l, s, a);
			}

			//note that valuePrelogged is only utilised if this is a log scale
			rgbcolour getRgbLog(value_type value, bool valuePrelogged) const
			{
				if (m_hls)
					return getHlsLog(value, valuePrelogged).convertToRgb();

				double r, g, b, a;
				interpolateLog(value, r, g, b, a, valuePrelogged);
				return rgbcolour(r, g, b, a);
			}

			//note that valuePrelogged is only utilised if this is a log scale
			hlscolour getHlsLog(value_type value, bool valuePreLogged) const
			{
				if (!m_hls)
					return getRgbLog(value, valuePreLogged).convertToHls();

				double h, l, s, a;
				interpolateLog(value, h, l, s, a, valuePreLogged);
				return hlscolour(degree(h), l, s, a);
			}

			rgbcolour getRgbOffscaleBottom() const
			{
				if (!m_hls)
					return rgbcolour(m_colour1s[0], m_colour2s[0], m_colour3s[0], m_alphas[0]);
				else
					return hlscolour(degree(m_colour1s[0]), m_colour2s[0], m_colour3s[0], m_alphas[0]).convertToRgb();
			}

			rgbcolour getRgbOffscaleTop() const
			{
				if (!m_hls)
					return rgbcolour(m_colour1s.back(), m_colour2s.back(), m_colour3s.back(), m_alphas.back());
				else
					return hlscolour(degree(m_colour1s.back()), m_colour2s.back(), m_colour3s.back(), m_alphas.back()).convertToRgb();
			}

			hlscolour getHlsOffscaleBottom() const
			{
				if (!m_hls)
					return rgbcolour(m_colour1s[0], m_colour2s[0], m_colour3s[0], m_alphas[0]).convertToHls();
				else
					return hlscolour(degree(m_colour1s[0]), m_colour2s[0], m_colour3s[0], m_alphas[0]);
			}

			hlscolour getHlsOffscaleTop() const
			{
				if (!m_hls)
					return rgbcolour(m_colour1s.back(), m_colour2s.back(), m_colour3s.back(), m_alphas.back()).convertToHls();
				else
					return hlscolour(degree(m_colour1s.back()), m_colour2s.back(), m_colour3s.back(), m_alphas.back());
			}

			virtual ~ColourScale()
			{};

			bool isDiscrete() const
			{
				return m_discrete;
			}

			std::vector<value_type> getLinearDiscreteValues() const
			{
				std::vector<value_type> result(m_normalisedValues.size() / 2 + 1);
				value_type min = Scale<T>::getLinearMin();
				value_type max = Scale<T>::getLinearMax();
				value_type range = max - min;
				for (size_t i = 0; i < result.size() - 1; ++i)
					result[i] = min + m_normalisedValues[i * 2] * range;
				result.back() = min + m_normalisedValues.back() * range;
			
				return result;
			}

			std::vector<unitless_type> getLogDiscreteValues() const
			{
				std::vector<unitless_type> result(m_logValues.size() / 2 + 1);
				unitless_type min = Scale<T>::getLogMin();
				unitless_type max = Scale<T>::getLogMax();
				unitless_type range = max - min;
				for (size_t i = 0; i < result.size() - 1; ++i)
					result[i] = min + m_logValues[i * 2] * range;
				result.back() = min + m_logValues.back() * range;

				return result;
			}
			bool fillOffscaleBottom() const
			{
				return m_fillOffscaleBottom;
			}

			bool fillOffscaleTop() const
			{
				return m_fillOffscaleTop;
			}

			bool setFilOffscaleBottom(bool fill)
			{
				m_fillOffscaleBottom = fill;
			}
			bool setFilOffscaleTop(bool fill)
			{
				m_fillOffscaleTop = fill;
			}
		private:
			void setupdefault()
			{
				//assign values as are
				m_colour1s.resize(0);
				m_colour2s.resize(0);
				m_colour3s.resize(0);
				m_alphas.resize(0);
				m_colour1s.push_back(240.0);
				m_colour1s.push_back(0.0);
				m_colour2s.push_back(0.5);
				m_colour2s.push_back(0.5);
				m_colour3s.push_back(1.0);
				m_colour3s.push_back(1.0);
				m_alphas.push_back(1.0);
				m_alphas.push_back(1.0);
				m_normalisedValues.resize(0);
				m_normalisedValues.push_back(unitless_type(0.0));
				m_normalisedValues.push_back(unitless_type(1.0));
				m_hls = true;
				m_fillOffscaleBottom = false;
				m_fillOffscaleTop = false;
			}
			void setup(std::span<const value_type> values, std::span<const rgbcolour> colours, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop)
			{
				m_normalisedValues = sci::GridData<double, 1>(values.begin(), values.end());
				sci::GridData<rgbcolour, 1> colourCopy(colours.begin(), colours.end());
				value_type linearMin;
				value_type linearMax;
				unitless_type logMin;
				unitless_type logMax;
				Scale<T>::setupInterpolatingScale(values, m_normalisedValues, m_logValues, colourCopy, linearMin, linearMax, logMin, logMax);

				//if this is a fixed scale set the limits
				if (!autostretch)
				{
					if (Scale<T>::isLog())
						Scale<T>::setFixedScale(sci::pow(10, logMin), sci::pow(10, logMax)); //note the log limits might not be the logs of the linear limits due to -ve numbers
					else
						Scale<T>::setFixedScale(linearMin, linearMax);
				}

				//assign colours
				m_colour1s.resize(colours.size());
				m_colour2s.resize(colours.size());
				m_colour3s.resize(colours.size());
				m_alphas.resize(colours.size());
				for (size_t i = 0; i < colours.size(); ++i)
				{
					m_colour1s[i] = colours[i].r();
					m_colour2s[i] = colours[i].g();
					m_colour3s[i] = (double)colours[i].b();
					m_alphas[i] = (double)colours[i].a();
				}
				m_hls = false;
				m_fillOffscaleBottom = fillOffscaleBottom;
				m_fillOffscaleTop = fillOffscaleTop;
			}

			void setup(std::span<const value_type>values , std::span<const hlscolour> colours, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop)
			{
				sci::GridData<hlscolour, 1> colourCopy(colours.begin(), colours.end());
				value_type linearMin;
				value_type linearMax;
				unitless_type logMin;
				unitless_type logMax;
				Scale<T>::setupInterpolatingScale(values, m_normalisedValues, m_logValues, colourCopy, linearMin, linearMax, logMin, logMax);

				//if this is a fixed scale set the limits
				if (!autostretch)
				{
					if (Scale<T>::isLog())
						Scale<T>::setFixedScale(sci::pow(unitless_type(10), logMin), sci::pow(unitless_type(10), logMax)); //note the log limits might not be the logs of the linear limits due to -ve numbers
					else
						Scale<T>::setFixedScale(linearMin, linearMax);
				}

				m_colour1s.resize(colours.size());
				m_colour2s.resize(colours.size());
				m_colour3s.resize(colours.size());
				m_alphas.resize(colours.size());
				for (size_t i = 0; i < colours.size(); ++i)
				{
					m_colour1s[i] = colours[i].h().value<degree>();
					m_colour2s[i] = colours[i].l();
					m_colour3s[i] = colours[i].s();
					m_alphas[i] = colours[i].a();
				}
				m_hls = true;
				m_fillOffscaleBottom = fillOffscaleBottom;
				m_fillOffscaleTop = fillOffscaleTop;

				//deal with nans for hues: hue should be a nan for greys which will help with blending
				//if they are all nans then set them all to 0.0;
				bool allnans = true;
				bool anynans = false;
				for (size_t i = 0; i < m_colour1s.size(); ++i)
				{
					if (m_colour1s[i] == m_colour1s[i]) allnans = false;
					else anynans = true;
				}
				if (allnans)
				{
					m_colour1s = 0.0; //set every value to zero
				}
				else if (anynans && m_colour1s.size() == 2)
				{
					if (m_colour1s[0] != m_colour1s[0]) m_colour1s[0] = m_colour1s[1];
					else m_colour1s[1] = m_colour1s[0];
				}
				else if (anynans)
				{
					while (anynans)
					{
						anynans = false;
						//loop through interpolating any single nans
						for (size_t i = 1; i < m_colour1s.size() - 1; ++i)
						{
							if (m_colour1s[i] != m_colour1s[i])
							{
								if (m_colour1s[i - 1] == m_colour1s[i - 1] && m_colour1s[i + 1] == m_colour1s[i + 1])
								{
									m_colour1s[i] = m_colour1s[i - 1] + unitless((m_normalisedValues[i] - m_normalisedValues[i - 1]) / (m_normalisedValues[i + 1] - m_normalisedValues[i - 1])).value<unitless>() * (m_colour1s[i + 1] - m_colour1s[i - 1]);
								}
							}
						}
						//then set any nans preceded by a non-nan to that non nan
						for (size_t i = 1; i < m_colour1s.size(); ++i)
						{
							if (m_colour1s[i] != m_colour1s[i])
							{
								if (m_colour1s[i - 1] == m_colour1s[i - 1])
								{
									m_colour1s[i] = m_colour1s[i - 1];
									++i;
								}
							}
						}
						//then set any nans succeded by a non-nan to that non-nan
						for (size_t i = m_colour1s.size() - 2; i != 0; --i)//note this works due to unsigned size_t 
						{
							if (m_colour1s[i] != m_colour1s[i])
							{
								if (m_colour1s[i + 1] == m_colour1s[i + 1])
								{
									m_colour1s[i] = m_colour1s[i + 1];
									--i;
									if (i == 0) break;
								}
							}
						}
						//check index 0 separately to other suceeded values
						if (m_colour1s[0] != m_colour1s[0])
						{
							if (m_colour1s[1] == m_colour1s[1]) m_colour1s[0] = m_colour1s[1];
						}

						//check if any nans remain and loop again if so
						for (size_t i = 0; i < m_colour1s.size(); ++i)
						{
							if (m_colour1s[i] != m_colour1s[i]) anynans = true;
						}
					}
				}
			}

			sci::GridData<unitless_type, 1> m_normalisedValues;
			sci::GridData<unitless_type, 1> m_logValues;
			sci::GridData<double, 1> m_colour1s;
			sci::GridData<double, 1> m_colour2s;
			sci::GridData<double, 1> m_colour3s;
			sci::GridData<double, 1> m_alphas;
			bool m_hls; //true for hls, false for rgb colour representation
			bool m_discrete;
			bool m_fillOffscaleBottom;
			bool m_fillOffscaleTop;

			//note that valuePrelogged is only utilised if this is a log scale
			void interpolateLinear(value_type value, double& c1, double& c2, double& c3, double& a) const
			{
				bool offscaleBottom = value < Scale<T>::getLinearMin();
				bool offscaleTop = value > Scale<T>::getLinearMax();
				bool onMin = value == Scale<T>::getLinearMin();


				if (offscaleBottom)
				{
					if (m_fillOffscaleBottom)
					{
						c1 = m_colour1s[0];
						c2 = m_colour2s[0];
						c3 = m_colour3s[0];
						a = m_alphas[0];
					}
					else
					{
						c1 = std::numeric_limits<double>::quiet_NaN();
						c2 = std::numeric_limits<double>::quiet_NaN();
						c3 = std::numeric_limits<double>::quiet_NaN();
						a = std::numeric_limits<double>::quiet_NaN();
					}
					return;
				}

				if (offscaleTop)
				{
					if (m_fillOffscaleTop)
					{
						c1 = m_colour1s.back();
						c2 = m_colour2s.back();
						c3 = m_colour3s.back();
						a = m_alphas.back();
					}
					else
					{
						c1 = std::numeric_limits<double>::quiet_NaN();
						c2 = std::numeric_limits<double>::quiet_NaN();
						c3 = std::numeric_limits<double>::quiet_NaN();
						a = std::numeric_limits<double>::quiet_NaN();
					}
					return;
				}

				if (onMin)
				{
					c1 = m_colour1s[0];
					c2 = m_colour2s[0];
					c3 = m_colour3s[0];
					a = m_alphas[0];
					return;
				}


				size_t maxIndex = 1;
				double highWeight;
				value = (value - Scale<T>::getLinearMin()) / (Scale<T>::getLinearMax() - Scale<T>::getLinearMin());

				while (value > m_normalisedValues[maxIndex])
					maxIndex++;

				highWeight = unitless((value - m_normalisedValues[maxIndex - 1]) / (m_normalisedValues[maxIndex] - m_normalisedValues[maxIndex - 1])).value<unitless>();

				c1 = m_colour1s[maxIndex] * highWeight + m_colour1s[maxIndex - 1] * (1.0 - highWeight);
				c2 = m_colour2s[maxIndex] * highWeight + m_colour2s[maxIndex - 1] * (1.0 - highWeight);
				c3 = m_colour3s[maxIndex] * highWeight + m_colour3s[maxIndex - 1] * (1.0 - highWeight);
				a = m_alphas[maxIndex] * highWeight + m_alphas[maxIndex - 1] * (1.0 - highWeight);
			}

			void interpolateLog(unitless_type value, double& c1, double& c2, double& c3, double& a, bool valuePreLogged) const
			{
				if (!valuePreLogged)
					value = sci::log10(value);

				bool offscaleBottom = value < Scale<T>::getLogMin();
				bool offscaleTop = value > Scale<T>::getLogMax();
				bool onMin = value == Scale<T>::getLogMin();


				if (offscaleBottom)
				{
					if (m_fillOffscaleBottom)
					{
						c1 = m_colour1s[0];
						c2 = m_colour2s[0];
						c3 = m_colour3s[0];
						a = m_alphas[0];
					}
					else
					{
						c1 = std::numeric_limits<double>::quiet_NaN();
						c2 = std::numeric_limits<double>::quiet_NaN();
						c3 = std::numeric_limits<double>::quiet_NaN();
						a = std::numeric_limits<double>::quiet_NaN();
					}
					return;
				}

				if (offscaleTop)
				{
					if (m_fillOffscaleTop)
					{
						c1 = m_colour1s.back();
						c2 = m_colour2s.back();
						c3 = m_colour3s.back();
						a = m_alphas.back();
					}
					else
					{
						c1 = std::numeric_limits<double>::quiet_NaN();
						c2 = std::numeric_limits<double>::quiet_NaN();
						c3 = std::numeric_limits<double>::quiet_NaN();
						a = std::numeric_limits<double>::quiet_NaN();
					}
					return;
				}

				if (onMin)
				{
					c1 = m_colour1s[0];
					c2 = m_colour2s[0];
					c3 = m_colour3s[0];
					a = m_alphas[0];
					return;
				}


				size_t maxIndex = 1;
				double highWeight;
				value = (value - Scale<T>::getLogMin()) / (Scale<T>::getLogMax() - Scale<T>::getLogMin());

				while (value > m_logValues[maxIndex])
					maxIndex++;

				highWeight = unitless((value - m_logValues[maxIndex - 1]) / (m_logValues[maxIndex] - m_logValues[maxIndex - 1])).value<unitless>();
				

				c1 = m_colour1s[maxIndex] * highWeight + m_colour1s[maxIndex - 1] * (1.0 - highWeight);
				c2 = m_colour2s[maxIndex] * highWeight + m_colour2s[maxIndex - 1] * (1.0 - highWeight);
				c3 = m_colour3s[maxIndex] * highWeight + m_colour3s[maxIndex - 1] * (1.0 - highWeight);
				a = m_alphas[maxIndex] * highWeight + m_alphas[maxIndex - 1] * (1.0 - highWeight);
			}
		};



		class SizeScale : public Scale<double>
		{
			friend class splot;
			friend class splot2d;
			friend class splotlegend;
		public:
			SizeScale(std::span<const double> value = sci::GridData<double, 1>(0), std::span<const double> size = sci::GridData<double, 1>(0), bool logarithmic = false, bool autostretch = false, bool fillOffscaleBottom = false, bool fillOffscaleTop = false)
				:sci::plot::Scale<double>(logarithmic, Direction::none, 0.0)
			{
				m_value = sci::GridData<double, 1>(value.begin(), value.end());
				m_size = sci::GridData<double, 1>(size.begin(), size.end());
				m_fillOffscaleBottom = fillOffscaleBottom;
				m_fillOffscaleTop = fillOffscaleTop;
				double linearMin;
				double linearMax;
				double logMin;
				double logMax;
				Scale<double>::setupInterpolatingScale(value, m_value, m_logValue, m_size, linearMin, linearMax, logMin, logMax);

				//if this is a fixed scale set the limits
				if (!autostretch)
				{
					if (isLog())
						setFixedScale(std::pow(10, logMin), std::pow(10, logMax)); //note the log limits might not be the logs of the linear limits due to -ve numbers
					else
						setFixedScale(linearMin, linearMax);
				}
			}

			~SizeScale()
			{};

			//note that valuePrelogged is only utilised if this is a log scale
			double getSizeLinear(double value) const
			{

				bool offscaleBottom = value < getLinearMin();
				bool offscaleTop = value > getLinearMax();
				bool onMin = value == getLinearMin();

				if (offscaleBottom)
				{
					if (m_fillOffscaleBottom)
						return m_size[0];
					else
						return std::numeric_limits<double>::quiet_NaN();
				}

				if (offscaleTop)
				{
					if (m_fillOffscaleTop)
						return m_size.back();
					else
						return std::numeric_limits<double>::quiet_NaN();
				}

				if (onMin)
					return m_size[0];

				size_t maxIndex = 1;
				double highWeight;
				value = (value - getLinearMin()) / (getLinearMax() - getLinearMin());

				while (value > m_value[maxIndex])
					maxIndex++;

				highWeight = (value - m_value[maxIndex - 1]) / (m_value[maxIndex] - m_value[maxIndex - 1]);

				return m_size[maxIndex] * highWeight + m_size[maxIndex - 1] * (1.0 - highWeight);
			}

			//note that valuePrelogged is only utilised if this is a log scale
			double getSizeLog(double value, bool valuePreLogged) const
			{
				if (!valuePreLogged)
					value = std::log10(value);

				bool offscaleBottom = value < getLogMin();
				bool offscaleTop = value > getLogMax();
				bool onMin = value == getLogMin();

				if (offscaleBottom)
				{
					if (m_fillOffscaleBottom)
						return m_size[0];
					else
						return std::numeric_limits<double>::quiet_NaN();
				}

				if (offscaleTop)
				{
					if (m_fillOffscaleTop)
						return m_size.back();
					else
						return std::numeric_limits<double>::quiet_NaN();
				}

				if (onMin)
					return m_size[0];

				size_t maxIndex = 1;
				double highWeight;
				
				value = (value - getLogMin()) / (getLogMax() - getLogMin());

				while (value > m_logValue[maxIndex])
					maxIndex++;

				highWeight = (value - m_logValue[maxIndex - 1]) / (m_logValue[maxIndex] - m_logValue[maxIndex - 1]);
				

				return m_size[maxIndex] * highWeight + m_size[maxIndex - 1] * (1.0 - highWeight);
			}
			bool fillOffscaleBottom() const
			{
				return m_fillOffscaleBottom;
			}
			bool fillOffscaleTop() const
			{
				return m_fillOffscaleTop;
			}
			bool setFilOffscaleBottom(bool fill)
			{
				m_fillOffscaleBottom = fill;
			}
			bool setFilOffscaleTop(bool fill)
			{
				m_fillOffscaleTop = fill;
			}
		private:
			::sci::GridData<double, 1> m_value;
			::sci::GridData<double, 1> m_logValue;
			::sci::GridData<double, 1> m_size;
			bool m_fillOffscaleBottom;
			bool m_fillOffscaleTop;
		};


		class LevelScale : public Scale<double>
		{
			friend class splot;
			friend class splot2d;
			friend class splotlegend;
		public:
			LevelScale(std::span<const double> value = sci::GridData<double, 1>(0), bool logarithmic = false, bool autostretch = false)
				:sci::plot::Scale<double>(logarithmic, Direction::none, 0.0)
			{
				m_value = sci::GridData<double, 1>(value.begin(), value.end());
				sci::GridData<double, 1> dummy = m_value;
				double linearMin;
				double linearMax;
				double logMin;
				double logMax;
				setupInterpolatingScale(value, m_value, m_logValue, dummy, linearMin, linearMax, logMin, logMax);

				//if this is a fixed scale set the limits
				if (!autostretch)
				{
					if (isLog())
						setFixedScale(std::pow(10, logMin), std::pow(10, logMax)); //note the log limits might not be the logs of the linear limits due to -ve numbers
					else
						setFixedScale(linearMin, linearMax);
				}
			}
			~LevelScale() {};
			sci::GridData<double, 1> getLevelsLinear() const
			{
				sci::GridData<double, 1> result;
				if (isLog())
				{
					//result = sci::pow(10.0, getLogMin() + m_logValue * (getLogMax() - getLogMin())); //this should work but doesn't so do it manually
					result.reshape(m_logValue.shape());
					auto logValueIter = m_logValue.begin();
					for (auto resultIter = result.begin(); resultIter != result.end(); ++resultIter, ++logValueIter)
						*resultIter = sci::pow(10.0, getLogMin() + *logValueIter * (getLogMax() - getLogMin()));
				}
				else
					result = getLinearMin() + m_value * (getLinearMax() - getLinearMin());
				return result;
			}
			sci::GridData<double, 1> getLevelsLoggedIfNeeded() const
			{
				sci::GridData<double, 1> result;
				if (isLog())
				{
					result = getLogMin() + m_logValue * (getLogMax() - getLogMin());
				}
				else
					result = getLinearMin() + m_value * (getLinearMax() - getLinearMin());
				return result;
			}
		private:
			sci::GridData<double, 1> m_value;
			sci::GridData<double, 1> m_logValue;
		};

		class LineStyle
		{
		public:
			LineStyle(Length width = millimetre(0.5), const rgbcolour& colour = rgbcolour(0.0, 0.0, 0.0, 1.0), const std::vector<Length>& dashes = std::vector<Length>(0))
				: m_width(width), m_colour(colour), m_dashes(dashes)
			{
			}

			LineStyle(Length width, const rgbcolour& colour, sci::string pattern)
				: m_width(width), m_colour(colour)
			{
				parseLineStyle(pattern, width, m_dashes);
			}

			Length getWidth() const
			{
				return m_width;
			}

			std::vector<Length> getPattern() const
			{
				return m_dashes;
			}

			rgbcolour getColour() const
			{
				return m_colour;
			}

			//Converts a series of characters into dots/dashes and spaces.
			//Here a space represents a gap of 0.5m, a tab 1.5 mm and for dashes a . is 0.5 mm, a - or a _ are 1.5 mm.
			//Adjacent space or dash characters are summed, so a 2 mm gap followed by a 2 mm dash could be "    ...."
			//or " \t._"
			static void parseLineStyle(const sci::string& pattern, Length lineWidth, std::vector<Length>& dashes)
			{
				//set outputs to zero size
				dashes.resize(0);

				//return empty vectors if style is empty
				if (pattern.length() == 0)
					return;


				//set up to start on a mark
				bool onmark = true;
				if (pattern[0] == ' ' || pattern[0] == '\t')
					dashes.push_back(millimetre(0.0));

				//initialise our current lengths to zero
				Length marklength = millimetre(0.0);
				Length spacelength = millimetre(0.0);

				//work through each character of style
				for (size_t i = 0; i < pattern.length(); ++i)
				{
					//if we have changed between a space and mark record the length
					if (onmark == true && (pattern[i] == ' ' || pattern[i] == '\t'))
					{
						dashes.push_back(marklength);
						marklength = millimetre(0.0);
						onmark = false;
					}
					else if (onmark == false && (pattern[i] == '_' || pattern[i] == '.' || pattern[i] == '-'))
					{
						dashes.push_back(spacelength);
						spacelength = millimetre(0.0);
						onmark = true;
					}
					//add the current character to the current length
					if (pattern[i] == ' ')
						spacelength += lineWidth * unitless(0.5);
					else if (pattern[i] == '\t')
						spacelength += lineWidth * unitless(2.0);
					else if (pattern[i] == '.')
						marklength += lineWidth * unitless(0.5);
					else if (pattern[i] == '_')
						marklength += lineWidth * unitless(2.0);
					else if (pattern[i] == '-')
						marklength += lineWidth * unitless(2.0);
				}
				//add the last mark or space
				if (onmark == true)
					dashes.push_back(marklength);
				else
					dashes.push_back(spacelength);

				if (dashes.size() == 0)
					return;

				//if we ended on a mark merge it with the first mark and remove it
				if (onmark)
				{
					dashes[0] += dashes.back();
					dashes.pop_back();
				}
			}
			void setPen(Renderer& renderer) const
			{
				renderer.setPen(m_colour, m_width, m_dashes);
			}
		private:
			Length m_width;
			rgbcolour m_colour;
			std::vector<Length> m_dashes;
		};

		const LineStyle noLine(millimetre(0.0));

		class DrawableItem
		{
		public:
			virtual ~DrawableItem() {}
			virtual void preDraw() = 0;
			virtual void draw(Renderer& renderer, perMillimetre scale)
			{
			}
			virtual bool readyToDraw() const = 0;
		};

		class Axis : public Scale<double>, public DrawableItem
		{
			friend class splot;
			friend class splot2d;
		public:
			class Options
			{
			public:
				Options(sci::string title = sU(""))
					:m_title(title), m_titleDistance(textPoint(42)), m_majorTickLength(millimetre(4.0)), m_minorTickLength(millimetre(2.0)),
					m_ticksLeftOrDown(true), m_ticksRightOrUp(false), m_labelsLeftOrDown(true), m_labelDirection(millimetre(1.0), millimetre(0.0)), m_majorInterval(0.0),
					m_nSubticks(0), m_autoMajorInterval(true), m_autoNSubticks(true), m_customlabelcreator(nullptr), m_maxDigits(4)
				{
				}
				static Options getDefaultAxis()
				{
					return Options();
				}
				static Options getBlankAxis()
				{
					Options result;
					result.m_lineStyle = noLine;
					result.m_labelFont.m_size = millimetre(0.0);
					return result;
				}
				Options& setTitle(const sci::string& title)
				{
					m_title = title;
					return *this;
				}
				//to do create functions like above for all properties
				//so we can chain setting parameters
				sci::string m_title;
				Renderer::Font m_titleFont;
				Length m_titleDistance;
				LineStyle m_lineStyle;
				Length m_majorTickLength;
				Length m_minorTickLength;
				bool m_ticksLeftOrDown;
				bool m_ticksRightOrUp;
				Renderer::Font m_labelFont;
				bool m_labelsLeftOrDown;
				Distance m_labelDirection;
				double m_majorInterval;
				size_t m_nSubticks;
				bool m_autoMajorInterval;
				bool m_autoNSubticks;
				sci::string m_timeformat;
				sci::string(*m_customlabelcreator)(double axisvalue);
				size_t m_maxDigits;
			};
			Axis(double min, double max, bool log, Point start, Point end, Options options = Options(), Direction direction = Direction::none)
				:Scale(min, max, log, direction), m_options(options), m_start(start), m_end(end), m_hasChanged(true)
			{
			}
			Axis(bool log, Point start, Point end, Options options = Options(), Direction direction = Direction::none)
				:Scale(log, direction, 0.05), m_options(options), m_start(start), m_end(end), m_hasChanged(true)
			{

			}
			Axis(Point start, Point end, Options options = Options(), Direction direction = Direction::none)
				:Scale(direction, 0.05), m_options(options), m_start(start), m_end(end), m_hasChanged(true)
			{

			}

			~Axis() {};
			const Options& getOptions() const
			{
				return m_options;
			}

			void setOptions(const Options& options)
			{
				m_hasChanged = true;
				m_options = options;
			}

			Direction getBestDirection(Renderer& renderer) const
			{
				Direction direction = Scale::getDirection();
				if (Direction::none == direction)
				{
					degree angle = renderer.getAngle(m_end - m_start);
					if (angle < degree(0.0))
						angle += degree(360.0);
					if (angle < degree(45.0))
						direction = Direction::horizontal;
					else if (angle < degree(135.0))
						direction = Direction::vertical;
					else if (angle < degree(225.0))
						direction = Direction::horizontal;
					else if (angle < degree(315.0))
						direction = Direction::vertical;
					else
						direction = Direction::horizontal;
				}
				return direction;
			}

			void setPosition(Point start, Point end)
			{
				m_hasChanged = true;
				m_start = start;
				m_end = end;
			}

			void getPosition(Point& start, Point& end) const
			{
				start = m_start;
				end = m_end;
			}

			inline Point getStart() const
			{
				return m_start;
			}

			inline Point getEnd() const
			{
				return m_end;
			}

			void preDraw() override
			{
			}

			void draw(Renderer& renderer, perMillimetre scale) override
			{
				sci::graphics::StatePusher statePusher(&renderer);
				m_options.m_lineStyle.setPen(renderer);
				if (isLog())
					drawLog(renderer, scale);
				else
					drawLinear(renderer, scale);
			}

			bool readyToDraw() const override
			{
				return true;
			}

			template<class T>
			Distance alongAxisDistanceFromLinearData(T value) const
			{
				double fraction;
				if (isLog())
					fraction = (std::log10(value) - getLogMin()) / (getLogMax() - getLogMin());
				else
					fraction = (value - getLinearMin()) / (getLinearMax() - getLinearMin());
				return (m_end - m_start) * unitless(fraction);
			}

			template<class T>
			Distance alongAxisDistanceFromLoggedIfNeededData(T value) const
			{
				double fraction;
				if (isLog())
					fraction = (value - getLogMin()) / (getLogMax() - getLogMin());
				else
					fraction = (value - getLinearMin()) / (getLinearMax() - getLinearMin());
				return (m_end - m_start) * unitless(fraction);
			}

		private:
			void drawLinear(Renderer& renderer, perMillimetre scale)
			{
				//calculate the min, max and span
				double min = getLinearMin();
				double max = getLinearMax();
				if (min > max)
					std::swap(min, max);
				double span = max - min;

				//calculate major interval if needed
				double majorInterval;
				size_t nAutoSubticks = 0;
				if (m_options.m_autoMajorInterval)
				{
					double spanMagnitude = std::floor(std::log10(span));
					double spanNormalised = span / std::pow(10.0, spanMagnitude); //this will be in the range 1-9.99999;
					if (spanNormalised <= 2.0)
					{
						majorInterval = 0.2 * std::pow(10.0, spanMagnitude);
						nAutoSubticks = 3;
					}
					else if (spanNormalised <= 5.0)
					{
						majorInterval = 0.5 * std::pow(10.0, spanMagnitude);
						nAutoSubticks = 4;
					}
					else
					{
						majorInterval = 1.0 * std::pow(10.0, spanMagnitude);
						nAutoSubticks = 4;
					}
				}
				else
					majorInterval = m_options.m_majorInterval;

				//calculate the number of subticks
				size_t nSubticks;
				if (m_options.m_autoNSubticks)
				{
					if (m_options.m_autoMajorInterval)
						nSubticks = nAutoSubticks;
					else
						nSubticks = 4;
				}
				else
					nSubticks = m_options.m_nSubticks;

				//draw the ticks and labels
				renderer.setFont(m_options.m_labelFont);
				double currentMajorPosition = std::floor(min / majorInterval) * majorInterval; //start with the tick mark at or below the axis start

				double minorInterval = majorInterval / double(nSubticks + 1);
				millimetre maxLabelSize(0);
				while (currentMajorPosition <= max) //note this will be false if currentMajorPosition is a nan, so we don't need to worry about infite loops
				{
					if (currentMajorPosition >= min)
					{
						drawTick(renderer, scale, currentMajorPosition, false);
						millimetre labelSize = drawLabel(renderer, scale, currentMajorPosition, minorInterval);
						maxLabelSize = std::max(labelSize, maxLabelSize);
					}
					for (size_t i = 0; i < nSubticks; ++i)
					{
						double minorPosition = currentMajorPosition + double(i + 1) * minorInterval;
						if (minorPosition <= max && minorPosition >= min)
							drawTick(renderer, scale, minorPosition, true);
					}
					currentMajorPosition += majorInterval;
				}

				//draw the axis
				renderer.line(m_start, m_end);

				//draw the title
				renderer.setFont(m_options.m_titleFont);
				drawTitle(renderer, scale, m_options.m_majorTickLength * unitless(1.4) + maxLabelSize);
			}

			void drawLog(Renderer& renderer, perMillimetre scale)
			{
				//calculate the min, max and span
				double logMin = getLogMin();
				double logMax = getLogMax();
				if (logMin > logMax)
					std::swap(logMin, logMax);
				double logSpan = logMax - logMin;

				//calculate major interval if needed
				double majorLogInterval;
				if (m_options.m_autoMajorInterval)
					majorLogInterval = std::ceil(logSpan / 10.0);
				else
					majorLogInterval = m_options.m_majorInterval;

				//calculate the number of subticks and the interval
				size_t nSubticks;
				if (m_options.m_autoNSubticks)
					nSubticks = 8;
				else
					nSubticks = m_options.m_nSubticks;
				//If major interval divided by the sub intervals is an integer, then multiply subticks by this number
				double minorLogInterval = 0;;
				double testInterval = majorLogInterval / double(nSubticks + 1);
				if (nSubticks > 0 && testInterval == ceil(testInterval))
					minorLogInterval = testInterval;
				double minorInterval = (std::pow(10.0, majorLogInterval) - 1.0) / double(nSubticks + 1);

				//draw the ticks and labels
				renderer.setFont(m_options.m_labelFont);
				double currentMajorLogPosition = std::floor(logMin / majorLogInterval) * majorLogInterval;
				millimetre maxLabelSize(0);

				while (currentMajorLogPosition <= logMax) //note this will be false if currentMajorPosition is a nan, so we don't need to worry about infite loops
				{
					double currentMajorPosition = std::pow(10.0, currentMajorLogPosition);
					if (currentMajorLogPosition >= logMin)
					{
						drawTick(renderer, scale, currentMajorPosition, false);
						millimetre labelSize = drawLabel(renderer, scale, currentMajorPosition, 0.0);
						maxLabelSize = std::max(labelSize, maxLabelSize);
					}
					if (minorLogInterval != 0.0)
					{
						for (size_t i = 0; i < nSubticks; ++i)
						{
							double currentMinorPosition = std::pow(10.0, currentMajorLogPosition + double(i + 1) * minorLogInterval);
							if (currentMinorPosition <= getLinearMax() && currentMinorPosition >= getLinearMin())
								drawTick(renderer, scale, currentMinorPosition, true);
						}

					}
					else
					{
						for (size_t i = 0; i < nSubticks; ++i)
						{
							double currentMinorPosition = currentMajorPosition * double((i + 1) * minorInterval + 1.0);
							if (currentMinorPosition <= getLinearMax() && currentMinorPosition >= getLinearMin())
								drawTick(renderer, scale, currentMinorPosition, true);
						}
					}
					currentMajorLogPosition += majorLogInterval;
				}

				//draw the axis
				renderer.line(m_start, m_end);

				//draw the title
				renderer.setFont(m_options.m_titleFont);
				drawTitle(renderer, scale, m_options.m_majorTickLength * unitless(1.4) + maxLabelSize);
			}

			void drawTick(Renderer& renderer, perMillimetre scale, double plotPosition, bool minor)
			{
				Point pagePosition = m_start + alongAxisDistanceFromLinearData(plotPosition);
				Length length = minor ? m_options.m_minorTickLength : m_options.m_majorTickLength;
				Point p1 = Point(millimetre(0.0), millimetre(0.0));
				Point p2 = p1;
				Direction direction = getBestDirection(renderer);
				if (direction == Direction::horizontal)
				{
					//set the position of both ends of the tick on the axis initially
					p1 = pagePosition;
					p2 = pagePosition;
					//extend the tick out in either direction as needed
					if (m_options.m_ticksRightOrUp)
						p1 += Distance(millimetre(0.0), -length);
					if (m_options.m_ticksLeftOrDown)
						p2 += Distance(millimetre(0.0), length);
				}
				else if (direction == Direction::vertical)
				{
					//set the position of both ends of the tick on the axis initially
					p1 = pagePosition;
					p2 = pagePosition;
					//extend the tick out in either direction as needed
					if (m_options.m_ticksLeftOrDown)
						p1 += Distance(-length, millimetre(0.0));
					if (m_options.m_ticksRightOrUp)
						p2 += Distance(length, millimetre(0.0));
				}
				if (p1 != p2)
					renderer.line(p1, p2);
			}

			millimetre drawLabel(Renderer& renderer, perMillimetre scale, double plotPosition, double minorInterval) //returns the sise of the label perpedicular to the axis
			{
				sci::graphics::StatePusher statePusher(&renderer);
				renderer.setBrush(m_options.m_labelFont.m_colour);
				Point pagePosition = m_start + alongAxisDistanceFromLinearData(plotPosition);
				Length distanceFromAxis = m_options.m_majorTickLength * unitless(1.2);

				sci::string label;
				{
					//make a scope so we destroy the stream and don't access it later
					sci::stringstream strm;
					strm.precision(m_options.m_maxDigits);

					//check we are not so close to zero we should just be zero
					double temp = std::log10(std::abs(plotPosition) / minorInterval);
					if (!isLog() && std::log10(std::abs(plotPosition) / minorInterval) < -double(m_options.m_maxDigits))
						strm << 0;
					else
						strm << plotPosition;

					//make scientific notation nicer
					size_t ePosition = strm.str().find(sU("e"));
					if (ePosition != sci::string::npos && strm.str().length() > ePosition + 2)
					{
						label = strm.str().substr(0, ePosition);
						label = label + sU("\u00D710^{");
						size_t exponentPosition = ePosition + 1;
						if (strm.str()[exponentPosition] == sU('-'))
							label = label + sU('-');
						++exponentPosition; //skip the + or -
						while (strm.str()[exponentPosition] == sU('0') && exponentPosition < strm.str().length())
							++exponentPosition;
						label = label + strm.str().substr(exponentPosition) + sU("}");
					}
					else
						label = strm.str();
				}

				unitless alignment(0.5);
				millimetre labelSize(0);
				Direction direction = getBestDirection(renderer);
				if (direction == Direction::horizontal)
				{
					//set the horizontal position and the alignment
					if (!m_options.m_labelsLeftOrDown)
					{
						pagePosition += Distance(millimetre(0.0), -distanceFromAxis);
						alignment = unitless(1.0);
					}
					else
					{
						pagePosition += Distance(millimetre(0.0), distanceFromAxis);
						alignment = unitless(0.0);
					}

					labelSize = (renderer.formattedText(label, pagePosition, unitless(0.5), unitless(alignment))).ascent;
				}
				else if (direction == Direction::vertical)
				{
					//set the horizontal position and the alignment
					if (m_options.m_labelsLeftOrDown)
					{
						pagePosition += Distance(-distanceFromAxis, millimetre(0.0));
						alignment = unitless(1.0);
					}
					else
					{
						pagePosition += Distance(distanceFromAxis, millimetre(0.0));
						alignment = unitless(0.0);
					}

					labelSize = (renderer.formattedText(label, pagePosition, unitless(alignment), unitless(0.5))).width;
				}

				return labelSize;
			}

			void drawTitle(Renderer& renderer, perMillimetre scale, Length distanceFromAxis)
			{
				sci::graphics::StatePusher statePusher(&renderer);
				renderer.setBrush(m_options.m_titleFont.m_colour);
				Point pagePosition = m_start + (m_end - m_start) * unitless(0.5);

				unitless alignment(0.5);
				Direction direction = getBestDirection(renderer);
				if (direction == Direction::horizontal)
				{
					//set the horizontal position and the alignment
					if (!m_options.m_labelsLeftOrDown)
					{
						pagePosition += Distance(millimetre(0.0), -distanceFromAxis);
						alignment = unitless(1.0);
					}
					else
					{
						pagePosition += Distance(millimetre(0.0), distanceFromAxis);
						alignment = unitless(0.0);
					}

					renderer.formattedText(m_options.m_title, pagePosition, unitless(0.5), unitless(alignment));
				}
				else if (direction == Direction::vertical)
				{
					//set the horizontal position and the alignment
					if (m_options.m_labelsLeftOrDown)
					{
						pagePosition += Distance(-distanceFromAxis, millimetre(0.0));
						alignment = unitless(1.0);
					}
					else
					{
						pagePosition += Distance(distanceFromAxis, millimetre(0.0));
						alignment = unitless(0.0);
					}

					renderer.formattedText(m_options.m_title, pagePosition, unitless(0.5), unitless(alignment), degree(90));
				}
			}

			std::string createploptstring() const;

			bool m_hasChanged;
			Options m_options;
			Point m_start;
			Point m_end;
		};

		class CubehelixColourscale : public ColourScale<double>
		{
		public:
			CubehelixColourscale(std::span<const double> values, double startHue, double hueRotation, double startBrightness, double endBrightness, double saturation, double gamma, bool logarithmic, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop, bool discrete)
				: ColourScale(values, CubehelixColourscale::getCubehelixColours(values, logarithmic, startHue, hueRotation, startBrightness, endBrightness, saturation, gamma, discrete),
					logarithmic, autostretch, fillOffscaleBottom, fillOffscaleTop)
			{

			}
			CubehelixColourscale(double minValue, double maxValue, size_t nPoints, double startHue, double hueRotation, double startBrightness, double endBrightness, double saturation, double gamma, bool logarithmic, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop, bool discrete)
				: ColourScale(getEvenlyDistributedValues(minValue, maxValue, nPoints, logarithmic),
					getCubehelixColours(minValue, maxValue, nPoints, logarithmic, startHue, hueRotation, startBrightness, endBrightness, saturation, gamma, discrete),
					logarithmic, autostretch, fillOffscaleBottom, fillOffscaleTop)
			{

			}
			CubehelixColourscale(std::span<const double> values, bool logarithmic, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop, bool discrete)
				: ColourScale(values, CubehelixColourscale::getCubehelixColours(values, logarithmic, 180, 540, 0.8, 0.2, 1.0, 1.0, discrete),
					logarithmic, autostretch, fillOffscaleBottom, fillOffscaleTop)
			{

			}
			CubehelixColourscale(double minValue, double maxValue, size_t nPoints, bool logarithmic, bool autostretch, bool fillOffscaleBottom, bool fillOffscaleTop, bool discrete)
				: ColourScale(getEvenlyDistributedValues(minValue, maxValue, nPoints, logarithmic),
					getCubehelixColours(minValue, maxValue, nPoints, logarithmic, 180, 540, 0.8, 0.2, 1.0, 1.0, discrete),
					logarithmic, autostretch, fillOffscaleBottom, fillOffscaleTop)
			{

			}
			static std::vector<rgbcolour> getCubehelixColours(double minValue, double maxValue, size_t nPoints, bool logarithmic, double startHue, double hueRotation, double startBrightness, double endBrightness, double saturation, double gamma, bool discrete)
			{
				if (logarithmic)
				{
					minValue = std::log10(minValue);
					maxValue = std::log10(maxValue);
				}
				sci::GridData<double, 1> values(nPoints);
				for (size_t i = 0; i < nPoints; ++i)
				{
					double f = (double)i / double(nPoints - 1);
					values[i] = minValue * (1 - f) + maxValue * f;
				}
				//call the other get colours funtion with the derived values. Note we always set logaritmic to false in this call as we have already logged the values.
				return getCubehelixColours(values, false, startHue, hueRotation, startBrightness, endBrightness, saturation, gamma, discrete);
			}
			static std::vector<rgbcolour> getCubehelixColours(std::span<const double> values, bool logarithmic, double startHue, double hueRotation, double startBrightness, double endBrightness, double saturation, double gamma, bool discrete)
			{
				//if the scale is logarithmic, call this fuction again, but with logged values
				if (logarithmic)
				{
					std::vector<double> logValues(values.size());
					for (size_t i = 0; i < values.size(); ++i)
						logValues[i] = std::log10(values[i]);
					return getCubehelixColours(logValues, false, startHue, hueRotation, startBrightness, endBrightness, saturation, gamma, discrete);
				}


				if (discrete)
				{
					sci::GridData<double, 1> newValues(values.size() - 1);

					for (size_t i = 0; i < newValues.size(); ++i)
						newValues[i] = (values[i] + values[i + 1]) / 2.0;
					return getCubehelixColours(newValues, false, startHue, hueRotation, startBrightness, endBrightness, saturation, gamma, false);
				}
				//make the hue consistent with the start parameter used in D. A. Green 2011
				startHue -= std::floor(startHue / 360.0) * 360.0;
				startHue /= 120;
				startHue += 1;

				//create a vector to hold the colours
				std::vector<rgbcolour> colours(values.size());

				//calculate each colour
				double valuesRange = values.back() - values.front();
				double brightnessRange = endBrightness - startBrightness;
				for (size_t i = 0; i < colours.size(); ++i)
				{
					double brightness = startBrightness + (values[i] - values[0]) / valuesRange * brightnessRange;
					double angle = (2.0 * M_PI) * (startHue / 3.0 + 1.0 + hueRotation / 360.0 * brightness);
					double gammaBrightness = std::pow(brightness, gamma);
					double amplitude = saturation * gammaBrightness * (1 - gammaBrightness) / 2.0;
					double red = gammaBrightness + amplitude * (-0.14861 * std::cos(angle) + 1.78277 * std::sin(angle));
					double green = gammaBrightness + amplitude * (-0.29227 * std::cos(angle) - 0.90649 * std::sin(angle));
					double blue = gammaBrightness + amplitude * (1.97294 * std::cos(angle));
					colours[i] = rgbcolour(red, green, blue);
				}
				//std::fstream fout;
				//fout.open("colourscale", std::ios::out);
				//for (size_t i = 0; i < colours.size(); ++i)
				//	fout << values[i] << "," << colours[i].r() << "," << colours[i].g() << "," << colours[i].b() << "\n";
				//fout.close();
				return colours;
			}
		private:
			static sci::GridData<double, 1> getEvenlyDistributedValues(double minValue, double maxValue, size_t nPoints, bool logarithmic)
			{
				if (logarithmic)
				{
					minValue = std::log10(minValue);
					maxValue = std::log10(maxValue);
					sci::GridData<double, 1> values(nPoints);
					for (size_t i = 0; i < nPoints; ++i)
					{
						double f = (double)i / double(nPoints - 1);
						values[i] = std::pow(10, minValue * (1 - f) + maxValue * f);
					}
					return values;
				}
				sci::GridData<double, 1> values(nPoints);
				for (size_t i = 0; i < nPoints; ++i)
				{
					double f = (double)i / double(nPoints - 1);
					values[i] = minValue * (1 - f) + maxValue * f;
				}
				return values;
			}
		};
	}
}

		//have to #include this down here as classes in here need some of the declarations from
		//above, but they don't get included because of the #ifdef protection
#include"elements.h"
#include"gridContour.h"

namespace sci
{
	namespace plot
	{


		class PlotFrame : public DrawableItem
		{
		public:
			PlotFrame(const Point topLeft, const Point bottomRight, const FillStyle& fillStyle = FillStyle(), const LineStyle& lineStyle = noLine,
				::sci::string title = sU(""), Length titlesize = Length(textPoint(12.0)), Length titledistance = Length(textPoint(12.0)),
				::sci::string titlefont = sU(""), int32_t titlestyle = 0, rgbcolour titlecolour = rgbcolour(0, 0, 0))
			{
				m_topLeft = topLeft;
				m_bottomRight = bottomRight;
				m_fillStyle = fillStyle;
				m_lineStyle = lineStyle;
				m_title = title;
				m_titlesize = titlesize;
				m_titledistance = titledistance;
				m_titlefont = titlefont;
				m_titlestyle = titlestyle;
				m_titlecolour = titlecolour;
			}
			void preDraw() override
			{
			}
			void draw(Renderer& renderer, perMillimetre scale) override
			{
				sci::graphics::StatePusher state(&renderer);
				m_fillStyle.setBrush(renderer);
				m_lineStyle.setPen(renderer);
				renderer.rectangle(m_topLeft, m_bottomRight - m_topLeft);

				Point titlePosition(m_topLeft.getX() + (m_bottomRight.getX() - m_topLeft.getX()) / unitless(2), m_topLeft.getY() - m_titledistance);

				renderer.setFont(m_titlefont, m_titlesize, m_titlecolour);
				renderer.formattedText(m_title, titlePosition, unitless(0.5), unitless(0.0));
			}
			bool readyToDraw() const override
			{
				return true;
			}
		private:
			Point m_topLeft;
			Point m_bottomRight;
			FillStyle m_fillStyle;
			LineStyle m_lineStyle;
			::sci::string m_title;
			Length m_titlesize;
			Length m_titledistance;
			::sci::string m_titlefont;
			int32_t m_titlestyle;
			rgbcolour m_titlecolour;
		};



		class HorizontalColourBar : public DrawableItem
		{
		public:
			HorizontalColourBar(Point bottomLeft, Point topRight, std::shared_ptr<ColourScale<double>> colourscale, Axis::Options axisOptions = Axis::Options())
				:m_colourscale(colourscale),
				m_xAxis(new Axis(m_colourscale->getLinearMin(), m_colourscale->getLinearMax(), m_colourscale->isLog(), bottomLeft, Point(topRight.getX(), bottomLeft.getY()), axisOptions, sci::plot::Scale<double>::Direction::horizontal)),
				m_yAxis(new Axis(0.0, 1.0, false, bottomLeft, Point(bottomLeft.getX(), topRight.getY()), Axis::Options::getBlankAxis(), sci::plot::Scale<double>::Direction::horizontal))
			{
			}
			void preDraw() override
			{
			}
			void draw(Renderer& renderer, perMillimetre scale) override
			{
				if (m_colourscale->isDiscrete())
				{
					sci::GridData<double, 2> cb({ 2, 2 });
					cb[0][0] = m_colourscale->getLinearMin();
					cb[0][1] = cb[0][0];
					cb[1][0] = m_colourscale->getLinearMax();
					cb[1][1] = cb[1][0];
					std::vector<double> cbX{ cb[0][0], cb[1][0] };
					std::vector<double> cbY{ 0.0, 1.0 };

					Contours<1, 1> data(cbX, cbY, cb, m_xAxis, m_yAxis, m_colourscale, noLine);

					data.draw(renderer, scale);
					m_xAxis->draw(renderer, scale);
				}
				else
				{
					sci::GridData<double, 2> z({ 256, 1 });
					std::vector<double> cbX(z.shape()[0] + 1);


					double min = m_colourscale->isLog() ? m_colourscale->getLogMin() : m_colourscale->getLinearMin();
					double max = m_colourscale->isLog() ? m_colourscale->getLogMax() : m_colourscale->getLinearMax();

					if (m_colourscale->isLog())
					{
						double range = max - min;
						double step = range / (z.shape()[0]);

						for (size_t i = 0; i < cbX.size(); ++i)
							cbX[i] = std::pow(10, min + i * step);

						for (size_t i = 0; i < z.size(); ++i)
							z[i][0] = std::pow(10.0, (min + (i + 0.5) * step));
					}
					else
					{
						double range = max - min;
						double step = range / (z.size());

						for (size_t i = 0; i < cbX.size(); ++i)
							cbX[i] = min + i * step;

						for (size_t i = 0; i < z.size(); ++i)
							z[i][0] = (cbX[i] + cbX[i + 1]) / 2.0;
					}

					std::vector<double> cbY{ 0.0, 1.0 };

					Grid<1, 1> data(cbX, cbY, z, m_xAxis, m_yAxis, m_colourscale);

					data.draw(renderer, scale);
					m_xAxis->draw(renderer, scale);
				}
			}
			bool readyToDraw() const override
			{
				return true;
			}
		private:
			std::shared_ptr<ColourScale<double>> m_colourscale;
			std::shared_ptr<Axis> m_xAxis;
			std::shared_ptr<Axis> m_yAxis;

		};

		class Label : public DrawableItem
		{
		public:
			Label(sci::string text, Point position, unitless horizontalAlignment = unitless(0.0),
				unitless verticalAlignment = unitless(0.0), Renderer::Font font = Renderer::Font(),
				degree rotation = degree(0.0), Length minTextSize = textPoint(5.0))
				:m_text(text), m_position(position), m_horizontalAlignment(horizontalAlignment),
				m_verticalAlignment(verticalAlignment), m_font(font), m_rotation(rotation),
				m_minTextSize(minTextSize)
			{

			}

			virtual void preDraw() override
			{
			};

			void draw(Renderer& renderer, perMillimetre scale) override
			{
				sci::graphics::StatePusher statePusher(&renderer);
				renderer.setFont(m_font);
				renderer.formattedText(m_text, m_position, m_horizontalAlignment, m_verticalAlignment, m_rotation);
			}

			virtual bool readyToDraw() const override
			{
				return true;
			}

		private:
			sci::string m_text;
			Point m_position;
			Renderer::Font m_font;
			unitless m_horizontalAlignment;
			unitless m_verticalAlignment;
			degree m_rotation;
			Length m_minTextSize;
		};

		class PlotCanvas
		{
		public:
			void addItem(std::shared_ptr<DrawableItem> item)
			{
				m_drawableItems.push_back(item);
			}
			void addItem(std::shared_ptr<DrawableItem> item, size_t index)
			{
				if (m_drawableItems.size() > index)
					m_drawableItems.insert(m_drawableItems.begin() + index, item);
				m_drawableItems.push_back(item);
			}
			size_t getItemIndex(std::shared_ptr<DrawableItem> item)
			{
				for (auto i = m_drawableItems.begin(); i != m_drawableItems.end(); ++i)
				{
					if (*i == item)
					{
						return i - m_drawableItems.begin();
					}
				}
				return size_t(-1);
			}
			void removeItem(std::shared_ptr<DrawableItem> item)
			{
				for (auto i = m_drawableItems.begin(); i != m_drawableItems.end(); ++i)
				{
					if (*i == item)
					{
						m_drawableItems.erase(i);
						--i;
					}
				}
			}
			bool swapItem(std::shared_ptr<DrawableItem> oldItem, std::shared_ptr<DrawableItem> newItem)
			{
				for (auto i = m_drawableItems.begin(); i != m_drawableItems.end(); ++i)
				{
					if (*i == oldItem)
					{
						*i = newItem;
						return true;
					}
				}
				return false;
			}
			void render(Renderer& renderer, perMillimetre scale)
			{
				//remove any items we wish to skip
				std::vector<std::shared_ptr<DrawableItem>> itemsToRender;
				for (auto& d : m_drawableItems)
				{
					bool render = true;
					for (auto& p : m_itemsToSkipNextRendering)
						if (p == d)
							render = false;
					if (render)
						itemsToRender.push_back(d);
				}

				//cycle through all the Drawable Items ensuring they are ready to draw
				//note this may need calling multiple times as predraw on one item may
				//invalidate the predraw on another item
				bool ready = false;
				do
				{
					for (auto& d : itemsToRender)
					{
						if (!d->readyToDraw())
							d->preDraw();
					}
					ready = true;
					for (auto& d : itemsToRender)
					{
						ready = ready && d->readyToDraw();
					}
				} while (!ready);

				//draw the Drawable Items
				for (auto& d : itemsToRender)
				{
					d->draw(renderer, scale);
				}

				//clear the items to skip
				m_itemsToSkipNextRendering.resize(0);
			}
			bool writetofile(::sci::string filename, int width, int height, perMillimetre scale)
			{
				//get the extension
				wxFileName fullfile = wxString(sci::toNativeUnicode(filename));
				wxString extension = fullfile.GetExt().Lower();

				bool result = true;


				if (extension == "ps")
				{
					//here we redraw the plot like OnPaint but using a postscript DC.
					wxPrintData setupdata;
					setupdata.SetColour(true);
					setupdata.SetFilename(sci::toNativeUnicode(filename));
					//setupdata.SetPaperId(wxPAPER_A4);
					setupdata.SetPaperId(wxPAPER_NONE);
					//note we set the image size in mm, but ps uses pts(1/72 inch, ~0.35 mm) and uses integer coordinates. 
					//So setting the image size to the screen resolution gives us a ps resolution of about 3 times bettr 
					//than the screen. Here we've multiplied by 10 to get 30 times better.
					int sizemultiplier = 10;
					setupdata.SetPaperSize(wxSize(width * sizemultiplier, height * sizemultiplier));
					setupdata.SetPrintMode(wxPRINT_MODE_FILE);
					//setupdata.SetQuality(wxPRINT_QUALITY_HIGH); //doesn't seem to do anything
					wxPostScriptDC psdc(setupdata);
					result = psdc.StartDoc(sci::toNativeUnicode(sU("Writing ") + filename));
					if (result == false)
						return result;
					sci::graphics::wxDcRenderer renderer(&psdc, wxSize(width, height), scale);
					render(renderer, scale);
					psdc.EndDoc();
				}
#ifdef _WIN32
				else if (extension == "emf")
				{
					//here we redraw the plot like OnPaint but using a wxMetafile DC.
					wxMetafileDC metadc(sci::toNativeUnicode(filename), width, height);
					sci::graphics::wxDcRenderer renderer(&metadc, wxSize(width, height), scale);
					render(renderer, scale);;//0 gives vector output
					//close the file - note this gives us a copy of the file in memory which we must delete
					wxMetafile* metafile = metadc.Close();
					result = metafile != NULL;
					delete metafile;
					//we must call this function to make the file importable into other software
					int minx = metadc.MinX();
					int maxx = metadc.MaxX();
					int miny = metadc.MinY();
					int maxy = metadc.MaxY();
					//wxMakeMetaFilePlaceable(minx,miny,maxx,maxy);
				}
#endif
				else if (extension == "svg")
				{
					wxSVGFileDC dc(sci::toNativeUnicode(filename), width, height, scale.value<perInch>());
					sci::graphics::wxDcRenderer renderer(&dc, wxSize(width, height), scale);
					render(renderer, scale);
					result = true;
				}
				else
				{
					//load the image handlers
					wxInitAllImageHandlers();
					//create a wxBitmapType to define the type saved as
					//use PNG as default
					wxBitmapType type = wxBITMAP_TYPE_PNG;
					if (extension == wxT("jpg")) type = wxBITMAP_TYPE_JPEG;
					else if (extension == wxT("bmp")) type = wxBITMAP_TYPE_BMP;
					else if (extension == wxT("tif")) type = wxBITMAP_TYPE_TIF;
					else if (extension == wxT("pcx")) type = wxBITMAP_TYPE_PCX;
					else if (extension == wxT("xpm")) type = wxBITMAP_TYPE_XPM;
					else if (extension == wxT("xbm")) type = wxBITMAP_TYPE_XBM;
					else if (extension == wxT("pnm")) type = wxBITMAP_TYPE_PNM;


					//create a wxMemoryDC which will be linked to the bitmap. We'll use this to draw to the bitmap
					//except if using AGG then we create an image instead which we'll need to copy to the bitmap
					wxMemoryDC memdc;
					wxBitmap bitmap(width, height, 32);
					if (!bitmap.IsOk())
						return false;
					memdc.SelectObject(bitmap);
					//fill the bitmap with white giving a white background for plplot
					//or to show blank if there are no plots
					memdc.FloodFill(0, 0, *wxWHITE, wxFLOOD_BORDER);

					sci::graphics::wxDcRenderer renderer(&memdc, wxSize(width, height), scale);
					render(renderer, scale);


					//reselect null bitmap for the memdc
					memdc.SelectObject(wxNullBitmap);

					//write the bitmap to file
					result = bitmap.SaveFile(sci::toNativeUnicode(filename), type);

				}

				return result;
			}
			void skipNextRenderingOf(std::shared_ptr<DrawableItem> item)
			{
				m_itemsToSkipNextRendering.push_back(item);
			}
		private:
			std::vector<std::shared_ptr<DrawableItem>> m_drawableItems;
			std::vector<std::shared_ptr<DrawableItem>> m_itemsToSkipNextRendering;
			wxSize m_previousSize;

		};
	}
}

#endif