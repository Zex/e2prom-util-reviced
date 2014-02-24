/* 
 * checksum.hpp
 *
 * Data structures definition in E2PROM
 *
 * Author: Zex <zex@hytera.com>
 *
 *
 * See also:
 *  the E2prom Data Modules
 *  DIB-CP1 SAD
 */
#include <utils/checksum.hpp>

ns_begin(e2prom)

uint16_t crc16(const uint8_t *data, unsigned int end)
{
	uint16_t ret = 0;
	unsigned int i;

	for (i = 0; i < end; i += 2) {
		ret += (((unsigned int)(data[i+1]) << 8) + 
				 (((unsigned int)(data[i])) & 0x000000ff));
	}

	return (0x10000 - ret);
}

ns_end(e2prom)
