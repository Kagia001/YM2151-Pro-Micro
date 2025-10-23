/**
*	YM2151 - Chip Controller Software for ARDUINO
*	(C) 2016  Marcel Weiß
*
*	This program is free software : you can redistribute it and / or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#include  <EEPROM.h>

#include "EPROMManager.h"


void EPROMManagerClass::init()
{
}

uint8_t EPROMManagerClass::loadByte(uint16_t address)
{
	// No, we are not allocating RAM here!!!
	// uint8_t* values = new uint8_t[length];
	return EEPROM.read(address);
}

void EPROMManagerClass::saveByte(uint16_t address, uint8_t value)
{
	EEPROM.write(address,value);
}

EPROMManagerClass EPROMManager;

