using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Hooks;


static class Widescreen {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawOverworldRegionPanel_Area016Hook(short x, short y) {
		if ((Game.World.FieldRenderFlags & 0x1B) == 0)
			return;

		Game.Render.DrawOverworldUiTextureFrame(x, y, 4);
		Game.Render.DrawOverworldUiTextureFrame((short)(x + 128), y, 5);

		TextManager.Instance.TryGetDialogue(16, 4, out var text);

		// Remove leading spaces
		while (text.GameString.Pointer[0] == ' ')
			text = new(new(text.GameString.Pointer + 1), text.Length - 1, text.ByteCount - 1);

		x += 64;
		x -= (short)Math.Round(text.Length * GlyphManager.Instance.ScaledGlyphAdvance / 2.0f);

		Game.Render.DrawStringNormal(x, (short)(y + 3), 0, 0xFF, text.GameString);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawOverworldCompassNeedleHook(short x, short y) {
		// Negate shader offset
		x -= (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.OriginalDrawOverworldCompassNeedle(x, y);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe nint DrawTitleDemoTextureFrameHook(short x, short y, byte frameIndex, byte drawIndex, byte translucent) {
		if (frameIndex == 10)		// 10 = Demo logo
			x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		return Game.Render.OriginalDrawTitleDemoTextureFrame(x, y, frameIndex, drawIndex, translucent);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawTapestryHook() {
		if (Game.Intro.TapestryState == 0)
			return;

		var scroll = (short)(Game.Intro.TapestryScroll + 2);
		Game.Intro.TapestryScroll = scroll;

		var fade = (int)Game.Intro.TapestryFade;
		var fadeStep = Math.Clamp(4 - scroll / 640, 0, 4);

		switch (Game.Intro.TapestryState) {
			case 1:
				fade = Math.Min(128, fade + fadeStep);

				if (fade == 128)
					Game.Intro.TapestryState = 2;

				break;

			case 3:
				fade = Math.Max(0, fade - fadeStep);

				if (fade == 0)
					Game.Intro.TapestryState = 0;

				break;

			default:
				fade = 128;

				break;
		}

		Game.Intro.TapestryFade = (byte)fade;

		if (Game.Intro.TapestryState == 0)
			return;

		for (var section = 0; section < 4; section++)
			DrawTapestrySection(section, scroll, (byte)fade);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawDialoguePageIndicatorHook(short x, short y) {
		Game.Gui.OriginalDrawDialoguePageIndicator(x, (short)(y + 2));
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawSkyHook(Game.Render.SkyRenderCommand* command) {
		if (Game.Terrain.BackgroundDrawFlags == 0)
			return;

		var x = (uint)(0x7FFF - Game.Terrain.PositionX) >> 8;
		var y = (uint)(0x8000 - Game.Terrain.PositionY) >> 8;
		var bounds = command->Bounds;

		if (x < bounds.X || y < bounds.Y || x > bounds.Z || y > bounds.W)
			return;

		ref var drawPrim = ref RendererHelpers.GetGpuPrim<Game.Render.GpuPrim_SetDrawEnv>();
		Game.Render.InitGpuPrim_SetDrawEnv(ref drawPrim, 0, 1, 149, ref Unsafe.NullRef<Game.Render.PSX_RECT>());
		Game.Render.PushGpuPrim(7, (byte)Game.Render.GpuPrim_SetDrawEnv.UnmanagedSize);

		var colours = Game.Render.UseAlternateColours ? command->AlternateColours : command->Colours;

		Vec3b cStart = new(
			(byte)((colours & 31) * 8),
			(byte)(((colours >> 5) & 31) * 8),
			(byte)(((colours >> 10) & 31) * 8)
		);

		Vec3b cEnd = new(
			(byte)(((colours >> 16) & 31) * 8),
			(byte)(((colours >> 21) & 31) * 8),
			(byte)(((colours >> 26) & 31) * 8)
		);

		var left = -ConfigManager.Instance.HalfRenderWidthDiff;
		var right = ConfigManager.Instance.ScaledRenderWidth + left;

		ref var prim = ref RendererHelpers.GetGpuPrim<Game.Render.GpuPrim_QuadGouraud>();
		prim.Value = (byte)((int)Game.Render.GpuPrimType.QuadGouraud << 2);
		prim.Vert1 = new(left, 0, 0.01f);
		prim.Vert2 = new(right, 0, 0.01f);
		prim.Vert3 = new(left, 240, 0.01f);
		prim.Vert4 = new(right, 240, 0.01f);
		prim.Colour1 = prim.Colour2 = cStart;
		prim.Colour3 = prim.Colour4 = cEnd;

		Game.Render.PushGpuPrim(7, (byte)Game.Render.GpuPrim_QuadGouraud.UnmanagedSize);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawSelectedFrameOutlineHook(short x, short y, ushort width, ushort height, int pulse, int colour) {
		ref var window = ref Game.Gui.CurrentUiWindow;

		if (!Unsafe.IsNullRef(ref window) && window.Type == 6 && window.State == 11) {
			var col = window.StateParamsU8[3] & 1;
			var panelX = Gui.GetSlidingWindowX(window.X, 150 + 75 * col, 320 + 75 * col);
			x += (short)(panelX - window.X);
		}

		// TODO: Replace the hooked function entirely to use quads instead of lines

		var cfgMgr = ConfigManager.Instance;
		var scale = cfgMgr.RenderScale;

		x -= (short)Math.Round(scale / 4.0f);
		y -= (short)Math.Round(scale / 4.0f);
		width += (ushort)Math.Round(scale / 2.0f);
		height += (ushort)Math.Round(scale);

		Game.Gui.OriginalDrawSelectedFrameOutline(x, y, width, height, pulse, colour);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawStartMenuItemHook(short y, byte index, bool greyed) {
		var x = (320 - 254) / 2;
		var whiteness = Game.Gui.StartMenuItemWhiteness[index];

		if (greyed) {
			var c = (byte)(whiteness * 2);

			RendererHelpers.DrawRectWHTexture(
				x, y, 254, 32, 0, index * 32,
				Game.Render.GetTPage(0, 2, 896, 0), Game.Render.CreatePaletteIndex(0, 491),
				new(c, c, c), true
			);
		}

		RendererHelpers.DrawRectWHTexture(
			x, y, 254, 32, 0, index * 32,
			Game.Render.GetTPage(0, 1, 896, 0), Game.Render.CreatePaletteIndex(32, 483),
			new(whiteness, whiteness, whiteness), greyed
		);
	}


	static unsafe void DrawTapestrySection(int section, short scroll, byte fade) {
		var texturePageX = 320 + section * 128;
		var tpage = Game.Render.GetTPage(1, 0, texturePageX, 0);

		ref var drawPrim = ref RendererHelpers.GetGpuPrim<Game.Render.GpuPrim_SetDrawEnv>();

		Game.Render.InitGpuPrim_SetDrawEnv(ref drawPrim, 0, 0, tpage, ref Unsafe.NullRef<Game.Render.PSX_RECT>());
		Game.Render.PushGpuPrim(2, (byte)Game.Render.GpuPrim_SetDrawEnv.UnmanagedSize);

		var x = (short)(320 - scroll + 255 * section);
		var textureIndex = (byte)(11 + section);
		var pPrim = Game.Render.DrawTitleDemoTextureFrame(x, 24, textureIndex, 2, 0);

		ref var prim = ref Unsafe.AsRef<Game.Render.GpuPrim_RectWHTexture>((void*)pPrim);
		prim.Colour1 = new(fade, fade, fade);
	}


	public static unsafe void Enable() {
		Game.Gui.HookDrawOverworldRegionPanel(&DrawOverworldRegionPanel_Area016Hook);
		Game.Gui.HookDrawOverworldCompassNeedle(&DrawOverworldCompassNeedleHook);
		Game.Render.HookDrawTitleDemoTextureFrame(&DrawTitleDemoTextureFrameHook);
		Game.Intro.HookDrawTapestry(&DrawTapestryHook);
		Game.Gui.HookDrawDialoguePageIndicator(&DrawDialoguePageIndicatorHook);
		Game.Render.HookDrawSky(&DrawSkyHook);
		Game.Gui.HookDrawSelectedFrameOutline(&DrawSelectedFrameOutlineHook);
		Game.Gui.HookDrawStartMenuItem(&DrawStartMenuItemHook);
	}
}