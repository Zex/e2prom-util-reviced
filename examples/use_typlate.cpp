#include "typlate/typlate.hpp"

use_ns(e2prom)

int main( int argc, char* argv[] )
{
#if 0 /* packing */
	PackTypl pt("dib-r5.bin");
	for (auto &x: pt.start()){
		std::cout << std::hex << x << '\n';
	}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#else /* unpacking */
	std::ifstream ifs;
	std::string buf;

	ifs.open("dib-r5.bin", std::ios::binary);
	std::getline(ifs, buf);
	
	UnpackTypl ut(buf.data());
	ut.start();
	ifs.close();
// ================= data examing ================
	std::cout << block_gap();
	std::cout << "EEPROMSIZE: " << ut.get_header_s("EEPROMSIZE") << '\n';
	std::cout << "HWCODE: " << ut.get_header_s("HWCODE") << '\n';

	try
	{
		uint8vec_t v1 = ut.get_user_uint8(18030, "hello");
		for ( auto &x : v1 )
			std::cout << (int)x << '\n';
	} catch( E2promException &e ) {
		std::cerr << e.what() << '\n';
	}
	
	std::cout << block_gap();
	
	try {
		int32vec_t v2 = ut.get_user_int32(18031, "BSCU_1");
	} catch( E2promException &e ) {
		std::cerr << e.what() << '\n';
//		doublevec_t v3 = ut.get_user_double(18055);
	}
//	ut.get_statisc();
#endif

	return 0;
}
