#include"../include/svector/dep/serr.h"
#include"../include/svector/dep/nc.h"
#include<cstring>
void sci::NcFileBase::openReadOnly(const std::string &fileName)
{
	sci::assertThrow(!m_open, sci::err());
	sci::assertThrow(nc_open(fileName.c_str(), NC_NOWRITE, &m_id) == NC_NOERR, sci::err());
	m_open = true;
}

void sci::NcFileBase::openWritable(const std::string &fileName)
{
	sci::assertThrow(!m_open, sci::err());
	sci::assertThrow(nc_create(fileName.c_str(), NC_CLOBBER, &m_id) == NC_NOERR, sci::err());
	m_open = true;
}

void sci::NcFileBase::close()
{
	if (m_open)
	{
		m_open = false;
		sci::assertThrow(nc_close(m_id) == NC_NOERR, sci::err()); //we can get an error, but I assume the file isstill closed
	}
}

sci::InputNcFile::InputNcFile(const std::string &fileName)
{
	openReadOnly(fileName);
}

std::vector<std::string> sci::InputNcFile::getVariableNames()
{
	int nVars;
	sci::assertThrow(nc_inq_nvars(getId(), &nVars) == NC_NOERR, sci::err());
	std::vector<std::string> result(nVars);
	int count = 0;
	int id = 0;
	while (count<nVars)
	{
		char name[NC_MAX_NAME + 1];
		try
		{
			sci::assertThrow(nc_inq_varname(getId(), id, name) == NC_NOERR, sci::err());
			result[count] = std::string(name);
			++count;
		}
		catch (...)
		{
		}
		++id;
	}
	return result;
}

template<>
std::vector<double> sci::InputNcFile::getVariableFromId<double>(int id, size_t nValues)
{
	std::vector<double> result(nValues);
	if (nValues > 0)
		sci::assertThrow(nc_get_var_double(getId(), id, &result[0]) == NC_NOERR, sci::err());

	return result;
}

template<>
std::vector<float> sci::InputNcFile::getVariableFromId<float>(int id, size_t nValues)
{
	std::vector<float> result(nValues);
	if (nValues > 0)
		sci::assertThrow(nc_get_var_float(getId(), id, &result[0]) == NC_NOERR, sci::err());

	return result;
}

template<>
std::vector<short> sci::InputNcFile::getVariableFromId<short>(int id, size_t nValues)
{
	std::vector<short> result(nValues);
	if (nValues > 0)
		sci::assertThrow(nc_get_var_short(getId(), id, &result[0]) == NC_NOERR, sci::err());

	return result;
}

template<>
std::vector<int> sci::InputNcFile::getVariableFromId<int>(int id, size_t nValues)
{
	std::vector<int> result(nValues);
	if (nValues > 0)
		sci::assertThrow(nc_get_var_int(getId(), id, &result[0]) == NC_NOERR, sci::err());

	return result;
}

template<>
std::vector<long> sci::InputNcFile::getVariableFromId<long>(int id, size_t nValues)
{
	std::vector<long> result(nValues);
	if (nValues > 0)
		sci::assertThrow(nc_get_var_long(getId(), id, &result[0]) == NC_NOERR, sci::err());

	return result;
}

template<>
std::vector<int8_t> sci::InputNcFile::getVariableFromId<int8_t>(int id, size_t nValues)
{
	std::vector<int8_t> result(nValues);
	if (nValues > 0)
		sci::assertThrow(nc_get_var_schar(getId(), id, &result[0]) == NC_NOERR, sci::err());

	return result;
}

template<>
std::vector<uint8_t> sci::InputNcFile::getVariableFromId<uint8_t>(int id, size_t nValues)
{
	std::vector<uint8_t> result(nValues);
	if (nValues > 0)
		sci::assertThrow(nc_get_var_uchar(getId(), id, &result[0]) == NC_NOERR, sci::err());

	return result;
}

sci::NcDimension::NcDimension(const std::string &name, size_t length)
{
	setName(name);
	setLength(length);
	m_hasId = false;
}

void sci::NcDimension::setName(const std::string &name)
{
	m_name = name;
}

void sci::NcDimension::setLength(size_t length)
{
	m_length = length;
}

void sci::NcDimension::load(const InputNcFile &ncFile)
{
	sci::assertThrow(ncFile.isOpen(), sci::err());
	sci::assertThrow(nc_inq_dimid(ncFile.getId(), m_name.c_str(), &m_id) == NC_NOERR, sci::err());
	sci::assertThrow(nc_inq_dimlen(ncFile.getId(), m_id, &m_length) == NC_NOERR, sci::err());
	m_hasId = true;
}

void sci::NcDimension::write(const OutputNcFile &ncFile) const
{
	sci::assertThrow(ncFile.isOpen(), sci::err());
	sci::assertThrow(nc_def_dim(ncFile.getId(), m_name.c_str(), m_length, &m_id) == NC_NOERR, sci::err());
	m_hasId = true;
}


sci::NcDimension::NcDimension(const std::string &name, const InputNcFile &ncFile)
{
	setName(name);
	load(ncFile);
}

sci::NcDimension::NcDimension(const std::string &name, size_t length, const OutputNcFile &ncFile)
{
	setName(name);
	setLength(length);
	write(ncFile);
}


sci::NcAttribute::NcAttribute(const sci::NcAttribute &attribute)
{
	setNull();
	*this = attribute;
}
sci::NcAttribute::NcAttribute(sci::NcAttribute &&attribute)
{
	*this = std::move(attribute);
}
sci::NcAttribute& sci::NcAttribute::operator=(const sci::NcAttribute &attribute)
{
	if (this != &attribute)
	{
		m_name = attribute.m_name;
		m_nValues = attribute.m_nValues;
		m_nBytes = attribute.m_nBytes;
		if (m_values)
			free(m_values);
		m_values = malloc(m_nBytes);
		memcpy(m_values, attribute.m_values, m_nBytes);
		m_writeType = attribute.m_writeType;
	}
	return *this;
}
sci::NcAttribute& sci::NcAttribute::operator=(sci::NcAttribute &&attribute)
{
	if (this != &attribute)
	{
		std::swap(m_name, attribute.m_name);
		m_nValues = attribute.m_nValues;
		m_nBytes = attribute.m_nBytes;
		m_values = attribute.m_values;
		attribute.m_nValues = 0;
		attribute.m_nBytes = 0;
		attribute.m_values = nullptr;
	}
	return *this;
}


sci::NcAttribute::NcAttribute()
{
	setNull();
}

void sci::NcAttribute::setNull()
{
	m_nValues = 0;
	m_nBytes = 0;
	m_values = nullptr;
	m_writeType = NC_NAT;
}

template<>
sci::NcAttribute::NcAttribute(const std::string& name, std::string value)
{
	m_name = name;
	m_nValues = value.length();
	m_writeType = NC_CHAR;
	m_nBytes = m_nValues;
	if (m_nValues == 0)
		m_values = nullptr;
	else
	{
		m_values = malloc(m_nBytes);
		memcpy(m_values, &value[0], m_nBytes);
	}
}

template<>
sci::NcAttribute::NcAttribute(const std::string& name, const char *value)
{
	m_name = name;
	m_nValues = strlen(value);
	m_writeType = NC_CHAR;
	m_nBytes = m_nValues;
	if (m_nValues == 0)
		m_values = nullptr;
	else
	{
		m_values = malloc(m_nBytes);
		memcpy(m_values, &value[0], m_nBytes);
	}
}

void sci::NcAttribute::write(const sci::OutputNcFile & ncFile) const
{
	sci::assertThrow(ncFile.isOpen(), sci::err());
	sci::assertThrow(nc_put_att(ncFile.getId(), NC_GLOBAL, m_name.c_str(), m_writeType, m_nValues, m_values) == NC_NOERR, sci::err());
}

/*template<class T>
void sci::NcAttribute::setValues(const T *values, size_t nValues)
{
	if (m_values)
		delete[] values;
	m_values = new T*[nValues];
	memcpy(m_values, values, nValues * sizeof(T));
}*/


sci::OutputNcFile::OutputNcFile(const std::string &fileName)
{
	m_inDefineMode = true;
	openWritable(fileName);
}

sci::OutputNcFile::OutputNcFile()
{
	m_inDefineMode = true;
}