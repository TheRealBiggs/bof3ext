module;

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <cstdint>
#include <d3d.h>
#include <ddraw.h>

export module bof3ext.hooks:texture;

import bof3ext.helpers;
import bof3ext.configManager;
import bof3ext.glyphManager;

import bof3.render;
import bof3.texture;

import std;


#define IID_IDirect3DTexture2 GUID { 0x93281502, 0x8CF8, 0x11D0, { 0x89, 0xAB, 0x00, 0xA0, 0xC9, 0x05, 0x41, 0x29 } }
#define IID_IDirectDrawSurface4 GUID { 0x0B2B8630, 0xAD35, 0x11D0, { 0x8E, 0xA6, 0x00, 0x60, 0x97, 0x97, 0xEA, 0x5B } }


struct ReplacementTexture {
	IDirectDrawSurface4* surface;
	IDirect3DTexture2* texture;
};

struct UnkStruct_7 {
	uint32_t dword0;
	void* vp4;
};


void DumpTexture(const std::string& filename) {
	if (!ConfigManager::Get().GetDumpTextures())
		return;

	IDirect3DTexture2* tex;
	IDirectDrawSurface4* surf;

	if (!std::filesystem::exists(filename)) {
		FILE* file;
		fopen_s(&file, filename.c_str(), "wb");
		DWORD magic = 0x20534444;
		fwrite(&magic, 4, 1, file);

		g_IDirect3DDevice3->GetTexture(0, &tex);
		tex->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*)&surf);

		DDSURFACEDESC2 desc{ 0 };
		desc.dwSize = sizeof(DDSURFACEDESC2);
		desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		surf->Lock(nullptr, &desc, 1, 0);

		fwrite(&desc, sizeof(DDSURFACEDESC2), 1, file);
		fwrite(desc.lpSurface, desc.lPitch * desc.dwHeight, 1, file);

		surf->Unlock(nullptr);

		fflush(file);
		fclose(file);
	}
}


ArrayAccessor<0x6C2A40, UnkStruct_7> stru_6C2A40;


std::map<uint64_t, ReplacementTexture> replacementTextures;
//IDirectDrawSurface4* textSurface = nullptr;


Func<0x5A2CA0, void, int32_t /* surfId */, uint16_t /* charCode */, int32_t /* paletteIdx */> LoadGlyphTexture;
FuncHook<decltype(LoadGlyphTexture)> LoadGlyphTextureHook = [](auto surfId, auto charCode, auto paletteIdx) {
	if (charCode + '!' < 0x7F)
		charCode += '!';

	if (!GlyphManager::Get().HasGlyph(charCode))
		charCode = '*';

	auto renderScale = ConfigManager::Get().GetRenderScale();
	auto surfSize = (int)(16 * renderScale);

	/*if (textSurface == nullptr) {
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
	}*/

	auto* fontGlyph = &g_FontGlyphs[surfId];
	auto* glyphSurf = fontGlyph->surface;

	if (glyphSurf == nullptr) {
		DDSURFACEDESC2 sd{ 0 };
		sd.dwSize = sizeof(DDSURFACEDESC2);
		sd.dwWidth = surfSize;
		sd.dwHeight = surfSize;
		sd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
		sd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
		sd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
		sd.dwTextureStage = 0;
		sd.lPitch = surfSize * 4;

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

	//RECT rect{ 0, 0, 48, 48 };

	UINT err;

	//if ((err = textSurface->Lock(nullptr, &sd, 1, 0)) != S_OK) {
	if ((err = glyphSurf->Lock(nullptr, &sd, 1, 0)) != S_OK) {
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
		//std::memset(sd.lpSurface, 0, 320 * 256 * 4);
		std::memset(sd.lpSurface, 0, surfSize * surfSize * 4);

		for (auto i = 0U; i < bitmap.rows; ++i) {
			auto* surf = (DWORD*)sd.lpSurface;
			//auto pos = (i + bitmap.top - 12) * 320 + bitmap.left;
			auto pos = (i + bitmap.top - (int)(3 * renderScale)) * surfSize + bitmap.left;

			std::memcpy(
				&surf[pos],
				&bitmap.buffer[i * bitmap.pitch],
				bitmap.pitch
			);

			auto* bSurf = (BYTE*)sd.lpSurface;

			for (int j = 0; j < bitmap.pitch; j += 4) {
				auto pxPos = pos * 4 + j;

				bSurf[pxPos + 0] = (BYTE)(bSurf[pxPos + 0] * b);
				bSurf[pxPos + 1] = (BYTE)(bSurf[pxPos + 1] * g);
				bSurf[pxPos + 2] = (BYTE)(bSurf[pxPos + 2] * r);
				bSurf[pxPos + 3] = (BYTE)(bSurf[pxPos + 3] * a);
			}
		}
	}

	//textSurface->Unlock(nullptr);
	glyphSurf->Unlock(nullptr);

	//glyphSurf->BltFast(0, 0, textSurface, (LPRECT)&rect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

	fontGlyph->charCode = (charCode < 0x7F) ? charCode - '!' : charCode;
	fontGlyph->paletteIndex = paletteIdx;
	fontGlyph->dword4 = stru_6C2A40[paletteIdx >> 6].dword0;
};

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

			for (int i = 0; i < height * width; ++i) {
				auto c = ((uint32_t*)img)[i];
				auto r = c & 0xFF;
				auto g = (c >> 8) & 0xFF;
				auto b = (c >> 16) & 0xFF;
				auto a = (c >> 24);

				c = (a << 24) | (r << 16) | (g << 8) | b;

				((uint32_t*)_sd.lpSurface)[i] = c;
			}

			rep.surface->Unlock(nullptr);

			stbi_image_free(img);

			replacementTextures[key] = rep;

			g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
			g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);

			g_IDirect3DDevice3->SetTexture(0, rep.texture);
		} else {
			g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT);
			g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);

			SetTexture.Original(a1, a2);

			DumpTexture(std::format("NewData\\Textures\\Dumped\\texture_{}_{}.dds", a1, a2));
		}
	}
};

FuncHook<decltype(sub_5A3160)> sub_5A3160Hook = [](auto a1, auto a2, auto a3, auto a4) {
	auto r = sub_5A3160.Original(a1, a2, a3, a4);

	uint64_t key = ((uint64_t)a1 << 48) | ((uint64_t)a2 << 32) | ((uint64_t)a3 << 16) | a4;

	if (replacementTextures.count(key) > 0) {
		const auto& rep = replacementTextures.at(key);

		/*g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
		g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);*/

		g_IDirect3DDevice3->SetTexture(0, rep.texture);
	} else {
		const auto& filename = std::format("NewData\\Textures\\texture_{}_{}_{}_{}.png", a1, a2, a3, a4);

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

				return r;
			}

			rep.surface->QueryInterface(IID_IDirect3DTexture2, (void**)&rep.texture);

			DDSURFACEDESC2 _sd{ 0 };
			_sd.dwSize = sizeof(DDSURFACEDESC2);

			rep.surface->Lock(nullptr, &_sd, 1, 0);

			for (int i = 0; i < height * width; ++i) {
				auto c = ((uint32_t*)img)[i];
				auto r = c & 0xFF;
				auto g = (c >> 8) & 0xFF;
				auto b = (c >> 16) & 0xFF;
				auto a = (c >> 24);

				c = (a << 24) | (r << 16) | (g << 8) | b;

				((uint32_t*)_sd.lpSurface)[i] = c;
			}

			rep.surface->Unlock(nullptr);

			stbi_image_free(img);

			replacementTextures[key] = rep;

			/*g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
			g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);*/

			g_IDirect3DDevice3->SetTexture(0, rep.texture);
		} else {
			/*g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT);
			g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);*/

			DumpTexture(std::format("NewData\\Textures\\Dumped\\texture_{}_{}_{}_{}.dds", a1, a2, a3, a4));
		}
	}

	return r;
};


export void EnableTextureHooks() {
	EnableHook(LoadGlyphTexture, LoadGlyphTextureHook);
	EnableHook(SetTexture, SetTextureHook);
	//EnableHook(sub_5A3160, sub_5A3160Hook);

	WriteProtectedMemory(0x5C4638, 0);	// Fix UV coordinates for textures
}