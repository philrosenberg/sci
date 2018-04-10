#include<svector/NumberFromTextValidator.h>
template<>
double NumberFromTextValidator<double>::textToNumber(const wxString &text) const
{
	double result;
	if (!text.ToDouble(&result))
		throw(std::string("Error: Invalid double precision floating point value."));
	if (m_extraRangeChecks && (result < m_minVal || result > m_maxVal))
		throw(std::string("Error: out of range."));
	return result;
}

template<>
float NumberFromTextValidator<float>::textToNumber(const wxString &text) const
{
	double result;
	if (!text.ToDouble(&result))
		throw(std::string("Error: Invalid single precision floating point value."));
	if (std::abs(result) > std::numeric_limits<float>::max())
		throw(std::string("Error: Out of range single precision floating point value."));
	float finalResult = float(result);
	if (m_extraRangeChecks && (finalResult < m_minVal || finalResult > m_maxVal))
		throw(std::string("Error: out of range."));
	return finalResult;
}