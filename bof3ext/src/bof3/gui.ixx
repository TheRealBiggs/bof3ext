module;

#include <cstdint>

export module bof3.gui;

import bof3ext.helpers;


export ArrayAccessor<0x66386C, uint8_t> g_EquipWindowItemIconIds;
export ArrayAccessor<0x66383C, uint8_t> byte_66383C;


export Func<0x57CF60, void, int16_t /* x */, int16_t /* y */, uint16_t /* w */, uint16_t /* h */, uint8_t /* flags */, uint8_t /* paletteIdx */> DrawWindowBackground;
export Func<0x57D910, void, int16_t /* x */, int16_t /* y */, char* /* a3 */, uint8_t /* a4 */> DrawUIGroup;