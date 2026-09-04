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
	static __forceinline T& Get() {
		return *(T*)Address;
	}

	static __forceinline void Set(const T& val) {
		*(T*)Address = val;
	}

	static __forceinline void Set(T* val) {
		*(T*)Address = *val;
	}

	static __forceinline T* Ptr() {
		return (T*)Address;
	}

	Accessor() = delete;
};

export template <uintptr_t Address, typename T>
struct ArrayAccessor {
	static __forceinline T& At(auto index) {
		return ((T*)Address)[index];
	}

	static __forceinline T* Ptr() {
		return (T*)Address;
	}

	ArrayAccessor() = delete;
};

export template <uintptr_t Address, typename T>
struct PointerAccessor {
	static __forceinline T*& Get() {
		return *(T**)Address;
	}

	template <typename U/*, class = typename std::enable_if_t<std::is_base_of_v<U, T>>*/>
	static __forceinline U*& Get() {
		return *(U**)Address;
	}

	PointerAccessor() = delete;
};


// Helper templates for accessing and hooking a function by its address

export template<uintptr_t Address, typename ReturnType, typename... ArgTypes>
struct Func {
	typedef ReturnType(__cdecl* FuncType)(ArgTypes...);

	static inline FuncType Pointer = (FuncType)Address;
	static inline FuncType Original = (FuncType)Address;

	static ReturnType __forceinline Call(ArgTypes... args) {
		return Pointer(args...);
	}

	Func() = delete;
};

export template<uintptr_t Address, typename ReturnType, typename... ArgTypes>
struct FuncStd {
	typedef ReturnType(__stdcall* FuncType)(ArgTypes...);

	static inline FuncType Pointer = (FuncType)Address;
	static inline FuncType Original = (FuncType)Address;

	static ReturnType __forceinline Call(ArgTypes... args) {
		return Pointer(args...);
	}

	FuncStd() = delete;
};


export template<typename FuncType>
void __forceinline EnableHook(typename FuncType::FuncType hook) {
	static_assert(std::is_same_v<decltype(FuncType::Pointer), decltype(hook)>, "Function and hook type do not match!");

	auto res = MH_CreateHook(FuncType::Pointer, hook, (LPVOID*)&FuncType::Original);
	res = MH_EnableHook(FuncType::Pointer);

	FuncType::Pointer = hook;
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

export void __forceinline WriteCall(uintptr_t address, void* function) {
	uint8_t code[5];
	code[0] = 0xE8;
	*(uint32_t*)&code[1] = (uint32_t)function - (address + 5);

	WriteProtectedMemory(address, code);
}

export template<size_t S>
void __forceinline WriteNops(uintptr_t address) {
	uint8_t nops[S];
	std::memset(nops, 0x90, S);

	WriteProtectedMemory(address, nops);
}

export template<size_t S>
void __forceinline WriteCallAndNops(uintptr_t address, void* function) {
	WriteCall(address, function);
	WriteNops<S>(address + 5);
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