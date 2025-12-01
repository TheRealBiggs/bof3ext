module;

#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>

#include <cstdint>

export module bof3ext.glyphManager;

import bof3ext.helpers;
import bof3ext.singleton;

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

		if ((err = FT_Set_Pixel_Sizes(face, 40, 40)) != FT_Err_Ok) {
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

		if ((err = FT_Set_Pixel_Sizes(face, 40, 40)) != FT_Err_Ok) {
			LogError("Error setting font size! Error code: %i\n", err);

			return false;
		}

		LoadGlyph(ft, face, u'ƶ');

		FT_Done_FreeType(ft);

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

		pos = { 2 * 64, -2 * 64 };
		FT_Bitmap_Blend(ft, &face->glyph->bitmap, { 0, 0 }, &bitmap, &pos, FT_Color{ 224, 224, 224, 255 });

		auto buf = new unsigned char[bitmap.rows * bitmap.pitch];
		std::memcpy(buf, bitmap.buffer, bitmap.rows * bitmap.pitch);

		cachedGlyphs[charCode] = CachedGlyph{
			bitmap.rows,
			bitmap.pitch,
			face->glyph->bitmap_left,
			48 - face->glyph->bitmap_top,
			buf
		};

		FT_Bitmap_Done(ft, &bitmap);
	}


	std::map<uint16_t, CachedGlyph> cachedGlyphs;
	unsigned int glyphAdvance;
};