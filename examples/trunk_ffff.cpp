#include <iostream>
#include <fstream>
#include <unistd.h>

void trunk_ffff(char *raw)
{
	std::cout << __PRETTY_FUNCTION__ << '\n';
	size_t nr(0);
	uint16_t id;
	std::ofstream ofs;
	ofs.open("noffff.bin", std::ios::binary|std::ios::out);

	/* find end mark */
	for (; nr < 8192; nr++) {
		
		id = (*reinterpret_cast<uint16_t*>(&raw[nr])) << 8;
		id += (*reinterpret_cast<uint8_t*>(&raw[nr+1]));
		
		if (0xffff == id) {
			nr +=2;
			break;
		}

		ofs.write(const_cast<char*>(&raw[nr]), 1);
	}

	ofs.close();

//	line.erase(nr, line.size()-nr);
}

int main(int argc, char* argv[])
{
	if (2 > argc) return -1;
	
	std::ifstream ifs;
	char *raw = new char[8192];

	ifs.open(argv[1], std::ios::binary);
	ifs.read(raw, 8192);
	ifs.close();

	trunk_ffff(raw);
	delete raw;

	return 0;
}
