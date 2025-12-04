module;

#include <cstdint>

export module bof3ext.hooks:gui.start;

import bof3ext.helpers;


export void EnableGuiStartHooks() {
	// Set width of all start menu items to 254 (full screen width)

	WriteProtectedMemory(0x5888E8, (uint8_t)254);
	WriteProtectedMemory(0x5888ED, (uint8_t)254);
	WriteProtectedMemory(0x5888F2, (uint8_t)254);
}