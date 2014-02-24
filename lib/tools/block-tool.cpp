/* 
 * block-tool.cpp
 *
 * Tools to deal with data structures in E2PROM
 * An interface connecting internal EEPROM data structure and files
 *
 * Author: Zex <zex@hytera.com>
 */
#include <tools/block-tool.hpp>

ns_begin(e2prom)

//int encres.line_nr = 0;
//int encres.line_in_blk = 0;
//uint32_t byte_nr = 0;
//uint8_t encres.blocks_mark = 0;
//bool end_reached = false;
/* ./gen-ascii dibr5_template para1 para2 para3 ...
 * the 1th. parameter indicates which template we are using */
//int para_nr = 2;

#if 0
void initialize()
{
	encres.line_nr = 0;
	encres.line_in_blk = 0;
	byte_nr = 0;
	blocks_mark = 0;
	end_reached = false;
	encres.para_nr = 2;

	GLOBAL_ALIGNMENT = 16;		/* bits */
	GLOBAL_EEPROM_SIZE = 1024; /* bytes */
	GLOBAL_ASCII_WIDTH = 20;
	GLOBAL_BLOCK_GAP = 3;		/* gap between each block in line */
	GLOBAL_MAX_BIN_SIZE = 1024*32; /* kbytes */
	THE_FIRST_ALIGNMENT_IS_READ = false;
	THE_FIRST_EEPROMSIZE_IS_READ = false;
	GLOBAL_PAYLOAD_SIZE = 0;

	all_header.clear();
	all_modifi.clear();
	all_repair.clear();
	all_statisc.clear();

	all_user.clear();

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
#endif

/* check missing mandatory blocks */
void check_mandatory_blocks(encode_result &encres)
{
	if (!((1 << IDENT) & encres.blocks_mark))
	{
		throw E2promTypeException(
			"Header block is missing",
			to_string<int>(encres.line_nr),
			usage_allowed_tags
			);
	}
	if (!((1 << MODIFICATION) & encres.blocks_mark))
	{
		throw E2promTypeException(
			"Modification block is missing",
			to_string<int>(encres.line_nr),
			usage_allowed_tags
			);
	}
	if (!((1 << END) & encres.blocks_mark))
	{
		throw E2promTypeException(
			"End block is missing",
			to_string<int>(encres.line_nr),
			usage_allowed_tags
			);
	}
}

uint16_t get_encode_date(char* c, std::string &buffer, encode_result &encres)
{
	while (!IS_EOL(*c) && !IS_EOS(*c))
 	{
 		if (IS_DIGIT(*c) || IS_HYPHEN(*c))
		{
			buffer += *c++;
			continue;
		} else {
			throw E2promValueException(
				"Date should be in format of year-month-day",
				to_string<int>(encres.line_nr),
 				"Ex: 2030-12-01"
			);
		}
 	}
	
	return encode_date(buffer);
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
uint32_t get_encode_ophours(char* c, std::string &buffer, encode_result &encres) 
{
	uint32_t ophours = 0;

	while (!IS_EOL(*c) && !IS_EOS(*c)) {
	
		if (IS_DIGIT(*c) || IS_FSLASH(*c)) {

			buffer += *c++;

		} else if(IS_SPACE(*c)) {

			break;

		} else {
		
			throw E2promTypeException(
				"Format of operating hour: \n\t[times of switch]/[hours of operating time]",
				to_string<int>(encres.line_nr)
			);
		}
	}

	std::vector<std::string> buf = split_by('/', buffer);

	ophours = (to_digits<uint32_t>(buf[0]) << 18) 
			+ to_digits<uint16_t>(buf[1]);

	return ophours;
}

/*
 * read a FORMAT block which has a column of string-type value
 */
//	template<typename DATA_T>
std::string read_user_format_bytes(std::ifstream &ifs, UserFormatColumn<char*> &ufc,
	std::string &line, bool &need_readline, encode_result &encres, char* argv[])
{
	std::string ret, fm_buffer;
	char* c;
	bool dquote_on;
	int dquote_count;
	ret.clear();

	while (std::getline(ifs, line)) {

		encres.line_nr++;
		encres.line_in_blk++;
		c = const_cast<char*>(line.c_str());
		fm_buffer.clear();
		dquote_on = false;
		dquote_count = 0;

		CONTINUE_IF_COMMENT(line);
		/* end of FORMATs of a user block */
		if (IS_BLANK(line)) {
		
			need_readline = false;
			break;
		}
		/* Met next format */	
		if (IS_LETTER(*c)) {
			need_readline = false;
			break;
		}

		while (!IS_EOL(*c) && !IS_EOS(*c)) {
			/* Meeting the next FORMAT, which means this format is done */
			if (IS_LETTER(*c) && !dquote_on) {

				need_readline = false;
				break;

			} else if (IS_DQUOTE(*c)) { /* Met start or end of string */

				c++;
				dquote_on ^= true;
				dquote_count++;

			} else if (IS_PLACEHOLD(*c)) {

				if (!TEMPLATE_MODE)
					throw E2promValueException("Placehold should be used in template file",
						to_string<int>(encres.line_nr), usage_allowed_user_format_type);

				E2promMsg("Reading parameter for user "+to_string<uint16_t>(ufc.uheader.ID)+
						" @ line "+to_string<int>(encres.line_nr));

				read_user_parameters(fm_buffer, ufc.uheader.ID, encres, argv);
				dquote_on ^= true;
				dquote_count++;
				break;

			} else if (dquote_on && IS_PRINTABLE(*c)) { /* In string */
				fm_buffer += *c++;
			} else if (!dquote_on && fm_buffer.size()) { /* Met the end of string */
				break;
			} else if (!dquote_on) { /* Met comments */
				break;
			} else if (IS_COMMENT_SIGN(*c)) {
				if (!dquote_on) break;

				throw E2promValueException("Incomplete string",
					to_string<int>(encres.line_nr), usage_allowed_user_format_type);

			} else {

				throw E2promValueException(
					"Unexpected characters found for type",
					to_string<int>(encres.line_nr), usage_allowed_user_format_type);

			}

		} /* end of read-char while-loop */

		if (TEMPLATE_MODE)
			process_fm_buffer(fm_buffer, ufc.uheader.ID, encres);
		if (dquote_on)
			throw E2promValueException(
				"Expected \" for STRING-type value is missing",
				to_string<int>(encres.line_nr));

		if (!dquote_count || dquote_count%2)
			throw E2promValueException(
				"Unexpected non-strying value in a format of STRING",
				to_string<int>(encres.line_nr));

		if (fm_buffer.size()) {
			ret += fm_buffer;
			ret += '\0';
		}
		
	} /* end of read-line while-loop */
	
//		ufc += const_cast<char*>(ret.c_str());
	ufc.uheader.blocksize += ret.size();
	return ret;
}

void set_modif_svcode(ModificationBlock &modif, std::string &buffer)
{
	std::vector<std::string> buf = split_by(',', buffer);
	int i = 0;

	for (auto &b : buf) {
	
		modif.SERVICECODE[i++] = to_digits<uint16_t>(b);
		
		/* read enough service code */
		if (MAX_RECENT_SVC_NR <= i)
			break;
	}
}

void read_tag(std::string &line, char* c, std::string &tag, encode_result &encres)
{
	tag.clear();

	while (true) {

		if (IS_UPPER(*c)) {//|| IS_LOWER(*c))
		
			tag += *c++;

		} else if (IS_COLON(*c)) { /* done reading tag */
		
			break;

		} else if (IS_EOL(*c)) { /* find no colon in this line */

			throw E2promTypeException(
				"Expected ':' after tag",
				to_string<int>(encres.line_nr));
				
		} else {

			throw E2promTypeException(
				"Tag of block should be upper-case letters",
				to_string<int>(encres.line_nr),
				usage_allowed_tags
				);
		}
	}
}

std::string block_gap()
{
	std::string ret;
	int i(GLOBAL_BLOCK_GAP);

	while(i--) ret += "\n";
	return ret;
}

void print_header(std::ostream &o, std::string title, 
	std::string id, std::string ali, std::string ees, std::string pn, std::string hw,
	std::string pi, std::string sn, std::string pd, std::string rc,
	std::string ti, std::string name, std::string tail)
{
	if (!title.empty())
		write_file(o, title+"\n");

	write_file(o, "# Header block\n");
	print_item_with_interval_l(o, "ID:", id << '\n');
	print_item_with_interval_l(o, "ALIGNMENT:", ali << '\n');
	print_item_with_interval_l(o, "EEPROMSIZE:", ees << '\n');
	print_item_with_interval_l(o, "PARTNUMBER:", pn << '\n');
	print_item_with_interval_l(o, "HWCODE:", hw << '\n');
	print_item_with_interval_l(o, "PRODUCTINDEX:", pi << '\n');
	print_item_with_interval_l(o, "SN:", sn << '\n');
	print_item_with_interval_l(o, "PRODUCTDATE:", pd << '\n');
	print_item_with_interval_l(o, "READCODE:", rc << '\n');
	print_item_with_interval_l(o, "TESTINSTRUCTION:", ti << '\n');
	print_item_with_interval_l(o, "NAME:", name << '\n');
	write_file(o, tail << block_gap());
}

void print_modif(std::ostream &o, std::string title, 
	ModificationBlock &modif, std::string tail)
{
	eeprom_t cur;

	if (!title.empty())
		write_file(o, title+"\n");

	cur["ID"] = std::string(strof(MODIFICATION))+"_"+to_string<uint16_t>(modif.ID);
	/* start to write Modification block */
	write_file(o, "# Modification block\n");
	print_each_item_l(o, cur)
	
	/* writing service code */
	for (int i = 0; i < 10; i++) {
		print_item_with_interval_l(o, 
			"SERVICECODE:", modif.SERVICECODE[i]) << '\n';
	}

	write_file(o, tail << block_gap());
	/* end of writing Modification block */
}

void print_statisc(std::ostream &o, std::string title,
	eeprom_t &cur, std::string tail)
{
	if (!title.empty())
		write_file(o, title+"\n");
	/* write ASCII file */
	write_file(o, "# Statistics block\n");
	print_each_item_l(o, cur);
	write_file(o, tail << block_gap());
	/* end of ASCII-file writing */
}

void print_repair(std::ostream &o, std::string title, eeprom_t &cur, std::string tail)
{
	if (!title.empty())
		write_file(o, title+"\n");
	/* write ASCII file */
	write_file(o, "# Repair block\n");
	print_item_with_interval_l(o, "ID:", cur["ID"]);
	print_item_with_interval_l(o, "DEPARTMENT:", cur["DEPARTMENT"]);
	print_item_with_interval_l(o, "SERVICEDATE:", cur["SERVICEDATE"]);
	print_item_with_interval_l(o, "OPHOURS:", cur["OPHOURS"]);
	print_item_with_interval_l(o, "REPAIRNUMBER:", cur["REPAIRNUMBER"]);
	print_item_with_interval_l(o, "SERVICECODE:", cur["SERVICECODE"]);
	write_file(o, tail << block_gap());
	/* end of ASCII-file writing */
}

void print_end(std::ostream &o)
{
	eeprom_t cur;

	cur["ID"] = std::string(strof(END));
	write_file(o, "# End block\n");
	print_each_item_l(o, cur);
}

void parse_alignment_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres)
{
	while (!IS_EOL(*c) && !IS_EOS(*c)) {
 
 		if (IS_DIGIT(*c)) {
		
			buffer += *c++;
			continue;
		}

		throw E2promValueException(
			"Alignment should be a number",
			to_string<int>(encres.line_nr));
 	}
	
	set_member<HeaderBlock, uint16_t>(
		header, &HeaderBlock::ALIGNMENT, buffer);
	/* set alignment of generating binary */
	if (!encres.THE_FIRST_ALIGNMENT_IS_READ) {
		encres.GLOBAL_ALIGNMENT = header.ALIGNMENT;
		encres.THE_FIRST_ALIGNMENT_IS_READ = true;
	}
}

void parse_eepromsize_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres)
{
	while (!IS_EOL(*c) && !IS_EOS(*c)) {
 	
 		if (IS_DIGIT(*c)) {
		
			buffer += *c++;
			continue;

		} else if (IS_SPACE(*c)) {
		
			break;
		}

		throw E2promValueException(
			"Size of EEPROM should be a number",
			to_string<int>(encres.line_nr));
 	}
	
	set_member<HeaderBlock, uint32_t>(
		header, &HeaderBlock::EEPROMSIZE, buffer);

	if (!encres.THE_FIRST_EEPROMSIZE_IS_READ) {
		encres.GLOBAL_EEPROM_SIZE = header.EEPROMSIZE;
		encres.THE_FIRST_EEPROMSIZE_IS_READ = true;
	}
}

void parse_partnumber_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres)
{
	int8_t dot_c = 1;

	while (!IS_EOL(*c) && !IS_EOS(*c)) {
 	
 		if (IS_DIGIT(*c)) {

			buffer += *c++;
			continue;

		} else if (IS_PERIOD(*c) && c++) {

			if (dot_c++ == 2) {
				set_member<HeaderBlock, uint32_t>(
					header, &HeaderBlock::PARTNUMBER, buffer);
				buffer.clear();
			}
		} else {

			throw E2promValueException(
				"Part number should be in format of xxxx.yyyy.zz",
				to_string<int>(encres.line_nr));
		}
 	}

	if (2 <= dot_c)
		header.PARTNUMBER_RESV = to_digits<uint8_t>(buffer);
	else 
		set_member<HeaderBlock, uint32_t>(
			header, &HeaderBlock::PARTNUMBER, buffer);
}

void parse_hwcode_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres)
{
	while (!IS_EOL(*c) && !IS_EOS(*c)) {
 	
 		if (IS_DIGIT(*c)) {
		
			buffer += *c++;
			continue;

		} else {
			
			throw E2promValueException(
				"Hardware code should be digits",
				to_string<int>(encres.line_nr));
		}
 	}
	
	header.HWCODE = to_digits<uint8_t>(buffer);
}

void parse_productindex_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres, char* argv[])
{
	if (argv) c = argv[encres.para_nr++];

	while (!IS_EOL(*c) && !IS_EOS(*c)) {
 	
 		if (IS_DIGIT(*c)) {
		
			buffer += *c++;
			continue;

		} else if (IS_PERIOD(*c) && c++);
		else {
		
			throw E2promValueException(
				"Product index should be in format as xx.yy",
				to_string<int>(encres.line_nr));
		}
 	}
	
	set_member<HeaderBlock, uint16_t>(
		header, &HeaderBlock::PRODUCTINDEX, buffer);
}

void parse_sn_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres, char* argv[])
{
	int max_len = 10;

	if (argv) c = argv[encres.para_nr++];

	while (!IS_EOL(*c) && !IS_EOS(*c)) {

 		if (IS_DIGIT(*c)) {
		
			buffer += *c++;
			continue;
		}
		else if (!max_len) { break; }
		else if (IS_FSLASH(*c) && c++);
		else if (IS_SPACE(*c)) { break; }
		else {
		
			throw E2promValueException(
				"Serial number should be in format of xxxxxx/yyy",
				to_string<int>(encres.line_nr), "Ex: 783456/032 => 783456032");
		}
		max_len--;
 	}
	
	set_member<HeaderBlock, uint32_t>(
		header, &HeaderBlock::SN, buffer);
}

void parse_productdate_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres, char* argv[])
{
	if (argv) c = argv[encres.para_nr++];
	header.PRODUCTDATE = get_encode_date(c, buffer, encres);
	c = c + buffer.size();
}

void parse_readcode_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres)
{
	while (!IS_EOL(*c) && !IS_EOS(*c)) {

 		if (IS_DIGIT(*c)) {
		
			buffer += *c++;
			continue;
		} else {
		
			throw E2promValueException(
			"Read code should be a number in range of 0~0xffff",
				to_string<int>(encres.line_nr));
		}
 	}

	set_member<HeaderBlock, uint16_t>(
		header, &HeaderBlock::READCODE, buffer);
	
}

void parse_testinstruction_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres, char* argv[])
{
	if (argv) c = argv[encres.para_nr++];

	while (!IS_EOL(*c) && !IS_EOS(*c)) {

 		if (IS_DIGIT(*c)) {// || IS_PERIOD(*c))
		
			buffer += *c++;
			continue;

		} else if (IS_PERIOD(*c)) {
		
			c++;
			continue;

		} else {
		
			throw E2promValueException(
				"Test instruction is composed from digits and period",
				to_string<int>(encres.line_nr), "Ex: 11.03");
		}
 	}

	set_member<HeaderBlock, uint16_t>(
		header, &HeaderBlock::TESTINSTRUCTION, buffer);
}

void parse_name_(char* c, std::string &buffer, HeaderBlock &header, encode_result &encres)
{
	while (!IS_EOL(*c) && !IS_EOS(*c)) {

 		if (IS_UPPER(*c) || IS_LOWER(*c)
		|| IS_DIGIT(*c) || IS_ULINE(*c) || IS_SPACE(*c)
		|| IS_LPARENT(*c) || IS_RPARENT(*c) || IS_HYPHEN(*c)) {
		
			buffer += *c++;
			continue;

		} else {

			throw E2promValueException(
				"Unallowed characters found in name",
				to_string<int>(encres.line_nr),// '-' is allowed ??
				"Allowed characters: [A..Z, a..z, 0..9, (), _, -, space]");
		}
 	} if (MAX_HEADNAME_LEN < buffer.size()) {
	
		throw E2promValueException(
			"Name is too long", to_string<int>(encres.line_nr),
			"Max length of name string is 13");
	}

	strcpy(header.NAME, buffer.c_str());
	for (size_t i = buffer.size(); i < MAX_HEADNAME_LEN; i++)
		header.NAME[i] = 0;
	header.NAME[MAX_HEADNAME_LEN] = '\0';
}

/*
 * ./temp2bin abu_temp.txt pi sn pd ti sn1 pd1
 * ./temp2bin bcb_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3
 * ./temp2bin bscu_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3
 * ./temp2bin dibr5_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 sn4 pd4 sn5 pd5 sn6 pd6 sn7 pd7 sn8 pd8
 * ./temp2bin fan_temp.txt pi sn pd ti
 *
 * 1 FORMAT:             UINT8
 * 2 100
 * 3 105
 * 4 188
 * 5 20
 * 6 <0>
 * 7 <1>
 * 8 FORMAT:             STRING
 * 9 "ETH_0"
 */
void read_user_param_18030_(std::string &fm_buffer, uint16_t uid, encode_result &encres, char* argv[])
{
	if (!argv) return;

	int sn_para_nr = 3;
	std::string sn_para(argv[sn_para_nr]);
	uint8_t macaddr[] = {0, 0, 0, 0, 0, 0};

	calc_macaddr(split_by('/', sn_para).at(0), macaddr);

	switch(encres.line_in_blk) {

		 case 6: {

			fm_buffer = to_string<uint16_t>(macaddr[4]);
			break;

		} case 7: {
			fm_buffer = to_string<uint16_t>(macaddr[5]);
			break;
		}
		default:
			throw E2promValueException(
		"No template defined for user "+to_string<uint16_t>(uid),
		to_string<int>(encres.line_nr));
	}
}
/*
 * 1  FORMAT:             STRING
 * 2  "5502.0504.04"      # PARTNUMBER
 * 3  "04.00"             # PRODUCTINDEX
 * 4  "<SN3>/002"         # SN
 * 5  "<PD3>"             # PRODUCTDATE
 * 6  FORMAT:             UINT16
 * 7  0                   # READCODE
 * 8  FORMAT:             STRING
 * 9  "00.01"             # TESTINSTRUCTION
 * 10 "DPA_BOARD"         # NAME
 */
void read_user_param_18031_(std::string &fm_buffer, uint16_t uid, encode_result &encres, char* argv[])
{
	char* c;
	std::string buf, test;
	HeaderBlock header(2);

	if (!argv)
		throw E2promValueException(
		"No value for user "+to_string<uint16_t>(uid)+" to read",
		to_string<int>(encres.line_nr));

	c = argv[encres.para_nr];
	switch(encres.line_in_blk) {

		 case 4: {
			parse_sn_(c, test, header, encres, argv);
			fm_buffer += argv[encres.para_nr-1];
			break;
		} case 5: {
			parse_productdate_(c, test, header, encres, argv);
			fm_buffer += argv[encres.para_nr-1];
			break;
		}
		default:
			throw E2promValueException(
		"No template defined for user "+to_string<uint16_t>(uid),
		to_string<int>(encres.line_nr));
	}
}

void read_user_parameters(std::string &fm_buffer, uint16_t uid, encode_result &encres, char* argv[])
{
	switch(uid) {
		case 18031: read_user_param_18031_(fm_buffer, uid, encres, argv); break;
		case 18030: read_user_param_18030_(fm_buffer, uid, encres, argv); break;
		default:
			throw E2promValueException(
		"No template defined for user "+to_string<uint16_t>(uid),
		to_string<int>(encres.line_nr));
	}
}

/*
 * ./temp2bin abu_temp.txt pi sn pd ti sn1 pd1
 * ./temp2bin bcb_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3
 * ./temp2bin bscu_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3
 * ./temp2bin dibr5_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 sn4 pd4 sn5 pd5 sn6 pd6 sn7 pd7 sn8 pd8
 * ./temp2bin fan_temp.txt pi sn pd ti
 */
//void process_fm_buffer_18030_(std::string &fm_buffer, uint16_t uid)
//{
//}

/*
 * 1  FORMAT:             STRING
 * 2  "5502.0504.04"      # PARTNUMBER
 * 3  "04.00"             # PRODUCTINDEX
 * 4  "<SN3>/002"         # SN
 * 5  "<PD3>"             # PRODUCTDATE
 * 6  FORMAT:             UINT16
 * 7  0                   # READCODE
 * 8  FORMAT:             STRING
 * 9  "00.01"             # TESTINSTRUCTION
 * 10 "DPA_BOARD"         # NAME
 */
void process_fm_buffer_18031_(std::string &fm_buffer, uint16_t uid, encode_result &encres)
{
	char* c = const_cast<char*>(fm_buffer.c_str());
	std::string test;
	HeaderBlock header(2);

	switch(encres.line_in_blk) {
		case 2: parse_partnumber_(c, test, header, encres); break;
		case 3: parse_productindex_(c, test, header, encres); break;
//		case 7: parse_readcode_(c, test, header); break; // uint16
		case 9: parse_testinstruction_(c, test, header, encres); break;
		case 4: //parse_sn_(c, test, header); break;
		case 5: //parse_productdate_(c, test, header); break;
		case 1:
		case 6:
		case 8: 
		case 10: break;
		default:
			throw E2promValueException(
		"Incorrect template defined for user "+to_string<uint16_t>(uid),
		to_string<int>(encres.line_nr));
	}
}

void process_fm_buffer(std::string &fm_buffer, uint16_t uid, encode_result &encres)
{
	switch(uid) {
		case 18031: process_fm_buffer_18031_(fm_buffer, uid, encres); break;
		case 18030: break;//process_fm_buffer_18030_(fm_buffer, uid, encres); break;
		default:
			throw E2promValueException(
		"No template defined for user "+to_string<uint16_t>(uid),
		to_string<int>(encres.line_nr));
	}
}

ns_end(e2prom)
