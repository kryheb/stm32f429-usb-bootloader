/*
 ============================================================================
 Name        : flash.c
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : Flash operations
 ============================================================================
 */

#include "flash.h"
#include "timeout.h"

/*
    1. Write KEY1 = 0x45670123 in the Flash key register (FLASH_KEYR)
    2. Write KEY2 = 0xCDEF89AB in the FLASH_KEYR register.
 */

void unlock_flash()
{
  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;
}


static BootloaderStatus_t busy_check()
{
  set_timeout(10);
  while(1)
  {
    if (!(FLASH->SR & FLASH_SR_BSY_Msk)) {
      return OK;
    } else if (has_timed_out()) {
      return BUSY_CHECK_TIMEOUT;
    }
  }
}


static void clear_prog_errs()
{
  FLASH->SR |=
      FLASH_SR_PGSERR  |
      FLASH_SR_PGAERR  |
      FLASH_SR_WRPERR  |
      FLASH_SR_PGPERR  |
      FLASH_SR_SOP;
}


/*
  Flash status register errors
  ------------------------------------------------------------------------------------------------------
	Bit 16 BSY: Busy
	This bit indicates that a Flash memory operation is in progress to/from one bank. It is set at the
	beginning of a Flash memory operation and cleared when the operation finishes or an error
	occurs.
  ------------------------------------------------------------------------------------------------------
	Bit 8 RDERR: Proprietary readout protection (PCROP) error
	Set by hardware when a read access through the D-bus is performed to an address belonging
	to a proprietary readout protected Flash sector.
	Cleared by writing 1.
  ------------------------------------------------------------------------------------------------------
	Bit 7 PGSERR: Programming sequence error
	Set by hardware when a write access to the Flash memory is performed by the code while the
	control register has not been correctly configured.
	Cleared by writing 1.
  ------------------------------------------------------------------------------------------------------
	Bit 6 PGPERR: Programming parallelism error
	Set by hardware when the size of the access (byte, half-word, word, double word) during the
	program sequence does not correspond to the parallelism configuration PSIZE (x8, x16, x32,
	x64).
	Cleared by writing 1.
  ------------------------------------------------------------------------------------------------------
	Bit 5 PGAERR: Programming alignment error
	Set by hardware when the data to program cannot be contained in the same 128-bit Flash
	memory row.
	Cleared by writing 1.
  ------------------------------------------------------------------------------------------------------
	Bit 4 WRPERR: Write protection error
	Set by hardware when an address to be erased/programmed belongs to a write-protected part
	of the Flash memory.
	Cleared by writing 1.
  ------------------------------------------------------------------------------------------------------
	Bit 1 OPERR: Operation error
	Set by hardware when a flash operation (programming/erase/read) request is detected and
	can not be run because of parallelism, alignment, write or read (PCROP) protection error. This
	bit is set only if error interrupts are enabled (ERRIE = 1).
  ------------------------------------------------------------------------------------------------------
	Bit 0 EOP: End of operation
	Set by hardware when one or more Flash memory operations (program/erase) has/have
	completed successfully. It is set only if the end of operation interrupts are enabled (EOPIE = 1).
	Cleared by writing a 1.
  ------------------------------------------------------------------------------------------------------
 */



static BootloaderStatus_t get_flash_error()
{
  if (FLASH->SR & FLASH_SR_PGSERR_Msk) {
    return FLASH_PROG_SEQ_ERROR;
  } else if (FLASH->SR & FLASH_SR_PGAERR_Msk) {
    return FLASH_PROG_ALIGNMENT_ERROR;
  } else if (FLASH->SR & FLASH_SR_WRPERR_Msk) {
    return FLASH_WRITE_PROTECTION_ERROR;
  } else if (FLASH->SR & FLASH_SR_PGPERR_Msk) {
    return FLASH_PROG_PARALLELISM_ERROR;
  } else {
    return OK;
  }
}

/*
	 Name      | Block base addresses      | Size
	 --------------------------------------------------
	 Sector 0  | 0x0800 0000 - 0x0800 3FFF | 16 Kbytes
	 Sector 1  | 0x0800 4000 - 0x0800 7FFF | 16 Kbytes
	 Sector 2  | 0x0800 8000 - 0x0800 BFFF | 16 Kbytes
	 Sector 3  | 0x0800 C000 - 0x0800 FFFF | 16 Kbyte
	 Sector 4  | 0x0801 0000 - 0x0801 FFFF | 64 Kbytes
	 Sector 5  | 0x0802 0000 - 0x0803 FFFF | 128 Kbytes
	 Sector 6  | 0x0804 0000 - 0x0805 FFFF | 128 Kbytes
	 - - -
	 Sector 11 | 0x080E 0000 - 0x080F FFFF | 128 Kbytes
 */

uint32_t get_sector_addr(uint8_t _sector)
{
  if (_sector==0) {
    return FLASH_BEGIN;
  } else if (_sector>5) {
    return 0x20000 + get_sector_addr(--_sector);
  } else if (_sector==5) {
    return 0x10000 + get_sector_addr(--_sector);
  } else if (_sector<4) {
    return 0x4000 + get_sector_addr(--_sector);
  }

  // sector out of range
  return 0;
}


/*
	Sector Erase
	To erase a sector, follow the procedure below:
	1. Check that no Flash memory operation is ongoing by checking the BSY bit in the
	FLASH_SR register
	2. Set the SER bit and select the sector out of the 12 sectors (for STM32F405xx/07xx and
	STM32F415xx/17xx) and out of 24 (for STM32F42xxx and STM32F43xxx) in the main
	memory block you wish to erase (SNB) in the FLASH_CR register
	3. Set the STRT bit in the FLASH_CR register
	4. Wait for the BSY bit to be cleared
 */

BootloaderStatus_t erase_sector(uint8_t _sector)
{
  if (_sector >= SECTOR_NUM) {
    return FLASH_SECTOR_OUT_OF_RANGE;
  }

  if (busy_check() != OK) {
    return BUSY_CHECK_TIMEOUT;
  }

  clear_prog_errs();

  FLASH->CR |= FLASH_CR_SER;
  FLASH->CR &= ~(0xFF << FLASH_CR_SNB_Pos);
  FLASH->CR |=  (_sector << FLASH_CR_SNB_Pos);
  FLASH->CR |= FLASH_CR_STRT;

  if (busy_check() != OK) {
    return BUSY_CHECK_TIMEOUT;
  }

  FLASH->CR &= ~(FLASH_CR_SER);

  return get_flash_error();
}



/*
	Standard programming
	The Flash memory programming sequence is as follows:
	1. Check that no main Flash memory operation is ongoing by checking the BSY bit in the
	FLASH_SR register.
	2. Set the PG bit in the FLASH_CR register
	3. Perform the data write operation(s) to the desired memory address (inside main
	memory block or OTP area):
	– Byte access in case of x8 parallelism
	– Half-word access in case of x16 parallelism
	– Word access in case of x32 parallelism
	– Double word access in case of x64 parallelism
	4. Wait for the BSY bit to be cleared.
	Note:
	Successive write operations are possible without the need of an erase operation when
	changing bits from ‘1’ to ‘0’. Writing ‘1’ requires a Flash memory erase operation.
	If an erase and a program operation are requested simultaneously, the erase operation is
	performed first.
 */

BootloaderStatus_t program_data(uint32_t _addr, uint8_t* _pdata, uint16_t _size)
{
  if (busy_check() != OK) {
    return BUSY_CHECK_TIMEOUT;
  }
  clear_prog_errs();

  FLASH->CR |= FLASH_CR_PG;

  int i;

  for (i=0; i<_size; i++) {
    uint32_t offset = i*8;
    *(__IO uint8_t*)(_addr + offset) = (uint8_t)_pdata[i];
    if (busy_check() != OK) {
      return BUSY_CHECK_TIMEOUT;
    }
  }

  FLASH->CR &= ~(FLASH_CR_PG);
  return get_flash_error();
}
