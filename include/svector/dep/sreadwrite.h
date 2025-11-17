#ifndef sreadwrite_H
#define sreadwrite_H


#include<string>
#include<limits>
#include<fstream>
#include<sstream>
#include<vector>
#include"nc.h"
#include"../string.h"
#include"../codepage.h"

#ifdef max
#define SVECTOR_RESTORE_MAX max
#undef max
#endif
#ifdef min
#define SVECTOR_RESTORE_MIN min
#undef min
#endif

namespace sci
{
	enum csv_err
	{
		csv_ok=0,
		csv_filefail, //file could not be opened
		csv_columns_omitted, //file has some rows with more columns than the first - excess columns were not read
		csv_no_columns //there are no columns of data in the file
	};

	enum readtype
	{
		rt_ignore,
		rt_double,
		rt_string
	};

	//read csv data into a 2d vector where dimension 1 is the column number and dimension 2 is the row number
	//this is best if you wish to be able to extract individual columns from the data.
	//Any blanks will be filled with NANs, lines must finish with a newline except the final line. If a comma immediately
	//preceedes the newline it be treated like a blank - i.e a NAN will be appended to the row of data. The data output will
	//have a number of columns equal to the number of commas in the first data row plus one. If any subsequent data rows have fewer 
	//columns data output will be padded with NANs. If any subsequent data rows have more columns then the excess columns 
	//will not be saved and csv_columns_omitted will be returned.
	//The file read will start at startpos then it will skip startrow rows and read untill it has either read maxrows or 
	//reached the end of the file. If the function returns csv_ok and endpos is not null then endpos will be filled with
	//the position of the get pointer. This is useful for reading a file in chunks.
	//Inputs
	//filename:		name of file to read from
	//nheaderlines: number of header lines in the file before the data starts
	//Outputs
	//header:		Text of the header lines - the last newline character before the data will be omitted
	//data:			The data read from the file
	csv_err readcsvcolumns(std::ifstream &fin, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, size_t startpos = 0, std::streamoff *endpos = NULL, size_t startrow = 0, size_t maxrows = (0 - 1));
	csv_err readcsvcolumns(std::string filename, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, size_t startpos = 0, std::streamoff *endpos = NULL, size_t startrow = 0, size_t maxrows = (0 - 1));
	csv_err readcsvcolumns(std::wstring filename, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, size_t startpos=0, std::streamoff *endpos=NULL, size_t startrow=0, size_t maxrows=(0-1));

	
	//this is as above but you can specify columns to ignore or to read in as text
	csv_err readtextcolumns(std::ifstream &fin, std::string delimiters, bool mergeAdjacentDelimiters, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, std::vector< std::vector< std::string > > &text, const std::vector<readtype> &type);
	csv_err readtextcolumns(std::string filename, std::string delimiters, bool mergeAdjacentDelimiters, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, std::vector< std::vector< std::string > > &text, const std::vector<readtype> &type);
	csv_err readtextcolumns(std::wstring filename, std::string delimiters, bool mergeAdjacentDelimiters, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, std::vector< std::vector< std::string > > &text, const std::vector<readtype> &type);

	//read csv data into a 2d vector where dimension 1 is the row number and dimension 2 is the column number
	//this is best if you wish to be able to extract individual rows from the data.
	//Any blanks will be filled with NANs, lines must finish with a newline except the final line. If a comma immediately
	//preceedes the newline it be treated like a blank - i.e a NAN will be appended to the row of data. Each row of the data output will
	//have a number of columns equal to the number of commas in this row plus one. Each row may contain a different number of columns.
	//Inputs
	//filename:		name of file to read from
	//nheaderlines: number of header lines in the file before the data starts
	//Outputs
	//header:		Text of the header lines - the last newline character before the data will be omitted
	//data:			The data read from the file
	csv_err readcsvrows(std::ifstream &fin, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data);
	csv_err readcsvrows(std::string filename, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data);
	csv_err readcsvrows(std::wstring filename, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data);

	//takes a string of numbers separated by commas and puts them in a vactor of doubles. Returns the number of elements in the vector.
	//Any blanks will be filled with NANs.Any non-numberic values will be filled with NANs.
	//If the last character is a comma then this will be treated as a blanck element at the end. The vector will have
	//a number of elements equal to the number of commas in the string plus one.
	size_t splitcommastring(const std::string &datastring, std::vector <double> &data);

	//write csv data from 2D array data into file filename. The data must be able to be called with two [][] operators
	template<class T>
	csv_err writecsvcolumns(std::ofstream &fout, std::string header, const T &data)
	{

		if (!fout.is_open()) return sci::csv_filefail;
		if (header.length() > 0)fout << header.c_str() << "\n";
		if (data.size() == 0)
		{
			fout.close();
			return sci::csv_ok;
		}
		for (size_t i = 0; i < data[0].size(); ++i)
		{
			fout << data[0][i];
			for (size_t j = 1; j < data.size(); j++) fout << "," << data[j][i];
			fout << "\n";
		}
		
		fout.close();
		return sci::csv_ok;
	}
	template<class T>
	csv_err writecsvcolumns(sci::string filename, sci::string header, const T& data)
	{
		std::ofstream fout;
		fout.open(sci::nativeUnicode(filename).c_str(), std::ios::out);
		return sci::writecsvcolumns(fout, sci::nativeCodepage(header), data);
	}
	template<class T>
	csv_err writecsvcolumns(std::string filename, std::string header, const T& data)
	{
		std::ofstream fout;
		fout.open(filename.c_str(), std::ios::out);
		return sci::writecsvcolumns(fout, header, data);
	}
#ifdef _WIN32
	template<class T>
	csv_err writecsvcolumns(std::wstring filename, std::string header, const T &data)
	{
		std::ofstream fout;
		fout.open(filename.c_str(), std::ios::out);
		return sci::writecsvcolumns(fout, header, data);
	}
#endif
	//write csv data from 1D array into file filename.
	csv_err writecsvcolumn(std::string filename, std::string header, const std::vector<double>  &data);
	csv_err writecsvcolumn(std::wstring filename, std::string header, const std::vector<double>  &data);

	//write csv data from 2D array data into file filename. If the vector is not rectangular and pad is true then blanks are put in to fill the gaps
	csv_err writecsvrows(std::ofstream &fout, std::string header, const std::vector< std::vector <double> > &data, bool pad = true);
	csv_err writecsvrows(std::string filename, std::string header, const std::vector< std::vector <double> > &data, bool pad = true);
	csv_err writecsvrows(std::wstring filename, std::string header, const std::vector< std::vector <double> > &data, bool pad=true);
	//write csv data from 1D array into file filename.
	csv_err writecsvrow(std::string filename, std::string header, const std::vector<double>  &data);
	csv_err writecsvrow(std::wstring filename, std::string header, const std::vector<double>  &data);

	double atofcustom(const std::string &numstr);
	inline bool iswhitespace(char c) {return (c==' ' || c=='\t');}
	inline bool isnumber(char c) {return (c>'/' || c<':');}
	inline bool ispoint(char c) {return c=='.';}
	inline bool ise (char c) {return (c=='e' || c=='E');}
	inline bool issign (char c) {return (c=='+' || c=='-');}
	inline double getsign (char c) {return c=='+'? 1.0:-1.0;} //must only be called when issign returns true on the character
	double getnumber (char c);
	
	

#ifdef _WIN32
	/*
	//reads variable called varname from netcdf file filename and puts it in vector var.
	//var must be a vector and must have the correct number of dimensions to hold the variable.
	//If the file cannot be opened var will be emptied and the function will return false
	//If the variable cannot be read then var will be emptied and the function will return false
	//If everything works then the function will return true.
	template<class T>
	bool readncvariable(std::string filename, std::string varname, std::vector<T>& var)
	{
		std::vector<T> tempvar;
		return readncvariable(filename,varname,var,tempvar);
	}
	template<class T>
	bool readncvariable(std::wstring filename, std::string varname, std::vector<T>& var)
	{
		std::vector<T> tempvar;
		return readncvariable(filename, varname, var, tempvar);
	}
	//looks in the netcdf for variables with names that consist of varnamestart followed
	//by a non-negative integer number. These are all read into vars, with varsn being put into the nth element
	//of vars. vars must have one more demension than the variables. Any variables that are
	//not of the same type as vars will not be read.
	//If the numbers in the variable names are not 
	//If the file cannot be opened vars will be emptied and the function will return false
	//If the variable cannot be read then the corresponding element of vars will be emptied
	//If any variables are read the function will return true, otherwise it will return false
	template<class T>
	bool readpostnumberedncvariables(std::string filename, std::string varnamestart, std::vector< std::vector<T> >& vars)
	{
		return readnumberedncvariables(filename,varnamestart,"",vars);
	}
	template<class T>
	bool readpostnumberedncvariables(std::wstring filename, std::string varnamestart, std::vector< std::vector<T> >& vars)
	{
		return readnumberedncvariables(filename, varnamestart, "", vars);
	}

	//looks in the netcdf for variables with names that consist of varnamestart followed
	//by a non-negative integer number, followed by varnameend. These are all read into vars, 
	//with varnamestartnvarnamesend being put into the nth element of vars. vars must have one 
	//more demension than the variables. Any variables that are not of the same type as vars 
	//will not be read.
	//If the file cannot be opened vars will be emptied and the function will return false
	//If the variable cannot be read then the corresponding element of vars will be emptied
	//If any variables are read the function will return true, otherwise it will return false
	template<class T>
	bool readnumberedncvariables(sci::InputNcFile &file, std::string varnamestart, std::string varnameend, std::vector< std::vector<T> >& vars,size_t minnumber=0, size_t maxnumber=std::numeric_limits<size_t>::max())
	{
		try
		{
			std::vector<std::string> varlist = file.getVariableNames();

			std::vector<size_t> indices;
			for (unsigned long i = 0; i < varlist.size(); i++)
			{
				if (varlist[i].compare(0, varnamestart.length(), varnamestart) == 0) //check if the string starts with varnamestart
				{
					size_t index;
					std::istringstream stream(varlist[i].substr(varnamestart.length()), std::istringstream::binary);
					try
					{
						stream >> index;
						//Check details in case we're not using exceptions
						//note we also check if the whole string has been read as we can successfully read a number
						//and leave remaining text, which we don't want
						if (stream.fail() || stream.bad())
						{
							//not a match - remove from list
							varlist.erase(varlist.begin() + i);
							--i;
						}
						else if (index<minnumber || index > maxnumber)
						{
							//not a match - remove from list
							varlist.erase(varlist.begin() + i);
							--i;
						}

						else
						{
							//we know we have varnamestart, followed by a number. Now check the remainder
							//is equal to varnameend
							bool hasFailed = stream.fail();
							bool end = stream.eof();
							std::string remainder = stream.eof() ? "" : varlist[i].substr((unsigned long)stream.tellg() + (unsigned long)varnamestart.length());
							size_t get = (unsigned long)stream.tellg();
							size_t startlen = varnamestart.length();
							if (varnameend.compare(remainder) == 0)
							{
								//*****************************************
								//we have found a match, remember its index
								//*****************************************
								indices.push_back(index - minnumber);
							}
							else
							{
								//not a match - remove from list
								varlist.erase(varlist.begin() + i);
								--i;
							}
						}
					}
					catch (...)
					{
						//not a match - remove from list
						varlist.erase(varlist.begin() + i);
						--i;
					}
				}
				else
				{
					//not a match - remove from list
					varlist.erase(varlist.begin() + i);
					--i;
				}
			}

			//we now have a list of indices and a list of variable names to get
			//simply grab each one in turn

			//resize our output to hold the correct number of variables
			vars.resize(sci::max(indices) + size_t(1));
			for (size_t i = 0; i < varlist.size(); ++i)
			{
				vars[indices[i]] = file.getVariable<T>(varlist[i]); 
			}
		}
		catch (...)
		{
			return false;
		}
		return true;
	}
	template<class T>
	bool readnumberedncvariables(std::string filename, std::string varnamestart, std::string varnameend, std::vector< std::vector<T> >& vars, size_t minnumber = 0, size_t maxnumber = std::numeric_limits<size_t>::max())
	{
		try
		{
			sci::InputNcFile file(filename);
			return readnumberedncvariables(file, varnamestart, varnameend, vars, minnumber, maxnumber);
		}
		catch (...)
		{
			return false;
		}
	}
	template<class T>
	bool readnumberedncvariables(std::wstring filename, std::string varnamestart, std::string varnameend, std::vector< std::vector<T> >& vars, size_t minnumber = 0, size_t maxnumber = std::numeric_limits<size_t>::max())
	{
		try
		{
			sci::InputNcFile file(filename);
			return readnumberedncvariables(file, varnamestart, varnameend, vars, minnumber, maxnumber);
		}
		catch (...)
		{
			return false;
		}
	}

	//internal function
	//function that gets called by the above functions if tempvar has more than 1 dimension
	//the idea is to recursively call this function, stating with tempvar having the same
	//number of dimensions as var, until tempvar has only 1 dimension then we can use it to
	//read the data
	template<class T, class A>
	bool readncvariable(std::string filename, std::string varname, std::vector<T>& var, std::vector< std::vector<A> > &tempvar)
	{
		tempvar.resize(1);
		return readncvariable(filename,varname,var,tempvar[0]);
	}
	template<class T, class A>
	bool readncvariable(std::wstring filename, std::string varname, std::vector<T>& var, std::vector< std::vector<A> > &tempvar)
	{
		tempvar.resize(1);
		return readncvariable(filename, varname, var, tempvar[0]);
	}

	//function that gets called by the above functions if tempvar is 1d
	template<class T, class A>
	bool readncvariable(InputNcFile &file, std::string varname, std::vector<T>& var, std::vector<A> &tempvar)
	{
		//empty the var and shape vectors
		var.clear();
		tempvar.clear();
		try
		{
			if (sci::ndims(var) == 1)
			{
				//the elaborate and hacky pointer play below is so that 
				//even when var is multi dimensional and this code isn't called
				//it will still compile
				std::vector<A> *varPtr = (std::vector<A> *)((void*)&var); 
				*varPtr = file.getVariable<A>(sci::fromCodepage(varname));
			}
			else
			{
				std::vector<size_t> shape;
				tempvar = file.getVariable<A>(sci::fromCodepage(varname), shape);
				sci::assertThrow(shape.size() == sci::ndims(var), sci::err(SERR_NC, -9999, "readncvariable called with a vector with a number of dimensions which does not match the number of dimensions of the variable in the netcdf file."));
				sci::reshape(var, tempvar, shape);
			}
		}
		catch (...)
		{
			return false;
		}
		
		return true;
	}
	template<class T, class A>
	bool readncvariable(std::string filename, std::string varname, std::vector<T>& var, std::vector<A> &tempvar)
	{
		try
		{
			sci::InputNcFile file(filename);
			return readncvariable(file, varname, var, tempvar);
		}
		catch (...)
		{
			return false;
		}
	}
	template<class T, class A>
	bool readncvariable(std::wstring filename, std::string varname, std::vector<T>& var, std::vector<A> &tempvar)
	{
		try
		{
			sci::InputNcFile file(sci::fromNativeUnicode(filename));
			return readncvariable(file, varname, var, tempvar);
		}
		catch (...)
		{
			return false;
		}
	}


	inline bool readncvariableattribute(const std::string &filename, const std::string &variablename, const std::string &attributename, std::string &attributevalue)
	{
		return false;
	}
	inline bool readncvariableattribute(const std::wstring &filename, const std::string &variablename, const std::string &attributename, std::string &attributevalue)
	{
		return false;
	}
	
	bool readncglobalattribute(const std::string &filename, const std::string &attributename, std::string &attributevalue);
	bool readncglobalattribute(const std::string &filename, const std::string &attributename, std::vector<std::string> &attributevalues);

	template<class T>
	bool readncglobalattribute(const std::string &filename, const std::string &attributename, T &attributevalue)
	{
		std::vector<T> values;
		bool succeeded=readncglobalattribute(filename,attributename,values);
		if(!succeeded)
			return false;
		if(values.size()!=1)
			return false;
		attributevalue=values[0];
		return true;
	}

	template<class T>
	bool readncglobalattribute(const std::string &filename, const std::string &attributename, std::vector<T> &values)
	{
		NcError ncerr(NcError::verbose_nonfatal);
		NcFile file(filename.c_str(),NcFile::ReadOnly);
		if(!file.is_valid())return false;

		NcAtt *ncAtt=file.get_att(attributename.c_str());

		return sci::ncrawread(values,ncAtt);
	}

	bool readncvariableattribute(const std::string &filename, const std::string &variablename, const std::string &attributename, std::string &attributevalue);

	template<class T>
	bool readncvariableattribute(const std::string &filename, const std::string &variablename, const std::string &attributename, T &attributevalue)
	{

		std::vector<T> values;
		bool succeeded=readncvariableattribute(filename,variablename,attributename,values);
		if(!succeeded)
			return false;
		if(values.size()!=1)
			return false;
		attributevalue=values[0];
		return true;
	}

	template<class T>
	bool readncvariableattribute(const std::string &filename, const std::string &variablename, const std::string &attributename, std::vector<T> &values)
	{
		NcError ncerr(NcError::verbose_nonfatal);
		NcFile file(filename.c_str(),NcFile::ReadOnly);
		if(!file.is_valid())return false;
		NcVar *ncVar=file.get_var(variablename.c_str());
		if(ncVar==NULL)
			return false;
		NcAtt *ncAtt=ncVar->get_att(attributename.c_str());

		return sci::ncrawread(values,ncAtt);
	}

	//not as fast as the get method of NcVar as it requires an extra copy, but needed
	//for reading in an attribute vector. It is macro'd to give a version for each valid
	//type
	//Note NCTYPE should be the enumeration of the type e.g. ncDouble, not ncdouble
#define NCRAWREAD(CPPTYPE, NCTYPE) \
	inline bool ncrawread(std::vector<CPPTYPE> &vals, const NcTypedComponent *ncTypedComponent) \
	{ \
		if(ncTypedComponent->type()!=NCTYPE) \
			return false; \
		NcValues *ncValues=ncTypedComponent->values(); \
		vals.resize(ncValues->num()); \
		CPPTYPE *data=(CPPTYPE*)ncValues->base(); \
		memcpy(&vals[0],data,vals.size()*sizeof(CPPTYPE)); \
		delete ncValues; \
		return true;\
	}

	NCRAWREAD(unsigned char, ncByte)
	NCRAWREAD(char, ncChar)
	NCRAWREAD(short, ncShort)
	NCRAWREAD(int, ncInt)
	NCRAWREAD(long, ncLong)
	NCRAWREAD(float, ncFloat)
	NCRAWREAD(double, ncDouble)

	inline bool ncrawread(std::string &string, const NcTypedComponent *ncTypedComponent) 
	{ 
		if(ncTypedComponent->type()!=ncChar)
			return false;
		NcValues *ncValues=ncTypedComponent->values();
		char *data=(char*)ncValues->base();
		size_t len=ncValues->num();
		string.assign(data,len);
		delete ncValues;
		return true;
	}

	bool createncfile(const std::string &filename, const std::vector<std::string> &limiteddimensions, const std::vector<size_t> &limiteddimensionsizes, const std::string &unlimiteddimension="");
	bool createncfile(const std::string &filename, const std::string &limiteddimension, size_t limiteddimensionsize, const std::string &unlimiteddimension="");
	bool predefinencvariable(const std::string &filename, const std::string &variablename, const std::vector<std::string> &linkeddimensions);

	bool writencglobalattribute(const std::string &filename, const std::string &attributename, const std::string &attributevalue);

	template<class T>
	bool writencglobalattribute(const std::string &filename, const std::string &attributename, const T &attributevalue)
	{
		NcError ncerr(NcError::verbose_nonfatal);
		NcFile file(filename.c_str(),NcFile::Write);
		if(!file.is_valid())return false;

		file.add_att(attributename.c_str(),attributevalue);
		bool result=ncerr.get_err()==NC_NOERR;
		file.close();
		return result;
	}

	template<class T>
	bool writencglobalattribute(const std::string &filename, const std::string &attributename, const  std::vector<T> &attributevalue)
	{
		if(attributevalue.size()==0) return false;
		NcError ncerr(NcError::verbose_nonfatal);
		NcFile file(filename.c_str(),NcFile::Write);
		if(!file.is_valid())return false;

		file.add_att(attributename.c_str(),attributevalue.size(),&attributevalue[0]);
		bool result=ncerr.get_err()==NC_NOERR;
		file.close();
		return result;
	}

	bool writencvariableattribute(const std::string &filename, const std::string &variablename, const std::string &attributename, const std::string &attributevalue);

	template<class T>
	bool writencvariableattribute(const std::string &filename, const std::string &variablename, const std::string &attributename, const T &attributevalue)
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
		var->add_att(attributename.c_str(),attributevalue);
		bool result=ncerr.get_err()==NC_NOERR;
		file.close();
		return result;
	}

	template<class T>
	bool writencvariableattribute(const std::string &filename, const std::string &variablename, const std::string &attributename, const  std::vector<T> &attributevalue)
	{
		if(attributevalue.size()==0) return false;
		NcError ncerr(NcError::verbose_nonfatal);
		NcFile file(filename.c_str(),NcFile::Write);
		if(!file.is_valid())return false;
		
		NcVar *var=file.get_var(variablename.c_str());
		if(var==NULL) 
		{
			file.close();
			return false;
		}
		var->add_att(attributename.c_str(),attributevalue.size(),&attributevalue[0]);
		bool result=ncerr.get_err()==NC_NOERR;
		file.close();
		return result;
	}

	
	//big fat warning - shape should be an array of longs according to the netcdf headers, but in the
	//source it gets cast to a size_t*. In 32 bit builds this still worked okay, but for 64 bit
	//it breaks. So we pass in an array of size_ts, cast as a long *, then this gets cast back to a
	//size_t*. At some point the netcdf code might get fixed or change which might break this workaround
	template <class T>
	bool writencvariable(const std::string &filename, const std::string &variablename, const std::vector<std::string> &linkeddimensions, const std::vector<T> &data, const std::vector<size_t> datashape)
	{
		sci::assertThrow(datashape.size()>0, sci::err());
		//check the data type, currently only dealing with double and signed integers
		NcType type;
		basetype mytype=sci::getbasetype(data);
		if(mytype==SBASE_DOUBLE) type=ncDouble;
		else if(mytype==SBASE_FLOAT) type=ncFloat;
		else if(mytype==SBASE_SIGNEDSHORT) 
		{
			if(sizeof(short) == 2)
				type=ncShort;
			else if(sizeof(short) == 4)
				type=ncInt;
			else 
				return false;
		}
		else if(mytype==SBASE_SIGNEDINT) 
		{
			if(sizeof(int) == 2)
				type=ncShort;
			else if(sizeof(int) == 4)
				type=ncInt;
			else 
				return false;
		}
		else if(mytype==SBASE_SIGNEDLONG) 
		{
			if(sizeof(long) == 2)
				type=ncShort;
			else if(sizeof(long) == 4)
				type=ncInt;
			else 
				return false;
		}
		else if(mytype==SBASE_SIGNEDCHAR)
			type=ncChar;
		else return false;

		if(sci::product(datashape)!=data.size()) return false;

		NcError ncerr(NcError::verbose_nonfatal);
		NcFile file(filename.c_str(),NcFile::Write);
		if(!file.is_valid())return false;

		//get the dimensions
		std::vector<const NcDim*> dims(linkeddimensions.size()); //note that calls to add_var won't compile without the const
		for(size_t i=0; i<linkeddimensions.size(); ++i) 
		{
			dims[i]=file.get_dim(linkeddimensions[i].c_str());
			if(dims[i]==NULL)
			{
				file.close();
				return false;
			}
		}

		//check if the variable has been added
		NcVar *var=file.get_var(variablename.c_str());
		if(var==NULL)
		{
			if(dims.size()==0) var=file.add_var(variablename.c_str(),type);
			else var=file.add_var(variablename.c_str(),type,(int)linkeddimensions.size(),&dims[0]);
			if(var==NULL) return false;
		}

		std::vector<long> dataShapeLong(datashape.size());
		for( size_t i=0; i<datashape.size(); ++i)
		{
			sci::assertThrow( datashape[i] < LONG_MAX, sci::err() );
			dataShapeLong[i] = (long)datashape[i];
		}
				
		if(data.size()>0)
			var->put(&data[0],(long*)&datashape[0]);
		int err=ncerr.get_err();
		return err==NC_NOERR;
	}
	
	template <class T>
	bool writencvariable(const std::string &filename, const std::string &variablename, const std::string &linkeddimension, const std::vector<T> &data)
	{
		std::vector<std::string> linkeddims(1,linkeddimension);
		sci::assertThrow(data.size()<=(size_t)std::numeric_limits<long>::max(),sci::err());
		std::vector<size_t> datashape(1,data.size());
		return writencvariable(filename,variablename,linkeddims,data,datashape);
	}

	template <class T>
	bool writencvariable(const std::string &filename, const std::string &variablename, const std::vector<std::string> &linkeddimensions, const std::vector<std::vector<T>> &data)
	{
		std::vector<T> flatdata;
		sci::flatten(flatdata,data);
		if(flatdata.size()==0) return false;
		std::vector<size_t> datashape=sci::shape(data);
		return writencvariable(filename,variablename,linkeddimensions,flatdata,datashape);
	}
	*/
#endif
	
	void splitstring(const std::string &datastring, const std::string &separators, bool mergeadjacentseparators, std::vector<std::string> &splitstring);
	std::vector<std::string> splitstring(const std::string& datastring, const std::string& separators, bool mergeadjacentseparators);
	void splitstring(const sci::string& datastring, const sci::string& separators, bool mergeadjacentseparators, std::vector<sci::string>& splitstring);
	std::vector<sci::string> splitstring(const sci::string& datastring, const sci::string& separators, bool mergeadjacentseparators);
	bool getFileLocked (std::string fileName);

	std::vector< sci::string > getAllFiles( sci::string directory, bool recursive, bool includeHidden );
	sci::string getFileExtension( sci::string fileName );
	sci::string getFileName( sci::string fullPath );
	sci::string getContainingDirectoryPath( sci::string fullPath );
	sci::string concatPath( sci::string part1, sci::string part2);
	
}


#ifdef SVECTOR_RESTORE_MAX
#define max SVECTOR_RESTORE_MAX
#undef SVECTOR_RESTORE_MAX
#endif
#ifdef SVECTOR_RESTORE_MIN
#define min SVECTOR_RESTORE_MIN
#undef SVECTOR_RESTORE_MIN
#endif

#endif
