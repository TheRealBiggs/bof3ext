module;

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <climits>
#include <cstdint>
#include <cstring>
#include <windows.h>

#undef LoadImage

export module bof3ext.hooks:winmain;

import bof3ext.helpers;
import bof3ext.configManager;
import bof3ext.glGlyphManager;
import bof3ext.renderer;
import bof3.math;
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


typedef Accessor<0x6BC620, HWND> g_hWnd;
typedef Accessor<0x6BC624, HINSTANCE> g_hInstance;
typedef Accessor<0x7DECE0, DISPENV> g_DispEnv;

typedef ArrayAccessor<0x6C3F40, UnkStruct_4> stru_6C3F40;
typedef ArrayAccessor<0x7CA9E0, float> g_TexCoordTable;
typedef ArrayAccessor<0x7CAE38, ActiveSprite> g_ActiveSprites;
typedef ArrayAccessor<0x903880, ENV> g_Env;

typedef PointerAccessor<0x937F84, ENV> g_CurrentEnv;


typedef Func<0x454960, void
> sub_454960;

typedef Func<0x461F00, void
> sub_461F00;

typedef Func<0x496B60, void
> GameMode_First;

typedef Func<0x4FC6A0, void
> sub_4FC6A0;

typedef FuncStd<0x4FCB00, int,
	HINSTANCE,	// hInstance
	HINSTANCE,	// hPrevInstance
	LPSTR,		// lpCmdLine
	int			// nShowCmd
> _WinMain;

typedef Func<0x4FD030, void
> ReadCfgFile;

typedef Func<0x4FD110, void
> sub_4FD110;

typedef Func<0x4FD230, void
> sub_4FD230;

typedef Func<0x4FD290, void
> sub_4FD290;

typedef Func<0x587C70, void
> sub_587C70;

typedef Func<0x59EA70, void,
	PSX_RECT*,	// rect
	uint8_t*	// imgData
> LoadImage2;

typedef Func<0x59EB00, void,
	PSX_RECT*,	// rect
	uint8_t*	// imgData
> LoadImage;

typedef Func<0x5A6780, void
> sub_5A6780;

typedef Func<0x5A7960, void,
	void*[8],	// orderingTable
	int			// count
> ClearOTagR;

typedef Func<0x5A98A0, void
> sub_5A98A0;

typedef Func<0x5A9907, void
> sub_5A9907;

typedef Func<0x5A9914, void,
	int,					// a1
	void(__cdecl*)(void)	// func
> SetGameMode;


void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	Renderer::Get().SetViewport(0, 0, width, height);
	ConfigManager::Get().SetWindowSize(width, height);
}

void PutDispEnv(DISPENV* env) {
	g_DispEnv::Set(env);

	// TODO: Change window size?
}

void PutDrawEnv(DRAWENV* env) {
	auto& curEnv = g_DrawEnv::Get();

	if (curEnv.r0 != env->r0
		|| curEnv.g0 != env->g0
		|| curEnv.b0 != env->b0)
		glClearColor(env->r0 / 255.0f, env->g0 / 255.0f, env->b0 / 255.0f, 1);

	g_DrawEnv::Set(env);
}

void DrawOTag(void** ordering_table) {
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
						//ProcessGpuPrim_Tri((GpuPrim_Tri*)prim);
						break;

					case (int)GpuPrimType::QuadTexture:
						Renderer::Get().ProcessPrim_QuadTexture((GpuPrim_QuadTexture*)prim);
						break;

					case (int)GpuPrimType::QuadGouraud:
						Renderer::Get().ProcessPrim_QuadGouraud((GpuPrim_QuadGouraud*)prim);
						break;

					case (int)GpuPrimType::Line:
						Renderer::Get().ProcessPrim_Lines((GpuPrim_Line*)prim, 1);
						break;

					case (int)GpuPrimType::TwoLines:
						Renderer::Get().ProcessPrim_Lines((GpuPrim_TwoLines*)prim, 2);
						break;

					case (int)GpuPrimType::ThreeLines:
						Renderer::Get().ProcessPrim_Lines((GpuPrim_ThreeLines*)prim, 3);
						break;

					case (int)GpuPrimType::RectWHTexture:
						Renderer::Get().ProcessPrim_RectWHTexture((GpuPrim_RectWHTexture*)prim);
						break;

					case (int)GpuPrimType::TextGlyph:
						Renderer::Get().ProcessPrim_TextGlyph((GpuPrim_TextGlyph*)prim);
						break;

					case (int)GpuPrimType::SetDrawEnv: {
						auto _prim = (GpuPrim_SetDrawEnv*)prim;

						auto& env = g_DrawEnv::Get();
						g_DrawEnv::Get().dfe = (int)_prim->flags & 1;
						g_DrawEnv::Get().dtd = (int)_prim->flags & 2;
						g_DrawEnv::Get().tpage = _prim->texturePage;

						if (_prim->rect != nullptr)
							env.tw = *_prim->rect;

						break;
					}

					case (int)GpuPrimType::Sprite:
						Renderer::Get().ProcessPrim_Sprite((GpuPrim_Sprite*)prim);
						break;

					default: {
						if (prim->value != 0 && prim->value != 0x80)
							LogDebug("%i - %i\n", prim->value, prim->command);
						break;
					}
				}
			}

			ordering_table = (void**)prim;
		} while (prim->tag != (void*)-1);

		for (int i = 0; i < 128; ++i)
			g_ActiveSprites::At(i).word14 = 0;
	}
}


auto __stdcall _WinMainHook(auto hInstance, auto hPrevInstance, auto lpCmdLine, auto nShowCmd) {
	const auto wndSize = ConfigManager::Get().GetWindowSize();

	// TODO: Remove this as it won't be needed once we replace input and sound code
	g_hInstance::Set(hInstance);
	ReadCfgFile::Call();

	std::memset(stru_6C3F40::Ptr(), 0, sizeof(UnkStruct_4) * 32);
	std::memset(g_CLUT::Ptr(), 0, sizeof(UnkStruct_7) * 512);

	//*(uint32_t*)0x6C3A4C = 0;
	//std::memset((void*)0x6BE9F8, 0, 28);
	//std::memset((void*)0x7CADE8, 0, 28);
	std::memset(g_ActiveSprites::Ptr(), 0, sizeof(ActiveSprite) * 128);
	//std::memset((void*)0x7DED60, 0, 192);
	//*(uint32_t*)0x6C3A48 = 0;
	//*(uint32_t*)0x6C3A44 = 0;
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
	glfwWindowHint(GLFW_SAMPLES, 4);

	auto wnd = glfwCreateWindow(wndSize.x, wndSize.y, "Breath of Fire III - OpenGL Renderer", nullptr, nullptr);
	glfwMakeContextCurrent(wnd);
	glfwSetFramebufferSizeCallback(wnd, FramebufferSizeCallback);
	g_hWnd::Get() = glfwGetWin32Window(wnd);

	auto& renderer = Renderer::Get();
	renderer.Initialise();
	renderer.SetViewport(0, 0, wndSize.x, wndSize.y);

	GlGlyphManager::Get().Initialise();

	//capcomBG = renderer.LoadTexture("NewData/Textures/capcom_bg.png");
	//capcomLogo = renderer.LoadTexture("NewData/Textures/capcom.png");

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
			SetGameMode::Call(0, GameMode_First::Pointer);

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
				PutDispEnv(&g_CurrentEnv::Get()->disp);
				PutDrawEnv(&g_CurrentEnv::Get()->env);
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

		if (someFloat > UINT_MAX + 1)
			someFloat -= UINT_MAX + 1;

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

auto LoadImage2Hook(auto rect, auto data) {
	Renderer::Get().LoadTexturePageData(rect, data);
}


export void EnableWinMainHooks() {
	EnableHook<_WinMain>(_WinMainHook);
	EnableHook<LoadImage>(LoadImage2Hook);
	EnableHook<LoadImage2>(LoadImage2Hook);
}