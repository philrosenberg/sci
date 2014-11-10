#include"../idl/idl_h.h"
#include<string>
#include<vector>
#include<svector/serr.h>
#include<svector/apexWorkspace.h>


class RpcStringBinding
{
public:
	//constructs the string based upon the address and port
	RpcStringBinding(RPC_WSTR address, RPC_WSTR port)
	{
		RPC_STATUS status;
		status = RpcStringBindingCompose(
		NULL, // UUID to bind to.
		reinterpret_cast<RPC_WSTR>(L"ncacn_ip_tcp"), // Use TCP/IP protocol.
		address, // TCP/IP network address to use.
		port, // TCP/IP port to use.
		NULL, // Protocol dependent network options to use.
		&m_string); // String binding output.

		sci::assertThrow(status==0,sci::err());
	}
	//frees the string
	~RpcStringBinding()
	{
		if(m_string)
			RpcStringFree(&m_string);
		m_string=NULL;
	}
	//returns the raw pointer of the string
	RPC_WSTR getString()
	{
		return m_string;
	}
private:
	RPC_WSTR m_string;

};

class RpcBindingHolder
{
public:
	RpcBindingHolder(handle_t binding, RpcStringBinding & rpcStringBinding) : m_binding(binding)
	{
		RPC_STATUS status;
		// Validates the format of the string binding handle and converts
		// it to a binding handle.
		// Connection is not done here either.
		status = RpcBindingFromStringBinding(
			rpcStringBinding.getString(), // The string binding to validate.
			&m_binding); // Put the result in the implicit binding
                         // handle defined in the IDL file.

		sci::assertThrow(status==0,sci::err());
	}
private:
	handle_t m_binding;
};


size_t ApexConnection::m_connectionCounter(0);

ApexConnection::ApexConnection()
{
	connect("localhost", "4747");
}
ApexConnection::ApexConnection(const std::string &address, const std::string &port)
{
	connect(address, port);
}

void ApexConnection::connect(const std::string &address, const std::string &port)
{
	sci::assertThrow(m_connectionCounter==0, sci::err());
	++m_connectionCounter;
	RPC_STATUS status;
	RPC_WSTR szStringBinding = NULL;

	RpcStringBinding stringBinding(reinterpret_cast<RPC_WSTR>(L"localhost"), reinterpret_cast<RPC_WSTR>(L"4747"));


	// Validates the format of the string binding handle and converts
	// it to a binding handle.
	// Connection is not done here either.
	status = RpcBindingFromStringBinding(
		stringBinding.getString(), // The string binding to validate.
		&hApexBinding); // Put the result in the implicit binding
                          // handle defined in the IDL file.

	sci::assertThrow(status==0,sci::err());
}

void ApexConnection::store(const std::string &name,  const std::vector<double> &v)
{
	unsigned char * uName=new unsigned char[name.size()+1];
	for(size_t i=0; i<name.size(); ++i)
		uName[i]=name[i];
	uName[name.size()]='\0';
	RpcTryExcept
	{
		storeDoubleArray(uName,v.size(),&v[0]);
	}
	RpcExcept(1)
	{
		//sci::assertThrow(false,sci::err());
		//RpcExceptionCode() - this is the function to get the error code, google it to get the list of possible codes
	}
	RpcEndExcept
	delete uName;
}

void ApexConnection::retreive(const std::string &name,  std::vector<double> &v)
{
	unsigned char * uName=new unsigned char[name.size()+1];
	for(size_t i=0; i<name.size(); ++i)
		uName[i]=name[i];
	uName[name.size()]='\0';
	RpcTryExcept
	{
		unsigned long size;
		checkLengthDoubleArray(uName, &size);
		v.resize(size);
		double *pointer=&v[0];
		retreiveDoubleArray(uName,v.size(),pointer);
	}
	RpcExcept(1)
	{
		//sci::assertThrow(false,sci::err());
		//RpcExceptionCode() - this is the function to get the error code, google it to get the list of possible codes
	}
	RpcEndExcept
	delete uName;
}

// Memory allocation function for RPC.
// The runtime uses these two functions for allocating/deallocating
// enough memory to pass the string to the server.
void* __RPC_USER midl_user_allocate(size_t size)
{
    return malloc(size);
}

// Memory deallocation function for RPC.
void __RPC_USER midl_user_free(void* p)
{
    free(p);
}