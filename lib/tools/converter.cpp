/* 
 * tools/converter.cpp
 *
 * Demonstration of ascii-to-binary convertor for E2PROM
 *
 * Author: Zex <zex@hytera.com>
 */
#include <tools/converter.hpp>

ns_begin(e2prom)

Bin2Ascii::Bin2Ascii(const char* binfile, const char* user_data_conf, const char* outfile)
{
	ifs.open(binfile, std::ios::binary);
	ifs_conf.open(user_data_conf);
	std::string asciifile;

	if (!outfile) {
	
		std::string fname =  binfile;
		asciifile = split_by('.', fname)[0] + AFILE_SURFIX;

	} else {
	
		asciifile = outfile;
	}

	if (WRITE_FILE)
		ofs.open(asciifile, std::ios::out);

	try {
		dealing_bin(ifs, ifs_conf, ofs);
	} catch (E2promException &err) {
		clear_scene();	
		std::cerr << err.what() << "\n";
	}
}

//Ascii2Bin::Ascii2Bin(const char* asciifile, const char* binfile)
Temp2Bin::Temp2Bin(const char* asciifile, const char* binfile, char* argv[])
{
	ifs.open(asciifile, std::ios::in);

	std::string fname(asciifile);
	std::string binfile_name;
	encode_result encres;

	if (WRITE_FILE && !binfile) {

		binfile_name = split_by('.', fname).at(0) + BFILE_SURFIX;

	} else {

		binfile_name = binfile;
		
	}

	if (WRITE_FILE)
		ofs.open(binfile_name, std::ios::out|std::ios::binary);

	try {

		encres = dealing_ascii(ifs, ofs, argv);

	} catch (E2promException &err) {

		clear_scene();	
		std::cerr << err.what() << "\n";
	}
	
	clear_scene();
	transfer_eeprom_bin(binfile_name.c_str(), encres);
	del_temp_file();
}

ns_end(e2prom)

