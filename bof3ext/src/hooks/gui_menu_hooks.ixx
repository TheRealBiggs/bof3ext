module;

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

export module bof3ext.hooks:gui.menu;

import bof3ext.helpers;
import bof3ext.configManager;
import bof3ext.glyphManager;
import bof3ext.renderHelpers;
import bof3ext.textManager;
import bof3.character;
import bof3.item;
import bof3.gui;
import bof3.render;
import bof3.text;


struct UnkStruct_D {
	uint8_t gap0[4];
	uint16_t x;
	uint16_t y;
	uint8_t gap1;
	uint8_t flags;
	uint8_t category;
	uint8_t gap3;
	uint8_t gap4;
	uint8_t gap5;
	uint8_t gap6[2];
	uint16_t gap7;
	uint16_t gap;
};

struct UnkStruct_5 {
	uint8_t textLen;
	int8_t x;
	char text[14];
};


typedef ArrayAccessor<0x6536F8, UnkStruct_5> stru_6536F8;
typedef ArrayAccessor<0x653808, uint8_t> byte_653808;


typedef Func<0x5918E0, uint8_t,
	uint8_t	// a1
> sub_5918E0;

typedef Func<0x5919B0, uint16_t,
	uint8_t,	// a1
	uint8_t,	// a2
	uint8_t		// a3
> sub_5919B0;

typedef Func<0x590AB0, void,
	uint8_t,	// characterId
	char*,		// newEquip
	uint8_t*,	// outColours
	uint16_t*	// outStats
> GetNewStats;

typedef Func<0x57D360, void,
	int16_t,	// x
	int16_t,	// y
	uint8_t,	// id
	bool		// greyed
> DrawItemIcon;

typedef Func<0x57D860, void,
	int16_t,	// x
	int16_t,	// y
	uint8_t,	// a3
	char		// a4
> sub_57D860;

typedef Func<0x591940, void,
	uint8_t,	// a1
	uint8_t,	// a2
	char*		// a3
> sub_591940;

typedef Func<0x5905D0, void,
	int16_t,	// x
	int16_t		// y
> DrawCursor;


typedef Func<0x574890, void,
	int16_t,	// x
	int16_t,	// y
	uint8_t,	// index
	uint8_t		// selectedTabIdx 
> DrawMenuTabs;
auto DrawMenuTabsHook(auto x, auto y, auto index, auto selectedTabIdx) {
	auto count = byte_66383C::At(index * 5);
	auto startIndex = index * 5 + 1;

	auto advance = GlyphManager::Get().GetScaledGlyphAdvance();

	for (int i = 0; i < count; ++i) {
		DrawWindowBackground::Call(x + 48 * i, y, 45, 20, 0, *(uint8_t*)0x903A5A);

		auto id = byte_66383C::At(startIndex + i);

		const auto& text = TextManager::Get().GetMenuTabText(id);
		auto len = text.length();

		auto offset = std::round(advance / 2 * len);

		auto _x = (int16_t)(x + 22 - offset + i * 48);

		auto paletteIdx = 0;

		if ((selectedTabIdx < count && i != selectedTabIdx)
			|| (id == 18 && (*(char*)0x905BA2 & 1) == 0)
			|| (id == 8 && !sub_5918E0::Call(15) && !sub_5919B0::Call(0, 88, 0)))
			paletteIdx = 7;

		DrawString::Call(_x, y + 3, paletteIdx, (uint8_t)len, text.c_str());

		auto a3 = (char*)0x663428;

		if (i == selectedTabIdx)
			a3 = (char*)0x663440;

		DrawUIGroup::Call(x + 48 * i, y, a3, 0);
	}
}

typedef Func<0x574EC0, void,
	uint8_t /* characterId */,
	int16_t /* x */,
	int16_t /* y */,
	char* /* a4 */,
	int16_t /* a5 */,
	int8_t* /* a6 */
> DrawEquipmentWindow;
auto DrawEquipmentWindowHook(auto characterId, auto x, auto y, auto a4, auto a5, auto a6) {
	DrawWindowBackground::Call(x + 4, y + 4, 120, 163, 0, *(uint8_t*)0x903A5A);

	auto* character = &g_Characters::At(characterId);

	DrawString::Call(x + 32, y + 7, 0, 5, character->name);
	DrawString::Call(x + 5, y + 26, 0, 5, "Power");
	DrawString::Call(x + 5, y + 39, 0, 7, "Defense");
	DrawString::Call(x + 5, y + 52, 0, 12, "Intellect");
	DrawString::Call(x + 5, y + 65, 0, 7, "Agility");

	char buf[6];

	auto advance = std::ceil(GlyphManager::Get().GetScaledGlyphAdvance());
	auto statXPos = x + 120 - advance * 3 - 5;

	if (a5 == 0)
		statXPos -= advance * 4;

	auto iStatXPos = static_cast<int16_t>(statXPos);

	sprintf_s(buf, "%i", character->power);
	DrawString::Call(iStatXPos, y + 26, 0, 3, buf);

	sprintf_s(buf, "%i", character->defence);
	DrawString::Call(iStatXPos, y + 39, 0, 3, buf);

	sprintf_s(buf, "%i", character->intelligence);
	DrawString::Call(iStatXPos, y + 52, 0, 3, buf);

	sprintf_s(buf, "%i", character->agility);
	DrawString::Call(iStatXPos, y + 65, 0, 3, buf);

	if (a5 == 0) {
		uint8_t colours[4];
		uint16_t newStats[4];

		uint16_t arrow = EncodeUnicodeCharacter(u'»');
		GetNewStats::Call(characterId, a4, colours, newStats);

		for (int i = 0; i < 4; ++i) {
			auto _y = y + 26 + 13 * i;

			DrawString::Call(static_cast<int16_t>(statXPos + advance * 3), _y, 0, 1, (char*)&arrow);

			auto colour = colours[i];
			auto newStat = newStats[i];

			sprintf_s(buf, "%i", newStat);
			DrawString::Call(static_cast<int16_t>(statXPos + advance * 4), _y, colour, 3, buf);
		}
	}

	const char* equipNames[6] = {
		GetItemName::Call(1, character->weaponId),
		GetItemName::Call(2, character->armorId1),
		GetItemName::Call(2, character->armorId2),
		GetItemName::Call(2, character->armorId3),
		GetItemName::Call(3, character->accessoryId1),
		GetItemName::Call(3, character->accessoryId2)
	};

	for (int i = 0; i < 6; ++i) {
		auto _y = y + i + 12 * i;

		DrawItemIcon::Call(x + 6, _y + 87, g_EquipWindowItemIconIds::At(i), false);

		auto len = GetStringLength::Call(equipNames[i]);
		DrawString::Call(x + 17, _y + 85, 0, len, equipNames[i]);
	}

	DrawUIGroup::Call(x, y, (char*)0x663458, 1);

	for (int i = 0; i < 7; ++i) {
		auto _y = y + 8 * i + 24;

		sub_57D860::Call(x, _y, 4, 1);		// Left side of frame, top portion
		sub_57D860::Call(x + 120, _y, 8, 1);	// Right side of frame, top portion
	}

	for (int i = 0; i < 9; ++i) {
		auto _y = y + 8 * i + 88;

		sub_57D860::Call(x, _y, 4, 1);		// Left side of frame, bottom portion
		sub_57D860::Call(x + 120, _y, 8, 1);	// Right side of frame, bottom portion
	}

	for (int i = 0; i < 14; ++i) {
		auto _x = x + 8 * i + 8;

		sub_57D860::Call(_x, y + 80, 14, 1);	// Bottom of frame, top portion
		sub_57D860::Call(_x, y + 160, 17, 1); // Bottom of frame, bottom portion
	}

	sub_57D860::Call(x, y + 80, 13, 1);
	sub_57D860::Call(x + 120, y + 80, 15, 1);
	sub_57D860::Call(x, y + 160, 16, 1);
	sub_57D860::Call(x + 120, y + 160, 18, 1);
}

typedef Func<0x574610, void,
	int16_t /* x */,
	int16_t /* y */,
	int /* a3 */,
	int /* a4 */
> DrawZennyPanel;
auto DrawZennyPanelHook(auto x, auto y, auto a3, auto a4) {
	auto diff = ConfigManager::Get().GetScaledRenderWidth() - 320;

	const auto& glyphMgr = GlyphManager::Get();
	auto advance = (int)std::ceil(GlyphManager::Get().GetScaledGlyphAdvance());
	auto largeAdvance = (int)std::ceil(GlyphManager::Get().GetScaledGlyphAdvance() * (16.0 / 12.0));

	DrawWindowBackground::Call(x + 3 + diff, y + 3, 101, 16, 0, ((uint8_t*)0x9039E0)[122]);

	char buf[8];
	sprintf_s(buf, "%7d", a4);
	DrawStringLarge::Call(x + 101 - 5 - advance * 2 - 7 * largeAdvance + diff, y + 4, 0, buf);

	*(uint16_t*)&buf[0] = EncodeUnicodeCharacter(u'ƶ');
	DrawString::Call(x + 101 - 5 - advance + diff, y + 4, 0, 1, buf);
	DrawUIGroup::Call(x + diff, y, (char*)0x6633A8, 0);

	for (int i = 0; i < 11; ++i)
		DrawUIGroup::Call(x + 8 * i + diff, y, (char*)0x6633B4, 0);

	DrawUIGroup::Call(x + diff, y, (char*)0x6633C0, 0);
}

typedef Func<0x5738A0, void, int16_t /* x */, int16_t /* y */, uint8_t /* charId */> DrawStatusWindow;
auto DrawStatusWindowHook(auto x, auto y, auto charId) {
	DrawWindowBackground::Call(x + 5, y + 3, 164, 45, 0, ((uint8_t*)0x9039E0)[122]);

	char buf[4];

	DrawStringNumFont::Call(x + 11, y + 10, 0, "Power");
	sprintf_s(buf, "%3d", g_Characters::At(charId).power);
	DrawStringNumFont::Call(x + 56, y + 10, 0, buf);

	DrawStringNumFont::Call(x + 11, y + 23, 0, "Defense");
	sprintf_s(buf, "%3d", g_Characters::At(charId).defence);
	DrawStringNumFont::Call(x + 56, y + 23, 0, buf);

	DrawStringNumFont::Call(x + 83 + 6, y + 10, 0, "Intellect");
	sprintf_s(buf, "%3d", g_Characters::At(charId).intelligence);
	DrawStringNumFont::Call(x + 146, y + 10, 0, buf);

	DrawStringNumFont::Call(x + 83 + 6, y + 23, 0, "Agility");
	sprintf_s(buf, "%3d", g_Characters::At(charId).agility);
	DrawStringNumFont::Call(x + 146, y + 23, 0, buf);

	// Status effect maybe?
	if (g_Characters::At(charId).byte1F != 255) {
		auto v3 = (char*)GetText::Call(g_Characters::At(charId).byte1F + 273);
		sub_591940::Call(0, 8u, v3);
		auto v4 = (char*)GetText::Call(0x34u);
		DrawString::Call(x + 11, y + 34, 0, 255, v4);
	}

	DrawUIGroup::Call(x, y, (char*)0x663358, 0);

	for (int i = 0; i < 18; ++i)
		DrawUIGroup::Call(x + 8 * i, y, (char*)0x663384, 0);

	DrawUIGroup::Call(x + 16, y, (char*)0x663390, 0);
}

typedef Func<0x461800, void, int16_t /* x */, int16_t /* y */, uint8_t /* index */, uint8_t /* a4 */> DrawConfigCategory;
auto DrawConfigCategoryHook(auto x, auto y, auto index, auto a4) {
	DrawWindowBackground::Call(x, y - a4, 249, 2 * a4 + 11, 0, ((uint8_t*)0x9039E0)[122]);

	auto _x = x + 58;

	const auto& text = TextManager::Get().GetConfigText(index);
	auto advance = GlyphManager::Get().GetScaledGlyphAdvance();
	advance /= 2;

	if (a4 == 3) {
		_x -= static_cast<int16_t>(advance * text.length());
		DrawString::Call(_x, y - 1, 0, static_cast<uint8_t>(text.length()), text.c_str());

		if (*(uint8_t*)0x929F02 != 1)
			DrawCursor::Call(x + 4, y);
	} else {
		_x -= static_cast<int16_t>(advance * SMALL_TEXT_SCALE * text.length());
		DrawStringSmall::Call(_x, y + 3, 0, static_cast<uint8_t>(text.length()), text.c_str());
	}

	DrawLine(x + 116, y - a4 + 1, x + 116, a4 + y + 9, { 128, 128, 128 });
}

typedef Func<0x461970, void, int16_t /* x */, int16_t /* y */, uint8_t /* index */, uint8_t /* selectedIndex */, int /* a5 */> DrawConfigValues;
auto DrawConfigValuesHook(auto x, auto y, auto index, auto selectedIndex, auto a5) {
	static int xOffsets[] = {
		13, 65, 117,
		15, 50, 81, 116,
		15, 50, 81, 116,
		13, 85,
		13, 85
	};

	auto startIndex = byte_653808::At(index);
	auto count = ((uint8_t*)0x653810)[index];

	for (int i = 0; i < count; ++i) {
		auto _x = x + 116 + xOffsets[startIndex + i];

		auto colour = i == selectedIndex ? 2 : 0;

		const auto& text = TextManager::Get().GetConfigText(6 + startIndex + i);
		auto advance = GlyphManager::Get().GetScaledGlyphAdvance();

		if (i == selectedIndex && a5 == 3) {
			if (index < 3) {
				if (i > 0 && i < count - 1)
					_x -= static_cast<int16_t>(advance / 2 * text.length());
				else if (i == count - 1)
					_x -= static_cast<int16_t>(advance * text.length());
			}

			DrawString::Call(_x, y - 1, colour, static_cast<uint8_t>(text.length()), text.c_str());
		} else {
			if (index < 3) {
				if (i > 0 && i < count - 1)
					_x -= static_cast<int16_t>(advance / 2 * SMALL_TEXT_SCALE * text.length());
				else if (i == count - 1)
					_x -= static_cast<int16_t>(advance * SMALL_TEXT_SCALE * text.length());
			}

			DrawStringSmall::Call(_x, y + 3, colour, static_cast<uint8_t>(text.length()), text.c_str());
		}
	}
}


void DrawConfigBackgroundWindow(int16_t x, int16_t y, uint8_t a3, uint8_t a4) {
	auto rect = (PSX_RECT*)g_GpuPrims::Get();
	rect->x = 0;
	rect->y = 0;
	rect->w = 256;
	rect->h = 256;

	g_GpuPrims::Get() += sizeof(PSX_RECT);

	TexturePageAttribute tpage = {
		.x = 832 / 64,
		.y = 256 / 256,
	};

	Init_GpuPrim_SetDrawEnv::Call(g_GpuPrims::Get<GpuPrim_SetDrawEnv>(), 0, 0, tpage.value, rect);
	PushGpuPrim_0::Call(1, sizeof(GpuPrim_SetDrawEnv));

	sub_57D860::Call(x, y, 35, 1);								// Top-left corner
	sub_57D860::Call(x + 8 * a3 - 16, y, 38, 1);					// Top-right corner
	sub_57D860::Call(x, y + 8 * a4 - 16, 39, 1);					// Bottom-left corner
	sub_57D860::Call(x + 8 * a3 - 16, y + 8 * a4 - 16, 41, 1);	// Bottom-right corner

	// Top edge
	rect = (PSX_RECT*)g_GpuPrims::Get();
	rect->x = 112;
	rect->y = 152;
	rect->w = 8;
	rect->h = 8;

	g_GpuPrims::Get() += sizeof(PSX_RECT);

	Init_GpuPrim_SetDrawEnv::Call(g_GpuPrims::Get<GpuPrim_SetDrawEnv>(), 0, 0, tpage.value, rect);
	PushGpuPrim_0::Call(1, sizeof(GpuPrim_SetDrawEnv));

	Vec3b colour = { 128, 128, 128 };
	uint16_t palette = CreatePaletteIdx::Call(176, 481);

	auto prim = (GpuPrim_RectWHTexture*)g_GpuPrims::Get();
	Init_GpuPrim_RectWHTexture::Call(prim);

	prim->v1.x = x + 16.0f;
	prim->v1.y = y;
	prim->t1.x = prim->t1.y = 0;
	prim->size.x = 8 * a3 - 32;
	prim->size.y = 8;
	prim->colour = colour;
	prim->palette = palette;

	PushGpuPrim_0::Call(1, sizeof(GpuPrim_RectWHTexture));

	// Bottom edge
	rect = (PSX_RECT*)g_GpuPrims::Get();
	rect->x = 152;
	rect->y = 160;
	rect->w = 8;
	rect->h = 8;

	g_GpuPrims::Get() += sizeof(PSX_RECT);

	Init_GpuPrim_SetDrawEnv::Call(g_GpuPrims::Get<GpuPrim_SetDrawEnv>(), 0, 0, tpage.value, rect);
	PushGpuPrim_0::Call(1, sizeof(GpuPrim_SetDrawEnv));

	prim = (GpuPrim_RectWHTexture*)g_GpuPrims::Get();
	Init_GpuPrim_RectWHTexture::Call(prim);

	prim->v1.x = x + 16.0f;
	prim->v1.y = static_cast<float>(y + 8 * a4 - 8);
	prim->t1.x = prim->t1.y = 0;
	prim->size.x = 8 * a3 - 32;
	prim->size.y = 8;
	prim->colour = colour;
	prim->palette = palette;

	PushGpuPrim_0::Call(1, sizeof(GpuPrim_RectWHTexture));

	// Left edge
	rect = (PSX_RECT*)g_GpuPrims::Get();
	rect->x = 176;
	rect->y = 152;
	rect->w = 8;
	rect->h = 8;

	g_GpuPrims::Get() += sizeof(PSX_RECT);

	Init_GpuPrim_SetDrawEnv::Call(g_GpuPrims::Get<GpuPrim_SetDrawEnv>(), 0, 0, tpage.value, rect);
	PushGpuPrim_0::Call(1, sizeof(GpuPrim_SetDrawEnv));

	prim = (GpuPrim_RectWHTexture*)g_GpuPrims::Get();
	Init_GpuPrim_RectWHTexture::Call(prim);

	prim->v1.x = x;
	prim->v1.y = y + 16.0f;
	prim->t1.x = prim->t1.y = 0;
	prim->size.x = 8;
	prim->size.y = 8 * a4 - 32;
	prim->colour = colour;
	prim->palette = palette;

	PushGpuPrim_0::Call(1, sizeof(GpuPrim_RectWHTexture));

	// Right edge
	rect = (PSX_RECT*)g_GpuPrims::Get();
	rect->x = 176;
	rect->y = 160;
	rect->w = 8;
	rect->h = 8;

	g_GpuPrims::Get() += sizeof(PSX_RECT);

	Init_GpuPrim_SetDrawEnv::Call(g_GpuPrims::Get<GpuPrim_SetDrawEnv>(), 0, 0, tpage.value, rect);
	PushGpuPrim_0::Call(1, sizeof(GpuPrim_SetDrawEnv));

	prim = (GpuPrim_RectWHTexture*)g_GpuPrims::Get();
	Init_GpuPrim_RectWHTexture::Call(prim);

	prim->v1.x = static_cast<float>(x + 8 * a3 - 8);
	prim->v1.y = y + 16.0f;
	prim->t1.x = prim->t1.y = 0;
	prim->size.x = 8;
	prim->size.y = 8 * a4 - 32;
	prim->colour = colour;
	prim->palette = palette;

	PushGpuPrim_0::Call(1, sizeof(GpuPrim_RectWHTexture));

	// Center
	rect = (PSX_RECT*)g_GpuPrims::Get();
	rect->x = 112;
	rect->y = 160;
	rect->w = 8;
	rect->h = 8;

	g_GpuPrims::Get() += sizeof(PSX_RECT);

	Init_GpuPrim_SetDrawEnv::Call(g_GpuPrims::Get<GpuPrim_SetDrawEnv>(), 0, 0, tpage.value, rect);
	PushGpuPrim_0::Call(1, sizeof(GpuPrim_SetDrawEnv));

	prim = (GpuPrim_RectWHTexture*)g_GpuPrims::Get();
	Init_GpuPrim_RectWHTexture::Call(prim);

	prim->v1.x = x + 8.0f;
	prim->v1.y = y + 8.0f;
	prim->t1.x = prim->t1.y = 0;
	prim->size.x = 8 * a3 - 16;
	prim->size.y = 8 * a4 - 16;
	prim->colour = colour;
	prim->palette = palette;

	PushGpuPrim_0::Call(1, sizeof(GpuPrim_RectWHTexture));
}

void DrawConfigControllerBackgroundWindow(int16_t x, int16_t y, uint8_t a3, uint8_t a4) {
	DrawConfigBackgroundWindow(x, y, a3 + 3, a4);
}


static const float HALF = 0.5f;


static void __declspec(naked) FixTextCenteringInventoryCategory() {
	__asm {
		push ecx;				// Save ECX register (textLen)
		call GlyphManager::Get;
		mov ecx, eax;			// Move GlyphManager instance into ECX for __thiscall
		call GlyphManager::GetScaledGlyphAdvance;
		fmul[HALF];				// Divide ST0 (advance) by 2
		fimul[esp];				// Multiply ST0 by textLen
		fistp[esp];				// Move and truncate ST0 into space on stack (reserved by previous `push ecx`)
		mov edx, 77;			// 77 is X offset of center of textbox
		pop ecx;				// Pop converted float (half textLen * advance) into ECX
		sub edx, ecx;			// Subtract converted float from 77 to get final X offset
		ret;
	}
}

static void __declspec(naked) FixTextCenteringSkillCategory() {
	__asm {
		push ecx;				// Save ECX register (textLen)
		call GlyphManager::Get;
		mov ecx, eax;			// Move GlyphManager instance into ECX for __thiscall
		call GlyphManager::GetScaledGlyphAdvance;
		fmul[HALF];				// Divide ST0 (advance) by 2
		fimul[esp];				// Multiply ST0 by textLen
		fistp[esp];				// Move and truncate ST0 into space on stack (reserved by previous `push ecx`)
		mov ecx, 77;			// 77 is X offset of center of textbox
		pop eax;				// Pop converted float (half textLen * advance) into EAX
		sub ecx, eax;			// Subtract converted float from 77 to get final X offset
		ret;
	}
}

static void __declspec(naked) FixTextCenteringMainMenu() {
	__asm {
		push edx;				// Save EDX register (&struct_905B84)
		push eax;				// Save EAX register (textLen)
		call GlyphManager::Get;
		mov ecx, eax;			// Move GlyphManager instance into ECX for __thiscall
		call GlyphManager::GetScaledGlyphAdvance;
		fmul[HALF];				// Divide ST0 (advance) by 2
		fimul[esp];				// Multiply ST0 by textLen
		fistp[esp];				// Move and truncate ST0 into space on stack (reserved by previous `push EAX`)
		pop ecx;				// Pop converted float (half textLen * advance) into ECX
		pop edx;				// Restore EDX
		mov ax, [edx + 4];		// Move struct_905B84->x into EAX
		ret;
	}
}

static void __declspec(naked) FixTextCenteringConfigController() {
	__asm {
		push ebp;				// Save EBP register (x)
		push ecx;				// Save ECX register (textLen)
		call GlyphManager::Get;
		mov ecx, eax;
		call GlyphManager::GetScaledGlyphAdvance;
		fmul[HALF];
		fimul[esp];
		fistp[esp];
		pop ecx;				// Half textLen * advance
		pop eax;				// X
		add eax, 32;
		sub eax, ecx;
		mov cx, ax;
		ret;
	}
}

static void __declspec(naked) FixTextCenteringMaster() {
	__asm {
		push edx;		// Save EDX register (x)
		push eax;		// Save EAX register (textLen)
		call GlyphManager::Get;
		mov ecx, eax;
		call GlyphManager::GetScaledGlyphAdvance;
		fmul[HALF];
		fimul[esp];
		fistp[esp];
		pop ecx;		// Half textLen * advance
		pop edx;		// X
		ret;
	}
}


export void EnableGuiMenuHooks() {
	EnableHook<DrawMenuTabs>(DrawMenuTabsHook);
	EnableHook<DrawEquipmentWindow>(DrawEquipmentWindowHook);
	EnableHook<DrawZennyPanel>(DrawZennyPanelHook);
	EnableHook<DrawStatusWindow>(DrawStatusWindowHook);
	EnableHook<DrawConfigValues>(DrawConfigValuesHook);
	EnableHook<DrawConfigCategory>(DrawConfigCategoryHook);

	// Increase width of status window selection outline
	WriteProtectedMemory(0x66B090, (uint16_t)288);

	// Fix text centering for category in inventory window
	WriteCallAndNops<7>(0x575C7A, FixTextCenteringInventoryCategory);

	// Fix text centering for category in skill window
	WriteCallAndNops<7>(0x576101, FixTextCenteringSkillCategory);

	// Fix text centering for category in equip window
	WriteCallAndNops<7>(0x5766EA, FixTextCenteringInventoryCategory);

	// Fix text centering for unique item count in inventory window
	auto advance = GlyphManager::Get().GetScaledGlyphAdvance();
	auto offset = advance * SMALL_TEXT_SCALE / 2 * 7;	// 7 = strlen("xxx/yyy")
	offset = std::floor(113 - offset);

	WriteProtectedMemory(0x575CE4, (uint8_t)offset);

	// TODO: Fix text alignment for item count in item info

	// Fix text centering for submenu text in pause menu
	WriteCallAndNops<4>(0x59A015, FixTextCenteringMainMenu);

	WriteProtectedMemory(0x59A023, (uint8_t)36);

	// Fix text centering for action names in controller config
	WriteCallAndNops<7>(0x461B36, FixTextCenteringConfigController);

	// Fix text centering for Master title text
	WriteCall(0x59C5D4, FixTextCenteringMaster);

	// Intercept call to nullsub_2 in order to draw config background window
	WriteCall(0x461778, DrawConfigBackgroundWindow);
	WriteCall(0x461A84, DrawConfigControllerBackgroundWindow);

	// Remove draw of 2nd ':' in time text for save entries
	WriteNops<5>(0x576ACD);
}