module;

#include <MinHook.h>

#include <cstdarg>
#include <cstdio>
#include <cstdint>

#include <windows.h>

export module bof3ext.helpers;

import std;


// Windows type redefinitions to avoid needing to import windows.h and to make intellisense shut up
// about indirectly-included headers

export struct Rect {
	long left, top, right, bottom;
};

export typedef int BOOL;


// Helper templates for accessing a variable by its address

export template <uintptr_t Address, typename T>
struct Accessor {
	__forceinline T& operator *() {
		return *(T*)Address;
	}

	__forceinline T* operator ->() {
		return (T*)Address;
	}

	__forceinline T* operator &() {
		return (T*)Address;
	}
};

export template <uintptr_t Address, typename T>
struct ArrayAccessor {
	__forceinline T& operator [](auto index) {
		return ((T*)Address)[index];
	}
};

export template <uintptr_t Address, typename T>
struct PointerAccessor {
	__forceinline T* operator *() {
		return *(T**)Address;
	}

	__forceinline T* operator ->() {
		return *(T**)Address;
	}
};


// Helper templates for accessing and hooking a function by its address

export template<uintptr_t Address, typename ReturnType, typename... ArgTypes>
struct Func {
	typedef ReturnType(__cdecl* FuncType)(ArgTypes...);

	FuncType FuncPointer = (FuncType)Address;
	FuncType Original = (FuncType)Address;

	ReturnType __forceinline operator()(ArgTypes... args) {
		return FuncPointer(args...);
	}
};


export template<typename FuncType>
void __forceinline EnableHook(FuncType& func, typename FuncType::FuncType hook) {
	static_assert(std::is_same_v<decltype(func.FuncPointer), decltype(hook)>, "Function and hook type do not match!");

	auto res = MH_CreateHook(func.FuncPointer, hook, (LPVOID*)&func.Original);
	res = MH_EnableHook(func.FuncPointer);

	func.FuncPointer = hook;
}


// Code-editing functions

export template<typename T>
void __forceinline WriteProtectedMemory(uintptr_t address, T val) {
	unsigned long oldProtect;

	VirtualProtect((void*)address, sizeof(T), PAGE_READWRITE, &oldProtect);
	*(T*)address = val;
	VirtualProtect((void*)address, sizeof(T), oldProtect, &oldProtect);
}

export template<typename T, size_t S>
void __forceinline WriteProtectedMemory(uintptr_t address, T(& val)[S]) {
	unsigned long oldProtect;

	VirtualProtect((void*)address, sizeof(T) * S, PAGE_READWRITE, &oldProtect);
	std::memcpy((void*)address, val, sizeof(T) * S);
	VirtualProtect((void*)address, sizeof(T) * S, oldProtect, &oldProtect);
}


// Logging functions

#ifdef ENABLE_LOGGING

export template<typename... ArgTypes>
void __forceinline LogDebug(const char* msg, ArgTypes... args) {
	fprintf(stdout, msg, args...);
}

export template<typename... ArgTypes>
void __forceinline LogError(const char* msg, ArgTypes... args) {
	fprintf(stderr, msg, args...);
}

#else

export void __forceinline LogDebug(const char*, ...) { __noop; }
export void __forceinline LogError(const char*, ...) { __noop; }

#endif


// Text-encoding functions

export uint16_t __forceinline EncodeUnicodeCharacter(char16_t c) {
	return ((c | 0x8000) >> 8) | ((c & 0xFF) << 8);
}