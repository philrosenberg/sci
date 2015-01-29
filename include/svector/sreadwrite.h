#ifndef sreadwrite_H
#define sreadwrite_H

#include"svector.h"
#include"sstring.h"
#include<string>
#include<nc/netcdfcpp.h>
#include"serr.h"
#include<limits>
#include<fstream>
#include<sstream>
#include"ppfile.h"

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
	csv_err readcsvcolumns(std::string filename, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, size_t startpos=0, std::streamoff *endpos=NULL, size_t startrow=0, size_t maxrows=(0-1));

	
	//this is as above but you can specify columns to ignore or to read in as text
	csv_err readtextcolumns(std::string filename, std::string delimiters, bool mergeAdjacentDelimiters, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data, std::vector< std::vector< std::string > > &text, const std::vector<readtype> &type);

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
	csv_err readcsvrows(std::string filename, unsigned long nheaderlines, std::string &header, std::vector< std::vector <double> > &data);

	//takes a string of numbers separated by commas and puts them in a vactor of doubles. Returns the number of elements in the vector.
	//Any blanks will be filled with NANs.Any non-numberic values will be filled with NANs.
	//If the last character is a comma then this will be treated as a blanck element at the end. The vector will have
	//a number of elements equal to the number of commas in the string plus one.
	size_t splitcommastring(const std::string &datastring, std::vector <double> &data);

	//write csv data from 2D array data into file filename. If the vector is not rectangular then blanks are put in to fill the gaps
	csv_err writecsvcolumns(std::string filename, std::string header, const std::vector< std::vector <double> > &data);
	//write csv data from 1D array into file filename.
	csv_err writecsvcolumn(std::string filename, std::string header, const std::vector<double>  &data);

	//write csv data from 2D array data into file filename. If the vector is not rectangular and pad is true then blanks are put in to fill the gaps
	csv_err writecsvrows(std::string filename, std::string header, const std::vector< std::vector <double> > &data, bool pad=true);
	//write csv data from 1D array into file filename.
	csv_err writecsvrow(std::string filename, std::string header, const std::vector<double>  &data);

	double atofcustom(const std::string &numstr);
	inline bool iswhitespace(char c) {return (c==' ' || c=='\t');}
	inline bool isnumber(char c) {return (c>'/' || c<':');}
	inline bool ispoint(char c) {return c=='.';}
	inline bool ise (char c) {return (c=='e' || c=='E');}
	inline bool issign (char c) {return (c=='+' || c=='-');}
	inline double getsign (char c) {return c=='+'? 1.0:-1.0;} //must only be called when issign returns true on the character
	double getnumber (char c);
	
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
		/*
		//try to open the file
		NcFile *nc=new NcFile(filename.c_str(),NcFile::ReadOnly);
		if(!nc->is_valid())
		{
			delete nc;
			vars.resize(0);
			return false;
		}

		std::vector<std::string> varlist;
		std::vector<size_t> indices;
		unsigned long nvars=nc->num_vars();
		for(unsigned long i=0; i<nvars; i++)varlist.push_back(nc->get_var(i)->name());
		for(unsigned long i=0; i<varlist.size(); i++)
		{
			if(varlist[i].compare(0,varnamestart.length(),varnamestart)==0) //check if the string starts with varnamestart
			{
				size_t index;
				std::istringstream stream(varlist[i].substr(varnamestart.length()));
				try
				{
					stream >> index;
					//Check details in case we're not using exceptions
					//note we also check if the whole string has been read as we can successfully read a number
					//and leave remaining text, which we don't want
					if (stream.fail()||stream.bad()||((unsigned long)stream.tellg())!=(unsigned long)(varlist[i].length()-varnamestart.length()))
					{
						varlist.erase(varlist.begin()+i);
						--i;
					}
					else
					{
						//check the remainder of the string is empty
						indices.push_back(index);
					}
				}
				catch(...)
				{
					//if we failed then remove this item from the varlist
					varlist.erase(varlist.begin()+i);
					--i;
				}
			}
			else
			{
				//remove this item from the varlist
				varlist.erase(varlist.begin()+i);
				--i;
			}
		}
		delete nc;

		//we now have a list of indices and a list of variable names to get
		//simply grab each one in turn

		//resize our output to hold the correct number of variables
		vars.resize(sci::max<size_t>(indices+size_t(1)));
		//grab the variables
		bool succeeded=false;
		for(size_t i=0; i<varlist.size(); ++i) 
		{
			bool thissucceeded=sci::readncvariable(filename,varlist[i],vars[indices[i]]);
			succeeded=succeeded||thissucceeded;
		}
		return succeeded;*/
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
	bool readnumberedncvariables(std::string filename, std::string varnamestart, std::string varnameend, std::vector< std::vector<T> >& vars,size_t minnumber=0, size_t maxnumber=std::numeric_limits<size_t>::max())
	{
		//try to open the file
		NcFile *nc=new NcFile(filename.c_str(),NcFile::ReadOnly);
		if(!nc->is_valid())
		{
			delete nc;
			vars.resize(0);
			return false;
		}

		std::vector<std::string> varlist;
		std::vector<size_t> indices;
		unsigned long nvars=nc->num_vars();
		for(unsigned long i=0; i<nvars; i++)varlist.push_back(nc->get_var(i)->name());
		for(unsigned long i=0; i<varlist.size(); i++)
		{
			if(varlist[i].compare(0,varnamestart.length(),varnamestart)==0) //check if the string starts with varnamestart
			{
				size_t index;
				std::istringstream stream(varlist[i].substr(varnamestart.length()),std::istringstream::binary);
				try
				{
					stream >> index;
					//Check details in case we're not using exceptions
					//note we also check if the whole string has been read as we can successfully read a number
					//and leave remaining text, which we don't want
					if (stream.fail()||stream.bad())
					{
						//not a match - remove from list
						varlist.erase(varlist.begin()+i);
						--i;
					}
					else if(index<minnumber || index > maxnumber)
					{
						//not a match - remove from list
						varlist.erase(varlist.begin()+i);
						--i;
					}
					
					else
					{
						//we know we have varnamestart, followed by a number. Now check the remainder
						//is equal to varnameend
						bool hasFailed=stream.fail();
						bool end=stream.eof();
						std::string remainder=stream.eof() ? "" : varlist[i].substr((unsigned long)stream.tellg()+(unsigned long)varnamestart.length());
						size_t get=(unsigned long)stream.tellg();
						size_t startlen=varnamestart.length();
						if(varnameend.compare(remainder)==0)
						{
							//*****************************************
							//we have found a match, remember its index
							//*****************************************
							indices.push_back(index-minnumber);
						}
						else
						{
							//not a match - remove from list
							varlist.erase(varlist.begin()+i);
							--i;
						}
					}
				}
				catch(...)
				{
					//not a match - remove from list
					varlist.erase(varlist.begin()+i);
					--i;
				}
			}
			else
			{
				//not a match - remove from list
				varlist.erase(varlist.begin()+i);
				--i;
			}
		}
		delete nc;

		//we now have a list of indices and a list of variable names to get
		//simply grab each one in turn

		//resize our output to hold the correct number of variables
		vars.resize(sci::max<size_t>(indices+size_t(1)));
		//grab the variables
		bool succeeded=false;
		for(size_t i=0; i<varlist.size(); ++i) 
		{
			bool thissucceeded=sci::readncvariable(filename,varlist[i],vars[indices[i]]);
			succeeded=succeeded||thissucceeded;
		}
		return succeeded;
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

	//function that gets called by the above functions if tempvar is 1d
	template<class T, class A>
	bool readncvariable(std::string filename, std::string varname, std::vector<T>& var, std::vector<A> &tempvar)
	{
		//to do: replace ifs with a chack that A is the right size
		
		//empty the var and shape vectors
		var.clear();
		tempvar.clear();

		//create an ncError object. This will catch any errors allowing us to check what they are
		//and will stop the default behavour of simply quiting the program
		NcError ncerr(NcError::silent_nonfatal);

		//try to open the file
		NcFile *nc=new NcFile(filename.c_str(),NcFile::ReadOnly);
		if(!nc->is_valid())
		{
			delete nc;
			sci::reporterr(SERR_NC,ncerr.get_err(),filename);
			return false;
		}

		//read the variable from the file into an NcVar and delete our NcFile
		NcVar *ncvar = nc->get_var(varname.c_str());

		//check that the variable was read correctly
		if(ncvar==NULL)
		{
			//report error and return false
			sci::reporterr(SERR_NC,ncerr.get_err(),varname);
			return false;
		}
		//get the number of elements and the shape

		//check the type and read in the data
		NcType type=ncvar->type();
		bool succeeded=true;

		//get the size and shape of the variable
		size_t npoints=ncvar->num_vals();
		int ndims=ncvar->num_dims();
		long* pshape=ncvar->edges();
		std::vector<size_t> shape(ndims);
		for(int i=0; i<ndims; ++i)shape[i]=pshape[i];

		std::vector<A> *readvar=&tempvar;
		if(sci::ndims(var)==1) readvar=(std::vector<A> *)&var;

		NcBool gotvar;
		if(ndims>0)
		{
			readvar->resize(sci::product(shape),std::numeric_limits<A>::quiet_NaN()); //create a temporary vector to initially read the data into a 1D array
			//although get claims to want a long*, it casts this to a size_t*, which stuffs up on 64 bit builds
			//where long and size_t are different sizes - pass it a size_t* to fix this.
			gotvar=ncvar->get(&((*readvar)[0]),(long*)&shape[0]); //read the elements into the temp vector
		}
		else
		{
			readvar->resize(1,std::numeric_limits<A>::quiet_NaN());
			gotvar=ncvar->get(&((*readvar)[0]),1);
		}

		if(gotvar==false || ncerr.get_err()!=0) //check if the read failed
		{
			sci::reporterr(SERR_NC,ncerr.get_err(),varname);
			succeeded=false;
		}

		//sometimes this fails for some reason, but still returns true. Check if the vector has been filled and if not then read in
		//each value individually - this is probably a lot slower than above
		if(readvar->at(0)!=readvar->at(0))
		{
			//to do check if this is still needed
			//this is a warning for me to check what's going on
			//wxMessageBox(wxT("netcdf reported correct reading but the first element is still a qnan for variable ")+varname+wxT(" and file ")+filename);
			NcValues *ncvalues=ncvar->values();
			for(size_t i=0; i<npoints; i++) *(readvar->begin()+i)=(A)ncvalues->as_double((long)i);
			//delete[] ncvalues;
		}
		//copy the elements from the temp vector to the final double vector
		if(sci::ndims(var)!=1)sci::reshape(var,tempvar,shape);
		

		delete[] pshape;
		delete nc;
		return succeeded;
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

		if(data.size()>0)
			var->put(&data[0],(long*)&datashape[0]);//this is the botch casting the size_t* to a long*
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
		std::vector<double> flatdata;
		sci::flatten(flatdata,data);
		if(flatdata.size()==0) return false;
		std::vector<size_t> datashape=sci::shape(data);
		/*std::vector<long> datashapelong(datashape.size());
		for(size_t i=0; i<datashape.size(); ++i) 
		{
			if(datashape[i]>size_t(std::numeric_limits<long>::max())) return false;
			datashapelong[i]=(long)datashape[i];
		}*/
		return writencvariable(filename,variablename,linkeddimensions,flatdata,datashape);
	}

	
	void splitstring(const std::string &datastring, const std::string &separators, bool mergeadjacentseparators, std::vector<std::string> &splitstring);
	std::vector<std::string> splitstring(const std::string &datastring, const std::string &separators, bool mergeadjacentseparators);
	bool getFileLocked (std::string fileName);

	std::vector< std::string > getAllFiles( std::string directory, bool recursive, bool includeHidden );
	std::string getFileExtension( std::string fileName );
	std::string getFileName( std::string fullPath );
	std::string getContainingDirectoryPath( std::string fullPath );
	std::string concatPath( std::string part1, std::string part2);
	
}



#endif