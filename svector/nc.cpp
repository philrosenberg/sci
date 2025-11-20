#include"../include/svector/serr.h"
#include"../include/svector/dep/nc.h"
#include"../include/svector/string.h"
#include"../include/svector/codepage.h"
#include<cstring>

void sci::checkNcCall(long errorCode)
{
	sci::NcError err(errorCode);
	if (errorCode != NC_NOERR)
		throw(err);
}

sci::NcError::NcError(long code)
	:m_code(code)
{
}

sci::err sci::NcError::getError() const
{
	std::string message;
	if (m_code <= 0)
		message = nc_strerror(m_code);
	else
		message = std::string("System error: ") + std::string(strerror(m_code));
	
	return sci::err(SERR_NC, m_code, message);
}

void sci::NcFileBase::openReadOnly(const sci::string &fileName, bool diskless)
{
	//annoyingly netcdf only provides an interface for using 8bit strings
	//and these strings are simply passed into the OS file API, so on windows
	//that means the ansi functions.
	//Convert our string to ansi. There is little point trying to convert
	//characters if they are not representable given the file will need to exist
	//anyway for the open to work.
	sci::assertThrow(!m_open, sci::err(SERR_NC, localNcError, "sci::NcFileBase::OpenReadOnly called when the file is already open."));
	int flag = NC_NOWRITE;
	if (diskless)
		flag = flag | NC_DISKLESS;
	checkNcCall(nc_open(sci::toCodepage(fileName).c_str(), flag, &m_id));
	m_open = true;
	m_fileName = fileName;
}

void sci::NcFileBase::openWritable(const sci::string &fileName, int formatFlags, bool diskless)
{
	sci::assertThrow(!m_open, sci::err(SERR_NC, localNcError, "sci::NcFileBase::OpenWritable called when the file is already open."));
	checkNcCall(nc_create(sci::toCodepage(fileName).c_str(), NC_CLOBBER | formatFlags | (diskless ? NC_DISKLESS : 0), &m_id));
	m_open = true;
	m_fileName = fileName;
}

void sci::NcFileBase::openWritable(const sci::string &fileName, char unicodeReplacementCharacter, int formatFlags, bool diskless)
{
	sci::assertThrow(!m_open, sci::err(SERR_NC, localNcError, "sci::NcFileBase::OpenWritable called when the file is already open."));
	checkNcCall(nc_create(sci::toCodepage(fileName, unicodeReplacementCharacter).c_str(), NC_CLOBBER | formatFlags | (diskless ? NC_DISKLESS : 0), &m_id));
	m_open = true;
	m_fileName = fileName;
}

void sci::NcFileBase::close()
{
	if (m_open)
	{
		m_fileName = sU("");
		m_open = false;
		checkNcCall(nc_close(m_id)); //we can get an error, but I assume the file isstill closed
	}
}

sci::InputNcFile::InputNcFile(const sci::string &fileName)
{
	openReadOnly(fileName);
}

std::vector<sci::string> sci::InputNcFile::getVariableNames()
{
	int nVars;
	checkNcCall(nc_inq_nvars(getId(), &nVars));
	std::vector<sci::string> result(nVars);
	int count = 0;
	int id = 0;
	while (count<nVars)
	{
		char8_t name[NC_MAX_NAME + 1];
		try
		{
			checkNcCall(nc_inq_varname(getId(), id, (char*)name));
			result[count] = sci::fromUtf8<sci::string>(name);
			++count;
		}
		catch (...)
		{
		}
		++id;
	}
	return result;
}

std::vector<size_t> sci::InputNcFile::getVariableShape(const sci::string& name)
{
	int varId;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(name).c_str(), &varId));
	int nDims;
	checkNcCall(nc_inq_varndims(getId(), varId, &nDims));
	std::vector<size_t> shape(nDims);
	if (nDims > 0)
	{
		std::vector<int> dimIds(nDims);
		checkNcCall(nc_inq_vardimid(getId(), varId, &dimIds[0]));
		for (int i = 0; i < nDims; ++i)
		{
			checkNcCall(nc_inq_dimlen(getId(), dimIds[i], &shape[i]));
		}
	}

	return shape;
}

template<>
double sci::InputNcFile::getGlobalAttribute<double>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested variable ") + name + sU(" as a single variable, but it is an array.")));
	
	double result;
	checkNcCall(nc_get_att_double(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result));

	return result;
}

template<>
float sci::InputNcFile::getGlobalAttribute<float>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested variable ") + name + sU(" as a single variable, but it is an array.")));
	
	float result;
	checkNcCall(nc_get_att_float(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result));

	return result;
}

template<>
short sci::InputNcFile::getGlobalAttribute<short>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested variable ") + name + sU(" as a single variable, but it is an array.")));
	
	short result;
	checkNcCall(nc_get_att_short(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result));

	return result;
}

template<>
int sci::InputNcFile::getGlobalAttribute<int>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested variable ") + name + sU(" as a single variable, but it is an array.")));
	
	int result;
	checkNcCall(nc_get_att_int(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result));

	return result;
}

template<>
long sci::InputNcFile::getGlobalAttribute<long>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested variable ") + name + sU(" as a single variable, but it is an array.")));
	
	long result;
	checkNcCall(nc_get_att_long(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result));

	return result;
}

template<>
int8_t sci::InputNcFile::getGlobalAttribute<int8_t>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested variable ") + name + sU(" as a single variable, but it is an array.")));

	int8_t result;
	checkNcCall(nc_get_att_schar(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result));

	return result;
}

template<>
uint8_t sci::InputNcFile::getGlobalAttribute<uint8_t>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested variable ") + name + sU(" as a single variable, but it is an array.")));

	uint8_t result;
	checkNcCall(nc_get_att_ubyte(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result));

	return result;
}

template<>
sci::string sci::InputNcFile::getGlobalAttribute<sci::string>(const sci::string &name)
{
	size_t nChars;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nChars));
	std::vector<char> characters(nChars + 1); // they are all initialised to 0
	checkNcCall(nc_get_att_text(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &characters[0]));

	std::u8string utf8String((char8_t*) & characters[0]);

	return sci::fromUtf8<sci::string>(utf8String);
}

template<>
sci::RawAttributeData sci::InputNcFile::getGlobalAttribute<sci::RawAttributeData>(const sci::string& name)
{
	sci::RawAttributeData result;
	checkNcCall(nc_inq_atttype(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result.type));
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result.nValues));
	result.data.resize(result.nValues*getNcDataSize(result.type));
	checkNcCall(nc_get_att(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), (void*)&result.data[0]));

	return result;
}

template<>
std::vector<double> sci::InputNcFile::getGlobalAttribute<std::vector<double>>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));

	std::vector<double> result(nValues);
	checkNcCall(nc_get_att_double(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result[0]));

	return result;
}

template<>
std::vector<float> sci::InputNcFile::getGlobalAttribute<std::vector<float>>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));

	std::vector<float> result(nValues);
	checkNcCall(nc_get_att_float(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result[0]));

	return result;
}

template<>
std::vector<short> sci::InputNcFile::getGlobalAttribute<std::vector<short>>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));

	std::vector<short> result(nValues);
	checkNcCall(nc_get_att_short(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result[0]));

	return result;
}

template<>
std::vector<int> sci::InputNcFile::getGlobalAttribute<std::vector<int>>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));

	std::vector<int> result(nValues);
	checkNcCall(nc_get_att_int(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result[0]));

	return result;
}

template<>
std::vector<long> sci::InputNcFile::getGlobalAttribute<std::vector<long>>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));

	std::vector<long> result(nValues);
	checkNcCall(nc_get_att_long(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result[0]));

	return result;
}

template<>
std::vector<int8_t> sci::InputNcFile::getGlobalAttribute<std::vector<int8_t>>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));

	std::vector<int8_t> result(nValues);
	checkNcCall(nc_get_att_schar(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result[0]));

	return result;
}

template<>
std::vector<uint8_t> sci::InputNcFile::getGlobalAttribute<std::vector<uint8_t>>(const sci::string &name)
{
	size_t nValues;
	checkNcCall(nc_inq_attlen(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &nValues));

	std::vector<uint8_t> result(nValues);
	checkNcCall(nc_get_att_uchar(getId(), NC_GLOBAL, (char*)sci::toUtf8(name).c_str(), &result[0]));

	return result;
}

std::vector<sci::string> sci::InputNcFile::getGlobalAttributeList()
{
	int nAttributes;
	checkNcCall(nc_inq_natts(getId(), &nAttributes));
	std::vector<sci::string> result(nAttributes);
	for (int i = 0; i < nAttributes; ++i)
	{
		std::vector<char8_t> thisName(NC_MAX_NAME + 1);
		nc_inq_attname(getId(), NC_GLOBAL, i, (char*)&thisName[0]);
		result[i] = sci::fromUtf8<sci::string>(&thisName[0]);
	}
	return result;
}


template<>
double sci::InputNcFile::getVariableAttribute<double>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested attribute ") + variableName + sU(":") + attributeName + sU(" as a single attribute, but it is an array.")));

	double result;
	checkNcCall(nc_get_att_double(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result));

	return result;
}
template<>
float sci::InputNcFile::getVariableAttribute<float>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested attribute ") + variableName + sU(":") + attributeName + sU(" as a single attribute, but it is an array.")));

	float result;
	checkNcCall(nc_get_att_float(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result));

	return result;
}
template<>
short sci::InputNcFile::getVariableAttribute<short>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested attribute ") + variableName + sU(":") + attributeName + sU(" as a single attribute, but it is an array.")));

	short result;
	checkNcCall(nc_get_att_short(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result));

	return result;
}
template<>
int sci::InputNcFile::getVariableAttribute<int>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested attribute ") + variableName + sU(":") + attributeName + sU(" as a single attribute, but it is an array.")));

	int result;
	checkNcCall(nc_get_att_int(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result));

	return result;
}
template<>
long sci::InputNcFile::getVariableAttribute<long>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested attribute ") + variableName + sU(":") + attributeName + sU(" as a single attribute, but it is an array.")));

	long result;
	checkNcCall(nc_get_att_long(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result));

	return result;
}
template<>
int8_t sci::InputNcFile::getVariableAttribute<int8_t>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested attribute ") + variableName + sU(":") + attributeName + sU(" as a single attribute, but it is an array.")));

	int8_t result;
	checkNcCall(nc_get_att_schar(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result));

	return result;
}
template<>
uint8_t sci::InputNcFile::getVariableAttribute<uint8_t>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));
	sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested attribute ") + variableName + sU(":") + attributeName + sU(" as a single attribute, but it is an array.")));

	uint8_t result;
	checkNcCall(nc_get_att_ubyte(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result));

	return result;
}
template<>
sci::string sci::InputNcFile::getVariableAttribute<sci::string>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	nc_type type;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));
	checkNcCall(nc_inq_atttype(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &type));
	sci::assertThrow(type == NC_CHAR || type == NC_STRING, sci::err(SERR_NC, -999, sU("Requested attribute ") + variableName + sU(":") + attributeName + sU(" as a string, but it is not a string or character type.")));
	
	sci::string result;
	if (type == NC_STRING)
	{
		sci::assertThrow(nValues == 1, sci::err(SERR_NC, -9999, sU("Requested attribute ") + variableName + sU(":") + attributeName + sU(" as a single attribute, but it is an array.")));

		char8_t* utf8String = nullptr;
		try
		{
			checkNcCall(nc_get_att_string(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), (char**) &utf8String));
			result = sci::fromUtf8<sci::string>(std::u8string(utf8String));
		}
		catch (...)
		{
			if (utf8String)
				nc_free_string(1, (char**) & utf8String);
		}
		if (utf8String)
			nc_free_string(1, (char**) & utf8String);
	}
	else
	{
		std::vector<char8_t> utf8String(nValues+1);
		utf8String[nValues] = '\0';
		checkNcCall(nc_get_att_text(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), (char*) &utf8String[0]));
		result = sci::fromUtf8<sci::string>(std::u8string(&utf8String[0]));
	}

	return result;
}

template<>
sci::RawAttributeData sci::InputNcFile::getVariableAttribute<sci::RawAttributeData>(const sci::string& variableName, const sci::string& attributeName)
{
	sci::RawAttributeData result;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_atttype(getId(), id, (char*)sci::toUtf8(variableName).c_str(), &result.type));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(variableName).c_str(), &result.nValues));
	result.data.resize(result.nValues*getNcDataSize(result.type));
	checkNcCall(nc_get_att(getId(), id, (char*)sci::toUtf8(variableName).c_str(), (void*)&result.data[0]));

	return result;
}

template<>
std::vector<double> sci::InputNcFile::getVariableAttribute<std::vector<double>>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));

	std::vector<double> result(nValues);
	checkNcCall(nc_get_att_double(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result[0]));

	return result;
}

template<>
std::vector<float> sci::InputNcFile::getVariableAttribute<std::vector<float>>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));

	std::vector<float> result(nValues);
	checkNcCall(nc_get_att_float(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result[0]));

	return result;
}

template<>
std::vector<short> sci::InputNcFile::getVariableAttribute<std::vector<short>>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));

	std::vector<short> result(nValues);
	checkNcCall(nc_get_att_short(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result[0]));

	return result;
}

template<>
std::vector<int> sci::InputNcFile::getVariableAttribute<std::vector<int>>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));

	std::vector<int> result(nValues);
	checkNcCall(nc_get_att_int(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result[0]));

	return result;
}

template<>
std::vector<long> sci::InputNcFile::getVariableAttribute<std::vector<long>>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));

	std::vector<long> result(nValues);
	checkNcCall(nc_get_att_long(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result[0]));

	return result;
}

template<>
std::vector<int8_t> sci::InputNcFile::getVariableAttribute<std::vector<int8_t>>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));

	std::vector<int8_t> result(nValues);
	checkNcCall(nc_get_att_schar(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result[0]));

	return result;
}

template<>
std::vector<uint8_t> sci::InputNcFile::getVariableAttribute<std::vector<uint8_t>>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));

	std::vector<uint8_t> result(nValues);
	checkNcCall(nc_get_att_ubyte(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &result[0]));

	return result;
}

template<>
std::vector<sci::string> sci::InputNcFile::getVariableAttribute<std::vector<sci::string>>(const sci::string& variableName, const sci::string& attributeName)
{
	size_t nValues;
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	checkNcCall(nc_inq_attlen(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), &nValues));

	std::vector<char8_t*> utf8Strings(nValues, nullptr);
	std::vector<sci::string> result(nValues);
	try
	{
		checkNcCall(nc_get_att_string(getId(), id, (char*)sci::toUtf8(attributeName).c_str(), (char**) & utf8Strings[0]));
		for (size_t i = 0; i < result.size(); ++i)
			result[i] = sci::fromUtf8<sci::string>(std::u8string(utf8Strings[i]));
	}
	catch (...)
	{
		size_t count = 0;
		for (size_t i = 0; i < utf8Strings.size(); ++i)
			if (utf8Strings[i])
				++count;
		if (count > 0)
			nc_free_string(count, (char**) & utf8Strings[0]);
	}
	nc_free_string(utf8Strings.size(), (char**) & utf8Strings[0]);

	return result;
}

std::vector<sci::string> sci::InputNcFile::getVariableAttributeList(const sci::string& variableName)
{
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	int nAttributes;
	checkNcCall(nc_inq_varnatts(getId(), id, &nAttributes));
	std::vector<sci::string> result(nAttributes);
	for (int i = 0; i < nAttributes; ++i)
	{
		std::vector<char8_t> thisName(NC_MAX_NAME + 1);
		nc_inq_attname(getId(), id, i, (char*) & thisName[0]);
		result[i] = sci::fromUtf8<sci::string>(std::u8string(&thisName[0]));
	}
	return result;
}

std::vector<sci::string> sci::InputNcFile::getVariableDimensions(const sci::string& variableName)
{
	int id;
	checkNcCall(nc_inq_varid(getId(), (char*)sci::toUtf8(variableName).c_str(), &id));
	int nDims;
	checkNcCall(nc_inq_varndims(getId(), id, &nDims));
	std::vector<int> dimIds(nDims);
	checkNcCall(nc_inq_vardimid(getId(), id, &dimIds[0]));
	std::vector<sci::string> result(nDims);
	for (size_t i = 0; i < result.size(); ++i)
	{
		char8_t utf8Name[NC_MAX_NAME + 1];
		checkNcCall(nc_inq_dimname(getId(), dimIds[i], (char*)utf8Name));
		result[i] = sci::fromUtf8<sci::string>(std::u8string(utf8Name));
	}
	return result;
}

sci::NcDimension::NcDimension(const sci::string &name, size_t length)
{
	setName(name);
	setLength(length);
	m_hasId = false;
}

void sci::NcDimension::setName(const sci::string &name)
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
	checkNcCall(nc_inq_dimid(ncFile.getId(), (char*)sci::toUtf8(m_name).c_str(), &m_id));
	checkNcCall(nc_inq_dimlen(ncFile.getId(), m_id, &m_length));
	m_hasId = true;
}

void sci::NcDimension::write(const OutputNcFile &ncFile) const
{
	sci::assertThrow(ncFile.isOpen(), sci::err(SERR_NC, localNcError, "sci::NcDimension::write called before the file was opened."));
	checkNcCall(nc_def_dim(ncFile.getId(), (char*)sci::toUtf8(m_name).c_str(), m_length, &m_id));
	m_hasId = true;
}


sci::NcDimension::NcDimension(const sci::string &name, const InputNcFile &ncFile)
{
	setName(name);
	load(ncFile);
}

sci::NcDimension::NcDimension(const sci::string &name, size_t length, const OutputNcFile &ncFile)
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

sci::NcAttribute::NcAttribute(const sci::string& name, sci::string value)
{
	m_name = name;
	std::u8string utf8String = sci::toUtf8(value);
	m_nValues = utf8String.length() + 1;
	m_writeType = NC_CHAR;
	m_nBytes = m_nValues;
	m_values = malloc(m_nBytes);
	memcpy(m_values, utf8String.data(), m_nBytes);
}

sci::NcAttribute::NcAttribute(const sci::string& name, const std::vector<sci::string> &value, const sci::string &separator)
{
	m_name = name;
	sci::ostringstream stream;
	if (value.size() > 0)
		stream << value[0];
	for (size_t i = 1; i < value.size(); ++i)
		stream << separator << value[i];
	std::u8string utf8String = sci::toUtf8(stream.str());
	m_nValues = utf8String.length() + 1;
	m_writeType = NC_CHAR;
	m_nBytes = m_nValues;
	m_values = malloc(m_nBytes);
	memcpy(m_values, utf8String.data(), m_nBytes);
}


sci::NcAttribute::NcAttribute(const sci::string& name, const std::vector<const char16_t *> &value)
{
	m_name = name;
	sci::ostringstream stream;
	if (value.size() > 0)
		stream << value[0];
	for (size_t i = 1; i < value.size(); ++i)
		stream << sU(", ") << value[i];
	std::u8string utf8String = sci::toUtf8(stream.str());
	m_nValues = utf8String.length() + 1;
	m_writeType = NC_CHAR;
	m_nBytes = m_nValues;
	m_values = malloc(m_nBytes);
	memcpy(m_values, utf8String.data(), m_nBytes);
}

sci::NcAttribute::NcAttribute(const sci::string& name, const char16_t *value)
{
	m_name = name;
	std::u8string utf8String = sci::toUtf8(value);
	m_nValues = utf8String.length() + 1;
	m_writeType = NC_CHAR;
	m_nBytes = m_nValues;
	m_values = malloc(m_nBytes);
	memcpy(m_values, utf8String.data(), m_nBytes);
}

sci::NcAttribute::NcAttribute(const sci::string& name, const sci::RawAttributeData& value)
{
	m_name = name;
	m_nValues = value.nValues;
	m_writeType = value.type;
	m_nBytes = value.data.size();
	m_values = malloc(m_nBytes);
	memcpy(m_values, &value.data[0], m_nBytes);
}

void sci::NcAttribute::write(const sci::OutputNcFile & ncFile) const
{
	sci::assertThrow(ncFile.isOpen(), sci::err(SERR_NC, localNcError, "sci::NcAttribute::write called before the file was opened."));
	checkNcCall(nc_put_att(ncFile.getId(), NC_GLOBAL, (char*)sci::toUtf8(m_name).c_str(), m_writeType, m_nValues, m_values));
}

/*template<class T>
void sci::NcAttribute::setValues(const T *values, size_t nValues)
{
	if (m_values)
		delete[] values;
	m_values = new T*[nValues];
	memcpy(m_values, values, nValues * sizeof(T));
}*/


sci::OutputNcFile::OutputNcFile(const sci::string &fileName, bool diskless)
{
	m_inDefineMode = true;
	m_flags = NC_NETCDF4; //use netcdf4 by default - would be nice to check for support for this and use netcdf3 if needed, but it doesn't seem possible
	openWritable(fileName, diskless);
}

sci::OutputNcFile::OutputNcFile(const sci::string& fileName, char unicodeReplacementCharacter, bool diskless)
{
	m_inDefineMode = true;
	m_flags = NC_NETCDF4; //use netcdf4 by default - would be nice to check for support for this and use netcdf3 if needed, but it doesn't seem possible
	openWritable(fileName, unicodeReplacementCharacter, diskless);
}

sci::OutputNcFile::OutputNcFile()
{
	m_inDefineMode = true;
	m_flags = NC_NETCDF4;
}

sci::OutputNcFile::OutputNcFile(const sci::string& fileName, int flags, bool diskless)
{
	m_inDefineMode = true;
	m_flags = flags;
	openWritable(fileName, diskless);
}

sci::OutputNcFile::OutputNcFile(const sci::string& fileName, char unicodeReplacementCharacter, int flags, bool diskless)
{
	m_inDefineMode = true;
	m_flags = flags;
	openWritable(fileName, unicodeReplacementCharacter, diskless);
}

sci::OutputNcFile::OutputNcFile(int flags)
{
	m_inDefineMode = true;
	m_flags = flags;
}

void sci::OutputNcFile::openWritable(const sci::string& fileName, bool diskless)
{
	NcFileBase::openWritable(fileName, m_flags, diskless);
}

void sci::OutputNcFile::openWritable(const sci::string& fileName, char unicodeReplacementCharacter, bool diskless)
{
	NcFileBase::openWritable(fileName, unicodeReplacementCharacter, m_flags, diskless);
}
