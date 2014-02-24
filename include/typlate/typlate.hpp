/* 
 * typlate/typlate.hpp
 *
 * Interfaces to access e2prom-util library for DSW
 *
 * Author: Zex <zex@hytera.com>
 *
 *
 * See also:
 *  the E2prom Data Modules
 *  DIB-CP1 SAD
 */
#ifndef TYPLATE_HPP_
#define TYPLATE_HPP_

#include <time.h>
#include "../tools/ascii-tool.hpp"
#include "xtype.hpp"

use_ns(e2prom)

#define TYPEPLATE_BASE 0x0
#define TYPEPLATE_MAX_OFFSET 0x0400
#define TYPEPLATE_MAX_LENGTH 0x0400
#define TYPEPLATE_IS_VALID(o,l) \
	((o+l) < TYPEPLATE_MAX_OFFSET)
#define TYPEPLATE_MAX_DATA_PER_PACK 32*7

extern const char* DEFAULT_CACHE_DIR;

struct PackTypl
{
	uint16_t offset_;
	uint16_t len_;
	std::string data_;
	
	PackTypl(int size_to_read = 0)
	: offset_(TYPEPLATE_BASE)
	{
		init(size_to_read);
	}

	PackTypl(const char *path, bool is_bin_file = true)
	: offset_(TYPEPLATE_BASE)
	{
		if (is_bin_file)
			read_bin_file(path);
//		else /* read ascii file */
//			read_ascii_file(path);
	}

	void init(int size_to_read);
	std::vector<std::string> start();

	friend std::ostream& operator<< (std::ostream &o, PackTypl &b)
	{
		const char *buf(b.data_.data());

		o.write(reinterpret_cast<char*>(&b),
			(sizeof(b.offset_)+sizeof(b.len_)));
		o.write(const_cast<char*>(buf), b.data_.size());
		
		return o;
	}
protected:	
	void read_bin_file(const char *path);
//	void read_ascii_file(const char *path);
};
#if 0
#define return_data_if_size(cur) \
	if (cur.blocksize) return cur;
#endif
#define return_data_if_exist(cur, uh) \
	if (cur.count(uh)) return cur[uh];

#define	throw_user_not_exist(id, name)        \
	throw E2promValueException(               \
		"USER_"+to_string<uint16_t>(id)+" with subcomponent "+name+" doesn't exist", \
		to_string<int>(__LINE__));

#define throw_checksum_error(blk)	\
	throw E2promValueException(		\
		blk+std::string(" ")+std::string(CRC16_INCORRECT), \
		to_string<int>(__LINE__));

/*
 * Unpack a complete typeplate buffer
 * 
 * Result:
 * HeaderBlock
 * ModificationBlock
 * StatisticsBlock
 * RepairBlock
 * { UserBlock1, UserBlock2, UserBlock3 ... }
 */
struct UnpackTypl
{
private:
	char* raw_;//[GLOBAL_MAX_BIN_SIZE];
	std::string entry_;
	std::ifstream xconf;
	std::ofstream ofs;
	decode_result decres;
protected:	
	std::string get_cache_path()
	{
		return (std::string(DEFAULT_CACHE_DIR) + "/" + entry_ + BFILE_SURFIX);
	}
public:
	/* read from byte stream */
	UnpackTypl(const char* buf)
	: raw_(const_cast<char*>(buf))
	{
//		memcpy(raw_, buf, GLOBAL_MAX_BIN_SIZE);
		init_xtype(global_xtype);
	}

	/* read from cache */
	UnpackTypl(const std::string &entry)
	: entry_(entry)
	{
		read_cache(entry_);
		init_xtype(global_xtype);
	}

	void read_cache();

	void read_cache(std::string &buf)
	{
		read_cache();
		buf = raw_;
	}

	~UnpackTypl()
	{
		std::cout << __PRETTY_FUNCTION__ << '\n';
	}

	void update_cache(std::string entry, size_t esize = GLOBAL_MAX_BIN_SIZE);
	
	/* start to decode */
	void start();

//	void clean_buffer();

	strvec_t get_user_string(uint16_t id, std::string name);
	
	int8vec_t get_user_int8(uint16_t id, std::string name);
	
	uint8vec_t get_user_uint8(uint16_t id, std::string name);
	
	int16vec_t get_user_int16(uint16_t id, std::string name);
	
	uint16vec_t get_user_uint16(uint16_t id, std::string name);
	
	int32vec_t get_user_int32(uint16_t id, std::string name);
	
	uint32vec_t get_user_uint32(uint16_t id, std::string name);
	
	int64vec_t get_user_int64(uint16_t id, std::string name);
	
	uint64vec_t get_user_uint64(uint16_t id, std::string name);
	
	floatvec_t get_user_float(uint16_t id, std::string name);
	
	doublevec_t get_user_double(uint16_t id, std::string name);
	
	ldoublevec_t get_user_ldouble(uint16_t id, std::string name);
#if 0
	HeaderBlock get_header()
	{
		return_data_if_size(current_header);
		throw_checksum_error("Header block");
	}

	ModificationBlock get_modification()
	{
		return_data_if_size(current_modifi);
		throw_checksum_error("Modification block");
	}

	RepairBlock get_repair()
	{
		return_data_if_size(current_repair);
		throw_checksum_error("Service block");
	}

	StatisticsBlock get_statisc()
	{
		return_data_if_size(current_statisc);
		throw_checksum_error("Statics block");
	}
#endif
	/* for multi headers */
	std::string get_header_s(std::string name, std::string entry);
	/* for unique header */
	std::string get_header_s(std::string entry);

#if 0
	std::string get_all_value_of_user(std::string uid)
	{
		if (decres.all_user.count(uid))
			return decres.all_user[uid];
		return std::string();
	}

	std::vector<std::string> list_decres.all_header()
	{
		std::vector<std::string> ret;
		for (auto &x : decres.all_header)
			ret.push_back(x.first);
		return ret;
	}
	
	std::vector<std::string> list_all_user()
	{
		std::vector<std::string> ret;
		for (auto &x : decres.all_user)
			ret.push_back(x.first);
		return ret;
	}
#endif
	/* get all user name with the same @userid #_-!! */
	std::vector<std::string> list_all_user(uint16_t userid);

};

#endif /* TYPLATE_HPP_ */
