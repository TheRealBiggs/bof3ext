using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using unsafe DrawCenteredTextSig = delegate* unmanaged[Cdecl]<short, short, uint, byte, BoF3Ext.GameString, nint>;
using unsafe DrawConfigBackgroundWindowSig = delegate* unmanaged[Cdecl]<short, short, byte, byte, void>;

namespace BoF3Ext.Patches;


static partial class Gui {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	public static void DrawConfigBackgroundWindow(short x, short y, byte columns, byte rows)
		=> RendererHelpers.DrawConfigBackgroundWindow(x, y, columns, rows);

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static nint DrawCenteredText(short x, short y, uint paletteIndex, byte len, GameString text)
		=> DrawCenteredTextFix(x, y, paletteIndex, len, text, 0);

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static nint DrawCenteredText_Sub2(short x, short y, uint paletteIndex, byte len, GameString text)
		=> DrawCenteredTextFix(x, y, paletteIndex, len, text, -2);


	static nint DrawCenteredTextFix(short x, short y, uint paletteIndex, byte len, GameString text, short offset) {
		var textLen = Game.Text.GetStringLength(text);
		var advance = (int)Math.Round(GlyphManager.Instance.ScaledGlyphAdvance);

		x += (short)(textLen * 6);
		x -= (short)Math.Round(textLen * advance * 0.5f);
		x += offset;

		return Game.Render.DrawStringNormal(x, y, paletteIndex, len, text);
	}


	static unsafe void ApplyMenuGuiPatches() {
		// Increase width of status window selection outline
		Game.Gui.MenuOutlineSizes[0].X = 288;

		// Fix text centering for category in inventory window
		ProtectedMemory.WriteCall(0x575C8B, (DrawCenteredTextSig)(&DrawCenteredText_Sub2));

		// Fix text centering for category in skill window
		ProtectedMemory.WriteCall(0x576112, (DrawCenteredTextSig)(&DrawCenteredText_Sub2));

		// Fix text centering for category in equip window
		ProtectedMemory.WriteCall(0x5766FB, (DrawCenteredTextSig)(&DrawCenteredText_Sub2));

		// Fix text centering for submenu text in pause menu
		ProtectedMemory.WriteCall(0x59A025, (DrawCenteredTextSig)(&DrawCenteredText));

		// Fix text centering for Master title text
		ProtectedMemory.WriteCall(0x59C5E0, (DrawCenteredTextSig)(&DrawCenteredText));

		// Fix missing config background
		ProtectedMemory.WriteCall(0x461778, (DrawConfigBackgroundWindowSig)(&DrawConfigBackgroundWindow));

		// Fix text centering for unique item count in inventory window
		var advance = GlyphManager.Instance.ScaledGlyphAdvance;
		var offset = advance * (10.0f / 12.0f) / 2 * 7; // 7 = strlen("xxx/yyy")
		offset = (float)Math.Floor(113 - offset);

		ProtectedMemory.Write(0x575CE4, (byte)offset);

		// TODO: Fix text alignment for item count in item info

		// Fix text centering for submenu text in pause menu
		ProtectedMemory.Write(0x59A023, (byte)36);

		// Align the controller cursor with the expanded window
		ProtectedMemory.Write(0x461450, 176 - Hooks.Gui.CONFIG_CONTROLLER_TEXT_WIDTH_ADJ);

		// Remove draw of 2nd ':' in time text for save entries
		ProtectedMemory.Nop(0x576ACD, 5);
	}
}