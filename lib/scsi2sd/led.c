//	Copyright (C) 2014 Michael McMaster <michael@codesrc.com>
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

#include "led.h"

#ifdef STM32F4xx
#include "stm32f4xx.h"
#endif

#include "gpio.h"
#include "time.h"

void s2s_ledInit()
{
	// LED GPIO pin is already initialised as an output with pull-down.
	// At this stage we can remove the pull-down
	s2s_ledOff();

	GPIO_InitTypeDef ledDef = {
		LED_IO_Pin, // Pin
		GPIO_MODE_OUTPUT_PP, // Mode
		GPIO_NOPULL, // Pull(ups)
		GPIO_SPEED_FREQ_LOW, // Speed (2MHz)
		0 // Alternate function
	};
	HAL_GPIO_Init(LED_IO_GPIO_Port, &ledDef);
}

static uint16_t blink_count = 0;
static uint32_t blink_start = 0;
static uint32_t blink_delay = 0;
static uint32_t blink_end_delay= 0;

void s2s_ledOn()
{
	if (blink_count > 0) return;
	HAL_GPIO_WritePin(LED_IO_GPIO_Port, LED_IO_Pin, GPIO_PIN_SET);
}

void s2s_ledOff()
{
	if (blink_count > 0) return;
	HAL_GPIO_WritePin(LED_IO_GPIO_Port, LED_IO_Pin, GPIO_PIN_RESET);
}


void s2s_ledBlink(uint8_t times, uint32_t delay, uint32_t end_delay)
{
	if (!s2s_ledBlinkPoll() && blink_count == 0)
	{
		blink_start = s2s_getTime_ms();
		blink_count = 2 * times;
		blink_delay = delay / 2;
		blink_end_delay =  end_delay;
		HAL_GPIO_WritePin(LED_IO_GPIO_Port, LED_IO_Pin, GPIO_PIN_SET);
	}

}

uint8_t s2s_ledBlinkPoll()
{
	uint8_t is_blinking = 1;
	if (blink_count == 0)
	{
		is_blinking = 0;
	}
	else if (blink_count == 1 && ((uint32_t)(s2s_getTime_ms() - blink_start)) > blink_end_delay )
	{
		HAL_GPIO_WritePin(LED_IO_GPIO_Port, LED_IO_Pin, GPIO_PIN_RESET);
		blink_count = 0;
		is_blinking = 0;
	}
	else if (blink_count > 1 && ((uint32_t)(s2s_getTime_ms() - blink_start)) > blink_delay)
	{
		if (1 & blink_count)
			HAL_GPIO_WritePin(LED_IO_GPIO_Port, LED_IO_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(LED_IO_GPIO_Port, LED_IO_Pin, GPIO_PIN_RESET);
		blink_count--;
		blink_start = s2s_getTime_ms();
	}
	return is_blinking;
}

void s2s_blinkCancel()
{
	blink_count = 0;
}