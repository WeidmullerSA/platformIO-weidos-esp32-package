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

#ifndef __PERIPHERAL_ADS1015_H__
#define __PERIPHERAL_ADS1015_H__

#include "Arduino.h"
#include "esp32-hal-i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

bool max11613_init(uint8_t i2cNum, uint8_t addr);
uint16_t max11613_get_input(uint8_t i2cNum, uint8_t addr, uint8_t index);

#ifdef __cplusplus
}
#endif


#endif
