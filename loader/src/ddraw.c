#include <Windows.h>
#include <sysinfoapi.h>
#include <string.h>
#include <libloaderapi.h>


#define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)


typedef HRESULT(WINAPI* DirectDrawEnumerateA_t)(void*, void*);
typedef HRESULT(WINAPI* DirectDrawCreate_t)(void*, void*, void*);
typedef void(WINAPI* ReleaseDDThreadLock_t)();
typedef void(WINAPI* AcquireDDThreadLock_t)();
typedef DWORD(WINAPI* DDInternalUnlock_t)(DWORD);
typedef DWORD(WINAPI* DDInternalLock_t)(DWORD, DWORD);
typedef HRESULT(WINAPI* D3DParseUnknownCommand_t)(void*, void*);


DirectDrawEnumerateA_t _DirectDrawEnumerateA = NULL;
DirectDrawCreate_t _DirectDrawCreate = NULL;
ReleaseDDThreadLock_t _ReleaseDDThreadLock = NULL;
AcquireDDThreadLock_t _AcquireDDThreadLock = NULL;
DDInternalUnlock_t _DDInternalUnlock = NULL;
DDInternalLock_t _DDInternalLock = NULL;
D3DParseUnknownCommand_t _D3DParseUnknownCommand = NULL;


HRESULT WINAPI DirectDrawEnumerateA(void* lpCallback, void* lpContext) {
#pragma EXPORT
	return _DirectDrawEnumerateA(lpCallback, lpContext);
}

HRESULT WINAPI DirectDrawCreate(void* lpGUID, void* lplpDD, void* pUnkOuter) {
#pragma EXPORT
	return _DirectDrawCreate(lpGUID, lplpDD, pUnkOuter);
}

void WINAPI ReleaseDDThreadLock() {
#pragma EXPORT
	return _ReleaseDDThreadLock();
}

void WINAPI AcquireDDThreadLock() {
#pragma EXPORT
	return _AcquireDDThreadLock();
}

DWORD WINAPI DDInternalUnlock(DWORD arg) {
#pragma EXPORT
	return _DDInternalUnlock(arg);
}

DWORD WINAPI DDInternalLock(DWORD arg1, DWORD arg2) {
#pragma EXPORT
	return _DDInternalLock(arg1, arg2);
}

HRESULT WINAPI D3DParseUnknownCommand(void* lpCmd, void* lpRetCmd) {
#pragma EXPORT
	return _D3DParseUnknownCommand(lpCmd, lpRetCmd);
}


void ProxyDDraw() {
	char buf[MAX_PATH];
	GetSystemDirectoryA(buf, MAX_PATH);
	strcat_s(buf, MAX_PATH, "\\ddraw.dll");

	auto ddraw = LoadLibraryA(buf);

	if (ddraw == INVALID_HANDLE_VALUE || ddraw == 0)
		return FALSE;

	_DirectDrawEnumerateA = (DirectDrawEnumerateA_t)GetProcAddress(ddraw, "DirectDrawEnumerateA");
	_DirectDrawCreate = (DirectDrawCreate_t)GetProcAddress(ddraw, "DirectDrawCreate");
	_ReleaseDDThreadLock = (ReleaseDDThreadLock_t)GetProcAddress(ddraw, "ReleaseDDThreadLock");
	_AcquireDDThreadLock = (AcquireDDThreadLock_t)GetProcAddress(ddraw, "AcquireDDThreadLock");
	_DDInternalUnlock = (DDInternalUnlock_t)GetProcAddress(ddraw, "DDInternalUnlock");
	_DDInternalLock = (DDInternalLock_t)GetProcAddress(ddraw, "DDInternalLock");
	_D3DParseUnknownCommand = (D3DParseUnknownCommand_t)GetProcAddress(ddraw, "D3DParseUnknownCommand");
}