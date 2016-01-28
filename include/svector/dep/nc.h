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
	private:

		std::string m_name;
		size_t m_nValues;
		void* m_values;
		size_t m_nBytes;
		nc_type m_writeType;
	};

	template <class T>
	sci::NcAttribute::NcAttribute(const std::string& name, T value)
	{
		m_name = name;
		m_nValues = 1;
		m_writeType = sci_internal::NcTraits<T>::ncType;
		m_nBytes = m_nValues * sizeof(T);
		T *v = malloc(m_nBytes);
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

	class OutputNcFile;

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

	class OutputNcFile : public NcFileBase
	{
	public:
		OutputNcFile(const std::string &fileName);
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
	private:
		template<class T>
		void setValues(T *values, size_t nValues);
		std::string m_name;
		size_t m_length;
		mutable int m_id;
		mutable bool m_hasId;
	};
}