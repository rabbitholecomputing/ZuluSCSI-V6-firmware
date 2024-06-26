//	Copyright (C) 2013 Michael McMaster <michael@codesrc.com>
//
//	This file is part of SCSI2SD.
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
#ifndef S2S_LED_H
#define S2S_LED_H
#include <stdint.h>

void s2s_ledInit(void);
void s2s_ledOn(void);
void s2s_ledOff(void);
void s2s_ledBlink(uint8_t times, uint32_t delay, uint32_t end_delay);
uint8_t s2s_ledBlinkPoll();
void s2s_blinkCancel();
#endif
