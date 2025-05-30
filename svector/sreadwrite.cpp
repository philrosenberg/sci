#include"../include/svector/sreadwrite.h"
#include"../include/svector/serr.h"
#include"../include/svector/svector.h"
#include"../include/svector/sstring.h"
#include<wx/filefn.h>
#include<wx/dir.h>

sci::csv_err sci::readcsvcolumns(std::string filename, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, size_t startpos, std::streamoff *endpos, size_t startrow, size_t maxrows)
{
	std::ifstream fin;
	fin.open(filename.c_str(), std::ios::in | std::ios_base::binary);
	return sci::readcsvcolumns(fin, nheaderlines, header, data, startpos, endpos, startrow, maxrows);
}

#ifdef _WIN32
sci::csv_err sci::readcsvcolumns(std::wstring filename, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, size_t startpos, std::streamoff *endpos, size_t startrow, size_t maxrows)
{
	std::ifstream fin;
	fin.open(filename.c_str(), std::ios::in | std::ios_base::binary);
	return sci::readcsvcolumns(fin, nheaderlines, header, data, startpos, endpos, startrow, maxrows);
}
#endif

sci::csv_err sci::readcsvcolumns(std::ifstream &fin, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, size_t startpos, std::streamoff *endpos, size_t startrow, size_t maxrows)
{
	//empty the header and the data vector
	data.resize(0);
	header="";
	if (!fin.is_open())
	{
		return sci::csv_filefail;
	}

	//move to the startpos
	fin.seekg(startpos);

	//read in the header and write it to the header variable removing the last newline
	std::string headerline;
	for(unsigned long i=0; i<nheaderlines; i++) 
	{
		std::getline(fin,headerline);
		header=header+headerline+std::string("\n");
	}
	header=header.substr(0,header.length()-1);

	//check to make sure we want any rows
	if(maxrows==0)
	{
		if(endpos)
			*endpos=fin.tellg();
		return sci::csv_ok;
	}

	//read in the number of lines to throw away
	for(size_t i=0; i<startrow; ++i)
		std::getline(fin,headerline);
		

	//variables for reading data
	std::vector< std::vector<double> > tempdata; //we use this because we can reserve space for it then copy to data without empty reserved space
	std::string dataline;
	std::vector<double> datarow;
	datarow.reserve(128);
	unsigned long capacity=128;
	//read the first line of data and check how many columns there are
	std::getline(fin,dataline);
	size_t ncols=sci::splitcommastring(dataline,datarow);
	if(ncols==0)return sci::csv_no_columns;
	tempdata.resize(ncols);
	for(size_t i=0; i<ncols; i++) tempdata[i].push_back(datarow[i]);

	//read in the next row and begin cycling through rows
	std::getline(fin,dataline);
	sci::splitcommastring(dataline,datarow);
	size_t nreadrows=1; //we have already read one
	while(!(fin.eof()||fin.fail()||fin.bad()) && nreadrows<maxrows)
	{
		//check if we need to reserve more space
		if(tempdata.begin()->size()==capacity)
		{
			capacity*=2;
			for(unsigned long i=0; i<ncols; i++) tempdata[i].reserve(capacity);
		}
		//check if the row is the correct length and either cut or pad appropriately
		if(datarow.size()!=ncols)
		{
			datarow.resize(ncols,std::numeric_limits<double>::quiet_NaN());
		}
		//add the data
		std::vector<double>::iterator datarowi=datarow.begin();
		for(std::vector< std::vector<double> >::iterator tempdatai=tempdata.begin(); tempdatai!=tempdata.end(); ++tempdatai) 
		{
			tempdatai->push_back(*datarowi);
			++datarowi;
		}
		//set the data size to zero
		datarow.resize(0);
		//read the next line
		std::getline(fin,dataline);
		sci::splitcommastring(dataline,datarow);
		++nreadrows;
	}
	//check to make sure we copied the last row
	if(datarow.size()>0 &&nreadrows<maxrows)
	{
		//check if we need to reserve more space
		if(tempdata.begin()->size()==capacity)
		{
			capacity*=2;
			for(unsigned long i=0; i<ncols; i++) tempdata[i].reserve(capacity);
		}
		//check if the row is the correct length and either cut or pad appropriately
		if(datarow.size()!=ncols)
		{
			datarow.resize(ncols,std::numeric_limits<double>::quiet_NaN());
		}
		//add the data
		std::vector<double>::iterator datarowi=datarow.begin();
		for(std::vector< std::vector<double> >::iterator tempdatai=tempdata.begin(); tempdatai!=tempdata.end(); ++tempdatai) 
		{
			tempdatai->push_back(*datarowi);
			++datarowi;
		}
	}

	if(endpos)
		*endpos=fin.tellg();

	//close the file
	fin.close();

	//copy the data
	data=tempdata;

	//return
	return sci::csv_ok;
}

sci::csv_err sci::readtextcolumns(std::string filename, std::string delimiters, bool mergeAdjacentDelimiters, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, std::vector< std::vector< std::string > > &text, const std::vector<sci::readtype> &type)
{
	std::ifstream fin;
	fin.open(filename.c_str(), std::ios::in);
	return sci::readtextcolumns(fin, delimiters, mergeAdjacentDelimiters, nheaderlines, header, data, text, type);
}
#ifdef _WIN32
sci::csv_err sci::readtextcolumns(std::wstring filename, std::string delimiters, bool mergeAdjacentDelimiters, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, std::vector< std::vector< std::string > > &text, const std::vector<sci::readtype> &type)
{
	std::ifstream fin;
	fin.open(filename.c_str(), std::ios::in);
	return sci::readtextcolumns(fin, delimiters, mergeAdjacentDelimiters, nheaderlines, header, data, text, type);
}
#endif
sci::csv_err sci::readtextcolumns(std::ifstream &fin, std::string delimiters, bool mergeAdjacentDelimiters, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, std::vector< std::vector< std::string > > &text, const std::vector<sci::readtype> &type)
{
	std::vector<size_t> datadest(type.size());
	std::vector<size_t> textdest(type.size());
	//work out how many string and data columns we have
	size_t ndatacols=0;
	size_t ntextcols=0;
	for(size_t i=0; i<type.size(); ++i)
	{
		if(type[i]==rt_double)
		{
			datadest[i]=ndatacols;
			++ndatacols;
		}
		if(type[i]==rt_string)
		{
			textdest[i]=ntextcols;
			++ntextcols;
		}
	}

	//empty the header and the data vector
	data.resize(ndatacols);
	for(size_t i=0; i<data.size(); ++i) data[i].resize(0);
	text.resize(ntextcols);
	for(size_t i=0; i<text.size(); ++i) text[i].resize(0);


	header="";
	//open file returning an error code if this fails
	if(!fin.is_open()) return sci::csv_filefail;

	//read in the header and write it to the header variable removing the last newline
	std::string headerline;
	for(unsigned long i=0; i<nheaderlines; i++) 
	{
		std::getline(fin,headerline);
		header=header+headerline+std::string("\n");
	}
	header=header.substr(0,header.length()-1);


	//variables for reading data
	std::string dataline;
	std::vector<std::string> splitstring;
	//read the first line of data and check how many columns there are
	std::getline(fin,dataline);
	sci::splitstring(dataline,delimiters,mergeAdjacentDelimiters,splitstring);
	size_t ncols=splitstring.size();
	if(ncols==0)return sci::csv_no_columns;
	ncols=std::min(ncols,type.size());
	std::istringstream stream;
	for(size_t i=0; i<ncols; ++i)
	{
		if(type[i]==rt_double)
		{
			stream.str(splitstring[i]);
			double temp;
			stream >> temp;
			data[datadest[i]].push_back(temp);
		}
		if(type[i]==rt_string)
		{
			text[textdest[i]].push_back(splitstring[i]);
		}
	}
	//resize the results in case ncols was less than type.size()
	while (data.size()>0 && data.back().size()==0) data.erase(data.end()-1);
	while (text.size()>0 && text.back().size()==0) text.erase(text.end()-1);

	//read in the next row and begin cycling through rows
	splitstring.resize(0);
	std::getline(fin,dataline);
	sci::splitstring(dataline,delimiters,mergeAdjacentDelimiters,splitstring);
	while(!(fin.eof()||fin.fail()||fin.bad()))
	{
		//check if the row is the correct length and either cut or pad appropriately
		splitstring.resize(ncols,"");
		//add the data
		for(size_t i=0; i<ncols; ++i)
		{
			if(type[i]==rt_double)
			{
				stream.str(splitstring[i]);
				double temp;
				stream >> temp;
				data[datadest[i]].push_back(temp);
			}
			if(type[i]==rt_string)
			{
				text[textdest[i]].push_back(splitstring[i]);
			}
		}
		//set the data size to zero
		splitstring.resize(0);
		//read the next line
		std::getline(fin,dataline);
		sci::splitstring(dataline,delimiters,mergeAdjacentDelimiters,splitstring);
	}
	//check to make sure we copied the last row
	if(splitstring.size()>0)
	{
		//check if the row is the correct length and either cut or pad appropriately
		splitstring.resize(ncols,"");
		//add the data
		for(size_t i=0; i<ncols; ++i)
		{
			if(type[i]==rt_double)
			{
				stream.str(splitstring[i]);
				double temp;
				stream >> temp;
				data[datadest[i]].push_back(temp);
			}
			if(type[i]==rt_string)
			{
				text[textdest[i]].push_back(splitstring[i]);
			}
		}
	}

	//close the file
	fin.close();

	//return
	return sci::csv_ok;
}

sci::csv_err sci::readcsvrows(std::string filename, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data)
{
	std::ifstream fin;
	fin.open(filename.c_str(), std::ios::in);
	return sci::readcsvrows(fin, nheaderlines, header, data);
}
#ifdef _WIN32
sci::csv_err sci::readcsvrows(std::wstring filename, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data)
{
	std::ifstream fin;
	fin.open(filename.c_str(), std::ios::in);
	return sci::readcsvrows(fin, nheaderlines, header, data);
}
#endif
sci::csv_err sci::readcsvrows(std::ifstream &fin, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data)
{
	//empty the header and the data vector
	data.resize(0);
	header="";
	//open file returning an error code if this fails
	if(!fin.is_open()) return sci::csv_filefail;

	//read in the header and write it to the header variable removing the last newline
	std::string headerline;
	for(unsigned long i=0; i<nheaderlines; i++) 
	{
		std::getline(fin,headerline);
		header=header+headerline+std::string("\n");
	}
	header=header.substr(0,header.length()-1);


	//variables for reading data
	std::vector< std::vector<double> > tempdata; //we use this because we can reserve space for it then copy to data without empty reserved space
	std::string dataline;
	std::vector<double> datarow;
	tempdata.reserve(128);
	unsigned long capacity=128;	

	//read in the first row and begin cycling through rows
	std::getline(fin,dataline);
	sci::splitcommastring(dataline,datarow);
	while(!(fin.eof()||fin.fail()||fin.bad()))
	{
		//check if we need to reserve more space
		if(tempdata.size()==capacity)
		{
			capacity*=2;
			tempdata.reserve(capacity);
		}
		//add the data
		tempdata.push_back(datarow);
		//set the data size to zero
		datarow.resize(0);
		//read the next line
		std::getline(fin,dataline);
		sci::splitcommastring(dataline,datarow);
	}
	//check to make sure we copies the last row
	if(datarow.size()>0)
	{
		//check if we need to reserve more space
		if(tempdata.size()==capacity)
		{
			capacity*=2;
			tempdata.reserve(capacity);
		}
		//add the data
		tempdata.push_back(datarow);
	}

	//close the file
	fin.close();

	//copy the data
	data=tempdata;

	//return
	return sci::csv_ok;
}

size_t sci::splitcommastring(const std::string &datastring, std::vector <double> &data)
{
	if(datastring.size()==0)
	{
		data.resize(0);
		return 0;
	}
	//create a stringstream to convert text to numbers
	std::stringstream ss;

	//count how many commas we have and set their locations
	size_t n=0;
	std::vector<size_t> commas(0);
	commas.reserve(datastring.length());
	/*size_t nextcomma=datastring.find(',',0);
	while(nextcomma!=std::string::npos)
	{
		commas.push_back(nextcomma);
		nextcomma=datastring.find(',',nextcomma+1);
		++n;
	}*/
	for(size_t i=0; i<datastring.size(); ++i) if(datastring[i]==',') 
	{
		++n;
		commas.push_back(i);
	}
	//size our vector appropriately
	data.resize(n+1);

	//if we have no commas then just attempt to read the value into data using the stringstream
	if(n==0)
	{
		ss<<datastring;
		ss>>data[0];
		if(ss.fail()) data[0]=std::numeric_limits<double>::quiet_NaN();
		return 1;
	}

	
	//feed each comma delimited segment into ss and read out the number
	ss<<datastring.substr(0,commas[0]);
	ss>>data[0];
	//if we fail or don't read all the way to the end then this means we have some non numeric characters
	//set the data to NAN
	if(ss.fail()) data[0]=std::numeric_limits<double>::quiet_NaN();
	//reset the stream to empty and clear any error flags including eof
	ss.str("");
	ss.clear();
	for(size_t i=1; i<n; i++)
	{
		ss<<datastring.substr(commas[i-1]+1,commas[i]-commas[i-1]-1);
		ss>>data[i];
		if(ss.fail()) data[i]=std::numeric_limits<double>::quiet_NaN();
		ss.str("");
		ss.clear();
	}
	ss<<datastring.substr(commas.back()+1,std::string::npos);
	ss>>data.back();
	if(ss.fail()) data.back()=std::numeric_limits<double>::quiet_NaN();
	//no need to reset things here as we're done. This could give a small speed boost for reading many short rows

	//return the number of columns
	return n+1;
}
		
sci::csv_err sci::writecsvcolumn(std::string filename, std::string header, const std::vector<double>  &data)
{
	std::vector<std::vector<double> > data2d;
	data2d.push_back(data);
	return writecsvcolumns(filename,header,data2d);
}
#ifdef _WIN32
sci::csv_err sci::writecsvcolumn(std::wstring filename, std::string header, const std::vector<double>  &data)
{
	std::vector<std::vector<double> > data2d;
	data2d.push_back(data);
	return writecsvcolumns(filename, header, data2d);
}
#endif
sci::csv_err sci::writecsvrows(std::string filename, std::string header, const std::vector< std::vector <double> > &data, bool pad)
{
	std::ofstream fout;
	fout.open(filename.c_str(), std::ios::out);
	return sci::writecsvrows(fout, header, data, pad);
}
#ifdef _WIN32
sci::csv_err sci::writecsvrows(std::wstring filename, std::string header, const std::vector< std::vector <double> > &data, bool pad)
{
	std::ofstream fout;
	fout.open(filename.c_str(), std::ios::out);
	return sci::writecsvrows(fout, header, data, pad);
}
#endif
sci::csv_err sci::writecsvrows(std::ofstream &fout, std::string header, const std::vector< std::vector <double> > &data, bool pad)
{
	if(!fout.is_open()) return sci::csv_filefail;
	if(header.length()>0)fout << header.c_str() << "\n";
	if(data.size()==0) 
	{
		fout.close();
		return sci::csv_ok;
	}
	if(sci::rectangular(data))
	{
		for(size_t i=0; i<data.size(); ++i)
		{
			fout <<data[i][0];
			for(size_t j=1; j<data[i].size(); j++) fout << "," << data[i][j];
			fout << "\n";
		}
	}
	else
	{
		size_t maxSize = 0;
		if( pad )
		{
			std::vector<size_t>sizes(data.size());
			for(size_t i=0; i<data.size(); ++i)
				sizes[i]=data[i].size();
			maxSize=sci::max<size_t>(sizes);
		}
		for(size_t i=0; i<data.size(); ++i)
		{
			fout <<data[i][0];
			for(size_t j=1; j<data[i].size(); j++)
				fout << "," << data[i][j];
			if(maxSize>data[i].size())
			{
				size_t extras=maxSize-data[i].size();
				for(size_t j=1; j<extras; j++)
					fout << ",";
			}
			fout << "\n";
		}
	}
	fout.close();
	return sci::csv_ok;
}
sci::csv_err sci::writecsvrow(std::string filename, std::string header, const std::vector<double>  &data)
{
	std::fstream fout;
	fout.open(filename.c_str(), std::ios::out);
	if(!fout.is_open()) return sci::csv_filefail;
	if(header.length()>0)fout << header.c_str() << "\n";
	if(data.size()==0) 
	{
		fout.close();
		return sci::csv_ok;
	}
	fout << data[0];
	for(size_t i=1; i<data.size(); i++) fout << "," << data[i];
	fout << "\n";
	fout.close();
	return sci::csv_ok;
}
#ifdef _WIN32
sci::csv_err sci::writecsvrow(std::wstring filename, std::string header, const std::vector<double>  &data)
{
	std::fstream fout;
	fout.open(filename.c_str(), std::ios::out);
	if (!fout.is_open()) return sci::csv_filefail;
	if (header.length()>0)fout << header.c_str() << "\n";
	if (data.size() == 0)
	{
		fout.close();
		return sci::csv_ok;
	}
	fout << data[0];
	for (size_t i = 1; i<data.size(); i++) fout << "," << data[i];
	fout << "\n";
	fout.close();
	return sci::csv_ok;
}
#endif

double sci::atofcustom(const std::string &numstr)
{
	double sign=1.0;
	double base=std::numeric_limits<double>::quiet_NaN();
	double exponentsign;
	double exponent=std::numeric_limits<double>::quiet_NaN();
	double currentfraction=0.1;
	bool pastinitialwhitespace=false;
	bool pastsign=false;
	bool pastwholenumbers=false;
	bool pastpoint=false;
	bool pastfraction=false;
	bool paste=false;
	bool pastexponentsign=false;
	bool pastallnumber=false;
	for(size_t i=0; i<numstr.length(); ++i)
	{
		char c=numstr[i];
		if(iswhitespace(c))
		{
			if(pastinitialwhitespace) 
			{
				pastsign=true;
				pastwholenumbers=true;
				pastpoint=true;
				pastfraction=true;
				paste=true;
				pastexponentsign=true;
				pastallnumber=true;
			}
		}
		else if(issign(c))
		{
			pastinitialwhitespace=true;
			if(!pastsign)
			{
				sign=getsign(c);
				pastsign=true;
			}
			else if(paste && !pastexponentsign)
			{
				exponentsign=getsign(c);
				pastwholenumbers=true;
				pastpoint=true;
				pastfraction=true;
				pastexponentsign=true;
			}
			else return std::numeric_limits<double>::quiet_NaN();
		}
		else if(ise(c))
		{
			if(paste) return std::numeric_limits<double>::quiet_NaN();
			if(!pastwholenumbers) return std::numeric_limits<double>::quiet_NaN();
			pastwholenumbers=true;
			pastpoint=true;
			pastfraction=true;
			paste=true;
		}
		else if(ispoint(c))
		{
			if(pastpoint) return std::numeric_limits<double>::quiet_NaN();
			pastwholenumbers=true;
			pastpoint=true;
			if(base!=base) base=0.0;
		}
		else if(isnumber(c))
		{
			if(pastallnumber) return std::numeric_limits<double>::quiet_NaN();
			if(paste)
			{
				pastexponentsign=true;
				if(exponent!=exponent)
				{
					exponent=getnumber(c);
				}
				else
				{
					exponent*=10;
					exponent+=getnumber(c);
				}
			}
			if(pastpoint)
			{
				if(pastfraction) return std::numeric_limits<double>::quiet_NaN();
				base+=getnumber(c)*currentfraction;
				currentfraction/=10.0;
			}
			else
			{
				pastinitialwhitespace=true;
				pastsign=true;
				if(base!=base) base=getnumber(c);
				else
				{
					base*=10.0;
					base+=getnumber(c);
				}
			}
		}
		else return std::numeric_limits<double>::quiet_NaN();
	}
	return std::pow(10.0,exponent*exponentsign)*base*sign;
}


double sci::getnumber (char c)
{
	if(c=='1') return 1.0;
	if(c=='2') return 2.0;
	if(c=='3') return 3.0;
	if(c=='4') return 4.0;
	if(c=='5') return 5.0;
	if(c=='6') return 6.0;
	if(c=='7') return 7.0;
	if(c=='8') return 8.0;
	if(c=='9') return 9.0;
	return 0.0;
}
/*
#ifdef _WIN32
bool sci::createncfile(const std::string &filename, const std::vector<std::string> &limiteddimensions, const std::vector<size_t> &limiteddimensionsizes, const std::string &unlimiteddimension)
{
	if(limiteddimensions.size()!=limiteddimensionsizes.size()) return false;
	NcError ncerr(NcError::verbose_nonfatal);
	NcFile file(filename.c_str(),NcFile::Replace);
	if(!file.is_valid())return false;

	bool result;
	if(unlimiteddimension.size()>0) 
	{
		file.add_dim(unlimiteddimension.c_str());
		result=ncerr.get_err()==NC_NOERR;
		if (result==false)
		{
			file.close();
			return false;
		}
	}
	for(size_t i=0; i<limiteddimensions.size(); ++i) 
	{
		file.add_dim(limiteddimensions[i].c_str(),(long) limiteddimensionsizes[i]);
		result=ncerr.get_err()==NC_NOERR;
		if (result==false)
		{
			file.close();
			return false;
		}
	}
	file.close();
	return true;
}

bool sci::createncfile(const std::string &filename, const std::string &limiteddimension, size_t limiteddimensionsize, const std::string &unlimiteddimension)
{
	std::vector<std::string> limdims(1,limiteddimension);
	std::vector<size_t> sizelimdims(1,limiteddimensionsize);
	return sci::createncfile(filename,limdims,sizelimdims,unlimiteddimension);
}

bool sci::readncglobalattribute(const std::string &filename, const std::string &attributename, std::string &attributevalue)
{
	NcError ncerr(NcError::verbose_nonfatal);
	NcFile file(filename.c_str(),NcFile::ReadOnly);
	if(!file.is_valid())return false;

	NcAtt *ncAtt=file.get_att(attributename.c_str());

	return sci::ncrawread(attributevalue,ncAtt);
}

bool sci::readncvariableattribute(const std::string &filename, const std::string &variablename, const std::string &attributename, std::string &attributevalue)
{
	NcError ncerr(NcError::verbose_nonfatal);
	NcFile file(filename.c_str(),NcFile::ReadOnly);
	if(!file.is_valid())return false;
	NcVar *ncVar=file.get_var(variablename.c_str());
	if(ncVar==NULL)
		return false;
	NcAtt *ncAtt=ncVar->get_att(attributename.c_str());

	return sci::ncrawread(attributevalue,ncAtt);
}

bool sci::writencglobalattribute(const std::string &filename, const std::string &attributename, const std::string &attributevalue)
{
	NcError ncerr(NcError::verbose_nonfatal);
	NcFile file(filename.c_str(),NcFile::Write);
	if(!file.is_valid())return false;

	file.add_att(attributename.c_str(),attributevalue.c_str());
	bool result=ncerr.get_err()==NC_NOERR;
	file.close();
	return result;
}

bool sci::writencvariableattribute(const std::string &filename, const std::string &variablename, const std::string &attributename, const std::string &attributevalue)
{
	NcError ncerr(NcError::verbose_nonfatal);
	NcFile file(filename.c_str(),NcFile::Write);
	if(!file.is_valid())return false;

	NcVar *var=file.get_var(variablename.c_str());
	if(var==NULL) 
	{
		file.close();
		return false;
	}
	var->add_att(attributename.c_str(),attributevalue.c_str());
	bool result=ncerr.get_err()==NC_NOERR;
	file.close();
	return result;
}
#endif
*/

void sci::splitstring(const std::string &datastring, const std::string &separators, bool mergeadjacentseparators, std::vector<std::string> &splitstring)
{
	splitstring.resize(0);
	if(datastring.size()==0)
	{
		return;
	}

	//count how many separators we have and set their locations
	std::vector<size_t> separatorlocations(1, -1);
	separatorlocations.reserve(datastring.length());
	/*size_t nextcomma=datastring.find(',',0);
	while(nextcomma!=std::string::npos)
	{
		commas.push_back(nextcomma);
		nextcomma=datastring.find(',',nextcomma+1);
		++n;
	}*/
	for(size_t i=0; i<datastring.length(); ++i) 
	{
		for(size_t j=0; j<separators.length(); ++j)
		{
			if(datastring[i]==separators[j])
			{
				separatorlocations.push_back(i);
				break;
			}
		}
	}
	//size our vector appropriately
	splitstring.reserve( separatorlocations.size() );

	//if we have no separators then just push back the string as is
	if( separatorlocations.size() == 0 )
	{
		splitstring.push_back( datastring );
		return;
	}
	for(size_t i=1; i<separatorlocations.size(); ++i)
	{
		if( separatorlocations[i-1] != separatorlocations[i]-1 || !mergeadjacentseparators )
			splitstring.push_back( datastring.substr(separatorlocations[i-1]+1,separatorlocations[i]-separatorlocations[i-1]-1) );
	}
	splitstring.push_back( datastring.substr(separatorlocations.back()+1) );
}

std::vector<std::string> sci::splitstring(const std::string& datastring, const std::string& separators, bool mergeadjacentseparators)
{
	std::vector< std::string > result;
	sci::splitstring(datastring, separators, mergeadjacentseparators, result);
	return result;
}

void sci::splitstring(const sci::string& datastring, const sci::string& separators, bool mergeadjacentseparators, std::vector<sci::string>& splitstring)
{
	splitstring.resize(0);
	if (datastring.size() == 0)
	{
		return;
	}

	//count how many separators we have and set their locations
	std::vector<size_t> separatorlocations(1, -1);
	separatorlocations.reserve(datastring.length());
	for (size_t i = 0; i < datastring.length(); ++i)
	{
		for (size_t j = 0; j < separators.length(); ++j)
		{
			if (datastring[i] == separators[j])
			{
				separatorlocations.push_back(i);
				break;
			}
		}
	}
	//size our vector appropriately
	splitstring.reserve(separatorlocations.size());

	//if we have no separators then just push back the string as is
	if (separatorlocations.size() == 0)
	{
		splitstring.push_back(datastring);
		return;
	}
	for (size_t i = 1; i < separatorlocations.size(); ++i)
	{
		if (separatorlocations[i - 1] != separatorlocations[i] - 1 || !mergeadjacentseparators)
			splitstring.push_back(datastring.substr(separatorlocations[i - 1] + 1, separatorlocations[i] - separatorlocations[i - 1] - 1));
	}
	splitstring.push_back(datastring.substr(separatorlocations.back() + 1));
}

std::vector<sci::string> sci::splitstring(const sci::string &datastring, const sci::string &separators, bool mergeadjacentseparators)
{
	std::vector< sci::string > result;
	sci::splitstring( datastring, separators, mergeadjacentseparators, result );
	return result;
}


bool sci::getFileLocked (std::string fileName)
{
	if(!wxFileExists(fileName))
		return false;
	std::fstream fout;
	fout.open(fileName.c_str(), std::ios::app|std::ios::binary);
	if(fout.is_open())
	{
		fout.close();
		return false;
	}
	else 
		return true;
}

std::vector< sci::string > sci::getAllFiles( sci::string directory, bool recursive, bool includeHidden )
{
	wxArrayString wxResult(false);
	int flags = wxDIR_FILES | ( recursive ? wxDIR_DIRS : 0 ) | ( includeHidden ? wxDIR_HIDDEN : 0 );
	if(directory.length() > 0)
		wxDir::GetAllFiles( sci::nativeUnicode(directory), &wxResult, "", flags);
	else
	{
		wxString cwd = wxGetCwd();
		if (!cwd.EndsWith('/') && !cwd.EndsWith('\\'))
			cwd += '/';
		wxDir::GetAllFiles(wxGetCwd(), &wxResult, "", flags);
		for (size_t i = 0; i < wxResult.size(); ++i)
			wxResult[i] = wxResult[i].substr(cwd.length());
	}
	std::vector<sci::string> result(wxResult.size());
	for(size_t i=0; i<wxResult.size(); ++i)
		result[i] = fromUtf8(wxResult[i].utf8_str().data());

	return result;
}

size_t getLastDirectorySeparator( sci::string path )
{
#ifdef _WIN32
	return path.find_last_of( sU("/\\") );
#else
	return path.find_last_of( sU("/") );
#endif
}

bool isDirectorySeparator( sci::char_t character )
{
#ifdef _WIN32
	return character == sU('/') || character == sU('\\');
#else
	return character == sU('/');
#endif
}

sci::string getDirectorySeparator()
{
#ifdef _WIN32
	return sU("\\");
#else
	return sU("/");
#endif
}

sci::string sci::getFileExtension( sci::string fileName )
{
	std::string result;
	size_t lastSlash = getLastDirectorySeparator( fileName );
	size_t lastDot = fileName.find_last_of( '.' );
	if( lastDot > lastSlash || lastSlash == std::string::npos )
		return fileName.substr( lastDot+1 );
	else
		return sU("");
}

//Returns the last element of fullPath assuming fullPath is a
//path separated by apropriate slashes (\ or / or Windows, / on
//linux). If the last character is a slash then an empty string
//will be returned.
sci::string sci::getFileName( sci::string fullPath )
{
	size_t lastSlash = getLastDirectorySeparator( fullPath );
	if( lastSlash == std::string::npos )
		return fullPath;
	return fullPath.substr( lastSlash +1 );
}

//Returns the full path of the directory part of fullPath. The 
//trailing slash is left on the path. If fullpath ends with a 
//slash then the string will be returned unchanged. This behaviour
//means that fullPath == getContainingDirectoryPath( fullPath ) +
//getFileName( fullPath ) will be true;
sci::string sci::getContainingDirectoryPath( sci::string fullPath )
{
	size_t lastSlash = getLastDirectorySeparator( fullPath );
	if( lastSlash == std::string::npos )
		return sU("");
	return fullPath.substr( 0, lastSlash +1 );
}

//Returns the concatenation of the two paths with a slash added
//if necessary
sci::string sci::concatPath( sci::string part1, sci::string part2)
{
	if( part1.length() == 0 )
		return part2;
	if( isDirectorySeparator( part1.back() ) )
		return part1 + part2;
	return part1 + getDirectorySeparator() + part2;
}
