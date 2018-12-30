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

#define HP_MIN_RECORD_SIZE                 5
#define HP_SIZE_INDEX                      0
#define HP_ADDR_INDEX_MSB                  HP_SIZE_INDEX+1
#define HP_ADDR_INDEX_LSB                  HP_ADDR_INDEX_MSB+1
#define HP_RTYPE_INDEX                     HP_ADDR_INDEX_LSB+1
#define HP_DATA_INDEX                      HP_RTYPE_INDEX+1
#define HP_CHECKSUM_INDEX(_data_size)      HP_DATA_INDEX+_data_size

typedef enum {
  HEX_PARSER_OK,
  HEX_PARSER_INVALID_LEN,
  HEX_PARSER_INVALID_CHECKSUM,
  HEX_PARSER_PARSING_ERROR
} HexParserStatus_t;


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
} HexRec_t;

HexParserStatus_t parse_record(HexRec_t* _hex_rec, const uint8_t* _buffer, size_t _buff_len);

#endif /* IHEXPARSER_H_ */
