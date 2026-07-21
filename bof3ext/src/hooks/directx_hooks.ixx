module;

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <algorithm>
#include <cstdint>
#include <cstring>

#undef LoadImage

export module bof3ext.hooks:directx;

import bof3ext.helpers;
import bof3ext.configManager;
import bof3ext.math;
import bof3ext.renderer;
import bof3.render;


struct UnkStruct_4 {
	uint8_t byte0;
	uint8_t byte1;
	uint16_t word2;
	uint32_t dword4;
	uint32_t dword8;
	uint32_t dwordC;
	void* surface;
	void* tex;
	uint8_t gap[744];
};

struct ActiveSprite {
	uint16_t width;
	uint16_t height;
	uint16_t surfWidth;
	uint16_t surfHeight;
	int16_t left;
	int16_t right;
	int16_t top;
	int16_t bottom;
	uint16_t word10;
	uint16_t word12;
	uint16_t word14;
	uint16_t word16;
	uint32_t dword18;
	uint32_t dword1C;
	void* surface;
	void* texture;
};


typedef Accessor<0x6BC620, void*> g_hWnd;
typedef Accessor<0x6BC624, void*> g_hInstance;
typedef ArrayAccessor<0x6C3F40, UnkStruct_4> stru_6C3F40;
typedef ArrayAccessor<0x7CAE38, ActiveSprite> g_ActiveSprites;
typedef ArrayAccessor<0x7CA9E0, float> g_TexCoordTable;
typedef ArrayAccessor<0x903880, ENV> g_Env;
typedef PointerAccessor<0x937F84, ENV> g_CurrentEnv;

typedef Func<0x4FD030, void> ReadCfgFile;
typedef Func<0x4FD110, void> sub_4FD110;
typedef Func<0x5A9914, void, int, void(__cdecl*)(void)> SetGameMode;
typedef Func<0x496B60, void> GameMode_First;
typedef Func<0x4FC6A0, void> sub_4FC6A0;
typedef Func<0x5A9907, void> sub_5A9907;
typedef Func<0x5A7860, void, DISPENV*> PutDispEnv;
typedef Func<0x5A7890, void, DRAWENV*> PutDrawEnv;
typedef Func<0x454960, void> sub_454960;
typedef Func<0x461F00, void> sub_461F00;
typedef Func<0x587C70, void> sub_587C70;
typedef Func<0x5A7960, void, void**, int> ClearOTagR;
typedef Func<0x4FD230, void> sub_4FD230;
typedef Func<0x5A6780, void> sub_5A6780;
typedef Func<0x5A98A0, void> sub_5A98A0;
typedef Func<0x4FD290, void> sub_4FD290;


Texture* capcomLogo, * capcomBG, * placeholder;

uint32_t lastTickCount = -1;

uint32_t GetTickDelta() {
	if (lastTickCount == -1)
		lastTickCount = GetTickCount();

	auto t = GetTickCount();
	auto r = t - lastTickCount;

	if (lastTickCount != t)
		lastTickCount = t;

	return r;
}


bool DrawCapcomLogo() {
	static float alpha = -70;
	static float wait = 0;
	static float fade = 0;

	const auto& cfgMgr = ConfigManager::Get();
	const auto wndSize = cfgMgr.GetWindowSize();

	auto _x = (float)(wndSize.x * 0.7f) / capcomLogo->width;
	auto _h = capcomLogo->height * _x;

	Vec2f size = { wndSize.x * 0.7f, _h };
	Rectf rect = { wndSize.x * 0.15f, (wndSize.y - _h) / 2, size.x, size.y };

	auto delta = GetTickDelta();

	auto& renderer = Renderer::Get();

	renderer.DrawTexture(capcomBG, { 0, 0, (float)wndSize.x, (float)wndSize.y });
	renderer.DrawTexture(capcomLogo, rect, { 255, 255, 255, (uint8_t)(std::max(alpha, 0.f)) });

	alpha += delta / 2.5f;

	if (alpha > 255)
		alpha = 255;

	if (alpha == 255 && wait < 500)
		wait += delta / 2.5f;

	if (wait >= 500) {
		renderer.DrawRectangle({ 0, 0, (float)wndSize.x, (float)wndSize.y }, { 0, 0, 0, (uint8_t)fade });

		fade += delta / 2.5f;

		if (fade > 255)
			fade = 255;
	}

	return fade == 255;
}

void ProcessGpuPrim_Tri(GpuPrim_Tri* prim) {
	//DrawTriangle3D(
	//	{ prim->v1.x /** *g_RenderScaleX*/, prim->v1.y /** *g_RenderScaleY*/, prim->v1.z },
	//	{ prim->v2.x /** *g_RenderScaleX*/, prim->v2.y /** *g_RenderScaleY*/, prim->v2.z },
	//	{ prim->v3.x /** *g_RenderScaleX*/, prim->v3.y /** *g_RenderScaleY*/, prim->v3.z },
	//	WHITE
	//);
}

//void ProcessGpuPrim_TexturedQuad(GpuPrim_TexturedQuad* prim) {
//	Vec4b c = { prim->colour.r, prim->colour.g, prim->colour.b, 255 };
//
//	Renderer::Get().DrawTexturedQuad(
//		placeholder,
//		{ prim->v1.x * *g_RenderScaleX, prim->v1.y * *g_RenderScaleY, prim->v1.z }, c, { (float)prim->t1.x, (float)prim->t1.y },
//		{ prim->v2.x * *g_RenderScaleX, prim->v2.y * *g_RenderScaleY, prim->v2.z }, c, { (float)prim->t2.x, (float)prim->t2.y },
//		{ prim->v3.x * *g_RenderScaleX, prim->v3.y * *g_RenderScaleY, prim->v3.z }, c, { (float)prim->t3.x, (float)prim->t3.y },
//		{ prim->v4.x * *g_RenderScaleX, prim->v4.y * *g_RenderScaleY, prim->v4.z }, c, { (float)prim->t4.x, (float)prim->t4.y }
//	);
//}

void ProcessGpuPrim_ShadedQuad(GpuPrim_ShadedQuad* prim) {
	auto sx = g_RenderScaleX::Get();
	auto sy = g_RenderScaleY::Get();

	Renderer::Get().DrawQuad(
		{ prim->v1.x * sx, prim->v1.y * sy, prim->v1.z }, { prim->colour.r, prim->colour.g, prim->colour.b, 255 },
		{ prim->v2.x * sx, prim->v2.y * sy, prim->v2.z }, { prim->c2.r, prim->c2.g, prim->c2.b, 255 },
		{ prim->v3.x * sx, prim->v3.y * sy, prim->v3.z }, { prim->c3.r, prim->c3.g, prim->c3.b, 255 },
		{ prim->v4.x * sx, prim->v4.y * sy, prim->v4.z }, { prim->c4.r, prim->c4.g, prim->c4.b, 255 }
	);
}

void ProcessGpuPrim_SetDrawEnv(GpuPrim_SetDrawEnv* prim) {
	LogDebug("dfe: %i, dtd: %i, tpage: (x: %i, y: %i)\n", (int)prim->flags & 1, ((int)prim->flags & 2) != 0, prim->texturePage & 0b1111, (prim->texturePage >> 4) & 0b1);

	if (prim->rect != nullptr)
		LogDebug("rect: %i, %i, %i, %i\n", prim->rect->x, prim->rect->y, prim->rect->w, prim->rect->h);

	g_DrawEnv::Get().dfe = (int)prim->flags & 1;
	g_DrawEnv::Get().dtd = (int)prim->flags & 2;
	g_DrawEnv::Get().tpage = prim->texturePage;

	if (prim->rect == nullptr)
		return;

	g_DrawEnv::Get().tw = *prim->rect;
}

void __cdecl DrawOTag(void** ordering_table) {
	if (!g_DrawEnv::Get().dfe)
		return;

	if (*ordering_table != (void*)-1) {
		GpuPrim* prim;

		do {
			prim = (GpuPrim*)*ordering_table;

			if (((uint32_t)prim & 0xFF000000) != 0)
				prim = (GpuPrim*)((uint32_t)prim & 0xFFFFFF);
			else {
				switch (prim->command) {
					case (int)GpuPrimType::Tri:
						ProcessGpuPrim_Tri((GpuPrim_Tri*)prim);
						break;
					case (int)GpuPrimType::TexturedQuad:
						Renderer::Get().ProcessPrim_TexturedQuad((GpuPrim_TexturedQuad*)prim);
						break;
					case (int)GpuPrimType::ShadedQuad:
						ProcessGpuPrim_ShadedQuad((GpuPrim_ShadedQuad*)prim);
						break;
					case (int)GpuPrimType::TexturedRectWH:
						Renderer::Get().ProcessPrim_TexturedRectWH((GpuPrim_TexturedRectWH*)prim);
						break;
					case (int)GpuPrimType::SetDrawEnv:
						ProcessGpuPrim_SetDrawEnv((GpuPrim_SetDrawEnv*)prim);
						break;
					default: {
						if (prim->value != 0)
							LogDebug("%i - %i\n", prim->value, prim->command);
						break;
					}
				}
			}

			ordering_table = (void**)prim;
		} while (prim->tag != (void*)-1);

		for (int i = 0; i < 128; ++i) {
			g_FontGlyphs::At(i).word10 = 0;
			g_ActiveSprites::At(i).word14 = 0;
		}
	}
}


typedef Func<0x59EDF0, void> sub_59EDF0;
auto sub_59EDF0Hook() {

}

typedef Func<0x59EB00, void, PSX_RECT*, uint8_t*> LoadImage;
typedef Func<0x59EA70, void, PSX_RECT*, uint8_t*> LoadImage2;
auto LoadImage2Hook(auto rect, auto data) {
	/*if (rect->x + rect->w > 1024
		|| rect->y + rect->h > 512)
		return;

	auto buf = &texturePages[rect->y * 2048 + rect->x * sizeof(uint16_t)];
	auto _w = rect->w * sizeof(uint16_t);
	auto _a = 4 * (_w >> 2);
	auto _b = _w & 3;

	for (int i = 0; i < rect->h; ++i) {
		std::memcpy(buf, data, _a);
		auto _r = &buf[_a];
		std::memcpy(_r, &data[_a], _b);
		buf += 2048;
		data += _w;
	}*/

	Renderer::Get().LoadTexturePageData(rect, data);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Renderer::Get().SetViewport(0, 0, width, height);
	ConfigManager::Get().SetWindowSize(width, height);
}


typedef FuncSTD<0x4FCB00, int, void*, void*, void*, int> _WinMain;
auto __stdcall _WinMainHook(auto hInstance, auto hPrevInstance, auto lpCmdLine, auto nShowCmd) {
	const auto wndSize = ConfigManager::Get().GetWindowSize();

	g_hInstance::Get() = hInstance;
	ReadCfgFile::Call();

	std::memset(stru_6C3F40::Ptr(), 0, sizeof(UnkStruct_4) * 32);
	std::memset(g_CLUT::Ptr(), 0, sizeof(UnkStruct_7) * 512);
	std::memset(g_FontGlyphs::Ptr(), 0, sizeof(FontGlyph) * 128);

	auto fg = g_FontGlyphs::Ptr();

	do {
		fg->charCode = -1;
		++fg;
	} while ((int)fg < (int)0x7CA950);

	*(uint32_t*)0x6C3A4C = 0;
	std::memset((void*)0x6BE9F8, 0, 28);
	std::memset((void*)0x7CADE8, 0, 28);
	std::memset(g_ActiveSprites::Ptr(), 0, sizeof(ActiveSprite) * 128);
	std::memset((void*)0x7DED60, 0, 192);
	*(uint32_t*)0x6C3A48 = 0;
	*(uint32_t*)0x6C3A44 = 0;
	auto v17 = 0;
	auto v8 = g_TexCoordTable::Ptr();

	do {
		++v8;
		auto v9 = (double)v17++ * 0.00390625;
		*(v8 - 1) = static_cast<float>(v9 + 0.002000000094994903);
	} while ((int)v8 < (int)0x7CADE0);

	auto renderScale = ConfigManager::Get().GetRenderScale();

	g_RenderScaleX::Get() = renderScale;
	g_RenderScaleY::Get() = renderScale;

	//InitAudioDevice();

	//Sound capcomAudio = LoadSound("NewData/Audio/capcom.ogg");
	////SetSoundVolume(capcomAudio, 0.4);
	//PlaySound(capcomAudio);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto wnd = glfwCreateWindow(wndSize.x, wndSize.y, "Breath of Fire III - OpenGL Renderer", nullptr, nullptr);
	glfwMakeContextCurrent(wnd);
	glfwSetFramebufferSizeCallback(wnd, framebuffer_size_callback);
	g_hWnd::Get() = glfwGetWin32Window(wnd);

	auto& renderer = Renderer::Get();
	renderer.Initialise();
	renderer.SetViewport(0, 0, wndSize.x, wndSize.y);

	//capcomBG = renderer.LoadTexture("NewData/Textures/capcom_bg.png");
	//capcomLogo = renderer.LoadTexture("NewData/Textures/capcom.png");
	placeholder = renderer.LoadTexture("NewData/Textures/placeholder.png");

	//bool isCapcomLogo = true;
	bool firstGameModeSet = false;

	double someFloat = GetTickCount() + 33.34;

	while (!glfwWindowShouldClose(wnd)) {
		/*if (isCapcomLogo) {
			glClear(GL_COLOR_BUFFER_BIT);
			isCapcomLogo = !DrawCapcomLogo();

			if (!isCapcomLogo) {
				renderer.DeleteTexture(capcomBG);
				renderer.DeleteTexture(capcomLogo);
			}

			glfwSwapBuffers(wnd);
		} else {*/
			if (!firstGameModeSet) {
				sub_4FD110::Call();
			LABEL_15:
				SetGameMode::Call(0, GameMode_First::FuncPointer);

				firstGameModeSet = true;
			}

			//if (*(uint8_t*)0x6BC639)
			//	Window

			if (!*(uint8_t*)0x6BC63A)
				sub_4FC6A0::Call();

			glfwPollEvents();

			if (*(uint8_t*)0x6BC63C) {
				*(uint8_t*)0x6BC63C = 0;
				sub_5A9907::Call();
				goto LABEL_15;
			}

			auto tick = GetTickCount();

			if (tick < someFloat) {
				renderer.Begin();
				{
					PutDispEnv::Call(&g_CurrentEnv::Get()->disp);
					PutDrawEnv::Call(&g_CurrentEnv::Get()->env);
					sub_454960::Call();
					sub_461F00::Call();
					DrawOTag(&g_CurrentEnv::Get()->ordering_table[7]);
				}
				renderer.End();

				glfwSwapBuffers(wnd);
			}

			uint32_t v11;

			do {
				sub_587C70::Call();
				v11 = GetTickCount();
			} while ((double)v11 < someFloat);

			someFloat += 33.334;

			if (someFloat > 4294967300.0)
				someFloat -= 4294967300.0;

			*(uint8_t*)0x905B89 ^= 1u;
			*(ENV**)0x937F84 = &g_Env::At(*(uint8_t*)0x905B89);
			ClearOTagR::Call(g_CurrentEnv::Get()->ordering_table, 8);
			sub_4FD230::Call();
			sub_5A6780::Call();

			sub_5A98A0::Call();

			sub_4FD290::Call();
			(*(uint32_t*)0x937F94)++;
		//}
	}

	//UnloadSound(capcomAudio);

	//CloseAudioDevice();
	//CloseWindow();

	glfwTerminate();

	return 0;
}

typedef Func<0x59EE50, void, void**> _DrawOTag;


export void EnableDirectXHooks() {
	EnableHook<_WinMain>(_WinMainHook);
	EnableHook<sub_59EDF0>(sub_59EDF0Hook);
	EnableHook<LoadImage>(LoadImage2Hook);
	EnableHook<LoadImage2>(LoadImage2Hook);
}