module;

#include <cstdint>

export module bof3.dat;

import bof3ext.helpers;


export int LastLoadedDatFileId;


export ArrayAccessor<0x64F368, char*> g_DatFileNames;
export ArrayAccessor<0x80B780, uint8_t> g_DatChunk_0_8200;


//FUNC(int, OpenAndCacheFile, 0x5A7380, const char*, filepath);
//FUNC(size_t, ReadBytesFromFile, 0x5A7470, int, index, void*, buf, size_t, length);
//FUNC(int, GetFileLength, 0x5A74D0, int, id);
//FUNC(int, CloseFile, 0x5A7510, int, index);