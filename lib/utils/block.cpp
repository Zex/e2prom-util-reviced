/* 
 * block.cpp
 *
 * Data structures definition in E2PROM
 *
 * Author: Zex <zex@hytera.com>
 *
 *
 * See also:
 *  the E2prom Data Modules
 *  DIB-CP1 SAD
 */
#include <utils/block.hpp>

ns_begin(e2prom)

uint32_t GLOBAL_ASCII_WIDTH = 20;
uint32_t GLOBAL_BLOCK_GAP = 3;		/* gap between each block in line */
size_t GLOBAL_MAX_BIN_SIZE = 1024*32; /* kbytes */
const char* GLOBAL_TEMP_FILE = "eeprom.tmp";

void UserBlockHeader::set_checksum(std::string &fmstr, encode_result &encres)
{
	std::stringstream ss;
	uint16_t fix_by = 0;
	uint8_t fix_by8 = 0;
	size_t by_to_fix = 0;
	
	if (ALIGN_32 == encres.GLOBAL_ALIGNMENT) 
		blocksize += sizeof(uint16_t);
	
	while (need_fix_align(blocksize, encres.GLOBAL_ALIGNMENT)) {
		blocksize++;
		by_to_fix++;
	}
	
	ss.write(reinterpret_cast<char*>(this), sizeof(UserBlockHeader));

	if (ALIGN_32 == encres.GLOBAL_ALIGNMENT)
		ss.write(reinterpret_cast<char*>(&fix_by), sizeof(fix_by));

	ss.write(fmstr.c_str(), fmstr.size());

	while (by_to_fix--)	
		ss.write(reinterpret_cast<char*>(&fix_by8), sizeof(fix_by8));

	checksum = crc16(reinterpret_cast<uint8_t*>(
		const_cast<char*>(ss.str().c_str())), blocksize);
}

UserBlockHeader::UserBlockHeader(uint16_t id)
: checksum(0)
{
	ID = id;
	blocksize = sizeof(UserBlockHeader);
}

void UserBlockHeader::write_bin(std::ostream &o, encode_result &encres)
{
	uint16_t fix_by = 0;

	if (!WRITE_FILE) {

		write_str(*this, encres.eeprom_bin_str);
		if (ALIGN_32 == encres.GLOBAL_ALIGNMENT)
			write_str(fix_by, encres.eeprom_bin_str);

	} else {

		o << *this;
		if (ALIGN_32 == encres.GLOBAL_ALIGNMENT)
			o.write(reinterpret_cast<char*>(&fix_by), sizeof(uint16_t));
	}
}

void EndBlock::write_payload_size(encode_result &encres)
{
	std::ofstream ofs;

	ofs.open(GLOBAL_TEMP_FILE, std::ios::binary|std::ios::out);
	ofs.write(reinterpret_cast<char*>(&encres.GLOBAL_PAYLOAD_SIZE), sizeof(encres.GLOBAL_PAYLOAD_SIZE));
	ofs.close();
}

//friend std::ostream& operator<< (std::ostream &o, EndBlock &b)
void EndBlock::write_bin(std::ostream &o, encode_result &encres)
{
	uint8_t fix_by = 0;

	if (!WRITE_FILE) {

		write_str(*this, encres.eeprom_bin_str);
		while ((encres.GLOBAL_EEPROM_SIZE) > encres.eeprom_bin_str.size()) 
			write_str(fix_by, encres.eeprom_bin_str);

	} else {

		o << *this;
		encres.GLOBAL_PAYLOAD_SIZE = o.tellp();
		encres.GLOBAL_PAYLOAD_SIZE += sizeof(encres.GLOBAL_PAYLOAD_SIZE);
		write_payload_size(encres);
		while ((encres.GLOBAL_EEPROM_SIZE) > o.tellp()) 
			o.write(reinterpret_cast<char*>(&fix_by), sizeof(fix_by));
	}
//	return o;
}

encode_result::encode_result()
:	end_reached(false),
	line_nr(0),
	line_in_blk(0),
	para_nr(2),
	blocks_mark(0),
	GLOBAL_ALIGNMENT(16),		/* bits */
	GLOBAL_EEPROM_SIZE(1024), /* bytes */
//	GLOBAL_ASCII_WIDTH = 20;
//	GLOBAL_BLOCK_GAP = 3;		/* gap between each block in line */
//	GLOBAL_MAX_BIN_SIZE = 1024*32; /* kbytes */
	THE_FIRST_ALIGNMENT_IS_READ(false),
	THE_FIRST_EEPROMSIZE_IS_READ(false),
	GLOBAL_PAYLOAD_SIZE(0)
{
}

decode_result::decode_result()
:	byte_nr(2),	/* skip the first 2 types, which indicates payload size */
	end_reached(false),
	GLOBAL_ALIGNMENT(16),		/* bits */
	GLOBAL_EEPROM_SIZE(1024), /* bytes */
	THE_FIRST_ALIGNMENT_IS_READ(false),
	THE_FIRST_EEPROMSIZE_IS_READ(false)
{
	clear_cur_user();
	clear_all();
}

void decode_result::clear_all()
{
	all_header.clear();
	all_modifi.clear();
	all_repair.clear();
	all_statisc.clear();

	all_user.clear();

	all_user_string.clear();
	all_user_uint8.clear();
	all_user_int8.clear();
	all_user_uint16.clear();
	all_user_int16.clear();
	all_user_uint32.clear();
	all_user_int32.clear();
	all_user_uint64.clear();
	all_user_int64.clear();
	all_user_double.clear();
	all_user_float.clear();
	all_user_ldouble.clear();
}

void decode_result::clear_cur_user()
{
	cur_user_uint8.clear();
	cur_user_int8.clear();
	cur_user_uint16.clear();
	cur_user_int16.clear();
	cur_user_uint32.clear();
	cur_user_int32.clear();
	cur_user_uint64.clear();
	cur_user_int64.clear();
	cur_user_double.clear();
	cur_user_float.clear();
	cur_user_ldouble.clear();
}

void decode_result::set_all_user(uint16_t uid, std::string username)
{
	safe_set_all(all_user_int8, cur_user_int8, uid, username)
	safe_set_all(all_user_uint8, cur_user_uint8, uid, username)
	safe_set_all(all_user_int16, cur_user_int16, uid, username)
	safe_set_all(all_user_uint16, cur_user_uint16, uid, username)
	safe_set_all(all_user_int32, cur_user_int32, uid, username)
	safe_set_all(all_user_uint32, cur_user_uint32, uid, username)
	safe_set_all(all_user_int64, cur_user_int64, uid, username)
	safe_set_all(all_user_uint64, cur_user_uint64, uid, username)
	safe_set_all(all_user_double, cur_user_double, uid, username)
	safe_set_all(all_user_float, cur_user_float, uid, username)
	safe_set_all(all_user_ldouble, cur_user_ldouble, uid, username)
}
#if 0
std::map<UserHeader, strvec_t> all_user_string;
std::map<UserHeader, uint8vec_t> all_user_uint8;
std::map<UserHeader, int8vec_t> all_user_int8;
std::map<UserHeader, uint16vec_t> all_user_uint16;
std::map<UserHeader, int16vec_t> all_user_int16;
std::map<UserHeader, uint32vec_t> all_user_uint32;
std::map<UserHeader, int32vec_t> all_user_int32;
std::map<UserHeader, uint64vec_t> all_user_uint64;
std::map<UserHeader, int64vec_t> all_user_int64;
std::map<UserHeader, doublevec_t> all_user_double;
std::map<UserHeader, floatvec_t> all_user_float;
std::map<UserHeader, ldoublevec_t> all_user_ldouble;

/* userid => username1, username2, ..., username n */
std::vector<UserHeader> all_user;

// id => {[]:[],[]:[], ...}
std::map<std::string, eeprom_t> all_header;
std::map<std::string, eeprom_t> all_modifi;
std::map<std::string, eeprom_t> all_repair;
std::map<std::string, eeprom_t> all_statisc;
// uheader => [hex string]
//std::map<std::string, std::string> all_user;
//std::vector<std::string> all_user;
//std::string current_user_allval;
#endif
//std::string eeprom_bin_str;

#if 0
/* global user block buffer, clear them each time new user block is read */
uint8vec_t cur_user_uint8;
int8vec_t cur_user_int8;
uint16vec_t cur_user_uint16;
int16vec_t cur_user_int16;
uint32vec_t cur_user_uint32;
int32vec_t cur_user_int32;
uint64vec_t cur_user_uint64;
int64vec_t cur_user_int64;
doublevec_t cur_user_double;
floatvec_t cur_user_float;
ldoublevec_t cur_user_ldouble;

void clear_cur_user()
{
	cur_user_uint8.clear();
	cur_user_int8.clear();
	cur_user_uint16.clear();
	cur_user_int16.clear();
	cur_user_uint32.clear();
	cur_user_int32.clear();
	cur_user_uint64.clear();
	cur_user_int64.clear();
	cur_user_double.clear();
	cur_user_float.clear();
	cur_user_ldouble.clear();
}

void set_all_user(uint16_t uid, std::string username)
{
	safe_set_all(all_user_int8, cur_user_int8, uid, username)
	safe_set_all(all_user_uint8, cur_user_uint8, uid, username)
	safe_set_all(all_user_int16, cur_user_int16, uid, username)
	safe_set_all(all_user_uint16, cur_user_uint16, uid, username)
	safe_set_all(all_user_int32, cur_user_int32, uid, username)
	safe_set_all(all_user_uint32, cur_user_uint32, uid, username)
	safe_set_all(all_user_int64, cur_user_int64, uid, username)
	safe_set_all(all_user_uint64, cur_user_uint64, uid, username)
	safe_set_all(all_user_double, cur_user_double, uid, username)
	safe_set_all(all_user_float, cur_user_float, uid, username)
	safe_set_all(all_user_ldouble, cur_user_ldouble, uid, username)
}
#endif

/*
 * | 8   | 4     | 4
 * | year| month | day
 *
 * date = (year*2^9) + (month*2^5 + day)
 */
uint16_t encode_date(std::string &buffer)
{
	std::vector<std::string> buf = split_by('-', buffer);
	uint16_t date;
	
	date =
		((to_digits<uint16_t>(buf[0]) - BASE_YEAR) << 9)+ /* year */
		(to_digits<uint16_t>(buf[1])<<5)+ /* month */
		(to_digits<uint16_t>(buf[2]));    /* day */

	return date;
}
/* get date string like 2011-03-28 */
std::string decode_date(uint16_t date)
{
	std::stringstream ss;

	uint16_t year = ((date >> 9) & 0xff);
	uint16_t right = date - (year << 9);
	year += BASE_YEAR;
	uint16_t mon = (((right) >> 5) & 0xf);
	uint16_t day = (right - (mon << 5));
	
	ss << year << '-';
	if (mon < 10) ss << '0';
	ss << mon << '-';
	if (day < 10) ss << '0';
	ss << day;

	return ss.str();
}

std::string decode_serial_number(uint32_t sn)
{
	std::stringstream ss;
	std::string buf;
	size_t max = 9;

	ss << sn;
	buf = ss.str();
	
	while (buf.size() < max)
		buf = '0' + buf;

	std::string first(&buf[0], &buf[6]);
	std::string second(&buf[6], &buf[9]);

	return (first + '/' + second);
}

/* get xx.yy string */
std::string decode_dotted_uint16(uint16_t pi)
{
	std::stringstream ss;
	std::string ret;

	ss << pi;
	ret = ss.str();

	fill_front_zero(ret, 4);
	ret.insert(2, ".");

	return ret;
}

/* get xxxx.yyyy.zz string */
std::string decode_partnumber(uint32_t pn, uint8_t pn_resv)
{
	std::stringstream ss;
	std::string ret;
	int msz = 8;

	ss << pn;
	ret = ss.str();

	fill_front_zero(ret, msz);

//	fill_front_zero(ret, 4);
	ret.insert(4, ".");
	
	if (pn_resv) {
		ret += ".";

		std::string resv(to_string<uint16_t>(pn_resv));
		fill_front_zero(resv, 2);

		ret += resv;
	}

	return ret;
}

/*
 * OPHOURS = (cycle_counts * (4 cycles per day)) << 16 + ophour_counts
 *
 * Ex:
 * 	 1301 hours of operating time and 798 times switch
 * 	 ==> 798/1301(ascii)
 * 	 ==> (798*4)<<16 + 1301
 * 	 ==> 0x0x1505780c(binary)
 */
std::string decode_ophours(uint32_t ophours)
{
	uint32_t cycle_counts = (ophours >> 18) & 0x3ffff;
	uint16_t ophour_counts = ophours - (cycle_counts << 18);

	return (to_string<uint32_t>(cycle_counts) + '/' + to_string<uint16_t>(ophour_counts));
}

void write_bin_ufc(std::ofstream &o, std::string &buffer, encode_result &encres)
{
	uint8_t fix_by = 0;
	size_t written_by = 0;

	if (!WRITE_FILE) {
		write_bin_str(buffer, encres);
		return;
	}

	o.write(buffer.c_str(), buffer.size());
	written_by += buffer.size();
	
	if (ALIGN_32 == encres.GLOBAL_ALIGNMENT)
		while (need_fix_align(written_by, ALIGN_32)) {
			o.write(reinterpret_cast<char*>(&fix_by), sizeof(fix_by));
			written_by += sizeof(fix_by);
		}

	else// if (ALIGN_16 == GLOBAL_ALIGNMENT)
		while (need_fix_align(written_by, ALIGN_16)) {
			o.write(reinterpret_cast<char*>(&fix_by), sizeof(fix_by));
			written_by += sizeof(fix_by);
		}
}

void write_bin_str(std::string &buffer, encode_result &encres)
{
	uint8_t fix_by = 0;
	size_t written_by = 0;

//	eeprom_bin_str += buffer;
	written_by += buffer.size();
	
	if (ALIGN_32 == encres.GLOBAL_ALIGNMENT)
		while (need_fix_align(written_by, ALIGN_32)) {
			write_str(fix_by, encres.eeprom_bin_str);
			written_by += sizeof(fix_by);
		}

	else// if (ALIGN_16 == GLOBAL_ALIGNMENT)
		while (need_fix_align(written_by, ALIGN_16)) {
			write_str(fix_by, encres.eeprom_bin_str);
			written_by += sizeof(fix_by);
		}
}

void payload_to_temp(const char* dest)
{
	std::ifstream ifs;
	std::ofstream ofs;
	char payload[GLOBAL_MAX_BIN_SIZE];

	ifs.open(dest, std::ios::binary);
	ofs.open(GLOBAL_TEMP_FILE, std::ios::binary|std::ios::app);

	ifs.read(payload, GLOBAL_MAX_BIN_SIZE);
	ofs.write(payload, GLOBAL_MAX_BIN_SIZE);

	ifs.close();
	ofs.close();
}

void temp_to_payload(const char* dest, encode_result &encres)
{
#ifndef MinGW
	int out_fd, in_fd;
	off_t offs = 0;

	out_fd = open(dest, O_RDWR);
	in_fd = open(GLOBAL_TEMP_FILE, O_RDONLY);
	sendfile(out_fd, in_fd, &offs, encres.GLOBAL_EEPROM_SIZE);
	close(in_fd);
	close(out_fd);
#else
	std::ifstream ifs;
	std::ofstream ofs;
	char payload[encres.GLOBAL_EEPROM_SIZE];

	ifs.open(GLOBAL_TEMP_FILE, std::ios::binary);
	ofs.open(dest, std::ios::binary|std::ios::out);

	ifs.read(payload, encres.GLOBAL_EEPROM_SIZE);
	ofs.write(payload, encres.GLOBAL_EEPROM_SIZE);

	ifs.close();
	ifs.close();
#endif
}

void del_temp_file()
{
	std::remove(GLOBAL_TEMP_FILE);
}

void transfer_eeprom_bin(const char* dest, encode_result &encres)
{
	payload_to_temp(dest);
	temp_to_payload(dest, encres);
}


ns_end(e2prom)
