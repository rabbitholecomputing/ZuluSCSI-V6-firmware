//	Copyright (C) 2016 Michael McMaster <michael@codesrc.com>
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

#include "bootloader.h"

#ifdef STM32F4xx
#include "stm32f4xx_hal.h"
#endif

#define SYS_DFU_MAGIC 0xDEADBEEF
// "System Memory" address of the bootloader. Specific to stm32f2xxxx
#define SYSMEM_RESET_VECTOR            0x1fff0000
extern void OrigSystemInit(void);

//--------------------------------------------------------------------+
// Constant - from lib/tinyuf2/src/board_api.h
//--------------------------------------------------------------------+
#define DBL_TAP_MAGIC            0xf01669ef // Enter DFU magic
#define DBL_TAP_MAGIC_QUICK_BOOT 0xf02669ef // Skip double tap delay detection
#define DBL_TAP_MAGIC_ERASE_APP  0xf5e80ab4 // Erase entire application !!
// defined by linker script - also taken from tinyuf2
extern uint32_t _board_dfu_dbl_tap[];
#define DBL_TAP_REG   _board_dfu_dbl_tap[0]


// This symbol is in a section of ram that isn't initialised by the
// Reset_Handler that calls SystemInit.
extern uint32_t _bootloaderMagic[];

// Override STM32CubeMX supplied SystemInit method.
void SystemInit(void)
{
	if (_bootloaderMagic[0] == SYS_DFU_MAGIC)
	{

#ifdef ENABLE_MPU_DFU_MODE
		void (*bootloader)(void) = (void (*)(void)) (*(uint32_t*)(SYSMEM_RESET_VECTOR + 4));
		_bootloaderMagic[0] = 0;
		__set_MSP(* ((uint32_t*)SYSMEM_RESET_VECTOR));
		bootloader();
		while (1) {}
#else
		_bootloaderMagic[0] = 0;
		NVIC_SystemReset();
#endif
	}
	else
	{
		OrigSystemInit();
	}
}

void s2s_enterBootloader()
{
	_bootloaderMagic[0] = SYS_DFU_MAGIC;
#ifndef ENABLE_MPU_DFU_MODE
	DBL_TAP_REG = DBL_TAP_MAGIC;
#endif
	NVIC_SystemReset();
}

