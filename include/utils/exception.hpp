/* 
 * exception.hpp
 *
 * Exception definition for E2PROM
 *
 * Author: Zex <zex@hytera.com>
 */
#include <stdexcept>

ns_begin(e2prom)

static std::string err_prefix("e2prom-lib: ");

#define E2promMsg(s) do {std::cout<<s<<std::endl;} while(0);
#define E2promErrMsg(s) do {std::cerr<<s<<std::endl;} while(0);

class E2promException : public std::exception
{
public:
	E2promException(std::string err) : errstr (err) {}

	const char* what() const throw()
	{
		return errstr.c_str();
	}
	
	std::string errstr;
}; /* class E2promException */

class E2promTypeException : public E2promException
{
public:
E2promTypeException(std::string err, std::string line, std::string (*usage)() = 0) : E2promException (err_prefix)
	{
		if (line.size())
			errstr += "[" + line + "]";
		
		errstr += err;

		if (usage)
			errstr += '\n' + (*usage)();
	}
	
E2promTypeException(std::string err, std::string line, std::string usage)
	: E2promException (err_prefix)
	{
		if (line.size())
			errstr += "[" + line + "]";
		
		errstr += err;

		if (usage.size())
			errstr += '\n' + usage;
	}
}; /* class E2promTypeException */

class E2promValueException : public E2promException
{
public:
E2promValueException(std::string err, std::string line, std::string (*usage)() = 0) : E2promException (err_prefix)
	{
		if (line.size())
			errstr += "[" + line + "]";

		errstr += err;

		if (usage)
			errstr += '\n' + (*usage)();
	}
	
E2promValueException(std::string err, std::string line, std::string usage)
	: E2promException (err_prefix)
	{
		if (line.size())
			errstr += "[" + line + "]";

		errstr += err;

		if (usage.size())
			errstr += '\n' + usage;
	}
}; /* class E2promValueException */

class E2promParamException : public E2promException
{
public:
E2promParamException(std::string err, std::string line, std::string (*usage)() = 0)
	: E2promException (err_prefix)
	{
		if (line.size())
			errstr += "[" + line + "]";

		errstr += err;

		if (usage)
			errstr += '\n' + (*usage)();
	}
	
E2promParamException(std::string err, std::string line, std::string usage)
	: E2promException (err_prefix)
	{
		if (line.size())
			errstr += "[" + line + "]";

		errstr += err;

		if (usage.size())
			errstr += '\n' + usage;
	}
}; /* class E2promParamException */

class E2promFormatException : public E2promException
{
public:
E2promFormatException(std::string err, std::string line, std::string (*usage)() = 0) : E2promException (err_prefix)
	{
		if (line.size())
			errstr += "[" + line + "]";

		errstr += err;

		if (usage)
			errstr += '\n' + (*usage)();
	}
	
E2promFormatException(std::string err, std::string line, std::string usage)
	: E2promException (err_prefix)
	{
		if (line.size())
			errstr += "[" + line + "]";

		errstr += err;

		if (usage.size())
			errstr += '\n' + usage;
	}
}; /* class E2promFormatException */

ns_end(e2prom)
