using System;
using System.Runtime.CompilerServices;

namespace BoF3Ext;


static class RendererHelpers {
	public static ref T GetGpuPrim<T>() where T : unmanaged
		=> ref Unsafe.As<Game.Render.GpuPrim, T>(ref Game.Render.GpuPrims);

	public static void AdvanceGpuPrims<T>() where T : unmanaged
		=> Game.Render.GpuPrimsPointer += Unsafe.SizeOf<T>();

	public static void SetDrawEnv(byte tp, byte abr, int tx, int ty) {
		ref var prim = ref GetGpuPrim<Game.Render.GpuPrim_SetDrawEnv>();

		var tpage = Game.Render.GetTPage(tp, abr, tx, ty);
		Game.Render.InitGpuPrim_SetDrawEnv(ref prim, 0, 0, tpage, ref Unsafe.NullRef<Game.Render.PSX_RECT>());

		Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_SetDrawEnv.UnmanagedSize);
	}

	public static void DrawRectWH(int x, int y, int width, int height, Vec3b colour, bool blend) {
		ref var prim = ref GetGpuPrim<Game.Render.GpuPrim_RectWH>();
		prim.Value = (int)Game.Render.GpuPrimType.RectWH << 2;

		prim.Colour1 = colour;
		prim.Vert1 = new(x, y, 0.01f);
		prim.Size = new(width, height);

		if (blend)
			prim.Value |= 1 << 1;

		Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_RectWH.UnmanagedSize);
	}

	public static void DrawRectWHTexture(
		int x, int y, int width, int height, int tx, int ty, ushort tpage, ushort palette, Vec3b colour, bool blend
	) {
		ref var drawPrim = ref GetGpuPrim<Game.Render.GpuPrim_SetDrawEnv>();
		Game.Render.InitGpuPrim_SetDrawEnv(ref drawPrim, 0, 0, tpage, ref Unsafe.NullRef<Game.Render.PSX_RECT>());
		Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_SetDrawEnv.UnmanagedSize);

		ref var prim = ref GetGpuPrim<Game.Render.GpuPrim_RectWHTexture>();
		prim.Value = (int)Game.Render.GpuPrimType.RectWHTexture << 2;

		prim.Palette = palette;
		prim.Colour1 = colour;
		prim.Vert1 = new(x, y, 0.01f);
		prim.Uv1 = new((byte)tx, (byte)ty);
		prim.Size = new((ushort)width, (ushort)height);

		if (blend)
			prim.Value |= 1 << 1;

		Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_RectWHTexture.UnmanagedSize);
	}

	public static void DrawQuad(Vec2i v1, Vec2i v2, Vec2i v3, Vec2i v4, Vec3b colour, bool blend) {
		ref var prim = ref GetGpuPrim<Game.Render.GpuPrim_Quad>();
		prim.Value = (int)Game.Render.GpuPrimType.Quad << 2;

		prim.Colour1 = colour;
		prim.Vert1 = new(v1.X, v1.Y, 0.01f);
		prim.Vert2 = new(v2.X, v2.Y, 0.01f);
		prim.Vert3 = new(v3.X, v3.Y, 0.01f);
		prim.Vert4 = new(v4.X, v4.Y, 0.01f);

		if (blend)
			prim.Value |= 1 << 1;

		Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_Quad.UnmanagedSize);
	}

	public static void DrawQuadTexture(
		Vec2i v1, Vec2i v2, Vec2i v3, Vec2i v4,
		Vec2b uv1, Vec2b uv2, Vec2b uv3, Vec2b uv4,
		ushort tpage, ushort palette,
		Vec3b colour, bool blend
	) {
		ref var prim = ref GetGpuPrim<Game.Render.GpuPrim_QuadTexture>();
		prim.Value = (int)Game.Render.GpuPrimType.QuadTexture << 2;

		prim.Palette = palette;
		prim.TPage = tpage;
		prim.Colour1 = colour;
		prim.Vert1 = new(v1.X, v1.Y, 0.01f);
		prim.Vert2 = new(v2.X, v2.Y, 0.01f);
		prim.Vert3 = new(v3.X, v3.Y, 0.01f);
		prim.Vert4 = new(v4.X, v4.Y, 0.01f);
		prim.Uv1 = uv1;
		prim.Uv2 = uv2;
		prim.Uv3 = uv3;
		prim.Uv4 = uv4;

		if (blend)
			prim.Value |= 1 << 1;

		Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_QuadTexture.UnmanagedSize);
	}

	public static void DrawBorderedPanel(int x, int y, int width, int height, Vec3b colour, bool blend = true) {
		SetDrawEnv(0, 0, 960, 0);

		// Background
		DrawRectWH(x, y + 2, width, height - 4, colour, blend);

		// Background top
		DrawQuad(new(x + 2, y), new(x + width - 2, y), new(x, y + 2), new(x + width, y + 2), colour, blend);

		// Background bottom
		DrawQuad(
			new(x, y + height - 2),
			new(x + width, y + height - 2),
			new(x + 2, y + height),
			new(x + width - 2, y + height),
			colour,
			blend
		);

		SetDrawEnv(0, 1, 960, 0);

		// Top line
		DrawRectWH(x + 2, y + 2, width - 4, 1, colour, true);

		// Left line
		DrawRectWH(x + 2, y + 2, 1, height - 4, colour, true);

		SetDrawEnv(0, 0, 960, 0);

		// Bottom line
		DrawRectWH(x + 2, y + height - 3, width - 4, 1, colour, true);

		// Right line
		DrawRectWH(x + width - 3, y + 2, 1, height - 4, colour, true);
	}

	public static void DrawDialogueBackground(int x, int y, int width, int height) {
		SetDrawEnv(0, 0, 960, 0);

		DrawRectWH(x, y, width + 1, height + 1, new(0, 0, 0), true);

		SetWindowTextureWindow(0, 240, 16, 16, 15);

		ref var prim = ref GetGpuPrim<Game.Render.GpuPrim_QuadTextureGouraud>();
		prim.Value = (byte)(((int)Game.Render.GpuPrimType.QuadTextureGouraud << 2) | 2);

		prim.Colour1 = new(255, 255, 255);
		prim.Colour2 = prim.Colour3 = prim.Colour4 = prim.Colour1;

		prim.Vert1 = new(x, y, 0.01f);
		prim.Vert2 = new(x + width + 1, y, 0.01f);
		prim.Vert3 = new(x, y + height + 1, 0.01f);
		prim.Vert4 = new(x + width + 1, y + height + 1, 0.01f);

		prim.Uv1 = new(0, 0);
		prim.Uv2 = new((byte)width, 0);
		prim.Uv3 = new(0, (byte)height);
		prim.Uv4 = new((byte)width, (byte)height);

		var paletteIdx = Game.World.PersistentStateData.WindowColour;

		prim.Palette = (ushort)((481 << 6) | ((16 + paletteIdx * 32) >> 4));
		prim.TPage = Game.Render.GetTPage(0, 0, 960, 0);

		Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_QuadTextureGouraud.UnmanagedSize);

		SetWindowTextureWindow(0, 0, 256, 256, 5);
	}

	public static void DrawDialogueBorder(int x, int y, int width, int height) {
		var textureId = Renderer.Instance.DialogueBorderTextureId;

		ReadOnlySpan<int> xEdges = [x, x + 2, x + width - 2, x + width];
		ReadOnlySpan<int> yEdges = [y, y + 2, y + height - 2, y + height];

		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				if (row == 1 && col == 1)
					continue;

				var sliceWidth = xEdges[col + 1] - xEdges[col];
				var sliceHeight = yEdges[row + 1] - yEdges[row];

				if (sliceWidth <= 0 || sliceHeight <= 0)
					continue;

				DrawTextureFrame(
					textureId,
					new(col / 3f, row / 3f, (col + 1) / 3f, (row + 1) / 3f),
					xEdges[col],
					yEdges[row],
					sliceWidth,
					sliceHeight
				);
			}
		}
	}

	public static void SetWindowTextureWindow(short x, short y, short width, short height, ushort tpage) {
		ref var rect = ref GetGpuPrim<Game.Render.PSX_RECT>();
		rect = new(x, y, width, height);

		AdvanceGpuPrims<Game.Render.PSX_RECT>();

		ref var prim = ref GetGpuPrim<Game.Render.GpuPrim_SetDrawEnv>();

		Game.Render.InitGpuPrim_SetDrawEnv(ref prim, 0, 0, tpage, ref rect);
		Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_SetDrawEnv.UnmanagedSize);
	}

	public static void DrawTextureFrame(uint textureId, Vec4f srcRect, int x, int y, int width, int height) {
		ref var prim = ref GetGpuPrim<Game.Render.GpuPrim_GlTextureFrame>();
		prim.Value = (int)Game.Render.GpuPrimType.GlTextureFrame << 2;
		prim.Colour1 = new(128, 128, 128);

		prim.Vert1 = new(x, y, 0.01f);
		prim.Size = new(width, height);
		prim.TextureId = textureId;
		prim.SrcRect = srcRect;

		Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_GlTextureFrame.UnmanagedSize);
	}

	public static void DrawConfigBackgroundWindow(int x, int y, int columns, int rows) {
		ref var rect = ref GetGpuPrim<Game.Render.PSX_RECT>();
		rect = new(0, 0, 256, 256);

		AdvanceGpuPrims<Game.Render.PSX_RECT>();

		var tpage = Game.Render.GetTPage(0, 0, 832, 256);

		ref var drawPrim = ref GetGpuPrim<Game.Render.GpuPrim_SetDrawEnv>();

		Game.Render.InitGpuPrim_SetDrawEnv(ref drawPrim, 0, 0, tpage, ref rect);
		Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_SetDrawEnv.UnmanagedSize);

		var x1 = (short)x;
		var y1 = (short)y;
		var x2 = (short)(x + 8 * columns - 16);
		var y2 = (short)(y + 8 * rows - 16);

		// Top-left
		Game.Gui.DrawUiPart(x1, y1, 35, 1);

		// Top-right
		Game.Gui.DrawUiPart(x2, y1, 38, 1);

		// Bottom-left
		Game.Gui.DrawUiPart(x1, y2, 39, 1);

		// Bottom-right
		Game.Gui.DrawUiPart(x2, y2, 41, 1);

		Vec3b colour = new(128, 128, 128);
		var palette = Game.Render.CreatePaletteIndex(176, 481);


		static void DrawRepeating(
			Game.Render.PSX_RECT rect,
			int x,
			int y,
			int width,
			int height,
			int tx,
			int ty,
			ushort tpage,
			Vec3b colour,
			ushort palette
		) {
			ref var rectPrim = ref GetGpuPrim<Game.Render.PSX_RECT>();
			rectPrim = rect;

			AdvanceGpuPrims<Game.Render.PSX_RECT>();

			ref var drawPrim = ref GetGpuPrim<Game.Render.GpuPrim_SetDrawEnv>();

			Game.Render.InitGpuPrim_SetDrawEnv(ref drawPrim, 0, 0, tpage, ref rectPrim);
			Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_SetDrawEnv.UnmanagedSize);

			ref var prim = ref GetGpuPrim<Game.Render.GpuPrim_RectWHTexture>();
			Game.Render.InitGpuPrim_RectWHTexture(ref prim);

			prim.Vert1.X = x;
			prim.Vert1.Y = y;
			prim.Uv1 = new((byte)tx, (byte)ty);
			prim.Size = new((ushort)width, (ushort)height);
			prim.Colour1 = colour;
			prim.Palette = palette;

			Game.Render.PushGpuPrim(1, (byte)Game.Render.GpuPrim_RectWHTexture.UnmanagedSize);
		}


		var hWidth = 8 * columns - 32;
		var vHeight = 8 * rows - 32;

		// Top
		DrawRepeating(new(112, 152, 8, 8), x + 16, y, hWidth, 8, 0, 0, tpage, colour, palette);

		// Bottom
		DrawRepeating(new(152, 160, 8, 8), x + 16, y + 8 * rows - 8, hWidth, 8, 0, 0, tpage, colour, palette);

		// Left
		DrawRepeating(new(176, 152, 8, 8), x, y + 16, 8, vHeight, 0, 0, tpage, colour, palette);

		// Right
		DrawRepeating(new(176, 160, 8, 8), x + 8 * columns - 8, y + 16, 8, vHeight, 0, 0, tpage, colour, palette);

		// Middle
		DrawRepeating(new(112, 160, 8, 8), x + 8, y + 8, 8 * columns - 16, 8 * rows - 16, 0, 0, tpage, colour, palette);
	}
}
