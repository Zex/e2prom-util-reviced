/* 
 * typlate/dsw_iface_wrapper.cpp
 *
 * Interfaces wrapper for DSW
 *
 * Author: Zex <zex@hytera.com>
 *
 *
 * See also:
 *  the E2prom Data Modules
 *  DIB-CP1 SAD
 */
#include <typlate/dsw_iface_wrapper.hpp>

use_ns(e2prom)

std::vector<std::string> TyplWrap::getUserSections(
	uint16_t UserSection)
{
	UnpackTypl ty(entry_);//(buf_);
	ty.start();

	return ty.list_all_user(UserSection);
}

std::string TyplWrap::getParameterHeaderSection (
	std::string &EEPROMSize, std::string &PartNumber,
	std::string &HwCode, std::string &ProductIndex,
	std::string &SerialNumber, std::string &ProductDate,
	std::string &ReadCode, std::string &TestInstruction,
	std::string &BoardName)
{
	UnpackTypl ty(entry_);//(buf_);
	ty.start();

	EEPROMSize = ty.get_header_s("EEPROMSIZE");
	PartNumber = ty.get_header_s("PARTNUMBER");
	HwCode = ty.get_header_s("HWCODE");
	ProductIndex = ty.get_header_s("PRODUCTINDEX");
	SerialNumber = ty.get_header_s("SN");
	ProductDate = ty.get_header_s("PRODUCTDATE");
	ReadCode = ty.get_header_s("READCODE");
	TestInstruction = ty.get_header_s("TESTINSTRUCTION");
	BoardName = ty.get_header_s("NAME");

	return ty.get_header_s("ALIGNMENT");
}

std::vector<uint8_t> TyplWrap::getParameterUserSection18030 (
	const std::string &IfName)
{
	UnpackTypl ty(entry_);//(buf_);
	ty.start();

	return ty.get_user_uint8(18030, IfName);
}

std::string TyplWrap::getParameterUserSection18031 (
	const std::string &SubComponentName, 
	std::string &ProductIndex, std::string &SerialNumber,
	std::string &ProductDate, std::string &ReadCode,
	std::string &TestInstruction)
{
	uint16_t uid(18031);
	UnpackTypl ty(entry_);//(buf_);
	ty.start();
	
	std::vector<std::string> vals =
		ty.get_user_string(uid, SubComponentName);

	ProductIndex = vals.at(1);
	SerialNumber = vals.at(2);
	ProductDate = vals.at(3);
	ReadCode = to_string<uint16_t>(
		ty.get_user_uint16(uid, SubComponentName).at(0));
	TestInstruction = vals.at(4);

	return vals.at(0); /* PartNumber */
}
	
void TyplWrap::update_cache(size_t esize)
{
	try {
		update_cache(entry_, esize);
	} catch (std::exception &e) {
		E2promErrMsg(e.what());
	}
}

void TyplWrap::update_cache(std::string &entry, size_t esize)  //GLOBAL_MAX_BIN_SIZE)
{
	try {
		UnpackTypl py(buf_);
		py.update_cache(entry, esize);
	} catch (std::exception &e) {
		E2promErrMsg(e.what());
	}
}



