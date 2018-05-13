/*
 ============================================================================
 Name        : defs.h
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : Common definitions
 ============================================================================
 */

#ifndef DEFS_H_
#define DEFS_H_

typedef enum {
  OK,
  HEXLINE_INVALID_LEN,
  HEXLINE_INVALID_CHECKSUM,
  HEXLINE_PARSING_ERROR,
  HEXLINE_MALLOC_ERROR,
  FLASH_SECTOR_OUT_OF_RANGE,
  FLASH_PROG_SEQ_ERROR,
  FLASH_PROG_PARALLELISM_ERROR,
  FLASH_PROG_ALIGNMENT_ERROR,
  FLASH_WRITE_PROTECTION_ERROR,
  FLASH_PROG_ERROR,
  BUSY_CHECK_TIMEOUT
} BootloaderStatus_t;


#endif /* DEFS_H_ */
