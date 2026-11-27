using System;
using System.Runtime.CompilerServices;

using Silk.NET.OpenGL;

namespace BoF3Ext;


partial class Renderer {
	public void ProcessPrim_Tri(ref Game.Render.GpuPrim_Tri p) {
		ref var env = ref Game.Render.DrawEnv;

		GetBlendAndAlpha(ref p, env.TPage, out bool blendEnabled, out int blendMode, out float alpha);

		var cmd = CreateOrGetDrawCommand(DrawCommandType.Psx, -1, blendEnabled, blendMode, env.TextureWindow);

		Vec4f c = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, alpha);

		var count = (uint)cmd.Verts.Count;

		cmd.Verts.Add(new(p.Vert1, c, new(0, 0), -1, 0));
		cmd.Verts.Add(new(p.Vert2, c, new(0, 0), -1, 0));
		cmd.Verts.Add(new(p.Vert3, c, new(0, 0), -1, 0));

		cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
	}

	public void ProcessPrim_TriGouraud(ref Game.Render.GpuPrim_TriGouraud p) {
		ref var env = ref Game.Render.DrawEnv;

		GetBlendAndAlpha(ref p, env.TPage, out bool blendEnabled, out int blendMode, out float alpha);

		var cmd = CreateOrGetDrawCommand(DrawCommandType.Psx, -1, blendEnabled, blendMode, env.TextureWindow);

		Vec4f c1 = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, alpha);
		Vec4f c2 = new(p.Colour2.X / 255f, p.Colour2.Y / 255f, p.Colour2.Z / 255f, alpha);
		Vec4f c3 = new(p.Colour3.X / 255f, p.Colour3.Y / 255f, p.Colour3.Z / 255f, alpha);

		var count = (uint)cmd.Verts.Count;

		cmd.Verts.Add(new(p.Vert1, c1, new(0, 0), -1, 0));
		cmd.Verts.Add(new(p.Vert2, c2, new(0, 0), -1, 0));
		cmd.Verts.Add(new(p.Vert3, c3, new(0, 0), -1, 0));

		cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
	}

	public void ProcessPrim_Quad(ref Game.Render.GpuPrim_Quad p) {
		ref var env = ref Game.Render.DrawEnv;

		GetBlendAndAlpha(ref p, env.TPage, out bool blendEnabled, out int blendMode, out float alpha);

		var cmd = CreateOrGetDrawCommand(DrawCommandType.Psx, -1, blendEnabled, blendMode, env.TextureWindow);

		Vec4f c = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, alpha);

		var count = (uint)cmd.Verts.Count;

		cmd.Verts.Add(new(p.Vert1, c, new(0, 0), -1, 0));
		cmd.Verts.Add(new(p.Vert2, c, new(0, 0), -1, 0));
		cmd.Verts.Add(new(p.Vert3, c, new(0, 0), -1, 0));
		cmd.Verts.Add(new(p.Vert4, c, new(0, 0), -1, 0));

		cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
		cmd.Indices.Add(count + 2); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 3);
	}

	public void ProcessPrim_QuadGouraud(ref Game.Render.GpuPrim_QuadGouraud p) {
		ref var env = ref Game.Render.DrawEnv;

		GetBlendAndAlpha(ref p, env.TPage, out bool blendEnabled, out int blendMode, out float alpha);

		var cmd = CreateOrGetDrawCommand(DrawCommandType.Psx, -1, blendEnabled, blendMode, env.TextureWindow);

		Vec4f c1 = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, alpha);
		Vec4f c2 = new(p.Colour2.X / 255f, p.Colour2.Y / 255f, p.Colour2.Z / 255f, alpha);
		Vec4f c3 = new(p.Colour3.X / 255f, p.Colour3.Y / 255f, p.Colour3.Z / 255f, alpha);
		Vec4f c4 = new(p.Colour4.X / 255f, p.Colour4.Y / 255f, p.Colour4.Z / 255f, alpha);

		var count = (uint)cmd.Verts.Count;

		cmd.Verts.Add(new(p.Vert1, c1, new(0, 0), -1, 0));
		cmd.Verts.Add(new(p.Vert2, c2, new(0, 0), -1, 0));
		cmd.Verts.Add(new(p.Vert3, c3, new(0, 0), -1, 0));
		cmd.Verts.Add(new(p.Vert4, c4, new(0, 0), -1, 0));

		cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
		cmd.Indices.Add(count + 2); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 3);
	}

	public void ProcessPrim_QuadTexture(ref Game.Render.GpuPrim_QuadTexture p) {
		ref var env = ref Game.Render.DrawEnv;

		int mintx = Math.Min(p.Uv1.X, Math.Min(p.Uv2.X, Math.Min(p.Uv3.X, p.Uv4.X)));
		int maxtx = Math.Max(p.Uv1.X, Math.Max(p.Uv2.X, Math.Max(p.Uv3.X, p.Uv4.X)));
		int minty = Math.Min(p.Uv1.Y, Math.Min(p.Uv2.Y, Math.Min(p.Uv3.Y, p.Uv4.Y)));
		int maxty = Math.Max(p.Uv1.Y, Math.Max(p.Uv2.Y, Math.Max(p.Uv3.Y, p.Uv4.Y)));
		int tw = maxtx - mintx;
		int th = maxty - minty;

#if DEBUG
		if (ConfigManager.Instance.DumpTextures)
			TextureReplacer.Instance.DumpFrame(mintx, minty, tw, th, p.TPage, p.Palette);
#endif

		var rtex = TextureReplacer.Instance.GetReplacementTexture(
			mintx, minty, tw, th, p.TPage, p.Palette
		);

		GetBlendAndAlpha(ref p, p.TPage, out bool blendEnabled, out int blendMode, out float alpha);

		ref var textureWindow = ref env.TextureWindow;

		DrawCommand cmd;

		if (rtex.Atlas != null) {
			cmd = CreateOrGetDrawCommand(
				DrawCommandType.ReplacedTexture,
				(int)rtex.Atlas.Id,
				blendEnabled,
				blendMode,
				textureWindow
			);
		} else
			cmd = CreateOrGetDrawCommand(DrawCommandType.Psx, -1, blendEnabled, blendMode, textureWindow);

		Vec4f c = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, alpha);

		var count = (uint)cmd.Verts.Count;

		if (rtex.Atlas != null) {
			float t1x = p.Uv1.X - mintx, t1y = p.Uv1.Y - minty;
			float t2x = p.Uv2.X - mintx, t2y = p.Uv2.Y - minty;
			float t3x = p.Uv3.X - mintx, t3y = p.Uv3.Y - minty;
			float t4x = p.Uv4.X - mintx, t4y = p.Uv4.Y - minty;

			t1x /= tw; t1y /= th;
			t2x /= tw; t2y /= th;
			t3x /= tw; t3y /= th;
			t4x /= tw; t4y /= th;

			t1x *= rtex.Width; t1y *= rtex.Height;
			t2x *= rtex.Width; t2y *= rtex.Height;
			t3x *= rtex.Width; t3y *= rtex.Height;
			t4x *= rtex.Width; t4y *= rtex.Height;

			t1x += rtex.X; t1y += rtex.Y;
			t2x += rtex.X; t2y += rtex.Y;
			t3x += rtex.X; t3y += rtex.Y;
			t4x += rtex.X; t4y += rtex.Y;

			t1x /= rtex.Atlas.Width; t1y /= rtex.Atlas.Height;
			t2x /= rtex.Atlas.Width; t2y /= rtex.Atlas.Height;
			t3x /= rtex.Atlas.Width; t3y /= rtex.Atlas.Height;
			t4x /= rtex.Atlas.Width; t4y /= rtex.Atlas.Height;

			cmd.Verts.Add(new(p.Vert1, c, new(t1x, t1y)));
			cmd.Verts.Add(new(p.Vert2, c, new(t2x, t2y)));
			cmd.Verts.Add(new(p.Vert3, c, new(t3x, t3y)));
			cmd.Verts.Add(new(p.Vert4, c, new(t4x, t4y)));
		} else {
			var tpage = p.TPage;
			var palette = p.Palette;

			cmd.Verts.Add(new(p.Vert1, c, new(p.Uv1.X, p.Uv1.Y), tpage, palette));
			cmd.Verts.Add(new(p.Vert2, c, new(p.Uv2.X, p.Uv2.Y), tpage, palette));
			cmd.Verts.Add(new(p.Vert3, c, new(p.Uv3.X, p.Uv3.Y), tpage, palette));
			cmd.Verts.Add(new(p.Vert4, c, new(p.Uv4.X, p.Uv4.Y), tpage, palette));
		}

		cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
		cmd.Indices.Add(count + 2); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 3);
	}

	public void ProcessPrim_QuadTextureGouraud(ref Game.Render.GpuPrim_QuadTextureGouraud p) {
		ref var env = ref Game.Render.DrawEnv;

		int mintx = Math.Min(p.Uv1.X, Math.Min(p.Uv2.X, Math.Min(p.Uv3.X, p.Uv4.X)));
		int maxtx = Math.Max(p.Uv1.X, Math.Max(p.Uv2.X, Math.Max(p.Uv3.X, p.Uv4.X)));
		int minty = Math.Min(p.Uv1.Y, Math.Min(p.Uv2.Y, Math.Min(p.Uv3.Y, p.Uv4.Y)));
		int maxty = Math.Max(p.Uv1.Y, Math.Max(p.Uv2.Y, Math.Max(p.Uv3.Y, p.Uv4.Y)));
		int tw = maxtx - mintx;
		int th = maxty - minty;

#if DEBUG
		if (ConfigManager.Instance.DumpTextures)
			TextureReplacer.Instance.DumpFrame(mintx, minty, tw, th, p.TPage, p.Palette);
#endif

		var rtex = TextureReplacer.Instance.GetReplacementTexture(
			mintx, minty, tw, th, p.TPage, p.Palette
		);

		GetBlendAndAlpha(ref p, p.TPage, out bool blendEnabled, out int blendMode, out float alpha);

		ref var textureWindow = ref env.TextureWindow;

		DrawCommand cmd;

		if (rtex.Atlas != null) {
			cmd = CreateOrGetDrawCommand(
				DrawCommandType.ReplacedTexture,
				(int)rtex.Atlas.Id,
				blendEnabled,
				blendMode,
				textureWindow
			);
		} else
			cmd = CreateOrGetDrawCommand(DrawCommandType.Psx, -1, blendEnabled, blendMode, textureWindow);

		Vec4f c1 = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, alpha);
		Vec4f c2 = new(p.Colour2.X / 255f, p.Colour2.Y / 255f, p.Colour2.Z / 255f, alpha);
		Vec4f c3 = new(p.Colour3.X / 255f, p.Colour3.Y / 255f, p.Colour3.Z / 255f, alpha);
		Vec4f c4 = new(p.Colour4.X / 255f, p.Colour4.Y / 255f, p.Colour4.Z / 255f, alpha);

		var count = (uint)cmd.Verts.Count;

		if (rtex.Atlas != null) {
			float t1x = p.Uv1.X - mintx, t1y = p.Uv1.Y - minty;
			float t2x = p.Uv2.X - mintx, t2y = p.Uv2.Y - minty;
			float t3x = p.Uv3.X - mintx, t3y = p.Uv3.Y - minty;
			float t4x = p.Uv4.X - mintx, t4y = p.Uv4.Y - minty;

			t1x /= tw; t1y /= th;
			t2x /= tw; t2y /= th;
			t3x /= tw; t3y /= th;
			t4x /= tw; t4y /= th;

			t1x *= rtex.Width; t1y *= rtex.Height;
			t2x *= rtex.Width; t2y *= rtex.Height;
			t3x *= rtex.Width; t3y *= rtex.Height;
			t4x *= rtex.Width; t4y *= rtex.Height;

			t1x += rtex.X; t1y += rtex.Y;
			t2x += rtex.X; t2y += rtex.Y;
			t3x += rtex.X; t3y += rtex.Y;
			t4x += rtex.X; t4y += rtex.Y;

			t1x /= rtex.Atlas.Width; t1y /= rtex.Atlas.Height;
			t2x /= rtex.Atlas.Width; t2y /= rtex.Atlas.Height;
			t3x /= rtex.Atlas.Width; t3y /= rtex.Atlas.Height;
			t4x /= rtex.Atlas.Width; t4y /= rtex.Atlas.Height;

			cmd.Verts.Add(new(p.Vert1, c1, new(t1x, t1y)));
			cmd.Verts.Add(new(p.Vert2, c2, new(t2x, t2y)));
			cmd.Verts.Add(new(p.Vert3, c3, new(t3x, t3y)));
			cmd.Verts.Add(new(p.Vert4, c4, new(t4x, t4y)));
		} else {
			var tpage = p.TPage;
			var palette = p.Palette;

			cmd.Verts.Add(new(p.Vert1, c1, new(p.Uv1.X, p.Uv1.Y), tpage, palette));
			cmd.Verts.Add(new(p.Vert2, c2, new(p.Uv2.X, p.Uv2.Y), tpage, palette));
			cmd.Verts.Add(new(p.Vert3, c3, new(p.Uv3.X, p.Uv3.Y), tpage, palette));
			cmd.Verts.Add(new(p.Vert4, c4, new(p.Uv4.X, p.Uv4.Y), tpage, palette));
		}

		cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
		cmd.Indices.Add(count + 2); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 3);
	}

	public void ProcessPrim_RectWH(ref Game.Render.GpuPrim_RectWH p) {
		ref var env = ref Game.Render.DrawEnv;

		GetBlendAndAlpha(ref p, env.TPage, out bool blendEnabled, out int blendMode, out float alpha);

		DrawCommand cmd = CreateOrGetDrawCommand(DrawCommandType.Psx, -1, blendEnabled, blendMode, env.TextureWindow);

		Vec4f c = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, alpha);

		float x = p.Vert1.X;
		float y = p.Vert1.Y;
		float z = p.Vert1.Z;

		var w = (int)p.Size.X;
		var h = (int)p.Size.Y;

		var v = cmd.Verts;

		var count = (uint)v.Count;

		v.Add(new(new(x, y, z), c, new(0, 0), -1, 0));
		v.Add(new(new(x + w, y, z), c, new(0, 0), -1, 0));
		v.Add(new(new(x, y + h, z), c, new(0, 0), -1, 0));
		v.Add(new(new(x + w, y + h, z), c, new(0, 0), -1, 0));

		cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
		cmd.Indices.Add(count + 2); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 3);
	}

	public void ProcessPrim_RectWHTexture(ref Game.Render.GpuPrim_RectWHTexture p) {
		ref var env = ref Game.Render.DrawEnv;

		int tx = p.Uv1.X;
		int ty = p.Uv1.Y;
		int w = p.Size.X;
		int h = p.Size.Y;
		var tpage = (ushort)(env.TPage & ~0x3000);
		var flipV = (env.TPage & 0x2000) != 0;

#if DEBUG
		if (ConfigManager.Instance.DumpTextures)
			TextureReplacer.Instance.DumpFrame(tx, ty, w, h, tpage, p.Palette);
#endif

		var rtex = TextureReplacer.Instance.GetReplacementTexture(
			tx, ty, w, h, tpage, p.Palette
		);

		GetBlendAndAlpha(ref p, tpage, out bool blendEnabled, out int blendMode, out float alpha);

		ref var textureWindow = ref env.TextureWindow;

		DrawCommand cmd;

		if (rtex.Atlas != null) {
			cmd = CreateOrGetDrawCommand(
				DrawCommandType.ReplacedTexture,
				(int)rtex.Atlas.Id,
				blendEnabled,
				blendMode,
				textureWindow
			);
		} else
			cmd = CreateOrGetDrawCommand(DrawCommandType.Psx, -1, blendEnabled, blendMode, textureWindow);

		Vec4f c = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, alpha);

		float x = p.Vert1.X;
		float y = p.Vert1.Y;
		float z = p.Vert1.Z;

		var ftx = (float)tx;
		var fty = (float)ty;

		var v = cmd.Verts;

		var count = (uint)v.Count;

		if (rtex.Atlas != null) {
			var tx1 = (float)rtex.X / rtex.Atlas.Width;
			var tx2 = ((float)rtex.X + rtex.Width) / rtex.Atlas.Width;
			var ty1 = (float)rtex.Y / rtex.Atlas.Height;
			var ty2 = ((float)rtex.Y + rtex.Height) / rtex.Atlas.Height;

			if (flipV)
				(ty1, ty2) = (ty2, ty1);

			v.Add(new(new(x, y, z), c, new(tx1, ty1)));
			v.Add(new(new(x + w, y, z), c, new(tx2, ty1)));
			v.Add(new(new(x, y + h, z), c, new(tx1, ty2)));
			v.Add(new(new(x + w, y + h, z), c, new(tx2, ty2)));
		} else {
			var palette = p.Palette;

			var tx1 = ftx + 0.001f;
			var ty1 = fty + 0.001f;
			var tx2 = ftx + w - 0.001f;
			var ty2 = fty + h - 0.001f;

			if (flipV)
				(ty1, ty2) = (ty2, ty1);

			v.Add(new(new(x, y, z), c, new(tx1, ty1), tpage, palette));
			v.Add(new(new(x + w, y, z), c, new(tx2, ty1), tpage, palette));
			v.Add(new(new(x, y + h, z), c, new(tx1, ty2), tpage, palette));
			v.Add(new(new(x + w, y + h, z), c, new(tx2, ty2), tpage, palette));
		}

		cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
		cmd.Indices.Add(count + 2); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 3);
	}

	public void ProcessPrim_Lines(ref Game.Render.GpuPrim_Line p, int count) {
		ref var env = ref Game.Render.DrawEnv;

		GetBlendAndAlpha(ref p, env.TPage, out bool blendEnabled, out int blendMode, out float alpha);

		var cmd = CreateOrGetDrawCommand(DrawCommandType.Lines, -1, blendEnabled, blendMode, env.TextureWindow);

		Vec4f c = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, alpha);

		var vcount = (uint)cmd.Verts.Count;

		cmd.Verts.Add(new(p.Vert1, c, new(0, 0), -1, 0));
		cmd.Verts.Add(new(p.Vert2, c, new(0, 0), -1, 0));

		cmd.Indices.Add(vcount); cmd.Indices.Add(vcount + 1);

		if (count > 1) {
			ref var _p = ref Unsafe.As<Game.Render.GpuPrim_Line, Game.Render.GpuPrim_TwoLines>(ref p);

			cmd.Verts.Add(new(_p.Vert3, c, new(0, 0), -1, 0));

			cmd.Indices.Add(vcount + 2);
		}

		if (count > 2) {
			ref var _p = ref Unsafe.As<Game.Render.GpuPrim_Line, Game.Render.GpuPrim_ThreeLines>(ref p);

			cmd.Verts.Add(new(_p.Vert4, c, new(0, 0), -1, 0));

			cmd.Indices.Add(vcount + 3);
		}
	}

	public void ProcessPrim_TextGlyph(ref Game.Render.GpuPrim_TextGlyph p) {
		ref var env = ref Game.Render.DrawEnv;

		var glyphMgr = GlyphManager.Instance;
		var glyphUv = glyphMgr.GetGlyphUv(p.CharCode);

		var cmd = CreateOrGetDrawCommand(DrawCommandType.Text, (int)glyphMgr.TextureId, true, 0, env.TextureWindow);

		var width = p.Vert4.X - p.Vert1.X;
		var height = p.Vert4.Y - p.Vert1.Y;
		var sx = width / 12f;
		var sy = height / 12f;

		var pX = (p.PaletteIndex & 0x3F) << 4;
		var pY = p.PaletteIndex >> 6;

		var clutX = pY * 2048 + (pX + 1) * 2;

		var pC = cpuTPages[clutX] | (cpuTPages[clutX + 1] << 8);

		Vec4f c = new(
			(pC & 31) / 31f * (p.Colour1.X / 128f),
			((pC >> 5) & 31) / 31f * (p.Colour1.Y / 128f),
			((pC >> 10) & 31) / 31f * (p.Colour1.Z / 128f),
			1f
		);

		float x1 = p.Vert1.X;
		float y1 = p.Vert1.Y - (2.0f * sy);
		float x2 = p.Vert1.X + glyphMgr.CellWidth / GlyphManager.FONT_SCALE * sx;
		float y2 = p.Vert1.Y - (2.0f * sy) + glyphMgr.CellHeight / GlyphManager.FONT_SCALE * sy;
		float z = 0.99000001f;

		var tx1 = glyphUv.X;
		var ty1 = glyphUv.Y;
		var tx2 = glyphUv.Z;
		var ty2 = glyphUv.W;

		var count = (uint)cmd.Verts.Count;

		cmd.Verts.Add(new(new(x1, y1, z), c, new(tx1, ty1), 0, 0));
		cmd.Verts.Add(new(new(x2, y1, z), c, new(tx2, ty1), 0, 0));
		cmd.Verts.Add(new(new(x2, y2, z), c, new(tx2, ty2), 0, 0));
		cmd.Verts.Add(new(new(x1, y2, z), c, new(tx1, ty2), 0, 0));

		cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
		cmd.Indices.Add(count + 2); cmd.Indices.Add(count + 3); cmd.Indices.Add(count);
	}

	public void ProcessPrim_Rect8Texture(ref Game.Render.GpuPrim_Rect8Texture p) {
		ref var env = ref Game.Render.DrawEnv;

		int tx = p.Uv1.X;
		int ty = p.Uv1.Y;

#if DEBUG
		if (ConfigManager.Instance.DumpTextures)
			TextureReplacer.Instance.DumpFrame(tx, ty, 8, 8, env.TPage, p.Palette);
#endif

		var rtex = TextureReplacer.Instance.GetReplacementTexture(
			tx, ty, 8, 8, env.TPage, p.Palette
		);

		GetBlendAndAlpha(ref p, env.TPage, out bool blendEnabled, out int blendMode, out float alpha);

		ref var textureWindow = ref env.TextureWindow;

		DrawCommand cmd;

		if (rtex.Atlas != null) {
			cmd = CreateOrGetDrawCommand(
				DrawCommandType.ReplacedTexture,
				(int)rtex.Atlas.Id,
				blendEnabled,
				blendMode,
				textureWindow
			);
		} else
			cmd = CreateOrGetDrawCommand(DrawCommandType.Psx, -1, blendEnabled, blendMode, textureWindow);

		Vec4f c = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, alpha);

		float x = p.Vert1.X;
		float y = p.Vert1.Y;
		float z = p.Vert1.Z;

		var ftx = (float)tx;
		var fty = (float)ty;

		var v = cmd.Verts;

		var count = (uint)v.Count;

		if (rtex.Atlas != null) {
			var tx1 = (float)rtex.X / rtex.Atlas.Width;
			var tx2 = ((float)rtex.X + rtex.Width) / rtex.Atlas.Width;
			var ty1 = (float)rtex.Y / rtex.Atlas.Height;
			var ty2 = ((float)rtex.Y + rtex.Height) / rtex.Atlas.Height;

			v.Add(new(new(x, y, z), c, new(tx1, ty1)));
			v.Add(new(new(x + 8, y, z), c, new(tx2, ty1)));
			v.Add(new(new(x, y + 8, z), c, new(tx1, ty2)));
			v.Add(new(new(x + 8, y + 8, z), c, new(tx2, ty2)));
		} else {
			var tpage = env.TPage;
			var palette = p.Palette;

			var tx1 = ftx + 0.001f;
			var ty1 = fty + 0.001f;
			var tx2 = ftx + 8f - 0.001f;
			var ty2 = fty + 8f - 0.001f;

			v.Add(new(new(x, y, z), c, new(tx1, ty1), tpage, palette));
			v.Add(new(new(x + 8, y, z), c, new(tx2, ty1), tpage, palette));
			v.Add(new(new(x, y + 8, z), c, new(tx1, ty2), tpage, palette));
			v.Add(new(new(x + 8, y + 8, z), c, new(tx2, ty2), tpage, palette));
		}

		cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
		cmd.Indices.Add(count + 2); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 3);
	}

	public void ProcessPrim_Sprite(ref Game.Render.GpuPrim_Sprite p) {
		ref var env = ref Game.Render.DrawEnv;

		GetBlendAndAlpha(ref p, env.TPage, out bool blendEnabled, out int blendMode, out float alpha);

		Vec4f c = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, alpha);

		for (int i = p.BlockIndex; i < p.BlockIndex + p.BlockCount; ++i) {
			ref var block = ref Game.Render.TextureBlockInfos[i];

			int tx = block.Tx;
			int ty = block.Ty;

			int w = 8 * (block.Size & 0xF);
			int h = (block.Size >> 1) & 0x78;

#if DEBUG
			if (ConfigManager.Instance.DumpTextures)
				TextureReplacer.Instance.DumpFrame(tx, ty, w, h, block.TPage, p.Palette);
#endif

			var rtex = TextureReplacer.Instance.GetReplacementTexture(
				tx, ty, w, h, block.TPage, p.Palette
			);

			var x = p.Position.X;
			var y = p.Position.Y;

			float x1, x2;

			if ((p.TPage & 0x400) != 0) {
				x1 = x - block.RelX * p.Scale.X;
				x2 = x1 - w * p.Scale.X;
			} else {
				x1 = x + block.RelX * p.Scale.X;
				x2 = x1 + w * p.Scale.X;
			}

			var y1 = y + block.RelY * p.Scale.Y;
			var y2 = y + (block.RelY + h) * p.Scale.Y;

			float tx1, tx2;
			float ty1, ty2;

			if (rtex.Atlas != null) {
				tx1 = (float)rtex.X / rtex.Atlas.Width;
				tx2 = ((float)rtex.X + rtex.Width) / rtex.Atlas.Width;
				ty1 = (float)rtex.Y / rtex.Atlas.Height;
				ty2 = ((float)rtex.Y + rtex.Height) / rtex.Atlas.Height;
			} else {
				tx1 = block.Tx + 0.001f;
				tx2 = block.Tx + w - 0.001f;
				ty1 = block.Ty + 0.001f;
				ty2 = block.Ty + h - 0.001f;
			}

			if ((block.TPage & 0x200) != 0)
				(tx1, tx2) = (tx2, tx1);

			ref var textureWindow = ref env.TextureWindow;

			DrawCommand cmd;

			if (rtex.Atlas != null) {
				cmd = CreateOrGetDrawCommand(
					DrawCommandType.ReplacedTexture,
					(int)rtex.Atlas.Id,
					blendEnabled,
					blendMode,
					textureWindow
				);
			} else
				cmd = CreateOrGetDrawCommand(DrawCommandType.Psx, -1, blendEnabled, blendMode, textureWindow);

			var v = cmd.Verts;

			var count = (uint)v.Count;

			var tpage = block.TPage;
			var palette = p.Palette;

			v.Add(new(new(x1, y1, 0.99f), c, new(tx1, ty1), tpage, palette));
			v.Add(new(new(x2, y1, 0.99f), c, new(tx2, ty1), tpage, palette));
			v.Add(new(new(x2, y2, 0.99f), c, new(tx2, ty2), tpage, palette));
			v.Add(new(new(x1, y2, 0.99f), c, new(tx1, ty2), tpage, palette));

			cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
			cmd.Indices.Add(count + 2); cmd.Indices.Add(count + 3); cmd.Indices.Add(count);
		}
	}

	public void ProcessPrim_TPageCopy(ref Game.Render.GpuPrim_TPageCopy p) {
		int w = p.SrcRect.W;
		int h = p.SrcRect.H;

		if (w == 0 || h == 0)
			return;

		int sx = p.SrcRect.X;
		int sy = p.SrcRect.Y;
		int dx = (int)p.DstX;
		int dy = (int)p.DstY;

		if (sx >= 0) {
			if (w + sx > 1024)
				w = 1024 - sx;
		} else {
			w = sx + w;
			dx += sx;
			sx = 0;
		}

		if (sy >= 0) {
			if (h + sy > 512)
				h = 512 - sy;
		} else {
			h = sy + h;
			dy += sy;
			sy = 0;
		}

		if (w <= 0 || h <= 0)
			return;

		gl.BindBuffer(BufferTargetARB.CopyReadBuffer, ssboTPages);
		gl.BindBuffer(BufferTargetARB.CopyWriteBuffer, ssboTPages);

		nint sofs = sy * 2048 + sx * 2;
		nint dofs = dy * 2048 + dx * 2;

		for (int i = 0; i < h; ++i) {
			gl.CopyBufferSubData(
				GLEnum.CopyReadBuffer,
				GLEnum.CopyWriteBuffer,
				sofs + i * 2048,
				dofs + i * 2048,
				(nuint)(w * 2)
			);

			Unsafe.CopyBlockUnaligned(ref cpuTPages[dofs + i * 2048], ref cpuTPages[sofs + i * 2048], (uint)(w * 2));
		}
	}

	public void ProcessPrim_GlTextureFrame(ref Game.Render.GpuPrim_GlTextureFrame p) {
		ref var env = ref Game.Render.DrawEnv;

		var cmd = CreateOrGetDrawCommand(DrawCommandType.ReplacedTexture, (int)p.TextureId, true, 0, env.TextureWindow);

		var x1 = p.Vert1.X;
		var y1 = p.Vert1.Y;
		var x2 = x1 + p.Size.X;
		var y2 = y1 + p.Size.Y;

		Vec4f colour = new(p.Colour1.X / 255f, p.Colour1.Y / 255f, p.Colour1.Z / 255f, 1f);

		var tx1 = p.SrcRect.X;
		var tx2 = p.SrcRect.Z;
		var ty1 = p.SrcRect.Y;
		var ty2 = p.SrcRect.W;

		var count = (uint)cmd.Verts.Count;

		cmd.Verts.Add(new(new(x1, y1, 0.99000001f), colour, new(tx1, ty1)));
		cmd.Verts.Add(new(new(x2, y1, 0.99000001f), colour, new(tx2, ty1)));
		cmd.Verts.Add(new(new(x2, y2, 0.99000001f), colour, new(tx2, ty2)));
		cmd.Verts.Add(new(new(x1, y2, 0.99000001f), colour, new(tx1, ty2)));

		cmd.Indices.Add(count); cmd.Indices.Add(count + 1); cmd.Indices.Add(count + 2);
		cmd.Indices.Add(count + 2); cmd.Indices.Add(count + 3); cmd.Indices.Add(count);
	}
}
