/* 
 * ascii2bin.cpp
 *
 * Demonstration of ascii-to-binary convertor for E2PROM
 *
 * Author: Zex <zex@hytera.com>
 */
#include <tools/converter.hpp>
#include "version.h"

use_ns(e2prom)

void read_ascii(char* asciifile, char* binfile = 0)
{
	Ascii2Bin a2b(asciifile, binfile);
}

void parse_parm(int argc, char* argv[])
{
	if (2 > argc) {
	
		throw e2prom::E2promParamException(
			"Too few arguments for ascii2bin",
			e2prom::to_string<int>(__LINE__),
			"Usage: ascii2bin [ascii file] [output file]");
	}
}

int main(int argc, char* argv[])
{
	E2promMsg(PROJECT+std::string(" utility ascii2bin V")+VERSION);
	WRITE_FILE = true;

	try {

		parse_parm(argc, argv);

	} catch (e2prom::E2promException &err) {
	
		std::cerr << err.what() << '\n';
		return -1;
	}

	if (2 < argc) 
		read_ascii(argv[1], argv[2]);
	else
		read_ascii(argv[1]);

	return 0;
}
