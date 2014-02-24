/* 
 * bin-tool.cpp
 *
 * Tools to read binary-format file
 *
 * Author: Zex <zex@hytera.com>
 */
#ifndef TOOLS_BIN_TOOL_HPP_
#define TOOLS_BIN_TOOL_HPP_

#include "block-tool.hpp"

ns_begin(e2prom)

/* 
 * Structure of FormatTypes
 * +-------------------------------------------------->
 * |0            |1   |2      |3      |4     |n      |n+1 ...
 * +-------------------------------------------------->
 * |name of block|rows|columns|type id|length|type id|length ...
 * +-------------------------------------------------->
 *
 *  Structure of UserBlkTps
 * +---------------------+
 * |user id1|FormatTypes1|
 * +---------------------+
 * |user id2|FormatTypes2|
 * +---------------------+
 * |user id3|FormatTypes3|
 * +---------------------+
 * :  :::   :    :::     :
 * V        V            V
 */
typedef std::vector<std::string> FormatTypes;
typedef std::map<uint16_t, FormatTypes> UserBlkTps;

extern UserBlkTps global_xtype;

extern FormatTypes get_types(const char *argv[]);

extern UserBlkTps read_xtype(std::ifstream &ifs_conf);
extern uint16_t get_type_id(std::string &buffer);
extern uint16_t get_id(char *raw, size_t nr);
//bool has_endmark_(char *raw, size_t esize = GLOBAL_MAX_BIN_SIZE);
//extern bool has_endmark(const char *raw, size_t esize = GLOBAL_MAX_BIN_SIZE);
extern void check_end_mark(char *raw, decode_result &decres);

/* operation dispater for binary-format file, entrance
 * of decoding from raw string */
extern void dealing_bin(std::ifstream &ifs_bin, std::ifstream &ifs_conf, std::ofstream &ofs);

/* operations for binary-format file */
//extern uint16_t read_id(std::string &line);
//extern void read_block(std::string &line, std::ifstream &ifs_conf, std::ofstream &ofs);
//extern void read_IDENT(std::string &line, std::ofstream &ofs);
//extern void read_MODIFICATION(std::string &line, std::ofstream &ofs);
//extern void read_STATISTICS(std::string &line, std::ofstream &ofs);
//extern void read_USER(std::string &line, UserBlkTps &types, std::ofstream &ofs);
//extern void read_SERVICE(std::string &line, std::ofstream &ofs);
//extern void read_END(std::string &line, std::ofstream &ofs);
extern uint16_t read_id(char *raw, decode_result &decres);
extern decode_result read_block(char *raw, std::ifstream &ifs_conf, std::ofstream &ofs);
extern void read_IDENT(char *raw, std::ofstream &ofs, decode_result &decres);
extern void read_MODIFICATION(char *raw, std::ofstream &ofs, decode_result &decres);
extern void read_STATISTICS(char *raw, std::ofstream &ofs, decode_result &decres);
extern void read_USER(char *raw/*, UserBlkTps &types*/, std::ofstream &ofs, decode_result &decres);
extern void read_SERVICE(char *raw, std::ofstream &ofs, decode_result &decres);
extern void read_END(char *raw, std::ofstream &ofs, decode_result &decres);
/* end of operations for binary-format file */
	
ns_end(e2prom)

#endif /* TOOLS_BIN_TOOL_HPP_ */
