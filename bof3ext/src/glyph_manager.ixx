module;

#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>
#include <stb/stb_image.h>

#include <cstdint>

export module bof3ext.glyphManager;

import bof3ext.helpers;
import bof3ext.singleton;
import bof3ext.configManager;

import std;


export struct CachedGlyph {
	unsigned int rows;
	int pitch;
	int left;
	int top;
	unsigned char* buffer;
};


export class GlyphManager : public Singleton<GlyphManager> {
public:
	bool Initialise() {
		FT_Library ft;
		FT_Face face;

		FT_Error err;

		if ((err = FT_Init_FreeType(&ft)) != FT_Err_Ok) {
			LogError("Error initialising FreeType! Error code: %i\n", err);

			return false;
		}

		if ((err = FT_New_Face(ft, ".\\NewData\\Fonts\\SimplyMono-Book.ttf", 0, &face)) != FT_Err_Ok) {
			LogError("Error loading font! Error code: %i\n", err);

			return false;
		}

		auto fontSize = (int)(0.625 * 16 * ConfigManager::Get().GetRenderScale());

		if ((err = FT_Set_Pixel_Sizes(face, fontSize, fontSize)) != FT_Err_Ok) {
			LogError("Error setting font size! Error code: %i\n", err);

			return false;
		}

		for (char c = '!'; c <= '~'; ++c)
			LoadGlyph(ft, face, c);

		LoadGlyph(ft, face, u'»');

		FT_Done_Face(face);

		if ((err = FT_New_Face(ft, ".\\NewData\\Fonts\\ConsolaMono-Book.ttf", 0, &face)) != FT_Err_Ok) {
			LogError("Error loading font! Error code: %i\n", err);

			return false;
		}

		if ((err = FT_Set_Pixel_Sizes(face, fontSize, fontSize)) != FT_Err_Ok) {
			LogError("Error setting font size! Error code: %i\n", err);

			return false;
		}

		LoadGlyph(ft, face, u'ƶ');

		FT_Done_FreeType(ft);

		int width, height, channels;

		uint8_t* img = stbi_load(".\\NewData\\Fonts\\EX.png", &width, &height, &channels, 0);
		auto buf = new uint8_t[width * height * channels];

		for (int i = 0; i < width * height; i++) {
			auto c = ((uint32_t*)img)[i];
			auto r = c & 0xFF;
			auto g = (c >> 8) & 0xFF;
			auto b = (c >> 16) & 0xFF;
			auto a = (c >> 24);

			c = (a << 24) | (r << 16) | (g << 8) | b;

			*((uint32_t*)buf + i) = c;
		}

		stbi_image_free(img);

		CachedGlyph ex{ 0 };
		ex.buffer = buf;
		ex.pitch = width * channels;
		ex.rows = height;
		ex.top = 48 - 24;

		cachedGlyphs[0x101] = ex;

		return true;
	}


	bool HasGlyph(unsigned short charCode) const {
		return cachedGlyphs.count(charCode) != 0;
	}

	const CachedGlyph& GetGlyph(unsigned short charCode) const {
		return cachedGlyphs.at(charCode);
	}

	unsigned int GetGlyphAdvance() const {
		return glyphAdvance / 64;
	}

	float GetScaledGlyphAdvance() const {
		return GetGlyphAdvance() / ConfigManager::Get().GetRenderScale();
	}


private:
	void LoadGlyph(FT_Library ft, FT_Face face, unsigned short charCode) {
		FT_Load_Char(face, charCode, FT_LOAD_NO_BITMAP);

		if (charCode == '0')
			glyphAdvance = face->glyph->advance.x;

		FT_Error err;

		if ((err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) != FT_Err_Ok) {
			LogError("Error rendering glyph! Error code: %i\n", err);

			return;
		}

		FT_Bitmap bitmap;
		FT_Bitmap_Init(&bitmap);

		FT_Vector pos{ 0 * 64, 0 * 64 };

		FT_Bitmap_Blend(ft, &face->glyph->bitmap, { 0, 0 }, &bitmap, &pos, FT_Color{ 32, 32, 32, 255 });

		auto renderScale = ConfigManager::Get().GetRenderScale();

		auto shadowOffset = 0.5 * renderScale;

		pos = { (long)(shadowOffset * 64), -(long)(shadowOffset * 64) };
		FT_Bitmap_Blend(ft, &face->glyph->bitmap, { 0, 0 }, &bitmap, &pos, FT_Color{ 224, 224, 224, 255 });

		auto buf = new unsigned char[bitmap.rows * bitmap.pitch];
		std::memcpy(buf, bitmap.buffer, bitmap.rows * bitmap.pitch);

		auto surfRectSize = (int)(0.75 * 16 * renderScale);

		cachedGlyphs[charCode] = CachedGlyph{
			bitmap.rows,
			bitmap.pitch,
			face->glyph->bitmap_left,
			surfRectSize - face->glyph->bitmap_top,
			buf
		};

		FT_Bitmap_Done(ft, &bitmap);
	}


	std::map<uint16_t, CachedGlyph> cachedGlyphs;
	unsigned int glyphAdvance;
};