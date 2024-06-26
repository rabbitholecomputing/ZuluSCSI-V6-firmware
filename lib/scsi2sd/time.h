//	Copyright (C) 2014 Michael McMaster <michael@codesrc.com>
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
#ifndef S2S_TIME_H
#define S2S_TIME_H

#include <stdint.h>

void s2s_timeInit(void);
uint32_t s2s_getTime_ms(void); // Returns milliseconds since init
uint32_t s2s_diffTime_ms(uint32_t start, uint32_t end);
uint32_t s2s_elapsedTime_ms(uint32_t since);

// ZuluSCSI v6.4 is under-clocked to 108MHz
#ifdef ZULUSCSI_V6_4
    #define s2s_cpu_freq  108000000LL
#elif defined(STM32F4xx)
    #define s2s_cpu_freq 180000000LL
#endif

#define s2s_delay_ms(delay) s2s_delay_clocks((delay) * (s2s_cpu_freq / 1000))
#define s2s_delay_us(delay) s2s_delay_clocks((delay) * (s2s_cpu_freq / 1000000))
#define s2s_delay_ns(delay) s2s_delay_clocks(((delay) * ((s2s_cpu_freq * 64LL + 500000000) / 1000000000)) / 64)
void s2s_delay_clocks(uint32_t delay);

#endif
