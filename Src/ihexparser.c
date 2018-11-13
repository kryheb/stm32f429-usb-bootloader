/*
 ============================================================================
 Name        : ihexparser.c
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : Intel hex parser
 ============================================================================
 */

#include "ihexparser.h"


static BootloaderStatus_t check_valid(const uint8_t* _raw_bytes, const size_t _size, const uint8_t _expected_checksum)
{
  // calculate two's complement
  int i;
  uint8_t sum = 0;

  for (i=0; i<(_size-1); i++) {
    sum += _raw_bytes[i];
  }

  sum ^= 0xFF;
  sum++;

  return (sum == _expected_checksum) ? OK : HEXLINE_INVALID_CHECKSUM;
}


BootloaderStatus_t parse_line(HexLine_t* _hex_line, const char* _string)
{
  size_t byte_cnt = strlen(_string)/2;

  if (byte_cnt < MIN_BYTES_SIZE) {
    return HEXLINE_INVALID_LEN;
  }

  BootloaderStatus_t ret_value;
  uint8_t* bytes = malloc(sizeof(uint8_t) * byte_cnt);

  if (bytes) {
    int i;
    char* str_8 = calloc(sizeof(char), BYTE_STR_LEN+1);

    for (i=0; i<byte_cnt; i++) {
      size_t str_offset = SIZE_STR_OFFSET +(i * BYTE_STR_LEN);
      strncpy(str_8, _string+str_offset, BYTE_STR_LEN);
      uint8_t byte = strtoul(str_8, NULL, 16);

      if (byte == 0 && (strncmp(str_8, "00", BYTE_STR_LEN) != 0)) {
        ret_value = HEXLINE_PARSING_ERROR;
        goto FINISH;
      }

      bytes[i] = byte;
    }
    free(str_8);
  } else {
    return HEXLINE_MALLOC_ERROR;
  }

  /*
   * HEX DATA LINE:
   * :1042FC004FF090424FF090439B6943F4001393614D
   * DATA SIZE  | ADDR OFFSET   | RECORD TYPE   | DATA                              | CHECKSUM
   * 10         | 42FC          | 00            | 4FF090424FF090439B6943F400139361  | 4D
   */

  _hex_line->size = bytes[SIZE_INDEX];
  _hex_line->checksum = bytes[CHECKSUM_INDEX(_hex_line->size)];
  ret_value = check_valid(bytes, byte_cnt, _hex_line->checksum);

  if (ret_value != OK) {
    goto FINISH;
  }

  _hex_line->addr = bytes[ADDR_INDEX_MSB]<<8 | bytes[ADDR_INDEX_LSB];
  _hex_line->rtype = (RecordType_t)bytes[RTYPE_INDEX];

  if (_hex_line->size) {
    _hex_line->data = malloc(sizeof(uint8_t) * _hex_line->size);
    if (_hex_line->data) {
      memcpy(_hex_line->data, bytes+DATA_INDEX, _hex_line->size);
    }
  }

  FINISH:
  free(bytes);

  return ret_value;
}





