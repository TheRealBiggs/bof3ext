using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Hooks;


static partial class Gui {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawBorderedTextPanelWithModeHook(short x, short y, ushort width, ushort height, byte mode) {
		var c = mode != 0 ? (byte)255 : (byte)172;

		Vec3b colour = new(c, c, c);

		RendererHelpers.DrawBorderedPanel(x, y, width, height, colour);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawBorderedTextPanelWithPaletteHook(short x, short y, ushort width, ushort height, byte paletteIndex) {
		var colourVal = Game.Dat.Palettes.Rows[1].Colours[paletteIndex * 32 + 20];

		Vec3b colour = new(
			(byte)(8 * (colourVal & 0x1F)),
			(byte)(8 * ((colourVal >> 5) & 0x1F)),
			(byte)(8 * ((colourVal >> 10) & 0x1F))
		);

		RendererHelpers.DrawBorderedPanel(x, y, width, height, colour);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawDialogueWindowHook(short x, short y, byte width, byte height) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		RendererHelpers.DrawDialogueBackground(x, y, width, height);
		RendererHelpers.DrawDialogueBorder(x - 2, y - 2, width + 5, height + 5);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawDialogueWindowBorderHook(short x, short y, byte width, byte height) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		RendererHelpers.DrawDialogueBorder(x - 2, y - 2, width + 5, height + 5);
	}


	public static unsafe void Enable() {
		Game.Gui.HookDrawBorderedTextPanel_0(&DrawBorderedTextPanelWithModeHook);
		Game.Gui.HookDrawBorderedTextPanel_1(&DrawBorderedTextPanelWithPaletteHook);
		Game.Gui.HookDrawBorderedTextPanel_2(&DrawBorderedTextPanelWithPaletteHook);
		Game.Gui.HookDrawDialogueWindow(&DrawDialogueWindowHook);
		Game.Gui.HookDrawDialogueWindowBorder(&DrawDialogueWindowBorderHook);

		EnableBattleGuiHooks();
		EnableMenuGuiHooks();
	}
}