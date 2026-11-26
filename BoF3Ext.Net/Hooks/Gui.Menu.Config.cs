using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Hooks;


static partial class Gui {
	public const int CONFIG_CONTROLLER_TEXT_WIDTH_ADJ = 48;


	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawConfigCategoryHook(short x, short y, byte index, byte rowExpand) {
		Game.Gui.DrawWindowBackground(
			x, (short)(y - rowExpand), 249, (ushort)(2 * rowExpand + 11), 0, Game.World.PersistentStateData.WindowColour
		);

		var _x = x + 56;

		TextManager.Instance.TryGetConfigText(index, out var text);

		var advance = GlyphManager.Instance.ScaledGlyphAdvance;
		advance /= 2;

		if (rowExpand == 3) {
			_x -= (short)(advance * text.Length);
			Game.Render.DrawStringNormal((short)_x, (short)(y - 1), 0, (byte)text.Length, text.GameString);

			if (Game.Gui.ConfigMenuState != 1)
				Game.Gui.DrawCursor((short)(x + 4), y);
		} else {
			_x -= (short)(advance * (10f / 12f) * text.Length);
			Game.Render.DrawStringSmall((short)_x, (short)(y + 3), 0, (byte)text.Length, text.GameString);
		}

		RendererHelpers.DrawRectWH(x + 116, y - rowExpand + 2, 1, 2 * rowExpand + 7, new(128, 128, 128), false);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawConfigValuesHook(short x, short y, byte index, byte selectedIndex, int rowExpand) {
		ReadOnlySpan<int> xOffsets = [
			13, 65, 117,
			15, 50, 81, 116,
			15, 50, 81, 116,
			13, 85,
			13, 85
		];

		var startIndex = Game.Gui.ConfigValueStartIndices[index];
		var count = Game.Gui.ConfigValueCounts[index];

		for (var i = 0; i < count; ++i) {
			var _x = x + 116 + xOffsets[startIndex + i];

			var colour = (byte)(i == selectedIndex ? 2 : 0);

			TextManager.Instance.TryGetConfigText((uint)(6 + startIndex + i), out var text);
			var advance = GlyphManager.Instance.ScaledGlyphAdvance;

			if (i == selectedIndex && rowExpand == 3) {
				if (index < 3) {
					if (i > 0 && i < count - 1)
						_x -= (short)(advance / 2 * text.Length);
					else if (i == count - 1)
						_x -= (short)(advance * text.Length);
				}

				Game.Render.DrawStringNormal((short)_x, (short)(y - 1), colour, (byte)text.Length, text.GameString);
			} else {
				if (index < 3) {
					if (i > 0 && i < count - 1)
						_x -= (short)(advance / 2 * (10f / 12f) * text.Length);
					else if (i == count - 1)
						_x -= (short)(advance * (10f / 12f) * text.Length);
				}

				Game.Render.DrawStringSmall((short)_x, (short)(y + 3), colour, (byte)text.Length, text.GameString);
			}
		}
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawConfigControllerWindowHook(short x, short y) {
		ReadOnlySpan<byte> ACTIONS = [
			0,	// Speak
			1,	// Move
			6,	// Action
			2,	// Menu
			3,	// View
			4	// Change
		];

		ReadOnlySpan<Game.Input.Buttons> BUTTONS = [
			Game.Input.Buttons.Circle,
			Game.Input.Buttons.Cross,
			Game.Input.Buttons.Triangle,
			Game.Input.Buttons.Square,
			Game.Input.Buttons.L1,
			Game.Input.Buttons.R1
		];

		ReadOnlySpan<byte> COLOURS = [
			2,	// Red
			1,	// Blue
			6,	// Green
			5,	// Pink
			0,	// White
			0
		];

		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		RendererHelpers.DrawConfigBackgroundWindow(
			(short)(x - CONFIG_CONTROLLER_TEXT_WIDTH_ADJ),
			y,
			15 + CONFIG_CONTROLLER_TEXT_WIDTH_ADJ / 8,
			15
		);

		var paletteIdx = Game.World.PersistentStateData.WindowColour;
		var advance = (int)Math.Round(GlyphManager.Instance.ScaledGlyphAdvance);

		var rowX = x + 5;
		var bindingX = x + 88;

		var textMgr = TextManager.Instance;

		for (var i = 0; i < 6; i++) {
			var rowY = (short)(y + 6 + i * 18);

			Game.Gui.DrawWindowBackground(
				(short)(rowX - CONFIG_CONTROLLER_TEXT_WIDTH_ADJ),
				rowY,
				104 + CONFIG_CONTROLLER_TEXT_WIDTH_ADJ,
				16,
				0,
				paletteIdx
			);

			textMgr.TryGetConfigText((uint)(21 + i), out var text);

			var textX = (short)(rowX + 32 - CONFIG_CONTROLLER_TEXT_WIDTH_ADJ / 2 - 1);
			textX -= (short)Math.Round(text.Length * advance * 0.5f);

			var textY = (short)(rowY + 1);

			Game.Render.DrawStringNormal(textX, textY, 0, 0xFF, text.GameString);

			Vec3b colour = new(128, 128, 128);

			RendererHelpers.DrawRectWH(rowX + 63, rowY + 2, 1, 12, colour, false);
			RendererHelpers.DrawRectWH(rowX + 83, rowY + 2, 1, 12, colour, false);

			var button = Game.Input.CurrentButtonMappings[ACTIONS[i]];

			for (int j = 0; j < 6; j++) {
				if ((int)(button & BUTTONS[j]) == 0)
					continue;

				ref var keyTextArray = ref Game.Text.ConfigControllerKeyTexts[5 - j];
				GameString keyText = new((byte*)Unsafe.AsPointer(ref keyTextArray[0]));

				var btnText = Game.Text.ConfigControllerButtonTexts[j];

				Game.Render.DrawStringNormal((short)(bindingX - 14), textY, 0, 3, keyText);
				Game.Render.DrawStringNormal((short)(bindingX + 3), textY, COLOURS[j], 3, btnText);

				break;
			}
		}
	}


	static unsafe void EnableConfigMenuGuiHooks() {
		Game.Gui.HookDrawConfigCategory(&DrawConfigCategoryHook);
		Game.Gui.HookDrawConfigValues(&DrawConfigValuesHook);
		Game.Gui.HookDrawConfigControllerWindow(&DrawConfigControllerWindowHook);
	}
}
