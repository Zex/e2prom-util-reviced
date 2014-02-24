/* 
 * block.hpp
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
#ifndef UTILS_BLOCK_HPP_
#define UTILS_BLOCK_HPP_

#include <fcntl.h>
#include <unistd.h>

#ifndef MinGW
#include <sys/sendfile.h>
#endif

#include <fstream>
#include <tuple>
#include <string.h>
#include <map>
#include "checksum.hpp"

ns_begin(e2prom)

#define MAX_HEADNAME_LEN 13                           /* max length of name in header block */
#define MAX_RECENT_SVC_NR 10						  /* max number of recent service */

#define IS_HEADER_BLOCK(b) (1 < b.ID && 100 > b.ID)		/* 2..99 */
#define IS_MODIFICATION_BLOCK(b) (99 < b.ID && 200 > b.ID)/* 100..199 */
#define IS_REPAIR_BLOCK(b) (199 < b.ID && 300 > b.ID)		/* 200..299 */
#define IS_STATISTICS_BLOCK(b) (299 < b.ID && 400 > b.ID)	/* 300..399 */
#define IS_USER_BLOCK(b) (9999 < b.ID && 32768 > b.ID)	/* 10000..32767 */
#define IS_END_BLOCK(b) (b.ID == 0xffff)

extern uint32_t GLOBAL_ASCII_WIDTH;
extern uint32_t GLOBAL_BLOCK_GAP;
extern size_t GLOBAL_MAX_BIN_SIZE;
extern const char* GLOBAL_TEMP_FILE;

typedef std::map<std::string, std::string> eeprom_t;
/* (userid, username) */
typedef std::tuple<uint16_t, std::string> UserHeader;

enum ID_PREFIX
{
	IDENT = 1,
	MODIFICATION,
    SERVICE,
	STATISTICS,
	USER,
	END,
};

enum HeaderTag
{
	ALIGNMENT = 1,
	EEPROMSIZE,
	PARTNUMBER,
	HWCODE,
	PRODUCTINDEX,
	SN,
	PRODUCTDATE,
	READCODE,
	TESTINSTRUCTION,
	NAME,
};

enum UserTag
{
	FORMAT = 1,
};

enum RepairTag
{
	DEPARTMENT = 1,
	SERVICEDATE,
	OPHOURS,
	REPAIRNUMBER,
	SERVICECODE,
};

enum
{
	NONE, // 0
	UINT8, INT8, // 1, 2
	UINT16, INT16, // 3, 4
	UINT32, INT32, // 5, 6
	UINT64, INT64, // 7, 8
	FLOAT, DOUBLE, LONG_DOUBLE, // 9 .. 11
// FIX16D1, FIX16D2, FIX16D3, FIX16D4,  // 12 .. 15
// FIX32D1, FIX32D2, FIX32D3, FIX32D4,  // 16 .. 19
// CHAR, CHAR5, CHAR10, CHAR15, CHAR20, // 20 .. 24
	STRING								 //25
};

struct encode_result
{
/* do we reach the end ??*/
bool end_reached;
/* line  number of under-processed line */
int line_nr;
/* line number of under-process line blockwise */
int line_in_blk;
/* the nr-th parameter we are using for current template */
int para_nr;
uint8_t blocks_mark;
std::string eeprom_bin_str;

uint8_t GLOBAL_ALIGNMENT;		/* bits */
uint32_t GLOBAL_EEPROM_SIZE; /* bytes */
bool THE_FIRST_ALIGNMENT_IS_READ;
bool THE_FIRST_EEPROMSIZE_IS_READ;
uint16_t GLOBAL_PAYLOAD_SIZE;

encode_result();

};

struct decode_result
{
/* number of read byte */
uint32_t byte_nr;
/* do we reach the end ??*/
bool end_reached;
uint8_t GLOBAL_ALIGNMENT;		/* bits */
uint32_t GLOBAL_EEPROM_SIZE; /* bytes */
bool THE_FIRST_ALIGNMENT_IS_READ;
bool THE_FIRST_EEPROMSIZE_IS_READ;

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

decode_result();
void clear_all();
void clear_cur_user();
void set_all_user(uint16_t uid, std::string username);
};

template <typename BLOCK_T>
struct Block
{
	uint16_t ID;

	friend std::ostream& operator<< (std::ostream &o, BLOCK_T &b)
	{
		return o.write(reinterpret_cast<char*>(&b), sizeof(BLOCK_T));
	}
	friend std::istream& operator>> (std::istream &o, BLOCK_T &b)
	{
		return o.read(reinterpret_cast<char*>(&b), sizeof(BLOCK_T));
	}
	bool operator== (BLOCK_T &b) const
	{
		return (ID == b.ID);
	}
	bool operator< (const BLOCK_T &b) const
	{
		return (ID < b.ID);
	}
};

/*
 * This method shall be called after the rest of fields in
 * this block are initialized except the checksum field.
 */
template <typename BLOCK_T>
void set_checksum(BLOCK_T &b)
{
	b.checksum = crc16(reinterpret_cast<uint8_t*>(&b), b.blocksize);
}

/*
 * DIB-CP1_SAD says block 1, 3 will be used in DIB-R5 project,
 * block 4 might also be chosen.
 *
 * 1.	Identification block
 * 2.	Statistics block
 * 3.	Service block
 * 4.	User specific block
 */

/*
 * Mandatory
 */
struct HeaderBlock : public Block<HeaderBlock>
{
	/* ID: IDENT_[2..99]*/
	/* uint16_t ID; */
	/* omitted items in ascii file */
	uint16_t blocksize;
	uint16_t checksum;
	/* end of omitted items in ascii file */
	uint16_t ALIGNMENT;
	/* size of the e2prom data in bytes */
	uint32_t EEPROMSIZE;
	uint32_t PARTNUMBER;  /* for xxxx.yyyy */
	uint8_t PARTNUMBER_RESV; /* reserved, for the zz in xxxx.yyyy.zz */
	uint8_t HWCODE;
	uint16_t PRODUCTINDEX;
	uint32_t SN;	/* serial number */
	uint16_t PRODUCTDATE;
	uint16_t READCODE;
	uint16_t TESTINSTRUCTION;
	char NAME[MAX_HEADNAME_LEN+1]; /* 13characters + '\0' */

	HeaderBlock(uint16_t id = 0)
	: checksum(0),
	PARTNUMBER_RESV(0)
	{
		ID = id;
		blocksize = sizeof(HeaderBlock);
	}
};// __attribute__((aligned (global_ali)));

#define GLOBAL_HEADER_BLK_SIZE sizeof(HeaderBlock)

/*
 * Mandatory
 */
struct ModificationBlock : public Block<ModificationBlock>
{
	/* ID: MODIFICATION_[100..199]*/
	/* uint16_t ID; */
	/* omitted items in ascii file */
	uint16_t blocksize;
	uint16_t checksum;
	uint16_t resv; /* (0x0000) */
	/* end of omitted items in ascii file */
	uint16_t SERVICECODE[10];

	ModificationBlock(uint16_t id = 0)
	: checksum(0), resv(0)
	{
		ID = id;
		blocksize = sizeof(ModificationBlock);
		memset((char*)SERVICECODE, 0, 10*sizeof(uint16_t));
	}
};

/* Optional */
struct RepairBlock : public Block<RepairBlock>
{
	/* ID: SERVICE_[200..299]*/
	/* uint16_t ID; */
	/* omitted items in ascii file */
	uint16_t blocksize;
	uint16_t checksum;
	uint16_t resv; /* (0x0000) */
	/* end of omitted items in ascii file */
	int8_t DEPARTMENT[6];
	uint16_t SERVICEDATE;
	/*
	 * OPHOURS = (cycle_counts * (4 cycles per day)) << 16 + ophour_counts
	 *
	 * Ex:
	 * 	 1301 hours of operating time and 798 times switch
	 * 	 ==> 798/1301(ascii)
	 * 	 ==> (798*4)<<16 + 1301
	 * 	 ==> 0x1505780c
	 */
	uint32_t OPHOURS;
	uint16_t REPAIRNUMBER;
	/* 1..21 */
	uint16_t SERVICECODE;

	RepairBlock(uint16_t id = 0)
	: checksum(0), resv(0)
	{
		ID = id;
		blocksize = sizeof(RepairBlock);
	}
};

/* Optional */
struct StatisticsBlock : Block<StatisticsBlock>
{
	/* ID: STATISTICS_[300..399]*/
	/* uint16_t ID; */
	/* omitted items in ascii file */
	uint16_t blocksize;
	uint16_t checksum;
	uint16_t resv; /* (0x0000) */
	/* end of omitted items in ascii file */
	uint32_t OPHOURS;	/* x/y ==> xy*/

	StatisticsBlock(uint16_t id = 0)
	: checksum(0), resv(0)
	{
		ID = id;
		blocksize = sizeof(StatisticsBlock);
	}
};

/*
 * FORMAT node within user block
 *
 * An user block might contents multiple format nodes
 *
 * Ex:
 *    column = 4
 *    row =5 
 *
 *    +====+===================================+    +........................+  
 *    |    | int16  | float  | uint32 |  char  |<---: Row Description, share :
 *    +====+===================================+    : the same type.         :
 *    |row1| data1  | data2  | data3  | data4  |    +........................+
 *    +    +-----------------------------------+
 *    |row2| data5  | data6  | data7  | data8  |
 *    +    +-----------------------------------+
 *    |row3| data9  | data10 | data11 | data12 |
 *    +    +-----------------------------------+
 *    |row4| data13 | data14 | data15 | data16 |
 *    +    +-----------------------------------+
 *    |row5| data17 | data18 | data19 | data20 |
 *    +====+-----------------------------------+
 *      ^                                      A
 *      |                                       \
 *      |                                        \
 *  +...............................+         +..............................+
 *  : Column Description, share the :         : Payload, share the same data :
 *  : same type.                    :         : type within the same column. :
 *  +...............................+         +..............................+
 *
 */
template<typename DATA_T>
struct UserFormatEntry// : public Block<UserFormatEntry<DATA_T>>
{
	DATA_T value;
	
	UserFormatEntry(DATA_T v) : value(v) {}
	UserFormatEntry() {}

friend std::ostream& operator<< (std::ostream &o, UserFormatEntry<DATA_T> &b)
{
	return o.write(reinterpret_cast<char*>(&b), sizeof(DATA_T));
}

friend std::istream& operator>> (std::istream &o, UserFormatEntry<DATA_T> &b)
{
	return o.read(reinterpret_cast<char*>(&b.value), sizeof(DATA_T));
}

};

/*
 * Optional
 *
 * Blocksize(USER_xxx) = Size(ID) + Size(Blocksize) + Size(CRC16) + Size(Data)
 *
 * Ex:
 *  
 *  NOTICE: FORMATn(x, y) means the nth. FORMAT contains x number of type y data 
 *
 *  +----------+---------+---------+-----------------+
 *  |2         |2        |2        |6*1 = 6          |
 *  +----------+---------+---------+-----------------+
 *  |UINT16(2) |UINT16(2)|UINT16(2)|6*UINT8(1)       |
 *  +----------+---------+---------+-----------------+
 * 	|USER_18030|Blocksize|CRC16    |FORMAT1(6, UINT8)|
 *  +----------+---------+---------+-----------------+
 *
 *  Blocksize(USER_18030) = 2 + 2 + 2 + 6 = 12 = 0x0c
 *
 *  +----------+---------+---------+------------------+------------------+------------------+----------------------------------------+
 *  |2         |2        |2        |3*4 = 12          |2*1 = 2           |4*2 = 8           |1*14 = 14                               |
 *  +----------+---------+---------+------------------+------------------+------------------+----------------------------------------+
 *  |UINT16(2) |UINT16(2)|UINT16(2)|3*UINT32(4)       |2*UINT8(1)	     |4*UINT16(2)       |Size("where is it ?") = 14(one for '\0')|
 *  +----------+---------+---------+------------------+------------------+------------------+----------------------------------------+
 * 	|USER_18501|Blocksize|CRC16    |FORMAT1(3, UINT32)|FORMAT2(2, UINT8) |FORMAT3(4, UINT16)|FORMAT4(1, STRING)                      |
 *  +----------+---------+---------+------------------+------------------+------------------+----------------------------------------+
 *
 *  Blocksize(USER_18030) = 2 + 2 + 2 + 12 + 2 + 8 + 14 = 42 = 0x2a
 */
struct UserBlockHeader : public Block<UserBlockHeader>
{
	/* ID: USER_[10000..32767]*/
	/* uint16_t ID; */
	/* omitted items in ascii file */
	uint16_t blocksize;
	uint16_t checksum;
	/* end of omitted items in ascii file */

	/* A user block requires at least one format node */
	void set_checksum(std::string &fmstr, encode_result &encres);
	UserBlockHeader(uint16_t id = 0);
	void write_bin(std::ostream &o, encode_result &encres);
	
};

struct EndBlock : public Block<EndBlock>
{
	/* ID: END[0xffff]*/
	/* uint16_t ID; */
	/* omitted items in ascii file */
	//uint16_t resv; /* (0x0000) */
	/* end of omitted items in ascii file */
	EndBlock() { ID = 0xffff; }

	void write_payload_size(encode_result &encres);
	//friend std::ostream& operator<< (std::ostream &o, EndBlock &b)
	void write_bin(std::ostream &o, encode_result &encres);

};

extern void del_temp_file();
extern void transfer_eeprom_bin(const char* dest, encode_result &encres);

// 2 bytes for end mark
//#define GLOBAL_MANDOTARY_BLK_SIZE (GLOBAL_HEADER_BLK_SIZE+sizeof(ModificationBlock)+2)

#define safe_set_all(all, cur, uid, username)                  \
{                                                              \
/* buf.push_back(val);                   */                    \
/*  UserHeader uh(uid, username)   */                          \
/*  all_user_allval += get_hex_str<DATA_T>(val);*/             \
 if (all.count(UserHeader(uid, username))) {                   \
  all[UserHeader(uid, username)].insert(                       \
   all[UserHeader(uid, username)].end(), cur.begin(), cur.end()); \
 } else { all[UserHeader(uid, username)] = cur; }              \
}

/*
 * Ex:
 *
 * +--------+----------+----------+-------+--->
 * |uheader1|column1   |column2   |column2|
 * |        |<uint16_t>|<uint32_t>|<float>|...
 * +--------+---------------------+-------+--->
 * +--------+----------+----------+----------+--->
 * |uheader2|column1   |column2   |column2   |
 * |        |<int8_t>  |<double>  |<uint32_t>|...
 * +--------+---------------------+----------+--->
 *        :::                     :::
 *        :::                     :::
 */
// struct UserBlock
template<typename DATA_T>
struct UserFormatColumn// : public UserFormatEntry<DATA_T>
{
	typedef UserFormatEntry<DATA_T> entry_t;

	/* All format in the same user block share one user block header */
	UserBlockHeader &uheader;
	/* All entry in one column share the same data type */
	std::vector<entry_t> col;
	
//	size_t written_by; /* byte */

	UserFormatColumn<DATA_T>(UserBlockHeader &uh)
	: uheader(uh)//, written_by(0)
	{}

	UserFormatColumn operator+=(DATA_T d)
	{
		entry_t e;
		e.value = d;
		/* append a new column entry for user block */
		col.push_back(e);
		/* increase size of user block */
		uheader.blocksize += sizeof(DATA_T);

		return *this;
	}

	bool in_same_block(UserFormatColumn &b)
	{
		return (uheader == b.uheader);
	}

friend std::ostream& operator<< (std::ostream &o, UserFormatColumn<DATA_T> &b)
//void write_bin(std::ostream &o)
{
// Notes: the header of each block needs to be parsed before all formats
// 	within this block.
//	uint8_t fix_by = 0;

	for (auto &e : b.col) {
//		 o << e;
		o.write(reinterpret_cast<char*>(&e), sizeof(DATA_T));
//		 b.written_by += sizeof(DATA_T);
	}

	return o;
}

friend std::istream& operator>> (std::istream &ism, UserFormatColumn<DATA_T> &b)
{
// Notes: the header of each block needs to be parsed before all formats
// 	within this block.
	char* raw = new char[sizeof(DATA_T)];
	entry_t e;
	
	while (ism.read(raw, sizeof(DATA_T))) {

		DATA_T val;
		memcpy(&val, raw, sizeof(DATA_T));

		e.value = val;
		b.col.push_back(e);
	}
	
	delete raw;
	return ism;
}
	
void write_ascii(std::string &buffer, std::ostream &o, decode_result &decres)
{
	std::stringstream ss(buffer);
	ss >> *this;
	
	for (auto &x : col) {
		if (1 == sizeof(DATA_T)) {
write_file(o, ((*reinterpret_cast<uint16_t*>(&x.value)) & 0x00ff) << '\n');
		} else {
			write_file(o, x.value << '\n');
		}
		set_current(x.value, decres);
	}
}
#define safe_set_current(cur, buf, val) buf.push_back(val) 
#if 0
#define safe_set_all(all, cur, uid, username) \
{                                        \
/* buf.push_back(val);                   */ \
 UserHeader uh(uid, username)    \
/*  all_user_allval += get_hex_str<DATA_T>(val);*/ \
 if (all.count(uh)) {                   \
  all[uh].insert(                       \
   all[uh].end(), cur.begin(), cur.end()); \
 } else { all[uh] = cur; }              \
}
#endif
#if 0
#define safe_set_all_user(cur) \
{                              \
 std::string t =               \
 std::string(strof(USER))+"_"+to_string<uint16_t>(cur[uheader].ID); \
/* 	all_user[t] = all_user_allval; */\
}
#endif
void set_current(DATA_T val, decode_result &decres)
{
	if (IS_INT8(DATA_T)) {
		safe_set_current(decres.all_user_int8, decres.cur_user_int8, val);
	} else if (IS_UINT8(DATA_T)) {
		safe_set_current(decres.all_user_uint8, decres.cur_user_uint8, val);
	} else if (IS_INT16(DATA_T)) {
		safe_set_current(decres.all_user_int16, decres.cur_user_int16, val);
	} else if (IS_UINT16(DATA_T)) {
		safe_set_current(decres.all_user_uint16, decres.cur_user_uint16, val);
	} else if (IS_INT32(DATA_T)) {
		safe_set_current(decres.all_user_int32, decres.cur_user_int32, val);
	} else if (IS_UINT32(DATA_T)) {
		safe_set_current(decres.all_user_uint32, decres.cur_user_uint32, val);
	} else if (IS_INT64(DATA_T)) {
		safe_set_current(decres.all_user_int64, decres.cur_user_int64, val);
	} else if (IS_UINT64(DATA_T)) {
		safe_set_current(decres.all_user_uint64, decres.cur_user_uint64, val);
	} else if (IS_DOUBLE(DATA_T)) {
		safe_set_current(decres.all_user_double, decres.cur_user_double, val);
	} else if (IS_FLOAT(DATA_T)) {
		safe_set_current(decres.all_user_float, decres.cur_user_float, val);
	} else if (IS_LDOUBLE(DATA_T)) {
		safe_set_current(decres.all_user_ldouble, decres.cur_user_ldouble, val);
	} else ;
}
};

extern void write_bin_ufc(std::ofstream &o, std::string &buffer, encode_result &encres);
/*
 * Ex:
 *
 *  UserBlockHeader uh;
 * 	UserFormatColumn<uint16_t> ufc1(uh);
 * 	UserFormatColumn<float> ufc2(uh);
 * 	UserFormatColumn<char> ufc3(uh);
 *
 *  std::string fmstr = strize_formats<uint16_t, float, char>(ufc1, ufc2, ufc3);
 *
 *	uh.set_checksum(fmstr);
 *
 *  @return		raw string of FORMATS
 * TODO: iterate all member in formats;
 */
template<typename ...MULTI_DATA_T>
std::string strize_formats(UserFormatColumn<MULTI_DATA_T> &...multi_col)
{
	typedef std::tuple<UserFormatColumn<MULTI_DATA_T>...> FORMATS;
	FORMATS formats(multi_col...);
	std::stringstream o;
		
	o << std::get<0>(formats);
// 	const unsigned int max_size = std::tuple_size<decltype(formats)>::value;

	return o.str();
}

/*
 * | 8   | 4     | 4
 * | year| month | day
 *
 * date = (year*2^9) + (month*2^5 + day)
 */
extern uint16_t encode_date(std::string &buffer);
extern std::string decode_date(uint16_t date);
extern std::string decode_serial_number(uint32_t sn);
extern std::string decode_dotted_uint16(uint16_t pi);
extern std::string decode_partnumber(uint32_t pn, uint8_t pn_resv);
extern std::string decode_ophours(uint32_t ophours);

template<typename BLOCK_T>
bool is_checksum16_correct(BLOCK_T &b, std::string data)
{
	/* omit the checksum itself */
	data[4] = 0;
	data[5] = 0;

	return (checksum16_match(
		(reinterpret_cast<uint8_t*>(const_cast<char*>(data.c_str()))),
		b.blocksize , b.checksum));
}

template<class BLOCK_T, class VAL_T>
void set_member(BLOCK_T &obj, VAL_T (BLOCK_T::*member), std::string &buffer)
{
	(obj.*member) = to_digits<VAL_T>(buffer);
}
	
extern void write_bin_str(std::string &buffer, encode_result &encres);

ns_end(e2prom)

#endif /* UTILS_BLOCK_HPP_ */
