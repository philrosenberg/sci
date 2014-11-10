#include "app.h"
#include "mainFrame.h"
#include "../idl/idl_h.h"

IMPLEMENT_APP( MyApp );

bool MyApp::OnInit()
{
	mainFrame* frame = new mainFrame( 0L, wxT("Apex Workspace") );
	frame->SetIcon( wxICON( amain ) );
	frame->Show();
	m_serverThread.setConnectionData("localhost", "4747");
	m_serverThread.Run();
	return true;
}

int MyApp::OnExit()
{
	RpcMgmtStopServerListening(NULL);
	return 0;
}

std::string MyApp::convertName(const unsigned char* name)
{
	size_t len=0;
	while(name[len]!='\0')
		++len;
	std::string result(len,'\0');
	for(size_t i=0; i<len; ++i)
		result[i]=name[i];
	return result;
}


void MyApp::addVD(const unsigned char * name, unsigned long size, const double * data)
{
	try
	{
		wxMutexLocker locker(m_mutex);
		std::vector<double> &localData=m_vdMap[convertName(name)];
		localData.resize(size);
		double *local=&localData[0];
		for(size_t i=0; i<size; ++i)
			*(local+i)=*(data+i);
	}
	catch(...)
	{
	}
}

size_t MyApp::checkLengthVD(const unsigned char * name)
{
	try
	{
		wxMutexLocker locker(m_mutex);
		return m_vdMap[convertName(name)].size();
	}
	catch(...)
	{
		return 0;
	}
}

void MyApp::retreiveVD(const unsigned char * name, unsigned long size, double * data)
{
	try
	{
		wxMutexLocker locker(m_mutex);
		std::vector<double> &localData=m_vdMap[convertName(name)];
		if(size!=localData.size())
		{
			//do something
		}
		else
		{
			double *local=&localData[0];
			for(size_t i=0; i<size; ++i)
				*(data+i)=*(local+i);
		}
	}
	catch(...)
	{
	}
}

ServerThread::ServerThread(const std::string &address, const std::string &port)
{
	setConnectionData(address, port);
}

void ServerThread::setConnectionData(const std::string &address, const std::string &port)
{
	m_address=address;
	m_port=port;
}


// Naive security callback.
RPC_STATUS CALLBACK SecurityCallback(RPC_IF_HANDLE /*hInterface*/, void* /*pBindingHandle*/)
{
    return RPC_S_OK; // Always allow anyone.
}


wxThread::ExitCode ServerThread::Entry()
{
   RPC_STATUS status;

   // Uses the protocol combined with the endpoint for receiving
   // remote procedure calls.
   status = RpcServerUseProtseqEp(
      reinterpret_cast<RPC_WSTR>(L"ncacn_ip_tcp"), // Use TCP/IP protocol.
      RPC_C_PROTSEQ_MAX_REQS_DEFAULT, // Backlog queue length for TCP/IP.
      reinterpret_cast<RPC_WSTR>(L"4747"), // TCP/IP port to use.
      NULL); // No security.

   if (status)
   {
	   if(status==RPC_S_PROTSEQ_NOT_SUPPORTED)
	   {
		   sci::assertThrow(false,sci::err());
	   }
	   if(status==RPC_S_INVALID_RPC_PROTSEQ)
	   {
		   sci::assertThrow(false,sci::err());
	   }
	   if(status==RPC_S_INVALID_ENDPOINT_FORMAT)
	   {
		   sci::assertThrow(false,sci::err());
	   }
	   if(status==RPC_S_OUT_OF_MEMORY)
	   {
		   sci::assertThrow(false,sci::err());
	   }
	   if(status==RPC_S_DUPLICATE_ENDPOINT)
	   {
		   sci::assertThrow(false,sci::err());
	   }
	   if(status==RPC_S_INVALID_SECURITY_DESC)
	   {
		   sci::assertThrow(false,sci::err());
	   }
      exit(status);
   }

   // Registers the Example1 interface.
   status = RpcServerRegisterIf2(
      apex_v1_0_s_ifspec, // Interface to register.
      NULL, // Use the MIDL generated entry-point vector.
      NULL, // Use the MIDL generated entry-point vector.
      RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, // Forces use of security callback.
      RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Use default number of concurrent calls.
      (unsigned)-1, // Infinite max size of incoming data blocks.
      SecurityCallback); // Naive security callback.

   if (status)
      exit(status);

   // Start to listen for remote procedure
   // calls for all registered interfaces.
   // This call will not return until
   // RpcMgmtStopServerListening is called.
   status = RpcServerListen(
     1, // Recommended minimum number of threads.
     RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Recommended maximum number of threads.
     FALSE); // Start listening now.

   if (status)
      exit(status);

   return 0;
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

extern "C" void storeDoubleArray( 
    /* [string][in] */ const unsigned char *name,
    /* [in] */ unsigned long arraySize,
    /* [size_is][in] */ const double array[  ])
{
	wxGetApp().addVD(name, arraySize, array);
}

extern "C" void checkLengthDoubleArray( 
    /* [string][in] */ const unsigned char *name,
    /* [out] */ unsigned long *arraySize)
{
	*arraySize=wxGetApp().checkLengthVD(name);
}

extern "C" void retreiveDoubleArray( 
    /* [string][in] */ const unsigned char *name,
    /* [in] */ unsigned long arraySize,
    /* [size_is][out] */ double array[  ])
{
	wxGetApp().retreiveVD(name,arraySize,array);
}