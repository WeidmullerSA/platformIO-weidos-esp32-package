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

#include "peripheral-max11613.h"
#include "esp32-hal-log.h"

//Registers values and masks




//Setup byte format
#define RESET_CONFIG_REG		0x00
#define NO_CONFIG_REG_RESET		0x02
#define UNIPOLAR_MODE			0x00
#define BIPOLAR_MODE			0x04
#define INTERNAL_CLOCK 			0x00
#define EXTERNAL_CLOCK			0x08
#define CONFIG_BYTE				0x00
#define SETUP_BYTE				0x80

//Configuration Byte 
#define DIFFERENTIAL_INPUT		0x00 
#define	SINGLE_ENDED_INPUT		0x01
#define AIN0					0x00
#define AIN1					0x01
#define AIN2					0x02
#define AIN3					0x03
#define SCAN_UP					0x00
#define MULTIPLE_CONVERSION		0x20
#define SCAN_UPPER_HALF			0x40
#define SINGE_SCAN				0x60
#define CONFIG_BYTE				0x00



static const uint16_t i2cTimeout = 50;

bool max11613_init(uint8_t i2cNum, uint8_t addr) {
	return true;
}

uint16_t max11613_get_input(uint8_t i2cNum, uint8_t addr, uint8_t index) {
	uint8_t mux = 0;
	switch (index) {
		case 0:
			mux = AIN0;
			break;

		case 1:
			mux = AIN1;
			break;

		case 2:
			mux = AIN2;
			break;

		case 3:
			mux = AIN3;
			break;
	}


	uint8_t buffer[2];
	//buffer[0] =  CONFIG_BYTE | SINGE_SCAN | mux<<1 | SINGLE_ENDED_INPUT;
	buffer[0] = SETUP_BYTE | NO_CONFIG_REG_RESET | UNIPOLAR_MODE | INTERNAL_CLOCK;
	buffer[1] = CONFIG_BYTE | SINGLE_ENDED_INPUT | mux<<1 | SINGE_SCAN;
//buffer[0] = 0b10010010;  
//buffer[1] = 0b01100011;
	if (i2cWrite(i2cNum, addr, buffer, 2, i2cTimeout) != ESP_OK) {
		return 5;
	}

	delay(1);

	//uint8_t convReg = 0;
	size_t len = 0;

	if(i2cRead(i2cNum, addr, buffer, 2, i2cTimeout, &len)){
		return 7;
	}	
	
	//if (i2cWriteReadNonStop(i2cNum, addr, &convReg, 0, buffer, 2, i2cTimeout, &len) != ESP_OK) {
	//	return 6;
	//}

	uint16_t res = (((buffer[0]&0x0F) << 8) | (buffer[1]));
	//res = buffer[1];
	/*if (res > 0x07ff) {
		res = 0;
	}*/

	return res;
}
