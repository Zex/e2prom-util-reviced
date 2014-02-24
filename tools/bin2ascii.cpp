/* 
 * bin2ascii.cpp
 *
 * Demonstration of ascii-to-binary convertor for E2PROM
 *
 * Author: Zex <zex@hytera.com>
 */
#include <tools/converter.hpp>
#include "version.h"

use_ns(e2prom)

void read_bin(char* binfile, char* user_data_conf, char* outfile = 0)
{
	Bin2Ascii b2a(binfile, user_data_conf, outfile);
}

void parse_parm(int argc, char* argv[])
{
	if (3 > argc) {
		throw e2prom::E2promParamException(
		"Too few arguments for bin2ascii", e2prom::to_string<int>(__LINE__),
		"Usage: bin2ascii [binary file] [user data config file] [output file]");
	}
}

int main(int argc, char* argv[])
{
	E2promMsg(PROJECT+std::string(" utility bin2ascii V")+VERSION);
	WRITE_FILE = true;

	try {

		parse_parm(argc, argv);

	} catch (e2prom::E2promException &err) {
	
		std::cerr << err.what() << '\n';
		return -1;
	}

	if (3 < argc)
		read_bin(argv[1], argv[2], argv[3]);
	else
		read_bin(argv[1], argv[2]);

	return 0;
}

