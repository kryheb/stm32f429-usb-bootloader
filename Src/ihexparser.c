/*
 ============================================================================
 Name        : ihexparser.c
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : Intel hex parser
 ============================================================================
 */

#include "ihexparser.h"
#include <stdbool.h>


static bool is_valid(const uint8_t* _raw_bytes, const size_t _size, const uint8_t _expected_checksum)
{
  // calculate two's complement
  int i;
  uint8_t sum = 0;

  for (i=0; i<(_size-1); i++) {
    sum += _raw_bytes[i];
  }

  sum ^= 0xFF;
  sum++;

  return (sum == _expected_checksum);
}


/*
 * HEX DATA LINE:
 * :1042FC004FF090424FF090439B6943F4001393614D
 * DATA SIZE  | ADDR OFFSET   | RECORD TYPE   | DATA                              | CHECKSUM
 * 10         | 42FC          | 00            | 4FF090424FF090439B6943F400139361  | 4D
 */

HexParserState_t parse_record(HexRec_t* _hex_rec, uint8_t* _buffer, size_t _buff_len)
{
  _hex_rec->size = _buffer[HP_SIZE_INDEX];
  if (_buff_len < (_hex_rec->size + HP_MIN_RECORD_SIZE)) {
    return HEX_PARSER_INVALID_LEN;
  }

  size_t checksum_index = HP_CHECKSUM_INDEX(_hex_rec->size);
  _hex_rec->checksum = _buffer[checksum_index];

  if (!is_valid(_buffer, checksum_index, _hex_rec->checksum)) {
    return HEX_PARSER_INVALID_CHECKSUM;
  }

  _hex_rec->addr = (_buffer[HP_ADDR_INDEX_MSB] << 8) | _buffer[HP_ADDR_INDEX_LSB];
  _hex_rec->rtype = _buffer[HP_RTYPE_INDEX];
  _hex_rec->data = (_hex_rec->size != 0) ? (_buffer + HP_DATA_INDEX) : NULL;

  return HEX_PARSER_OK;
}






