module;

#include <cstdint>

export module bof3.dat;

import bof3ext.helpers;


export int LastLoadedDatFileId;


export typedef ArrayAccessor<0x64F368, char*> g_DatFileNames;
export typedef ArrayAccessor<0x80B780, uint8_t> g_DatChunk_0_8200;

export typedef Func<0x454590, void,
	int	// id
> LoadDatFile;

export typedef Func<0x454770, void,
	int	// id
> LoadDatFile2;