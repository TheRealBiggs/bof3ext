#include <Windows.h>

#include <fstream>
#include <string>


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			DisableThreadLibraryCalls(hModule);

			std::ifstream cfg("loader.cfg");

			std::string line;

			while (std::getline(cfg, line)) {
				if (line[0] == '#')
					continue;

				LoadLibraryA(line.c_str());
			}
		}
		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}