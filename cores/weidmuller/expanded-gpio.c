 /*
* This file incorporates work covered by the following copyright and
* permission notice:
*/

/*
Copyright (C) 2022 Weidmüller SA. All rights reserved.
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "expanded-gpio.h"

#include "esp32-hal-i2c.h"
#include "esp32-hal-log.h"
#include "pins_arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp32-hal-ledc.h"

static const uint8_t i2cNum = 0;
static const uint8_t i2cGeneralCallAddress = 0x00;
static const uint8_t i2cSoftReset = 0b00000110;

extern void __pinMode(uint32_t pin, uint8_t mode);
extern void __digitalWrite(uint32_t pin, uint8_t value);
extern int __digitalRead(uint32_t pin);
extern uint16_t __analogRead(uint32_t pin);

#define isExpandedPin(pin) (pin > 0xff)
#define expandedPinToDeviceAddress(p) ((p >> 8) & 0xff)
#define expandedPinToDeviceSecondAddress(p) ((p >> 16) & 0xff)
#define expandedPinToDeviceIndex(p) (p & 0xff)

// MCP23008
#ifdef HAVE_MCP23008
#include "peripheral-mcp23008.h"
#endif

// MAX11613
#ifdef HAVE_MAX11613
#include "peripheral-max11613.h"
#endif


static bool isAddressIntoArray(uint8_t addr, const uint8_t* arr, uint8_t len) {
	while (len--) {
		if (*arr++ == addr) {
			return true;
		}
	}
	return false;
}

void initExpandedGpio() {
	if (!i2cIsInit(i2cNum)) {
		if (i2cInit(i2cNum, SDA, SCL, 0) != ESP_OK) {
			return;
		}
	}

	// Reset devices
	uint8_t resetFunction = i2cSoftReset;
	if (i2cWrite(i2cNum, i2cGeneralCallAddress, &resetFunction, sizeof(resetFunction), 50) != ESP_OK) {
		return;
	}
	delay(10);


#ifdef HAVE_MCP23008
	for (int i = 0; i < NUM_MCP23008_DEVICES; ++i) {
		mcp23008_init(i2cNum, mcp23008Addresses[i]);
	}
#endif
	delay(5);
}

void pinMode(uint32_t pin, uint8_t mode) {
	if (!isExpandedPin(pin)) {
		__pinMode(pin, mode);
		return;
	}

	uint8_t addr = expandedPinToDeviceAddress(pin);
	uint8_t index = expandedPinToDeviceIndex(pin);

#ifdef HAVE_MCP23008
	if (isAddressIntoArray(addr, mcp23008Addresses, NUM_MCP23008_DEVICES)) {
		mcp23008_set_pin_mode(i2cNum, addr, index, mode);
	}
#endif
}

void digitalWrite(uint32_t pin, uint8_t value) {
	if (!isExpandedPin(pin)) {
		__digitalWrite(pin, value);
		return;
	}

	uint8_t addr = expandedPinToDeviceAddress(pin);
	uint8_t index = expandedPinToDeviceIndex(pin);


#ifdef HAVE_MCP23008
	if (isAddressIntoArray(addr, mcp23008Addresses, NUM_MCP23008_DEVICES)) {
		mcp23008_set_output(i2cNum, addr, index, value);
	}
#endif
}

int digitalRead(uint32_t pin) {
	if (!isExpandedPin(pin)) {
		return __digitalRead(pin);
	}

	uint8_t addr = expandedPinToDeviceAddress(pin);
	uint8_t index = expandedPinToDeviceIndex(pin);

#ifdef HAVE_MCP23008
	if (isAddressIntoArray(addr, mcp23008Addresses, NUM_MCP23008_DEVICES)) {
		return mcp23008_get_input(i2cNum, addr, index);
	}
#endif

	return LOW;
}



uint16_t analogRead(uint32_t pin) {
	if (!isExpandedPin(pin)) {
		return __analogRead(pin);
	}

	uint8_t addr = expandedPinToDeviceSecondAddress(pin);
	uint8_t index = expandedPinToDeviceIndex(pin);

#ifdef HAVE_MAX11613
	return max11613_get_input(i2cNum, addr, index);
#endif

	return 0;
}
