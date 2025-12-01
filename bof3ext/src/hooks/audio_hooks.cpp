//#include "helpers.hpp"
//
//#include "bof3/audio.hpp"
//
////#define MINIAUDIO_IMPLEMENTATION
////#include <miniaudio.h>
//
//#include <mmeapi.h>
//#include <dsound.h>
//
//#include <cstdio>
//#include <filesystem>
//#include <format>
//
//
////ma_engine* engine;
////ma_sound* bgm;
////bool bgmPlaying = false;
////bool maInited = false;
//
//
////FUNC_HOOK(void, LoadSndFile, int, id) {
////	printf("LoadSndFile: %i - %s\n", id, g_SndFileNames[id >> 12][id & 0xFFF]);
////
////	CALL_ORIG(LoadSndFile, id);
////}
////
////
////FUNC_HOOK(void, LoadBgmFile, int, id) {
////	printf("LoadBgmFile: %03d.DAT\n", id);
////
////	if (!maInited) {
////		engine = new ma_engine();
////		ma_engine_init(nullptr, engine);
////
////		bgm = new ma_sound();
////
////		maInited = true;
////	}
////
////	if (bgmPlaying) {
////		ma_sound_stop(bgm);
////		ma_sound_uninit(bgm);
////		bgmPlaying = false;
////	}
////
////	auto filepath = std::format("_bgm/{:03}.flac", id);
////
////	if (!std::filesystem::exists(filepath)) {
////		filepath = std::format("BGM/{:03}.DAT", id);
////
////		if (!std::filesystem::exists(filepath)) {
////			filepath = std::format("BGM/{:03}N.DAT", id);
////
////			if (!std::filesystem::exists(filepath))
////				return;
////		}
////	}
////
////	auto res = ma_sound_init_from_file(engine, filepath.c_str(), 0, nullptr, nullptr, bgm);
////
////	if (res != ma_result::MA_SUCCESS)
////		return;
////
////	ma_sound_set_looping(bgm, true);
////
////	*g_LoadedBgmId = id;
////}
////
////
////FUNC_HOOK(void, SetBgmVolume, float, volume) {
////	ma_engine_set_volume(engine, volume / 100.0f);
////}
////
////
////FUNC_HOOK(void, PlayBgm, int, id, int, a2) {
////	if (*g_CurrentBgmId == id)
////		return;
////
////	*g_CurrentBgmId = id;
////
////	if (id != *g_LoadedBgmId)
////		LoadBgmFile(id);
////
////	*g_CurrentBgmVolume = 0.0f;
////	SetBgmVolume(0.0f);
////
////	ma_sound_start(bgm);
////
////	sub_587BA0(a2);
////
////	bgmPlaying = true;
////}
////
////
////FUNC_HOOK(void, StopBgm) {
////	__asm {
////		push ecx;
////	}
////
////	ma_sound_stop(bgm);
////
////	__asm {
////		pop ecx;
////	}
////}
//
//
//HFUNC(void, sub_5A6BB0, 0x5A6BB0, int, a1, LPDIRECTSOUNDBUFFER, sb, DWORD, frequency);
//
//FUNC_HOOK(void, sub_5A6BB0, int, a1, LPDIRECTSOUNDBUFFER, sb, DWORD, frequency) {
//	sb->SetVolume(-750);
//	CALL_ORIG(sub_5A6BB0, a1, sb, frequency);
//}
//
//
//void EnableAudioHooks() {
//	//ENABLE_HOOK(LoadSndFile);
//	//ENABLE_HOOK(LoadBgmFile);
//	//ENABLE_HOOK(PlayBgm);
//	//ENABLE_HOOK(SetBgmVolume);
//	//ENABLE_HOOK(StopBgm);
//	ENABLE_HOOK(sub_5A6BB0);
//}