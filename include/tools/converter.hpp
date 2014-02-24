/* 
 * tools/converter.hpp
 *
 * Demonstration of ascii-to-binary convertor for E2PROM
 *
 * Author: Zex <zex@hytera.com>
 */
#ifndef TOOLS_CONVERTER_HPP_
#define TOOLS_CONVERTER_HPP_

#include "ascii-tool.hpp"
#include "bin-tool.hpp"

ns_begin(e2prom)

class Convertor
{
protected:
	std::ifstream ifs;
	std::ofstream ofs;
public:
	~Convertor()
	{
		clear_scene();
	}
	void clear_scene()
	{
		ifs.close();
		ofs.close();
	}
};

class Bin2Ascii : Convertor
{
	std::ifstream ifs_conf;
public:
	Bin2Ascii(const char* binfile, const char* user_data_conf, const char* outfile = 0);
	~Bin2Ascii()
	{
		ifs_conf.close();
	}
};

class Temp2Bin : Convertor
{
public:
	Temp2Bin(const char* asciifile, const char* binfile = 0, char* argv[] = 0);
};

class Ascii2Bin : Temp2Bin
{
public:
	Ascii2Bin(const char* asciifile, const char* binfile = 0)
	: Temp2Bin(asciifile, binfile, 0)
	{}
};

ns_end(e2prom)

#endif /* TOOLS_CONVERTER_HPP_ */
