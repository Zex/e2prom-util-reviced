/* 
 * temp2bin.cpp
 *
 * Generate binary file from template with given parameters
 *
 * Author: Zex <zex@hytera.com>
 */
//#include <tools/ascii-gen-tool.hpp>
#include <tools/converter.hpp>
#include "version.h"

use_ns(e2prom)

#define usage_temp2bin \
"   temp2bin [abu template file] pi sn pd ti sn1 pd1 [output file]\n"\
"   temp2bin [bcb template file] pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 [output file]\n"\
"   temp2bin [bscu template file] pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 [output file]\n"\
"   temp2bin [dibr5 template file] pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 sn4 pd4 sn5 pd5 sn6 pd6 sn7 pd7 sn8 pd8 [output file]\n"\
"   temp2bin [fan template file] pi sn pd ti [output file]\n"\
"   pi      product index\n"\
"   sn      serial number\n"\
"   pd      product date\n"\
"   ti      test instruction\n"
/*
 * ./temp2bin abu_temp.txt pi sn pd ti sn1 pd1
 * ./temp2bin bcb_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3
 * ./temp2bin bscu_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3
 * ./temp2bin dibr5_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 sn4 pd4 sn5 pd5 sn6 pd6 sn7 pd7 sn8 pd8
 * ./temp2bin fan_temp.txt pi sn pd ti
 */
void parse_parm(int argc, char* argv[], bool &custom_output, char* path = 0)
{
	switch (argc) {
	case 0: E2promMsg(usage_temp2bin); break;
	case 6:
	case 8:
	case 12:
	case 22: custom_output = false; break;
	case 7: path = argv[6]; custom_output = true; break;
	case 9: path = argv[8]; custom_output = true; break;
	case 13: path = argv[12]; custom_output = true; break;
	case 23: path = argv[22]; custom_output = true; break;
	default:
		throw E2promParamException(
		"Too few arguments for temp2bin", e2prom::to_string<int>(__LINE__), usage_temp2bin);
	}
}

void read_temp(char* asciifile, char* binfile = 0, char* argv[] = 0)
{
	Temp2Bin t2b(asciifile, binfile, argv);
}

std::string get_dest_path(char* path)
{
	std::string dest("e2prom"), src(path);
	char dir_mark;

#ifdef MinGW
	dir_mark = '\\';
#else
	dir_mark = '/';
#endif

	if (!path_exist(dest.c_str()))
#ifndef MinGW
		if (mkdir(dest.c_str(), 0755))
#else
		if (mkdir(dest.c_str()))
#endif
			throw E2promException("Failed to create destination directory");

	strvec_t src_vec  = split_by(dir_mark, src);
	dest += dir_mark;
	dest += split_by('.', src_vec.at(src_vec.size()-1)).at(0);
	dest += BFILE_SURFIX;

	return dest;//const_cast<char*>(dest.c_str());
}

int main(int argc, char* argv[])
{
	E2promMsg(PROJECT+std::string(" utility temp2bin V")+VERSION);
	WRITE_FILE = true;
	TEMPLATE_MODE = true;

	bool custom_output = false;
	char* path = 0;

	try {

		parse_parm(argc, argv, custom_output, path);

		if (custom_output && strlen(path))
			read_temp(argv[1], path, argv);
		else
			read_temp(argv[1], const_cast<char*>(get_dest_path(argv[1]).c_str()), argv); 

	} catch (e2prom::E2promException &err) {
	
		std::cerr << err.what() << '\n';
		return -1;
	}

	return 0;
}
