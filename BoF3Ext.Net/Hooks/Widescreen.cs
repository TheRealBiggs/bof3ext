using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace BoF3Ext.Hooks;


static class Widescreen {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawOverworldRegionPanel_Area016Hook(short x, short y) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

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
		// Negate geometry offset
		x -= (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.OriginalDrawOverworldCompassNeedle(x, y);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawLargeMenuIconHook(byte id, short x, short y, byte width, byte height, byte whiteness) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.OriginalDrawLargeMenuIcon(id, x, y, width, height, whiteness);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawConfigMenuTabsHook(byte index, short x, short y) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.OriginalDrawConfigMenuTabs(index, x, y);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawConfigMenuHook(short x, short y) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.OriginalDrawConfigMenu(x, y);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe nint DrawTitleDemoTextureFrameHook(short x, short y, byte frameIndex, byte drawIndex, byte translucent) {
		x += (frameIndex == 10)		// 10 = Demo logo
			? (short)ConfigManager.Instance.RenderWidthDiff
			: (short)ConfigManager.Instance.HalfRenderWidthDiff;

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
	static unsafe void DrawDialogueTextHook() {
		var origX = Game.Text.DialogueTextX;

		Game.Text.DialogueTextX += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.OriginalDrawDialogueText();

		Game.Text.DialogueTextX = origX;
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawDialoguePageIndicatorHook(short x, short y) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.OriginalDrawDialoguePageIndicator(x, (short)(y + 2));
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawSelectedFrameOutlineHook(short x, short y, ushort width, ushort height, int pulse, int colour) {
		// TODO: Replace the hooked function entirely to use quads instead of lines

		var cfgMgr = ConfigManager.Instance;
		var scale = cfgMgr.RenderScale;

		x -= (short)Math.Round(scale / 4.0f);
		y -= (short)Math.Round(scale / 4.0f);
		width += (ushort)Math.Round(scale / 2.0f);
		height += (ushort)Math.Round(scale);

		x += (short)cfgMgr.HalfRenderWidthDiff;

		Game.Gui.OriginalDrawSelectedFrameOutline(x, y, width, height, pulse, colour);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawSaveEntryHook(byte index, short x, short y, nint saveFile) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.OriginalDrawSaveEntry(index, x, y, saveFile);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawStartMenuLogoHook(short x, short y, byte spriteIndex, byte drawIndex) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Render.OriginalDrawStartMenuLogo(x, y, spriteIndex, drawIndex);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawMenuIconHook(byte id, short x, short y, byte width, byte height, byte whiteness) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.OriginalDrawMenuIcon(id, x, y, width, height, whiteness);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe GameString DrawDialogueChoiceHook(short x, short y, GameString text) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		return Game.Gui.OriginalDrawDialogueChoice(x, y, text);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void SetGeomOffsetHook(int x, int y) {
		x += ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Render.OriginalSetGeomOffset(x, y);
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
		Game.Gui.HookDrawLargeMenuIcon(&DrawLargeMenuIconHook);
		Game.Gui.HookDrawConfigMenuTabs(&DrawConfigMenuTabsHook);
		Game.Gui.HookDrawConfigMenu(&DrawConfigMenuHook);
		Game.Render.HookDrawTitleDemoTextureFrame(&DrawTitleDemoTextureFrameHook);
		Game.Intro.HookDrawTapestry(&DrawTapestryHook);
		Game.Gui.HookDrawDialogueText(&DrawDialogueTextHook);
		Game.Gui.HookDrawDialoguePageIndicator(&DrawDialoguePageIndicatorHook);
		Game.Gui.HookDrawSelectedFrameOutline(&DrawSelectedFrameOutlineHook);
		Game.Gui.HookDrawSaveEntry(&DrawSaveEntryHook);
		Game.Render.HookDrawStartMenuLogo(&DrawStartMenuLogoHook);
		Game.Gui.HookDrawMenuIcon(&DrawMenuIconHook);
		Game.Gui.HookDrawDialogueChoice(&DrawDialogueChoiceHook);
		Game.Render.HookSetGeomOffset(&SetGeomOffsetHook);
	}
}