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
#include <stdint.h>
#include "basic.hpp"

ns_begin(e2prom)

extern uint16_t crc16(const uint8_t *data, unsigned int end);

#define CRC16_CORRECT "Checksum correct"
#define CRC16_INCORRECT "Checksum incorrect"

#define checksum16_match(data,end,checksum) (crc16(data,end) == checksum)

ns_end(e2prom)
