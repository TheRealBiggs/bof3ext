module;

export module bof3ext.hooks:dat;

import bof3ext.helpers;
import bof3.dat;


Func<0x454590, void, int /* id */> LoadDatFile;
auto LoadDatFileHook(auto id) {
	LogDebug("LoadDatFile: %i - %s\n", id, g_DatFileNames[id]);

	if (id == 2 || (id >= 210 && id <= 251) || id == 549)
		LastLoadedDatFileId = id;

	LoadDatFile.Original(id);
}

Func<0x454770, void, int /* id */> LoadDatFile2;
auto LoadDatFile2Hook(auto id) {
	LogDebug("LoadDatFile2: %i - %s\n", id, g_DatFileNames[id]);

	LoadDatFile2.Original(id);
}


export void EnableDatHooks() {
	EnableHook(LoadDatFile, LoadDatFileHook);
	EnableHook(LoadDatFile2, LoadDatFile2Hook);
}