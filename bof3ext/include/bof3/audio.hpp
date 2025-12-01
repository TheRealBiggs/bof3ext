//#pragma once
//
//#include "helpers.hpp"
//
//#include <cstdint>
//
//
//FUNC(void, sub_587BA0, 0x587BA0, int, a1);
//
//HFUNC(void, LoadSndFile, 0x587910, int, id);
//HFUNC(void, LoadBgmFile, 0x587A20, int, id);
//HFUNC(void, PlayBgm, 0x587AE0, int, id, int, a2);
//HFUNC(void, SetBgmVolume, 0x5A6FB0, float, volume);
//HFUNC(void, StopBgm, 0x5A7050);
//
//
//#define g_SndFileNames		AS(char***, 0x6653B0)
//#define g_LoadedBgmId		AS(int*, 0x6653BC)
//#define g_BgmFileData		AS(char**, 0x6BDE50)
//#define g_BgmFileDataLength AS(size_t*, 0x6BDE48)
//#define g_CurrentBgmVolume	AS(float*, 0x6BDE5C)
//#define g_CurrentBgmId		AS(uint8_t*, 0x904131)