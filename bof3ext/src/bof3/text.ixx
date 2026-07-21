module;

#include <cstdint>

export module bof3.text;

import bof3ext.helpers;


export typedef Func<0x497740, const char*,
	uint16_t	// index
> GetText;

export typedef Func<0x57D800, uint8_t,
	const char*	// text
> GetStringLength;