#ifndef APEX_APP_H
#define APEX_APP_H

#include<wx/wx.h>
#include<vector>
#include<string>
#include<map>

class ServerThread : public wxThread
{
public:
	ServerThread(){}
	ServerThread(const std::string &address, const std::string &port);
	void setConnectionData(const std::string &address, const std::string &port);
	virtual ExitCode Entry();
private:
	std::string m_address;
	std::string m_port;
};

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
	void addVD(const unsigned char * name, unsigned long size, const double * data);
	size_t checkLengthVD(const unsigned char * name);
	void retreiveVD(const unsigned char * name, unsigned long size, double * data);
private:
	ServerThread m_serverThread;
	std::map<std::string,std::vector<double>> m_vdMap;
	wxMutex m_mutex;
	std::string convertName(const unsigned char* name); 
};

#endif // APEX_APP_H
