/* 
 * block-tool.hpp
 *
 * Tools to deal with data structures in E2PROM
 *
 * Author: Zex <zex@hytera.com>
 */
#ifndef TOOLS_BLOCK_TOOL_HPP_
#define TOOLS_BLOCK_TOOL_HPP_

#include "../utils/block.hpp"
#include "../utils/exception.hpp"

ns_begin(e2prom)

#define DEFAULT_TYPE_CONF "xtype.ini"
#define DEFAULT_OUTFILE "eeprom"
#define AFILE_SURFIX ".ascii"
#define BFILE_SURFIX ".bin"
#define TXT_SURFIX ".txt"

#define	BREAK_IF_BLANK(line) \
	if (IS_BLANK(line)) { break; }
#define CONTINUE_IF_COMMENT(line)\
	if (IS_COMMENT(line)) { continue; }
	/* line  number of under-processed line */
//	extern int line_nr;
	/* line number of under-process line blockwise */
//	extern int line_in_blk;
	/* number of read byte */
//	extern uint32_t byte_nr;
	/* for checking all mandotory block is included in ascii file */
//	extern int8_t blklist;
	/* do we reach the end ??*/
//	extern bool end_reached;
	/* the nr-th parameter we are using for current template */
//	extern int para_nr;
	/* do we reach the end of binary file ??*/
#define all_bytes_read(decres) (decres.byte_nr >= decres.GLOBAL_EEPROM_SIZE*8)

#define IS_VALID_ID(blkty,id) \
	/* 2..99 */ 	(blkty == IDENT ? (1 < id && 100 > id)\
	/* 100..199 */		: blkty == MODIFICATION ? (99 < id && 200 > id)\
	/* 200..299 */		: blkty == SERVICE ? (199 < id && 300 > id)\
	/* 300..399 */		: blkty == STATISTICS ? (299 < id && 400 > id)\
	/* 10000..32767 */	: blkty == USER ? (9999 < id && 32768 > id)\
	: blkty == END ? (id == 0xffff)\
	: false)
	
#define usage_allowed_tags "Allowed tags:\n"\
	"For header block: [ID|ALIGNMENT|EEPROMSIZE|PARTNUMBER|HWCODE|PRODUCTINDEX|SN|PRODUCTDATE|READCODE|TESTINSTRUCTION|NAME]\n"\
	"For modification block: [ID|SERVICECODE]\n"\
	"For user block: [ID|FORMAT]\n"\
	"For statistic block: [ID|OPHOURS]\n"\
	"For end block: [ID]\n"

#define usage_allowed_user_format_type \
	"Allowed types of format column:\n"\
	"UINT8,\tINT8,\tUINT16,\tINT16\n"\
	"UINT32,\tINT32,\tUINT64,\tINT64\n"\
	"FLOAT,\tDOUBLE,\tLONG_DOUBLE\nSTRING\n"
#if 0
	"NONE, FIX16D1,\tFIX16D2,\tFIX16D3,\tFIX16D4\n"\
	"FIX32D1,\tFIX32D2,\tFIX32D3,\tFIX32D4\n"\
	"CHAR,\tCHAR5,\tCHAR10,\tCHAR15,\tCHAR20,\tSTRING"
#endif

#define usage_allowed_id_prefix\
	"Allowed ID prefix: [IDENT|MODIFICATION|STATISTICS|USER|END]\n"\
	"Allowed range of each prefix:\n"\
	"IDENT\t\t2..99\n"\
	"MODIFICATION\t100..199\n"\
	"SERVICE\t\t200..299\n"\
	"STATISTICS\t300..399\n"

#define HAS_MANDATORY_BLOCKS(blocks_mark) \
	((1 << IDENT) & blocks_mark) &&\
	((1 << MODIFICATION) & blocks_mark)&&\
	((1 << END) & blocks_mark)
#define HAS_BLOCK(b, blocks_mark) (blocks_mark |= (1 << b))
#define HAS_TAG(taglist, tag) (taglist |= (1 << tag))
#define HAS_ALL_TAG(taglist, fullmark) (taglist == fullmark)

/* initialize globals */
//extern void initialize();

/* check missing mandatory blocks */
extern void check_mandatory_blocks(encode_result &encres);
extern uint16_t get_encode_date(char* c, std::string &buffer, encode_result &encres); 
extern uint32_t get_encode_ophours(char* c, std::string &buffer, encode_result &encres);
extern void set_modif_svcode(ModificationBlock &modif, std::string &buffer);

/* parse header elements */
extern void parse_alignment_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres);
extern void parse_eepromsize_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres);
extern void parse_partnumber_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres);
extern void parse_hwcode_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres);
extern void parse_productindex_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres, char* argv[] = 0);
extern void parse_sn_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres, char* argv[] = 0);
extern void parse_productdate_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres, char* argv[] = 0);
extern void parse_readcode_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres);
extern void parse_testinstruction_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres, char* argv[] = 0);
extern void parse_name_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres);

/* user format templates for templates */
extern void read_user_param_18030_(std::string &fm_buffer, uint16_t uid, encode_result &encres, char* argv[] = 0);
extern void read_user_param_18031_(std::string &fm_buffer, uint16_t uid, encode_result &encres, char* argv[] = 0);
extern void read_user_parameters(std::string &fm_buffer, uint16_t uid, encode_result &encres, char* argv[]);

extern void process_fm_buffer_18031_(std::string &fm_buffer, uint16_t uid, encode_result &encres);
extern void process_fm_buffer(std::string &fm_buffer, uint16_t uid, encode_result &encres);

/*
 * read a FORMAT block which has a column of string-type value
 */
extern std::string read_user_format_bytes(std::ifstream &ifs, UserFormatColumn<char*> &ufc,
	std::string &line, bool &need_readline, encode_result &encres, char* argv[] = 0);

template<typename DATA_T>
std::string read_user_format_number(std::ifstream &ifs, UserFormatColumn<DATA_T> &ufc, 
	std::string &line, bool &need_readline, encode_result &encres, char* argv[] = 0)
{
	std::string ret, fm_buffer;
	DATA_T val = 0;
	char* c;

	ret.clear();

	while (std::getline(ifs, line)) {
	
		encres.line_nr++;
		encres.line_in_blk++;
		c = const_cast<char*>(line.c_str());
		fm_buffer.clear();
		
		if (IS_COMMENT(line))
			continue;

		/* end of FORMATs of a user block */
		if (IS_BLANK(line)) {
		
			need_readline = false;
			break;
		}

		while (!IS_EOL(*c) && !IS_EOS(*c)) {
		
			/* Meeting the next FORMAT, which means this format is done */
			if (IS_LETTER(*c)) {
			
				need_readline = false;
				break;

			} else if (IS_PLACEHOLD(*c)) {

				if (!TEMPLATE_MODE)
					throw E2promValueException("Placehold should be used in template file",
						to_string<int>(encres.line_nr), usage_allowed_user_format_type);

				E2promMsg("Reading parameter for user "+to_string<uint16_t>(ufc.uheader.ID)+
					" @ line "+to_string<int>(encres.line_nr));

				read_user_parameters(fm_buffer, ufc.uheader.ID, encres, argv);
				break;

			} else if (IS_DIGIT(*c) || IS_PERIOD(*c)) {
			
				fm_buffer += *c++;

			} else if (IS_SPACE(*c) || (IS_COMMENT_SIGN(*c))) {
			
				break;

			} else {

				throw E2promValueException(
					"Unexpected characters found for type",
					to_string<int>(encres.line_nr),
					usage_allowed_user_format_type
					);
			}
		} /* end of read-char while-loop */

//		if (TEMPLATE_MODE)
//			process_fm_buffer(fm_buffer, ufc.uheader.ID);
		
		if (!fm_buffer.empty()) {
		
			val = to_digits<DATA_T>(fm_buffer);
			ufc += val;
		}

		if (!IS_LETTER(*c) && !IS_BLANK(line)) {
		
			need_readline = true;

		} else {
		
			break;
		}
	} /* end of read-line while-loop */

//		ret += strize_formats<DATA_T>(ufc);
	std::stringstream ss;
//	ss.width(decres.GLOBAL_ALIGNMENT);
	ss << ufc;
	
	return ss.str();
}

extern void set_service_svcode(ModificationBlock &modif, std::string &buffer);
#define print_interval(o) \
   write_file(o, std::left << std::setw(GLOBAL_ASCII_WIDTH))
// o << std::left << std::setw(GLOBAL_ASCII_WIDTH) 

#define print_item_with_interval_l(o, a, b) \
 print_interval(o) << a << b

#define print_each_item_l(o, cur) \
 for (auto &x : cur) { \
  print_item_with_interval_l(o, x.first+":", x.second+'\n'); \
 }

extern std::string block_gap();
extern void print_header(std::ostream &o, std::string title, 
	std::string id, std::string ali, std::string ees, std::string pn, std::string hw,
	std::string pi, std::string sn, std::string pd, std::string rc,
	std::string ti, std::string name, std::string tail);

extern void print_modif(std::ostream &o, std::string title, 
	ModificationBlock &modif, std::string tail);
extern void print_statisc(std::ostream &o, std::string title,
	eeprom_t &cur, std::string tail);
extern void print_repair(std::ostream &o, std::string title, eeprom_t &cur, std::string tail);
extern void print_end(std::ostream &o);

//extern void parse_alignment_(char* c, std::string &buffer, HeaderBlock &header);
//extern void parse_eepromsize_(char* c, std::string &buffer, HeaderBlock &header);
//extern void parse_partnumber_(char* c, std::string &buffer, HeaderBlock &header);
//extern void parse_hwcode_(char* c, std::string &buffer, HeaderBlock &header);
//extern void parse_productindex_(char* c, std::string &buffer, HeaderBlock &header, char* argv[] = 0);
//extern void parse_sn_(char* c, std::string &buffer, HeaderBlock &header, char* argv[] = 0);
//extern void parse_productdate_(char* c, std::string &buffer, HeaderBlock &header, char* argv[] = 0);
//extern void parse_readcode_(char* c, std::string &buffer, HeaderBlock &header);
//extern void parse_testinstruction_(char* c, std::string &buffer, HeaderBlock &header, char* argv[] = 0);
//extern void parse_name_(char* c, std::string &buffer, HeaderBlock &header);
//
//extern void read_user_param_18030_(std::string &fm_buffer, uint16_t uid, char* argv[] = 0);
//extern void read_user_param_18031_(std::string &fm_buffer, uint16_t uid, char* argv[] = 0);
//extern void read_user_parameters(std::string &fm_buffer, uint16_t uid, char* argv[] = 0);
//
//extern void process_fm_buffer_18031_(std::string &fm_buffer, uint16_t uid);
//extern void process_fm_buffer(std::string &fm_buffer, uint16_t uid);

ns_end(e2prom)

#endif /* TOOLS_BLOCK_TOOL_HPP_ */
