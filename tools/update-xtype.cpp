/* 
 * update_xtype.cpp
 *
 * Tool for updating xtype header and source from xtype.ini
 *
 * Author: Zex <zex@hytera.com>
 */
#include <tools/bin-tool.hpp>
#include <fcntl.h>
#include <unistd.h>
#include "version.h"

use_ns(e2prom)

const char* xtype_header = "include/typlate/xtype.hpp";
const char* xtype_src = "lib/typlate/xtype.cpp";

void generate_xtype_header(UserBlkTps xt)
{
	E2promMsg(PROJECT+std::string(" utility update-xtype V")+VERSION);
	std::ofstream oxt;
	oxt.open(xtype_header, std::ios::out);

	oxt << "/* Autogenerated file for xtype configuration\n";
	oxt << " * by running bin2ascii with xtype.ini file.\n";
	oxt << " * The xtype header should be updated by developer\n";
	oxt << " * once the xtype.ini configuration is updated.\n";
	oxt << " * --------------! DO NOT EDIT -----------------*/\n";
	oxt << "#ifndef XTYPE_HPP_\n";
	oxt << "#define XTYPE_HPP_\n\n";
	oxt << "#include <tools/bin-tool.hpp>\n\n";
	oxt << "use_ns(e2prom)\n\n";

	/* xtype.hpp, xtype.ini data */
	for (auto &x : xt) {
		oxt << "extern const char* user_" << x.first << 
		"[" << x.second.size()+1 << "]" << ";\n";
	}

	oxt << "extern void init_xtype(UserBlkTps &ubt);\n";
	oxt << "#endif /* XTYPE_HPP_ */\n\n";
	oxt.close();

	/* xtype.cpp */
	oxt.open(xtype_src, std::ios::out);
	oxt << "/* Autogenerated file for xtype configuration\n";
	oxt << " * by running bin2ascii with xtype.ini file.\n";
	oxt << " * The xtype header should be updated by developer\n";
	oxt << " * once the xtype.ini configuration is updated.\n";
	oxt << " * --------------! DO NOT EDIT -----------------*/\n";
	oxt << "#include <typlate/xtype.hpp>\n\n";

	for (auto &x : xt) {
		oxt << "const char* user_" << x.first <<// " = {";
		"[" << x.second.size()+1 << "] = {";
		for (auto &y : x.second)
			oxt << "\"" << y << "\", ";
		oxt << "0};\n";
	}

	oxt << "\nvoid init_xtype(UserBlkTps& ubt)\n{\n";
	
	for (auto &x : xt) {
		oxt << "\tubt[" << x.first << "] = get_types(user_" << x.first << ");\n";
	}

	oxt << "}\n\n";

	oxt.close();
}

bool need_backup(const char *file_n)
{
	return path_exist(file_n);
}

int backup_file(const char* file)
{
	std::string file_ori(file);
	struct stat stat_ori;
	off_t offs = 0;
	int fd_in, fd_out;

	file_ori += ".ori";

	if (0 > stat(file, &stat_ori))
		return -1;

	fd_in = open(file, O_RDONLY);
	fd_out = open(file_ori.c_str(), O_CREAT|O_WRONLY, 0644);

	if (0 > sendfile(fd_out, fd_in, &offs, stat_ori.st_size))
		perror("sendfile");

	close(fd_in);
	close(fd_out);

	if (offs != stat_ori.st_size)
		return -1;

	return 0;
}

int backup_original_xtype()
{
	int ret = 0;

	if (need_backup(xtype_header))
		ret += backup_file(xtype_header);
			
	if (need_backup(xtype_src))
		ret += backup_file(xtype_src);

	return ret;
}

int main(int argc, char *argv[])
{
	if (2 > argc) return -1;

	std::ifstream ifs_conf;

	if (backup_original_xtype()) {
		E2promErrMsg("Backup original xtype failed\n");
		return -1;
	}

	ifs_conf.open(argv[1], std::ios::in);
	generate_xtype_header(read_xtype(ifs_conf));

	ifs_conf.close();

	return 0;
}
