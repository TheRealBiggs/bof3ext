using System;
using System.Collections.Generic;

using Silk.NET.OpenGL;

using BoF3Ext.Wrappers;

namespace BoF3Ext;


unsafe class GlyphManager : Singleton<GlyphManager>, IDisposable {
	public const float FONT_SCALE = 4f;

	const int FONT_SIZE = (int)(10 * FONT_SCALE);
	const int CELL_SIZE = 64;
	const int TEXTURE_SIZE = 2048;
	const int SDF_SPREAD = 4;


	public uint TextureId => atlasTexId;
	public int CellWidth => cellWidth;
	public int CellHeight => cellHeight;
	public float ScaledGlyphAdvance => glyphAdvance / 64f / FONT_SCALE;


	GL? gl;
	nint ft;
	FreeType.Face* face;
	uint atlasTexId;
	int cellWidth;
	int cellHeight;
	readonly Dictionary<ushort, Vec4f> glyphMap = [];
	uint glyphAdvance;
	Vec4f zero = new();


	public bool Initialise() {
		int err;

		if ((err = FreeType.FT_Init_FreeType(out ft)) != 0) {
			Console.WriteErrorLine($"Error initialising FreeType! Error code: {err}");

			return false;
		}

		if ((err = FreeType.FT_New_Face(ft, ".\\NewData\\Fonts\\SimplyMono-Book.ttf", 0, out face)) != 0) {
			Console.WriteErrorLine($"Error loading font! Error code: {err}");

			return false;
		}

		if ((err = FreeType.FT_Set_Pixel_Sizes(face, FONT_SIZE, FONT_SIZE)) != 0) {
			Console.WriteErrorLine($"Error setting font size! Error code: {err}");

			return false;
		}

		if ((err = FreeType.FT_Select_Charmap(face, FreeType.ENCODING_UNICODE)) != 0) {
			Console.WriteErrorLine($"Error selecting unicode charmap! Error code: {err}");

			return false;
		}

		var spread = SDF_SPREAD;
		FreeType.FT_Property_Set(ft, "sdf", "spread", &spread);

		FreeType.FT_Load_Glyph(
			face,
			FreeType.FT_Get_Char_Index(face, '0'),
			FreeType.LOAD_NO_BITMAP | FreeType.LOAD_NO_HINTING
		);
		glyphAdvance = (uint)face->Glyph->Advance.X;

		return true;
	}

	public bool GenerateCache(GL gl) {
		this.gl = gl;
		byte[] empty = new byte[TEXTURE_SIZE * TEXTURE_SIZE];

		atlasTexId = gl.GenTexture();

		gl.BindTexture(TextureTarget.Texture2D, atlasTexId);
		{
			gl.TexImage2D(
				TextureTarget.Texture2D,
				0,
				InternalFormat.R8,
				TEXTURE_SIZE,
				TEXTURE_SIZE,
				0,
				PixelFormat.Red,
				PixelType.UnsignedByte,
				empty
			);

			cellWidth = CELL_SIZE;
			cellHeight = CELL_SIZE;

			var cols = (int)Math.Floor((float)TEXTURE_SIZE / cellWidth);
			var rows = (int)Math.Floor((float)TEXTURE_SIZE / cellHeight);

			int x = 0;
			int y = 0;
			uint glyphId = UInt32.MaxValue;

			var charCode = FreeType.FT_Get_First_Char(face, out glyphId);

			gl.PixelStore(PixelStoreParameter.UnpackAlignment, 1);

			while (glyphId != 0) {
				LoadGlyph(ft, face, glyphId);

				gl.TexSubImage2D(
					TextureTarget.Texture2D,
					0,
					x * cellWidth + face->Glyph->BitmapLeft + SDF_SPREAD,
					y * cellHeight + (cellHeight - face->Glyph->BitmapTop) - (cellHeight - 2 - FONT_SIZE) + SDF_SPREAD,
					face->Glyph->Bitmap.Width,
					face->Glyph->Bitmap.Rows,
					PixelFormat.Red,
					PixelType.UnsignedByte,
					face->Glyph->Bitmap.Buffer
				);

				glyphMap[(ushort)charCode] = new(
					x * cellWidth / (float)TEXTURE_SIZE,
					y * cellHeight / (float)TEXTURE_SIZE,
					(x + 1) * cellWidth / (float)TEXTURE_SIZE,
					(y + 1) * cellHeight / (float)TEXTURE_SIZE
				);

				x++;

				if (x >= cols) {
					x = 0;
					y++;
				}

				charCode = FreeType.FT_Get_Next_Char(face, charCode, out glyphId);
			}

			ReadOnlySpan<ushort> BUTTON_CHAR_CODES = [
				'◯', '☓', '⬜', '△',
			'⮪', 0, '⮫', 0
			];

			// Button glyphs
			TextureImage image;
			try {
				image = TextureImage.Load("NewData\\Fonts\\buttons_sdf_psx.png", greyscale: true);
			} catch (Exception e) {
				Console.WriteErrorLine($"Failed to load button SDF image: {e.Message}");

				return false;
			}

			var width = image.Width;
			var height = image.Height;
			var srcCols = width / CELL_SIZE;

			gl.GetInteger(GetPName.UnpackRowLength, out int rowLength);
			gl.PixelStore(PixelStoreParameter.UnpackRowLength, width);

			for (var i = 0; i < BUTTON_CHAR_CODES.Length; ++i) {
				var buttonCharCode = BUTTON_CHAR_CODES[i];

				if (buttonCharCode == 0)
					continue;

				var srcX = (i % srcCols) * CELL_SIZE;
				var srcY = (i / srcCols) * CELL_SIZE;

				gl.TexSubImage2D(
					TextureTarget.Texture2D,
					0,
					x * cellWidth,
					y * cellHeight,
					CELL_SIZE,
					CELL_SIZE,
					PixelFormat.Red,
					PixelType.UnsignedByte,
					ref image.Data[srcY * width + srcX]
				);

				glyphMap[buttonCharCode] = new(
					(x * cellWidth + 4) / (float)TEXTURE_SIZE,
					(y * cellHeight + 4) / (float)TEXTURE_SIZE,
					((x + 1) * cellWidth - 4) / (float)TEXTURE_SIZE,
					((y + 1) * cellHeight - 4) / (float)TEXTURE_SIZE
				);

				x++;

				if (x >= cols) {
					x = 0;
					y++;
				}
			}

			gl.PixelStore(PixelStoreParameter.UnpackRowLength, rowLength);

			gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)GLEnum.ClampToEdge);
			gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)GLEnum.ClampToEdge);
			gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)GLEnum.Linear);
			gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)GLEnum.Linear);
		}
		gl.BindTexture(TextureTarget.Texture2D, 0);

		FreeType.FT_Done_Face(face);
		FreeType.FT_Done_FreeType(ft);
		face = null;
		ft = 0;

		return true;
	}

	public Vec4f GetGlyphUv(ushort charCode) {
		if (glyphMap.TryGetValue(charCode, out Vec4f value))
			return value;

		return zero;
	}

	public void Dispose() {
		if (face != null) {
			FreeType.FT_Done_Face(face);
			face = null;
		}

		if (ft != 0) {
			FreeType.FT_Done_FreeType(ft);
			ft = 0;
		}

		if (atlasTexId != 0) {
			gl!.DeleteTexture(atlasTexId);
			atlasTexId = 0;
		}

		glyphMap.Clear();
	}


	bool LoadGlyph(nint ft, FreeType.Face* face, uint index) {
		FreeType.FT_Load_Glyph(face, index, FreeType.LOAD_NO_BITMAP | FreeType.LOAD_NO_HINTING);

		int err;

		if ((err = FreeType.FT_Render_Glyph(face->Glyph, FreeType.RENDER_MODE_SDF)) != 0) {
			Console.WriteErrorLine($"Error rendering glyph! Error code: {err}");

			return false;
		}

		return true;
	}
}
