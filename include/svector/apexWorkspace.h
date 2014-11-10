#ifndef APEXWORKSPACE_H
#define APEXWORKSPACE_H

class ApexConnection
{
public:
	ApexConnection();
	ApexConnection(const std::string &address, const std::string &port);

	void store(const std::string &name,  const std::vector<double> &v);
	void retreive(const std::string &name,  std::vector<double> &v);

private:
	void connect(const std::string &address, const std::string &port);
	static size_t m_connectionCounter;
};


#endif