/* 
 * basic.cpp
 *
 * Author: Zex <zex@hytera.com>
 *
 *
 * See also:
 *  the E2prom Data Modules
 *  DIB-CP1 SAD
 */
#include <utils/basic.hpp>

ns_begin(e2prom)

bool WRITE_FILE = false;
bool TEMPLATE_MODE = false;

void fill_front_zero(std::string &str, size_t howmany)
{
	while (howmany > str.size())
		str = '0' + str;
}

/* split string by delim */
std::vector<std::string> split_by(char delim, std::string &str)
{
	std::vector<std::string> ret;
	std::string tmp;
	
	for (size_t i = 0; i < str.size(); i++) {
		if (delim == str[i]) {
			ret.push_back(tmp);
			tmp.clear();
		} else {
			tmp += str[i];
		}
	}

	/* append the rest */
	if (tmp.size())
		ret.push_back(tmp);

	return ret;
}
	
bool need_fix_align(size_t size_by, size_t alignment)
{
	if ((size_by*8 < alignment) || ((size_by*8) % alignment))
		return true;
	return false;
}

#if DEBUG
void debug_bin(std::string line)
{
	uint8_t in_byte;
	bool set_space = true;

	for (size_t i = 0; i < line.size(); i++) {
		
		in_byte = *reinterpret_cast<uint8_t*>(&line[i]);
		
		if (!in_byte || in_byte < 0x10) printf("0");

		printf("%x", in_byte);
		set_space ^= true;
		
		if (set_space) printf(" ");
		if (!((1+i) % 16)) printf("\n");
	}
	printf("\n");
}
	
#endif
bool path_exist(const char* path)
{
	struct stat stat_f;

	if (0 > stat(path, &stat_f))
		/* file doesn't exist */
		if (ENOENT == errno)
			return false;
	return true;
}

void calc_macaddr(std::string serial_nr, uint8_t macaddr[6])
{
	uint16_t tmp; /* ret[2]; */
	serial_nr.erase(serial_nr.begin());
	std::stringstream ss(serial_nr);

	ss >> tmp;
//	ret[0] = tmp >> 8;
//	ret[1] = tmp & 0xff;

	macaddr[4] = tmp >> 8; //to_string<uint16_t>(ret[0]);
	macaddr[5] = tmp & 0xff; //to_string<uint16_t>(ret[1]);
}

ns_end(e2prom)


