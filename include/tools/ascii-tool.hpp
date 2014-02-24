/* 
 * ascii-tool.hpp
 *
 * Tools to parse ASCII-format file
 *
 * Author: Zex <zex@hytera.com>
 * 
 * A sample ASCII-format file looks like
 *
 * +-------------------------+
 * |# this is comments       |
 * |# this is comments       |
 * |                         |
 * |tag:        element      |
 * |tag:        element      |
 * |                         |
 * |# this is comments       |
 * |tag:        element      |
 * |                         |
 * |:::          :::         |
 * |                         |
 * +-------------------------+
 * We need to parse the tag to learn about what
 * we are dealing with, then the element, line
 * by line util the end of the whole file. Each
 * block is identified by the element of ID tag.
 *
 */
#ifndef TOOLS_ASCII_TOOL_HPP_
#define TOOLS_ASCII_TOOL_HPP_

#include "block-tool.hpp"

ns_begin(e2prom)

#define parse_block_(prefix,ifs,ofs,id, encres, argv) parse_##prefix(ifs,ofs,id,encres,argv)

/* choose a right parser to parse a block */
extern void parse_block(std::ifstream &ifs, std::ofstream &ofs, std::string &buffer, encode_result &encres, char* argv[] = 0);
/* operation dispater for ASCII-format file, entrance
 * of encoding to raw string */
extern encode_result dealing_ascii(std::ifstream &ifs, std::ofstream &ofs, char* argv[] = 0);
/* operations for ASCII-format file */
extern void read_tag(std::string &line, char* c, std::string &tag, encode_result &encres);
extern void parse_STATISTICS(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[] = 0);
extern void parse_END(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[] = 0);
extern void parse_USER(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[] = 0);
extern void parse_IDENT(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[] = 0);
extern void parse_MODIFICATION(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[] = 0);
extern void parse_SERVICE(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[] = 0);
/* end of operations for ASCII-format file */

ns_end(e2prom)

#endif /* TOOLS_ASCII_TOOL_HPP_ */
