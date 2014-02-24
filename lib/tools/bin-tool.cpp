/* 
 * bin-tool.cpp
 *
 * Tools to read binary-format file
 *
 * Author: Zex <zex@hytera.com>
 */
#include <tools/bin-tool.hpp>

ns_begin(e2prom)

UserBlkTps global_xtype;

uint16_t get_id(char *raw, size_t nr)
{
	uint16_t id;

	/* two bytes for block id */
	id = (*reinterpret_cast<uint16_t*>(&raw[nr])) << 8;
	id += (*reinterpret_cast<uint8_t*>(&raw[nr+1]));

	return ntohs(id);
}
/*
bool has_endmark_(char *raw, size_t esize)
{
	for (size_t i(0); i < esize; i++)
		if (0xffff == get_id(raw, i))
			return true;
	return false;
}

bool has_endmark(const char *raw, size_t esize)
{
	return has_endmark_(const_cast<char*>(raw), esize);
}
*/
/* find out which block we've met */
//uint16_t read_id(std::string &line)
uint16_t read_id(char *raw, decode_result &decres)
{
	E2promMsg("Reading block id at 0x"+get_hex_str(decres.byte_nr));
	return get_id(raw, decres.byte_nr);
}

//void read_IDENT(std::string &line, std::ofstream &ofs)
void read_IDENT(char *raw, std::ofstream &ofs, decode_result &decres)
{
	E2promMsg("Reading header block");
	
	HeaderBlock header;
	eeprom_t cur;
	std::string data, chsum_stat;
	
	for (size_t i = 0 ; i < GLOBAL_HEADER_BLK_SIZE; i++)
		data += raw[decres.byte_nr++];
//	data += line.substr(byte_nr, sizeof(HeaderBlock));
	
	std::stringstream ss(data);
	ss >> header;
	/* check for old format eeprom  */
	if (header.blocksize != GLOBAL_HEADER_BLK_SIZE)
		throw E2promValueException("Old format EEPROM binary",
			to_string<int>(decres.byte_nr));
	
	// chsum_stat = is_checksum16_correct<HeaderBlock>(header, data) ?
	// CRC16_CORRECT : CRC16_INCORRECT;
	if (is_checksum16_correct<HeaderBlock>(header, data)) {
	//	current_header = header;
		chsum_stat = CRC16_CORRECT;
	} else {
	//	current_header.blocksize = 0;
		chsum_stat = CRC16_INCORRECT;
	}
	
	if (!decres.THE_FIRST_EEPROMSIZE_IS_READ) {
		decres.GLOBAL_EEPROM_SIZE = header.EEPROMSIZE;
		decres.THE_FIRST_EEPROMSIZE_IS_READ = true;
	}
	if (!decres.THE_FIRST_ALIGNMENT_IS_READ) {
		decres.GLOBAL_ALIGNMENT = header.ALIGNMENT;
		decres.THE_FIRST_ALIGNMENT_IS_READ = true;
	}
	
	cur["ID"] = std::string(strof(IDENT))+"_"+to_string<uint16_t>(header.ID);
	cur["ALIGNMENT"] = to_string<uint16_t>(header.ALIGNMENT);
	cur["EEPROMSIZE"] = to_string<uint16_t>(header.EEPROMSIZE);
	cur["PARTNUMBER"] = decode_partnumber(header.PARTNUMBER, header.PARTNUMBER_RESV);
	cur["HWCODE"] = to_string<uint16_t>(header.HWCODE);
	cur["PRODUCTINDEX"] = decode_dotted_uint16(header.PRODUCTINDEX);
	cur["SN"] = decode_serial_number(header.SN);
	cur["PRODUCTDATE"] = decode_date(header.PRODUCTDATE);
	cur["READCODE"] = to_string<uint16_t>(header.READCODE);
	cur["TESTINSTRUCTION"] = decode_dotted_uint16(header.TESTINSTRUCTION);
	cur["NAME"] = header.NAME;

	decres.all_header[cur["NAME"]] = cur;
	/* write ASCII file */
	print_header(ofs, "", 
  	cur["ID"], cur["ALIGNMENT"], cur["EEPROMSIZE"], cur["PARTNUMBER"],
  	cur["HWCODE"], cur["PRODUCTINDEX"], cur["SN"], cur["PRODUCTDATE"],
  	cur["READCODE"], cur["TESTINSTRUCTION"], cur["NAME"],
	"# "+chsum_stat);
	/* end of ASCII-file writing */
}

//void read_MODIFICATION(std::string &line, std::ofstream &ofs)
void read_MODIFICATION(char *raw, std::ofstream &ofs, decode_result &decres)
{
	E2promMsg("Reading modification block");
	
	ModificationBlock modif;
	eeprom_t cur;
	std::string data, chsum_stat, svco;
	
	for (size_t i = 0; i < sizeof(ModificationBlock); i++) 
		 data += raw[decres.byte_nr++];
	
	std::stringstream ss(data);
	ss >> modif;
	
	chsum_stat = is_checksum16_correct<ModificationBlock>(modif, data) ?
						CRC16_CORRECT : CRC16_INCORRECT;

//	cur["ID"] = std::string(strof(MODIFICATION))+"_"+to_string<uint16_t>(modif.ID);
	
	for (int i = 0; i < 10; i++) 
		svco += to_string<uint16_t>(modif.SERVICECODE[i])+" ";
	
	cur["SERVICECODE"] = svco;
	decres.all_modifi[cur["ID"]] = cur;

	print_modif(ofs, "", modif, "# "+chsum_stat);
}

//void read_SERVICE(std::string &line, std::ofstream &ofs)
void read_SERVICE(char *raw, std::ofstream &ofs, decode_result &decres)
{
	E2promMsg("Reading repair block");
	
	RepairBlock repair;
	eeprom_t cur;
	std::string data, chsum_stat;
	
	for (size_t i = 0 ; i < sizeof(RepairBlock); i++)
		data += raw[decres.byte_nr++];
	
	std::stringstream ss(data);
	ss >> repair;
	
	chsum_stat = is_checksum16_correct<RepairBlock>(repair, data) ?
			CRC16_CORRECT : CRC16_INCORRECT;

cur["ID"] = std::string(strof(SERVICE))+"_"+to_string<uint16_t>(repair.ID);
cur["DEPARTMENT"] =	reinterpret_cast<char*>(&repair.DEPARTMENT);
cur["SERVICEDATE"] = decode_date(repair.SERVICEDATE);
cur["OPHOURS"] = decode_ophours(repair.OPHOURS);
cur["REPAIRNUMBER"] = to_string<uint16_t>(repair.REPAIRNUMBER);
cur["SERVICECODE"] = to_string<uint16_t>(repair.SERVICECODE);
decres.all_repair[cur["ID"]] = cur;

print_repair(ofs, "", cur, "# "+chsum_stat);
}

//void read_STATISTICS(std::string &line, std::ofstream &ofs)
void read_STATISTICS(char *raw, std::ofstream &ofs, decode_result &decres)
{
	E2promMsg("Reading statistics block");
	
	StatisticsBlock statisc;
	eeprom_t cur;
	std::string data, chsum_stat;
	
	for (size_t i = 0 ; i < sizeof(StatisticsBlock); i++)
		data += raw[decres.byte_nr++];
	
	std::stringstream ss(data);
	ss >> statisc;
	
	chsum_stat = is_checksum16_correct<StatisticsBlock>(statisc, data) ?
	 	CRC16_CORRECT : CRC16_INCORRECT;

	cur["ID"] =
		std::string(strof(STATISTICS))+"_"+to_string<uint16_t>(statisc.ID);
	cur["OPHOURS"] = decode_ophours(statisc.OPHOURS);
	
	decres.all_statisc[cur["ID"]] = cur;
	
	print_statisc(ofs, "", cur, "# "+chsum_stat);
	/* end of ASCII-file writing */
}

uint16_t get_type_id(std::string &buffer)
{
	return (strof(UINT8)==buffer) ? UINT8 : (strof(INT8)==buffer) ? INT8 :
		(strof(UINT16)==buffer) ? UINT16 : (strof(INT16)==buffer) ? INT16 :
		(strof(UINT32)==buffer) ? UINT32 : (strof(INT32)==buffer) ? INT32 :
		(strof(UINT64)==buffer) ? UINT64 : (strof(INT64)==buffer) ? INT64 :
		(strof(FLOAT)==buffer) ? FLOAT : (strof(DOUBLE)==buffer) ? DOUBLE :
		(strof(LONG_DOUBLE)==buffer) ? LONG_DOUBLE :
		(strof(STRING)==buffer) ? STRING :
		NONE;
}

uint16_t get_sizeof(std::string &buffer)
{
	return (strof(UINT8)==buffer) ? sizeof(uint8_t) :
		(strof(INT8)==buffer) ? sizeof(int8_t) :
		(strof(UINT16)==buffer) ? sizeof(uint16_t) :
		(strof(INT16)==buffer) ? sizeof(int16_t) :
		(strof(UINT32)==buffer) ? sizeof(uint32_t) :
		(strof(INT32)==buffer) ? sizeof(int32_t) :
		(strof(UINT64)==buffer) ? sizeof(uint64_t) :
		(strof(INT64)==buffer) ? sizeof(int64_t) :
		(strof(FLOAT)==buffer) ? sizeof(float) :
		(strof(DOUBLE)==buffer) ? sizeof(double) :
		(strof(LONG_DOUBLE)==buffer) ? sizeof(long double) :
		(strof(STRING)==buffer) ? 999 :
		0;
}

FormatTypes get_types(const char* argv[])
{
    FormatTypes ret;
 
    for (size_t i = 0; argv[i]; i++)
        ret.push_back(argv[i]); 
    return ret; 
}

UserBlkTps read_xtype(std::ifstream &ifs_conf)
{
	E2promMsg("Reading user data type configure");
	
	std::string line, buffer;
	UserBlkTps ret;
	char* c;
	int cline_nr = 0, comma_count;
	uint16_t id;
	FormatTypes ft;
	
	while (std::getline(ifs_conf, line)) {

		cline_nr++;
		c = const_cast<char*>(line.c_str());
		buffer.clear();
		comma_count = 0;
		
		while (!IS_EOL(*c) || !IS_EOS(*c)) {
	
			while (IS_LETTER(*c))
				buffer += *c++;
	
			if (IS_COLON(*c)) {
				ft.push_back(" "); /* placehold for block name */
				buffer.clear();
				c++;
				/* read number of rows and columns */
				while (true) {

					if (IS_DIGIT(*c)) {
						buffer += *c++;
					} else if (IS_COMMA(*c)) {
						comma_count++;
	
						if (1 == comma_count) {
							/* set user ID */
							id = to_digits<uint16_t>(buffer);
						} else {
							/* set number of rows or columns */
							ft.push_back(buffer);
						}
						buffer.clear();
						c++;
					} else if (IS_UPPER(*c)) {
						break;
					} else if (IS_SPACE(*c) || IS_TAB(*c)) {
						c++;
					} else {
						throw E2promValueException("Invalid character found",
							to_string<int>(cline_nr));
					}
				} /* end of read number of rows and columns */
				
				/* read data type and size of data in each format */
				for (uint16_t i = 0; i < to_digits<uint16_t>(ft[2]); i++) {

					while (IS_SPACE(*c)) c++;
					while (IS_UPPER(*c) || IS_DIGIT(*c)) 
						buffer += *c++;

					if (strof(NONE) == buffer) {
						throw E2promValueException("None-type found",
							to_string<int>(cline_nr));
					}
					/* add format type */
					ft.push_back(buffer);
					buffer.clear();
	
					while (IS_SPACE(*++c));
					while (IS_DIGIT(*c)) buffer += *c++;
					/* deal with UNLTD */
					while (IS_UPPER(*c)) buffer += *c++;
	
					/* add length of format type */
					ft.push_back(buffer);
					buffer.clear();
					c++;
				}
	
				break;
			} else if (IS_TAB(*c)) {
				break;
	//			while(IS_SPACE(*++c));
			} else if (IS_ULINE(*c)) {

				while (IS_ULINE(*c) || IS_LETTER(*c))
					buffer += *c++;
	
				ft[0] = buffer;
				ret[id] = ft;
				ft.clear();
				break;
			} else {
				throw E2promTypeException("Invalid character found",
					to_string<int>(cline_nr));
			} /* end of reading a line */
		} /* end of reading format types in a user block*/
	} /* end of getline while-loop */

	return ret;
}

//void read_USER(std::string &line, UserBlkTps &types, std::ofstream &ofs)
void read_USER(char *raw/*, UserBlkTps &types*/, std::ofstream &ofs, decode_result &decres)
{
	E2promMsg("Reading user block");
	
	UserBlockHeader uheader;
	std::stringstream ss;//(uh_buf);
	std::string uh_buf, data, chsum_stat, username;
	uint16_t cur_s = 0;
	uint8_t fix_by8 = 0;
	
	decres.clear_cur_user();

	/* reading header of user block */
	for (size_t i = 0 ; i < sizeof(UserBlockHeader); i++) 
		uh_buf += raw[decres.byte_nr++];
	
	ss.write(uh_buf.c_str(), sizeof(UserBlockHeader));
	ss >> uheader;

	cur_s += sizeof(UserBlockHeader);
	
	if (ALIGN_32 == decres.GLOBAL_ALIGNMENT) {
		decres.byte_nr += sizeof(uint16_t);
	//	cur_s += sizeof(uint16_t);
	}
	
	/* reading formats in user block
	 * Structure of FormatTypes
	 * +-------------------------------------------------->
	 * |0            |1   |2      |3      |4     |n      |n+1 ...
	 * +-------------------------------------------------->
	 * |name of block|rows|columns|type id|length|type id|length ...
	 * +-------------------------------------------------->
	 */
	if (!global_xtype.count(uheader.ID))
		throw E2promValueException("No type configure for user id",
			"USER_"+to_string<uint16_t>(uheader.ID));
		
	FormatTypes ft = global_xtype[uheader.ID];
	
	if (ft.empty())
		throw E2promValueException("Empty user format types",
			to_string<int>(decres.byte_nr++));
	
	std::string blk_name = ft[0];
	/* FormatTypes index */
	size_t n = 3;
	/* size of type */
	uint16_t type_sz;
	
	/* write ASCII file */
	write_file(ofs, "# User block " << blk_name << '\n');
	print_item_with_interval_l(ofs, std::string("ID:"),
	std::string(strof(USER))+"_"+to_string<uint16_t>(uheader.ID)+"\n");

	std::string buffer;
	uint16_t howmany = 0;
	std::vector<std::string> buf;
	
	if (ALIGN_32 == decres.GLOBAL_ALIGNMENT) {
		uint16_t fix_by = 0;

		ss.write(reinterpret_cast<char*>(&fix_by), sizeof(fix_by));
		cur_s += sizeof(fix_by);
	}

	for (; cur_s < uheader.blocksize; n += 2) {

		/* end of useful data is reached */
		if (n >= ft.size()) {
			decres.byte_nr += uheader.blocksize-cur_s;
			cur_s += uheader.blocksize-cur_s;
			break;
		}
		print_item_with_interval_l(ofs, "FORMAT:", ft[n]+"\n");
		howmany = to_digits<uint16_t>(ft[n+1]);
		
		/* met string-type data */
		if (999 == (type_sz = get_sizeof(ft[n]))) {

			for (uint16_t i = 0; i < howmany; i++) {
			
				std::string s;
				write_file(ofs, '"');
				
				while (!IS_EOS(raw[decres.byte_nr])) {
				
					write_file(ofs, raw[decres.byte_nr]);
					s += raw[decres.byte_nr];
					cur_s++;
					data += raw[decres.byte_nr++];
				}
				buf.push_back(s);
				write_file(ofs, "\"\n");
				cur_s++;
				data += raw[decres.byte_nr++];
			}
		} else { /* non-string type data */
		
			buffer.clear();
			/* read byte buffer for a format */
			for (uint16_t i = 0; i < howmany; i++){
				for (uint16_t j = 0; j < type_sz; j++) {

					data += raw[decres.byte_nr+j];
					//buffer += *const_cast<char*>(&line[decres.byte_nr+j]);
					buffer += raw[decres.byte_nr+j];
				}
				
				decres.byte_nr += type_sz;
				cur_s += type_sz;
			}
			/* reading a non-string format */
			switch (get_type_id(ft[n]))
		
				{ case UINT8: {
					UserFormatColumn<uint8_t> ufc(uheader);
					ufc.write_ascii(buffer, ofs, decres);
					break;
				} case INT8: {
					UserFormatColumn<int8_t> ufc(uheader);
					ufc.write_ascii(buffer, ofs, decres);
					break;
				} case UINT16: {
					UserFormatColumn<uint16_t> ufc(uheader);
					ufc.write_ascii(buffer, ofs, decres);
					break;
				} case INT16: {
					UserFormatColumn<int16_t> ufc(uheader);
					ufc.write_ascii(buffer, ofs, decres);
					break;
				} case UINT32: {
					UserFormatColumn<uint32_t> ufc(uheader);
					ufc.write_ascii(buffer, ofs, decres);
					break;
				} case INT32: {
					UserFormatColumn<int32_t> ufc(uheader);
					ufc.write_ascii(buffer, ofs, decres);
					break;
				} case UINT64: {
					UserFormatColumn<uint64_t> ufc(uheader);
					ufc.write_ascii(buffer, ofs, decres);
					break;
				} case INT64: {
					UserFormatColumn<int64_t> ufc(uheader);
					ufc.write_ascii(buffer, ofs, decres);
					break;
				} case DOUBLE: {
					UserFormatColumn<double> ufc(uheader);
					ufc.write_ascii(buffer, ofs, decres);
					break;
				} case FLOAT: {
					UserFormatColumn<float> ufc(uheader);
					ufc.write_ascii(buffer, ofs, decres);
					break;
				} case LONG_DOUBLE: {
					UserFormatColumn<long double> ufc(uheader);
					ufc.write_ascii(buffer, ofs, decres);
					break;
				} default:;
			} /* end of reading a non-string format */
		} /* end of reading a format */
	}
	/* end of reading formats in user block */
	
	ss.write(data.c_str(), data.size());

	/* fix alignment for user block header */
	if (ALIGN_32 == decres.GLOBAL_ALIGNMENT)
		cur_s += sizeof(uint16_t);

	/* fix alignment for the whole user block */
	while (need_fix_align(cur_s, decres.GLOBAL_ALIGNMENT)) {
		cur_s++;
		ss.write(reinterpret_cast<char*>(&fix_by8), sizeof(fix_by8));
	}
	
	chsum_stat = is_checksum16_correct<UserBlockHeader>(uheader, ss.str()) ? CRC16_CORRECT : CRC16_INCORRECT;

	if (buf.size()) {
		UserHeader uh(uheader.ID, buf.at(buf.size()-1));
		username = buf.at(buf.size()-1);
		decres.all_user_string[uh] = buf;
		decres.all_user.push_back(uh);
		decres.set_all_user(uheader.ID, username);
	}
	
	write_file(ofs, "# " << chsum_stat << block_gap());
	/* end of ASCII-file writing */
}

//void read_END(std::string &line, std::ofstream &ofs)
void read_END(char *raw, std::ofstream &ofs, decode_result &decres)
{
	E2promMsg("Reading end block");
	
	decres.byte_nr++;
	print_end(ofs);
}

void check_end_mark(char *raw, decode_result &decres)
{
	size_t nr(0);

	/* find end mark */
	for (; nr < GLOBAL_MAX_BIN_SIZE; nr++)
		if (0xffff == get_id(raw, nr)) {
			nr +=2;
			break;
		}

	if (nr == GLOBAL_MAX_BIN_SIZE) {
		throw E2promTypeException("Unknown ID read in byte string",
			to_string<int>(decres.byte_nr), "End mark(0xffff) is missing, incomplete package");
	}
}

//void read_block(std::string &line, std::ifstream &ifs_conf, std::ofstream &ofs)
decode_result read_block(char *raw, std::ifstream &ifs_conf, std::ofstream &ofs)
{
	uint16_t id = 0;

//	initialize();
	decode_result decres;

	if (!raw) 
		throw E2promTypeException("No data for decoding", to_string<int>(decres.byte_nr));

	/* skip the first 2 types, which indicates payload size */
//	decres.byte_nr += 2;

	if (WRITE_FILE) /* decode with bin2ascii */
		global_xtype = read_xtype(ifs_conf);
	else if (global_xtype.empty())
		throw E2promTypeException("No xtype configure infomation",
			to_string<int>(decres.byte_nr));
	
	while (!decres.end_reached && !all_bytes_read(decres)) {
		
		id = read_id(raw, decres);
		E2promMsg("Preparing for reading block with ID "+to_string<uint16_t>(id));
	
		if (IS_VALID_ID(IDENT, id)) {
			read_IDENT(raw, ofs, decres);
		} else if (IS_VALID_ID(MODIFICATION, id)) {
			read_MODIFICATION(raw, ofs, decres);
		} else if (IS_VALID_ID(SERVICE, id)) {
			read_SERVICE(raw, ofs, decres);
		} else if (IS_VALID_ID(STATISTICS, id)) {
			read_STATISTICS(raw, ofs, decres);
		} else if (IS_VALID_ID(USER, id)) {
			read_USER(raw, ofs, decres);
		} else if (IS_VALID_ID(END, id)) {
			decres.end_reached = true;
			read_END(raw, ofs, decres);
		} else {
			check_end_mark(raw, decres);
			throw E2promTypeException("Unknown ID read in byte string",
				to_string<int>(decres.byte_nr));
		}
	} /* end of reading-binary-raw while-loop */
	
	if (!decres.end_reached)
		E2promErrMsg("EEPROM data is incomplete!!");
	return decres;
}
#if 0
void clear_empty_byte(std::string &line)
{
	size_t nr(0);
	uint16_t id;

	/* find end mark */
	for (; nr < line.size(); nr++) {
		
		id = (*reinterpret_cast<uint16_t*>(&line[nr])) << 8;
		id += (*reinterpret_cast<uint8_t*>(&line[nr+1]));
		
		if (0xffff == id) {
			nr +=2;
			break;
		}
	}

	line.erase(nr, line.size()-nr);
}
#endif
void dealing_bin(std::ifstream &ifs_bin, std::ifstream &ifs_conf, std::ofstream &ofs)
{
//	std::string line;
//  std::string line((std::istreambuf_iterator<char>(ifs_bin)), 
//  std::istreambuf_iterator<char>());

//	if (std::getline(ifs_bin, line)) {
	char *raw = new char [GLOBAL_MAX_BIN_SIZE];
	ifs_bin.read(raw, GLOBAL_MAX_BIN_SIZE);
#if DEBUG
//		debug_bin(line);
#endif
//		clear_empty_byte();
		read_block(raw, ifs_conf, ofs);
//	} else {
//		E2promErrMsg("Can't read EEPROM data");
//	}

	E2promMsg("Done with binary file");
	delete raw;
}

ns_end(e2prom)
