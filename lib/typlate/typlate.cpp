/* 
 * typlate/typlate.cpp
 *
 * Interfaces to access e2prom-util library for DSW
 *
 * Author: Zex <zex@hytera.com>
 *
 * See also:
 *  the E2prom Data Modules
 *  DIB-CP1 SAD
 */
#include <typlate/typlate.hpp>

const char* DEFAULT_CACHE_DIR = "/tmp/eeprom-cache";

void PackTypl::read_bin_file(const char *path)
{
	std::ifstream ifs;
	ifs.open(path, std::ios::binary);
	data_ = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

	len_ = std::min((size_t)TYPEPLATE_MAX_DATA_PER_PACK, (data_.size()-offset_));
	ifs.close();
}
#if 0
void PackTypl::read_ascii_file(const char *path)
{
	std::ifstream ifs;
	std::ofstream ofs;

	ifs.open(path, std::ios::binary);
	dealing_ascii(ifs, ofs);
	data_ = eeprom_bin_str;
	len_ = std::min((size_t)TYPEPLATE_MAX_DATA_PER_PACK, (data_.size()-offset_));
	ifs.close();
}
#endif
void PackTypl::init(int size_to_read = 0)
{
	uint8_t fix_by = 0;
	int i;// = GLOBAL_MAX_BIN_SIZE;
	std::stringstream s;

	if (0 >= size_to_read)
		i = GLOBAL_MAX_BIN_SIZE;
	else
		i = size_to_read;

	while (i--)
		s.write(reinterpret_cast<char*>(&fix_by), 1);

	data_ = s.str();
	len_ = std::min((size_t)TYPEPLATE_MAX_DATA_PER_PACK, (data_.size()-offset_));
}

std::vector<std::string> PackTypl::start()
{
	std::vector<std::string> ret;
	size_t i(0);

	while (((offset_+len_) <= data_.size()) && len_) {
		std::stringstream s;

		uint16_t noff = htons(offset_);
		uint16_t nlen = htons(len_);
		s.write(reinterpret_cast<char*>(&noff), sizeof(offset_));
		s.write(reinterpret_cast<char*>(&nlen), sizeof(len_));

		for (i = offset_; i < offset_+len_; i++ )
			s.write(const_cast<char*>(&data_.at(i)), 1);

		offset_ += len_;//TYPEPLATE_MAX_DATA_PER_PACK;
		len_ = std::min((size_t)TYPEPLATE_MAX_DATA_PER_PACK, (data_.size()-offset_));
	
		ret.push_back(s.str());
	}

	return ret;
}

void UnpackTypl::update_cache(std::string entry, size_t esize)
{
	std::ofstream ofs_cache;
	entry_ = entry;
//	uint8_t fix_by = 0;

	if (!path_exist(DEFAULT_CACHE_DIR))
		if (mkdir(DEFAULT_CACHE_DIR, 0755))
			throw E2promException("Failed to create eeprom cache");
	
//	uint16_t payload_size // = get_id(raw_, 0);
//		= std::min((uint16_t)GLOBAL_MAX_BIN_SIZE, get_id(raw_, 0));
//	E2promMsg("payload_size: 0x" + get_hex_str<uint16_t>(payload_size));

	size_t i = 0;

//	if (GLOBAL_MANDOTARY_BLK_SIZE > payload_size) {
//		throw E2promException("Incomplete or old-format eeprom binary");
//	}
	ofs_cache.open(get_cache_path(), std::ios::binary);

//	while (i < esize)//payload_size) 
	do {
		ofs_cache.write(raw_+i, 1);
	} while (++i < esize);

//	while(i++ < esize)
//		ofs_cache.write(reinterpret_cast<char*>(&fix_by), 1);

	ofs_cache.close();
	E2promMsg("Updated cache @ " + get_cache_path());
	E2promMsg("Wrote: 0x" + get_hex_str<size_t>(i));
}

void UnpackTypl::read_cache()
{
	std::ifstream ifs;
	struct stat cache_stat;

	E2promMsg("Reading cache @ " + get_cache_path());

	ifs.open(get_cache_path(), std::ios::binary);
//	raw_ = new char [GLOBAL_MAX_BIN_SIZE];
	char tmp[GLOBAL_MAX_BIN_SIZE];
	raw_ = tmp;
	memset(raw_, 0, GLOBAL_MAX_BIN_SIZE);
	
	if (0 > stat(get_cache_path().c_str(), &cache_stat))
		throw E2promException("Failed to read eeprom cache @ "+get_cache_path());

	ifs.read(raw_, GLOBAL_MAX_BIN_SIZE);//cache_stat.st_size);
	ifs.close();
}

/* start to decode */
void UnpackTypl::start()
{
	try {
		if (!raw_)	return;
		decres = read_block(raw_, xconf, ofs);
	} catch (std::exception &e){
		E2promErrMsg(e.what());
	}
}
#if 0
void UnpackTypl::clean_buffer()
{
	if (raw_) {
		delete [] raw_;
		raw_ = 0;
	}
}
#endif
strvec_t UnpackTypl::get_user_string(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_string, uh);
	throw_user_not_exist(id, name);
}

int8vec_t UnpackTypl::get_user_int8(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_int8, uh);
	throw_user_not_exist(id, name);
}

uint8vec_t UnpackTypl::get_user_uint8(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_uint8, uh);
	throw_user_not_exist(id, name);
}

int16vec_t UnpackTypl::get_user_int16(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_int16, uh);
	throw_user_not_exist(id, name);
}

uint16vec_t UnpackTypl::get_user_uint16(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_uint16, uh);
	throw_user_not_exist(id, name);
}

int32vec_t UnpackTypl::get_user_int32(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_int32, uh);
	throw_user_not_exist(id, name);
}

uint32vec_t UnpackTypl::get_user_uint32(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_uint32, uh);
	throw_user_not_exist(id, name);
}

int64vec_t UnpackTypl::get_user_int64(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_int64, uh);
	throw_user_not_exist(id, name);
}

uint64vec_t UnpackTypl::get_user_uint64(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_uint64, uh);
	throw_user_not_exist(id, name);
}

floatvec_t UnpackTypl::get_user_float(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_float, uh);
	throw_user_not_exist(id, name);
}

doublevec_t UnpackTypl::get_user_double(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_double, uh);
	throw_user_not_exist(id, name);
}

ldoublevec_t UnpackTypl::get_user_ldouble(uint16_t id, std::string name)
{
	UserHeader uh(id, name);

	return_data_if_exist(decres.all_user_ldouble, uh);
	throw_user_not_exist(id, name);
}
#if 0
HeaderBlock UnpackTypl::get_header()
{
	return_data_if_size(current_header);
	throw_checksum_error("Header block");
}

ModificationBlock UnpackTypl::get_modification()
{
	return_data_if_size(current_modifi);
	throw_checksum_error("Modification block");
}

RepairBlock UnpackTypl::get_repair()
{
	return_data_if_size(current_repair);
	throw_checksum_error("Service block");
}

StatisticsBlock UnpackTypl::get_statisc()
{
	return_data_if_size(current_statisc);
	throw_checksum_error("Statics block");
}
#endif
/* for multi headers */
std::string UnpackTypl::get_header_s(std::string name, std::string entry)
{
	if (decres.all_header.count(name))
		if (decres.all_header[name].count(entry))
			return decres.all_header[name][entry];
	return std::string();
}

/* for unique header */
std::string UnpackTypl::get_header_s(std::string entry)
{
	for (auto &x : decres.all_header)
		return decres.all_header[x.first][entry];
	return std::string();
}

#if 0
std::string UnpackTypl::get_all_value_of_user(std::string uid)
{
	if (decres.all_user.count(uid))
		return decres.all_user[uid];
	return std::string();
}

std::vector<std::string> UnpackTypl::list_all_header()
{
	std::vector<std::string> ret;
	for (auto &x : decres.all_header)
		ret.push_back(x.first);
	return ret;
}

std::vector<std::string> UnpackTypl::list_all_user()
{
	std::vector<std::string> ret;
	for (auto &x : decres.all_user)
		ret.push_back(x.first);
	return ret;
}
#endif
/* get all user name with the same @userid #_-!! */
std::vector<std::string> UnpackTypl::list_all_user(uint16_t userid)
{
	std::vector<std::string> ret;
//		if (decres.all_user.count(userid))
//			return decres.all_user[userid];
	for (auto &x : decres.all_user)
		if (userid == std::get<0>(x))
			ret.push_back(std::get<1>(x));
	return ret;
}
