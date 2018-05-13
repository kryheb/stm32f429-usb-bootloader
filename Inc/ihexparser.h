/*
 ============================================================================
 Name        : ihexparser.h
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : Intel hex parser interface
 ============================================================================
 */

#ifndef IHEXPARSER_H_
#define IHEXPARSER_H_

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "defs.h"

#define BYTE_STR_LEN        2
#define MIN_BYTES_SIZE      5

#define SIZE_STR_OFFSET                     1
#define SIZE_INDEX                      0
#define ADDR_INDEX_MSB                  SIZE_INDEX+1
#define ADDR_INDEX_LSB                  ADDR_INDEX_MSB+1
#define RTYPE_INDEX                     ADDR_INDEX_LSB+1
#define DATA_INDEX                      RTYPE_INDEX+1
#define CHECKSUM_INDEX(_data_size)      DATA_INDEX+_data_size


typedef enum {
  RTYPE_DATA,
  RTYPE_EOF,
  RTYPE_EXT_SEG_ADDR,
  RTYPE_START_SEG_ADDR,
  RTYPE_EXT_LINEAR_ADDR,
  RTYPE_START_LINEAR_ADDR
} RecordType_t;

typedef struct {
  uint8_t size;
  uint16_t addr;
  RecordType_t rtype;
  uint8_t* data;
  uint8_t checksum;
} HexLine_t;

BootloaderStatus_t parse_line(HexLine_t* _hex_line, const char* _string);

#endif /* IHEXPARSER_H_ */
