#include <MinHook.h>

#include <cstdio>
#include <libloaderapi.h>
#include <consoleapi.h>

import bof3ext.hooks;
import bof3ext.configManager;
import bof3ext.glyphManager;
import bof3ext.textManager;

import std;


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hModule);

			ConfigManager::Get().Initialise();

#ifdef ENABLE_LOGGING
			if (ConfigManager::Get().GetEnableConsole()) {
				AllocConsole();
				FILE* _;

				if (freopen_s(&_, "CONOUT$", "w", stdout))
					FreeConsole();

				if (freopen_s(&_, "CONOUT$", "w", stderr))
					FreeConsole();
			}
#endif

			GlyphManager::Get().Initialise();
			TextManager::Get().Initialise();

			MH_Initialize();

			ApplyWidescreenPatches();
			//EnableAudioHooks();
			EnableDatHooks();
			EnableEnemyHooks();
			EnableItemHooks();
			EnableGuiHooks();
			EnableRenderHooks();
			EnableTextHooks();
			EnableTextureHooks();
			EnableWindowHooks();

			break;
		}

		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}