//	Copyright (C) 2020 Michael McMaster <michael@codesrc.com>
//  ZuluSCSI™ - Portions Copyright (c) 2024 Rabbit Hole Computing
//
//	This file is part of SCSI2SD.
//  This file has been modified for use with ZuluSCSI™
//
//	SCSI2SD is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	SCSI2SD is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with SCSI2SD.  If not, see <http://www.gnu.org/licenses/>.

#include "hwversion.h"

#ifdef STM32F4xx
#include "stm32f4xx.h"
#endif

#include "gpio.h"

#include "config.h"
#include "time.h"

// Store hardware version details to the "One Time Programmable" flash memory
// This is 512 bytes that can only be written to ONCE and once only.
// It can be read by dfu-util, so once we write the marker it can be
// detected even when the firmware isn't running.

// Values for STM32F401RE
#define OTP_SIZE         512
#define OTP_BLOCKS       16
#define OTP_BLOCK_SIZE   OTP_SIZE / OTP_BLOCKS
#define OTP_BLOCK_NUM    0
#define OTP_POINTER      FLASH_OTP_BASE + OTP_BLOCK_NUM * OTP_BLOCK_SIZE
#define OTP_LOCK_POINTER FLASH_OTP_BASE + OTP_SIZE + OTP_BLOCK_NUM



// Define some pointers for writing, but also to allow easy reading back values
const volatile uint8_t *otp = (uint8_t*) OTP_POINTER;
const volatile uint32_t *otp32 = (uint32_t*) OTP_POINTER;
const volatile uint8_t *lock = (uint8_t*) OTP_LOCK_POINTER;

#ifdef ZULUSCSI_V6_4
const uint32_t marker = 0x64000000;
#endif

static void
checkHwSensePins()
{

#ifdef ZULUSCSI_V6_4
    if ((DBGMCU->IDCODE & 0x00000FFF) != 0x0421)
    {
		// Oh dear, wrong cpu. Do not pass go.
		while (1) {}
    }
#endif
}

void
s2s_checkHwVersion()
{
#ifndef SKIP_OTP_MARKER_CHECK
	checkHwSensePins();

	// Write a marker to flash that can be read by dfu-util now that we know
	// the version is correct.
	if (*otp32 != marker)
	{
		// Double-check the pins are good.
		s2s_delay_ms(10);
		checkHwSensePins();

		// Well, pins are good. Make sure marker isn't set at all
		if (*otp32 != 0xffffffff)
		{
			// Some other version was set.
			while (1) {}
		}

		// Write the marker to flash.
		if (HAL_FLASH_Unlock() != HAL_OK)
		{
			return;
		}

		// Write 4 bytes to the start of OTP.
		if (HAL_FLASH_Program(
			FLASH_TYPEPROGRAM_WORD,
			(uint32_t)otp,
			marker) != HAL_OK)
		{
			HAL_FLASH_Lock();
			return;
		}

		// Lock OTP page
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, (uint32_t)lock, 0x00)) {
			HAL_FLASH_Lock();
			return;
		}

		HAL_FLASH_Lock();
	}
#endif
}

