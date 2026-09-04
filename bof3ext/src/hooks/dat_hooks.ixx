module;

export module bof3ext.hooks:dat;

import bof3ext.helpers;
import bof3.dat;


auto LoadDatFileHook(auto id) {
	LogDebug("LoadDatFile: %i - %s\n", id, g_DatFileNames::At(id));

	if (id == 2 || (id >= 210 && id <= 251) || id == 549)
		LastLoadedDatFileId = id;

	if (id == 27) {
		auto asd = true;
	}

	LoadDatFile::Original(id);
}

auto LoadDatFile2Hook(auto id) {
	LogDebug("LoadDatFile2: %i - %s\n", id, g_DatFileNames::At(id));

	LoadDatFile2::Original(id);
}


export void EnableDatHooks() {
	EnableHook<LoadDatFile>(LoadDatFileHook);
	EnableHook<LoadDatFile2>(LoadDatFile2Hook);
}