#ifndef SVECTOR_NC_H
#define SVECTOR_NC_H
#include<string>
#include<vector>
#include<netcdf.h>
#include<algorithm>
#include"../serr.h"
#include"../svector.h"
#include"../sstring.h"
#include"../Units.h"

namespace sci_internal
{
	template <class T>
	struct NcTraits
	{
	};
	template <>
	struct NcTraits<int8_t>
	{
		const static nc_type ncType = NC_CHAR;
	};
	template <>
	struct NcTraits<uint8_t>
	{
		const static nc_type ncType = NC_BYTE;
	};
	template <>
	struct NcTraits<int16_t>
	{
		const static nc_type ncType = NC_SHORT;
	};
	template <>
	struct NcTraits<int32_t>
	{
		const static nc_type ncType = NC_INT;
	};
	template <>
	struct NcTraits<float>
	{
		const static nc_type ncType = NC_FLOAT;
	};
	template <>
	struct NcTraits<double>
	{
		const static nc_type ncType = NC_DOUBLE;
	};
	template <class T, class VALUE_TYPE>
	struct NcTraits<sci::Physical<T, VALUE_TYPE>>
	{
		const static nc_type ncType = NcTraits<VALUE_TYPE>::ncType;
	};

	template<nc_type I>
	struct NcTypeTraits
	{

	};
	template<>
	struct NcTypeTraits<NC_BYTE>
	{
		typedef int8_t type;
	};
#ifdef USE_NETCDF4
	template <>
	struct NcTraits<uint16_t>
	{
		const static nc_type ncType = NC_USHORT;
	};
	template <>
	struct NcTraits<uint32_t>
	{
		const static nc_type ncType = NC_UINT;
	};
	template <>
	struct NcTraits<int64_t>
	{
		const static nc_type ncType = NC_INT64;
	};
	template <>
	struct NcTraits<uint64_t>
	{
		const static nc_type ncType = NC_UINT64;
	};
	template <>
	struct NcTraits<uint32_t>
	{
		const static nc_type ncType = NC_INT;
	};
#endif
}


namespace sci
{
	void checkNcCall(long errorCode);
	class OutputNcFile;
	class NcDimension;
	template<class T> class NcVariable;

	const long localNcError = -10000;


	class NcError
	{
	public:
		NcError(long code);
		sci::err getError() const;
	private:
		const long m_code;
	};

	inline size_t getNcDataSize(nc_type type)
	{
		if (type == NC_NAT)
			return 0;
		else if (type == NC_BYTE || type == NC_CHAR || type == NC_UBYTE || type == NC_STRING)
			return 1;
		else if (type == NC_SHORT || type == NC_USHORT)
			return 2;
		else if (type == NC_INT || type == NC_UINT)
			return 4;
		else if (type == NC_INT64 || type == NC_UINT64)
			return 8;
		else if (type == NC_FLOAT)
			return 4;
		else if (type == NC_DOUBLE)
			return 8;

		return 0;
	}


	struct RawAttributeData
	{
		RawAttributeData(nc_type t = NC_NAT, const std::vector<uint8_t>& d = std::vector<uint8_t>(0))
			:type(t), data(d)
		{
			if (type == NC_NAT)
				nValues = 0;
			else
				nValues = data.size() / getNcDataSize(type);
		}
		nc_type type;
		std::vector<uint8_t> data;
		size_t nValues;
	};

	class NcAttribute
	{
	public:
		//copy, assignment, move
		NcAttribute(const NcAttribute &attribute);
		NcAttribute(NcAttribute &&attribute);
		NcAttribute &operator=(const NcAttribute &attribute);
		NcAttribute &operator=(NcAttribute &&attribute);


		NcAttribute();

		template<class T>
		NcAttribute(const sci::string& name, T value);
		template<class T, class WRITETYPE>
		NcAttribute(const sci::string& name, T value, const WRITETYPE &);
		template<class T>
		NcAttribute(const sci::string& name, std::vector<T> values);
		template<class T, class WRITETYPE>
		NcAttribute(const sci::string& name, std::vector<T> values, const WRITETYPE &);
		NcAttribute(const sci::string& name, sci::string value);
		NcAttribute(const sci::string& name, const char16_t *value);
		NcAttribute(const sci::string& name, const std::vector<sci::string> &value, const sci::string &separator=sU(", "));
		NcAttribute(const sci::string& name, const std::vector<const char16_t*>& value);
		NcAttribute(const sci::string& name, const RawAttributeData &value);
		~NcAttribute()
		{
			if (m_values)
			{
				free (m_values);
			}
		}

		void write(const sci::OutputNcFile & ncFile) const;
		template<class T>
		void write(const sci::OutputNcFile & ncFile, const sci::NcVariable<T> &variable) const;
	private:

		sci::string m_name;
		size_t m_nValues;
		void* m_values;
		size_t m_nBytes;
		nc_type m_writeType;

		void setNull();
	};

	class NcFileBase
	{
	public:
#pragma warning(push)
#pragma warning(disable : 26495)
		NcFileBase() { m_open = false; }
#pragma warning(pop)
		virtual ~NcFileBase() { close(); };
		void openReadOnly(const sci::string &fileName, bool diskless = false);
		void openWritable(const sci::string &fileName, int formatFlags, bool diskless = false);
		//This version permits the filename to have unicode characters not represented
		//by the current codepage to be replaced by another character. Only really of
		//use on Windows because netcdf on windows only permits ansi file names.
		void openWritable(const sci::string &fileName, char unicodeReplacementCharacter, int formatFlags, bool diskless = false);
		void close();
		bool isOpen() const { return m_open; }
		int getId() const { return m_id; }
		//add move constructors
		NcFileBase(NcFileBase&& other)
		{
			m_open = other.m_open;
			m_id = other.m_id;
			other.m_open = false;
		}
		NcFileBase operator=(NcFileBase&& other)
		{
			m_open = other.m_open;
			m_id = other.m_id;
			other.m_open = false;
		}
	private:
		bool m_open;
		int m_id;

		//remove copy constructors
		NcFileBase(const NcFileBase&) = delete;
		NcFileBase operator=(const NcFileBase&) = delete;
	};

	class InputNcFile : public NcFileBase
	{
	public:
		InputNcFile(const sci::string &fileName);
		template<class T>
		std::vector<T> getVariable(const sci::string &name);
		template<class T>
		std::vector<T> getVariable(const sci::string &name, std::vector<size_t> &shape);
		template<class T>
		std::vector<T> getVariable(const sci::string& name, const std::vector<size_t> &start, const std::vector<size_t> &shape);
		std::vector<size_t> getVariableShape(const sci::string& name);
		std::vector<sci::string>getVariableNames();
		template<class T>
		T getGlobalAttribute(const sci::string &name);
		std::vector<sci::string> getGlobalAttributeList();
		template<class T>
		T getVariableAttribute(const sci::string& variableName, const sci::string& attributeName);
		std::vector<sci::string> getVariableAttributeList(const sci::string& variableName);
		std::vector<sci::string> getVariableDimensions(const sci::string& variableName);
	private:
		template<class T>
		std::vector<T> getVariableFromId(int id, size_t nValues);
		template<class T>
		std::vector<T> getVariableFromId(int id, const std::vector<size_t>& start, const std::vector<size_t>& shape);
		//template<class T>
		//T getVariableAttributeFromId(int variableId, const sci::string& AttributeName);
	};

	template<>
	std::vector<double> InputNcFile::getVariableFromId<double>(int id, size_t nValues);
	template<>
	std::vector<float> InputNcFile::getVariableFromId<float>(int id, size_t nValues);
	template<>
	std::vector<short> InputNcFile::getVariableFromId<short>(int id, size_t nValues);
	template<>
	std::vector<int> InputNcFile::getVariableFromId<int>(int id, size_t nValues);
	template<>
	std::vector<long> InputNcFile::getVariableFromId<long>(int id, size_t nValues);
	template<>
	std::vector<int8_t> InputNcFile::getVariableFromId<int8_t>(int id, size_t nValues);
	template<>
	std::vector<uint8_t> InputNcFile::getVariableFromId<uint8_t>(int id, size_t nValues);
	template<>
	std::vector<char> InputNcFile::getVariableFromId<char>(int id, size_t nValues);


	template<>
	std::vector<double> InputNcFile::getVariableFromId<double>(int id, const std::vector<size_t>& start, const std::vector<size_t>& shape);
	template<>
	std::vector<float> InputNcFile::getVariableFromId<float>(int id, const std::vector<size_t>& start, const std::vector<size_t>& shape);
	template<>
	std::vector<short> InputNcFile::getVariableFromId<short>(int id, const std::vector<size_t>& start, const std::vector<size_t>& shape);
	template<>
	std::vector<int> InputNcFile::getVariableFromId<int>(int id, const std::vector<size_t>& start, const std::vector<size_t>& shape);
	template<>
	std::vector<long> InputNcFile::getVariableFromId<long>(int id, const std::vector<size_t>& start, const std::vector<size_t>& shape);
	template<>
	std::vector<int8_t> InputNcFile::getVariableFromId<int8_t>(int id, const std::vector<size_t>& start, const std::vector<size_t>& shape);
	template<>
	std::vector<uint8_t> InputNcFile::getVariableFromId<uint8_t>(int id, const std::vector<size_t>& start, const std::vector<size_t>& shape);


	template<>
	double InputNcFile::getGlobalAttribute<double>(const sci::string &name);
	template<>
	float InputNcFile::getGlobalAttribute<float>(const sci::string &name);
	template<>
	short InputNcFile::getGlobalAttribute<short>(const sci::string &name);
	template<>
	int InputNcFile::getGlobalAttribute<int>(const sci::string &name);
	template<>
	long InputNcFile::getGlobalAttribute<long>(const sci::string &name);
	template<>
	int8_t InputNcFile::getGlobalAttribute<int8_t>(const sci::string &name);
	template<>
	uint8_t InputNcFile::getGlobalAttribute<uint8_t>(const sci::string &name);
	template<>
	sci::string InputNcFile::getGlobalAttribute<sci::string>(const sci::string &name);
	template<>
	RawAttributeData InputNcFile::getGlobalAttribute<RawAttributeData>(const sci::string& name);

	template<>
	std::vector<double> InputNcFile::getGlobalAttribute<std::vector<double>>(const sci::string &name);
	template<>
	std::vector<float> InputNcFile::getGlobalAttribute<std::vector<float>>(const sci::string &name);
	template<>
	std::vector<short> InputNcFile::getGlobalAttribute<std::vector<short>>(const sci::string &name);
	template<>
	std::vector<int> InputNcFile::getGlobalAttribute<std::vector<int>>(const sci::string &name);
	template<>
	std::vector<long> InputNcFile::getGlobalAttribute<std::vector<long>>(const sci::string &name);
	template<>
	std::vector<int8_t> InputNcFile::getGlobalAttribute<std::vector<int8_t>>(const sci::string &name);
	template<>
	std::vector<uint8_t> InputNcFile::getGlobalAttribute<std::vector<uint8_t>>(const sci::string &name);
	template<>
	std::vector<sci::string> InputNcFile::getGlobalAttribute<std::vector<sci::string>>(const sci::string &name);

	template<>
	double InputNcFile::getVariableAttribute<double>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	float InputNcFile::getVariableAttribute<float>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	short InputNcFile::getVariableAttribute<short>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	int InputNcFile::getVariableAttribute<int>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	long InputNcFile::getVariableAttribute<long>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	int8_t InputNcFile::getVariableAttribute<int8_t>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	uint8_t InputNcFile::getVariableAttribute<uint8_t>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	sci::string InputNcFile::getVariableAttribute<sci::string>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	RawAttributeData InputNcFile::getVariableAttribute<RawAttributeData>(const sci::string& variableName, const sci::string& attributeName);

	template<>
	std::vector<double> InputNcFile::getVariableAttribute<std::vector<double>>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	std::vector<float> InputNcFile::getVariableAttribute<std::vector<float>>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	std::vector<short> InputNcFile::getVariableAttribute<std::vector<short>>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	std::vector<int> InputNcFile::getVariableAttribute<std::vector<int>>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	std::vector<long> InputNcFile::getVariableAttribute<std::vector<long>>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	std::vector<int8_t> InputNcFile::getVariableAttribute<std::vector<int8_t>>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	std::vector<uint8_t> InputNcFile::getVariableAttribute<std::vector<uint8_t>>(const sci::string& variableName, const sci::string& attributeName);
	template<>
	std::vector<sci::string> InputNcFile::getVariableAttribute<std::vector<sci::string>>(const sci::string& variableName, const sci::string& attributeName);

	//This class encapsulates a netcdf variable. If you want you can inherit from this class to allow
	//you to write other types of data than just (multi-d) vectors or built in types.
	//If you do this then you probably need to override the flattenData static method to convert
	//data from your input format to a 1d vector of a type that netcdf can write. the vector type
	//must match the write_type typedef of the NcVariable class you are inheriting from. If your
	//data is not in multi-d vectors with one data point per element then you may need to override
	//the static method getDataShape too.
	//If you do this then you can call ncOutputFile::write with your custom data type. But note that
	//this utilises template specialization, not inheritance via virtual functions, so you must use
	//the derived class in the call to write
	//See the NcVariable<Physical<T>> specialization below for an example.
	template<class T>
	class NcVariable
	{
	public:
		NcVariable(sci::string name, const OutputNcFile &ncFile, const NcDimension& dimension, int deflateLevel0to9=0, bool shuffle = false);
		NcVariable(sci::string name, const OutputNcFile &ncFile, const std::vector<NcDimension *> &dimensions, int deflateLevel0to9 = 0, bool shuffle = false);
		NcVariable(NcVariable &&) = default;
		//NcVariable(sci::string name, const OutputNcFile &ncFile, const std::vector<T> &data, const NcDimension& dimension);
		//NcVariable(sci::string name, const OutputNcFile &ncFile, const std::vector<std::vector<T>> &data, const std::vector<const NcDimension&> &dimensions);
		void addAttribute(const NcAttribute &attribute, const OutputNcFile &file)
		{
			m_attributes.push_back(attribute);
			//if we already have the ID (i.e. the variable has been written) then write the attribute
			if(m_hasId)
				attribute.write(file, *this);
		}
		void write(const OutputNcFile &file) const;
		int getId() const
		{
			sci::assertThrow(m_hasId, sci::err(SERR_NC, localNcError, "NcVariable::getId called before the variable has got an id from being written."));
			return m_id;
		}
		size_t getNDimensions() const { return m_dimensionIds.size(); }
		bool hasId() const { return m_hasId; }
		template<class U>
		static std::vector<size_t> getDataShape(const U &data)
		{
			return sci::shape(data);
		}
		static std::vector<T> flattenData(const std::vector<T> & data) { return data; }
		template<class U>
		static std::vector<T> flattenData(const std::vector<std::vector<U>> & data)
		{
			std::vector<T> flattenedData;
			sci::flatten(flattenedData, data);
			return flattenedData;
		}
		typedef T write_type;
	private:
		mutable bool m_hasId;
		mutable int m_id;
		std::vector<NcAttribute> m_attributes;
		std::vector<int> m_dimensionIds;
		sci::string m_name;
		int m_deflateLevel;
		bool m_shuffle;

		//remove copy constructors
		NcVariable(const NcVariable&) = delete;
		NcVariable operator=(const NcVariable&) = delete;
	};

	template<class T>
	class NcVariable<Physical<T, double>> : public NcVariable<double>
	{
	public:
		NcVariable(sci::string name, const OutputNcFile &ncFile, const NcDimension& dimension, int deflateLevel0to9 = 0, bool shuffle = false) : NcVariable<double>(name, ncFile, dimension, deflateLevel0to9, shuffle) {}
		NcVariable(sci::string name, const OutputNcFile &ncFile, const std::vector<NcDimension *> &dimensions, int deflateLevel0to9 = 0, bool shuffle = false) : NcVariable<double>(name, ncFile, dimensions, deflateLevel0to9, shuffle) {}
		NcVariable(NcVariable &&) = default;
		template<class U>
		static std::vector<double> flattenData(const std::vector<Physical<U, double>> &data) { return sci::physicalsToValues<sci::Physical<T, double>>(data); }
		template<class U>
		static::std::vector<double> flattenData(const std::vector<std::vector<U>> &data)
		{
			return NcVariable<double>::flattenData(sci::physicalsToValues<sci::Physical<T, double>>(data));
		}
	};

	template<class T>
	class NcVariable<Physical<T, float>> : public NcVariable<float>
	{
	public:
		NcVariable(sci::string name, const OutputNcFile &ncFile, const NcDimension& dimension) : NcVariable<float>(name, ncFile, dimension) {}
		NcVariable(sci::string name, const OutputNcFile &ncFile, const std::vector<NcDimension *> &dimensions) : NcVariable<float>(name, ncFile, dimensions) {}
		NcVariable(NcVariable &&) = default;
		template<class U>
		static std::vector<float> flattenData(const std::vector<Physical<U, float>> &data) { return sci::physicalsToValues<sci::Physical<T, float>>(data); }
		template<class U>
		static::std::vector<float> flattenData(const std::vector<std::vector<U>> &data)
		{
			return NcVariable<float>::flattenData(sci::physicalsToValues<sci::Physical<T, float>>(data));
		}
	};

	class OutputNcFile : public NcFileBase
	{
	public:
		OutputNcFile(const sci::string &fileName, bool diskless=false);
		OutputNcFile(const sci::string& fileName, char unicodeReplacementCharacter, bool diskless = false);
		OutputNcFile();
		void openWritable(const sci::string& fileName, bool diskless = false);
		void openWritable(const sci::string& fileName, char unicodeReplacementCharacter, bool diskless = false);
		template<class T>
		void write(const T &item) const { item.write(*this); }
		template<class T, class U>
		void write(const NcVariable<T> &variable, const U &data);
	private:
		bool m_inDefineMode;
		int m_flags;
	protected:
		OutputNcFile(const sci::string& fileName, int flags, bool diskless);
		OutputNcFile(const sci::string& fileName, char unicodeReplacementCharacter, int flags, bool diskless);
		OutputNcFile(int flags);
	};

	class OutputNc4File : public OutputNcFile
	{
	public:
		OutputNc4File(const sci::string& fileName, bool diskless = false)
			:OutputNcFile(fileName, NC_NETCDF4, diskless)
		{
		}
		OutputNc4File(const sci::string& fileName, char unicodeReplacementCharacter, bool diskless = false)
			:OutputNcFile(fileName, unicodeReplacementCharacter, NC_NETCDF4, diskless)
		{
		}
		OutputNc4File()
		{}
	};

	class OutputNc3File : public OutputNcFile
	{
	public:
		OutputNc3File(const sci::string& fileName, bool sixtyFourBit = false, bool diskless = false)
			:OutputNcFile(fileName, sixtyFourBit ? NC_64BIT_DATA : 0, diskless)
		{}
		OutputNc3File(const sci::string & fileName, char unicodeReplacementCharacter, bool sixtyFourBit = false, bool diskless = false)
			:OutputNcFile(fileName, unicodeReplacementCharacter, sixtyFourBit ? NC_64BIT_DATA : 0, diskless)
		{}
		OutputNc3File(bool sixtyFourBit = false)
			:OutputNcFile(sixtyFourBit ? NC_64BIT_DATA : 0)
		{}
	};

	class NcDimension
	{
	public:
		NcDimension(const sci::string &name, size_t length);
		NcDimension(const sci::string &name, const InputNcFile &ncFile);
		NcDimension(const sci::string &name, size_t length, const OutputNcFile &ncFile);
		sci::string getName() { return m_name; }
		void setName(const sci::string &name);
		size_t getLength() { return m_length; }
		void setLength(size_t length);
		void load(const InputNcFile &ncFile);
		void write(const OutputNcFile &ncFile) const;
		int getId() const { sci::assertThrow(m_hasId, sci::err(SERR_NC, localNcError, "sci::NcDimension::getId called when the dimension does not yet have an id.")); return m_id; }
	private:
		sci::string m_name;
		size_t m_length;
		mutable int m_id;
		mutable bool m_hasId;

		//remove copy constructors
		NcDimension(const NcDimension&) = delete;
		NcDimension operator=(const NcDimension&) = delete;
	};

	template <class T>
	sci::NcAttribute::NcAttribute(const sci::string& name, T value)
	{
		m_name = name;
		m_nValues = 1;
		m_writeType = sci_internal::NcTraits<T>::ncType;
		m_nBytes = m_nValues * sizeof(T);
		T *v = (T*) malloc(m_nBytes);
		sci::assertThrow(v, sci::err(sci::SERR_NC, 0, sU("Error allocating memory for a netcdf attribute.")));
#pragma warning(push)
#pragma warning(disable : 6011)
		v[0] = value;
#pragma warning (pop)
		m_values = v;
	}


	template<class T, class WRITETYPE>
	sci::NcAttribute::NcAttribute(const sci::string& name, T value, const WRITETYPE &)
	{
		m_name = name;
		m_nValues = 1;
		m_writeType = sci_internal::NcTraits<WRITETYPE>::ncType;
		m_nBytes = m_nValues * sizeof(WRITETYPE);
		WRITETYPE *v = malloc(m_nBytes);
		sci::assertThrow(v, sci::err(sci::SERR_NC, 0, sU("Error allocating memory for a netcdf attribute.")));
#pragma warning(push)
#pragma warning(disable : 6011)
		v[0] = (WRITETYPE)value;
#pragma warning (pop)
		m_values = v;
	}

	template <class T>
	sci::NcAttribute::NcAttribute(const sci::string& name, std::vector<T> values)
	{
		m_name = name;
		m_nValues = values.size();
		m_writeType = sci_internal::NcTraits<T>::ncType;
		m_nBytes = m_nValues * sizeof(T);
		if (m_nValues == 0)
			m_values = nullptr;
		else
		{
			m_values = malloc(m_nBytes);
			sci::assertThrow(m_values, sci::err(sci::SERR_NC, 0, sU("Error allocating memory for a netcdf attribute.")));
#pragma warning(push)
#pragma warning(disable : 6387)
			memcpy(m_values, &values[0], m_nBytes);
#pragma warning (pop)
		}
	}


	template<class T, class WRITETYPE>
	sci::NcAttribute::NcAttribute(const sci::string& name, std::vector<T> values, const WRITETYPE &)
	{
		m_name = name;
		m_nValues = values.size();
		m_writeType = sci_internal::NcTraits<WRITETYPE>::ncType;
		m_nBytes = m_nValues * sizeof(WRITETYPE);
		if (m_nValues == 0)
			m_values = nullptr;
		else
		{
			WRITETYPE *v = malloc(m_nBytes);
			sci::assertThrow(v, sci::err(sci::SERR_NC, 0, sU("Error allocating memory for a netcdf attribute.")));
#pragma warning(push)
#pragma warning(disable : 6011)
			WRITETYPE *vEnd = v + m_nValues;
			T *v2i = &values[0];
			for (WRITETYPE *vi = v; vi != vEnd; ++vi, ++v2i)
				*vi = *v2i;
#pragma warning(pop)
			m_values = v;
		}
	}

	template<class T>
	void NcAttribute::write(const sci::OutputNcFile & ncFile, const sci::NcVariable<T> &variable) const
	{
		sci::assertThrow(ncFile.isOpen(), sci::err(SERR_NC, localNcError, "sci::NcAttribute::write called before the file was opened."));
		checkNcCall(nc_put_att(ncFile.getId(), variable.getId(), toUtf8(m_name).c_str(), m_writeType, m_nValues, m_values));
	}

	template<class T>
	std::vector<T> InputNcFile::getVariable(const sci::string &name, std::vector<size_t> &shape)
	{
		int varId;
		checkNcCall(nc_inq_varid(getId(), sci::toUtf8(name).c_str(), &varId));
		int nDims;
		checkNcCall(nc_inq_varndims(getId(), varId, &nDims));
		size_t nValues = 1;
		shape.resize(nDims);
		if (nDims > 0)
		{
			std::vector<int> dimIds(nDims);
			checkNcCall(nc_inq_vardimid(getId(), varId, &dimIds[0]));
			for (int i = 0; i < nDims; ++i)
			{
				checkNcCall(nc_inq_dimlen(getId(), dimIds[i], &shape[i]));
				nValues *= shape[i];
			}
		}

		return getVariableFromId<T>(varId, nValues);
	}

	template<class T>
	std::vector<T> InputNcFile::getVariable(const sci::string &name)
	{
		std::vector<size_t> shape;
		return getVariable<T>(name, shape);
	}

	template<class T>
	std::vector<T> InputNcFile::getVariable(const sci::string& name, const std::vector<size_t>& start, const std::vector<size_t> &shape)
	{
		int varId;
		checkNcCall(nc_inq_varid(getId(), sci::toUtf8(name).c_str(), &varId));
		sci::assertThrow(start.size() == shape.size(), sci::err(sci::SERR_NC, 0, sU("Attempted to read a part of a netcdf file with the start having a different number of elements to the lengths.")));
		std::vector<size_t> variableShape = getVariableShape(name);
		sci::assertThrow(start.size() == variableShape.size(), sci::err(sci::SERR_NC, 0, sU("Attempted to read a part of a netcdf file with the start having a different number of elements to the variable shape.")));
		for(size_t i=0; i<start.size(); ++i)
			sci::assertThrow(start[i]+shape[i]<=variableShape[i], sci::err(sci::SERR_NC, 0, sU("Attempted to read past the end of a netcdf variable")));

		return getVariableFromId<T>(varId, start, shape);
	}

	template<class T>
	NcVariable<T>::NcVariable(sci::string name, const OutputNcFile &ncFile, const NcDimension& dimension, int deflateLevel0to9, bool shuffle)
	{
		m_name = name;
		m_deflateLevel = deflateLevel0to9;
		m_shuffle = shuffle;
		m_hasId = false;
		m_dimensionIds.push_back(dimension.getId());
		sci::assertThrow(ncFile.isOpen(), sci::err(SERR_NC, localNcError, "sci::NcVariable construction failed because the ncFile passed was not open."));
		write(ncFile);
	}

	template<class T>
	NcVariable<T>::NcVariable(sci::string name, const OutputNcFile &ncFile, const std::vector<NcDimension*> &dimensions, int deflateLevel0to9, bool shuffle)
	{
		m_name = name;
		m_deflateLevel = deflateLevel0to9;
		m_shuffle = shuffle;
		m_hasId = false;
		for (size_t i = 0; i < dimensions.size(); ++i)
			m_dimensionIds.push_back(dimensions[i]->getId());
		sci::assertThrow(ncFile.isOpen(), sci::err(SERR_NC, localNcError, "sci::NcVariable construction failed because the ncFile passed was not open."));
		sci::assertThrow(m_dimensionIds.size() < std::numeric_limits<int>::max(), sci::err(SERR_NC, 0, "Attempted to create NEtCDF variable with more dimensions than supported."));
		write(ncFile);
	}

	/*
	template<class T>
	NcVariable<T>::NcVariable(sci::string name, const NcFile &ncFile, const std::vector<T> &data, const NcDimension& dimension)
	{

	}
	template<class T>
	NcVariable<T>::NcVariable(sci::string name, const NcFile &ncFile, const std::vector<std::vector<T>> &data, const std::vector<const NcDimension&> &dimensions)
	{

	}*/

	template<class T>
	void NcVariable<T>::write(const OutputNcFile &file) const
	{
		sci::assertThrow(!m_hasId, sci::err(SERR_NC, localNcError, "sci::NcVariable::write called multiple times on the same variable."));
		checkNcCall(nc_def_var(file.getId(), toUtf8(m_name).c_str(), sci_internal::NcTraits<T>::ncType, (int)m_dimensionIds.size(), &m_dimensionIds[0], &m_id));
		m_hasId = true;
		//checkNcCall(nc_def_var_chunking(file.getId(), m_id, NC_CHUNKED, { size_t(1024 * 1024) })); //1MB chunk size
		if(m_deflateLevel !=0)
			checkNcCall(nc_def_var_deflate(file.getId(), m_id, m_shuffle ? 1 : 0, 1, m_deflateLevel));
		for (size_t i = 0; i < m_attributes.size(); ++i)
			m_attributes[i].write(file, *this);
	}

	template<class T, class U>
	void OutputNcFile::write(const NcVariable<T> &variable, const U &data)
	{
		if (m_inDefineMode)
		{
			nc_enddef(getId());
			m_inDefineMode = false;
		};
		std::vector<size_t> shape = NcVariable<T>::getDataShape(data);
		sci::assertThrow(variable.getNDimensions() == shape.size(), sci::err(SERR_NC, localNcError, "sci::OutputNcFile::write called with a variable and data with differing numbers of dimensions."));
		std::vector<size_t> starts(variable.getNDimensions(), 0);
		size_t size = sci::product(shape);
		auto flattenedData = NcVariable<T>::flattenData(data);
		sci::assertThrow(size == flattenedData.size(), sci::err(SERR_NC, localNcError, "In sci::OutputNcFile::write NcVariable::flattenData returned data of an unexpected size."));
		//Check we are getting the right kind of data back
		static_assert(std::is_same<typename decltype(flattenedData)::value_type, typename NcVariable<T>::write_type>::value, "NcVariable::flattenData returned a vector of the incorrect type.");
		if (flattenedData.size() > 0)
			checkNcCall(nc_put_vara(getId(), variable.getId(), &starts[0], &shape[0], &flattenedData[0]));
	}
}

#endif
