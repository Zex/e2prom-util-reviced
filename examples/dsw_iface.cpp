#include "typlate/dsw_iface_wrapper.hpp"

use_ns(e2prom)

#define pr_all_in(l) for (auto &x : l) write_file(std::cout, x << '\n');
#define pr_all_in_n(l) for (auto &x : l) write_file(std::cout, (uint32_t)x << '\n');

void old_version_iface()
{
	std::ofstream ofs;
#if 1 /* packing */
	PackTypl pt1("bscu.bin");
	strvec_t res = pt1.start();

	ofs.open("bscu.typ", std::ios::binary);
	for (auto &x : res) ofs << x;
	ofs.close();
//-----------------------------------------------------
	PackTypl pt2;
	strvec_t res2 = pt2.start();

	ofs.open("bscu.empty", std::ios::binary);
	for (auto &x : res2) ofs << x;
	ofs.close();
//-----------------------------------------------------
	WRITE_FILE = false;
	PackTypl pt3("ref/bscu_eeprom.txt", false);
	strvec_t res3 = pt3.start();

	ofs.open("bscu.fromascii", std::ios::binary);
	for (auto &x : res3) ofs << x;
	ofs.close();
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#else /* unpacking */
//-----------------testing--------------------
	std::string buf;
	std::ifstream ifs;
	ifs.open("eeprom.bin", std::ios::binary);
	std::getline(ifs, buf);
	UnpackTypl ty(buf, "ref/xtype.ini.old");
	ty.start();
#  if 0
//------------------------------
	for (auto &x: ty.list_all_header())
		std::cout << x << '\n';
//------------------------------
	for (auto &x: ty.list_all_user())
		std::cout << x << '\n';
//------------------------------
	std::cout << ty.get_header_s("FAN UNIT", "PARTNUMBER") << '\n';
	std::cout << ty.get_header_s("FAN UNIT", "HWCODE") << '\n';
	std::cout << ty.get_header_s("FAN UNIT", "PRODUCTINDEX") << '\n';
	std::cout << ty.get_header_s("FAN UNIT", "SN") << '\n';
	std::cout << ty.get_header_s("FAN UNIT", "PRODUCTDATE") << '\n';
	std::cout << ty.get_header_s("FAN UNIT", "READCODE") << '\n';
	std::cout << ty.get_header_s("FAN UNIT", "TESTINSTRUCTION") << '\n';
//------------------------------
	std::cout << ty.get_all_value_of_user("USER_18502") << '\n';
	std::cout << ty.get_all_value_of_user("USER_18030") << '\n';
	std::cout << ty.get_all_value_of_user("USER_18500") << '\n';
	std::cout << ty.get_all_value_of_user("USER_18501") << '\n';
//-----------------end of testing--------------------
   /* to check value of USER_18090, which own uint8 values
	* [170, 57, 27, 184, 144, 0]
	* $ ./dsw_iface 2> buf;xxd buf
	* $ 0000000: aa39 1bb8 9000  .9.... */
	std::cerr << ty.get_all_value_of_user("USER_18030");
#  endif
#endif
}

void try_eeprom_bin()
{
	std::string buf;
	std::ifstream ifs;
	ifs.open("eeprom.bin", std::ios::binary);
	std::getline(ifs, buf);
	UnpackTypl ty(buf.c_str());//, "ref/xtype.ini.old");
	ty.start();
//------------------------------------------------------------
// no user name for 18030 in eeprom.bin
//	strvec_t users = ty.list_all_user(18030);

//	for (auto &x : users)
//		std::cout << x << '\n';
//------------------------------------------------------------
	E2promMsg("header-----------------------------------------");
	write_file(std::cout, ty.get_header_s("PARTNUMBER") << '\n');
	write_file(std::cout, ty.get_header_s("HWCODE") << '\n');
	write_file(std::cout, ty.get_header_s("PRODUCTINDEX") << '\n');
	write_file(std::cout, ty.get_header_s("SN") << '\n');
	write_file(std::cout, ty.get_header_s("PRODUCTDATE") << '\n');
	write_file(std::cout, ty.get_header_s("READCODE") << '\n');
	write_file(std::cout, ty.get_header_s("TESTINSTRUCTION") << '\n');

	ifs.close();
}

void try_dibr5_bin()
{
	std::string buf;
	std::ifstream ifs;
	ifs.open("dib-r5.bin", std::ios::binary);
	std::getline(ifs, buf);
	UnpackTypl ty(buf.data());
//-------------testing cache--------------------
	ty.update_cache("dibr5", 8192);return;
//------------end testing cache-----------------
	ty.start();
	ifs.close();
//-----------------------------------------------
//	strvec_t users = ty.list_all_user(18030);

//-----------------------------------------------
	E2promMsg("header:''''''''''''''''''''''''''''''''''''''''''\n");
	std::cout << ty.get_header_s("PARTNUMBER") << '\n';
	std::cout << ty.get_header_s("HWCODE") << '\n';
	std::cout << ty.get_header_s("PRODUCTINDEX") << '\n';
	std::cout << ty.get_header_s("SN") << '\n';
	std::cout << ty.get_header_s("PRODUCTDATE") << '\n';
	std::cout << ty.get_header_s("READCODE") << '\n';
	std::cout << ty.get_header_s("TESTINSTRUCTION") << '\n';
//-----------------------------------------------
	strvec_t tetrachu1 = ty.get_user_string(18031, "TETRA_CHU_1");
	E2promMsg("TETRA CHU''''''''''''''''''''''''''''''''''''''''\n");
	for (auto &x:tetrachu1)
		std::cout << x << '\n';
	
	uint16vec_t tetrachu3 = ty.get_user_uint16(18031, "TETRA_CHU_3");
	for (auto &x:tetrachu3)
		std::cout << x << '\n';
}

void try_dibr5_cache()
{
	std::string entry("dib-r5");
	UnpackTypl ty(entry);
	ty.start();

	E2promMsg("header:''''''''''''''''''''''''''''''''''''''''''\n");
	std::cout << ty.get_header_s("PARTNUMBER") << '\n';
	std::cout << ty.get_header_s("HWCODE") << '\n';
	std::cout << ty.get_header_s("PRODUCTINDEX") << '\n';
	std::cout << ty.get_header_s("SN") << '\n';
	std::cout << ty.get_header_s("PRODUCTDATE") << '\n';
	std::cout << ty.get_header_s("READCODE") << '\n';
	std::cout << ty.get_header_s("TESTINSTRUCTION") << '\n';
//-----------------------------------------------
	strvec_t tetrachu1 = ty.get_user_string(18031, "TETRA_CHU_1");
	E2promMsg("TETRA CHU''''''''''''''''''''''''''''''''''''''''\n");
	for (auto &x:tetrachu1)
		std::cout << x << '\n';
	
	uint16vec_t tetrachu3 = ty.get_user_uint16(18031, "TETRA_CHU_3");
	for (auto &x:tetrachu3)
		std::cout << x << '\n';
}

void try_dibr5_TyplWrap()
{
	std::string entry("dib-r5");
#if 0
	TyplWrap tw(entry);
	std::vector<std::string> ret = tw.getUserSections(18031);

	std::cout << "user sections:\n";
	for (auto &x : ret)
		std::cout << x << '\n';
#else
//-------------getting eeprom buffer-------------------
	std::string buf;
	std::ifstream ifs;
	ifs.open("dib-r5.bin", std::ios::binary);
	std::getline(ifs, buf);
	ifs.close();
//----------end getting eeprom buffer-------------------
	TyplWrap tw(buf.data(), entry);
//------------------testing cache-----------------------
	tw.update_cache(8192);
//----------------end testing cache---------------------
		
	std::string  EEPROMSize,  PartNumber,
	 HwCode,  ProductIndex,
	 SerialNumber,  ProductDate,
	 ReadCode,  TestInstruction,
	 BoardName;

	std::cout << "ALIGNMENT = " << 
		tw.getParameterHeaderSection(
		 EEPROMSize, PartNumber,
		 HwCode, ProductIndex,
		 SerialNumber, ProductDate,
		 ReadCode, TestInstruction,
		 BoardName) << '\n';

	std::cout << "EEPROMSIZE = " << EEPROMSize << '\n'
		<< "PARTNUMBER = " <<  PartNumber << '\n'
		<< "HWCODE = " << HwCode << '\n'
		<< "PRODUCTINDEX = " << ProductIndex << '\n'
		<< "SN = " << SerialNumber << '\n'
		<< "PRODUCTDATE = "<< ProductDate << '\n'
		<< "READCODE = " << ReadCode << '\n'
		<< "TESTINSTRUCTION = " << TestInstruction << '\n'
		<< "BOARDNAME = " <<  BoardName << '\n';
//-----------------------------------------------------
//	entry = "bscu";
//	TyplWrap tw2(entry);
//		
//	std::cout << "ALIGNMENT = " << 
//		tw2.getParameterHeaderSection(
//		 EEPROMSize, PartNumber,
//		 HwCode, ProductIndex,
//		 SerialNumber, ProductDate,
//		 ReadCode, TestInstruction,
//		 BoardName) << '\n';
//
//	std::cout << "EEPROMSIZE = " << EEPROMSize << '\n'
//		<< "PARTNUMBER = " <<  PartNumber << '\n'
//		<< "HWCODE = " << HwCode << '\n'
//		<< "PRODUCTINDEX = " << ProductIndex << '\n'
//		<< "SN = " << SerialNumber << '\n'
//		<< "PRODUCTDATE = "<< ProductDate << '\n'
//		<< "READCODE = " << ReadCode << '\n'
//		<< "TESTINSTRUCTION = " << TestInstruction << '\n'
//		<< "BOARDNAME = " <<  BoardName << '\n';
#endif
}
void try_bscu_eeprom()
{
	std::string entry = "bscu";
	TyplWrap tw2(entry);

	std::string  EEPROMSize,  PartNumber,
	 HwCode,  ProductIndex,
	 SerialNumber,  ProductDate,
	 ReadCode,  TestInstruction,
	 BoardName;
		
	std::cout << "ALIGNMENT = " << 
		tw2.getParameterHeaderSection(
		 EEPROMSize, PartNumber,
		 HwCode, ProductIndex,
		 SerialNumber, ProductDate,
		 ReadCode, TestInstruction,
		 BoardName) << '\n';

	std::cout << "EEPROMSIZE = " << EEPROMSize << '\n'
		<< "PARTNUMBER = " <<  PartNumber << '\n'
		<< "HWCODE = " << HwCode << '\n'
		<< "PRODUCTINDEX = " << ProductIndex << '\n'
		<< "SN = " << SerialNumber << '\n'
		<< "PRODUCTDATE = "<< ProductDate << '\n'
		<< "READCODE = " << ReadCode << '\n'
		<< "TESTINSTRUCTION = " << TestInstruction << '\n'
		<< "BOARDNAME = " <<  BoardName << '\n';
}
void try_bcb_bin()
{
	std::string entry = "bcb";

	std::string buf;
	std::ifstream ifs;
	ifs.open("bcb.bin", std::ios::binary);
	std::getline(ifs, buf);
	UnpackTypl ty(buf.data());
//-------------testing cache--------------------
	ty.update_cache("bcb", 64*1024);
//------------end testing cache-----------------
	ty.start();
	ifs.close();

	TyplWrap tw2(entry);

	std::string  EEPROMSize,  PartNumber,
	 HwCode,  ProductIndex,
	 SerialNumber,  ProductDate,
	 ReadCode,  TestInstruction,
	 BoardName;
		
	std::cout << "ALIGNMENT = " << 
		tw2.getParameterHeaderSection(
		 EEPROMSize, PartNumber,
		 HwCode, ProductIndex,
		 SerialNumber, ProductDate,
		 ReadCode, TestInstruction,
		 BoardName) << '\n';

	std::cout << "EEPROMSIZE = " << EEPROMSize << '\n'
		<< "PARTNUMBER = " <<  PartNumber << '\n'
		<< "HWCODE = " << HwCode << '\n'
		<< "PRODUCTINDEX = " << ProductIndex << '\n'
		<< "SN = " << SerialNumber << '\n'
		<< "PRODUCTDATE = "<< ProductDate << '\n'
		<< "READCODE = " << ReadCode << '\n'
		<< "TESTINSTRUCTION = " << TestInstruction << '\n'
		<< "BOARDNAME = " <<  BoardName << '\n';
}

int main( int argc, char* argv[] )
{
	WRITE_FILE = false;
#if 1
//	try_eeprom_bin();
	try_dibr5_TyplWrap();
	try_dibr5_bin();
	try_dibr5_cache();
//	try_bscu_eeprom();
	try_bcb_bin();
#else
	try_bcb_bin();
//	old_version_iface();
#endif

	return 0;
}
