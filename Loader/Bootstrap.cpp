// Only the startup patch is native. Managed code runs after the loader lock is released.
// These declarations keep this small object independent of SDK header search paths.
typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef int BOOL;
typedef void* HANDLE;

extern "C" {
	__declspec(dllimport) HANDLE __stdcall GetModuleHandleW(const wchar_t* name);
	__declspec(dllimport) void* __stdcall GetProcAddress(HANDLE module, const char* name);
	__declspec(dllimport) BOOL __stdcall VirtualProtect(void* address, DWORD size, DWORD protection, DWORD* previous);
	__declspec(dllimport) BOOL __stdcall FlushInstructionCache(HANDLE process, const void* address, DWORD size);
	__declspec(dllimport) HANDLE __stdcall GetCurrentProcess();
	__declspec(dllimport) BOOL __stdcall TerminateProcess(HANDLE process, unsigned int exitCode);
	BOOL __stdcall _DllMainCRTStartup(HANDLE module, DWORD reason, void* reserved);
}


static HANDLE loaderModule;
static BYTE* gameEntry;
static BYTE originalCode[5];
static bool patched;


static BOOL RestoreGameEntry() {
	DWORD protection;

	if (!VirtualProtect(gameEntry, 5, 0x40, &protection))
		return 0;

	for (int i = 0; i < 5; ++i)
		gameEntry[i] = originalCode[i];

	DWORD previous;
	BOOL restored = VirtualProtect(gameEntry, 5, protection, &previous);
	BOOL flushed = FlushInstructionCache(GetCurrentProcess(), gameEntry, 5);
	patched = false;
	return restored && flushed;
}

static void __stdcall InitialiseGame() {
	if (!RestoreGameEntry()) {
		TerminateProcess(GetCurrentProcess(), 1);
		return;
	}

	auto initialise = reinterpret_cast<void(__cdecl*)()>(GetProcAddress(loaderModule, "Initialise"));

	if (!initialise) {
		TerminateProcess(GetCurrentProcess(), 1);
		return;
	}

	initialise();
	reinterpret_cast<void(__stdcall*)()>(gameEntry)();
}

static BOOL PatchGameEntry() {
	auto image = static_cast<BYTE*>(GetModuleHandleW(nullptr));

	if (!image || *reinterpret_cast<unsigned short*>(image) != 0x5A4D)
		return 0;

	auto header = image + *reinterpret_cast<DWORD*>(image + 0x3C);

	if (*reinterpret_cast<DWORD*>(header) != 0x4550
		|| *reinterpret_cast<unsigned short*>(header + 0x18) != 0x10B)
		return 0;

	gameEntry = image + *reinterpret_cast<DWORD*>(header + 0x28);

	DWORD protection;

	if (!VirtualProtect(gameEntry, 5, 0x40, &protection))
		return 0;

	for (int i = 0; i < 5; ++i)
		originalCode[i] = gameEntry[i];

	gameEntry[0] = 0xE9;
	*reinterpret_cast<DWORD*>(gameEntry + 1) = reinterpret_cast<DWORD>(&InitialiseGame) - reinterpret_cast<DWORD>(gameEntry + 5);
	patched = true;

	DWORD previous;
	BOOL restored = VirtualProtect(gameEntry, 5, protection, &previous);
	BOOL flushed = FlushInstructionCache(GetCurrentProcess(), gameEntry, 5);

	return restored && flushed;
}


extern "C" BOOL __stdcall LoaderDllMain(HANDLE mod, DWORD reason, void* reserved) {
	if (reason == 0 && patched && !RestoreGameEntry())
		return 0;

	if (!_DllMainCRTStartup(mod, reason, reserved))
		return 0;

	if (reason == 1) {
		loaderModule = mod;
		return PatchGameEntry();
	}

	return 1;
}
