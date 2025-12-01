module;

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <cstdint>
#include <d3d.h>
#include <ddraw.h>

export module bof3ext.hooks:texture;

import bof3ext.glyphManager;
import bof3ext.helpers;

import bof3.render;

import std;


#define IID_IDirect3DTexture2 GUID { 0x93281502, 0x8CF8, 0x11D0, { 0x89, 0xAB, 0x00, 0xA0, 0xC9, 0x05, 0x41, 0x29 } }


struct ReplacementTexture {
	IDirectDrawSurface4* surface;
	IDirect3DTexture2* texture;
};

struct UnkStruct_7 {
	uint32_t dword0;
	void* vp4;
};


ArrayAccessor<0x6C2A40, UnkStruct_7> stru_6C2A40;


std::map<uint64_t, ReplacementTexture> replacementTextures;
IDirectDrawSurface4* textSurface = nullptr;


Func<0x5A2CA0, void, int32_t /* surfId */, uint16_t /* charCode */, int32_t /* paletteIdx */> LoadGlyphTexture;
FuncHook<decltype(LoadGlyphTexture)> LoadGlyphTextureHook = [](auto surfId, auto charCode, auto paletteIdx) {
	if (charCode + '!' < 0x7F)
		charCode += '!';

	if (!GlyphManager::Get().HasGlyph(charCode))
		charCode = '*';

	if (textSurface == nullptr) {
		DDSURFACEDESC2 sd{ 0 };
		sd.dwSize = sizeof(DDSURFACEDESC2);
		sd.dwWidth = 320;
		sd.dwHeight = 256;
		sd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		sd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;
		sd.ddsCaps.dwCaps2 = 0;
		sd.lPitch = 320 * 4;

		sd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		sd.ddpfPixelFormat.dwFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
		sd.ddpfPixelFormat.dwRGBBitCount = 32;
		sd.ddpfPixelFormat.dwBBitMask = 0xFF;
		sd.ddpfPixelFormat.dwGBitMask = 0xFF00;
		sd.ddpfPixelFormat.dwRBitMask = 0xFF0000;
		sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;

		HRESULT err;

		if ((err = g_IDirectDraw4->CreateSurface(&sd, &textSurface, nullptr)) != S_OK) {
			LogDebug("Failed to create text surface! Error code: %i\n", err);

			return;
		}
	}

	auto* fontGlyph = &g_FontGlyphs[surfId];
	auto* glyphSurf = fontGlyph->surface;

	if (glyphSurf == nullptr) {
		DDSURFACEDESC2 sd{ 0 };
		sd.dwSize = sizeof(DDSURFACEDESC2);
		sd.dwWidth = 64;
		sd.dwHeight = 64;
		sd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
		sd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
		sd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
		sd.dwTextureStage = 0;
		sd.lPitch = 64 * 4;

		sd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		sd.ddpfPixelFormat.dwFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
		sd.ddpfPixelFormat.dwRGBBitCount = 32;
		sd.ddpfPixelFormat.dwBBitMask = 0xFF;
		sd.ddpfPixelFormat.dwGBitMask = 0xFF00;
		sd.ddpfPixelFormat.dwRBitMask = 0xFF0000;
		sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;

		UINT err;

		if ((err = g_IDirectDraw4->CreateSurface(&sd, &fontGlyph->surface, NULL)) != S_OK) {
			LogDebug("Error creating DirectDraw surface! Error code: %i\n", err);

			return;
		}

		fontGlyph->surface->QueryInterface(IID_IDirect3DTexture2, (void**)&fontGlyph->texture);

		glyphSurf = fontGlyph->surface;
	}

	DDSURFACEDESC2 sd{ 0 };
	sd.dwSize = sizeof(DDSURFACEDESC2);
	sd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

	RECT rect{ 0, 0, 48, 48 };

	UINT err;

	if ((err = textSurface->Lock(nullptr, &sd, 1, 0)) != S_OK) {
		LogDebug("Failed to lock surface! Error code: %i\n", err);

		return;
	}

	auto colour = GetPalette(paletteIdx)[1];
	auto b = (colour & 0b11111) / 31.0;
	auto g = ((colour >> 5) & 0b11111) / 31.0;
	auto r = ((colour >> 10) & 0b11111) / 31.0;
	auto a = (colour >> 15) ? 1.0 : 0;

	auto& bitmap = GlyphManager::Get().GetGlyph(charCode);

	if (sd.lpSurface != NULL) {
		std::memset(sd.lpSurface, 0, 320 * 256 * 4);

		for (auto i = 0U; i < bitmap.rows; ++i) {
			auto* surf = (DWORD*)sd.lpSurface;
			auto pos = (i + bitmap.top - 12) * 320 + bitmap.left;

			std::memcpy(
				&surf[pos],
				&bitmap.buffer[i * bitmap.pitch],
				bitmap.pitch
			);

			auto* bSurf = (BYTE*)sd.lpSurface;

			for (int j = 0; j < bitmap.pitch; j += 4) {
				auto _b = bSurf[pos * 4 + j];
				auto _g = bSurf[pos * 4 + j + 1];
				auto _r = bSurf[pos * 4 + j + 2];
				auto _a = bSurf[pos * 4 + j + 3];

				auto pxPos = pos * 4 + j;

				bSurf[pxPos + 0] = (BYTE)(bSurf[pxPos + 0] * b);
				bSurf[pxPos + 1] = (BYTE)(bSurf[pxPos + 1] * g);
				bSurf[pxPos + 2] = (BYTE)(bSurf[pxPos + 2] * r);
				bSurf[pxPos + 3] = (BYTE)(bSurf[pxPos + 3] * a);
			}
		}
	}

	textSurface->Unlock(nullptr);

	glyphSurf->BltFast(0, 0, textSurface, (LPRECT)&rect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

	fontGlyph->charCode = (charCode < 0x7F) ? charCode - '!' : charCode;
	fontGlyph->paletteIndex = paletteIdx;
	fontGlyph->dword4 = stru_6C2A40[paletteIdx >> 6].dword0;
};


Func<0x59FFE0, void, uint32_t /* a1 */, int32_t /* a2 */> SetTexture;
FuncHook<decltype(SetTexture)> SetTextureHook = [](auto a1, auto a2) {
	uint64_t key = ((uint64_t)a1 << 32) | a2;

	if (replacementTextures.count(key) > 0) {
		const auto& rep = replacementTextures.at(key);

		g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
		g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);

		g_IDirect3DDevice3->SetTexture(0, rep.texture);
	} else {
		const auto& filename = std::format("NewData\\Textures\\texture_{}_{}.png", a1, a2);

		if (std::filesystem::exists(filename)) {
			ReplacementTexture rep{ 0 };

			int width, height, channels;

			uint8_t* img = stbi_load(filename.c_str(), &width, &height, &channels, 0);

			DDSURFACEDESC2 sd{ 0 };
			sd.dwSize = sizeof(DDSURFACEDESC2);
			sd.dwWidth = width;
			sd.dwHeight = height;
			sd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
			sd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
			sd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
			sd.dwTextureStage = 0;
			sd.lPitch = width * channels;

			sd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
			sd.ddpfPixelFormat.dwFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
			sd.ddpfPixelFormat.dwRGBBitCount = 32;
			sd.ddpfPixelFormat.dwBBitMask = 0xFF;
			sd.ddpfPixelFormat.dwGBitMask = 0xFF00;
			sd.ddpfPixelFormat.dwRBitMask = 0xFF0000;
			sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;

			UINT err;

			if ((err = g_IDirectDraw4->CreateSurface(&sd, &rep.surface, NULL)) != S_OK) {
				LogDebug("Error creating DirectDraw surface! Error code: %i\n", err);

				return;
			}

			rep.surface->QueryInterface(IID_IDirect3DTexture2, (void**)&rep.texture);

			DDSURFACEDESC2 _sd{ 0 };
			_sd.dwSize = sizeof(DDSURFACEDESC2);

			rep.surface->Lock(nullptr, &_sd, 1, 0);

			for (int i = 0; i < width; ++i) {
				for (int j = 0; j < height; ++j) {
					auto pos = i * width + j;

					auto c = ((uint32_t*)img)[pos];
					auto r = c & 0xFF;
					auto g = (c >> 8) & 0xFF;
					auto b = (c >> 16) & 0xFF;
					auto a = (c >> 24);

					c = (a << 24) | (r << 16) | (g << 8) | b;

					*((uint32_t*)_sd.lpSurface + pos) = c;
				}
			}

			rep.surface->Unlock(nullptr);

			replacementTextures[key] = rep;

			g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
			g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);

			g_IDirect3DDevice3->SetTexture(0, rep.texture);
		} else {
			g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT);
			g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);

			SetTexture.Original(a1, a2);
		}
	}

	//LogDebug("SetTexture: %i, %i\n", a1, a2);
};


export void EnableTextureHooks() {
	EnableHook(LoadGlyphTexture, LoadGlyphTextureHook);
	EnableHook(SetTexture, SetTextureHook);
}