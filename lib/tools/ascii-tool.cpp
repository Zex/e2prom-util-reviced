/* 
 * ascii-tool.cpp
 *
 * Tools to parse ASCII-format file
 *
 * Author: Zex <zex@hytera.com>
 */
#include <tools/ascii-tool.hpp>

ns_begin(e2prom)

void parse_block(std::ifstream &ifs, std::ofstream &ofs, std::string &buffer, encode_result &encres, char* argv[])
{
	E2promMsg("Preparing for parsing block with ID " + buffer);

	std::vector<std::string> ID = split_by('_', buffer);
	std::string id_prefix = ID[0];
	uint16_t id = 0;

	if (ID.size() > 1)
	{
		id = to_digits<uint16_t>(ID[1]);
	}
	
	if ((strof(IDENT) == id_prefix ) && IS_VALID_ID(IDENT, id)) {
	
		HAS_BLOCK(IDENT, encres.blocks_mark);
		parse_block_(IDENT, ifs, ofs, id, encres,  argv);
	
	} else if ((strof(MODIFICATION) == id_prefix ) && IS_VALID_ID(MODIFICATION, id)) {
	
		HAS_BLOCK(MODIFICATION, encres.blocks_mark);
		parse_block_(MODIFICATION, ifs, ofs, id, encres, argv);
	
	} else if ((strof(SERVICE) == id_prefix ) && IS_VALID_ID(SERVICE, id)) {
	
		parse_block_(SERVICE, ifs, ofs, id, encres, argv);
	
	} else if ((strof(USER) == id_prefix ) && IS_VALID_ID(USER, id)) {
	
		parse_block_(USER, ifs, ofs, id, encres, argv);
	
	} else if ((strof(END) == id_prefix)) {
	
		HAS_BLOCK(END, encres.blocks_mark);
								// id here might not be used
		parse_block_(END, ifs, ofs, 0xffff, encres, argv);
	
	} else if ((strof(STATISTICS) == id_prefix ) && IS_VALID_ID(STATISTICS, id)) {
	
		parse_block_(STATISTICS, ifs, ofs, id, encres, argv);

	} else {
	
		throw E2promValueException(
			"Unrecognized type of block or invalid id for block",
			to_string<int>(encres.line_nr), usage_allowed_id_prefix);
	}
}

void parse_STATISTICS(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[])
{
	E2promMsg("Parsing statistics block");

	std::string buffer, tag, line;
	StatisticsBlock statisc(id);
	/* for checking missed tag in block */
	uint16_t taglist = 0;
	uint16_t full = (1 << 1);
	char* c = 0;

	while (std::getline(ifs, line)) {
	
		encres.line_nr++;
		c = const_cast<char*>(line.c_str());
		buffer.clear();
		
		BREAK_IF_BLANK(line);
		CONTINUE_IF_COMMENT(line);

		read_tag(line, c, tag, encres);
							// 1 for colon
		c = c + (tag.size() + 1);
		while (IS_SPACE(*++c));

		if ("OPHOURS" == tag) {
		
			HAS_TAG(taglist, 1);
			
			statisc.OPHOURS = get_encode_ophours(c, buffer, encres);
			c = c + buffer.size();
		
		} else if (HAS_ALL_TAG(taglist, full)) {
		
			break;
		
		} else {
		
			throw E2promTypeException(
					tag += " is unrecognized for modification block",
					to_string<int>(encres.line_nr), usage_allowed_tags);
		}
	} /* end of while */

	if (!HAS_ALL_TAG(taglist, full)) {
	
		throw E2promTypeException(
				"Tags in statistcs block is incompleted",
				to_string<int>(encres.line_nr), "Ten service code are needed");
			
	} else {
	
		set_checksum<StatisticsBlock>(statisc);
		write_file(ofs, statisc);
		write_str(statisc, encres.eeprom_bin_str);
	}
}

void parse_SERVICE(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[])
{
	E2promMsg("Parsing repair block");

	std::string buffer, tag, line;
	RepairBlock repair(id);
	/* for checking missed tag in block */
	uint16_t taglist = 0;
	/* preset the full condition */
	uint16_t full = ((1 << DEPARTMENT) | 
		(1 << SERVICEDATE) | (1 << RepairTag::OPHOURS) |
		(1 << REPAIRNUMBER) | (1 << RepairTag::SERVICECODE));
	char* c = 0;

	while (std::getline(ifs, line))
	{
		encres.line_nr++;
		c = const_cast<char*>(line.c_str());
		buffer.clear();

		BREAK_IF_BLANK(line);
		CONTINUE_IF_COMMENT(line);

		read_tag(line, c, tag, encres);
		c = c + (tag.size() + 1);

		/* omit space */
		while (IS_SPACE(*++c));

		if (strof(DEPARTMENT) == tag) {
		
			HAS_TAG(taglist, DEPARTMENT);
			
			while (!IS_EOL(*c)) {
		
				if (IS_UPPER(*c) || IS_DIGIT(*c) || IS_SPACE(*c)) {
				
					buffer += *c++;
			
				} else {
				
					throw E2promTypeException(
						"Tag of block should be upper-case letters",
						to_string<int>(encres.line_nr), usage_allowed_tags);
				}
			} /* end of reading department */

			if (sizeof(repair.DEPARTMENT) < buffer.size()) {
			
				throw E2promValueException(
					"Name string is too long for DEPARTMENT",
					to_string<int>(encres.line_nr));
			}
			strncpy((char*)repair.DEPARTMENT, buffer.c_str(), sizeof(repair.DEPARTMENT));

		} else if (strof(SERVICEDATE) == tag) {
		
			HAS_TAG(taglist, SERVICEDATE);

			repair.SERVICEDATE = get_encode_date(c, buffer, encres);
			c = c + buffer.size();
		
		} else if (strof(OPHOURS) == tag) {
		
			HAS_TAG(taglist, OPHOURS);

			repair.OPHOURS = get_encode_ophours(c, buffer, encres);
			c = c + buffer.size();
		} else if (strof(REPAIRNUMBER) == tag) {
		
			HAS_TAG(taglist, REPAIRNUMBER);

			while (!IS_EOL(*c) && !IS_EOS(*c)) {
		 	
		 		if (IS_DIGIT(*c)) {
				
					buffer += *c++;
				
				} else {
				
					throw E2promValueException(
						"Repair number should be a number of four digits",
						to_string<int>(encres.line_nr));
				}
		 	}

			set_member<RepairBlock, uint16_t>(
				repair, &RepairBlock::REPAIRNUMBER, buffer);

		} else if (strof(SERVICECODE) == tag) {
		
			HAS_TAG(taglist, SERVICECODE);
			
			while (!IS_EOL(*c) && !IS_EOS(*c)) {
		 	
		 		if (IS_DIGIT(*c)) {
				
					buffer += *c++;
				
				} else {
				
					throw E2promValueException(
						"Service code requires a number in range 1..21",
						to_string<int>(encres.line_nr));
				}
		 	}

			set_member<RepairBlock, uint16_t>(
				repair, &RepairBlock::SERVICECODE, buffer);

		} else if (HAS_ALL_TAG(taglist, full)) {
		
			break;

		} else {
		
			throw E2promTypeException(
					tag += " is unrecognized for repair block",
					to_string<int>(encres.line_nr), usage_allowed_tags);
		} /* end of reading tag */
	} /* end of while */

	if (!HAS_ALL_TAG(taglist, full)) {
	
		throw E2promTypeException(
				"Tags in repair block is incompleted",
				to_string<int>(encres.line_nr), usage_allowed_tags);
	}
	
	set_checksum<RepairBlock>(repair);
	write_file(ofs, repair);
	write_str(repair, encres.eeprom_bin_str);
}

void parse_USER(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[])
{
	E2promMsg("Parsing user block");
	std::string buffer, tag, line, fm_buffer;
	UserBlockHeader uheader(id);
	/* for checking missed tag in block */
	uint16_t taglist = 0;
	uint16_t full = 1; /* at lease one format in this block */
	char* c = 0;
	bool need_readline = true;
	fm_buffer.clear();

	encres.line_in_blk = 0;

	while (true) {
	
		if (need_readline) {
			if (std::getline(ifs, line)) {
				encres.line_nr++;
				encres.line_in_blk++;
			}
			else break;
		}
		
		BREAK_IF_BLANK(line);
		CONTINUE_IF_COMMENT(line);

		c = const_cast<char*>(line.c_str());
		read_tag(line, c, tag, encres);

			// 1 for IS_COLON(*c)
		c = c + (tag.size() +1);
		buffer.clear();

		if (strof(FORMAT) == tag) {
		
			taglist++;

			while (IS_SPACE(*++c));
//			if (IS_COMMENT_SIGN(*c)) continue;

			/* read type of FORMAT */
			while (!IS_EOL(*c) && !IS_EOS(*c)) {
			
				if (IS_UPPER(*c) || IS_DIGIT(*c)) {
					buffer += *c++;
				} else if (IS_SPACE(*c)) {
					break;
				} else {
					throw E2promValueException(
						"Unexpected type of format column",
						to_string<int>(encres.line_nr), usage_allowed_user_format_type);
				}
			}
			/* parse FORMAT */
			if (buffer.size()) {
			
				if ("NONE" == buffer) {
				
					throw E2promValueException(
						"NONE-type is not allowed",
						to_string<int>(encres.line_nr), usage_allowed_user_format_type);
						
				} else if ("UINT8" == buffer) {
					UserFormatColumn<uint8_t> ufc(uheader);
					/* read values of FORMAT */
					fm_buffer += read_user_format_number<uint8_t>(ifs, ufc, line, need_readline, encres, argv);
				} else if ("INT8" == buffer) {
					UserFormatColumn<int8_t> ufc(uheader);
					/* read values of FORMAT */
					fm_buffer += read_user_format_number<int8_t>(ifs, ufc, line, need_readline, encres, argv);
				} else if ("UINT16" == buffer) {
					UserFormatColumn<uint16_t> ufc(uheader);
					/* read values of FORMAT */
					fm_buffer += read_user_format_number<uint16_t>(ifs, ufc, line, need_readline, encres, argv);
				} else if ("INT16" == buffer) {
					UserFormatColumn<int16_t> ufc(uheader);
					/* read values of FORMAT */
					fm_buffer += read_user_format_number<int16_t>(ifs, ufc, line, need_readline, encres, argv);
				} else if ("UINT32" == buffer) {
					UserFormatColumn<uint32_t> ufc(uheader);
					/* read values of FORMAT */
					fm_buffer += read_user_format_number<uint32_t>(ifs, ufc, line, need_readline, encres, argv);
				} else if ("INT32" == buffer) {
					UserFormatColumn<int32_t> ufc(uheader);
					/* read values of FORMAT */
					fm_buffer += read_user_format_number<int32_t>(ifs, ufc, line, need_readline, encres, argv);
				} else if ("UINT64" == buffer) {
					UserFormatColumn<uint64_t> ufc(uheader);
					/* read values of FORMAT */
					fm_buffer += read_user_format_number<uint64_t>(ifs, ufc, line, need_readline, encres, argv);
				} else if ("FLOAT" == buffer) {
					UserFormatColumn<float> ufc(uheader);
					/* read values of FORMAT */
					fm_buffer += read_user_format_number<float>(ifs, ufc, line, need_readline, encres, argv);
				} else if ("DOUBLE" == buffer) {
					UserFormatColumn<double> ufc(uheader);
					/* read values of FORMAT */
					fm_buffer += read_user_format_number<double>(ifs, ufc, line, need_readline, encres, argv);
				} else if ("LONG_DOUBLE" == buffer) {
					// TODO: ???
					UserFormatColumn<long double> ufc(uheader);
					/* read values of FORMAT */
					fm_buffer += read_user_format_number<long double>(ifs, ufc, line, need_readline, encres, argv);
				} else if ("STRING" == buffer) {
					UserFormatColumn<char*> ufc(uheader);
					/* read values of FORMAT */
					fm_buffer += read_user_format_bytes(ifs, ufc, line, need_readline, encres, argv);
				} else {
					throw E2promValueException(
						"Unexpected type of format column found: "+buffer,
						to_string<int>(encres.line_nr), usage_allowed_user_format_type);
				} /* end of identifying format type */
			} else {

				throw E2promValueException(
					"Type of values in FORMAT is needed",
					to_string<int>(encres.line_nr), usage_allowed_user_format_type);
			} /* end of parsing format value */
		} else {/* end of parsing a format */

			throw E2promTypeException(
				"Unexpected type of format column",
				to_string<int>(encres.line_nr), usage_allowed_user_format_type);
		} /* end of parsing format in a user block */
	} /* end of read-line while-loop */

	if (taglist < full) {
		throw E2promTypeException(
				"Each user block requirs at least one FORMAT",
				to_string<int>(encres.line_nr));
	} else {
		/* header + all FORMATs */
		if (fm_buffer.size()) {

			uheader.set_checksum(fm_buffer, encres);
			uheader.write_bin(ofs, encres);
			write_bin_ufc(ofs, fm_buffer, encres);
		}
	}

	/* end of procesing one user block */
}

void parse_END(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[])
{
	E2promMsg("Parsing end block");

	EndBlock end;
	std::string line;

	end.write_bin(ofs, encres);
	
	/* nothing should be read from now on */
	while (std::getline(ifs, line)) {
	
		encres.line_nr++;

		if (!IS_BLANK(line)) {

			throw E2promFormatException(
				"The end block should be the last block of configuration file",
				to_string<int>(encres.line_nr));

		} else {

			encres.end_reached = true;
		}
	}
}
/*
 * Parse header block, which's ID is named
 * after IDENT_xxx  in ascii file
 */
void parse_IDENT(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[])
{
	E2promMsg("Parsing header block");

	std::string buffer, tag, line;
	HeaderBlock header(id);
	/* for checking missed tag in block */
	uint16_t taglist = 0;
	uint16_t full = (1 << ALIGNMENT) | (1<< EEPROMSIZE) | (1<< PARTNUMBER)
				  | (1 << HWCODE) | (1 << PRODUCTINDEX) | (1 << SN)
				  | (1 << PRODUCTDATE) | (1 << READCODE)
				  | (1 << TESTINSTRUCTION) | (1 << NAME);
	char* c = 0;

	while (std::getline(ifs, line)) {

		encres.line_nr++;
		c = const_cast<char*>(line.c_str());

		BREAK_IF_BLANK(line);
		CONTINUE_IF_COMMENT(line);
		
		read_tag(line, c, tag, encres);
		c = c + (tag.size() + 1);
		
		/* omit space */
		while (IS_SPACE(*++c));

		buffer.clear();

		if (strof(ALIGNMENT) == tag) {
		
			HAS_TAG(taglist, ALIGNMENT);
			parse_alignment_(c, buffer, header, encres);
		
		} else if (strof(EEPROMSIZE) == tag) {
		
			HAS_TAG(taglist, EEPROMSIZE);
			parse_eepromsize_(c, buffer, header, encres);

		} else if (strof(PARTNUMBER) == tag) { /* Ex: 1122.9737.09 */
		
			HAS_TAG(taglist, PARTNUMBER);
			parse_partnumber_(c, buffer, header, encres);

		} else if (strof(HWCODE) == tag) {
		
			HAS_TAG(taglist, HWCODE);
			parse_hwcode_(c, buffer, header, encres);

		} else if (strof(PRODUCTINDEX) == tag) {
		
			HAS_TAG(taglist, PRODUCTINDEX);
			parse_productindex_(c, buffer, header, encres, argv);

		} else if (strof(SN) == tag) {
		/*
		 * A full serial number is composed from 6 digits, plus a zero-leading variant
		 *
		 * Ex: 783456/032 => 783456032
         */
			HAS_TAG(taglist, SN);
			parse_sn_(c, buffer, header, encres, argv);

		} else if (strof(PRODUCTDATE) == tag) {
		/*
		 * note: number of year is based upon 1980, which is year of 0
		 */
			HAS_TAG(taglist, PRODUCTDATE);
			parse_productdate_(c, buffer, header, encres, argv);

		} else if (strof(READCODE) == tag) {
		/* range: 0~0xffff */
			HAS_TAG(taglist, READCODE);
			parse_readcode_(c, buffer, header, encres);

		} else if (strof(TESTINSTRUCTION) == tag) {
		/*
		 * A change-index test instruction number consists of 
		 * a major-changing index and a minor-changing index.
		 * Both two portions contain two digits.
		 *
		 * Ex: 11.03 => 1103
		 *
		 * In this case, 11 indicates the major one, meanwhile
		 * 03 indicates the minor one.
		 */
			HAS_TAG(taglist, TESTINSTRUCTION);
			parse_testinstruction_(c, buffer, header, encres, argv);
		
		} else if (strof(NAME) == tag) {
		/* 
		 * max length: 13 characters + '\0'
		 * allowed characters: A..Z, a..z, 0..9, (), _ and space
		 */
		
			HAS_TAG(taglist, NAME);
			parse_name_(c, buffer, header, encres);

		} else if (HAS_ALL_TAG(taglist, full)) { break;
		} else {

			throw E2promTypeException(
					tag += " is unrecognized for header block",
					to_string<int>(encres.line_nr), usage_allowed_tags);
		}
	} /* end of while */

	if (!HAS_ALL_TAG(taglist, full)) {

		throw E2promTypeException(
				"Tags in header block is incompleted",
				to_string<int>(encres.line_nr), usage_allowed_tags);
	} else {

		set_checksum<HeaderBlock>(header);
		write_file(ofs, header);
		write_str(header, encres.eeprom_bin_str);
	}
}

/*
 * Parse service block, which's ID is named
 * after MODIFICATION_xxx  in ascii file
 */
void parse_MODIFICATION(std::ifstream &ifs, std::ofstream &ofs, uint16_t id, encode_result &encres, char* argv[])
{
	E2promMsg("Parsing modification block");

	std::string buffer, tag, line;
	ModificationBlock modif(id);
	/* for checking missed tag in block */
	uint16_t taglist = 0;
	uint16_t full = 10;
	char* c = 0;

	while (std::getline(ifs, line)) {
	
		encres.line_nr++;
		c = const_cast<char*>(line.c_str());
		/* don't clear buffer here !! */

		BREAK_IF_BLANK(line);
		CONTINUE_IF_COMMENT(line);

		read_tag(line, c, tag, encres);
		c = c + (tag.size() + 1);

		/* omit space */
		while (IS_SPACE(*++c));

		if ("SERVICECODE" == tag) {
		
			taglist++;
			/* 
			 * While leaving the while loop, buffer might be string like 
			 *
			 * 1213,25235,13241,4343,32355,3444,22,0,0,0,
			 *
			 * each number in buffer indicates a service code.
			 */
			while (!IS_EOL(*c) && !IS_EOS(*c)) {
			
		 		if (IS_DIGIT(*c)) {
				
					buffer += *c++;
					continue;
				
				} else if (IS_SPACE(*c)) {
				
					break;
				}

				throw E2promValueException(
					"Service code should be a number",
					to_string<int>(encres.line_nr));
					
		 	} /* end of reading service code value */
			
			buffer += ',';

		} else if (HAS_ALL_TAG(taglist, full)) { break;
		} else {

			throw E2promTypeException(
					tag += " is unrecognized for modification block",
					to_string<int>(encres.line_nr), usage_allowed_tags);
		}
	} /* end of while */
	
	if (!HAS_ALL_TAG(taglist, full)) {
	
		throw E2promTypeException(
				"Tags in modification block is incompleted",
				to_string<int>(encres.line_nr),
				"Ten service code are needed");
	} else {

		set_modif_svcode(modif, buffer);
		set_checksum<ModificationBlock>(modif);
		write_file(ofs, modif);
		write_str(modif, encres.eeprom_bin_str);
	}
}

encode_result dealing_ascii(std::ifstream &ifs, std::ofstream &ofs, char* argv[])
{
	std::string file_type;
	std::string tag, buffer, line;
	char *c = 0;
	encode_result encres;

	if (TEMPLATE_MODE) {
		file_type = "template file";
	} else {
		file_type = "ASCII file";
	}
	E2promMsg("Start to process " << file_type);

//	initialize();

	while (std::getline(ifs, line)) {

		encres.line_nr++;
		tag.clear();

		if (IS_BLANK(line) || IS_COMMENT(line))
			continue;
		if (encres.end_reached) break;

		c = const_cast<char*>(line.c_str());
		buffer.clear();

		/*
		 * parse line with tag
		 *
		 * sample ascii file looks like
		 *
		 * # this is comments
		 * # this is comments
		 *
		 * tag:        element
		 * tag:        element
		 *
		 * # this is comments
		 * tag:        element
		 */
		if (IS_UPPER(*c)) {
		
			/* reading tag */
			read_tag(line, c, tag, encres);
			c = c + tag.size();

			/* reading element */
			if (IS_COLON(*c)) {
				
				/* omit space */
				while (IS_SPACE(*++c));
	
				/* parse ID of block to determine type of block */
				if ("ID" == tag) {
				
					while (!IS_EOL(*c) && !IS_EOS(*c)) {
					
						if (IS_UPPER(*c) || IS_DIGIT(*c) || IS_ULINE(*c)) {
						
							 buffer += *c++;
							 continue;

						} else if (IS_EOL(*c) || IS_EOS(*c)) { break;
						} else {
						
							throw E2promValueException(
									"Unrecognized ID string",
									to_string<int>(encres.line_nr), usage_allowed_id_prefix);
						}
					}
					/* start to parse the current block */
					parse_block(ifs, ofs, buffer, encres, argv);
				}
			}
		} else if (IS_SPACE(*c)) { continue;
		} else {

			throw E2promTypeException(
				"Unrecognized tag of block",
				to_string<int>(encres.line_nr), usage_allowed_tags);
		}
	} /* end of while-loop: done reading all lines in file */

	if (!HAS_MANDATORY_BLOCKS(encres.blocks_mark))
		check_mandatory_blocks(encres);

	E2promMsg("Done with " << file_type);
	return encres;
}

ns_end(e2prom)
