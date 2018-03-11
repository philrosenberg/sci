#include"../include/svector/dep/serr.h"
#include"../include/svector/dep/nc.h"
#include"../include/svector/dep/sstring.h"
#include<cstring>

void sci::checkNcCall(long errorCode)
{
	sci::NcError err(errorCode);
	sci::assertThrow(errorCode == NC_NOERR, err.getError());
}

sci::NcError::NcError(long code)
	:m_code(code)
{
}

sci::err sci::NcError::getError() const
{
	std::string message;
	switch (m_code)
	{
	case    NC_NOERR            : message = "No Error."; break;
	case    NC2_ERR             : message = "Returned for all errors in the v2 API."; break;
	case	NC_EBADID           : message = "Not a netcdf id"; break;
	case	NC_ENFILE           : message = "Too many netcdfs open"; break;
	case	NC_EEXIST           : message = "netcdf file exists && NC_NOCLOBBER"; break;
	case	NC_EINVAL           : message = "Invalid Argument"; break;
	case	NC_EPERM            : message = "Write to read only"; break;
	case	NC_ENOTINDEFINE     : message = "Operation not allowed in data mode"; break;
	case	NC_EINDEFINE        : message = "Operation not allowed in define mode"; break;
	case	NC_EINVALCOORDS     : message = "Index exceeds dimension bound"; break;
	case	NC_EMAXDIMS         : message = "NC_MAX_DIMS exceeded"; break;
	case	NC_ENAMEINUSE       : message = "String match to name in use"; break;
	case	NC_ENOTATT          : message = "Attribute not found"; break;
	case	NC_EMAXATTS         : message = "NC_MAX_ATTRS exceeded"; break;
	case	NC_EBADTYPE         : message = "Not a netcdf data type"; break;
	case	NC_EBADDIM          : message = "Invalid dimension id or name"; break;
	case	NC_EUNLIMPOS        : message = "NC_UNLIMITED in the wrong index"; break;
	case	NC_EMAXVARS         : message = "NC_MAX_VARS exceeded"; break;
	case	NC_ENOTVAR          : message = "Variable not found"; break;
	case	NC_EGLOBAL          : message = "Action prohibited on NC_GLOBAL varid"; break;
	case	NC_ENOTNC           : message = "Not a netcdf file"; break;
	case	NC_ESTS             : message = "In Fortran, string too short"; break;
	case	NC_EMAXNAME         : message = "NC_MAX_NAME exceeded"; break;
	case	NC_EUNLIMIT         : message = "NC_UNLIMITED size already in use"; break;
	case	NC_ENORECVARS       : message = "nc_rec op when there are no record vars"; break;
	case	NC_ECHAR            : message = "Attempt to convert between text & numbers"; break;
	case	NC_EEDGE            : message = "Start+count exceeds dimension bound"; break;
	case	NC_ESTRIDE          : message = "Illegal stride"; break;
	case	NC_EBADNAME         : message = "Attribute or variable name contains illegal characters"; break;
	case	NC_ERANGE           : message = "Math result not representable"; break;
	case	NC_ENOMEM           : message = "Memory allocation (malloc) failure"; break;
	case	NC_EVARSIZE         : message = "One or more variable sizes violate."; break;
	case	NC_EDIMSIZE         : message = "Invalid dimension size."; break;
	case	NC_ETRUNC           : message = "File likely truncated or possibly corrupted."; break;
	case	NC_EAXISTYPE        : message = "Unknown axis type."; break;
	case	NC_EDAP             : message = "Generic DAP client error."; break;
	case	NC_ECURL            : message = "Generic libcurl error."; break;
	case	NC_EIO              : message = "Generic IO error."; break;
	case	NC_ENODATA          : message = "Attempt to access variable with no data."; break;
	case	NC_EDAPSVC          : message = "DAP Server side error."; break;
	case	NC_EDAS             : message = "Malformed or inaccessible DAS."; break;
	case	NC_EDDS             : message = "Malformed or inaccessible DDS."; break;
	case	NC_EDATADDS         : message = "Malformed or inaccessible DATADDS."; break;
	case	NC_EDAPURL          : message = "Malformed DAP URL."; break;
	case	NC_EDAPCONSTRAINT   : message = "Malformed DAP Constraint."; break;
	}
	return sci::err(SERR_NC, m_code, message);
}

void sci::NcFileBase::openReadOnly(const std::string &fileName)
{
	sci::assertThrow(!m_open, sci::err(SERR_NC, localNcError, "sci::NcFileBase::OpenReadOnly called when the file is already open."));
	checkNcCall(nc_open(fileName.c_str(), NC_NOWRITE, &m_id));
	m_open = true;
}

void sci::NcFileBase::openReadOnly(const std::wstring &fileName)
{
	sci::assertThrow(!m_open, sci::err(SERR_NC, localNcError, "sci::NcFileBase::OpenReadOnly called when the file is already open."));
	checkNcCall(nc_open(utf16To8(fileName).c_str(), NC_NOWRITE, &m_id));
	m_open = true;
}

void sci::NcFileBase::openWritable(const std::string &fileName)
{
	sci::assertThrow(!m_open, sci::err(SERR_NC, localNcError, "sci::NcFileBase::OpenWritable called when the file is already open."));
	checkNcCall(nc_create(fileName.c_str(), NC_CLOBBER, &m_id));
	m_open = true;
}

void sci::NcFileBase::openWritable(const std::wstring &fileName)
{
	sci::assertThrow(!m_open, sci::err(SERR_NC, localNcError, "sci::NcFileBase::OpenWritable called when the file is already open."));
	checkNcCall(nc_create(utf16To8(fileName).c_str(), NC_CLOBBER, &m_id));
	m_open = true;
}

void sci::NcFileBase::close()
{
	if (m_open)
	{
		m_open = false;
		checkNcCall(nc_close(m_id)); //we can get an error, but I assume the file isstill closed
	}
}

sci::InputNcFile::InputNcFile(const std::string &fileName)
{
	openReadOnly(fileName);
}

sci::InputNcFile::InputNcFile(const std::wstring &fileName)
{
	openReadOnly(fileName);
}

std::vector<std::string> sci::InputNcFile::getVariableNames()
{
	int nVars;
	checkNcCall(nc_inq_nvars(getId(), &nVars));
	std::vector<std::string> result(nVars);
	int count = 0;
	int id = 0;
	while (count<nVars)
	{
		char name[NC_MAX_NAME + 1];
		try
		{
			checkNcCall(nc_inq_varname(getId(), id, name));
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
		checkNcCall(nc_get_var_double(getId(), id, &result[0]));

	return result;
}

template<>
std::vector<float> sci::InputNcFile::getVariableFromId<float>(int id, size_t nValues)
{
	std::vector<float> result(nValues);
	if (nValues > 0)
		checkNcCall(nc_get_var_float(getId(), id, &result[0]));

	return result;
}

template<>
std::vector<short> sci::InputNcFile::getVariableFromId<short>(int id, size_t nValues)
{
	std::vector<short> result(nValues);
	if (nValues > 0)
		checkNcCall(nc_get_var_short(getId(), id, &result[0]));

	return result;
}

template<>
std::vector<int> sci::InputNcFile::getVariableFromId<int>(int id, size_t nValues)
{
	std::vector<int> result(nValues);
	if (nValues > 0)
		checkNcCall(nc_get_var_int(getId(), id, &result[0]));

	return result;
}

template<>
std::vector<long> sci::InputNcFile::getVariableFromId<long>(int id, size_t nValues)
{
	std::vector<long> result(nValues);
	if (nValues > 0)
		checkNcCall(nc_get_var_long(getId(), id, &result[0]));

	return result;
}

template<>
std::vector<int8_t> sci::InputNcFile::getVariableFromId<int8_t>(int id, size_t nValues)
{
	std::vector<int8_t> result(nValues);
	if (nValues > 0)
		checkNcCall(nc_get_var_schar(getId(), id, &result[0]));

	return result;
}

template<>
std::vector<uint8_t> sci::InputNcFile::getVariableFromId<uint8_t>(int id, size_t nValues)
{
	std::vector<uint8_t> result(nValues);
	if (nValues > 0)
		checkNcCall(nc_get_var_uchar(getId(), id, &result[0]));

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
	sci::assertThrow(ncFile.isOpen(), sci::err(SERR_NC, localNcError, "sci::NcDimension::load called before the file was opened."));
	checkNcCall(nc_inq_dimid(ncFile.getId(), m_name.c_str(), &m_id));
	checkNcCall(nc_inq_dimlen(ncFile.getId(), m_id, &m_length));
	m_hasId = true;
}

void sci::NcDimension::write(const OutputNcFile &ncFile) const
{
	sci::assertThrow(ncFile.isOpen(), sci::err(SERR_NC, localNcError, "sci::NcDimension::write called before the file was opened."));
	checkNcCall(nc_def_dim(ncFile.getId(), m_name.c_str(), m_length, &m_id));
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
		m_writeType = attribute.m_writeType;
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
sci::NcAttribute::NcAttribute(const std::string& name, std::wstring value)
{
	m_name = name;
	std::string utf8Value = sci::utf16To8(value);
	m_nValues = utf8Value.length();
	m_writeType = NC_CHAR;
	m_nBytes = m_nValues;
	if (m_nValues == 0)
		m_values = nullptr;
	else
	{
		m_values = malloc(m_nBytes);
		memcpy(m_values, &utf8Value[0], m_nBytes);
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
	sci::assertThrow(ncFile.isOpen(), sci::err(SERR_NC, localNcError, "sci::NcAttribute::write called before the file was opened."));
	checkNcCall(nc_put_att(ncFile.getId(), NC_GLOBAL, m_name.c_str(), m_writeType, m_nValues, m_values));
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

sci::OutputNcFile::OutputNcFile(const std::wstring &fileName)
{
	m_inDefineMode = true;
	openWritable(fileName);
}

sci::OutputNcFile::OutputNcFile()
{
	m_inDefineMode = true;
}