/* 
 * basic.hpp
 *
 * Author: Zex <zex@hytera.com>
 */
#ifndef UTILS_BASIC_HPP_
#define UTILS_BASIC_HPP_

#include <sys/stat.h>
#include <vector>
#include <typeinfo>
#include <iomanip>

#ifdef MinGW
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <iostream>
#include <sstream>

#define ns_begin(n) namespace n {
#define ns_end(n) }
#define use_ns(n) using namespace n;

ns_begin(e2prom)

#define IS_UPPER(c) ('A' <= c && 'Z' >= c)	/* A..Z */
#define IS_LOWER(c) ('a' <= c && 'z' >= c)		/* a..z */
#define IS_DIGIT(c) ('0' <= c && '9' >= c)		/* 0..9 */
#define IS_COMMENT_SIGN(c) ('#' == c)			/* # */
#define IS_COLON(c)	(':' == c)				/* : */
#define IS_SPACE(c) (' ' == c)				/* space */
#define IS_TAB(c) 	('\t' == c)				/* tab */
#define IS_EOS(c)	('\0' == c)				/* end of string */
#define IS_ULINE(c)	('_' == c)				/* under line */
#define IS_PERIOD(c) ('.' == c)				/* . */
#define IS_COMMA(c) (',' == c)				/* , */
#define IS_FSLASH(c) ('/' == c)				/* / */
#define IS_BSLASH(c) ('\\' == c)				/* \ */
#define IS_HYPHEN(c) ('-' == c)				/* - */
#define IS_LPARENT(c) ('(' == c)				/* (*/
#define IS_RPARENT(c) (')' == c)				/*) */
#define IS_DQUOTE(c) ('"' == c)				/* " */
#define IS_SQUOTE(c) ('\'' == c)				/* ' */
#define IS_VBAR(c) ('|' == c)					/* | */
#define IS_PRINTABLE(c) (0x20 <= c && 0x7e >= c)	/* non-controling */
#define IS_CR(c)	(0xd == c)				/* carriage return */
#define IS_EOL(c)	('\n' == c || IS_CR(c))	/* end of line */
#define strof(name)	#name
#define IS_COMMENT(l) IS_COMMENT_SIGN(l[0]) /* is this line a comment ? */
#define IS_BLANK(l) (line.empty() || IS_CR(line[0]))  /* is this line blank ? */
#define IS_LETTER(c) (IS_UPPER(c) || IS_LOWER(c))
#define IS_PLACEHOLD(c) ('<' == c)

#define TO_BYTE(x) (*reinterpret_cast<uint8_t*>(x))
#define digitize(c) (c-'0')
#define NETWORK_ENDIAN_32(e) (htonl(e))
#define NETWORK_ENDIAN_16(e) (htons(e))
#define HOST_ENDIAN_32(e)	(ntonl(e))
#define HOST_ENDIAN_16(e)	(ntons(e))
#define BASE_YEAR 1980

#define IS_INT64(d)		(typeid(d) == typeid(int64_t))
#define IS_UINT64(d)	(typeid(d) == typeid(uint64_t))
#define IS_UINT32(d)	(typeid(d) == typeid(uint32_t))
#define IS_INT32(d)		(typeid(d) == typeid(int32_t))
#define IS_UINT16(d)	(typeid(d) == typeid(uint16_t))
#define IS_INT16(d)		(typeid(d) == typeid(int16_t))
#define IS_UINT8(d)		(typeid(d) == typeid(uint8_t))
#define IS_INT8(d)		(typeid(d) == typeid(int8_t))
#define IS_CHARSTR(d)	(typeid(d) == typeid(char*))
#define IS_UINT8STR(d)	(typeid(d) == typeid(uint8_t*))
#define IS_STRING(d)	(typeid(d) == typeid(std::string))
#define IS_LDOUBLE(d)	(typeid(d) == typeid(long double))
#define IS_DOUBLE(d)	(typeid(d) == typeid(double))
#define IS_FLOAT(d)		(typeid(d) == typeid(float))
#define IS_UCHAR(d)		(typeid(d) == typeid(unsigned char))
#define IS_CHAR(d)		(typeid(d) == typeid(char))

#define ALIGN_16 16 /* bytes */
#define ALIGN_32 32 /* bytes */

extern bool WRITE_FILE;
extern bool TEMPLATE_MODE;

#define write_file(o, d) if (WRITE_FILE) o << d
#define write_str(d, eeprom_bin_str)               \
	{                              \
		std::stringstream s;       \
		s.write(reinterpret_cast<char*>(&d), sizeof(d));                    \
		eeprom_bin_str += s.str(); \
	}

typedef std::vector<std::string> strvec_t;
typedef std::vector<int8_t> int8vec_t;
typedef std::vector<uint8_t> uint8vec_t;
typedef std::vector<int16_t> int16vec_t;
typedef std::vector<uint16_t> uint16vec_t;
typedef std::vector<int32_t> int32vec_t;
typedef std::vector<uint32_t> uint32vec_t;
typedef std::vector<int64_t> int64vec_t;
typedef std::vector<uint64_t> uint64vec_t;
typedef std::vector<float> floatvec_t;
typedef std::vector<double> doublevec_t;
typedef std::vector<long double> ldoublevec_t;

template<typename C_TYPE>
std::string to_string(C_TYPE digis)
{
	std::stringstream ss;
	
	ss << digis;
	return ss.str();
}

template<typename DATA_T>
std::string get_hex_str(DATA_T val)
{
	std::stringstream s;
	s << std::hex << val;
	return s.str();
}

/* split string by delim */
extern std::vector<std::string> split_by(char delim, std::string &str);
extern void fill_front_zero(std::string &str, size_t howmany);

template<typename DATA_T>
DATA_T to_digits(std::string str)
{
	uint64_t ret;
	std::stringstream ss(str);
	
	ss >> ret;
	void *vret = &ret;
	return *(reinterpret_cast<DATA_T*>(vret));
}

extern bool need_fix_align(size_t size_by, size_t alignment);

#if DEBUG
extern void debug_bin(std::string line);
#endif

extern bool path_exist(const char* path);
extern void calc_macaddr(std::string serial_nr, uint8_t macaddr[6]);

ns_end(e2prom)

#endif /* UTILS_BASIC_HPP_ */

