//module;
//
//#include <algorithm>
//#include <cstdint>
//#include <cstring>
//
//#undef LoadImage
//
//export module bof3ext.hooks:directx;
//
//import bof3ext.helpers;
//import bof3ext.configManager;
//import bof3ext.glGlyphManager;
//import bof3ext.renderer;
//import bof3.math;
//import bof3.render;
//
//
//struct UnkStruct_4 {
//	uint8_t byte0;
//	uint8_t byte1;
//	uint16_t word2;
//	uint32_t dword4;
//	uint32_t dword8;
//	uint32_t dwordC;
//	void* surface;
//	void* tex;
//	uint8_t gap[744];
//};
//
//struct ActiveSprite {
//	uint16_t width;
//	uint16_t height;
//	uint16_t surfWidth;
//	uint16_t surfHeight;
//	int16_t left;
//	int16_t right;
//	int16_t top;
//	int16_t bottom;
//	uint16_t word10;
//	uint16_t word12;
//	uint16_t word14;
//	uint16_t word16;
//	uint32_t dword18;
//	uint32_t dword1C;
//	void* surface;
//	void* texture;
//};
//
//
//typedef Accessor<0x6BC620, void*> g_hWnd;
//typedef Accessor<0x6BC624, void*> g_hInstance;
//typedef ArrayAccessor<0x6C3F40, UnkStruct_4> stru_6C3F40;
//typedef ArrayAccessor<0x7CAE38, ActiveSprite> g_ActiveSprites;
//typedef ArrayAccessor<0x7CA9E0, float> g_TexCoordTable;
//typedef ArrayAccessor<0x903880, ENV> g_Env;
//typedef PointerAccessor<0x937F84, ENV> g_CurrentEnv;
//
//typedef Func<0x4FD030, void> ReadCfgFile;
//typedef Func<0x4FD110, void> sub_4FD110;
//typedef Func<0x5A9914, void, int, void(__cdecl*)(void)> SetGameMode;
//typedef Func<0x496B60, void> GameMode_First;
//typedef Func<0x4FC6A0, void> sub_4FC6A0;
//typedef Func<0x5A9907, void> sub_5A9907;
//typedef Func<0x5A7860, void, DISPENV*> PutDispEnv;
//typedef Func<0x5A7890, void, DRAWENV*> PutDrawEnv;
//typedef Func<0x454960, void> sub_454960;
//typedef Func<0x461F00, void> sub_461F00;
//typedef Func<0x587C70, void> sub_587C70;
//typedef Func<0x5A7960, void, void**, int> ClearOTagR;
//typedef Func<0x4FD230, void> sub_4FD230;
//typedef Func<0x5A6780, void> sub_5A6780;
//typedef Func<0x5A98A0, void> sub_5A98A0;
//typedef Func<0x4FD290, void> sub_4FD290;
//
//
//Texture* capcomLogo, * capcomBG, * placeholder;
//
//uint32_t lastTickCount = -1;
//
//uint32_t GetTickDelta() {
//	if (lastTickCount == -1)
//		lastTickCount = GetTickCount();
//
//	auto t = GetTickCount();
//	auto r = t - lastTickCount;
//
//	if (lastTickCount != t)
//		lastTickCount = t;
//
//	return r;
//}
//
//
//bool DrawCapcomLogo() {
//	static float alpha = -70;
//	static float wait = 0;
//	static float fade = 0;
//
//	const auto& cfgMgr = ConfigManager::Get();
//	const auto wndSize = cfgMgr.GetWindowSize();
//
//	auto _x = (float)(wndSize.x * 0.7f) / capcomLogo->width;
//	auto _h = capcomLogo->height * _x;
//
//	Vec2f size = { wndSize.x * 0.7f, _h };
//	Rectf rect = { wndSize.x * 0.15f, (wndSize.y - _h) / 2, size.x, size.y };
//
//	auto delta = GetTickDelta();
//
//	auto& renderer = Renderer::Get();
//
//	renderer.DrawTexture(capcomBG, { 0, 0, (float)wndSize.x, (float)wndSize.y });
//	renderer.DrawTexture(capcomLogo, rect, { 255, 255, 255, (uint8_t)(std::max(alpha, 0.f)) });
//
//	alpha += delta / 2.5f;
//
//	if (alpha > 255)
//		alpha = 255;
//
//	if (alpha == 255 && wait < 500)
//		wait += delta / 2.5f;
//
//	if (wait >= 500) {
//		renderer.DrawRectangle({ 0, 0, (float)wndSize.x, (float)wndSize.y }, { 0, 0, 0, (uint8_t)fade });
//
//		fade += delta / 2.5f;
//
//		if (fade > 255)
//			fade = 255;
//	}
//
//	return fade == 255;
//}
//
//void ProcessGpuPrim_Tri(GpuPrim_Tri* prim) {
//	//DrawTriangle3D(
//	//	{ prim->v1.x /** *g_RenderScaleX*/, prim->v1.y /** *g_RenderScaleY*/, prim->v1.z },
//	//	{ prim->v2.x /** *g_RenderScaleX*/, prim->v2.y /** *g_RenderScaleY*/, prim->v2.z },
//	//	{ prim->v3.x /** *g_RenderScaleX*/, prim->v3.y /** *g_RenderScaleY*/, prim->v3.z },
//	//	WHITE
//	//);
//}
//
//void ProcessGpuPrim_ShadedQuad(GpuPrim_ShadedQuad* prim) {
//	Renderer::Get().DrawQuad(
//		{ prim->v1.x, prim->v1.y, prim->v1.z }, { prim->colour.r, prim->colour.g, prim->colour.b, 255 },
//		{ prim->v2.x, prim->v2.y, prim->v2.z }, { prim->c2.r, prim->c2.g, prim->c2.b, 255 },
//		{ prim->v3.x, prim->v3.y, prim->v3.z }, { prim->c3.r, prim->c3.g, prim->c3.b, 255 },
//		{ prim->v4.x, prim->v4.y, prim->v4.z }, { prim->c4.r, prim->c4.g, prim->c4.b, 255 }
//	);
//}
//
//void ProcessGpuPrim_SetDrawEnv(GpuPrim_SetDrawEnv* prim) {
//	LogDebug("dfe: %i, dtd: %i, tpage: (x: %i, y: %i)\n", (int)prim->flags & 1, ((int)prim->flags & 2) != 0, prim->texturePage & 0b1111, (prim->texturePage >> 4) & 0b1);
//
//	if (prim->rect != nullptr)
//		LogDebug("rect: %i, %i, %i, %i\n", prim->rect->x, prim->rect->y, prim->rect->w, prim->rect->h);
//
//	g_DrawEnv::Get().dfe = (int)prim->flags & 1;
//	g_DrawEnv::Get().dtd = (int)prim->flags & 2;
//	g_DrawEnv::Get().tpage = prim->texturePage;
//
//	if (prim->rect == nullptr)
//		return;
//
//	g_DrawEnv::Get().tw = *prim->rect;
//}
//
//
//typedef Func<0x59EDF0, void> sub_59EDF0;
//auto sub_59EDF0Hook() {
//
//}
//
//
//export void EnableDirectXHooks() {
//	EnableHook<sub_59EDF0>(sub_59EDF0Hook);
//}