#pragma once
#include<string>
#include<vector>
#include<netcdf.h>
#include<algorithm>



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
	class OutputNcFile;
	class NcDimension;
	template<class T> class NcVariable;

	class NcAttribute
	{
	public:
		//copy, assignment, move
		NcAttribute(const NcAttribute &attribute);
		NcAttribute(NcAttribute &&attribute);
		NcAttribute &operator=(const NcAttribute &attribute);
		NcAttribute &operator=(NcAttribute &&attribute);

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
	};

	template<>
	sci::NcAttribute::NcAttribute(const std::string& name, std::string value);
	template<>
	sci::NcAttribute::NcAttribute(const std::string& name, const char *value);

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
		sci::assertThrow(ncFile.isOpen(), sci::err());
		sci::assertThrow(nc_put_att(ncFile.getId(), variable.getId(), m_name.c_str(), m_writeType, m_nValues, m_values) == NC_NOERR, sci::err());
	}


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
		void close();
		bool isOpen() const { return m_open; }
		int getId() const { return m_id; }
	private:
		bool m_open;
		int m_id;
	};

	class InputNcFile : public NcFileBase
	{
	public:
		InputNcFile(const std::string &fileName);
		template<class T>
		std::vector<T> getVariable(const std::string &name);
		template<class T>
		std::vector<T> getVariable(const std::string &name, std::vector<size_t> &shape);
		std::vector<std::string>getVariableNames();
	private:
		template<class T>
		std::vector<T> getVariableFromId(int id, size_t nValues);
	};

	template<class T>
	std::vector<T> InputNcFile::getVariable(const std::string &name, std::vector<size_t> &shape)
	{
		int varId;
		sci::assertThrow(nc_inq_varid(getId(), name.c_str(), &varId) == NC_NOERR, sci::err());
		int nDims;
		sci::assertThrow(nc_inq_varndims(getId(), varId, &nDims) == NC_NOERR, sci::err());
		size_t nValues = 1;
		shape.resize(nDims);
		if (nDims > 0)
		{
			std::vector<int> dimIds(nDims);
			sci::assertThrow(nc_inq_vardimid(getId(), varId, &dimIds[0]) == NC_NOERR, sci::err());
			for (int i = 0; i < nDims; ++i)
			{
				sci::assertThrow(nc_inq_dimlen(getId(), dimIds[i], &shape[i]) == NC_NOERR, sci::err());
				nValues *= shape[i];
			}
		}

		return getVariableFromId<T>(varId, nValues);
	}

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

	template<class T>
	std::vector<T> InputNcFile::getVariable(const std::string &name)
	{
		std::vector<size_t> shape;
		return getVariable<T>(name, shape);
	}


	template<class T>
	class NcVariable
	{
	public:
		NcVariable(std::string name, const OutputNcFile &ncFile, const NcDimension& dimension);
		NcVariable(std::string name, const OutputNcFile &ncFile, const std::vector<NcDimension> &dimensions);
		//NcVariable(std::string name, const OutputNcFile &ncFile, const std::vector<T> &data, const NcDimension& dimension);
		//NcVariable(std::string name, const OutputNcFile &ncFile, const std::vector<std::vector<T>> &data, const std::vector<const NcDimension&> &dimensions);
		void addAttribute(const NcAttribute &attribute) { m_attributes.push_back(attribute); }
		void write(const OutputNcFile &file) const;
		int getId() const
		{
			sci::assertThrow(m_hasId, sci::err()); return m_id;
		}
		size_t getNDimensions() const { return m_dimensionIds.size(); }
	private:
		mutable bool m_hasId;
		mutable int m_id;
		std::vector<NcAttribute> m_attributes;
		std::vector<int> m_dimensionIds;
		std::string m_name;
	};

	template<class T>
	NcVariable<T>::NcVariable(std::string name, const OutputNcFile &ncFile, const NcDimension& dimension)
	{
		m_name = name;
		m_hasId = false;
		m_dimensionIds.push_back(dimension.getId());
		sci::assertThrow(ncFile.isOpen(), sci::err());
		//int dimensionId = dimension.getId();
		//sci::assertThrow(nc_def_var(ncFile.getId(), m_name.c_str(), sci_internal::NcTraits<T>::ncType, 1, &dimensionId, &m_id) == NC_NOERR, sci::err());
		//m_hasId = true;
	}

	/*template<class T>
	NcVariable<T>::NcVariable(std::string name, const NcFile &ncFile, const std::vector<const NcDimension&> &dimensions)
	{
		m_name = name;
		m_hasId = false;
		for (size_t i = 0; i < dimensions.size(); ++i)
			m_dimensionIds.push_back(dimension.getId());
	}

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
		sci::assertThrow(!m_hasId, sci::err());
		sci::assertThrow(nc_def_var(getId(), m_name.c_str(), sci_internal::NcTraits<T>::ncType, m_dimensionIds.size(), &m_dimensionIds[0], &m_id) == NC_NOERR, sci::err());
		m_hasId = true;
		for (size_t i = 0; i < m_attributes.size(); ++i)
			m_attributes[i].write(file, *this);
	}

	class OutputNcFile : public NcFileBase
	{
	public:
		OutputNcFile(const std::string &fileName);
		template<class T>
		void write(const T &item) const { item.write(*this); }
		template<class T>
		void write(const NcVariable<T> &variable, const std::vector<T> &data) const;
		template<class T>
		void write(const NcVariable<T> &variable, const std::vector<std::vector<T>> &data) const;
	};

	template<class T>
	void OutputNcFile::write(const NcVariable<T> &variable, const std::vector<T> &data) const
	{
		size_t size = data.size();
		if( data.size() > 0)
			sci::assertThrow(nc_put_vara(getId(), variable.getId(), { size_t(0) }, &size, &data[0]) == NC_NOERR, sci::err());
	}

	template<class T>
	void OutputNcFile::write(const NcVariable<T> &variable, const std::vector<std::vector<T>> &data) const
	{
		std::vector<size_t> shape = sci::shape(data);
		sci::assertThrow(shape.size() == ncDimensions.size(), sci::err());
		std::vector<size_t> starts(dimensionIds.size(), 0);
		size_t size = sci::product(data);
		std::vector<decltype(sci::anyBaseVal(data))> flattenedData;
		sci::reshape(flattenedData, data, { size });
		if (data.size() > 0)
			sci::assertThrow(nc_put_vara(getId(), variable.getId(), &starts[0], &shape[0], &data[0]) == NC_NOERR, sci::err());
	}

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
		int getId() const { sci::assertThrow(m_hasId, sci::err()); return m_id; }
	private:
		std::string m_name;
		size_t m_length;
		mutable int m_id;
		mutable bool m_hasId;
	};
}