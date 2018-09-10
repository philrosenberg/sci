#ifndef SVECTOR_NC_H
#define SVECTOR_NC_H
#include<string>
#include<vector>
#include<netcdf.h>
#include<algorithm>
#include<svector\serr.h>


namespace sci_internal
{
	template <class T>
	struct NcTraits
	{
	};
	template <>
	struct NcTraits<int8_t>
	{
		const static nc_type ncType = NC_BYTE;
	};
	template <>
	struct NcTraits<uint8_t>
	{
		const static nc_type ncType = NC_CHAR;
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
		NcAttribute(const std::string& name, T value);
		template<class T, class WRITETYPE>
		NcAttribute(const std::string& name, T value, const WRITETYPE &);
		template<class T>
		NcAttribute(const std::string& name, std::vector<T> values);
		template<class T, class WRITETYPE>
		NcAttribute(const std::string& name, std::vector<T> values, const WRITETYPE &);
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

		std::string m_name;
		size_t m_nValues;
		void* m_values;
		size_t m_nBytes;
		nc_type m_writeType;

		void setNull();
	};

	template<>
	sci::NcAttribute::NcAttribute(const std::string& name, std::string value);
	template <>
	sci::NcAttribute::NcAttribute(const std::string& name, std::wstring value);
	template<>
	sci::NcAttribute::NcAttribute(const std::string& name, const char *value);

	class AttributeContainer
	{
	public:
		void writeAttributes(OutputNcFile *outputNcFile);
		void addAttribute(const NcAttribute &attribute);
		virtual int getId() = 0;
	private:
		std::vector<NcAttribute> m_attributes;
	};

	class NcFileBase
	{
	public:
		NcFileBase() { m_open = false; }
		virtual ~NcFileBase() { close(); };
		void openReadOnly(const std::string &fileName);
		void openWritable(const std::string &fileName);
		void openReadOnly(const std::wstring &fileName);
		void openWritable(const std::wstring &fileName);
		void close();
		bool isOpen() const { return m_open; }
		int getId() const { return m_id; }
	private:
		bool m_open;
		int m_id;

		//remove copy constructors
		NcFileBase(const NcFileBase&);
		NcFileBase operator=(const NcFileBase&);
	};

	class InputNcFile : public NcFileBase
	{
	public:
		InputNcFile(const std::string &fileName);
		InputNcFile(const std::wstring &fileName);
		template<class T>
		std::vector<T> getVariable(const std::string &name);
		template<class T>
		std::vector<T> getVariable(const std::string &name, std::vector<size_t> &shape);
		std::vector<std::string>getVariableNames();
		template<class T>
		T getGlobalAttribute(const std::string &name);
		std::vector<std::string> getGlobalAttributeList();
	private:
		template<class T>
		std::vector<T> getVariableFromId(int id, size_t nValues);

		//remove copy constructors
		InputNcFile(const InputNcFile&);
		InputNcFile operator=(const InputNcFile&);
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
	double InputNcFile::getGlobalAttribute<double>(const std::string &name);
	template<>
	float InputNcFile::getGlobalAttribute<float>(const std::string &name);
	template<>
	short InputNcFile::getGlobalAttribute<short>(const std::string &name);
	template<>
	int InputNcFile::getGlobalAttribute<int>(const std::string &name);
	template<>
	long InputNcFile::getGlobalAttribute<long>(const std::string &name);
	template<>
	int8_t InputNcFile::getGlobalAttribute<int8_t>(const std::string &name);
	template<>
	uint8_t InputNcFile::getGlobalAttribute<uint8_t>(const std::string &name);
	template<>
	std::string InputNcFile::getGlobalAttribute<std::string>(const std::string &name);

	template<>
	std::vector<double> InputNcFile::getGlobalAttribute<std::vector<double>>(const std::string &name);
	template<>
	std::vector<float> InputNcFile::getGlobalAttribute<std::vector<float>>(const std::string &name);
	template<>
	std::vector<short> InputNcFile::getGlobalAttribute<std::vector<short>>(const std::string &name);
	template<>
	std::vector<int> InputNcFile::getGlobalAttribute<std::vector<int>>(const std::string &name);
	template<>
	std::vector<long> InputNcFile::getGlobalAttribute<std::vector<long>>(const std::string &name);
	template<>
	std::vector<int8_t> InputNcFile::getGlobalAttribute<std::vector<int8_t>>(const std::string &name);
	template<>
	std::vector<uint8_t> InputNcFile::getGlobalAttribute<std::vector<uint8_t>>(const std::string &name);
	template<>
	std::vector<std::string> InputNcFile::getGlobalAttribute<std::vector<std::string>>(const std::string &name);

	template<class T>
	class NcVariable
	{
	public:
		NcVariable(std::string name, const OutputNcFile &ncFile, const NcDimension& dimension);
		NcVariable(std::string name, const OutputNcFile &ncFile, const std::vector<NcDimension *> &dimensions);
		NcVariable(NcVariable &&) = default;
		//NcVariable(std::string name, const OutputNcFile &ncFile, const std::vector<T> &data, const NcDimension& dimension);
		//NcVariable(std::string name, const OutputNcFile &ncFile, const std::vector<std::vector<T>> &data, const std::vector<const NcDimension&> &dimensions);
		void addAttribute(const NcAttribute &attribute) { m_attributes.push_back(attribute); }
		void write(const OutputNcFile &file) const;
		int getId() const
		{
			sci::assertThrow(m_hasId, sci::err(SERR_NC, localNcError, "NcVariable::getId called before the variable has got an id from being written.")); return m_id;
		}
		size_t getNDimensions() const { return m_dimensionIds.size(); }
		bool hasId() const { return m_hasId; }
	private:
		mutable bool m_hasId;
		mutable int m_id;
		std::vector<NcAttribute> m_attributes;
		std::vector<int> m_dimensionIds;
		std::string m_name;

		//remove copy constructors
		NcVariable(const NcVariable&);
		NcVariable operator=(const NcVariable&);
	};

	class OutputNcFile : public NcFileBase
	{
	public:
		OutputNcFile(const std::string &fileName);
		OutputNcFile(const std::wstring &fileName);
		OutputNcFile();
		template<class T>
		void write(const T &item) const { item.write(*this); }
		template<class T>
		void write(const NcVariable<T> &variable, const std::vector<T> &data);
		template<class T, class U>
		void write(const NcVariable<T> &variable, const std::vector<std::vector<U>> &data);
	private:
		bool m_inDefineMode;
		//remove copy constructors
		OutputNcFile(const OutputNcFile&);
		OutputNcFile operator=(const OutputNcFile&);
	};

	class NcDimension
	{
	public:
		NcDimension(const std::string &name, size_t length);
		NcDimension(const std::string &name, const InputNcFile &ncFile);
		NcDimension(const std::string &name, size_t length, const OutputNcFile &ncFile);
		std::string getName() { return m_name; }
		void setName(const std::string &name);
		size_t getLength() { return m_length; }
		void setLength(size_t length);
		void load(const InputNcFile &ncFile);
		void write(const OutputNcFile &ncFile) const;
		int getId() const { sci::assertThrow(m_hasId, sci::err(SERR_NC, localNcError, "sci::NcDimension::getId called when the dimension does not yet have an id.")); return m_id; }
	private:
		std::string m_name;
		size_t m_length;
		mutable int m_id;
		mutable bool m_hasId;

		//remove copy constructors
		NcDimension(const NcDimension&);
		NcDimension operator=(const NcDimension&);
	};

	template <class T>
	sci::NcAttribute::NcAttribute(const std::string& name, T value)
	{
		m_name = name;
		m_nValues = 1;
		m_writeType = sci_internal::NcTraits<T>::ncType;
		m_nBytes = m_nValues * sizeof(T);
		T *v = (T*) malloc(m_nBytes);
		v[0] = value;
		m_values = v;
	}


	template<class T, class WRITETYPE>
	sci::NcAttribute::NcAttribute(const std::string& name, T value, const WRITETYPE &)
	{
		m_name = name;
		m_nValues = 1;
		m_writeType = sci_internal::NcTraits<WRITETYPE>::ncType;
		m_nBytes = m_nValues * sizeof(WRITETYPE);
		WRITETYPE *v = malloc(m_nBytes);
		v[0] = (WRITETYPE)value;
		m_values = v;
	}

	template <class T>
	sci::NcAttribute::NcAttribute(const std::string& name, std::vector<T> values)
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
			memcpy(m_values, &values[0], m_nBytes);
		}
	}


	template<class T, class WRITETYPE>
	sci::NcAttribute::NcAttribute(const std::string& name, std::vector<T> values, const WRITETYPE &)
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
			WRITETYPE *vEnd = v + m_nValues;
			T *v2i = &values[0];
			for (WRITETYPE *vi = v; vi != vEnd; ++vi, ++v2i)
				*vi = *v2i;
			m_values = v;
		}
	}

	template<class T>
	void NcAttribute::write(const sci::OutputNcFile & ncFile, const sci::NcVariable<T> &variable) const
	{
		sci::assertThrow(ncFile.isOpen(), sci::err(SERR_NC, localNcError, "sci::NcAttribute::write called before the file was opened."));
		checkNcCall(nc_put_att(ncFile.getId(), variable.getId(), m_name.c_str(), m_writeType, m_nValues, m_values));
	}

	template<class T>
	std::vector<T> InputNcFile::getVariable(const std::string &name, std::vector<size_t> &shape)
	{
		int varId;
		checkNcCall(nc_inq_varid(getId(), name.c_str(), &varId));
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
	std::vector<T> InputNcFile::getVariable(const std::string &name)
	{
		std::vector<size_t> shape;
		return getVariable<T>(name, shape);
	}

	template<class T>
	NcVariable<T>::NcVariable(std::string name, const OutputNcFile &ncFile, const NcDimension& dimension)
	{
		m_name = name;
		m_hasId = false;
		m_dimensionIds.push_back(dimension.getId());
		sci::assertThrow(ncFile.isOpen(), sci::err(SERR_NC, localNcError, "sci::NcVariable construction failed because the ncFile passed was not open."));
		//int dimensionId = dimension.getId();
		//sci::assertThrow(nc_def_var(ncFile.getId(), m_name.c_str(), sci_internal::NcTraits<T>::ncType, 1, &dimensionId, &m_id) == NC_NOERR, sci::err());
		//m_hasId = true;
	}

	template<class T>
	NcVariable<T>::NcVariable(std::string name, const OutputNcFile &ncFile, const std::vector<NcDimension*> &dimensions)
	{
		m_name = name;
		m_hasId = false;
		for (size_t i = 0; i < dimensions.size(); ++i)
			m_dimensionIds.push_back(dimensions[i]->getId());
		sci::assertThrow(ncFile.isOpen(), sci::err(SERR_NC, localNcError, "sci::NcVariable construction failed because the ncFile passed was not open."));
	}
	/*
	template<class T>
	NcVariable<T>::NcVariable(std::string name, const NcFile &ncFile, const std::vector<T> &data, const NcDimension& dimension)
	{

	}
	template<class T>
	NcVariable<T>::NcVariable(std::string name, const NcFile &ncFile, const std::vector<std::vector<T>> &data, const std::vector<const NcDimension&> &dimensions)
	{

	}*/

	template<class T>
	void NcVariable<T>::write(const OutputNcFile &file) const
	{
		sci::assertThrow(!m_hasId, sci::err(SERR_NC, localNcError, "sci::NcVariable::write called multiple times on the same variable."));
		checkNcCall(nc_def_var(file.getId(), m_name.c_str(), sci_internal::NcTraits<T>::ncType, m_dimensionIds.size(), &m_dimensionIds[0], &m_id));
		m_hasId = true;
		for (size_t i = 0; i < m_attributes.size(); ++i)
			m_attributes[i].write(file, *this);
	}

	template<class T>
	void OutputNcFile::write(const NcVariable<T> &variable, const std::vector<T> &data)
	{
		if (m_inDefineMode)
		{
			nc_enddef(getId());
			m_inDefineMode = false;
		}
		sci::assertThrow(variable.getNDimensions() == 1, sci::err(SERR_NC, localNcError, "sci::OutputNcFile::write called with a multi-dimensional variable and single-dimensional data."));
		size_t size = data.size();
		if (data.size() > 0)
		{
			size_t start = 0;
			checkNcCall(nc_put_vara(getId(), variable.getId(), &start, &size, &data[0]));
		}
			
	}

	template<class T, class U>
	void OutputNcFile::write(const NcVariable<T> &variable, const std::vector<std::vector<U>> &data)
	{
		if (m_inDefineMode)
		{
			nc_enddef(getId());
			m_inDefineMode = false;
		};
		std::vector<size_t> shape = sci::shape(data);
		sci::assertThrow(variable.getNDimensions() == shape.size(), sci::err(SERR_NC, localNcError, "sci::OutputNcFile::write called with a variable and data with differing numbers of dimensions."));
		std::vector<size_t> starts(variable.getNDimensions(), 0);
		size_t size = sci::product(shape);
		std::vector<T> flattenedData;
		sci::flatten(flattenedData, data);
		if (data.size() > 0)
			checkNcCall(nc_put_vara(getId(), variable.getId(), &starts[0], &shape[0], &flattenedData[0]));
	}
}

#endif