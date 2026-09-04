module;

#include <freetype/freetype.h>
#include <freetype/ftmodapi.h>
#include <GLAD/glad.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <cstdint>

export module bof3ext.glGlyphManager;

import bof3ext.helpers;
import bof3ext.singleton;
import bof3ext.configManager;
import bof3.math;

import std;


constexpr int TEXTURE_SIZE = 2048;
constexpr int SDF_SPREAD = 4;


export class GlGlyphManager : public Singleton<GlGlyphManager> {
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

		if ((err = FT_Select_Charmap(face, FT_ENCODING_UNICODE)) != FT_Err_Ok) {
			LogError("Error selecting unicode charmap! Error code: %i\n", err);

			return false;
		}

		auto glyphWidth = (int)ceil(face->size->metrics.max_advance / 64.f);
		auto glyphHeight = (int)ceil(face->size->metrics.height / 64.f);

		cellWidth = glyphWidth + SDF_SPREAD * 2 + 4;
		cellHeight = glyphHeight + SDF_SPREAD * 2 + 4;

		auto cols = (int)floor((float)TEXTURE_SIZE / cellWidth);
		auto rows = (int)floor((float)TEXTURE_SIZE / cellHeight);

		auto empty = new uint8_t[TEXTURE_SIZE * TEXTURE_SIZE];
		memset(empty, 0, TEXTURE_SIZE * TEXTURE_SIZE);

		glGenTextures(1, &atlasTexId);
		glBindTexture(GL_TEXTURE_2D, atlasTexId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, empty);

		delete[] empty;

		FT_Property_Set(ft, "sdf", "spread", &SDF_SPREAD);

		int x = 0;
		int y = 0;
		FT_UInt glyphId = -1;

		auto charCode = FT_Get_First_Char(face, &glyphId);

		while (glyphId != 0) {
			LoadGlyph(ft, face, glyphId);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				x * cellWidth + face->glyph->bitmap_left + SDF_SPREAD,
				y * cellHeight + (cellHeight - face->glyph->bitmap_top) - (cellHeight - 2 - fontSize) + SDF_SPREAD,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			glyphMap[(uint16_t)charCode] = {
				x * cellWidth / (float)TEXTURE_SIZE,
				y * cellHeight / (float)TEXTURE_SIZE,
				(x + 1) * cellWidth / (float)TEXTURE_SIZE,
				(y + 1) * cellHeight / (float)TEXTURE_SIZE,
			};

			x++;

			if (x >= cols) {
				x = 0;
				y++;
			}

			charCode = FT_Get_Next_Char(face, charCode, &glyphId);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	GLuint GetTextureId() const {
		return atlasTexId;
	}

	int GetCellWidth() const {
		return cellWidth;
	}

	int GetCellHeight() const {
		return cellHeight;
	}

	const Vec4f& GetGlyphUv(uint16_t charCode) const {
		if (glyphMap.contains(charCode))
			return glyphMap.at(charCode);

		return zero;
	}

private:
	bool LoadGlyph(FT_Library ft, FT_Face face, unsigned index) {
		FT_Load_Glyph(face, index, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING);

		FT_Error err;

		if ((err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF)) != FT_Err_Ok) {
			LogError("Error rendering glyph! Error code: %i\n", err);

			return false;
		}

		return true;
	}


	GLuint atlasTexId;
	int cellWidth;
	int cellHeight;
	std::map<uint16_t, Vec4f> glyphMap;
	Vec4f zero{};
};