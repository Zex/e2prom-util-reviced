/* 
 * typlate/dsw_iface_wrapper.hpp
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
#ifndef TYPEPLATE_HPP_
#define TYPEPLATE_HPP_
#include "typlate.hpp"

struct TyplWrap
{
//	std::string buf_;
	char* buf_;
	std::string entry_;
//	bool need_update_cache_;

	TyplWrap(const char* buf)
	: buf_(const_cast<char*>(buf))
	{}

	TyplWrap(std::string &entry)
	: entry_(entry)
	{}
	
	TyplWrap(const char* buf, std::string &entry)//, bool update_cache = true)
	: buf_(const_cast<char*>(buf)),
		entry_(entry)//, need_update_cache_(update_cache)
	{}

	~TyplWrap()
	{
		std::cout << __PRETTY_FUNCTION__ << '\n';
	}
	

	std::vector<std::string> getUserSections(
		uint16_t UserSection);

	std::string getParameterHeaderSection (
		std::string &EEPROMSize, std::string &PartNumber,
		std::string &HwCode, std::string &ProductIndex,
		std::string &SerialNumber, std::string &ProductDate,
		std::string &ReadCode, std::string &TestInstruction,
		std::string &BoardName);

	std::vector<uint8_t> getParameterUserSection18030 (
		const std::string &IfName);

	std::string getParameterUserSection18031 (
		const std::string &SubComponentName, 
		std::string &ProductIndex, std::string &SerialNumber,
		std::string &ProductDate, std::string &ReadCode,
		std::string &TestInstruction);

	void update_cache(size_t esize = GLOBAL_MAX_BIN_SIZE);
	void update_cache(std::string &entry, size_t esize = GLOBAL_MAX_BIN_SIZE);
};

#endif /* TYPEPLATE_HPP_ */
