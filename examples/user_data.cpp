#include <tools/block-tool.hpp>
#include <string.h>

using namespace std;
using namespace e2prom;

int main(int argc, char* argv[])
{
	stringstream ss;

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	encode_result encres;
	HeaderBlock hb;

	hb.ID = 2;
	hb.ALIGNMENT = 0x10;
	hb.EEPROMSIZE = 16384;
	hb.PARTNUMBER = 11229737;
	hb.PARTNUMBER_RESV = 9;
	hb.HWCODE = 0;
	hb.PRODUCTINDEX = 233;
	hb.SN = 101163009;
	hb.PRODUCTDATE = 0x3e7c;
	hb.READCODE = 0;
	hb.TESTINSTRUCTION = 504;
	string device_name("PowerQuicc II");
	strcpy(hb.NAME, device_name.c_str());

	set_checksum<HeaderBlock>(hb);

	ss << hb;
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	UserBlockHeader bh(18502);
	UserFormatColumn<uint32_t> ufc(bh);
	ufc += 8388608;
	ufc += 486000000;
	ufc += 486000000;

	std::string fmstr = strize_formats<uint32_t>(ufc);

	bh.set_checksum(fmstr, encres);
	ss << bh;
	ss << ufc;

	EndBlock end;
	ss << end;
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	cout << ss.str();


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	stringstream ss_enc;
	decode_result decres;

	ss_enc << ufc;

	//--------------------------------------

	UserBlockHeader bh_dec(18502);
	UserFormatColumn<uint32_t> ufc_dec(bh_dec);

	stringstream ss_dec(ss_enc.str());

	cout << "\nufc_dec data<\n";

	//--------------------------------------
	std::string s_dec(ss_dec.str());
	ufc_dec.write_ascii(s_dec, cout, decres);
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//	UserFormatColumn<uint16_t> ufc2(bh);
//	std::string fmstr2 = strize_formats<uint32_t,uint16_t>(ufc, ufc2);
//
//	cout << "...............";
//	cout << ss.str();

	return 0;
}
